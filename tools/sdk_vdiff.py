#!/usr/bin/env python3
"""Alinha as listagens de um documento de bloco (docs/sdk_blocks/*.md ou
docs/sdk_find/auto/*.md) contra uma referência e mostra onde cada variante difere.

  sdk_vdiff.py <doc.md> <trecho do título da referência> [trecho de outro título ...]

Cada seção `## ...` é uma listagem (jogo, ou jogo + entrada); linhas `; ` abrem outra
janela na mesma seção. Alinhamento: difflib.SequenceMatcher sobre os opcodes.
"""
import difflib
import re
import sys


def parse(path):
    secs, cur, win = {}, None, None
    for line in open(path):
        if line.startswith('## '):
            cur = line[3:].strip()
            win = []
            secs[cur] = [win]
        elif line.startswith('; ') and cur:
            win = []
            secs[cur].append(win)
        else:
            m = re.match(r'  ([0-9A-F]{8})  ([0-9A-F]{4})  (.*)', line.rstrip())
            if m and win is not None:
                win.append((int(m.group(1), 16), m.group(2), m.group(3)))
    return {k: [w for w in v if w] for k, v in secs.items() if any(v)}


def main():
    path, ref = sys.argv[1], sys.argv[2]
    only = sys.argv[3:]
    secs = parse(path)
    rname = next(n for n in secs if ref in n)
    rw = secs[rname][0]
    ops = lambda w: [x[1] for x in w]
    for name, ws in secs.items():
        if name == rname or (only and not any(o in name for o in only)):
            continue
        best = max(ws, key=lambda w: difflib.SequenceMatcher(None, ops(rw), ops(w), autojunk=False).ratio())
        sm = difflib.SequenceMatcher(None, ops(rw), ops(best), autojunk=False)
        print('\n##### %s  (ref: %s)  ratio=%.2f' % (name, rname, sm.ratio()))
        for tag, i1, i2, j1, j2 in sm.get_opcodes():
            if tag == 'equal':
                continue
            print('-- %s  ref[%d:%d] var[%d:%d]' % (tag, i1, i2, j1, j2))
            for x in rw[i1:i2][:16]:
                print('   R %08X %s %s' % x)
            for x in best[j1:j2][:16]:
                print('   V %08X %s %s' % x)


if __name__ == '__main__':
    main()
