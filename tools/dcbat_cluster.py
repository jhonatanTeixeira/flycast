#!/usr/bin/env python3
"""Acha padroes (trechos quase-identicos) entre os blocos SH4 do consolidado.

Metodo (nao usa rede neural):
  1. Normaliza a instrucao trocando operandos pelo TIPO (registrador vira `r`,
     imediato vira `#I`, endereco absoluto vira `A`, deslocamento vira `D`) --
     assim "mov.l @(24,pc),r2" e "mov.l @(32,pc),r5" viram o mesmo texto.
  2. n-gramas (n=4) da sequencia normalizada -> conjunto.
  3. MinHash (k hashes) + LSH (bandas) -> pares candidatos -> Jaccard real.
  4. Clusteriza por union-find e escreve os grupos (>= MIN_SIZE membros),
     ordenados por tamanho, com o snippet representativo + membros.

Uso: cluster_blocks.py <consolidado.txt> <saida.txt> [--min-size N] [--n N]
"""
import re
import sys
import hashlib
from collections import defaultdict

REG = re.compile(r'\b(fr|dr|fv|r)(\d+)\b')
IMM = re.compile(r'#(?:-?0x[0-9A-Fa-f]+|-?\d+)')
ABS = re.compile(r'\b[0-9A-Fa-f]{8}\b')
DISP = re.compile(r'@\(([^)]*)\)')


def normalize(disasm):
    """Mnemonic + operand kinds (sem valores)."""
    # separa mnemonic do resto
    parts = disasm.split(None, 1)
    mn = parts[0]
    rest = parts[1] if len(parts) > 1 else ''
    rest = REG.sub(lambda m: m.group(1), rest)     # r2 -> r, fr5 -> fr
    rest = IMM.sub('#I', rest)                      # #4 / #0x20 -> #I
    rest = ABS.sub('A', rest)                       # 8C0000E8 -> A
    rest = DISP.sub(lambda m: '@(' + re.sub(r'\d+', 'D', m.group(1)) + ')', rest)
    return (mn + ' ' + rest).strip()


def parse_consolidado(path):
    """Devolve [(jogo, vaddr, [linhas_normalizadas], [linhas_originais])]."""
    blocks = []
    game = '?'
    cur = None
    hdr = re.compile(r'^=== ([0-9A-Fa-f]{8})\s+sh4=')
    for line in open(path, errors='replace'):
        if line.startswith('## ') and 'rom:' not in line and 'exit:' not in line:
            game = line[3:].strip()
        elif line.startswith('=== '):
            m = hdr.match(line)
            if m:
                cur = {'game': game, 'va': int(m.group(1), 16), 'norm': [], 'orig': []}
                blocks.append(cur)
            else:
                cur = None
        elif cur is not None and line.startswith('  '):
            # "  PC  OP  DISASM"
            p = line.rstrip('\n').split(None, 2)
            if len(p) == 3 and re.fullmatch(r'[0-9A-Fa-f]{8}', p[0]):
                cur['norm'].append(normalize(p[2]))
                cur['orig'].append(p[2])
    return blocks


def minhash_hashes(tokens, n, k):
    """k hashes do menor valor de cada hash de n-grama."""
    grams = set()
    for i in range(len(tokens) - n + 1):
        g = '\x1f'.join(tokens[i:i + n])
        grams.add(int.from_bytes(hashlib.blake2b(g.encode(), digest_size=8).digest(), 'little'))
    if not grams:
        return tuple([0xFFFFFFFFFFFFFFFF] * k)
    sig = []
    for seed in range(k):
        m = 0xFFFFFFFFFFFFFFFF
        for h in grams:
            v = (h ^ (seed * 0x9E3779B97F4A7C15)) & 0xFFFFFFFFFFFFFFFF
            v = (v * 0x100000001B3) & 0xFFFFFFFFFFFFFFFF
            if v < m:
                m = v
        sig.append(m)
    return tuple(sig)


def main():
    src, out = sys.argv[1], sys.argv[2]
    min_size = int(sys.argv[sys.argv.index('--min-size') + 1]) if '--min-size' in sys.argv else 3
    n = int(sys.argv[sys.argv.index('--n') + 1]) if '--n' in sys.argv else 4
    k = 32          # hashes por assinatura
    bands = 8       # bandas (k/bands linhas por banda)
    rows = k // bands

    print("lendo blocos...", file=sys.stderr)
    blocks = parse_consolidado(src)
    print("blocos:", len(blocks), file=sys.stderr)

    # assinaturas
    sigs = []
    for b in blocks:
        sigs.append(minhash_hashes(b['norm'], n, k))
    print("assinaturas prontas", file=sys.stderr)

    # LSH: buckets por (banda, valor)
    buckets = defaultdict(list)
    for idx, sig in enumerate(sigs):
        for b in range(bands):
            key = (b, sig[b * rows:(b + 1) * rows])
            buckets[key].append(idx)

    # candidatos
    parent = list(range(len(blocks)))

    def find(x):
        while parent[x] != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x

    def union(a, b):
        ra, rb = find(a), find(b)
        if ra != rb:
            parent[rb] = ra

    checked = set()
    for key, idxs in buckets.items():
        if len(idxs) < 2 or len(idxs) > 1500:
            continue
        for i in range(len(idxs)):
            for j in range(i + 1, len(idxs)):
                a, b = idxs[i], idxs[j]
                if a > b:
                    a, b = b, a
                if (a, b) in checked:
                    continue
                checked.add((a, b))
                # Jaccard real sobre os conjuntos de n-gramas
                # (recomputa os conjuntos so quando os minhashes batem)
                sa = set(sigs[a]); sb = set(sigs[b])
                inter = len(sa & sb)
                if inter / k >= 0.7:
                    union(a, b)
    print("pares checados:", len(checked), file=sys.stderr)

    clusters = defaultdict(list)
    for i in range(len(blocks)):
        clusters[find(i)].append(i)

    big = [(root, mem) for root, mem in clusters.items() if len(mem) >= min_size]
    big.sort(key=lambda x: -len(x[1]))
    print("clusters >= %d: %d" % (min_size, len(big)), file=sys.stderr)

    with open(out, 'w') as f:
        f.write("# Clusters de blocos SH4 quase-identicos (n=%d, MinHash/LSH, Jaccard>=0.7)\n" % n)
        f.write("# %d clusters com >= %d membros\n\n" % (len(big), min_size))
        for ci, (root, mem) in enumerate(big):
            rep = blocks[mem[0]]
            games = defaultdict(int)
            for m in mem:
                games[blocks[m]['game']] += 1
            f.write("=" * 100 + "\n")
            f.write("## CLUSTER %d — %d membros — %d jogo(s)\n" % (ci, len(mem), len(games)))
            f.write("## snippet (bloco %08X, %s):\n" % (rep['va'], rep['game']))
            for l in rep['orig'][:40]:
                f.write("   " + l + "\n")
            if len(rep['orig']) > 40:
                f.write("   ... (%d instr)\n" % len(rep['orig']))
            f.write("## membros:\n")
            for m in mem[:200]:
                f.write("   %08X  %s\n" % (blocks[m]['va'], blocks[m]['game']))
            if len(mem) > 200:
                f.write("   ... (+%d)\n" % (len(mem) - 200))
            f.write("\n")
    print("escrito:", out, file=sys.stderr)


if __name__ == '__main__':
    main()
