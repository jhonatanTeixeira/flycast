// Strips transformados (sem luz / com tabela de luzes direcionais e pontuais) para a TA —
// pseudo-C++ para todos os jogos do grupo 004
// (docs/sdk_find/auto/004_matriz_produto_escalar_divisao.md). API: ver
// docs/sdk_blocks/luz_e_transformacao_de_vertices_pseudo.cpp. Referência bit-exata do
// laço irmão (mesma biblioteca, mesmo stream de entrada): core/rec-ARM64/hle_fn.cpp
// (doa2_run) e docs/sdk_blocks/vertices_com_clamp_pseudo.cpp.
// Além daquela API: exit_to(pc) = estado JÁ gravado em s, o despachante segue de pc (ao
// contrário de bail(pc), que recusa sem tocar em nada: o bloco do JIT roda normalmente);
// is_ram(a); assinatura_sem_luz(pc) = compara os bytes com os da 8C101930 (aceitando
// fldi0/fldi1 em +6); custos_*(…) = guest_cycles dos blocos do JIT (ver CUSTOS).
//
// O QUE FAZ
//   Três rotinas da mesma biblioteca (jogos de luta / AM2), que o sdk_find juntou num
//   grupo porque compartilham o laço de transformação e o stream de entrada:
//
//   A  SEM LUZ (strips): por vértice, ftrv + 1/w e 32 bytes na SQ:
//        [PCW, X/w, Y/w, 1/w, u, v, 1.0, K]       (os dois últimos gravados 1 vez só)
//      K = 0.0 (fldi0 fr3) na cópia "sem luz" do despachante por r8, e 1.0 (fldi1 fr3)
//      na cauda r8 < 0 das rotinas com luz (A'). Laço desenrolado em 2 vértices: um em
//      fv4 (fr4..fr7, u/v em fr0/fr1), o seguinte em fv8 (fr8..fr11); o 1/w de um sai
//      enquanto o ftrv do outro roda.
//   B  COM LUZ (strips) e C COM LUZ (triângulos soltos): por vértice, ilumina a NORMAL
//      (espaço do objeto) com as luzes ativas de uma tabela, faz o ftrv dentro da
//      última luz, e grava 64 bytes na SQ (as duas metades, um `pref` em cada):
//        [PCW, X/w, Y/w, 1/w, u, v, —, —, A, R·r, G·g, B·b, 0, 0, 0, 0]
//      (A,R,G,B) = material (MAT) e (r,g,b) = ambiente + Σ luzes; a cor offset (os
//      últimos 16 bytes) é zerada uma vez na entrada e nunca mais escrita; os bytes
//      +24..+31 (ignorados pela TA) também não são escritos.
//      B = strips (mesmo stream da A); C = r9 grupos de 3 vértices (triângulos), cada
//      grupo fechado com PCW de fim de strip, sem palavra separadora.
//   Despacho: as rotinas com luz começam com `cmp/pz r8; bt +8; bra <A'>; nop`
//   (r8 < 0 → A', sem luz com K = 1.0). A A (K = 0) é alcançada pelo despachante de 3
//   modos (DOA2 8C101920: r8 == 0 → 8C101A52 (grupo 005); r8 ímpar (`tst #1,r0; bf`)
//   → 8C101930 (este arquivo, A); r8 par ≠ 0 → 8C101BA6 (vertices_com_clamp, onde r8 é
//   o ponteiro da tabela de cor, por isso par). O gancho fica na 1ª instrução da A
//   (bloco natural: alvo do `bf`), não no despachante.
//
// Entrada (FPSCR.PR = 0, SZ = 0; o `fschg` do prólogo liga SZ e o do fim desliga: dentro,
// todo `fmov.s` com registrador par move um PAR de floats, fr[n] no endereço baixo)
//   r4   stream de controle (RAM):
//          A, B: +0 contagem de vértices do 1º strip (u32); C: +0 nº de triângulos (r9)
//          por vértice uma entrada (bit 0 da 1ª palavra):
//            formato 1 (bit0 = 1): o registro de 32 bytes EMBUTIDO no stream (a 1ª
//                                  palavra é o próprio x; o bit baixo serve de marca)
//            formato 0 (bit0 = 0): 8 bytes {flags, rel}; registro em (entrada + 8) + rel
//          A, B: depois do último vértice de um strip vêm {sep, n}: se (s32)sep > 0 e
//          (sep & 0x80), novo strip com n vértices; senão fim. C: os grupos de 3 seguem
//          direto, sem separador.
//   Registro (32 bytes): x, y, z, nx, ny, nz, u, v
//   r6   endereço na Store Queue (0xE…). A: 32 bytes por vértice, alternando as metades;
//        B/C: 64 bytes por vértice (as duas metades)
//   XMTRX matriz (componente 0 do ftrv = w: 1/w = 1/fr4; componentes 1, 2 → X, Y)
//   r8   modo (só o sinal importa aqui, nas rotinas com luz)
//   r5   anda +32 (A) / +64 (B, C) por vértice; não é desreferenciado
// Literais das rotinas com luz (lidos do `mov.l @(disp,PC)` de cada jogo):
//   MASKP  → u32 máscara de luzes ativas (bit k = luz k); relida a cada vértice
//   AMB    → 4 floats: fr12 (não usado na cor; a luz pontual sobrescreve), fr13..15 =
//            ambiente r,g,b (valor inicial dos acumuladores, recarregado a cada vértice)
//   LIGHTS → tabela de luzes, 0xB0 bytes por luz:
//            +0x02 s16 tipo: 2 = direcional, 3 = pontual (outros: caminho não observado)
//            +0x18 cor r, g, b
//            direcional: +0x80 direção (3 floats);  I = N·D   (se N·D ≥ 0; senão nada)
//            pontual:    +0x74 posição; +0x98 alcance (comparado com d²);
//                        +0x4C, +0x50, +0x54 = a0, a1, a2;  att = a0 + a2·d² + a1·d
//                        I = (N·(P_l − P)/d) / att  se att > 1, senão N·(P_l − P)/d
//                        (só se N·(P_l−P) ≥ 0 e d² < alcance)
//            acumula: (r, g, b) += I·cor  (fmac, fundido)
//   MAT    → 4 floats A, R, G, B do material
//   (os 4 literais apontam para variáveis globais do jogo; valores abaixo)
// PCW: o próprio endereço do vértice na SQ (0xE…: bits 31..29 = 7 → parâmetro de
//   vértice, bit 28 = 0). Último vértice de um strip/triângulo: endereço >> 1 aritmético
//   (0xF…: liga o bit 28, fim de strip) — A: (V+24) >> 1 ou 0xFFFFFFFF no Power Stone;
//   B/C: (V+48) >> 1. Os vértices intermediários da A usam V+24 (não V).
// Saída nos registros: a do JIT, campo a campo (todos os registros que a rotina toca são
//   locais em `Loc` e voltam no fim). `rts` (pr intacto); SZ volta a 0.
//
// MEMBROS (6 jogos, 12 entradas da tabela do sdk_find)                    perf (% da emu;
//   Jogo               entrada md   rotina                       instr.   só 7 jogos têm)
//   Dead or Alive 2    8C101930     A  (K=0)                     141      0,26%
//   Dead or Alive 2    8C102A80     B  com luz, strips           203      6,28%  ← o alvo
//   Dead or Alive 2    8C102DE0     C  com luz, triângulos       192      0,33%
//   Marvel vs Capcom 2 8C12A7F0     A  (K=0)                     141      2,52%
//   Capcom vs SNK 2    8C17C3D0     A  (K=0)                     141      —
//   Capcom vs SNK 2    8C17FDF0     B  + cauda A' em 8C180024    283      —
//   Capcom vs SNK 2    8C180150     C                            129      —
//   Power Stone        0C0E7610     A  (variante PCW 0xFF)       138      —
//   Project Justice    0C150C00     despachante + A em 0C150C10  140      —
//   Project Justice    0C1546BC     `bra` + A' em 0C154962       140      —
//   Shenmue II         8C1D8A90     A  (K=0)                     138      0,22%
//   Shenmue II         8C1DC360     despacho r8 + A' 8C1DC570    142      0,16%
//   Cópias do mesmo código: CvS2 8C17C3D0 = 8C180024, PJ 0C150C10 = 0C154962, Shenmue II
//   8C1D8A90 = 8C1DC570 (A e A', só o fldi em +6 muda); DOA2 B e C (e CvS2 B e C)
//   compartilham byte a byte o laço de luzes e o fechamento do vértice.
//   Valores dos literais: DOA2 (B e C) MASKP 0x8C112550, AMB 0x8C112540, LIGHTS
//   0x8C2EF7E8, MAT 0x8C112300. CvS2: MASKP 0x8C3B1490, AMB 0x8C3B1480, LIGHTS
//   0x8C39B154; MAT 0x8C3B1240 na C; na B o literal (8C180020) está em outra página e o
//   SHIL não o dobrou: valor NÃO visto (provavelmente o mesmo 0x8C3B1240; ler da RAM).
//
// VARIANTES (comparadas nos dumps, opcode a opcode por offset na A e com alinhamento
// normalizando deslocamentos de desvio/literal nas com luz — tools/sdk_vdiff.py e
// script equivalente sobre a listagem plana de todos os blocos compilados)
//   A: DOA2 8C101930, MvC2 8C12A7F0, CvS2 8C17C3D0, PJ 0C150C10, Shenmue II 8C1D8A90 —
//      byte a byte IGUAIS (172/174/140/155 posições comparadas, 0 diferenças); só a
//      cobertura muda (PJ e Shenmue II não executaram o fim de lista com fv4 pendente,
//      PJ não executou o formato 0 na entrada).
//   A': CvS2 8C180024, PJ 0C154962, Shenmue II 8C1DC570 — iguais à A exceto `fldi1 fr3`
//      em +6 (K = 1.0). PJ 0C154962 começa em endereço ≡ 2 (mod 4): some o word de
//      alinhamento depois do `bra` de +0xA2 e os offsets depois de +0xA6 andam −2 bytes
//      (só os deslocamentos de desvio mudam; nenhuma saída nossa cai depois disso).
//   Power Stone 0C0E7610: a única diferença REAL da A. O PCW de fim de strip é
//      `mov #0xFF,r2` (0xFFFFFFFF) em vez de `mov r6,r2; shar r2`, nos 3 lugares
//      (reinício de strip nos dois meios e fim da lista) → esses blocos têm 1 instrução
//      e 1 ciclo a menos, os offsets andam depois de +0xB6, e o T de saída não vem do
//      shar (fica o do último teste).
//   B: DOA2 8C102A80 × CvS2 8C17FDF0 — iguais nos trechos que os dois executaram (167
//      opcodes normalizados iguais, 0 substituições). A CvS2 tem dois pools de literais
//      embutidos (8C17FE30..37, 8C17FEA4..AB) → os offsets andam +8 e +16: cabeça das
//      luzes em +0x6C (DOA2 +0x64), fechamento em +0x16A (DOA2 +0x15A).
//   C: DOA2 8C102DE0 × CvS2 8C180150 — idem (145 iguais, 0 substituições; mesmos +8/+16).
//   B × C (mesmo jogo): 200 opcodes iguais; mudam só a cabeça (`mov.l @r4+,r9; mov
//      #3,r3`) e o fim de cada grupo (8C102F9C: PCW de fim, `dt r9`, sem separador).
//   Shenmue II 8C1DC360: só o despacho r8 < 0 e a cauda A' rodaram; o corpo com luz
//      (8C1DC368…) não está no dump — recusado aqui (r8 ≥ 0 → bail).
//   Fora do grupo: PJ 0C1546C0 (r8 par no despachante 0C1546B0) parece a B mas zera a
//      cor com `add #48,r6` (outro formato) — não coberto.
//
// NÃO DETERMINADO (nunca executado em nenhum dump; o código recusa em vez de adivinhar)
//   - máscara de luzes = 0: B/C desviam para +0xAC (8C102B2C), não visto → bail.
//   - última luz ativa pontual: cai em 8C102B4A (provável `ftrv xmtrx,fv4`, como o
//     8C102B00 da direcional, mas não visto) → bail.
//   - tipo de luz ≠ 2 e ≠ 3: 8C102B64..8C102B88 (spot?) não visto → bail.
//   - bloco 8C102BB8 (atenuação ≤ 1): o código está visível (é o fim do bloco BB6), mas o
//     bloco nunca foi compilado → o custo em ciclos não foi observado (C.p_le).
//   - bit 0 da máscara ligado: bloco 8C102AEA não roda no DOA2; visto na CvS2 (8C17FE62).
//   - DOA2 8C102A84 / 8C102DE4 (`bra` da cauda r8 < 0): o alvo não foi visto → lido da
//     RAM e conferido com assinatura_sem_luz.
//   - despachante da PJ (0C150C06 e 0C150C0C: r8 == 0 e r8 par) não visto; não é usado.
//
// CUSTOS (ciclos = guest_cycles dos blocos do JIT, já com o sh4clock; o nativo desconta
//   exatamente a soma dos blocos do caminho real). Na produção, montar a tabela no gancho
//   decodificando os blocos nos offsets abaixo (mesma função do decoder); os números
//   são os do dump do DOA2 (iguais na MvC2 e na CvS2 para os mesmos blocos).
//   Fronteiras naturais (início de bloco no fluxo do JIT, onde dá para devolver com o
//   estado em dia): A → a cabeça do laço (+0x2A, alvo do `bra` de volta, SZ = 1);
//   B/C → a cabeça da iluminação (+0x64/+0x6C, depois do `bt.s` de "sem luz", SZ = 0).
//   Orçamento checado uma vez por volta (A, 2 vértices) / por vértice (B/C) contra um
//   limite superior do caminho; sem saldo, devolve ao JIT na fronteira (ele faz o
//   UpdateSystem no bloco certo). Validar com FC_STATE_HASH como o doa2_run.

#pragma GCC optimize("fp-contract=off")   // só as fusões que o JIT faz (ftrv, fmac)
#include <arm_neon.h>
#include <math.h>

namespace stripxf {

// ---------------------------------------------------------------------------------------
// Operações de float idênticas às do JIT (rec_arm64.cpp)

// ftrv: fmul da coluna 0 + três fmla fundidas
static inline float32x4_t ftrv(const float32x4_t m[4], float a, float b, float c, float d)
{
	float32x4_t acc = vmulq_n_f32(m[0], a);
	acc = vfmaq_n_f32(acc, m[1], b);
	acc = vfmaq_n_f32(acc, m[2], c);
	return vfmaq_n_f32(acc, m[3], d);
}
// fipr: produtos sem fusão, soma (p0+p1)+(p2+p3); os 4 termos sempre (fr3·fr11 incluso:
// com fr3 = 0 ele só importa para NaN/inf/sinal do zero, mas o JIT faz a conta)
static inline float fipr(float a0, float a1, float a2, float a3, float b0, float b1, float b2, float b3)
{
	const float32x4_t p = vmulq_f32((float32x4_t){ a0, a1, a2, a3 }, (float32x4_t){ b0, b1, b2, b3 });
	const float32x4_t s = vpaddq_f32(p, p);
	return vpadds_f32(vget_low_f32(s));
}
static inline float fsrra(float x) { return 1.f / sqrtf(x); }              // fsqrt + fdiv (não frsqrte)
static inline float fmac(float f0, float m, float n) { return __builtin_fmaf(f0, m, n); } // FRn += FR0·FRm

static inline bool in_sq(u32 a) { return (a >> 26) == 0x38; }
static inline void sq_pair(u32 a, float lo, float hi) { sq_write(a, f2u(lo)); sq_write(a + 4, f2u(hi)); }
static inline s32 max2(s32 a, s32 b) { return a > b ? a : b; }
static inline s32 max3(s32 a, s32 b, s32 c) { return max2(a, max2(b, c)); }
static inline u32 bra_target(u32 pc) { const s32 d = (s32)((u32)(u16)ram16s(pc) << 20) >> 20; return pc + 4 + d * 2; }
// endereço do literal de um `mov.l @(disp,PC),Rn` (0xDndd)
static inline u32 lit_de(u32 pc) { const u16 op = (u16)ram16s(pc); return ((pc + 4) & ~3u) + (op & 0xFF) * 4; }

// Estado do SH4 que as rotinas tocam, em locais
struct Loc
{
	u32 r0, r1, r2, r3, r4, r5, r6, r9, r13, r14, fpul;
	bool T;
	float f[16];
	explicit Loc(const Sh4 &s)
	{
		r0 = s.r[0]; r1 = s.r[1]; r2 = s.r[2]; r3 = s.r[3]; r4 = s.r[4]; r5 = s.r[5]; r6 = s.r[6];
		r9 = s.r[9]; r13 = s.r[13]; r14 = s.r[14]; fpul = s.fpul; T = s.sr.T;
		for (int i = 0; i < 16; i++) f[i] = s.fr[i];
	}
	void store(Sh4 &s, u32 sz) const
	{
		s.r[0] = r0; s.r[1] = r1; s.r[2] = r2; s.r[3] = r3; s.r[4] = r4; s.r[5] = r5; s.r[6] = r6;
		s.r[9] = r9; s.r[13] = r13; s.r[14] = r14; s.fpul = fpul; s.sr.T = T;
		for (int i = 0; i < 16; i++) s.fr[i] = f[i];
		s.fpscr.SZ = sz;                         // na produção: via UpdateFPSCR do core
	}
};
static inline void ftrv_fv4(Loc &L, const float32x4_t m[4]) { vst1q_f32(&L.f[4], ftrv(m, L.f[4], L.f[5], L.f[6], L.f[7])); }
static inline void ftrv_fv8(Loc &L, const float32x4_t m[4]) { vst1q_f32(&L.f[8], ftrv(m, L.f[8], L.f[9], L.f[10], L.f[11])); }

// =======================================================================================
// A / A' — sem luz
// =======================================================================================

// Custos dos blocos (DOA2 8C101930 = head; entre parênteses o Power Stone quando muda)
struct CustosSemLuz
{
	s32 head;            // 8C101930  7   (até o `bf` do formato)
	s32 head_f0;         // 8C101948  8   formato 0 + 1º vértice + cabeça do laço (até o bt.s)
	s32 head_f1;         // 8C10194E  5   formato 1 + idem
	s32 loop;            // 8C10195A  4   cabeça do laço quando vem do `bra` (fronteira, +0x2A)
	s32 a_next;          // 8C101966  4   formato do próximo (fv4 pendente)
	s32 a_f0, a_f1;      // 8C101974 12 / 8C10196E 15   grava fv4, carrega e transforma fv8
	s32 a_end1, a_end2;  // 8C101A0C 3 / 8C101A12 3     fim do strip com fv4 pendente
	s32 a_restart;       // 8C101A18  7 (6)  novo strip: PCW de fim + formato
	s32 a_restart_f1;    // 8C101A26  4   (depois vai para 8C101974 = a_f0)
	s32 a_fin;           // 8C101A30 11 (10) fim da lista com fv4 pendente (+ rts)
	s32 b_next;          // 8C1019A4  3
	s32 b_f0, b_f1;      // 8C1019B0  9 / 8C1019AA 12   grava fv8, carrega e transforma fv4
	s32 b_end1, b_end2;  // 8C1019D8 3 / 8C1019DE 3
	s32 b_restart;       // 8C1019E4  7 (6)
	s32 b_restart_f1;    // 8C1019F2  4   (depois 8C1019B0 = b_f0)
	s32 b_fin;           // 8C1019FC  5   fv8 → fv4, depois:
	s32 fin;             // 8C101A36  8 (7)  grava o último vértice (+ rts)
};
constexpr u32 OFF_LOOP = 0x2A;           // 8C10195A - 8C101930 (igual em todas as cópias)

// PCW de fim de strip: shar (0xE… → 0xF…, T = bit 0) ou 0xFFFFFFFF (Power Stone, T intacto)
static inline void pcw_fim(Loc &L, bool pcw_ff)
{
	if (pcw_ff) L.r2 = 0xFFFFFFFF;
	else { L.T = L.r6 & 1; L.r2 = (u32)((s32)L.r6 >> 1); }
}
// Lê a entrada do próximo vértice (r0 = 1ª palavra, r14 = 2ª, r4 já +8): resolve r14 para
// o registro (formato 0: rel; formato 1: a própria entrada) — o `bt.s …; add r4,r14` + 3
static inline bool resolve_reg(Loc &L)
{
	L.T = (L.r0 & 1) == 0;
	L.r14 += L.r4;                           // slot do bt.s: roda nos dois casos
	if (!L.T) { L.r14 = L.r4; L.r4 += 24; L.r14 -= 8; return true; }   // formato 1
	return false;
}
// Último vértice (fv4) + rts (8C101A36). r4 volta para o `sep`.
static void fim_sem_luz(Loc &L, bool pcw_ff)
{
	L.f[6] *= L.f[7];
	pcw_fim(L, pcw_ff);
	L.f[5] *= L.f[7];
	L.r6 -= 8; sq_pair(L.r6, L.f[0], L.f[1]);            // u, v
	L.r4 -= 8;
	L.r6 -= 8; sq_pair(L.r6, L.f[6], L.f[7]);            // Y/w, 1/w
	L.r5 += 32;
	L.r6 -= 8; sq_write(L.r6 + 4, f2u(L.f[5]));          // X/w (o fr4 de +0 é coberto pelo PCW)
	sq_write(L.r6, L.r2);                                // PCW de fim
	sq_flush(L.r6);
	L.r6 += 32;                                          // slot do rts
}

// entry = bloco onde o gancho entrou (head, ou a entrada da rotina com luz quando r8 < 0);
// pre = ciclos dos blocos de despacho já percorridos antes do head.
static void run_sem_luz(Sh4 &s, Cycles &cyc, u32 entry, u32 head, s32 pre, bool pcw_ff)
{
	if (s.fpscr.PR || s.fpscr.SZ || !is_ram(s.r[4]) || !in_sq(s.r[6]))
		return bail(entry);
	const CustosSemLuz &C = custos_sem_luz(head, pcw_ff);
	const s32 parte_a = max3(C.a_next + max2(C.a_f0, C.a_f1),
	                         C.a_end1 + C.a_end2 + C.a_restart + C.a_restart_f1 + C.a_f0,
	                         C.a_end1 + C.a_end2 + C.a_fin);
	const s32 parte_b = max3(C.b_next + max2(C.b_f0, C.b_f1),
	                         C.b_end1 + C.b_end2 + C.b_restart + C.b_restart_f1 + C.b_f0,
	                         C.b_end1 + C.b_end2 + C.b_fin + C.fin);
	const s32 volta_max = C.loop + parte_a + parte_b;
	if (cyc.left() < pre + C.head + max2(C.head_f0, C.head_f1) + parte_a + parte_b)
		return bail(entry);                              // nada mudou: o JIT roda a chamada toda

	const float K = (u16)ram16s(head + 6) == 0xF39D ? 1.f : 0.f;   // fldi1 fr3 (A') | fldi0 (A)
	const float32x4_t m[4] = { vld1q_f32(&s.xf[0]), vld1q_f32(&s.xf[4]),
	                           vld1q_f32(&s.xf[8]), vld1q_f32(&s.xf[12]) };
	Loc L(s);
	s32 c = pre + C.head;

	// 8C101930: contagem; intensidades (1.0, K) em +24 das DUAS metades da SQ (uma vez)
	L.r3 = ram32(L.r4); L.r4 += 4;
	L.f[2] = 1.f; L.f[3] = K;
	L.r6 -= 8; sq_pair(L.r6, 1.f, K);
	L.r6 += 32; sq_pair(L.r6, 1.f, K);                   // r6 = V+24 do 1º vértice
	L.r0 = ram32(L.r4); L.r14 = L.r4;
	L.T = (L.r0 & 1) == 0; L.r4 += 32;
	if (L.T) { L.r14 = ram32(L.r14 + 4); L.r4 -= 24; L.r14 += L.r4; c += C.head_f0; }
	else c += C.head_f1;
	// 1º vértice em fv4
	L.f[4] = ramf(L.r14); L.f[5] = ramf(L.r14 + 4); L.r14 += 8;
	L.f[6] = ramf(L.r14); L.f[7] = ramf(L.r14 + 4); L.r14 += 16;
	L.f[7] = 1.f;
	L.f[0] = ramf(L.r14); L.f[1] = ramf(L.r14 + 4);      // u, v
	ftrv_fv4(L, m);

	for (bool first = true;; first = false)
	{
		// ---- cabeça (8C10195A): no 1º passe está dentro dos blocos de entrada ----
		if (!first)
		{
			cyc.take(c); c = 0;
			if (cyc.left() < volta_max)                  // fronteira natural, SZ = 1 aqui
			{
				L.store(s, 1);
				return exit_to(head + OFF_LOOP);
			}
			c += C.loop;
		}
		L.f[7] = 1.f; L.f[7] = L.f[7] / L.f[4];          // 1/w de fv4
		L.r3--; L.T = L.r3 == 0;
		L.r0 = ram32(L.r4); L.r4 += 4;
		L.r14 = ram32(L.r4); L.r4 += 4;                  // slot do bt.s
		if (L.T)                                         // fim do strip, fv4 pendente
		{
			c += C.a_end1;                               // cmp/pl r0; mov.l @r4+,r2; bf
			L.T = (s32)L.r0 > 0; L.r2 = ram32(L.r4); L.r4 += 4;
			bool acabou = !L.T;
			if (!acabou) { c += C.a_end2; L.T = (L.r0 & 0x80) == 0; L.r3 = L.r14; acabou = L.T; }
			if (acabou)                                  // fim da lista (8C101A30)
			{
				c += C.a_fin;
				L.r4 -= 4;                               // +44, pref (RAM), −48
				fim_sem_luz(L, pcw_ff);
				cyc.take(c); L.store(s, 0);
				return return_to(s.pr);
			}
			c += C.a_restart;                            // novo strip: este vértice fecha o atual
			L.r0 = L.r2; pcw_fim(L, pcw_ff);
			L.r14 = ram32(L.r4); L.r4 += 4;
			if (resolve_reg(L)) c += C.a_restart_f1;
			c += C.a_f0;
		}
		else
		{
			c += C.a_next;
			L.r2 = L.r6;                                 // PCW = V+24 (0xE…, sem fim)
			c += resolve_reg(L) ? C.a_f1 : C.a_f0;
		}

		// ---- 8C101974: grava fv4, carrega o próximo em fv8 e transforma ----
		L.f[8] = ramf(L.r14); L.f[9] = ramf(L.r14 + 4); L.r14 += 8;
		L.f[10] = ramf(L.r14); L.f[11] = ramf(L.r14 + 4); L.r14 += 16;   // (pref @r4+32: RAM)
		L.f[11] = 1.f;
		L.f[6] *= L.f[7];
		L.r6 -= 8; sq_pair(L.r6, L.f[0], L.f[1]);
		L.f[5] *= L.f[7];
		L.r6 -= 8; sq_pair(L.r6, L.f[6], L.f[7]);
		ftrv_fv8(L, m);
		L.r6 -= 8; sq_write(L.r6 + 4, f2u(L.f[5]));
		sq_write(L.r6, L.r2);                            // PCW
		L.f[0] = ramf(L.r14); L.f[1] = ramf(L.r14 + 4);
		L.r3--; L.T = L.r3 == 0;
		sq_flush(L.r6);
		L.r6 += 56; L.r2 = L.r6;
		L.f[11] = 1.f; L.f[11] = L.f[11] / L.f[8];       // 1/w de fv8
		L.r0 = ram32(L.r4); L.r4 += 4;
		L.r14 = ram32(L.r4); L.r4 += 4;
		if (L.T)                                         // fim do strip, fv8 pendente
		{
			c += C.b_end1;
			L.T = (s32)L.r0 > 0; L.r2 = ram32(L.r4); L.r4 += 4;
			bool acabou = !L.T;
			if (!acabou) { c += C.b_end2; L.T = (L.r0 & 0x80) == 0; L.r3 = L.r14; acabou = L.T; }
			if (acabou)                                  // fim da lista (8C1019FC → 8C101A36)
			{
				c += C.b_fin + C.fin;
				L.r4 -= 4;
				L.f[4] = L.f[8]; L.f[5] = L.f[9];        // fmov dr8,dr4 (SZ=1: par)
				L.r5 += 32;
				L.f[6] = L.f[10]; L.f[7] = L.f[11];      // fmov dr10,dr6
				fim_sem_luz(L, pcw_ff);
				cyc.take(c); L.store(s, 0);
				return return_to(s.pr);
			}
			c += C.b_restart;
			L.r0 = L.r2; pcw_fim(L, pcw_ff);
			L.r14 = ram32(L.r4); L.r4 += 4;
			if (resolve_reg(L)) c += C.b_restart_f1;
			c += C.b_f0;
		}
		else
		{
			c += C.b_next;                               // (r2 já = V+24 deste vértice)
			c += resolve_reg(L) ? C.b_f1 : C.b_f0;
		}

		// ---- 8C1019B0: grava fv8, carrega o próximo em fv4 e transforma ----
		L.f[4] = ramf(L.r14); L.f[5] = ramf(L.r14 + 4); L.r14 += 8;
		L.f[6] = ramf(L.r14); L.f[7] = ramf(L.r14 + 4); L.r14 += 16;
		L.f[7] = 1.f;
		L.f[10] *= L.f[11];
		L.f[9] *= L.f[11];
		L.r6 -= 8; sq_pair(L.r6, L.f[0], L.f[1]);
		L.r6 -= 8; sq_pair(L.r6, L.f[10], L.f[11]);
		ftrv_fv4(L, m);
		L.r6 -= 8; sq_write(L.r6 + 4, f2u(L.f[9]));
		L.r5 += 64;
		sq_write(L.r6, L.r2);
		L.f[0] = ramf(L.r14); L.f[1] = ramf(L.r14 + 4);
		sq_flush(L.r6);
		L.r6 += 56;                                      // slot do `bra 8C10195A`
	}
}

// Gancho na A (8C101930 e cópias). Power Stone: pcw_ff = true.
void run_a(Sh4 &s, Cycles &cyc, u32 head, bool pcw_ff) { run_sem_luz(s, cyc, head, head, 0, pcw_ff); }

// =======================================================================================
// B / C — com luz
// =======================================================================================

enum Kind { LUZ_STRIPS, LUZ_TRIANGULOS };
struct Variant
{
	Kind kind;
	u32 off_lights;   // cabeça da iluminação (8C102AE4): 0x64 DOA2, 0x6C CvS2; 0 = corpo não observado (Shenmue II)
	u32 off_final;    // fechamento do vértice (8C102BDA): 0x15A DOA2, 0x16A CvS2
};
//   DOA2 8C102A80 {LUZ_STRIPS, 0x64, 0x15A}     DOA2 8C102DE0 {LUZ_TRIANGULOS, 0x64, 0x15A}
//   CvS2 8C17FDF0 {LUZ_STRIPS, 0x6C, 0x16A}     CvS2 8C180150 {LUZ_TRIANGULOS, 0x6C, 0x16A}
//   Shenmue II 8C1DC360 {LUZ_STRIPS, 0, 0}      (só r8 < 0)

// Custos (DOA2 8C102A80; a C tem os mesmos blocos de luz/fechamento em 8C102Exx/Fxx)
struct CustosLuz
{
	s32 e_head;               // A80 2   cmp/pz r8; bt
	s32 e_bra;                // A84 2   bra <A'>; nop (visto na CvS2 8C17FDF4)
	s32 e_body;               // A88 8   (C: 2DE8 9)
	s32 e_f0, e_f1;           // AA4 7 / AAA 4   (C: 2E06 7 / 2E0C 4)
	s32 ada;                  // ADA 2   normal (alvo do bra)
	s32 l_head;               // AE4 9   shlr r13 (bit 0) + literal LIGHTS  ← fronteira
	s32 l_bit0;               // AEA 5   (CvS2 8C17FE62; não roda no DOA2)
	s32 l_skip;               // B1C 2   (bit vazio / luz direcional de costas)
	s32 l_shift;              // B20 4
	s32 l_go;                 // B28 2   bra AEC; mov.w tipo
	s32 l_scan;               // BCE 4   um bit vazio a mais
	s32 l_go2;                // BD6 2   bra AEC; mov.w tipo
	s32 l_type;               // AEC 4   fldi0 fr3; cmp/eq #2
	s32 l_shift2;             // BD0 3
	s32 d_body;               // AF6 3
	s32 d_last, d_mid;        // B00 4 (com ftrv) / B02 4
	s32 acc;                  // BBA 2   3 fmac
	s32 after_acc;            // BC4 2   bra BD0; shlr
	s32 p_body;               // B34 3
	s32 p_dot;                // B4C 1
	s32 p_range;              // B58 1
	s32 p_type;               // B5C 2   flds; fsrra
	s32 p_att;                // B8A 6
	s32 p_gt;                 // BB6 3   att > 1
	s32 p_le;                 // BB8 ?   att ≤ 1 — bloco NUNCA compilado: custo não observado
	s32 p_skip;               // BC8 4   de costas / fora do alcance
	s32 fin;                  // BDA 11  fechamento + SQ
	s32 t_next;               // C10 3   PCW + formato do próximo
	s32 t_f1, t_f0;           // C16 2 + AC0 10 = 12 / C18 13 + ADA 2 = 15
	s32 s_end, s_end2;        // C3C 6 / C48 3   fim do strip
	s32 s_new;                // C4E 9
	s32 s_new_f1, s_new_f0;   // C60 1 + ACC 5 = 6 / C62 7 + ADA 2 = 9
	s32 ret;                  // C7A 5   (+ rts)
	s32 g_end, g_ret;         // C: 2F9C 9 / 2FB4 2
	s32 g_next;               // C: 2FB0 2 + 2DFA 6 = 8
	s32 g_f0, g_f1;           // C: 2E06 7 + 2E3A 2 = 9 / 2E0C 4 + 2E3A 2 = 6
};

struct Luz
{
	bool pontual;             // tipo 3; senão tipo 2 (direcional)
	float v[3];               // direcional: direção (+0x80); pontual: posição (+0x74)
	float cor[3];             // +0x18
	float alcance;            // pontual: +0x98 (limite de d²)
	float a0, a1, a2;         // pontual: +0x4C, +0x50, +0x54
	s32 ida_bd0, ida_b20;     // ciclos para chegar a ESTA luz vindo da anterior, via BD0 / via B20
};
struct Plano { int n; u32 mask; s32 ida_primeira; s32 vertice_max; Luz l[32]; };

// Decodifica a máscara uma vez por chamada: a rotina relê máscara, tipos e parâmetros a
// cada vértice, mas nada dentro dela escreve na RAM (só na SQ), então são constantes.
static bool planeja(Plano &P, u32 mask, u32 lights, const CustosLuz &C, Kind kind)
{
	P.n = 0; P.mask = mask;
	if (mask == 0) return false;                         // 8C102B2C: não visto
	auto via_b20 = [&](int g) { return C.l_shift + (g == 1 ? C.l_go : C.l_scan * (g - 1) + C.l_go2) + C.l_type; };
	int prev = -1;
	for (int k = 0; k < 32; k++)
	{
		if (!((mask >> k) & 1)) continue;
		const u32 e = lights + 0xB0 * k;
		const s16 tipo = ram16s(e + 2);
		if (tipo != 2 && tipo != 3) return false;        // 8C102B64..B88: não visto
		Luz &l = P.l[P.n++];
		l.pontual = tipo == 3;
		const u32 pv = l.pontual ? e + 0x74 : e + 0x80;
		l.v[0] = ramf(pv); l.v[1] = ramf(pv + 4); l.v[2] = ramf(pv + 8);
		l.cor[0] = ramf(e + 0x18); l.cor[1] = ramf(e + 0x1C); l.cor[2] = ramf(e + 0x20);
		l.alcance = ramf(e + 0x98);
		l.a0 = ramf(e + 0x4C); l.a1 = ramf(e + 0x50); l.a2 = ramf(e + 0x54);
		l.ida_bd0 = l.ida_b20 = 0;
		if (prev < 0)   // AE4 consome o bit 0; com k > 0, B1C + o caminho do B20 a partir do bit 1
			P.ida_primeira = C.l_head + (k == 0 ? C.l_bit0 : C.l_skip + via_b20(k));
		else
		{
			const int g = k - prev;
			l.ida_bd0 = C.l_shift2 + C.l_scan * (g - 1) + C.l_go2 + C.l_type;
			l.ida_b20 = via_b20(g);
		}
		prev = k;
	}
	if (P.l[P.n - 1].pontual) return false;              // 8C102B4A: não visto
	// limite superior do vértice: cada luz no caminho mais caro + fechamento + cauda
	s32 v = P.ida_primeira + C.fin;
	for (int i = 0; i < P.n; i++)
	{
		const Luz &l = P.l[i];
		const bool ult = i == P.n - 1;
		const s32 bd0 = ult ? 0 : P.l[i + 1].ida_bd0, b20 = ult ? 0 : P.l[i + 1].ida_b20;
		if (!l.pontual)
			v += C.d_body + (ult ? C.d_last : C.d_mid) + max2(C.acc + (ult ? 0 : C.after_acc + bd0), C.l_skip + b20);
		else
			v += C.p_body + C.p_dot + max2(C.p_range + C.p_skip + bd0,
			                              C.p_range + C.p_type + C.p_att + max2(C.p_gt, C.p_le) + C.after_acc + bd0);
	}
	v += kind == LUZ_STRIPS
	   ? max3(C.t_next + max2(C.t_f0, C.t_f1), C.s_end + C.s_end2 + C.s_new + max2(C.s_new_f0, C.s_new_f1),
	          C.s_end + C.s_end2 + C.ret)
	   : max3(C.t_next + max2(C.t_f0, C.t_f1), C.g_end + C.g_next + max2(C.g_f0, C.g_f1), C.g_end + C.g_ret);
	P.vertice_max = v;
	return true;
}

// Luzes de um vértice (8C102AE4..8C102BD8). Entra com fr4..fr6 = posição, fr8..fr10 =
// normal, fr12..fr15 = AMB; sai com fv4 = XMTRX·(x,y,z,1) (o ftrv roda dentro da última
// luz, que é sempre direcional) e fr13..15 = ambiente + Σ I·cor. r0/r1/r2/T do caminho
// não escapam (o fechamento sobrescreve todos); fr0, fr1, fr12, FPUL escapam.
static void ilumina(Loc &L, const Plano &P, const CustosLuz &C, const float32x4_t m[4], s32 &c)
{
	c += P.ida_primeira;
	for (int i = 0; i < P.n; i++)
	{
		const Luz &l = P.l[i];
		const bool ult = i == P.n - 1;
		const Luz *prox = ult ? nullptr : &P.l[i + 1];
		L.f[3] = 0.f;                                    // fldi0 fr3 (AEC)
		if (!l.pontual)
		{
			// AF6..B1A: direcional
			L.f[7] = 1.f;
			L.f[0] = l.v[0];
			if (ult) ftrv_fv4(L, m);                     // B00 (fr7 = 1)
			L.f[1] = l.v[1]; L.f[2] = l.v[2];
			L.f[11] = fipr(L.f[8], L.f[9], L.f[10], L.f[11], L.f[0], L.f[1], L.f[2], L.f[3]);  // fipr fv0,fv8
			L.f[1] = l.cor[0]; L.f[2] = l.cor[1]; L.f[3] = l.cor[2];
			const bool costas = 0.f > L.f[11];           // fldi0 fr0; fcmp/gt fr11,fr0
			L.f[0] = L.f[11];
			c += C.d_body + (ult ? C.d_last : C.d_mid);
			if (!costas)
			{
				L.f[13] = fmac(L.f[0], L.f[1], L.f[13]);     // BBA
				L.f[14] = fmac(L.f[0], L.f[2], L.f[14]);
				L.f[15] = fmac(L.f[0], L.f[3], L.f[15]);
				c += C.acc + (ult ? 0 : C.after_acc + prox->ida_bd0);
			}
			else
				c += C.l_skip + (ult ? 0 : prox->ida_b20);
			continue;
		}
		// B34..: pontual (nunca a última — planeja recusa)
		L.f[0] = l.v[0] - L.f[4];
		L.f[1] = l.v[1] - L.f[5];
		L.f[7] = 1.f;
		L.f[11] = 0.f;
		L.f[2] = l.v[2] - L.f[6];
		L.f[11] = fipr(L.f[8], L.f[9], L.f[10], L.f[11], L.f[0], L.f[1], L.f[2], L.f[3]);  // N·(Pl−P)
		L.f[12] = 0.f;
		L.f[3] = fipr(L.f[0], L.f[1], L.f[2], L.f[3], L.f[0], L.f[1], L.f[2], L.f[3]);     // d²
		const bool costas = L.f[12] > L.f[11];          // fcmp/gt fr11,fr12
		L.f[12] = l.alcance;
		c += C.p_body + C.p_dot;
		if (costas) { c += C.p_skip + prox->ida_bd0; continue; }
		c += C.p_range;
		if (!(L.f[12] > L.f[3])) { c += C.p_skip + prox->ida_bd0; continue; }   // fcmp/gt fr3,fr12
		c += C.p_type;
		L.fpul = f2u(L.f[3]);                            // flds fr3,FPUL
		L.f[3] = fsrra(L.f[3]);                          // 1/d (slot do bt.s)
		// B8A: atenuação
		L.f[2] = l.a2;
		L.f[0] = u2f(L.fpul);                            // d²
		L.f[11] = L.f[11] * L.f[3];                      // N·(Pl−P)/d
		L.f[12] = l.a0;
		L.f[3] = L.f[3] * L.f[0];                        // (1/d)·d² ≈ d
		L.f[12] = fmac(L.f[0], L.f[2], L.f[12]);         // a0 + d²·a2
		L.f[2] = l.a1;
		L.f[0] = L.f[3];
		L.f[12] = fmac(L.f[0], L.f[2], L.f[12]);         // + d·a1
		L.f[0] = L.f[11];
		L.f[11] = L.f[11] / L.f[12];                     // fica em fr11 mesmo se não usado
		L.f[2] = 1.f;
		const bool atenua = L.f[12] > L.f[2];
		L.f[1] = l.cor[0]; L.f[2] = l.cor[1]; L.f[3] = l.cor[2];
		if (atenua) L.f[0] = L.f[11];                    // BB6
		L.f[13] = fmac(L.f[0], L.f[1], L.f[13]);
		L.f[14] = fmac(L.f[0], L.f[2], L.f[14]);
		L.f[15] = fmac(L.f[0], L.f[3], L.f[15]);
		c += C.p_att + (atenua ? C.p_gt : C.p_le) + C.after_acc + prox->ida_bd0;
	}
	L.r13 = 0;                                           // todos os bits saíram pelo shlr
}

// Abre o vértice da entrada em r4 (8C102A98.., C18, C4E, 2DFA): r14 → registro, r4 →
// próxima entrada + 32 (o `pref @r4` do laço olha 32 bytes à frente), AMB recarregado,
// posição em fr4..fr7 e (ADA) normal em fr8..fr10. Devolve "formato 1".
static bool abre_vertice(Loc &L, u32 maskp, const float amb[4])
{
	const u32 ctl = L.r4;
	const bool f1 = ram32(ctl) & 1;
	if (f1) { L.r14 = ctl; L.r4 = ctl + 64; }
	else    { L.r14 = ctl + 8 + ram32(ctl + 4); L.r4 = ctl + 40; }
	L.r13 = ram32(maskp);
	L.f[12] = amb[0]; L.f[13] = amb[1]; L.f[14] = amb[2]; L.f[15] = amb[3];
	L.f[4] = ramf(L.r14); L.f[5] = ramf(L.r14 + 4); L.f[6] = ramf(L.r14 + 8); L.f[7] = ramf(L.r14 + 12);
	L.r14 += 16;
	L.f[8] = L.f[7];                                     // ADA: nx (fmov fr7,fr8, SZ = 0)
	L.f[9] = ramf(L.r14); L.f[10] = ramf(L.r14 + 4); L.r14 += 8;
	L.T = L.r13 == 0;                                    // (falso: máscara ≠ 0 garantida)
	return f1;
}

void run_luz(Sh4 &s, Cycles &cyc, u32 base, const Variant &v)
{
	const CustosLuz &C = custos_luz(base, v);
	if ((s32)s.r[8] < 0)                                 // r8 < 0 → cauda sem luz, K = 1.0
	{
		const u32 tail = bra_target(base + 4);
		if (!assinatura_sem_luz(tail)) return bail(base);   // DOA2: este `bra` nunca rodou
		return run_sem_luz(s, cyc, base, tail, C.e_head + C.e_bra, false);
	}
	if (v.off_lights == 0) return bail(base);            // Shenmue II: corpo com luz não visto
	if (s.fpscr.PR || s.fpscr.SZ || !is_ram(s.r[4]) || !in_sq(s.r[6]))
		return bail(base);
	const u32 d = v.kind == LUZ_TRIANGULOS ? 2 : 0;      // a C tem `mov #3,r3` a mais na cabeça
	const u32 maskp  = ram32(lit_de(base + 0x1E + d));
	const u32 ambp   = ram32(lit_de(base + 0x2A + d));
	const u32 lights = ram32(lit_de(base + v.off_lights + 2));
	const u32 matp   = ram32(lit_de(base + v.off_final));
	Plano P;
	if (!planeja(P, ram32(maskp), lights, C, v.kind)) return bail(base);
	if (cyc.left() < C.e_head + C.e_body + max2(C.e_f0, C.e_f1) + C.ada + P.vertice_max)
		return bail(base);

	const float amb[4] = { ramf(ambp), ramf(ambp + 4), ramf(ambp + 8), ramf(ambp + 12) };
	const float mat[4] = { ramf(matp), ramf(matp + 4), ramf(matp + 8), ramf(matp + 12) };
	const float32x4_t m[4] = { vld1q_f32(&s.xf[0]), vld1q_f32(&s.xf[4]),
	                           vld1q_f32(&s.xf[8]), vld1q_f32(&s.xf[12]) };
	Loc L(s);

	// ---- entrada (8C102A88 / 8C102DE8) ----
	s32 c = C.e_head + C.e_body + C.ada;
	if (v.kind == LUZ_TRIANGULOS) { L.r9 = ram32(L.r4); L.r4 += 4; L.r3 = 3; }
	else { L.r3 = ram32(L.r4); L.r4 += 4; }
	L.f[0] = 0.f; L.f[1] = 0.f;
	L.r6 += 64;
	L.r6 -= 8; sq_pair(L.r6, 0.f, 0.f);                  // cor offset = 0 (V+56, V+48),
	L.r6 -= 8; sq_pair(L.r6, 0.f, 0.f);                  // nunca mais escrita → todos os vértices
	c += abre_vertice(L, maskp, amb) ? C.e_f1 : C.e_f0;
	L.r0 = ambp + 16;                                    // r0 depois dos `fmov @r0+` do AMB

	for (;;)
	{
		// ---- fronteira natural: bloco da cabeça da iluminação (8C102AE4), SZ = 0 ----
		cyc.take(c); c = 0;
		if (cyc.left() < P.vertice_max)
		{
			L.store(s, 0);
			return exit_to(base + v.off_lights);
		}
		ilumina(L, P, C, m, c);

		// ---- 8C102BDA: fechamento — 1/w, cor final, SQ ----
		c += C.fin;
		L.f[7] = 1.f; L.f[7] = L.f[7] / L.f[4];          // 1/w
		L.r4 -= 32;                                      // (pref @r4: RAM) → entrada seguinte
		L.f[8] = mat[0]; L.f[9] = mat[1]; L.f[10] = mat[2]; L.f[11] = mat[3];
		L.r2 = matp + 16;
		const u32 Q = L.r6;                              // = V + 48
		L.r1 = (u32)((s32)Q >> 1);                       // PCW de fim de strip (shar)
		L.f[9] *= L.f[13];
		L.r0 = ram32(L.r4);                              // 1ª palavra da entrada seguinte
		L.f[10] *= L.f[14];
		L.f[2] = ramf(L.r14); L.f[3] = ramf(L.r14 + 4); L.r14 += 8;   // u, v
		L.f[11] *= L.f[15];
		L.r13 = maskp;
		L.r3--; L.T = L.r3 == 0;
		const u32 V = Q - 48;
		sq_pair(V + 40, L.f[10], L.f[11]);
		L.r14 = L.r4;
		sq_pair(V + 32, L.f[8], L.f[9]);                 // (A, R·r, G·g, B·b)
		L.f[6] *= L.f[7];
		sq_pair(V + 16, L.f[2], L.f[3]);                 // u, v
		L.f[5] *= L.f[7];
		sq_pair(V + 8, L.f[6], L.f[7]);                  // Y/w, 1/w
		sq_write(V + 4, f2u(L.f[5]));                    // X/w (o fr4 de V+0 vira PCW)
		L.r6 = V;

		if (!L.T)
		{
			// ---- C10: próximo vértice do mesmo strip/triângulo ----
			c += C.t_next;
			sq_write(V, V);                              // PCW = V (0xE…, sem fim)
			sq_flush(V);                                 // 1ª metade
			L.r6 = V + 32; L.r1 = L.r6; L.r5 += 64;
			c += abre_vertice(L, maskp, amb) ? C.t_f1 : C.t_f0;
			L.r0 = ambp + 16;
			L.r6 = V + 112;                              // = V' + 48
			sq_flush(L.r1);                              // 2ª metade (cor), depois das cargas
			continue;
		}

		if (v.kind == LUZ_TRIANGULOS)
		{
			// ---- 8C102F9C: fim do triângulo ----
			c += C.g_end;
			sq_write(V, L.r1);                           // PCW de fim
			L.r9--; L.T = L.r9 == 0;
			sq_flush(V);
			L.r5 += 64;
			sq_flush(V + 32);
			if (L.T)                                     // rts; r6 = V + 64
			{
				c += C.g_ret;
				L.r6 = V + 64;
				cyc.take(c); L.store(s, 0);
				return return_to(s.pr);
			}
			c += C.g_next;
			L.r3 = 3;                                    // slot do `bra 8C102DFA`
			L.r6 = V + 112;
			c += abre_vertice(L, maskp, amb) ? C.g_f1 : C.g_f0;
			L.r0 = ambp + 16;
			continue;
		}

		// ---- C3C: fim do strip ----
		c += C.s_end;
		L.r0 = ram32(L.r4); L.r4 += 4;                   // sep
		sq_write(V, L.r1);                               // PCW de fim
		L.T = (s32)L.r0 > 0;
		sq_flush(V);
		L.r6 = V + 32;
		bool acabou = !L.T;
		if (!acabou)
		{
			c += C.s_end2;
			L.T = (L.r0 & 0x80) == 0;
			L.r13 = maskp;
			acabou = L.T;
		}
		if (acabou)                                      // C7A: rts (T = 0 do cmp/pl ou 1 do tst)
		{
			c += C.ret;
			L.r4 -= 4;                                   // aponta para o sep
			L.r5 += 64;
			sq_flush(L.r6);                              // 2ª metade
			L.r6 += 32;
			cyc.take(c); L.store(s, 0);
			return return_to(s.pr);
		}
		// C4E: novo strip
		c += C.s_new;
		L.r3 = ram32(L.r4); L.r4 += 4;
		L.r1 = L.r6;                                     // V + 32
		L.r5 += 64;
		c += abre_vertice(L, maskp, amb) ? C.s_new_f1 : C.s_new_f0;
		L.r0 = ambp + 16;
		L.r6 = V + 112;
		sq_flush(L.r1);
	}
}

} // namespace stripxf

// Ganchos (bytes conferidos pelo hle_fn_lookup na compilação do bloco):
//   A:  DOA2 8C101930, MvC2 8C12A7F0, CvS2 8C17C3D0, PJ 0C150C10, Shenmue II 8C1D8A90
//       → run_a(s, cyc, pc, false);  Power Stone 0C0E7610 → run_a(s, cyc, pc, true)
//   A': CvS2 8C180024, PJ 0C154962, Shenmue II 8C1DC570 → run_a(s, cyc, pc, false)
//       (K sai do opcode em +6); também alcançadas por run_luz quando r8 < 0
//   B/C: DOA2 8C102A80 / 8C102DE0, CvS2 8C17FDF0 / 8C180150, Shenmue II 8C1DC360 → run_luz
