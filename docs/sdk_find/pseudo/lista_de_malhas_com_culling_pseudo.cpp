// Lista de malhas com culling por esfera e cabeçalhos do TA — pseudo-C++ para os 6 jogos
// do grupo 011 (docs/sdk_find/auto/011_matriz_divisao_pref.md). API: ver
// docs/sdk_blocks/luz_e_transformacao_de_vertices_pseudo.cpp. Semântica bit-exata de
// referência (ftrv, Store Queue, ciclos por bloco, ENTER): core/rec-ARM64/hle_fn.cpp
// (doa2_run). Os laços de vértice que esta função chama por `bsr` são os grupos 005
// (normal; contém o laço 8C101BC4 do DOA2, já nativo) e 033 (com clipping).
//
// Pseudo-código: não compila no core. Ainda não existe versão nativa.
//
// O que faz
//   Percorre a lista de malhas de um modelo. Para cada malha: aplica as máscaras globais
//   ao PCW/TSP; no modo "fade" (fr4 < 1) joga a malha na lista translúcida e liga o alfa
//   no TSP; transforma o centro da esfera envolvente pela XMTRX (ftrv); escolhe o
//   ponteiro de escrita da lista do TA (e, se a lista mudou, o QACR0/1 da Store Queue);
//   testa a esfera contra o plano de fundo e 4 planos laterais; malha recusada → pula o
//   corpo (e, se for intensidade modo 1, ainda manda um cabeçalho falso com a cor de face,
//   para o modo 2 das seguintes não herdar cor errada); malha aceita → guarda
//   PCW/ISP/TSP/TCW num cabeçalho global, manda a cor de face, escolhe a luz pelo byte de
//   modo e, para cada comando de strip, manda o cabeçalho de polígono ao TA e chama o laço
//   de vértices certo (normal ou com clipping × 2 formatos). Volta com rts.
//
// Entrada
//   r4     lista − 24 (o prólogo soma 24): 1ª malha em r4+24
//   r5     parâmetros (36 bytes): +0 fr10, +4 fr8, +8 fr9 (direção da luz; frame+56/48/52),
//          +12 r12 (bits de um float: limite do plano de fundo; o bit 0 é reaproveitado
//          como flag "esfera cruza o fundo"), +16 fr14, +20 fr12, +24 fr13 (× 1/G),
//          +28 → frame+36 (escala da intensidade, multiplica fr15), +32 → r7 (e frame+60)
//   fr4    fator de fade/alfa: entrada em base+2 (DOA2) traz do chamador; entrada em
//          base (fldi1 fr4, todos os outros) usa 1.0
//   XMTRX  matriz de vista; resultado (p0, p1, p2, p3) — p0 multiplica os planos
//          laterais e p3 é comparado com o plano de fundo
//   globais (via literal pool, endereços por jogo abaixo):
//          G      float lido e zerado para 1.0 a cada chamada; |G| = escala do raio,
//                 1/G escala fr12..fr14
//          P8, P12, P16, P20  coeficientes dos planos laterais (frame+8..+20)
//          K0, K1 deslocamento dos planos laterais (centro de projeção) — não existe no PS
//          COL3   3 floats: escala R,G,B da cor de face (frame+24..+32)
//          OFS3   3 floats: escala R,G,B da cor offset
//          OR     2 palavras: OR no PCW (frame+40) e no TSP (frame+44)
//          WPTAB  ponteiro para a tabela de ponteiros de escrita por tipo de lista
//          FADEX  XOR aplicado ao TSP no modo fade (com bytes permutados, ver `fade`)
//          HDR    cabeçalho corrente (16 bytes: PCW, ISP, TSP, TCW) lido pelos laços
//          LIGHTS tabela de luzes, 512 bytes por modo
//          CB     função chamada no modo −3
//
// Malha (H = r4 no topo do laço):
//   H+0 PCW (r8)  H+4 ISP (fr2, bits crus)  H+8 TSP (r1)  H+12 TCW (fr3, bits crus)
//   H+16..+24 centro xyz  H+28 raio  H+32 (não lido)  H+36 modo (s8)  H+40 intensidade
//   H+44..+56 cor de face A,R,G,B  H+60..+72 cor offset A,R,G,B  H+76 tamanho do corpo
//   H+80 corpo: comandos; cmd > 0 = strip (o laço de vértices consome os dados e deixa
//        r4 na próxima palavra), cmd < 0 = PCW da próxima malha (H' = endereço do cmd),
//        cmd = 0 = fim. A malha pulada salta H+80+tamanho e cai direto nessa palavra.
// Modo (H+36): > 0 luz da tabela (r8 = LIGHTS + 512·modo, fr8..fr10 = direção);
//   0 sem luz; −1 fr15 = 1; −3 chama CB (DOA2); −2 não coberto.
// Comando de strip: bits 0-1 → modo de culling do ISP (bits 27-28); bit 3 / bit 4 →
//   escolhe o laço; bit 5 → T na entrada dos laços de clipping; bit 6 → Gouraud (PCW bit 1).
//
// Quadro (r15, 64 bytes; 72 no Shenmue II):
//   +0 fr4  +4 |G|  +8 *P8  +12 *P12  +16 *P16  +20 *P20  +24..+32 COL3  +36 param+28
//   +40 OR[0]  +44 OR[1]  +48 param+4  +52 param+8  +56 param+0  +60 param+32 (não no PS)
//   Shenmue II, cópia alternativa: +64 HELPER, +68 0.0
//
// Saída: cabeçalhos e cores de face na SQ (destino pelo QACR: buffer de lista na RAM ou
// o TA), ponteiros de escrita de volta na tabela, HDR, QACR0/1, G = 1.0.
//
//   Jogo               base (fldi1 fr4)   %perf   observação
//   Capcom vs. SNK 2   8C17BFE0           s/ perf
//   Dead or Alive 2    8C101540           2,05%   chamado em 8C101542 (fr4 do chamador);
//                                                 o fldi1 em 8C101540 não executou
//   Marvel vs. Capcom 2 8C12A400          1,31%
//   Power Stone        0C0E7240           s/ perf
//   Project Justice    0C150820           s/ perf
//   Shenmue II         8C1D8680           1,47%   principal
//                      8C1D97C0           0,14%   cópia alternativa (global 8C260F2C ≠ 0);
//                                                 entra por `jmp` no meio do prólogo
//
// Variantes (comparação opcode a opcode, normalizando deslocamentos de desvio/literal,
// e por offset nos dumps):
//   A  DOA2 = MvC2 (438 de 438 opcodes no mesmo offset, inclusive deslocamentos) =
//      Project Justice (301 de 301; o resto não executou) = CvS2 (425 dos 426 comuns; o
//      diferente é o deslocamento do 4º `bsr`, em +0x390: o laço V3 fica em base+0xBD0,
//      não +0xBE0 — por isso os laços saem do opcode, não de offset fixo).
//      Só cobertura: o CvS2 não rodou o modo > 0 nem o laço V1; o PJ não rodou fade,
//      cabeçalho falso, modos < 0, clipping nem luz.
//   B  Shenmue II principal = A + 7 instruções em +0x060 (lê o global FLAG; ≠ 0 → `jmp`
//      para a cópia alternativa com r0 = 0x48) e quadro de 72 bytes; resto = A + 0x14.
//   C  Shenmue II alternativa = o resto do prólogo e o corpo de A, com literal pool e
//      laços próprios (8C1D9B80, 8C1DA5E0, 8C1D9F20, 8C1DA3E0); antes grava
//      frame+68 = 0.0 e frame+64 = HELPER (8C1D9B04: rotina de luz que os laços dela
//      chamam). Sem o nop de alinhamento depois do prólogo e com um literal no meio:
//      offsets = A − 0x58 até +0x15A, A − 0x54 depois.
//   D  Power Stone (SDK mais antigo): (a) não grava r7 em frame+60 e não o recarrega
//      antes dos laços de clipping; (b) planos laterais sem o termo K0·p0/K1·p0 e com o
//      TSP gravado em HDR+8 antes do 1º teste (no A, só depois de passar nele); (c) o
//      trecho dos modos −2/−3 tem 4 instruções a menos (nunca executou → bail); literal
//      pool em outro offset (ver tabela).
//   Valores do literal pool são endereços diferentes em cada jogo (lidos da RAM ao plugar;
//   tabela no fim do cabeçalho).
//
// Não determinado (nunca executou em nenhum dump):
//   - modo −2: 9 instruções em A+0x266 (DOA2 8C1017A6..8C1017B6); no PS, todo modo < 0
//     diferente de −1 (PS+0x250 em diante). → bail.
//   - máscara OR[0] do PCW negativa: 1 instrução em A+0x2FC (DOA2 8C10183C), PS+0x2E0.
//     → bail.
//   - o contrato do CB (modo −3): volta com T = fim da lista e, senão, r4/r8 já na
//     próxima malha (deduzido do código depois do jsr; o CB não foi lido).
//   - qual pista de p é profundidade (p3 vs. o plano de fundo; p0 nos laterais — o laço
//     8C101BC4 divide por p0); os nomes "fundo/laterais" são pela forma dos testes.
//   - CB/LIGHTS da cópia alternativa do Shenmue II: offsets deduzidos (A − 0x54), não vistos.
//   - os 16 bytes da janela depois do TCW (cabeçalho de 32 bytes) e +16..+31 do cabeçalho
//     falso não são escritos: vão os bytes antigos da SQ (o TA ignora).
//
// Rótulos (offset do bloco do JIT a partir da base) e ciclos (campo do dump):
//   rótulo     A    (DOA2)    ciclos | B      | C      | D     ciclos D
//   BT_FADE    0A8  8C1015E8   1     | 0BC    | 052    | 0A6
//   LOOP       0AC  8C1015EC   9     | 0C0    | 054    | 0A8
//   FADE       0C4  8C101604  32     | 0D8    | 06C    | 0C0
//   BSPH       0EA  8C10162A  13     | 0FE    | 092    | 0E6
//   QACR       112  8C101652  19     | 126    | 0BA    | 10E
//   SAME       124  8C101664  10     | 138    | 0CC    | 120
//   FAR2       142  8C101682   2     | 156    | 0EA    | 13E
//   SKIP       148  8C101688   9     | 15C    | 0F0    | 144
//   SKIP_END   15A  8C10169A   2     | 16E    | 102    | 156
//   SKIP_HDR   160  8C1016A0   7     | 174    | 10C    | 15C
//   SKIP_C0    174  8C1016B4   1     | 188    | 120    | 170
//   SKIP_M1    176  8C1016B6  14     | 18A    | 122    | 172
//   NEXT_HDR   1BE  8C1016FE   2     | 1D2    | 16A    | 1BA
//   CULL1      1C8  8C101708   5     | 1DC    | 174    | 1C4   4
//   CULL2      1EA  8C10172A   2     | 1FE    | 196    | 1DE   1
//   CULL3      1FA  8C10173A   1     | 20E    | 1A6    | 1E8   1
//   CULL4      204  8C101744   3     | 218    | 1B0    | 1F0   3
//   DRAW       210  8C101750   7     | 224    | 1BC    | 1FC
//   FACE       222  8C101762   5     | 236    | 1CE    | 20E
//   MODE       258  8C101798   2     | 26C    | 204    | 244
//   NEG        25C  8C10179C   4     | 270    | 208    | 248
//   NEG_M2     264  8C1017A4   1     | 278    | 210*   | —
//   CB_PREP    278  8C1017B8  15     | 28C    | 224*   | —
//   CB_CALL    282  8C1017C2   1     | 296    | 22E*   | —
//   CB_RET     288  8C1017C8   1     | 29C    | 234*   | —
//   CB_NEXT    28A  8C1017CA   2     | 29E    | 236*   | —
//   UNIT       294  8C1017D4   1     | 2A8    | 240*   | 278
//   POS        298  8C1017D8   3     | 2AC    | 244    | 27C
//   LIGHT      29E  8C1017DE  12     | 2B2    | 24A*   | 282
//   SCALE      2B0  8C1017F0   6     | 2C4    | 25C    | 294
//   CMD0       2B6  8C1017F6   5     | 2CA    | 262    | 29A
//   CMD        2B8  8C1017F8   4     | 2CC    | 264    | 29C
//   CMD_END    2C0  8C101800   4     | 2D4    | 26C    | 2A4
//   CMD_NEXT   2C8  8C101808   2     | 2DC    | 274    | 2AC
//   STRIP      2D0  8C101810  12     | 2E4    | 27C    | 2B4
//   STRIP_M2   2E8  8C101828  10     | 2FC    | 294    | 2CC
//   STRIP_HDR  2EE  8C10182E   7     | 302    | 29A    | 2D2
//   STRIP_TX   2FE  8C10183E  16     | 312    | 2AA    | 2E2
//   FACE_TX    32C  8C10186C   7     | 340    | 2D8    | 310
//   ROT        332  8C101872   4     | 346    | 2DE    | 316
//   NRM        33A  8C10187A   2     | 34E    | 2E6    | 31E
//   CALL_V0    33E  8C10187E   2     | 352    | 2EA    | 322
//   RET_V0     342  8C101882   2     | 356    | 2EE    | 326
//   CALL_V1    350  8C101890   2     | 364    | 2FC    | 334
//   RET_V1     354  8C101894   2     | 368    | 300    | 338
//   EPI        358  8C101898  11     | 36C    | 304    | 33C
//   CLIP       378  8C1018B8   7     | 38C    | 324    | 35C   6
//   CALL_V2    386  8C1018C6   2     | 39A    | 332    | 368
//   RET_V2     38A  8C1018CA   2     | 39E    | 336    | 36C
//   CALL_V3    390  8C1018D0   2     | 3A4    | 33C    | 370
//   RET_V3     394  8C1018D4   2     | 3A8    | 340    | 374
//   U_NEG2     266  8C1017A6   bail  | 27A    | 212*   | 250 (todo modo < 0 ≠ −1)
//   U_ORMASK   2FC  8C10183C   bail  | 310    | 2A8    | 2E0
//   (* = deduzido, o bloco não executou nessa cópia.) Ciclos de B, C e D iguais aos de A,
//   salvo a coluna D e os prólogos: A 55 (base e base+2); D 54; B 33 até o teste do FLAG,
//   +0x068 `jmp` 3, +0x074 resto 26; C 29 (bloco de entrada da alternativa).
//
// Instruções `mov.l @(disp,PC)` (literal) e `bsr` (laços) — offset a partir da base:
//   nome   A    B    C    D   | DOA2      MvC2      CvS2      PJ        PS        SH2
//   G      004  004  —    004 | 8C2F4E28  8C32BC48  8C3AFFA8  0C33BA48  0C7FD648  8C2C1D88
//   P8     032  032  —    032 | 8C2F06F0  8C2D7398  8C39C058  0C333720  0C7FCF68  8C311868
//   P16    038  038  —    038 | 8C1CA8D4  8C16C5B4  8C1EAC40  0C2BD5E8  0C346AC4  8C2610F8
//   P20    03E  03E  —    03E | 8C1CA8D8  8C16C5B8  8C1EAC44  0C2BD5EC  0C346AC8  8C2610FC
//   P12    040  040  —    040 | 8C2F06F4  8C2D739C  8C39C05C  0C333724  0C7FCF6C  8C3118F0
//   FLAG   —    060  —    —   |                                                  8C260F2C
//   ALT    —    068  —    —   |                                                  8C1D97C0
//   HELPER —    —    004  —   |                                                  8C1D9B04
//   COL3   074  088  01E  072 | 8C1CA920  8C16C680  8C1EAD08  0C2BD6B0  0C3468B0  8C260EF0
//   OR     082  096  02C  080 | 8C2EF7E0  8C2DF68C  8C3AFF70  0C332814  0C7FD4C8  8C310944
//   WPTAB  08C  0A0  036  08A | 8C2B6F34  8C2AAD08  8C36BBBC  0C307068  0C803DF4  8C31C034
//   FADEX  0CC  0E0  074  0C8 | 8C1CA5D8  8C2AACC4  8C36BB78  ?         0C3468E8  8C30C8BC
//   HDR    13C  150  0E4  138 | 8C2F0800  8C2DF6A0  8C3B1000  0C33CAA0  0C7FD240  8C2C1C20
//   OFS3   180  194  12C  17C | 8C1CA930  8C16C690  8C1EAD18  0C2BD6C0  0C3468C0  8C260F00
//   K0     1CA  1DE  176  —   | 8C1CA8DC  8C16C5BC  8C1EAC48  0C2BD5F0  —         8C261100
//   K1     1DE  1F2  18A  —   | 8C1CA8E0  8C16C5C0  8C1EAC4C  0C2BD5F4  —         8C261104
//   CB     27E  292  22A* ?   | 8C109A80  8C1302A0  8C188FA0  ?         ?         8C1DB3E0
//   LIGHTS 2AC  2C0  258* 290 | 8C2F046C  8C2D7024  ?         ?         0C7F42AC  8C307F24
//   V0     33E  352  2EA  322 | 8C101920  8C12A7E0  8C17C3C0  0C150C00  0C0E7600  8C1D8A80
//   V1     350  364  2FC  334 | 8C102320  8C12B1E0  ?         ?         0C0E8000  8C1D9480
//   V2     386  39A  332  368 | 8C101C80  8C12AB40  8C17C720  ?         0C0E7960  8C1D8DE0
//   V3     390  3A4  33C  370 | 8C102120  8C12AFE0  8C17CBB0  ?         0C0E7E00  8C1D9280
//   (? = a instrução não executou; o valor sai da RAM ao plugar. A alternativa C do SH2
//   usa os mesmos globais da principal; laços: 8C1D9B80, 8C1DA5E0, 8C1D9F20, 8C1DA3E0.)
//   V0 = normal, cmd bit 3 = 0; V1 = normal, bit 3 = 1; V2 = clipping, bit 4 = 1;
//   V3 = clipping, bit 4 = 0.

#pragma GCC optimize("fp-contract=off")   // só a fusão que o JIT faz (ftrv)
#include <arm_neon.h>

namespace meshlist {

enum Lbl { BT_FADE, LOOP, FADE, BSPH, QACR, SAME, FAR2, SKIP, SKIP_END, SKIP_HDR, SKIP_C0,
           SKIP_M1, NEXT_HDR, CULL1, CULL2, CULL3, CULL4, DRAW, FACE, MODE, NEG, NEG_M2,
           CB_PREP, CB_CALL, CB_RET, CB_NEXT, UNIT, POS, LIGHT, SCALE, CMD0, CMD, CMD_END,
           CMD_NEXT, STRIP, STRIP_M2, STRIP_HDR, STRIP_TX, FACE_TX, ROT, NRM, CALL_V0,
           RET_V0, CALL_V1, RET_V1, EPI, CLIP, CALL_V2, RET_V2, CALL_V3, RET_V3,
           U_NEG2, U_ORMASK, N_LBL };

struct Layout { u16 off[N_LBL]; u8 cyc[N_LBL]; };   // colunas A/B/C/D da tabela acima

// Uma cópia da função num jogo. Os endereços saem da RAM ao plugar: cada `mov.l
// @(disp,PC)` dá ((pc & ~3) + 4 + disp·4) e cada `bsr` dá pc + 4 + disp·2.
struct Inst {
	u32 base;
	const Layout *L;
	bool ps;              // variante D
	u32 frame;            // 64; 72 no Shenmue II
	const Inst *alt;      // B → C (Shenmue II); nullptr nos outros
	u32 g0, p8, p12, p16, p20, col3, orm, wptab, fadex, hdr, k0, k1, ofs3, cb, lights;
	u32 flag, helper;     // só B / C
	u32 v[4];             // laços de vértice
	u32 pc(Lbl l) const { return base + L->off[l]; }
	s32 cy(Lbl l) const { return L->cyc[l]; }   // escalado pelo clock como no doa2_run
};

enum Entry { ENT_BASE, ENT_FR4, ENT_RET_CB, ENT_RET_V0, ENT_RET_V1, ENT_RET_V2, ENT_RET_V3 };

static inline void ftrv(const float32x4_t m[4], float &a, float &b, float &c, float &d)
{
	float32x4_t acc = vmulq_n_f32(m[0], a);
	acc = vfmaq_n_f32(acc, m[1], b);
	acc = vfmaq_n_f32(acc, m[2], c);
	acc = vfmaq_n_f32(acc, m[3], d);
	a = vgetq_lane_f32(acc, 0); b = vgetq_lane_f32(acc, 1);
	c = vgetq_lane_f32(acc, 2); d = vgetq_lane_f32(acc, 3);
}
static inline void sq_pair(u32 a, float lo, float hi) { sq_write(a, f2u(lo)); sq_write(a + 4, f2u(hi)); }
static inline u32 swapw(u32 x) { return (x >> 16) | (x << 16); }                       // swap.w
static inline u32 swapb(u32 x) { return (x & 0xFFFF0000) | ((x & 0xFF) << 8) | ((x >> 8) & 0xFF); } // swap.b

// Locais r0..r15, f0..f15, fpul, T, pr: LOAD(s) no começo, SAVE(s) antes de toda saída.
// SZ fica 0 em todas as fronteiras de bloco (os fschg desta função vêm em pares dentro do
// mesmo bloco). ENTER(l) = entrada no bloco `l` igual à do doa2_run (desconta os ciclos;
// fatia acabou → SAVE, UpdateSystem nesta fronteira, LOAD; interrupção → JIT em pc(l)).
#define ENTER(l)    do { if (!cyc.enter(s, F->pc(l), F->cy(l))) return; } while (0)
#define BAIL(l)     do { SAVE(s); return bail(F->pc(l)); } while (0)
#define ENTER_AT(pc, n) do { if (!cyc.enter(s, (pc), (n))) return; } while (0)
#define CALL(fn, l) do { pr = F->pc(l); SAVE(s); return jump(fn); } while (0)   // bsr/jsr
#define PUSH(v)     do { r15 -= 4; wr32(r15, (v)); } while (0)                 // r15 local
#define POP()       (r15 += 4, ram32(r15 - 4))

// cor de face e offset da malha (H+44..H+75), como em 8C1016B6 e 8C101762: face × (fade,
// COL3), offset A cru e RGB × OFS3. Sai com r2 = H+76, r0 = OFS3+12, f11 = OFS3[2].
#define FACE_COLORS() do { \
	f0 = ramf(r2); f1 = ramf(r2 + 4); f2 = ramf(r2 + 8); f3 = ramf(r2 + 12); r2 += 16; \
	f0 = f0 * f11; r0 = F->ofs3; f11 = ramf(r0); r0 += 4; \
	f1 = f1 * f5;  f4 = ramf(r2); f5 = ramf(r2 + 4); r2 += 8; f5 = f5 * f11; \
	f11 = ramf(r0); r0 += 4; \
	f2 = f2 * f6;  f6 = ramf(r2); r2 += 4; f6 = f6 * f11; \
	f11 = ramf(r0); r0 += 4; \
	f3 = f3 * f7;  f7 = ramf(r2); r2 += 4; f7 = f7 * f11; \
} while (0)

// `inst` = a cópia do jogo (principal; a alternativa do Shenmue II é trocada por dentro);
// `e` = qual bloco o JIT casou: a entrada da função ou a volta de um laço / do CB (para a
// alternativa do Shenmue II as voltas casam com os blocos dela e passam a Inst dela). O
// bloco de entrada já foi descontado pelo JIT ao chamar (como no doa2_run). Os laços de
// vértice e o CB rodam fora daqui (no JIT ou no nativo deles): CALL grava pr e salta.
void run(Sh4 &s, Cycles &cyc, const Inst &inst, Entry e)
{
	const Inst *F = &inst;
	if (s.fpscr.PR || s.fpscr.SZ)
		return bail(s.pc);
	if (!is_ram(s.r[15]) || !is_ram(s.r[4]) || (e <= ENT_FR4 && !is_ram(s.r[5])))
		return bail(s.pc);
	LOAD(s);
	const float32x4_t m[4] = { vld1q_f32(&s.xf[0]), vld1q_f32(&s.xf[4]),
	                           vld1q_f32(&s.xf[8]), vld1q_f32(&s.xf[12]) };

	switch (e) {
	case ENT_BASE: f4 = 1.f; break;                  // fldi1 fr4
	case ENT_FR4: break;                             // DOA2: fr4 do chamador
	case ENT_RET_CB: goto cb_ret;
	case ENT_RET_V0: case ENT_RET_V1: case ENT_RET_V2: case ENT_RET_V3:
		r0 = ram32(r4); r4 += 4;                     // bra 8C1017F8; slot mov.l @r4+,r0
		goto cmd;
	}

	// ---- prólogo -------------------------------------------------------------------
	r4 += 24;
	r7 = F->g0;
	f1 = 1.f;
	PUSH(f2u(f15)); f0 = ramf(r7); PUSH(f2u(f14)); PUSH(f2u(f13));
	f1 = f1 / f0;                                    // fdiv: 1/G
	PUSH(f2u(f12));
	PUSH(r14); PUSH(r13); PUSH(r12); PUSH(r11); PUSH(r10); PUSH(r9); PUSH(r8);
	r0 = 0;
	PUSH(pr);
	r15 -= F->frame;
	f7 = 1.f;
	wrf(r15 + 0, f4);
	r0 = 4;
	f0 = fabsf(f0); wrf(r15 + 4, f0);
	r0 = F->p8; wrf(r7, f7);                         // G = 1.0
	r1 = ram32(r0); r2 = F->p16; wr32(r15 + 8, r1);
	r1 = ram32(r2); r0 = F->p20; r2 = F->p12; wr32(r15 + 16, r1);
	r3 = ram32(r2); r1 = ram32(r0); wr32(r15 + 12, r3);
	f10 = ramf(r5); r5 += 4; wr32(r15 + 20, r1);
	f8 = ramf(r5); r5 += 4; f9 = ramf(r5); r5 += 4; r12 = ram32(r5); r5 += 4;
	r0 = 0x38; wrf(r15 + 0x38, f10); r0 = 0x30; wrf(r15 + 0x30, f8);
	r0 = 0x34; wrf(r15 + 0x34, f9);

	if (F->alt) {                                    // B: Shenmue II
		r0 = ram32(F->flag);
		T = r0 == 0;
		if (!T) {
			ENTER_AT(F->base + 0x068, 3);            // bloco do jmp
			r1 = F->alt->base; r0 = 0x48;            // jmp @r1; slot mov #0x48,r0
			F = F->alt;                              // daqui em diante: rótulos/literais da C
			ENTER_AT(F->base, 29);                   // bloco de entrada da alternativa
			r0 += r15;
			f12 = 0.f;
			r1 = F->helper;
			r0 -= 4; wrf(r0, f12);                   // frame+68 = 0.0
			r0 -= 4; wr32(r0, r1);                   // frame+64 = HELPER
		} else
			ENTER_AT(F->base + 0x074, 26);           // resto do prólogo
	}
	f14 = ramf(r5); r5 += 4; f12 = ramf(r5); r5 += 4; f13 = ramf(r5); r5 += 4;
	f12 = f12 * f1;
	r0 = ram32(r5); r5 += 4;
	f13 = f13 * f1;
	wr32(r15 + 36, r0);
	r7 = ram32(r5); r5 += 4;
	f14 = f14 * f1;
	if (!F->ps)
		wr32(r15 + 60, r7);
	r0 = F->col3;
	r1 = ram32(r0); r2 = ram32(r0 + 4); r3 = ram32(r0 + 8); r0 += 12;
	wr32(r15 + 24, r1); wr32(r15 + 28, r2); wr32(r15 + 32, r3);
	r0 = F->orm;
	r5 = ram32(r0); r0 += 4; wr32(r15 + 40, r5);
	r6 = ram32(r0); r0 += 4; wr32(r15 + 44, r6);
	r10 = F->wptab;
	r3 = r15;
	r8 = ram32(r4); r4 += 4;                         // PCW da 1ª malha
	r3 += 4;
	r10 = ram32(r10);                                // tabela de ponteiros de escrita
	r11 = 0xFFFFFFFF;                                // tipo de lista anterior: nenhum
	f11 = f4;
	r8 |= r5;
	T = 1.f > f11;                                   // fade?
	f2 = ramf(r4); r4 += 4;
	r1 = ram32(r4); r4 += 4;
	r1 |= r6;
	f3 = ramf(r4); r4 += 4;                          // slot do bf.s
	if (!T)
		goto bsph;
	ENTER(BT_FADE);                                  // bt 8C101604 (T = 1 sempre aqui)
	goto fade;

	// ---- topo do laço: próxima malha (r8 = PCW, r1 = TSP, f2 = ISP, f3 = TCW) --------
loop:	ENTER(LOOP);
	r3 = ram32(r15 + 40); r0 = ram32(r15 + 44);
	r8 |= r3;
	r3 = r15;
	f11 = ramf(r3); r3 += 4;                         // fr4 do chamador
	r1 |= r0;
	f7 = 1.f;
	T = f7 > f11;
	r0 = r4 + 48;                                    // slot: pref @r0 (RAM → sem efeito)
	if (!T)
		goto bsph;

fade:	ENTER(FADE);                                 // malha vai para a lista translúcida
	r2 = 0x02000000;
	r0 = r1;
	r8 |= r2;                                        // lista 0/1/4 → 2/3
	r0 |= 0xC0;
	r0 = swapw(r0);
	r9 = ram32(F->fadex);
	r0 |= 0x10;
	r0 = swapb(r0);
	r0 |= 0xFC;
	r0 ^= r9;                                        // XOR em ordem de bytes permutada
	r0 = swapb(r0);
	r0 = swapw(r0);
	r2 += r2;
	r1 = r0;                                         // TSP: instr. de textura 3, alfa, src/dst
	r2 = ~r2;
	r8 &= r2;                                        // limpa o bit 26 do PCW
	goto bsph_body;                                  // o mesmo bloco segue no 8C10162A

bsph:	ENTER(BSPH);
bsph_body:                                           // esfera envolvente
	r0 = r8;
	f4 = ramf(r4); r4 += 4;
	r0 >>= 16;
	f5 = ramf(r4); r4 += 4;
	r0 >>= 8;
	f6 = ramf(r4); r4 += 4;
	r0 &= 7;                                         // tipo de lista (PCW bits 24-26)
	r2 = 4;
	r0 <<= 2;
	ftrv(m, f4, f5, f6, f7);                         // p = M·(centro, 1)
	f1 = ramf(r3); r3 += 4;                          // |G|
	r2 <<= 8;
	f0 = ramf(r4); r4 += 4;                          // raio
	T = r11 == r0;
	r11 = r0;
	r6 = 0xFFFFFFE0;
	f0 = f0 * f1;                                    // r' = raio·|G|
	r2 <<= 16;
	r5 = ram32(r10 + r0);                            // slot: ponteiro de escrita da lista
	if (T)
		goto same;
	ENTER(QACR);                                     // a lista mudou: aponta a SQ para ela
	r0 = 0xFF000038;
	r9 = r5 >> 24;
	mmio_write32(r0, r9);                            // QACR0 (handler real: troca o
	mmio_write32(r0 + 4, r9);                        // QACR1  destino do sq_flush)
	goto same_body;

same:	ENTER(SAME);
same_body:
	r2 -= 1;                                         // 0x03FFFFFF
	fpul = r12;
	r0 = 0xFFFFFFFE;
	f7 = f7 + f0;                                    // p3 + r'
	r2 &= r5;
	f1 = u2f(fpul);                                  // limite do fundo
	r6 <<= 8;
	T = f7 > f1;
	r6 <<= 16;
	r6 |= r2;                                        // janela na SQ = 0xE0000000 | (r5 & 0x03FFFFFF)
	f7 = f7 - f0;
	r12 &= r0;                                       // flag "cruza o fundo" = 0
	r9 = F->hdr;
	f7 = f7 - f0;                                    // slot: p3 − r' (pela soma, não exato)
	if (!T)
		goto cull;                                   // esfera toda antes do fundo
	ENTER(FAR2);
	{
		const bool beyond = f7 > f1;
		r12 += 1;                                    // slot: flag "cruza o fundo" = 1
		if (!beyond)
			goto cull;
	}
	// esfera toda além do fundo: cai em SKIP

skip:	ENTER(SKIP);                                 // malha recusada: pula o corpo
	r2 = r4;                                         // H+32
	r4 += 44;
	r0 = ram32(r4); r4 += 4;                         // tamanho do corpo (H+76)
	sq_write(r6, r8);                                // PCW na janela (sem pref aqui)
	r1 = r8;
	r4 += r0;
	r8 = ram32(r4); r4 += 4;                         // PCW da próxima malha ou 0
	T = r8 == 0;
	if (T) {
		ENTER(SKIP_END);
		goto epi;
	}
	ENTER(SKIP_HDR);
	r9 = r15 + 24;
	f5 = ramf(r9); f6 = ramf(r9 + 4); r9 += 8;       // COL3 R, G
	r2 += 12;                                        // H+44
	f7 = ramf(r9); r9 += 4;                          // COL3 B
	r0 = r1;
	{
		const bool odd = r0 & 16;
		T = !(r0 & 32);                              // slot
		if (odd)
			goto next_hdr;                           // Col_Type 1/3
	}
	ENTER(SKIP_C0);
	if (T)
		goto next_hdr;                               // Col_Type 0
	ENTER(SKIP_M1);                                  // Col_Type 2: cabeçalho falso + cor
	FACE_COLORS();
	r0 = 0;
	sq_write(r6 + 4, 0); sq_write(r6 + 8, 0); sq_write(r6 + 12, 0);   // ISP/TSP/TCW = 0
	sq_flush(r6);
	r6 += 64;
	r6 -= 8; sq_pair(r6, f6, f7);
	r6 -= 8; sq_pair(r6, f4, f5);
	r0 = r11;
	r6 -= 8; sq_pair(r6, f2, f3);
	r5 += 64;
	r6 -= 8; sq_pair(r6, f0, f1);
	sq_flush(r6);                                    // cor de face/offset
	r6 += 32;
	wr32(r10 + r0, r5);
	goto next_hdr_body;

next_hdr: ENTER(NEXT_HDR);
next_hdr_body:
	f2 = ramf(r4); r1 = ram32(r4 + 4); f3 = ramf(r4 + 8); r4 += 12;
	goto loop;

	// ---- planos laterais; grava o cabeçalho global à medida que os testes passam ----
cull:	ENTER(CULL1);
	if (!F->ps) {                                    // A, B, C
		f1 = ramf(r3); r3 += 4;                      // *P8
		r0 = F->k0;
		r9 += 16;
		f7 = ramf(r0);
		f7 = f7 * f4;                                // K0·p0
		f0 = -f0;
		f1 = f1 * f0;                                // a = P8·(−r')
		r9 -= 4; wrf(r9, f3);                        // HDR+12 = TCW
		f5 = f5 - f7;                                // p1 − K0·p0
		f3 = ramf(r3); r3 += 4;                      // *P12
		r2 = r4;
		r0 = F->k1;
		T = f5 > f1;
		f7 = ramf(r0);
		f3 = f3 * f0;                                // b = P12·(−r')
		f7 = f7 * f4;                                // slot: K1·p0
		if (!T)
			goto skip;
		ENTER(CULL2);
		r9 -= 4; wr32(r9, r1);                       // HDR+8 = TSP
		f6 = f6 - f7;                                // p2 − K1·p0
		f0 = ramf(r3); r3 += 4;                      // *P16
		T = f6 > f3;
		f7 = ramf(r3); r3 += 4;                      // *P20
		f0 = f0 * f4;
		f0 = f0 - f1;                                // slot: c = P16·p0 − a
		if (!T)
			goto skip;
		ENTER(CULL3);
		T = f5 > f0;
		f5 = ramf(r3); r3 += 4;                      // COL3 R
		f7 = f7 * f4;
		r9 -= 4; wrf(r9, f2);                        // slot: HDR+4 = ISP
		if (T)
			goto skip;
	} else {                                         // D: Power Stone, planos simétricos
		f1 = ramf(r3); r3 += 4;
		r9 += 16;
		f0 = -f0;
		f1 = f1 * f0;                                // a
		r9 -= 4; wrf(r9, f3);                        // HDR+12 = TCW
		r2 = r4;
		f3 = ramf(r3); r3 += 4;
		const bool t1 = f5 > f1;                     // p1 > a
		r9 -= 4; wr32(r9, r1);                       // HDR+8 = TSP (antes do 1º teste)
		f3 = f3 * f0;                                // b
		f0 = ramf(r3); r3 += 4;
		T = f6 > f3;                                 // slot: p2 > b
		if (!t1)
			goto skip;
		ENTER(CULL2);
		f7 = ramf(r3); r3 += 4;
		f0 = f0 * f4;
		f0 = f0 - f1;                                // c
		{
			const bool t2 = T;
			T = f5 > f0;                             // slot
			if (!t2)
				goto skip;
		}
		ENTER(CULL3);
		f5 = ramf(r3); r3 += 4;
		f7 = f7 * f4;
		r9 -= 4; wrf(r9, f2);                        // slot: HDR+4 = ISP
		if (T)
			goto skip;
	}
	ENTER(CULL4);
	f7 = f7 - f3;                                    // d = P20·p0 − b
	r9 -= 4; wr32(r9, r8);                           // HDR+0 = PCW
	r1 = 8;
	T = f6 > f7;
	f6 = ramf(r3); r3 += 4;                          // COL3 G
	if (T)
		goto skip;

	// ---- malha aceita ---------------------------------------------------------------
	ENTER(DRAW);
	r0 = r8;
	T = !(r0 & 16);
	r0 = (u32)(s32)(s8)ram8(r2 + 4); r2 += 8;        // modo (H+36)
	f7 = ramf(r3); r3 += 4;                          // COL3 B
	r8 = r0;
	f15 = ramf(r2); r2 += 4;                         // intensidade (H+40)
	r4 += 48;                                        // slot: r4 = H+80 (corpo)
	if (!T)
		goto mode;                                   // Col_Type 1/3: sem cor de face
	ENTER(FACE);
	FACE_COLORS();
	r6 += 64;
	r6 -= 8; sq_pair(r6, f6, f7);                    // janela+32..+63: só preenche;
	r6 -= 8; sq_pair(r6, f4, f5);                    // o pref vem no 1º strip
	r6 -= 8; sq_pair(r6, f2, f3);
	r6 -= 8; sq_pair(r6, f0, f1);
	r6 -= 32;
	goto mode_body;

mode:	ENTER(MODE);
mode_body:
	if ((s32)r8 >= 0)
		goto pos;
	ENTER(NEG);
	r0 = r8;
	{
		const bool m1 = r0 == 0xFFFFFFFF;
		T = r0 == 0xFFFFFFFE;                        // slot
		if (m1)
			goto unit;
	}
	if (F->ps)
		BAIL(U_NEG2);                                // D: −2/−3 não cobertos
	ENTER(NEG_M2);
	if (T)
		BAIL(U_NEG2);                                // −2: não coberto
	ENTER(CB_PREP);
	r7 = ram32(r15 + 60);
	T = r0 == 0xFFFFFFFD;
	r0 = 36;
	r1 = F->cb;
	if (!T)
		goto unit;
	ENTER(CB_CALL);                                  // −3: função do jogo
	f1 = ramf(r15 + r0);
	f15 = f15 * f1;                                  // slot do jsr
	CALL(r1, CB_RET);
cb_ret:                                              // (entrada ENT_RET_CB: bloco já descontado)
	if (T) {
		ENTER(EPI);
		goto epi_body;
	}
	ENTER(CB_NEXT);
	f2 = ramf(r4); r1 = ram32(r4 + 4); f3 = ramf(r4 + 8); r4 += 12;
	goto loop;

unit:	ENTER(UNIT);
	f15 = 1.f;                                       // slot do bra
	goto cmd0;

pos:	ENTER(POS);
	T = (s32)r8 > 0;
	r0 = 0x38;
	if (!T)
		goto scale;
	ENTER(LIGHT);
	f10 = ramf(r15 + 0x38); r0 = 0x30; f8 = ramf(r15 + 0x30);
	r0 = 0x34; f9 = ramf(r15 + 0x34);
	r0 = 9;
	r8 <<= 9;
	r0 = F->lights;
	r8 += r0;                                        // luz = LIGHTS + 512·modo
	goto scale_body;
scale:	ENTER(SCALE);
scale_body:
	r0 = 0x24;
	f1 = ramf(r15 + 0x24);
	f15 = f15 * f1;                                  // intensidade × param+28
	goto cmd0_body;

cmd0:	ENTER(CMD0);
cmd0_body:
	r0 = ram32(r4); r4 += 4;
	goto cmd_body;

	// ---- comandos do corpo ------------------------------------------------------------
cmd:	ENTER(CMD);                                  // volta dos laços
cmd_body:
	{
		const bool strip_cmd = (s32)r0 > 0;
		r9 = F->hdr;
		T = (s32)r0 >= 0;                            // slot
		if (strip_cmd)
			goto strip;
	}
	ENTER(CMD_END);
	r8 = r0;
	r0 = r11;
	wr32(r10 + r0, r5);                              // devolve o ponteiro de escrita
	if (T) {                                         // 0: fim da lista
		ENTER(EPI);
		goto epi_body;
	}
	ENTER(CMD_NEXT);                                 // < 0: PCW da próxima malha
	f2 = ramf(r4); r1 = ram32(r4 + 4); f3 = ramf(r4 + 8); r4 += 12;
	goto loop;

strip:	ENTER(STRIP);
	r1 = r0;
	r2 = 0xFFFFFFFB;
	r3 = ram32(r9); r9 += 4;                         // PCW do HDR
	r0 &= 64;
	r0 >>= 5;                                        // shld −5: bit 6 → bit 1
	r2 = 0xFFFFFFFD;
	r3 &= r2;
	r0 |= r3;                                        // Gouraud = bit 6 do comando
	T = !(r0 & 16);
	r4 += 32;
	fpul = r0;                                       // slot
	if (!T)
		goto strip_hdr;
	ENTER(STRIP_M2);                                 // 1º strip em modo 1: os próximos
	r0 |= 16;                                        // usam o modo 2 (cor já enviada)
	r2 = r9 - 4;
	wr32(r2, r0);                                    // HDR+0 = PCW | 16
	goto strip_hdr_body;
strip_hdr: ENTER(STRIP_HDR);
strip_hdr_body:
	r2 = ram32(r15 + 40);
	r3 = T;                                          // movt: 1 = este é o 1º strip
	r0 = r1;
	T = (s32)r2 >= 0;
	r2 = 27;
	r0 &= 3;                                         // slot
	if (!T)
		BAIL(U_ORMASK);                              // OR[0] negativo: não coberto
	ENTER(STRIP_TX);
	r0 <<= 27;                                       // modo de culling → ISP bits 27-28
	T = (s32)r3 > 0;
	r3 = 3u << 27;
	/* pref @r4 (r4 = cmd+36): RAM → sem efeito */
	r3 = ~r3;
	r2 = ram32(r9); r9 += 4;                         // ISP do HDR
	r2 &= r3;
	f0 = u2f(fpul);
	r2 |= r0;
	fpul = r2;
	r4 -= 32;                                        // r4 = cmd+4: dados do strip
	f1 = u2f(fpul);
	r6 += 16;
	f2 = ramf(r9); f3 = ramf(r9 + 4); r9 += 8;       // TSP, TCW (par, SZ=1)
	r6 -= 8; sq_pair(r6, f2, f3);
	r5 += 32;
	r6 -= 8; sq_pair(r6, f0, f1);                    // PCW, ISP
	sq_flush(r6);                                    // cabeçalho de polígono
	r6 += 32;                                        // slot do bf.s
	if (!T)
		goto rot;
	ENTER(FACE_TX);
	sq_flush(r6);                                    // 1º strip: manda a cor de face
	r6 += 32;
	r5 += 32;
	goto rot_body;
rot:	ENTER(ROT);
rot_body:
	{
		const bool clip = r12 & 1;                   // rotcr r12: T ← bit 0
		r12 = (r12 >> 1) | ((u32)T << 31);
		r0 = r1;
		T = r12 >> 31;                               // slot: shll r12 (bit 0 fica 0)
		r12 <<= 1;
		if (clip)
			goto clip;
	}
	ENTER(NRM);
	T = !(r0 & 8);
	if (!T) {
		ENTER(CALL_V1);
		CALL(F->v[1], RET_V1);
	}
	ENTER(CALL_V0);
	CALL(F->v[0], RET_V0);

clip:	ENTER(CLIP);
	r1 = 0;
	T = !(r0 & 64);
	{
		const u64 t = (u64)r12 + r1 + T;             // addc: bit 0 = (bit 6 do cmd == 0)
		r12 = (u32)t;
		T = t >> 32;
	}
	if (!F->ps)
		r7 = ram32(r15 + 60);
	{
		const bool c16 = !(r0 & 16);
		T = !(r0 & 8);                               // slot
		if (c16) {
			ENTER(CALL_V3);
			T = !(r0 & 32);                          // slot do bsr
			CALL(F->v[3], RET_V3);
		}
	}
	ENTER(CALL_V2);
	T = !(r0 & 32);
	CALL(F->v[2], RET_V2);

	// ---- epílogo ----------------------------------------------------------------------
epi:	ENTER(EPI);
epi_body:
	r15 += F->frame;
	pr = POP();
	r8 = POP(); r9 = POP(); r10 = POP(); r11 = POP(); r12 = POP(); r13 = POP(); r14 = POP();
	f12 = u2f(POP()); f13 = u2f(POP()); f14 = u2f(POP()); f15 = u2f(POP());
	SAVE(s);
	return return_to(pr);
}

#undef ENTER
#undef BAIL
#undef ENTER_AT
#undef CALL
#undef PUSH
#undef POP
#undef FACE_COLORS

} // namespace meshlist
