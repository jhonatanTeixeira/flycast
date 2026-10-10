// Laços de `memset` (16 bits) e `ocbp` — pseudo-C++ para todos os jogos identificados
// (docs/sdk_blocks/memset_e_ocbp.md). API: ver luz_e_transformacao_de_vertices_pseudo.cpp.
// HLE escrito em core/rec-ARM64/hle_fn.cpp, **não validado** (tech_debits.md 4.119).
//
// Variantes (três laços diferentes; cada jogo tem um ou mais, em endereços próprios):
//   memset16 contador em r6:  mov.w r7,@r4; add #-1,r6; add #2,r4; tst r6,r6; bf
//   memset16 contador em r5:  mov.w r7,@r4; add #-1,r5; add #2,r4; tst r5,r5; bf
//   ocbp:                     shlr2; shlr2; shlr (÷32); add #1; ocbp @r4; dt r5; bf.s; add #32,r4
//
// O ponto delicado é o tempo, não a conta: o laço do SH4 roda N voltas e o JIT faz uma
// checagem de fatia por volta. A versão nativa precisa descontar os ciclos das N voltas
// e, se a fatia acabar no meio, parar exatamente na volta em que o JIT pararia (senão as
// interrupções caem em outro ponto e o jogo diverge — foi o que impediu validar o 4.119).

namespace fill {

enum Kind { MEMSET16_R6, MEMSET16_R5, OCBP };

constexpr s32 CYC_MEMSET_ITER = 5;           // 5 instruções por volta (× clock do SH4)
constexpr s32 CYC_OCBP_ITER = 3;

// memset16: escreve r7 (16 bits) em `count` meias-palavras a partir de r4
void memset16(Sh4 &s, Cycles &cyc, Kind k, u32 loopPc)
{
	u32 &count = k == MEMSET16_R6 ? s.r[6] : s.r[5];
	u32 dst = s.r[4];
	const u16 value = (u16)s.r[7];
	// quantas voltas cabem na fatia sem chamar o UpdateSystem
	const u32 fit = (u32)std::max<s32>(0, cyc.left() / CYC_MEMSET_ITER);
	const u32 n = std::min(count, std::max(fit, 1u));
	if (is_plain_ram(dst, n * 2))
		fill16(ram_ptr(dst), value, n);       // NEON: 8 meias-palavras por instrução
	else
		for (u32 i = 0; i < n; i++)
			write16(dst + 2 * i, value);      // MMIO/VRAM/código: caminho com efeitos
	dst += 2 * n; count -= n;
	cyc.take(n * CYC_MEMSET_ITER);
	s.r[4] = dst;
	s.sr.T = count == 0;
	if (count != 0)
		return continue_at(loopPc);           // fatia acabou: o JIT segue na volta certa
}

// ocbp: purga da cache de dados por linha de 32 bytes. No emulador a cache não existe
// (a RAM já é a verdade): o laço só gasta tempo. Basta avançar r4/r5 e descontar.
void ocbp(Sh4 &s, Cycles &cyc, u32 loopPc)
{
	u32 lines = s.r[5];
	const u32 fit = (u32)std::max<s32>(0, cyc.left() / CYC_OCBP_ITER);
	const u32 n = std::min(lines, std::max(fit, 1u));
	s.r[4] += 32 * n;
	s.r[5] = lines - n;
	cyc.take(n * CYC_OCBP_ITER);
	if (s.r[5] != 0)
		return continue_at(loopPc);
}

// Despacho: o JIT reconhece qual dos três laços está compilando pelos bytes.
void run(Sh4 &s, Cycles &cyc, Kind k, u32 loopPc)
{
	if (k == OCBP)
		ocbp(s, cyc, loopPc);
	else
		memset16(s, cyc, k, loopPc);
}

} // namespace fill
