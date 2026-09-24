#include "types.h"

/*
	tag, as passed on sh4_sched_register
	sch_cycles, the cycle duration that the callback requested (sh4_sched_request)
	jitter, the number of cycles that the callback was delayed, [0... 448]
*/
typedef int sh4_sched_callback(int tag, int sch_cycl, int jitter);

/*
	Registed a callback to the scheduler. The returned id 
	is used for sh4_sched_request and sh4_sched_elapsed calls
*/
int sh4_sched_register(int tag, sh4_sched_callback* ssc);

/*
	current time in SH4 cycles, referenced to boot.
	Wraps every ~21 secs
*/
u32 sh4_sched_now();

/*
	current time, in SH4 cycles, referenced to boot.
	Does not wrap, 64 bits.
*/
u64 sh4_sched_now64();

/*
	Schedule a callback to be called sh4 *cycles* after the
	invocation of this function. *Cycles* range is (0, 200M].
	
	Passing a value of 0 disables the callback.
	If called multiple times, only the last call is in effect
*/
void sh4_sched_request(int id, int cycles);

/*
	Tick for *cycles*
*/
void sh4_sched_tick(int cycles);

void sh4_sched_ffts();

/*
	Idle fast-forward: the guest has been proven to be only waiting (see the
	signatures in dyna/decoder.cpp), so consume every cycle up to -- but never
	past -- the next scheduled event, exactly as if it had kept spinning.
	Called from JIT code; the caller must end the current timeslice right after
	so the event is handled immediately.
*/
void sh4_sched_idle_fastforward();
void DYNACALL sh4_delay_loop_skip(u32 pc, u32 cyc);
extern u64 g_idleFFCalls, g_idleFFCycles;

struct sched_list
{
	sh4_sched_callback* cb;
	int tag;
	int start;
	int end;
};
