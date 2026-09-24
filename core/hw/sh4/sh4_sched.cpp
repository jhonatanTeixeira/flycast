
#include "types.h"
#include "sh4_interrupts.h"
#include "sh4_core.h"
#include "sh4_sched.h"
#include "sh4_mem.h"
#include <cstdlib>


//sh4 scheduler

/*

	register handler
	request callback at time

	single fire events only

	sh4_sched_register(id)
	sh4_sched_request(id, in_cycles)

	sh4_sched_now()

*/
u64 sh4_sched_ffb;


std::vector<sched_list> sch_list;

int sh4_sched_next_id=-1;

u32 sh4_sched_remaining(int id, u32 reference)
{
	if (sch_list[id].end != -1)
      return sch_list[id].end - reference;
   return -1;
}

u32 sh4_sched_remaining(int id)
{
	return sh4_sched_remaining(id, sh4_sched_now());
}

void sh4_sched_ffts(void)
{
	u32 diff=-1;
	int slot=-1;

	for (size_t i=0;i<sch_list.size();i++)
	{
		if (sh4_sched_remaining(i)<diff)
		{
			slot=i;
			diff=sh4_sched_remaining(i);
		}
	}

	sh4_sched_ffb-=Sh4cntx.sh4_sched_next;

	sh4_sched_next_id=slot;
   if (slot!=-1)
      Sh4cntx.sh4_sched_next=diff;
   else
      Sh4cntx.sh4_sched_next=SH4_MAIN_CLOCK;

	sh4_sched_ffb+=Sh4cntx.sh4_sched_next;
}

u64 g_idleFFCalls, g_idleFFCycles;

void sh4_sched_idle_fastforward()
{
	// Optional cap per call (FC_IDLE_FF_MAX, in SH4 cycles) for experiments;
	// unlimited by default.
	static s32 cap = -2;
	if (cap == -2)
	{
		const char *e = getenv("FC_IDLE_FF_MAX");
		cap = e != nullptr ? atoi(e) : -1;
	}
	// sh4_sched_next is the distance to the next event. Stopping one cycle
	// short leaves it positive, so the next UpdateSystem() crosses it and
	// sh4_sched_tick() fires it through the normal path -- jumping past it
	// would make its remaining time negative and it would never fire.
	s32 skip = Sh4cntx.sh4_sched_next - 1;
	if (cap >= 0 && skip > cap)
		skip = cap;
	if (skip <= 0)
		return;
	Sh4cntx.sh4_sched_next -= skip;
	g_idleFFCalls++;
	g_idleFFCycles += skip;
}

// Variante com checagem (tech_debits 4.43): so avanca se o endereco lido
// (r[reg]) esta na RAM principal (area 3, 0x0C000000-0x0FFFFFFF em qualquer
// espelho P0-P3). Registrador de hardware (TMU etc.) muda com o tempo e o
// avanco passaria do ponto em que o laco sairia.
u64 g_idleFFSkippedHw;
void DYNACALL sh4_sched_idle_fastforward_if_ram(u32 reg)
{
	const u32 a = r[reg & 15];
	if (a < 0xE0000000 && (a & 0x1C000000) == 0x0C000000)
		sh4_sched_idle_fastforward();
	else
		g_idleFFSkippedHw++;
}

// Pulo do laco de atraso (decoder.cpp delay_loop_match, tech_debits 4.40).
// Chamado na entrada do bloco, antes de qualquer registrador ser alocado:
// r4 e o contador de voltas no contexto. `cyc` = ciclos cobrados por volta.
u64 g_delaySkipCalls, g_delaySkipIters;
void DYNACALL sh4_delay_loop_skip(u32 pc, u32 cyc)
{
	const u32 n = r[4];	// `r` e a macro do banco de registradores (sh4_core.h)
	if (n == 0 || n > 0x7FFFFFFF || cyc == 0)
		return;		// ultima volta, ou contagem absurda (deixa o laco real)
	const s32 avail = Sh4cntx.sh4_sched_next - 1;	// ate o proximo evento
	if (avail < (s32)cyc)
		return;
	u32 k = (u32)avail / cyc;
	if (k > n)
		k = n;		// sobra >= 1 volta real (r4 >= 0), que faz a saida normal
	const u16 op = ReadMem16(pc);
	const u32 lit = (pc & ~3u) + 4 + (op & 0xFF) * 4;
	const u32 ctr = ReadMem32(lit);
	WriteMem32(ctr, ReadMem32(ctr) + k);
	r[4] = n - k;
	Sh4cntx.sh4_sched_next -= (s32)(k * cyc);
	g_delaySkipCalls++;
	g_delaySkipIters += k;
}

int sh4_sched_register(int tag, sh4_sched_callback* ssc)
{
	sched_list t={ssc,tag,-1,-1};

	sch_list.push_back(t);

	return sch_list.size()-1;
}

/*
	Return current cycle count, in 32 bits (wraps after 21 dreamcast seconds)
*/
u32 sh4_sched_now(void)
{
	return sh4_sched_ffb-Sh4cntx.sh4_sched_next;
}

/*
	Return current cycle count, in 64 bits (effectively never wraps)
*/
u64 sh4_sched_now64(void)
{
	return sh4_sched_ffb-Sh4cntx.sh4_sched_next;
}
void sh4_sched_request(int id, int cycles)
{
	verify(cycles== -1 || (cycles >= 0 && cycles <= SH4_MAIN_CLOCK));

	sch_list[id].start = sh4_sched_now();
   sch_list[id].end   = -1;

	if (cycles != -1)
	{
		sch_list[id].end = sch_list[id].start + cycles;
		if (sch_list[id].end == -1)
			sch_list[id].end++;
	}

	sh4_sched_ffts();
}

/* Returns how much time has passed for this callback */
static int sh4_sched_elapsed(int id)
{
   if (sch_list[id].end == -1)
      return -1;

   int rv=sh4_sched_now()-sch_list[id].start;
   sch_list[id].start=sh4_sched_now();
   return rv;
}

// FC_REND_SPLIT (2026-09-24, DOA2): tempo da EMU thread em cada periferico
// despachado pelo scheduler (som/ARM7, timers, video, GD-ROM...), pelo
// contador de hardware. Responde quanto da emu thread fica FORA do JIT.
u64 g_schedTicks[32];
u32 g_schedCalls[32];
static inline u64 sched_ticks()
{
#if defined(__aarch64__)
	u64 v;
	asm volatile("mrs %0, cntvct_el0" : "=r"(v));
	return v;
#else
	return 0;
#endif
}

static void handle_cb(int id)
{
	int remain=sch_list[id].end-sch_list[id].start;
	int elapsd=sh4_sched_elapsed(id);
	int jitter=elapsd-remain;

	sch_list[id].end=-1;
	extern bool g_rendSplitEnabled;
	u64 k0 = g_rendSplitEnabled ? sched_ticks() : 0;
	int re_sch=sch_list[id].cb(sch_list[id].tag,remain,jitter);
	if (g_rendSplitEnabled && id < 32)
	{
		g_schedTicks[id] += sched_ticks() - k0;
		g_schedCalls[id]++;
	}

	if (re_sch > 0)
		sh4_sched_request(id, std::max(0, re_sch - jitter));
}

void sh4_sched_tick(int cycles)
{
	/*
	Sh4cntx.sh4_sched_time+=cycles;
	Sh4cntx.sh4_sched_next-=cycles;
	*/

	if (Sh4cntx.sh4_sched_next<0)
	{
		u32 fztime=sh4_sched_now()-cycles;
		if (sh4_sched_next_id!=-1)
		{
			for (int i = 0; i < sch_list.size(); i++)
			{
				int remaining = sh4_sched_remaining(i, fztime);
				verify(remaining >= 0 || remaining == -1);
				if (remaining >= 0 && remaining <= (u32)cycles)
					handle_cb(i);
			}
		}
		sh4_sched_ffts();
	}
}
