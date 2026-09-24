#!/usr/bin/env python3
"""dyn-<pid>.bin (destinos das saidas dinamicas, FC_JIT_DUMP) + jit-<pid>.txt
-> dyn_off.bin: offset no cache de codigo do bloco de cada destino, para o
tools/dispatch_branch_bench.c recriar o layout real dos alvos.

Uso: jit_dyn_offsets.py jit-<pid>.txt dyn-<pid>.bin dyn_off.bin [N]
"""
import sys
import numpy as np

code = {}
for l in open(sys.argv[1], errors='replace'):
    if l[0] == 'B':
        p = l.split()
        code[int(p[2], 16)] = int(p[1], 16)
base = min(code.values())
n = int(sys.argv[4]) if len(sys.argv) > 4 else 8000000
a = np.fromfile(sys.argv[2], dtype=np.uint32)[:n]
off = np.array([code.get(int(x), base) - base for x in a], dtype=np.uint32)
off.tofile(sys.argv[3])
print('%d saidas, codigo alvo espalhado em %.1f MB' % (len(off), off.max() / 1e6))
