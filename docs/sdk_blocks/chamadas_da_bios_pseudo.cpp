// Chamadas da BIOS (syscalls por vetor) — pseudo-C++ para todos os jogos identificados
// (docs/sdk_blocks/chamadas_da_bios.md). API: ver luz_e_transformacao_de_vertices_pseudo.cpp.
//
// O stub é sempre o mesmo (20 jogos, inclusive o TR Chronicles/WinCE):
//     mov.l @(lit_r7),r7      ; número da função (às vezes `mov #imm,r7` / `mov #0,r6`)
//     mov.l @(lit_vec),r0     ; endereço do vetor: 0x8C0000B0/B4/B8/BC/C0/E0
//     mov.l @r0,r0            ; o que está no vetor
//     jmp   @r0
// Variantes: só os valores do literal pool (função e vetor). Nenhuma diferença de código.
//
// O emulador já tem a versão nativa disso: o reios (BIOS em HLE,
// core/reios/reios.cpp) grava nos vetores endereços-gancho e trata cada chamada em C
// (gdrom_hle.cpp para o GD-ROM). Com a BIOS real, o vetor aponta para o código da BIOS
// e o JIT o executa. Nativizar o stub não ganha nada (são 4 instruções); o que importa
// é qual BIOS está em uso.

namespace bios {

enum Vector : u32 {
	SYSTEM = 0x8C0000B0, FONT = 0x8C0000B4, FLASHROM = 0x8C0000B8,
	GDROM = 0x8C0000BC, GDROM2 = 0x8C0000C0, MISC = 0x8C0000E0,
};

// Grupos do vetor do GD-ROM (r6) e funções mais comuns (r7), como no KallistiOS:
//   r6 = 0  : GD-ROM. r7: 0 envia comando, 1 estado do comando, 2 executa o servidor,
//             3 inicializa o sistema, 4 estado do drive, 5 fim do DMA, 6 pede DMA,
//             7 checa DMA, 8 aborta leitura, 9 reset, 10 muda o tipo de dado
//   r6 = -1 : funções "misc" do mesmo vetor
// Ex.: DOA2 8C129BB4 = r6 0, r7 3 → inicializa o sistema do GD-ROM.

void stub(Sh4 &s, u32 vectorAddr, u32 function)
{
	s.r[7] = function;
	const u32 target = ram32(vectorAddr);
	if (reios_owns(target))
		return reios_call(vectorAddr, s);    // BIOS em HLE: trata em C (já existe)
	return jump_to(target);                  // BIOS real: o JIT segue no código dela
}

} // namespace bios
