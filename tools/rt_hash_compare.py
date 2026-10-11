#!/usr/bin/env python3
"""Compara dois logs do FC_RT_HASH (render threads, 4.131): hash dos pixels de cada
frame do jogo, pelo id do frame (pedidos de render desde a carga do savestate).

  rt_hash_compare.py <base.txt> <novo.txt>

Compara so os ids presentes nos dois (os frames descartados variam entre rodadas).
Para o resultado valer, as duas rodadas precisam ser deterministicas
(FC_RTC_FIXED=1 FC_INPUT_NEUTRAL=1 e FC_EMU_WAIT_RE=1, que faz a emulacao esperar o
Process -- a VRAM nao muda enquanto as texturas sao lidas).
"""
import sys


def load(path):
    d = {}
    for line in open(path):
        p = line.split()
        if len(p) == 2:
            d.setdefault(int(p[0]), p[1])
    return d


def main():
    a, b = load(sys.argv[1]), load(sys.argv[2])
    common = sorted(set(a) & set(b))
    diff = [i for i in common if a[i] != b[i]]
    print('frames: base %d, novo %d, em comum %d' % (len(a), len(b), len(common)))
    print('identicos %d, diferentes %d' % (len(common) - len(diff), len(diff)))
    if diff:
        print('primeiros diferentes:', diff[:20])


if __name__ == '__main__':
    main()
