#pragma once
#include "types.h"

struct host_context_t {
#if HOST_CPU != CPU_GENERIC
	size_t pc;
#endif

#if HOST_CPU == CPU_X86
	u32 eax;
	u32 ecx;
	u32 esp;
#elif HOST_CPU == CPU_ARM
	u32 r[15];
#elif HOST_CPU == CPU_ARM64
	u64 x2;
	// x0: preserved by GenWriteMemoryFast()/GenReadMemoryFast() (only x7/x1 are
	// clobbered computing the masked fastmem offset) -- carries the original,
	// unmasked guest address at the exact point a fastmem access faults. Used
	// by ngen_Rewrite() to recognize special address ranges (e.g. SH4 Store
	// Queues) without adding any check to the common/fast path -- see
	// docs/arm64jit_improvement_plan.md item 1.
	u64 x0;
#endif
};
