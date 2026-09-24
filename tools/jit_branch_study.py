#!/usr/bin/env python3
"""Vies dos desvios condicionais e forma dos lacos (FC_JIT_DUMP, linhas C/B).

Uso: jit_branch_study.py jit-<pid>.txt

C code vaddr tomado caiu destino proximo -- contadores das saidas
condicionais. Responde se vale montar superblocos (seguir o lado quente) e
quanto dos desvios sao lacos (destino <= inicio do bloco) ou saltos curtos
para frente (pulam poucas instrucoes, candidatos a execucao condicional).
"""
import collections
import sys

last = {}
size = {}
for l in open(sys.argv[1], errors='replace'):
    if l[0] == 'B':
        p = l.split()
        size[(p[1], p[2])] = int(p[4])      # bytes SH4
    elif l[0] == 'C':
        p = l.split()
        last[(p[1], p[2])] = (int(p[3]), int(p[4]), int(p[5], 16), int(p[6], 16), int(p[2], 16))
tot = 0
bias = collections.Counter()
kinds = collections.Counter()
hot = []
for key, (t, n, br, nx, va) in last.items():
    e = t + n
    if e == 0:
        continue
    tot += e
    m = max(t, n) / e
    for thr in (0.9, 0.99, 0.999):
        if m >= thr:
            bias[thr] += e
    sh = size.get(key, 0)
    end = va + sh
    if br <= va:
        k = 'laco (destino <= inicio do bloco)'
    elif br - end <= 8:
        k = 'salto curto p/ frente (pula <= 4 instr)'
    else:
        k = 'outro'
    kinds[k] += e
    hot.append((e, va, t, n, br, nx, k))
print('saidas condicionais executadas: %.1f M em %d blocos' % (tot / 1e6, len(last)))
for thr in (0.9, 0.99, 0.999):
    print('  lado dominante >= %.1f%%: %5.1f%% das execucoes' % (thr * 100, 100.0 * bias[thr] / tot))
print('forma:')
for k, v in kinds.most_common():
    print('  %5.1f%%  %s' % (100.0 * v / tot, k))
hot.sort(reverse=True)
print('mais executadas:')
for e, va, t, n, br, nx, k in hot[:15]:
    print('  %08X  %6.2f%% das saidas  tomado %5.1f%%  -> %08X / %08X  %s' % (va, 100.0 * e / tot, 100.0 * t / e, br, nx, k))
