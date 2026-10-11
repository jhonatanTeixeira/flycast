#include "aica.h"
#include "aica_if.h"
#include "aica_mem.h"
#include "sgc_if.h"
#include "hw/holly/holly_intc.h"
#include "hw/holly/sb.h"
#include "hw/sh4/sh4_sched.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include <sched.h>
#include <sys/sysinfo.h>

#define SH4_IRQ_BIT (1 << (holly_SPU_IRQ & 31))

// Dual core do som (fase 3 do docs/mmap_moe_mtp_plan.md, a la Dolphin): o ARM7 + controle
// das vozes rodam numa thread propria, um quantum (32 amostras, 0,73 ms emulados) por vez.
// No instante T_k a emu thread espera o quantum anterior, aplica na ARAM as escritas do SH4
// que ficaram na fila (com o ARM7 parado), entrega a interrupcao do SPU ao SH4 e dispara o
// quantum k. O ARM7 ve as escritas do SH4 no mesmo instante que no modo sincrono; a unica
// diferenca e a interrupcao do SPU chegar ao SH4 um quantum depois. Leituras da ARAM e
// acessos aos registradores pelo SH4 esperam o quantum em voo (aica_dual_drain): exatos.
// FC_AICA_DUAL=N liga, com N quanta de folga (abaixo).
thread_local bool t_aicaThread;	// estamos na thread do som
bool aica_dual_on();
void aica_dual_drain();

CommonData_struct* CommonData;
DSPData_struct* DSPData;
InterruptInfo* MCIEB;
InterruptInfo* MCIPD;
InterruptInfo* MCIRE;
InterruptInfo* SCIEB;
InterruptInfo* SCIPD;
InterruptInfo* SCIRE;

//Interrupts
//arm side
static u32 GetL(u32 which)
{
   if (which > 7)
      which = 7; //higher bits share bit 7

   u32 bit = 1 << which;
   u32 rv  = 0;

   if (CommonData->SCILV0 & bit)
      rv = 1;

   if (CommonData->SCILV1 & bit)
      rv |= 2;

   if (CommonData->SCILV2 & bit)
      rv |= 4;

   return rv;
}

static void update_arm_interrupts()
{
   u32 p_ints=SCIEB->full & SCIPD->full;

   u32 Lval=0;
   if (p_ints)
   {
      u32 bit_value=1;//first bit
      //scan all interrupts , lo to hi bit.I assume low bit ints have higher priority over others
      for (u32 i=0;i<11;i++)
      {
         if (p_ints & bit_value)
         {
            //for the first one , Set the L reg & exit
            Lval=GetL(i);
            break;
         }
         bit_value<<=1; //next bit
      }
   }

   libARM_InterruptChange(p_ints,Lval);
}

//sh4 side
static void UpdateSh4Ints()
{
   if (t_aicaThread)
      return;	// dual core: o nivel e calculado no fim do quantum e entregue na emu thread
   u32 p_ints = MCIEB->full & MCIPD->full;
   if (p_ints)
   {
      if ((SB_ISTEXT & SH4_IRQ_BIT) == 0)
         //if no interrupt is already pending then raise one :)
         asic_RaiseInterrupt(holly_SPU_IRQ);
   }
   else
   {
      if ((SB_ISTEXT & SH4_IRQ_BIT) != 0)
         asic_CancelInterrupt(holly_SPU_IRQ);
   }

}


AicaTimer timers[3];
int aica_schid = -1;
const int AICA_TICK = 145125;	// 44.1 KHz / 32

// FC_REND_SPLIT: tempo do ARM7 (driver de som) e do controle das vozes na emu
// thread; o render das vozes (thread de som) e medido em sgc_if.cpp.
u64 g_aicaArmTicks, g_aicaCtrlTicks;
static inline u64 aica_ticks()
{
#if defined(__aarch64__)
	u64 v;
	asm volatile("mrs %0, cntvct_el0" : "=r"(v));
	return v;
#else
	return 0;
#endif
}

// ---------------------------------------------------------------- dual core
// Folga de N quanta (FC_AICA_DUAL=N, N >= 1), como o dual core do Dolphin: a emu thread
// enfileira um quantum por fronteira T_k (com as escritas do SH4 na ARAM desde T_{k-1} e o
// CDDA lido na emu thread); a thread do som processa a fila em sequencia, sem dormir entre
// quanta. O nivel da interrupcao do SPU e calculado no fim de cada quantum e entregue ao
// SH4 em T_{k+N}: o estado e deterministico. A emu so espera se o som ficar N quanta atras.
struct AramW { u32 addr; u32 data; u32 sz; };
struct DualJob { std::vector<AramW> writes; s16 cddaL[32], cddaR[32]; bool irq; };
static const u32 DUAL_RING = 64;
static DualJob g_jobs[DUAL_RING];
static std::vector<AramW> g_aramQ;		// escritas do SH4 desde a ultima fronteira (emu thread)
static std::atomic<u64> g_jobSubmitted { 0 }, g_jobDone { 0 };
static u64 g_jobApplied;			// saidas ja entregues ao SH4 (emu thread)
static std::thread g_dualThread;
static std::mutex g_dualM;
static std::condition_variable g_dualCvJob, g_dualCvDone;
static bool g_dualQuit;
u64 g_aicaDualWaitUs, g_aicaDualDrains, g_aicaDualWaits, g_aicaDualQuanta, g_aicaDualWorkUs, g_aicaDualWakeUs;
static inline u64 dual_now_us()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (u64)ts.tv_sec * 1000000ull + ts.tv_nsec / 1000;
}
void aica_cdda_prefill(s16 *L, s16 *R);	// sgc_if.cpp
extern s16 *g_cddaPreL, *g_cddaPreR;

static u32 dual_lag()
{
	static int n = -1;
	if (n < 0)
	{
		const char *e = getenv("FC_AICA_DUAL");
		n = e != nullptr ? std::max(0, std::min(atoi(e), (int)DUAL_RING / 2)) : 0;
	}
	return (u32)n;
}

bool aica_dual_on()
{
	return dual_lag() != 0 && !settings.aica.NoBatch;
}

template<u32 sz> static inline void aram_w(u32 a, u32 d) { WriteMemArr<sz>(aica_ram.data, a, d); }
static void apply_writes(std::vector<AramW> &v)
{
	for (const AramW &w : v)
	{
		if (w.sz == 1) aram_w<1>(w.addr, w.data);
		else if (w.sz == 2) aram_w<2>(w.addr, w.data);
		else aram_w<4>(w.addr, w.data);
	}
	v.clear();
}

// nivel da interrupcao do SPU no fim de um quantum -> SH4 (so na emu thread)
static void apply_spu_irq(bool level)
{
	if (level)
	{
		if ((SB_ISTEXT & SH4_IRQ_BIT) == 0)
			asic_RaiseInterrupt(holly_SPU_IRQ);
	}
	else if ((SB_ISTEXT & SH4_IRQ_BIT) != 0)
		asic_CancelInterrupt(holly_SPU_IRQ);
}

static void aica_dual_worker()
{
	t_aicaThread = true;
	if (get_nprocs() == 4)
	{
		// FC_AICA_DUAL_CPU (padrao 2): fora do nucleo 3 (emulacao) e do 1 (worker de render
		// que mais trabalha); -1 = nucleos 0-2
		const char *e = getenv("FC_AICA_DUAL_CPU");
		const int cpu = e != nullptr ? atoi(e) : 2;
		cpu_set_t set;
		CPU_ZERO(&set);
		if (cpu >= 0 && cpu <= 3)
			CPU_SET(cpu, &set);
		else
			for (int c = 0; c <= 2; c++)
				CPU_SET(c, &set);
		sched_setaffinity(0, sizeof(set), &set);
	}
	for (;;)
	{
		{
			std::unique_lock<std::mutex> l(g_dualM);
			const u64 w = dual_now_us();
			g_dualCvJob.wait(l, [] { return g_jobDone.load(std::memory_order_relaxed) < g_jobSubmitted.load(std::memory_order_acquire) || g_dualQuit; });
			if (g_dualQuit)
				break;
			g_aicaDualWakeUs += dual_now_us() - w;
		}
		// processa tudo que ha na fila sem dormir entre os quanta
		for (;;)
		{
			const u64 d = g_jobDone.load(std::memory_order_relaxed);
			if (d >= g_jobSubmitted.load(std::memory_order_acquire))
				break;
			DualJob &j = g_jobs[d % DUAL_RING];
			const u64 w0 = dual_now_us();
			apply_writes(j.writes);
			g_cddaPreL = j.cddaL;
			g_cddaPreR = j.cddaR;
			aicaarm::run(32);
			AICA_Sample32();
			j.irq = (MCIEB->full & MCIPD->full) != 0;
			g_aicaDualWorkUs += dual_now_us() - w0;
			g_aicaDualQuanta++;
			{
				std::lock_guard<std::mutex> l(g_dualM);
				g_jobDone.store(d + 1, std::memory_order_release);
			}
			g_dualCvDone.notify_all();
		}
	}
}

// emu thread: espera ate o quantum 'target' (exclusivo) estar pronto
static void dual_wait_done(u64 target)
{
	if (g_jobDone.load(std::memory_order_acquire) >= target)
		return;
	g_aicaDualWaits++;
	const u64 t0 = dual_now_us();
	std::unique_lock<std::mutex> l(g_dualM);
	while (!g_dualCvDone.wait_for(l, std::chrono::seconds(2), [&] { return g_jobDone.load(std::memory_order_acquire) >= target; }))
		fprintf(stderr, "aica dual: espera > 2 s -- alvo %llu feitos %llu enviados %llu\n", (unsigned long long)target,
				(unsigned long long)g_jobDone.load(), (unsigned long long)g_jobSubmitted.load());
	g_aicaDualWaitUs += dual_now_us() - t0;
}

// Ponto de sincronia do SH4 (leitura da ARAM, registradores, DMA G2, savestate, hash):
// espera todos os quanta em voo, entrega as saidas e aplica a fila. Na thread do som nao
// faz nada.
void aica_dual_drain()
{
	if (t_aicaThread || !aica_dual_on())
		return;
	g_aicaDualDrains++;
	const u64 s = g_jobSubmitted.load(std::memory_order_relaxed);
	dual_wait_done(s);
	while (g_jobApplied < s)
		apply_spu_irq(g_jobs[(g_jobApplied++) % DUAL_RING].irq);
	apply_writes(g_aramQ);
}

// SH4 escreve na ARAM (handler da area 0, sb_mem.cpp): fila ate a proxima fronteira
bool aica_dual_aram_write(u32 addr, u32 data, u32 sz)
{
	if (t_aicaThread || !aica_dual_on())
		return false;
	g_aramQ.push_back({ addr & ARAM_MASK, data, sz });
	return true;
}

static void aica_dual_stop()
{
	if (!g_dualThread.joinable())
		return;
	dual_wait_done(g_jobSubmitted.load(std::memory_order_relaxed));
	{
		std::lock_guard<std::mutex> l(g_dualM);
		g_dualQuit = true;
	}
	g_dualCvJob.notify_all();
	g_dualThread.join();
	g_dualQuit = false;
}

static int AicaUpdate(int tag, int c, int j)
{
	if (aica_dual_on())
	{
		if (!g_dualThread.joinable())
			g_dualThread = std::thread(aica_dual_worker);
		const u64 s = g_jobSubmitted.load(std::memory_order_relaxed);
		const u32 lag = dual_lag();
		// entrega as saidas dos quanta que vencem agora (o quantum j vence em T_{j+lag})
		while (g_jobApplied + lag <= s)
		{
			dual_wait_done(g_jobApplied + 1);
			apply_spu_irq(g_jobs[(g_jobApplied++) % DUAL_RING].irq);
		}
		// quantum s: escritas do SH4 desde a ultima fronteira + CDDA (estado do GD-ROM, aqui)
		DualJob &jb = g_jobs[s % DUAL_RING];
		jb.writes.swap(g_aramQ);
		g_aramQ.clear();
		aica_cdda_prefill(jb.cddaL, jb.cddaR);
		{
			std::lock_guard<std::mutex> l(g_dualM);
			g_jobSubmitted.store(s + 1, std::memory_order_release);
		}
		g_dualCvJob.notify_one();
		return AICA_TICK;
	}
	extern bool g_rendSplitEnabled;
	u64 k0 = g_rendSplitEnabled ? aica_ticks() : 0;
   aicaarm::run(32);
	u64 k1 = g_rendSplitEnabled ? aica_ticks() : 0;
	// Com DSP tambem em lote (o DSP roda no render, docs/mmap_moe_mtp_plan.md fase 1a)
	if (!settings.aica.NoBatch)
		AICA_Sample32();
	if (g_rendSplitEnabled)
	{
		g_aicaArmTicks += k1 - k0;
		g_aicaCtrlTicks += aica_ticks() - k1;
	}

	return AICA_TICK;
}

//Mainloop

void libAICA_TimeStep()
{
   for (int i=0;i<3;i++)
		timers[i].StepTimer(1);

	SCIPD->SAMPLE_DONE = 1;
	MCIPD->SAMPLE_DONE = 1;

	if (settings.aica.NoBatch)
		AICA_Sample();

	//Make sure sh4/arm interrupt system is up to date :)
	update_arm_interrupts();
	UpdateSh4Ints();	
}

static void AicaInternalDMA()
{
	if (!CommonData->DEXE)
		return;

	// Start dma
	DEBUG_LOG(AICA, "AICA internal DMA: DGATE %d DDIR %d DLG %x", CommonData->DGATE, CommonData->DDIR, CommonData->DLG);
	if (CommonData->DGATE)
	{
		// Clear memory/registers
		if (CommonData->DDIR)
		{
			// to wave mem
			u32 addr = ((CommonData->DMEA_hi << 16) | (CommonData->DMEA_lo << 2)) & ARAM_MASK;
			u32 len = std::min(CommonData->DLG, ARAM_SIZE - addr);
			memset(&aica_ram.data[addr], 0, len * 4);
		}
		else
		{
			// to regs
			u32 addr = CommonData->DRGA << 2;
			for (u32 i = 0; i < CommonData->DLG; i++, addr += 4)
				WriteMem_aica_reg(addr, 0, 4);
		}
	}
	else
	{
		// Data xfer
		u32 waddr = ((CommonData->DMEA_hi << 16) | (CommonData->DMEA_lo << 2)) & ARAM_MASK;
		u32 raddr = CommonData->DRGA << 2;
		u32 len = std::min(CommonData->DLG, ARAM_SIZE - waddr);
		if (CommonData->DDIR)
		{
			// reg to wave mem
			for (u32 i = 0; i < len; i++, waddr += 4, raddr += 4)
				*(u32*)&aica_ram[waddr] = ReadMem_aica_reg(raddr, 4);
		}
		else
		{
			// wave mem to regs
			for (u32 i = 0; i < len; i++, waddr += 4, raddr += 4)
				WriteMem_aica_reg(raddr, *(u32*)&aica_ram[waddr], 4);
		}
	}
	CommonData->DEXE = 0;
	MCIPD->DMA_END = 1;
	UpdateSh4Ints();
	SCIPD->DMA_END = 1;
	update_arm_interrupts();
}

//Memory i/o
template<u32 sz>
void WriteAicaReg(u32 reg,u32 data)
{
	switch (reg)
	{
	case SCIPD_addr:
		verify(sz!=1);
		if (data & (1<<5))
		{
			SCIPD->SCPU=1;
			update_arm_interrupts();
		}
		//Read only
		return;

	case SCIRE_addr:
		{
			verify(sz!=1);
			SCIPD->full&=~(data /*& SCIEB->full*/ );	//is the & SCIEB->full needed ? doesn't seem like it
			data=0;//Write only
			update_arm_interrupts();
		}
		break;

	case MCIPD_addr:
		if (data & (1<<5))
		{
			verify(sz!=1);
			MCIPD->SCPU=1;
			UpdateSh4Ints();
		}
		//Read only
		return;

	case MCIRE_addr:
		{
			verify(sz!=1);
			MCIPD->full&=~data;
			UpdateSh4Ints();
			//Write only
		}
		break;

	case TIMER_A:
		WriteMemArr<sz>(aica_reg, reg, data);
		timers[0].RegisterWrite();
		break;

	case TIMER_B:
		WriteMemArr<sz>(aica_reg, reg, data);
		timers[1].RegisterWrite();
		break;

	case TIMER_C:
		WriteMemArr<sz>(aica_reg, reg, data);
		timers[2].RegisterWrite();
		break;

	// DEXE, DDIR, DLG
	case 0x288C:
		WriteMemArr<sz>(aica_reg, reg, data);
		AicaInternalDMA();
		break;

	default:
		WriteMemArr<sz>(aica_reg, reg, data);
		break;
	}
}



template void WriteAicaReg<1>(u32 reg,u32 data);
template void WriteAicaReg<2>(u32 reg,u32 data);

//misc :p
s32 libAICA_Init()
{
	init_mem();
	aica_Init();

	static_assert(sizeof(*CommonData) == 0x508, "Invalid CommonData size");
	static_assert(sizeof(*DSPData) == 0x15C8, "Invalid DSPData size");

	CommonData=(CommonData_struct*)&aica_reg[0x2800];
	DSPData=(DSPData_struct*)&aica_reg[0x3000];
	//slave cpu (arm7)

	SCIEB=(InterruptInfo*)&aica_reg[0x289C];
	SCIPD=(InterruptInfo*)&aica_reg[0x289C+4];
	SCIRE=(InterruptInfo*)&aica_reg[0x289C+8];
	//Main cpu (sh4)
	MCIEB=(InterruptInfo*)&aica_reg[0x28B4];
	MCIPD=(InterruptInfo*)&aica_reg[0x28B4+4];
	MCIRE=(InterruptInfo*)&aica_reg[0x28B4+8];

	sgc_Init();
	if (aica_schid == -1)
	{
		aica_schid = sh4_sched_register(0, &AicaUpdate);
		sh4_sched_request(aica_schid, AICA_TICK);
	}

	return 0;
}

void libAICA_Reset(bool hard)
{
	aica_dual_drain();
	aica_mix_sync();
	if (hard)
	{
		init_mem();
		sgc_Init();
	}
	for (u32 i = 0; i < 3; i++)
		timers[i].Init(aica_reg, i);
	aica_Reset(hard);
}

void libAICA_Term()
{
	aica_dual_stop();
	sgc_Term();
	term_mem();
}
