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

extern uintptr_t t2_last_pc;
extern float g_lutSh4Clock;
extern void *ta_sq_stub;

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

struct Span { u32 addr; u32 count; const u16 *ops; };

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

bool spans_match(const Span *s, size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		const u8 *mem = GetMemPtr(s[i].addr, s[i].count * 2);
		if (mem == nullptr || memcmp(mem, s[i].ops, s[i].count * 2) != 0)
			return false;
	}
	return true;
}

inline bool is_ram(u32 a) { return ((a >> 29) & 7) != 7 && ((a >> 26) & 7) == 3; }
inline u32 rd32(u32 a) { u32 v; memcpy(&v, &mem_b.data[a & RAM_MASK], 4); return v; }
inline u32 f2u(f32 f) { u32 v; memcpy(&v, &f, 4); return v; }
inline f32 u2f(u32 u) { f32 v; memcpy(&v, &u, 4); return v; }

struct Stats { u64 calls, bails; } lightxf_stats;

u64 lightxf_decline(u32 entry, const char *why)
{
	static u32 n;
	if (hleLog() && n++ < 20)
		fprintf(stderr, "hle: lightxf recusou (entrada %u): %s r3=%08X r4=%08X r5=%08X r7=%08X r15=%08X fpscr=%08X\n",
				entry, why, r[3], r[4], r[5], r[7], r[15], fpscr.full);
	return 0;
}

// entry 0: 8C14DDC0 (entrada da funcao); entry 1: 8C14DDD6 (cabeca do laco)
u64 lightxf_run(s32 c, u32 entry)
{
	static const u32 lit = 0x8C14DE08;	// mov.l @(disp,PC),r1 (DDEA)

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
			t2_last_pc = 0; \
			if (UpdateSystem() != 0) { next = rdv_DoInterrupts_pc(addr); goto out; } \
			if (!Sh4cntx.CpuRunning) { next = addr; goto out; } \
			RELOAD(); \
		} \
	} while (0)
	// Sai na entrada do bloco `addr` sem descontar nada: o JIT continua dali.
#define BAIL(addr) do { lightxf_stats.bails++; next = addr; goto out_bail; } while (0)

	RELOAD();
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
}

} // namespace

// Chamado pelo JIT na compilacao do bloco: esta funcao e conhecida aqui?
bool hle_fn_lookup(u32 vaddr, u32 *id)
{
	if (!hleEnabled() || (vaddr != 0x8C14DDC0 && vaddr != 0x8C14DDD6))
		return false;
	if (!spans_match(lightxf_sig, sizeof(lightxf_sig) / sizeof(lightxf_sig[0])))
		return false;
	*id = vaddr == 0x8C14DDC0 ? 0 : 1;
	static bool logged;
	if (!logged)
		fprintf(stderr, "hle: lightxf (8C14DDC0) instalada\n");
	logged = true;
	return true;
}

// Retorno: bit 32 = tratou (w27 = 32 bits baixos, next_pc no contexto);
// 0 = nao tratou, o bloco do JIT segue normalmente.
extern "C" u64 hle_fn_run(s32 cycles, u32 id)
{
	return lightxf_run(cycles, id);
}
