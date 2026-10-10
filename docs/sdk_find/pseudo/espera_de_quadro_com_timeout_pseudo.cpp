// Espera de fim de quadro com timeout (grupo 009 do sdk_find, rótulo automático "copia")
// — pseudo-C++ para os 10 jogos do grupo (docs/sdk_find/auto/009_copia.md).
// API: ver docs/sdk_blocks/luz_e_transformacao_de_vertices_pseudo.cpp; extras no fim deste
// cabeçalho. Pseudo-código: não compila no core; nada disto foi escrito em hle_fn.cpp.
//
// O rótulo "copia" do gerador está errado: não há cópia de memória no laço. A função é a
// rotina de FIM DE QUADRO de uma biblioteca comum aos RPG/aventura (EGG, Evolution 1/2,
// Grandia II, KOF Evolution, Macross M3, Napple, PSO v2, RE Code: Veronica, Skies). O
// trecho quente é uma ESPERA OCUPADA: lê o TMU0 em volta, até uma flag que a interrupção
// zera ou até estourar um timeout. As 5 instruções de "cópia" que o gerador viu são o
// final (copia 5 palavras de uma struct para globais), que roda uma vez por quadro.
//
// O que a função faz (nomes = papel, não símbolo; endereços do EGG):
//   tmr()      8C0618EA  return ~TCNT0 (lê 0xFFD8000C; "mov #-1,r0; sub r3,r0")
//   dif(a,b)   8C0618F4  return b - a
//   cnv(d)     8C06192E  ((d & 127) * 100 >> 7) + (d >> 7) * 100       (= d × 100/128)
//
//   void fim_de_quadro()
//   {
//       u32 t = 0;                                   // r14
//       *T0 = tmr();                                 // r12 = T0
//       if (*ESPERA_LIGADA) {                        // literal em +0x18
//           while (*FLAG != 0 && t < LIMITE)         // FLAG: literal em +0x3A; LIMITE 0x28870
//               t = cnv(dif(*T0, tmr()));            // a ordem real: testa antes de medir
//           if (t < LIMITE) cb_opcional();           // 8C063CD6: if (*P) (*P)()
//       }
//       *FLAG = 1; F1();                             // F1 = 8C083BE0 no EGG
//       *C0 = *C1 = *C2 = 0; (*QUADROS)++;           // 3 zeros + contador de quadros
//       F2(); F3();                                  // 8C0836A0, 8C061356 no EGG
//       *T1 = tmr();
//       copia 5 palavras de *PTR para G[0..4];       // a "cópia" que deu o rótulo
//   }
//
// Quem zera a FLAG: no EGG, as rotinas 8C063186 e 8C05DB60 (writem 0 em 8C39AAB8),
// chamadas a partir de interrupção (uma delas também incrementa um contador e chama
// outras: cara de V-blank; a outra liga outra flag e chama um callback: cara de fim de
// render). **Não determinado** qual interrupção é qual.
//
// Unidade do timeout: o jogo programa TCR0 = 2 (Pφ/64, 1,28 µs por tick; 8C0618C4 no EGG).
// cnv() multiplica por 100/128, ou seja é a conversão µs→contagem, aplicada a uma diferença
// de contagens; 0x28870 = 166000 "unidades" = 212480 ticks ≈ 272 ms (~16 quadros). A
// conversão contagem→µs é a vizinha 8C0618FA (×128/100, com __modlu/__divlu), usada pelo
// grupo 008. Se isso é bug da biblioteca ou intenção, **não determinado**; para o nativo
// só importa repetir a conta.
//
// Jogos (entrada da função; T0 = r12; FLAG = flag de espera; tmr = r13):
//
//   Jogo                                   entrada     T0          FLAG        tmr
//   Elemental Gimmick Gear                 8C063CEC    8C39B104    8C39AAB8    8C0618EA
//   Evolution - The World of Sacred Device 8C1663F8    8C304FF4    8C3049A8    8C163F16
//   Evolution 2 - Far Off Promise          8C17247E    8C36CFB4    8C36C964    8C16FDFE
//   Grandia II                             8C03B8D6    8C2267B4    8C226164    8C0381F2
//   King of Fighters Evolution             8C31E056    8C7826F8    8C7820A8    8C31B762
//   Macross M3                             8C1C5CEE    8C63F96C    8C63F31C    8C1D3976
//   Napple Tale                            8C108E22    8C353FBC    8C35396C    8C113F0A
//   Phantasy Star Online Ver. 2            8C38D456    8C58D144    8C58CAF4    8C37B636
//   Resident Evil - Code: Veronica         8C17B172    8C360CD8    8C360688    8C1787C2
//   Skies of Arcadia (disco 1)             8C25A686    8C5999A0    8C599350    8C25761E
//
// Em todos: dif = tmr + 0x0A, cnv = tmr + 0x44, LIMITE = 0x28870. Uma cópia da função por
// jogo nos dumps. O tmr() (5 opcodes) aparece em outro endereço também no PSO (8C0199CE)
// e no Le Mans (8C01D916, 8C04A756, 8C21E122), de outras partes do código; o Le Mans não
// tem esta função nos dumps.
//
// Perf (só EGG e Napple têm amostras; % da thread de emulação):
//   EGG    8C063CEC  5,42% nos blocos da função (8C063D1A 1,44, D24 1,39, D20 0,99,
//          D16 0,83, D2E 0,71) + 4,16% nas três folhas que o laço chama (tmr 8C0618EA 1,60,
//          cnv 8C06192E 1,63, dif 8C0618F4 0,93) ≈ 9,6%, mais parte dos 2,9% do
//          despachante (cada volta faz 3 jsr dinâmicos e 3 rts = 6 saltos via `jdyn`).
//   Napple 8C108E22  0,36% (no Napple a espera acontece em outro laço: ver grupo 008).
//   Demais: sem amostra de perf.
//
// Variantes: **nenhuma de código**. Os 10 jogos têm os mesmos 91 opcodes (todos executados
// nos dumps, nenhum buraco); só mudam os deslocamentos dos `mov.l @(disp,PC)` (EGG e
// Evolution 1 com o literal pool numa posição, os outros 8 noutra) e os valores do pool
// (endereços). As três folhas (tmr/dif/cnv) são byte a byte iguais nos 10. Os ciclos dos
// blocos no dump também são iguais nos 10 (sh4clock 1,0):
//
//   bloco (offset)                         ciclos   obs.
//   +2A  jsr @r13              (D16)          2
//   tmr  (folha, 5 instr.)                  224     teto max_cycles = SH4_TIMESLICE/2: o
//                                                   bloco bate no idle_hash (×100, cortado)
//   +2E  mov r0,r5; jsr @r10; mov.l @r12,r4   3
//   dif  (folha, 3 instr.)                    3
//   +34  jsr @r9; mov r0,r4                   2
//   cnv  (folha, 13 instr.)                  13
//   +38  mov r0,r14; testa FLAG; bt +0x48     5
//   +42  cmp/hs LIMITE,r14; bf +0x2A          9     3 instr. × FC_IDLE_MUL (3): truque "small"
//   +48  cmp/hs LIMITE,r14; bt (saída)        3     (primeiro bloco depois do laço)
//   ⇒ 261 ciclos por volta; a fatia é 448 (×multiplicador do core): ~1,7 volta por fatia.
//
// Esses números dependem da configuração (sh4clock, LUT por jogo, idleskip, FC_IDLE_MUL):
// o Le Mans, por exemplo, roda com clock 0,8 e os mesmos blocos saem 8/4/2/7. O nativo
// NÃO deve fixar 261: deve pegar o guest_cycles que o decodificador dá a cada bloco
// (blk_cyc(pc) abaixo — decodificar o bloco numa RuntimeBlockInfo de rascunho, ou
// replicar as regras de decoder.cpp: ×clk truncado, ×FC_IDLE_MUL no bloco "small", teto
// 224 do idle_hash, ×1,5 com idleskip desligado).
//
// Por que dá para ser exato e rápido: o TCNT0 lido é `base - (sh4_sched_now64() >> 8)` e
// sh4_sched_now64() = sh4_sched_ffb - sh4_sched_next só muda no UpdateSystem (fronteira de
// fatia). A FLAG e *T0 só mudam por interrupção/evento, que também só acontecem ali. Então
// dentro de uma fatia todas as voltas dão o mesmo resultado; o que importa é parar na
// mesma fronteira de bloco em que o JIT chamaria o UpdateSystem e, se houver interrupção,
// sair com os registradores exatamente como o JIT os teria naquele bloco.
//
// Plug: o JIT reconhece o bloco +0x2A (cabeça do laço, `jsr @r13`) pelos bytes da função
// (91 opcodes, com máscara nos `mov.l @(disp,PC)`) e chama o nativo depois de descontar os
// 2 ciclos desse bloco. As folhas não são assinadas pelo endereço fixo: a cada chamada o
// nativo confere os bytes em r13, r10 e r9 (são registradores, poderiam apontar outra coisa).
//
// Extras de API usados aqui:
//   cyc.enter(pc, n)  como o ENTER do hle_fn.cpp: desconta n; se a fatia acabou grava o
//                     estado, faz UpdateSystem e, havendo interrupção (ou CpuRunning = 0),
//                     devolve false com next_pc = rdv_DoInterrupts_pc(pc) (ou pc).
//   continue_at(pc)   sai para o JIT no bloco pc, sem descontar nada dele.
//   read_tcnt0()      o mesmo handler que o readm do JIT chama (read_TMU_TCNT<0>).
//   bytes_eq(a, sig)  compara a RAM em a com a assinatura.

namespace espera_quadro {

constexpr u32 OFF_HEAD = 0x2A;      // jsr @r13           (entrada do nativo)
constexpr u32 OFF_D1A  = 0x2E;      // mov r0,r5; jsr @r10; mov.l @r12,r4
constexpr u32 OFF_D20  = 0x34;      // jsr @r9; mov r0,r4
constexpr u32 OFF_D24  = 0x38;      // mov r0,r14; r3 = FLAG; r2 = *FLAG; tst; bt
constexpr u32 OFF_D2E  = 0x42;      // r2 = LIMITE; cmp/hs r2,r14; bf HEAD
constexpr u32 OFF_OUT  = 0x48;      // primeiro bloco depois do laço (as duas saídas)
constexpr u32 OFF_DIF  = 0x0A;      // dif = tmr + 0x0A
constexpr u32 OFF_CNV  = 0x44;      // cnv = tmr + 0x44

const u16 SIG_TMR[] = { 0xD243, 0xE0FF, 0x6322, 0x000B, 0x3038 };   // disp do D243 igual nos 10
const u16 SIG_DIF[] = { 0x6053, 0x000B, 0x3048 };
const u16 SIG_CNV[] = { 0xE07F, 0x6243, 0x2049, 0xE564, 0x0057, 0xE3F9, 0x423D,
                        0x001A, 0x0257, 0x403D, 0x021A, 0x000B, 0x302C };

// Literal de um `mov.l @(disp,PC),Rn` (0xDndd): endereço = (pc & ~3) + 4 + disp*4.
static inline u32 literal(u32 pc) { return ram32((pc & ~3u) + 4 + (ram16(pc) & 0xFF) * 4); }

// Ciclos por bloco, montados na instalação (não fixar os números do dump).
struct Plan {
	u32 base, flag, limit, tcnt_addr;
	s32 c_tmr, c_d1a, c_dif, c_d20, c_cnv, c_d24, c_d2e, c_head;
};

static bool plan(Plan &p, u32 base, const Sh4 &s)
{
	p.base = base;
	p.flag = literal(base + OFF_D24 + 2);         // D342 em +0x3A
	p.limit = literal(base + OFF_D2E);             // D241 em +0x42 (0x28870 nos 10)
	// as folhas vêm dos registradores: confere o código que de fato vai rodar
	if (!bytes_eq(s.r[13], SIG_TMR) || s.r[10] != s.r[13] + OFF_DIF || s.r[9] != s.r[13] + OFF_CNV
	    || !bytes_eq(s.r[10], SIG_DIF) || !bytes_eq(s.r[9], SIG_CNV))
		return false;
	p.tcnt_addr = literal(s.r[13]);               // 0xFFD8000C nos 10
	if (p.tcnt_addr != 0xFFD8000C)
		return false;
	p.c_head = blk_cyc(base + OFF_HEAD);  p.c_tmr = blk_cyc(s.r[13]);
	p.c_d1a  = blk_cyc(base + OFF_D1A);   p.c_dif = blk_cyc(s.r[10]);
	p.c_d20  = blk_cyc(base + OFF_D20);   p.c_cnv = blk_cyc(s.r[9]);
	p.c_d24  = blk_cyc(base + OFF_D24);   p.c_d2e = blk_cyc(base + OFF_D2E);
	return true;
}

// Entrada: o JIT acabou de descontar o bloco HEAD desta volta. Estado vindo do JIT:
// r9/r10/r12/r13 fixos, r14 = t da volta anterior (< LIMITE), FLAG != 0 na última leitura.
void run(Sh4 &s, Cycles &cyc, u32 base)
{
	Plan p;
	if (!plan(p, base, s))
		return bail(base + OFF_HEAD);

	for (;;)
	{
		// HEAD (já descontado): jsr @r13 — pr = volta para D1A
		s.pr = base + OFF_D1A;

		// tmr(): r2 = &TCNT0; r0 = -1; r3 = TCNT0; rts; r0 = -1 - r3
		if (!cyc.enter(s.r[13], p.c_tmr)) return;
		const u32 tcnt = read_tcnt0();
		s.r[2] = p.tcnt_addr; s.r[3] = tcnt; s.r[0] = ~tcnt;

		// D1A: mov r0,r5; jsr @r10; mov.l @r12,r4
		if (!cyc.enter(base + OFF_D1A, p.c_d1a)) return;
		s.r[5] = s.r[0]; s.pr = base + OFF_D20; s.r[4] = ram32(s.r[12]);

		// dif(): rts; r0 = r5 - r4
		if (!cyc.enter(s.r[10], p.c_dif)) return;
		s.r[0] = s.r[5] - s.r[4];

		// D20: jsr @r9; mov r0,r4
		if (!cyc.enter(base + OFF_D20, p.c_d20)) return;
		s.pr = base + OFF_D24; s.r[4] = s.r[0];

		// cnv(): mesma ordem de efeitos do SH4 (MACL fica com o 2º produto)
		if (!cyc.enter(s.r[9], p.c_cnv)) return;
		{
			const u32 d = s.r[4];
			const u32 lo = ((d & 127) * 100) >> 7;
			const u32 hi = (d >> 7) * 100;            // mul.l: 32 bits baixos
			s.r[5] = 100; s.r[3] = (u32)-7; s.r[2] = hi; s.macl = hi;
			s.r[0] = lo + hi;
		}

		// D24: mov r0,r14; r3 = FLAG; r2 = *FLAG; tst r2,r2; bt OUT
		if (!cyc.enter(base + OFF_D24, p.c_d24)) return;
		s.r[14] = s.r[0]; s.r[3] = p.flag; s.r[2] = ram32(p.flag);
		s.sr.T = s.r[2] == 0;
		if (s.sr.T)
			return continue_at(base + OFF_OUT);        // a interrupção zerou a FLAG

		// D2E: r2 = LIMITE; cmp/hs r2,r14; bf HEAD
		if (!cyc.enter(base + OFF_D2E, p.c_d2e)) return;
		s.r[2] = p.limit;
		s.sr.T = s.r[14] >= p.limit;
		if (s.sr.T)
			return continue_at(base + OFF_OUT);        // timeout (~272 ms)

		if (!cyc.enter(base + OFF_HEAD, p.c_head)) return;
	}
}

// Nota sobre `cyc.enter` aqui: o estado gravado na saída por interrupção é o da ENTRADA do
// bloco (como no JIT, que testa a fatia antes de executar o bloco). Por isso cada passo
// acima só escreve os registradores DEPOIS do enter do seu bloco. Registradores que o laço
// toca e que precisam bater no FC_STATE_HASH: r0 r2 r3 r4 r5 r14 pr macl T.
//
// Custo: por volta, ~8 subtrações, 2 leituras de RAM e 1 leitura do TMU, contra 8 blocos
// do JIT com 6 saltos dinâmicos (jsr/rts por `jdyn` passam pelo despachante). O
// UpdateSystem continua sendo chamado a cada fatia, como no JIT.
//
// Opcional (não incluído, validar à parte): numa fatia sem evento (sh4_sched_next -
// timeslice >= 0, interrupt_pend == 0, tier2 sem poll) o UpdateSystem só faz
// `sh4_sched_next -= timeslice`; dá para fazê-lo em linha e continuar no laço sem a
// chamada. Ir além disso (pular direto até o próximo evento, como o idle_fastforward do
// 4.14) muda só o número de UpdateSystem e não o resultado, MAS o laço lê hardware (TMU,
// o caso que o 4.43 exclui do salto): o salto precisaria parar também na fatia em que
// cnv(dif(*T0, tmr())) passa do LIMITE, que dá para calcular (cnv é monotônica até
// d ≈ 2^32/100). Não feito aqui.

} // namespace espera_quadro

// Não determinado:
// - Qual interrupção zera a FLAG (8C063186 / 8C05DB60 no EGG) e o que são F1/F2/F3.
// - Se a unidade do timeout (cnv = µs→contagem aplicada a contagens) é intencional.
// - blk_cyc(): os valores acima são os do dump com sh4clock 1,0; com outra configuração a
//   tabela muda (a regra é a do decoder.cpp), por isso o nativo deve calculá-la.
