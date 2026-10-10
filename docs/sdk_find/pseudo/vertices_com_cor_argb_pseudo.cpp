// Vértices com cor ARGB empacotada (strips e triângulos) — pseudo-C++ para os 3 jogos do
// grupo 007 (docs/sdk_find/auto/007_matriz_divisao_pref.md). API: ver
// docs/sdk_blocks/luz_e_transformacao_de_vertices_pseudo.cpp. Semântica bit-exata de
// referência (pares com SZ=1, ftrv, fdiv, SQ, ciclos por bloco): core/rec-ARM64/hle_fn.cpp
// (doa2_run) e docs/sdk_blocks/vertices_com_clamp_pseudo.cpp.
//
// Pseudo-código: não compila no core. Ainda não existe versão nativa.
//
// O que faz
//   Transforma cada vértice pela XMTRX, divide por w e emite para o TA, pela Store Queue,
//   um vértice de 64 bytes do tipo "textura + cor em float" (PCW, X Y Z U V, 2 palavras
//   ignoradas, cor base ARGB, cor offset ARGB). A cor vem empacotada no registro (ARGB de
//   8 bits) e vira float multiplicada pelas escalas fr12..fr15. O offset sai sempre 0.
//   No MvC2 (8C1304E0, 6,13% da emulação) deve ser o desenho dos sprites/cenários 2D
//   (palpite pelo jogo e pelo formato; não conferido na tela).
//   Duas funções vizinhas com o mesmo corpo de vértice:
//     STRIPS (8C1304E0 no MvC2): lista de strips, cada um com contagem própria;
//     TRIS   (8C130600 no MvC2): r9 triângulos soltos de 3 vértices.
//
// Entrada
//   r4     fluxo de controle (RAM):
//            STRIPS: [n] e n entradas; depois uma palavra M: M > 0 e (M & 0x80) → outro
//                    strip ([n'] e n' entradas); senão termina (r4 sai apontando M).
//            TRIS:   [t] e t × 3 entradas, sem cabeçalho entre os triângulos.
//          Entrada de vértice, pela 1ª palavra (bit 0):
//            bit 0 = 1 → o registro de 32 bytes está ali mesmo (a palavra é o próprio x;
//                        o bit 0 do x é a marca) e a próxima entrada fica 32 bytes adiante;
//            bit 0 = 0 → entrada de 8 bytes: +4 = deslocamento; registro em entrada+8+desl.
//   Registro (32 bytes): +0 x, +4 y, +8 z, +12 palavra cujo byte alto (+15, com sinal) é
//          uma marca (> 0 → caminho NÃO coberto), +16 cor ARGB (u32), +20 (não lido),
//          +24 u, +28 v.
//   r6     janela na Store Queue (0xE0000000..), 64 bytes por vértice
//   r5     espelho do ponteiro de escrita do TA (só soma 64 por vértice)
//   XMTRX  matriz; o resultado é (w, x, y, ·): a pista 0 é o w
//   fr12..fr15  escalas de A, R, G, B
//   FPSCR  entra com SZ=0, PR=0; a função liga SZ (fschg) e desliga na saída
//
// Saída por vértice, janela W = r6 de entrada + 64·k:
//   W+0  PCW = W+16 (endereço da SQ: bits 31-29 = 7, vértice) ou, no último vértice do
//        strip, (W+24) >> 1 aritmético (o bit 28, fim de strip, liga)
//   W+4  X = x'·(1/w)   W+8 Y = y'·(1/w)   W+12 Z = 1/w   W+16 u   W+20 v
//   W+24..W+31 não são escritos (o TA ignora; ficam os bytes antigos da SQ)
//   `pref W` (1ª rajada), depois
//   W+32 A·fr12  W+36 R·fr13  W+40 G·fr14  W+44 B·fr15  W+48..W+60 = 0   `pref W+32`
// Saída final: r6 = r6 de entrada + 64·N; r5 += 64·N; r4 = palavra final (STRIPS) ou
// a entrada seguinte (TRIS); FPSCR.SZ de volta a 0; rts.
//
//   Jogo                 STRIPS      %perf   TRIS        %perf
//   Capcom vs. SNK 2     8C1891C0    (s/ perf) 8C1892D0  (s/ perf)
//   Dead or Alive 2      8C109CC0    0,73%   8C109DE0    0,02%
//   Marvel vs. Capcom 2  8C1304E0    6,13%   8C130600    0,81%
//
// Não há segunda cópia em nenhum jogo: as duas entradas por jogo são as duas funções
// acima (a TRIS fica 0x120 depois da STRIPS no DOA2/MvC2 e 0x110 no CvS2, só
// alinhamento). Sem literal pool: nenhuma constante muda entre os jogos.
//
// Variantes: **nenhuma**. Comparação opcode a opcode no mesmo offset, nos dumps:
//   STRIPS: 93 de 93 iguais no DOA2 e no CvS2 (ref. MvC2);
//   TRIS:   91 de 91 iguais no DOA2 e no CvS2.
//   A cobertura também é a mesma nos 3 jogos (os mesmos blocos do JIT, nenhum a mais).
//   STRIPS e TRIS têm o mesmo corpo de vértice; na TRIS ele fica 2 bytes adiante (o
//   `mov #3,r3` a mais no prólogo). Só o fim de strip e o prólogo diferem (ver abaixo).
//
// Não determinado (nunca executou em nenhum dos 3 dumps):
//   - o caminho com o byte +15 do registro > 0: 38 instruções em entrada+0x2C..+0x76
//     (MvC2 8C13050C..8C130556) e mais 3 em +0xAA..+0xAE (8C13058A..8C13058E), que só
//     esse caminho alcança (chega no 8C130558 com T=1). Provavelmente outro modo de cor
//     ou um 3º vértice de offset; a versão nativa devolve ao JIT em entrada+0x2C.
//   - o que é a palavra M (> 0 com bit 7) de continuação de strip; aqui só a semântica.
//   - a palavra +20 do registro (não é lida neste caminho).
//
// Ciclos por bloco (campo do dump; iguais nos 3 jogos; offsets a partir da entrada,
// "+d" = +2 na TRIS):
//   entrada STRIPS 7 / TRIS 8 (já descontado pelo JIT ao chamar, como no doa2_run)
//   +0x06+d cabeçalho de strip 5 | +0x10+d entrada de índice 10 | +0x16+d vértice 7
//   +0x78+d projeção/cor 15 | +0xB0+d cauda 3 | +0xBA+d meio 12 | +0xBC+d último 11
//   +0xE6+d próxima entrada 4 | +0xEE+d entrada embutida 4
//   STRIPS: +0xF6 fim de strip 4 | +0xFE outro strip 2 | +0x102 saída 4 | +0x104 saída 3
//   TRIS:   +0xF8 fim de triângulo 5 | +0x102 saída 2
//   Vértice típico: 7+15+3+12+4 = 41 ciclos (+4 se a entrada for embutida).

#pragma GCC optimize("fp-contract=off")   // só a fusão que o JIT faz (ftrv)
#include <arm_neon.h>

namespace vtxargb {

enum Kind { STRIPS, TRIS };

// rótulos (offset a partir da entrada; somar d = 2 na TRIS)
constexpr u32 O_STRIP = 0x06, O_IDX = 0x10, O_VTX = 0x16, O_FLAG = 0x2C, O_PROJ = 0x78,
              O_TAIL = 0xB0, O_MID = 0xBA, O_LAST = 0xBC, O_NEXT = 0xE6, O_INL = 0xEE;
constexpr u32 S_END = 0xF6, S_MORE = 0xFE, S_RET4 = 0x102, S_RET3 = 0x104;   // só STRIPS
constexpr u32 T_END = 0xF8, T_RET = 0x102;                                   // só TRIS

// ciclos (já escalados pelo clock do SH4 como o cyc() do doa2_run)
constexpr s32 C_STRIP = 5, C_IDX = 10, C_VTX = 7, C_PROJ = 15, C_TAIL = 3, C_MID = 12,
              C_LAST = 11, C_NEXT = 4, C_INL = 4,
              C_SEND = 4, C_SMORE = 2, C_SRET4 = 4, C_SRET3 = 3, C_TEND = 5, C_TRET = 2;

// ftrv como o JIT: fmul da coluna 0 + três fmla fundidas
static inline void ftrv(const float32x4_t m[4], float &a, float &b, float &c, float &d)
{
	float32x4_t acc = vmulq_n_f32(m[0], a);
	acc = vfmaq_n_f32(acc, m[1], b);
	acc = vfmaq_n_f32(acc, m[2], c);
	acc = vfmaq_n_f32(acc, m[3], d);
	a = vgetq_lane_f32(acc, 0); b = vgetq_lane_f32(acc, 1);
	c = vgetq_lane_f32(acc, 2); d = vgetq_lane_f32(acc, 3);
}

// fmov.s DRm,@-Rn com SZ=1: par de 8 bytes, o registro par no endereço menor
static inline void sq_pair(u32 a, float lo, float hi) { sq_write(a, f2u(lo)); sq_write(a + 4, f2u(hi)); }

// float FPUL,FRn: inteiro com sinal → float (os bytes da cor são 0..255, exato)
static inline float i2f(u32 v) { return (float)(s32)v; }

// Locais: r0..r15, f0..f15, fpul, T e sz copiados de `s` no LOAD e devolvidos no SAVE
// (antes de qualquer saída). ENTER(pc, n) = entrada de bloco igual à do doa2_run:
// desconta n; se a fatia acabou, SAVE + UpdateSystem nesta fronteira e LOAD; interrupção
// pendente (ou CPU parada) → SAVE e o JIT continua em `pc`.
#define ENTER(pc, n)  do { if (!cyc.enter(s, (pc), (n))) return; } while (0)
#define BAIL(pc)      do { SAVE(s); return bail(pc); } while (0)

// `entry` = 1ª instrução da função neste jogo (tabela acima); chamado pelo JIT ao
// compilar o bloco de entrada com os bytes SH4 batendo.
void run(Sh4 &s, Cycles &cyc, u32 entry, Kind kind)
{
	if (s.fpscr.PR || s.fpscr.SZ)                    // entra com SZ=0 (o fschg liga)
		return bail(entry);
	if (!is_ram(s.r[4]) || (s.r[6] >> 26) != 0x38)  // fluxo na RAM, saída na área da SQ
		return bail(entry);

	const u32 d = kind == TRIS ? 2 : 0;
	LOAD(s);
	const float32x4_t m[4] = { vld1q_f32(&s.xf[0]), vld1q_f32(&s.xf[4]),
	                           vld1q_f32(&s.xf[8]), vld1q_f32(&s.xf[12]) };

	// ---- prólogo (bloco de entrada, já descontado pelo JIT)
	if (kind == TRIS) { r9 = ram32(r4); r4 += 4; r6 += 32; r3 = 3; }   // r9 = triângulos
	else              { r3 = ram32(r4); r4 += 4; r6 += 32; }           // r3 = vértices do strip
	sz = 1;                                          // fschg: daqui até a saída, fmov = par
	goto strip_body;                                 // o 1º cabeçalho está no mesmo bloco

strip:	ENTER(entry + O_STRIP + d, C_STRIP);
strip_body:
	r0 = ram32(r4);
	T = !(r0 & 1);
	r14 = r4;
	r4 += 32;                                        // slot do bf.s
	if (!T)
		goto vtx;                                    // registro embutido (r14 = entrada)
	ENTER(entry + O_IDX + d, C_IDX);                 // entrada de 8 bytes
	r14 = ram32(r14 + 4);
	r4 -= 24;                                        // r4 = entrada + 8
	r14 += r4;                                       // registro = entrada + 8 + desloc.
	goto vtx_body;                                   // mesmo bloco do corpo do vértice

vtx:	ENTER(entry + O_VTX + d, C_VTX);
vtx_body:
	f4 = ramf(r14); f5 = ramf(r14 + 4); r14 += 8;   // fmov.s @r14+,fr4 (par): x, y
	f6 = ramf(r14); f7 = ramf(r14 + 4);              // fmov.s @r14,fr6 (par): z, palavra +12
	r1 = r14;
	f7 = 1.f;
	r1 += 16;                                        // → u, v
	r0 = (u32)(s32)(s8)ram8(r14 + 7);                // byte +15 do registro, com sinal
	r8 = ram32(r14 + 8);                             // cor ARGB
	T = (s32)r0 > 0;
	r0 = r8 & 0xFF;                                  // B
	ftrv(m, f4, f5, f6, f7);                         // slot do bf.s: (w, x', y', ·)
	if (T)
		BAIL(entry + O_FLAG + d);                    // marca > 0: caminho nunca executado

	ENTER(entry + O_PROJ + d, C_PROJ);               // 8C130558: cor → float, 1/w
	fpul = r0;
	r8 >>= 8;
	r0 = r8 & 0xFF;                                  // G
	/* pref @(r4+32): endereço na RAM → só prefetch, sem efeito */
	r8 >>= 8;
	f11 = i2f(fpul);                                 // B
	f7 = 1.f;
	f7 = f7 / f4;                                    // fdiv: 1/w
	fpul = r0;
	f10 = i2f(fpul);                                 // G
	r0 = r8 & 0xFF;                                  // R
	fpul = r0;
	r8 >>= 8;                                        // r8 = A
	f9 = i2f(fpul);                                  // R
	fpul = r8;
	r6 -= 8;
	f8 = i2f(fpul);                                  // A
	f0 = ramf(r1); f1 = ramf(r1 + 4);                // u, v (par)
	r0 = r6;                                         // W+24
	f3 = 0.f;
	f11 = f11 * f15;                                 // B·escala
	r6 -= 8; sq_pair(r6, f0, f1);                    // slot do bf.s: W+16 = u, v
	// bf.s 8C130590: neste caminho T é sempre 0 (o T=1 só vem do caminho não coberto)

	ENTER(entry + O_TAIL + d, C_TAIL);
	r0 = (u32)((s32)r0 >> 1);                        // shar: PCW com fim de strip
	f2 = 0.f;
	T = --r3 == 0;                                   // dt r3
	f6 = f6 * f7;                                    // y'·(1/w)
	if (!T) {
		ENTER(entry + O_MID + d, C_MID);
		r0 = r6;                                     // não é o último: PCW = W+16
	} else
		ENTER(entry + O_LAST + d, C_LAST);
	f5 = f5 * f7;                                    // x'·(1/w)
	r6 -= 8; sq_pair(r6, f6, f7);                    // W+8:  Y, Z = 1/w
	r5 += 64;
	r6 -= 8; sq_pair(r6, f4, f5);                    // W+0:  (w), X
	f10 = f10 * f14;
	sq_write(r6, r0);                                // PCW por cima do w
	sq_flush(r6);                                    // pref: 1ª rajada
	r14 = ram32(r4 + 4);
	r6 += 64;
	r0 = ram32(r4); r4 += 4;                         // próxima entrada (ou a palavra M)
	f9 = f9 * f13;
	r6 -= 8; sq_pair(r6, f2, f3);                    // W+56: offset = 0
	f8 = f8 * f12;
	r6 -= 8; sq_pair(r6, f2, f3);                    // W+48
	r14 += r4;
	r6 -= 8; sq_pair(r6, f10, f11);                  // W+40: G, B
	r14 += 4;                                        // r14 = registro da próxima (se índice)
	r6 -= 8; sq_pair(r6, f8, f9);                    // W+32: A, R
	sq_flush(r6);                                    // slot do bt.s: 2ª rajada
	if (T)
		goto strip_end;

	ENTER(entry + O_NEXT + d, C_NEXT);
	T = !(r0 & 1);
	r6 += 64;
	r4 += 4;                                         // slot do bt.s
	if (T)
		goto vtx;                                    // entrada de índice: r14 pronto
	ENTER(entry + O_INL + d, C_INL);
	r14 = r4;
	r4 += 24;
	r14 -= 8;                                        // slot do bra: registro embutido
	goto vtx;

strip_end:
	if (kind == STRIPS) {
		ENTER(entry + S_END, C_SEND);
		r6 += 64;
		const bool more = (s32)r0 > 0;               // cmp/pl r0 (r0 = M)
		T = !(r0 & 0x80);                            // slot do bf.s
		if (!more)
			goto s_ret3;
		ENTER(entry + S_MORE, C_SMORE);
		r3 = ram32(r4); r4 += 4;                     // slot: contagem do próximo strip
		if (!T)
			goto strip;                              // bit 7: outro strip
		ENTER(entry + S_RET4, C_SRET4);
		r4 -= 4;
		goto s_ret;
	s_ret3:
		ENTER(entry + S_RET3, C_SRET3);
	s_ret:
		sz = 0;                                      // fschg
		r4 -= 4;                                     // r4 = palavra M
		r6 -= 32;                                    // slot do rts
		SAVE(s);
		return return_to(s.pr);
	}

	ENTER(entry + T_END, C_TEND);                    // TRIS: fim do triângulo
	r4 -= 4;                                         // r4 = entrada seguinte
	T = --r9 == 0;
	r6 += 64;
	r3 = 3;                                          // slot do bf.s
	if (!T)
		goto strip;
	ENTER(entry + T_RET, C_TRET);
	sz = 0;                                          // fschg
	r6 -= 32;                                        // slot do rts
	SAVE(s);
	return return_to(s.pr);
}

#undef ENTER
#undef BAIL

} // namespace vtxargb
