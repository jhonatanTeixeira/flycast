#!/usr/bin/env python3
"""Funcoes mais quentes a partir do dump do JIT + amostras do perf.

  jit_hot_functions.py <dump/jit-*.txt> <samples.txt[.gz]> [--top 15] [--tcb-off HEX]
                       [--so-base HEX] [--list]

Por que existe: o `perf` so ve um blob (SH4_TCB) e o `jit_lite_report.py` resolve
amostra -> BLOCO. Para decidir o que nativizar (HLE) precisamos da FUNCAO: este
script agrupa os blocos em funcoes (entradas = alvo de `bsr` e bloco que nao e
fall-through de ninguem), soma o tempo amostrado por funcao e, com `--list`,
imprime o SH4 desmontado de cada funcao quente (para escrever o pseudo-C do HLE,
como em docs/sdk_blocks/*_pseudo.cpp).
"""
import argparse
import bisect
import collections
import gzip
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import jit_lite_report as J   # parse_dump / Resolver / SAMPLE
import sh4dis as S


def load_blocks_with_ops(path):
    """{vaddr: (n_ops, [opcodes])} a partir das linhas B + G do dump."""
    blocks = {}
    cur = None
    for line in open(path, errors='replace'):
        if line.startswith('B '):
            p = line.split()
            cur = int(p[2], 16)
            blocks[cur] = []
        elif line.startswith('G ') and cur is not None:
            blocks[cur] = [int(x, 16) for x in line.split()[1:]]
    return blocks


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('dump')
    ap.add_argument('samples')
    ap.add_argument('--top', type=int, default=15)
    ap.add_argument('--tcb-off', default=None)
    ap.add_argument('--so-base', default=None)
    ap.add_argument('--freq', type=int, default=299)
    ap.add_argument('--list', action='store_true', help='imprime o SH4 das funcoes quentes')
    a = ap.parse_args()

    blocks, clock = J.parse_dump(a.dump)
    so_base = int(a.so_base, 16) if a.so_base else None
    if so_base is None:
        for line in open(a.dump, errors='replace'):
            if line.startswith('M so_base'):
                so_base = int(line.split()[2], 16)
                break
    tcb = int(a.tcb_off, 16) if a.tcb_off else 0
    CODE_SIZE = 15 << 20
    cc = ((so_base + tcb + 4095) & ~4095) if so_base else None
    res = J.Resolver(blocks)
    ops = load_blocks_with_ops(a.dump)

    rows = []
    opener = gzip.open if a.samples.endswith('.gz') else open
    with opener(a.samples, 'rt', errors='replace') as f:
        for line in f:
            m = J.SAMPLE.match(line)
            if m:
                rows.append((m.group(1), int(m.group(2)), int(float(m.group(3)) * 1e9),
                             int(m.group(4), 16), m.group(5).strip()))
    if not rows:
        raise SystemExit('sem amostras')

    # thread de emulacao = a que tem mais amostras em SH4_TCB
    jit_tid = collections.Counter(r[0] for r in rows if r[4].startswith('SH4_TCB'))
    emu = jit_tid.most_common(1)[0][0] if jit_tid else None
    per_block = collections.Counter()
    for tid, _ts, ip, sym in [(r[0], r[2], r[3], r[4]) for r in rows]:
        if tid != emu or not sym.startswith('SH4_TCB'):
            continue
        b = res.find(ip, _ts)
        if b is not None:
            per_block[b[4]] += 1
    total = sum(per_block.values()) or 1

    # entradas de funcao
    entries = set()
    fall = {}
    for va, o in ops.items():
        if not o:
            continue
        last = o[-1]
        # bsr (1011 dddddddddddd) / jsr (0100 nnnn 0000 1011) sem alvo estatico aqui
        if (last & 0xF000) == 0xB000:
            d = last & 0xFFF
            d = d - 0x1000 if d & 0x800 else d
            tgt = va + 4 + d * 2
            entries.add(tgt)
        fall[va] = va + 2 + 2 * len(o)          # fall-through apos o bloco
    for va in ops:
        if va not in entries:
            if not any(f == va for f in fall.values()):
                entries.add(va)
    ent = sorted(entries)

    def func_of(va):
        i = bisect.bisect_right(ent, va) - 1
        return ent[i] if i >= 0 else va

    per_func = collections.Counter()
    for va, n in per_block.items():
        per_func[func_of(va)] += n
    nfun = {f: sum(1 for v in ops if func_of(v) == f) for f in per_func}

    print('thread de emulacao: tid %s, %d amostras em %d blocos' % (emu, total, len(per_block)))
    print('--- funcoes mais quentes (% do tempo na thread de emulacao) ---')
    acc = 0
    for f, n in per_func.most_common(a.top):
        acc += n
        # tamanho em bytes da funcao: blocos contiguos
        addrs = sorted(v for v in ops if func_of(v) == f)
        size = (max(addrs) - min(addrs) + 2) if addrs else 0
        print('  %08X  %5.2f%%  acum %5.1f%%  blocos=%-3d faixa=%d B' %
              (f, 100.0 * n / total, 100.0 * acc / total, nfun[f], size))
    if a.list:
        for f, n in per_func.most_common(a.top):
            print('\n===== funcao %08X  (%.2f%% do tempo) =====' % (f, 100.0 * n / total))
            for va in sorted(v for v in ops if func_of(v) == f):
                for i, op in enumerate(ops[va]):
                    print('  %08X  %04X  %s' % (va + 2 * i, op, S.dis(op, va + 2 * i)))


if __name__ == '__main__':
    S.load_table()
    main()
