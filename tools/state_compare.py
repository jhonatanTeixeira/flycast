#!/usr/bin/env python3
"""Compara duas rodadas do mesmo savestate (FC_STATE_HASH + FC_AUDIO_DUMP).

Uso: state_compare.py hashA.txt hashB.txt [pcmA.raw pcmB.raw]

Rodar as duas com: FC_RTC_FIXED=600000000 FC_INPUT_NEUTRAL=1
FC_STATE_HASH=<arq> FC_STATE_HASH_EVERY=30 FC_AUDIO_DUMP=<arq>.
Alinha os pedidos de render pelo ciclo emulado (a partir da ultima carga de
savestate, linha L), ignora pedidos com TA vazio e o 1o pedido depois da
carga (leva sobra do boot no modo sem render em thread). Sai com codigo 1 se
achar diferenca -- serve de teste de regressao de um JIT novo contra o atual.
"""
import sys


def load(p):
    lines = [l.split() for l in open(p)]
    k = max([i for i, l in enumerate(lines) if l and l[0] == 'L'] + [-1])
    frames = [l for l in lines[k + 1:] if l and l[0] == 'F']
    return {f[2]: f[3:] for f in frames[1:] if f[3] != '0'}


def main():
    a, b = load(sys.argv[1]), load(sys.argv[2])
    common = sorted(set(a) & set(b), key=int)
    bad = []
    for c in common:
        x, y = a[c], b[c]
        if x[:2] != y[:2] or (len(x) > 2 and len(y) > 2 and x[2:] != y[2:]):
            bad.append(c)
    print('pedidos de render em comum: %d, so em A: %d, so em B: %d, diferentes: %d' % (
        len(common), len(set(a) - set(b)), len(set(b) - set(a)), len(bad)))
    for c in bad[:5]:
        print('  ciclo %s\n    A %s\n    B %s' % (c, ' '.join(a[c]), ' '.join(b[c])))
    ok = not bad and common
    if len(sys.argv) > 4:
        A = open(sys.argv[3], 'rb').read()
        B = open(sys.argv[4], 'rb').read()
        m = min(len(A), len(B)) // 4096 * 4096
        d = [i for i in range(0, m, 4096) if A[i:i + 4096] != B[i:i + 4096]]
        print('audio: %.1fs em comum, blocos de 4KB diferentes: %d%s' % (
            m / 4 / 44100, len(d), (' (primeiro em %.2fs)' % (d[0] / 4 / 44100)) if d else ''))
        ok = ok and not d
    print('IDENTICOS' if ok else 'DIFERENTES')
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()
