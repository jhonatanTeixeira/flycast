#!/usr/bin/env python3
"""Simboliza /tmp/tinyprof-<pid>.txt (FC_TINYPROF=hz) com os modulos do build.

  tools/tinyprof.py <core.so> <samples> [--so-dir DIR] [--top 30]

O arquivo traz PCs crus + /proc/self/maps (linhas "#map"). O script separa as
amostras por modulo (core, libc, libmali/driver, JIT...) e resolve o simbolo
mais proximo por baixo. Com --so-dir, simboliza tambem libc/libmali/etc.
copiados do device (mesmo arquivo do device: os PCs sao casados por offset).
"""
import argparse
import bisect
import collections
import os
import re
import subprocess


def load_syms(so):
    out = subprocess.run(['nm', '-C', '--defined-only', '-n', so],
                         capture_output=True, text=True, errors='replace').stdout
    syms = []
    for line in out.splitlines():
        p = line.split(' ', 2)
        if len(p) < 3:
            continue
        try:
            addr = int(p[0], 16)
        except ValueError:
            continue
        typ, name = p[1], p[2]
        if typ.lower() not in ('t', 'w', 'v'):
            continue
        if name.startswith('$'):
            continue
        syms.append((addr, name))
    syms.sort()
    return syms


def load_dynsyms(so):
    out = subprocess.run(['nm', '-D', '-C', '--defined-only', '-n', so],
                         capture_output=True, text=True, errors='replace').stdout
    syms = []
    for line in out.splitlines():
        p = line.split(' ', 2)
        if len(p) < 3:
            continue
        try:
            addr = int(p[0], 16)
        except ValueError:
            continue
        if p[1].lower() not in ('t', 'i', 'w'):
            continue
        if p[2].startswith('$'):
            continue
        syms.append((addr, p[2]))
    syms.sort()
    return syms


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('so')
    ap.add_argument('samples')
    ap.add_argument('--so-dir', default=None)
    ap.add_argument('--top', type=int, default=30)
    a = ap.parse_args()

    symtabs = {'core': (load_syms(a.so), [s[0] for s in load_syms(a.so)])}
    if a.so_dir:
        for fn in os.listdir(a.so_dir):
            if not fn.endswith('.so') and '.so.' not in fn:
                continue
            path = os.path.join(a.so_dir, fn)
            try:
                syms = load_dynsyms(path)
            except Exception:
                continue
            if syms:
                symtabs[fn] = (syms, [s[0] for s in syms])

    maps = []       # (lo, hi, off, path)
    pcs = []
    base = None
    for line in open(a.samples):
        line = line.strip()
        if line.startswith('# tinyprof'):
            m = re.search(r'base ([0-9a-f]+)', line)
            if m:
                base = int(m.group(1), 16)
            continue
        if line.startswith('#map '):
            parts = line[5:].split()
            if len(parts) < 6:
                continue
            lo, hi = [int(x, 16) for x in parts[0].split('-')]
            off = int(parts[2], 16)
            path = ' '.join(parts[5:])
            maps.append((lo, hi, off, path))
            continue
        if not line or line.startswith('#'):
            continue
        try:
            pcs.append(int(line, 16))
        except ValueError:
            pass

    def find(pc):
        for lo, hi, off, path in maps:
            if lo <= pc < hi:
                return lo, off, path
        return None

    bymod = collections.Counter()
    hist = collections.Counter()
    total = len(pcs)
    for pc in pcs:
        m = find(pc)
        if m is None:
            bymod['?'] += 1
            continue
        lo, off, path = m
        name = path.rsplit('/', 1)[-1]
        key = 'core' if 'flycast' in name else name
        bymod[key] += 1
        tab = symtabs.get(key) if key == 'core' else symtabs.get(name)
        if tab is None:
            continue
        syms, addrs = tab
        fo = pc - lo + off
        i = bisect.bisect_right(addrs, fo) - 1
        if i >= 0:
            hist['%s|%s' % (key, syms[i][1])] += 1
    print('amostras %d' % total)
    print('--- por modulo')
    for name, n in bymod.most_common(12):
        print('%6.2f%%  %6d  %s' % (100.0 * n / max(1, total), n, name))
    print('--- simbolos (%% do total)')
    for name, n in hist.most_common(a.top):
        print('%6.2f%%  %6d  %s' % (100.0 * n / max(1, total), n, name))


if __name__ == '__main__':
    main()
