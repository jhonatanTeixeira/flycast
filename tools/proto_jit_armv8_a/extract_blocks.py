#!/usr/bin/env python3
"""Extrai de um dump FC_JIT_DUMP o codigo ARM64 de um conjunto de blocos para
blocks.json (entrada do gen_cur.py).

Uso: extract_blocks.py jit-<pid>.txt blocks.json VADDR [VADDR...]

Por bloco: corpo = bytes do H desde o `subs w27` (checagem de ciclos) ate o
inicio do relink (E); saida = desmontagem do relink; destinos pelos
registros C (condicional) ou pelo desvio estatico decodificado do G.
Recusa corpo com desvio relativo para fora dele (alem do caminho frio do
agendador, que o harness nunca executa com ciclos de sobra).
"""
import json
import subprocess
import sys
import tempfile

sys.path.insert(0, __file__.rsplit('/', 3)[0] + '/tools')
from region_study import branch_info  # noqa: E402

OBJDUMP = 'aarch64-linux-gnu-objdump'


def disasm(hexs):
    with tempfile.NamedTemporaryFile(suffix='.bin') as f:
        f.write(bytes.fromhex(hexs))
        f.flush()
        out = subprocess.run([OBJDUMP, '-b', 'binary', '-m', 'aarch64', '-D', f.name],
                             capture_output=True, text=True).stdout
    ins = []
    for l in out.splitlines():
        p = l.split('\t')
        if len(p) >= 3 and p[0].strip().endswith(':'):
            ins.append((int(p[0].strip()[:-1], 16), '\t'.join(p[2:]).strip()))
    return ins


def main():
    dump, outp, want = sys.argv[1], sys.argv[2], [v.upper() for v in sys.argv[3:]]
    blk = {}
    cond = {}
    cur = None
    for l in open(dump, errors='replace'):
        t = l[0]
        if t == 'B':
            p = l.split()
            cur = p[2] if p[2] in want and p[2] not in blk else None
            if cur:
                blk[cur] = {'host': p[1], 'sh4_bytes': int(p[4]), 'cycles': int(p[6])}
        elif cur and t == 'G':
            blk[cur]['ops'] = [int(x, 16) for x in l.split()[1:]]
        elif cur and t == 'E':
            p = l.split()
            blk[cur]['relink'] = int(p[3])
        elif cur and t == 'H':
            blk[cur]['hexall'] = l.split()[1]
        elif t == 'C':
            p = l.split()
            if p[2] in blk and p[1] == blk[p[2]]['host']:
                cond[p[2]] = [p[5], p[6]]
    out = {}
    for va in want:
        b = blk[va]
        ins = disasm(b['hexall'])
        start = next(o for o, s in ins if s.startswith('subs\tw27, w27'))
        rl = b['relink']
        body = b['hexall'][2 * start:2 * rl]
        cold = next((o for o, s in ins if o > start and s.startswith('b.pl')), None)
        for o, s in ins:
            if start <= o < rl and (s.startswith('bl\t') or s.startswith('b\t')):
                tgt = int(s.split()[1], 16)
                if not (start <= tgt <= rl) and o > (cold or 0) + 7 * 4:
                    raise SystemExit('%s: desvio para fora do corpo em +%d: %s' % (va, o, s))
        k, tgt = branch_info(int(va, 16), b['ops'])
        e = {'cycles': b['cycles'], 'hex': body,
             'exit': [s for o, s in ins if o >= rl]}
        if va in cond:
            e['cond'] = cond[va]
        elif k == 'bra':
            e['next'] = '%08X' % tgt
        elif k == 'queda':
            e['next'] = '%08X' % (int(va, 16) + b['sh4_bytes'])
        else:
            raise SystemExit('%s: fim de bloco %s nao suportado' % (va, k))
        out[va] = e
    json.dump(out, open(outp, 'w'), indent=1)
    for va, e in out.items():
        print(va, len(e['hex']) // 2, 'bytes', e.get('cond') or e.get('next'), 'ciclos', e['cycles'])


if __name__ == '__main__':
    main()
