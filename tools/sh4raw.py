#!/usr/bin/env python3
"""Desmonta um dump cru da RAM do SH4 (ex.: fc_ctrl.py mem ... > ram.bin).

  sh4raw.py ram.bin BASE_HEX INI_HEX FIM_HEX     listagem
  sh4raw.py ram.bin BASE_HEX INI_HEX FIM_HEX --funcs   so os inicios de funcao

Inicio de funcao = endereco logo depois de um 'rts' + delay slot (e alinhado),
ou alvo de 'bsr'. Heuristica: serve para orientar a leitura, nao e exata.
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sh4dis


def main():
    sh4dis.load_table()
    path, base, lo, hi = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16), int(sys.argv[4], 16)
    funcs = '--funcs' in sys.argv
    data = open(path, 'rb').read()
    op = lambda a: struct.unpack_from('<H', data, a - base)[0]
    starts = set()
    for a in range(lo, hi, 2):
        o = op(a)
        if o == 0x000B:		# rts
            s = a + 4
            while s < hi and op(s) == 0x0009:	# nops de alinhamento
                s += 2
            starts.add(s)
        if o & 0xF000 == 0xB000:	# bsr
            d = o & 0xFFF
            d = d - 0x1000 if d & 0x800 else d
            starts.add(a + 4 + d * 2)
    if funcs:
        for s in sorted(x for x in starts if lo <= x < hi):
            print('%08X' % s)
        return
    for a in range(lo, hi, 2):
        if a in starts:
            print('\n%08X <funcao>' % a)
        o = op(a)
        print('  %08X  %04X  %s' % (a, o, sh4dis.dis(o, a)))


if __name__ == '__main__':
    main()
