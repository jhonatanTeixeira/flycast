// Servidor de 32 slots cronometrado (grupo 008 do sdk_find, rótulo automático "copia")
// — pseudo-C++ para os 8 jogos do grupo (docs/sdk_find/auto/008_copia.md) + Le Mans.
// API: ver docs/sdk_blocks/luz_e_transformacao_de_vertices_pseudo.cpp; extras em
// espera_de_quadro_com_timeout_pseudo.cpp (cyc.enter, continue_at, literal, blk_cyc).
// Pseudo-código: não compila no core; nada disto foi escrito em hle_fn.cpp.
//
// O rótulo "copia" está errado: não há cópia. A função é um SERVIDOR periódico de uma
// biblioteca (a mesma família de RPG/aventura do grupo 009): mede o próprio tempo com o
// TMU0, chama um sub-servidor opcional e varre uma tabela FIXA de 32 slots, chamando o
// tratador de cada slot em uso. O que o gerador viu como "leitura + escrita em laço" é a
// varredura (lê +4/+8 do slot, escreve o campo +0x50/+0x58).
//
// Hipótese (não confirmada por símbolo): é o `ADXT_ExecServer` da CRI (ADX). O tratador do
// slot (8C134FAC no Napple) é uma máquina de estados sobre o campo +8 com os códigos 0..6
// e trata o 6 voltando para 3 — o mesmo conjunto dos ADXT_STAT_* (STOP, DECINFO, PREP,
// PLAYING, DECEND, PLAYEND, ERROR) — e todos estes jogos usam ADX.
//
// Folhas usadas (mesma biblioteca do grupo 009; endereços do Napple):
//   tmr()      8C113F0A  return ~TCNT0
//   dif(a,b)   8C113F14  return b - a                      (= tmr + 0x0A)
//   us(d)      8C113F1A  (d/100)*128 + ((d%100)*128)/100   (= tmr + 0x10; ×1,28 = contagem
//                        → µs com o TCR0 = Pφ/64 do jogo; chama __modlu e __divlu)
//
// Semântica (referência; layout B, Napple):
//
//   void servidor()
//   {
//       *T_INI = tmr();                              // 8C1BBB50
//       u32 a = tmr();                               // r14
//       if (*SUB_LIGADO == 1) sub_servidor();        // 8C1BBAD8 / 8C162CF0
//       u32 b = tmr();                               // r12
//       s32 t = us(dif(a, b));
//       if (t >= 1000) { *S0 = a; *S1 = b; *S2 += t; }      // cmp/ge com sinal; 8C1BBAF4..AFC
//       u32 c = tmr();                               // r8
//       for (u32 off = 0; off < LIMITE; off += PASSO) {    // r14, r11, r10
//           u32 *e = TABELA + off;                   // r4
//           if (e[1] == 0) continue;                 // +4: slot em uso?
//           if (e[2] == 0) { CAMPO(e) = 0; continue; }       // +8: estado (0 = parado)
//           CAMPO(e) = 1; trata_slot(e);             // bsr para a rotina logo antes
//       }
//       u32 d = tmr();
//       s32 u = us(dif(c, d));
//       if (u >= 50) { *S3 = c; *S4 = d; *S5 += u; }       // 8C1BBB00..B08
//       *T_ANT = *T_INI;                             // 8C1BBB48
//       *T_FIM = tmr();                              // 8C1BBB4C
//   }
//
// Quem chama: no Napple ela é registrada como callback (8C135B48: r4 = 8C13503C) numa
// lista (fn, arg) que 8C1368A4 percorre; essa lista é chamada pelo laço principal ocioso
// 8C1368D4 (`for (;;) if (*8C136900 == 1) quadro(); else roda_callbacks();`). Ou seja, no
// Napple esta função roda EM VOLTA enquanto o jogo espera o próximo quadro (30 fps
// travado): os 5,86% são, na prática, tempo de espera. O ganho máximo viria de pular a
// espera no laço 8C1368D4 (como o 4.14/4.38), mas isso muda o número de vezes que o
// servidor roda (estatísticas, ADX) — fora do escopo deste arquivo.
//
// Jogos (entrada da função; layout; TABELA = literal do `mov.l` em +0x68):
//
//   Jogo                                   entrada     layout  TABELA      passo  limite  campo
//   Elemental Gimmick Gear                 8C05D81A    A       8C38EA64    0x8C   0x1180  +0x50
//   Evolution - The World of Sacred Device 8C15E84E    A       8C2F888C    0x8C   0x1180  +0x50
//   Evolution 2 - Far Off Promise          8C17BD82    A       8C37AD4C    0x8C   0x1180  +0x50
//   Resident Evil - Code: Veronica         8C1727C2    A       8C334304    0x8C   0x1180  +0x50
//   Grandia II                             8C028204    B       8C1FFC0C    0x1BC  0x3780  +0x58
//   King of Fighters Evolution             8C316F7C    B       (*)         (*)    (*)     +0x58
//   Macross M3                             8C1F70A0    B       8C67EA14    0x1BC  0x3780  +0x58
//   Napple Tale                            8C13503C    B       8C367BAC    0x1BC  0x3780  +0x58
//   Le Mans 24 Hours (fora do grupo)       8C05AD5C    B       8C0ED8CC    0x1BC  0x3780  +0x58
//   (*) no KOF a página não é read-only no dump: o SHIL lê o literal da RAM e o valor não
//       aparece (literais em 8C31707C / 8C317054 / 8C317056); presumido igual aos outros B.
//   Le Mans: mesmo código (laço em 8C05ADC4), em área de overlay — o sdk_find não montou a
//   função; ciclos com clock 0,8 (11/5/9/3 → 8/4/7/2).
//
// Nos dois layouts são 32 slots (0x1180/0x8C = 0x3780/0x1BC = 32); o tamanho do slot muda
// (140 vs 444 bytes), como o campo — duas versões da biblioteca.
//
// Perf (só EGG e Napple têm amostra; % da thread de emulação):
//   Napple 8C13503C  5,86% nos blocos da função; a varredura sozinha é 3,69% (cabeça
//          8C1350A4 2,07% + cauda 8C1350C0 1,62%). Fora da função, parte de tmr 8C113F0A
//          (1,08%) e de us() 8C113F1A.. (~1,3%) vem daqui (4 tmr + 2 us por chamada), e o
//          laço que a chama (8C1368B6/C4, 0,98 + 0,81%).
//   EGG    8C05D81A  0,01%. Demais: sem amostra.
//
// Variantes (comparação byte a byte das listagens e do SHIL dos dumps):
//   - Layout A (EGG, Evolution 1, Evolution 2, RE CV) × layout B (Grandia, KOF, Macross,
//     Napple, Le Mans): **muda de verdade**, mas só em forma, não em lógica. A: literal
//     pool NO MEIO da função (+0x80..+0xB4), o ramo "slot ativo" depois do pool, campo
//     +0x50, slot de 0x8C bytes. B: pool no fim, ramos em sequência, campo +0x58, slot de
//     0x1BC. Pré-laço e pós-laço são as mesmas instruções (só deslocamentos de PC e, em B,
//     o pós-laço 0x38 bytes antes).
//   - Dentro de cada layout: idênticos a menos dos deslocamentos `@(disp,PC)` e dos
//     endereços no pool.
//   - Cobertura: EGG, Evolution 2, Grandia e Macross nunca executaram o ramo "estado 0"
//     (+0x78); RE CV não executou a chamada do sub-servidor (+0x26) nem nenhum slot em uso
//     (+0x72..+0x7C, +0xB6); Evolution 1, KOF e Napple têm tudo. Os demais buracos das
//     listagens A (+0x7E..+0xB4) são o literal pool.
//
// Ciclos dos blocos do laço no dump (sh4clock 1,0; o nativo usa blk_cyc, ver 009):
//   CABECA +0x68 (5)  r4 = TABELA + r14; r2 = e[1]; tst; bt CAUDA
//   CORPO  +0x72 (9)  r3 = e[2]; tst; bf ATIVO            (3 instr. × FC_IDLE_MUL)
//   ZERO   +0x78 (3)  r0 = CAMPO; bra CAUDA; CAMPO(e) = r12 (= 0)
//   ATIVO  +0x7E/B6 (3)  r0 = CAMPO; bsr trata_slot; CAMPO(e) = r9 (= 1)
//   CAUDA  +0x84/BC (3)  r14 += r10; cmp/hs r11,r14; bf CABECA
//   A primeira cabeça está dentro do bloco +0x5C (11 instr., 11 ciclos), que a compila
//   junto com o pré-laço; por isso o nativo tem DUAS entradas: CABECA e CAUDA.
//
// O nativo cobre só a varredura (o ponto quente): o pré e o pós-laço chamam código do jogo
// (sub-servidor, trata_slot) e as folhas de divisão. Cobrir também o pré/pós-laço (nível 2)
// exigiria reproduzir o estado dos registradores na entrada de cada bloco do __modlu/
// __divlu (o JIT agrega div0u + 32×(rotcl/div1) em shop_div32u, blocos de 70 ciclos) para o
// caso de a interrupção cair ali — **não detalhado aqui**.

namespace servidor_slots {

struct Layout {
	u32 head, body, zero, ativo, tail, out;    // offsets a partir da entrada
};
constexpr Layout LAYOUT_A = { 0x68, 0x72, 0x78, 0xB6, 0xBC, 0xC2 };   // EGG, Evo 1/2, RE CV
constexpr Layout LAYOUT_B = { 0x68, 0x72, 0x78, 0x7E, 0x84, 0x8A };   // Grandia, KOF, Macross, Napple, Le Mans

// A assinatura (opcodes da função com máscara nos `mov.l/mov.w @(disp,PC)` e nos desvios)
// diz qual layout é; os dois diferem em 0x70 (bt: 8924 × 8908) e 0x76 (bf: 8B1E × 8B02).
struct Plan {
	Layout L;
	u32 base, table, campo;
	s32 c_head, c_body, c_zero, c_tail;
};

static void plan(Plan &p, u32 base, const Layout &L)
{
	p.L = L; p.base = base;
	p.table = literal(base + L.head);               // D412 / D425: TABELA
	p.campo = ram16(base + L.zero) & 0xFF;           // E050 / E058: o campo vem do próprio código
	p.c_head = blk_cyc(base + L.head);  p.c_body = blk_cyc(base + L.body);
	p.c_zero = blk_cyc(base + L.zero);  p.c_tail = blk_cyc(base + L.tail);
}

// entry = HEAD: o JIT descontou a cabeça desta volta; entry = TAIL: descontou a cauda.
// Registradores fixos durante o laço (montados no bloco +0x5C): r9 = 1, r10 = PASSO,
// r11 = LIMITE, r12 = 0, r8 = c. O nativo lê os quatro primeiros de s.r (não supõe valor).
void run(Sh4 &s, Cycles &cyc, u32 base, const Layout &L, bool entryIsTail)
{
	Plan p;
	plan(p, base, L);
	// toca no máximo TABELA + LIMITE - PASSO + CAMPO + 4 < TABELA + LIMITE
	if (!is_plain_ram(p.table, s.r[11]))
		return bail(base + (entryIsTail ? L.tail : L.head));

	bool atTail = entryIsTail;
	for (;;)
	{
		if (!atTail)
		{
			// CABECA (já descontada): r4 = TABELA + r14; r2 = e[1]; tst r2,r2; bt CAUDA
			const u32 e = p.table + s.r[14];
			s.r[4] = e;
			s.r[2] = ram32(e + 4);
			s.sr.T = s.r[2] == 0;
			if (!s.sr.T)
			{
				// CORPO: r3 = e[2]; tst r3,r3; bf ATIVO
				if (!cyc.enter(base + L.body, p.c_body)) return;
				s.r[3] = ram32(e + 8);
				s.sr.T = s.r[3] == 0;
				if (!s.sr.T)
					return continue_at(base + L.ativo);  // slot rodando: o JIT chama trata_slot
				                                         // e volta pela CAUDA -> nativo de novo
				// ZERO: r0 = CAMPO; bra CAUDA; mov.l r12,@(r0,r4)
				if (!cyc.enter(base + L.zero, p.c_zero)) return;
				s.r[0] = p.campo;
				write32(e + p.campo, s.r[12]);         // mesmo caminho de escrita do JIT
			}
			if (!cyc.enter(base + L.tail, p.c_tail)) return;
		}
		atTail = false;

		// CAUDA (já descontada): add r10,r14; cmp/hs r11,r14; bf CABECA
		s.r[14] += s.r[10];
		s.sr.T = s.r[14] >= s.r[11];
		if (s.sr.T)
			return continue_at(base + L.out);          // fim da varredura: o JIT segue no pós-laço

		if (!cyc.enter(base + L.head, p.c_head)) return;
	}
}

// Detalhes de exatidão:
// - Ordem de efeitos igual à do JIT: cada bloco só altera registradores depois do seu
//   cyc.enter; se a fatia acaba e há interrupção, o estado gravado é o da entrada do bloco.
// - A leitura de e[1]/e[2] é feita na volta em que o JIT a faria (depois do UpdateSystem
//   que porventura caiu antes): um evento que mude a tabela no meio da varredura é visto
//   no mesmo slot que no JIT.
// - Não há atalho de "pular slots vazios em bloco": cada slot vazio custa CABECA + CAUDA
//   (8 ciclos no dump) e a fatia pode acabar entre eles. Como são só 32 slots, o laço
//   direto já é ~ns por slot; ganho vem de tirar ~64 entradas de bloco do JIT por chamada.
// - Registradores que precisam bater no FC_STATE_HASH: r0 r2 r3 r4 r14 T.

} // namespace servidor_slots

// Não determinado:
// - Identidade da biblioteca (ADXT_ExecServer é hipótese pelo conjunto de estados 0..6).
// - TABELA/PASSO/LIMITE do KOF (literais não dobrados no dump).
// - Para que servem S0..S5 (cara de estatística "maior atraso", lida por debug/perfil).
// - Nível 2 (pré/pós-laço com as divisões) e o salto da espera no laço ocioso 8C1368D4 do
//   Napple: não escritos.
