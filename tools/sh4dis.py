#!/usr/bin/env python3
"""Desmontador SH4 a partir da tabela do proprio flycast + listagem de funcao
a partir do FC_JIT_DUMP (linhas B/G e, se houver, execucoes R/D).

  sh4dis.py <jit-pid.txt> <ini_hex> <fim_hex>

Lista, em ordem de endereco, cada bloco compilado na faixa: execucoes, bytes
ARM64 gerados e o SH4 desmontado. Serve para ler o codigo do jogo e o custo
que o JIT cobra por ele (analise estatica, nao profiling).
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TABLE = os.path.join(HERE, '..', 'core', 'hw', 'sh4', 'sh4_opcode_list.cpp')

MASKS = {}
OPS = []


def load_table():
    src = open(TABLE, errors='replace').read()
    for m in re.finditer(r'#define (Mask_\w+) (0x[0-9A-Fa-f]+)', src):
        MASKS[m.group(1)] = int(m.group(2), 16)
    for m in re.finditer(r'\{[^{}]*?,\s*[^,{}]*?,\s*(Mask_\w+)\s*,\s*(0x[0-9A-Fa-f]+)\s*,[^,]*,\s*"([^"]*)"', src):
        mask = MASKS.get(m.group(1))
        if mask is None:
            continue
        OPS.append((mask, int(m.group(2), 16), m.group(3)))
    # mais especifico primeiro (mais bits na mascara)
    OPS.sort(key=lambda o: -bin(o[0]).count('1'))


def sext(v, bits):
    return v - (1 << bits) if v & (1 << (bits - 1)) else v


def dis(op, pc):
    for mask, key, text in OPS:
        if op & mask == key:
            n, m = (op >> 8) & 15, (op >> 4) & 15
            d4, d8, d12 = op & 15, op & 0xFF, op & 0xFFF
            rep = {
                '<REG_N>': 'r%d' % n, '<REG_M>': 'r%d' % m,
                '<FREG_N>': 'fr%d' % n, '<FREG_M>': 'fr%d' % m, '<FREG_0>': 'fr0',
                '<FREG_N_SD_F>': 'fr%d' % n, '<FREG_M_SD_F>': 'fr%d' % m,
                '<FREG_N_SD_A>': 'fr%d' % n, '<FREG_M_SD_A>': 'fr%d' % m,
                '<DR_N>': 'dr%d' % (n & 14), '<FV_N>': 'fv%d' % (n & 12), '<FV_M>': 'fv%d' % ((op >> 6 & 3) * 4),
                '<RM_BANK>': 'r%d_bank' % (m & 7),
                '<imm8>': '#%d' % d8, '<simm8>': '#%d' % sext(d8, 8), '<simm8hex>': '#0x%02X' % d8,
                '<disp4b>': '%d' % d4, '<disp4w>': '%d' % (d4 * 2), '<disp4dw>': '%d' % (d4 * 4),
                '<disp8b>': '%d' % d8, '<disp8w>': '%d' % (d8 * 2), '<disp8dw>': '%d' % (d8 * 4),
                '<GBRdisp8b>': '%d' % d8, '<GBRdisp8w>': '%d' % (d8 * 2), '<GBRdisp8dw>': '%d' % (d8 * 4),
                '<PCdisp8d>': '[%08X]' % ((pc & ~3) + 4 + d8 * 4),
                '<PCdisp8w>': '[%08X]' % (pc + 4 + d8 * 2),
                '<bdisp8>': '%08X' % (pc + 4 + sext(d8, 8) * 2),
                '<bdisp12>': '%08X' % (pc + 4 + sext(d12, 12) * 2),
            }
            for k, v in rep.items():
                text = text.replace(k, v)
            return text
    return '.word 0x%04X' % op


def main():
    load_table()
    path, lo, hi = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
    blocks = {}
    runs = {}
    cur = None
    for line in open(path, errors='replace'):
        c = line[0]
        if c == 'B':
            p = line.split()
            va = int(p[2], 16)
            cur = va if lo <= va < hi else None
            if cur is not None:
                blocks[cur] = {'hbytes': int(p[7]), 'G': []}
        elif c == 'G' and cur is not None:
            blocks[cur]['G'] = [int(x, 16) for x in line.split()[1:]]
        elif c in 'RD':
            p = line.split()
            va = int(p[2], 16)
            if lo <= va < hi:
                runs[va] = max(runs.get(va, 0), int(p[3]))
    for va in sorted(blocks):
        b = blocks[va]
        g = b['G']
        print('\n=== %08X  execucoes=%s  sh4=%d instr  arm64=%d bytes (%.1f instr host/SH4)' % (
            va, runs.get(va, '?'), len(g), b['hbytes'], b['hbytes'] / 4 / max(1, len(g))))
        for i, op in enumerate(g):
            pc = va + 2 * i
            print('  %08X  %04X  %s' % (pc, op, dis(op, pc)))


if __name__ == '__main__':
    main()
