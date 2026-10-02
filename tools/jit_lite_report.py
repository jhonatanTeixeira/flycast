#!/usr/bin/env python3
"""Tempo por bloco do JIT a partir do dumper leve (FC_JIT_DUMP_LITE) + perf.

Entrada:
  jit-<pid>.txt   FC_JIT_DUMP=<dir> FC_JIT_DUMP_LITE=1 (linhas B = bloco
                  compilado com endereco do host; t = relogio; Z = limpeza)
  samples.txt     perf record -F 999 -k mono -p <pid>; depois
                  perf script -F comm,tid,time,ip,sym --ns > samples.txt

O cache de codigo e um array dentro do .so (simbolo SH4_TCB), entao o perf
nao separa os blocos: a resolucao amostra -> bloco e feita aqui, pelo
endereco do host e pelo instante (limpezas de cache invalidam os blocos).

Uso:
  jit_lite_report.py jit-<pid>.txt samples.txt [--top 30]
                     [--window INI FIM]      segundos desde a 1a amostra
                     [--at HH:MM:SS --span S] janela centrada na hora local
"""
import argparse
import bisect
import collections
import datetime
import re


def parse_dump(path):
    """Blocos como (inicio_ns, fim_ns, code, end, vaddr, sh4b, temp)."""
    blocks = []
    live = []             # indices de blocos vivos
    last_t = 0
    clock = None          # (mono_ns, realtime_ns) da 1a linha t
    with open(path, errors='replace') as f:
        for line in f:
            c = line[0]
            if c == 'B':
                p = line.split()
                flags = dict(x.split('=') for x in p[8:] if '=' in x)
                code = int(p[1], 16)
                blocks.append([last_t, None, code, code + int(p[7]), int(p[2], 16),
                               int(p[4]), flags.get('temp') == '1'])
                live.append(len(blocks) - 1)
            elif c == 't':
                p = line.split()
                last_t = int(p[1])
                if clock is None and len(p) > 2:
                    clock = (int(p[1]), int(p[2]))
            elif c == 'Z':
                only_temp = line.startswith('Z temp')
                keep = []
                for i in live:
                    if only_temp and not blocks[i][6]:
                        keep.append(i)
                    else:
                        blocks[i][1] = last_t
                live = keep
    return blocks, clock


class Resolver:
    def __init__(self, blocks):
        self.blocks = sorted(blocks, key=lambda b: b[2])
        self.starts = [b[2] for b in self.blocks]

    def find(self, ip, ts):
        best = None
        i = bisect.bisect_right(self.starts, ip) - 1
        # varre para tras os que comecam antes do ip (blocos se sobrepoem
        # entre limpezas); fica com o compilado mais recente vivo em ts
        n = 0
        while i >= 0 and n < 64:
            b = self.blocks[i]
            if b[2] <= ip < b[3] and b[0] <= ts and (b[1] is None or ts < b[1]):
                if best is None or b[0] > best[0]:
                    best = b
            i -= 1
            n += 1
        return best


SAMPLE = re.compile(r'^\s*(\S+)\s+(\d+)\s+([\d.]+):\s+([0-9a-f]+)\s*(.*)$')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('dump')
    ap.add_argument('samples')
    ap.add_argument('--top', type=int, default=30)
    ap.add_argument('--window', nargs=2, type=float)
    ap.add_argument('--at')
    ap.add_argument('--span', type=float, default=10.0)
    ap.add_argument('--log', help='live.log do retrorun (regioes do tier2, em ordem de emissao)')
    ap.add_argument('--tcb-off', default='0x3a2568', help='offset do simbolo SH4_TCB no .so (nm)')
    a = ap.parse_args()

    blocks, clock = parse_dump(a.dump)
    so_base = None
    with open(a.dump, errors='replace') as f:
        for line in f:
            if line.startswith('M so_base'):
                so_base = int(line.split()[2], 16)
                break
    # layout do cache (blockmanager.h / driver.cpp / tier2.cpp): SH4_TCB alinhado a
    # 4 KB; [0, 14 MB) blocos; [14, 15 MB) tier2 (T2_AREA = 1 MB); [15, 16 MB) temp
    CODE_SIZE, T2_AREA = 15 << 20, 1 << 20
    cc = ((so_base + int(a.tcb_off, 16) + 4095) & ~4095) if so_base else None
    regions = []    # (inicio, fim, id, blocos) dentro da area do tier2
    if a.log and cc:
        pos = cc + CODE_SIZE - T2_AREA
        rx = re.compile(r'regiao #(\d+): .*?, (\d+) bytes.*blocos: (.*)$')
        for line in open(a.log, errors='replace'):
            m = rx.search(line)
            if m:
                size = int(m.group(2))
                regions.append((pos, pos + size, int(m.group(1)), m.group(3).split()[:6]))
                pos += size
    res = Resolver(blocks)

    rows = []
    with open(a.samples, errors='replace') as f:
        for line in f:
            m = SAMPLE.match(line)
            if m:
                rows.append((int(m.group(2)), int(float(m.group(3)) * 1e9), int(m.group(4), 16),
                             m.group(5).strip()))
    if not rows:
        raise SystemExit('sem amostras')
    t0 = rows[0][1]
    lo, hi = None, None
    if a.window:
        lo, hi = t0 + a.window[0] * 1e9, t0 + a.window[1] * 1e9
    elif a.at:
        if clock is None:
            raise SystemExit('dump sem linha t com relogio')
        day = datetime.datetime.fromtimestamp(clock[1] / 1e9)
        hh, mm, ss = (int(x) for x in a.at.split(':'))
        target = day.replace(hour=hh, minute=mm, second=ss).timestamp() * 1e9
        center = clock[0] + (target - clock[1])
        lo, hi = center - a.span / 2 * 1e9, center + a.span / 2 * 1e9
    if lo is not None:
        rows = [r for r in rows if lo <= r[1] < hi]
    if not rows:
        raise SystemExit('nenhuma amostra na janela')
    dur = (rows[-1][1] - rows[0][1]) / 1e9 or 1.0

    per_tid = collections.Counter(r[0] for r in rows)
    jit_tid = collections.Counter(r[0] for r in rows if r[3].startswith('SH4_TCB'))
    emu = jit_tid.most_common(1)[0][0] if jit_tid else per_tid.most_common(1)[0][0]

    print('janela: %.1f s, %d amostras (perf a ~999 Hz por thread)' % (dur, len(rows)))
    print('\n--- Threads (amostras ~ ms de CPU; %% de um core) ---')
    for tid, n in per_tid.most_common(8):
        tag = ' <- emu (SH4 JIT)' if tid == emu else ''
        print('  tid %-7d %6d  %5.1f%% de um core%s' % (tid, n, 100.0 * n / (dur * 999), tag))

    emu_rows = [r for r in rows if r[0] == emu]
    by_block = collections.Counter()
    by_sym = collections.Counter()
    info = {}
    unresolved = 0
    unres_kind = collections.Counter()
    for tid, ts, ip, sym in emu_rows:
        if sym.startswith('SH4_TCB'):
            b = res.find(ip, ts)
            if b is None:
                unresolved += 1
                if cc:
                    off = ip - cc
                    if CODE_SIZE - T2_AREA <= off < CODE_SIZE:
                        tag = 'tier2 (area)'
                        for r0, r1, rid, rb in regions:
                            if r0 <= ip < r1:
                                tag = 'tier2 regiao #%d (%s)' % (rid, ' '.join(rb))
                                break
                    elif CODE_SIZE <= off < CODE_SIZE + (1 << 20):
                        tag = 'cache temporario'
                    else:
                        tag = 'stubs/despachante (cache principal, fora de bloco)'
                    unres_kind[tag] += 1
                continue
            by_block[b[4]] += 1
            info[b[4]] = b
        else:
            by_sym[sym if sym and sym != '[unknown]' else ('kernel' if ip >= 0xffff000000000000 else '?')] += 1
    n_emu = len(emu_rows)
    n_jit = sum(by_block.values())
    print('\n--- Thread de emulacao: %d amostras ---' % n_emu)
    print('  codigo do JIT (blocos)        %5.1f%%' % (100.0 * n_jit / n_emu))
    print('  JIT sem bloco (tier2/stubs)   %5.1f%%' % (100.0 * unresolved / n_emu))
    print('  resto (C++, kernel, libs)     %5.1f%%' % (100.0 * sum(by_sym.values()) / n_emu))

    if unres_kind:
        print('\n--- JIT sem bloco, por lugar ---')
        for k, n in unres_kind.most_common(12):
            print('  %5.2f%%  %s' % (100.0 * n / n_emu, k))
    print('\n--- Blocos mais caros (tempo real, %% da thread de emulacao) ---')
    acc = 0
    for va, n in by_block.most_common(a.top):
        acc += n
        print('  %08X  %5.2f%%  acum %5.1f%%  instr SH4=%d' % (va, 100.0 * n / n_emu, 100.0 * acc / n_emu, info[va][5] // 2))

    print('\n--- Fora do JIT na thread de emulacao ---')
    for s, n in by_sym.most_common(15):
        print('  %5.2f%%  %s' % (100.0 * n / n_emu, s[:90]))


if __name__ == '__main__':
    main()
