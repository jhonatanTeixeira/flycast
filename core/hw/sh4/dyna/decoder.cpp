/*
	Ugly, hacky, bad code
	It decodes sh4 opcodes too
*/

#include "types.h"

#if FEAT_SHREC != DYNAREC_NONE

#include "decoder.h"
#include "shil.h"
#include "ngen.h"
#include "hw/sh4/sh4_opcode_list.h"
#include "hw/sh4/sh4_core.h"
#include "hw/sh4/sh4_mem.h"
#include "decoder_opcodes.h"

#define BLOCK_MAX_SH_OPS_SOFT 500
#define BLOCK_MAX_SH_OPS_HARD 511

static RuntimeBlockInfo* blk;

static const char idle_hash[] =
	//BIOS
       ">:1:05:13B23363"
       ">:1:04:2E23A33B"
       ">:1:04:FB498832"
       ">:1:0A:50A249F9"

	//SC
       ">:1:0A:B4E90338"
       ">:1:04:11578A16"
       ">:1:04:C281CC52"

	//HH
       ">:1:07:0757DC10"
       ">:1:04:1476CC5E"

	//these look very suspicious, but I'm not sure about any of them
	//cross testing w/ IKA makes them more suspects than not
       ">:1:0D:8C2921FF"
       ">:1:04:B806EEE4"

       // Dead or Alive 2
       ">:1:08:0A37187A";

// Idle fast-forward signatures (docs/tech_debits.md 4.14). Each one is guest
// code where arriving at `entry` PROVES the CPU is only waiting, so the block
// that starts there gets `idle_fastforward` and the JIT jumps time to the next
// scheduled event instead of emulating the wait cycle by cycle -- the same
// idea as Dolphin's idle skipping.
//
// Why not idle_hash above: RuntimeBlockInfo::hash() reads the block as bytes
// and only its first `guest_opcodes` of them, so a short block hashes just its
// first couple of instructions -- for the 4-op block below that would be
// "mov r15,r4 / add #4,r4", which any compiled C function taking a local's
// address starts with. Matching the whole surrounding function is exact.
//
// `mask` lets bsr displacements vary (0xF000 keeps only the opcode class), so
// the same library linked at another address in another game still matches.
struct IdleFFSig
{
	const char *name;
	u32 entry;			// byte offset of the waiting PC from the start of ops[]
	u32 count;
	u16 ops[32];
	u16 mask[32];
};

#define M_ 0xFFFF
#define MB 0xF000
static const IdleFFSig idle_ff_sigs[] = {
	// Cooperative task kernel linked into several games from different
	// makers and boards -- found in kofxi at 8C023128, and the same code
	// matches in kofnw (Atomiswave) and MBAA (not Atomiswave), so it's a
	// shared library, not a board feature.
	// yield():  next = cur < 4 ? cur + 1 : cur;  switch_to(next);
	// Task 4 is the idle task: when it yields it switches to ITSELF, and it
	// does so ~2100 times per frame waiting for the next interrupt -- 75% of
	// all JIT host instructions. 8C023146 (+0x1E) is only reachable through
	// the `bt` taken when cur >= 4, i.e. only on that self-yield; the ~2 real
	// yields per frame (cur < 4) enter the block at +0x18 instead.
	{ "coop-task-kernel-idle-yield", 0x1E, 26,
	  { 0x4F22, 0x7FF8, 0x64F3, 0x7404, 0xB000, 0x0009, 0xB000, 0x0009,
	    0x2F02, 0xE304, 0x3033, 0x8902, 0x61F2, 0x7101, 0x2F12, 0x64F3,
	    0x7404, 0xB000, 0x0009, 0x64F2, 0xB000, 0x0009, 0x7F08, 0x4F26,
	    0x000B, 0x0009 },
	  { M_, M_, M_, M_, MB, M_, MB, M_,
	    M_, M_, M_, M_, M_, M_, M_, M_,
	    M_, MB, M_, M_, MB, M_, M_, M_,
	    M_, M_ } },
	// Laco de espera do Dead or Alive 2 (8C12F99E, docs/tech_debits.md 4.38):
	//   while (*flag == r14) { (*contador)--; tarefa(); }
	// O flag so muda por interrupcao (vblank/fim de render). Era ~64% do tempo
	// emulado, 184 mil voltas por segundo emulado e ~286 mil UpdateSystem/s,
	// porque o idle_hash antigo so cobra 224 ciclos por bloco. O contador ja
	// descia ~17x menos que no hardware (690 ciclos cobrados por volta contra
	// ~40 reais) sem efeito visivel; o avanco ate o proximo evento e a mesma
	// distorcao, maior. Entrada = o proprio bloco do teste do flag.
	{ "doa2-wait-flag-loop", 0x00, 8,
	  { 0x63F2, 0x6232, 0x72FF, 0x2322, 0x53F1, 0x6232, 0x32E0, 0x8B03 },
	  { M_, M_, M_, M_, M_, M_, M_, M_ } },
	// Laco de espera do Shenmue (0C03A1F8, docs/tech_debits.md 4.38):
	//   do { tarefa(); compara contadores; } while (*flag != 0);
	// A tarefa e quase vazia (2-11 instrucoes, volta sem trabalho); o flag e
	// zerado por interrupcao. ~50% do tempo emulado na cena pesada do save.
	// Assinatura = bloco de comparacao (0C03A1E0) + 2 instrucoes que nao
	// rodam no laco (mascara 0) + teste do flag; entrada no teste (+0x18).
	{ "shenmue-wait-flag-loop", 0x18, 16,
	  { 0xD120, 0x6312, 0xD020, 0x6202, 0xD11B, 0x323C, 0x7201, 0x6312,
	    0x3326, 0x8B01, 0x0000, 0x0000, 0xD318, 0x6232, 0x2228, 0x8BEB },
	  { M_, M_, M_, M_, M_, M_, M_, M_,
	    M_, M_, 0x0000, 0x0000, M_, M_, M_, M_ } },
};
#undef M_
#undef MB

static bool idle_ff_match(u32 addr)
{
	static int enabled = -1;
	if (enabled == -1)
	{
		const char *e = getenv("FC_IDLE_FF");
		enabled = (e == nullptr || atoi(e) != 0) ? 1 : 0;
	}
	if (!enabled)
		return false;
	for (const IdleFFSig& sig : idle_ff_sigs)
	{
		if (addr < sig.entry)
			continue;
		const u16 *code = (const u16 *)GetMemPtr(addr - sig.entry, sig.count * 2);
		if (code == nullptr)
			continue;
		u32 i = 0;
		while (i < sig.count && (code[i] & sig.mask[i]) == sig.ops[i])
			i++;
		if (i == sig.count)
		{
			INFO_LOG(DYNAREC, "Idle fast-forward: %s at %08X", sig.name, addr);
			return true;
		}
	}
	return false;
}

// Laco de ATRASO por contagem (docs/tech_debits.md 4.40), achado no Le Mans
// 24h (8C1730F6, 54% do tempo emulado e 69% do trabalho do JIT):
//   L: mov.l @(disp,PC),r1 ; mov.l @r1,r2 ; add #1,r2 ; mov.l r2,@r1
//      tst r4,r4 ; bf.s L ; add #-1,r4
// = while (r4-- != 0) (*contador)++  (N+1 voltas, r4 termina em -1). Nao
// espera evento nenhum: o numero de voltas e fixo. O JIT pula k voltas de uma
// vez na entrada (contador += k, r4 -= k, k*ciclos cobrados), sem passar do
// proximo evento e deixando ao menos uma volta real para a saida normal.
// So o deslocamento do literal pode variar; o desvio tem de voltar ao bloco.
static bool delay_loop_match(u32 addr)
{
	static int enabled = -1;
	if (enabled == -1)
		enabled = getenv("FC_NO_DELAY_SKIP") != nullptr ? 0 : 1;
	if (!enabled)
		return false;
	static const u16 ops[7]  = { 0xD100, 0x6212, 0x7201, 0x2122, 0x2448, 0x8FF9, 0x74FF };
	static const u16 mask[7] = { 0xFF00, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };
	const u16 *code = (const u16 *)GetMemPtr(addr, 14);
	if (code == nullptr)
		return false;
	for (int i = 0; i < 7; i++)
		if ((code[i] & mask[i]) != ops[i])
			return false;
	// bf.s em addr+10 com disp -7: alvo = addr+10+4-14 = addr (volta ao bloco)
	INFO_LOG(DYNAREC, "Delay loop skip at %08X", addr);
	return true;
}

static inline shil_param mk_imm(u32 immv)
{
	return shil_param(FMT_IMM,immv);
}

static inline shil_param mk_reg(Sh4RegType reg)
{
	return shil_param(reg);
}

static inline shil_param mk_regi(int reg)
{
	return mk_reg((Sh4RegType)reg);
}

static state_t state;

static void Emit(shilop op,
      shil_param rd=shil_param(),
      shil_param rs1=shil_param(),
      shil_param rs2=shil_param(),
      u32 flags=0,
      shil_param rs3=shil_param(),
      shil_param rd2=shil_param()
      )
{
	shil_opcode sp;
		
	sp.flags=flags;
	sp.op=op;
	sp.rd=(rd);
	sp.rd2=(rd2);
	sp.rs1=(rs1);
	sp.rs2=(rs2);
	sp.rs3=(rs3);
	sp.guest_offs=state.cpu.rpc-blk->vaddr;
	sp.delay_slot = state.cpu.is_delayslot;

	blk->oplist.push_back(sp);
}

static void dec_fallback(u32 op)
{
	shil_opcode opcd;
	opcd.op=shop_ifb;

	opcd.rs1=shil_param(FMT_IMM,OPCODE_NEEDPC(OpDesc[op]->type));

	opcd.rs2=shil_param(FMT_IMM,state.cpu.rpc+2);
	opcd.rs3=shil_param(FMT_IMM,op);

	opcd.guest_offs = state.cpu.rpc - blk->vaddr;
	opcd.delay_slot = state.cpu.is_delayslot;
	blk->oplist.push_back(opcd);
}

static void dec_DynamicSet(u32 regbase,u32 offs=0)
{
	if (offs==0)
		Emit(shop_jdyn,reg_pc_dyn,mk_reg((Sh4RegType)regbase));
	else
		Emit(shop_jdyn,reg_pc_dyn,mk_reg((Sh4RegType)regbase),mk_imm(offs));
}

static void dec_End(u32 dst,BlockEndType flags,bool delay)
{
	if (state.ngen.OnlyDynamicEnds && flags == BET_StaticJump)
	{
		Emit(shop_mov32,mk_reg(reg_nextpc),mk_imm(dst));
		dec_DynamicSet(reg_nextpc);
		dec_End(0xFFFFFFFF,BET_DynamicJump,delay);
		return;
	}

	if (state.ngen.OnlyDynamicEnds)
	{
		verify(flags == BET_DynamicJump);
	}

	state.BlockType=flags;
	state.NextOp=delay?NDO_Delayslot:NDO_End;
	state.DelayOp=NDO_End;
	state.JumpAddr=dst;
	state.NextAddr=state.cpu.rpc+2+(delay?2:0);
}

#define GetN(str) ((str>>8) & 0xf)
#define GetM(str) ((str>>4) & 0xf)
#define GetImm4(str) ((str>>0) & 0xf)
#define GetImm8(str) ((str>>0) & 0xff)
#define GetSImm8(str) ((s8)((str>>0) & 0xff))
#define GetImm12(str) ((str>>0) & 0xfff)
#define GetSImm12(str) (((s16)((GetImm12(str))<<4))>>4)


#define SR_STATUS_MASK 0x700083F2
#define SR_T_MASK 1

static u32 dec_jump_simm8(u32 op)
{
	return state.cpu.rpc + GetSImm8(op)*2 + 4;
}
static u32 dec_jump_simm12(u32 op)
{
	return state.cpu.rpc + GetSImm12(op)*2 + 4;
}
static u32 dec_set_pr()
{
	u32 retaddr=state.cpu.rpc + 4;
	Emit(shop_mov32,reg_pr,mk_imm(retaddr));
	return retaddr;
}
static void dec_write_sr(shil_param src)
{
	Emit(shop_and,mk_reg(reg_sr_status),src,mk_imm(SR_STATUS_MASK));
	Emit(shop_and,mk_reg(reg_sr_T),src,mk_imm(SR_T_MASK));
}
//bf <bdisp8>
sh4dec(i1000_1011_iiii_iiii)
{
	dec_End(dec_jump_simm8(op),BET_Cond_0,false);
}
//bf.s <bdisp8>
sh4dec(i1000_1111_iiii_iiii)
{
	blk->has_jcond=true;
	Emit(shop_jcond,reg_pc_dyn,reg_sr_T);
	dec_End(dec_jump_simm8(op),BET_Cond_0,true);
}
//bt <bdisp8>
sh4dec(i1000_1001_iiii_iiii)
{
	dec_End(dec_jump_simm8(op),BET_Cond_1,false);
}
//bt.s <bdisp8>
sh4dec(i1000_1101_iiii_iiii)
{
	blk->has_jcond=true;
	Emit(shop_jcond,reg_pc_dyn,reg_sr_T);
	dec_End(dec_jump_simm8(op),BET_Cond_1,true);
}
//bra <bdisp12>
sh4dec(i1010_iiii_iiii_iiii)
{
	dec_End(dec_jump_simm12(op),BET_StaticJump,true);
}
//braf <REG_N>
sh4dec(i0000_nnnn_0010_0011)
{
	u32 n = GetN(op);

	dec_DynamicSet(reg_r0+n,state.cpu.rpc + 4);
	dec_End(0xFFFFFFFF,BET_DynamicJump,true);
}
//jmp @<REG_N>
sh4dec(i0100_nnnn_0010_1011)
{
	u32 n = GetN(op);

	dec_DynamicSet(reg_r0+n);
	dec_End(0xFFFFFFFF,BET_DynamicJump,true);
}
//bsr <bdisp12>
sh4dec(i1011_iiii_iiii_iiii)
{
	//TODO: set PR
	dec_set_pr();
	dec_End(dec_jump_simm12(op),BET_StaticCall,true);
}
//bsrf <REG_N>
sh4dec(i0000_nnnn_0000_0011)
{
	u32 n = GetN(op);
	//TODO: set PR
	u32 retaddr=dec_set_pr();
	dec_DynamicSet(reg_r0+n,retaddr);
	dec_End(0xFFFFFFFF,BET_DynamicCall,true);
}
//jsr @<REG_N>
sh4dec(i0100_nnnn_0000_1011) 
{
	u32 n = GetN(op);

	//TODO: Set pr
	dec_set_pr();
	dec_DynamicSet(reg_r0+n);
	dec_End(0xFFFFFFFF,BET_DynamicCall,true);
}
//rts
sh4dec(i0000_0000_0000_1011)
{
	dec_DynamicSet(reg_pr);
	dec_End(0xFFFFFFFF,BET_DynamicRet,true);
}
//rte
sh4dec(i0000_0000_0010_1011)
{
	//TODO: Write SR, Check intr
	dec_write_sr(reg_ssr);
	Emit(shop_sync_sr);
	dec_DynamicSet(reg_spc);
	dec_End(0xFFFFFFFF,BET_DynamicIntr,true);
}
//trapa #<imm>
sh4dec(i1100_0011_iiii_iiii)
{
	//TODO: ifb
	dec_fallback(op);
	dec_DynamicSet(reg_nextpc);
	dec_End(0xFFFFFFFF,BET_DynamicJump,false);
}
//sleep
sh4dec(i0000_0000_0001_1011)
{
	//TODO: ifb
	dec_fallback(op);
	dec_DynamicSet(reg_nextpc);
	dec_End(0xFFFFFFFF,BET_DynamicJump,false);
}

//ldc.l @<REG_N>+,SR
/*
sh4dec(i0100_nnnn_0000_0111)
{
	u32 sr_t;
	ReadMemU32(sr_t,r[n]);
	if (sh4_exept_raised)
		return;
	sr.SetFull(sr_t);
	r[n] += 4;
	if (UpdateSR())
	{
		//FIXME only if interrupts got on .. :P
		UpdateINTC();
	}
	dec_End(0xFFFFFFFF,BET_StaticIntr,false);
}
*/

//ldc <REG_N>,SR
sh4dec(i0100_nnnn_0000_1110)
{
	u32 n = GetN(op);

	dec_write_sr((Sh4RegType)(reg_r0+n));
	Emit(shop_sync_sr);
	dec_End(0xFFFFFFFF,BET_StaticIntr,false);
}

//Guarded FPSCR write, shared by the two lds-to-FPSCR forms below.
//
//Why this isn't just a mov: state.cpu.FPR64/FSZ64 (the PR and SZ bits) are
//captured ONCE per block, at state_Setup(), and they decide how every FP
//opcode after this point in the block gets decoded (single vs double
//precision, register pairing). A runtime value written here could disagree
//with decisions already baked into the rest of the block.
//
//This fork's answer to that, since 2015, was to end the block after every
//FPSCR write (the OPCODE_SETFPSCR check further down, in the fallback
//path). That is always correct but always paid: measured on kofnw, 11.9M
//FPSCR writes and PR/SZ changed in ZERO of them, so ~9.6k block ends per
//frame bought nothing. So instead of ending the block unconditionally, emit
//a runtime guard -- the block only bails out to a fresh compile on the cold
//path where PR/SZ really did change. Same shape as redream's ir_assert_eq
//guard; see docs/fpscr_native_translation_plan.md for the measurements.
//
//rs1 = the PR|SZ value this block was compiled for, rs2 = where to resume
//if the guard fails. In a delay slot we can't bail out mid-instruction, so
//fall back to the old unconditional block end there (it's rare, and the
//branch that owns the delay slot ends the block anyway).
static void dec_write_fpscr()
{
	if (state.cpu.is_delayslot)
	{
		// The branch that owns this delay slot ends the block itself -- we
		// must not end it here, and we can't bail out mid-instruction either.
		Emit(shop_sync_fpscr);
		return;
	}
	if (!state.ngen.FpscrGuard)
	{
		// Backend without the guard: keep the old unconditional block end.
		Emit(shop_sync_fpscr);
		dec_End(state.cpu.rpc+2,BET_StaticJump,false);
		return;
	}
	u32 expected_pr_sz = (state.cpu.FSZ64 ? (1 << 20) : 0) | (state.cpu.FPR64 ? (1 << 19) : 0);
	Emit(shop_sync_fpscr, shil_param(), mk_imm(expected_pr_sz), mk_imm(state.cpu.rpc + 2));
}

//lds <REG_N>,FPSCR
sh4dec(i0100_nnnn_0110_1010)
{
	u32 n = GetN(op);

	Emit(shop_mov32,reg_fpscr,(Sh4RegType)(reg_r0+n));
	dec_write_fpscr();
}

//lds.l @<REG_N>+,FPSCR
//Same as above, but the value comes from memory (post-increment) instead
//of a GPR -- mirrors dec_LDM(PRM_SREG)'s memory-read shape (used by the
//sibling MACH/MACL/PR/FPUL cases).
sh4dec(i0100_nnnn_0110_0110)
{
	u32 n = GetN(op);
	Sh4RegType rn=(Sh4RegType)(reg_r0+n);

	state.info.has_readm=true;
	Emit(shop_readm,reg_fpscr,rn,shil_param(),4);
	Emit(shop_add,rn,rn,mk_imm(4));
	dec_write_fpscr();
}

//nop !
sh4dec(i0000_0000_0000_1001)
{
}

//fschg
sh4dec(i1111_0011_1111_1101)
{
	//fpscr.SZ is bit 20
	Emit(shop_xor,reg_fpscr,reg_fpscr,mk_imm(1<<20));
	state.cpu.FSZ64=!state.cpu.FSZ64;
}

//frchg
sh4dec(i1111_1011_1111_1101)
{
	Emit(shop_xor,reg_fpscr,reg_fpscr,mk_imm(1<<21));
	Emit(shop_mov32,reg_old_fpscr,reg_fpscr);
	shil_param rmn;//null param
	Emit(shop_frswap,regv_xmtrx,regv_fmtrx,regv_xmtrx,0,rmn,regv_fmtrx);
}

//not-so-elegant, but avoids extra opcodes and temporalities ..
//rotcl
sh4dec(i0100_nnnn_0010_0100)
{
	u32 n = GetN(op);
	Sh4RegType rn=(Sh4RegType)(reg_r0+n);
	
	Emit(shop_rocl,rn,rn,reg_sr_T,0,shil_param(),reg_sr_T);
	/*
	Emit(shop_ror,rn,rn,mk_imm(31));
	Emit(shop_xor,rn,rn,reg_sr_T);              //Only affects last bit (swap part a)
	Emit(shop_xor,reg_sr_T,reg_sr_T,rn);        //srT -> rn
	Emit(shop_and,reg_sr_T,reg_sr_T,mk_imm(1)); //Keep only last bit
	Emit(shop_xor,rn,rn,reg_sr_T);              //Only affects last bit (swap part b)
	*/
}

//rotcr
sh4dec(i0100_nnnn_0010_0101)
{
	u32 n = GetN(op);
	Sh4RegType rn=(Sh4RegType)(reg_r0+n);

	Emit(shop_rocr,rn,rn,reg_sr_T,0,shil_param(),reg_sr_T);
	/*
	Emit(shop_xor,rn,rn,reg_sr_T);              //Only affects last bit (swap part a)
	Emit(shop_xor,reg_sr_T,reg_sr_T,rn);        //srT -> rn
	Emit(shop_and,reg_sr_T,reg_sr_T,mk_imm(1)); //Keep only last bit
	Emit(shop_xor,rn,rn,reg_sr_T);              //Only affects last bit (swap part b)

	Emit(shop_ror,rn,rn,mk_imm(1));
	*/
}

static const Sh4RegType SREGS[] =
{
	reg_mach,
	reg_macl,
	reg_pr,
	reg_sgr,
	NoReg,
	reg_fpul,
	reg_fpscr,
	NoReg,

	NoReg,
	NoReg,
	NoReg,
	NoReg,
	NoReg,
	NoReg,
	NoReg,
	reg_dbr,
};

static const Sh4RegType CREGS[] =
{
	reg_sr,
	reg_gbr,
	reg_vbr,
	reg_ssr,
	reg_spc,
	NoReg,
	NoReg,
	NoReg,

	reg_r0_Bank,
	reg_r1_Bank,
	reg_r2_Bank,
	reg_r3_Bank,
	reg_r4_Bank,
	reg_r5_Bank,
	reg_r6_Bank,
	reg_r7_Bank,
};

static void dec_param(DecParam p,shil_param& r1,shil_param& r2, u32 op)
{
   switch(p)
   {
      //constants
      case PRM_PC_D8_x2:
         r1=mk_imm((state.cpu.rpc+4)+(GetImm8(op)<<1));
         break;

      case PRM_PC_D8_x4:
         r1=mk_imm(((state.cpu.rpc+4)&0xFFFFFFFC)+(GetImm8(op)<<2));
         break;

      case PRM_ZERO:
         r1= mk_imm(0);
         break;

      case PRM_ONE:
         r1= mk_imm(1);
         break;

      case PRM_TWO:
         r1= mk_imm(2);
         break;

      case PRM_TWO_INV:
         r1= mk_imm(~2);
         break;

      case PRM_ONE_F32:
         r1= mk_imm(0x3f800000);
         break;

         //imms
      case PRM_SIMM8:
         r1=mk_imm(GetSImm8(op));
         break;
      case PRM_UIMM8:
         r1=mk_imm(GetImm8(op));
         break;

         //direct registers
      case PRM_R0:
         r1=mk_reg(reg_r0);
         break;

      case PRM_RN:
         r1=mk_regi(reg_r0+GetN(op));
         break;

      case PRM_RM:
         r1=mk_regi(reg_r0+GetM(op));
         break;

      case PRM_FRN_SZ:
         if (state.cpu.FSZ64)
         {
            int rx=GetN(op)/2;
            if (GetN(op)&1)
               rx+=regv_xd_0;
            else
               rx+=regv_dr_0;

            r1=mk_regi(rx);
            break;
         }
      case PRM_FRN:
         r1=mk_regi(reg_fr_0+GetN(op));
         break;

      case PRM_FRM_SZ:
         if (state.cpu.FSZ64)
         {
            int rx=GetM(op)/2;
            if (GetM(op)&1)
               rx+=regv_xd_0;
            else
               rx+=regv_dr_0;

            r1=mk_regi(rx);
            break;
         }
      case PRM_FRM:
         r1=mk_regi(reg_fr_0+GetM(op));
         break;

      case PRM_FPUL:
         r1=mk_regi(reg_fpul);
         break;

      case PRM_FPN:	//float pair, 3 bits
         r1=mk_regi(regv_dr_0+GetN(op)/2);
         break;

      case PRM_FVN:	//float quad, 2 bits
         r1=mk_regi(regv_fv_0+GetN(op)/4);
         break;

      case PRM_FVM:	//float quad, 2 bits
         r1=mk_regi(regv_fv_0+(GetN(op)&0x3));
         break;

      case PRM_XMTRX:	//float matrix, 0 bits
         r1=mk_regi(regv_xmtrx);
         break;

      case PRM_FRM_FR0:
         r1=mk_regi(reg_fr_0+GetM(op));
         r2=mk_regi(reg_fr_0);
         break;

      case PRM_SR_T:
         r1=mk_regi(reg_sr_T);
         break;

      case PRM_SR_STATUS:
         r1=mk_regi(reg_sr_status);
         break;

      case PRM_SREG:	//FPUL/FPSCR/MACH/MACL/PR/DBR/SGR
         r1=mk_regi(SREGS[GetM(op)]);
         break;
      case PRM_CREG:	//SR/GBR/VBR/SSR/SPC/<RM_BANK>
         r1=mk_regi(CREGS[GetM(op)]);
         break;

         //reg/imm reg/reg
      case PRM_RN_D4_x1:
      case PRM_RN_D4_x2:
      case PRM_RN_D4_x4:
         {
            u32 shft=p-PRM_RN_D4_x1;
            r1=mk_regi(reg_r0+GetN(op));
            r2=mk_imm(GetImm4(op)<<shft);
         }
         break;

      case PRM_RN_R0:
         r1=mk_regi(reg_r0+GetN(op));
         r2=mk_regi(reg_r0);
         break;

      case PRM_RM_D4_x1:
      case PRM_RM_D4_x2:
      case PRM_RM_D4_x4:
         {
            u32 shft=p-PRM_RM_D4_x1;
            r1=mk_regi(reg_r0+GetM(op));
            r2=mk_imm(GetImm4(op)<<shft);
         }
         break;

      case PRM_RM_R0:
         r1=mk_regi(reg_r0+GetM(op));
         r2=mk_regi(reg_r0);
         break;

      case PRM_GBR_D8_x1:
      case PRM_GBR_D8_x2:
      case PRM_GBR_D8_x4:
         {
            u32 shft=p-PRM_GBR_D8_x1;
            r1=mk_regi(reg_gbr);
            r2=mk_imm(GetImm8(op)<<shft);
         }
         break;

      default:
         die("Non-supported parameter used");
   }
}

#define MASK_N_M 0xF00F
#define MASK_N   0xF0FF
#define MASK_NONE   0xFFFF

#define DIV0U_KEY 0x0019
#define DIV0S_KEY 0x2007
#define DIV1_KEY 0x3004
#define ROTCL_KEY 0x4024

static Sh4RegType div_som_reg1;
static Sh4RegType div_som_reg2;
static Sh4RegType div_som_reg3;

static u32 MatchDiv32(u32 pc , Sh4RegType &reg1,Sh4RegType &reg2 , Sh4RegType &reg3)
{
   u32 v_pc=pc;
   u32 match=1;
   for (int i=0;i<32;i++)
   {
		u16 opcode=IReadMem16(v_pc);
      v_pc+=2;
      if ((opcode&MASK_N)==ROTCL_KEY)
      {
         if (reg1==NoReg)
            reg1=(Sh4RegType)GetN(opcode);
         else if (reg1!=(Sh4RegType)GetN(opcode))
            break;
         match++;
      }
      else
      {
         //printf("DIV MATCH BROKEN BY: %s\n",OpDesc[opcode]->diss);
         break;
      }

		opcode=IReadMem16(v_pc);
      v_pc+=2;
      if ((opcode&MASK_N_M)==DIV1_KEY)
      {
         if (reg2==NoReg)
            reg2=(Sh4RegType)GetM(opcode);
         else if (reg2!=(Sh4RegType)GetM(opcode))
            break;

         if (reg2==reg1)
            break;

         if (reg3==NoReg)
            reg3=(Sh4RegType)GetN(opcode);
         else if (reg3!=(Sh4RegType)GetN(opcode))
            break;

         if (reg3==reg1)
            break;

         match++;
      }
      else
         break;
   }

   return match;
}

static bool MatchDiv32u(u32 op,u32 pc)
{
   if (settings.dynarec.DisableDivMatching)
		return false;

	div_som_reg1=NoReg;
	div_som_reg2=NoReg;
	div_som_reg3=NoReg;

	u32 match=MatchDiv32(pc+2,div_som_reg1,div_som_reg2,div_som_reg3);

	//log("DIV32U matched %d%% @ 0x%X\n",match*100/65,pc);
	if (match==65) /* DIV32U was perfectly matched */
		return true;

   return false;
}

static bool MatchDiv32s(u32 op,u32 pc)
{
   if (settings.dynarec.DisableDivMatching)
		return false;

   u32 n = GetN(op);
   u32 m = GetM(op);

   div_som_reg1=NoReg;
   div_som_reg2=(Sh4RegType)m;
   div_som_reg3=(Sh4RegType)n;

   u32 match=MatchDiv32(pc+2,div_som_reg1,div_som_reg2,div_som_reg3);
#ifndef NDEBUG
   printf("DIV32S matched %d%% @ 0x%X\n",match*100/65,pc);
#endif

   if (match==65) /* DIV32S was perfectly matched */
   {
#ifndef NDEBUG
      printf("div32s %d/%d/%d\n",div_som_reg1,div_som_reg2,div_som_reg3);
#endif
      return true;
   }

   return false;
}

static bool dec_generic(u32 op)
{
	DecMode mode;DecParam d;DecParam s;shilop natop;u32 e;
	if (OpDesc[op]->decode==0)
		return false;
	
	u64 inf=OpDesc[op]->decode;

	e=(u32)(inf>>32);
	mode=(DecMode)((inf>>24)&0xFF);
	d=(DecParam)((inf>>16)&0xFF);
	s=(DecParam)((inf>>8)&0xFF);
	natop=(shilop)((inf>>0)&0xFF);

	bool transfer_64=false;
	if (op>=0xF000)
	{
		state.info.has_fpu=true;
		//return false;//FPU off for now
		if (state.cpu.FPR64 /*|| state.cpu.FSZ64*/)
			return false;

		if (state.cpu.FSZ64 && (d==PRM_FRN_SZ || d==PRM_FRM_SZ || s==PRM_FRN_SZ || s==PRM_FRM_SZ))
			transfer_64=true;
	}

	shil_param rs1,rs2,rs3,rd;

	dec_param(s,rs2,rs3,op);
	dec_param(d,rs1,rs3,op);

	switch(mode)
	{
	case DM_ReadSRF:
		Emit(shop_mov32,rs1,reg_sr_status);
		Emit(shop_or,rs1,rs1,reg_sr_T);
		break;

	case DM_WriteTOp:
		Emit(natop,reg_sr_T,rs1,rs2);
		break;

	case DM_DT:
		verify(natop==shop_sub);
		Emit(natop,rs1,rs1,rs2);
		Emit(shop_seteq,mk_reg(reg_sr_T),rs1,mk_imm(0));
		break;

	case DM_Shift:
		if (natop==shop_shl && e==1)
			Emit(shop_shr,mk_reg(reg_sr_T),rs1,mk_imm(31));
		else if (e==1)
			Emit(shop_and,mk_reg(reg_sr_T),rs1,mk_imm(1));

		Emit(natop,rs1,rs1,mk_imm(e));
		break;

	case DM_Rot:
		if (!(((s32)e>=0?e:-e)&0x1000))
		{
			if ((s32)e<0)
			{
				//left rotate
				Emit(shop_shr,mk_reg(reg_sr_T),rs2,mk_imm(31));
				e=-e;
			}
			else
			{
				//right rotate
				Emit(shop_and,mk_reg(reg_sr_T),rs2,mk_imm(1));
			}
		}
		e&=31;

		Emit(natop,rs1,rs2,mk_imm(e));
		break;

	case DM_BinaryOp://d=d op s
		if (e&1)
			Emit(natop,rs1,rs1,rs2,0,rs3);
		else
			Emit(natop,shil_param(),rs1,rs2,0,rs3);
		break;

	case DM_UnaryOp: //d= op s
		if (transfer_64 && natop==shop_mov32) 
			natop=shop_mov64;

		if (natop==shop_cvt_i2f_n && state.cpu.RoundToZero)
			natop=shop_cvt_i2f_z;

		if (e&1)
			Emit(natop,shil_param(),rs1);
		else
			Emit(natop,rs1,rs2);
		break;

	case DM_WriteMSRF:
		// stc.l SR,@-Rn. SR is kept split in this fork (sr.status + sr.T, see
		// sh4_sr_GetFull()), which is the only reason this opcode had no native
		// path while every sibling (GBR/VBR/SSR/SPC/DBR/SGR) uses dec_STM --
		// it was the top interpreter fallback left in the 2D fighters (~2.4k
		// calls/frame in mbaa, ~1.9k in kofnw). Rebuild the full value into the
		// scratch reg with the same two ops DM_ReadSRF already uses for the
		// non-.l `stc SR,Rn`, then fall through so the pre-decrement store --
		// including its MMU / exception-fixup handling -- stays shared with
		// every other stc.l instead of being duplicated here.
		Emit(shop_mov32,mk_reg(reg_temp),mk_reg(reg_sr_status));
		Emit(shop_or,mk_reg(reg_temp),mk_reg(reg_temp),mk_reg(reg_sr_T));
		rs2=mk_reg(reg_temp);
		// fall through

	case DM_WriteM: //write(d,s)
		{
			//0 has no effect, so get rid of it
			if (rs3.is_imm() && rs3._imm==0)
				rs3=shil_param();

			state.info.has_writem=true;
			if (transfer_64) e=(s32)e*2;
			bool update_after=false;
			if ((s32)e<0)
			{
				if (rs1._reg!=rs2._reg && !mmu_enabled()) //reg shouldn't be updated if its written
				{
					Emit(shop_sub,rs1,rs1,mk_imm(-e));
				}
				else
				{
					verify(rs3.is_null());
					rs3=mk_imm(e);
					update_after=true;
				}
			}

			Emit(shop_writem,shil_param(),rs1,rs2,(s32)e<0?-e:e,rs3);

			if (update_after)
			{
				Emit(shop_sub,rs1,rs1,mk_imm(-e));
			}
		}
		break;

	case DM_ReadM:
		//0 has no effect, so get rid of it
		if (rs3.is_imm() && rs3._imm==0)
				rs3=shil_param();

		state.info.has_readm=true;
		if (transfer_64) e=(s32)e*2;

		Emit(shop_readm,rs1,rs2,shil_param(),(s32)e<0?-e:e,rs3);
		if ((s32)e<0)
		{
			if (rs1._reg!=rs2._reg)//the reg shouldn't be updated if it was just read.
				Emit(shop_add,rs2,rs2,mk_imm(-e));
		}
		break;

	case DM_fiprOp:
		{
			shil_param rdd=mk_regi(rs1._reg+3);
			Emit(natop,rdd,rs1,rs2);
		}
		break;

	case DM_EXTOP:
		{
			Emit(natop,rs1,rs2,mk_imm(e==1?0xFF:0xFFFF));
		}
		break;
	
	case DM_MUL:
		{
			shilop op;
			shil_param rd=mk_reg(reg_macl);
			shil_param rd2=shil_param();

			switch((s32)e)
			{
				case 16:  op=shop_mul_u16; break;
				case -16: op=shop_mul_s16; break;

				case -32: op=shop_mul_i32; break;

				case 64:  op=shop_mul_u64; rd2 = mk_reg(reg_mach); break;
				case -64: op=shop_mul_s64; rd2 = mk_reg(reg_mach); break;

				default:
					die("DM_MUL: Failed to classify opcode");
               return false;
			}

			Emit(op,rd,rs1,rs2,0,shil_param(),rd2);
		}
		break;

	case DM_DIV0:
		{
			if (e==1)
			{
				if (MatchDiv32u(op,state.cpu.rpc))
				{
					verify(!state.cpu.is_delayslot);
					//div32u
					Emit(shop_div32u, mk_reg(div_som_reg1), mk_reg(div_som_reg1), mk_reg(div_som_reg2), 0, mk_reg(div_som_reg3), mk_reg(div_som_reg3));
					
					Emit(shop_and, mk_reg(reg_sr_T), mk_reg(div_som_reg1), mk_imm(1));
					Emit(shop_shr, mk_reg(div_som_reg1), mk_reg(div_som_reg1), mk_imm(1));

					Emit(shop_div32p2, mk_reg(div_som_reg3), mk_reg(div_som_reg3), mk_reg(div_som_reg2), 0, mk_reg(reg_sr_T));
					
					//skip the aggregated opcodes
					state.cpu.rpc += 128;
					blk->guest_cycles += CPU_RATIO*64;
				}
				else
				{
					//clear QM (bits 8,9)
					u32 qm=(1<<8)|(1<<9);
					Emit(shop_and,mk_reg(reg_sr_status),mk_reg(reg_sr_status),mk_imm(~qm));
					//clear T !
					Emit(shop_mov32,mk_reg(reg_sr_T),mk_imm(0));
				}
			}
			else
			{
				if (MatchDiv32s(op,state.cpu.rpc))
				{
					verify(!state.cpu.is_delayslot);
					//div32s
					Emit(shop_xor, mk_reg(reg_sr_T), mk_reg(div_som_reg3), mk_reg(div_som_reg2));	// get quotient sign
					Emit(shop_and, mk_reg(reg_sr_T), mk_reg(reg_sr_T), mk_imm(1 << 31));			// isolate sign bit

					Emit(shop_div32s, mk_reg(div_som_reg1), mk_reg(div_som_reg1), mk_reg(div_som_reg2), 0, mk_reg(div_som_reg3), mk_reg(div_som_reg3));
					
					Emit(shop_and, mk_reg(reg_temp), mk_reg(div_som_reg1), mk_imm(1));				// set quotient lsb in temp reg
					Emit(shop_sar, mk_reg(div_som_reg1), mk_reg(div_som_reg1), mk_imm(1));				// shift quotient right
					Emit(shop_or, mk_reg(reg_sr_T), mk_reg(reg_sr_T), mk_reg(reg_temp));			// store quotient lsb in T

					Emit(shop_div32p2, mk_reg(div_som_reg3), mk_reg(div_som_reg3), mk_reg(div_som_reg2), 0, mk_reg(reg_sr_T));

					Emit(shop_and, mk_reg(reg_sr_T), mk_reg(reg_sr_T), mk_imm(1));							// clean up T
					
					//skip the aggregated opcodes
					state.cpu.rpc += 128;
					blk->guest_cycles += CPU_RATIO * 64;
				}
				else
				{
					//Clear Q & M
					Emit(shop_and,mk_reg(reg_sr_status),mk_reg(reg_sr_status),mk_imm(~((1<<8)|(1<<9))));

					//sr.Q=r[n]>>31;
					Emit(shop_sar,mk_reg(reg_sr_T),rs1,mk_imm(31));
					Emit(shop_and,mk_reg(reg_sr_T),mk_reg(reg_sr_T),mk_imm(1<<8));
					Emit(shop_or,mk_reg(reg_sr_status),mk_reg(reg_sr_status),mk_reg(reg_sr_T));

					//sr.M=r[m]>>31;
					Emit(shop_sar,mk_reg(reg_sr_T),rs2,mk_imm(31));
					Emit(shop_and,mk_reg(reg_sr_T),mk_reg(reg_sr_T),mk_imm(1<<9));
					Emit(shop_or,mk_reg(reg_sr_status),mk_reg(reg_sr_status),mk_reg(reg_sr_T));

					//sr.T=sr.M^sr.Q;
					Emit(shop_xor,mk_reg(reg_sr_T),rs1,rs2);
					Emit(shop_shr,mk_reg(reg_sr_T),mk_reg(reg_sr_T),mk_imm(31));
				}
			}
		}
		break;

	case DM_ADC:
		{
			Emit(natop,rs1,rs1,rs2,0,mk_reg(reg_sr_T),mk_reg(reg_sr_T));
		}
		break;

	case DM_NEGC:
		Emit(natop, rs1, rs2, mk_reg(reg_sr_T), 0, shil_param(), mk_reg(reg_sr_T));
		break;

	default:
		verify(false);
	}

	return true;
}

static void state_Setup(u32 rpc,fpscr_t fpu_cfg)
{
	state.cpu.rpc=rpc;
	state.cpu.is_delayslot=false;
	state.cpu.FPR64=fpu_cfg.PR;
	state.cpu.FSZ64=fpu_cfg.SZ;
	state.cpu.RoundToZero=fpu_cfg.RM==1;
	//verify(fpu_cfg.RM<2);	// Happens with many wince games (set to 3)
   //what about fp/fs ?

   state.NextOp=NDO_NextOp;
   state.BlockType=BET_SCL_Intr;
   state.JumpAddr=0xFFFFFFFF;
   state.NextAddr=0xFFFFFFFF;

   state.info.has_readm=false;
   state.info.has_writem=false;
   state.info.has_fpu=false;
}

bool dec_DecodeBlock(RuntimeBlockInfo* rbi,u32 max_cycles)
{
	blk=rbi;
	state_Setup(blk->vaddr, blk->fpu_cfg);
	ngen_GetFeatures(&state.ngen);
	
	blk->guest_opcodes=0;
	// If full MMU, don't allow the block to extend past the end of the current 4K page
	u32 max_pc = mmu_enabled() ? ((state.cpu.rpc >> 12) + 1) << 12 : 0xFFFFFFFF;
	
	for(;;)
	{
		switch(state.NextOp)
		{
		case NDO_Delayslot:
			state.NextOp=state.DelayOp;
			state.cpu.is_delayslot=true;
			//there is no break here by design
		case NDO_NextOp:
			{
				if ((blk->oplist.size() >= BLOCK_MAX_SH_OPS_SOFT || blk->guest_cycles >= max_cycles || state.cpu.rpc >= max_pc)
						&& !state.cpu.is_delayslot)
				{
					dec_End(state.cpu.rpc,BET_StaticJump,false);
				}
				else
				{
               u32 op = IReadMem16(state.cpu.rpc);

					blk->guest_opcodes++;
					if (!mmu_enabled())
					{
						if (op>=0xF000)
							blk->guest_cycles+=0;
						else
							blk->guest_cycles+=CPU_RATIO;
					}
					else
					{
						blk->guest_cycles += std::max((int)OpDesc[op]->LatencyCycles, 1);
					}
					if (OpDesc[op]->IsFloatingPoint())
					{
						if (sr.FD == 1)
						{
							// We need to know FPSCR to compile the block, so let the exception handler run first
							// as it may change the fp registers
							Do_Exception(next_pc, 0x800, 0x100);
							return false;
						}
						blk->has_fpu_op = true;
					}

					verify(!(state.cpu.is_delayslot && OPCODE_SETPC(OpDesc[op]->type)));
					if (state.ngen.OnlyDynamicEnds || !OpDesc[op]->rec_oph)
					{
						if (state.ngen.InterpreterFallback || !dec_generic(op))
						{
							dec_fallback(op);
							if (OPCODE_SETPC(OpDesc[op]->type))
							{
								dec_DynamicSet(reg_nextpc);
								dec_End(0xFFFFFFFF,BET_DynamicJump,false);
							}
							if (OPCODE_SETFPSCR(OpDesc[op]->type) && !state.cpu.is_delayslot)
							{
								dec_End(state.cpu.rpc+2,BET_StaticJump,false);
							}
						}
					}
					else
					{
						OpDesc[op]->rec_oph(op);
					}
					state.cpu.rpc+=2;
				}
			}
			break;

		case NDO_Jump:
			die("Too old");
			state.NextOp=state.JumpOp;
			state.cpu.rpc=state.JumpAddr;
			break;

		case NDO_End:
			goto _end;
		}
	}

_end:
	blk->sh4_code_size=state.cpu.rpc-blk->vaddr;
	blk->NextBlock=state.NextAddr;
	blk->BranchBlock=state.JumpAddr;
	blk->BlockType=state.BlockType;

	verify(blk->oplist.size() <= BLOCK_MAX_SH_OPS_HARD);
	
#if 0
#ifdef _WIN32
   switch(rbi->addr)
   {
      case 0x8C09ED16:
      case 0x8C0BA50E:
      case 0x8C0BA506:
      case 0x8C0BA526:
      case 0x8C224800:
		INFO_LOG(DYNAREC, "HASH: %08X reloc %s",blk->addr,blk->hash());
         break;
   }
#endif
#endif

	//cycle tricks
	if (settings.dynarec.idleskip)
	{
		if (!mmu_enabled() && idle_ff_match(blk->addr))
			blk->idle_fastforward = true;
		if (!mmu_enabled() && delay_loop_match(blk->addr))
			blk->delay_skip = true;

		// FC_IDLE_LOG: registra cada bloco que um truque de ciclos marcou
		// (docs/tech_debits.md 4.38) -- o hash le so metade do bloco, em bytes.
		static const bool idleLog = getenv("FC_IDLE_LOG") != nullptr;
		//Experimental hash-id based idle skip
		if (!mmu_enabled() && strstr(idle_hash, blk->hash()))
		{
			//printf("IDLESKIP: %08X reloc match %s\n",blk->addr,blk->hash());
			if (idleLog)
				fprintf(stderr, "IDLETRICK hash %08X ops=%u %s\n", blk->vaddr, blk->guest_opcodes, blk->hash());
			blk->guest_cycles=max_cycles*100;
		}
		else
		{
			//Small-n-simple idle loop detector :p
			//
			// The two bounds are tunable (FC_IDLE_OPS / FC_IDLE_MUL) because the
			// defaults miss real spin loops by a hair. Measured case, Metal Slug 6:
			// the block that polls the wait flag (8C05B3A4) is read-only, has no
			// FPU, and is BET_Cond_0 -- it matches every condition here except it
			// has exactly 6 guest opcodes, so `<6` rejects it by one. That loop
			// alone (4 blocks, 187M iterations in 30s) is 74.6% of all host
			// instructions the JIT executes, and it's pure waiting: the whole loop
			// only reads, so it can't change its own exit condition -- that can
			// only come from a scheduled event. Charging more emulated cycles per
			// iteration makes the scheduler reach that event in fewer real
			// iterations. See docs/tech_debits.md and the block profiler
			// (FC_BLOCK_PROF) that found it.
			static int idleOps = -1, idleMul = -1;
			if (idleOps == -1)
			{
				const char *e = getenv("FC_IDLE_OPS");
				idleOps = e != nullptr ? atoi(e) : 6;
				e = getenv("FC_IDLE_MUL");
				idleMul = e != nullptr ? atoi(e) : 3;
			}
			if (state.info.has_readm && !state.info.has_writem && !state.info.has_fpu && (int)blk->guest_opcodes<idleOps)
			{
				if (blk->BlockType==BET_Cond_0 || (blk->BlockType==BET_Cond_1 && blk->BranchBlock<=blk->vaddr))
				{
					blk->guest_cycles*=idleMul;
					if (idleLog)
						fprintf(stderr, "IDLETRICK small %08X ops=%u x%d\n", blk->vaddr, blk->guest_opcodes, idleMul);
				}

				if (blk->BranchBlock==blk->vaddr)
				{
					blk->guest_cycles*=10;
					if (idleLog)
						fprintf(stderr, "IDLETRICK self %08X ops=%u x10\n", blk->vaddr, blk->guest_opcodes);
				}
			}

			//if in syscalls area (ip.bin etc) skip fast :p
			if ((blk->addr&0x1FFF0000)==0x0C000000)
			{
				if (blk->addr&0x8000)
				{
					//ip.bin (boot loader/img etc)
					blk->guest_cycles*=15;
				}
				else
				{
					//syscalls
					blk->guest_cycles*=5;
				}
			}

			//blk->guest_cycles=5;
		}
	}
	else
	{
		blk->guest_cycles*=1.5;
	}
	// Boost dynarec always
	/*if (mmu_enabled())*/
	blk->guest_cycles = blk->guest_cycles * settings.dreamcast.sh4clock;

	//make sure we don't use wayy-too-many cycles
	blk->guest_cycles = std::min(blk->guest_cycles,max_cycles);
	//make sure we don't use wayy-too-few cycles
	blk->guest_cycles = std::max(1U,blk->guest_cycles);
	blk=0;

	return true;
}

#endif
