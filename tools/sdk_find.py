#!/usr/bin/env python3
"""Varredura completa de funções repetidas ou parecidas entre jogos de Dreamcast, a
partir dos dumps do JIT que já existem (docs/native_sdk_code.md, 4.125).

  sdk_find.py <saida_dir> <dump_dir...>

Cada <dump_dir> é uma pasta de sessão (jit-*.txt, rom.txt, samples.txt.gz opcional).
Só entram ROMs de Dreamcast (.chd). Um dump por jogo: o primeiro da lista que tiver
amostras do perf; senão o de mais blocos.

Algoritmo
 1. Funções: mapa endereço→opcode (versão mais recente de cada bloco) e descida
    recursiva a partir dos alvos de `bsr` e dos inícios de bloco não cobertos, seguindo
    bra/bt/bf (+ slots), parando em rts/rte/jmp/braf; chamadas continuam depois do slot.
 2. Normalização: o opcode com os campos de deslocamento zerados (literal PC-relativo,
    mova, desvios); registradores e imediatos ficam.
 3. Iguais: hash da sequência normalizada (em ordem de endereço).
 4. Parecidas: índice invertido de 6-gramas normalizados (ignora os comuns demais) →
    pares candidatos → contenção (interseção / menor) ≥ 0,8, porque o dump só tem o
    trecho executado em cada jogo. Segunda visão, imune à ordem das instruções:
    por bloco, o multiconjunto das operações SHIL (registradores renomeados,
    endereços mascarados) → contenção ≥ 0,85 com ≥ 3 blocos.
 5. Grupos: união dos pares; só grupos com 2+ jogos. Ranking por tempo amostrado
    (perf) somado nos jogos que têm amostra, depois por número de jogos.
"""
import collections
import glob
import gzip
import os
import re
import subprocess
import sys
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import sh4dis  # noqa: E402

# funções que já rodam em nativo (hle_fn.cpp): o tempo delas não aparece nos blocos do
# JIT, então ficariam no fim do ranking; são marcadas e listadas à parte
KNOWN_NATIVE = [('Napple', 0x8C14DDC0, 'lightxf'), ('Napple', 0x8C14D440, 'stripemit'),
                ('Dead or Alive', 0x8C101BC4, 'doa2 (laço de vértices)')]

MIN_INSTR = 8
NGRAM = 6
DF_MAX = 60          # n-grama em mais funções que isso é idioma comum (prólogo etc.)
J_SH4 = 0.8         # contenção mínima dos n-gramas SH4
J_SHIL = 0.85       # contenção mínima dos blocos SHIL
MIN_GRAMS = 8       # o menor lado precisa ter pelo menos isso de n-gramas


# ---------------------------------------------------------------- leitura do dump

def rom_of(d):
    try:
        return open(os.path.join(d, 'rom.txt'), errors='ignore').read().strip()
    except OSError:
        return ''


def load_dump(path):
    """mem: endereço→opcode (último bloco compilado ganha); blocks: início→(ops, shil)"""
    mem, blocks = {}, {}
    pc = None
    ops, shil = None, None
    for line in open(path, errors='ignore'):
        c = line[:2]
        if c == 'B ':
            if pc is not None and ops is not None:
                blocks[pc] = (ops, shil)
            pc = int(line.split()[2], 16)
            ops, shil = None, []
        elif c == 'G ' and pc is not None:
            ops = [int(x, 16) for x in line.split()[1:]]
            for i, op in enumerate(ops):
                mem[pc + 2 * i] = op
        elif c == 'O ' and shil is not None:
            shil.append(line.rstrip('\n'))
    if pc is not None and ops is not None:
        blocks[pc] = (ops, shil)
    return mem, blocks


# ---------------------------------------------------------------- SH4: fluxo e forma

def sext(v, b):
    return v - (1 << b) if v & (1 << (b - 1)) else v


def flow(op, pc):
    """(tipo, alvo) — tipo: 'br' desvio incondicional, 'cb' condicional, 'cbs'
    condicional com slot, 'call' chamada com alvo, 'icall' chamada indireta,
    'ret' fim, None instrução comum"""
    hi = op >> 12
    if hi == 0xA:
        return 'br', pc + 4 + sext(op & 0xFFF, 12) * 2
    if hi == 0xB:
        return 'call', pc + 4 + sext(op & 0xFFF, 12) * 2
    if op & 0xFF00 in (0x8900, 0x8B00):
        return 'cb', pc + 4 + sext(op & 0xFF, 8) * 2
    if op & 0xFF00 in (0x8D00, 0x8F00):
        return 'cbs', pc + 4 + sext(op & 0xFF, 8) * 2
    if op in (0x000B, 0x002B) or op & 0xF0FF in (0x402B, 0x0023):
        return 'ret', None
    if op & 0xF0FF in (0x400B, 0x0003):
        return 'icall', None
    return None, None


def norm(op):
    """opcode com os deslocamentos zerados (o que muda quando o código muda de lugar)"""
    hi = op >> 12
    if hi in (0xA, 0xB):
        return op & 0xF000
    if hi in (0x9, 0xD):                 # mov.w/mov.l @(disp,PC),Rn
        return op & 0xFF00
    if op & 0xFF00 in (0xC700, 0x8900, 0x8B00, 0x8D00, 0x8F00):
        return op & 0xFF00
    return op


def functions(mem, blocks):
    """entrada → lista ordenada de endereços da função"""
    entries = set()
    for a, op in mem.items():
        k, t = flow(op, a)
        if k == 'call' and t in mem:
            entries.add(t)
    funcs = {}
    covered = set()

    def descend(e):
        seen, work = set(), [e]
        while work and len(seen) < 4000:
            pc = work.pop()
            while pc in mem and pc not in seen:
                seen.add(pc)
                k, t = flow(mem[pc], pc)
                if k is None:
                    pc += 2
                    continue
                if k == 'cb':
                    work.append(t)
                    pc += 2
                    continue
                seen.add(pc + 2) if pc + 2 in mem else None      # slot
                if k == 'br':
                    work.append(t)
                    break
                if k == 'cbs':
                    work.append(t)
                    pc += 4
                    continue
                if k in ('call', 'icall'):
                    pc += 4
                    continue
                break                                         # ret
        return sorted(seen)

    # cada endereço pertence a uma função só: entrada já coberta (2ª entrada, `bsr`
    # para o meio, cauda compartilhada) não vira função nova, e a descida para no
    # código de uma função anterior (não conta o mesmo bloco duas vezes)
    for e in sorted(entries) + sorted(blocks):
        if e in covered:
            continue
        f = [a for a in descend(e) if a not in covered]
        funcs[e] = f
        covered.update(f)
    return funcs


# ---------------------------------------------------------------- SHIL normalizado

ADDR = re.compile(r'0x(8c|ac|0c|a0|e0|ff)[0-9a-f]{6}')
REG = re.compile(r'\b(r\d+|fr\d+|xf\d+|sr\.T|pr|macl|mach|fpul|gbr|pc_dyn|jdyn)\.\d+\b')


def shil_block_token(lines):
    """multiconjunto das operações do bloco, sem ordem, registradores sem versão"""
    toks = []
    for l in lines:
        p = l.split(None, 8)
        if len(p) < 9:
            continue
        body = ADDR.sub('ADDR', p[8])
        body = REG.sub(lambda m: m.group(1), body)
        toks.append(body)
    toks.sort()
    return zlib.crc32('|'.join(toks).encode())


# ---------------------------------------------------------------- perf

def perf_by_block(d, cache):
    samples = os.path.join(d, 'samples.txt.gz')
    jits = glob.glob(os.path.join(d, 'jit-*.txt'))
    if not os.path.exists(samples) or not jits:
        return {}
    out = os.path.join(cache, os.path.basename(d) + '.rep')
    if not os.path.exists(out):
        with open(out, 'w') as fh:
            subprocess.run([sys.executable, '-I', os.path.join(HERE, 'jit_lite_report.py'),
                            '--top', '1000000', jits[0], samples], stdout=fh,
                           stderr=subprocess.DEVNULL, timeout=1800)
    res = {}
    for l in open(out):
        m = re.match(r'\s+([0-9A-F]{8})\s+([\d.]+)%', l)
        if m:
            res[int(m.group(1), 16)] = res.get(int(m.group(1), 16), 0) + float(m.group(2))
    return res


# ---------------------------------------------------------------- nome legível

def hint(ops):
    txt = ' '.join(sh4dis.dis(op, 0).split()[0] for op in ops)
    tags = []
    has = lambda w: w in txt
    if has('ftrv'): tags.append('matriz')
    if has('fipr'): tags.append('produto_escalar')
    if has('fsrra') or has('fsqrt'): tags.append('raiz')
    if has('fdiv'): tags.append('divisao')
    if has('tas.b'): tags.append('trava')
    if has('rte') or 'SSR' in ' '.join(sh4dis.dis(o, 0) for o in ops): tags.append('contexto')
    if has('ocbp') or has('ocbi') or has('ocbwb'): tags.append('cache')
    if has('pref'): tags.append('pref')
    if has('braf'): tags.append('switch')
    if has('trapa'): tags.append('trap')
    fpu = sum(1 for op in ops if op >> 12 == 0xF)
    if not tags and fpu * 3 > len(ops): tags.append('float')
    loads = sum(1 for op in ops if (op >> 12) == 6 and (op & 0xF) in (0, 1, 2, 4, 5, 6))
    stores = sum(1 for op in ops if (op >> 12) == 2 and (op & 0xF) in (0, 1, 2, 4, 5, 6))
    loop = any(flow(op, 0)[0] in ('cb', 'cbs') and sext(op & 0xFF, 8) < 0 for op in ops)
    if loop and not tags:
        tags.append('copia' if loads and stores else 'preenche' if stores else 'laco')
    if has('dt') and not tags: tags.append('contador')
    return '_'.join(tags[:3]) or 'geral'


# ---------------------------------------------------------------- principal

class UF:
    def __init__(self, n): self.p = list(range(n))
    def f(self, x):
        while self.p[x] != x:
            self.p[x] = self.p[self.p[x]]
            x = self.p[x]
        return x
    def u(self, a, b):
        a, b = self.f(a), self.f(b)
        if a != b: self.p[max(a, b)] = min(a, b)


def main():
    outdir, dirs = sys.argv[1], sys.argv[2:]
    sh4dis.load_table()
    os.makedirs(outdir, exist_ok=True)
    cache = os.environ.get('SDK_FIND_CACHE', os.path.join(outdir, '.cache'))
    os.makedirs(cache, exist_ok=True)

    # um dump por jogo de Dreamcast
    pick = {}
    for d in dirs:
        rom = rom_of(d)
        jits = glob.glob(os.path.join(d, 'jit-*.txt'))
        if not rom.lower().endswith('.chd') or not jits or os.path.getsize(jits[0]) < 100000:
            continue
        game = os.path.splitext(os.path.basename(rom))[0]
        has_perf = os.path.exists(os.path.join(d, 'samples.txt.gz'))
        key = (has_perf, os.path.getsize(jits[0]))
        if game not in pick or key > pick[game][0]:
            pick[game] = (key, d, jits[0])

    F = []            # (jogo, entrada, endereços, opcodes, blocos)
    perf = {}
    for game, (_, d, jit) in sorted(pick.items()):
        mem, blocks = load_dump(jit)
        pb = perf_by_block(d, cache)
        perf[game] = bool(pb)
        for e, addrs in functions(mem, blocks).items():
            if len(addrs) < MIN_INSTR:
                continue
            ops = [mem[a] for a in addrs]
            bl = [a for a in addrs if a in blocks]
            t = sum(pb.get(b, 0) for b in bl)
            F.append(dict(game=game, entry=e, addrs=addrs, ops=ops, dump=jit,
                          shil=[shil_block_token(blocks[b][1]) for b in bl], perf=t))
        sys.stderr.write('%s: %d funcoes\n' % (game, sum(1 for f in F if f['game'] == game)))

    n = len(F)
    uf = UF(n)
    # 3. iguais
    exact = collections.defaultdict(list)
    for i, f in enumerate(F):
        f['norm'] = [norm(o) for o in f['ops']]
        exact[zlib.crc32(bytes(x & 0xFF for x in f['norm']) + bytes(x >> 8 for x in f['norm']))].append(i)
    for ids in exact.values():
        for j in ids[1:]:
            uf.u(ids[0], j)
    # 4. parecidas: índice invertido de n-gramas
    def grams(seq, k):
        return {zlib.crc32(repr(seq[i:i + k]).encode()) for i in range(len(seq) - k + 1)}
    G = [grams(f['norm'], NGRAM) for f in F]
    S = [collections.Counter(f['shil']) for f in F]
    inv = collections.defaultdict(list)
    for i, g in enumerate(G):
        for h in g:
            inv[h].append(i)
    cand = collections.Counter()
    for h, ids in inv.items():
        if len(ids) > DF_MAX:
            continue
        for a in range(len(ids)):
            for b in range(a + 1, len(ids)):
                cand[(ids[a], ids[b])] += 1
    pairs = 0
    for (a, b), shared in cand.items():
        if F[a]['game'] == F[b]['game'] and F[a]['entry'] == F[b]['entry']:
            continue
        # contenção (interseção / menor), não Jaccard: o dump só tem o trecho que cada
        # jogo executou, então uma função inteira e um pedaço dela precisam casar
        small = min(len(G[a]), len(G[b]))
        ja = len(G[a] & G[b]) / small if small >= MIN_GRAMS else 0.0
        js = 0.0
        if min(len(F[a]['shil']), len(F[b]['shil'])) >= 3:
            inter = sum((S[a] & S[b]).values())
            js = inter / min(sum(S[a].values()), sum(S[b].values()))
        if ja >= J_SH4 or js >= J_SHIL:
            uf.u(a, b)
            pairs += 1
    sys.stderr.write('%d funcoes, %d pares parecidos\n' % (n, pairs))

    groups = collections.defaultdict(list)
    for i in range(n):
        groups[uf.f(i)].append(i)
    out = []
    for ids in groups.values():
        games = sorted({F[i]['game'] for i in ids})
        if len(games) < 2:
            continue
        t = sum(F[i]['perf'] for i in ids)
        rep = max(ids, key=lambda i: (len(F[i]['ops']), F[i]['perf']))
        variants = len({tuple(F[i]['norm']) for i in ids})
        native = sorted({nm for i in ids for g, a, nm in KNOWN_NATIVE
                         if g in F[i]['game'] and a in set(F[i]['addrs'])})
        out.append(dict(ids=ids, games=games, perf=t, rep=rep, variants=variants, native=native))
    out.sort(key=lambda g: (not g['native'], -g['perf'], -len(g['games']), -len(F[g['rep']]['ops'])))

    write_report(outdir, F, out, perf)


def write_report(outdir, F, groups, perf):
    lines = ['# Descoberta automática de funções repetidas (Dreamcast)', '',
             '> Gerado por `tools/sdk_find.py` a partir dos dumps do JIT em `/mnt/1TB` '
             '(sem jogar de novo). Método e limites no cabeçalho do script e em '
             '`docs/native_sdk_code.md`.', '',
             'Jogos (um dump cada; * = tem amostras do perf, entra no tempo):', '']
    for g in sorted(perf):
        lines.append('- %s%s' % (g, ' *' if perf[g] else ''))
    lines += ['', 'Grupos com a mesma função (ou muito parecida) em 2+ jogos: **%d**. '
              'Os 60 primeiros têm listagem própria em `auto/`. Os grupos que já têm versão '
              'nativa vêm primeiro (nome `nativo_*`): o tempo deles roda em C++, fora dos '
              'blocos do JIT, e por isso aparece ~0. O tempo só existe para os jogos com `*`; '
              'nos outros a coluna conta 0.' % len(groups), '',
              '| # | Nome | Jogos | Instr. | Variantes | Tempo perf (% emu, soma) | Exemplo |',
              '|---|---|---|---|---|---|---|']
    os.makedirs(os.path.join(outdir, 'auto'), exist_ok=True)
    for k, g in enumerate(groups):
        r = F[g['rep']]
        name = '%03d_%s' % (k + 1, ('nativo_' + g['native'][0].split()[0]) if g['native'] else hint(r['ops']))
        link = '[%s](auto/%s.md)' % (name, name) if k < 60 else name
        lines.append('| %d | %s | %d | %d | %d | %.2f | %s `%08X` |' % (
            k + 1, link, len(g['games']), len(r['ops']), g['variants'], g['perf'],
            r['game'][:28], r['entry']))
        if k < 60:
            write_group(os.path.join(outdir, 'auto', name + '.md'), name, F, g)
    open(os.path.join(outdir, 'descoberta.md'), 'w').write('\n'.join(lines) + '\n')


def write_group(path, name, F, g):
    out = ['# %s' % name, '',
           '> Gerado por `tools/sdk_find.py`. Jogos: %d · variantes (sequências '
           'normalizadas distintas): %d · tempo perf somado: %.2f%% da emu.' % (
               len(g['games']), g['variants'], g['perf']), '',
           '| Jogo | Entrada | Instr. | Tempo perf |', '|---|---|---|---|']
    mem_by = sorted(g['ids'], key=lambda i: (F[i]['game'], F[i]['entry']))
    for i in mem_by:
        f = F[i]
        out.append('| %s | `%08X` | %d | %.2f%% |' % (f['game'], f['entry'], len(f['ops']), f['perf']))
    shown = set()
    for i in mem_by:
        f = F[i]
        key = tuple(f['norm'])
        if key in shown:
            continue                     # uma listagem por variante
        shown.add(key)
        out += ['', '## %s `%08X`' % (f['game'], f['entry']), '', 'Dump: `%s`' % f['dump'], '', '```']
        prev = None
        for a, op in zip(f['addrs'], f['ops']):
            if prev is not None and a != prev + 2:
                out.append('  ...')
            out.append('  %08X  %04X  %s' % (a, op, sh4dis.dis(op, a)))
            prev = a
        out.append('```')
    open(path, 'w').write('\n'.join(out) + '\n')


if __name__ == '__main__':
    main()
