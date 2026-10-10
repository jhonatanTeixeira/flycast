// Strips de vértices com luz difusa (modo r8 == 0 da rotina de T&L da biblioteca dos
// jogos de luta/AM2) — pseudo-C++ para todos os jogos do grupo 005
// (docs/sdk_find/auto/005_matriz_produto_escalar_divisao.md). API: ver
// docs/sdk_blocks/luz_e_transformacao_de_vertices_pseudo.cpp. Referência bit-exata do
// laço irmão (modo r8 par ≠ 0, clamp por tabela): core/rec-ARM64/hle_fn.cpp (doa2_run) e
// docs/sdk_blocks/vertices_com_clamp_pseudo.cpp.
// Além daquela API: write32/writef (RAM), exit_to(pc) (estado JÁ gravado em s, o
// despachante segue de pc — ao contrário de bail(pc), que recusa sem tocar no estado),
// bytes_match, is_ram, sh4_clock_factor() (o `clk` do doa2_run).
//
// O QUE FAZ
//   Uma única entrada (`base`) despacha por r8 em três modos:
//     r8 == 0          → +0x132: este arquivo (luz difusa + ambiente, cor por intensidade)
//     r8 ímpar         → +0x010: sem luz (intensidade fixa 1.0) — grupo 004 (DOA2 8C101930…)
//     r8 par, ≠ 0      → +0x286 (+0x276 no Power Stone): clamp de cor por tabela em r8 (r8 é
//                        o ponteiro da tabela, por isso par) —
//                        vertices_com_clamp (DOA2 8C101BC4 = base+0x2A4, já nativo)
//   (+000: `tst #1,r0` liga T com o bit 0 zerado; `bf +0x010` salta com r8 ímpar.)
//   ⇒ o bloco de entrada +000 é o MESMO do grupo 004: o plug nesse endereço tem que
//     despachar por r8 (r8 == 0 aqui; resto: o nativo do 004 / clamp, ou recusa).
//
//   Roda com FPSCR.SZ=1 (o `fschg` do prólogo liga; o do fim desliga): todo `fmov.s`
//   com registrador par move um PAR de floats (8 bytes, fr[n] = endereço baixo).
//
// Entrada
//   r4   stream de controle (RAM):  +0 contagem de vértices da 1ª strip (u32), depois
//        uma entrada por vértice, em um de dois formatos (bit 0 da 1ª palavra):
//          formato 1 (bit0 = 1): o registro de 32 bytes EMBUTIDO no stream (a 1ª palavra
//                                é o próprio x; o bit baixo da mantissa serve de flag)
//          formato 0 (bit0 = 0): 8 bytes {flags, rel}; registro em (entrada + 8) + rel
//        Fim de strip: as duas palavras seguintes são {cabeçalho, contagem}; se
//        (s32)cabeçalho > 0 e bit 7 → nova strip com `contagem` vértices; senão fim.
//   Registro (32 bytes): x, y, z, nx, ny, nz, u, v
//   r6   destino na Store Queue (0xE…); cada vértice ocupa [r6, r6+32) e r6 anda 64
//        (sempre a mesma SQ: o `pref` de cada vértice manda os 32 bytes ao TA)
//   XMTRX matriz (linha 0 = w: fr4 do `ftrv` é o divisor)
//   fv12 = (fr12, fr13, fr14, fr15) = luz direcional já permutada (Ly, Lz, Lx) e
//        fr15 = intensidade ambiente (o 4º termo do `fipr` multiplica fr3)
//   r5   anda +32 por vértice (não é desreferenciado aqui)
// Saída por vértice (TA, vértice tipo 7: textura + intensidade), 32 bytes:
//   [PCW, X/w, Y/w, 1/w, u, v, I, 0.0]
//   PCW = endereço do vértice anterior na SQ (0xE… = parâmetro de vértice, sem fim de
//         strip); no último vértice da strip, r6 >> 1 aritmético (0xF…: liga o bit 28,
//         end-of-strip) — no Power Stone, 0xFFFFFFFF.
//   I   = ambiente + max(N·L, 0)   [+ extra da 2ª luz na variante com callback]
// Saída nos registros: a do JIT (ver `tail`): r0 = última palavra lida (0x44 na variante
//   com callback), r3 = 0 (ou a contagem lida se o cabeçalho final for > 0 sem bit 7),
//   T = (difusa do último vértice > 0), FPUL = fr15, SZ = 0; `rts`.
//
// MEMBROS (5 jogos, 6 cópias)                                     perf (% da thread de emu)
//   Jogo                    entrada    layout     instr. no dump   (só 7 jogos têm perf)
//   Capcom vs. SNK 2        8C17C3C0   DOA2       166              —
//   Dead or Alive 2         8C101920   DOA2       173              2,57%
//   Marvel vs. Capcom 2     8C12A7E0   DOA2       170              0,09%
//   Power Stone             0C0E7600   PSTONE     171              —
//   Shenmue II              8C1D8A80   DOA2       173              6,02%  ← o alvo
//   Shenmue II              8C1D9B80   CALLBACK   204              0,85% (+~0,6% no callee
//                                                                         8C1D9B04)
//   Shenmue II 8C1D8A80 é o jogo mais lento do projeto (~73%); pelo jit_lite_report os
//   blocos quentes são todos deste caminho: +0x184/+0x186 (corpo do 1º meio, 1,29% +
//   0,93%), +0x15A (cabeça, 0,70%), +0x1D8/+0x1DA (corpo do 2º meio, 0,68% + 0,62%),
//   +0x172/+0x1C6 (entradas formato 1, 0,26% + 0,27%). Formato 1 (registro embutido)
//   domina no Shenmue II.
//
// VARIANTES (comparadas nos dumps, não só nas listagens do md)
//   • Idênticas byte a byte (mesmos opcodes E mesmos deslocamentos de desvio, mesmo
//     layout): DOA2 8C101920, MvC2 8C12A7E0, CvS2 8C17C3C0, Shenmue II 8C1D8A80. O que
//     difere nas listagens é só COBERTURA: MvC2 não executou +0x144..+0x148 (1º vértice
//     no formato 0); CvS2 também não executou +0x258..+0x25E (fim de lista logo após o
//     vértice do 1º meio) nem o desvio do modo r8 par (+0x00C, `bra` para o clamp). Shenmue II 8C1D8A80
//     executou o caminho inteiro (e os modos r8 par/ímpar).
//   • Power Stone 0C0E7600 — difere de verdade em UM ponto: o PCW de fim de strip é
//     `mov #0xFF,r2` (= 0xFFFFFFFF) em vez de `mov r6,r2 ; shar r2`, nos dois blocos de
//     saída (equivalentes a +0x204 e +0x234). Efeitos: o valor gravado no PCW e o r2
//     final mudam; esses dois blocos custam 3 ciclos em vez de 4. O modo r8 ímpar (sem luz), que vem
//     antes na memória, tem a mesma troca, então este caminho começa em +0x128 (não
//     +0x132); há 2 bytes nunca executados em +0x1FA (provável `nop` de alinhamento).
//     Fora isso, opcode a opcode igual.
//   • Shenmue II 8C1D9B80 (CALLBACK) — difere de verdade: por vértice chama a função em
//     @(0x40,r15) (`sts.l PR,@-r15 ; jsr @r1|@r14 ; fschg` … `lds.l @r15+,PR ; fschg`,
//     SZ=0 durante a chamada) e soma à intensidade o float em @(0x44,r15) (que o callee
//     grava). O 2º `dt r3` do laço muda de lugar (depois da 2ª chamada; mesmo efeito), r0
//     termina 0x44 e fr0 termina com o extra. O callee visto (o ÚNICO que rodou na
//     sessão) é 8C1D9B04: 2ª luz direcional, d2 = fipr((L2y,L2z,L2x,fr15),(ny,nz,nx,0)),
//     grava max(d2,0) em r0 = r15_chamador+0x44 e DEVOLVE fr3 = d2 (cru). Consequência
//     (literal, provável bug do jogo): o `fipr` da luz principal logo depois usa fv0 com
//     fr3 = d2, então difusa = (Ly·ny + Lz·nz) + (Lx·nx + ambiente·d2).
//     O laço com clamp dessa cópia está em +0x2E2 (8C1D9E62 em vertices_com_clamp).
//   • Parentes fora do grupo (mesmo despacho por r8, mas listas de TRIÂNGULOS: r9 =
//     número de triângulos, r3 = 3 a cada um): DOA2 8C102320, MvC2 8C12B1E0, Shenmue II
//     8C1DA5E0 (esta com o mesmo callback). Não cobertos aqui.
//
// CICLOS: custo de cada bloco do JIT = 3º número da linha `B` do dump (mesmo valor nos
// 4 jogos de layout DOA2; tabelas abaixo). O JIT desconta o bloco na ENTRADA e só chama
// o nativo depois (rec_arm64.cpp): o bloco em que o nativo é chamado já está pago. Aqui
// a fatia é checada uma vez por meia-volta (um vértice) contra o PIOR caminho dela; sem
// folga, o estado vai para o Sh4 e o JIT segue da cabeça — o UpdateSystem cai então no
// mesmo bloco em que cairia sem o nativo (nenhum bloco é pago "pela metade").

#pragma GCC optimize("fp-contract=off")   // só as fusões que o JIT faz (as fmla do ftrv)
#include <arm_neon.h>

namespace stripluz {

// --- custos dos blocos do JIT (ciclos-guest, antes da escala de clock) ---------------
// Nomes pelos endereços do DOA2 (offset = endereço − 8C101920).
struct Costs {
	u8 entry2;          // +006  bra +132 (slot: tst #1,r0)
	u8 pro;             // +132  contagem, fschg, 1ª entrada
	u8 pro0, pro1;      // +144 / +14A  1º vértice (formato 0/1) + topo do laço (+ 1ª chamada no CB)
	u8 headA;           // +15A  cabeça do 1º meio (A7A)  [CB: o bloco do jsr]
	u8 postA;           // CB: +166, volta da chamada até o bt.s (0 nos outros: está no headA)
	u8 a_tst;           // +16A  tst #1,r0 / bt.s
	u8 a_f1, a_f0;      // +172 / +178  formato 1 / formato 0 (A98)
	u8 a_body;          // +184 = +186  corpo do 1º meio (com ou sem o fadd da difusa)
	u8 a_post;          // CB: +1D4, volta da 2ª chamada
	u8 headB;           // +1BE  cabeça do 2º meio (ADE)
	u8 b_f1, b_f0;      // +1C6 / +1CC
	u8 b_body;          // +1D8 = +1DA
	u8 b24, b2e, b34, b3e, b48, b80;   // fim de strip no 2º meio
	u8 b54, b5e, b64, b6e, b78;        // fim de strip no 1º meio
	u8 tail;            // +268 = +26A  último vértice + rts
	u8 callee;          // CB: 8C1D9B04 (4) + 8C1D9B1E|8C1D9B20 (2)
};

struct Variant {
	bool cb;            // chama @(0x40,r15) por vértice e soma @(0x44,r15) na intensidade
	bool eosAll1;       // PCW de fim de strip = 0xFFFFFFFF (Power Stone) em vez de r6>>1
	u16 headA, headB;   // offsets das duas cabeças: reentrada (plug) e devolução ao JIT
	Costs c;
};

//                                       e2 pro p0 p1 hA pA  at f1 f0 ab ap  hB f1 f0 bb  b24 2e 34 3e 48 80  b54 5e 64 6e 78  tl ce
constexpr Variant DOA2     { false, false, 0x15A, 0x1BE,
	{ 2, 7, 8, 5,  4, 0,  3, 5, 2, 11, 0,  3, 5, 2, 8,  4, 3, 5, 4, 4, 1,  4, 3, 5, 4, 5,  6, 0 } };
constexpr Variant PSTONE   { false, true,  0x150, 0x1B4,
	{ 2, 7, 8, 5,  4, 0,  3, 5, 2, 11, 0,  3, 5, 2, 8,  3, 3, 5, 4, 4, 1,  3, 3, 5, 4, 5,  6, 0 } };
constexpr Variant CALLBACK { true,  false, 0x15A, 0x1E8,
	{ 2, 7, 9, 6,  5, 5,  3, 5, 2, 13, 5,  3, 5, 2, 9,  4, 3, 5, 4, 4, 1,  4, 3, 5, 4, 5,  7, 6 } };

struct Member { u32 base; const Variant *v; };
constexpr Member MEMBERS[] = {
	{ 0x8C17C3C0, &DOA2 },      // Capcom vs. SNK 2
	{ 0x8C101920, &DOA2 },      // Dead or Alive 2
	{ 0x8C12A7E0, &DOA2 },      // Marvel vs. Capcom 2
	{ 0x0C0E7600, &PSTONE },    // Power Stone
	{ 0x8C1D8A80, &DOA2 },      // Shenmue II (quente, 6%)
	{ 0x8C1D9B80, &CALLBACK },  // Shenmue II (2ª cópia, com callback)
};
// Assinaturas (para casar por bytes em vez de endereço fixo):
//   cabeça A, DOA2:     6046 4310 FF1D F79D F743 6146 8D65 F3ED   (PSTONE: …8D64 F3ED)
//   cabeça A, CALLBACK: E040 30FC 6106 4F22 410B F3FD
//   callee conhecido (8C1D9B04, 20 opcodes):
//     FFCB FFDB FFEB 2F06 D00C 6002 FE09 FC09 FD09 F3ED FC8D F3C5 8B00
//     FC3C 60F6 F0CA FEF9 FDF9 000B FCF9    literal em +0x3C (= 0x8C260F2C no Shenmue II)

// --- operações como o JIT -------------------------------------------------------------
// ftrv: fmul da coluna 0 + três fmla fundidas
static inline float32x4_t ftrv(const float32x4_t m[4], float a, float b, float c, float d)
{
	float32x4_t acc = vmulq_n_f32(m[0], a);
	acc = vfmaq_n_f32(acc, m[1], b);
	acc = vfmaq_n_f32(acc, m[2], c);
	return vfmaq_n_f32(acc, m[3], d);
}
// fipr: produtos sem fusão, soma (p0+p1)+(p2+p3)
static inline float fipr(float32x4_t a, float32x4_t b)
{
	const float32x4_t p = vmulq_f32(a, b);
	const float32x4_t s = vpaddq_f32(p, p);
	return vpadds_f32(vget_low_f32(s));
}

// Custos escalados pelo clock do SH4 BLOCO A BLOCO (como cyc() no doa2_run: truncar cada
// bloco, nunca a soma); 0 continua 0 (campo sem bloco nesta variante).
struct Scaled {
	s32 entry2, pro, pro0, pro1, headA, postA, a_tst, a_f1, a_f0, a_body, a_post,
	    headB, b_f1, b_f0, b_body, b24, b2e, b34, b3e, b48, b80, b54, b5e, b64, b6e, b78,
	    tail, callee;
	s32 unitA, unitB, entry;     // piores caminhos (abaixo)
};
static Scaled scale(const Variant &V, float clk);   // campo a campo + os 3 máximos abaixo
// Piores caminhos (valores sem escala, layout DOA2 / CALLBACK):
//   cabA   = headA (+ callee + postA no CB)                                     4 / 16
//   corpoA = max(a_tst + a_f1 + a_body, b54 + b5e + b64 + b6e + a_f0 + a_body)
//            (+ callee + a_post no CB)                                         29 / 42
//   desvB  = b24 + b2e + b34 + b3e + b_f0 + b_body (fim de strip no 2º meio
//            dentro da mesma volta; os caminhos de fim de lista são menores)    26 / 27
//   unitA  = cabA + corpoA + desvB                                             59 / 85
//   unitB  = headB + max(b_f1, b_f0) + b_body                                  16 / 17
//   entry  = entry2 + pro + max(pro0, pro1) (+ callee + postA) + unitA − cabA   72 / 98

// --- a 2ª luz do Shenmue II (callee 8C1D9B04), efeito exato da chamada ------------------
struct Luz2 {
	u32 fn;             // @(0x40,r15)
	u32 R;              // r15 do chamador
	float32x4_t l;      // (P[1], P[2], P[0], fr15): fr12, fr13, fr14 do callee + fr15
	float out;          // último max(d2, 0) gravado em R+0x44
};
static bool luz2_known(const Sh4 &s, Luz2 &k)
{
	k.R = s.r[15];
	k.fn = ram32(k.R + 0x40);
	if (!bytes_match(k.fn, LUZ2_SIG, 20))  // outro callback: não coberto (nunca visto)
		return false;
	const u32 P = ram32(ram32(k.fn + 0x3C));     // literal → variável global → vetor
	k.l = (float32x4_t){ ramf(P + 4), ramf(P + 8), ramf(P), s.fr[15] };
	k.out = ramf(k.R + 0x44);                    // reentrada no 2º meio lê o extra daqui
	return true;
}
// sts.l PR,@-r15 ; jsr ; (callee empilha fr12, fr13, fr14, r0; calcula; desempilha) ;
// lds.l @r15+,PR. Visível depois: fr3 = d2, a pilha abaixo de R e o slot R+0x44.
// r0 (= R+0x44), r1|r14 (= fn) e T (fcmp do callee) são sobrescritos pelo chamador antes
// de qualquer leitura/saída. As 5 palavras da pilha são as mesmas em toda chamada: basta
// gravá-las uma vez por execução.
static inline void luz2_call(const Sh4 &s, Luz2 &k, float f0, float f1, float f2, float &f3)
{
	write32(k.R - 4, s.pr);
	write32(k.R - 8, f2u(s.fr[12])); write32(k.R - 12, f2u(s.fr[13]));
	write32(k.R - 16, f2u(s.fr[14])); write32(k.R - 20, k.R + 0x44);
	f3 = fipr(k.l, (float32x4_t){ f0, f1, f2, f3 });   // fr3 = 0 em toda chamada
	k.out = f3 > 0.f ? f3 : 0.f;                       // fmov fr3,fr12 | fldi0 fr12
	writef(k.R + 0x44, k.out);
}

// --- a função -------------------------------------------------------------------------
// entryOff: 0 (entrada da função), V.headA ou V.headB (o bloco dessa cabeça já foi pago).
template <const Variant &V>
static void run_t(Sh4 &s, Cycles &cyc, u32 base, u32 entryOff)
{
	const Scaled C = scale(V, sh4_clock_factor());
	const u32 here = base + entryOff;

	// Pré-condições. Recusar (bail no próprio bloco) é sempre exato: o JIT segue.
	if (entryOff == 0 && s.r[8] != 0)
		return bail(here);                      // modos r8 ≠ 0: grupo 004 / clamp
	if (s.fpscr.PR || s.fpscr.SZ != (entryOff == 0 ? 0 : 1))
		return bail(here);
	if (!is_ram(s.r[4]) || (s.r[6] >> 26) != 0x38)
		return bail(here);                      // stream fora da RAM / destino fora da SQ
	Luz2 k{};
	if (V.cb && !luz2_known(s, k))
		return bail(here);
	const s32 need = entryOff == 0 ? C.entry
	               : entryOff == V.headA ? C.unitA - C.headA : C.unitB - C.headB;
	if (cyc.left() < need)
		return bail(here);

	// Espelho dos registros do SH4 em locais (o compilador os mantém em registrador).
	u32 r0 = s.r[0], r1 = s.r[1], r2 = s.r[2], r3 = s.r[3], r4 = s.r[4], r5 = s.r[5],
	    r6 = s.r[6], r14 = s.r[14], fpul = s.fpul;
	bool T = s.sr.T;
	float f0 = s.fr[0], f1 = s.fr[1], f2 = s.fr[2], f3 = s.fr[3], f4 = s.fr[4], f5 = s.fr[5],
	      f6 = s.fr[6], f7 = s.fr[7], f8 = s.fr[8], f9 = s.fr[9], f10 = s.fr[10], f11 = s.fr[11];
	const float32x4_t m[4] = { vld1q_f32(&s.xf[0]), vld1q_f32(&s.xf[4]),
	                           vld1q_f32(&s.xf[8]), vld1q_f32(&s.xf[12]) };
	const float32x4_t L = vld1q_f32(&s.fr[12]);       // fv12 (fr15 = ambiente)
	const u32 amb = f2u(s.fr[15]);                    // flds fr15,FPUL

	auto pair = [](u32 a, float &lo, float &hi) { lo = ramf(a); hi = ramf(a + 4); };
	auto sqp = [&](float lo, float hi) {              // fmov.s drN,@-r6 (par)
		r6 -= 8; sq_write(r6, f2u(lo)); sq_write(r6 + 4, f2u(hi));
	};
	auto tr = [&](float &a, float &b, float &c, float &d) {
		const float32x4_t p = ftrv(m, a, b, c, d);
		a = vgetq_lane_f32(p, 0); b = vgetq_lane_f32(p, 1);
		c = vgetq_lane_f32(p, 2); d = vgetq_lane_f32(p, 3);
	};
	auto fipr0 = [&] { f3 = fipr((float32x4_t){ f0, f1, f2, f3 }, L); };   // fipr fv12,fv0
	auto eos = [&]() -> u32 { return V.eosAll1 ? 0xFFFFFFFFu : (u32)((s32)r6 >> 1); };
	auto flush = [&](bool sz) {
		s.r[0] = r0; s.r[1] = r1; s.r[2] = r2; s.r[3] = r3; s.r[4] = r4; s.r[5] = r5;
		s.r[6] = r6; s.r[14] = r14; s.fpul = fpul; s.sr.T = T; s.fpscr.SZ = sz;
		s.fr[0] = f0; s.fr[1] = f1; s.fr[2] = f2; s.fr[3] = f3; s.fr[4] = f4; s.fr[5] = f5;
		s.fr[6] = f6; s.fr[7] = f7; s.fr[8] = f8; s.fr[9] = f9; s.fr[10] = f10; s.fr[11] = f11;
	};

	if (entryOff == V.headA) goto headA_body;
	if (entryOff == V.headB) goto headB_body;

	// +000 (pago pelo JIT): tst r8,r8 ; mov r8,r0 ; bf (não salta: r8 == 0)
	r0 = 0;
	cyc.take(C.entry2);                     // +006: bra +132 ; slot tst #1,r0
	T = true;
	cyc.take(C.pro);                        // +132
	r3 = ram32(r4); r4 += 4;                // contagem da 1ª strip
	r6 += 32;                               // fschg: SZ 0 → 1
	r0 = ram32(r4); r14 = r4; f2 = 0.f;
	T = !(r0 & 1); r4 += 32;
	if (T) { cyc.take(C.pro0); r14 = ram32(r14 + 4); r4 -= 24; r14 += r4; }   // formato 0
	else     cyc.take(C.pro1);                                                 // formato 1
	// +14A: 1º vértice (o bloco segue pela cabeça A sem pagá-la)
	pair(r14, f4, f5); r14 += 8;
	r2 = r6;
	pair(r14, f6, f7); r14 += 8;
	f2 = f2 + f7;                           // 0 + nx (não simplificar: −0 vira +0)
	f7 = 1.f; f3 = 0.f;
	pair(r14, f0, f1); r14 += 8;
	tr(f4, f5, f6, f7);
	goto headA_body;

// ---- 1º meio: termina o vértice em fv4, prepara o seguinte em fv8 --------------------
headA:
	if (cyc.left() < C.unitA) { flush(true); return exit_to(base + V.headA); }
	cyc.take(C.headA);
headA_body:
	if (V.cb) { luz2_call(s, k, f0, f1, f2, f3); cyc.take(C.callee + C.postA); }
	r0 = ram32(r4); r4 += 4;
	T = --r3 == 0;                          // dt r3
	fpul = amb;
	f7 = 1.f; f7 = f7 / f4;                 // 1/w
	r1 = ram32(r4); r4 += 4;
	fipr0();                                // slot do bt.s: difusa (+ ambiente·fr3)
	if (T) goto endA;                       // a strip acabou neste vértice
	cyc.take(C.a_tst);
	T = !(r0 & 1); r1 += r4; f2 = u2f(fpul);
	if (!T) { cyc.take(C.a_f1); r1 = r4; r4 += 24; r1 -= 8; }   // registro embutido
	else      cyc.take(C.a_f0);
bodyA:                                      // A98 (o fim de strip volta para cá)
	f0 = 0.f; T = f3 > f0;
	pair(r14, f0, f1);                      // u, v deste vértice
	r4 += 32;                               // (+32/−32 só para o `pref @r4`: RAM, sem efeito)
	pair(r1, f8, f9); r1 += 8;              // x, y do próximo
	if (T) f2 = f2 + f3;                    // ambiente + difusa
	cyc.take(C.a_body);
	if (V.cb) { r0 = 0x44; f10 = k.out; f2 = f2 + f10; }   // fschg; @(0x44,r15) 4 bytes; fschg
	f3 = 0.f;
	if (!V.cb) T = --r3 == 0;               // dt do próximo (no CB fica depois da 2ª chamada)
	sqp(f2, f3);                            // [24] I, 0.0
	f2 = f2 - f2;                           // literal: NaN se I for Inf/NaN
	r4 -= 32;
	pair(r1, f10, f11); r1 += 8;            // z, nx do próximo
	f6 = f6 * f7;
	sqp(f0, f1);                            // [16] u, v
	f2 = f2 + f11;                          // (I − I) + nx
	f11 = 1.f;
	f5 = f5 * f7;
	pair(r1, f0, f1); r1 += 8;              // ny, nz do próximo; r1 → u,v dele
	r5 += 32;
	sqp(f6, f7);                            // [8]  Y/w, 1/w
	tr(f8, f9, f10, f11);
	sqp(f4, f5);                            // [0]  (w), X/w
	sq_write(r6, r2);                       // [0]  PCW por cima do w
	r2 = r6;
	sq_flush(r6);                           // pref @r6
	r6 += 64;
	if (V.cb) { luz2_call(s, k, f0, f1, f2, f3); cyc.take(C.callee + C.a_post); T = --r3 == 0; }
	r0 = ram32(r4); r4 += 4;
	r14 = ram32(r4); r4 += 4;
	fpul = amb;
	fipr0();
	f11 = 1.f; f11 = f11 / f8;              // slot do bt.s
	if (T) goto endB;
	goto headB;                             // cai no bloco seguinte (sem desvio)

// ---- 2º meio: termina o vértice em fv8, prepara o seguinte em fv4 --------------------
headB:
	if (cyc.left() < C.unitB) { flush(true); return exit_to(base + V.headB); }
	cyc.take(C.headB);
headB_body:
	T = !(r0 & 1); r14 += r4; f2 = u2f(fpul);
	if (!T) { cyc.take(C.b_f1); r14 = r4; r4 += 24; r14 -= 8; }
	else      cyc.take(C.b_f0);
bodyB:                                      // AEC
	f0 = 0.f; T = f3 > f0;
	pair(r1, f0, f1);
	r4 += 32;
	pair(r14, f4, f5); r14 += 8;
	if (T) f2 = f2 + f3;
	cyc.take(C.b_body);
	if (V.cb) { r0 = 0x44; f6 = k.out; f2 = f2 + f6; }
	f3 = 0.f;
	sqp(f2, f3);
	f2 = f2 - f2;
	r4 -= 32;
	pair(r14, f6, f7); r14 += 8;
	f10 = f10 * f11;
	sqp(f0, f1);
	f2 = f2 + f7;
	f7 = 1.f;
	f9 = f9 * f11;
	pair(r14, f0, f1); r14 += 8;
	r5 += 32;
	sqp(f10, f11);
	tr(f4, f5, f6, f7);
	sqp(f8, f9);
	sq_write(r6, r2); r2 = r6; sq_flush(r6);
	r6 += 64;                               // slot do bra para a cabeça A
	goto headA;

// ---- fim de strip --------------------------------------------------------------------
endA:                                       // +234 (B54): último vértice = fv4
	cyc.take(C.b54);
	T = (s32)r0 > 0; f2 = u2f(fpul); r2 = eos();
	if (T) {
		cyc.take(C.b5e);
		T = !(r0 & 0x80); r3 = r1;
		if (!T) {                           // nova strip: r3 = contagem
			cyc.take(C.b64);
			r0 = ram32(r4); r4 += 4; T = !(r0 & 1);
			r1 = ram32(r4); r4 += 4; r1 += r4;
			if (!T) { cyc.take(C.b6e); r1 = r4; r4 += 24; r1 -= 8; }
			cyc.take(C.a_f0);               // bloco A98
			goto bodyA;
		}
	}
	cyc.take(C.b78);                        // B78 (inclui B80..B86)
	r1 = r14;                               // (r4 +48/−48 do pref: líquido 0)
	goto tail;

endB:                                       // +204 (B24): último vértice = fv8
	cyc.take(C.b24);
	T = (s32)r0 > 0; f2 = u2f(fpul); r2 = eos();
	if (T) {
		cyc.take(C.b2e);
		T = !(r0 & 0x80); r3 = r14;
		if (!T) {
			cyc.take(C.b34);
			r0 = ram32(r4); r4 += 4; T = !(r0 & 1);
			r14 = ram32(r4); r4 += 4; r14 += r4;
			if (!T) { cyc.take(C.b3e); r14 = r4; r4 += 24; r14 -= 8; }
			cyc.take(C.b_f0);               // bloco AEC
			goto bodyB;
		}
	}
	cyc.take(C.b48 + C.b80);                // B48 → bra B80
	f4 = f8; f5 = f9;                       // fmov dr8,dr4
	f6 = f10; f7 = f11;                     // slot: fmov dr10,dr6

tail:                                       // B80..BA4: último vértice com PCW de fim
	f0 = 0.f; T = f3 > f0;
	pair(r1, f10, f11);                     // u, v
	if (T) f2 = f2 + f3;
	cyc.take(C.tail);
	if (V.cb) { r0 = 0x44; f0 = k.out; f2 = f2 + f0; }
	f3 = 0.f;
	f6 = f6 * f7;
	sqp(f2, f3);
	f5 = f5 * f7;
	sqp(f10, f11);
	r4 -= 8;
	sqp(f6, f7);
	r5 += 32;
	sqp(f4, f5);
	sq_write(r6, r2);
	sq_flush(r6);                           // (fschg antes: SZ 1 → 0)
	r6 += 32;                               // slot do rts
	flush(false);
	return return_to(s.pr);
}

// Chamado pelo plug: base = entrada da função no jogo, entryOff = bloco casado.
void run(Sh4 &s, Cycles &cyc, u32 base, u32 entryOff)
{
	for (const Member &mb : MEMBERS)
		if (mb.base == base)
		{
			if (mb.v == &CALLBACK) return run_t<CALLBACK>(s, cyc, base, entryOff);
			if (mb.v == &PSTONE)   return run_t<PSTONE>(s, cyc, base, entryOff);
			return run_t<DOA2>(s, cyc, base, entryOff);
		}
	return bail(base + entryOff);
}

// NÃO DETERMINADO / A CONFIRMAR NA VALIDAÇÃO (FC_STATE_HASH, tier2 desligado)
//  1. Callback: só o 8C1D9B04 rodou na sessão; o chamador (8C1D9A24.., `bsr 8C1D9B80`
//     em 8C1D9AAA) pode passar outro ponteiro em @(0x40,r15) — qualquer outro → bail.
//     O vetor da 2ª luz vem de *(*(fn+0x3C)); o literal só aparece dobrado na SHIL
//     (0x8C260F2C), não foi lido da RAM.
//  2. Modos r8 ≠ 0 não cobertos aqui (grupo 004 e vertices_com_clamp); o plug no +000 é
//     compartilhado com o 004.
//  3. CvS2 e Power Stone não têm perf (0,00% = sem amostras, não "frio"); no CvS2 o
//     bloco B78 nunca rodou e no MvC2/CvS2 o 1º vértice no formato 0 também não — o
//     código é o mesmo do DOA2, mas esses caminhos só foram vistos rodando no DOA2/Shenmue II.
//  4. Supõe registros/stream em RAM principal e destino na SQ (mesmo critério do
//     doa2_run); o JIT leria de qualquer região. `pref @r4` tratado como no-op (RAM).
//  5. Denormais/NaN: o nativo roda na mesma thread (mesmo FPCR do JIT); `0 + nx`,
//     `I − I` e `(I − I) + nx` não podem ser simplificados (nada de -ffast-math).
//  6. Custos por bloco tirados do campo de ciclos das linhas `B` (iguais nos 4 jogos de
//     layout DOA2; PSTONE e CALLBACK medidos nos próprios dumps). A escala por clock
//     segue o doa2_run (por bloco); se o JIT mudar o modelo de ciclos, a tabela muda.
//  7. A difusa da variante CALLBACK inclui ambiente·d2 (fr3 sujo pelo callee): é o que o
//     SH4 faz; mantido de propósito.

} // namespace stripluz
