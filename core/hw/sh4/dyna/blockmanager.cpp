/*
	Tiny cute block manager. Doesn't keep block graphs or anything fancy ...
	Its based on a simple hashed-lists idea
*/

#include <algorithm>
#include <set>
#include <map>
#include <unordered_map>
#include "blockmanager.h"
#include "ngen.h"

#include "../sh4_core.h"
#include "hw/sh4/sh4_mem.h"
#include "hw/sh4/sh4_sched.h"
#include "hw/sh4/sh4_opcode_list.h"	// DissasembleOpcode(), for bm_DumpHotBlocks()


#if defined(__unix__) && defined(DYNA_OPROF)
#include <opagent.h>
op_agent_t          oprofHandle;
#endif

#if FEAT_SHREC != DYNAREC_NONE


typedef std::vector<RuntimeBlockInfoPtr> bm_List;
typedef std::set<RuntimeBlockInfoPtr> bm_Set;
typedef std::map<void*, RuntimeBlockInfoPtr> bm_Map;

static bm_Set all_temp_blocks;
static bm_List del_blocks;

bool unprotected_pages[RAM_SIZE_MAX/PAGE_SIZE];
static std::set<RuntimeBlockInfo*> blocks_per_page[RAM_SIZE_MAX/PAGE_SIZE];

// Re-protecting pages that stopped being written (docs/tech_debits.md 4.19).
// A write to a page holding compiled code unprotects it -- before this, for
// good: every block compiled there afterwards carries the inline anti-SMC
// compare on EVERY execution. Games that load code overlays at runtime
// (Shenmue) end up with their hottest loops on such pages: 490 checked blocks
// on 9 pages cost ~19% fps in Shenmue's vertex transform. The checked blocks
// of each unprotected page are tracked here, and a page that has been quiet
// for a while is protected again: its checked blocks are discarded and
// recompile protected. A new write faults as usual; each re-fault doubles the
// quiet time, and after a few the page is left unprotected (the old
// behaviour) so a page mixing code with hot data can't thrash. Same idea as
// the PCSX2 recompiler's page protection modes.
static std::set<RuntimeBlockInfo*> checked_blocks_per_page[RAM_SIZE_MAX/PAGE_SIZE];
static u32 page_unprotected_at[RAM_SIZE_MAX/PAGE_SIZE];	// in DreamcastSecond ticks
static u8 page_reprotections[RAM_SIZE_MAX/PAGE_SIZE];
static u32 bm_seconds;
u32 g_smcReprotected, g_smcRefaults, g_smcGaveUp;
static const u32 SMC_QUIET_SECONDS = 3;
static const u8 SMC_MAX_REPROTECTIONS = 6;

// Data writes into pages that hold compiled code (docs/tech_debits.md 4.19).
// Logged in Shenmue: every write that kept its hottest code pages
// unprotected landed OUTSIDE the compiled code -- variables living in the same
// 4KB page as the routine that uses them, not self-modifying code. Unprotecting
// the page for that made every block on it carry the inline anti-SMC compare
// (~19% of Shenmue's frame). Instead, a JIT store that faults on a locked code
// page is rewritten (ngen_RewriteCodePageStore) to a stub that checks, per
// write, whether it really hits compiled code: if not, it writes through an
// unlocked mirror of the RAM and the page stays protected; if it does, the
// usual SMC path runs. The stub's own fast path skips all this for pages that
// aren't locked, so a generic store site that once hit a code page stays cheap.
u8 bm_code_page_locked[RAM_SIZE_MAX / PAGE_SIZE];
// 1 bit per 32-byte chunk of RAM that holds guest code of a protected block.
// The store stubs test it inline: a data write into a locked page but outside
// any code chunk goes straight to the RAM mirror without calling C. First
// attempt without it sent every such write to C: 8.7M calls in 40s in Shenmue,
// 20.3 -> 11.2 fps.
u8 bm_code_chunks[RAM_SIZE_MAX / 32 / 8];

static void bm_MarkCodeChunks(u32 ram_start, u32 size)
{
	for (u32 c = ram_start / 32; c <= (ram_start + size - 1) / 32; c++)
		bm_code_chunks[c / 8] |= (u8)(1 << (c % 8));
}
static void bm_RebuildPageChunks(u32 page);
static std::unordered_map<u32, std::vector<RuntimeBlockInfo*>> fold_chunk_blocks;
static std::unordered_map<RuntimeBlockInfo*, std::vector<u32>> block_fold_chunks;
static u8 bm_fold_volatile[RAM_SIZE_MAX / 32 / 8];
u32 g_foldedReads, g_foldInvalidations;
static void bm_ForgetFoldedReads(RuntimeBlockInfo* block);
static void bm_InvalidateFoldedReads(u32 ram_addr, u32 size);
static bool bm_WriteHitsCode(u32 ram_addr, u32 size);
static u8 *bm_RamAlias();

// Constant-address RAM stores compiled to write through the RAM mirror
// (GenWriteMemoryImmediate): which blocks do it, into which 32-byte chunks.
// If a protected block is ever compiled over such a chunk, those blocks are
// discarded -- recompiled, their store falls back to the locked view and SMC
// detection works as always.
static std::unordered_map<u32, std::vector<RuntimeBlockInfo*>> alias_chunk_blocks;
static std::unordered_map<RuntimeBlockInfo*, std::vector<u32>> block_alias_chunks;
u32 g_aliasedConstStores;

static void bm_ForgetAliasedStores(RuntimeBlockInfo* block)
{
	auto it = block_alias_chunks.find(block);
	if (it == block_alias_chunks.end())
		return;
	for (u32 chunk : it->second)
	{
		auto cit = alias_chunk_blocks.find(chunk);
		if (cit == alias_chunk_blocks.end())
			continue;
		auto& v = cit->second;
		v.erase(std::remove(v.begin(), v.end(), block), v.end());
		if (v.empty())
			alias_chunk_blocks.erase(cit);
	}
	block_alias_chunks.erase(it);
}

u8 *bm_ram_alias;		// a RAM mirror bm_LockPage never locks
u32 g_codePageDataWrites, g_codePageSmcWrites, g_codePageRewrites;

static bm_Map blkmap;
// Stats
u32 protected_blocks;
u32 unprotected_blocks;

#define FPCA(x) ((DynarecCodeEntryPtr&)sh4rcb.fpcb[(x>>1)&FPCB_MASK])

// addr must be a physical address
// This returns an executable address
static DynarecCodeEntryPtr DYNACALL bm_GetCode(u32 addr)
{
	DynarecCodeEntryPtr rv = FPCA(addr);

	return rv;
}

// addr must be a virtual address
// This returns an executable address
DynarecCodeEntryPtr DYNACALL bm_GetCodeByVAddr(u32 addr)
{
#ifndef NO_MMU
	if (!mmu_enabled())
#endif
		return bm_GetCode(addr);
#ifndef NO_MMU
	else
	{
		if (addr & 1)
		{
			switch (addr)
			{
#ifdef USE_WINCE_HACK
			case 0xfffffde7: // GetTickCount
				// This should make this syscall faster
				r[0] = sh4_sched_now64() * 1000 / SH4_MAIN_CLOCK;
				next_pc = pr;
				break;

			case 0xfffffd05: // QueryPerformanceCounter(u64 *)
				{
					u32 paddr;
					if (mmu_data_translation<MMU_TT_DWRITE, u64>(r[4], paddr) == MMU_ERROR_NONE)
					{
						_vmem_WriteMem64(paddr, sh4_sched_now64() >> 4);
						r[0] = 1;
						next_pc = pr;
					}
					else
					{
						Do_Exception(addr, 0xE0, 0x100);
					}
				}
				break;
#endif

			default:
				Do_Exception(addr, 0xE0, 0x100);
				break;
			}
			addr = next_pc;
		}

		u32 paddr;
		u32 rv = mmu_instruction_translation(addr, paddr);
		if (rv != MMU_ERROR_NONE)
		{
			DoMMUException(addr, rv, MMU_TT_IREAD);
			mmu_instruction_translation(next_pc, paddr);
		}

		return bm_GetCode(paddr);
	}
#endif
}

// addr must be a physical address
// This returns an executable address
RuntimeBlockInfoPtr DYNACALL bm_GetBlock(u32 addr)
{
	DynarecCodeEntryPtr cde = bm_GetCode(addr);  // Returns RX ptr

	if (cde == ngen_FailedToFindBlock)
		return NULL;
	else
		return bm_GetBlock2((void*)cde);  // Returns RX ptr
}

// This takes a RX address and returns the info block ptr (RW space)
RuntimeBlockInfoPtr bm_GetBlock2(void* dynarec_code)
{
	if (blkmap.empty())
		return NULL;

	void *dynarecrw = CC_RX2RW(dynarec_code);
	// Returns a block who's code addr is bigger than dynarec_code (or end)
	auto iter = blkmap.upper_bound(dynarecrw);
	if (iter == blkmap.begin())
		return NULL;
	iter--;  // Need to go back to find the potential candidate

	// However it might be out of bounds, check for that
	if ((u8*)iter->second->code + iter->second->host_code_size < (u8*)dynarecrw)
		return NULL;

	verify(iter->second->contains_code((u8*)dynarecrw));
	return iter->second;
}

static void bm_CleanupDeletedBlocks()
{
	del_blocks.clear();
}

// Takes RX pointer and returns a RW pointer
RuntimeBlockInfoPtr bm_GetStaleBlock(void* dynarec_code)
{
   void *dynarecrw = CC_RX2RW(dynarec_code);
	if (del_blocks.empty())
		return NULL;
	// Start from the end to get the youngest one
	auto it = del_blocks.end();
	do
	{
		it--;
		if ((*it)->contains_code((u8*)dynarecrw))
			return *it;
	} while (it != del_blocks.begin());

	return NULL;
}

void bm_AddBlock(RuntimeBlockInfo* blk)
{
	RuntimeBlockInfoPtr block(blk);
	if (block->temp_block)
		all_temp_blocks.insert(block);
	auto iter = blkmap.find((void*)blk->code);
	if (iter != blkmap.end()) {
		INFO_LOG(DYNAREC, "DUP: %08X %p %08X %p", iter->second->addr, iter->second->code, block->addr, block->code);
		verify(false);
	}
	blkmap[(void*)block->code] = block;

	verify((void*)bm_GetCode(block->addr) == (void*)ngen_FailedToFindBlock);
	FPCA(block->addr) = (DynarecCodeEntryPtr)CC_RW2RX(block->code);

#ifdef DYNA_OPROF
	if (oprofHandle)
	{
		char fname[512];

		sprintf(fname,"sh4:%08X,c:%d,s:%d,h:%d", block->addr, block->guest_cycles, block->guest_opcodes, block->host_opcodes);

		if (op_write_native_code(oprofHandle, fname, (uint64_t)block->code, (void*)block->code, block->host_code_size) != 0)
		{
			INFO_LOG(DYNAREC, "op_write_native_code error");
		}
	}
#endif

}

void bm_DiscardBlock(RuntimeBlockInfo* block)
{
	// Remove from block map
	auto it = blkmap.find((void*)block->code);
	verify(it != blkmap.end());
	RuntimeBlockInfoPtr block_ptr = it->second;

	blkmap.erase(it);

	block_ptr->pNextBlock = NULL;
	block_ptr->pBranchBlock = NULL;
	block_ptr->Relink();

	// Remove from jump table
	verify((void*)bm_GetCode(block_ptr->addr) == CC_RW2RX((void*)block_ptr->code));
	FPCA(block_ptr->addr) = ngen_FailedToFindBlock;

	if (block_ptr->temp_block)
		all_temp_blocks.erase(block_ptr);

	del_blocks.push_back(block_ptr);
	block_ptr->Discard();
}

static void bm_ReprotectQuietPages()
{
	static int enabled = -1;
	if (enabled == -1)
	{
		const char *e = getenv("FC_SMC_REPROTECT");
		enabled = (e == nullptr || atoi(e) != 0) ? 1 : 0;
	}
	if (!enabled)
		return;
	for (u32 page = 0; page < RAM_SIZE_MAX / PAGE_SIZE; page++)
	{
		if (!unprotected_pages[page] || page_reprotections[page] >= SMC_MAX_REPROTECTIONS)
			continue;
		const u32 quiet = SMC_QUIET_SECONDS << page_reprotections[page];
		if (bm_seconds - page_unprotected_at[page] < quiet)
			continue;
		// Only worth it if code actually lives here.
		if (checked_blocks_per_page[page].empty())
			continue;
		std::vector<RuntimeBlockInfo*> list_copy(checked_blocks_per_page[page].begin(),
				checked_blocks_per_page[page].end());
		for (RuntimeBlockInfo* block : list_copy)
		{
			// Only blocks still live in the block map: a temp block already
			// dropped by bm_ResetTempCache waits in del_blocks for cleanup.
			auto it = blkmap.find((void*)block->code);
			if (it != blkmap.end() && it->second.get() == block)
				bm_DiscardBlock(block);
		}
		checked_blocks_per_page[page].clear();
		// The page is still writable; it gets locked again when the first
		// protected block is compiled on it (SetProtectedFlags).
		unprotected_pages[page] = false;
		page_reprotections[page]++;
		g_smcReprotected++;
	}
}

void bm_DumpJitRuns();
void bm_Periodical_1s()
{
	bm_DumpJitRuns();	// FC_JIT_DUMP (emu thread, junto das compilacoes)
	bm_CleanupDeletedBlocks();
	bm_seconds++;
	// After the cleanup: blocks discarded now stay in del_blocks until the
	// next second, so a fault inside one still running can be rewritten.
	bm_ReprotectQuietPages();
}


void bm_vmem_pagefill(void** ptr, u32 size_bytes)
{
	for (size_t i = 0; i < size_bytes / sizeof(ptr[0]); i++)
	{
		ptr[i]=(void*)ngen_FailedToFindBlock;
	}
}

void bm_Reset()
{
	bm_CleanupDeletedBlocks();
	protected_blocks = 0;
	unprotected_blocks = 0;

	if (_nvmem_enabled())
	{
		// Windows cannot lock/unlock a region spanning more than one VirtualAlloc or MapViewOfFile
		// so we have to unlock each region individually
		// No need for this mess in 4GB mode since windows doesn't use it
		if (RAM_SIZE == 16 * 1024 * 1024)
		{
			mem_region_unlock(virt_ram_base + 0x0C000000, RAM_SIZE);
			mem_region_unlock(virt_ram_base + 0x0D000000, RAM_SIZE);
			mem_region_unlock(virt_ram_base + 0x0E000000, RAM_SIZE);
			mem_region_unlock(virt_ram_base + 0x0F000000, RAM_SIZE);
		}
		else
		{
			mem_region_unlock(virt_ram_base + 0x0C000000, RAM_SIZE);
			mem_region_unlock(virt_ram_base + 0x0E000000, RAM_SIZE);
		}
		if (_nvmem_4gb_space())
		{
			mem_region_unlock(virt_ram_base + 0x8C000000, 0x90000000 - 0x8C000000);
			mem_region_unlock(virt_ram_base + 0xAC000000, 0xB0000000 - 0xAC000000);
		}
	}
	else
	{
		mem_region_unlock(&mem_b[0], RAM_SIZE);
	}
}

static void bm_LockPage(u32 addr)
{
	addr = addr & (RAM_MASK - PAGE_MASK);
	bm_code_page_locked[addr / PAGE_SIZE] = 1;
	if (_nvmem_enabled())
	{
		if (!mmu_enabled() || !_nvmem_4gb_space())
			mem_region_lock(virt_ram_base + 0x0C000000 + addr, PAGE_SIZE);
		if (_nvmem_4gb_space())
		{
			mem_region_lock(virt_ram_base + 0x8C000000 + addr, PAGE_SIZE);
			mem_region_lock(virt_ram_base + 0xAC000000 + addr, PAGE_SIZE);
			// TODO wraps
		}
	}
	else
	{
		mem_region_lock(&mem_b[addr], PAGE_SIZE);
	}
}

static void bm_UnlockPage(u32 addr)
{
	addr = addr & (RAM_MASK - PAGE_MASK);
	bm_code_page_locked[addr / PAGE_SIZE] = 0;
	if (_nvmem_enabled())
	{
		if (!mmu_enabled() || !_nvmem_4gb_space())
			mem_region_unlock(virt_ram_base + 0x0C000000 + addr, PAGE_SIZE);
		if (_nvmem_4gb_space())
		{
			mem_region_unlock(virt_ram_base + 0x8C000000 + addr, PAGE_SIZE);
			mem_region_unlock(virt_ram_base + 0xAC000000 + addr, PAGE_SIZE);
			// TODO wraps
		}
	}
	else
	{
		mem_region_unlock(&mem_b[addr], PAGE_SIZE);
	}
}

void bm_ResetCache()
{
	ngen_ResetBlocks();
	_vmem_bm_reset();

	for (const auto& it : blkmap)
	{
		RuntimeBlockInfoPtr block = it.second;
		block->relink_data = 0;
		block->pNextBlock = 0;
		block->pBranchBlock = 0;
		// needed for the transition to full mmu. Could perhaps limit it to the current block.
		block->Relink();
		// Avoid circular references
		block->Discard();
		del_blocks.push_back(block);
	}

	blkmap.clear();
	// blkmap includes temp blocks as well
	all_temp_blocks.clear();

	for (auto& block_list : blocks_per_page)
		block_list.clear();

	memset(unprotected_pages, 0, sizeof(unprotected_pages));
	for (auto& list : checked_blocks_per_page)
		list.clear();
	memset(page_unprotected_at, 0, sizeof(page_unprotected_at));
	memset(bm_code_page_locked, 0, sizeof(bm_code_page_locked));
	memset(bm_code_chunks, 0, sizeof(bm_code_chunks));
	alias_chunk_blocks.clear();
	block_alias_chunks.clear();
	fold_chunk_blocks.clear();
	block_fold_chunks.clear();
	memset(bm_fold_volatile, 0, sizeof(bm_fold_volatile));
	memset(page_reprotections, 0, sizeof(page_reprotections));

#ifdef DYNA_OPROF
	if (oprofHandle)
	{
		for (int i=0;i<del_blocks.size();i++)
		{
			if (op_unload_native_code(oprofHandle, (uint64_t)del_blocks[i]->code) != 0)
			{
				INFO_LOG(DYNAREC, "op_unload_native_code error");
			}
		}
	}
#endif
}

void bm_ResetTempCache(bool full)
{
	if (!full)
	{
		for (const auto& block : all_temp_blocks)
		{
			FPCA(block->addr) = ngen_FailedToFindBlock;
			blkmap.erase((void*)block->code);
		}
	}
	del_blocks.insert(del_blocks.begin(),all_temp_blocks.begin(),all_temp_blocks.end());
	all_temp_blocks.clear();
}

void bm_Init()
{

#ifdef DYNA_OPROF
	oprofHandle=op_open_agent();
	if (oprofHandle==0)
		INFO_LOG(DYNAREC, "bm: Failed to open oprofile");
	else
		INFO_LOG(DYNAREC, "bm: Oprofile integration enabled !");
#endif
	bm_Reset();
}

void bm_Term()
{
#ifdef DYNA_OPROF
	if (oprofHandle) op_close_agent(oprofHandle);
	
	oprofHandle=0;
#endif
	bm_Reset();
}

// Opt-in (FC_BLOCK_PROF) hot-block report. The ARM64 backend emits a bump of
// block->runs at every block entry when the same env var is set (see
// rec_arm64.cpp), which is what the x86 backend has always done
// (rec_x86_driver.cpp) and what this fork's ARM64 side never had -- so until
// now there was no way to tell WHICH generated code is hot, only that the JIT
// buffer as a whole was (perf shows one anonymous SH4_TCB blob, and the
// FC_PERF_MAP symbol map didn't get picked up).
//
// Ranked by runs*host_opcodes: host instructions actually executed by that
// block, which is the closest proxy for time we can get without timing every
// block individually (which would cost more than it measures). guest addr is
// printed so the block can be matched against the SH4 code and, with
// bm_WriteBlockMap(), against its SHIL.
void bm_DumpHotBlocks(const std::string& file)
{
	struct Row { u32 addr, runs, host_ops, guest_ops, guest_cycles, host_bytes; double work; };
	std::vector<Row> rows;
	double total_work = 0, total_runs = 0;
	for (auto& it : blkmap)
	{
		RuntimeBlockInfoPtr& b = it.second;
		if (b->runs == 0)
			continue;
		double work = (double)b->runs * b->host_opcodes;
		rows.push_back({ b->vaddr, b->runs, b->host_opcodes, b->guest_opcodes,
						 b->guest_cycles, b->host_code_size, work });
		total_work += work;
		total_runs += b->runs;
	}
	std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) { return a.work > b.work; });

	FILE *f = fopen(file.c_str(), "w");
	if (f == nullptr)
		return;
	fprintf(f, "# blocks=%zu total_runs=%.0f total_host_ops=%.0f\n",
			rows.size(), total_runs, total_work);
	fprintf(f, "#rank\tvaddr\truns\thost_ops\tguest_ops\tcycles\thost_bytes\twork%%\tcum%%\n");
	double cum = 0;
	for (size_t i = 0; i < rows.size() && i < 60; i++)
	{
		const Row& row = rows[i];	// not `r`: that's a macro for the SH4 GPR file
		cum += row.work;
		fprintf(f, "%zu\t%08X\t%u\t%u\t%u\t%u\t%u\t%.2f\t%.2f\n", i + 1, row.addr, row.runs,
				row.host_ops, row.guest_ops, row.guest_cycles, row.host_bytes,
				total_work > 0 ? row.work * 100 / total_work : 0,
				total_work > 0 ? cum * 100 / total_work : 0);
	}

	// Every block, for diffing two snapshots over a window (the counters are
	// cumulative since boot). runs*cycles is EMULATED time, runs*host_ops is
	// host work: a block that is cheap per run but eats emulated time is a
	// wait loop, and emulated time is what the game's speed is made of.
	{
		FILE *fa = fopen((file + ".all").c_str(), "w");
		if (fa != nullptr)
		{
			for (const Row& row : rows)
				fprintf(fa, "%08X\t%u\t%u\t%u\t%u\t%u\n", row.addr, row.runs, row.host_ops,
						row.guest_cycles, row.guest_ops, row.host_bytes);
			fclose(fa);
		}
	}
	// SH4 code of the blocks eating the most EMULATED time.
	{
		std::vector<Row> byCycles = rows;
		std::sort(byCycles.begin(), byCycles.end(), [](const Row& a, const Row& b) {
			return (double)a.runs * a.guest_cycles > (double)b.runs * b.guest_cycles; });
		for (size_t i = 0; i < byCycles.size() && i < 16; i++)
		{
			RuntimeBlockInfoPtr blk;
			for (auto& it : blkmap)
				if (it.second->vaddr == byCycles[i].addr) { blk = it.second; break; }
			if (!blk)
				continue;
			fprintf(f, "\n=== by-cycles #%zu block %08X  runs=%u cycles=%u host_ops=%u branch=%08X next=%08X ===\n",
					i + 1, blk->vaddr, blk->runs, blk->guest_cycles, blk->host_opcodes, blk->BranchBlock, blk->NextBlock);
			for (u32 pc = blk->vaddr; pc < blk->vaddr + blk->sh4_code_size; pc += 2)
			{
				u16 opcode = IReadMem16(pc);
				const char *diss = OpDesc[opcode] != nullptr ? OpDesc[opcode]->diss : "?";
				fprintf(f, "  SH4 %08X: %04X  %s\n", pc, opcode, diss);
			}
		}
	}

	// Detail for the hottest few: the SH4 code and the SHIL it compiled to.
	// Needed to tell a genuinely hot routine apart from a spin loop waiting on
	// a hardware flag -- the counts alone can't distinguish them.
	for (size_t i = 0; i < rows.size() && i < 8; i++)
	{
		RuntimeBlockInfoPtr blk;
		for (auto& it : blkmap)
			if (it.second->vaddr == rows[i].addr) { blk = it.second; break; }
		if (!blk)
			continue;
		fprintf(f, "\n=== #%zu block %08X  runs=%u  type=%d  branch=%08X next=%08X ===\n",
				i + 1, blk->vaddr, blk->runs, blk->BlockType, blk->BranchBlock, blk->NextBlock);
		for (u32 pc = blk->vaddr; pc < blk->vaddr + blk->sh4_code_size; pc += 2)
		{
			u16 opcode = IReadMem16(pc);
			// OpDesc[] is the live opcode table; its `diss` is the mnemonic
			// template. DissasembleOpcode() isn't linked in LIBRETRO builds.
			const char *diss = OpDesc[opcode] != nullptr ? OpDesc[opcode]->diss : "?";
			fprintf(f, "  SH4 %08X: %04X  %s\n", pc, opcode, diss);
		}
		for (size_t j = 0; j < blk->oplist.size(); j++)
			fprintf(f, "  shil %s\n", blk->oplist[j].dissasm().c_str());
	}
	fclose(f);
}

void bm_WriteBlockMap(const std::string& file)
{
	FILE* f=fopen(file.c_str(),"wb");
	if (f)
	{
		INFO_LOG(DYNAREC, "Writing block map !");
		for (auto& it : blkmap)
		{
			RuntimeBlockInfoPtr& block = it.second;
			fprintf(f, "block: %d:%08X:%p:%d:%d:%d\n", block->BlockType, block->addr, block->code, block->host_code_size, block->guest_cycles, block->guest_opcodes);
			for(size_t j = 0; j < block->oplist.size(); j++)
				fprintf(f,"\top: %zd:%d:%s\n", j, block->oplist[j].guest_offs, block->oplist[j].dissasm().c_str());
		}
		fclose(f);
		INFO_LOG(DYNAREC, "Finished writing block map");
	}
}

#if 0
void sh4_jitsym(FILE* out)
{
	for (auto& it : blkmap)
	{
		RuntimeBlockInfoPtr& block = it.second;
		fprintf(out, "%p %d %08X\n", block->code, block->host_code_size, block->addr);
	}
}

u32 GetLookup(RuntimeBlockInfo* elem)
{
	return elem->lookups;
}

bool UDgreater ( RuntimeBlockInfo* elem1, RuntimeBlockInfo* elem2 )
{
	return elem1->runs > elem2->runs;
}

bool UDgreater2 ( RuntimeBlockInfo* elem1, RuntimeBlockInfo* elem2 )
{
	return elem1->runs*elem1->host_opcodes > elem2->runs*elem2->host_opcodes;
}

bool UDgreater3 ( RuntimeBlockInfo* elem1, RuntimeBlockInfo* elem2 )
{
	return elem1->runs*elem1->host_opcodes/elem1->guest_cycles > elem2->runs*elem2->host_opcodes/elem2->guest_cycles;
}

void bm_PrintTopBlocks()
{
	double total_lups=0;
	double total_runs=0;
	double total_cycles=0;
	double total_hops=0;
	double total_sops=0;

	for (size_t i=0;i<all_blocks.size();i++)
	{
		total_lups+=GetLookup(all_blocks[i]);
		total_cycles+=all_blocks[i]->runs*all_blocks[i]->guest_cycles;
		total_hops+=all_blocks[i]->runs*all_blocks[i]->host_opcodes;
		total_sops+=all_blocks[i]->runs*all_blocks[i]->guest_opcodes;
		total_runs+=all_blocks[i]->runs;
	}

	INFO_LOG(DYNAREC, "Total lookups:  %.0fKRuns, %.0fKLuops, Total cycles: %.0fMhz, Total Hops: %.0fMips, Total Sops: %.0fMips!",total_runs/1000,total_lups/1000,total_cycles/1000/1000,total_hops/1000/1000,total_sops/1000/1000);
	total_hops/=100;
	total_cycles/=100;
	total_runs/=100;

	double sel_hops=0;
	for (size_t i=0;i<(all_blocks.size()/100);i++)
	{
		INFO_LOG(DYNAREC, "Block %08X: %p, r: %d (c: %d, s: %d, h: %d) (r: %.2f%%, c: %.2f%%, h: %.2f%%)",
			all_blocks[i]->addr, all_blocks[i]->code,all_blocks[i]->runs,
			all_blocks[i]->guest_cycles,all_blocks[i]->guest_opcodes,all_blocks[i]->host_opcodes,

			all_blocks[i]->runs/total_runs,
			all_blocks[i]->guest_cycles*all_blocks[i]->runs/total_cycles,
			all_blocks[i]->host_opcodes*all_blocks[i]->runs/total_hops);
		
		sel_hops+=all_blocks[i]->host_opcodes*all_blocks[i]->runs;
	}

	INFO_LOG(DYNAREC, " >-< %.2f%% covered in top 1%% blocks",sel_hops/total_hops);

	size_t i;
	for (i=all_blocks.size()/100;sel_hops/total_hops<50;i++)
	{
		INFO_LOG(DYNAREC, "Block %08X: %p, r: %d (c: %d, s: %d, h: %d) (r: %.2f%%, c: %.2f%%, h: %.2f%%)",
			all_blocks[i]->addr, all_blocks[i]->code,all_blocks[i]->runs,
			all_blocks[i]->guest_cycles,all_blocks[i]->guest_opcodes,all_blocks[i]->host_opcodes,

			all_blocks[i]->runs/total_runs,
			all_blocks[i]->guest_cycles*all_blocks[i]->runs/total_cycles,
			all_blocks[i]->host_opcodes*all_blocks[i]->runs/total_hops);
		
		sel_hops+=all_blocks[i]->host_opcodes*all_blocks[i]->runs;
	}

	INFO_LOG(DYNAREC, " >-< %.2f%% covered in top %.2f%% blocks",sel_hops/total_hops,i*100.0/all_blocks.size());

}

void bm_Sort()
{
	INFO_LOG(DYNAREC, "!!!!!!!!!!!!!!!!!!! BLK REPORT !!!!!!!!!!!!!!!!!!!!");

	INFO_LOG(DYNAREC, "     ---- Blocks: Sorted based on Runs ! ----     ");
	std::sort(all_blocks.begin(),all_blocks.end(),UDgreater);
	bm_PrintTopBlocks();

	INFO_LOG(DYNAREC, "<><><><><><><><><><><><><><><><><><><><><><><><><>");

	INFO_LOG(DYNAREC, "     ---- Blocks: Sorted based on hops ! ----     ");
	std::sort(all_blocks.begin(),all_blocks.end(),UDgreater2);
	bm_PrintTopBlocks();

	INFO_LOG(DYNAREC, "<><><><><><><><><><><><><><><><><><><><><><><><><>");

	INFO_LOG(DYNAREC, "     ---- Blocks: Sorted based on wefs ! ----     ");
	std::sort(all_blocks.begin(),all_blocks.end(),UDgreater3);
	bm_PrintTopBlocks();

	INFO_LOG(DYNAREC, "^^^^^^^^^^^^^^^^^^^ END REPORT ^^^^^^^^^^^^^^^^^^^");

	for (size_t i=0;i<all_blocks.size();i++)
	{
		all_blocks[i]->runs=0;
	}
}
#endif

void jit_dump_line(const char *fmt, ...);
bool jit_dump_enabled();
void jit_dump_flush();

// FC_JIT_DUMP: execucoes de todos os blocos vivos (R code vaddr runs), a
// cada ~150 frames (libretro.cpp); os descartados saem no destrutor (D).
void bm_DumpJitRuns()
{
	if (!jit_dump_enabled())
		return;
	jit_dump_line("T\n");
	for (auto& it : blkmap)
	{
		RuntimeBlockInfoPtr& b = it.second;
		if (b->runs != 0)
			jit_dump_line("R %zx %08X %u\n", (size_t)b->code, b->vaddr, b->runs);
	}
	jit_dump_flush();
}

RuntimeBlockInfo::~RuntimeBlockInfo()
{
	if (runs != 0 && code != nullptr)
		jit_dump_line("D %zx %08X %u\n", (size_t)code, vaddr, runs);
	bm_ForgetAliasedStores(this);
	bm_ForgetFoldedReads(this);
	if (sh4_code_size != 0)
	{
		// bm_Reset() zeroes these counters while discarded blocks may still be
		// waiting in del_blocks; don't let their destruction wrap them around.
		if (read_only)
		{
			if (protected_blocks > 0)
				protected_blocks--;
		}
		else
		{
			if (unprotected_blocks > 0)
				unprotected_blocks--;
			// Some paths drop blocks without Discard() (bm_ResetTempCache):
			// never leave a dangling pointer in the re-protection lists.
			if (IsOnRam(this->addr))
				for (u32 a = this->addr & ~PAGE_MASK; a < this->addr + this->sh4_code_size; a += PAGE_SIZE)
					checked_blocks_per_page[(a & RAM_MASK) / PAGE_SIZE].erase(this);
		}
	}
}

void RuntimeBlockInfo::AddRef(const RuntimeBlockInfoPtr& other)
{ 
	pre_refs.push_back(other); 
}

void RuntimeBlockInfo::RemRef(const RuntimeBlockInfoPtr& other)
{ 
	bm_List::iterator it = std::find(pre_refs.begin(), pre_refs.end(), other);
	if (it != pre_refs.end())
		pre_refs.erase(it);
}

void RuntimeBlockInfo::Discard()
{
	// Update references
	for (RuntimeBlockInfoPtr& ref : pre_refs)
	{
		if (ref->NextBlock == vaddr)
			ref->pNextBlock = NULL;
		if (ref->BranchBlock == vaddr)
			ref->pBranchBlock = NULL;
		ref->relink_data = 0;
		ref->Relink();
	}
	pre_refs.clear();

	bm_ForgetAliasedStores(this);
	bm_ForgetFoldedReads(this);
	if (!read_only && sh4_code_size != 0 && IsOnRam(this->addr))
	{
		for (u32 addr = this->addr & ~PAGE_MASK; addr < this->addr + this->sh4_code_size; addr += PAGE_SIZE)
			checked_blocks_per_page[(addr & RAM_MASK) / PAGE_SIZE].erase(this);
	}
	if (read_only)
	{
		// Remove this block from the per-page block lists
		for (u32 addr = this->addr & ~PAGE_MASK; addr < this->addr + this->sh4_code_size; addr += PAGE_SIZE)
		{
         auto& block_list = blocks_per_page[(addr & RAM_MASK) / PAGE_SIZE];
			block_list.erase(this);
			bm_RebuildPageChunks((addr & RAM_MASK) / PAGE_SIZE);
		}
	}
}

void RuntimeBlockInfo::SetProtectedFlags()
{
#ifndef TARGET_NO_EXCEPTIONS
	// Don't write protect rom and BIOS/IP.BIN (Grandia II)
	if (!IsOnRam(addr) || (addr & 0x1FFF0000) == 0x0c000000)
#endif
	{
		this->read_only = false;
		unprotected_blocks++;
		return;
	}
	for (u32 addr = this->addr & ~PAGE_MASK; addr < this->addr + sh4_code_size; addr += PAGE_SIZE)
	{
		if (unprotected_pages[(addr & RAM_MASK) / PAGE_SIZE])
		{
			this->read_only = false;
			unprotected_blocks++;
			// Track it on every unprotected page it covers, so re-protecting
			// any of them can find and discard it.
			for (u32 a = this->addr & ~PAGE_MASK; a < this->addr + sh4_code_size; a += PAGE_SIZE)
				if (unprotected_pages[(a & RAM_MASK) / PAGE_SIZE])
					checked_blocks_per_page[(a & RAM_MASK) / PAGE_SIZE].insert(this);
			return;
		}
	}
	this->read_only = true;
	protected_blocks++;
	for (u32 addr = this->addr & ~PAGE_MASK; addr < this->addr + sh4_code_size; addr += PAGE_SIZE)
	{
		auto& block_list = blocks_per_page[(addr & RAM_MASK) / PAGE_SIZE];
		if (block_list.empty())
			bm_LockPage(addr);
		block_list.insert(this);
	}
	bm_MarkCodeChunks(this->addr & RAM_MASK, sh4_code_size);
	if (!alias_chunk_blocks.empty())
	{
		const u32 start = this->addr & RAM_MASK;
		std::vector<RuntimeBlockInfo*> writers;
		for (u32 c = start / 32; c <= (start + sh4_code_size - 1) / 32; c++)
		{
			auto it = alias_chunk_blocks.find(c);
			if (it != alias_chunk_blocks.end())
				writers.insert(writers.end(), it->second.begin(), it->second.end());
		}
		std::sort(writers.begin(), writers.end());
		writers.erase(std::unique(writers.begin(), writers.end()), writers.end());
		for (RuntimeBlockInfo* w : writers)
		{
			if (w == this)
				continue;
			static int logged = getenv("FC_SMC_LOG") != nullptr ? 0 : 1 << 30;
			if (logged++ < 40)
				fprintf(stderr, "ALIAS writer %08x discarded: block %08x compiled over its target\n", w->vaddr, this->vaddr);
			auto bit = blkmap.find((void*)w->code);
			if (bit != blkmap.end() && bit->second.get() == w)
				bm_DiscardBlock(w);
			else
				bm_ForgetAliasedStores(w);
		}
	}
}

int g_unprotectSource;	// set by the fault handler: 1 = JIT store, 0 = anything else
void bm_RamWriteAccess(u32 addr)
{
	addr &= RAM_MASK;
	{
		// Who unprotects code pages, and does the write touch compiled code?
		static int logged = getenv("FC_SMC_LOG") != nullptr ? 0 : 1 << 30;
		if (logged < 60 && !unprotected_pages[addr / PAGE_SIZE] && !blocks_per_page[addr / PAGE_SIZE].empty())
		{
			logged++;
			fprintf(stderr, "UNPROTECT page %08x write %08x pc~%08x from=%s hits_code=%d blocks=%zu\n",
					addr & ~PAGE_MASK, addr, next_pc, g_unprotectSource ? "JIT" : "other",
					(int)bm_WriteHitsCode(addr, 1), blocks_per_page[addr / PAGE_SIZE].size());
		}
	}
	if (unprotected_pages[addr / PAGE_SIZE])
	{
		ERROR_LOG(DYNAREC, "Page %08x already unprotected", addr);
		die("Fatal error");
	}
	unprotected_pages[addr / PAGE_SIZE] = true;
	page_unprotected_at[addr / PAGE_SIZE] = bm_seconds;
	if (page_reprotections[addr / PAGE_SIZE] != 0)
	{
		g_smcRefaults++;
		if (page_reprotections[addr / PAGE_SIZE] >= SMC_MAX_REPROTECTIONS)
			g_smcGaveUp++;
		// Is the write inside compiled code (real self-modifying code) or
		// just data sharing the 4KB page with it? Decides the fix.
		static int logged = getenv("FC_SMC_LOG") != nullptr ? 0 : 1 << 30;
		if (logged < 40)
		{
			logged++;
			const RuntimeBlockInfo* hit = nullptr;
			for (RuntimeBlockInfo* b : blocks_per_page[addr / PAGE_SIZE])
				if ((b->addr & RAM_MASK) <= addr && addr < (b->addr & RAM_MASK) + b->sh4_code_size)
					hit = b;
			INFO_LOG(DYNAREC, "SMC refault: write to %08x (page %08x) pc~%08x -> %s%08x",
					addr, addr & ~PAGE_MASK, next_pc,
					hit ? "INSIDE block " : "outside code, blocks on page=", hit ? hit->vaddr : (u32)blocks_per_page[addr / PAGE_SIZE].size());
			fprintf(stderr, "SMC refault: write to %08x pc~%08x -> %s %08x\n", addr, next_pc,
					hit ? "INSIDE block" : "outside code, blocks_on_page", hit ? hit->vaddr : (u32)blocks_per_page[addr / PAGE_SIZE].size());
		}
	}
	bm_UnlockPage(addr);
   std::set<RuntimeBlockInfo*>& block_list = blocks_per_page[addr / PAGE_SIZE];
   std::vector<RuntimeBlockInfo*> list_copy;
	list_copy.insert(list_copy.begin(), block_list.begin(), block_list.end());
	if (!list_copy.empty())
		DEBUG_LOG(DYNAREC, "bm_RamWriteAccess write access to %08x pc %08x", addr, next_pc);
	for (auto& block : list_copy)
	{
		bm_DiscardBlock(block);
	}
	verify(block_list.empty());
}

// Does [ram_addr, ram_addr+size) overlap the guest code of a protected block?
static bool bm_WriteHitsCode(u32 ram_addr, u32 size)
{
	for (u32 page = ram_addr / PAGE_SIZE; page <= (ram_addr + size - 1) / PAGE_SIZE; page++)
		for (const RuntimeBlockInfo* b : blocks_per_page[page])
		{
			const u32 start = b->addr & RAM_MASK;
			if (ram_addr < start + b->sh4_code_size && ram_addr + size > start)
				return true;
		}
	return false;
}

// Reads the SSA optimizer folded into a constant at compile time (ssa.h: a
// protected block reading a fixed address on its own pages). That was only
// valid because any write to the page faulted and discarded the block. With
// data writes going through the RAM mirror without faulting, such a write
// must discard the blocks that folded the old value -- missing this is what
// crashed KOF XI and MBAA at boot (the task kernel folds reads of variables
// that live right next to its code). A chunk found to be written after being
// folded becomes "volatile": never folded again, so it can't thrash.

static void bm_ForgetFoldedReads(RuntimeBlockInfo* block)
{
	auto it = block_fold_chunks.find(block);
	if (it == block_fold_chunks.end())
		return;
	for (u32 chunk : it->second)
	{
		auto cit = fold_chunk_blocks.find(chunk);
		if (cit == fold_chunk_blocks.end())
			continue;
		auto& v = cit->second;
		v.erase(std::remove(v.begin(), v.end(), block), v.end());
		if (v.empty())
			fold_chunk_blocks.erase(cit);
	}
	block_fold_chunks.erase(it);
}

static bool bm_CodePageStoresEnabled()
{
	static int enabled = -1;
	if (enabled == -1)
	{
		// ON by default: validated on 11 games (Shenmue, kofxi, MBAA, kofnw,
		// mslug6, ggxxsla, gwing2, sfz3ugd, Ikaruga, capsnk, meltybld),
		// including real self-modifying code in Ikaruga/capsnk.
		// FC_CODEPAGE_STORES=0 turns it off.
		const char *e = getenv("FC_CODEPAGE_STORES");
		enabled = (e == nullptr || atoi(e) != 0) ? 1 : 0;
	}
	return enabled == 1;
}

static bool bm_ChunkBit(const u8 *bitmap, u32 chunk)
{
	return (bitmap[chunk / 8] >> (chunk % 8)) & 1;
}

// Discard live blocks from a list (or just drop the registration of blocks
// already out of the block map).
static void bm_DiscardListed(std::vector<RuntimeBlockInfo*> list, RuntimeBlockInfo* except,
		void (*forget)(RuntimeBlockInfo*))
{
	std::sort(list.begin(), list.end());
	list.erase(std::unique(list.begin(), list.end()), list.end());
	for (RuntimeBlockInfo* b : list)
	{
		if (b == except)
			continue;
		auto bit = blkmap.find((void*)b->code);
		if (bit != blkmap.end() && bit->second.get() == b)
			bm_DiscardBlock(b);
		else
			forget(b);
	}
}

// SSA, at compile time: may this block fold a read of addr into a constant?
bool bm_CanFoldRead(RuntimeBlockInfo* block, u32 addr, u32 size)
{
	if (!bm_CodePageStoresEnabled() || !IsOnRam(addr))
		return true;		// every write to the page faults: folding is safe as before
	const u32 ram_addr = addr & RAM_MASK;
	const u32 first = ram_addr / 32, last = (ram_addr + size - 1) / 32;
	for (u32 c = first; c <= last; c++)
		if (bm_ChunkBit(bm_fold_volatile, c))
			return false;
	// A chunk that constant stores already write (through the mirror) is known
	// to change: don't fold it, ever. Discarding those writers instead would
	// just make them fault on the locked view -- a form the store rewrite
	// can't handle -- and unprotect the whole page (Shenmue: 23.6 -> 19.9 fps).
	for (u32 c = first; c <= last; c++)
		if (alias_chunk_blocks.count(c) != 0)
		{
			for (u32 v = first; v <= last; v++)
				bm_fold_volatile[v / 8] |= (u8)(1 << (v % 8));
			return false;
		}
	for (u32 c = first; c <= last; c++)
	{
		fold_chunk_blocks[c].push_back(block);
		block_fold_chunks[block].push_back(c);
		bm_code_chunks[c / 8] |= (u8)(1 << (c % 8));	// stub writes here go to C
	}
	g_foldedReads++;
	return true;
}

// JIT, at compile time: may this constant-address store write through the RAM
// mirror? Yes unless it writes into compiled code right now.
u8 *bm_ConstStoreAlias(RuntimeBlockInfo* block, u32 addr, u32 size)
{
	static int enabled = -1;
	if (enabled == -1)
	{
		const char *const_env = getenv("FC_CODEPAGE_CONST");
		enabled = (bm_CodePageStoresEnabled() && (const_env == nullptr || atoi(const_env) != 0)) ? 1 : 0;
	}
	if (!enabled || !_nvmem_enabled() || mmu_enabled() || !IsOnRam(addr) || bm_RamAlias() == nullptr)
		return nullptr;
	const u32 ram_addr = addr & RAM_MASK;
	if (bm_WriteHitsCode(ram_addr, size))
		return nullptr;
	// A chunk some block folded a read of: that fold is wrong now that the
	// chunk is known to be written. Discard the folding blocks (they recompile
	// without the fold, the chunk becomes volatile) -- unless it's the block
	// being compiled, whose code already embeds the value: then stay on the
	// locked view, the safe path.
	bool folded = false;
	for (u32 c = ram_addr / 32; c <= (ram_addr + size - 1) / 32; c++)
	{
		if (fold_chunk_blocks.count(c) == 0)
			continue;
		folded = true;
		auto self = block_fold_chunks.find(block);
		if (self != block_fold_chunks.end()
				&& std::find(self->second.begin(), self->second.end(), c) != self->second.end())
			return nullptr;
	}
	if (folded)
		bm_InvalidateFoldedReads(ram_addr, size);
	for (u32 c = ram_addr / 32; c <= (ram_addr + size - 1) / 32; c++)
	{
		alias_chunk_blocks[c].push_back(block);
		block_alias_chunks[block].push_back(c);
	}
	g_aliasedConstStores++;
	{
		static int logged = getenv("FC_SMC_LOG") != nullptr ? 0 : 1 << 30;
		if (logged++ < 40)
			fprintf(stderr, "ALIAS store %08x size %u in block %08x (alias base %p, virt %p, RAM_SIZE %x)\n",
					addr, size, block->vaddr, bm_RamAlias(), virt_ram_base, RAM_SIZE);
	}
	return bm_RamAlias() + ram_addr;
}

// A chunk may be shared by several blocks: recompute the page from the blocks
// still registered on it.
static void bm_RebuildPageChunks(u32 page)
{
	memset(&bm_code_chunks[page * (PAGE_SIZE / 32) / 8], 0, PAGE_SIZE / 32 / 8);
	const u32 page_start = page * PAGE_SIZE;
	for (const RuntimeBlockInfo* b : blocks_per_page[page])
	{
		u32 start = b->addr & RAM_MASK;
		u32 end = start + b->sh4_code_size;
		start = std::max(start, page_start);
		end = std::min(end, page_start + PAGE_SIZE);
		if (end > start)
			bm_MarkCodeChunks(start, end - start);
	}
	if (!fold_chunk_blocks.empty())
		for (u32 c = page_start / 32; c < (page_start + PAGE_SIZE) / 32; c++)
			if (fold_chunk_blocks.count(c) != 0)
				bm_code_chunks[c / 8] |= (u8)(1 << (c % 8));
}

static u8 *bm_RamAlias()
{
	// Area 3 maps the RAM with mirrors every RAM_SIZE bytes; bm_LockPage only
	// locks the first one (and its P1/P2 views), never this one.
	if (bm_ram_alias == nullptr && _nvmem_enabled())
		bm_ram_alias = virt_ram_base + 0x0C000000 + RAM_SIZE;
	return bm_ram_alias;
}

// Fault handler: is this a data write to a locked code page (as opposed to
// self-modifying code)? Only then may the store site be rewritten.
int g_codePageLastReason;
bool bm_IsDataWriteToCodePage(void *p)
{
	static int enabled = -1;
	if (enabled == -1)
	{
		const char *rw_env = getenv("FC_CODEPAGE_REWRITE");
		enabled = (bm_CodePageStoresEnabled() && (rw_env == nullptr || atoi(rw_env) != 0)) ? 1 : 0;
	}
	g_codePageLastReason = 1;
	if (!enabled || !_nvmem_enabled() || mmu_enabled() || bm_RamAlias() == nullptr)
		return false;
	const u8 *limit = virt_ram_base + (_nvmem_4gb_space() ? 0x100000000L : 0x20000000);
	g_codePageLastReason = 2;
	if ((u8 *)p < virt_ram_base || (u8 *)p >= limit)
		return false;
	u32 addr = (u32)((u8 *)p - virt_ram_base);
	if (!IsOnRam(addr) || ((addr >> 29) > 0 && (addr >> 29) < 4))
	{
		static int logged = getenv("FC_SMC_LOG") != nullptr ? 0 : 1 << 30;
		if (logged++ < 5)
			fprintf(stderr, "CODEPAGE reason2: p=%p base=%p 4gb=%d guest=%08x onram=%d\n", p, virt_ram_base,
					(int)_nvmem_4gb_space(), addr, (int)IsOnRam(addr));
		return false;
	}
	addr &= RAM_MASK;
	g_codePageLastReason = 3;
	if (!bm_code_page_locked[addr / PAGE_SIZE])
		return false;
	g_codePageLastReason = 4;
	if (unprotected_pages[addr / PAGE_SIZE])
		return false;
	// Largest store is 8 bytes: be conservative about what it touches.
	g_codePageLastReason = 5;
	return !bm_WriteHitsCode(addr, 8);
}

// A data write about to land in chunks some blocks folded reads of: discard
// those blocks and never fold these chunks again.
static void bm_InvalidateFoldedReads(u32 ram_addr, u32 size)
{
	std::vector<RuntimeBlockInfo*> folders;
	for (u32 c = ram_addr / 32; c <= (ram_addr + size - 1) / 32; c++)
	{
		auto it = fold_chunk_blocks.find(c);
		if (it == fold_chunk_blocks.end())
			continue;
		folders.insert(folders.end(), it->second.begin(), it->second.end());
		bm_fold_volatile[c / 8] |= (u8)(1 << (c % 8));
	}
	if (folders.empty())
		return;
	g_foldInvalidations++;
	bm_DiscardListed(folders, nullptr, bm_ForgetFoldedReads);
	// Clear the watch bits the folds set, unless code still lives there.
	for (u32 page = ram_addr / PAGE_SIZE; page <= (ram_addr + size - 1) / PAGE_SIZE; page++)
		bm_RebuildPageChunks(page);
}

// Slow path of the rewritten store sites (the stub only gets here for locked
// pages or addresses outside main RAM).
template<typename T>
static void DYNACALL bm_WriteMemCodePage(u32 addr, T data)
{
	if (IsOnRam(addr))
	{
		const u32 ram_addr = addr & RAM_MASK;
		if (bm_code_page_locked[ram_addr / PAGE_SIZE] && !unprotected_pages[ram_addr / PAGE_SIZE])
		{
			if (!bm_WriteHitsCode(ram_addr, sizeof(T)))
			{
				g_codePageDataWrites++;
				bm_InvalidateFoldedReads(ram_addr, sizeof(T));
				memcpy(bm_RamAlias() + ram_addr, &data, sizeof(T));
				return;
			}
			// Real self-modifying code: the usual path (unprotect + discard).
			g_codePageSmcWrites++;
			bm_RamWriteAccess(ram_addr);
		}
	}
	switch (sizeof(T))
	{
	case 1: WriteMem8(addr, (u8)data); break;
	case 2: WriteMem16(addr, (u16)data); break;
	case 4: WriteMem32(addr, (u32)data); break;
	default: WriteMem64(addr, (u64)data); break;
	}
}
void DYNACALL bm_WriteMemCodePage8(u32 addr, u8 data) { bm_WriteMemCodePage<u8>(addr, data); }
void DYNACALL bm_WriteMemCodePage16(u32 addr, u16 data) { bm_WriteMemCodePage<u16>(addr, data); }
void DYNACALL bm_WriteMemCodePage32(u32 addr, u32 data) { bm_WriteMemCodePage<u32>(addr, data); }
void DYNACALL bm_WriteMemCodePage64(u32 addr, u64 data) { bm_WriteMemCodePage<u64>(addr, data); }

bool bm_RamWriteAccess(void *p)
{
	if (_nvmem_enabled())
	{
		if (_nvmem_4gb_space())
		{
			if ((u8 *)p < virt_ram_base || (u8 *)p >= virt_ram_base + 0x100000000L)
				return false;
		}
		else
		{
			if ((u8 *)p < virt_ram_base || (u8 *)p >= virt_ram_base + 0x20000000)
				return false;
		}
		u32 addr = (u8*)p - virt_ram_base;
		if (mmu_enabled() && _nvmem_4gb_space() && (addr & 0x80000000) == 0)
			// If mmu enabled, let vmem32 manage user space
			// shouldn't be necessary since it's called first
			return false;
		if (!IsOnRam(addr) || ((addr >> 29) > 0 && (addr >> 29) < 4))	// system RAM is not mapped to 20, 40 and 60 because of laziness
			return false;
		bm_RamWriteAccess(addr);
	}
	else
	{
		if ((u8 *)p < &mem_b[0] || (u8 *)p >= &mem_b[RAM_SIZE])
			return false;
		bm_RamWriteAccess((u32)((u8 *)p - &mem_b[0]));
	}

	return true;
}

bool print_stats;
#if 0
void fprint_hex(FILE* d,const char* init,u8* ptr, u32& ofs, u32 limit)
{
	int base=ofs;
	int cnt=0;
	while(ofs<limit)
	{
		if (cnt==32)
		{
			fputs("\n",d);
			cnt=0;
		}

		if (cnt==0)
			fprintf(d,"%s:%d:",init,ofs-base);

		fprintf(d," %02X",ptr[ofs++]);
		cnt++;
	}
	fputs("\n",d);
}



void print_blocks()
{
	FILE* f=0;

	if (print_stats)
	{
		f=fopen(get_writable_data_path("/blkmap.lst").c_str(),"w");
		print_stats=false;

		INFO_LOG(DYNAREC, "Writing blocks to %p", f);
	}

	for (auto it : blkmap)
	{
		RuntimeBlockInfoPtr blk = it.second;
		if (f)
		{
			fprintf(f,"block: %p\n",blk.get());
			fprintf(f,"vaddr: %08X\n",blk->vaddr);
			fprintf(f,"paddr: %08X\n",blk->addr);
			fprintf(f,"hash: %s\n",blk->hash());
			fprintf(f,"hash_rloc: %s\n",blk->hash());
			fprintf(f,"code: %p\n",blk->code);
			fprintf(f,"runs: %d\n",blk->runs);
			fprintf(f,"BlockType: %d\n",blk->BlockType);
			fprintf(f,"NextBlock: %08X\n",blk->NextBlock);
			fprintf(f,"BranchBlock: %08X\n",blk->BranchBlock);
			fprintf(f,"pNextBlock: %p\n",blk->pNextBlock);
			fprintf(f,"pBranchBlock: %p\n",blk->pBranchBlock);
			fprintf(f,"guest_cycles: %d\n",blk->guest_cycles);
			fprintf(f,"guest_opcodes: %d\n",blk->guest_opcodes);
			fprintf(f,"host_opcodes: %d\n",blk->host_opcodes);
			fprintf(f,"il_opcodes: %zd\n",blk->oplist.size());

			u32 hcode=0;
			s32 gcode=-1;
			u8* pucode=(u8*)blk->code;

			size_t j=0;
			
			fprintf(f,"{\n");
			for (;j<blk->oplist.size();j++)
			{
				shil_opcode* op = &blk->oplist[j];
				//fprint_hex(f,"//h:",pucode,hcode,op->host_offs);

				if (gcode!=op->guest_offs)
				{
					gcode=op->guest_offs;
					u32 rpc=blk->vaddr+gcode;
#ifndef NO_MMU
					try {
#endif
						u16 op=IReadMem16(rpc);

					char temp[128];
						OpDesc[op]->Disassemble(temp,rpc,op);

						fprintf(f,"//g: %04X %s\n", op, temp);
#ifndef NO_MMU
					} catch (SH4ThrownException& ex) {
						fprintf(f,"//g: ???? (page fault)\n");
					}
#endif
				}

            std::string s=op->dissasm();
				fprintf(f,"//il:%d:%d:%s\n",op->guest_offs,op->host_offs,s.c_str());
			}
			
			//fprint_hex(f,"//h:",pucode,hcode,blk->host_code_size);

			fprintf(f,"}\n");
		}

		blk->runs=0;
	}

	if (f) fclose(f);
}
#endif
#endif

