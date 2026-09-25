#!/usr/bin/env python3
"""Gerador offline do nivel 2: regiao de blocos do dump (FC_JIT_DUMP) -> ARM64.

Uso: tier2_gen.py jit-<pid>.txt saida.S ENTRADA VADDR [VADDR...]

Le o SHIL de cada bloco da regiao (registros B/G/O do dump) e gera um .S com
a interface do harness de tools/proto_jit_armv8_a (new_run(ctx, ciclos) ->
pc de saida; g_cycles com os ciclos restantes), comecando no bloco ENTRADA.
Op fora do conjunto suportado: recusa a regiao (no sistema real ela fica no
JIT antigo).

Regras da v1 (tools/proto_jit_armv8_a/README.md, regiao do DOA2):
  ciclos    guarda nas entradas da regiao e no inicio dos blocos com volta de
            laco (caminho mais longo ate a proxima guarda); cada bloco original
            subtrai os seus; guarda falhou -> sai antes de executar
  registr.  SH4 escritos na regiao em callee-saved (w19-w26; floats s16-s31
            caller-saved); so lidos: GPR em caller-saved recarregados depois de
            chamada, floats em s8-s15, XMTRX (so via ftrv) em v4-v7
  chamada   descarga da SQ = chamada C completa: spill so do que esta vivo
            depois dela (as saidas contam como uso de tudo que a regiao
            escreve), reload do que e usado
  pref      sobe no bloco ate depois da ultima escrita/definicao do endereco
            (a descarga da SQ so le a propria SQ)
  layout    sucessor mais executado (contadores C) logo em seguida
Ainda nao: T fundido em flags, registrador derivado, stores agrupados, blocos
equivalentes compartilhados.
"""
import collections
import re
import struct
import sys

sys.path.insert(0, __file__.rsplit('/', 1)[0])
from region_study import branch_info  # noqa: E402

# offsets no Sh4Context (sh4_if.h): xf 0-63, fr 64-127, r 128-191
CTX_FPUL = 260
CTX_T = 276
CALLEE_GPR = ['w19', 'w20', 'w21', 'w22', 'w23', 'w24', 'w25', 'w26']
CALLER_GPR = ['w9', 'w10', 'w11', 'w12', 'w13']
CALLEE_FP = ['s8', 's9', 's10', 's11', 's12', 's13', 's14', 's15']
CALLER_FP = ['s%d' % i for i in range(16, 32)]


class Reject(Exception):
    pass


def is_fr(x):
    return x[0] == 'f' and x[1:].isdigit()


def is_xf(x):
    return is_fr(x) and int(x[1:]) >= 16


def is_gpr(x):
    return x[0] == 'r' and x[1:].isdigit()


def ctx_off(var):
    if var == 'fpul':
        return CTX_FPUL
    if var == 'T':
        return CTX_T
    if var[0] == 'r':
        return 128 + 4 * int(var[1:])
    if var[0] == 'f':
        n = int(var[1:])
        return 64 + 4 * n if n < 16 else 4 * (n - 16)
    if var == 'fpul':
        return CTX_FPUL
    if var == 'T':
        return CTX_T
    raise Reject('sem offset: ' + var)


# ---------------------------------------------------------------- parse
OPND = re.compile(r'^(?:(r|f)(\d+)(?:v(\d+))?|(sr\.T)|(fpul)|(fpscr)|(pc_dyn))\.\d+$')


def parse_opnd(s):
    s = s.strip()
    m = OPND.match(s)
    if m:
        if m.group(1):
            n = int(m.group(2))
            cnt = int(m.group(3)) if m.group(3) else 1
            return ('reg', ['%s%d' % (m.group(1), n + i) for i in range(cnt)])
        if m.group(4):
            return ('reg', ['T'])
        if m.group(5):
            return ('reg', ['fpul'])
        if m.group(6):
            return ('fpscr', None)
        return ('pcdyn', None)
    v = int(s, 16) if s.startswith('0x') or s.startswith('-0x') else int(s)
    return ('imm', v & 0xFFFFFFFF)


def parse_op(text):
    name, _, rest = text.partition(' ')
    rd = None
    srcs = []
    if '<-' in rest:
        left, _, right = rest.partition('<-')
        if left.strip():
            rd = parse_opnd(left)
        srcs = [parse_opnd(x) for x in right.split(',') if x.strip()]
    return {'op': name, 'rd': rd, 'src': srcs, 'text': text}


def load_region(dump, want):
    blocks = {}
    cond = {}
    runs = collections.Counter()
    cur = None
    for l in open(dump, errors='replace'):
        t = l[0]
        if t == 'B':
            p = l.split()
            cur = p[2] if p[2] in want and p[2] not in blocks else None
            if cur:
                blocks[cur] = {'host': p[1], 'bytes': int(p[4]), 'cycles': int(p[6]), 'ops': []}
        elif cur and t == 'G':
            blocks[cur]['sh4'] = [int(x, 16) for x in l.split()[1:]]
        elif cur and t == 'O':
            p = l.split(None, 8)
            blocks[cur]['ops'].append(parse_op(p[8].strip()))
        elif t == 'C':
            p = l.split()
            if p[2] in blocks and p[1] == blocks[p[2]]['host']:
                cond[p[2]] = (int(p[3]), int(p[4]))
        elif t == 'R':
            p = l.split()
            if p[2] in blocks and p[1] == blocks[p[2]]['host']:
                runs[p[2]] = int(p[3])
    for va in want:
        if va in blocks:
            blocks[va]['runs'] = runs[va]
    for va in want:
        if va not in blocks:
            raise Reject('bloco %s nao esta no dump' % va)
    for va, b in blocks.items():
        v = int(va, 16)
        k, tgt = branch_info(v, b['sh4'])
        nxt = '%08X' % (v + b['bytes'])
        if k == 'cond':
            i = len(b['sh4']) - 2 if (b['sh4'][-2] >> 8) in (0x89, 0x8B, 0x8D, 0x8F) else len(b['sh4']) - 1
            op = b['sh4'][i] >> 8
            b['end'] = ('cond', op in (0x89, 0x8D), '%08X' % tgt, nxt)	# (desvia se T==1?, destino, proximo)
            b['counts'] = cond.get(va, (1, 1))
        elif k == 'bra':
            b['end'] = ('jump', '%08X' % tgt)
        elif k == 'queda':
            b['end'] = ('jump', nxt)
        else:
            raise Reject('%s: fim de bloco %s (dinamico) nao suportado' % (va, k))
    return blocks


# ---------------------------------------------------------------- semantica
FP_BIN = {'fadd': 'fadd', 'fsub': 'fsub', 'fmul': 'fmul', 'fdiv': 'fdiv'}
INT_BIN = {'add': 'add', 'sub': 'sub', 'and': 'and', 'or': 'orr', 'xor': 'eor'}
SHIFT = {'shl': 'lsl', 'shr': 'lsr', 'sar': 'asr'}
SETCC = {'seteq': 'eq', 'setge': 'ge', 'setgt': 'gt', 'setae': 'hs', 'setab': 'hi'}
SUPPORTED = set(FP_BIN) | set(INT_BIN) | set(SHIFT) | set(SETCC) | {
    'readm', 'writem', 'pref', 'jcond', 'mov32', 'test', 'fsetgt', 'fseteq', 'fipr', 'ftrv',
    'cvt_f2i_t', 'fabs', 'fneg', 'neg', 'not'}


def uses_defs(o):
    """(usos, definicoes) em variaveis do SH4."""
    u, d = [], []
    for s in o['src']:
        if s[0] == 'reg':
            u += s[1]
    if o['rd'] and o['rd'][0] == 'reg':
        d += o['rd'][1]
    if o['op'] == 'jcond':
        d = []
    return u, d


def is_sq_call(o):
    return o['op'] == 'pref'


# ---------------------------------------------------------------- gerador
class Gen:
    def __init__(self, blocks, order, entry):
        self.B = blocks
        self.order = order		# vaddrs da regiao
        self.entry = entry
        self.out = []
        self.cold = []
        self.lab = 0

    def label(self, p='L'):
        self.lab += 1
        return '.L%s%d' % (p, self.lab)

    def e(self, s):
        self.out.append('\t' + s)

    # -- analise
    def analyse(self):
        B, R = self.B, set(self.order)
        for va, b in B.items():
            for o in b['ops']:
                if o['op'] not in SUPPORTED:
                    if o['op'] == 'xor' and o['rd'] and o['rd'][0] == 'fpscr':
                        continue
                    raise Reject('%s: op nao suportada: %s' % (va, o['text']))
            nx = sum(1 for o in b['ops'] if o['op'] == 'xor' and o['rd'] and o['rd'][0] == 'fpscr')
            if nx % 2:
                raise Reject('%s: fschg impar (SZ muda na saida)' % va)
            # pref sobe ate depois da ultima escrita / definicao do endereco
            ops = [o for o in b['ops'] if not (o['op'] == 'xor' and o['rd'] and o['rd'][0] == 'fpscr')]
            for i in range(len(ops)):
                if ops[i]['op'] != 'pref':
                    continue
                addr = ops[i]['src'][0][1][0] if ops[i]['src'][0][0] == 'reg' else None
                j = i
                while j > 0:
                    p = ops[j - 1]
                    _, d = uses_defs(p)
                    if p['op'] in ('writem', 'pref', 'jcond') or (addr in d):
                        break
                    j -= 1
                ops.insert(j, ops.pop(i))
            b['ir'] = ops
        # sucessores dentro da regiao
        self.succ = {}
        for va, b in B.items():
            e = b['end']
            tg = [e[2], e[3]] if e[0] == 'cond' else [e[1]]
            self.succ[va] = [t for t in tg if t in R]
        preds = collections.defaultdict(set)
        for va, ss in self.succ.items():
            for s in ss:
                preds[s].add(va)
        # entradas: a pedida + blocos sem predecessor na regiao
        self.entries = [self.entry] + [v for v in self.order if v != self.entry and not preds[v]]
        # volta de laco (DFS a partir das entradas): a origem ganha guarda
        self.latch = set()
        color = {}

        def dfs(v):
            color[v] = 1
            for s in self.succ[v]:
                if color.get(s) == 1:
                    self.latch.add(v)
                elif s not in color:
                    dfs(s)
            color[v] = 2
        for en in self.entries:
            if en not in color:
                dfs(en)
        # a guarda da volta sobe para o predecessor unico que so leva ao bloco
        # (fica antes das chamadas do predecessor: falhar la nao exige estado
        # depois da chamada)
        moved = set()
        for g in sorted(self.latch):
            while g not in self.entries and len(preds[g]) == 1:
                p = next(iter(preds[g]))
                if self.succ[p] != [g] or p in moved or p == g:
                    break
                g = p
            moved.add(g)
        self.latch = moved
        # valor das guardas: caminho mais longo ate a proxima guarda interna
        memo = {}

        def longest(v):
            if v in memo:
                return memo[v]
            memo[v] = 0
            best = 0
            for s in self.succ[v]:
                if s in self.latch:
                    continue
                best = max(best, longest(s))
            memo[v] = B[v]['cycles'] + best
            return memo[v]
        self.guard = {}
        for v in set(self.entries) | self.latch:
            memo.clear()
            if v in self.latch:
                best = max([longest(s) for s in self.succ[v] if s not in self.latch] or [0])
                self.guard[v] = B[v]['cycles'] + best
            else:
                self.guard[v] = longest(v)
        # variaveis
        W, U = set(), set()
        self.vec_xf = False
        for va in self.order:
            for o in B[va]['ir']:
                u, d = uses_defs(o)
                if o['op'] == 'ftrv':
                    if any(is_xf(x) for x in d):
                        raise Reject('ftrv escrevendo xf')
                    self.vec_xf = True
                    u = [x for x in u if not is_xf(x)]
                W |= set(d)
                U |= set(u)
        for x in W | U:
            if is_xf(x):
                raise Reject('xf usado fora do ftrv: ' + x)
        self.W, self.U = W, U
        # alocacao
        home = {}
        wg = sorted([x for x in W if is_gpr(x)], key=lambda x: int(x[1:]))
        rg = sorted([x for x in U - W if is_gpr(x)], key=lambda x: int(x[1:]))
        wf = sorted([x for x in W if is_fr(x)], key=lambda x: int(x[1:]))
        rf = sorted([x for x in U - W if is_fr(x)], key=lambda x: int(x[1:]))
        if len(wg) > len(CALLEE_GPR):
            raise Reject('GPR escritos demais: %s' % wg)
        for x, h in zip(wg, CALLEE_GPR):
            home[x] = h
        if len(rg) > len(CALLER_GPR):
            raise Reject('GPR so lidos demais')
        for x, h in zip(rg, CALLER_GPR):
            home[x] = h
        home['fpul'] = 'w14'
        home['T'] = 'w15'
        if len(wf) > len(CALLER_FP):
            raise Reject('floats escritos demais')
        for x, h in zip(wf, CALLER_FP):
            home[x] = h
        rest = CALLER_FP[len(wf):]
        if len(rf) > len(CALLEE_FP) + len(rest):
            raise Reject('floats so lidos demais')
        for x, h in zip(rf, CALLEE_FP + rest):
            home[x] = h
        self.home = home
        self.callee = set(CALLEE_GPR) | set(CALLEE_FP)
        self.readonly = (U - W) - {'fpul', 'T'}
        # liveness por bloco (saida conta como uso de tudo que a regiao escreve)
        live_in = {v: set() for v in self.order}
        changed = True
        while changed:
            changed = False
            for v in reversed(self.order):
                lo = self.live_out(v, live_in)
                li = self.transfer(v, lo)
                if li != live_in[v]:
                    live_in[v] = li
                    changed = True
        self.live_in = live_in

    def live_out(self, v, live_in):
        b = self.B[v]
        e = b['end']
        tg = [e[2], e[3]] if e[0] == 'cond' else [e[1]]
        lo = set()
        for t in tg:
            lo |= live_in[t] if t in live_in else (self.W | {'T'})
        if v in self.latch:
            pass
        return lo

    def transfer(self, v, lo):
        live = set(lo)
        for o in reversed(self.B[v]['ir']):
            u, d = uses_defs(o)
            live -= set(d)
            live |= set(u)
        if v in self.latch:		# guarda de laco falhando sai com tudo
            live |= self.W | {'T'}
        return live

    # -- emissao de operandos
    def R(self, var):
        return self.home[var]

    def src_gpr(self, s, scratch='w0'):
        if s[0] == 'imm':
            self.movimm(scratch, s[1])
            return scratch
        v = s[1][0]
        if is_fr(v):
            self.e('fmov %s, %s' % (scratch, self.R(v)))
            return scratch
        return self.R(v)

    def movimm(self, reg, v):
        v &= 0xFFFFFFFF
        if v == 0:
            self.e('mov %s, wzr' % reg)
        elif v < 0x10000:
            self.e('mov %s, #0x%x' % (reg, v))
        elif (v & 0xFFFF) == 0:
            self.e('mov %s, #0x%x' % (reg, v))
        else:
            self.e('mov %s, #0x%x' % (reg, v & 0xFFFF))
            self.e('movk %s, #0x%x, lsl #16' % (reg, v >> 16))

    def fimm(self, reg, bits):
        f = struct.unpack('<f', struct.pack('<I', bits))[0]
        if bits == 0:
            self.e('fmov %s, wzr' % reg)
            return
        # fmov imediato: +-(16..31)/16 * 2^(-3..4)
        for e_ in range(-3, 5):
            for m in range(16, 32):
                for sg in (1, -1):
                    if sg * m / 16.0 * 2.0 ** e_ == f:
                        self.e('fmov %s, #%r' % (reg, f))
                        return
        self.movimm('w0', bits)
        self.e('fmov %s, w0' % reg)

    def addimm(self, rd, rs, v, op='add'):
        v &= 0xFFFFFFFF
        if v >= 0x80000000:
            v = (1 << 32) - v
            op = 'sub' if op == 'add' else 'add'
        if v < 4096:
            self.e('%s %s, %s, #%d' % (op, rd, rs, v))
        else:
            self.movimm('w1', v)
            self.e('%s %s, %s, w1' % (op, rd, rs))

    def addr(self, o, scratch='w0'):
        """registrador com o endereco de readm/writem (rs1 [+ rs3])."""
        base = o['src'][0]
        if o['op'] == 'readm':
            off = o['src'][1] if len(o['src']) > 1 else None
        else:
            off = o['src'][2] if len(o['src']) > 2 else None
        b = self.src_gpr(base, 'w0')
        if off is None:
            return b
        if off[0] == 'imm':
            self.addimm(scratch, b, off[1])
        else:
            self.e('add %s, %s, %s' % (scratch, b, self.R(off[1][0])))
        return scratch

    # -- chamada da SQ (C completa)
    def sq_call(self, addr_reg, live_after):
        spill = [x for x in sorted(live_after) if x in self.home and self.home[x] not in self.callee
                 and x not in self.readonly]
        for x in spill:
            self.e('str %s, [x28, #%d]' % (self.R(x), ctx_off(x)))
        self.e('mov w0, %s' % addr_reg)
        self.e('bl sq_stub')
        for x in spill:
            self.e('ldr %s, [x28, #%d]' % (self.R(x), ctx_off(x)))
        for x in sorted(self.readonly):
            if self.home[x] not in self.callee:
                self.e('ldr %s, [x28, #%d]' % (self.R(x), ctx_off(x)))
        if self.vec_xf:
            self.e('ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x28]')

    # -- uma op
    def op(self, o, live_after, sq_likely):
        n = o['op']
        rd, src = o['rd'], o['src']
        if n == 'xor':		# fschg em par: sem efeito liquido
            return
        if n == 'readm':
            dv = rd[1]
            a = self.addr(o)
            if len(dv) == 2:
                self.e('add x0, x27, %s, uxtw' % a)
                self.e('ldp %s, %s, [x0]' % (self.R(dv[0]), self.R(dv[1])))
            elif is_fr(dv[0]):
                self.e('ldr %s, [x27, %s, uxtw]' % (self.R(dv[0]), a))
            else:
                self.e('ldr %s, [x27, %s, uxtw]' % (self.R(dv[0]), a))
            return
        if n == 'writem':
            a = self.addr(o)
            data = src[1]
            if data[0] == 'reg' and len(data[1]) == 2:
                self.e('add x0, x27, %s, uxtw' % a)
                self.e('stp %s, %s, [x0]' % (self.R(data[1][0]), self.R(data[1][1])))
            elif data[0] == 'reg' and is_fr(data[1][0]):
                self.e('str %s, [x27, %s, uxtw]' % (self.R(data[1][0]), a))
            else:
                d = self.src_gpr(data, 'w1')
                self.e('str %s, [x27, %s, uxtw]' % (d, a))
            return
        if n == 'pref':
            a = self.src_gpr(src[0], 'w2')
            sq, done = self.label('sq'), self.label('pd')
            self.e('lsr w0, %s, #26' % a)
            self.e('cmp w0, #0x38')
            if sq_likely:
                ram = self.label('ram')
                self.e('b.ne %s' % ram)
                self.sq_call(a, live_after)
                self.out.append('%s:' % done)
                # caminho de RAM fora da linha
                self.cold += ['%s:' % ram, '\tprfm pldl1keep, [x27, %s, uxtw]' % a, '\tb %s' % done]
            else:
                self.e('b.eq %s' % sq)
                self.e('prfm pldl1keep, [x27, %s, uxtw]' % a)
                self.out.append('%s:' % done)
                keep = self.out
                self.out = []
                self.out.append('%s:' % sq)
                self.sq_call(a, live_after)
                self.e('b %s' % done)
                self.cold += self.out
                self.out = keep
            return
        if n == 'jcond':
            if self.t_after_jcond:	# o slot muda T: guarda a decisao
                self.e('mov w16, %s' % self.R('T'))
                self.decision = 'w16'
            else:
                self.decision = self.R('T')
            return
        if n == 'mov32':
            d, s = rd[1][0], src[0]
            if is_fr(d):
                if s[0] == 'imm':
                    self.fimm(self.R(d), s[1])
                else:
                    self.e('fmov %s, %s' % (self.R(d), self.R(s[1][0])))
            else:
                if s[0] == 'imm':
                    self.movimm(self.R(d), s[1])
                elif is_fr(s[1][0]):
                    self.e('fmov %s, %s' % (self.R(d), self.R(s[1][0])))
                else:
                    self.e('mov %s, %s' % (self.R(d), self.R(s[1][0])))
            return
        if n in INT_BIN:
            d = self.R(rd[1][0])
            a = self.src_gpr(src[0], 'w0')
            if src[1][0] == 'imm' and n in ('add', 'sub'):
                self.addimm(d, a, src[1][1], n)
            else:
                b = self.src_gpr(src[1], 'w1')
                self.e('%s %s, %s, %s' % (INT_BIN[n], d, a, b))
            return
        if n in SHIFT:
            d = self.R(rd[1][0])
            a = self.src_gpr(src[0], 'w0')
            if src[1][0] != 'imm':
                raise Reject('deslocamento por registrador')
            self.e('%s %s, %s, #%d' % (SHIFT[n], d, a, src[1][1] & 31))
            return
        if n in ('neg', 'not'):
            d = self.R(rd[1][0])
            a = self.src_gpr(src[0], 'w0')
            self.e('%s %s, %s' % ('neg' if n == 'neg' else 'mvn', d, a))
            return
        if n in SETCC or n == 'test':
            a = self.src_gpr(src[0], 'w0')
            if src[1][0] == 'imm' and src[1][1] < 4096 and n != 'test':
                self.e('cmp %s, #%d' % (a, src[1][1]))
            elif n == 'test' and src[1][0] == 'imm':
                m = src[1][1]
                if m and (m & (m + 1)) == 0 or (m & (m - 1)) == 0 and m:
                    self.e('tst %s, #0x%x' % (a, m))
                else:
                    self.movimm('w1', m)
                    self.e('tst %s, w1' % a)
            else:
                b = self.src_gpr(src[1], 'w1')
                self.e('%s %s, %s' % ('tst' if n == 'test' else 'cmp', a, b))
            self.e('cset %s, %s' % (self.R('T'), 'eq' if n == 'test' else SETCC[n]))
            return
        if n in ('fsetgt', 'fseteq'):
            self.e('fcmp %s, %s' % (self.R(src[0][1][0]), self.R(src[1][1][0])))
            self.e('cset %s, %s' % (self.R('T'), 'gt' if n == 'fsetgt' else 'eq'))
            return
        if n in FP_BIN:
            d = self.R(rd[1][0])
            a = self.fsrc(src[0], 's0')
            b = self.fsrc(src[1], 's1')
            self.e('%s %s, %s, %s' % (FP_BIN[n], d, a, b))
            return
        if n in ('fabs', 'fneg'):
            self.e('%s %s, %s' % (n, self.R(rd[1][0]), self.R(src[0][1][0])))
            return
        if n == 'cvt_f2i_t':
            self.e('fcvtzs %s, %s' % (self.R(rd[1][0]), self.R(src[0][1][0])))
            return
        if n == 'fipr':
            a = [self.R(x) for x in src[0][1]]
            b = [self.R(x) for x in src[1][1]]
            self.e('fmul s0, %s, %s' % (a[0], b[0]))
            self.e('fmul s1, %s, %s' % (a[1], b[1]))
            self.e('fadd s0, s0, s1')
            self.e('fmul s1, %s, %s' % (a[2], b[2]))
            self.e('fmul s2, %s, %s' % (a[3], b[3]))
            self.e('fadd s1, s1, s2')
            self.e('fadd %s, s0, s1' % self.R(rd[1][0]))
            return
        if n == 'ftrv':
            v = [self.R(x) for x in src[0][1]]
            vd = [self.R(x) for x in rd[1]]
            vn = ['v' + x[1:] for x in v]
            self.e('fmul v0.4s, v4.4s, %s.s[0]' % vn[0])
            self.e('fmla v0.4s, v5.4s, %s.s[0]' % vn[1])
            self.e('fmla v0.4s, v6.4s, %s.s[0]' % vn[2])
            self.e('fmla v0.4s, v7.4s, %s.s[0]' % vn[3])
            for i in range(4):
                self.e('mov %s, v0.s[%d]' % (vd[i], i))
            return
        raise Reject('sem emissor: ' + o['text'])

    def fsrc(self, s, scratch):
        if s[0] == 'imm':
            self.fimm(scratch, s[1])
            return scratch
        return self.R(s[1][0])

    # -- saidas
    def exit_stub(self, pc, t_const=None):
        lab = self.label('X')
        c = ['%s:' % lab]
        for x in sorted(self.W, key=lambda x: (x[0], int(x[1:]) if x[1:].isdigit() else 0)):
            if x in ('T',):
                continue
            h = self.R(x)
            c.append('\tstr %s, [x28, #%d]' % (h, ctx_off(x)))
        if t_const is None:
            c.append('\tstr w15, [x28, #%d]' % CTX_T)
        else:
            c.append('\tmov w1, #%d' % t_const)
            c.append('\tstr w1, [x28, #%d]' % CTX_T)
        c.append('\tmov w0, #0x%s' % pc[4:])
        c.append('\tmovk w0, #0x%s, lsl #16' % pc[:4])
        c.append('\tb X_ret')
        self.cold += c
        return lab

    # -- layout: sucessor mais executado em seguida
    def layout(self):
        seen, lay = set(), []

        def chain(v):
            while v and v not in seen:
                seen.add(v)
                lay.append(v)
                b = self.B[v]
                e = b['end']
                if e[0] == 'cond':
                    tk, nt = b['counts']
                    cand = [e[2], e[3]] if tk >= nt else [e[3], e[2]]
                else:
                    cand = [e[1]]
                v = next((c for c in cand if c in self.B and c not in seen), None)
        for v in sorted(self.order, key=lambda v: -self.B[v].get('runs', 0)):
            chain(v)
        return lay

    def gen(self):
        self.analyse()
        lay = self.layout()
        W = self.W
        o = self.out
        o += ['\t.text', '\t.align 4', '\t.global new_run', '\t.global sq_stub', '\t.global g_membase',
              '\t.global g_sink', '\t.global g_cycles', 'new_run:',
              '\tstp x29, x30, [sp, #-160]!', '\tstp x19, x20, [sp, #16]', '\tstp x21, x22, [sp, #32]',
              '\tstp x23, x24, [sp, #48]', '\tstp x25, x26, [sp, #64]', '\tstp x27, x28, [sp, #80]',
              '\tstp d8, d9, [sp, #96]', '\tstp d10, d11, [sp, #112]', '\tstp d12, d13, [sp, #128]',
              '\tstp d14, d15, [sp, #144]', '\tmov x28, x0', '\tmov w29, w1', '\tadd x27, x28, #0x1c0',
              '\tb E_%s' % self.entry]
        # entradas: carregam tudo que a regiao usa (inclusive o que ela escreve)
        for en in self.entries:
            o.append('E_%s:' % en)
            for x in sorted(self.U | self.W):
                self.e('ldr %s, [x28, #%d]' % (self.R(x), ctx_off(x)))
            if self.vec_xf:
                self.e('ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x28]')
            self.e('cmp w29, #%d' % self.guard[en])
            self.e('b.lt %s' % self.exit_stub(en))
            self.e('b B_%s' % en)
        stats = {'guardas': dict(self.guard), 'entradas': self.entries, 'lacos': sorted(self.latch)}
        for i, v in enumerate(lay):
            b = self.B[v]
            o.append('B_%s:' % v)
            if v in self.latch:
                self.e('cmp w29, #%d' % self.guard[v])
                self.e('b.lt %s' % self.exit_stub(v))
            self.e('sub w29, w29, #%d' % b['cycles'])
            # liveness dentro do bloco (para as chamadas)
            ir = b['ir']
            live = self.live_out(v, self.live_in)
            after = [None] * len(ir)
            for j in range(len(ir) - 1, -1, -1):
                after[j] = set(live)
                u, d = uses_defs(ir[j])
                live -= set(d)
                live |= set(u)
            self.decision = None
            jc = next((j for j, p in enumerate(ir) if p['op'] == 'jcond'), None)
            self.t_after_jcond = jc is not None and any('T' in uses_defs(p)[1] for p in ir[jc + 1:])
            for j, op in enumerate(ir):
                sq_likely = False
                if op['op'] == 'pref' and op['src'][0][0] == 'reg':
                    a = op['src'][0][1][0]
                    sq_likely = any(p['op'] == 'writem' and p['src'][0][0] == 'reg' and p['src'][0][1][0] == a
                                    for vv in self.order for p in self.B[vv]['ir'])
                self.op(op, after[j], sq_likely)
            e = b['end']
            nxt = lay[i + 1] if i + 1 < len(lay) else None
            if e[0] == 'jump':
                t = e[1]
                if t in self.B:
                    if t != nxt:
                        self.e('b B_%s' % t)
                else:
                    self.e('b %s' % self.exit_stub(t))
            else:
                _, on_t, tgt, fall = e
                dec = self.decision or self.R('T')
                taken = tgt if on_t else fall		# destino quando T==1
                other = fall if on_t else tgt		# destino quando T==0
                tc = (None, None) if self.t_after_jcond else (1, 0)
                lt = 'B_%s' % taken if taken in self.B else self.exit_stub(taken, tc[0])
                lo = 'B_%s' % other if other in self.B else self.exit_stub(other, tc[1])
                if other == nxt:
                    self.e('cbnz %s, %s' % (dec, lt))
                elif taken == nxt:
                    self.e('cbz %s, %s' % (dec, lo))
                else:
                    self.e('cbnz %s, %s' % (dec, lt))
                    self.e('b %s' % lo)
        o += ['\t.p2align 4', 'region_hot_end:']
        o += self.cold
        o += ['X_ret:', '\tadrp x9, g_cycles', '\tstr w29, [x9, :lo12:g_cycles]',
              '\tldp d14, d15, [sp, #144]', '\tldp d12, d13, [sp, #128]', '\tldp d10, d11, [sp, #112]',
              '\tldp d8, d9, [sp, #96]', '\tldp x27, x28, [sp, #80]', '\tldp x25, x26, [sp, #64]',
              '\tldp x23, x24, [sp, #48]', '\tldp x21, x22, [sp, #32]', '\tldp x19, x20, [sp, #16]',
              '\tldp x29, x30, [sp], #160', '\tret',
              'sq_stub:', '\tadrp x3, g_membase', '\tldr x3, [x3, :lo12:g_membase]',
              '\tand w0, w0, #0xFFFFFFE0', '\tadd x3, x3, x0', '\tldp q0, q1, [x3]', '\tadrp x3, g_sink',
              '\tadd x3, x3, :lo12:g_sink', '\tldr x2, [x3]', '\tstp q0, q1, [x2], #32', '\tstr x2, [x3]', '\tret',
              '\t.data', '\t.align 3', 'g_membase: .quad 0', 'g_sink: .quad 0', 'g_cycles: .word 0']
        return stats


def main():
    dump, outp, entry = sys.argv[1], sys.argv[2], sys.argv[3].upper()
    want = [v.upper() for v in sys.argv[4:]]
    if entry not in want:
        want.insert(0, entry)
    try:
        blocks = load_region(dump, want)
        g = Gen(blocks, want, entry)
        st = g.gen()
    except Reject as ex:
        print('REGIAO RECUSADA:', ex)
        sys.exit(2)
    # comeco das primeiras linhas marca a entrada da regiao para medir tamanho
    lines = g.out
    i = lines.index('E_%s:' % entry)
    lines.insert(i, 'region_begin:')
    open(outp, 'w').write('\n'.join(lines) + '\n')
    print('regiao: %d blocos, entradas %s, lacos (guarda na origem) %s' % (len(want), st['entradas'], st['lacos']))
    print('guardas (ciclos):', ' '.join('%s=%d' % kv for kv in sorted(st['guardas'].items())))
    print('casas:', ' '.join('%s:%s' % (k, g.home[k]) for k in sorted(g.home, key=lambda x: (x[0], x))))


if __name__ == '__main__':
    main()
