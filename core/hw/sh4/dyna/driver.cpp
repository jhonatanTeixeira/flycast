#include "types.h"
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <vector>

#include "deps/xxhash/xxhash.h"
#include "../sh4_interpreter.h"
#include "../sh4_opcode_list.h"
#include "../sh4_core.h"
#include "../sh4_if.h"
#include "hw/sh4/sh4_interrupts.h"

#include "hw/mem/_vmem.h"
#include "hw/sh4/sh4_mem.h"
#include "hw/sh4/modules/mmu.h"
#include "hw/pvr/pvr_mem.h"
#include "hw/aica/aica_if.h"
#include "hw/gdrom/gdrom_if.h"

#include <time.h>
#include <float.h>

#include "blockmanager.h"
#include "ngen.h"
#include "decoder.h"

#if FEAT_SHREC != DYNAREC_NONE

u8 SH4_TCB[CODE_SIZE + TEMP_CODE_SIZE + 4096]
#if defined(_WIN32) || FEAT_SHREC != DYNAREC_JIT
	;
#elif defined(__linux__) || defined(__HAIKU__) || \
      defined(__FreeBSD__) || defined(__DragonFly__) || \
	  defined(HAVE_LIBNX)
	__attribute__((section(".text")));
#elif defined(__MACH__)
	__attribute__((section("__TEXT,.text")));
#else
	#error SH4_TCB ALLOC
#endif

u8* CodeCache;
u8* TempCodeCache;
uintptr_t cc_rx_offset;

u32 LastAddr = 0;
u32 LastAddr_min = 0;
u32 TempLastAddr = 0;
u32* emit_ptr = nullptr;
u32* emit_ptr_limit = nullptr;

std::unordered_set<u32> smc_hotspots;

void* emit_GetCCPtr(void)
{
   if (emit_ptr)
      return (void*)emit_ptr;
   return (void*)&CodeCache[LastAddr];
}
void emit_WriteCodeCache() {}

void emit_SetBaseAddr(void)
{
   LastAddr_min = LastAddr;
}

void jit_dump_line(const char *fmt, ...);
void jit_dump_time();
void clear_temp_cache(bool full)
{
	jit_dump_time();
	jit_dump_line("Z temp full=%d used=%u\n", (int)full, TempLastAddr);
	//printf("recSh4:Temp Code Cache clear at %08X\n", curr_pc);
	TempLastAddr = 0;
	bm_ResetTempCache(full);
}

static void jit_hot_reset_samples();

// Limpezas do cache por motivo (FC_SYNC_STATS, fase 1b do docs/mmap_moe_mtp_plan.md):
// 0 falha de checagem de codigo, 1 cache cheio / PC de boot, 2 externa (load de
// savestate, MMU, ResetCache), 3 reset, 4 arena quente. Cada limpeza descarta todo o
// codigo compilado (120-140 ms) e tudo e recompilado sob demanda.
u32 g_ccClears[5];
u64 g_ccClearUs;
static int g_ccReason = 2;

static void recSh4_ClearCache(void)
{
	struct timespec t0, t1;
	clock_gettime(CLOCK_MONOTONIC, &t0);
	g_ccClears[g_ccReason < 0 || g_ccReason > 4 ? 2 : g_ccReason]++;
	g_ccReason = 2;		// quem nao marca o motivo e externo (cpu->ResetCache)
	jit_hot_reset_samples();	// as amostras da arena quente apontam para o cache antigo
	jit_dump_time();
	jit_dump_line("Z full pc=%08X free=%u\n", next_pc, emit_FreeSpace());
	INFO_LOG(DYNAREC, "recSh4:Dynarec Cache clear at %08X free space %d", next_pc, emit_FreeSpace());
	LastAddr=LastAddr_min;
	bm_ResetCache();
	smc_hotspots.clear();
	clear_temp_cache(true);
	clock_gettime(CLOCK_MONOTONIC, &t1);
	g_ccClearUs += (u64)(t1.tv_sec - t0.tv_sec) * 1000000ull + (t1.tv_nsec - t0.tv_nsec) / 1000;
}

static void recSh4_Run(void)
{
   sh4_int_bCpuRun = true;
	RestoreHostRoundingMode();

	sh4_dyna_rcb=(u8*)&Sh4cntx + sizeof(Sh4cntx);
	INFO_LOG(DYNAREC, "cntx // fpcb offset: %td // pc offset: %td // pc %08X", (u8*)&sh4rcb.fpcb - sh4_dyna_rcb, (u8*)&sh4rcb.cntx.pc - sh4_dyna_rcb, sh4rcb.cntx.pc);
	//verify(rcb_noffs(&next_pc)==-184);
	ngen_mainloop(sh4_dyna_rcb);

	sh4_int_bCpuRun=false;
}

void emit_Write32(u32 data)
{
	if (emit_ptr)
	{
		*emit_ptr=data;
		emit_ptr++;
	}
	else
	{
		*(u32*)&CodeCache[LastAddr]=data;
		LastAddr+=4;
	}
}

void emit_Skip(u32 sz)
{
	if (emit_ptr)
		emit_ptr = (u32*)((u8*)emit_ptr + sz);
	else
		LastAddr+=sz;
}
u32 emit_FreeSpace()
{
	if (emit_ptr)
		return (emit_ptr_limit - emit_ptr) * sizeof(u32);
	else
		return CODE_SIZE - LastAddr;
}

void AnalyseBlock(RuntimeBlockInfo* blk);

static char block_hash[1024];

const char* RuntimeBlockInfo::hash()
{
	XXH32_hash_t hash = 0;

	u8* ptr = GetMemPtr(this->addr, this->sh4_code_size);

	if (ptr)
	{
		XXH32_state_t *state = XXH32_createState();
		XXH32_reset(state, 7);
			for (u32 i=0; i<this->guest_opcodes; i++)
			{
				u16 data=ptr[i];
				//Do not count PC relative loads (relocated code)
				if ((ptr[i]>>12)==0xD)
					data=0xD000;

			XXH32_update(state, &data, 2);
			}
		hash = XXH32_digest(state);
		XXH32_freeState(state);
		}

	sprintf(block_hash, ">:1:%02X:%08X", this->guest_opcodes, hash);

	return block_hash;
}

bool RuntimeBlockInfo::Setup(u32 rpc,fpscr_t rfpu_cfg)
{
	staging_runs=addr=lookups=runs=host_code_size=0;
	guest_cycles=guest_opcodes=host_opcodes=0;
	sh4_code_size = 0;
	pBranchBlock=pNextBlock=0;
	code=0;
	has_jcond=false;
	BranchBlock=NextBlock=csc_RetCache=0xFFFFFFFF;
	dyn_link_pc=0;
	BlockType=BET_SCL_Intr;
	has_fpu_op = false;
	temp_block = false;
	idle_fastforward = false;
	idle_ff_ram_reg = 0;
	delay_skip = false;
	scan_skip = false;
	dt_skip = false;
	dt_skip_reg = 0;
	
	vaddr=rpc;
#ifndef NO_MMU
	if (mmu_enabled())
	{
		u32 rv = mmu_instruction_translation(vaddr, addr);
		if (rv != MMU_ERROR_NONE)
		{
			DoMMUException(vaddr, rv, MMU_TT_IREAD);
			return false;
		}
	}
	else
#endif // NO_MMU
		addr = vaddr;
	fpu_cfg=rfpu_cfg;
	
	oplist.clear();

#if !defined(NO_MMU)
	try {
#endif
		if (!dec_DecodeBlock(this, SH4_TIMESLICE / 2))
			return false;
#if !defined(NO_MMU)
	}
	catch (SH4ThrownException& ex) {
		Do_Exception(rpc, ex.expEvn, ex.callVect);
		return false;
	}
#endif
	SetProtectedFlags();


	AnalyseBlock(this);

	return true;
}

static RuntimeBlockInfo *compile_block(u32 pc, fpscr_t fpu_cfg, u32 blockcheck_failures);

// ---------------------------------------------------------------- D3: arena quente
// docs/jit_hot_path.md fase 3, 4.127. O stub intc_sched guarda, a cada g_hotEvery fatias
// (~448 ciclos do SH4 cada), o endereco de retorno da chamada -- que fica na cauda fria
// do bloco onde a fatia acabou (D1). E uma amostragem do tempo por bloco, sem instrucao
// extra nos blocos. Quando a maior parte das amostras cai fora da arena (cena nova, cache
// limpo), num ponto seguro (rdv_DoInterrupts, que volta ao despachante) o cache e limpo
// e os blocos mais quentes sao recompilados primeiro, um atras do outro, no comeco do
// cache: o codigo quente fica contiguo (menos linhas de L1I e paginas). Mesmo codigo
// gerado, mesma configuracao de FPU de cada bloco. FC_JIT_HOT=1 liga (diagnostico).
u32 g_hotCountdown = 64;
static const u32 g_hotEvery = 64;
static const u32 HOT_RING = 16384;
static void *g_hotRing[HOT_RING];
static u32 g_hotN;			// amostras no anel desde a ultima limpeza do cache
static u32 g_hotIn, g_hotOut;		// amostras dentro/fora da arena desde a ultima reorganizacao
static u32 g_hotArenaEnd;		// fim da arena (offset no CodeCache); 0 = sem arena
static u64 g_hotLastUs;		// ultima reorganizacao (ou o inicio)
static bool g_hotDue;
static u32 g_hotRelayouts;

static u64 hot_now_us()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (u64)ts.tv_sec * 1000000ull + ts.tv_nsec / 1000;
}

bool jit_hot_enabled()
{
	// Desligado por padrao ate a fase 2 do docs/mmap_moe_mtp_plan.md (promocao bloco a
	// bloco): a reorganizacao por limpeza do cache travava 0,15-1 s e mudava as fronteiras.
	static const bool en = getenv("FC_JIT_HOT") != nullptr && atoi(getenv("FC_JIT_HOT")) != 0;
	return en;
}

void jit_hot_sample(void *host_pc)
{
	g_hotCountdown = g_hotEvery;
	g_hotRing[g_hotN % HOT_RING] = host_pc;
	g_hotN++;
	const u8 *p = (const u8 *)CC_RX2RW(host_pc);
	if (g_hotArenaEnd != 0 && p >= CodeCache + LastAddr_min && p < CodeCache + g_hotArenaEnd)
		g_hotIn++;
	else
		g_hotOut++;
	if (g_hotDue || (g_hotN & 1023) != 0)
		return;
	const u64 now = hot_now_us();
	if (g_hotLastUs == 0)
		g_hotLastUs = now;
	// a primeira apos ~5 s; depois so se >= 20 s e mais de 15% das amostras fora da arena
	const u64 wait = g_hotArenaEnd == 0 ? 5000000ull : 20000000ull;
	const u32 total = g_hotIn + g_hotOut;
	if (now - g_hotLastUs >= wait && g_hotN >= 8192 && total > 0 && g_hotOut * 100 > total * 15)
		g_hotDue = true;
}

static void jit_hot_reset_samples()
{
	g_hotN = g_hotIn = g_hotOut = 0;
	g_hotArenaEnd = 0;
}

static void recSh4_ClearCache(void);

static void jit_hot_relayout()
{
	g_hotDue = false;
	const u64 t0 = hot_now_us();
	struct Ent { u32 n; fpscr_t cfg; };
	std::unordered_map<u32, Ent> cnt;
	const u32 n = std::min(g_hotN, HOT_RING);
	for (u32 i = 0; i < n; i++)
	{
		RuntimeBlockInfoPtr b = bm_GetBlock2(g_hotRing[i]);
		if (!b || b->temp_block)
			continue;
		Ent &e = cnt[b->vaddr];
		e.n++;
		e.cfg = b->fpu_cfg;
	}
	std::vector<std::pair<u32, Ent>> hot(cnt.begin(), cnt.end());
	std::sort(hot.begin(), hot.end(), [](const std::pair<u32, Ent> &a, const std::pair<u32, Ent> &b) {
		return a.second.n > b.second.n || (a.second.n == b.second.n && a.first < b.first);
	});
	u32 total = 0;
	for (auto &h : hot)
		total += h.second.n;
	const u64 tSort = hot_now_us();
	g_ccReason = 4;
	recSh4_ClearCache();		// zera as amostras (o anel apontava para o cache antigo)
	const u64 tClear = hot_now_us();
	static const bool noCompile = getenv("FC_JIT_HOT_NOCOMPILE") != nullptr;	// diagnostico
	u32 acc = 0, compiled = 0;
	for (auto &h : hot)
	{
		if (noCompile)
			break;
		if (acc * 100 >= total * 95 || compiled >= 1500)
			break;
		acc += h.second.n;
		if (bm_GetBlock(h.first))
			continue;
		if (compile_block(h.first, h.second.cfg, 0) != nullptr)
			compiled++;
	}
	g_hotArenaEnd = LastAddr;
	g_hotLastUs = hot_now_us();
	g_hotRelayouts++;
	fprintf(stderr, "jit hot: reorganizacao %u -- %u amostras, %u blocos quentes (de %zu), arena %u KB, %.1f ms (contagem %.1f, limpeza %.1f, compilacao %.1f)\n",
			g_hotRelayouts, total, compiled, hot.size(), (g_hotArenaEnd - LastAddr_min) / 1024,
			(g_hotLastUs - t0) / 1000.0, (tSort - t0) / 1000.0, (tClear - tSort) / 1000.0, (g_hotLastUs - tClear) / 1000.0);
}

DynarecCodeEntryPtr rdv_CompilePC(u32 blockcheck_failures)
{
	u32 pc=next_pc;
	//printf("rdv_CompilePC next_pc %p\n", next_pc);

	if (emit_FreeSpace()<16*1024 || pc==0x8c0000e0 || pc==0xac010000 || pc==0xac008300)
	{
		g_ccReason = 1;
		recSh4_ClearCache();
	}

	RuntimeBlockInfo *rbi = compile_block(pc, fpscr, blockcheck_failures);
	return rbi != nullptr ? rbi->code : nullptr;
}

// Compila o bloco de pc com a configuracao de FPU dada e o registra (rdv_CompilePC e a
// arena quente).
static RuntimeBlockInfo *compile_block(u32 pc, fpscr_t fpu_cfg, u32 blockcheck_failures)
{
	RuntimeBlockInfo* rbi = ngen_AllocateBlock();

	if (!rbi->Setup(pc,fpu_cfg))
	{
		delete rbi;
		return NULL;
	}
	rbi->blockcheck_failures = blockcheck_failures;
	if (smc_hotspots.find(rbi->addr) != smc_hotspots.end())
	{
		if (TEMP_CODE_SIZE - TempLastAddr < 16 * 1024)
			clear_temp_cache(false);
		emit_ptr = (u32 *)(TempCodeCache + TempLastAddr);
		emit_ptr_limit = (u32 *)(TempCodeCache + TEMP_CODE_SIZE);
		rbi->temp_block = true;
		if (rbi->read_only)
			INFO_LOG(DYNAREC, "WARNING: temp block %x (%x) is protected!", rbi->vaddr, rbi->addr);
	}
		bool do_opts = !rbi->temp_block;
		rbi->staging_runs=do_opts?100:-100;
		bool block_check = rbi->read_only ? false : IsOnRam(rbi->addr);
		// FC_NO_BLOCK_CHECK (diagnosis only, UNSAFE: self-modifying code in an
		// unprotected page goes undetected): measures what the inline
		// anti-SMC compare costs. See docs/tech_debits.md 1.1 / 4.19.
		{
			static int noCheck = -1;
			if (noCheck == -1)
				noCheck = getenv("FC_NO_BLOCK_CHECK") != nullptr ? 1 : 0;
			if (noCheck)
				block_check = false;
		}
		ngen_Compile(rbi, block_check, (pc & 0xFFFFFF) == 0x08300 || (pc & 0xFFFFFF) == 0x10000, false, do_opts);
		verify(rbi->code!=0);

		bm_AddBlock(rbi);

	if (emit_ptr != NULL)
	{
		TempLastAddr = (u8*)emit_ptr - TempCodeCache;
		emit_ptr = NULL;
		emit_ptr_limit = NULL;
	}

	return rbi;
}

DynarecCodeEntryPtr DYNACALL rdv_FailedToFindBlock_pc()
{
	return rdv_FailedToFindBlock(next_pc);
}

DynarecCodeEntryPtr DYNACALL rdv_FailedToFindBlock(u32 pc)
{
	//printf("rdv_FailedToFindBlock ~ %08X\n",pc);
	next_pc=pc;
	DynarecCodeEntryPtr code = rdv_CompilePC(0);
	if (code == NULL)
		code = bm_GetCodeByVAddr(next_pc);
	else
		code = (DynarecCodeEntryPtr)CC_RW2RX(code);
	return code;
}

u32 DYNACALL rdv_DoInterrupts_pc(u32 pc) {
	next_pc = pc;
	UpdateINTC();

	return next_pc;
}

static void ngen_FailedToFindBlock_internal() {
	rdv_FailedToFindBlock(Sh4cntx.pc);
}

void (*ngen_FailedToFindBlock)() = &ngen_FailedToFindBlock_internal;

u32 DYNACALL rdv_DoInterrupts(void* block_cpde)
{
	RuntimeBlockInfoPtr rbi = bm_GetBlock2(block_cpde);
	const u32 pc = rdv_DoInterrupts_pc(rbi->vaddr);
	// Ponto seguro para reorganizar o cache: daqui o stub vai ao despachante (no_update)
	// e o bloco de onde viemos nao continua.
	if (g_hotDue && !mmu_enabled())
		jit_hot_relayout();
	return pc;
}

// addr must be the physical address of the start of the block
DynarecCodeEntryPtr DYNACALL rdv_BlockCheckFail(u32 addr)
{
	u32 blockcheck_failures = 0;
	if (mmu_enabled())
	{
		RuntimeBlockInfoPtr block = bm_GetBlock(addr);
		blockcheck_failures = block->blockcheck_failures + 1;
		if (blockcheck_failures > 5)
		{
			bool inserted = smc_hotspots.insert(addr).second;
			if (inserted)
				DEBUG_LOG(DYNAREC, "rdv_BlockCheckFail SMC hotspot @ %08x fails %d", addr, blockcheck_failures);
		}
		bm_DiscardBlock(block.get());
	}
	else
	{
		next_pc = addr;
		g_ccReason = 0;
		recSh4_ClearCache();
	}
#if !defined(NO_MMU)
	// Robustez (plano 0.1): a recompilacao do bloco aqui pode executar/decodificar
	// codigo invalido e lancar SH4ThrownException (ex.: `iNimp` ao executar
	// dados). Sem catch, a excecao escapava desta funcao (chamada do stub de
	// check-fail do JIT) e chamava std::terminate, FECHANDO o emulador.
	// Tratamos como o interpretador faz e retomamos; pode virar glitch, mas
	// nao crash.
	try {
#endif
		return (DynarecCodeEntryPtr)CC_RW2RX(rdv_CompilePC(blockcheck_failures));
#if !defined(NO_MMU)
	}
	catch (SH4ThrownException& ex) {
		ERROR_LOG(DYNAREC, "SH4 exception escaped rdv_BlockCheckFail (addr=%08X epc=%08X evn=%03X) -- handled", addr, ex.epc, ex.expEvn);
		Do_Exception(ex.epc, ex.expEvn, ex.callVect);
		// A excecao redirecionou next_pc para o handler do guest; devolve o
		// bloco de entrada desse handler (FindOrCompile compila se faltar).
		return rdv_FindOrCompile();
	}
#endif
}

DynarecCodeEntryPtr rdv_FindOrCompile()
{
	DynarecCodeEntryPtr rv = bm_GetCodeByVAddr(next_pc);  // Returns exec addr
	if (rv == ngen_FailedToFindBlock)
		rv = (DynarecCodeEntryPtr)CC_RW2RX(rdv_CompilePC(0)); // Returns rw addr
	
	return rv;
}

void* DYNACALL rdv_LinkBlock(u8* code,u32 dpc)
{
	// code is the RX addr to return after, however bm_GetBlock returns RW
	RuntimeBlockInfoPtr rbi=bm_GetBlock2((void*)code);
	bool stale_block = false;
	if (!rbi)
	{
		stale_block = true;
		rbi = bm_GetStaleBlock(code);
	}
	
	verify(rbi != NULL);

	u32 bcls = BET_GET_CLS(rbi->BlockType);

	if (bcls == BET_CLS_Static)
	{
		if (rbi->BlockType == BET_StaticIntr)
			next_pc = rbi->NextBlock;
		else
			next_pc=rbi->BranchBlock;
	}
	else if (bcls == BET_CLS_Dynamic)
	{
		next_pc=dpc;
	}
	else if (bcls == BET_CLS_COND)
	{
		if (dpc)
			next_pc=rbi->BranchBlock;
		else
			next_pc=rbi->NextBlock;
	}

	DynarecCodeEntryPtr rv = rdv_FindOrCompile();  // Returns rx ptr

	// Se a compilacao do alvo falhou, normalmente foi porque Do_Exception foi
	// chamado durante o decode (ex.: instrucao de FPU com FD=1, ver
	// decoder.cpp) e sobrescreveu o next_pc global com o vetor do handler.
	// Nesse caso a execucao tem de continuar NO HANDLER (next_pc ja aponta
	// pra ele) -- nao seguir com o link do bloco atual, que usaria o next_pc
	// clobberado (0C000100) e cairia num bm_GetBlock() NULL.
	if (rv == NULL)
		return (void*)rdv_FailedToFindBlock(next_pc);

	// Compilar o alvo pode ter limpado o cache de codigo (cheio/reset): o rbi
	// morreu e o endereco dele ja pode ser de um bloco novo. Religar ali
	// corromperia o bloco novo -- so liga se o rbi ainda for o dono do codigo.
	if (!stale_block && bm_GetBlock2((void*)code).get() != rbi.get())
		stale_block = true;

	if (!mmu_enabled() && !stale_block)
	{
		if (bcls == BET_CLS_Dynamic)
		{
			verify(rbi->relink_data == 0 || rbi->pBranchBlock == NULL);

			if (rbi->pBranchBlock!= NULL)
			{
				// Alvo diferente do ligado: o salto e polimorfico, desliga de vez.
				rbi->pBranchBlock->RemRef(rbi);
				rbi->pBranchBlock = NULL;
				rbi->relink_data = 1;
			}
			else if (rbi->relink_data == 0)
			{
				rbi->pBranchBlock = bm_GetBlock(next_pc).get();
				// Blocos temporarios somem no bm_ResetTempCache sem Discard
				// (pre_refs nao seria atualizado): nao liga de/para eles.
				if (rbi->pBranchBlock != NULL && !rbi->temp_block && !rbi->pBranchBlock->temp_block)
				{
					rbi->dyn_link_pc = next_pc;
					rbi->pBranchBlock->AddRef(rbi);
				}
				else
				{
					rbi->pBranchBlock = NULL;
					rbi->relink_data = 1;
				}
			}
		}
		else
		{
			RuntimeBlockInfo* nxt = bm_GetBlock(next_pc).get();

			if (nxt != NULL)
			{
				if (rbi->BranchBlock == next_pc)
					rbi->pBranchBlock = nxt;
				if (rbi->NextBlock == next_pc)
					rbi->pNextBlock = nxt;

				nxt->AddRef(rbi);
			}
		}
		u32 ncs = rbi->relink_offset + rbi->Relink();
		verify(rbi->host_code_size >= ncs);
		// Nao encolhe o tamanho registrado: depois do relink ficam a cauda fria (D1,
		// 4.127) e os literais, que continuam sendo do bloco. Encolher tirava a cauda
		// do bloco e o bm_GetBlock2 do rdv_DoInterrupts (endereco de retorno da chamada
		// ao intc_sched, na cauda) nao achava mais o bloco.
		(void)ncs;
	}
	else
	{
		INFO_LOG(DYNAREC, "null RBI: from %08X to %08X -- unlinked stale block -- code %p next %p", rbi->vaddr, next_pc, code, rv);
	}
	
	return (void*)rv;
}

static void recSh4_Stop(void)
{
	Sh4_int_Stop();
}
static void recSh4_Start(void)
{
	Sh4_int_Start();
}

static void recSh4_Step(void)
{
	Sh4_int_Step();
}

static void recSh4_Skip(void)
{
	Sh4_int_Skip();
}

static void recSh4_Reset(bool hard)
{
	Sh4_int_Reset(hard);
	g_ccReason = 3;
	recSh4_ClearCache();
}

static void recSh4_Init(void)
{
	INFO_LOG(DYNAREC, "recSh4 Init");
	Sh4_int_Init();
	bm_Init();

#if 0
	verify(rcb_noffs(p_sh4rcb->fpcb) == FPCB_OFFSET);
#endif

	verify(rcb_noffs(p_sh4rcb->sq_buffer) == -512);

	verify(rcb_noffs(&p_sh4rcb->cntx.sh4_sched_next) == -152);
	verify(rcb_noffs(&p_sh4rcb->cntx.interrupt_pend) == -148);
	
	if (_nvmem_enabled())
	{
		if (!_nvmem_4gb_space())
		{
			verify(mem_b.data==((u8*)p_sh4rcb->sq_buffer+512+0x0C000000));
		}
		else
		{
			verify(mem_b.data==((u8*)p_sh4rcb->sq_buffer+512+0x8C000000));
		}
	}
	
	// Prepare some pointer to the pre-allocated code cache:
	void *candidate_ptr = (void*)(((unat)SH4_TCB + 4095) & ~4095);

	// Call the platform-specific magic to make the pages RWX
	CodeCache = NULL;
	#ifdef FEAT_NO_RWX_PAGES
	verify(vmem_platform_prepare_jit_block(candidate_ptr, CODE_SIZE + TEMP_CODE_SIZE, (void**)&CodeCache, &cc_rx_offset));
	#else
	verify(vmem_platform_prepare_jit_block(candidate_ptr, CODE_SIZE + TEMP_CODE_SIZE, (void**)&CodeCache));
	#endif
	// Ensure the pointer returned is non-null
	verify(CodeCache != NULL);

	memset(CodeCache, 0xFF, CODE_SIZE + TEMP_CODE_SIZE);
	TempCodeCache = CodeCache + CODE_SIZE;
	ngen_init();
	bm_ResetCache();
}

static void recSh4_Term(void)
{
	INFO_LOG(DYNAREC, "recSh4 Term");
	bm_Term();
	Sh4_int_Term();
}

static bool recSh4_IsCpuRunning(void)
{
	return Sh4_int_IsCpuRunning();
}

void Get_Sh4Recompiler(sh4_if* cpu)
{
	cpu->Run = recSh4_Run;
	cpu->Stop = recSh4_Stop;
	cpu->Start = recSh4_Start;
	cpu->Step = recSh4_Step;
	cpu->Skip = recSh4_Skip;
	cpu->Reset = recSh4_Reset;
	cpu->Init = recSh4_Init;
	cpu->Term = recSh4_Term;
	cpu->IsCpuRunning = recSh4_IsCpuRunning;
	cpu->ResetCache = recSh4_ClearCache;
}
#endif  // FEAT_SHREC != DYNAREC_NONE
