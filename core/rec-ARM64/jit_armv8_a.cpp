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

	void LoadGPR(Sh4RegType reg, const Register &dst)
	{
		if (IsPinned(reg))
			Mov(dst, Pinned(reg));
		else
			Ldr(dst, Ctx(reg));
	}
	Register GPR(Sh4RegType reg, const Register &scratch)
	{
		if (IsPinned(reg))
			return Pinned(reg);
		Ldr(scratch, Ctx(reg));
		return scratch;
	}
	// Le um param de 32 bits (int fixo no host ou float no contexto/imediato).
	void LoadParam32(shil_param &p, const Register &tmp)
	{
		if (p.is_imm())
			Mov(tmp, p._imm);
		else if (p.is_r32i() && IsPinned(p._reg))
			Mov(tmp, Pinned(p._reg));
		else
			Ldr(tmp, Ctx(p.reg_ptr()));
	}
	void StoreParam32(shil_param &p, const Register &src)
	{
		if (p.is_r32i() && IsPinned(p._reg))
			Mov(Pinned(p._reg), src);
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

	void CallRuntime(void *fn)
	{
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
		Register rd = IsPinned(op.rd._reg) ? Pinned(op.rd._reg) : ScratchR(12);
		Register r1 = GPR(op.rs1._reg, w10);
		Operand o2 = op.rs2.is_imm() ? Operand(op.rs2._imm) : Operand(GPR(op.rs2._reg, w11));
		((*this).*f)(rd, r1, o2);
		if (!IsPinned(op.rd._reg))
			Str(rd, Ctx(op.rd._reg));
	}
	void BinaryAddSub(shil_opcode &op, void (MacroAssembler::*f)(const Register &, const Register &, const Operand &, FlagsUpdate))
	{
		Register rd = IsPinned(op.rd._reg) ? Pinned(op.rd._reg) : ScratchR(12);
		Register r1 = GPR(op.rs1._reg, w10);
		Operand o2 = op.rs2.is_imm() ? Operand(op.rs2._imm) : Operand(GPR(op.rs2._reg, w11));
		((*this).*f)(rd, r1, o2, LeaveFlags);
		if (!IsPinned(op.rd._reg))
			Str(rd, Ctx(op.rd._reg));
	}
	void Unary(shil_opcode &op, void (MacroAssembler::*f)(const Register &, const Register &))
	{
		Register rd = IsPinned(op.rd._reg) ? Pinned(op.rd._reg) : ScratchR(12);
		Register r1 = GPR(op.rs1._reg, w10);
		((*this).*f)(rd, r1);
		if (!IsPinned(op.rd._reg))
			Str(rd, Ctx(op.rd._reg));
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
		if (op.rd.is_r32i() && IsPinned(op.rd._reg))
			Cset(Pinned(op.rd._reg), cond);
		else
		{
			Cset(ScratchR(12), cond);
			Str(ScratchR(12), Ctx(op.rd.reg_ptr()));
		}
	}

	// kind: 0 = lsl, 1 = lsr, 2 = asr, 3 = ror.
	void ShiftOp(shil_opcode &op, int kind)
	{
		Register rd = IsPinned(op.rd._reg) ? Pinned(op.rd._reg) : ScratchR(12);
		Register r1 = GPR(op.rs1._reg, w10);
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
			Register r2 = GPR(op.rs2._reg, w11);
			switch (kind)
			{
			case 0: Lsl(rd, r1, r2); break;
			case 1: Lsr(rd, r1, r2); break;
			case 2: Asr(rd, r1, r2); break;
			default: Ror(rd, r1, r2); break;
			}
		}
		if (!IsPinned(op.rd._reg))
			Str(rd, Ctx(op.rd._reg));
	}

	void ExtOp(shil_opcode &op, bool s8)
	{
		Register rd = IsPinned(op.rd._reg) ? Pinned(op.rd._reg) : ScratchR(12);
		Register r1 = GPR(op.rs1._reg, w10);
		if (s8)
			Sxtb(rd, r1);
		else
			Sxth(rd, r1);
		if (!IsPinned(op.rd._reg))
			Str(rd, Ctx(op.rd._reg));
	}

	void GenReadMemory(shil_opcode &op)
	{
		u32 size = op.flags & 0x7f;
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
	// FMA/fixNaN). Floats ficam sempre no contexto; usamos v16-v23 como
	// scratch (caller-saved, mas sem chamadas no meio de uma op).
	VRegister FLoad(shil_param &p, const VRegister &scratch)
	{
		if (p.is_imm())
			Fmov(scratch, reinterpret_cast<f32 &>(p._imm));
		else
			Ldr(scratch, Ctx(p.reg_ptr()));
		return scratch;
	}
	void FStore(shil_param &p, const VRegister &src)
	{
		Str(src, Ctx(p.reg_ptr()));
	}
	void FBinOp(shil_opcode &op, int kind)
	{
		VRegister a = FLoad(op.rs1, VixlS(16));
		VRegister b = FLoad(op.rs2, VixlS(17));
		switch (kind)
		{
		case 0: Fadd(VixlS(18), a, b); break;
		case 1: Fsub(VixlS(18), a, b); break;
		case 2: Fmul(VixlS(18), a, b); break;
		default: Fdiv(VixlS(18), a, b); break;
		}
		FStore(op.rd, VixlS(18));
	}
	void FUnOp(shil_opcode &op, int kind)
	{
		VRegister a = FLoad(op.rs1, VixlS(16));
		switch (kind)
		{
		case 0: Fabs(VixlS(18), a); break;
		case 1: Fneg(VixlS(18), a); break;
		case 2: Fsqrt(VixlS(18), a); break;
		default:	// fsrra = 1/sqrt
			Fsqrt(VixlS(18), a);
			Fmov(VixlS(17), 1.f);
			Fdiv(VixlS(18), VixlS(17), VixlS(18));
			break;
		}
		FStore(op.rd, VixlS(18));
	}
	void FCmpSet(shil_opcode &op)
	{
		VRegister a = FLoad(op.rs1, VixlS(16));
		VRegister b = FLoad(op.rs2, VixlS(17));
		Fcmp(a, b);
		Condition cond = op.op == shop_fsetgt ? gt : eq;
		if (op.rd.is_r32i() && IsPinned(op.rd._reg))
			Cset(Pinned(op.rd._reg), cond);
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
	Mov(w0, blk->addr);
	BranchAbs((void *)ngen_blockcheckfail);
	Bind(&success);
}

void Armv8AAssembler::Compile(RuntimeBlockInfo *blk, bool force_checks, bool, bool, bool optimise)
{
	this->block = blk;

	CheckBlock(force_checks, blk);

	// Truques de ciclos deste fork: pulo do laco de atraso / avanco ate o evento.
	if (blk->delay_skip)
	{
		Mov(w0, blk->vaddr);
		Mov(w1, blk->guest_cycles);
		CallRuntime((void *)sh4_delay_loop_skip);
	}
	if (blk->idle_fastforward)
	{
		if (blk->idle_ff_ram_reg != 0)
		{
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
	CallRuntime((void *)jit_armv8a_intc_sched());
	Label cpu_running;
	Cbnz(w0, &cpu_running);
	Mov(w29, blk->vaddr);
	Str(w29, Ctx(&next_pc));
	BranchAbs(jit_armv8a_no_update());
	Bind(&cpu_running);
	Bind(&cycles_remaining);

	if (jit_trace_enabled())
	{
		Mov(w0, blk->vaddr);
		Mov(w1, 0);
		CallRuntime((void *)jit_trace_block);
	}

	LoadPinned();

	// FC_ARMV8A_ALL_FALLBACK=1 (diagnostico): manda as ops de ALU para o
	// fallback canonico, deixando nativas so as que nao tem implementacao
	// canonica (mov/readm/writem/ifb/jdyn/jcond). Bisseccao do JIT novo.
	static const bool allFallback = getenv("FC_ARMV8A_ALL_FALLBACK") != nullptr;

	for (size_t i = 0; i < blk->oplist.size(); i++)
	{
		shil_opcode &op = blk->oplist[i];
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
			Str(w29, Ctx(reg_pc_dyn));
			break;

		case shop_mov32:
			if (op.rd.is_r32i() && IsPinned(op.rd._reg))
			{
				if (op.rs1.is_imm())
					Mov(Pinned(op.rd._reg), op.rs1._imm);
				else
				{
					LoadParam32(op.rs1, w9);
					Mov(Pinned(op.rd._reg), w9);
				}
			}
			else
			{
				LoadParam32(op.rs1, w9);
				Str(w9, Ctx(op.rd.reg_ptr()));
			}
			break;

		case shop_mov64:
			LoadParam64(op.rs1, x9);
			StoreParam64(op.rd, x9);
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
				Register rd = IsPinned(op.rd._reg) ? Pinned(op.rd._reg) : ScratchR(12);
				Register r1 = GPR(op.rs1._reg, w10);
				if (op.op == shop_neg)
					Neg(rd, r1);
				else
					Mvn(rd, r1);
				if (!IsPinned(op.rd._reg))
					Str(rd, Ctx(op.rd._reg));
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
				Register rd = IsPinned(op.rd._reg) ? Pinned(op.rd._reg) : ScratchR(12);
				Register r1 = GPR(op.rs1._reg, w10);
				Register r2 = ScratchR(11);
				if (op.rs2.is_imm())
					Mov(r2, op.rs2._imm);
				else
					LoadGPR(op.rs2._reg, r2);
				Mul(rd, r1, r2);
				if (!IsPinned(op.rd._reg))
					Str(rd, Ctx(op.rd._reg));
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
				VRegister a = FLoad(op.rs1, VixlS(16));
				VRegister b = FLoad(op.rs2, VixlS(17));
				VRegister c = FLoad(op.rs3, VixlS(18));
				Fmadd(VixlS(19), c, b, a);
				FStore(op.rd, VixlS(19));
			}
			break;
		case shop_fsetgt:
		case shop_fseteq:
			FCmpSet(op);
			break;
		case shop_fsca:
			{
				Mov(x1, (uintptr_t)&sin_table);
				if (op.rs1.is_reg())
					Add(x1, x1, Operand(GPR(op.rs1._reg, w9), UXTH, 3));
				else
					Add(x1, x1, Operand(op.rs1.imm_value() << 3));
				Ldr(x2, MemOperand(x1));
				Str(x2, Ctx(op.rd.reg_ptr()));
			}
			break;
		case shop_fipr:
			{
				Add(x9, x28, Ctx(op.rs1.reg_ptr()).GetOffset());
				Ld1(v16.V4S(), MemOperand(x9));
				if (op.rs1._reg != op.rs2._reg)
				{
					Add(x9, x28, Ctx(op.rs2.reg_ptr()).GetOffset());
					Ld1(v17.V4S(), MemOperand(x9));
					Fmul(v16.V4S(), v16.V4S(), v17.V4S());
				}
				else
					Fmul(v16.V4S(), v16.V4S(), v16.V4S());
				Faddp(v17.V4S(), v16.V4S(), v16.V4S());
				Faddp(VixlS(18), v17.V2S());
				FStore(op.rd, VixlS(18));
			}
			break;
		case shop_ftrv:
			{
				Add(x9, x28, Ctx(op.rs1.reg_ptr()).GetOffset());
				Ld1(v16.V4S(), MemOperand(x9));		// fn
				Add(x9, x28, Ctx(op.rs2.reg_ptr()).GetOffset());
				Ld1(v17.V4S(), MemOperand(x9, 16, PostIndex));	// fm
				Ld1(v18.V4S(), MemOperand(x9, 16, PostIndex));
				Ld1(v19.V4S(), MemOperand(x9, 16, PostIndex));
				Ld1(v20.V4S(), MemOperand(x9, 16, PostIndex));
				Fmul(v21.V4S(), v17.V4S(), VixlS(16), 0);
				Fmla(v21.V4S(), v18.V4S(), VixlS(16), 1);
				Fmla(v21.V4S(), v19.V4S(), VixlS(16), 2);
				Fmla(v21.V4S(), v20.V4S(), VixlS(16), 3);
				Add(x9, x28, Ctx(op.rd.reg_ptr()).GetOffset());
				St1(v21.V4S(), MemOperand(x9));
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
				Register rd = IsPinned(op.rd._reg) ? Pinned(op.rd._reg) : ScratchR(12);
				Fcvtzs(rd, FLoad(op.rs1, VixlS(16)));
				if (!IsPinned(op.rd._reg))
					Str(rd, Ctx(op.rd._reg));
			}
			break;
		case shop_cvt_i2f_n:
		case shop_cvt_i2f_z:
			{
				Register r1 = GPR(op.rs1._reg, w9);
				Scvtf(VixlS(18), r1);
				FStore(op.rd, VixlS(18));
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

	block->relink_offset = (u32)GetBuffer()->GetCursorOffset();
	block->relink_data = 0;
	RelinkBlock(blk);

	Finalize();
}

u32 Armv8AAssembler::RelinkBlock(RuntimeBlockInfo *blk)
{
	ptrdiff_t start_offset = GetBuffer()->GetCursorOffset();

	// Sempre grava os fixos antes de sair: a entrada fria pelo mainloop
	// recarrega do contexto.
	StorePinned();

	if (jit_trace_enabled())
	{
		Mov(w0, blk->vaddr);
		Mov(w1, 1);
		CallRuntime((void *)jit_trace_block);
	}

	switch (blk->BlockType)
	{
	case BET_StaticJump:
	case BET_StaticCall:
		if (blk->pBranchBlock == nullptr)
			CallRuntime((void *)ngen_LinkBlock_Generic_stub);
		else
			BranchRW((void *)blk->pBranchBlock->code);
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
				BranchRW((void *)blk->pBranchBlock->code);
			else
				CallRuntime((void *)ngen_LinkBlock_cond_Branch_stub);
			Bind(&branch_not_taken);
			if (blk->pNextBlock != nullptr)
				BranchRW((void *)blk->pNextBlock->code);
			else
				CallRuntime((void *)ngen_LinkBlock_cond_Next_stub);
		}
		break;

	case BET_DynamicJump:
	case BET_DynamicCall:
	case BET_DynamicRet:
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
