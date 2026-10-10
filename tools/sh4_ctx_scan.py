#!/usr/bin/env python3
"""Varre dumps do JIT (FC_JIT_DUMP / _LITE) atras do modelo de threads dos jogos
(docs/sh4_threading_model.md).

  sh4_ctx_scan.py scan <jit-*.txt...>            blocos com rte, ldc/stc SSR/SPC,
                                                 ldc SR, bancos, tas.b, sleep
  sh4_ctx_scan.py find "<op op ...>" <jit-*.txt...>
                                                 blocos cujo SH4 (linha G) contem a
                                                 sequencia de opcodes (hex, 4 digitos)

Assinaturas da biblioteca de threads (iguais nos 10 jogos que a usam):
  troca (salvamento):  "4F43 4F33 2FE6 2FE6 2FD6"   stc.l SPC; stc.l SSR; mov.l r14..
  yield:               "403E 002A 404E"             ldc r0,SSR; sts PR,r0; ldc r0,SPC
"""
import collections
import re
import sys

PATS = {
    'rte': r'002B', 'sleep': r'001B',
    'ldcSSR': r'4[0-9A-F]3E', 'ldcSPC': r'4[0-9A-F]4E',
    'stcSSR': r'0[0-9A-F]32', 'stcSPC': r'0[0-9A-F]42',
    'ldcSR': r'4[0-9A-F]0E', 'ldcBANK': r'4[0-9A-F][89A-F]E',
    'tas': r'4[0-9A-F]1B',
}


def game_of(path):
    # .../<AAAAMMDD-HHMMSS>_<jogo>/jit-<pid>.txt
    d = path.replace('\\', '/').split('/')[-2]
    return d.split('_', 1)[1][:30] if '_' in d else d


def blocks(path):
    pc = None
    for line in open(path, errors='ignore'):
        if line.startswith('B '):
            pc = line.split()[2]
        elif line.startswith('G ') and pc:
            yield pc, line.split()[1:]


def scan(files):
    pats = {k: re.compile('^' + v + '$') for k, v in PATS.items()}
    res = collections.defaultdict(lambda: collections.defaultdict(set))
    for f in files:
        g = game_of(f)
        for pc, ops in blocks(f):
            for k, p in pats.items():
                if any(p.match(o) for o in ops):
                    res[k][pc].add(g)
    for k in PATS:
        d = res[k]
        print(f'== {k}: {len(d)} blocos')
        for pc, gs in sorted(d.items(), key=lambda x: -len(x[1]))[:15]:
            print(f'   {pc} {len(gs):3d} jogos  {", ".join(sorted(gs)[:4])}')


def find(seq, files):
    needle = ' '.join(seq.upper().split())
    hits = collections.defaultdict(set)
    for f in files:
        g = game_of(f)
        for pc, ops in blocks(f):
            if needle in ' '.join(ops):
                hits[g].add(pc)
    for g, pcs in sorted(hits.items()):
        print(g, ' '.join(sorted(pcs)))


if __name__ == '__main__':
    if len(sys.argv) < 3 or sys.argv[1] not in ('scan', 'find'):
        print(__doc__)
        sys.exit(1)
    if sys.argv[1] == 'scan':
        scan(sys.argv[2:])
    else:
        find(sys.argv[2], sys.argv[3:])
