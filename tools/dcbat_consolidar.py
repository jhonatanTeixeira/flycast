#!/usr/bin/env python3
"""Consolida os dumps de JIT (FC_JIT_DUMP_LITE) + logs de uma bateria
num UNICO arquivo, com o SH4 de cada bloco desmontado.

Entrada: pasta com subpastas de captura (cada uma com jit-*.txt, live.log,
rom.txt, exit_code.txt). Saida: um arquivo so.

Uso: consolidar.py <dir_capturas> <arquivo_saida> [--limit-blocks N]
"""
import glob
import os
import re
import sys

REPO_TOOLS = '/media/jhonatanteixeira/Novo volume/projects/jhon/dreams/flycast/tools'
sys.path.insert(0, REPO_TOOLS)
import sh4dis  # noqa: E402


def parse_blocks(path):
    """Devolve [(vaddr, sh4_ops, arm64_bytes)] dos blocos do dump."""
    blocks = []
    cur = None
    for line in open(path, errors='replace'):
        c = line[0]
        if c == 'B':
            p = line.split()
            cur = {'va': int(p[2], 16), 'arm': int(p[7]), 'g': []}
            blocks.append(cur)
        elif c == 'G' and cur is not None:
            cur['g'] = [int(x, 16) for x in line.split()[1:]]
    return blocks


def error_lines(log):
    out = []
    for line in open(log, errors='replace'):
        if 'iNimp' in line or 'BAIL' in line:
            out.append(line.rstrip('\n'))
    return out


def main():
    srcdir, outpath = sys.argv[1], sys.argv[2]
    limit = 0
    if '--limit-blocks' in sys.argv:
        limit = int(sys.argv[sys.argv.index('--limit-blocks') + 1])

    sh4dis.load_table()
    folders = sorted(d for d in glob.glob(os.path.join(srcdir, '*'))
                     if os.path.isdir(d))
    total_blocks = 0
    with open(outpath, 'w') as out:
        out.write("# Consolidado da bateria: disassembly SH4 dos blocos do JIT + logs\n")
        out.write("# Fonte: %s\n\n" % srcdir)
        for d in folders:
            name = os.path.basename(d.rstrip('/'))
            rom = ''
            rp = os.path.join(d, 'rom.txt')
            if os.path.exists(rp):
                rom = open(rp, errors='replace').read().strip()
            ex = ''
            ep = os.path.join(d, 'exit_code.txt')
            if os.path.exists(ep):
                ex = open(ep, errors='replace').read().strip()
            jits = sorted(glob.glob(os.path.join(d, 'jit-*.txt')))
            log = os.path.join(d, 'live.log')
            out.write("\n" + "=" * 100 + "\n")
            out.write("## %s\n" % name)
            out.write("## rom: %s\n" % rom)
            out.write("## exit: %s\n" % ex)
            out.write("=" * 100 + "\n")

            # log (erros/BAIL)
            out.write("\n--- LOG (iNimp/BAIL) ---\n")
            if os.path.exists(log):
                for l in error_lines(log):
                    out.write(l + "\n")
            else:
                out.write("(sem live.log)\n")

            # disassembly
            for jf in jits:
                out.write("\n--- DISASSEMBLY SH4 (%s) ---\n" % os.path.basename(jf))
                blocks = parse_blocks(jf)
                out.write("# %d blocos no dump\n" % len(blocks))
                for b in blocks:
                    if limit and len(b['g']) > limit:
                        continue
                    n = len(b['g'])
                    out.write("\n=== %08X  sh4=%d instr  arm64=%d bytes (%.1f instr host/SH4)\n" % (
                        b['va'], n, b['arm'], b['arm'] / 4 / max(1, n)))
                    for i, op in enumerate(b['g']):
                        pc = b['va'] + 2 * i
                        out.write("  %08X  %04X  %s\n" % (pc, op, sh4dis.dis(op, pc)))
                    total_blocks += 1
    print("blocos desmontados:", total_blocks)


if __name__ == '__main__':
    main()
