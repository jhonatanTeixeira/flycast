// Biblioteca de threads — pseudo-C++ para todos os jogos identificados
// (docs/sdk_blocks/biblioteca_de_threads.md, docs/sh4_threading_model.md).
// API: ver luz_e_transformacao_de_vertices_pseudo.cpp.
//
// Custo ~0,5% da emu: nativizar não dá velocidade; serve de modelo (e para o
// diagnóstico de tempo por thread do sh4_threading_model.md §7).
//
// Variantes:
//   - Napple (referência), Evolution 1/2, Grandia II, KOF Evolution, Le Mans, Macross M3,
//     PSO v2, RE Code: Veronica: idênticos (277 de 277 opcodes).
//   - **Elemental Gimmick Gear** (versão mais antiga da biblioteca): a criação de thread
//     **não grava o FPSCR** no quadro inicial (5 instruções a menos); o resto é igual.
//
//   Jogo                     criar      yield      salvar     (base = criar)
//   Napple                   8C16BB28   8C16BB88   8C16BC18
//   Elemental Gimmick Gear   8C0775C4   8C077614   8C0776A4
//   Evolution 1              8C1B7554   ...        8C1B7644
//   (demais: base na tabela do .md; yield = base+0x60, salvar = base+0xF0)
//
// Modelo: um núcleo, threads que se revezam. A troca é feita "fingindo uma exceção":
// SPC/SSR recebem o ponto de retorno e o SR, o contexto inteiro vai para a pilha da
// thread e o escalonador (função C do jogo) devolve o r15 da próxima; `rte` a retoma.

namespace threads {

struct Variant { u32 base; bool savesFpscr; /* false no EGG */ };

// Quadro salvo na pilha de cada thread (ordem do push, do topo para baixo)
struct Frame {
	u32 bank[8];          // r0_bank..r7_bank
	u32 r[15];            // r0..r14
	u32 ssr, spc, gbr, mach, macl, pr, fpscr, fpul;
	float fr[2][16];      // os dois bancos de float (só se a thread usa FPU)
};

// --- criar: monta a pilha da thread para que o 1º `rte` entre em `entry(arg)`
//     r4 = topo da pilha, r5 = tamanho, r6 = entrada, r7 = argumento
void create(const Variant &v, u32 stackBase, u32 size, u32 entry, u32 arg)
{
	const u32 words = ram32(v.base + LIT_STACK_WORDS);
	u32 sp = stackBase + size;
	for (u32 i = 0; i < words; i++)          // zera o quadro inicial
		write32(sp -= 4, 0);
	write32(sp -= 4, ram32(v.base + LIT_FRAME_MAGIC));
	const u32 top = stackBase + size;
	write32(top - ram32(v.base + LIT_OFF_ENTRY), entry);   // vira o SPC
	write32(top - ram32(v.base + LIT_OFF_ARG), arg);       // vira o r4 da thread
	write32(top - ram32(v.base + LIT_OFF_SR), sr_full());  // vira o SSR
	if (v.savesFpscr)                                       // EGG: não grava
		write32(top - ram32(v.base + LIT_OFF_FPSCR), fpscr_full());
	// devolve o r15 inicial da thread em r0
}

// --- yield: cede a vez de dentro de código normal (não de interrupção)
void yield(Sh4 &s, const Variant &v, u32 reason)
{
	const u32 sr = sr_full();
	write32(ram32(v.base + LIT_SAVED_SR), sr);
	set_sr(sr | ram32(v.base + LIT_IMASK_BITS));   // mascara interrupções
	s.ssr = sr;                                    // finge a exceção:
	s.spc = s.pr;                                  //   volta para quem chamou o yield
	write32(ram32(v.base + LIT_REASON), reason);
	write32(ram32(v.base + LIT_FLAG), ram32(v.base + LIT_FLAG_VALUE));
	switch_context(s, v);
}

// --- troca: salva tudo, pergunta ao escalonador quem roda, restaura e `rte`
void switch_context(Sh4 &s, const Variant &v)
{
	Frame f;
	f.fpul = s.fpul; f.fpscr = s.fpscr.full; f.pr = s.pr; f.macl = s.macl; f.mach = s.mach;
	f.gbr = s.gbr; f.spc = s.spc; f.ssr = s.ssr;
	memcpy(f.r, s.r, sizeof(f.r));
	memcpy(f.bank, s.r_bank, sizeof(f.bank));
	const bool fpu = ram32(ram32(v.base + LIT_FPU_FLAG)) == 0;
	if (fpu)
	{
		memcpy(f.fr[0], s.fr, 64);              // banco atual
		memcpy(f.fr[1], s.xf, 64);              // banco de trás (troca FR no FPSCR)
	}
	s.r[15] = push_frame(s.r[15], f, fpu);

	// escalonador do jogo (C): recebe o r15 salvo, devolve o r15 da próxima thread
	const u32 cur = ram32(ram32(v.base + LIT_CURRENT));
	call_guest(ram32(v.base + LIT_SAVE_CUR), cur, s.r[15]);
	write32(ram32(v.base + LIT_PREV), ram32(ram32(v.base + LIT_NEXT)));
	s.r[15] = call_guest(ram32(v.base + LIT_PICK));

	const bool fpuNext = s.r[15] == ram32(v.base + LIT_FPU_MARK);
	pop_frame(s.r[15], f, fpuNext);
	// restauração na ordem inversa e `rte`: PC = SPC, SR = SSR (troca de banco e IMASK
	// voltam juntos)
	restore_all(s, f);
	rte(s);
}

} // namespace threads
