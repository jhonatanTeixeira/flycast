/*
	Nivel 2 (docs/current_plan.md, docs/tier2_patches.md): compila uma regiao
	de blocos do JIT antigo (rec_arm64.cpp) num codigo so, com o SH4 em
	registradores a regiao inteira. Porte em C++/VIXL das regras de
	tools/tier2_gen.py --emu (validadas no harness e no jogo com a regiao
	do DOA2 gerada offline, item 4.57).

	Etapa 1 (esta): a regiao vem de FC_TIER2_RT=ENTRADAS/BLOCOS (vaddrs em
	hexa separados por virgula) e e compilada na emu thread, num ponto
	seguro (UpdateSystem), a partir do oplist dos blocos ja compilados.

	Contrato com o JIT antigo:
	- entrada: a 1a instrucao do bloco antigo (`subs w27, w27, #ciclos`) vira
	  `b` para a entrada da regiao; guarda de ciclos falhando na entrada refaz
	  o `subs` e volta para a instrucao seguinte do bloco antigo;
	- dentro: x28 contexto, w27 ciclos, x13 base da memoria (refeita depois
	  de chamada); SH4 escrito em x19-x26 (callee-saved), fpul w14, T w15,
	  so lidos em w9-w12 (recarregados depois de chamada), floats escritos em
	  s16-s31, so lidos em s8-s15, XMTRX (so via ftrv) em v4-v7; x6/x7
	  ponteiros de grupo de store (x16/x17 sao temporarios do VIXL);
	- saida: estado no contexto, w29 = pc, despachante (no_update);
	- guarda de ciclos nas entradas e no predecessor unico de cada volta de
	  laco: passou, os blocos originais so subtraem; falhou, sai antes de
	  executar (exatamente equivalente a checar por bloco);
	- descarga da SQ = chamada C completa (do_sqw); stores de um bloco pela
	  mesma base vao direto no sq_buffer depois de uma checagem no inicio do
	  bloco (na SQ e sem wrap), senao sai para o bloco antigo;
	- bloco da regiao descartado (SMC, reset): ganchos desfeitos.
	Limites da etapa 1: fault dentro da regiao nao e tratado (o fault
	handler recusa e o core para com mensagem); so stores na SQ.
*/
#include "types.h"

#if FEAT_SHREC == DYNAREC_JIT

#include <vector>
#include <string>
#include <functional>
#include <bitset>
#include <map>
#include <set>
#include <algorithm>
#include <cstring>

#include "deps/vixl/aarch64/macro-assembler-aarch64.h"
using namespace vixl::aarch64;

#include "hw/sh4/sh4_core.h"
#include "hw/sh4/sh4_mem.h"
#include "hw/sh4/sh4_interpreter.h"
#include "hw/sh4/modules/mmu.h"
#include "hw/sh4/dyna/ngen.h"
#include "hw/sh4/dyna/shil.h"
#include "hw/sh4/dyna/blockmanager.h"
#undef r		// macros do core (Sh4cntx.r, sh4rcb.do_sqw_nommu)
#undef do_sqw_nommu

extern void vmem_platform_flush_cache(void *icache_start, void *icache_end, void *dcache_start, void *dcache_end);
extern "C" void *t2_no_update;		// rec_arm64.cpp: despachante
extern u8 *CodeCache;
extern u32 tier2_code_reserve;		// driver.cpp: cauda do cache reservada
extern bool tier2_poll;		// sh4_interpreter.cpp: UpdateSystem chama tier2_safe_point

static const u32 T2_AREA = 1024 * 1024;

namespace {

typedef std::bitset<sh4_reg_count> RegSet;

struct Patch { u32 *code; u32 orig; };
struct Region
{
	std::vector<u32> blocks;
	std::vector<Patch> patches;
	bool alive = false;
};

std::vector<Region> regions;
u8 *t2_ptr;
bool cfg_read;
std::vector<u32> cfgEntries, cfgBlocks;
u32 pollCount;
bool gaveUp;

struct Reject
{
	std::string why;
};

u32 ctx_off(int reg)
{
	return (u32)((u8 *)GetRegPtr(reg) - (u8 *)&p_sh4rcb->cntx);
}
bool is_fp(int r) { return r >= reg_fr_0 && r <= reg_xf_15; }
bool is_xf(int r) { return r >= reg_xf_0 && r <= reg_xf_15; }

void add_regs(RegSet &s, const shil_param &p)
{
	if (!p.is_reg())
		return;
	for (u32 i = 0; i < p.count(); i++)
		if (p._reg + i != reg_pc_dyn)
			s.set(p._reg + i);
}
bool is_fschg(const shil_opcode &o)
{
	return o.op == shop_xor && o.rd.is_reg() && o.rd._reg == reg_fpscr;
}
void uses_defs(const shil_opcode &o, RegSet &u, RegSet &d)
{
	u.reset();
	d.reset();
	add_regs(u, o.rs1);
	add_regs(u, o.rs2);
	add_regs(u, o.rs3);
	if (o.op != shop_jcond)
	{
		add_regs(d, o.rd);
		add_regs(d, o.rd2);
	}
}

struct BlockIn
{
	RuntimeBlockInfo *rbi;
	u32 vaddr;
	u32 cycles;
	std::vector<shil_opcode> ir;
	bool cond;
	bool takenOnT;		// cond: desvia para `branch` quando a decisao == 1
	u32 branch, next;	// cond: destino / proximo; salto: branch
	std::vector<u32> succ;	// sucessores dentro da regiao
};

class Tier2Compiler : public MacroAssembler
{
public:
	Tier2Compiler(u8 *buf, size_t size) : MacroAssembler(buf, size) {}

	std::map<u32, BlockIn> blk;
	std::vector<u32> order, entries;
	std::set<u32> latch;
	std::map<u32, u32> guard;
	std::map<u32, u32 *> entryCode;	// entrada -> codigo do bloco antigo
	RegSet W, U, readonly;
	int home[sh4_reg_count];
	bool homeCallee[sh4_reg_count];
	bool vec_xf = false;
	std::map<u32, RegSet> live_in;
	std::vector<std::function<void()>> cold;
	std::map<u32, Label *> blockLabel;
	std::vector<Label *> owned;
	std::map<u32, Label *> entryLabel;
	// estado por bloco
	std::map<const shil_opcode *, std::pair<int, s32>> groupOf;	// op -> (ptr x6/x7, deslocamento)
	bool tAfterJcond = false;
	Register decision = w15;

	~Tier2Compiler() { for (Label *l : owned) delete l; }
	Label *newLabel() { Label *l = new Label(); owned.push_back(l); return l; }

	Register W_(int reg) const { return Register::GetWRegFromCode(home[reg]); }
	VRegister S_(int reg) const { return VRegister::GetSRegFromCode(home[reg]); }
	MemOperand Ctx(int reg) { return MemOperand(x28, ctx_off(reg)); }

	void BranchAbs(const void *target, Condition cond = al)
	{
		ptrdiff_t off = (const u8 *)target - GetBuffer()->GetStartAddress<u8 *>();
		verify(off >= -128 * 1024 * 1024 && off < 128 * 1024 * 1024 && (off & 3) == 0);
		Label l;
		BindToOffset(&l, off);
		if (cond == al)
			B(&l);
		else
			B(&l, cond);
	}

	// ------------------------------------------------------------ analise
	void load(const std::vector<u32> &blockList, const std::vector<u32> &entryList)
	{
		order = blockList;
		std::set<u32> inRegion(order.begin(), order.end());
		for (u32 va : order)
		{
			RuntimeBlockInfo *rbi = bm_GetBlock(va).get();
			if (rbi == nullptr)
				throw Reject{ "bloco nao compilado" };
			if (!rbi->read_only || rbi->temp_block || rbi->idle_fastforward || rbi->delay_skip)
				throw Reject{ "bloco sem protecao de pagina, temporario ou com avanco" };
			BlockIn b;
			b.rbi = rbi;
			b.vaddr = va;
			b.cycles = rbi->guest_cycles;
			switch (rbi->BlockType)
			{
			case BET_Cond_0:
			case BET_Cond_1:
				b.cond = true;
				b.takenOnT = (rbi->BlockType & 1) != 0;
				b.branch = rbi->BranchBlock;
				b.next = rbi->NextBlock;
				break;
			case BET_StaticJump:
			case BET_StaticCall:
				b.cond = false;
				b.branch = rbi->BranchBlock;
				break;
			default:
				throw Reject{ "fim de bloco dinamico/interrupcao" };
			}
			// fschg em par (sem efeito liquido); pref sobe no bloco
			int nfschg = 0;
			for (const shil_opcode &o : rbi->oplist)
			{
				if (is_fschg(o))
				{
					if (!o.rs2.is_imm() || o.rs2._imm != 0x100000)
						throw Reject{ "xor fpscr" };
					nfschg++;
					continue;
				}
				b.ir.push_back(o);
			}
			if (nfschg & 1)
				throw Reject{ "fschg impar" };
			for (size_t i = 0; i < b.ir.size(); i++)
			{
				if (b.ir[i].op != shop_pref)
					continue;
				int addr = b.ir[i].rs1.is_reg() ? b.ir[i].rs1._reg : -1;
				size_t j = i;
				RegSet u, d;
				while (j > 0)
				{
					const shil_opcode &p = b.ir[j - 1];
					uses_defs(p, u, d);
					if (p.op == shop_writem || p.op == shop_pref || p.op == shop_jcond || (addr >= 0 && d.test(addr)))
						break;
					j--;
				}
				shil_opcode op = b.ir[i];
				b.ir.erase(b.ir.begin() + i);
				b.ir.insert(b.ir.begin() + j, op);
			}
			if (b.cond)
			{
				if (inRegion.count(b.branch)) b.succ.push_back(b.branch);
				if (inRegion.count(b.next)) b.succ.push_back(b.next);
			}
			else if (inRegion.count(b.branch))
				b.succ.push_back(b.branch);
			blk[va] = b;
		}
		std::map<u32, std::set<u32>> preds;
		for (auto &kv : blk)
			for (u32 s : kv.second.succ)
				preds[s].insert(kv.first);
		entries = entryList;
		for (u32 v : order)
			if (preds[v].empty() && std::find(entries.begin(), entries.end(), v) == entries.end())
				entries.push_back(v);
		for (u32 e : entries)
		{
			u32 *code = (u32 *)CC_RX2RW((void *)blk[e].rbi->code);
			// subs w27, w27, #imm (sem deslocamento)
			if ((code[0] & 0xFFC003FF) != 0x7100037B)
				throw Reject{ "entrada nao comeca com a checagem de ciclos" };
			entryCode[e] = code;
		}
		// voltas de laco (DFS a partir das entradas)
		std::map<u32, int> color;
		std::function<void(u32)> dfs = [&](u32 v) {
			color[v] = 1;
			for (u32 s : blk[v].succ)
			{
				if (color[s] == 1)
					latch.insert(v);
				else if (color[s] == 0)
					dfs(s);
			}
			color[v] = 2;
		};
		for (u32 e : entries)
			if (color[e] == 0)
				dfs(e);
		// a guarda da volta sobe para o predecessor unico que so leva ao bloco
		std::set<u32> moved;
		for (u32 g : latch)
		{
			while (preds[g].size() == 1)
			{
				u32 p = *preds[g].begin();
				if (blk[p].succ.size() != 1 || blk[p].succ[0] != g || moved.count(p) || p == g)
					break;
				g = p;
			}
			moved.insert(g);
		}
		latch = moved;
		std::map<u32, u32> memo;
		std::function<u32(u32)> longest = [&](u32 v) -> u32 {
			auto it = memo.find(v);
			if (it != memo.end())
				return it->second;
			memo[v] = 0;
			u32 best = 0;
			for (u32 s : blk[v].succ)
				if (!latch.count(s))
					best = std::max(best, longest(s));
			return memo[v] = blk[v].cycles + best;
		};
		for (u32 v : entries)
		{
			memo.clear();
			guard[v] = longest(v);
		}
		for (u32 v : latch)
		{
			memo.clear();
			u32 best = 0;
			for (u32 s : blk[v].succ)
				if (!latch.count(s))
					best = std::max(best, longest(s));
			guard[v] = blk[v].cycles + best;
		}
		for (auto &kv : guard)
			if (kv.second > 4095)
				throw Reject{ "guarda grande demais" };
		// variaveis
		W.reset();
		U.reset();
		for (u32 v : order)
			for (const shil_opcode &o : blk[v].ir)
			{
				check_supported(o);
				RegSet u, d;
				uses_defs(o, u, d);
				if (o.op == shop_ftrv)
				{
					for (int r = reg_xf_0; r <= reg_xf_15; r++)
					{
						if (d.test(r))
							throw Reject{ "ftrv escrevendo xf" };
						u.reset(r);
					}
					vec_xf = true;
				}
				W |= d;
				U |= u;
			}
		for (int r = reg_xf_0; r <= reg_xf_15; r++)
			if (W.test(r) || U.test(r))
				throw Reject{ "xf fora do ftrv" };
		allocate();
		liveness();
	}

	void check_supported(const shil_opcode &o)
	{
		switch (o.op)
		{
		case shop_readm:
		case shop_writem:
		case shop_pref:
		case shop_jcond:
		case shop_mov32:
		case shop_add: case shop_sub: case shop_and: case shop_or: case shop_xor:
		case shop_shl: case shop_shr: case shop_sar: case shop_neg: case shop_not:
		case shop_seteq: case shop_setge: case shop_setgt: case shop_setae: case shop_setab: case shop_test:
		case shop_fsetgt: case shop_fseteq:
		case shop_fadd: case shop_fsub: case shop_fmul: case shop_fdiv: case shop_fabs: case shop_fneg:
		case shop_fipr: case shop_ftrv: case shop_cvt_f2i_t:
			break;
		default:
			throw Reject{ std::string("op nao suportada: ") + o.dissasm() };
		}
		if ((o.op == shop_shl || o.op == shop_shr || o.op == shop_sar) && !o.rs2.is_imm())
			throw Reject{ "deslocamento por registrador" };
		if (o.op == shop_readm && (o.flags & 0x7f) != 4 && (o.flags & 0x7f) != 8)
			throw Reject{ "readm de 1/2 bytes" };
		if (o.op == shop_writem && (o.flags & 0x7f) != 4 && (o.flags & 0x7f) != 8)
			throw Reject{ "writem de 1/2 bytes" };
		if (o.op == shop_readm && o.rd.is_r32f() == false && o.rd.is_r32i() == false && o.rd.count() != 2)
			throw Reject{ "readm destino" };
	}

	void allocate()
	{
		for (int i = 0; i < sh4_reg_count; i++)
		{
			home[i] = -1;
			homeCallee[i] = false;
		}
		static const int calleeGpr[] = { 19, 20, 21, 22, 23, 24, 25, 26 };
		static const int callerGpr[] = { 9, 10, 11, 12 };
		static const int calleeFp[] = { 8, 9, 10, 11, 12, 13, 14, 15 };
		int ng = 0, nr = 0, nf = 0, nrf = 0;
		std::vector<int> restFp;
		for (int i = 16; i < 32; i++)
			restFp.push_back(i);
		RegSet all = W | U;
		readonly = U & ~W;
		readonly.reset(reg_fpul);
		readonly.reset(reg_sr_T);
		home[reg_fpul] = 14;
		home[reg_sr_T] = 15;
		for (int r = 0; r < sh4_reg_count; r++)
		{
			if (!all.test(r) || r == reg_fpul || r == reg_sr_T)
				continue;
			if (is_fp(r))
				continue;
			if (W.test(r))
			{
				if (ng >= 8) throw Reject{ "GPR escritos demais" };
				home[r] = calleeGpr[ng++];
				homeCallee[r] = true;
			}
			else
			{
				if (nr >= 4) throw Reject{ "GPR so lidos demais" };
				home[r] = callerGpr[nr++];
			}
		}
		for (int r = reg_fr_0; r <= reg_fr_15; r++)
			if (W.test(r))
			{
				if (nf >= (int)restFp.size()) throw Reject{ "floats escritos demais" };
				home[r] = restFp[nf++];
			}
		for (int r = reg_fr_0; r <= reg_fr_15; r++)
			if (all.test(r) && !W.test(r))
			{
				if (nrf < 8)
				{
					home[r] = calleeFp[nrf++];
					homeCallee[r] = true;
				}
				else
				{
					if (nf >= (int)restFp.size()) throw Reject{ "floats so lidos demais" };
					home[r] = restFp[nf++];
				}
			}
	}

	RegSet exitUse() const
	{
		RegSet s = W;
		s.set(reg_sr_T);
		return s;
	}
	RegSet live_out(u32 v)
	{
		const BlockIn &b = blk[v];
		RegSet lo;
		std::vector<u32> tg;
		if (b.cond) { tg.push_back(b.branch); tg.push_back(b.next); }
		else tg.push_back(b.branch);
		for (u32 t : tg)
			lo |= blk.count(t) ? live_in[t] : exitUse();
		return lo;
	}
	void liveness()
	{
		for (u32 v : order)
			live_in[v].reset();
		bool changed = true;
		while (changed)
		{
			changed = false;
			for (auto it = order.rbegin(); it != order.rend(); ++it)
			{
				u32 v = *it;
				RegSet live = live_out(v);
				const std::vector<shil_opcode> &ir = blk[v].ir;
				for (auto o = ir.rbegin(); o != ir.rend(); ++o)
				{
					RegSet u, d;
					uses_defs(*o, u, d);
					live &= ~d;
					live |= u;
				}
				if (latch.count(v))
					live |= exitUse();
				if (live != live_in[v])
				{
					live_in[v] = live;
					changed = true;
				}
			}
		}
	}

	// ------------------------------------------------------------ emissao
	void FMovImm(const VRegister &d, u32 bits)
	{
		float f;
		memcpy(&f, &bits, 4);
		if (bits == 0)
			Fmov(d, wzr);
		else if (Assembler::IsImmFP32(f))
			Fmov(d, f);
		else
		{
			Mov(w0, bits);
			Fmov(d, w0);
		}
	}
	// operando inteiro de 32 bits em registrador
	Register GprSrc(const shil_param &p, const Register &scratch)
	{
		if (p.is_imm())
		{
			Mov(scratch, p._imm);
			return scratch;
		}
		if (is_fp(p._reg))
		{
			Fmov(scratch, S_(p._reg));
			return scratch;
		}
		return W_(p._reg);
	}
	VRegister FpSrc(const shil_param &p, const VRegister &scratch)
	{
		if (p.is_imm())
		{
			FMovImm(scratch, p._imm);
			return scratch;
		}
		return S_(p._reg);
	}
	Register Addr(const shil_opcode &o)
	{
		Register base = GprSrc(o.rs1, w0);
		const shil_param &off = o.op == shop_readm ? o.rs3 : o.rs3;
		if (off.is_null())
			return base;
		if (off.is_imm())
			Add(w0, base, Operand(off._imm));
		else
			Add(w0, base, W_(off._reg));
		return w0;
	}

	void sqCall(const Register &addr, const RegSet &liveAfter)
	{
		std::vector<int> spill;
		for (int r = 0; r < sh4_reg_count; r++)
			if (liveAfter.test(r) && home[r] >= 0 && !homeCallee[r] && !readonly.test(r))
				spill.push_back(r);
		for (int r : spill)
		{
			if (is_fp(r)) Str(S_(r), Ctx(r));
			else Str(W_(r), Ctx(r));
		}
		Mov(w0, addr);
		Sub(x9, x28, offsetof(Sh4RCB, cntx) - offsetof(Sh4RCB, do_sqw_nommu));
		Ldr(x9, MemOperand(x9));
		Sub(x1, x28, offsetof(Sh4RCB, cntx) - offsetof(Sh4RCB, sq_buffer));
		Blr(x9);
		Add(x13, x28, sizeof(Sh4Context));
		for (int r : spill)
		{
			if (is_fp(r)) Ldr(S_(r), Ctx(r));
			else Ldr(W_(r), Ctx(r));
		}
		for (int r = 0; r < sh4_reg_count; r++)
			if (readonly.test(r) && home[r] >= 0 && !homeCallee[r])
			{
				if (is_fp(r)) Ldr(S_(r), Ctx(r));
				else Ldr(W_(r), Ctx(r));
			}
		if (vec_xf)
			Ld1(v4.V4S(), v5.V4S(), v6.V4S(), v7.V4S(), MemOperand(x28, ctx_off(reg_xf_0)));
	}

	Label *exitStub(u32 pc, int tconst = -1)
	{
		Label *l = newLabel();
		cold.push_back([this, l, pc, tconst]() {
			Bind(l);
			for (int r = 0; r < sh4_reg_count; r++)
			{
				if (!W.test(r) || r == reg_sr_T)
					continue;
				if (is_fp(r)) Str(S_(r), Ctx(r));
				else Str(W_(r), Ctx(r));
			}
			if (tconst < 0)
				Str(w15, Ctx(reg_sr_T));
			else
			{
				Mov(w1, tconst);
				Str(w1, Ctx(reg_sr_T));
			}
			Mov(w29, pc);
			Str(w29, Ctx(reg_nextpc));
			BranchAbs(t2_no_update);
		});
		return l;
	}

	void op(const shil_opcode &o, const RegSet &liveAfter, bool sqLikely)
	{
		switch (o.op)
		{
		case shop_readm:
		{
			Register a = Addr(o);
			if (o.rd.count() == 2)
			{
				Add(x0, x13, Operand(a, UXTW));
				Ldp(S_(o.rd._reg), S_(o.rd._reg + 1), MemOperand(x0));
			}
			else if (is_fp(o.rd._reg))
				Ldr(S_(o.rd._reg), MemOperand(x13, a, UXTW));
			else
				Ldr(W_(o.rd._reg), MemOperand(x13, a, UXTW));
			break;
		}
		case shop_writem:
		{
			auto it = groupOf.find(&o);
			if (it == groupOf.end())
				throw Reject{ "writem fora de grupo" };
			XRegister ptr = XRegister(it->second.first);
			s32 rel = it->second.second;
			if (o.rs2.is_reg() && o.rs2.count() == 2)
				Stp(S_(o.rs2._reg), S_(o.rs2._reg + 1), MemOperand(ptr, rel));
			else if (o.rs2.is_reg() && is_fp(o.rs2._reg))
				Str(S_(o.rs2._reg), MemOperand(ptr, rel));
			else
				Str(GprSrc(o.rs2, w1), MemOperand(ptr, rel));
			break;
		}
		case shop_pref:
		{
			Register a = GprSrc(o.rs1, w2);
			Lsr(w0, a, 26);
			Cmp(w0, 0x38);
			Label *done = newLabel();
			if (sqLikely)
			{
				Label *ram = newLabel();
				B(ram, ne);
				sqCall(a, liveAfter);
				Bind(done);
				int acode = a.GetCode();
				cold.push_back([this, ram, done, acode]() {
					Bind(ram);
					Prfm(PLDL1KEEP, MemOperand(x13, Register::GetWRegFromCode(acode), UXTW));
					B(done);
				});
			}
			else
			{
				Label *sq = newLabel();
				B(sq, eq);
				Prfm(PLDL1KEEP, MemOperand(x13, a, UXTW));
				Bind(done);
				int acode = a.GetCode();
				RegSet la = liveAfter;
				cold.push_back([this, sq, done, acode, la]() {
					Bind(sq);
					sqCall(Register::GetWRegFromCode(acode), la);
					B(done);
				});
			}
			break;
		}
		case shop_jcond:
			if (tAfterJcond)
			{
				Mov(w16, w15);		// o slot muda T: guarda a decisao
				decision = w16;
			}
			else
				decision = w15;
			break;
		case shop_mov32:
			if (is_fp(o.rd._reg))
			{
				if (o.rs1.is_imm())
					FMovImm(S_(o.rd._reg), o.rs1._imm);
				else if (is_fp(o.rs1._reg))
					Fmov(S_(o.rd._reg), S_(o.rs1._reg));
				else
					Fmov(S_(o.rd._reg), W_(o.rs1._reg));
			}
			else
			{
				if (o.rs1.is_imm())
					Mov(W_(o.rd._reg), o.rs1._imm);
				else if (is_fp(o.rs1._reg))
					Fmov(W_(o.rd._reg), S_(o.rs1._reg));
				else
					Mov(W_(o.rd._reg), W_(o.rs1._reg));
			}
			break;
		case shop_add: case shop_sub: case shop_and: case shop_or: case shop_xor:
		{
			Register d = W_(o.rd._reg);
			Register a = GprSrc(o.rs1, w0);
			Operand b = o.rs2.is_imm() ? Operand(o.rs2._imm) : Operand(GprSrc(o.rs2, w1));
			switch (o.op)
			{
			case shop_add: Add(d, a, b); break;
			case shop_sub: Sub(d, a, b); break;
			case shop_and: And(d, a, b); break;
			case shop_or: Orr(d, a, b); break;
			default: Eor(d, a, b); break;
			}
			break;
		}
		case shop_shl: Lsl(W_(o.rd._reg), GprSrc(o.rs1, w0), o.rs2._imm & 31); break;
		case shop_shr: Lsr(W_(o.rd._reg), GprSrc(o.rs1, w0), o.rs2._imm & 31); break;
		case shop_sar: Asr(W_(o.rd._reg), GprSrc(o.rs1, w0), o.rs2._imm & 31); break;
		case shop_neg: Neg(W_(o.rd._reg), GprSrc(o.rs1, w0)); break;
		case shop_not: Mvn(W_(o.rd._reg), GprSrc(o.rs1, w0)); break;
		case shop_test:
		case shop_seteq: case shop_setge: case shop_setgt: case shop_setae: case shop_setab:
		{
			Register a = GprSrc(o.rs1, w0);
			Operand b = o.rs2.is_imm() ? Operand(o.rs2._imm) : Operand(GprSrc(o.rs2, w1));
			if (o.op == shop_test)
				Tst(a, b);
			else
				Cmp(a, b);
			Condition c = o.op == shop_test || o.op == shop_seteq ? eq : o.op == shop_setge ? ge
					: o.op == shop_setgt ? gt : o.op == shop_setae ? hs : hi;
			Cset(w15, c);
			break;
		}
		case shop_fsetgt:
		case shop_fseteq:
			Fcmp(S_(o.rs1._reg), S_(o.rs2._reg));
			Cset(w15, o.op == shop_fsetgt ? gt : eq);
			break;
		case shop_fadd: case shop_fsub: case shop_fmul: case shop_fdiv:
		{
			VRegister d = S_(o.rd._reg);
			VRegister a = FpSrc(o.rs1, s0);
			VRegister b = FpSrc(o.rs2, s1);
			switch (o.op)
			{
			case shop_fadd: Fadd(d, a, b); break;
			case shop_fsub: Fsub(d, a, b); break;
			case shop_fmul: Fmul(d, a, b); break;
			default: Fdiv(d, a, b); break;
			}
			break;
		}
		case shop_fabs: Fabs(S_(o.rd._reg), S_(o.rs1._reg)); break;
		case shop_fneg: Fneg(S_(o.rd._reg), S_(o.rs1._reg)); break;
		case shop_cvt_f2i_t: Fcvtzs(W_(o.rd._reg), S_(o.rs1._reg)); break;
		case shop_fipr:
		{
			int a = o.rs1._reg, b = o.rs2._reg;
			// (a0*b0 + a1*b1) + (a2*b2 + a3*b3): ordem do JIT antigo (fmul + faddp)
			Fmul(s0, S_(a), S_(b));
			Fmul(s1, S_(a + 1), S_(b + 1));
			Fadd(s0, s0, s1);
			Fmul(s1, S_(a + 2), S_(b + 2));
			Fmul(s2, S_(a + 3), S_(b + 3));
			Fadd(s1, s1, s2);
			Fadd(S_(o.rd._reg), s0, s1);
			break;
		}
		case shop_ftrv:
		{
			int v = o.rs1._reg, d = o.rd._reg;
			Fmul(v0.V4S(), v4.V4S(), S_(v), 0);
			Fmla(v0.V4S(), v5.V4S(), S_(v + 1), 0);
			Fmla(v0.V4S(), v6.V4S(), S_(v + 2), 0);
			Fmla(v0.V4S(), v7.V4S(), S_(v + 3), 0);
			for (int i = 0; i < 4; i++)
				Mov(S_(d + i), v0.V4S(), i);
			break;
		}
		default:
			throw Reject{ "sem emissor" };
		}
	}

	// grupos de store de um bloco: mesma base, so somada de constantes
	struct Group { int reg; s32 lo; u32 span; std::vector<std::pair<const shil_opcode *, s32>> ops; };
	std::vector<Group> storeGroups(const BlockIn &b)
	{
		std::map<int, s32> delta;
		std::map<int, bool> known;
		std::map<int, Group> found;
		int callsBefore = 0;
		for (const shil_opcode &o : b.ir)
		{
			if (o.op == shop_writem)
			{
				if (!o.rs1.is_reg() || !o.rs3.is_null() || callsBefore)
					throw Reject{ "writem fora do padrao do grupo" };
				int x = o.rs1._reg;
				if (known.count(x) && !known[x])
					throw Reject{ "writem com base desconhecida" };
				s32 d = delta.count(x) ? delta[x] : 0;
				u32 size = o.flags & 0x7f;
				Group &g = found[x];
				g.reg = x;
				g.ops.push_back({ &o, d });
				if (g.ops.size() == 1) { g.lo = d; g.span = 0; }
				s32 lo = std::min(g.lo, d);
				s32 hi = std::max((s32)(g.lo + g.span), (s32)(d + size));
				g.lo = lo;
				g.span = hi - lo;
			}
			if (o.op == shop_pref)
				callsBefore++;
			RegSet u, dd;
			uses_defs(o, u, dd);
			for (int x = 0; x < sh4_reg_count; x++)
			{
				if (!dd.test(x))
					continue;
				bool constAdd = (o.op == shop_add || o.op == shop_sub) && o.rs1.is_reg() && o.rs1._reg == x
						&& o.rs2.is_imm() && (!known.count(x) || known[x]);
				if (constAdd)
				{
					s32 k = (s32)o.rs2._imm;
					delta[x] = (delta.count(x) ? delta[x] : 0) + (o.op == shop_add ? k : -k);
					known[x] = true;
				}
				else
					known[x] = false;
			}
		}
		std::vector<Group> out;
		for (auto &kv : found)
			out.push_back(kv.second);
		if (out.size() > 2)
			throw Reject{ "mais de 2 grupos de store" };
		return out;
	}

	std::vector<u32> layout()
	{
		std::set<u32> seen;
		std::vector<u32> lay;
		auto chain = [&](u32 v) {
			while (blk.count(v) && !seen.count(v))
			{
				seen.insert(v);
				lay.push_back(v);
				const BlockIn &b = blk[v];
				u32 nx = 0;
				bool found = false;
				std::vector<u32> cand;
				if (b.cond) { cand.push_back(b.next); cand.push_back(b.branch); }
				else cand.push_back(b.branch);
				for (u32 c : cand)
					if (blk.count(c) && !seen.count(c)) { nx = c; found = true; break; }
				if (!found)
					break;
				v = nx;
			}
		};
		for (u32 l : latch)
			for (u32 s : blk[l].succ)
				chain(s);
		for (u32 e : entries)
			chain(e);
		for (u32 v : order)
			chain(v);
		return lay;
	}

	void compile()
	{
		for (u32 v : order)
			blockLabel[v] = newLabel();
		std::map<u32, Label *> resume;
		// entradas
		for (u32 e : entries)
		{
			Label *el = newLabel();
			entryLabel[e] = el;
			Bind(el);
			Add(x13, x28, sizeof(Sh4Context));
			RegSet all = W | U;
			for (int r = 0; r < sh4_reg_count; r++)
				if (all.test(r) && home[r] >= 0)
				{
					if (is_fp(r)) Ldr(S_(r), Ctx(r));
					else Ldr(W_(r), Ctx(r));
				}
			if (vec_xf)
				Ld1(v4.V4S(), v5.V4S(), v6.V4S(), v7.V4S(), MemOperand(x28, ctx_off(reg_xf_0)));
			Label *rl = newLabel();
			Cmp(w27, guard[e]);
			B(rl, lt);
			B(blockLabel[e]);
			u32 *code = entryCode[e];
			cold.push_back([this, rl, code]() {
				// refaz o `subs w27, w27, #ciclos` trocado pelo gancho
				Bind(rl);
				Subs(w27, w27, (code[0] >> 10) & 0xFFF);
				BranchAbs(CC_RW2RX((void *)(code + 1)));
			});
		}
		std::vector<u32> lay = layout();
		for (size_t i = 0; i < lay.size(); i++)
		{
			u32 v = lay[i];
			const BlockIn &b = blk[v];
			Bind(blockLabel[v]);
			if (latch.count(v))
			{
				Cmp(w27, guard[v]);
				B(exitStub(v), lt);
			}
			groupOf.clear();
			std::vector<Group> groups = storeGroups(b);
			for (size_t gi = 0; gi < groups.size(); gi++)
			{
				const Group &g = groups[gi];
				int ptr = 6 + (int)gi;
				if (g.span > 64)
					throw Reject{ "grupo de store maior que a SQ" };
				Add(w1, W_(g.reg), Operand(g.lo));
				Label *xs = exitStub(v);
				Lsr(w0, w1, 26);
				Cmp(w0, 0x38);
				B(xs, ne);
				And(w2, w1, 0x3f);
				Cmp(w2, 64 - g.span);
				B(xs, hi);
				Sub(XRegister(ptr), x28, offsetof(Sh4RCB, cntx) - offsetof(Sh4RCB, sq_buffer));
				Add(XRegister(ptr), XRegister(ptr), x2);
				for (auto &p : g.ops)
					groupOf[p.first] = { ptr, p.second - g.lo };
			}
			Sub(w27, w27, b.cycles);
			// liveness dentro do bloco
			std::vector<RegSet> after(b.ir.size());
			RegSet live = live_out(v);
			for (int j = (int)b.ir.size() - 1; j >= 0; j--)
			{
				after[j] = live;
				RegSet u, d;
				uses_defs(b.ir[j], u, d);
				live &= ~d;
				live |= u;
			}
			int jc = -1;
			for (size_t j = 0; j < b.ir.size(); j++)
				if (b.ir[j].op == shop_jcond) { jc = (int)j; break; }
			tAfterJcond = false;
			if (jc >= 0)
				for (size_t j = jc + 1; j < b.ir.size(); j++)
				{
					RegSet u, d;
					uses_defs(b.ir[j], u, d);
					if (d.test(reg_sr_T)) tAfterJcond = true;
				}
			decision = w15;
			for (size_t j = 0; j < b.ir.size(); j++)
			{
				const shil_opcode &o = b.ir[j];
				bool sqLikely = false;
				if (o.op == shop_pref && o.rs1.is_reg())
					for (u32 vv : order)
						for (const shil_opcode &p : blk[vv].ir)
							if (p.op == shop_writem && p.rs1.is_reg() && p.rs1._reg == o.rs1._reg)
								sqLikely = true;
				op(o, after[j], sqLikely);
			}
			u32 nxt = i + 1 < lay.size() ? lay[i + 1] : 0xFFFFFFFF;
			if (!b.cond)
			{
				if (blk.count(b.branch))
				{
					if (b.branch != nxt)
						B(blockLabel[b.branch]);
				}
				else
					B(exitStub(b.branch));
			}
			else
			{
				u32 taken = b.takenOnT ? b.branch : b.next;	// destino quando a decisao == 1
				u32 other = b.takenOnT ? b.next : b.branch;
				int tc1 = tAfterJcond ? -1 : 1, tc0 = tAfterJcond ? -1 : 0;
				Label *lt_ = blk.count(taken) ? blockLabel[taken] : exitStub(taken, tc1);
				Label *lo_ = blk.count(other) ? blockLabel[other] : exitStub(other, tc0);
				if (other == nxt)
					Cbnz(decision, lt_);
				else if (taken == nxt)
					Cbz(decision, lo_);
				else
				{
					Cbnz(decision, lt_);
					B(lo_);
				}
			}
		}
		for (size_t i = 0; i < cold.size(); i++)	// cold pode crescer? nao: stubs nao geram stubs
			cold[i]();
		FinalizeCode();
	}
};

void read_cfg()
{
	cfg_read = true;
	const char *e = getenv("FC_TIER2_RT");
	if (e == nullptr)
		return;
	std::string s(e);
	size_t slash = s.find('/');
	auto parse = [](const std::string &t, std::vector<u32> &out) {
		size_t p = 0;
		while (p < t.size())
		{
			size_t c = t.find(',', p);
			if (c == std::string::npos) c = t.size();
			out.push_back(strtoul(t.substr(p, c - p).c_str(), nullptr, 16));
			p = c + 1;
		}
	};
	if (slash == std::string::npos)
		return;
	parse(s.substr(0, slash), cfgEntries);
	parse(s.substr(slash + 1), cfgBlocks);
	for (u32 en : cfgEntries)
		if (std::find(cfgBlocks.begin(), cfgBlocks.end(), en) == cfgBlocks.end())
			cfgBlocks.insert(cfgBlocks.begin(), en);
}

u8 *area_begin() { return CodeCache + CODE_SIZE - T2_AREA; }
u8 *area_end() { return CodeCache + CODE_SIZE; }

bool try_install(const std::vector<u32> &entryList, const std::vector<u32> &blockList)
{
	for (u32 va : blockList)
		if (bm_GetBlock(va).get() == nullptr)
			return false;		// ainda nao compilado: tenta de novo depois
	size_t room = area_end() - t2_ptr;
	if (room < 16 * 1024)
		return false;
	Tier2Compiler *c = new Tier2Compiler(t2_ptr, room);
	Region reg;
	try
	{
		c->load(blockList, entryList);
		c->compile();
	}
	catch (const Reject &r)
	{
		WARN_LOG(DYNAREC, "tier2: regiao recusada: %s", r.why.c_str());
		fprintf(stderr, "tier2: regiao recusada: %s\n", r.why.c_str());
		delete c;
		gaveUp = true;
		return false;
	}
	u32 size = c->GetBuffer()->GetSizeInBytes();
	vmem_platform_flush_cache(CC_RW2RX(t2_ptr), CC_RW2RX(t2_ptr + size), t2_ptr, t2_ptr + size);
	for (u32 e : c->entries)
	{
		u32 *code = c->entryCode[e];
		u8 *target = t2_ptr + c->entryLabel[e]->GetLocation();
		ptrdiff_t off = (u8 *)CC_RW2RX(target) - (u8 *)CC_RW2RX((void *)code);
		verify(off >= -128 * 1024 * 1024 && off < 128 * 1024 * 1024);
		reg.patches.push_back({ code, code[0] });
		code[0] = 0x14000000 | ((u32)(off >> 2) & 0x03FFFFFF);
		vmem_platform_flush_cache(CC_RW2RX((void *)code), CC_RW2RX((void *)(code + 1)), code, code + 1);
	}
	reg.blocks = blockList;
	reg.alive = true;
	regions.push_back(reg);
	fprintf(stderr, "tier2: regiao de %zu blocos ligada (%u bytes, entradas %zu, guardas:",
			blockList.size(), size, c->entries.size());
	for (auto &kv : c->guard)
		fprintf(stderr, " %08X=%u", kv.first, kv.second);
	fprintf(stderr, ")\n");
	t2_ptr += (size + 63) & ~63u;
	delete c;
	return true;
}

} // namespace

void tier2_init()
{
	if (!cfg_read)
		read_cfg();
	tier2_code_reserve = cfgBlocks.empty() ? 0 : T2_AREA;
}

void tier2_reset()
{
	if (!cfg_read)
		read_cfg();
	regions.clear();
	t2_ptr = area_begin();
	gaveUp = false;
	tier2_poll = !cfgBlocks.empty() && !mmu_enabled();
}

void tier2_safe_point()
{
	if (++pollCount & 255)
		return;
	if (gaveUp || cfgBlocks.empty())
	{
		tier2_poll = false;
		return;
	}
	for (const Region &r : regions)
		if (r.alive)
		{
			tier2_poll = false;
			return;
		}
	if (try_install(cfgEntries, cfgBlocks))
		tier2_poll = false;
}

void tier2_on_discard(RuntimeBlockInfo *block)
{
	if (regions.empty())
		return;
	for (Region &r : regions)
	{
		if (!r.alive || std::find(r.blocks.begin(), r.blocks.end(), block->vaddr) == r.blocks.end())
			continue;
		for (const Patch &p : r.patches)
		{
			p.code[0] = p.orig;
			vmem_platform_flush_cache(CC_RW2RX((void *)p.code), CC_RW2RX((void *)(p.code + 1)), p.code, p.code + 1);
		}
		r.alive = false;
		tier2_poll = !cfgBlocks.empty();
	}
}

bool tier2_owns_pc(uintptr_t pc)
{
	return !cfgBlocks.empty() && pc >= (uintptr_t)CC_RW2RX(area_begin()) && pc < (uintptr_t)CC_RW2RX(area_end());
}

#endif // FEAT_SHREC == DYNAREC_JIT
