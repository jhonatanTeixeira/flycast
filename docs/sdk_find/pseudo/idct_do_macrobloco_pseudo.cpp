// IDCT do macrobloco (decodificador de vídeo MPEG-1 da Sofdec/MPV) — pseudo-C++ para
// os 7 jogos do grupo 010 (docs/sdk_find/auto/010_matriz.md).
//
// Pseudo-código: não compila no core. É a base para um `hle_fn` relocável (4.125), no
// mesmo modelo de core/rec-ARM64/hle_fn.cpp (lightxf_run / stripemit_run / doa2_run).
//
// API usada nos *_pseudo.cpp (a mesma de docs/sdk_blocks/luz_e_transformacao_de_vertices_pseudo.cpp)
//   Sh4 &s            registradores: s.r[16], s.fr[16], s.xf[16] (XMTRX), s.fpscr, s.sr.T, s.fpul, s.pr
//   ram32/ramf/ram16  leitura/escrita direta da RAM emulada (endereço & RAM_MASK), sem checagem;
//                     ram32w/ram16w gravam; ramf_ptr(a) = ponteiro do host; is_ram(a)
//   push32/pop32      pilha do SH4 (r15)
//   Cycles &cyc       cyc.left() = o que sobra na fatia; cyc.take(n) desconta; cyc.update() =
//                     UpdateSystem na fronteira de bloco em que o JIT faria (true = interrupção
//                     ou CPU parada: o JIT assume dali)
//   bail(pc)          devolve ao JIT na entrada do bloco `pc` (estado já gravado, ciclos do
//                     bloco ainda não descontados)
//   leave_to_interrupt(pc)  saída do ENTER quando o UpdateSystem pede interrupção
//                     (rdv_DoInterrupts_pc(pc), como no hle_fn.cpp)
//   clk_sh4()         g_lutSh4Clock / settings.dreamcast.sh4clock
//   return_to(pc)     rts
//
// ===================================================================================
// O QUE A FUNÇÃO FAZ
// ===================================================================================
// IDCT 8×8 em ponto flutuante dos até 6 blocos de um macrobloco 4:2:0 (4 Y + Cb + Cr),
// separável, com a decomposição par/ímpar: duas matrizes 4×4 no XMTRX — A (parte par) e
// B (parte ímpar) — e a borboleta x[k] = par[k] ± ímpar[k]. Quatro passadas:
//
//   P1  A em todos os vetores "pares" de cada bloco codificado, em linha (sobrescreve
//       os coeficientes)                                               ftrv × 8 por bloco
//   P2  B nos vetores "ímpares" + borboleta com o resultado do P1 → buffer temporário
//       (float, 8 linhas × 4 colunas por meia-área)                    ftrv × 8 por bloco
//   P3  = P1 no buffer temporário (segunda dimensão)
//   P4  = P2 no temporário, mais o arredondamento: short = ftrc(x + 512.5f) − 512, ou
//       seja floor(x + 0.5) (sem clamp); grava 8×8 `short` (128 bytes, linha de 16
//       bytes) no endereço de saída do bloco (floor só vale para x > −512.5; abaixo
//       disso o ftrc trunca para cima)
//
// Bloco não codificado (bit do CBP em 0) é pulado nas 4 passadas (`shll r6; bf`), mas o
// ponteiro da lista de saída avança mesmo assim.
//
// Entrada: r4 = objeto do decodificador; a função usa P = r4 + 32. O objeto é montado por
// uma função de init que existe nos 7 dumps (Napple 8C1A8630) e que grava o ponteiro
// desta função em obj+12 (o chamador faz `jsr` por esse ponteiro), além das constantes
// abaixo (mesmos valores nos 7 jogos):
//
//   P+0   float  bias de arredondamento        = 512.5f  (0x44002000, init)
//   P+4   u32    passo entre blocos (bytes)    = 0x100   (init)  — P2 usa 0x100 fixo
//   P+8   u32    máscara de blocos (CBP << 26): bit 31 = bloco 0 … bit 26 = bloco 5
//   P+12  u32    coeficientes: 6 × 256 bytes (64 floats por bloco), sobrescritos pelo P1
//   P+20  u32    → lista de 6 ponteiros de saída; a função AVANÇA P+20 em 24 bytes
//   P+24  s32    bias inteiro                  = 0x200   (init)
//   P+28  u32    0x80 (init; não usado aqui)
//   P+32  u32    → matriz A (16 floats, ordem do XMTRX: coluna k = floats 4k..4k+3)
//   P+36  u32    → matriz B (idem). No Napple A e B vêm de *(8C3F1F68)/*(8C3F1F6C),
//                  copiadas de tabelas do executável (8C1A86BC / 8C1A86C0)
//   P+40  u32    temporário: 6 × 256 bytes (float)
//   P+44  u32    contador do P4 (escrito: termina 0)
//   P+48  u32    rascunho do P4 (r3 salvo a cada iteração)
//
// Layout de um bloco de 256 bytes (coeficientes e temporário): 16 vetores de 4 floats.
//   vetor k (k = 0..7)  = "par" do sinal k       (A age nele)
//   vetor 8+k           = "ímpar" do sinal k     (B age nele)
// O P1/P3 grava A·v com as metades trocadas: [y2 y3 y0 y1] (fmov DR0,@-r1 grava o par
// fr0/fr1 em +8 e depois fr2/fr3 em +0). O P2/P4 lê de volta trocando de novo
// (DR2 ← +0, DR0 ← +8), então em registrador volta a ser [y0 y1 y2 y3]. A RAM no meio
// tem que ficar trocada (estado bit a bit).
// Saída da borboleta, coluna i (i = 0..7), linhas (stride 16 bytes):
//   +0x00 e0+o0  +0x10 e3−o3  +0x20 e1+o1  +0x30 e2−o2
//   +0x40 e2+o2  +0x50 e1−o1  +0x60 e3+o3  +0x70 e0−o0
// No P2 a coluna i vai para temp + (i<4 ? 4i : 128 + 4(i−4)) (float); no P4 para
// saída + 2i (short).
//
// ===================================================================================
// JOGOS (1 variante: 369 de 369 opcodes iguais, os mesmos 23 blocos do JIT nos 7 dumps;
// comparado com tools/sh4dis.py em cada dump — md5 dos opcodes idêntico)
// ===================================================================================
//   Jogo                                       entrada     perf no dump da tabela
//   Elemental Gimmick Gear v1.001              8C0B03E4    0.05%
//   Evolution - The World of Sacred Device     8C213F34    0.00% (*)
//   Evolution 2 - Far Off Promise              8C221614    0.00%
//   Grandia II                                 8C140E38    0.00%
//   King of Fighters - Evolution               8C3E4C98    0.00%
//   Napple Tale (T-En Cargodin 1.0)            8C1C1658    5.39%
//   Phantasy Star Online Ver. 2                8C411198    0.00%
// (*) os dumps da tabela (dumps_por_jogo.tsv) não pegaram FMV nesses jogos. No FMV do
//     Evolution 1 (docs/fmv_plan.md, 2026-10-09) "75% nos blocos 8C213F82..8C214ED0":
//     8C213F82 = entrada+0x4E = corpo do P1 desta função. É a "IDCT da Sofdec" que o
//     fmv_plan pede; o RE: Code Veronica tem OUTRA versão (MPV 1.14, 8C20A374…8C20AA28,
//     ~0x6B4 bytes), fora deste grupo.
//
// Variantes: **nenhuma**. Só muda o endereço base; não há literal pool (só imediatos);
// os parâmetros vêm do objeto, que a init preenche igual nos 7 jogos (512.5f, 0x100,
// 0x200). Nenhum if/else por jogo no código abaixo.
//
// ===================================================================================
// BLOCOS DO JIT (offset da entrada; n = instruções não-FPU = ciclos × clock, mín. 1)
// e perf do Napple (jit_lite_report, % da thread de emulação)
// ===================================================================================
//   off    EGG       n   bloco                                          Napple
//   0x000  8C0B03E4  30  entrada: push, fschg, XMTRX←A, ptrs, shll r6     —
//   0x04A  8C0B042E   2  P1 cabeça: shll r6; bf
//   0x04E  8C0B0432  14  P1 corpo (8 ftrv) + dt/avança                   0.59%
//   0x0AE  8C0B0492   6  P1 bloco pulado
//   0x0BA  8C0B049E  11  P2 setup: XMTRX←B, ptrs, shll r6
//   0x0DC  8C0B04C0   2  P2 cabeça
//   0x0E0  8C0B04C4   4  P2 r2=4 + 1ª iteração                           0.23%
//   0x0E6  8C0B04CA   3  P2 laço 1 (iterações 2..4)                      0.64%
//   0x11E  8C0B0502   5  P2 r0+=112, r2=4 + 5ª iteração                  0.22%
//   0x122  8C0B0506   3  P2 laço 2 (iterações 6..8)                      0.61%
//   0x15A  8C0B053E  12  P2 volta ponteiros + avança                     0.05%
//   0x164  8C0B0548   7  P2 bloco pulado (avança)
//   0x172  8C0B0556  14  P3 setup: XMTRX←A, ptrs no temporário
//   0x19A  8C0B057E   2  P3 cabeça
//   0x19E  8C0B0582  14  P3 corpo                                        0.59%
//   0x1FE  8C0B05E2   6  P3 bloco pulado
//   0x20A  8C0B05EE  15  P4 setup: XMTRX←B + 1ª cabeça (lista de saída)
//   0x22E  8C0B0612   5  P4 cabeça: r0 = *lista++, shll r6
//   0x238  8C0B061C  31  P4 r2=8 + 1ª iteração                           0.31%
//   0x23E  8C0B0622  30  P4 laço (iterações 2..8)                        1.78%  ← o mais quente
//   0x2BA  8C0B069E  10  P4 volta ponteiros + dt do contador
//   0x2C2  8C0B06A6   6  P4 bloco pulado
//   0x2CE  8C0B06B2   9  epílogo: fschg, pops, rts
// Soma dos blocos acima no Napple ≈ 5.0% (5.39% com os blocos frios). Os outros blocos
// quentes do Napple citados junto (8C1C2440, 8C1C1E54, 8C1C223C, 8C1C21A4) ficam FORA
// desta função (8C1C1658..8C1C193A): são o resto da biblioteca de vídeo (compensação de
// movimento/VLC, outros grupos). 8C1C160C (obj+8, logo antes) preenche um bloco com um
// valor (fmov fr0,@-r5 × 16): provavelmente o caminho "só DC"; não coberto aqui.
//
// Custo de uma chamada (× clock): fixo 30+11+14+15+9 = 79; por bloco codificado
// P1 16 + P2 41 + P3 16 + P4 256 = 329 (cabeças incluídas; o 1º bloco não paga a
// cabeça); por bloco pulado 8 + 9 + 8 + 11. Macrobloco cheio ≈ 2042 ciclos ≈ 4,6 fatias
// de 448: o UpdateSystem cai NO MEIO da função quase sempre, então a contabilidade tem
// que ser por bloco do JIT (ENTER abaixo), não um desconto único.
//
// ===================================================================================
// REGRAS BIT A BIT (iguais ao hle_fn.cpp)
// ===================================================================================
// - ftrv = fmul da coluna 0 + três fmla fundidas (vmulq/vfmaq), como o JIT.
// - fadd/fsub da borboleta e do bias: sem fusão (fp-contract=off); vetor = escalar.
// - ftrc = fcvtzs do JIT (satura; NaN → 0). vcvtq_s32_f32 é o mesmo fcvtzs.
// - FPSCR.SZ=1 durante toda a função (fschg na entrada e no epílogo); o P2 troca para
//   SZ=0 só nas 8 gravações de 4 bytes e volta dentro do MESMO bloco. Em toda fronteira
//   de bloco SZ=1.
// - Ordem das leituras/gravações na RAM preservada por iteração (próximo vetor ímpar do
//   P4 é lido ANTES das gravações de `short`; no P2 é lido DEPOIS das gravações de
//   float), então buffers sobrepostos continuam exatos.
// - jdyn (scratch do dynarec, fora do FC_STATE_HASH) não é reproduzido.

#pragma GCC optimize("fp-contract=off")   // só as fusões que o JIT faz (ftrv)
#include <arm_neon.h>

namespace idct_mb {

// ciclos de cada bloco do JIT: n instruções não-FPU × clock do SH4, no mínimo 1
struct Cost
{
	s32 c04A, c04E, c0AE, c0BA, c0DC, c0E0, c0E6, c11E, c122, c15A, c164,
	    c172, c19A, c19E, c1FE, c20A, c22E, c238, c23E, c2BA, c2C2, c2CE;
};
static Cost make_cost(float clk)
{
	auto c = [clk](u32 n) -> s32 { u32 v = n; v = v * clk; return (s32)std::max(1u, v); };
	return { c(2), c(14), c(6), c(11), c(2), c(4), c(3), c(5), c(3), c(12), c(7),
	         c(14), c(2), c(14), c(6), c(15), c(5), c(31), c(30), c(10), c(6), c(9) };
}

static inline float32x4_t ld4(u32 a) { return vld1q_f32(ramf_ptr(a)); }       // 16 bytes da RAM
static inline void st4(u32 a, float32x4_t v) { vst1q_f32(ramf_ptr(a), v); }
static inline float32x4_t swap64(float32x4_t v) { return vextq_f32(v, v, 2); } // [2 3 0 1]

// ftrv XMTRX,FVn como o JIT: fmul da coluna 0 + três fmla fundidas
static inline float32x4_t ftrv(const float32x4_t X[4], float32x4_t v)
{
	float32x4_t acc = vmulq_laneq_f32(X[0], v, 0);
	acc = vfmaq_laneq_f32(acc, X[1], v, 1);
	acc = vfmaq_laneq_f32(acc, X[2], v, 2);
	return vfmaq_laneq_f32(acc, X[3], v, 3);
}

// fmov.s @r0+,XDk × 8 com SZ=1: XF0..XF15 = 16 floats em ordem de memória
static inline void load_xmtrx(float32x4_t X[4], u32 &r0)
{
	for (int k = 0; k < 4; k++) X[k] = ld4(r0 + 16 * k);
	r0 += 64;
}

static bool ram_ok(u32 a, u32 len, u32 align) { return (a & (align - 1)) == 0 && is_ram(a) && is_ram(a + len - 1); }

// Chamado pelo JIT ao compilar o bloco em `base` cujos bytes batem com a assinatura.
// Os ciclos do bloco de entrada (30 × clock) já foram descontados pelo JIT.
void run(Sh4 &s, Cycles &cyc, u32 base)
{
	// ---- pré-condições (fora delas o JIT roda tudo; nada foi feito ainda) ----------
	if (s.fpscr.PR || s.fpscr.SZ)                 // fschg da entrada leva SZ 0 → 1
		return bail(base);
	const u32 P = s.r[4] + 32;
	if (!ram_ok(P, 52, 4) || !ram_ok(s.r[15] - 32, 32, 4))
		return bail(base);
	const u32 mA = ram32(P + 32), mB = ram32(P + 36);
	const u32 coef = ram32(P + 12), temp = ram32(P + 40), list = ram32(P + 20);
	if (ram32(P + 4) != 0x100                     // único passo visto (init grava 0x100);
	    || !ram_ok(mA, 64, 8) || !ram_ok(mB, 64, 8)    // com outro valor P2 ≠ P1/P3/P4
	    || !ram_ok(coef, 6 * 256 + 16, 8)         // +16: o P2 lê 1 vetor além do bloco
	    || !ram_ok(temp, 6 * 256 + 16, 8)
	    || !ram_ok(list, 24, 4))
		return bail(base);

	const Cost C = make_cost(clk_sh4());

	// ---- registradores do SH4 em locais ---------------------------------------------
	u32 r0, r1, r2 = s.r[2], r3, r4, r5, r6, r7, sp = s.r[15], T, fpul = s.fpul;
	float32x4_t f0 = vld1q_f32(&s.fr[0]), f4 = vld1q_f32(&s.fr[4]);    // fr0-3, fr4-7
	float32x4_t f8 = vld1q_f32(&s.fr[8]), f12 = vld1q_f32(&s.fr[12]);  // fr8-11, fr12-15
	float32x4_t X[4];                                                   // XMTRX
	// r8..r14 = 0x10..0x70 depois do bloco de entrada (offsets das linhas da borboleta)

	auto flush = [&]() {                        // estado exato numa fronteira de bloco
		s.r[0] = r0; s.r[1] = r1; s.r[2] = r2; s.r[3] = r3; s.r[4] = r4; s.r[5] = r5;
		s.r[6] = r6; s.r[7] = r7; s.r[15] = sp;
		for (int k = 0; k < 7; k++) s.r[8 + k] = 0x10 * (k + 1);
		vst1q_f32(&s.fr[0], f0); vst1q_f32(&s.fr[4], f4);
		vst1q_f32(&s.fr[8], f8); vst1q_f32(&s.fr[12], f12);
		for (int k = 0; k < 4; k++) vst1q_f32(&s.xf[4 * k], X[k]);
		s.fpul = fpul; s.sr.T = T; s.fpscr.SZ = 1;
	};
	// Entrada do bloco do JIT `off`: desconta; se a fatia acabou, faz o que o intc_sched
	// do JIT faz ali (estado gravado, UpdateSystem). Interrupção → o JIT segue de `off`.
#define ENTER(off, c) do {                                                        \
		cyc.take(c);                                                              \
		if (__builtin_expect(cyc.left() < 0, 0)) {                                \
			flush();                                                              \
			if (cyc.update()) return leave_to_interrupt(base + (off));            \
		}                                                                         \
	} while (0)

	// ===== 0x000 entrada (já descontada) =============================================
	sp -= 4; ram32w(sp, s.r[14]);
	r4 = P;                                      // add #32,r4 (entre os pushes)
	sp -= 4; ram32w(sp, s.r[13]); sp -= 4; ram32w(sp, s.r[12]);
	sp -= 4; ram32w(sp, s.r[11]); sp -= 4; ram32w(sp, s.r[10]);
	sp -= 4; ram32w(sp, s.r[9]);  sp -= 4; ram32w(sp, s.r[8]);
	sp -= 4; ram32w(sp, P);                      // P fica no topo da pilha
	r0 = mA; load_xmtrx(X, r0);                  // SZ=1
	r1 = coef; r7 = 6; r6 = ram32(P + 8);
	r2 = r1 + 16; r5 = ram32(P + 4); r3 = r1 + 32;   // r5 = 0x100
	r0 = mB;                                     // mov.l @(36,r4),r0
	r4 = r1 + 48;
	T = r6 >> 31; r6 <<= 1;                      // shll r6; bf → pulado

	// ===== P1: A em linha nos coeficientes ============================================
	// Corpo (0x04E): 8 vetores pares em b, b+16 … b+112. Grava [y2 y3 y0 y1].
	// Registradores no fim do corpo: fv0/fv4/fv8/fv12 = A·v4/v5/v6/v7 (sem troca).
	auto pass_par = [&](u32 b) {
		float32x4_t y[8];
		for (int k = 0; k < 8; k++) { y[k] = ftrv(X, ld4(b + 16 * k)); st4(b + 16 * k, swap64(y[k])); }
		f0 = y[4]; f4 = y[5]; f8 = y[6]; f12 = y[7];
	};
	for (;;)
	{
		if (T) { ENTER(0x04E, C.c04E); pass_par(r1); }
		else     ENTER(0x0AE, C.c0AE);
		r1 += r5; r2 += r5; r3 += r5; r4 += r5;  // dt r7; add r5,rX; bf.s
		T = (--r7 == 0);
		if (T) break;
		ENTER(0x04A, C.c04A);
		T = r6 >> 31; r6 <<= 1;
	}

	// ===== P2: B nos ímpares + borboleta → temporário (float) =========================
	ENTER(0x0BA, C.c0BA);
	r4 = ram32(sp);                              // P
	load_xmtrx(X, r0);                           // B (r0 = P+36 lido na entrada)
	r5 = 0x80;                                   // mov #0x40; shll r5
	r6 = ram32(r4 + 8); r7 = 6; r3 = ram32(r4 + 12);
	r1 = r5 + r3; r0 = ram32(r4 + 40);
	T = r6 >> 31; r6 <<= 1;

	// Uma iteração (coluna): fv8 = B·ímpar; fv0 = par (destrocado); fv4 = fv0;
	// fv0 += fv8; fv4 −= fv8; 8 gravações float (SZ=0); dt r2; lê próximo ímpar; r0 += 4.
	auto p2_iter = [&]() {
		f8 = ftrv(X, f8);
		f0 = swap64(ld4(r3)); r3 += 16;          // DR2 ← [r3], DR0 ← [r3+8]
		f4 = f0;
		f0 = vaddq_f32(f0, f8);
		f4 = vsubq_f32(f4, f8);
		ram32w(r0 + 0x00, f2u(f0[0])); ram32w(r0 + 0x20, f2u(f0[1]));
		ram32w(r0 + 0x40, f2u(f0[2])); ram32w(r0 + 0x60, f2u(f0[3]));
		ram32w(r0 + 0x70, f2u(f4[0])); ram32w(r0 + 0x50, f2u(f4[1]));
		ram32w(r0 + 0x30, f2u(f4[2])); ram32w(r0 + 0x10, f2u(f4[3]));
		T = (--r2 == 0);
		f8 = ld4(r1); r1 += 16;                  // SZ=1 de novo; lido DEPOIS das gravações
		r0 += 4;                                 // slot do bf.s
	};
	for (;;)
	{
		if (T)
		{
			ENTER(0x0E0, C.c0E0); r2 = 4; f8 = ld4(r1); r1 += 16; p2_iter();
			while (!T) { ENTER(0x0E6, C.c0E6); p2_iter(); }
			ENTER(0x11E, C.c11E); r0 += 112; r2 = 4; p2_iter();
			while (!T) { ENTER(0x122, C.c122); p2_iter(); }
			ENTER(0x15A, C.c15A);
			r1 -= 16; r1 -= r5; r0 -= 16; r0 -= r5; r3 -= r5;
		}
		else
			ENTER(0x164, C.c164);
		// 0x548: shll r5; dt r7; add r5 ×3; bf.s; shlr r5 (o slot muda T depois do teste)
		r5 <<= 1;
		const bool fim = (--r7 == 0);
		r0 += r5; r3 += r5; r1 += r5;
		T = r5 & 1; r5 >>= 1;
		if (fim) break;
		ENTER(0x0DC, C.c0DC);
		T = r6 >> 31; r6 <<= 1;
	}

	// ===== P3: A em linha no temporário ===============================================
	ENTER(0x172, C.c172);
	r0 = ram32(r4 + 32); load_xmtrx(X, r0);      // A de novo
	r1 = ram32(r4 + 40); r7 = 6; r6 = ram32(r4 + 8);
	r2 = r1 + 16; r5 = ram32(r4 + 4); r3 = r1 + 32;
	r0 = ram32(r4 + 36); r4 = r1 + 48;
	T = r6 >> 31; r6 <<= 1;
	for (;;)
	{
		if (T) { ENTER(0x19E, C.c19E); pass_par(r1); }
		else     ENTER(0x1FE, C.c1FE);
		r1 += r5; r2 += r5; r3 += r5; r4 += r5;
		T = (--r7 == 0);
		if (T) break;
		ENTER(0x19A, C.c19A);
		T = r6 >> 31; r6 <<= 1;
	}

	// ===== P4: B + borboleta + arredondamento → short na saída =======================
	ENTER(0x20A, C.c20A);
	r7 = ram32(sp);                              // P
	load_xmtrx(X, r0);                           // B
	r3 = 6; ram32w(r7 + 44, r3);
	r5 = 0x80; r3 = ram32(r7 + 40);
	r1 = r3 + r5; r6 = ram32(r7 + 8);
	r5 = 0x100;                                  // shll r5 (0x610, só uma vez)

	// Uma iteração (coluna i): como p2_iter, mais fr12 = bias float, ftrc, − bias int.
	auto p4_iter = [&]() {
		f8 = ftrv(X, f8);
		f0 = swap64(ld4(r3)); r3 += 16;
		f4 = f0;
		f0 = vaddq_f32(f0, f8);
		f4 = vsubq_f32(f4, f8);
		f12 = vcombine_f32(vld1_f32(ramf_ptr(r7)), vget_high_f32(f12));   // DR12 ← [P]
		f8 = ld4(r1); r1 += 16;                  // próximo ímpar ANTES das gravações
		const float32x4_t bias = vdupq_laneq_f32(f12, 0);                  // 512.5f
		f0 = vaddq_f32(f0, bias);
		f4 = vaddq_f32(f4, bias);
		ram32w(r7 + 48, r3);
		r3 = ram32(r7 + 24);                     // 512
		const int32x4_t a = vcvtq_s32_f32(f0), b = vcvtq_s32_f32(f4);   // ftrc (fcvtzs)
		const int32x4_t ib = vdupq_n_s32((s32)r3);
		const int32x4_t sa = vsubq_s32(a, ib), sb = vsubq_s32(b, ib);
		ram16w(r0 + 0x00, sa[0]); ram16w(r0 + 0x20, sa[1]);
		ram16w(r0 + 0x40, sa[2]); ram16w(r0 + 0x60, sa[3]);
		ram16w(r0 + 0x70, sb[0]); ram16w(r0 + 0x50, sb[1]);
		ram16w(r0 + 0x30, sb[2]); ram16w(r0 + 0x10, sb[3]);
		r4 = (u32)sb[2]; r5 = (u32)sb[3]; fpul = (u32)b[3];   // últimos da sequência ftrc/sub
		T = (--r2 == 0);
		r0 += 2;
		r3 = ram32(r7 + 48);                     // slot do bf.s
	};
	for (bool first = true;; first = false)
	{
		if (!first) ENTER(0x22E, C.c22E);        // no 1º bloco este código é o fim do setup
		r2 = ram32(r7 + 20);
		T = r6 >> 31; r6 <<= 1;
		r0 = ram32(r2); r2 += 4;
		ram32w(r7 + 20, r2);                     // slot: avança a lista mesmo se pulado
		if (T)
		{
			if (!ram_ok(r0, 128, 2)) { flush(); return bail(base + 0x238); }   // saída fora da RAM
			ENTER(0x238, C.c238); r2 = 8; f8 = ld4(r1); r1 += 16; p4_iter();
			while (!T) { ENTER(0x23E, C.c23E); p4_iter(); }
			ENTER(0x2BA, C.c2BA);
			r1 -= 16; r5 = ram32(r7 + 4); r3 -= 128; r1 -= 128;
		}
		else
			ENTER(0x2C2, C.c2C2);
		// 0x6A6
		r4 = ram32(r7 + 44); r3 += r5;
		T = (--r4 == 0);
		ram32w(r7 + 44, r4);
		r1 += r5;
		if (T) break;
	}

	// ===== 0x2CE epílogo ================================================================
	ENTER(0x2CE, C.c2CE);
	sp += 4;                                     // descarta P
	for (int k = 8; k <= 14; k++) { s.r[k] = ram32(sp); sp += 4; }   // r14 no slot do rts
	s.r[0] = r0; s.r[1] = r1; s.r[2] = r2; s.r[3] = r3; s.r[4] = r4; s.r[5] = r5;
	s.r[6] = r6; s.r[7] = r7; s.r[15] = sp;
	vst1q_f32(&s.fr[0], f0); vst1q_f32(&s.fr[4], f4);
	vst1q_f32(&s.fr[8], f8); vst1q_f32(&s.fr[12], f12);
	for (int k = 0; k < 4; k++) vst1q_f32(&s.xf[4 * k], X[k]);         // XMTRX = B
	s.fpul = fpul; s.sr.T = T;                   // T = 1 (dt do contador chegou a 0)
	s.fpscr.SZ = 0;                              // fschg
	return_to(s.pr);                             // rts
#undef ENTER
}

} // namespace idct_mb

// ===================================================================================
// O QUE NÃO DEU PARA DETERMINAR (só código nos dumps, sem os dados)
// ===================================================================================
// - Os VALORES das matrizes A e B (ficam em tabelas do executável, que o dump não tem;
//   no Napple copiadas de 8C1A86BC/8C1A86C0 por 8C1A85D2). Por isso não dá para dizer
//   a ordem natural das linhas/colunas da saída nem a permutação dos coeficientes que o
//   VLC/dequant entrega (a forma "vetor k = par, 8+k = ímpar" e a borboleta saem do
//   código; qual frequência cai em qual elemento depende das matrizes). Não importa
//   para o nativo (que é bit a bit), só para a descrição. Dá para ler pelo socket
//   (`mem`) num savestate com o objeto montado.
// - Quem chama: a chamada é indireta (jsr pelo ponteiro em obj+12); o bloco do `jsr`
//   não foi identificado nos dumps. A init do objeto foi achada nos 7 (escreve
//   512.5f/0x100/0x200/0x80 e este endereço em obj+12).
// - Versão da biblioteca (string "MPV x.yy"): não lida. Diferente da do RE CV (1.14).
// - Endereços de saída: o código aceita qualquer endereço; o pseudo só cobre RAM
//   principal (bail no bloco 0x238 se não for). Não verificado para onde apontam.
// - Passo P+4 ≠ 0x100 nunca observado (init grava 0x100 nos 7): o pseudo devolve ao JIT.
// - Os 6 jogos além do Napple têm 0.00% nos dumps da tabela (sem FMV na captura). O
//   ganho por jogo precisa de uma captura em cima de um vídeo (Evolution 1 já tem:
//   fmv_plan.md, 2026-10-09).
// - Validação: ainda não feita (FC_STATE_HASH + FC_RTC_FIXED + FC_INPUT_NEUTRAL com
//   tier2 desligado), é pseudo-código.
