/*
	jit_armv8_a -- novo backend JIT ARM64 do SH4, ao lado do rec_arm64.

	Selecionado por FC_JIT_ARMV8_A=1. Quando ligado (e sem MMU), as funcoes
	globais do ngen em rec_arm64.cpp desviam para estas. Ver
	docs/jit_study.md e docs/jit_armv8_a_context_audit.md.
*/
#ifndef CORE_REC_ARM64_JIT_ARMV8_A_H_
#define CORE_REC_ARM64_JIT_ARMV8_A_H_

#include "types.h"
#include "hw/sh4/dyna/ngen.h"

// Ligado por FC_JIT_ARMV8_A=1 e so quando !mmu_enabled().
bool jit_armv8a_enabled();

void ngen_GetFeatures_a(ngen_features* dst);
void ngen_Compile_a(RuntimeBlockInfo* block, bool force_checks, bool reset, bool staging, bool optimise);
RuntimeBlockInfo* ngen_AllocateBlock_a();

void ngen_CC_Start_a(shil_opcode* op);
void ngen_CC_Param_a(shil_opcode* op, shil_param* par, CanonicalParamType tp);
void ngen_CC_Call_a(shil_opcode* op, void* function);
void ngen_CC_Finish_a(shil_opcode* op);

// Mainloop gerado pelo backend antigo (generate_mainloop); o jit_armv8_a o
// compartilha (mesma convencao x28/w27/w29).
void* jit_armv8a_no_update();
int (*jit_armv8a_intc_sched())();

#endif /* CORE_REC_ARM64_JIT_ARMV8_A_H_ */
