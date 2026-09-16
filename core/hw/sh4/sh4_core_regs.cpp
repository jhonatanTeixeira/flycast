/*
	Sh4 register storage/functions/utilities
*/

#include "types.h"
#include "sh4_core.h"
#include "sh4_interrupts.h"
#include <unistd.h>	// getpid(), for the opt-in FPSCR stats dump below


Sh4RCB* p_sh4rcb;
sh4_if  sh4_cpu;
u8* sh4_dyna_rcb;

static INLINE void ChangeGPR()
{
#ifdef HAVE_MODERN_CXX
   std::swap((u32 (&)[8])r, r_bank);
#else
	u32 temp;
	for (int i=0;i<8;i++)
	{
		temp      = r[i];
		r[i]      = r_bank[i];
		r_bank[i] = temp;
	}
#endif
}

static INLINE void ChangeFP()
{
#ifdef HAVE_MODERN_CXX
   std::swap((f32 (&)[16])Sh4cntx.xffr, *(f32 (*)[16])&Sh4cntx.xffr[16]);
#else
	u32 temp;
	for (int i=0;i<16;i++)
	{
		temp=fr_hex[i];
		fr_hex[i]=xf_hex[i];
		xf_hex[i]=temp;
	}
#endif
}

//called when sr is changed and we must check for reg banks etc.
//returns true if interrupt pending
bool UpdateSR()
{
	if (sr.MD)
	{
		if (old_sr.RB != sr.RB)
			ChangeGPR();//bank change
	}
	else
	{
		if (old_sr.RB)
			ChangeGPR();//switch
	}

	old_sr.status=sr.status;
	old_sr.RB &= sr.MD;

	return SRdecode();
}

//make host and sh4 rounding and denormal modes match
static u32 old_rm = 0xFF;
static u32 old_dn = 0xFF;

static void setHostRoundingMode()
{
	if ((old_rm!=fpscr.RM) || (old_dn!=fpscr.DN))
	{
		old_rm=fpscr.RM ;
		old_dn=fpscr.DN ;
        
        //Correct rounding is required by some games (SOTB, etc)
#ifdef _MSC_VER
        if (fpscr.RM == 1)  //if round to 0 , set the flag
            _controlfp(_RC_CHOP, _MCW_RC);
        else
            _controlfp(_RC_NEAR, _MCW_RC);
        
        if (fpscr.DN)     //denormals are considered 0
            _controlfp(_DN_FLUSH, _MCW_DN);
        else
            _controlfp(_DN_SAVE, _MCW_DN);
#else

#if HOST_CPU==CPU_X86 || HOST_CPU==CPU_X64

        u32 temp=0x1f80;	//no flush to zero && round to nearest

        if (fpscr.RM==1)  //if round to 0 , set the flag
           temp|=(3<<13);

        if (fpscr.DN)     //denormals are considered 0
           temp|=(1<<15);
        asm("ldmxcsr %0" : : "m"(temp));
#elif HOST_CPU==CPU_ARM
        static const unsigned int x = 0x04086060;
        unsigned int y = 0x02000000;
        if (fpscr.RM==1)  //if round to 0 , set the flag
           y|=3<<22;

        if (fpscr.DN)
           y|=1<<24;


        int raa;

        asm volatile
           (
            "fmrx   %0, fpscr   \n\t"
            "and    %0, %0, %1  \n\t"
            "orr    %0, %0, %2  \n\t"
            "fmxr   fpscr, %0   \n\t"
            : "=r"(raa)
            : "r"(x), "r"(y)
           );
#elif HOST_CPU == CPU_ARM64
		static const unsigned long off_mask = 0x04080000;
        unsigned long on_mask = 0x02000000;    // DN=1 Any operation involving one or more NaNs returns the Default NaN

        if (fpscr.RM == 1)		// if round to 0, set the flag
        	on_mask |= 3 << 22;

        if (fpscr.DN)
        	on_mask |= 1 << 24;	// flush denormalized numbers to zero

        asm volatile
            (
                "MRS    x10, FPCR     \n\t"
                "AND    x10, x10, %0  \n\t"
                "ORR    x10, x10, %1  \n\t"
                "MSR    FPCR, x10     \n\t"
                :
                : "r"(off_mask), "r"(on_mask)
            );
#else
	#error "SetFloatStatusReg: Unsupported platform"
#endif
#endif

	}
}

// Opt-in counters (FC_FPSCR_STATS in the environment), same pattern as
// FC_IFB_COUNT in rec_arm64.cpp. Answers the question the FPSCR JIT work
// needs before it can be finished: of all the FPSCR writes a game does,
// how many actually change anything? A write that leaves every meaningful
// bit alone pays a full JIT->C++ call plus a forced block end for nothing,
// so the no-op rate decides whether an inline guard is worth emitting, and
// the PR/SZ-unchanged rate decides whether the forced block end can be
// made conditional. See docs/fpscr_native_translation_plan.md.
u64 g_fpscrTotal;
u64 g_fpscrNoOp;        // nothing changed at all
u64 g_fpscrPrSzSame;    // PR/SZ unchanged (block end would be avoidable)
u64 g_fpscrFrChanged;   // FR changed (the expensive ChangeFP() path)
u64 g_fpscrRmDnChanged; // RM/DN changed (the expensive host FPCR write)

static bool FpscrStatsEnabled()
{
	static int enabled = -1;
	if (enabled == -1)
		enabled = getenv("FC_FPSCR_STATS") != nullptr ? 1 : 0;
	return enabled == 1;
}

//called when fpscr is changed and we must check for reg banks etc..
void UpdateFPSCR()
{
	if (FpscrStatsEnabled())
	{
		u32 diff = fpscr.full ^ old_fpscr.full;
		g_fpscrTotal++;
		if (diff == 0)
			g_fpscrNoOp++;
		if ((diff & ((1 << 20) | (1 << 19))) == 0)	// SZ is bit 20, PR is bit 19
			g_fpscrPrSzSame++;
		if (fpscr.FR != old_fpscr.FR)
			g_fpscrFrChanged++;
		if (fpscr.RM != old_fpscr.RM || fpscr.DN != old_fpscr.DN)
			g_fpscrRmDnChanged++;
	}

	if (fpscr.FR !=old_fpscr.FR)
		ChangeFP(); // FPU bank change

   old_fpscr=fpscr;
   setHostRoundingMode();
}

// Overwrites the file each time; caller decides when (libretro.cpp, gated
// by the same env var). Mirrors DumpIfbCounts()'s approach for the same
// reason: this core's clean-shutdown path is unreliable.
void DumpFpscrStats()
{
	char path[64];
	snprintf(path, sizeof(path), "/tmp/fpscr-stats-%d.txt", (int)getpid());
	FILE *f = fopen(path, "w");
	if (f == nullptr)
		return;
	fprintf(f, "total\t%llu\n", (unsigned long long)g_fpscrTotal);
	fprintf(f, "noop\t%llu\n", (unsigned long long)g_fpscrNoOp);
	fprintf(f, "prsz_same\t%llu\n", (unsigned long long)g_fpscrPrSzSame);
	fprintf(f, "fr_changed\t%llu\n", (unsigned long long)g_fpscrFrChanged);
	fprintf(f, "rmdn_changed\t%llu\n", (unsigned long long)g_fpscrRmDnChanged);
	fclose(f);
}

void RestoreHostRoundingMode()
{
	old_rm = 0xFF;
	old_dn = 0xFF;
	setHostRoundingMode();
}

static u32* Sh4_int_GetRegisterPtr(Sh4RegType reg)
{
   if ((reg>=reg_r0) && (reg<=reg_r15))
	{
		return &r[reg-reg_r0];
	}
	else if ((reg>=reg_r0_Bank) && (reg<=reg_r7_Bank))
	{
		return &r_bank[reg-reg_r0_Bank];
	}
	else if ((reg>=reg_fr_0) && (reg<=reg_fr_15))
	{
		return &fr_hex[reg-reg_fr_0];
	}
	else if ((reg>=reg_xf_0) && (reg<=reg_xf_15))
	{
		return &xf_hex[reg-reg_xf_0];
	}
	else
	{
		switch(reg)
		{
		case reg_gbr :
			return &gbr;
			break;
		case reg_vbr :
			return &vbr;
			break;

		case reg_ssr :
			return &ssr;
			break;

		case reg_spc :
			return &spc;
			break;

		case reg_sgr :
			return &sgr;
			break;

		case reg_dbr :
			return &dbr;
			break;

		case reg_mach :
			return &mac.h;
			break;

		case reg_macl :
			return &mac.l;
			break;

		case reg_pr :
			return &pr;
			break;

		case reg_fpul :
			return &fpul;
			break;


		case reg_nextpc :
			return &next_pc;
			break;

		case reg_old_sr_status :
			return &old_sr.status;
			break;

		case reg_sr_status :
			return &sr.status;
			break;

		case reg_sr_T :
			return &sr.T;
			break;

		case reg_old_fpscr :
			return &old_fpscr.full;
			break;

		case reg_fpscr :
			return &fpscr.full;
			break;

		case reg_pc_dyn:
			return &Sh4cntx.jdyn;

		case reg_temp:
			return &Sh4cntx.temp_reg;

		default:
			ERROR_LOG(SH4, "Unknown register ID %d", reg);
			die("Invalid reg");
			return 0;
			break;
		}
	}
}

u32* GetRegPtr(u32 reg)
{
	return Sh4_int_GetRegisterPtr((Sh4RegType)reg);
}
