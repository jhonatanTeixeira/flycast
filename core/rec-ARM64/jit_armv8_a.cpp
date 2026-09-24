/*
	jit_armv8_a -- novo backend JIT ARM64 do SH4.

	Fica ao lado do rec_arm64.cpp (nao o substitui): selecionado em runtime por
	FC_JIT_ARMV8_A=1 e usado apenas com !mmu_enabled(). As funcoes globais do
	ngen em rec_arm64.cpp desviam para as `*_a` daqui.

	Desenho (docs/jit_study.md, docs/jit_armv8_a_context_audit.md):
	- r0-r7 do SH4 fixos em x19-x26 entre ops (callee-saved: sobrevivem a
	  qualquer chamada C++ e ao caminho do intc_sched do mainloop);
	- o resto do banco fica no Sh4Context e e carregado/descarregado sob
	  demanda (nao ha alocador por bloco);
	- o bloco carrega r0-r7 no prologo e os grava no contexto antes de
	  qualquer saida (ligacao direta ou despacho), para que a entrada fria
	  pelo mainloop sempre leia estado atualizado;
	- ops nativas: ALU/deslocamento/extensao/teste/setcc, mov32/mov64,
	  leitura/escrita de memoria (por chamada C++ nesta primeira versao),
	  jdyn/jcond e ifb. O resto cai em shil_chf (implementacao canonica C++),
	  com flush/reload dos fixos em volta;
	- mainloop compartilhado com o backend antigo (x28 = contexto, w27 =
	ciclos, w29 = next_pc).
*/
#include "types.h"

#if FEAT_SHREC == DYNAREC_JIT

#include <vector>
#include <cstring>

#include "deps/vixl/aarch64/macro-assembler-aarch64.h"
using namespace vixl::aarch64;

#include "hw/sh4/sh4_opcode_list.h"
#include "hw/sh4/sh4_core.h"
#include "hw/sh4/sh4_mem.h"
#include "hw/sh4/sh4_mmr.h"
#include "hw/sh4/sh4_rom.h"
#include "hw/sh4/sh4_interrupts.h"
#include "hw/sh4/sh4_interpreter.h"
#include "hw/sh4/sh4_sched.h"
#include "hw/sh4/modules/mmu.h"
#include "hw/sh4/dyna/ngen.h"
#include "hw/sh4/dyna/shil.h"
#include "jit_armv8_a.h"
#include "hw/mem/_vmem.h"

extern "C" void ngen_LinkBlock_Generic_stub();
extern "C" void ngen_LinkBlock_cond_Branch_stub();
extern "C" void ngen_LinkBlock_cond_Next_stub();
extern "C" void ngen_blockcheckfail(u32 pc);
extern void vmem_platform_flush_cache(void *icache_start, void *icache_end, void *dcache_start, void *dcache_end);
// Caminho de escrita ciente de pagina de codigo (blockmanager.cpp): mantem a
// pagina protegida em escrita de DADOS e so desprotege em escrita de codigo,
// igual ao backend antigo. Sem isso o jit_armv8_a desprotegia a pagina em toda
// escrita e o bloco era recompilado, divergindo do backend antigo.
void DYNACALL bm_WriteMemCodePage8(u32 addr, u8 data);
void DYNACALL bm_WriteMemCodePage16(u32 addr, u16 data);
void DYNACALL bm_WriteMemCodePage32(u32 addr, u32 data);
void DYNACALL bm_WriteMemCodePage64(u32 addr, u64 data);
// Diagnostico FC_JIT_TRACE (definidos em rec_arm64.cpp).
bool jit_trace_enabled();
// rec_arm64.cpp: slots vivos (o trampolim salva) e sujos com offset no
// contexto (o trampolim de MMIO grava antes do C++) de um acesso compacto.
void jit_armv8a_record_site(uintptr_t pc, u16 live, const u8 *slots, const u16 *offs, int n);
void DYNACALL jit_trace_block(u32 vaddr, u32 phase);

// ---------------------------------------------------------------------------
// Estado global deste backend
// ---------------------------------------------------------------------------

bool jit_armv8a_enabled()
{
	static int enabled = -1;
	if (enabled == -1)
		enabled = getenv("FC_JIT_ARMV8_A") != nullptr ? 1 : 0;
	return enabled == 1 && !mmu_enabled();
}

// r0-r7 do SH4 ficam em x19-x26.
static const int kPinnedBase = 19;
static const int kPinnedCount = 8;

// Entrada fria do bloco = carga dos 8 fixos (despachante, FPCB, stubs de
// ligacao); entrada quente logo depois, usada pelos saltos diretos entre
// blocos, que mantem r0-r7 em x19-x26 sem passar pelo contexto.
static const u32 kColdEntryBytes = kPinnedCount * 4;

// Saida ainda nao ligada: `bl` para um destes stubs, que grava os fixos e
// salta (LR intacto) para o stub de ligacao de sempre. Mesmo tamanho que o
// `b` da saida ligada, entao o RelinkBlock reescreve no lugar.
static void *linkStubGeneric, *linkStubCondBranch, *linkStubCondNext;
void jit_armv8a_reset_stubs()
{
	linkStubGeneric = linkStubCondBranch = linkStubCondNext = nullptr;
}

static inline bool IsPinned(Sh4RegType reg)
{
	return reg >= reg_r0 && reg < (Sh4RegType)(reg_r0 + kPinnedCount);
}
static inline Register Pinned(Sh4RegType reg)
{
	return Register::GetWRegFromCode(kPinnedBase + (reg - reg_r0));
}

class Armv8AAssembler : public MacroAssembler
{
public:
	Armv8AAssembler() : Armv8AAssembler(emit_GetCCPtr()) {}
	Armv8AAssembler(void *buffer)
		: MacroAssembler((u8 *)buffer, emit_FreeSpace())
	{
	}

	RuntimeBlockInfo *block = nullptr;

	void Compile(RuntimeBlockInfo *blk, bool force_checks, bool reset, bool staging, bool optimise);
	void GenLinkStubs();
	void Finalize(bool rewrite = false);
	u32 RelinkBlock(RuntimeBlockInfo *blk);

	void CC_Start();
	void CC_Param(shil_param &prm, CanonicalParamType tp);
	void CC_Call(void *function);

private:
	// ---- helpers de contexto/registrador -------------------------------

	MemOperand Ctx(void *p)
	{
		u32 offset = (u8 *)p - (u8 *)&p_sh4rcb->cntx;
		verify((offset & 3) == 0 && offset <= 16380);
		return MemOperand(x28, offset);
	}
	MemOperand Ctx(Sh4RegType reg) { return Ctx(GetRegPtr(reg)); }

	// ---- cache de registradores inteiros (2026-09-25) --------------------
	// r8-r15, T, pr, fpul, mac... (o que nao e fixo) ficam em w2-w8 dentro
	// do bloco, com escrita adiada, como o cache de FP. Sem isso cada op ia
	// ao contexto (r15 = pilha: str seguido de ldr do mesmo endereco). Op que
	// nao conhece os caches roda com gcacheOn=false (contexto direto).
	// Trampolins de memoria (modo jit_armv8_a) salvam x2-x9 e gravam os sujos.
	struct GSlot { int reg; bool dirty; u32 lru; };
	GSlot gslot[7];
	u32 gclock = 0;
	u8 glocked = 0;
	bool gcacheOn = true;
	static Register GS(int i) { return Register::GetWRegFromCode(2 + i); }
	void GReset()
	{
		for (auto &g : gslot) { g.reg = -1; g.dirty = false; g.lru = 0; }
		glocked = 0;
	}
	int GFind(int reg)
	{
		for (int i = 0; i < 7; i++)
			if (gslot[i].reg == reg)
				return i;
		return -1;
	}
	void GSpill(int i)
	{
		if (gslot[i].reg >= 0 && gslot[i].dirty)
		{
			Str(GS(i), Ctx(GetRegPtr(gslot[i].reg)));
			gslot[i].dirty = false;
		}
	}
	int GAlloc(int reg)
	{
		int best = -1;
		for (int i = 0; i < 7 && best < 0; i++)
			if (gslot[i].reg < 0 && !(glocked & (1 << i)))
				best = i;
		if (best < 0)
		{
			u32 oldest = ~0u;
			for (int i = 0; i < 7; i++)
				if (!(glocked & (1 << i)) && gslot[i].lru < oldest)
				{
					oldest = gslot[i].lru;
					best = i;
				}
		}
		verify(best >= 0);
		GSpill(best);
		gslot[best].reg = reg;
		gslot[best].dirty = false;
		return best;
	}
	Register GRd(int reg)
	{
		int i = GFind(reg);
		if (i < 0)
		{
			i = GAlloc(reg);
			Ldr(GS(i), Ctx(GetRegPtr(reg)));
		}
		gslot[i].lru = ++gclock;
		glocked |= 1 << i;
		return GS(i);
	}
	Register GWr(int reg)
	{
		int i = GFind(reg);
		if (i < 0)
			i = GAlloc(reg);
		gslot[i].dirty = true;
		gslot[i].lru = ++gclock;
		glocked |= 1 << i;
		return GS(i);
	}
	void GFlushAll(bool invalidate)
	{
		for (int i = 0; i < 7; i++)
		{
			GSpill(i);
			if (invalidate)
				gslot[i].reg = -1;
		}
	}
	void GInvalidate(int reg)
	{
		int i = GFind(reg);
		if (i >= 0)
		{
			gslot[i].reg = -1;
			gslot[i].dirty = false;
		}
	}
	// destino inteiro: fixo, slot do cache, ou rascunho + GCommit (cache off)
	Register GDst(Sh4RegType reg)
	{
		if (IsPinned(reg))
			return Pinned(reg);
		if (gcacheOn)
			return GWr(reg);
		return ScratchR(12);
	}
	void GCommit(Sh4RegType reg, const Register &val)
	{
		if (!IsPinned(reg) && !gcacheOn)
			Str(val, Ctx(reg));
	}

	void LoadGPR(Sh4RegType reg, const Register &dst)
	{
		if (IsPinned(reg))
			Mov(dst, Pinned(reg));
		else if (gcacheOn)
			Mov(dst, GRd(reg));
		else
			Ldr(dst, Ctx(reg));
	}
	Register GPR(Sh4RegType reg, const Register &scratch)
	{
		if (IsPinned(reg))
			return Pinned(reg);
		if (gcacheOn)
			return GRd(reg);
		Ldr(scratch, Ctx(reg));
		return scratch;
	}
	// Le um param de 32 bits (int fixo/cache no host ou float no contexto/imediato).
	void LoadParam32(shil_param &p, const Register &tmp)
	{
		if (p.is_imm())
			Mov(tmp, p._imm);
		else if (p.is_r32i() && IsPinned(p._reg))
			Mov(tmp, Pinned(p._reg));
		else if (p.is_r32i() && gcacheOn)
			Mov(tmp, GRd(p._reg));
		else
			Ldr(tmp, Ctx(p.reg_ptr()));
	}
	void StoreParam32(shil_param &p, const Register &src)
	{
		if (p.is_r32i() && IsPinned(p._reg))
			Mov(Pinned(p._reg), src);
		else if (p.is_r32i() && gcacheOn)
			Mov(GWr(p._reg), src);
		else
			Str(src, Ctx(p.reg_ptr()));
	}
	void LoadParam64(shil_param &p, const Register &tmpX)
	{
		if (p.is_imm())
			Ldr(tmpX, (u64)p._imm);
		else
			Ldr(tmpX, Ctx(p.reg_ptr()));
	}
	void StoreParam64(shil_param &p, const Register &srcX)
	{
		Str(srcX, Ctx(p.reg_ptr()));
	}

	void LoadPinned()
	{
		for (int i = 0; i < kPinnedCount; i++)
			Ldr(Register::GetWRegFromCode(kPinnedBase + i), Ctx(GetRegPtr(reg_r0 + i)));
	}
	void StorePinned()
	{
		for (int i = 0; i < kPinnedCount; i++)
			Str(Register::GetWRegFromCode(kPinnedBase + i), Ctx(GetRegPtr(reg_r0 + i)));
	}

	// ---- chamadas/saltos absolutos (endereco real, nao buffer) ---------

	// x13 = base da memoria emulada (contexto + sizeof(Sh4Context)), como no
	// backend antigo: o fault handler e os trampolins de rec_arm64.cpp
	// reconhecem ldr/str [x13, wN, uxtw]. Chamada C++ ou rotulo invalidam.
	bool memBaseValid = false;
	void EnsureMemBase()
	{
		if (!memBaseValid)
		{
			Add(x13, x28, sizeof(Sh4Context));
			memBaseValid = true;
		}
	}
	using MacroAssembler::Bind;
	void Bind(Label *label) { memBaseValid = false; MacroAssembler::Bind(label); }

	void CallRuntime(void *fn)
	{
		memBaseValid = false;
		ptrdiff_t offset = (uintptr_t)fn - (uintptr_t)CC_RW2RX(GetBuffer()->GetStartAddress<void *>());
		verify(offset >= -128 * 1024 * 1024 && offset <= 128 * 1024 * 1024);
		verify((offset & 3) == 0);
		Label l;
		BindToOffset(&l, offset);
		Bl(&l);
	}
	void BranchAbs(void *target)
	{
		ptrdiff_t offset = (uintptr_t)target - (uintptr_t)CC_RW2RX(GetBuffer()->GetStartAddress<void *>());
		verify(offset >= -128 * 1024 * 1024 && offset <= 128 * 1024 * 1024);
		verify((offset & 3) == 0);
		Label l;
		BindToOffset(&l, offset);
		B(&l);
	}
	// Salto para um bloco (endereco RW dentro do mesmo buffer).
	void BranchRW(void *target, Condition cond = al)
	{
		ptrdiff_t offset = (uintptr_t)target - GetBuffer()->GetStartAddress<uintptr_t>();
		verify(offset >= -128 * 1024 * 1024 && offset < 128 * 1024 * 1024);
		verify((offset & 3) == 0);
		Label l;
		BindToOffset(&l, offset);
		if (cond == al)
			B(&l);
		else
			B(&l, cond);
	}

	// ---- emissao por op -------------------------------------------------

	void GenMemAddr(const shil_opcode &op, const Register &dst)
	{
		if (op.rs1.is_imm())
			Mov(dst, op.rs1._imm);
		else if (op.rs1.is_reg())
			LoadGPR(op.rs1._reg, dst);
		else
			die("GenMemAddr: rs1 invalido");

		if (op.rs3.is_imm())
			Add(dst, dst, op.rs3._imm);
		else if (op.rs3.is_reg())
		{
			Register r3 = GPR(op.rs3._reg, w9);
			Add(dst, dst, r3);
		}
	}

	static Register ScratchR(int n) { return Register::GetWRegFromCode(n); }
	// 's16' (registrador do vixl) colide com o typedef s16 de types.h; usa-se VixlS(n).
	static VRegister VixlS(int n) { return VRegister::GetSRegFromCode(n); }

	void BinaryRRO(shil_opcode &op, void (MacroAssembler::*f)(const Register &, const Register &, const Operand &))
	{
		Register r1 = GPR(op.rs1._reg, w10);
		Operand o2 = op.rs2.is_imm() ? Operand(op.rs2._imm) : Operand(GPR(op.rs2._reg, w11));
		Register rd = GDst(op.rd._reg);
		((*this).*f)(rd, r1, o2);
		GCommit(op.rd._reg, rd);
	}
	void BinaryAddSub(shil_opcode &op, void (MacroAssembler::*f)(const Register &, const Register &, const Operand &, FlagsUpdate))
	{
		Register r1 = GPR(op.rs1._reg, w10);
		Operand o2 = op.rs2.is_imm() ? Operand(op.rs2._imm) : Operand(GPR(op.rs2._reg, w11));
		Register rd = GDst(op.rd._reg);
		((*this).*f)(rd, r1, o2, LeaveFlags);
		GCommit(op.rd._reg, rd);
	}
	void Unary(shil_opcode &op, void (MacroAssembler::*f)(const Register &, const Register &))
	{
		Register r1 = GPR(op.rs1._reg, w10);
		Register rd = GDst(op.rd._reg);
		((*this).*f)(rd, r1);
		GCommit(op.rd._reg, rd);
	}
	void TestSet(shil_opcode &op)
	{
		Register r1 = GPR(op.rs1._reg, w10);
		if (op.op == shop_test)
		{
			if (op.rs2.is_imm())
				Tst(r1, op.rs2._imm);
			else
				Tst(r1, GPR(op.rs2._reg, w11));
		}
		else
		{
			if (op.rs2.is_imm())
				Cmp(r1, op.rs2._imm);
			else
				Cmp(r1, GPR(op.rs2._reg, w11));
		}
		static const Condition conds[] = { eq, eq, ge, gt, hs, hi };
		Condition cond = conds[op.op - shop_test];
		if (op.rd.is_r32i())
		{
			Register rd = GDst(op.rd._reg);
			Cset(rd, cond);
			GCommit(op.rd._reg, rd);
		}
		else
		{
			Cset(ScratchR(12), cond);
			Str(ScratchR(12), Ctx(op.rd.reg_ptr()));
		}
	}

	// kind: 0 = lsl, 1 = lsr, 2 = asr, 3 = ror.
	void ShiftOp(shil_opcode &op, int kind)
	{
		Register r1 = GPR(op.rs1._reg, w10);
		Register r2 = w11;
		if (!op.rs2.is_imm())
			r2 = GPR(op.rs2._reg, w11);
		Register rd = GDst(op.rd._reg);
		if (op.rs2.is_imm())
		{
			int sh = (int)op.rs2._imm;
			switch (kind)
			{
			case 0: Lsl(rd, r1, sh); break;
			case 1: Lsr(rd, r1, sh); break;
			case 2: Asr(rd, r1, sh); break;
			default: Ror(rd, r1, sh); break;
			}
		}
		else
		{
			switch (kind)
			{
			case 0: Lsl(rd, r1, r2); break;
			case 1: Lsr(rd, r1, r2); break;
			case 2: Asr(rd, r1, r2); break;
			default: Ror(rd, r1, r2); break;
			}
		}
		GCommit(op.rd._reg, rd);
	}

	void ExtOp(shil_opcode &op, bool s8)
	{
		Register r1 = GPR(op.rs1._reg, w10);
		Register rd = GDst(op.rd._reg);
		if (s8)
			Sxtb(rd, r1);
		else
			Sxth(rd, r1);
		GCommit(op.rd._reg, rd);
	}

	// Fastmem (2026-09-25): acesso direto ldr/str [x13, wAddr, uxtw] na
	// memoria emulada (mapeamento de 4GB). Fora da RAM o acesso da fault uma
	// vez e ngen_Rewrite troca o site por um trampolim (MMIO, SQ, OCRAM,
	// pagina de codigo protegida) -- o mesmo mecanismo do backend antigo, que
	// no modo jit_armv8_a grava r0-r7 no contexto antes de chamar o C++.
	// FC_NO_COMPACT_MEM=1 volta a chamada C++ em todo acesso.
	static bool CompactUsable(u32 size)
	{
		static int enabled = -1;
		if (enabled == -1)
			enabled = getenv("FC_NO_COMPACT_MEM") != nullptr ? 0 : 1;
		return enabled == 1 && _nvmem_enabled() && _nvmem_4gb_space() && !mmu_enabled()
				&& (size == 1 || size == 2 || size == 4 || size == 8);
	}
	// Endereco em registrador: o fixo direto se nao ha deslocamento, senao w14.
	Register CompactAddr(const shil_opcode &op)
	{
		if (op.rs3.is_null() && op.rs1.is_reg() && op.rs1.is_r32i())
			return GPR(op.rs1._reg, w14);
		GenMemAddr(op, w14);
		return w14;
	}

	void GenReadMemory(shil_opcode &op)
	{
		u32 size = op.flags & 0x7f;
		if (CompactUsable(size) && op.rd.is_reg())
		{
			Register addr = CompactAddr(op);
			EnsureMemBase();
			MemOperand mo(x13, addr, UXTW);
			if (size == 8)
			{
				FRecordSite();
				Ldr(x15, mo);
				if (op.rd.is_r64f())
				{
					Fmov(FWr(op.rd._reg), w15);
					Lsr(x15, x15, 32);
					Fmov(FWr(op.rd._reg + 1), w15);
				}
				else
				{
					FInvalidateRange(op.rd._reg, 2);
					StoreParam64(op.rd, x15);
				}
				return;
			}
			if (op.rd.is_r32f())
			{
				FRecordSite();
				Ldr(w15, mo);
				Fmov(FWr(op.rd._reg), w15);
				return;
			}
			const bool zx = (op.flags2 & 1) != 0;
			Register dst = w15;
			if (op.rd.is_r32i() && IsPinned(op.rd._reg))
				dst = Pinned(op.rd._reg);
			FRecordSite();
			switch (size)
			{
			case 1: if (zx) Ldrb(dst, mo); else Ldrsb(dst, mo); break;
			case 2: if (zx) Ldrh(dst, mo); else Ldrsh(dst, mo); break;
			default: Ldr(dst, mo); break;
			}
			if (!dst.Is(w15))
				return;
			StoreParam32(op.rd, w15);
			return;
		}
		GenMemAddr(op, w0);
		switch (size)
		{
		case 1: CallRuntime((void *)ReadMem8); if (!(op.flags2 & 1)) Sxtb(w0, w0); break;
		case 2: CallRuntime((void *)ReadMem16); if (!(op.flags2 & 1)) Sxth(w0, w0); break;
		case 4: CallRuntime((void *)ReadMem32); break;
		case 8: CallRuntime((void *)ReadMem64); break;
		default: die("GenReadMemory: tamanho invalido");
		}
		if (size == 8)
			StoreParam64(op.rd, x0);
		else
			StoreParam32(op.rd, w0);
	}
	void GenWriteMemory(shil_opcode &op)
	{
		u32 size = op.flags & 0x7f;
		if (CompactUsable(size) && (size != 8 || op.rs2.is_reg()))
		{
			Register addr = CompactAddr(op);
			if (size == 8)
			{
				if (op.rs2.is_r64f())
				{
					Fmov(w15, FRd(op.rs2._reg));
					Fmov(w9, FRd(op.rs2._reg + 1));
					Orr(x15, x15, Operand(x9, LSL, 32));
				}
				else
					LoadParam64(op.rs2, x15);
				EnsureMemBase();
				FRecordSite();
				Str(x15, MemOperand(x13, addr, UXTW));
				return;
			}
			Register data = w15;
			if (op.rs2.is_reg() && op.rs2.is_r32i())
				data = GPR(op.rs2._reg, w15);
			else if (op.rs2.is_r32f())
				Fmov(w15, FRd(op.rs2._reg));
			else
				LoadParam32(op.rs2, w15);
			EnsureMemBase();
			FRecordSite();
			MemOperand mo(x13, addr, UXTW);
			switch (size)
			{
			case 1: Strb(data, mo); break;
			case 2: Strh(data, mo); break;
			default: Str(data, mo); break;
			}
			return;
		}
		GenMemAddr(op, w0);
		// Sincroniza os fixos com o contexto antes da escrita: o pedido de
		// render (e o FC_STATE_HASH) leem o Sh4Context no meio do bloco, e sem
		// isso r0-r7 ficariam com valor velho. Custo aceitavel nesta versao.
		StorePinned();
		if (size == 8)
		{
			LoadParam64(op.rs2, x1);
			CallRuntime((void *)bm_WriteMemCodePage64);
		}
		else
		{
			LoadParam32(op.rs2, w1);
			switch (size)
			{
			case 1: CallRuntime((void *)bm_WriteMemCodePage8); break;
			case 2: CallRuntime((void *)bm_WriteMemCodePage16); break;
			case 4: CallRuntime((void *)bm_WriteMemCodePage32); break;
			default: die("GenWriteMemory: tamanho invalido");
			}
		}
		LoadPinned();
	}

	void CheckBlock(bool force_checks, RuntimeBlockInfo *blk);

	// ---- FPU nativa (mesmas instrucoes NEON do backend antigo, para o
	// resultado ser bit a bit igual; o fallback canonico C++ diverge em
	// FMA/fixNaN).
	//
	// Cache de ponto flutuante (2026-09-25): os floats do SH4 (fr/xf) ficam em
	// v16-v31 dentro do bloco, com escrita adiada (sujo -> contexto so quando
	// sai do cache, no fim do bloco ou antes de op que nao conhece o cache).
	// LRU; as fontes da op corrente ficam travadas. v0-v5 sao rascunho.
	// Acesso a memoria registra os slots vivos (o trampolim salva) e os sujos
	// com o offset no contexto (o trampolim de MMIO grava antes do C++).
	struct FSlot { int reg; bool dirty; u32 lru; };
	FSlot fslot[16];
	u32 fclock = 0;
	u16 flocked = 0;
	static bool IsFReg(int reg) { return reg >= reg_fr_0 && reg <= reg_xf_15; }
	static VRegister FS(int slot) { return VRegister::GetSRegFromCode(16 + slot); }
	void FReset()
	{
		for (auto &f : fslot) { f.reg = -1; f.dirty = false; f.lru = 0; }
		flocked = 0;
	}
	int FFind(int reg)
	{
		for (int i = 0; i < 16; i++)
			if (fslot[i].reg == reg)
				return i;
		return -1;
	}
	void FSpill(int i)
	{
		if (fslot[i].reg >= 0 && fslot[i].dirty)
		{
			Str(FS(i), Ctx(GetRegPtr(fslot[i].reg)));
			fslot[i].dirty = false;
		}
	}
	int FAlloc(int reg)
	{
		int best = -1;
		for (int i = 0; i < 16 && best < 0; i++)
			if (fslot[i].reg < 0 && !(flocked & (1 << i)))
				best = i;
		if (best < 0)
		{
			u32 oldest = ~0u;
			for (int i = 0; i < 16; i++)
				if (!(flocked & (1 << i)) && fslot[i].lru < oldest)
				{
					oldest = fslot[i].lru;
					best = i;
				}
		}
		verify(best >= 0);
		FSpill(best);
		fslot[best].reg = reg;
		fslot[best].dirty = false;
		return best;
	}
	VRegister FRd(int reg)
	{
		int i = FFind(reg);
		if (i < 0)
		{
			i = FAlloc(reg);
			Ldr(FS(i), Ctx(GetRegPtr(reg)));
		}
		fslot[i].lru = ++fclock;
		flocked |= 1 << i;
		return FS(i);
	}
	VRegister FWr(int reg)
	{
		int i = FFind(reg);
		if (i < 0)
			i = FAlloc(reg);
		fslot[i].dirty = true;
		fslot[i].lru = ++fclock;
		flocked |= 1 << i;
		return FS(i);
	}
	// operando de 32 bits: imediato vai para um rascunho (bits exatos)
	VRegister FRdP(shil_param &p, const VRegister &scratch)
	{
		if (p.is_imm())
		{
			Mov(w9, p._imm);
			Fmov(scratch, w9);
			return scratch;
		}
		return FRd(p._reg);
	}
	void FFlushAll(bool invalidate)
	{
		for (int i = 0; i < 16; i++)
		{
			FSpill(i);
			if (invalidate)
				fslot[i].reg = -1;
		}
	}
	void FFlushRange(int reg, int n)
	{
		for (int rr = reg; rr < reg + n; rr++)
		{
			int i = FFind(rr);
			if (i >= 0)
				FSpill(i);
		}
	}
	void FInvalidateRange(int reg, int n)
	{
		for (int rr = reg; rr < reg + n; rr++)
		{
			int i = FFind(rr);
			if (i >= 0)
			{
				fslot[i].reg = -1;
				fslot[i].dirty = false;
			}
		}
	}
	// antes de emitir um ldr/str da memoria emulada
	void FRecordSite()
	{
		u16 live = 0;
		u8 codes[32];
		u16 offs[32];
		int n = 0;
		for (int i = 0; i < 16; i++)
		{
			if (fslot[i].reg < 0)
				continue;
			live |= 1 << i;
			if (fslot[i].dirty)
			{
				codes[n] = (u8)(16 + i);	// s16-s31
				offs[n] = (u16)Ctx(GetRegPtr(fslot[i].reg)).GetOffset();
				n++;
			}
		}
		for (int i = 0; i < 7; i++)
			if (gslot[i].reg >= 0 && gslot[i].dirty)
			{
				codes[n] = (u8)(0x80 | (2 + i));	// w2-w8
				offs[n] = (u16)Ctx(GetRegPtr(gslot[i].reg)).GetOffset();
				n++;
			}
		if (live != 0 || n != 0)
			jit_armv8a_record_site((uintptr_t)CC_RW2RX(GetCursorAddress<void *>()), live, codes, offs, n);
	}
	static bool ParamTouchesFP(const shil_param &p)
	{
		return p.is_r32f() || p.is_r64f() || p.is_vector();
	}
	static bool OpTouchesFP(const shil_opcode &op)
	{
		return ParamTouchesFP(op.rd) || ParamTouchesFP(op.rd2) || ParamTouchesFP(op.rs1)
				|| ParamTouchesFP(op.rs2) || ParamTouchesFP(op.rs3);
	}

	void FBinOp(shil_opcode &op, int kind)
	{
		VRegister a = FRdP(op.rs1, VixlS(0));
		VRegister b = FRdP(op.rs2, VixlS(1));
		VRegister d = FWr(op.rd._reg);
		switch (kind)
		{
		case 0: Fadd(d, a, b); break;
		case 1: Fsub(d, a, b); break;
		case 2: Fmul(d, a, b); break;
		default: Fdiv(d, a, b); break;
		}
	}
	void FUnOp(shil_opcode &op, int kind)
	{
		VRegister a = FRdP(op.rs1, VixlS(0));
		VRegister d = FWr(op.rd._reg);
		switch (kind)
		{
		case 0: Fabs(d, a); break;
		case 1: Fneg(d, a); break;
		case 2: Fsqrt(d, a); break;
		default:	// fsrra = 1/sqrt
			Fsqrt(VixlS(2), a);
			Fmov(VixlS(1), 1.f);
			Fdiv(d, VixlS(1), VixlS(2));
			break;
		}
	}
	void FCmpSet(shil_opcode &op)
	{
		VRegister a = FRdP(op.rs1, VixlS(0));
		VRegister b = FRdP(op.rs2, VixlS(1));
		Fcmp(a, b);
		Condition cond = op.op == shop_fsetgt ? gt : eq;
		if (op.rd.is_r32i())
		{
			Register rd = GDst(op.rd._reg);
			Cset(rd, cond);
			GCommit(op.rd._reg, rd);
		}
		else
		{
			Cset(ScratchR(12), cond);
			Str(ScratchR(12), Ctx(op.rd.reg_ptr()));
		}
	}

	struct CC_PS { CanonicalParamType type; shil_param *prm; };
	std::vector<CC_PS> ccPars;
};

static Armv8AAssembler *g_compiler;

// ---------------------------------------------------------------------------
// Compilacao
// ---------------------------------------------------------------------------

void Armv8AAssembler::CheckBlock(bool force_checks, RuntimeBlockInfo *blk)
{
	if (!force_checks)
		return;

	Label fail;
	s32 sz = blk->sh4_code_size;
	u8 *ptr = GetMemPtr(blk->addr, sz);
	if (ptr != nullptr)
	{
		Ldr(x9, (uintptr_t)ptr);
		while (sz > 0)
		{
			if (sz >= 8)
			{
				Ldr(x10, MemOperand(x9, 8, PostIndex));
				Ldr(x11, *(u64 *)ptr);
				Cmp(x10, x11);
				sz -= 8;
				ptr += 8;
			}
			else if (sz >= 4)
			{
				Ldr(w10, MemOperand(x9, 4, PostIndex));
				Ldr(w11, *(u32 *)ptr);
				Cmp(w10, w11);
				sz -= 4;
				ptr += 4;
			}
			else
			{
				Ldrh(w10, MemOperand(x9, 2, PostIndex));
				Mov(w11, *(u16 *)ptr);
				Cmp(w10, w11);
				sz -= 2;
				ptr += 2;
			}
			B(&fail, ne);
		}
	}
	Label success;
	B(&success);
	Bind(&fail);
	StorePinned();
	Mov(w0, blk->addr);
	BranchAbs((void *)ngen_blockcheckfail);
	Bind(&success);
}

void Armv8AAssembler::Compile(RuntimeBlockInfo *blk, bool force_checks, bool, bool, bool optimise)
{
	this->block = blk;

	// entrada fria (block->code): carrega os fixos
	LoadPinned();
	verify(GetBuffer()->GetCursorOffset() == kColdEntryBytes);
	// entrada quente (blocos ligados direto): os fixos ja estao em x19-x26

	CheckBlock(force_checks, blk);

	// Truques de ciclos deste fork: pulo do laco de atraso / avanco ate o
	// evento. Leem (e o delay skip escreve) r4/r[reg] no contexto.
	if (blk->delay_skip)
	{
		StorePinned();
		Mov(w0, blk->vaddr);
		Mov(w1, blk->guest_cycles);
		CallRuntime((void *)sh4_delay_loop_skip);
		LoadPinned();
	}
	if (blk->idle_fastforward)
	{
		if (blk->idle_ff_ram_reg != 0)
		{
			StorePinned();
			Mov(w0, blk->idle_ff_ram_reg - 1);
			CallRuntime((void *)sh4_sched_idle_fastforward_if_ram);
		}
		else
			CallRuntime((void *)sh4_sched_idle_fastforward);
		Mov(w27, 0);
	}

	// Checagem de ciclos uma vez por bloco.
	Subs(w27, w27, blk->guest_cycles);
	Label cycles_remaining;
	B(&cycles_remaining, pl);
	StorePinned();
	CallRuntime((void *)jit_armv8a_intc_sched());
	Label cpu_running;
	Cbnz(w0, &cpu_running);
	Mov(w29, blk->vaddr);
	Str(w29, Ctx(&next_pc));
	BranchAbs(jit_armv8a_no_update());
	Bind(&cpu_running);
	LoadPinned();
	Bind(&cycles_remaining);

	if (jit_trace_enabled())
	{
		StorePinned();
		Mov(w0, blk->vaddr);
		Mov(w1, 0);
		CallRuntime((void *)jit_trace_block);
	}

	FReset();
	GReset();

	// FC_ARMV8A_ALL_FALLBACK=1 (diagnostico): manda as ops de ALU para o
	// fallback canonico, deixando nativas so as que nao tem implementacao
	// canonica (mov/readm/writem/ifb/jdyn/jcond). Bisseccao do JIT novo.
	static const bool allFallback = getenv("FC_ARMV8A_ALL_FALLBACK") != nullptr;

	for (size_t i = 0; i < blk->oplist.size(); i++)
	{
		shil_opcode &op = blk->oplist[i];
		flocked = 0;
		glocked = 0;
		gcacheOn = true;
		// Quem nao conhece o cache de FP: descarrega e invalida antes (le/grava
		// o contexto ou chama C++, que suja v16-v31).
		{
			bool aware;
			switch (op.op)
			{
			case shop_fadd: case shop_fsub: case shop_fmul: case shop_fdiv:
			case shop_fabs: case shop_fneg: case shop_fsqrt: case shop_fsrra:
			case shop_fmac: case shop_fsetgt: case shop_fseteq:
			case shop_cvt_f2i_t: case shop_cvt_i2f_n: case shop_cvt_i2f_z:
			case shop_mov32: case shop_fipr: case shop_ftrv: case shop_fsca:
				aware = true;
				break;
			case shop_mov64:
				aware = !op.rs1.is_imm();
				break;
			case shop_pref:
				aware = op.rs1.is_reg() && op.rs1.is_r32i() && !mmu_enabled();
				break;
			case shop_readm:
				aware = CompactUsable(op.flags & 0x7f) && op.rd.is_reg();
				break;
			case shop_writem:
				aware = CompactUsable(op.flags & 0x7f) && ((op.flags & 0x7f) != 8 || op.rs2.is_reg());
				break;
			case shop_neg: case shop_not: case shop_and: case shop_or: case shop_xor:
			case shop_add: case shop_sub: case shop_shl: case shop_shr: case shop_sar:
			case shop_ror: case shop_ext_s8: case shop_ext_s16: case shop_test:
			case shop_seteq: case shop_setge: case shop_setgt: case shop_setae:
			case shop_setab: case shop_mul_i32: case shop_jdyn: case shop_jcond:
				aware = !OpTouchesFP(op);
				break;
			default:
				aware = false;
				break;
			}
			if (allFallback || !aware)
			{
				FFlushAll(true);
				GFlushAll(true);
				gcacheOn = false;
			}
		}
		if (allFallback)
		{
			switch (op.op)
			{
			case shop_ifb:
			case shop_jdyn:
			case shop_jcond:
			case shop_mov32:
			case shop_mov64:
			case shop_readm:
			case shop_writem:
			case shop_fadd:
			case shop_fsub:
			case shop_fmul:
			case shop_fdiv:
			case shop_fabs:
			case shop_fneg:
			case shop_fsqrt:
			case shop_fsrra:
			case shop_fmac:
			case shop_fsetgt:
			case shop_fseteq:
			case shop_fsca:
			case shop_fipr:
			case shop_ftrv:
			case shop_frswap:
			case shop_cvt_f2i_t:
			case shop_cvt_i2f_n:
			case shop_cvt_i2f_z:
				break;
			default:
				StorePinned();
				shil_chf[op.op](&op);
				LoadPinned();
				continue;
			}
		}
		switch (op.op)
		{
		case shop_ifb:
			if (op.rs1._imm)
			{
				Mov(w9, op.rs2._imm);
				Str(w9, Ctx(&next_pc));
			}
			StorePinned();
			Mov(w0, op.rs3._imm);
			CallRuntime((void *)OpDesc[op.rs3._imm]->oph);
			LoadPinned();
			break;

		case shop_jdyn:
		case shop_jcond:
			if (op.rs1.is_reg())
				LoadGPR(op.rs1._reg, w29);
			else
				die("jdyn/jcond: rs1 invalido");
			if (op.rs2.is_imm())
				Add(w29, w29, op.rs2._imm);
			// O fim de bloco condicional (has_jcond) le jdyn do contexto; no
			// backend antigo quem gravava era o Cleanup() do regalloc.
			GInvalidate(reg_pc_dyn);
			Str(w29, Ctx(reg_pc_dyn));
			break;

		case shop_mov32:
			if (op.rd.is_r32f() && op.rs1.is_r32f())
			{
				VRegister src = FRd(op.rs1._reg);
				Fmov(FWr(op.rd._reg), src);
			}
			else if (op.rd.is_r32f())
			{
				LoadParam32(op.rs1, w9);
				Fmov(FWr(op.rd._reg), w9);
			}
			else if (op.rs1.is_r32f())
			{
				VRegister src = FRd(op.rs1._reg);
				Fmov(w9, src);
				StoreParam32(op.rd, w9);
			}
			else
			{
				LoadParam32(op.rs1, w9);
				StoreParam32(op.rd, w9);
			}
			break;

		case shop_mov64:
			if (op.rs1.is_r64f() && op.rd.is_r64f())
			{
				Fmov(VixlS(0), FRd(op.rs1._reg));
				Fmov(VixlS(1), FRd(op.rs1._reg + 1));
				Fmov(FWr(op.rd._reg), VixlS(0));
				Fmov(FWr(op.rd._reg + 1), VixlS(1));
			}
			else
			{
				FFlushRange(op.rs1.is_reg() ? op.rs1._reg : 0, op.rs1.is_reg() ? 2 : 0);
				LoadParam64(op.rs1, x9);
				if (op.rd.is_reg())
					FInvalidateRange(op.rd._reg, 2);
				StoreParam64(op.rd, x9);
			}
			break;

		case shop_readm:
			GenReadMemory(op);
			break;
		case shop_writem:
			GenWriteMemory(op);
			break;

		case shop_neg:
		case shop_not:
			{
				Register r1 = GPR(op.rs1._reg, w10);
				Register rd = GDst(op.rd._reg);
				if (op.op == shop_neg)
					Neg(rd, r1);
				else
					Mvn(rd, r1);
				GCommit(op.rd._reg, rd);
			}
			break;
		case shop_and: BinaryRRO(op, &MacroAssembler::And); break;
		case shop_or:  BinaryRRO(op, &MacroAssembler::Orr); break;
		case shop_xor: BinaryRRO(op, &MacroAssembler::Eor); break;
		case shop_add: BinaryAddSub(op, &MacroAssembler::Add); break;
		case shop_sub: BinaryAddSub(op, &MacroAssembler::Sub); break;
		case shop_shl: ShiftOp(op, 0); break;
		case shop_shr: ShiftOp(op, 1); break;
		case shop_sar: ShiftOp(op, 2); break;
		case shop_ror: ShiftOp(op, 3); break;
		case shop_ext_s8: ExtOp(op, true); break;
		case shop_ext_s16: ExtOp(op, false); break;
		case shop_test:
		case shop_seteq:
		case shop_setge:
		case shop_setgt:
		case shop_setae:
		case shop_setab:
			TestSet(op);
			break;
		case shop_mul_i32:
			{
				Register r1 = GPR(op.rs1._reg, w10);
				Register r2 = ScratchR(11);
				if (op.rs2.is_imm())
					Mov(r2, op.rs2._imm);
				else
					r2 = GPR(op.rs2._reg, w11);
				Register rd = GDst(op.rd._reg);
				Mul(rd, r1, r2);
				GCommit(op.rd._reg, rd);
			}
			break;

		case shop_fadd: FBinOp(op, 0); break;
		case shop_fsub: FBinOp(op, 1); break;
		case shop_fmul: FBinOp(op, 2); break;
		case shop_fdiv: FBinOp(op, 3); break;
		case shop_fabs: FUnOp(op, 0); break;
		case shop_fneg: FUnOp(op, 1); break;
		case shop_fsqrt: FUnOp(op, 2); break;
		case shop_fsrra: FUnOp(op, 3); break;
		case shop_fmac:
			{
				VRegister a = FRdP(op.rs1, VixlS(0));
				VRegister b = FRdP(op.rs2, VixlS(1));
				VRegister c = FRdP(op.rs3, VixlS(2));
				Fmadd(FWr(op.rd._reg), c, b, a);
			}
			break;
		case shop_fsetgt:
		case shop_fseteq:
			FCmpSet(op);
			break;
		case shop_fsca:
			{
				Mov(x10, (uintptr_t)&sin_table);
				if (op.rs1.is_reg())
					Add(x10, x10, Operand(GPR(op.rs1._reg, w9), UXTH, 3));
				else
					Add(x10, x10, Operand(op.rs1.imm_value() << 3));
				Ldr(x9, MemOperand(x10));
				FInvalidateRange(op.rd._reg, 2);
				Str(x9, Ctx(op.rd.reg_ptr()));
			}
			break;
		case shop_fipr:
			{
				// vetores lidos do contexto: grava antes o que estiver sujo
				FFlushRange(op.rs1._reg, 4);
				FFlushRange(op.rs2._reg, 4);
				Add(x9, x28, Ctx(op.rs1.reg_ptr()).GetOffset());
				Ld1(v0.V4S(), MemOperand(x9));
				if (op.rs1._reg != op.rs2._reg)
				{
					Add(x9, x28, Ctx(op.rs2.reg_ptr()).GetOffset());
					Ld1(v1.V4S(), MemOperand(x9));
					Fmul(v0.V4S(), v0.V4S(), v1.V4S());
				}
				else
					Fmul(v0.V4S(), v0.V4S(), v0.V4S());
				Faddp(v1.V4S(), v0.V4S(), v0.V4S());
				Faddp(FWr(op.rd._reg), v1.V2S());
			}
			break;
		case shop_ftrv:
			{
				FFlushRange(op.rs1._reg, 4);
				FFlushRange(op.rs2._reg, 16);
				Add(x9, x28, Ctx(op.rs1.reg_ptr()).GetOffset());
				Ld1(v0.V4S(), MemOperand(x9));		// fn
				Add(x9, x28, Ctx(op.rs2.reg_ptr()).GetOffset());
				Ld1(v1.V4S(), MemOperand(x9, 16, PostIndex));	// fm
				Ld1(v2.V4S(), MemOperand(x9, 16, PostIndex));
				Ld1(v3.V4S(), MemOperand(x9, 16, PostIndex));
				Ld1(v4.V4S(), MemOperand(x9, 16, PostIndex));
				Fmul(v5.V4S(), v1.V4S(), VixlS(0), 0);
				Fmla(v5.V4S(), v2.V4S(), VixlS(0), 1);
				Fmla(v5.V4S(), v3.V4S(), VixlS(0), 2);
				Fmla(v5.V4S(), v4.V4S(), VixlS(0), 3);
				FInvalidateRange(op.rd._reg, 4);
				Add(x9, x28, Ctx(op.rd.reg_ptr()).GetOffset());
				St1(v5.V4S(), MemOperand(x9));
			}
			break;
		case shop_frswap:
			{
				Add(x9, x28, Ctx(op.rs1.reg_ptr()).GetOffset());
				Add(x10, x28, Ctx(op.rd.reg_ptr()).GetOffset());
				Ld4(v16.V2D(), v17.V2D(), v18.V2D(), v19.V2D(), MemOperand(x9));
				Ld4(v20.V2D(), v21.V2D(), v22.V2D(), v23.V2D(), MemOperand(x10));
				St4(v20.V2D(), v21.V2D(), v22.V2D(), v23.V2D(), MemOperand(x9));
				St4(v16.V2D(), v17.V2D(), v18.V2D(), v19.V2D(), MemOperand(x10));
			}
			break;
		case shop_cvt_f2i_t:
			{
				VRegister src = FRdP(op.rs1, VixlS(0));
				Register rd = GDst(op.rd._reg);
				Fcvtzs(rd, src);
				GCommit(op.rd._reg, rd);
			}
			break;
		case shop_cvt_i2f_n:
		case shop_cvt_i2f_z:
			{
				Register r1 = GPR(op.rs1._reg, w9);
				Scvtf(FWr(op.rd._reg), r1);
			}
			break;

		case shop_pref:
			if (op.rs1.is_reg() && op.rs1.is_r32i() && !mmu_enabled())
			{
				// Igual ao backend antigo: endereco da SQ -> do_sqw (ponteiro no
				// Sh4RCB); senao prfm. O do_sqw pode acabar em C++ do TA: salva o
				// que esta vivo nos caches (x2-x9 e os v16-v31 com valor).
				Register addr = GPR(op.rs1._reg, w0);
				Label not_sqw, done;
				Lsr(w9, addr, 26);
				Cmp(w9, 0x38);
				B(&not_sqw, ne);
				bool gLive = false;
				for (int k = 0; k < 7; k++)
					gLive |= gslot[k].reg >= 0;
				CPURegList fsave(CPURegister::kVRegister, 64, 0);
				for (int k = 0; k < 16; k++)
					if (fslot[k].reg >= 0)
						fsave.Combine(VRegister::GetDRegFromCode(16 + k));
				if ((fsave.GetCount() % 2) != 0)
					fsave.Combine(d7);
				if (gLive)
				{
					Stp(x2, x3, MemOperand(sp, -64, PreIndex));
					Stp(x4, x5, MemOperand(sp, 16));
					Stp(x6, x7, MemOperand(sp, 32));
					Stp(x8, x9, MemOperand(sp, 48));
				}
				if (!fsave.IsEmpty())
					PushCPURegList(fsave);
				Mov(w0, addr);
				if (CCN_MMUCR.AT)
					Mov(x9, reinterpret_cast<uintptr_t>(&do_sqw_mmu));
				else
				{
					// do_sqw_nommu (macro em sh4_if.h) fica logo antes do sq_buffer no Sh4RCB
					Sub(x9, x28, offsetof(Sh4RCB, cntx) - offsetof(Sh4RCB, sq_buffer) + sizeof(void *));
					Ldr(x9, MemOperand(x9));
					Sub(x1, x28, offsetof(Sh4RCB, cntx) - offsetof(Sh4RCB, sq_buffer));
				}
				MacroAssembler::Blr(x9);
				memBaseValid = false;
				if (!fsave.IsEmpty())
					PopCPURegList(fsave);
				if (gLive)
				{
					Ldp(x4, x5, MemOperand(sp, 16));
					Ldp(x6, x7, MemOperand(sp, 32));
					Ldp(x8, x9, MemOperand(sp, 48));
					Ldp(x2, x3, MemOperand(sp, 64, PostIndex));
				}
				B(&done);
				MacroAssembler::Bind(&not_sqw);
				// prefetch de RAM (como o backend antigo)
				Mov(w1, addr);
				Add(x1, x1, sizeof(Sh4Context));
				Prfm(PLDL1KEEP, MemOperand(x28, x1));
				MacroAssembler::Bind(&done);
				memBaseValid = false;
			}
			else
			{
				StorePinned();
				shil_chf[op.op](&op);
				LoadPinned();
			}
			break;

		case shop_sync_fpscr:
			// UpdateFPSCR (canonica) + guarda PR/SZ igual ao backend antigo:
			// se o PR/SZ nao for mais o que o bloco assumiu, retoma em rs2.
			StorePinned();
			shil_chf[shop_sync_fpscr](&op);
			LoadPinned();
			if (op.rs1.is_imm())
			{
				Label prsz_ok;
				Ldr(w9, Ctx(&fpscr.full));
				And(w9, w9, (1 << 20) | (1 << 19));	// SZ | PR
				Cmp(w9, op.rs1._imm);
				B(&prsz_ok, eq);
				Mov(w29, op.rs2._imm);
				Str(w29, Ctx(&next_pc));
				BranchAbs(jit_armv8a_no_update());
				Bind(&prsz_ok);
			}
			break;

		default:
			// Resto: implementacao canonica C++ (shil_chf). Os fixos vao para
			// o contexto antes e voltam depois, porque as ops canonicas leem e
			// escrevem o Sh4Context.
			StorePinned();
			shil_chf[op.op](&op);
			LoadPinned();
			break;
		}
	}

	// os caches voltam ao contexto antes da saida (o RelinkBlock reemite
	// a saida sem saber deles)
	gcacheOn = true;
	FFlushAll(true);
	GFlushAll(true);

	block->relink_offset = (u32)GetBuffer()->GetCursorOffset();
	block->relink_data = 0;
	RelinkBlock(blk);

	Finalize();
}

u32 Armv8AAssembler::RelinkBlock(RuntimeBlockInfo *blk)
{
	ptrdiff_t start_offset = GetBuffer()->GetCursorOffset();

	// Saida direta para bloco ligado: vai a entrada quente sem passar pelo
	// contexto. Todo o resto (stub de ligacao, despachante, FPCB, C++) grava
	// os fixos antes; a entrada fria do destino recarrega.
	if (jit_trace_enabled())
	{
		StorePinned();
		Mov(w0, blk->vaddr);
		Mov(w1, 1);
		CallRuntime((void *)jit_trace_block);
	}

	switch (blk->BlockType)
	{
	case BET_StaticJump:
	case BET_StaticCall:
		if (blk->pBranchBlock == nullptr)
			CallRuntime(linkStubGeneric);
		else
			BranchRW((u8 *)blk->pBranchBlock->code + kColdEntryBytes);
		break;

	case BET_Cond_0:
	case BET_Cond_1:
		{
			if (blk->has_jcond)
				Ldr(w11, Ctx(&Sh4cntx.jdyn));
			else
				Ldr(w11, Ctx(&sr.T));
			Cmp(w11, blk->BlockType & 1);
			Label branch_not_taken;
			B(ne, &branch_not_taken);
			if (blk->pBranchBlock != nullptr)
				BranchRW((u8 *)blk->pBranchBlock->code + kColdEntryBytes);
			else
				CallRuntime(linkStubCondBranch);
			Bind(&branch_not_taken);
			if (blk->pNextBlock != nullptr)
				BranchRW((u8 *)blk->pNextBlock->code + kColdEntryBytes);
			else
				CallRuntime(linkStubCondNext);
		}
		break;

	case BET_DynamicJump:
	case BET_DynamicCall:
	case BET_DynamicRet:
		StorePinned();
		Str(w29, Ctx(&next_pc));
		{
			Sub(x2, x28, offsetof(Sh4RCB, cntx));
#if RAM_SIZE_MAX == 33554432
			Ubfx(w1, w29, 1, 24);
#else
			Ubfx(w1, w29, 1, 23);
#endif
			Ldr(x15, MemOperand(x2, x1, LSL, 3));
			Br(x15);
		}
		break;

	case BET_DynamicIntr:
	case BET_StaticIntr:
		StorePinned();
		if (blk->BlockType == BET_StaticIntr)
			Mov(w29, blk->NextBlock);
		Str(w29, Ctx(&next_pc));
		CallRuntime((void *)UpdateINTC);
		Ldr(w29, Ctx(&next_pc));
		BranchAbs(jit_armv8a_no_update());
		break;

	default:
		die("jit_armv8_a: tipo de fim de bloco invalido");
	}

	return GetBuffer()->GetCursorOffset() - start_offset;
}

void Armv8AAssembler::GenLinkStubs()
{
	void *const targets[3] = { (void *)ngen_LinkBlock_Generic_stub,
			(void *)ngen_LinkBlock_cond_Branch_stub, (void *)ngen_LinkBlock_cond_Next_stub };
	void **const out[3] = { &linkStubGeneric, &linkStubCondBranch, &linkStubCondNext };
	for (int i = 0; i < 3; i++)
	{
		*out[i] = CC_RW2RX(GetCursorAddress<void *>());	// RX: CallRuntime usa endereco real
		StorePinned();
		BranchAbs(targets[i]);
	}
	FinalizeCode();
	emit_Skip(GetBuffer()->GetSizeInBytes());
	vmem_platform_flush_cache(
		CC_RW2RX(GetBuffer()->GetStartAddress<void *>()), CC_RW2RX(GetBuffer()->GetEndAddress<void *>()),
		GetBuffer()->GetStartAddress<void *>(), GetBuffer()->GetEndAddress<void *>());
}

void Armv8AAssembler::Finalize(bool rewrite)
{
	Label code_end;
	Bind(&code_end);
	FinalizeCode();

	if (!rewrite)
	{
		block->code = GetBuffer()->GetStartAddress<DynarecCodeEntryPtr>();
		block->host_code_size = GetBuffer()->GetSizeInBytes();
		block->host_opcodes = GetLabelAddress<u32 *>(&code_end) - GetBuffer()->GetStartAddress<u32 *>();
		// FC_PERF_MAP: nome dos blocos para o perf (rec_arm64.cpp)
		extern void EmitPerfMapEntry(void *code, u32 size, u32 vaddr);
		EmitPerfMapEntry(CC_RW2RX(block->code), block->host_code_size, block->vaddr);
		{
			static const char *dumpList = getenv("FC_DUMP_BLOCK");
			if (dumpList != nullptr)
			{
				char key[16];
				snprintf(key, sizeof(key), "%08X", block->vaddr);
				if (strcasestr(dumpList, key) != nullptr)
				{
					char path[64];
					snprintf(path, sizeof(path), "/tmp/blockA-%08X.bin", block->vaddr);
					FILE *f = fopen(path, "wb");
					if (f != nullptr)
					{
						fwrite(GetBuffer()->GetStartAddress<void *>(), 1, block->host_code_size, f);
						fclose(f);
					}
					snprintf(path, sizeof(path), "/tmp/blockA-%08X.txt", block->vaddr);
					f = fopen(path, "w");
					if (f != nullptr)
					{
						for (size_t j = 0; j < block->oplist.size(); j++)
							fprintf(f, "%zu: %s\n", j, block->oplist[j].dissasm().c_str());
						fclose(f);
					}
				}
			}
		}
		emit_Skip(block->host_code_size);
	}

	vmem_platform_flush_cache(
		CC_RW2RX(GetBuffer()->GetStartAddress<void *>()), CC_RW2RX(GetBuffer()->GetEndAddress<void *>()),
		GetBuffer()->GetStartAddress<void *>(), GetBuffer()->GetEndAddress<void *>());
}

// ---------------------------------------------------------------------------
// CC (fallback canonico via shil_chf)
// ---------------------------------------------------------------------------

void Armv8AAssembler::CC_Start()
{
	ccPars.clear();
}

void Armv8AAssembler::CC_Param(shil_param &prm, CanonicalParamType tp)
{
	switch (tp)
	{
	case CPT_u32:
	case CPT_ptr:
	case CPT_f32:
		ccPars.push_back({tp, &prm});
		break;
	case CPT_u32rv:
	case CPT_u64rvL:
		// Resultado sempre vai para o contexto; LoadPinned() o recolhe depois.
		Str(w0, Ctx(prm.reg_ptr()));
		break;
	case CPT_u64rvH:
		Lsr(x9, x0, 32);
		Str(w9, Ctx(prm.reg_ptr()));
		break;
	case CPT_f32rv:
		Str(s0, Ctx(prm.reg_ptr()));
		break;
	}
}

void Armv8AAssembler::CC_Call(void *function)
{
	int regused = 0, fregused = 0;
	for (int i = (int)ccPars.size(); i-- > 0;)
	{
		CC_PS &p = ccPars[i];
		switch (p.type)
		{
		case CPT_u32:
			LoadParam32(*p.prm, Register::GetWRegFromCode(regused++));
			break;
		case CPT_f32:
			{
				VRegister fv = VRegister::GetSRegFromCode(fregused++);
				if (p.prm->is_imm())
					Fmov(fv, reinterpret_cast<f32 &>(p.prm->_imm));
				else
					Ldr(fv, Ctx(p.prm->reg_ptr()));
			}
			break;
		case CPT_ptr:
			Mov(Register::GetXRegFromCode(regused++), (uintptr_t)p.prm->reg_ptr());
			break;
		default:
			break;
		}
	}
	CallRuntime(function);
}

// ---------------------------------------------------------------------------
// Interface ngen
// ---------------------------------------------------------------------------

struct DynaRBIA : RuntimeBlockInfo
{
	u32 Relink() override;
	void Relocate(void *) override { verify(false); }
};

u32 DynaRBIA::Relink()
{
	Armv8AAssembler *a = new Armv8AAssembler((u8 *)this->code + this->relink_offset);
	u32 size = a->RelinkBlock(this);
	a->Finalize(true);
	delete a;
	return size;
}

void ngen_GetFeatures_a(ngen_features *dst)
{
	dst->InterpreterFallback = false;
	dst->OnlyDynamicEnds = false;
	// O guard PR/SZ nao e necessario aqui: as ops de FPU caem no fallback
	// canonico, que le o fpscr em runtime. Mantendo o guard ligado as
	// fronteiras de bloco ficam iguais as do backend antigo.
	dst->FpscrGuard = true;
}

RuntimeBlockInfo *ngen_AllocateBlock_a()
{
	return new DynaRBIA();
}

void ngen_Compile_a(RuntimeBlockInfo *block, bool force_checks, bool reset, bool staging, bool optimise)
{
	verify(emit_FreeSpace() >= 16 * 1024);

	if (linkStubGeneric == nullptr)
	{
		Armv8AAssembler *stubs = new Armv8AAssembler();
		stubs->GenLinkStubs();
		delete stubs;
	}

	Armv8AAssembler *compiler = new Armv8AAssembler();
	g_compiler = compiler;
	compiler->Compile(block, force_checks, reset, staging, optimise);
	g_compiler = nullptr;
	delete compiler;
}

void ngen_CC_Start_a(shil_opcode *)
{
	g_compiler->CC_Start();
}
void ngen_CC_Param_a(shil_opcode *, shil_param *par, CanonicalParamType tp)
{
	g_compiler->CC_Param(*par, tp);
}
void ngen_CC_Call_a(shil_opcode *, void *function)
{
	g_compiler->CC_Call(function);
}
void ngen_CC_Finish_a(shil_opcode *)
{
}

#endif /* FEAT_SHREC == DYNAREC_JIT */
