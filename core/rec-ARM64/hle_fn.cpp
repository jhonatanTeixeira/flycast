// Funcoes do jogo reescritas em nativo, instaladas no endereco (docs/tech_debits.md 4.99+).
//
// O JIT, ao compilar o bloco de entrada de uma funcao conhecida (e so se os
// bytes SH4 na RAM baterem com a assinatura), emite uma chamada para cá logo
// depois da checagem de ciclos. A funcao nativa executa o mesmo programa SH4
// com o MESMO resultado bit a bit (mesmas operacoes de float, na ordem e com a
// fusao que o JIT usa) e com a MESMA contabilidade de ciclos: ela desconta os
// ciclos de cada bloco do JIT na fronteira em que o JIT descontaria e, quando a
// fatia acaba, chama o UpdateSystem ali mesmo -- interrupcoes caem no mesmo
// ponto. Qualquer coisa fora do caminho coberto sai na entrada do bloco do JIT
// correspondente (next_pc = bloco, ciclos ainda nao descontados), o que e
// exatamente equivalente a ter rodado o JIT ate ali.
//
// FC_HLE=0 desliga (A/B). FC_HLE_LOG=1 loga instalacao e saidas antecipadas.

#pragma GCC optimize("fp-contract=off")	// nada de fmadd que o JIT nao faz

#include "types.h"
#include "hw/sh4/sh4_if.h"
#include "hw/sh4/sh4_core.h"
#include "hw/sh4/sh4_mem.h"
#include "hw/sh4/sh4_interpreter.h"
#include "hw/sh4/dyna/ngen.h"
#include "hw/pvr/pvr_mem.h"
#include <arm_neon.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>

extern float g_lutSh4Clock;
extern void *ta_sq_stub;
bool hle_gpu_enabled();
bool hle_gpu_lightxf(const float *src, u32 n, const float *m16, const float *scr4,
		int nl, const float *ldir4, const float *lcol4, float *out);
#include <chrono>
#include <vector>

namespace {

int hleEnabled()
{
	static int e = -1;
	if (e < 0)
	{
		const char *v = getenv("FC_HLE");
		e = v == nullptr || atoi(v) != 0;
	}
	return e;
}
int hleLog()
{
	static int e = -1;
	if (e < 0)
		e = getenv("FC_HLE_LOG") != nullptr;
	return e;
}

// ops: bytes na base canonica. mask (opcional, nullptr = exato): por halfword,
// (op & mask) == (mem & mask) -- para os mov.l @(disp,PC) cujo deslocamento muda
// com a posicao do literal pool (layout).
struct Span { u32 addr; u32 count; const u16 *ops; const u16 *mask; };

// Funcao nativa de biblioteca, casavel em QUALQUER endereco (4.125): os Span
// guardam os bytes na base canonica (canonBase, do jogo de referencia) e o
// match e feito em base + (span.addr - canonBase). entryOff[e] e o
// deslocamento da entrada e relativo a canonBase; a base do jogo sai de
// vaddr - entryOff[entrada]. Bytes iguais em outro jogo => mesma funcao.
struct HleFunc {
	const char *name;
	int funcId;
	u32 canonBase;
	const Span *spans;
	size_t nSpans;
	const u32 *entryOff;
	size_t nEntries;
};

static bool func_matches(const HleFunc &f, u32 base)
{
	for (size_t i = 0; i < f.nSpans; i++)
	{
		const Span &s = f.spans[i];
		const u8 *mem = GetMemPtr(base + (s.addr - f.canonBase), s.count * 2);
		if (mem == nullptr)
			return false;
		if (s.mask == nullptr)
		{
			if (memcmp(mem, s.ops, s.count * 2) != 0)
				return false;
		}
		else
		{
			for (u32 j = 0; j < s.count; j++)
			{
				const u16 v = mem[j * 2] | (mem[j * 2 + 1] << 8);
				if ((v & s.mask[j]) != (s.ops[j] & s.mask[j]))
					return false;
			}
		}
	}
	return true;
}

// ---------------------------------------------------------------------------
// lightxf: transformacao + iluminacao direcional de vertices (Napple Tale,
// 8C14DDC0; estilo biblioteca SEGA). Entrada r4 = {n, src, ocb, dst(SQ)} +
// 4 floats de tela. Por vertice: ftrv da posicao e da normal, varre a mascara
// de luzes (3 bits por luz: ativa / outro tipo / fim), N.L de cada luz ativa,
// 1/z, tela, 32 bytes na Store Queue.
const u16 lightxf_s0[] = {
	0xFFFB, 0xFFEB, 0xFFDB, 0xFFCB, 0x2F86, 0x6346, 0x6546, 0x6646, 0x6746, 0xE800, 0xE000, 0xFC59,
	0xFD59, 0xFE59, 0xFF9D, 0xFDFD, 0xF859, 0xF959, 0xFA59, 0xFB8D, 0xF9FD, 0xD107, 0xF18D, 0x6212,
	0xF28D, 0x7110, 0xF38D, 0x4205, 0x0183, 0x8D07, 0x4205, 0x4205, 0x7120, 0x8945, 0xAFF8, 0x4205 };
const u16 lightxf_s1[] = {
	0xF419, 0x8D17, 0xF519, 0xF619, 0xFB8D, 0xF9ED, 0xF78D, 0xF7B5, 0x7114, 0x8D03, 0x4205, 0x8935,
	0xAFE8, 0x4205, 0x71F4, 0xF0BC, 0xF419, 0xF04D, 0xF519, 0xF14E, 0xF619, 0xF25E, 0x8D2A, 0xF36E,
	0xAFDC, 0x4205 };
const u16 lightxf_s2[] = {
	0xF09D, 0x7720, 0xF0E5, 0xF0E3, 0xF849, 0xE000, 0xF949, 0x7518, 0xFA49, 0x380E, 0xFB49, 0x0583,
	0xFF9D, 0xF73B, 0x74F0, 0xF72B, 0xF8C2, 0xF71B, 0xF9D2, 0xF7FB, 0xFB9E, 0xF70B, 0xFA8E, 0xF7BB,
	0x75E8, 0xF7AB, 0x4310, 0xF7EB, 0x0693, 0x7620, 0x0783, 0x8F82, 0x7720, 0x6083, 0x68F6, 0xFCF9,
	0xFDF9, 0xFEF9, 0x000B, 0xFFF9 };
const Span lightxf_sig[] = {
	{ 0x8C14DDC0, sizeof(lightxf_s0) / 2, lightxf_s0 },
	{ 0x8C14DE0C, sizeof(lightxf_s1) / 2, lightxf_s1 },
	{ 0x8C14DE90, sizeof(lightxf_s2) / 2, lightxf_s2 },
};
// entradas (offset relativo a 8C14DDC0): 0 = funcao, 1 = cabeca do laco
static const u32 lightxf_eo[] = { 0x00, 0x16 };

inline bool is_ram(u32 a) { return ((a >> 29) & 7) != 7 && ((a >> 26) & 7) == 3; }
inline u32 rd32(u32 a) { u32 v; memcpy(&v, &mem_b.data[a & RAM_MASK], 4); return v; }
inline u32 f2u(f32 f) { u32 v; memcpy(&v, &f, 4); return v; }
inline f32 u2f(u32 u) { f32 v; memcpy(&v, &u, 4); return v; }

struct Stats { u64 calls, bails; } lightxf_stats;
// FC_HLE_GPU (experimento 4.102): tempo da ida e volta na GPU x tempo da versao
// nativa, por chamada, e quanto o resultado da GPU difere do da CPU
struct GpuStats { u64 calls, gpuNs, cpuNs, verts, vertsDiff; u32 maxUlp; } gpuStats;
std::vector<float> gpuRec, cpuRec, gpuIn;

u64 lightxf_decline(u32 entry, const char *why)
{
	static u32 n;
	if (hleLog() && n++ < 20)
		fprintf(stderr, "hle: lightxf recusou (entrada %u): %s r3=%08X r4=%08X r5=%08X r7=%08X r15=%08X fpscr=%08X\n",
				entry, why, r[3], r[4], r[5], r[7], r[15], fpscr.full);
	return 0;
}

// entry 0: +0x00 (entrada da funcao); entry 1: +0x16 (cabeca do laco)
u64 lightxf_run(s32 c, u32 entry, u32 vaddr)
{
	// base do jogo = endereco da entrada - offset canonico; tudo abaixo usa A()
	const u32 CANON = 0x8C14DDC0;
	const u32 base = vaddr - lightxf_eo[entry];
#define A(x) (base + ((u32)(x) - CANON))
	const u32 lit = A(0x8C14DE08);		// mov.l @(disp,PC),r1 (DDEA)

	// so o modo simples de FPU (fmov.s de 32 bits, precisao simples)
	if (fpscr.PR || fpscr.SZ)
		return lightxf_decline(entry, "fpscr");
	if (entry == 0)
	{
		// pre-condicoes antes de qualquer efeito: pilha, argumentos, destino SQ
		const u32 a = r[4];
		if (!is_ram(r[15] - 20) || !is_ram(a) || !is_ram(a + 31))
			return lightxf_decline(entry, "pilha/argumentos fora da RAM");
		const u32 n = rd32(a), src = rd32(a + 4), dst = rd32(a + 12);
		if (n == 0 || n > 0x10000)
			return lightxf_decline(entry, "contagem");
		if (!is_ram(src) || !is_ram(src + n * 24 + 48))
			return lightxf_decline(entry, "origem fora da RAM");
		if ((dst >> 26) != 0x38 || ((dst + n * 32) >> 26) != 0x38)
			return lightxf_decline(entry, "destino fora da SQ");
		if (!is_ram(rd32(lit)))
			return lightxf_decline(entry, "tabela de luzes");
	}
	else
	{
		if (r[3] == 0 || r[3] > 0x10000)
			return lightxf_decline(entry, "contagem");
		if (!is_ram(r[5]) || !is_ram(r[5] + r[3] * 24 + 48) || !is_ram(r[4] + 15) || !is_ram(rd32(lit)))
			return lightxf_decline(entry, "ponteiros fora da RAM");
		if ((r[7] >> 26) != 0x38 || ((r[7] + r[3] * 32) >> 26) != 0x38)
			return lightxf_decline(entry, "destino fora da SQ");
	}
	lightxf_stats.calls++;
	if (hleLog() && (lightxf_stats.calls & 0x3FFF) == 1)
		fprintf(stderr, "hle: lightxf %llu chamadas, %llu saidas antecipadas\n",
				(unsigned long long)lightxf_stats.calls, (unsigned long long)lightxf_stats.bails);

	// Ciclos de cada bloco do JIT: mesma regra do decoder sem MMU (1 por
	// instrucao nao-FPU, vezes o clock do SH4, no minimo 1). Uma vez por chamada.
	const float clk = g_lutSh4Clock > 0.f ? g_lutSh4Clock : settings.dreamcast.sh4clock;
	auto cyc = [clk](u32 n) -> s32 { u32 v = n; v = v * clk; return (s32)std::max(1u, v); };
	const s32 C_DDD6 = cyc(7), C_DDF8 = cyc(3), C_DDFE = cyc(3), C_DE04 = cyc(2), C_DE0C = cyc(1),
			C_DE12 = cyc(3), C_DE22 = cyc(1), C_DE24 = cyc(2), C_DE28 = cyc(2), C_DE3C = cyc(2),
			C_DE90 = cyc(13), C_DED2 = cyc(3);

	const u8 *ram = mem_b.data;
	const u32 ramMask = RAM_MASK;
	u8 *sq = (u8 *)p_sh4rcb->sq_buffer;
#define RD32(a) ({ u32 v_; memcpy(&v_, ram + ((a) & ramMask), 4); v_; })
#define RDF(a) ({ f32 v_; memcpy(&v_, ram + ((a) & ramMask), 4); v_; })
#define SQW(a, f) do { u32 v_ = f2u(f); memcpy(sq + ((a) & 0x3C), &v_, 4); } while (0)

	// estado do SH4 em escalares (o JIT deixa tudo no contexto na fronteira de bloco)
	u32 r0, r1, r2, r3, r4, r5, r6, r7, r8, r15, T, jd;
	f32 f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15;
	float32x4_t m0, m1, m2, m3;
	u32 next = 0;
	bool cond;
	int slots = 0;
	// Plano da varredura de luzes para uma mascara: luzes ativas (deslocamento
	// em r1), r1/r2 no fim e o custo em ciclos de cada caminho. Com o plano e
	// ciclos de sobra para o pior caso do vertice, nenhuma fronteira de bloco
	// dispara o agendador no meio (o contador so desce): o vertice roda direto,
	// so com as luzes ativas, e desconta o custo exato no fim.
	u32 planMask = 0;
	bool planValid = false, planOk = false;
	int planAct = 0;
	u32 planActOff[16];
	s32 planNeg[16], planPos[16];
	s32 planCost = 0, planMax = 0;
	u32 planR1Delta = 0, planR2 = 0;
	auto build_plan = [&](u32 m) {
		planMask = m;
		planValid = true;
		planOk = false;
		planAct = 0;
		planCost = 0;
		planMax = 0;
		u32 x = m, t;
		t = x & 1; x = (x >> 1) | (t << 31);			// rotr em DDF6
		bool cnd = t;
		t = x & 1; x = (x >> 1) | (t << 31);			// rotr no slot do bt.s (DDFC)
		for (int slot = 0; slot < 16; slot++)
		{
			if (cnd)
			{
				if (t)
					return;								// luz do outro tipo (DE40): caminho lento
				planCost += C_DE0C + C_DE12;
				t = x & 1; x = (x >> 1) | (t << 31);	// rotr no slot do bt.s (DE20)
				planActOff[planAct] = (u32)slot * 32;
				if (t)
				{
					planNeg[planAct] = C_DE28;
					planPos[planAct] = C_DE22;
				}
				else
				{
					planNeg[planAct] = C_DE28 + C_DE3C + C_DDF8;
					planPos[planAct] = C_DE22 + C_DE24 + C_DDF8;
				}
				planMax += std::max(planNeg[planAct], planPos[planAct]);
				planAct++;
				if (t)
				{
					planR1Delta = (u32)(slot + 1) * 32;
					planR2 = x;
					planOk = true;
					return;
				}
				t = x & 1; x = (x >> 1) | (t << 31);	// rotr no slot do bra (DE24/DE3C)
			}
			else
			{
				planCost += C_DDFE;
				t = x & 1; x = (x >> 1) | (t << 31);	// rotr em DDFE
				if (t)
				{
					planR1Delta = (u32)(slot + 1) * 32;
					planR2 = x;
					planOk = true;
					return;
				}
				planCost += C_DE04 + C_DDF8;
				t = x & 1; x = (x >> 1) | (t << 31);	// rotr no slot do bra (DE04)
			}
			cnd = t;									// DDF8: bt.s
			t = x & 1; x = (x >> 1) | (t << 31);		// rotr no slot
		}
	};
#define RELOAD() do { \
		r0 = r[0]; r1 = r[1]; r2 = r[2]; r3 = r[3]; r4 = r[4]; r5 = r[5]; r6 = r[6]; r7 = r[7]; r8 = r[8]; r15 = r[15]; \
		f0 = fr[0]; f1 = fr[1]; f2 = fr[2]; f3 = fr[3]; f4 = fr[4]; f5 = fr[5]; f6 = fr[6]; f7 = fr[7]; \
		f8 = fr[8]; f9 = fr[9]; f10 = fr[10]; f11 = fr[11]; f12 = fr[12]; f13 = fr[13]; f14 = fr[14]; f15 = fr[15]; \
		T = sr.T; jd = Sh4cntx.jdyn; \
		m0 = vld1q_f32(&xf[0]); m1 = vld1q_f32(&xf[4]); m2 = vld1q_f32(&xf[8]); m3 = vld1q_f32(&xf[12]); \
	} while (0)
#define FLUSH() do { \
		r[0] = r0; r[1] = r1; r[2] = r2; r[3] = r3; r[4] = r4; r[5] = r5; r[6] = r6; r[7] = r7; r[8] = r8; r[15] = r15; \
		fr[0] = f0; fr[1] = f1; fr[2] = f2; fr[3] = f3; fr[4] = f4; fr[5] = f5; fr[6] = f6; fr[7] = f7; \
		fr[8] = f8; fr[9] = f9; fr[10] = f10; fr[11] = f11; fr[12] = f12; fr[13] = f13; fr[14] = f14; fr[15] = f15; \
		sr.T = T; Sh4cntx.jdyn = jd; \
	} while (0)
#define ROTR(x) do { T = (x) & 1; (x) = ((x) >> 1) | (T << 31); } while (0)
	// ftrv: fmul da coluna 0 e tres fmla fundidas, como o JIT
#define FTRV(a, b, cc, d) do { \
		float32x4_t acc_ = vmulq_n_f32(m0, a); \
		acc_ = vfmaq_n_f32(acc_, m1, b); acc_ = vfmaq_n_f32(acc_, m2, cc); acc_ = vfmaq_n_f32(acc_, m3, d); \
		a = vgetq_lane_f32(acc_, 0); b = vgetq_lane_f32(acc_, 1); cc = vgetq_lane_f32(acc_, 2); d = vgetq_lane_f32(acc_, 3); \
	} while (0)
	// Entrada do bloco do JIT `addr`: desconta; se a fatia acabou, faz o que
	// o intc_sched do JIT faz (fatia + UpdateSystem + interrupcao).
#define ENTER(addr, C) do { \
		c -= (C); \
		if (__builtin_expect(c < 0, 0)) \
		{ \
			c += sh4_sched_timeslice; \
			FLUSH(); \
			if (UpdateSystem() != 0) { next = rdv_DoInterrupts_pc(A(addr)); goto out; } \
			if (!Sh4cntx.CpuRunning) { next = A(addr); goto out; } \
			RELOAD(); \
		} \
	} while (0)
	// Sai na entrada do bloco `addr` sem descontar nada: o JIT continua dali.
#define BAIL(addr) do { lightxf_stats.bails++; next = A(addr); goto out_bail; } while (0)

	RELOAD();
	bool gpuCmp = false;
	u32 gpuN = 0, vi = 0;
	std::chrono::steady_clock::time_point tCpu0;
	if (entry == 0 && hle_gpu_enabled())
	{
		const auto t0 = std::chrono::steady_clock::now();
		const u32 a = r4, n = RD32(a), src = RD32(a + 4);
		const u32 lbase = RD32(lit);
		build_plan(RD32(lbase));
		if (planOk && planAct <= 16)
		{
			gpuIn.resize((size_t)n * 6);
			for (u32 k = 0; k < n * 6; k++)
				gpuIn[k] = RDF(src + k * 4);
			float m16[16], scr4[4], ld[16 * 4], lc[16 * 4];
			memcpy(m16, xf, sizeof(m16));
			for (int k = 0; k < 4; k++)
				scr4[k] = RDF(a + 16 + k * 4);
			for (int k = 0; k < planAct; k++)
			{
				const u32 b = lbase + 16 + planActOff[k];
				for (int j = 0; j < 3; j++)
				{
					ld[k * 4 + j] = RDF(b + j * 4);
					lc[k * 4 + j] = RDF(b + 20 + j * 4);
				}
				ld[k * 4 + 3] = lc[k * 4 + 3] = 0.f;
			}
			gpuRec.resize((size_t)n * 8);
			if (hle_gpu_lightxf(gpuIn.data(), n, m16, scr4, planAct, ld, lc, gpuRec.data()))
			{
				gpuCmp = true;
				gpuN = n;
				cpuRec.resize((size_t)n * 8);
				gpuStats.gpuNs += std::chrono::duration_cast<std::chrono::nanoseconds>(
						std::chrono::steady_clock::now() - t0).count();
			}
		}
		tCpu0 = std::chrono::steady_clock::now();
	}
	if (entry == 1)
		goto vertex;

	// ---- 8C14DDC0: prologo (os ciclos do bloco ja foram descontados pelo JIT)
	r15 -= 4; WriteMem32(r15, f2u(f15));
	r15 -= 4; WriteMem32(r15, f2u(f14));
	r15 -= 4; WriteMem32(r15, f2u(f13));
	r15 -= 4; WriteMem32(r15, f2u(f12));
	r15 -= 4; WriteMem32(r15, r8);
	r3 = RD32(r4); r4 += 4;
	r5 = RD32(r4); r4 += 4;
	r6 = RD32(r4); r4 += 4;
	r7 = RD32(r4); r4 += 4;
	r8 = 0;
	r0 = 0;
	goto vertex;

top:	// ---- 8C14DDD6 (laco de volta do DE90)
	ENTER(0x8C14DDD6, C_DDD6);
vertex:
	f12 = RDF(r5); r5 += 4;
	f13 = RDF(r5); r5 += 4;
	f14 = RDF(r5); r5 += 4;
	f15 = 1.f;
	FTRV(f12, f13, f14, f15);
	f8 = RDF(r5); r5 += 4;
	f9 = RDF(r5); r5 += 4;
	f10 = RDF(r5); r5 += 4;
	f11 = 0.f;
	FTRV(f8, f9, f10, f11);
	r1 = RD32(lit);
	f1 = 0.f;
	r2 = RD32(r1);
	f2 = 0.f;
	r1 += 16;
	f3 = 0.f;
	if (!planValid || r2 != planMask)
		build_plan(r2);
	if (planOk && c - planCost - planMax - C_DE90 >= 0)	// pior caso do vertice inteiro
	{
		// vertice direto: so as luzes ativas, na ordem, mesmas operacoes
		s32 spent = planCost + C_DE90;
		for (int k = 0; k < planAct; k++)
		{
			const u32 b = r1 + planActOff[k];
			f4 = RDF(b);
			f5 = RDF(b + 4);
			f6 = RDF(b + 8);
			f11 = 0.f;
			{
				const float32x4_t va = { f8, f9, f10, f11 }, vb = { f4, f5, f6, f7 };
				const float32x4_t pr_ = vmulq_f32(va, vb);
				const float32x4_t s_ = vpaddq_f32(pr_, pr_);
				f11 = vpadds_f32(vget_low_f32(s_));
			}
			f7 = 0.f;
			if (f7 > f11)
			{
				f0 = -f11;
				f4 = RDF(b + 20);
				f5 = RDF(b + 24);
				f6 = RDF(b + 28);
				f1 = __builtin_fmaf(f0, f4, f1);
				f2 = __builtin_fmaf(f0, f5, f2);
				f3 = __builtin_fmaf(f0, f6, f3);
				spent += planNeg[k];
			}
			else
				spent += planPos[k];
		}
		r1 += planR1Delta;
		r2 = planR2;
		c -= spent;
		goto finish_body;	// T/jd sao regravados no DE90 antes de qualquer uso
	}
	ROTR(r2);
	slots = 0;
	// pref @r1 (prefetch de RAM: nada); bt.s DE0C; rotr r2 (slot)
	cond = T; jd = T; ROTR(r2);
	if (cond) goto light;
	goto skip;

check:	// ---- 8C14DDF8: pref @r1; bt.s DE0C; rotr r2
	if (++slots > 16)
		BAIL(0x8C14DDF8);	// mascara estranha: deixa o JIT seguir
	ENTER(0x8C14DDF8, C_DDF8);
	cond = T; jd = T; ROTR(r2);
	if (cond) goto light;
skip:	// ---- 8C14DDFE: rotr r2; add #32,r1; bt DE90
	ENTER(0x8C14DDFE, C_DDFE);
	ROTR(r2);
	r1 += 32;
	jd = T;
	if (T) goto finish;
	// ---- 8C14DE04: bra DDF8; rotr r2
	ENTER(0x8C14DE04, C_DE04);
	ROTR(r2);
	goto check;

light:	// ---- 8C14DE0C: fmov.s @r1+,fr4; bt.s DE40; fmov.s @r1+,fr5
	if (T)
		BAIL(0x8C14DE0C);	// luz do outro tipo (DE40): fora do caminho coberto
	ENTER(0x8C14DE0C, C_DE0C);
	f4 = RDF(r1); r1 += 4;
	jd = T;
	f5 = RDF(r1); r1 += 4;
	// ---- 8C14DE12: fr6; fldi0 fr11; fipr fv4,fv8; fldi0 fr7; fcmp/gt fr11,fr7;
	//               add #20,r1; bt.s DE28; rotr r2
	ENTER(0x8C14DE12, C_DE12);
	f6 = RDF(r1); r1 += 4;
	f11 = 0.f;
	{	// fipr: produtos sem fusao, soma em pares (p0+p1)+(p2+p3), como o JIT
		const float32x4_t va = { f8, f9, f10, f11 }, vb = { f4, f5, f6, f7 };
		const float32x4_t pr_ = vmulq_f32(va, vb);
		const float32x4_t s_ = vpaddq_f32(pr_, pr_);
		f11 = vpadds_f32(vget_low_f32(s_));
	}
	f7 = 0.f;
	T = f7 > f11;
	r1 += 20;
	cond = T; jd = T; ROTR(r2);
	if (cond) goto accum;
	// ---- 8C14DE22: bt DE90
	ENTER(0x8C14DE22, C_DE22);
	jd = T;
	if (T) goto finish;
	// ---- 8C14DE24: bra DDF8; rotr r2
	ENTER(0x8C14DE24, C_DE24);
	ROTR(r2);
	goto check;

accum:	// ---- 8C14DE28: cor da luz * -N.L
	ENTER(0x8C14DE28, C_DE28);
	r1 -= 12;
	f0 = f11;
	f4 = RDF(r1); r1 += 4;
	f0 = -f0;
	f5 = RDF(r1); r1 += 4;
	f1 = __builtin_fmaf(f0, f4, f1);
	f6 = RDF(r1); r1 += 4;
	f2 = __builtin_fmaf(f0, f5, f2);
	cond = T; jd = T;
	f3 = __builtin_fmaf(f0, f6, f3);
	if (cond) goto finish;
	// ---- 8C14DE3C: bra DDF8; rotr r2
	ENTER(0x8C14DE3C, C_DE3C);
	ROTR(r2);
	goto check;

finish:	// ---- 8C14DE90: 1/z, tela, 32 bytes na SQ
	ENTER(0x8C14DE90, C_DE90);
finish_body:
	f0 = 1.f;
	r7 += 32;
	T = f0 > f14;
	f0 = f0 / f14;
	f8 = RDF(r4); r4 += 4;
	r0 = 0;
	f9 = RDF(r4); r4 += 4;
	r5 += 24;
	f10 = RDF(r4); r4 += 4;
	{	// addc r0,r8
		const u64 s_ = (u64)r8 + r0 + T;
		r8 = (u32)s_;
		T = (u32)(s_ >> 32);
	}
	f11 = RDF(r4); r4 += 4;
	f15 = 1.f;
	r7 -= 4; SQW(r7, f3);
	r4 -= 16;
	r7 -= 4; SQW(r7, f2);
	f8 = f8 * f12;
	r7 -= 4; SQW(r7, f1);
	f9 = f9 * f13;
	r7 -= 4; SQW(r7, f15);
	f11 = __builtin_fmaf(f0, f9, f11);
	r7 -= 4; SQW(r7, f0);
	f10 = __builtin_fmaf(f0, f8, f10);
	r7 -= 4; SQW(r7, f11);
	r5 -= 24;
	r7 -= 4; SQW(r7, f10);
	r3--;
	T = r3 == 0;
	r7 -= 4; SQW(r7, f14);
	r6 += 32;		// ocbi @r6: nada no JIT
	if (gpuCmp && vi < gpuN)
		memcpy(&cpuRec[(size_t)vi++ * 8], sq + (r7 & 0x20), 32);
	{	// pref @r7 (r7 na area da SQ, conferido na entrada)
		sqw_fp *fn = do_sqw_nommu;			// macro: sh4rcb.do_sqw_nommu
		if ((void *)fn == ta_sq_stub)		// stub ARM64 do JIT (convencao propria): caminho C equivalente
			fn = (sqw_fp *)&TAWriteSQ;
		fn(r7, sq);
	}
	cond = !T; jd = T;
	r7 += 32;
	if (cond) goto top;
	// ---- 8C14DED2: epilogo + rts
	ENTER(0x8C14DED2, C_DED2);
	r0 = r8;
	r8 = ReadMem32(r15); r15 += 4;
	f12 = u2f(ReadMem32(r15)); r15 += 4;
	f13 = u2f(ReadMem32(r15)); r15 += 4;
	f14 = u2f(ReadMem32(r15)); r15 += 4;
	jd = pr;
	next = pr;
	f15 = u2f(ReadMem32(r15)); r15 += 4;
	FLUSH();
	next_pc = next;
	if (gpuCmp && vi == gpuN)
	{
		gpuStats.cpuNs += std::chrono::duration_cast<std::chrono::nanoseconds>(
				std::chrono::steady_clock::now() - tCpu0).count();
		gpuStats.calls++;
		for (u32 v = 0; v < gpuN; v++)
		{
			bool diff = false;
			for (int w = 0; w < 8; w++)
			{
				s32 x = (s32)f2u(cpuRec[v * 8 + w]), y = (s32)f2u(gpuRec[v * 8 + w]);
				if (x != y)
				{
					diff = true;
					u32 d = (u32)std::abs(x - y);
					if (d > gpuStats.maxUlp && d < 0x01000000)
						gpuStats.maxUlp = d;
				}
			}
			gpuStats.verts++;
			gpuStats.vertsDiff += diff;
		}
		if ((gpuStats.calls & 1023) == 1)
			fprintf(stderr, "hle-gpu: %llu chamadas | GPU (envia+dispatch+espera+le) %.1f us/chamada | CPU nativa %.1f us/chamada | %.1f vertices/chamada | vertices com resultado diferente da CPU: %.1f%% (max %u ulp)\n",
					(unsigned long long)gpuStats.calls, gpuStats.gpuNs / 1e3 / gpuStats.calls,
					gpuStats.cpuNs / 1e3 / gpuStats.calls, (double)gpuStats.verts / gpuStats.calls,
					100.0 * gpuStats.vertsDiff / std::max<u64>(1, gpuStats.verts), gpuStats.maxUlp);
	}
	return (1ull << 32) | (u32)c;

out_bail:
	if (hleLog() && lightxf_stats.bails <= 20)
		fprintf(stderr, "hle: lightxf sai no bloco %08X (r3=%u)\n", next, r3);
	FLUSH();
	next_pc = next;
	return (1ull << 32) | (u32)c;
out:
	// estado ja gravado pelo ENTER (FLUSH antes do UpdateSystem)
	next_pc = next;
	return (1ull << 32) | (u32)c;
#undef RD32
#undef RDF
#undef SQW
#undef RELOAD
#undef FLUSH
#undef ROTR
#undef FTRV
#undef ENTER
#undef BAIL
#undef A
}

// ---------------------------------------------------------------------------
// stripemit: emissor de strips de triangulo para o TA (Napple Tale, 8C14D440;
// consome os registros de 32 bytes que a lightxf grava). Por strip: cabecalho
// (contagem; negativo = tipo invertido), indice do 1o vertice; por vertice: u,v
// (int16 -> float * escala), indice do proximo, 2 rajadas de 32 bytes na SQ
// (PCW+xyz+uv, cores). Roda com FPSCR.SZ=1 (fschg): os fmov movem PARES (8
// bytes). O laco interno (8C14D4F6) e um bloco que volta para si mesmo.
const u16 strip_s0[] = {
	0x2F86, 0x2F96, 0x2FA6, 0x2FB6, 0x2FC6, 0xD057, 0x6243, 0x7206, 0x0283, 0x6163, 0x2109, 0xE2F2,
	0x462D, 0xE303, 0xE200, 0x2639, 0xE300, 0xF99D, 0x4600, 0xF591, 0xF691, 0xF791, 0xF3FD, 0xDB4F,
	0xDC4F, 0x9087, 0x6845, 0x6945, 0x490D, 0x395C, 0x0983, 0x4811, 0x8900, 0x688B, 0x6045, 0x6A45,
	0x405A, 0x6043, 0xFA2D, 0x7006, 0x4A5A, 0x0083, 0xFB2D, 0x338C, 0xFA82, 0x328C, 0xFB82, 0xF099,
	0x78FE, 0xF299, 0x7718, 0xF7AB, 0xF72B, 0xF70B, 0x27B2, 0x0783, 0x7720, 0xF099, 0xE005, 0xF299,
	0x6945, 0xF150, 0x490D, 0xF260, 0x395C, 0xF370, 0x0983, 0x7720, 0xF042, 0xF72B, 0xF390, 0xF70B,
	0xF290, 0xF72B, 0xF190, 0xF70B, 0x0783, 0x7720, 0x6045, 0x6A45, 0x405A, 0xF099, 0xFA2D, 0xF299,
	0x4A5A, 0x6043, 0xFB2D, 0x7006, 0xFA82, 0x0083, 0xFB82, 0x7718, 0xF7AB, 0xF72B, 0xF70B, 0x27B2,
	0x0783, 0x7720, 0xF099, 0xE005, 0xF299, 0x6945, 0xF150, 0x490D, 0xF260, 0x395C, 0xF370, 0x0983,
	0xF042, 0x7720, 0xF72B, 0xF390, 0xF70B, 0xF290, 0xF72B, 0xF190, 0xF70B, 0x0783, 0x7720, 0x6045,
	0x4810, 0x6A45, 0x405A, 0x346C, 0xFA2D, 0x6043, 0x4A5A, 0xFB2D, 0x7006, 0xFA82, 0x0083, 0xFB82,
	0xF099, 0x8FD4, 0xF299, 0x7718, 0xF7AB, 0xF72B, 0xF70B, 0x27C2, 0x0783, 0x7720, 0xF099, 0xF299,
	0xF150, 0xF260, 0xF370, 0xF042, 0x7720, 0xF72B, 0xF390, 0xF70B, 0xF290, 0x4110, 0xF72B, 0xF190,
	0xF70B, 0x0783, 0x7720, 0x8902, 0xAF77, 0x0009 };
const u16 strip_s1[] = {	// 8C14D586 (depois do literal .word em 8C14D584)
	0xD40B, 0x6073, 0xD50B, 0x6642, 0x6752, 0x362C, 0x373C, 0x2462, 0x2572, 0xF3FD, 0x6CF6, 0x6BF6,
	0x6AF6, 0x69F6, 0x000B, 0x68F6 };
const Span strip_sig[] = {
	{ 0x8C14D440, sizeof(strip_s0) / 2, strip_s0 },
	{ 0x8C14D586, sizeof(strip_s1) / 2, strip_s1 },
};
// entradas (offset relativo a 8C14D440): 0 = funcao, 1 = laco do vertice, 2 = cabeca da strip
static const u32 strip_eo[] = { 0x00, 0xB6, 0x32 };

Stats strip_stats;

// shld Rm,Rn do SH4 (deslocamento logico com sinal no Rm)
inline u32 sh4_shld(u32 rn, u32 rm)
{
	const s32 sm = (s32)rm;
	if (sm >= 0)
		return rn << (rm & 31);
	if ((rm & 31) == 0)
		return 0;
	return rn >> (((~rm) & 31) + 1);
}

// entry 0: +0x00 (entrada); 1: +0xB6 (laco do vertice); 2: +0x32 (cabeca da strip)
u64 stripemit_run(s32 c, u32 entry, u32 vaddr)
{
	const u32 CANON = 0x8C14D440;
	const u32 base = vaddr - strip_eo[entry];
#define A(x) (base + ((u32)(x) - CANON))
	const u32 lit5A8 = A(0x8C14D5A8), lit5AC = A(0x8C14D5AC), lit5B0 = A(0x8C14D5B0);
	const u32 lit584 = A(0x8C14D584), lit5B4 = A(0x8C14D5B4), lit5B8 = A(0x8C14D5B8);

	if (fpscr.PR || fpscr.SZ != (entry == 0 ? 0u : 1u))
		return 0;
	// pre-condicoes antes de qualquer efeito
	if (!is_ram(r[15] - 20) || !is_ram(r[4]) || !is_ram(r[5]) || (r[7] >> 26) != 0x38)
		return 0;
	if (entry == 0 && (r[6] & rd32(lit5A8)) == 0)
		return 0;		// nenhuma strip: deixa o JIT (caso raro)
	strip_stats.calls++;

	const float clk = g_lutSh4Clock > 0.f ? g_lutSh4Clock : settings.dreamcast.sh4clock;
	auto cyc = [clk](u32 n) -> s32 { u32 v = n; v = v * clk; return (s32)std::max(1u, v); };
	const s32 C_472 = cyc(8), C_482 = cyc(52), C_484 = cyc(51), C_4F6 = cyc(22), C_54E = cyc(9),
			C_580 = cyc(2), C_586 = cyc(15);

	const u8 *ram = mem_b.data;
	const u32 ramMask = RAM_MASK;
	u8 *sq = (u8 *)p_sh4rcb->sq_buffer;
#define RD32(a) ({ u32 v_; memcpy(&v_, ram + ((a) & ramMask), 4); v_; })
#define RDF(a) ({ f32 v_; memcpy(&v_, ram + ((a) & ramMask), 4); v_; })
#define RD16S(a) ({ s16 v_; memcpy(&v_, ram + ((a) & ramMask), 2); (u32)(s32)v_; })
#define SQW32(a, v) do { u32 v_ = (v); memcpy(sq + ((a) & 0x3C), &v_, 4); } while (0)
#define SQWF(a, f) SQW32(a, f2u(f))
	// fmov DRm,@-Rn (SZ=1): Rn -= 8; [Rn] = FRm; [Rn+4] = FRm+1
#define SQPAIR(lo, hi) do { r7 -= 8; SQWF(r7, lo); SQWF(r7 + 4, hi); } while (0)
	// fmov @Rm+,DRn (SZ=1): FRn = [Rm]; FRn+1 = [Rm+4]; Rm += 8
#define LDPAIR(lo, hi) do { lo = RDF(r9); hi = RDF(r9 + 4); r9 += 8; } while (0)
#define FLUSHSQ(a) do { \
		sqw_fp *fn_ = do_sqw_nommu; \
		if ((void *)fn_ == ta_sq_stub) fn_ = (sqw_fp *)&TAWriteSQ; \
		fn_((a), sq); \
	} while (0)

	u32 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r15, fpulv, T, jd, szbit;
	f32 f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11;
	u32 next = 0;
	bool cond;
#define RELOAD() do { \
		r0 = r[0]; r1 = r[1]; r2 = r[2]; r3 = r[3]; r4 = r[4]; r5 = r[5]; r6 = r[6]; r7 = r[7]; \
		r8 = r[8]; r9 = r[9]; r10 = r[10]; r11 = r[11]; r12 = r[12]; r15 = r[15]; \
		f0 = fr[0]; f1 = fr[1]; f2 = fr[2]; f3 = fr[3]; f4 = fr[4]; f5 = fr[5]; f6 = fr[6]; f7 = fr[7]; \
		f8 = fr[8]; f9 = fr[9]; f10 = fr[10]; f11 = fr[11]; \
		fpulv = fpul; T = sr.T; jd = Sh4cntx.jdyn; szbit = fpscr.SZ; \
	} while (0)
#define FLUSH() do { \
		r[0] = r0; r[1] = r1; r[2] = r2; r[3] = r3; r[4] = r4; r[5] = r5; r[6] = r6; r[7] = r7; \
		r[8] = r8; r[9] = r9; r[10] = r10; r[11] = r11; r[12] = r12; r[15] = r15; \
		fr[0] = f0; fr[1] = f1; fr[2] = f2; fr[3] = f3; fr[4] = f4; fr[5] = f5; fr[6] = f6; fr[7] = f7; \
		fr[8] = f8; fr[9] = f9; fr[10] = f10; fr[11] = f11; \
		fpul = fpulv; sr.T = T; Sh4cntx.jdyn = jd; fpscr.SZ = szbit; \
	} while (0)
#define ENTER(addr, C) do { \
		c -= (C); \
		if (__builtin_expect(c < 0, 0)) \
		{ \
			c += sh4_sched_timeslice; \
			FLUSH(); \
			if (UpdateSystem() != 0) { next = rdv_DoInterrupts_pc(A(addr)); goto out; } \
			if (!Sh4cntx.CpuRunning) { next = A(addr); goto out; } \
			RELOAD(); \
		} \
	} while (0)

	RELOAD();
	if (entry == 1)
		goto loopbody;
	if (entry == 2)
		goto head;

	// ---- 8C14D440 (ciclos do bloco ja descontados pelo JIT): prologo + 1a cabeca
	r15 -= 4; WriteMem32(r15, r8);
	r15 -= 4; WriteMem32(r15, r9);
	r15 -= 4; WriteMem32(r15, r10);
	r15 -= 4; WriteMem32(r15, r11);
	r15 -= 4; WriteMem32(r15, r12);
	r0 = RD32(lit5A8);
	r2 = r4;
	r2 += 6;
	r1 = r6;
	r1 &= r0;
	r2 = 0xFFFFFFF2;
	r6 = sh4_shld(r6, r2);
	r3 = 3;
	r2 = 0;
	r6 &= r3;
	r3 = 0;
	f9 = 1.f;
	T = r6 >> 31; r6 <<= 1;		// shll
	f5 = f5 - f9;
	f6 = f6 - f9;
	f7 = f7 - f9;
	szbit ^= 1;					// fschg
	r11 = RD32(lit5AC);
	r12 = RD32(lit5B0);
	goto head_body;

head:	// ---- 8C14D472: cabeca da strip
	ENTER(0x8C14D472, C_472);
head_body:
	r0 = RD16S(lit584);
	r8 = RD16S(r4); r4 += 2;
	r9 = RD16S(r4); r4 += 2;
	r9 = sh4_shld(r9, r0);
	r9 += r5;
	T = (s32)r8 >= 0;			// cmp/pz
	cond = T; jd = T;
	if (cond)
		goto b484;
	// ---- 8C14D482: neg + corpo do 8C14D484 (mesmo bloco)
	ENTER(0x8C14D482, C_482);
	r8 = 0 - r8;
	goto body484;
b484:
	ENTER(0x8C14D484, C_484);
body484:
	r0 = RD16S(r4); r4 += 2;
	r10 = RD16S(r4); r4 += 2;
	fpulv = r0;
	r0 = r4;
	f10 = (f32)(s32)fpulv;
	r0 += 6;
	fpulv = r10;
	f11 = (f32)(s32)fpulv;
	r3 += r8;
	f10 = f10 * f8;
	r2 += r8;
	f11 = f11 * f8;
	LDPAIR(f0, f1);
	r8 += 0xFFFFFFFE;
	LDPAIR(f2, f3);
	r7 += 24;
	SQPAIR(f10, f11);
	SQPAIR(f2, f3);
	SQPAIR(f0, f1);
	SQW32(r7, r11);
	FLUSHSQ(r7);
	r7 += 32;
	LDPAIR(f0, f1);
	r0 = 5;
	LDPAIR(f2, f3);
	r9 = RD16S(r4); r4 += 2;
	f1 = f1 + f5;
	r9 = sh4_shld(r9, r0);
	f2 = f2 + f6;
	r9 += r5;
	f3 = f3 + f7;
	r7 += 32;
	f0 = f0 * f4;
	SQPAIR(f2, f3);
	f3 = f3 + f9;
	SQPAIR(f0, f1);
	f2 = f2 + f9;
	SQPAIR(f2, f3);
	f1 = f1 + f9;
	SQPAIR(f0, f1);
	FLUSHSQ(r7);
	r7 += 32;
	r0 = RD16S(r4); r4 += 2;
	r10 = RD16S(r4); r4 += 2;
	fpulv = r0;
	LDPAIR(f0, f1);
	f10 = (f32)(s32)fpulv;
	LDPAIR(f2, f3);
	fpulv = r10;
	r0 = r4;
	f11 = (f32)(s32)fpulv;
	r0 += 6;
	f10 = f10 * f8;
	f11 = f11 * f8;
	goto loopbody;

loop:	// ---- 8C14D4F6: um vertice por volta
	ENTER(0x8C14D4F6, C_4F6);
loopbody:
	r7 += 24;
	SQPAIR(f10, f11);
	SQPAIR(f2, f3);
	SQPAIR(f0, f1);
	SQW32(r7, r11);
	FLUSHSQ(r7);
	r7 += 32;
	LDPAIR(f0, f1);
	r0 = 5;
	LDPAIR(f2, f3);
	r9 = RD16S(r4); r4 += 2;
	f1 = f1 + f5;
	r9 = sh4_shld(r9, r0);
	f2 = f2 + f6;
	r9 += r5;
	f3 = f3 + f7;
	f0 = f0 * f4;
	r7 += 32;
	SQPAIR(f2, f3);
	f3 = f3 + f9;
	SQPAIR(f0, f1);
	f2 = f2 + f9;
	SQPAIR(f2, f3);
	f1 = f1 + f9;
	SQPAIR(f0, f1);
	FLUSHSQ(r7);
	r7 += 32;
	r0 = RD16S(r4); r4 += 2;
	r8--; T = r8 == 0;			// dt
	r10 = RD16S(r4); r4 += 2;
	fpulv = r0;
	r4 += r6;
	f10 = (f32)(s32)fpulv;
	r0 = r4;
	fpulv = r10;
	f11 = (f32)(s32)fpulv;
	r0 += 6;
	f10 = f10 * f8;
	f11 = f11 * f8;
	LDPAIR(f0, f1);
	cond = !T; jd = T;
	LDPAIR(f2, f3);				// slot do bf.s
	if (cond)
		goto loop;

	// ---- 8C14D54E: ultimo vertice da strip (PCW de fim, r12)
	ENTER(0x8C14D54E, C_54E);
	r7 += 24;
	SQPAIR(f10, f11);
	SQPAIR(f2, f3);
	SQPAIR(f0, f1);
	SQW32(r7, r12);
	FLUSHSQ(r7);
	r7 += 32;
	LDPAIR(f0, f1);
	LDPAIR(f2, f3);
	f1 = f1 + f5;
	f2 = f2 + f6;
	f3 = f3 + f7;
	f0 = f0 * f4;
	r7 += 32;
	SQPAIR(f2, f3);
	f3 = f3 + f9;
	SQPAIR(f0, f1);
	f2 = f2 + f9;
	r1--; T = r1 == 0;			// dt r1
	SQPAIR(f2, f3);
	f1 = f1 + f9;
	SQPAIR(f0, f1);
	FLUSHSQ(r7);
	r7 += 32;
	cond = T; jd = T;
	if (cond)
		goto exit_;
	// ---- 8C14D580: bra 8C14D472; nop
	ENTER(0x8C14D580, C_580);
	goto head;

exit_:	// ---- 8C14D586: contadores + epilogo + rts
	ENTER(0x8C14D586, C_586);
	r4 = RD32(lit5B4);
	r0 = r7;
	r5 = RD32(lit5B8);
	r6 = ReadMem32(r4);
	r7 = ReadMem32(r5);
	r6 += r2;
	r7 += r3;
	WriteMem32(r4, r6);
	WriteMem32(r5, r7);
	szbit ^= 1;					// fschg
	r12 = ReadMem32(r15); r15 += 4;
	r11 = ReadMem32(r15); r15 += 4;
	r10 = ReadMem32(r15); r15 += 4;
	r9 = ReadMem32(r15); r15 += 4;
	jd = pr;
	next = pr;
	r8 = ReadMem32(r15); r15 += 4;	// slot do rts
	FLUSH();
	next_pc = next;
	return (1ull << 32) | (u32)c;
out:
	next_pc = next;
	return (1ull << 32) | (u32)c;
#undef RD32
#undef RDF
#undef RD16S
#undef SQW32
#undef SQWF
#undef SQPAIR
#undef LDPAIR
#undef FLUSHSQ
#undef RELOAD
#undef FLUSH
#undef ENTER
#undef A
}


// ---------------------------------------------------------------------------
// doa2: transformacao + iluminacao + clamp de vertices (Dead or Alive 2,
// laco interno 8C101BC4, ~45% do custo do JIT do jogo). Base FPSCR.SZ=1: os
// fmov.s movem PARES de 8 bytes (dr); o caminho da tabela de cor (8C101C2E)
// troca SZ por UMA instrucao so para ler 4 bytes (net: SZ volta a 1). Por
// vertice: carrega 3 pares, ftrv xmtrx, fipr (luz), fdiv (1/w), fcmp/gt +
// ftrc (clamp por tabela), 4 pares + cabecalho de 32 bytes na SQ, pref
// (flush). O laco e fechado: as bordas do laco externo (8C101C5C) e o
// epilogo (8C101C52) ficam no JIT. Entradas: cabeca do laco e reentradas do
// laco externo (8C101C08/C0E/BF0); qualquer desvio cai no JIT.
const u16 doa2_s_BC2[] = {
	0xF4E9, 0xF6E9, 0x6763, 0xF28D, 0xF270, 0xF79D, 0x7640, 0xF38D, 0x7520, 0xF0E9, 0xF5FD, 0xFB8D,
	0x4310, 0x6046, 0x8D3D, 0x61E3,
};
const u16 doa2_s_BC4[] = {
	0xF6E9, 0x6763, 0xF28D, 0xF270, 0xF79D, 0x7640, 0xF38D, 0x7520, 0xF0E9, 0xF5FD, 0xFB8D, 0x4310,
	0x6046, 0x8D3D, 0x61E3,
};
const u16 doa2_s_BE2[] = { 0xF3ED, 0xC801, 0x6E46, 0x8F10, 0xF79D };
const u16 doa2_s_BEC[] = {
	0xF743, 0x0483, 0xF3B5, 0x3E4C, 0xFF1D, 0xF8ED, 0x0E83, 0x8F1D, 0xF20D,
};
const u16 doa2_s_BF0[] = { 0xF3B5, 0x3E4C, 0xFF1D, 0xF8ED, 0x0E83, 0x8F1D, 0xF20D };
const u16 doa2_s_BFE[] = { 0xF230, 0xF38D, 0xA010, 0xFB3D };
const u16 doa2_s_C08[] = {
	0x74E4, 0xF79D, 0xF743, 0x7418, 0xF3B5, 0xFF1D, 0xF8ED, 0x6E43, 0x7EE0, 0x0483, 0x8F0C, 0xF20D,
};
const u16 doa2_s_C0C[] = {
	0xF743, 0x7418, 0xF3B5, 0xFF1D, 0xF8ED, 0x6E43, 0x7EE0, 0x0483, 0x8F0C, 0xF20D,
};
const u16 doa2_s_C0E[] = {
	0x7418, 0xF3B5, 0xFF1D, 0xF8ED, 0x6E43, 0x7EE0, 0x0483, 0x8F0C, 0xF20D,
};
const u16 doa2_s_C20[] = { 0xF230, 0xFB3D, 0xF38D, 0x005A, 0x3027, 0x4008, 0x8B05 };
const u16 doa2_s_C26[] = { 0x005A, 0x3027, 0x4008, 0x8B05 };
const u16 doa2_s_C2E[] = { 0xF3FD, 0xF386, 0xA002, 0xF3FD };
const u16 doa2_s_C38[] = {
	0xF38D, 0xF018, 0xF672, 0xF62B, 0xF572, 0xF60B, 0x2338, 0xF66B, 0xF64B, 0x8D02, 0x2672,
};
const u16 doa2_s_C3A[] = {
	0xF018, 0xF672, 0xF62B, 0xF572, 0xF60B, 0x2338, 0xF66B, 0xF64B, 0x8D02, 0x2672,
};
const u16 doa2_s_C4E[] = { 0xF4E9, 0xAFB8, 0x0683 };
const Span doa2_sig[] = {
	{ 0x8C101BC2, sizeof(doa2_s_BC2) / 2, doa2_s_BC2 },
	{ 0x8C101BC4, sizeof(doa2_s_BC4) / 2, doa2_s_BC4 },
	{ 0x8C101BE2, sizeof(doa2_s_BE2) / 2, doa2_s_BE2 },
	{ 0x8C101BEC, sizeof(doa2_s_BEC) / 2, doa2_s_BEC },
	{ 0x8C101BF0, sizeof(doa2_s_BF0) / 2, doa2_s_BF0 },
	{ 0x8C101BFE, sizeof(doa2_s_BFE) / 2, doa2_s_BFE },
	{ 0x8C101C08, sizeof(doa2_s_C08) / 2, doa2_s_C08 },
	{ 0x8C101C0C, sizeof(doa2_s_C0C) / 2, doa2_s_C0C },
	{ 0x8C101C0E, sizeof(doa2_s_C0E) / 2, doa2_s_C0E },
	{ 0x8C101C20, sizeof(doa2_s_C20) / 2, doa2_s_C20 },
	{ 0x8C101C26, sizeof(doa2_s_C26) / 2, doa2_s_C26 },
	{ 0x8C101C2E, sizeof(doa2_s_C2E) / 2, doa2_s_C2E },
	{ 0x8C101C38, sizeof(doa2_s_C38) / 2, doa2_s_C38 },
	{ 0x8C101C3A, sizeof(doa2_s_C3A) / 2, doa2_s_C3A },
	{ 0x8C101C4E, sizeof(doa2_s_C4E) / 2, doa2_s_C4E },
};
// entradas (offset relativo a 8C101BC2), na ordem dos ids 0x400..0x40E
static const u32 doa2_eo[] = {
	0x00, 0x02, 0x20, 0x2A, 0x2E, 0x3C, 0x46, 0x4A, 0x4C, 0x5E, 0x64, 0x6C, 0x76, 0x78, 0x8C,
};

Stats doa2_stats;

u64 doa2_run(s32 c, u32 entry, u32 vaddr)
{
	const u32 CANON = 0x8C101BC2;
	const u32 base = vaddr - doa2_eo[entry];
#define A(x) (base + ((u32)(x) - CANON))
	// base do laco: FPSCR.SZ=1 (fmov.s move pares); precisao simples
	if (fpscr.PR || fpscr.SZ != 1)
		return 0;
	// r4 (dados), r8 (tabela de cor) e r6 (SQ) sao usados como ponteiros
	// absolutos. r14 NAO: nas reentradas do laco externo ele e um offset
	// relativo (o `add r4,r14` do 8C101BF0 o torna absoluto) e so e
	// desreferenciado no fim (8C101C4E). r1 idem (stale ate o 8C101BC4).
	if (!is_ram(r[4]) || !is_ram(r[8]) || (r[6] >> 26) != 0x38)
	{
		if (hleLog())
		{
			static u32 n;
			if (n++ < 20)
				fprintf(stderr, "hle: doa2 recusou (entrada %u): r4=%08X r6=%08X r8=%08X r14=%08X sz=%u\n",
						entry, r[4], r[6], r[8], r[14], (u32)fpscr.SZ);
		}
		return 0;
	}

	const float clk = g_lutSh4Clock > 0.f ? g_lutSh4Clock : settings.dreamcast.sh4clock;
	auto cyc = [clk](u32 n) -> s32 { u32 v = n; v = v * clk; return (s32)std::max(1u, v); };
	const s32 C_BC4 = cyc(7), C_BE2 = cyc(3), C_BEC = cyc(4), C_BF0 = cyc(3), C_BFE = cyc(1),
			C_C08 = cyc(6), C_C0C = cyc(5), C_C0E = cyc(5), C_C20 = cyc(4), C_C26 = cyc(4),
			C_C2E = cyc(1), C_C38 = cyc(3), C_C3A = cyc(3), C_C4E = cyc(2);
	doa2_stats.calls++;
	if (hleLog() && (doa2_stats.calls & 0x3FFF) == 1)
		fprintf(stderr, "hle: doa2 %llu chamadas (entrada %u)\n",
				(unsigned long long)doa2_stats.calls, entry);

	const u8 *ram = mem_b.data;
	const u32 ramMask = RAM_MASK;
	u8 *sq = (u8 *)p_sh4rcb->sq_buffer;
#define RD32(a) ({ u32 v_; memcpy(&v_, ram + ((a) & ramMask), 4); v_; })
#define RDF(a) ({ f32 v_; memcpy(&v_, ram + ((a) & ramMask), 4); v_; })
#define LDP(lo, hi, a) do { lo = RDF(a); hi = RDF(a + 4); } while (0)
#define SQI(v, a) do { u32 v_ = (v); memcpy(sq + ((a) & 0x3C), &v_, 4); } while (0)
#define SQP(lo, hi, a) do { SQI(f2u(lo), a); SQI(f2u(hi), (a) + 4); } while (0)
#define FLUSHSQ(a) do { \
		sqw_fp *fn_ = do_sqw_nommu; \
		if ((void *)fn_ == ta_sq_stub) fn_ = (sqw_fp *)&TAWriteSQ; \
		fn_((a), sq); \
	} while (0)
	// pref @a: so tem efeito na SQ (flush de 32 bytes); RAM e prfm
#define PREF(a) do { if (((a) >> 26) == 0x38) FLUSHSQ(a); } while (0)

	u32 r0, r1, r2, r3, r4, r5, r6, r7, r8, r14, fpulv, T, jd;
	f32 f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15;
	float32x4_t m0, m1, m2, m3;
	u32 next = 0;
#define RELOAD() do { \
		r0 = r[0]; r1 = r[1]; r2 = r[2]; r3 = r[3]; r4 = r[4]; r5 = r[5]; r6 = r[6]; r7 = r[7]; r8 = r[8]; r14 = r[14]; \
		f0 = fr[0]; f1 = fr[1]; f2 = fr[2]; f3 = fr[3]; f4 = fr[4]; f5 = fr[5]; f6 = fr[6]; f7 = fr[7]; \
		f8 = fr[8]; f9 = fr[9]; f10 = fr[10]; f11 = fr[11]; f12 = fr[12]; f13 = fr[13]; f14 = fr[14]; f15 = fr[15]; \
		fpulv = fpul; T = sr.T; jd = Sh4cntx.jdyn; \
		m0 = vld1q_f32(&xf[0]); m1 = vld1q_f32(&xf[4]); m2 = vld1q_f32(&xf[8]); m3 = vld1q_f32(&xf[12]); \
	} while (0)
#define FLUSH() do { \
		r[0] = r0; r[1] = r1; r[2] = r2; r[3] = r3; r[4] = r4; r[5] = r5; r[6] = r6; r[7] = r7; r[8] = r8; r[14] = r14; \
		fr[0] = f0; fr[1] = f1; fr[2] = f2; fr[3] = f3; fr[4] = f4; fr[5] = f5; fr[6] = f6; fr[7] = f7; \
		fr[8] = f8; fr[9] = f9; fr[10] = f10; fr[11] = f11; fr[12] = f12; fr[13] = f13; fr[14] = f14; fr[15] = f15; \
		fpul = fpulv; sr.T = T; Sh4cntx.jdyn = jd; \
	} while (0)
#define ENTER(addr, C) do { \
		c -= (C); \
		if (__builtin_expect(c < 0, 0)) \
		{ \
			c += sh4_sched_timeslice; \
			FLUSH(); \
			if (UpdateSystem() != 0) { next = rdv_DoInterrupts_pc(A(addr)); goto out; } \
			if (!Sh4cntx.CpuRunning) { next = A(addr); goto out; } \
			RELOAD(); \
		} \
	} while (0)
	// ftrv: fmul da coluna 0 e tres fmla fundidas, como o JIT
#define FTRV(a, b, cc, d) do { \
		float32x4_t acc_ = vmulq_n_f32(m0, a); \
		acc_ = vfmaq_n_f32(acc_, m1, b); acc_ = vfmaq_n_f32(acc_, m2, cc); acc_ = vfmaq_n_f32(acc_, m3, d); \
		a = vgetq_lane_f32(acc_, 0); b = vgetq_lane_f32(acc_, 1); cc = vgetq_lane_f32(acc_, 2); d = vgetq_lane_f32(acc_, 3); \
	} while (0)
	// fipr: produtos sem fusao, soma em pares (p0+p1)+(p2+p3), como o JIT
#define FIPR(dst, a0, a1, a2, a3, b0, b1, b2, b3) do { \
		const float32x4_t va_ = { a0, a1, a2, a3 }; \
		const float32x4_t vb_ = { b0, b1, b2, b3 }; \
		const float32x4_t pr_ = vmulq_f32(va_, vb_); \
		const float32x4_t s_ = vpaddq_f32(pr_, pr_); \
		dst = vpadds_f32(vget_low_f32(s_)); \
	} while (0)
#define FTRC(x) ((u32)(s32)vcvts_s32_f32(x))

	RELOAD();
	switch (entry)
	{
	case 0: goto bc2_body;
	case 1: goto bc4_body;
	case 2: goto be2_body;
	case 3: goto bec_body;
	case 4: goto bf0_body;
	case 5: goto bfe_body;
	case 6: goto c08_body;
	case 7: goto c0c_body;
	case 8: goto c0e_body;
	case 9: goto c20_body;
	case 10: goto c26_body;
	case 11: goto c2e_body;
	case 12: goto c38_body;
	case 13: goto c3a_body;
	case 14: goto c4e_body;
	default: return 0;
	}

bc2_body:	// 8C101BC2: fmov.s @r14+,fr4 (par) + cabeca do laco (mesmo bloco do BC4)
	LDP(f4, f5, r14); r14 += 8;
	goto bc4_body;

bc4_top:
	ENTER(0x8C101BC4, C_BC4);
bc4_body:	// 8C101BC4: carrega par, ftrv, dt r3, bt.s 8C101C5C
	LDP(f6, f7, r14); r14 += 8;
	r7 = r6;
	f2 = f7 + 0.f;			// fldi0 fr2 + fadd fr7,fr2 (dobrado pelo JIT)
	f7 = 1.f;
	r6 += 64;
	f3 = 0.f;
	r5 += 32;
	LDP(f0, f1, r14); r14 += 8;
	FTRV(f4, f5, f6, f7);
	f11 = 0.f;
	r3 -= 1;
	T = r3 == 0;
	r0 = RD32(r4); r4 += 4;
	r1 = r14;			// slot do bt.s
	if (T) { next = A(0x8C101C5C); goto out_bail; }
	// cai no 8C101BE2

be2_top:
	ENTER(0x8C101BE2, C_BE2);
be2_body:	// 8C101BE2: fipr (luz); tst #1,r0; bf.s 8C101C0C
	FIPR(f3, f0, f1, f2, f3, f12, f13, f14, f15);
	T = (r0 & 1) == 0;
	r14 = RD32(r4); r4 += 4;
	f7 = 1.f;			// slot do bf.s
	if (!T) goto c0c_top;
	// cai no 8C101BEC

bec_top:
	ENTER(0x8C101BEC, C_BEC);
bec_body:	// 8C101BEC: fdiv fr4,fr7; pref @r4; cai no 8C101BF0
	f7 = f7 / f4;
	PREF(r4);
	// cai no 8C101BF0

bf0_body:	// 8C101BF0: fcmp/gt; add r4,r14; flds; fipr; pref; bf.s 8C101C38
	T = f3 > f11;
	r14 = r14 + r4;
	fpulv = f2u(f15);
	FIPR(f11, f8, f9, f10, f11, f0, f1, f2, f3);
	PREF(r14);
	f2 = u2f(fpulv);		// slot do bf.s
	if (!T) goto c38_top;
	// cai no 8C101BFE

bfe_top:
	ENTER(0x8C101BFE, C_BFE);
bfe_body:	// 8C101BFE: fadd fr3,fr2; fldi0 fr3; bra 8C101C26; ftrc
	f2 = f2 + f3;
	f3 = 0.f;
	fpulv = FTRC(f11);		// slot do bra
	goto c26_top;

c0c_top:
	ENTER(0x8C101C0C, C_C0C);
c0c_body:	// 8C101C0C: fdiv fr4,fr7; cai no 8C101C0E
	f7 = f7 / f4;
	// cai no 8C101C0E

c0e_body:	// 8C101C0E: add #24,r4; fcmp; flds; fipr; r14=r4-32; pref; bf.s 8C101C38
	r4 += 24;
	T = f3 > f11;
	fpulv = f2u(f15);
	FIPR(f11, f8, f9, f10, f11, f0, f1, f2, f3);
	r14 = r4 - 32;
	PREF(r4);
	f2 = u2f(fpulv);		// slot do bf.s
	if (!T) goto c38_top;
	// cai no 8C101C20

c20_top:
	ENTER(0x8C101C20, C_C20);
c20_body:	// 8C101C20: fadd fr3,fr2; ftrc; fldi0; sts; cmp/gt; shll2; bf 8C101C3A
	f2 = f2 + f3;
	fpulv = FTRC(f11);
	f3 = 0.f;
	r0 = fpulv;
	T = (s32)r0 > (s32)r2;
	r0 = r0 << 2;
	if (!T) goto c3a_top;
	goto c2e_top;			// cai no 8C101C2E (bf sem delay slot)

c26_top:
	ENTER(0x8C101C26, C_C26);
c26_body:	// 8C101C26: sts FPUL,r0; cmp/gt r2,r0; shll2 r0; bf 8C101C3A
	r0 = fpulv;
	T = (s32)r0 > (s32)r2;
	r0 = r0 << 2;
	if (!T) goto c3a_top;
	goto c2e_top;			// cai no 8C101C2E

c2e_top:
	ENTER(0x8C101C2E, C_C2E);
c2e_body:	// 8C101C2E: fschg; fmov.s @(R0,r8),fr3 (4 bytes); bra 8C101C3A; fschg
	f3 = RDF(r8 + r0);		// SZ trocado so para esta leitura de 4 bytes
	goto c3a_top;			// bra 8C101C3A

c38_top:
	ENTER(0x8C101C38, C_C38);
c38_body:	// 8C101C38: fldi0 fr3; cai no corpo do 8C101C3A (mesmo bloco)
	f3 = 0.f;
	goto c3a_body;

c3a_top:
	ENTER(0x8C101C3A, C_C3A);
c3a_body:	// 8C101C3A: 2 fmul, 4 pares + cabecalho de 32 bytes na SQ; bt.s 8C101C52
	LDP(f0, f1, r1);
	f6 = f6 * f7;
	r6 -= 8; SQP(f2, f3, r6);
	f5 = f5 * f7;
	r6 -= 8; SQP(f0, f1, r6);
	T = r3 == 0;
	r6 -= 8; SQP(f6, f7, r6);
	r6 -= 8; SQP(f4, f5, r6);
	SQI(r7, r6);			// slot do bt.s
	if (T) { next = A(0x8C101C52); goto out_bail; }
	// cai no 8C101C4E

c4e_top:
	ENTER(0x8C101C4E, C_C4E);
c4e_body:	// 8C101C4E: fmov.s @r14+,fr4 (par); bra 8C101BC4; pref @r6
	LDP(f4, f5, r14); r14 += 8;
	PREF(r6);			// slot do bra
	goto bc4_top;

c08_body:	// 8C101C08: add #-28,r4; fldi1 fr7; cai no 8C101C0C
	r4 -= 28;
	f7 = 1.f;
	goto c0c_body;

out_bail:
	FLUSH();
	next_pc = next;
	return (1ull << 32) | (u32)c;
out:
	// estado ja gravado pelo ENTER (FLUSH antes do UpdateSystem)
	next_pc = next;
	return (1ull << 32) | (u32)c;
#undef RD32
#undef RDF
#undef LDP
#undef SQI
#undef SQP
#undef FLUSHSQ
#undef PREF
#undef RELOAD
#undef FLUSH
#undef ENTER
#undef FTRV
#undef FIPR
#undef FTRC
#undef A
}


// ---------------------------------------------------------------------------
// memset16/32: lacos de limpeza (Padroes 1, 2 e 5 do perfil do JIT). O bloco
// casado e' o cabecalho do laco (store, decremento do contador, avanco do
// destino, teste e `bf` de volta); a versao nativa roda o laco inteiro e sai
// logo depois dele. entry 0: contador r6/passo 2 (mov.w); 1: r5/passo 2;
// 2: r6/passo 4 (mov.l). Valor em r7, destino em r4.
u64 memset_run(s32 c, u32 entry, u32 vaddr)
{
	const u32 val = r[7];
	const u32 dest = r[4];
	const u32 cntReg = (entry == 1) ? 5 : 6;
	const u32 cnt = r[cntReg];
	const u32 step = (entry == 2) ? 4 : 2;
	const bool is32 = (entry == 2);

	// Contador zero: o laco SH4 e' do-while (store antes do teste) e daria a
	// volta em 2^32. Declina e deixa o JIT rodar o bloco, como faria sem HLE.
	if (cnt == 0)
		return 0;

	// Escrita pelo mesmo caminho do JIT (WriteMem16/32 cuida de SMC/MMIO).
	for (u32 i = 0; i < cnt; i++)
	{
		if (is32)
			WriteMem32(dest + i * 4, val);
		else
			WriteMem16(dest + i * 2, val & 0xFFFF);
	}

	r[4] = dest + cnt * step;
	r[cntReg] = 0;
	sr.T = 1;	// tst <contador>,<contador> com 0 seta T

	// O JIT ja descontou a fatia de ciclos deste bloco (uma volta do laco)
	// antes de chamar o HLE; desconta as voltas restantes. 5 por volta =
	// mov(1)+add(1)+add(1)+tst(1)+bf(1).
	c -= (cnt - 1) * 5;
	next_pc = vaddr + 10;
	if (__builtin_expect(c < 0, 0))
	{
		c += sh4_sched_timeslice;
		if (UpdateSystem() != 0)
			next_pc = rdv_DoInterrupts_pc(vaddr + 10);
	}
	return (1ull << 32) | (u32)c;
}

// ---------------------------------------------------------------------------
// ocbp: expurgo de cache (Padrao 23). Bloco = `ocbp @r4 / dt r5 / bf.s /
// add #32,r4`. O JIT trata o ocbp como no-op (dec vazio, o dado ja esta na
// memoria do host), entao o nativo so avanca o ponteiro e zera o contador.
u64 ocbp_run(s32 c, u32 entry, u32 vaddr)
{
	const u32 dest = r[4];
	const u32 cnt = r[5];

	if (cnt == 0)
		return 0;	// dt daria a volta em 2^32: deixa o JIT rodar

	r[4] = dest + cnt * 32;
	r[5] = 0;
	sr.T = 1;	// dt r5 com 0 seta T

	// 4 por volta = ocbp(1)+dt(1)+bf.s(1)+add(1); uma volta ja foi descontada.
	c -= (cnt - 1) * 4;
	next_pc = vaddr + 8;
	if (__builtin_expect(c < 0, 0))
	{
		c += sh4_sched_timeslice;
		if (UpdateSystem() != 0)
			next_pc = rdv_DoInterrupts_pc(vaddr + 8);
	}
	return (1ull << 32) | (u32)c;
}

// ---------------------------------------------------------------------------
// B1: strips com luz difusa (modo r8 == 0) da rotina de T&L da biblioteca dos
// jogos de luta/AM2 (grupo 005 do sdk_find; Shenmue II 8C1D8A80 ~6% da emu).
// Pseudo bit-exato: docs/sdk_find/pseudo/strips_com_luz_difusa_pseudo.cpp. O
// modo r8 impar (sem luz, grupo 004) e o par (clamp, doa2_run) ficam como estao;
// aqui so o r8 == 0. Layout DOA2 (DOA2, MvC2, CvS2, Shenmue II 8C1D8A80).
const u16 stripluz_s_entry[] = { 0x2888, 0x6083, 0x8B00, 0xA094, 0xC801, 0x8B01, 0xA13B, 0x0009 };
const u16 stripluz_s_headA[] = { 0x6046, 0x4310, 0xFF1D, 0xF79D, 0xF743, 0x6146, 0x8D65, 0xF3ED };
const u16 stripluz_s_headB[] = { 0xC801, 0x3E4C, 0x8D03, 0xF20D };
const Span stripluz_sig[] = {
	{ 0x8C101920, sizeof(stripluz_s_entry) / 2, stripluz_s_entry, nullptr },
	{ 0x8C101A7A, sizeof(stripluz_s_headA) / 2, stripluz_s_headA, nullptr },
	{ 0x8C101ADE, sizeof(stripluz_s_headB) / 2, stripluz_s_headB, nullptr },
};
// entradas (offset relativo a 8C101920): 0 = funcao, 1 = cabeca A, 2 = cabeca B
static const u32 stripluz_eo[] = { 0x000, 0x15A, 0x1BE };

struct SlCosts { u8 entry2, pro, pro0, pro1, headA, postA, a_tst, a_f1, a_f0, a_body, a_post,
		headB, b_f1, b_f0, b_body, b24, b2e, b34, b3e, b48, b80, b54, b5e, b64, b6e, b78, tail, callee; };
struct SlVariant { bool cb, eosAll1; u16 headA, headB; SlCosts c; };
//                                     e2 pro p0 p1 hA pA  at f1 f0 ab ap  hB f1 f0 bb  b24 2e 34 3e 48 80  b54 5e 64 6e 78  tl ce
static const SlVariant SL_DOA2 = { false, false, 0x15A, 0x1BE,
	{ 2, 7, 8, 5,  4, 0,  3, 5, 2, 11, 0,  3, 5, 2, 8,  4, 3, 5, 4, 4, 1,  4, 3, 5, 4, 5,  6, 0 } };

struct SlScaled { s32 entry2, pro, pro0, pro1, headA, postA, a_tst, a_f1, a_f0, a_body, a_post,
		headB, b_f1, b_f0, b_body, b24, b2e, b34, b3e, b48, b80, b54, b5e, b64, b6e, b78, tail, callee,
		unitA, unitB, entry; };
static SlScaled sl_scale(const SlVariant &V, float clk)
{
	auto S = [clk](u8 n) -> s32 { return n == 0 ? 0 : (s32)std::max(1.f, n * clk); };
	SlScaled C;
	C.entry2 = S(V.c.entry2); C.pro = S(V.c.pro); C.pro0 = S(V.c.pro0); C.pro1 = S(V.c.pro1);
	C.headA = S(V.c.headA); C.postA = S(V.c.postA); C.a_tst = S(V.c.a_tst); C.a_f1 = S(V.c.a_f1);
	C.a_f0 = S(V.c.a_f0); C.a_body = S(V.c.a_body); C.a_post = S(V.c.a_post); C.headB = S(V.c.headB);
	C.b_f1 = S(V.c.b_f1); C.b_f0 = S(V.c.b_f0); C.b_body = S(V.c.b_body); C.b24 = S(V.c.b24);
	C.b2e = S(V.c.b2e); C.b34 = S(V.c.b34); C.b3e = S(V.c.b3e); C.b48 = S(V.c.b48); C.b80 = S(V.c.b80);
	C.b54 = S(V.c.b54); C.b5e = S(V.c.b5e); C.b64 = S(V.c.b64); C.b6e = S(V.c.b6e); C.b78 = S(V.c.b78);
	C.tail = S(V.c.tail); C.callee = S(V.c.callee);
	const s32 cabA = C.headA + (V.cb ? C.callee + C.postA : 0);
	const s32 corpoA = std::max(C.a_tst + C.a_f1 + C.a_body,
			C.b54 + C.b5e + C.b64 + C.b6e + C.a_f0 + C.a_body) + (V.cb ? C.callee + C.a_post : 0);
	const s32 desvB = C.b24 + C.b2e + C.b34 + C.b3e + C.b_f0 + C.b_body;
	C.unitA = cabA + corpoA + desvB;
	C.unitB = C.headB + std::max(C.b_f1, C.b_f0) + C.b_body;
	C.entry = C.entry2 + C.pro + std::max(C.pro0, C.pro1) + (V.cb ? C.callee + C.postA : 0)
			+ C.unitA - cabA;
	return C;
}

// entry 0: +000 (entrada); 1: +15A (cabeca A); 2: +1BE (cabeca B)
u64 stripluz_run(s32 c, u32 entry, u32 vaddr)
{
	const SlVariant &V = SL_DOA2;
	const SlScaled C = sl_scale(V, g_lutSh4Clock > 0.f ? g_lutSh4Clock : settings.dreamcast.sh4clock);
	const u32 base = vaddr - stripluz_eo[entry];
	const u32 here = vaddr;

	// Pre-condicoes. Recusar (bail no proprio bloco) e sempre exato: o JIT segue.
	if (entry == 0 && r[8] != 0)
		return 0;					// modos r8 != 0: grupo 004 / clamp
	if (fpscr.PR || fpscr.SZ != (entry == 0 ? 0u : 1u))
		return 0;
	if (!is_ram(r[4]) || (r[6] >> 26) != 0x38)
		return 0;					// stream fora da RAM / destino fora da SQ
	const s32 need = entry == 0 ? C.entry : entry == 1 ? C.unitA - C.headA : C.unitB - C.headB;
	if (c < need)
		return 0;

	const u8 *ram = mem_b.data;
	const u32 ramMask = RAM_MASK;
	u8 *sq = (u8 *)p_sh4rcb->sq_buffer;
#define RD32(a) ({ u32 v_; memcpy(&v_, ram + ((a) & ramMask), 4); v_; })
#define RDF(a) ({ f32 v_; memcpy(&v_, ram + ((a) & ramMask), 4); v_; })
#define SQW(a, v) do { u32 v_ = (v); memcpy(sq + ((a) & 0x3C), &v_, 4); } while (0)
#define FLUSHSQ(a) do { \
		sqw_fp *fn_ = do_sqw_nommu; \
		if ((void *)fn_ == ta_sq_stub) fn_ = (sqw_fp *)&TAWriteSQ; \
		fn_((a), sq); \
	} while (0)

	u32 r0, r1, r2, r3, r4, r5, r6, r14, fpulv, T, szbit;
	f32 f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11;
	u32 next = 0;
	float32x4_t m0, m1, m2, m3, Lv;
#define RELOAD() do { \
		r0 = r[0]; r1 = r[1]; r2 = r[2]; r3 = r[3]; r4 = r[4]; r5 = r[5]; r6 = r[6]; r14 = r[14]; \
		f0 = fr[0]; f1 = fr[1]; f2 = fr[2]; f3 = fr[3]; f4 = fr[4]; f5 = fr[5]; f6 = fr[6]; f7 = fr[7]; \
		f8 = fr[8]; f9 = fr[9]; f10 = fr[10]; f11 = fr[11]; fpulv = fpul; T = sr.T; szbit = fpscr.SZ; \
		m0 = vld1q_f32(&xf[0]); m1 = vld1q_f32(&xf[4]); m2 = vld1q_f32(&xf[8]); m3 = vld1q_f32(&xf[12]); \
		Lv = vld1q_f32(&fr[12]); \
	} while (0)
#define FLUSH_SZ(sz) do { \
		r[0] = r0; r[1] = r1; r[2] = r2; r[3] = r3; r[4] = r4; r[5] = r5; r[6] = r6; r[14] = r14; \
		fr[0] = f0; fr[1] = f1; fr[2] = f2; fr[3] = f3; fr[4] = f4; fr[5] = f5; fr[6] = f6; fr[7] = f7; \
		fr[8] = f8; fr[9] = f9; fr[10] = f10; fr[11] = f11; fpul = fpulv; sr.T = T; fpscr.SZ = (sz); \
	} while (0)
	// ftrv / fipr como o JIT
#define TR(a, b, cc, d) do { \
		float32x4_t acc_ = vmulq_n_f32(m0, a); \
		acc_ = vfmaq_n_f32(acc_, m1, b); acc_ = vfmaq_n_f32(acc_, m2, cc); acc_ = vfmaq_n_f32(acc_, m3, d); \
		a = vgetq_lane_f32(acc_, 0); b = vgetq_lane_f32(acc_, 1); cc = vgetq_lane_f32(acc_, 2); d = vgetq_lane_f32(acc_, 3); \
	} while (0)
#define FIPR0() do { \
		const float32x4_t va_ = { f0, f1, f2, f3 }; \
		const float32x4_t pr_ = vmulq_f32(va_, Lv); \
		const float32x4_t s_ = vpaddq_f32(pr_, pr_); \
		f3 = vpadds_f32(vget_low_f32(s_)); \
	} while (0)
#define PAIR(a, lo, hi) do { lo = RDF(a); hi = RDF(a + 4); } while (0)
#define SQP(lo, hi) do { r6 -= 8; SQW(r6, f2u(lo)); SQW(r6 + 4, f2u(hi)); } while (0)

	RELOAD();
	const u32 amb = f2u(fr[15]);
	auto eos = [&]() -> u32 { return (u32)((s32)r6 >> 1); };

	if (entry == 1) goto headA_body;
	if (entry == 2) goto headB_body;

	// +000 (pago pelo JIT): tst r8,r8 ; mov r8,r0 ; bf (nao salta: r8 == 0)
	r0 = 0;
	c -= C.entry2;					// +006: bra +132 ; slot tst #1,r0
	T = 1;
	c -= C.pro;						// +132
	r3 = RD32(r4); r4 += 4;
	r6 += 32;						// fschg: SZ 0 -> 1
	r0 = RD32(r4); r14 = r4; f2 = 0.f;
	T = !(r0 & 1); r4 += 32;
	if (T) { c -= C.pro0; r14 = RD32(r14 + 4); r4 -= 24; r14 += r4; }
	else   { c -= C.pro1; }
	PAIR(r14, f4, f5); r14 += 8;
	r2 = r6;
	PAIR(r14, f6, f7); r14 += 8;
	f2 = f2 + f7;
	f7 = 1.f; f3 = 0.f;
	PAIR(r14, f0, f1); r14 += 8;
	TR(f4, f5, f6, f7);
	goto headA_body;

headA:
	if (c < C.unitA) { FLUSH_SZ(1); next_pc = base + V.headA; return (1ull << 32) | (u32)c; }
	c -= C.headA;
headA_body:
	r0 = RD32(r4); r4 += 4;
	T = --r3 == 0;
	fpulv = amb;
	f7 = 1.f; f7 = f7 / f4;
	r1 = RD32(r4); r4 += 4;
	FIPR0();
	if (T) goto endA;
	c -= C.a_tst;
	T = !(r0 & 1); r1 += r4; f2 = u2f(fpulv);
	if (!T) { c -= C.a_f1; r1 = r4; r4 += 24; r1 -= 8; }
	else    { c -= C.a_f0; }
bodyA:
	f0 = 0.f; T = f3 > f0;
	PAIR(r14, f0, f1);
	r4 += 32;
	PAIR(r1, f8, f9); r1 += 8;
	if (T) f2 = f2 + f3;
	c -= C.a_body;
	f3 = 0.f;
	T = --r3 == 0;
	SQP(f2, f3);
	f2 = f2 - f2;
	r4 -= 32;
	PAIR(r1, f10, f11); r1 += 8;
	f6 = f6 * f7;
	SQP(f0, f1);
	f2 = f2 + f11;
	f11 = 1.f;
	f5 = f5 * f7;
	PAIR(r1, f0, f1); r1 += 8;
	r5 += 32;
	SQP(f6, f7);
	TR(f8, f9, f10, f11);
	SQP(f4, f5);
	SQW(r6, r2);
	r2 = r6;
	FLUSHSQ(r6);
	r6 += 64;
	r0 = RD32(r4); r4 += 4;
	r14 = RD32(r4); r4 += 4;
	fpulv = amb;
	FIPR0();
	f11 = 1.f; f11 = f11 / f8;
	if (T) goto endB;
	goto headB;

headB:
	if (c < C.unitB) { FLUSH_SZ(1); next_pc = base + V.headB; return (1ull << 32) | (u32)c; }
	c -= C.headB;
headB_body:
	T = !(r0 & 1); r14 += r4; f2 = u2f(fpulv);
	if (!T) { c -= C.b_f1; r14 = r4; r4 += 24; r14 -= 8; }
	else    { c -= C.b_f0; }
bodyB:
	f0 = 0.f; T = f3 > f0;
	PAIR(r1, f0, f1);
	r4 += 32;
	PAIR(r14, f4, f5); r14 += 8;
	if (T) f2 = f2 + f3;
	c -= C.b_body;
	f3 = 0.f;
	SQP(f2, f3);
	f2 = f2 - f2;
	r4 -= 32;
	PAIR(r14, f6, f7); r14 += 8;
	f10 = f10 * f11;
	SQP(f0, f1);
	f2 = f2 + f7;
	f7 = 1.f;
	f9 = f9 * f11;
	PAIR(r14, f0, f1); r14 += 8;
	r5 += 32;
	SQP(f10, f11);
	TR(f4, f5, f6, f7);
	SQP(f8, f9);
	SQW(r6, r2); r2 = r6; FLUSHSQ(r6);
	r6 += 64;
	goto headA;

endA:
	c -= C.b54;
	T = (s32)r0 > 0; f2 = u2f(fpulv); r2 = eos();
	if (T) {
		c -= C.b5e;
		T = !(r0 & 0x80); r3 = r1;
		if (!T) {
			c -= C.b64;
			r0 = RD32(r4); r4 += 4; T = !(r0 & 1);
			r1 = RD32(r4); r4 += 4; r1 += r4;
			if (!T) { c -= C.b6e; r1 = r4; r4 += 24; r1 -= 8; }
			c -= C.a_f0;
			goto bodyA;
		}
	}
	c -= C.b78;
	r1 = r14;
	goto tail;

endB:
	c -= C.b24;
	T = (s32)r0 > 0; f2 = u2f(fpulv); r2 = eos();
	if (T) {
		c -= C.b2e;
		T = !(r0 & 0x80); r3 = r14;
		if (!T) {
			c -= C.b34;
			r0 = RD32(r4); r4 += 4; T = !(r0 & 1);
			r14 = RD32(r4); r4 += 4; r14 += r4;
			if (!T) { c -= C.b3e; r14 = r4; r4 += 24; r14 -= 8; }
			c -= C.b_f0;
			goto bodyB;
		}
	}
	c -= C.b48 + C.b80;
	f4 = f8; f5 = f9;
	f6 = f10; f7 = f11;

tail:
	f0 = 0.f; T = f3 > f0;
	PAIR(r1, f10, f11);
	if (T) f2 = f2 + f3;
	c -= C.tail;
	f3 = 0.f;
	f6 = f6 * f7;
	SQP(f2, f3);
	f5 = f5 * f7;
	SQP(f10, f11);
	r4 -= 8;
	SQP(f6, f7);
	r5 += 32;
	SQP(f4, f5);
	SQW(r6, r2);
	FLUSHSQ(r6);
	r6 += 32;
	FLUSH_SZ(0);
	next_pc = pr;
	return (1ull << 32) | (u32)c;
#undef RD32
#undef RDF
#undef SQW
#undef FLUSHSQ
#undef RELOAD
#undef FLUSH_SZ
#undef TR
#undef FIPR0
#undef PAIR
#undef SQP
}

} // namespace

// Chamado pelo JIT na compilacao do bloco: esta funcao e conhecida aqui?
bool hle_fn_lookup(u32 vaddr, u32 *id)
{
	if (!hleEnabled())
		return false;
	// id = (funcao << 8) | entrada

	// Checa padrões dinâmicos (independente de vaddr)
	const u16 memset16_r6_ops[] = { 0x2471, 0x76FF, 0x7402, 0x2668, 0x8BFA };
	const u16 memset16_r5_ops[] = { 0x2471, 0x75FF, 0x7402, 0x2558, 0x8BFA };
	const u16 memset32_r6_ops[] = { 0x2472, 0x76FF, 0x7404, 0x2668, 0x8BFA };
	const u16 ocbp_r5_ops[] = { 0x04A3, 0x4510, 0x8FFC, 0x7420 };
	
	const u8 *mem = GetMemPtr(vaddr, 10);
	if (mem != nullptr) {
		if (memcmp(mem, memset16_r6_ops, 10) == 0) {
			if (hleLog()) fprintf(stderr, "hle: memset16 r6 @ %08X instalada\n", vaddr);
			*id = 0x200; return true;
		}
		if (memcmp(mem, memset16_r5_ops, 10) == 0) {
			if (hleLog()) fprintf(stderr, "hle: memset16 r5 @ %08X instalada\n", vaddr);
			*id = 0x201; return true;
		}
		if (memcmp(mem, memset32_r6_ops, 10) == 0) {
			if (hleLog()) fprintf(stderr, "hle: memset32 r6 @ %08X instalada\n", vaddr);
			*id = 0x202; return true;
		}
		if (memcmp(mem, ocbp_r5_ops, 8) == 0) {
			if (hleLog()) fprintf(stderr, "hle: ocbp r5 @ %08X instalada\n", vaddr);
			*id = 0x300; return true;
		}
	}

	// Funcoes de biblioteca casaveis em qualquer endereco (4.125): a base do
	// jogo sai de vaddr - entryOff[entrada] e o match completo (todos os Span)
	// decide. Uma variante de SDK = bytes diferentes = outra HleFunc.
	static const HleFunc funcs[] = {
		{ "lightxf", 0, 0x8C14DDC0, lightxf_sig, sizeof(lightxf_sig) / sizeof(lightxf_sig[0]),
			lightxf_eo, sizeof(lightxf_eo) / sizeof(lightxf_eo[0]) },
		{ "stripemit", 1, 0x8C14D440, strip_sig, sizeof(strip_sig) / sizeof(strip_sig[0]),
			strip_eo, sizeof(strip_eo) / sizeof(strip_eo[0]) },
		{ "doa2", 4, 0x8C101BC2, doa2_sig, sizeof(doa2_sig) / sizeof(doa2_sig[0]),
			doa2_eo, sizeof(doa2_eo) / sizeof(doa2_eo[0]) },
		{ "stripluz", 6, 0x8C101920, stripluz_sig, sizeof(stripluz_sig) / sizeof(stripluz_sig[0]),
			stripluz_eo, sizeof(stripluz_eo) / sizeof(stripluz_eo[0]) },
	};
	for (const HleFunc &f : funcs)
	{
		for (size_t e = 0; e < f.nEntries; e++)
		{
			const u32 base = vaddr - f.entryOff[e];
			if (!func_matches(f, base))
				continue;
			static u32 loggedMask;
			const u32 bit = 1u << f.funcId;
			if (!(loggedMask & bit))
				fprintf(stderr, "hle: %s instalada (base %08X)\n", f.name, base);
			loggedMask |= bit;
			*id = ((u32)f.funcId << 8) | (u32)e;
			return true;
		}
	}
	return false;
}

// Retorno: bit 32 = tratou (w27 = 32 bits baixos, next_pc no contexto);
// 0 = nao tratou, o bloco do JIT segue normalmente.
extern "C" u64 hle_fn_run(s32 cycles, u32 id, u32 vaddr)
{
	switch (id >> 8)
	{
	case 0: return lightxf_run(cycles, id & 0xFF, vaddr);
	case 1: return stripemit_run(cycles, id & 0xFF, vaddr);
	case 2: return memset_run(cycles, id & 0xFF, vaddr);
	case 3: return ocbp_run(cycles, id & 0xFF, vaddr);
	case 4: return doa2_run(cycles, id & 0xFF, vaddr);
	case 6: return stripluz_run(cycles, id & 0xFF, vaddr);
	default: return 0;
	}
}
