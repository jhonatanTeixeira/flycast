#!/usr/bin/env python3
"""Estudo offline do codigo que o JIT ARM64 gera (dump de FC_JIT_DUMP).

Uso: jit_study.py jit-<pid>.txt core.so [--top N] [--block VADDR]

Le o dump (formato em rec_arm64.cpp, jit_dump_*), desmonta todo o codigo
ARM64 de uma vez (aarch64-linux-gnu-objdump) e atribui cada instrucao do
host a uma categoria:

  entrada   : checagem anti-SMC, contador de perfil (artefato de medicao),
              ciclos/agendador, avanco de espera; o caminho frio do agendador
              (depois do b.pl) nao conta como executado
  reg_carga : alocador carregando registrador do SH4 do contexto
  corpo     : o trabalho da op SHIL (por tipo; memoria por regiao)
  reg_desc  : alocador gravando registrador de volta no contexto
  fim_desc  : descarga de fim de bloco
  saida     : ligacao para o proximo bloco

Execucoes = runs de cada bloco (FC_BLOCK_PROF). Bloco e linha reta: cada op
roda uma vez por execucao. As contagens de host sao estaticas x runs
(limite superior: desvios internos raros contam como executados).
"""
import bisect
import collections
import os
import re
import subprocess
import sys
import tempfile

OBJDUMP = 'aarch64-linux-gnu-objdump'
NM = 'aarch64-linux-gnu-nm'

FPU_OPS = {'fadd', 'fsub', 'fmul', 'fdiv', 'fsqrt', 'fipr', 'ftrv', 'fmac', 'fsca', 'fsrra', 'fabs',
           'fneg', 'fsetgt', 'fseteq', 'cvt_f2i_t', 'cvt_i2f_n', 'cvt_i2f_z', 'frswap', 'fswap'}
ALU_OPS = {'add', 'sub', 'and', 'or', 'xor', 'shl', 'shr', 'sar', 'ror', 'neg', 'not', 'test', 'seteq',
           'setge', 'setgt', 'setae', 'setab', 'setpeq', 'adc', 'sbc', 'negc', 'rocl', 'rocr', 'shld', 'shad',
           'mul_u16', 'mul_s16', 'mul_i32', 'mul_u64', 'mul_s64', 'div32u', 'div32s', 'div32p2', 'div1',
           'ext_s8', 'ext_s16', 'swaplb', 'xtrct', 'addc', 'subc', 'cmp', 'movt', 'max', 'min'}


def region(addr, write):
    """Regiao do mapa de memoria do Dreamcast para um endereco do guest."""
    if (addr >> 26) == 0x38:
        return 'video: SQ (fila de escrita p/ TA)'
    if addr >= 0xF0000000 or (addr & 0x1FFFFFFF) >> 24 == 0x1F and addr >= 0xE0000000:
        return 'sistema: registradores internos do SH4 (P4)'
    if 0x7C000000 <= addr < 0x80000000:
        return 'sistema: OCRAM (cache usado como RAM)'
    a = addr & 0x1FFFFFFF
    if 0x1F000000 <= a < 0x20000000:
        return 'sistema: registradores internos do SH4 (P4)'
    if 0x10000000 <= a < 0x14000000:
        return 'video: FIFO do TA'
    if 0x04000000 <= a < 0x08000000:
        return 'video: VRAM'
    if 0x005F8000 <= a < 0x005FA000:
        return 'video: registradores do PVR'
    if 0x00700000 <= a < 0x00710000:
        return 'som: registradores do AICA'
    if 0x00710000 <= a < 0x00720000:
        return 'sistema: RTC'
    if 0x00800000 <= a < 0x01000000:
        return 'som: ARAM (RAM de som)'
    if 0x005F7000 <= a < 0x005F7100:
        return 'sistema: GD-ROM'
    if 0x005F6C00 <= a < 0x005F6D00:
        return 'sistema: Maple (controles)'
    if 0x005F7800 <= a < 0x005F7A00:
        return 'sistema: G2 (DMA de som/ext)'
    if 0x005F6800 <= a < 0x005F7C00:
        return 'sistema: Holly (interrupcoes/DMA)'
    if 0x0C000000 <= a < 0x10000000:
        return 'RAM principal'
    if a < 0x00200000:
        return 'sistema: BIOS/flash'
    return 'outro'


def ctx_name(off):
    """Campo do Sh4Context (sh4_if.h) num offset de [x28, #off]."""
    if off < 64:
        return 'xf%d (banco 1 / XMTRX)' % (off // 4)
    if off < 128:
        return 'fr%d' % ((off - 64) // 4)
    if off < 192:
        return 'r%d' % ((off - 128) // 4)
    names = {192: 'macl', 196: 'mach', 232: 'gbr', 236: 'ssr', 240: 'spc', 244: 'sgr', 248: 'dbr',
             252: 'vbr', 256: 'pr', 260: 'fpul', 264: 'pc/next_pc', 268: 'jdyn', 272: 'sr.status',
             276: 'sr.T', 280: 'fpscr'}
    if 200 <= off < 232:
        return 'r_bank%d' % ((off - 200) // 4)
    return names.get(off, 'ctx+%d' % off)


def shil_cat(name):
    if name in FPU_OPS:
        return 'fpu'
    if name in ALU_OPS:
        return 'alu'
    if name in ('mov32', 'mov64'):
        return 'mov'
    if name in ('jdyn', 'jcond'):
        return 'desvio'
    if name.startswith('sync_'):
        return 'sync_modo (SR/FPSCR)'
    if name == 'ifb':
        return 'interpretador (ifb)'
    if name == 'pref':
        return 'video?: pref (descarga da SQ)'
    return 'outro:' + name


def sh4_class(op):
    """Classe grossa de uma instrucao SH4 (o que o jogo pede)."""
    n = op >> 12
    lo4 = op & 0xF
    if op == 0x0009:
        return 'nop'
    if n == 0xF:
        return 'fpu'
    if n in (0x8, 0x9, 0xA, 0xB, 0xD) or (n == 0 and lo4 in (0x3, 0xB) and (op & 0xFF) in (0x03, 0x23, 0x0B, 0x2B)):
        if n == 0x9 or n == 0xD:
            return 'load'  # mov.w/mov.l @(disp,PC)
        return 'desvio'
    if n == 0x4 and (op & 0xFF) in (0x0B, 0x2B):
        return 'desvio'
    if n in (0x5, 0x6) and not (n == 0x6 and lo4 in (0x3, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF)):
        return 'load'
    if n == 0x1 or (n == 0x2 and lo4 <= 0x6):
        return 'store'
    if n == 0x0 and lo4 in (0x4, 0x5, 0x6):
        return 'store'
    if n == 0x0 and lo4 in (0xC, 0xD, 0xE):
        return 'load'
    if n == 0x0 and (op & 0xFF) == 0x83:
        return 'pref'
    if n == 0xC and ((op >> 8) & 0xF) in (0x0, 0x1, 0x2, 0x4, 0x5, 0x6):
        return 'store' if ((op >> 8) & 0xF) < 3 else 'load'
    if n == 0x8 and ((op >> 8) & 0xF) in (0x0, 0x1, 0x4, 0x5):
        return 'store' if ((op >> 8) & 0xF) < 2 else 'load'
    if n == 0x4 and lo4 in (0x6, 0x7, 0xA, 0xE) or (n == 0 and lo4 in (0x2, 0xA)) or (n == 0x4 and lo4 in (0x2, 0x3)):
        return 'sistema (ldc/stc/lds/sts)'
    return 'alu'


def parse(path):
    blocks = []
    live = {}           # (code, vaddr) -> idx
    rewrites = []       # (idx_bloco, off, addr)
    pstores = []
    clears = []
    meta = {}
    cur = None
    by_code = []        # (code, idx) vivos para W
    with open(path, errors='replace') as f:
        for line in f:
            t = line[0]
            if t == 'B':
                p = line.split()
                cur = {
                    'code': int(p[1], 16), 'vaddr': int(p[2], 16), 'addr': int(p[3], 16),
                    'sh4b': int(p[4]), 'gops': int(p[5]), 'cyc': int(p[6]), 'hbytes': int(p[7]),
                    'flags': dict(x.split('=') for x in p[8:]), 'ops': [], 'runs': 0, 'final': False,
                    'regions': {},
                }
                idx = len(blocks)
                blocks.append(cur)
                key = (cur['code'], cur['vaddr'])
                old = live.get(key)
                if old is not None:
                    blocks[old]['final'] = True
                live[key] = idx
                by_code.append((cur['code'], idx))
            elif t == 'G':
                cur['G'] = [int(x, 16) for x in line.split()[1:]]
            elif t == 'E':
                p = line.split()
                cur['E'] = (int(p[1]), int(p[2]), int(p[3]))
            elif t == 'O':
                p = line.rstrip('\n').split(' ', 8)
                cur['ops'].append((int(p[1]), int(p[2]), int(p[3]), int(p[4]), int(p[5]), int(p[6]),
                                   int(p[7]), p[8] if len(p) > 8 else ''))
            elif t == 'H':
                cur['H'] = bytes.fromhex(line[2:].strip())
            elif t == 'R' or t == 'D':
                p = line.split()
                key = (int(p[1], 16), int(p[2], 16))
                idx = live.get(key)
                if idx is not None and not blocks[idx]['final']:
                    blocks[idx]['runs'] = int(p[3])
                    if t == 'D':
                        blocks[idx]['final'] = True
                        del live[key]
            elif t == 'W':
                p = line.split()
                pc, addr = int(p[1], 16), int(p[2], 16)
                # bloco vivo mais recente que contem pc
                best = None
                for code, idx in reversed(by_code[-40000:]):
                    b = blocks[idx]
                    if code <= pc < code + b['hbytes'] and not b['final']:
                        best = idx
                        break
                rewrites.append((best, pc - blocks[best]['code'] if best is not None else None, addr))
            elif t == 'P':
                pstores.append(int(line.split()[1], 16))
            elif t == 'Z':
                clears.append(line.strip())
            elif t == 'M':
                p = line.split()
                meta = {'so_base': int(p[2], 16)}
    return blocks, rewrites, pstores, clears, meta


def disassemble(blocks):
    """Desmonta todo o codigo de uma vez. Devolve, por bloco, {off: (mnem, ops)}."""
    offs = []
    data = bytearray()
    for b in blocks:
        offs.append(len(data))
        data += b.get('H', b'')
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as tf:
        tf.write(data)
        name = tf.name
    out = subprocess.run([OBJDUMP, '-D', '-b', 'binary', '-m', 'aarch64', '--no-show-raw-insn', name],
                         capture_output=True, text=True).stdout
    os.unlink(name)
    ins = {}
    rx = re.compile(r'^\s*([0-9a-f]+):\s+(\S+)\s*(.*)$')
    for l in out.splitlines():
        m = rx.match(l)
        if m:
            ins[int(m.group(1), 16)] = (m.group(2), m.group(3))
    res = []
    for i, b in enumerate(blocks):
        base = offs[i]
        d = {}
        for k in range(0, len(b.get('H', b'')), 4):
            d[k] = ins.get(base + k, ('?', ''))
        res.append((base, d))
    return res


def load_symbols(so, so_base):
    out = subprocess.run([NM, '-C', '--defined-only', so], capture_output=True, text=True).stdout
    syms = []
    for l in out.splitlines():
        p = l.split(' ', 2)
        if len(p) == 3 and p[1] in 'tTwW':
            syms.append((int(p[0], 16) + so_base, p[2]))
    syms.sort()
    return syms


def sym_at(syms, addr):
    i = bisect.bisect_right(syms, (addr, '\xff')) - 1
    if i >= 0 and addr - syms[i][0] < 0x10000:
        return syms[i][1]
    return '?%x' % addr


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        sys.exit(1)
    path, so = sys.argv[1], sys.argv[2]
    top = 25
    one = None
    if '--top' in sys.argv:
        top = int(sys.argv[sys.argv.index('--top') + 1])
    if '--block' in sys.argv:
        one = int(sys.argv[sys.argv.index('--block') + 1], 16)
    blocks, rewrites, pstores, clears, meta = parse(path)
    dis = disassemble(blocks)
    syms = load_symbols(so, meta.get('so_base', 0))

    # regioes dos acessos reescritos (fault -> fora da RAM)
    wregions = collections.Counter()
    for bi, off, addr in rewrites:
        if bi is None:
            continue
        b = blocks[bi]
        for o in b['ops']:
            if o[3] <= off < o[4] or o[2] <= off < o[5]:
                b['regions'][o[0]] = addr
                break
        wregions[region(addr, False)] += 1

    cat = collections.Counter()        # instrucoes do host executadas por categoria
    memreg = collections.Counter()     # acessos (execucoes) por regiao
    memreg_host = collections.Counter()
    shil_host = collections.Counter()
    shil_count = collections.Counter()
    calls = collections.Counter()
    guest = collections.Counter()
    guest_total = 0
    host_total = 0
    per_block = []
    exits = collections.Counter()
    ecount = collections.Counter()
    redund = collections.Counter()
    sizes = collections.Counter()
    perclass = collections.Counter()
    perclass_n = collections.Counter()
    regtraffic = collections.Counter()
    compiled = collections.Counter(b['vaddr'] for b in blocks)

    for bi, b in enumerate(blocks):
        runs = b['runs']
        if 'E' not in b or 'H' not in b:
            continue
        d = dis[bi][1]
        entry_end, loop_end, relink = b['E']
        hb = b['hbytes']
        label = {}
        # entrada: achar contador de perfil (mov x9..; ldr w10,[x9]; add; str w10,[x9]) e o b.pl
        keys = sorted(k for k in d if k < entry_end)
        prof_end = None
        for k in keys:
            mn, ops = d[k]
            if mn == 'str' and ops.startswith('w10, [x9]'):
                prof_end = k + 4
                break
        prof_start = None
        if prof_end is not None:
            k = prof_end - 16
            while k - 4 >= 0 and d.get(k - 4, ('', ''))[0] in ('mov', 'movk') and d[k - 4][1].startswith('x9'):
                k -= 4
            prof_start = k
        bpl = None
        for k in keys:
            if d[k][0] == 'b.pl':
                bpl = k
                break
        for k in keys:
            if prof_start is not None and prof_start <= k < prof_end:
                label[k] = 'entrada: contador de perfil (artefato)'
            elif prof_start is not None and k < prof_start:
                label[k] = 'entrada: checagem anti-SMC'
            elif bpl is not None and k > bpl:
                label[k] = 'entrada: caminho frio do agendador'
            elif b['flags'].get('idle') == '1' or b['flags'].get('delay') == '1':
                label[k] = 'entrada: ciclos + avanco de espera'
            else:
                label[k] = 'entrada: ciclos/agendador'
        opcat = {}
        for o in b['ops']:
            idx, goff, o0, o1, o2, o3, delay, text = o
            name = text.split()[0] if text else '?'
            if name in ('readm', 'writem'):
                if idx in b['regions']:
                    reg = region(b['regions'][idx], name == 'writem')
                else:
                    m = re.search(r'\[\s*(0x[0-9a-fA-F]+)', text)
                    reg = region(int(m.group(1), 16), name == 'writem') if m else 'RAM principal'
                c = ('mem_le: ' if name == 'readm' else 'mem_escreve: ') + reg
                memreg[c] += runs
            else:
                c = shil_cat(name)
            opcat[idx] = c
            shil_count[name] += runs
            for k in range(o0, o1, 4):
                label[k] = 'flycast: reg_carga (contexto -> registrador)'
            for k in range(o1, o2, 4):
                label[k] = 'corpo: ' + c
                shil_host[name] += runs
                if name in ('readm', 'writem'):
                    memreg_host[c] += runs
            for k in range(o2, o3, 4):
                label[k] = 'flycast: reg_desc (registrador -> contexto)'
        for k in range(loop_end, relink, 4):
            label[k] = 'flycast: descarga de fim de bloco'
        for k in range(relink, hb, 4):
            label[k] = 'flycast: saida/ligacao de bloco'
        # chamadas C++ a partir do JIT
        regimm = {}
        bcount = collections.Counter()
        for k in range(0, hb, 4):
            mn, ops = d.get(k, ('?', ''))
            lab = label.get(k, 'sem_rotulo')
            if lab == 'entrada: caminho frio do agendador':
                continue
            bcount[lab] += 1
            dst = ops.split(',')[0].strip() if ops else ''
            m = re.match(r'(x\d+), #0x([0-9a-f]+)(?:, lsl #(\d+))?', ops) if mn in ('mov', 'movz', 'movk') else None
            if m:
                r, v, sh = m.group(1), int(m.group(2), 16), int(m.group(3) or 0)
                regimm[r] = (regimm.get(r, 0) if mn == 'movk' else 0) | (v << sh)
            elif dst.startswith('x') and mn not in ('str', 'stp', 'st1', 'prfm', 'cmp', 'cbz', 'cbnz', 'blr', 'br', 'b', 'bl') and not mn.startswith('b.'):
                regimm.pop(dst, None)
            if mn == 'blr':
                r = ops.strip()
                if r in regimm:
                    calls[sym_at(syms, regimm[r] & 0xFFFFFFFFFFFF)] += runs
                else:
                    calls['(ponteiro carregado em tempo de execucao, ex.: do_sqw da SQ)'] += runs
            elif mn == 'bl':
                m = re.match(r'0x([0-9a-f]+)', ops)
                if m:
                    tgt = int(m.group(1), 16) - dis[bi][0] + b['code']
                    calls[sym_at(syms, tgt)] += runs
        # tipo de saida pelo codigo de ligacao desmontado (RelinkBlock)
        exit_mn = [d.get(k, ('?', ''))[0] + ' ' + d.get(k, ('?', ''))[1] for k in range(relink, hb, 4)]
        if any(x.startswith('br ') for x in exit_mn):
            etype = 'dinamica (pelo PC, tabela FPCB)'
        elif any(x.startswith('cmp w11') for x in exit_mn):
            etype = 'condicional'
        else:
            etype = 'estatica'
        ecount[etype] += 1
        exits[etype] += runs
        if etype == 'condicional' and bcount.get('flycast: saida/ligacao de bloco', 0) > 0:
            bcount['flycast: saida/ligacao de bloco'] -= 1
        # descargas redundantes e recargas dentro do bloco
        laststore = {}
        for k in range(0, hb, 4):
            mn, ops = d.get(k, ('?', ''))
            lab = label.get(k, '')
            if lab == 'entrada: caminho frio do agendador':
                continue
            if mn in ('bl', 'blr', 'br') or mn.startswith('b.') or mn == 'b':
                laststore.clear()
                continue
            m = re.match(r'([wxsdq]\d+|wzr|xzr), \[x28, #(\d+)\]$', ops)
            if not m:
                continue
            off = int(m.group(2))
            if mn in ('ldr', 'str'):
                regtraffic[(ctx_name(off), 'carga' if mn == 'ldr' else 'descarga')] += runs
            if mn == 'str' and ('reg_desc' in lab or 'descarga' in lab):
                if off in laststore:
                    redund['descarga sobrescrita depois no mesmo bloco'] += runs
                laststore[off] = k
                redund['descargas'] += runs
            elif mn == 'ldr' and 'reg_carga' in lab:
                redund['cargas'] += runs
                if off in laststore:
                    redund['carga do que o proprio bloco acabou de gravar'] += runs
            elif mn == 'ldr' and off in laststore:
                redund['recarga fora do alocador (ex.: saida le jdyn/T)'] += runs
        sizes[min(b['gops'], 40)] += runs
        # custo por classe SH4: corpo+carga+descarga das ops daquela instrucao
        g = b.get('G', [])
        for o in b['ops']:
            gi = o[1] // 2
            cls = sh4_class(g[gi]) if gi < len(g) else '?'
            n = (o[5] - o[2]) // 4
            perclass[cls] += n * runs
        for gi in range(len(g)):
            perclass_n[sh4_class(g[gi])] += runs
        for lab, n in bcount.items():
            cat[lab] += n * runs
            host_total += n * runs
        gi = 0
        for op in b.get('G', []):
            guest[sh4_class(op)] += runs
        guest_total += b['gops'] * runs
        per_block.append((sum(bcount.values()) * runs, bi, bcount))

    def pct(n, t):
        return 100.0 * n / t if t else 0

    work = host_total - sum(v for k, v in cat.items() if 'artefato' in k)
    print('=== Estudo do codigo gerado pelo JIT ===')
    print('blocos compilados: %d (vaddr distintos: %d, recompilados: %d vaddr / %d compilacoes extras)' % (
        len(blocks), len(compiled), sum(1 for v in compiled.values() if v > 1),
        sum(v - 1 for v in compiled.values())))
    print('codigo gerado: %.2f MB no total; limpezas de cache: %d' % (sum(b['hbytes'] for b in blocks) / 1e6, len(clears)))
    for z in clears:
        print('   ', z)
    print('acessos reescritos (fault fora da RAM): %d; escritas em pagina de codigo: %d' % (len(rewrites), len(pstores)))
    print('instrucoes SH4 executadas: %.0f M; instrucoes do host (sem artefato de perfil): %.0f M; host/SH4 = %.2f' % (
        guest_total / 1e6, work / 1e6, work / guest_total if guest_total else 0))
    print()
    print('--- Para onde vao as instrucoes do host (execucoes x instrucoes estaticas) ---')
    jogo = sum(v for k, v in cat.items() if k.startswith('corpo:'))
    fly = work - jogo
    print('  corpo das ops (o que o jogo pediu): %5.1f%%   overhead do flycast: %5.1f%%' % (pct(jogo, work), pct(fly, work)))
    for k, v in cat.most_common():
        if 'artefato' in k:
            continue
        print('  %6.2f%%  %-70s %8.1f M' % (pct(v, work), k, v / 1e6))
    print()
    print('--- O que o jogo pede (instrucoes SH4 executadas por classe) ---')
    for k, v in guest.most_common():
        print('  %6.2f%%  %-30s %8.1f M' % (pct(v, guest_total), k, v / 1e6))
    print()
    print('--- Acessos a memoria por regiao (execucoes; instrucoes do host no corpo) ---')
    tm = sum(memreg.values())
    for k, v in memreg.most_common():
        print('  %6.2f%%  %-62s %8.2f M acessos  %7.1f M instr' % (pct(v, tm), k, v / 1e6, memreg_host[k] / 1e6))
    print('  faults reescritos por regiao:', dict(wregions))
    print()
    print('--- Ops SHIL: execucoes e instrucoes do host no corpo ---')
    for k, v in shil_host.most_common(top):
        print('  %-14s %8.1f M exec  %8.1f M instr  %.1f instr/op' % (k, shil_count[k] / 1e6, v / 1e6, v / shil_count[k] if shil_count[k] else 0))
    print()
    print('--- Chamadas para C++ a partir do codigo gerado (execucoes; ligacao de bloco some depois de ligar) ---')
    for k, v in calls.most_common(top):
        if k.startswith('ngen_LinkBlock'):
            continue
        print('  %10.2f M  %s' % (v / 1e6, k))
    print()
    print('--- Saidas de bloco (execucoes) ---')
    te = sum(exits.values())
    for k, v in exits.most_common():
        print('  %6.2f%%  %-40s %8.1f M  (%d blocos)' % (pct(v, te), k, v / 1e6, ecount[k]))
    print()
    print('--- Tamanho dos blocos executados (instrucoes SH4 por bloco, pesado por execucao) ---')
    ts = sum(sizes.values())
    acc = 0
    for n in sorted(sizes):
        acc += sizes[n]
        if sizes[n] * 100 >= ts:
            print('  %2d%s instr: %5.1f%%  (acumulado %5.1f%%)' % (n, '+' if n == 40 else ' ', pct(sizes[n], ts), pct(acc, ts)))
    print('  media: %.1f instrucoes SH4 por bloco executado' % (guest_total / ts if ts else 0))
    print()
    print('--- Trafego registrador <-> contexto (execucoes) ---')
    for k in ('cargas', 'descargas', 'descarga sobrescrita depois no mesmo bloco',
              'carga do que o proprio bloco acabou de gravar', 'recarga fora do alocador (ex.: saida le jdyn/T)'):
        print('  %-55s %8.1f M' % (k, redund[k] / 1e6))
    print()
    print('--- Registradores do SH4 que mais trafegam com o contexto (execucoes) ---')
    per = collections.Counter()
    for (nm, kind), v in regtraffic.items():
        per[nm] += v
    for nm, v in per.most_common(24):
        print('  %-24s %8.1f M  (carga %6.1f M, descarga %6.1f M)' % (nm, v / 1e6, regtraffic[(nm, 'carga')] / 1e6, regtraffic[(nm, 'descarga')] / 1e6))
    print()
    print('--- Codigo quente x cache de instrucoes (L1I do A53 = 32KB) ---')
    rows = sorted(((w, blocks[bi]['hbytes']) for w, bi, bc in per_block), reverse=True)
    tw = sum(w for w, h in rows)
    acc = accb = 0
    marks = [50, 80, 90, 95, 99]
    for w, h in rows:
        acc += w
        accb += h
        while marks and acc * 100 >= marks[0] * tw:
            print('  %2d%% das instrucoes executadas cabem em %7.1f KB de codigo' % (marks[0], accb / 1024))
            marks.pop(0)
    print()
    print('--- Custo no host por classe de instrucao SH4 (ops + carga/descarga delas) ---')
    for k, v in perclass.most_common():
        print('  %-30s %8.1f M instr SH4  %8.1f M instr host  %.2f host/SH4' % (k, perclass_n[k] / 1e6, v / 1e6, v / perclass_n[k] if perclass_n[k] else 0))
    print()
    print('--- Blocos mais caros ---')
    per_block.sort(reverse=True)
    for w, bi, bc in per_block[:top]:
        b = blocks[bi]
        fl = sum(n for k, n in bc.items() if not k.startswith('corpo:') and 'artefato' not in k)
        tot = sum(n for k, n in bc.items() if 'artefato' not in k)
        print('  %08X runs=%9d sh4=%3d host=%4d (overhead %2d%%) compilado %dx  %5.2f%% do total' % (
            b['vaddr'], b['runs'], b['gops'], tot, pct(fl, tot), compiled[b['vaddr']], pct(w, host_total)))
    if one is not None:
        for bi, b in enumerate(blocks):
            if b['vaddr'] == one and 'H' in b:
                print('\n=== bloco %08X (runs %d) ===' % (one, b['runs']))
                d = dis[bi][1]
                opsby = {o[0]: o for o in b['ops']}
                starts = {o[2]: o for o in b['ops']}
                for k in range(0, b['hbytes'], 4):
                    if k in starts:
                        o = starts[k]
                        print('  ; op %d (sh4 +%d): %s' % (o[0], o[1], o[7]))
                    mn, ops = d.get(k, ('?', ''))
                    print('    %4x  %-8s %-40s' % (k, mn, ops))


if __name__ == '__main__':
    main()
