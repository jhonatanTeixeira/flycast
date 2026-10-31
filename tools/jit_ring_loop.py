#!/usr/bin/env python3
"""Mostra o LACO em que o jogo parou, a partir do ring buffer do FC_JIT_TRACE.

  jit_ring_loop.py <trace-ring> [--dump jit-*.txt] [--max 120]

O ring tem as ultimas 4096 entradas de bloco. O script acha o periodo do laco
(menor p tal que a cauda se repete) e imprime um ciclo: endereco do bloco,
quantas instrucoes SH4 e o desmonte (se --dump for dado).
"""
import argparse
import sys

sys.path.insert(0, __import__('os').path.dirname(__import__('os').path.abspath(__file__)))
import sh4dis as S


def load_ops(path):
    blocks = {}
    cur = None
    for line in open(path, errors='replace'):
        if line.startswith('B '):
            cur = int(line.split()[2], 16)
            blocks[cur] = []
        elif line.startswith('G ') and cur is not None:
            blocks[cur] = [int(x, 16) for x in line.split()[1:]]
    return blocks


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('ring')
    ap.add_argument('--dump')
    ap.add_argument('--max', type=int, default=120)
    a = ap.parse_args()
    seq = []
    for line in open(a.ring):
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        seq.append(int(line, 16))
    if not seq:
        raise SystemExit('ring vazio')
    # periodo: menor p em que os ultimos 3p batem
    n = len(seq)
    best = None
    for p in range(1, n // 4):
        if seq[n - p:] == seq[n - 2 * p:n - p] == seq[n - 3 * p:n - 2 * p]:
            best = p
            break
    print('entradas %d, periodo do laco %s' % (n, best))
    ops = load_ops(a.dump) if a.dump else {}
    S.load_table()
    cycle = seq[n - best:] if best else seq[-a.max:]
    if len(cycle) > a.max:
        cycle = cycle[:a.max]
    print('--- ciclo (%d blocos) ---' % len(cycle))
    uniq = {}
    for i, va in enumerate(cycle):
        o = ops.get(va)
        uniq[va] = o
        print('%3d  %08X  %s' % (i, va, ('%d instr' % len(o)) if o else ''))
    if ops:
        print('--- desmonte dos blocos unicos ---')
        for va, o in uniq.items():
            if not o:
                continue
            print('\n=== %08X' % va)
            for i, op in enumerate(o):
                print('  %08X  %04X  %s' % (va + 2 * i, op, S.dis(op, va + 2 * i)))


if __name__ == '__main__':
    main()
