/*
	Copyright 2019 flyinghead

	This file is part of reicast.

    reicast is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    reicast is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with reicast.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "types.h"

#if FEAT_SHREC == DYNAREC_JIT

#include <unistd.h>
#include <map>
#include <setjmp.h>

#include "deps/vixl/aarch64/macro-assembler-aarch64.h"
using namespace vixl::aarch64;

//#define EXPLODE_SPANS

#include "hw/sh4/sh4_opcode_list.h"

// Opt-in interpreter-fallback (shop_ifb) hit counter, by raw 16-bit SH4
// opcode value -- indexed the same way OpDesc[] is, so OpDesc[i]->diss
// gives the disassembly string directly for the dump. Off by default
// (only enabled by setting FC_IFB_COUNT in the environment before
// launching), same opt-in pattern as EmitPerfMapEntry below -- a debug/
// profiling aid, not something a normal player's session needs. See
// docs/tech_debits.md for the question this answers: which SH4 opcodes
// (if any) actually hit the interpreter fallback in real gameplay,
// since ~17 opcodes in the table have neither a hand-written recompiler
// handler nor a generic-decode descriptor and always fall here.
// Opt-in FPSCR counters, defined in sh4_core_regs.cpp next to UpdateFPSCR().
extern u64 g_fpscrWrites;
extern u64 g_fpscrGuardExit;
static bool FpscrStatsEnabled()
{
	static int enabled = -1;
	if (enabled == -1)
		enabled = getenv("FC_FPSCR_STATS") != nullptr ? 1 : 0;
	return enabled == 1;
}

// Opt-in per-block run counter. The x86 backend has always bumped
// block->runs (rec_x86_driver.cpp), the ARM64 one never did, which is why
// there was no way to tell WHICH generated code is hot -- perf only shows one
// anonymous SH4_TCB blob. Report side: bm_DumpHotBlocks() in blockmanager.cpp.
static bool BlockProfEnabled()
{
	static int enabled = -1;
	if (enabled == -1)
		enabled = getenv("FC_BLOCK_PROF") != nullptr ? 1 : 0;
	return enabled == 1;
}

static bool IfbCountEnabled()
{
	static int enabled = -1; // -1: not checked yet, 0: disabled, 1: enabled
	if (enabled == -1)
		enabled = getenv("FC_IFB_COUNT") != nullptr ? 1 : 0;
	return enabled == 1;
}
static u64 g_ifbHitCount[0x10000];
static void ifb_counted_call(u32 op)
{
	g_ifbHitCount[op]++;
	OpDesc[op]->oph(op);
}
// Dumps unconditionally when called -- caller (driver.cpp, gated by
// IfbCountEnabled()) decides when. Overwrites the file each time so the
// latest snapshot is always readable without waiting for a clean
// shutdown (this core's shutdown path is known unreliable, see
// docs/tech_debits.md item 5.3's crash addendum).
void DumpIfbCounts()
{
	char path[64];
	snprintf(path, sizeof(path), "/tmp/ifb-counts-%d.txt", (int)getpid());
	FILE *f = fopen(path, "w");
	if (f == nullptr)
		return;
	for (u32 i = 0; i < 0x10000; i++)
	{
		if (g_ifbHitCount[i] != 0 && OpDesc[i] != nullptr)
			fprintf(f, "%llu\t%04x\t%s\n", (unsigned long long)g_ifbHitCount[i], i, OpDesc[i]->diss);
	}
	fclose(f);
}

#include "hw/sh4/sh4_mmr.h"
#include "hw/sh4/sh4_interrupts.h"
#include "hw/sh4/sh4_core.h"
#include "hw/sh4/dyna/ngen.h"
#include "hw/sh4/sh4_mem.h"
#include <unordered_map>
#include "hw/sh4/sh4_rom.h"
#include "hw/sh4/sh4_sched.h"
#include "hw/mem/vmem32.h"
#include "arm64_regalloc.h"

#undef do_sqw_nommu

extern "C" void ngen_blockcheckfail(u32 pc);
extern "C" void ngen_LinkBlock_Generic_stub();
extern "C" void ngen_LinkBlock_cond_Branch_stub();
extern "C" void ngen_LinkBlock_cond_Next_stub();
extern "C" void ngen_FailedToFindBlock_mmu();
extern "C" void ngen_FailedToFindBlock_nommu();
extern void vmem_platform_flush_cache(void *icache_start, void *icache_end, void *dcache_start, void *dcache_end);
static void generate_mainloop();

struct DynaRBI : RuntimeBlockInfo
{
	virtual u32 Relink() override;

	virtual void Relocate(void* dst) override {
		verify(false);
	}
};

static jmp_buf jmp_env;
static u32 cycle_counter;

static void (*mainloop)(void *context);
static int (*arm64_intc_sched)();
static void (*arm64_no_update)();
// Shared, generated-once stub used by ngen_Rewrite() to special-case SH4
// Store Queue writes (addr>>26==0x38) without adding any check to the common
// fastmem path -- see docs/arm64jit_improvement_plan.md item 1 and
// docs/tech_debits.md item 1.7. Same lifecycle as `mainloop`: regenerated
// alongside it (generate_mainloop()), reset to null in ngen_ResetBlocks().
static void *sq_write_stub;
// Stores into pages holding compiled code, by size 1/2/4/8 (index 0..3). See
// bm_WriteMemCodePage() in blockmanager.cpp and docs/tech_debits.md 4.19.
static void *codepage_write_stub[4];
// Acessos compactos (item 4.27): quais S16-S31 estavam vivos em cada call site
// (endereco RX da instrucao -> mascara, bit i = S16+i). So entra quem tem
// algum; o trampolim do fault salva exatamente esses, como o GenCallRuntime
// faria. Entrada velha de codigo descartado so faz salvar a mais -- nunca a
// menos, porque todo site emitido com mascara != 0 sobrescreve a sua.
static std::unordered_map<uintptr_t, u16> compact_live_fregs;
extern u8 bm_code_page_locked[];
extern u8 bm_code_chunks[];
extern u8 *bm_ram_alias;
void DYNACALL bm_WriteMemCodePage8(u32 addr, u8 data);
void DYNACALL bm_WriteMemCodePage16(u32 addr, u16 data);
void DYNACALL bm_WriteMemCodePage32(u32 addr, u32 data);
void DYNACALL bm_WriteMemCodePage64(u32 addr, u64 data);

static bool restarting;

// Opt-in JIT symbol map for perf/Hotspot/speedscope: none of them can name
// individual compiled SH4 blocks on their own, since the blocks are raw
// machine code written straight into the SH4_TCB buffer at runtime -- they
// never pass through the linker, so there's no ELF symbol table entry for
// any of them (see docs/tech_debits.md item 4.10/4.11 for how much manual
// gdb archaeology that cost this session). `perf script`/`perf report`
// already know to look for a "/tmp/perf-<pid>.map" file (one "addr size
// name" line per symbol) and use it to resolve otherwise-anonymous regions
// -- this is the same mechanism V8/JVM JITs use. Off by default (only
// enabled by setting FC_PERF_MAP in the environment before launching) since
// it's a debug/profiling aid, not something a normal player's session needs.
static void EmitPerfMapEntry(void *code, u32 size, u32 vaddr)
{
	static int enabled = -1; // -1: not checked yet, 0: disabled, 1: enabled
	static FILE *perf_map_file = nullptr;
	if (enabled == -1)
	{
		enabled = getenv("FC_PERF_MAP") != nullptr ? 1 : 0;
		if (enabled)
		{
			char path[64];
			snprintf(path, sizeof(path), "/tmp/perf-%d.map", (int)getpid());
			perf_map_file = fopen(path, "a");
		}
	}
	if (perf_map_file != nullptr)
		fprintf(perf_map_file, "%llx %x SH4_%08x\n", (unsigned long long)(uintptr_t)code, size, vaddr);
}

void ngen_mainloop(void* v_cntx)
{
	do {
		restarting = false;
		generate_mainloop();

		mainloop(v_cntx);
		if (restarting)
			p_sh4rcb->cntx.CpuRunning = 1;
	} while (restarting);
}

void ngen_init()
{
	INFO_LOG(DYNAREC, "Initializing the ARM64 dynarec");
	ngen_FailedToFindBlock = &ngen_FailedToFindBlock_nommu;
}

void ngen_ResetBlocks()
{
	mainloop = NULL;
	compact_live_fregs.clear();
	sq_write_stub = nullptr;
	for (auto& stub : codepage_write_stub)
		stub = nullptr;
	if (mmu_enabled())
		ngen_FailedToFindBlock = &ngen_FailedToFindBlock_mmu;
	else
		ngen_FailedToFindBlock = &ngen_FailedToFindBlock_nommu;
	if (p_sh4rcb->cntx.CpuRunning)
	{
		// Force the dynarec out of mainloop() to regenerate it
		p_sh4rcb->cntx.CpuRunning = 0;
		restarting = true;
	}
}

void ngen_GetFeatures(ngen_features* dst)
{
	dst->InterpreterFallback = false;
	dst->OnlyDynamicEnds     = false;
	dst->FpscrGuard          = true;	// shop_sync_fpscr implements the PR/SZ runtime guard below
}

template<typename T>
static T ReadMemNoEx(u32 addr, u32, u32 pc)
{
#ifndef NO_MMU
	u32 ex;
	T rv = mmu_ReadMemNoEx<T>(addr, &ex);
	if (ex)
	{
		spc = pc;
		longjmp(jmp_env, 1);
	}
	return rv;
#else
	return (T)0;	// not used
#endif
}

template<typename T>
static void WriteMemNoEx(u32 addr, T data, u32 pc)
{
#ifndef NO_MMU
	u32 ex = mmu_WriteMemNoEx<T>(addr, data);
	if (ex)
	{
		spc = pc;
		longjmp(jmp_env, 1);
	}
#endif
}

static void interpreter_fallback(u16 op, OpCallFP *oph, u32 pc)
{
	try {
		oph(op);
	} catch (SH4ThrownException& ex) {
		if (pc & 1)
		{
			// Delay slot
			AdjustDelaySlotException(ex);
			pc--;
		}
		Do_Exception(pc, ex.expEvn, ex.callVect);
		longjmp(jmp_env, 1);
	}
}

static void do_sqw_mmu_no_ex(u32 addr, u32 pc)
{
	try {
		do_sqw_mmu(addr);
	} catch (SH4ThrownException& ex) {
		if (pc & 1)
		{
			// Delay slot
			AdjustDelaySlotException(ex);
			pc--;
		}
		Do_Exception(pc, ex.expEvn, ex.callVect);
		longjmp(jmp_env, 1);
	}
}

class Arm64Assembler : public MacroAssembler
{
	typedef void (MacroAssembler::*Arm64Op_RRO)(const Register&, const Register&, const Operand&);
	typedef void (MacroAssembler::*Arm64Op_RROF)(const Register&, const Register&, const Operand&, enum FlagsUpdate);
	typedef void (MacroAssembler::*Arm64Fop_RRR)(const VRegister&, const VRegister&, const VRegister&);

public:
	Arm64Assembler() : Arm64Assembler(emit_GetCCPtr())
	{
	}

	// x13 = base da RAM emulada (x28 + sizeof(Sh4Context)) para os acessos
	// compactos (item 4.27). E caller-saved: qualquer chamada pode destrui-lo,
	// e numa juncao de fluxo nao se sabe por onde se chegou. Entao vale so ate
	// o proximo Bl/Blr ou Bind -- estes wrappers escondem os do MacroAssembler
	// dentro desta classe, que e de onde todo o codigo do bloco e emitido.
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
	using MacroAssembler::Bl;
	void Bl(Label *label) { memBaseValid = false; MacroAssembler::Bl(label); }
	void Blr(const Register& xn) { memBaseValid = false; MacroAssembler::Blr(xn); }
	Arm64Assembler(void *buffer) : MacroAssembler((u8 *)buffer, emit_FreeSpace()), regalloc(this)
	{
		call_regs.push_back(&w0);
		call_regs.push_back(&w1);
		call_regs.push_back(&w2);
		call_regs.push_back(&w3);
		call_regs.push_back(&w4);
		call_regs.push_back(&w5);
		call_regs.push_back(&w6);
		call_regs.push_back(&w7);

		call_regs64.push_back(&x0);
		call_regs64.push_back(&x1);
		call_regs64.push_back(&x2);
		call_regs64.push_back(&x3);
		call_regs64.push_back(&x4);
		call_regs64.push_back(&x5);
		call_regs64.push_back(&x6);
		call_regs64.push_back(&x7);

		call_fregs.push_back(&s0);
		call_fregs.push_back(&s1);
		call_fregs.push_back(&s2);
		call_fregs.push_back(&s3);
		call_fregs.push_back(&s4);
		call_fregs.push_back(&s5);
		call_fregs.push_back(&s6);
		call_fregs.push_back(&s7);
	}

	void ngen_BinaryOp_RRO(shil_opcode* op, Arm64Op_RRO arm_op, Arm64Op_RROF arm_op2)
	{
		Operand op3 = Operand(0);
		if (op->rs2.is_imm())
		{
			op3 = Operand(op->rs2._imm);
		}
		else if (op->rs2.is_r32i())
		{
			op3 = Operand(regalloc.MapRegister(op->rs2));
		}
		if (arm_op != NULL)
			((*this).*arm_op)(regalloc.MapRegister(op->rd), regalloc.MapRegister(op->rs1), op3);
		else
			((*this).*arm_op2)(regalloc.MapRegister(op->rd), regalloc.MapRegister(op->rs1), op3, LeaveFlags);
	}

	void ngen_BinaryFop(shil_opcode* op, Arm64Fop_RRR arm_op)
	{
		VRegister reg1;
		VRegister reg2;
		if (op->rs1.is_imm())
		{
			Fmov(s0, reinterpret_cast<f32&>(op->rs1._imm));
			reg1 = s0;
		}
		else
		{
			reg1 = regalloc.MapVRegister(op->rs1);
		}
		if (op->rs2.is_imm())
		{
			Fmov(s1, reinterpret_cast<f32&>(op->rs2._imm));
			reg2 = s1;
		}
		else
		{
			reg2 = regalloc.MapVRegister(op->rs2);
		}
		((*this).*arm_op)(regalloc.MapVRegister(op->rd), reg1, reg2);
	}

	const Register& GenMemAddr(const shil_opcode& op, const Register* raddr = NULL)
	{
		const Register* ret_reg = raddr == NULL ? &w0 : raddr;

		if (op.rs3.is_imm())
		{
			if (regalloc.IsAllocg(op.rs1))
				Add(*ret_reg, regalloc.MapRegister(op.rs1), op.rs3._imm);
			else
			{
				Ldr(*ret_reg, sh4_context_mem_operand(op.rs1.reg_ptr()));
				Add(*ret_reg, *ret_reg, op.rs3._imm);
			}
		}
		else if (op.rs3.is_r32i())
		{
			if (regalloc.IsAllocg(op.rs1) && regalloc.IsAllocg(op.rs3))
				Add(*ret_reg, regalloc.MapRegister(op.rs1), regalloc.MapRegister(op.rs3));
			else
			{
				Ldr(*ret_reg, sh4_context_mem_operand(op.rs1.reg_ptr()));
				Ldr(w8, sh4_context_mem_operand(op.rs3.reg_ptr()));
				Add(*ret_reg, *ret_reg, w8);
			}
		}
		else if (!op.rs3.is_null())
		{
			die("invalid rs3");
		}
		else if (op.rs1.is_reg())
		{
			if (regalloc.IsAllocg(op.rs1))
			{
				if (raddr == NULL)
					ret_reg = &regalloc.MapRegister(op.rs1);
				else
					Mov(*ret_reg, regalloc.MapRegister(op.rs1));
			}
			else
			{
				Ldr(*ret_reg, sh4_context_mem_operand(op.rs1.reg_ptr()));
			}
		}
		else
		{
			verify(op.rs1.is_imm());
			Mov(*ret_reg, op.rs1._imm);
		}

		return *ret_reg;
	}

	void ngen_Compile(RuntimeBlockInfo* block, bool force_checks, bool reset, bool staging, bool optimise)
	{
		//printf("REC-ARM64 compiling %08x\n", block->addr);
		this->block = block;
		memBaseValid = false;
		CheckBlock(force_checks, block);

		// run register allocator
		regalloc.DoAlloc(block);

		// Opt-in (FC_BLOCK_PROF): bump this block's run counter before anything
		// else, so the count is exact even for blocks that bail out early on the
		// cycle check below. Uses x9/x10, the backend's hardcoded scratch pair,
		// before any allocated register is live.
		if (BlockProfEnabled())
		{
			Mov(x9, reinterpret_cast<uintptr_t>(&block->runs));
			Ldr(w10, MemOperand(x9));
			Add(w10, w10, 1);
			Str(w10, MemOperand(x9));
		}

		// scheduler
		if (mmu_enabled())
		{
			Mov(x1, reinterpret_cast<uintptr_t>(&cycle_counter));
			Ldr(w0, MemOperand(x1));
			Subs(w0, w0, block->guest_cycles);
			Str(w0, MemOperand(x1));
		}
		else
		{
			if (block->idle_fastforward)
			{
				// Proven idle (decoder.cpp signatures): jump time to just
				// before the next event, then end the timeslice so the
				// intc_sched call below handles that event right now.
				// Nothing is register-allocated yet at this point.
				GenCallRuntime(sh4_sched_idle_fastforward);
				Mov(w27, 0);
			}
			Subs(w27, w27, block->guest_cycles);
		}
		Label cycles_remaining;
		B(&cycles_remaining, pl);
		GenCall(*arm64_intc_sched);
		Label cpu_running;
		Cbnz(w0, &cpu_running);
		Mov(w29, block->vaddr);
		Str(w29, sh4_context_mem_operand(&next_pc));
		GenBranch(*arm64_no_update);
		Bind(&cpu_running);
		Bind(&cycles_remaining);

		for (size_t i = 0; i < block->oplist.size(); i++)
		{
			shil_opcode& op  = block->oplist[i];
			regalloc.OpBegin(&op, i);

			switch (op.op)
			{
			case shop_ifb:	// Interpreter fallback
				if (op.rs1._imm)	// if NeedPC()
				{
					Mov(w10, op.rs2._imm);
					Str(w10, sh4_context_mem_operand(&next_pc));
				}
				Mov(*call_regs[0], op.rs3._imm);

				if (!mmu_enabled())
				{
					// FC_IFB_COUNT (opt-in, see comment above IfbCountEnabled()):
					// route through a counting wrapper instead of calling the
					// interpreter handler directly, to see which opcodes (if
					// any) actually hit this fallback in real gameplay. Not
					// instrumented in the mmu_enabled() branch below -- MMU is
					// off for the content this was checked against.
					if (IfbCountEnabled())
						GenCallRuntime(ifb_counted_call);
					else
						GenCallRuntime(OpDesc[op.rs3._imm]->oph);
				}
				else
				{
					Mov(*call_regs64[1], reinterpret_cast<uintptr_t>(*OpDesc[op.rs3._imm]->oph));	// op handler
					Mov(*call_regs[2], block->vaddr + op.guest_offs - (op.delay_slot ? 1 : 0));	// pc

					GenCallRuntime(interpreter_fallback);
				}

				break;

			case shop_jcond:
			case shop_jdyn:
				{
					const Register rd = regalloc.MapRegister(op.rd);
					if (op.rs2.is_imm())
						Add(rd, regalloc.MapRegister(op.rs1), op.rs2._imm);
					else
						Mov(rd, regalloc.MapRegister(op.rs1));
					// Save it for the branching at the end of the block
					Mov(w29, rd);
				}
				break;

			case shop_mov32:
				verify(op.rd.is_reg());
				verify(op.rs1.is_reg() || op.rs1.is_imm());

				if (regalloc.IsAllocf(op.rd))
				{
					const VRegister rd = regalloc.MapVRegister(op.rd);
					if (op.rs1.is_imm())
						Fmov(rd, reinterpret_cast<f32&>(op.rs1._imm));
					else if (regalloc.IsAllocf(op.rs1))
						Fmov(rd, regalloc.MapVRegister(op.rs1));
					else
						Fmov(rd, regalloc.MapRegister(op.rs1));
				}
				else
				{
					const Register rd = regalloc.MapRegister(op.rd);
					if (op.rs1.is_imm())
						Mov(rd, op.rs1._imm);
					else if (regalloc.IsAllocg(op.rs1))
						Mov(rd, regalloc.MapRegister(op.rs1));
					else
						Fmov(rd, regalloc.MapVRegister(op.rs1));
				}
				break;

			case shop_mov64:
				verify(op.rd.is_reg());
				verify(op.rs1.is_reg() || op.rs1.is_imm());

#ifdef EXPLODE_SPANS
				Fmov(regalloc.MapVRegister(op.rd, 0), regalloc.MapVRegister(op.rs1, 0));
				Fmov(regalloc.MapVRegister(op.rd, 1), regalloc.MapVRegister(op.rs1, 1));
#else
				shil_param_to_host_reg(op.rs1, x15);
				host_reg_to_shil_param(op.rd, x15);
#endif
				break;

			case shop_readm:
				GenReadMemory(op, i, optimise);
				break;

			case shop_writem:
				GenWriteMemory(op, i, optimise);
				break;

			case shop_sync_sr:
				GenCallRuntime(UpdateSR);
				break;
			case shop_sync_fpscr:
				{
					// Opt-in (FC_FPSCR_STATS): count every FPSCR write the JIT
					// executes, so the no-op and guard-exit rates below can be
					// read as a fraction of the real total. Emitted inline rather
					// than as a call, to distort what it measures as little as
					// possible.
					if (FpscrStatsEnabled())
						GenCounterIncrement(&g_fpscrWrites);

					// Fast path 1 -- skip the call when the write changed nothing.
					// UpdateFPSCR() is already a no-op in that case (it only acts
					// if FR flipped, and setHostRoundingMode() has its own RM/DN
					// cache), so the entire cost is the JIT->C++ traversal:
					// PushCallerSaved + Bl + PopCallerSaved, which with the
					// extended float pool means several STP/LDP pairs. Measured on
					// kofnw: 60% of 11.9M FPSCR writes change nothing at all, so
					// that traversal was being paid ~5.8k times per frame to do
					// literally nothing.
					Label sync_done;
					Ldr(w9, sh4_context_mem_operand(&fpscr.full));
					Ldr(w10, sh4_context_mem_operand(&old_fpscr.full));
					Cmp(w9, w10);
					B(eq, &sync_done);
					GenCallRuntime(UpdateFPSCR);
					Bind(&sync_done);

					// Fast path 2 -- the PR/SZ guard that lets the block continue.
					// rs1 is the PR|SZ this block was compiled for and rs2 where to
					// resume if it no longer holds (see dec_write_fpscr() in
					// decoder.cpp). Only the cold path leaves the block, so the
					// unconditional block end this fork did since 2015 is gone:
					// measured on kofnw, PR/SZ changed in 0 of 11.9M writes.
					// Bailing out mid-block is safe because shop_sync_fpscr forces
					// every live reg to be written back first (ssa_regalloc.h),
					// exactly like shop_ifb does.
					if (op.rs1.is_imm())
					{
						Label prsz_ok;
						Ldr(w9, sh4_context_mem_operand(&fpscr.full));
						And(w9, w9, (1 << 20) | (1 << 19));		// SZ | PR
						Cmp(w9, op.rs1._imm);
						B(eq, &prsz_ok);
						if (FpscrStatsEnabled())
							GenCounterIncrement(&g_fpscrGuardExit);
						Mov(w29, op.rs2._imm);
						Str(w29, sh4_context_mem_operand(&next_pc));
						GenBranch(*arm64_no_update);
						Bind(&prsz_ok);
					}
				}
				break;

			case shop_swaplb:
				{
					const Register rs1 = regalloc.MapRegister(op.rs1);
					const Register rd = regalloc.MapRegister(op.rd);
					Mov(w9, Operand(rs1, LSR, 16));
					Rev16(rd, rs1);
					Bfi(rd, w9, 16, 16);
				}
				break;

			case shop_neg:
				Neg(regalloc.MapRegister(op.rd), regalloc.MapRegister(op.rs1));
				break;
			case shop_not:
				Mvn(regalloc.MapRegister(op.rd), regalloc.MapRegister(op.rs1));
				break;

			case shop_and:
				ngen_BinaryOp_RRO(&op, &MacroAssembler::And, NULL);
				break;
			case shop_or:
				ngen_BinaryOp_RRO(&op, &MacroAssembler::Orr, NULL);
				break;
			case shop_xor:
				ngen_BinaryOp_RRO(&op, &MacroAssembler::Eor, NULL);
				break;
			case shop_add:
				ngen_BinaryOp_RRO(&op, NULL, &MacroAssembler::Add);
				break;
			case shop_sub:
				ngen_BinaryOp_RRO(&op, NULL, &MacroAssembler::Sub);
				break;
			case shop_shl:
				if (op.rs2.is_imm())
					Lsl(regalloc.MapRegister(op.rd), regalloc.MapRegister(op.rs1), op.rs2._imm);
				else if (op.rs2.is_reg())
					Lsl(regalloc.MapRegister(op.rd), regalloc.MapRegister(op.rs1), regalloc.MapRegister(op.rs2));
				break;
			case shop_shr:
				if (op.rs2.is_imm())
					Lsr(regalloc.MapRegister(op.rd), regalloc.MapRegister(op.rs1), op.rs2._imm);
				else if (op.rs2.is_reg())
					Lsr(regalloc.MapRegister(op.rd), regalloc.MapRegister(op.rs1), regalloc.MapRegister(op.rs2));
				break;
			case shop_sar:
				if (op.rs2.is_imm())
					Asr(regalloc.MapRegister(op.rd), regalloc.MapRegister(op.rs1), op.rs2._imm);
				else if (op.rs2.is_reg())
					Asr(regalloc.MapRegister(op.rd), regalloc.MapRegister(op.rs1), regalloc.MapRegister(op.rs2));
				break;
			case shop_ror:
				if (op.rs2.is_imm())
					Ror(regalloc.MapRegister(op.rd), regalloc.MapRegister(op.rs1), op.rs2._imm);
				else if (op.rs2.is_reg())
					Ror(regalloc.MapRegister(op.rd), regalloc.MapRegister(op.rs1), regalloc.MapRegister(op.rs2));
				break;

			case shop_adc:
				{
					Register reg1;
					Operand op2;
					Register reg3;
					if (op.rs1.is_imm())
					{
						Mov(w0, op.rs1.imm_value());
						reg1 = w0;
					}
					else
					{
						reg1 = regalloc.MapRegister(op.rs1);
					}
					if (op.rs2.is_imm())
						op2 = Operand(op.rs2.imm_value());
					else
						op2 = regalloc.MapRegister(op.rs2);
					if (op.rs3.is_imm())
					{
						Mov(w1, op.rs3.imm_value());
						reg3 = w1;
					}
					else
					{
						reg3 = regalloc.MapRegister(op.rs3);
					}
					Cmp(reg3, 1);	// C = rs3
					Adcs(regalloc.MapRegister(op.rd), reg1, op2); // (C,rd)=rs1+rs2+rs3(C)
					Cset(regalloc.MapRegister(op.rd2), cs);	// rd2 = C
				}
				break;
			case shop_sbc:
				{
					Register reg1;
					Operand op2;
					Operand op3;
					if (op.rs1.is_imm())
					{
						Mov(w0, op.rs1.imm_value());
						reg1 = w0;
					}
					else
					{
						reg1 = regalloc.MapRegister(op.rs1);
					}
					if (op.rs2.is_imm())
						op2 = Operand(op.rs2.imm_value());
					else
						op2 = regalloc.MapRegister(op.rs2);
					if (op.rs3.is_imm())
						op3 = Operand(op.rs3.imm_value());
					else
						op3 = regalloc.MapRegister(op.rs3);
					Cmp(wzr, op3);	// C = ~rs3
					Sbcs(regalloc.MapRegister(op.rd), reg1, op2); // (C,rd) = rs1 - rs2 - ~rs3(C)
					Cset(regalloc.MapRegister(op.rd2), cc);	// rd2 = ~C
				}
				break;
			case shop_negc:
				{
					Operand op1;
					Operand op2;
					if (op.rs1.is_imm())
						op1 = Operand(op.rs1.imm_value());
					else
						op1 = regalloc.MapRegister(op.rs1);
					if (op.rs2.is_imm())
						op2 = Operand(op.rs2.imm_value());
					else
						op2 = regalloc.MapRegister(op.rs2);
					Cmp(wzr, op2);	// C = ~rs2
					Sbcs(regalloc.MapRegister(op.rd), wzr, op1);	// (C,rd) = 0 - rs1 - ~rs2(C)
					Cset(regalloc.MapRegister(op.rd2), cc);			// rd2 = ~C
				}
				break;

			case shop_rocr:
				{
					Register reg1;
					Register reg2;
					if (op.rs1.is_imm())
					{
						Mov(w1, op.rs1.imm_value());
						reg1 = w1;
					}
					else
					{
						reg1 = regalloc.MapRegister(op.rs1);
					}
					if (op.rs2.is_imm())
					{
						Mov(w2, op.rs2.imm_value());
						reg2 = w2;
					}
					else
					{
						reg2 = regalloc.MapRegister(op.rs2);
					}
					Ubfx(w0, reg1, 0, 1);										// w0 = rs1[0] (new C)
					const Register rd = regalloc.MapRegister(op.rd);
					Mov(rd, Operand(reg1, LSR, 1));	// rd = rs1 >> 1
					Bfi(rd, reg2, 31, 1);				// rd |= C << 31
					Mov(regalloc.MapRegister(op.rd2), w0);						// rd2 = w0 (new C)
				}
				break;
			case shop_rocl:
				{
					Register reg1;
					Register reg2;
					if (op.rs1.is_imm())
					{
						Mov(w0, op.rs1.imm_value());
						reg1 = w0;
					}
					else
					{
						reg1 = regalloc.MapRegister(op.rs1);
					}
					if (op.rs2.is_imm())
					{
						Mov(w1, op.rs2.imm_value());
						reg2 = w1;
					}
					else
					{
						reg2 = regalloc.MapRegister(op.rs2);
					}
					Tst(reg1, 0x80000000);						// Z = ~rs1[31]
					Orr(regalloc.MapRegister(op.rd), reg2, Operand(reg1, LSL, 1)); // rd = rs1 << 1 | rs2(C)
					Cset(regalloc.MapRegister(op.rd2), ne);		// rd2 = ~Z(C)
				}
				break;

			case shop_shld:
			case shop_shad:
				{
					Register reg1;
					if (op.rs1.is_imm())
					{
						Mov(w0, op.rs1.imm_value());
						reg1 = w0;
					}
					else
					{
						reg1 = regalloc.MapRegister(op.rs1);
					}
					Label positive_shift, negative_shift, end;
					const Register rs2 = regalloc.MapRegister(op.rs2);
					Tbz(rs2, 31, &positive_shift);
					Cmn(rs2, 32);
					B(&negative_shift, ne);
					const Register rd = regalloc.MapRegister(op.rd);
					// rs2 == -32 => rd = 0 (logical) or 0/-1 (arith)
					if (op.op == shop_shld)
						// Logical shift
						//Lsr(rd, reg1, 31);
						Mov(rd, wzr);
					else
						// Arithmetic shift
						Asr(rd, reg1, 31);
					B(&end);

					Bind(&positive_shift);
					// rs2 >= 0 => left shift
					Lsl(rd, reg1, rs2);
					B(&end);

					Bind(&negative_shift);
					// rs2 < 0 => right shift
					Neg(w1, rs2);
					if (op.op == shop_shld)
						// Logical shift
						Lsr(rd, reg1, w1);
					else
						// Arithmetic shift
						Asr(rd, reg1, w1);
					Bind(&end);
				}
				break;

			case shop_test:
			case shop_seteq:
			case shop_setge:
			case shop_setgt:
			case shop_setae:
			case shop_setab:
				{
					const Register rs1 = regalloc.MapRegister(op.rs1);
					if (op.op == shop_test)
					{
						if (op.rs2.is_imm())
							Tst(rs1, op.rs2._imm);
						else
							Tst(rs1, regalloc.MapRegister(op.rs2));
					}
					else
					{
						if (op.rs2.is_imm())
							Cmp(rs1, op.rs2._imm);
						else
							Cmp(rs1, regalloc.MapRegister(op.rs2));
					}

					static const Condition shop_conditions[] = { eq, eq, ge, gt, hs, hi };

					Cset(regalloc.MapRegister(op.rd), shop_conditions[op.op - shop_test]);
				}
				break;
			case shop_setpeq:
				{
					Register reg1;
					Register reg2;
					if (op.rs1.is_imm())
					{
						Mov(w0, op.rs1.imm_value());
						reg1 = w0;
					}
					else
					{
						reg1 = regalloc.MapRegister(op.rs1);
					}
					if (op.rs2.is_imm())
					{
						Mov(w1, op.rs2.imm_value());
						reg2 = w1;
					}
					else
					{
						reg2 = regalloc.MapRegister(op.rs2);
					}
					Eor(w1, reg1, reg2);
					const Register rd = regalloc.MapRegister(op.rd);
					Mov(rd, wzr);
					Mov(w2, wzr);	// wzr not supported by csinc (?!)
					Tst(w1, 0xFF000000);
					Csinc(rd, rd, w2, ne);
					Tst(w1, 0x00FF0000);
					Csinc(rd, rd, w2, ne);
					Tst(w1, 0x0000FF00);
					Csinc(rd, rd, w2, ne);
					Tst(w1, 0x000000FF);
					Csinc(rd, rd, w2, ne);
				}
				break;

			case shop_mul_u16:
				{
					Register reg2;
					if (op.rs2.is_imm())
					{
						Mov(w0, op.rs2.imm_value());
						reg2 = w0;
					}
					else
					{
						reg2 = regalloc.MapRegister(op.rs2);
					}
					Uxth(w10, regalloc.MapRegister(op.rs1));
					Uxth(w11, reg2);
					Mul(regalloc.MapRegister(op.rd), w10, w11);
				}
				break;
			case shop_mul_s16:
				{
					Register reg2;
					if (op.rs2.is_imm())
					{
						Mov(w0, op.rs2.imm_value());
						reg2 = w0;
					}
					else
					{
						reg2 = regalloc.MapRegister(op.rs2);
					}
					Sxth(w10, regalloc.MapRegister(op.rs1));
					Sxth(w11, reg2);
					Mul(regalloc.MapRegister(op.rd), w10, w11);
				}
				break;
			case shop_mul_i32:
				{
					Register reg2;
					if (op.rs2.is_imm())
					{
						Mov(w0, op.rs2.imm_value());
						reg2 = w0;
					}
					else
					{
						reg2 = regalloc.MapRegister(op.rs2);
					}
					Mul(regalloc.MapRegister(op.rd), regalloc.MapRegister(op.rs1), reg2);
				}
				break;
			case shop_mul_u64:
			case shop_mul_s64:
				{
					Register reg2;
					if (op.rs2.is_imm())
					{
						Mov(w0, op.rs2.imm_value());
						reg2 = w0;
					}
					else
					{
						reg2 = regalloc.MapRegister(op.rs2);
					}
					const Register& rd_xreg = Register::GetXRegFromCode(regalloc.MapRegister(op.rd).GetCode());
					if (op.op == shop_mul_u64)
						Umull(rd_xreg, regalloc.MapRegister(op.rs1), reg2);
					else
						Smull(rd_xreg, regalloc.MapRegister(op.rs1), reg2);
					const Register& rd2_xreg = Register::GetXRegFromCode(regalloc.MapRegister(op.rd2).GetCode());
					Lsr(rd2_xreg, rd_xreg, 32);
				}
				break;

			case shop_pref:
				{
					Label not_sqw;
					if (op.rs1.is_imm())
						Mov(*call_regs[0], op.rs1._imm);
					else
					{
						if (regalloc.IsAllocg(op.rs1))
							Lsr(w1, regalloc.MapRegister(op.rs1), 26);
						else
						{
							Ldr(w0, sh4_context_mem_operand(op.rs1.reg_ptr()));
							Lsr(w1, w0, 26);
						}
						Cmp(w1, 0x38);
						B(&not_sqw, ne);
						if (regalloc.IsAllocg(op.rs1))
							Mov(w0, regalloc.MapRegister(op.rs1));
					}

					if (mmu_enabled())
					{
						Mov(*call_regs[1], block->vaddr + op.guest_offs - (op.delay_slot ? 1 : 0));	// pc

						GenCallRuntime(do_sqw_mmu_no_ex);
					}
					else
					{
						if (CCN_MMUCR.AT)
						{
							Ldr(x9, reinterpret_cast<uintptr_t>(&do_sqw_mmu));
						}
						else
						{
							Sub(x9, x28, offsetof(Sh4RCB, cntx) - offsetof(Sh4RCB, do_sqw_nommu));
							Ldr(x9, MemOperand(x9));
							Sub(x1, x28, offsetof(Sh4RCB, cntx) - offsetof(Sh4RCB, sq_buffer));
						}
						Blr(x9);
					}
					// PREF fora da Store Queue: o jogo esta pedindo prefetch de
					// RAM (e assim que o SH4 esconde a latencia de memoria nos
					// lacos de T&L/decode). Antes era descartado; o A53 e in-order
					// e para em cada miss, entao repassa como PRFM do host. PRFM
					// nunca gera falta, nem em pagina protegida/invalida.
					// FC_NO_PREF_HINT=1 volta ao comportamento antigo.
					static const bool noPrefHint = getenv("FC_NO_PREF_HINT") != nullptr;
					if (!op.rs1.is_imm() && !noPrefHint && _nvmem_enabled() && !mmu_enabled())
					{
						Label done;
						B(&done);
						Bind(&not_sqw);
						const Register& addr = regalloc.IsAllocg(op.rs1) ? regalloc.MapRegister(op.rs1) : w0;
						if (!_nvmem_4gb_space())
							Ubfx(x1, addr.X(), 0, 29);
						else
							Mov(w1, addr);	// zero-extends into x1
						Add(x1, x1, sizeof(Sh4Context));
						Prfm(PLDL1KEEP, MemOperand(x28, x1));
						Bind(&done);
					}
					else
						Bind(&not_sqw);
				}
				break;

			case shop_ext_s8:
				Sxtb(regalloc.MapRegister(op.rd), regalloc.MapRegister(op.rs1));
				break;
			case shop_ext_s16:
				Sxth(regalloc.MapRegister(op.rd), regalloc.MapRegister(op.rs1));
				break;

			case shop_xtrct:
				{
					const Register rd = regalloc.MapRegister(op.rd);
					const Register rs1 = regalloc.MapRegister(op.rs1);
					const Register rs2 = regalloc.MapRegister(op.rs2);
					if (op.rs1._reg == op.rd._reg)
					{
						verify(op.rs2._reg != op.rd._reg);
						Lsr(rd, rs1, 16);
						Lsl(w0, rs2, 16);
					}
					else
					{
						Lsl(rd, rs2, 16);
						Lsr(w0, rs1, 16);
					}
					Orr(rd, rd, w0);
				}
				break;

			//
			// FPU
			//

			case shop_fadd:
				ngen_BinaryFop(&op, &MacroAssembler::Fadd);
				break;
			case shop_fsub:
				ngen_BinaryFop(&op, &MacroAssembler::Fsub);
				break;
			case shop_fmul:
				ngen_BinaryFop(&op, &MacroAssembler::Fmul);
				break;
			case shop_fdiv:
				ngen_BinaryFop(&op, &MacroAssembler::Fdiv);
				break;

			case shop_fabs:
				Fabs(regalloc.MapVRegister(op.rd), regalloc.MapVRegister(op.rs1));
				break;
			case shop_fneg:
				Fneg(regalloc.MapVRegister(op.rd), regalloc.MapVRegister(op.rs1));
				break;
			case shop_fsqrt:
				Fsqrt(regalloc.MapVRegister(op.rd), regalloc.MapVRegister(op.rs1));
				break;

			case shop_fmac:
				Fmadd(regalloc.MapVRegister(op.rd), regalloc.MapVRegister(op.rs3), regalloc.MapVRegister(op.rs2), regalloc.MapVRegister(op.rs1));
				break;

			case shop_fsrra:
				Fsqrt(s0, regalloc.MapVRegister(op.rs1));
				Fmov(s1, 1.f);
				Fdiv(regalloc.MapVRegister(op.rd), s1, s0);
				break;

			case shop_fsetgt:
			case shop_fseteq:
				Fcmp(regalloc.MapVRegister(op.rs1), regalloc.MapVRegister(op.rs2));
				Cset(regalloc.MapRegister(op.rd), op.op == shop_fsetgt ? gt : eq);
				break;

			case shop_fsca:
				Mov(x1, reinterpret_cast<uintptr_t>(&sin_table));
				if (op.rs1.is_reg())
					Add(x1, x1, Operand(regalloc.MapRegister(op.rs1), UXTH, 3));
				else
					Add(x1, x1, Operand(op.rs1.imm_value() << 3));
#ifdef EXPLODE_SPANS
				Ldr(regalloc.MapVRegister(op.rd, 0), MemOperand(x1, 4, PostIndex));
				Ldr(regalloc.MapVRegister(op.rd, 1), MemOperand(x1));
#else
				Ldr(x2, MemOperand(x1));
				Str(x2, sh4_context_mem_operand(op.rd.reg_ptr()));
#endif
				break;

			case shop_fipr:
				Add(x9, x28, sh4_context_mem_operand(op.rs1.reg_ptr()).GetOffset());
				Ld1(v0.V4S(), MemOperand(x9));
				if (op.rs1._reg != op.rs2._reg)
				{
					Add(x9, x28, sh4_context_mem_operand(op.rs2.reg_ptr()).GetOffset());
					Ld1(v1.V4S(), MemOperand(x9));
					Fmul(v0.V4S(), v0.V4S(), v1.V4S());
				}
				else
					Fmul(v0.V4S(), v0.V4S(), v0.V4S());
				Faddp(v1.V4S(), v0.V4S(), v0.V4S());
				Faddp(regalloc.MapVRegister(op.rd), v1.V2S());
				break;

			case shop_ftrv:
				Add(x9, x28, sh4_context_mem_operand(op.rs1.reg_ptr()).GetOffset());
				Ld1(v0.V4S(), MemOperand(x9));
				Add(x9, x28, sh4_context_mem_operand(op.rs2.reg_ptr()).GetOffset());
				Ld1(v1.V4S(), MemOperand(x9, 16, PostIndex));
				Ld1(v2.V4S(), MemOperand(x9, 16, PostIndex));
				Ld1(v3.V4S(), MemOperand(x9, 16, PostIndex));
				Ld1(v4.V4S(), MemOperand(x9, 16, PostIndex));
				Fmul(v5.V4S(), v1.V4S(), s0, 0);
				Fmla(v5.V4S(), v2.V4S(), s0, 1);
				Fmla(v5.V4S(), v3.V4S(), s0, 2);
				Fmla(v5.V4S(), v4.V4S(), s0, 3);
				Add(x9, x28, sh4_context_mem_operand(op.rd.reg_ptr()).GetOffset());
				St1(v5.V4S(), MemOperand(x9));
				break;

			case shop_frswap:
				Add(x9, x28, sh4_context_mem_operand(op.rs1.reg_ptr()).GetOffset());
				Add(x10, x28, sh4_context_mem_operand(op.rd.reg_ptr()).GetOffset());
				Ld4(v0.V2D(), v1.V2D(), v2.V2D(), v3.V2D(), MemOperand(x9));
				Ld4(v4.V2D(), v5.V2D(), v6.V2D(), v7.V2D(), MemOperand(x10));
				St4(v4.V2D(), v5.V2D(), v6.V2D(), v7.V2D(), MemOperand(x9));
				St4(v0.V2D(), v1.V2D(), v2.V2D(), v3.V2D(), MemOperand(x10));
				break;

			case shop_cvt_f2i_t:
				Fcvtzs(regalloc.MapRegister(op.rd), regalloc.MapVRegister(op.rs1));
				break;
			case shop_cvt_i2f_n:
			case shop_cvt_i2f_z:
				Scvtf(regalloc.MapVRegister(op.rd), regalloc.MapRegister(op.rs1));
				break;

			default:
				shil_chf[op.op](&op);
				break;
			}
			regalloc.OpEnd(&op);
		}
		regalloc.Cleanup();

		block->relink_offset = (u32)GetBuffer()->GetCursorOffset();
		block->relink_data = 0;

		RelinkBlock(block);

		Finalize();
	}

	void ngen_CC_Start(shil_opcode* op)
	{
		CC_pars.clear();
	}

	void ngen_CC_Param(shil_opcode& op, shil_param& prm, CanonicalParamType tp)
	{
		switch (tp)
		{

		case CPT_u32:
		case CPT_ptr:
		case CPT_f32:
		{
			CC_PS t = { tp, &prm };
			CC_pars.push_back(t);
		}
		break;

		case CPT_u64rvL:
		case CPT_u32rv:
			host_reg_to_shil_param(prm, w0);
			break;

		case CPT_u64rvH:
			Lsr(x10, x0, 32);
			host_reg_to_shil_param(prm, w10);
			break;

		case CPT_f32rv:
			host_reg_to_shil_param(prm, s0);
			break;
		}
	}

	void ngen_CC_Call(shil_opcode*op, void* function)
	{
		int regused = 0;
		int fregused = 0;

		// Args are pushed in reverse order by shil_canonical
		for (int i = CC_pars.size(); i-- > 0;)
		{
			verify(fregused < call_fregs.size() && regused < call_regs.size());
			shil_param& prm = *CC_pars[i].prm;
			switch (CC_pars[i].type)
			{
			// push the params

			case CPT_u32:
				shil_param_to_host_reg(prm, *call_regs[regused++]);

				break;

			case CPT_f32:
				if (prm.is_reg())
					Fmov(*call_fregs[fregused], regalloc.MapVRegister(prm));
				else if (prm.is_imm())
					Fmov(*call_fregs[fregused], reinterpret_cast<f32&>(prm._imm));
				else
					verify(prm.is_null());
				fregused++;
				break;

			case CPT_ptr:
				verify(prm.is_reg());
				// push the ptr itself
				Mov(*call_regs64[regused++], reinterpret_cast<uintptr_t>(prm.reg_ptr()));

				break;
			case CPT_u32rv:
			case CPT_u64rvL:
			case CPT_u64rvH:
			case CPT_f32rv:
				// return values are handled in ngen_CC_param()
				break;
			}
		}
		GenCallRuntime((void (*)())function);
	}

	// Opt-in (FC_FPSCR_STATS) inline 64-bit counter bump, for counting things
	// that happen inside generated code without paying a runtime call that
	// would swamp what's being measured. x9/x10 are the backend's hardcoded
	// scratch pair, same as everywhere else here.
	void GenCounterIncrement(u64 *counter)
	{
		Mov(x9, reinterpret_cast<uintptr_t>(counter));
		Ldr(x10, MemOperand(x9));
		Add(x10, x10, 1);
		Str(x10, MemOperand(x9));
	}

	MemOperand sh4_context_mem_operand(void *p)
	{
		u32 offset = (u8*)p - (u8*)&p_sh4rcb->cntx;
		verify((offset & 3) == 0 && offset <= 16380);	// FIXME 64-bit regs need multiple of 8 up to 32760
		return MemOperand(x28, offset);
	}

	void GenReadMemorySlow(u32 size)
	{
		Instruction *start_instruction = GetCursorAddress<Instruction *>();

		switch (size)
		{
		case 1:
			if (!mmu_enabled())
				GenCallRuntime(ReadMem8);
			else
				GenCallRuntime(ReadMemNoEx<u8>);
			Sxtb(w0, w0);
			break;

		case 2:
			if (!mmu_enabled())
				GenCallRuntime(ReadMem16);
			else
				GenCallRuntime(ReadMemNoEx<u16>);
			Sxth(w0, w0);
			break;

		case 4:
			if (!mmu_enabled())
				GenCallRuntime(ReadMem32);
			else
				GenCallRuntime(ReadMemNoEx<u32>);
			break;

		case 8:
			if (!mmu_enabled())
				GenCallRuntime(ReadMem64);
			else
				GenCallRuntime(ReadMemNoEx<u64>);
			break;

		default:
			{ ERROR_LOG(COMMON, "GenReadMemorySlow: size=%u invalido (bloco SH4 %08X) -- so 1/2/4/8 sao validos", size, block ? block->vaddr : 0); die("1..8 bytes"); }
			break;
		}
		EnsureCodeSize(start_instruction, read_memory_rewrite_size);
	}

	void GenWriteMemorySlow(u32 size)
	{
		Instruction *start_instruction = GetCursorAddress<Instruction *>();

		switch (size)
		{
		case 1:
			if (!mmu_enabled())
				GenCallRuntime(WriteMem8);
			else
				GenCallRuntime(WriteMemNoEx<u8>);
			break;

		case 2:
			if (!mmu_enabled())
				GenCallRuntime(WriteMem16);
			else
				GenCallRuntime(WriteMemNoEx<u16>);
			break;

		case 4:
			if (!mmu_enabled())
				GenCallRuntime(WriteMem32);
			else
				GenCallRuntime(WriteMemNoEx<u32>);
			break;

		case 8:
			if (!mmu_enabled())
				GenCallRuntime(WriteMem64);
			else
				GenCallRuntime(WriteMemNoEx<u64>);
			break;

		default:
			{ ERROR_LOG(COMMON, "GenWriteMemorySlow: size=%u invalido (bloco SH4 %08X) -- so 1/2/4/8 sao validos", size, block ? block->vaddr : 0); die("1..8 bytes"); }
			break;
		}
		EnsureCodeSize(start_instruction, write_memory_rewrite_size);
	}

	u32 RelinkBlock(RuntimeBlockInfo *block)
	{
		ptrdiff_t start_offset = GetBuffer()->GetCursorOffset();

		switch (block->BlockType)
		{

		case BET_StaticJump:
		case BET_StaticCall:
			// next_pc = block->BranchBlock;
			if (block->pBranchBlock == NULL)
			{
				if (!mmu_enabled())
					GenCallRuntime(ngen_LinkBlock_Generic_stub);
				else
				{
					Mov(w29, block->BranchBlock);
					Str(w29, sh4_context_mem_operand(&next_pc));
					GenBranch(*arm64_no_update);
				}
			}
			else
				GenBranch(block->pBranchBlock->code);
			break;

		case BET_Cond_0:
		case BET_Cond_1:
			{
				// next_pc = next_pc_value;
				// if (*jdyn == 0)
				//   next_pc = branch_pc_value;

				if (block->has_jcond)
					Ldr(w11, sh4_context_mem_operand(&Sh4cntx.jdyn));
				else
					Ldr(w11, sh4_context_mem_operand(&sr.T));

				Cmp(w11, block->BlockType & 1);

				Label branch_not_taken;

				B(ne, &branch_not_taken);
				if (block->pBranchBlock != NULL)
					GenBranch(block->pBranchBlock->code);
				else
				{
					if (!mmu_enabled())
						GenCallRuntime(ngen_LinkBlock_cond_Branch_stub);
					else
					{
						Mov(w29, block->BranchBlock);
						Str(w29, sh4_context_mem_operand(&next_pc));
						GenBranch(*arm64_no_update);
					}
				}

				Bind(&branch_not_taken);

				if (block->pNextBlock != NULL)
					GenBranch(block->pNextBlock->code);
				else
				{
					if (!mmu_enabled())
						GenCallRuntime(ngen_LinkBlock_cond_Next_stub);
					else
					{
						Mov(w29, block->NextBlock);
						Str(w29, sh4_context_mem_operand(&next_pc));
						GenBranch(*arm64_no_update);
					}
				}
			}
			break;

		case BET_DynamicJump:
		case BET_DynamicCall:
		case BET_DynamicRet:
			// next_pc = *jdyn;

			Str(w29, sh4_context_mem_operand(&next_pc));
			if (!mmu_enabled())
			{
				// TODO Call no_update instead (and check CpuRunning less frequently?)
            Sub(x2, x28, offsetof(Sh4RCB, cntx));
#if RAM_SIZE_MAX == 33554432
				Ubfx(w1, w29, 1, 24);
#else
				Ubfx(w1, w29, 1, 23);
#endif
				Ldr(x15, MemOperand(x2, x1, LSL, 3));	// Get block entry point
				Br(x15);
			}
			else
			{
				GenBranch(*arm64_no_update);
			}

			break;

		case BET_DynamicIntr:
		case BET_StaticIntr:
			if (block->BlockType == BET_StaticIntr)
				// next_pc = next_pc_value;
				Mov(w29, block->NextBlock);
			// else next_pc = *jdyn (already in w29)

			Str(w29, sh4_context_mem_operand(&next_pc));

			GenCallRuntime(UpdateINTC);

			Ldr(w29, sh4_context_mem_operand(&next_pc));
			GenBranch(*arm64_no_update);

			break;

		default:
			die("Invalid block end type");
		}

		return GetBuffer()->GetCursorOffset() - start_offset;
	}

	void Finalize(bool rewrite = false)
	{
		Label code_end;
		Bind(&code_end);

		FinalizeCode();

		if (!rewrite)
		{
			block->code = GetBuffer()->GetStartAddress<DynarecCodeEntryPtr>();
			block->host_code_size = GetBuffer()->GetSizeInBytes();
			block->host_opcodes = GetLabelAddress<u32*>(&code_end) - GetBuffer()->GetStartAddress<u32*>();

			EmitPerfMapEntry(block->code, block->host_code_size, block->vaddr);

			// FC_DUMP_BLOCK=<hex vaddr>[,<hex vaddr>...] (opt-in, diagnosis
			// only): write the generated ARM64 code of those blocks to
			// /tmp/block-<vaddr>.bin and their SHIL to .txt, to disassemble
			// off-device (objdump -b binary -m aarch64 -D). The block
			// profiler says WHICH blocks are hot; this shows WHAT we emit
			// for them.
			{
				static const char *dumpList = getenv("FC_DUMP_BLOCK");
				if (dumpList != nullptr)
				{
					char key[16];
					snprintf(key, sizeof(key), "%08X", block->vaddr);
					if (strcasestr(dumpList, key) != nullptr)
					{
						char path[64];
						snprintf(path, sizeof(path), "/tmp/block-%08X.bin", block->vaddr);
						FILE *f = fopen(path, "wb");
						if (f != nullptr)
						{
							fwrite(GetBuffer()->GetStartAddress<void*>(), 1, block->host_code_size, f);
							fclose(f);
						}
						snprintf(path, sizeof(path), "/tmp/block-%08X.txt", block->vaddr);
						f = fopen(path, "w");
						if (f != nullptr)
						{
							fprintf(f, "vaddr %08X guest_ops %u cycles %u host_bytes %u\n", block->vaddr,
									block->guest_opcodes, block->guest_cycles, block->host_code_size);
							for (size_t j = 0; j < block->oplist.size(); j++)
								fprintf(f, "%zu: %s\n", j, block->oplist[j].dissasm().c_str());
							fclose(f);
						}
					}
				}
			}

			emit_Skip(block->host_code_size);
		}

		// Flush and invalidate caches
		vmem_platform_flush_cache(
			CC_RW2RX(GetBuffer()->GetStartAddress<void*>()), CC_RW2RX(GetBuffer()->GetEndAddress<void*>()),
			GetBuffer()->GetStartAddress<void*>(), GetBuffer()->GetEndAddress<void*>());
#if 0
		if (rewrite && block != NULL)
		{
			INFO_LOG(DYNAREC, "BLOCK %08x", block->vaddr);
			Instruction* instr_start = (Instruction*)block->code;
//			Instruction* instr_end = GetLabelAddress<Instruction*>(&code_end);
			Instruction* instr_end = (Instruction*)((u8 *)block->code + block->host_code_size);
			Decoder decoder;
			Disassembler disasm;
			decoder.AppendVisitor(&disasm);
			Instruction* instr;
			for (instr = instr_start; instr < instr_end; instr += kInstructionSize) {
				decoder.Decode(instr);
				INFO_LOG(DYNAREC, "VIXL  %p:  %s",
						   reinterpret_cast<void*>(instr),
						   disasm.GetOutput());
			}
		}
#endif
	}

	void GenMainloop()
	{
		Label no_update;
		Label intc_sched;
		Label end_mainloop;

		// int intc_sched()
		arm64_intc_sched = GetCursorAddress<int (*)()>();
      verify((void *)arm64_intc_sched == (void *)CodeCache);
		B(&intc_sched);

		// void no_update()
		Bind(&no_update);				// next_pc _MUST_ be on w29

		Ldr(w0, MemOperand(x28, offsetof(Sh4Context, CpuRunning)));
		Cbz(w0, &end_mainloop);
		if (!mmu_enabled())
		{
			Sub(x2, x28, offsetof(Sh4RCB, cntx));
			if (RAM_SIZE == 32 * 1024 * 1024)
				Ubfx(w1, w29, 1, 24);	// 24+1 bits: 32 MB
			else if (RAM_SIZE == 16 * 1024 * 1024)
				Ubfx(w1, w29, 1, 23);	// 23+1 bits: 16 MB
			else
				die("Unsupported RAM_SIZE");
			Ldr(x0, MemOperand(x2, x1, LSL, 3));
		}
		else
		{
			Mov(w0, w29);
			GenCallRuntime(bm_GetCodeByVAddr);
		}
		Br(x0);

		// void mainloop(void *context)
		mainloop = (void (*)(void *)) CC_RW2RX(GetCursorAddress<uintptr_t>());

		// Save registers
		Stp(x19, x20, MemOperand(sp, -160, PreIndex));
		Stp(x21, x22, MemOperand(sp, 16));
		Stp(x23, x24, MemOperand(sp, 32));
		Stp(x25, x26, MemOperand(sp, 48));
		Stp(x27, x28, MemOperand(sp, 64));
		Stp(s14, s15, MemOperand(sp, 80));
		Stp(vixl::aarch64::s8, s9, MemOperand(sp, 96));
		Stp(s10, s11, MemOperand(sp, 112));
		Stp(s12, s13, MemOperand(sp, 128));
		Stp(x29, x30, MemOperand(sp, 144));

		Sub(x0, x0, sizeof(Sh4Context));
		if (mmu_enabled())
		{
			Ldr(x1, reinterpret_cast<uintptr_t>(&cycle_counter));
			// Push context, cycle_counter address
			Stp(x0, x1, MemOperand(sp, -16, PreIndex));
			Mov(w0, sh4_sched_timeslice);
			Str(w0, MemOperand(x1));

			Ldr(x0, reinterpret_cast<uintptr_t>(jmp_env));
			Ldr(x1, reinterpret_cast<uintptr_t>(&setjmp));
			Blr(x1);

			Ldr(x28, MemOperand(sp));	// Set context
		}
		else
		{
			// Use x28 as sh4 context pointer
			Mov(x28, x0);
			// Use x27 as cycle_counter
			Mov(w27, sh4_sched_timeslice);
		}
		Label do_interrupts;

		// w29 is next_pc
		Ldr(w29, MemOperand(x28, offsetof(Sh4Context, pc)));
		B(&no_update);

		Bind(&intc_sched);

		// Add timeslice to cycle counter
		if (!mmu_enabled())
		{
			Add(w27, w27, sh4_sched_timeslice);
		}
		else
		{
			Ldr(x1, MemOperand(sp, 8));	// &cycle_counter
			Ldr(w0, MemOperand(x1));	// cycle_counter
			Add(w0, w0, sh4_sched_timeslice);
			Str(w0, MemOperand(x1));
		}
		Mov(x29, lr);				// Trashing pc here but it will be reset at the end of the block or in DoInterrupts
		GenCallRuntime(UpdateSystem);
		Mov(lr, x29);
		Cbnz(w0, &do_interrupts);
		Ldr(w0, MemOperand(x28, offsetof(Sh4Context, CpuRunning)));
		Ret();

		Bind(&do_interrupts);
		Mov(x0, x29);
		GenCallRuntime(rdv_DoInterrupts);	// Updates next_pc based on host pc
		Mov(w29, w0);

		B(&no_update);

		Bind(&end_mainloop);
		if (mmu_enabled())
			// Pop context
			Add(sp, sp, 16);
		// Restore registers
		Ldp(x29, x30, MemOperand(sp, 144));
		Ldp(s12, s13, MemOperand(sp, 128));
		Ldp(s10, s11, MemOperand(sp, 112));
		Ldp(vixl::aarch64::s8, s9, MemOperand(sp, 96));
		Ldp(s14, s15, MemOperand(sp, 80));
		Ldp(x27, x28, MemOperand(sp, 64));
		Ldp(x25, x26, MemOperand(sp, 48));
		Ldp(x23, x24, MemOperand(sp, 32));
		Ldp(x21, x22, MemOperand(sp, 16));
		Ldp(x19, x20, MemOperand(sp, 160, PostIndex));
		Ret();

		FinalizeCode();
		emit_Skip(GetBuffer()->GetSizeInBytes());

		arm64_no_update = GetLabelAddress<void (*)()>(&no_update);

		// Flush and invalidate caches
		vmem_platform_flush_cache(
			CC_RW2RX(GetBuffer()->GetStartAddress<void*>()), CC_RW2RX(GetBuffer()->GetEndAddress<void*>()),
			GetBuffer()->GetStartAddress<void*>(), GetBuffer()->GetEndAddress<void*>());
	}

	// Shared, generated-once stub(s) for special-casing memory accesses that
	// ngen_Rewrite() only discovers are "not RAM" at runtime (fastmem fault).
	// See docs/arm64jit_improvement_plan.md item 1: unlike inlining a check at
	// every write site (tried and reverted, docs/tech_debits.md item 1.7 --
	// regressed ~6% because it taxed every 32-bit write, not just the rare
	// hot ones), ngen_Rewrite() only ever looks at a call site AFTER it has
	// actually faulted, so redirecting that one site to a specialized stub
	// costs nothing for the ~all other writes that never fault.
	void GenMemStubs()
	{
		// Store Queue direct-write stub (32-bit only -- real hardware only
		// supports 32-bit SQ stores, see core/hw/sh4/sh4_mmr.cpp comment).
		// Called with the guest address in w0 and the data in w1, exactly the
		// registers GenWriteMemoryFast() already receives them in -- no extra
		// argument marshaling needed at the call site.
		sq_write_stub = CC_RW2RX(GetCursorAddress<uintptr_t>());

		Label not_sq;
		Lsr(w9, w0, 26);
		Cmp(w9, 0x38);
		B(&not_sq, ne);
		And(w9, w0, 0x3f);
		Sub(x10, x28, offsetof(Sh4RCB, cntx) - offsetof(Sh4RCB, sq_buffer));
		Str(w1, MemOperand(x10, x9));
		Ret();

		Bind(&not_sq);
		// Mismatch -- expected to be rare to never (would need the exact same
		// compiled call site to compute a Store Queue address on one execution
		// and a different, non-SQ address on another), but re-verified rather
		// than assumed: silently trusting the caller here would risk corrupting
		// memory if it ever happened. This stub isn't compiled as part of any
		// block, so it has no per-call-site liveness info to do a targeted
		// PushCallerSaved() with -- conservatively save every caller-saved
		// float register instead (this path is not the hot one; the SQ match
		// above is).
		CPURegList allCallerSaved(CPURegister::kVRegister, 64, 16, 31);
		// LR TEM de ser salvo: este stub e chamado com Bl (LR = call site no
		// bloco), e o GenCallRuntime() abaixo emite outro Bl, que SOBRESCREVE
		// o LR. Sem salvar, o Ret() no fim volta para dentro do proprio stub
		// (logo depois do Bl interno) em vez de voltar ao chamador -- e cada
		// volta executa de novo o PopCPURegList, cujo `ldp d16,d17,[sp],#128`
		// SOMA 128 ao SP. O SP sobe 128 bytes por iteracao ate sair do topo da
		// pilha e bater no guard page: SIGSEGV.
		//
		// Era o crash do meltybld e do capsnk (tech_debits item 1.10),
		// diagnosticado em 2026-09-18. So dispara quando este caminho `not_sq`
		// e tomado -- o proprio comentario acima previa isso como "raro a
		// nunca" -- o que explica o crash ser especifico de alguns jogos.
		// 16 bytes mantem o alinhamento de pilha exigido pela ABI.
		Str(x30, MemOperand(sp, -16, PreIndex));
		PushCPURegList(allCallerSaved);
		GenCallRuntime(WriteMem32);
		PopCPURegList(allCallerSaved);
		Ldr(x30, MemOperand(sp, 16, PostIndex));
		Ret();

		// Code-page store stubs (docs/tech_debits.md 4.19). Same calling
		// convention as the fast store they replace: guest address in w0,
		// data in w1/x1. Fast path, inline: an address in main RAM whose page
		// is NOT locked is written straight away through the unlocked RAM
		// mirror -- so a generic store site (a memcpy, say) that once hit a
		// code page and got rewritten stays cheap for every other page. Only
		// locked pages (or non-RAM addresses) take the C slow path, which
		// decides between "data next to code" and real self-modifying code.
		if (_nvmem_enabled() && !mmu_enabled())
		{
			if (bm_ram_alias == nullptr)
				bm_ram_alias = virt_ram_base + 0x0C000000 + RAM_SIZE;
			for (int i = 0; i < 4; i++)
			{
				codepage_write_stub[i] = CC_RW2RX(GetCursorAddress<uintptr_t>());
				Label slow;
				Ubfx(w9, w0, 26, 3);			// area: 3 = main RAM (P0/P1/P2 views alike)
				Cmp(w9, 3);
				B(&slow, ne);
				And(w9, w0, RAM_MASK);
				Label write;
				Lsr(w10, w9, 12);
				Mov(x11, reinterpret_cast<uintptr_t>(bm_code_page_locked));
				Ldrb(w10, MemOperand(x11, x10));
				Cbz(w10, &write);
				// Locked page: only a 32-byte chunk holding compiled code needs
				// the exact check in C. A store straddling two chunks goes to C
				// too rather than testing both.
				const u32 size = 1u << i;
				if (size > 1)
				{
					And(w10, w9, 31);
					Cmp(w10, 32 - size);
					B(&slow, hi);
				}
				Lsr(w10, w9, 5);				// chunk index
				Lsr(w12, w10, 3);				// its byte in the bitmap
				Mov(x11, reinterpret_cast<uintptr_t>(bm_code_chunks));
				Ldrb(w12, MemOperand(x11, x12));
				And(w10, w10, 7);
				Lsr(w12, w12, w10);
				Tbnz(w12, 0, &slow);
				Bind(&write);
				Mov(x11, reinterpret_cast<uintptr_t>(bm_ram_alias));
				switch (i)
				{
				case 0: Strb(w1, MemOperand(x11, x9)); break;
				case 1: Strh(w1, MemOperand(x11, x9)); break;
				case 2: Str(w1, MemOperand(x11, x9)); break;
				default: Str(x1, MemOperand(x11, x9)); break;
				}
				Ret();

				Bind(&slow);
				// Same register discipline as the SQ stub above: save LR (the
				// call below overwrites it) and every caller-saved vector reg.
				Str(x30, MemOperand(sp, -16, PreIndex));
				PushCPURegList(allCallerSaved);
				switch (i)
				{
				case 0: GenCallRuntime(bm_WriteMemCodePage8); break;
				case 1: GenCallRuntime(bm_WriteMemCodePage16); break;
				case 2: GenCallRuntime(bm_WriteMemCodePage32); break;
				default: GenCallRuntime(bm_WriteMemCodePage64); break;
				}
				PopCPURegList(allCallerSaved);
				Ldr(x30, MemOperand(sp, 16, PostIndex));
				Ret();
			}
		}

		FinalizeCode();
		emit_Skip(GetBuffer()->GetSizeInBytes());

		vmem_platform_flush_cache(
			CC_RW2RX(GetBuffer()->GetStartAddress<void*>()), CC_RW2RX(GetBuffer()->GetEndAddress<void*>()),
			GetBuffer()->GetStartAddress<void*>(), GetBuffer()->GetEndAddress<void*>());
	}

	// Like GenCallRuntime(), but for a raw code address (e.g. a generated stub,
	// not a typed C++ function pointer) and without PushCallerSaved()/
	// PopCallerSaved() -- used by ngen_Rewrite() to redirect a specific
	// write call-site to sq_write_stub. Public: called from the free function
	// ngen_Rewrite(), not just from within this class. Pads with Nop() out to
	// write_memory_rewrite_size itself (same fixed-size reserved slot
	// GenWriteMemorySlow() pads to), since EnsureCodeSize() is private.
	void GenCallStubAddr(void *target)
	{
		Instruction *start_instruction = GetCursorAddress<Instruction *>();
		ptrdiff_t offset = reinterpret_cast<uintptr_t>(target) - reinterpret_cast<uintptr_t>(CC_RW2RX(GetBuffer()->GetStartAddress<void*>()));
		verify(offset >= -128 * 1024 * 1024 && offset <= 128 * 1024 * 1024);
		verify((offset & 3) == 0);
		Label function_label;
		BindToOffset(&function_label, offset);
		Bl(&function_label);
		EnsureCodeSize(start_instruction, write_memory_rewrite_size);
	}

	// Slow path, fora da linha, de um acesso compacto (GenRead/WriteMemoryCompact)
	// que deu fault. O fault handler troca a instrucao do acesso por um `b` para
	// ca. Como nao conhecemos a alocacao do bloco daqui, salva LR e TODOS os
	// vetores caller-saved (v16-v31) -- ao contrario do rewrite antigo, cujo
	// PushCallerSaved() com regalloc vazio nao salvava nada (item 4.27).
	// target: ReadMem/WriteMem generico ou um dos stubs (Store Queue, escrita
	// em pagina de codigo), ja resolvido pelo chamador (ngen_Rewrite*).
	// live_fregs: S16-S31 vivos no site (bit i = S16+i). Os stubs de SQ e de
	// pagina de codigo se protegem sozinhos -- o chamador passa 0 para eles.
	// LR nao e salvo: como no rewrite antigo, o codigo do bloco nao mantem x30
	// vivo (todo GenCallRuntime o destroi).
	void GenCompactTrampoline(bool is_read, u32 size, u32 rt, u32 rm, void *target, void *return_rx, bool is_unsigned = false, u16 live_fregs = 0)
	{
		CPURegList saved(CPURegister::kVRegister, 64, 0);
		for (int i = 0; i < 16; i++)
			if (live_fregs & (1 << i))
				saved.Combine(VRegister::GetDRegFromCode(16 + i));
		if ((saved.GetCount() % 2) != 0)
			saved.Combine(d7);
		if (!saved.IsEmpty())
			PushCPURegList(saved);
		Mov(w0, Register::GetWRegFromCode(rm));
		if (!is_read)
		{
			if (size == 8)
				Mov(x1, Register::GetXRegFromCode(rt));
			else
				Mov(w1, Register::GetWRegFromCode(rt));
		}
		GenBranchAbs(target, true);
		if (is_read)
		{
			if (size == 1)
			{
				if (is_unsigned) Uxtb(w0, w0); else Sxtb(w0, w0);
			}
			else if (size == 2)
			{
				if (is_unsigned) Uxth(w0, w0); else Sxth(w0, w0);
			}
		}
		if (!saved.IsEmpty())
			PopCPURegList(saved);
		if (is_read)
		{
			if (size == 8)
				Mov(Register::GetXRegFromCode(rt), x0);
			else
				Mov(Register::GetWRegFromCode(rt), w0);
		}
		// O bloco pode seguir contando com x13 valido depois deste acesso.
		Add(x13, x28, sizeof(Sh4Context));
		GenBranchAbs(return_rx, false);
	}

	// Trampolim de site de Store Queue (escrita de 32 bits que deu fault em
	// 0xE0000000-0xE3FFFFFF). Faz a escrita no sq_buffer aqui mesmo -- o mesmo
	// que o sq_write_stub faz --, sem o bl/ret do stub: no kofnw (SQ enchida
	// palavra a palavra, item 1.7) os desvios extras custavam ~3% de fps.
	// Endereco fora da SQ (nao esperado no mesmo site) cai no WriteMem32.
	void GenCompactTrampolineSQ(u32 rt, u32 rm, void *return_rx, u16 live_fregs)
	{
		const Register& addr = Register::GetWRegFromCode(rm);
		const Register& data = Register::GetWRegFromCode(rt);
		Label not_sq;
		Lsr(w9, addr, 26);
		Cmp(w9, 0x38);
		B(&not_sq, ne);
		And(w9, addr, 0x3f);
		Sub(x10, x28, offsetof(Sh4RCB, cntx) - offsetof(Sh4RCB, sq_buffer));
		Str(data, MemOperand(x10, x9));
		GenBranchAbs(return_rx, false);
		Bind(&not_sq);
		GenCompactTrampoline(false, 4, rt, rm, (void *)WriteMem32, return_rx, false, live_fregs);
	}

	void GenBranchAbs(void *target, bool link)
	{
		ptrdiff_t offset = reinterpret_cast<uintptr_t>(target) - reinterpret_cast<uintptr_t>(CC_RW2RX(GetBuffer()->GetStartAddress<void*>()));
		verify(offset >= -128 * 1024 * 1024 && offset <= 128 * 1024 * 1024);
		verify((offset & 3) == 0);
		Label l;
		BindToOffset(&l, offset);
		if (link)
			Bl(&l);
		else
			B(&l);
	}

	void FinalizeStub()
	{
		FinalizeCode();
		emit_Skip(GetBuffer()->GetSizeInBytes());
		vmem_platform_flush_cache(
			CC_RW2RX(GetBuffer()->GetStartAddress<void*>()), CC_RW2RX(GetBuffer()->GetEndAddress<void*>()),
			GetBuffer()->GetStartAddress<void*>(), GetBuffer()->GetEndAddress<void*>());
	}


private:
	// Runtime branches/calls need to be adjusted if rx space is different to rw space.
	// Therefore can't mix GenBranch with GenBranchRuntime!

	template <typename R, typename... P>
	void GenCallRuntime(R (*function)(P...))
	{
		regalloc.PushCallerSaved();
		ptrdiff_t offset = reinterpret_cast<uintptr_t>(function) - reinterpret_cast<uintptr_t>(CC_RW2RX(GetBuffer()->GetStartAddress<void*>()));
		verify(offset >= -128 * 1024 * 1024 && offset <= 128 * 1024 * 1024);
		verify((offset & 3) == 0);
		Label function_label;
		BindToOffset(&function_label, offset);
		Bl(&function_label);
		regalloc.PopCallerSaved();
	}

	template <typename R, typename... P>
	void GenCall(R (*function)(P...))
	{
		ptrdiff_t offset = reinterpret_cast<uintptr_t>(function) - GetBuffer()->GetStartAddress<uintptr_t>();
		verify(offset >= -128 * 1024 * 1024 && offset <= 128 * 1024 * 1024);
		verify((offset & 3) == 0);
		Label function_label;
		BindToOffset(&function_label, offset);
		Bl(&function_label);
	}

   template <typename R, typename... P>
	void GenBranchRuntime(R (*target)(P...))
	{
		ptrdiff_t offset = reinterpret_cast<uintptr_t>(target) - reinterpret_cast<uintptr_t>(CC_RW2RX(GetBuffer()->GetStartAddress<void*>()));
		verify(offset >= -128 * 1024 * 1024 && offset <= 128 * 1024 * 1024);
		verify((offset & 3) == 0);
		Label target_label;
		BindToOffset(&target_label, offset);
		B(&target_label);
	}

	template <typename R, typename... P>
	void GenBranch(R (*code)(P...), Condition cond = al)
	{
		ptrdiff_t offset = reinterpret_cast<uintptr_t>(code) - GetBuffer()->GetStartAddress<uintptr_t>();
		verify(offset >= -128 * 1024 * 1024 && offset < 128 * 1024 * 1024);
		verify((offset & 3) == 0);
		Label code_label;
		BindToOffset(&code_label, offset);
		if (cond == al)
			B(&code_label);
		else
			B(&code_label, cond);
	}

	// Acesso compacto a RAM emulada (item 4.27, FC_NO_COMPACT_MEM=1 desliga).
	// O caminho rapido antigo gasta 5 instrucoes por leitura -- mov do endereco
	// para w0, add do offset, ldr, nop de reserva e mov do resultado -- porque o
	// rewrite em caso de fault precisa do endereco em w0 e de um slot fixo. Aqui
	// sao 2: a base (x28 + sizeof(Sh4Context)) em x13, que o JIT nao usa para
	// mais nada, e um ldr/str com os registradores alocados direto, indice
	// estendido de 32 bits (UXTW). No fault, ngen_Rewrite reconhece a forma
	// (Rn == x13) e troca SO essa instrucao por um `b` para um trampolim
	// (GenCompactTrampoline). Dados de float passam por w15.
	static bool CompactMemEnabled()
	{
		static int enabled = -1;
		if (enabled == -1)
			enabled = getenv("FC_NO_COMPACT_MEM") != nullptr ? 0 : 1;
		return enabled == 1;
	}

	bool CompactMemUsable(const shil_opcode& op)
	{
		if (!CompactMemEnabled() || !_nvmem_enabled() || !_nvmem_4gb_space() || mmu_enabled())
			return false;
		u32 size = op.flags & 0x7f;
#ifdef EXPLODE_SPANS
		if (size != 1 && size != 2 && size != 4)
			return false;
#else
		if (size != 1 && size != 2 && size != 4 && size != 8)
			return false;
#endif
		return !op.rs1.is_imm();
	}

	const Register& CompactMemAddr(const shil_opcode& op)
	{
		if (op.rs3.is_null() && regalloc.IsAllocg(op.rs1))
			return regalloc.MapRegister(op.rs1);
		return GenMemAddr(op, &w14);
	}

	// Registra, para o acesso que vai ser emitido AGORA, os S16-S31 vivos.
	void RecordCompactSite()
	{
		const u16 mask = regalloc.LiveCallerSavedFMask();
		if (mask != 0)
			compact_live_fregs[(uintptr_t)CC_RW2RX(GetCursorAddress<void*>())] = mask;
	}

	bool GenReadMemoryCompact(const shil_opcode& op)
	{
		if (!CompactMemUsable(op) || !op.rd.is_reg())
			return false;
		if ((op.flags & 0x7f) == 8)
		{
			// Par de floats (fmov.d / SZ=1): vai para o contexto, como no
			// caminho antigo (Str(x0, ...)), mas sem os movs e o nop.
			const Register& addr = CompactMemAddr(op);
			EnsureMemBase();
			RecordCompactSite();
			Ldr(x15, MemOperand(x13, addr, UXTW));
			Str(x15, sh4_context_mem_operand(op.rd.reg_ptr()));
			return true;
		}
		if (op.rd.is_r64f())
			return false;
		const bool dstInt = regalloc.IsAllocg(op.rd);
		const Register& addr = CompactMemAddr(op);
		const Register& dst = dstInt ? regalloc.MapRegister(op.rd) : w15;
		EnsureMemBase();
		RecordCompactSite();
		MemOperand mo(x13, addr, UXTW);
		const bool zx = (op.flags2 & 1) != 0;
		switch (op.flags & 0x7f)
		{
		case 1: if (zx) Ldrb(dst, mo); else Ldrsb(dst, mo); break;
		case 2: if (zx) Ldrh(dst, mo); else Ldrsh(dst, mo); break;
		default: Ldr(dst, mo); break;
		}
		if (!dstInt)
			host_reg_to_shil_param(op.rd, w15);
		return true;
	}

	bool GenWriteMemoryCompact(const shil_opcode& op)
	{
		if (!CompactMemUsable(op))
			return false;
		if ((op.flags & 0x7f) == 8)
		{
			if (!op.rs2.is_reg())
				return false;
			const Register& addr = CompactMemAddr(op);
			Ldr(x15, sh4_context_mem_operand(op.rs2.reg_ptr()));
			EnsureMemBase();
			RecordCompactSite();
			Str(x15, MemOperand(x13, addr, UXTW));
			return true;
		}
		if (op.rs2.is_r64f())
			return false;
		const Register& addr = CompactMemAddr(op);
		const Register* data = &w15;
		if (op.rs2.is_reg() && regalloc.IsAllocg(op.rs2))
			data = &regalloc.MapRegister(op.rs2);
		else
			shil_param_to_host_reg(op.rs2, w15);
		EnsureMemBase();
		RecordCompactSite();
		MemOperand mo(x13, addr, UXTW);
		switch (op.flags & 0x7f)
		{
		case 1: Strb(*data, mo); break;
		case 2: Strh(*data, mo); break;
		default: Str(*data, mo); break;
		}
		return true;
	}

	void GenReadMemory(const shil_opcode& op, size_t opid, bool optimise)
	{
		if (GenReadMemoryImmediate(op))
			return;
		if (optimise && GenReadMemoryCompact(op))
			return;

		GenMemAddr(op, call_regs[0]);
		if (mmu_enabled())
			Mov(*call_regs[2], block->vaddr + op.guest_offs - (op.delay_slot ? 2 : 0));	// pc

		u32 size = op.flags & 0x7f;
		if (!optimise || !GenReadMemoryFast(op, opid))
			GenReadMemorySlow(size);

		if (op.flags2 & 1)	// ZeroExtendLoadPass: leitura sem sinal
		{
			if (size == 1)
				Uxtb(w0, w0);
			else if (size == 2)
				Uxth(w0, w0);
		}
		if (size < 8)
			host_reg_to_shil_param(op.rd, w0);
		else
		{
#ifdef EXPLODE_SPANS
			verify(op.rd.count() == 2 && regalloc.IsAllocf(op.rd, 0) && regalloc.IsAllocf(op.rd, 1));
			Fmov(regalloc.MapVRegister(op.rd, 0), w0);
			Lsr(x0, x0, 32);
			Fmov(regalloc.MapVRegister(op.rd, 1), w0);
#else
			Str(x0, sh4_context_mem_operand(op.rd.reg_ptr()));
#endif
		}
	}

	bool GenReadMemoryImmediate(const shil_opcode& op)
	{
		if (!op.rs1.is_imm())
			return false;

		u32 size = op.flags & 0x7f;
		u32 addr = op.rs1._imm;
#ifndef NO_MMU
      if (mmu_enabled() && mmu_is_translated<MMU_TT_DREAD>(addr, size))
		{
			if ((addr >> 12) != (block->vaddr >> 12))
				// When full mmu is on, only consider addresses in the same 4k page
				return false;
			u32 paddr;
			u32 rv;
			switch (size)
			{
			case 1:
				rv = mmu_data_translation<MMU_TT_DREAD, u8>(addr, paddr);
				break;
			case 2:
				rv = mmu_data_translation<MMU_TT_DREAD, u16>(addr, paddr);
				break;
			case 4:
			case 8:
				rv = mmu_data_translation<MMU_TT_DREAD, u32>(addr, paddr);
				break;
			default:
				die("Invalid immediate size");
				break;
			}
			if (rv != MMU_ERROR_NONE)
				return false;
			addr = paddr;
		}
#endif // NO_MMU
		bool isram = false;
		void* ptr = _vmem_read_const(addr, isram, size > 4 ? 4 : size);

		if (isram)
		{
			Ldr(x1, reinterpret_cast<uintptr_t>(ptr));	// faster than Mov
			if (regalloc.IsAllocAny(op.rd))
			{
				switch (size)
				{
				case 1:
					Ldrsb(regalloc.MapRegister(op.rd), MemOperand(x1));
					break;

				case 2:
					Ldrsh(regalloc.MapRegister(op.rd), MemOperand(x1));
					break;

				case 4:
					if (op.rd.is_r32f())
						Ldr(regalloc.MapVRegister(op.rd), MemOperand(x1));
					else
						Ldr(regalloc.MapRegister(op.rd), MemOperand(x1));
					break;

				default:
					die("Invalid size");
					break;
				}
			}
			else
			{
				switch (size)
				{
				case 1:
					Ldrsb(w1, MemOperand(x1));
					break;

				case 2:
					Ldrsh(w1, MemOperand(x1));
					break;

				case 4:
					Ldr(w1, MemOperand(x1));
					break;

				case 8:
					Ldr(x1, MemOperand(x1));
					break;

				default:
					die("Invalid size");
					break;
				}
				if (size == 8)
					Str(x1, sh4_context_mem_operand(op.rd.reg_ptr()));
				else
					Str(w1, sh4_context_mem_operand(op.rd.reg_ptr()));
			}
		}
		else
		{
			// Not RAM
			if (size == 8)
			{
				verify(!regalloc.IsAllocAny(op.rd));
				// Need to call the handler twice
				Mov(w0, addr);
				GenCallRuntime((void (*)())ptr);
				Str(w0, sh4_context_mem_operand(op.rd.reg_ptr()));

				Mov(w0, addr + 4);
				GenCallRuntime((void (*)())ptr);
				Str(w0, sh4_context_mem_operand((u8*)op.rd.reg_ptr() + 4));
			}
			else
			{
				Mov(w0, addr);

				switch(size)
				{
				case 1:
					GenCallRuntime((void (*)())ptr);
					Sxtb(w0, w0);
					break;

				case 2:
					GenCallRuntime((void (*)())ptr);
					Sxth(w0, w0);
					break;

				case 4:
					GenCallRuntime((void (*)())ptr);
					break;

				default:
					die("Invalid size");
					break;
				}

				if (regalloc.IsAllocg(op.rd))
					Mov(regalloc.MapRegister(op.rd), w0);
				else
				{
					verify(regalloc.IsAllocf(op.rd));
					Fmov(regalloc.MapVRegister(op.rd), w0);
				}
			}
		}

		return true;
	}

	bool GenReadMemoryFast(const shil_opcode& op, size_t opid)
	{
		// Direct memory access. Need to handle SIGSEGV and rewrite block as needed. See ngen_Rewrite()
		if (!_nvmem_enabled() || (mmu_enabled() && !vmem32_enabled()))
			return false;

		Instruction *start_instruction = GetCursorAddress<Instruction *>();

		// WARNING: the rewrite code relies on having 1 or 2 ops before the memory access
		// Update ngen_Rewrite (and perhaps read_memory_rewrite_size) if adding or removing code
		if (!_nvmem_4gb_space())
		{
			Ubfx(x1, *call_regs64[0], 0, 29);
			Add(x1, x1, sizeof(Sh4Context), LeaveFlags);
		}
		else
		{
			Add(x1, *call_regs64[0], sizeof(Sh4Context), LeaveFlags);
		}

		u32 size = op.flags & 0x7f;
		switch(size)
		{
		case 1:
			Ldrsb(w0, MemOperand(x28, x1));
			break;

		case 2:
			Ldrsh(w0, MemOperand(x28, x1));
			break;

		case 4:
			Ldr(w0, MemOperand(x28, x1));
			break;

		case 8:
			Ldr(x0, MemOperand(x28, x1));
			break;
		}
		EnsureCodeSize(start_instruction, read_memory_rewrite_size);

		return true;
	}

	void GenWriteMemory(const shil_opcode& op, size_t opid, bool optimise)
	{
		if (GenWriteMemoryImmediate(op))
			return;
		if (optimise && GenWriteMemoryCompact(op))
			return;

		GenMemAddr(op, call_regs[0]);
		if (mmu_enabled())
			Mov(*call_regs[2], block->vaddr + op.guest_offs - (op.delay_slot ? 2 : 0));	// pc

		u32 size = op.flags & 0x7f;
		if (size != 8)
			shil_param_to_host_reg(op.rs2, *call_regs[1]);
		else
		{
#ifdef EXPLODE_SPANS
			verify(op.rs2.count() == 2 && regalloc.IsAllocf(op.rs2, 0) && regalloc.IsAllocf(op.rs2, 1));
			Fmov(*call_regs[1], regalloc.MapVRegister(op.rs2, 1));
			Lsl(*call_regs64[1], *call_regs64[1], 32);
			Fmov(w2, regalloc.MapVRegister(op.rs2, 0));
			Orr(*call_regs64[1], *call_regs64[1], x2);
#else
			shil_param_to_host_reg(op.rs2, *call_regs64[1]);
#endif
		}
		if (optimise && GenWriteMemoryFast(op, opid))
			return;

		GenWriteMemorySlow(size);
	}

	bool GenWriteMemoryImmediate(const shil_opcode& op)
	{
		if (!op.rs1.is_imm())
			return false;

		u32 size = op.flags & 0x7f;
		u32 addr = op.rs1._imm;
#ifndef NO_MMU
      if (mmu_enabled() && mmu_is_translated<MMU_TT_DWRITE>(addr, size))
		{
			if ((addr >> 12) != (block->vaddr >> 12) && ((addr >> 12) != ((block->vaddr + block->guest_opcodes * 2 - 1) >> 12)))
				// When full mmu is on, only consider addresses in the same 4k page
				return false;
			u32 paddr;
			u32 rv;
			switch (size)
			{
			case 1:
				rv = mmu_data_translation<MMU_TT_DWRITE, u8>(addr, paddr);
				break;
			case 2:
				rv = mmu_data_translation<MMU_TT_DWRITE, u16>(addr, paddr);
				break;
			case 4:
			case 8:
				rv = mmu_data_translation<MMU_TT_DWRITE, u32>(addr, paddr);
				break;
			default:
				die("Invalid immediate size");
				break;
			}
			if (rv != MMU_ERROR_NONE)
				return false;
			addr = paddr;
		}
#endif // NO_MMU
		bool isram = false;
		void* ptr = _vmem_write_const(addr, isram, size > 4 ? 4 : size);
		// Data next to code (docs/tech_debits.md 4.19): write through the RAM
		// mirror that is never locked, unless the address is compiled code
		// right now -- then keep the locked view so SMC is detected.
		if (isram)
		{
			extern u8 *bm_ConstStoreAlias(RuntimeBlockInfo* block, u32 addr, u32 size);
			u8 *alias = bm_ConstStoreAlias(block, addr, size);
			if (alias != nullptr)
				ptr = alias;
		}

		Register reg2;
		if (size != 8)
		{
			if (op.rs2.is_imm())
			{
				Mov(w1, op.rs2._imm);
				reg2 = w1;
			}
			else if (regalloc.IsAllocg(op.rs2))
			{
				reg2 = regalloc.MapRegister(op.rs2);
			}
			else if (regalloc.IsAllocf(op.rs2))
			{
				Fmov(w1, regalloc.MapVRegister(op.rs2));
				reg2 = w1;
			}
			else
				die("Invalid rs2 param");
		}
		if (isram)
		{
			Ldr(x0, reinterpret_cast<uintptr_t>(ptr));
			switch (size)
			{
			case 1:
				Strb(reg2, MemOperand(x0));
				break;

			case 2:
				Strh(reg2, MemOperand(x0));
				break;

			case 4:
				Str(reg2, MemOperand(x0));
				break;

			case 8:
#ifdef EXPLODE_SPANS
				verify(op.rs2.count() == 2 && regalloc.IsAllocf(op.rs2, 0) && regalloc.IsAllocf(op.rs2, 1));
				Str(regalloc.MapVRegister(op.rs2, 0),  MemOperand(x1));
				Str(regalloc.MapVRegister(op.rs2, 1),  MemOperand(x1, 4));
#else
				shil_param_to_host_reg(op.rs2, x1);
				Str(x1, MemOperand(x0));
#endif
				break;

			default:
				die("Invalid size");
				break;
			}
		}
		else
		{
			// Not RAM
			Mov(w0, addr);
			if (size == 8)
			{
				// Need to call the handler twice
				shil_param_to_host_reg(op.rs2, x1);
				GenCallRuntime((void (*)())ptr);

				Mov(w0, addr + 4);
				shil_param_to_host_reg(op.rs2, x1);
				Lsr(x1, x1, 32);
				GenCallRuntime((void (*)())ptr);
			}
			else
			{
				Mov(w1, reg2);

				switch(size)
				{
				case 1:
					GenCallRuntime((void (*)())ptr);
					break;

				case 2:
					GenCallRuntime((void (*)())ptr);
					break;

				case 4:
					GenCallRuntime((void (*)())ptr);
					break;

				default:
					die("Invalid size");
					break;
				}
			}
		}

		return true;
	}

	bool GenWriteMemoryFast(const shil_opcode& op, size_t opid)
	{
		// Direct memory access. Need to handle SIGSEGV and rewrite block as needed. See ngen_Rewrite()
		if (!_nvmem_enabled() || (mmu_enabled() && !vmem32_enabled()))
			return false;

		Instruction *start_instruction = GetCursorAddress<Instruction *>();

		// WARNING: the rewrite code relies on having 1 or 2 ops before the memory access
		// Update ngen_Rewrite (and perhaps write_memory_rewrite_size) if adding or removing code
		if (!_nvmem_4gb_space())
		{
			Ubfx(x7, *call_regs64[0], 0, 29);
			Add(x7, x7, sizeof(Sh4Context), LeaveFlags);
		}
		else
		{
			Add(x7, *call_regs64[0], sizeof(Sh4Context), LeaveFlags);
		}

		u32 size = op.flags & 0x7f;
		switch(size)
		{
		case 1:
			Strb(w1, MemOperand(x28, x7));
			break;

		case 2:
			Strh(w1, MemOperand(x28, x7));
			break;

		case 4:
			Str(w1, MemOperand(x28, x7));
			break;

		case 8:
			Str(x1, MemOperand(x28, x7));
			break;
		}
		EnsureCodeSize(start_instruction, write_memory_rewrite_size);

		return true;
	}

	void EnsureCodeSize(Instruction *start_instruction, int code_size)
	{
		while (GetCursorAddress<Instruction *>() - start_instruction < code_size * kInstructionSize)
			Nop();
		// ESTOURO: se o codigo emitido ja passou do slot, o while acima nao faz
		// nada e o verify abaixo e NO-OP nesta build (-DNO_VERIFY) -- ou seja o
		// excesso sobrescreve silenciosamente o que vier depois. Logar alto.
		ptrdiff_t emitido = GetCursorAddress<Instruction *>() - start_instruction;
		if (emitido != code_size * kInstructionSize)
			ERROR_LOG(COMMON, "EnsureCodeSize ESTOUROU: emitiu %d instrucoes num slot de %d (bloco SH4 %08X)",
					(int)(emitido / kInstructionSize), code_size, block ? block->vaddr : 0);
		verify (GetCursorAddress<Instruction *>() - start_instruction == code_size * kInstructionSize);
	}

	void CheckBlock(bool force_checks, RuntimeBlockInfo* block)
	{
		if (!mmu_enabled() && !force_checks)
			return;

		Label blockcheck_fail;

		if (mmu_enabled())
		{
			Ldr(w10, sh4_context_mem_operand(&next_pc));
			Ldr(w11, block->vaddr);
			Cmp(w10, w11);
			B(ne, &blockcheck_fail);
		}
		if (force_checks)
		{
			s32 sz = block->sh4_code_size;
			u8* ptr = GetMemPtr(block->addr, sz);
			if (ptr != NULL)
			{
				Ldr(x9, reinterpret_cast<uintptr_t>(ptr));

				while (sz > 0)
				{
					if (sz >= 8)
					{
						Ldr(x10, MemOperand(x9, 8, PostIndex));
						Ldr(x11, *(u64*)ptr);
						Cmp(x10, x11);
						sz -= 8;
						ptr += 8;
					}
					else if (sz >= 4)
					{
						Ldr(w10, MemOperand(x9, 4, PostIndex));
						Ldr(w11, *(u32*)ptr);
						Cmp(w10, w11);
						sz -= 4;
						ptr += 4;
					}
					else
					{
						Ldrh(w10, MemOperand(x9, 2, PostIndex));
						Mov(w11, *(u16*)ptr);
						Cmp(w10, w11);
						sz -= 2;
						ptr += 2;
					}
					B(ne, &blockcheck_fail);
				}
			}
		}
		Label blockcheck_success;
		B(&blockcheck_success);
		Bind(&blockcheck_fail);
		Ldr(w0, block->addr);
		TailCallRuntime(ngen_blockcheckfail);

		Bind(&blockcheck_success);

		if (mmu_enabled() && block->has_fpu_op)
		{
			Label fpu_enabled;
			Ldr(w10, sh4_context_mem_operand(&sr));
			Tbz(w10, 15, &fpu_enabled);			// test SR.FD bit

			Mov(*call_regs[0], block->vaddr);	// pc
			Mov(*call_regs[1], 0x800);			// event
			Mov(*call_regs[2], 0x100);			// vector
			CallRuntime(Do_Exception);
			Ldr(w29, sh4_context_mem_operand(&next_pc));
			GenBranch(*arm64_no_update);

			Bind(&fpu_enabled);
		}
	}

	void shil_param_to_host_reg(const shil_param& param, const Register& reg)
	{
		if (param.is_imm())
		{
			Mov(reg, param._imm);
		}
		else if (param.is_reg())
		{
			if (param.is_r64f())
				Ldr(reg, sh4_context_mem_operand(param.reg_ptr()));
			else if (param.is_r32f())
			{
				if (regalloc.IsAllocf(param))
					Fmov(reg, regalloc.MapVRegister(param));
				else
					Ldr(reg, sh4_context_mem_operand(param.reg_ptr()));
			}
			else
			{
				if (regalloc.IsAllocg(param))
					Mov(reg, regalloc.MapRegister(param));
				else
					Ldr(reg, sh4_context_mem_operand(param.reg_ptr()));
			}
		}
		else
		{
			verify(param.is_null());
		}
	}

	void host_reg_to_shil_param(const shil_param& param, const CPURegister& reg)
	{
		if (reg.Is64Bits())
		{
			Str((const Register&)reg, sh4_context_mem_operand(param.reg_ptr()));
		}
		else if (regalloc.IsAllocg(param))
		{
			if (reg.IsRegister())
				Mov(regalloc.MapRegister(param), (const Register&)reg);
			else
				Fmov(regalloc.MapRegister(param), (const VRegister&)reg);
		}
		else if (regalloc.IsAllocf(param))
		{
			if (reg.IsVRegister())
				Fmov(regalloc.MapVRegister(param), (const VRegister&)reg);
			else
				Fmov(regalloc.MapVRegister(param), (const Register&)reg);
		}
		else
		{
			Str(reg, sh4_context_mem_operand(param.reg_ptr()));
		}
	}

	struct CC_PS
	{
		CanonicalParamType type;
		shil_param* prm;
	};
	std::vector<CC_PS> CC_pars;
	std::vector<const WRegister*> call_regs;
	std::vector<const XRegister*> call_regs64;
	std::vector<const VRegister*> call_fregs;
	Arm64RegAlloc regalloc;
	RuntimeBlockInfo* block = NULL;
	const int read_memory_rewrite_size = 3;	// ubfx, add, ldr
	const int write_memory_rewrite_size = 3; // ubfx, add, str
};

static Arm64Assembler* compiler;

void ngen_Compile(RuntimeBlockInfo* block, bool force_checks, bool reset, bool staging, bool optimise)
{
	verify(emit_FreeSpace() >= 16 * 1024);

	compiler = new Arm64Assembler();

	compiler->ngen_Compile(block, force_checks, reset, staging, optimise);

	delete compiler;
	compiler = NULL;
}

void ngen_CC_Start(shil_opcode* op)
{
	compiler->ngen_CC_Start(op);
}

void ngen_CC_Param(shil_opcode* op, shil_param* par, CanonicalParamType tp)
{
	compiler->ngen_CC_Param(*op, *par, tp);
}

void ngen_CC_Call(shil_opcode*op, void* function)
{
	compiler->ngen_CC_Call(op, function);
}

void ngen_CC_Finish(shil_opcode* op)
{

}

#define STR_LDR_MASK   0xFFE0EC00

static const u32 armv8_mem_ops[] = {
		0x38E06800,		// Ldrsb
		0x78E06800,		// Ldrsh
		0xB8606800,		// Ldr w
		0xF8606800,		// Ldr x
		0x38206800,		// Strb
		0x78206800,		// Strh
		0xB8206800,		// Str w
		0xF8206800,		// Str x
};
static const bool read_ops[] = {
		true,
		true,
		true,
		true,
		false,
		false,
		false,
		false,
};
static const u32 op_sizes[] = {
		1,
		2,
		4,
		8,
		1,
		2,
		4,
		8,
};
// Acesso compacto (GenRead/WriteMemoryCompact): ldr/str (registrador, UXTW)
// com base x13. Mesmo layout de bits do armv8_mem_ops acima, com option=010.
static const u32 compact_mem_ops[] = {
		0x38E04800,		// Ldrsb w
		0x78E04800,		// Ldrsh w
		0xB8604800,		// Ldr w
		0x38204800,		// Strb
		0x78204800,		// Strh
		0xB8204800,		// Str w
		0xF8604800,		// Ldr x
		0xF8204800,		// Str x
		0x38604800,		// Ldrb w (sem sinal, ZeroExtendLoadPass)
		0x78604800,		// Ldrh w
};
static const bool compact_read_ops[] = { true, true, true, false, false, false, true, false, true, true };
static const u32 compact_op_sizes[] = { 1, 2, 4, 1, 2, 4, 8, 8, 1, 2 };
static const bool compact_unsigned[] = { false, false, false, false, false, false, false, false, true, true };

static bool DecodeCompactMem(u32 op, bool& is_read, u32& size, u32& rt, u32& rm, bool *is_unsigned = nullptr)
{
	if (((op >> 5) & 31) != 13)
		return false;
	const u32 masked = op & STR_LDR_MASK;
	for (int i = 0; i < ARRAY_SIZE(compact_mem_ops); i++)
		if (masked == compact_mem_ops[i])
		{
			is_read = compact_read_ops[i];
			size = compact_op_sizes[i];
			if (is_unsigned != nullptr)
				*is_unsigned = compact_unsigned[i];
			rt = op & 31;
			rm = (op >> 16) & 31;
			return true;
		}
	return false;
}

// Gera o trampolim do slow path e troca a instrucao do acesso por um `b` para
// ele. host_pc nao muda: ao voltar do handler, a CPU executa o `b`.
static bool RewriteCompactMem(unat host_pc, bool is_read, u32 size, u32 rt, u32 rm, void *target, bool is_unsigned = false, bool is_stub = false, bool is_sq = false)
{
	u16 live = 0;
	if (!is_stub || is_sq)
	{
		auto it = compact_live_fregs.find((uintptr_t)host_pc);
		if (it != compact_live_fregs.end())
			live = it->second;
	}
	if (emit_FreeSpace() < 1024)
	{
		ERROR_LOG(DYNAREC, "RewriteCompactMem: sem espaco no cache de codigo para o trampolim");
		return false;
	}
	Arm64Assembler *a = new Arm64Assembler();
	void *tramp_rx = CC_RW2RX(a->GetBuffer()->GetStartAddress<void*>());
	if (is_sq)
		a->GenCompactTrampolineSQ(rt, rm, (void *)(host_pc + 4), live);
	else
		a->GenCompactTrampoline(is_read, size, rt, rm, target, (void *)(host_pc + 4), is_unsigned, live);
	a->FinalizeStub();
	delete a;

	u32 *code_rw = (u32 *)CC_RX2RW(host_pc);
	ptrdiff_t off = (u8 *)tramp_rx - (u8 *)host_pc;
	verify(off >= -128 * 1024 * 1024 && off < 128 * 1024 * 1024);
	*code_rw = 0x14000000 | ((u32)(off >> 2) & 0x03FFFFFF);
	vmem_platform_flush_cache((void *)host_pc, (u8 *)host_pc + 4, code_rw, (u8 *)code_rw + 4);
	extern u32 g_compactMemRewrites;
	g_compactMemRewrites++;
	// FC_COMPACT_LOG: confirma que o caminho de fault -> trampolim roda.
	static int logLeft = getenv("FC_COMPACT_LOG") != nullptr ? std::max(1, atoi(getenv("FC_COMPACT_LOG"))) : 0;
	if (logLeft > 0)
	{
		logLeft--;
		fprintf(stderr, "COMPACT rewrite #%u pc=%zx read=%d size=%u rt=w%u rm=w%u target=%p\n",
				g_compactMemRewrites, (size_t)host_pc, (int)is_read, size, rt, rm, target);
	}
	return true;
}
u32 g_compactMemRewrites;

static void *CompactSlowTarget(bool is_read, u32 size)
{
	if (is_read)
		return size == 1 ? (void *)ReadMem8 : size == 2 ? (void *)ReadMem16 : size == 4 ? (void *)ReadMem32 : (void *)ReadMem64;
	return size == 1 ? (void *)WriteMem8 : size == 2 ? (void *)WriteMem16 : size == 4 ? (void *)WriteMem32 : (void *)WriteMem64;
}

bool ngen_Rewrite(unat& host_pc, unat, unat acc)
{
	{
		bool c_read, c_unsigned = false; u32 c_size, c_rt, c_rm;
		if (DecodeCompactMem(*(u32 *)CC_RX2RW(host_pc), c_read, c_size, c_rt, c_rm, &c_unsigned))
		{
			static const bool noSqStubC = getenv("FC_NO_SQ_STUB") != nullptr;
			void *target = CompactSlowTarget(c_read, c_size);
			bool stub = false;
			if (!noSqStubC && !c_read && c_size == 4 && sq_write_stub != nullptr && ((u32)acc >> 26) == 0x38)
			{
				target = sq_write_stub;
				stub = true;
			}
			return RewriteCompactMem(host_pc, c_read, c_size, c_rt, c_rm, target, c_unsigned, stub, stub);
		}
	}

	//LOGI("ngen_Rewrite pc %zx\n", host_pc);
	u32 *code_ptr = (u32 *)CC_RX2RW(host_pc);
	u32 armv8_op = *code_ptr;
	bool is_read = false;
	// `size` PRECISA ser inicializada: o verify(found) abaixo e um NO-OP nesta
	// build (-DNO_VERIFY), entao uma instrucao nao reconhecida seguia adiante
	// com lixo de pilha aqui e batia no die("1..8 bytes") de
	// GenWriteMemorySlow(). Ver docs/tech_debits.md.
	u32 size = 0;
	bool found = false;
	u32 masked = armv8_op & STR_LDR_MASK;
	for (int i = 0; i < ARRAY_SIZE(armv8_mem_ops); i++)
	{
		if (masked == armv8_mem_ops[i])
		{
			size = op_sizes[i];
			is_read = read_ops[i];
			found = true;
			break;
		}
	}
	if (!found)
	{
		// Instrucao de acesso a memoria que este decodificador nao conhece.
		// Logar o encoding completo: sem isso o sintoma e um die() generico
		// e nao da pra saber qual forma de endereçamento faltou.
		// Nem todo fault dentro do buffer JIT e um acesso fastmem ao guest:
		// pode ser acesso a pilha (Push/PopCallerSaved usam LDP/STP com Rn=SP),
		// e nesse caso o fault e GENUINO e nao ha call site para reescrever.
		// Devolver false faz o handler de SIGSEGV tratar como fault real e
		// reportar com a linha "SIGSEGV @ ..." em vez de morrer aqui com a
		// mensagem enganosa "1..8 bytes" (o verify(found) abaixo e no-op nesta
		// build, -DNO_VERIFY, entao antes disto seguia com `size` lixo).
		ERROR_LOG(COMMON, "ngen_Rewrite: instrucao ARM64 nao reconhecida em %zx: op=%08X masked=%08X acc=%zx -- tratando como fault genuino",
				(size_t)host_pc, armv8_op, masked, (size_t)acc);
		return false;
	}
	verify(found);

	// Skip the preceding ops (add, ubfx)
	u32 *code_rewrite = code_ptr - 1 - (!_nvmem_4gb_space() ? 1 : 0);
	Arm64Assembler *assembler = new Arm64Assembler(code_rewrite);
	// Store Queue fast path (docs/arm64jit_improvement_plan.md item 1,
	// docs/tech_debits.md item 1.7): this call site's fastmem attempt just
	// faulted -- if the address it faulted on is a Store Queue address
	// (0xE0000000-0xE3FFFFFF, addr>>26==0x38), redirect THIS ONE call site to
	// the shared sq_write_stub instead of the generic slow path. No other
	// 32-bit write anywhere else in the game is touched by this check: it
	// only ever runs here, once, after an actual fault -- not on every write
	// like the earlier (reverted) attempt at this same optimization.
	// FC_NO_SQ_STUB=1 desliga o redirecionamento para o stub de Store Queue
	// (item 1.7) e usa o caminho lento generico. Teste de bissecao: o crash do
	// meltybld acontece dentro do caminho not_sq DESSE stub, com SP ja
	// corrompido na entrada -- isto responde se o stub esta implicado na causa
	// ou se e so onde a corrupcao vira fault. Ver tech_debits item 1.10.
	static const bool noSqStub = getenv("FC_NO_SQ_STUB") != nullptr;
	if (!noSqStub && !is_read && size == 4 && sq_write_stub != nullptr && ((u32)acc >> 26) == 0x38)
		assembler->GenCallStubAddr(sq_write_stub);
	else if (is_read)
		assembler->GenReadMemorySlow(size);
	else
		assembler->GenWriteMemorySlow(size);
	assembler->Finalize(true);
	delete assembler;
	host_pc = (unat)CC_RW2RX(code_rewrite);

	return true;
}

// Called by the fault handler when a JIT store faulted on a locked code page
// and the bytes it writes are not compiled code (bm_IsDataWriteToCodePage):
// point THIS store site at the code-page stub and keep the page protected.
bool ngen_RewriteCodePageStore(unat& host_pc)
{
	{
		bool c_read; u32 c_size, c_rt, c_rm;
		if (DecodeCompactMem(*(u32 *)CC_RX2RW(host_pc), c_read, c_size, c_rt, c_rm))
		{
			if (c_read)
				return false;
			const int cidx = c_size == 1 ? 0 : c_size == 2 ? 1 : c_size == 4 ? 2 : 3;
			if (codepage_write_stub[cidx] == nullptr)
				return false;
			if (!RewriteCompactMem(host_pc, false, c_size, c_rt, c_rm, codepage_write_stub[cidx], false, true))
				return false;
			extern u32 g_codePageRewrites;
			g_codePageRewrites++;
			return true;
		}
	}
	u32 *code_ptr = (u32 *)CC_RX2RW(host_pc);
	const u32 masked = *code_ptr & STR_LDR_MASK;
	u32 size = 0;
	bool found = false;
	for (int i = 0; i < ARRAY_SIZE(armv8_mem_ops); i++)
		if (masked == armv8_mem_ops[i])
		{
			if (read_ops[i])
				return false;
			size = op_sizes[i];
			found = true;
			break;
		}
	if (!found)
		return false;
	const int idx = size == 1 ? 0 : size == 2 ? 1 : size == 4 ? 2 : size == 8 ? 3 : -1;
	if (idx < 0 || codepage_write_stub[idx] == nullptr)
		return false;

	// Same layout ngen_Rewrite() relies on: 1 or 2 ops before the access.
	u32 *code_rewrite = code_ptr - 1 - (!_nvmem_4gb_space() ? 1 : 0);
	Arm64Assembler *assembler = new Arm64Assembler(code_rewrite);
	assembler->GenCallStubAddr(codepage_write_stub[idx]);
	assembler->Finalize(true);
	delete assembler;
	host_pc = (unat)CC_RW2RX(code_rewrite);
	extern u32 g_codePageRewrites;
	g_codePageRewrites++;
	return true;
}

static void generate_mainloop()
{
	if (mainloop != nullptr)
		return;
	compiler = new Arm64Assembler();

	compiler->GenMainloop();

	delete compiler;
	compiler = nullptr;

	// Separate assembler instance/Finalize cycle on purpose -- GenMainloop()
	// already did its own FinalizeCode()+emit_Skip() pass; reusing the same
	// instance here would double-count the cursor advance.
	compiler = new Arm64Assembler();

	compiler->GenMemStubs();

	delete compiler;
	compiler = nullptr;
}

RuntimeBlockInfo* ngen_AllocateBlock()
{
	generate_mainloop();
	return new DynaRBI();
}

void ngen_HandleException()
{
	longjmp(jmp_env, 1);
}

u32 DynaRBI::Relink()
{
	//printf("DynaRBI::Relink %08x\n", this->addr);
	Arm64Assembler *compiler = new Arm64Assembler((u8 *)this->code + this->relink_offset);

	u32 code_size = compiler->RelinkBlock(this);
	compiler->Finalize(true);
	delete compiler;

	return code_size;
}

void Arm64RegAlloc::Preload(u32 reg, eReg nreg)
{
	assembler->Ldr(Register(nreg, 32), assembler->sh4_context_mem_operand(GetRegPtr(reg)));
}
void Arm64RegAlloc::Writeback(u32 reg, eReg nreg)
{
	assembler->Str(Register(nreg, 32), assembler->sh4_context_mem_operand(GetRegPtr(reg)));
}
void Arm64RegAlloc::Preload_FPU(u32 reg, eFReg nreg)
{
	assembler->Ldr(VRegister(nreg, 32), assembler->sh4_context_mem_operand(GetRegPtr(reg)));
}
void Arm64RegAlloc::Writeback_FPU(u32 reg, eFReg nreg)
{
	assembler->Str(VRegister(nreg, 32), assembler->sh4_context_mem_operand(GetRegPtr(reg)));
}

u16 Arm64RegAlloc::LiveCallerSavedFMask()
{
	u16 mask = 0;
	for (auto const& it : reg_alloced)
		if (IsFloat(it.first))
		{
			eFReg hreg = (eFReg)it.second.host_reg;
			if (hreg >= S16 && hreg <= S31)
				mask |= 1 << (hreg - S16);
		}
	return mask;
}

void Arm64RegAlloc::PushCallerSaved()
{
	CPURegList vlist(CPURegister::kVRegister, 64, 0);
	for (auto const& it : reg_alloced)
	{
		if (IsFloat(it.first))
		{
			eFReg hreg = (eFReg)it.second.host_reg;
			if (hreg >= S16 && hreg <= S31)
				vlist.Combine(VRegister::GetDRegFromCode(hreg));
		}
	}
	if ((vlist.GetCount() % 2) != 0) vlist.Combine(d7);
	if (!vlist.IsEmpty()) assembler->PushCPURegList(vlist);
}

void Arm64RegAlloc::PopCallerSaved()
{
	CPURegList vlist(CPURegister::kVRegister, 64, 0);
	for (auto const& it : reg_alloced)
	{
		if (IsFloat(it.first))
		{
			eFReg hreg = (eFReg)it.second.host_reg;
			if (hreg >= S16 && hreg <= S31)
				vlist.Combine(VRegister::GetDRegFromCode(hreg));
		}
	}
	if ((vlist.GetCount() % 2) != 0) vlist.Combine(d7);
	if (!vlist.IsEmpty()) assembler->PopCPURegList(vlist);
}


extern "C" naked void do_sqw_nommu_area_3(u32 dst, u8* sqb)
{
	__asm__
	(
		"and x12, x0, #0x20			\n\t"	// SQ# selection, isolate
		"add x12, x12, x1			\n\t"	// SQ# selection, add to SQ ptr
		"ld2 { v0.2D, v1.2D }, [x12]\n\t"
		"movz x11, #0x0C00, lsl #16 \n\t"
		"add x11, x1, x11			\n\t"	// get ram ptr from x1, part 1
		"ubfx x0, x0, #5, #20		\n\t"	// get ram offset
		"add x11, x11, #512			\n\t"	// get ram ptr from x1, part 2
		"add x11, x11, x0, lsl #5	\n\t"	// ram + offset
		"st2 { v0.2D, v1.2D }, [x11] \n\t"
		"ret						\n"

		: : : "memory"
	);
}
#endif	// FEAT_SHREC == DYNAREC_JIT
