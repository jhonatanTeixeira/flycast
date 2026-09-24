#!/usr/bin/env python3
"""Regioes quentes para um compilador de nivel 2 (dump de FC_JIT_DUMP).

Uso: region_study.py jit-<pid>.txt [--min-edge P] [--max-sh4 N] [--top N]

Monta o grafo de blocos do SH4 com peso nas arestas e agrupa os blocos
quentes em regioes (lacos e cadeias ligadas por arestas dominantes). Mede o
que uma regiao compilada inteira eliminaria: transicoes entre blocos que
ficam dentro da regiao (sem gravar/recarregar contexto nem checar ciclos de
novo) e o custo que ela cobre.

Arestas:
  condicional  linha C: tomado/caiu, destino/proximo (contadores exatos)
  bra/bsr      desvio estatico decodificado da ultima instrucao do G
  queda        bloco que termina sem desvio (limite de tamanho): vaddr+bytes
  bsr/jsr      a volta do rts (98,75-99,97% previsivel, docs/jit_study.md)
               vira aresta chamada -> ponto de retorno
  jmp/jsr/rts  destino dinamico: sem aresta (saida da regiao)
Custo de um bloco = execucoes x instrucoes ARM do JIT antigo.
"""
import argparse
import collections


def sext(v, bits):
    return v - (1 << bits) if v & (1 << (bits - 1)) else v


def branch_info(vaddr, ops):
    """(tipo, destino estatico ou None) da instrucao que termina o bloco."""
    for back in (2, 1):		# desvio com slot de atraso e o penultimo
        if len(ops) < back:
            continue
        i = len(ops) - back
        op = ops[i]
        pc = vaddr + 2 * i
        hi4 = op >> 12
        if hi4 == 0xA:
            return 'bra', pc + 4 + sext(op & 0xFFF, 12) * 2
        if hi4 == 0xB:
            return 'bsr', pc + 4 + sext(op & 0xFFF, 12) * 2
        if op >> 8 in (0x89, 0x8B, 0x8D, 0x8F):
            return 'cond', pc + 4 + sext(op & 0xFF, 8) * 2
        if op == 0x000B:
            return 'rts', None
        if op == 0x002B:
            return 'rte', None
        if op & 0xF0FF == 0x400B:
            return 'jsr', None
        if op & 0xF0FF == 0x402B:
            return 'jmp', None
        if op & 0xF0FF in (0x0023, 0x0003):
            return 'braf', None
    return 'queda', None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('dump')
    ap.add_argument('--min-edge', type=float, default=0.25,
                    help='aresta entra na regiao se leva >= P das saidas da origem')
    ap.add_argument('--max-sh4', type=int, default=512, help='tamanho maximo da regiao em instr SH4')
    ap.add_argument('--top', type=int, default=25)
    ap.add_argument('--show', type=int, default=0, help='lista os blocos das N maiores regioes')
    ap.add_argument('--cover', type=float, default=0.9, help='blocos quentes = os que somam esta fracao do custo')
    a = ap.parse_args()

    info = {}			# vaddr -> (sh4 bytes, n instr, host bytes, ops)
    runs = collections.Counter()
    last_r = {}			# (host, vaddr) -> runs (cumulativo, fica o ultimo)
    cond = {}			# (host, vaddr) -> (tomado, caiu, destino, proximo)
    cur = None
    for l in open(a.dump, errors='replace'):
        t = l[0]
        if t == 'B':
            p = l.split()
            cur = int(p[2], 16)
            info[cur] = [int(p[4]), int(p[5]), int(p[7]), []]
        elif t == 'G' and cur is not None:
            info[cur][3] = [int(x, 16) for x in l.split()[1:]]
        elif t == 'R' or t == 'D':
            p = l.split()
            last_r[(p[1], p[2])] = int(p[3])
        elif t == 'C':
            p = l.split()
            cond[(p[1], p[2])] = (int(p[3]), int(p[4]), int(p[5], 16), int(p[6], 16))
    for (h, v), n in last_r.items():
        runs[int(v, 16)] += n
    edges = collections.defaultdict(collections.Counter)	# origem -> destino -> peso
    condw = collections.defaultdict(lambda: [0, 0, 0, 0])
    for (h, v), (tk, nt, br, nx) in cond.items():
        c = condw[int(v, 16)]
        c[0] += tk; c[1] += nt; c[2] = br; c[3] = nx
    kinds = collections.Counter()
    for v, (sb, ni, hb, ops) in info.items():
        n = runs.get(v, 0)
        if n == 0:
            continue
        k, tgt = branch_info(v, ops)
        if v in condw:
            tk, nt, br, nx = condw[v]
            edges[v][br] += tk
            edges[v][nx] += nt
            kinds['condicional'] += n
        elif k in ('bra', 'queda'):
            edges[v][tgt if k == 'bra' else v + sb] += n
            kinds[k] += n
        elif k in ('bsr', 'jsr'):
            if tgt is not None:
                edges[v][tgt] += n
            edges[v][v + sb] += 0	# a volta vem pelo rts do chamado
            kinds[k] += n
        else:
            kinds[k] += n
    cost = {v: runs[v] * info[v][2] / 4 for v in info if runs.get(v)}
    tot_cost = sum(cost.values())
    tot_runs = sum(runs[v] for v in cost)

    print('blocos executados: %d, execucoes: %.1f M, custo (instr ARM do JIT antigo): %.1f M'
          % (len(cost), tot_runs / 1e6, tot_cost / 1e6))
    print('fim de bloco (por execucao):')
    for k, n in kinds.most_common():
        print('  %5.1f%%  %s' % (100.0 * n / tot_runs, k))

    hot = []
    acc = 0
    for v, c in sorted(cost.items(), key=lambda x: -x[1]):
        hot.append(v)
        acc += c
        if acc >= a.cover * tot_cost:
            break
    hotset = set(hot)
    print('blocos quentes (%.0f%% do custo): %d' % (100 * a.cover, len(hot)))

    # union-find pelas arestas dominantes entre blocos quentes
    parent = {v: v for v in hot}
    size = {v: info[v][1] for v in hot}

    def find(x):
        while parent[x] != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x
    cand = []
    for s in hot:
        out = sum(edges[s].values())
        for d, w in edges[s].items():
            if d in hotset and d != s and out and w >= a.min_edge * out:
                cand.append((w, s, d))
    cand.sort(reverse=True)
    for w, s, d in cand:
        rs, rd = find(s), find(d)
        if rs != rd and size[rs] + size[rd] <= a.max_sh4:
            parent[rd] = rs
            size[rs] += size[rd]
    regs = collections.defaultdict(list)
    for v in hot:
        regs[find(v)].append(v)

    # transicoes: execucoes que saem de um bloco quente e para onde vao
    inside = 0
    trans = 0
    for s in hot:
        for d, w in edges[s].items():
            trans += w
            if d in hotset and find(d) == find(s):
                inside += w
    stats = []
    for r, bl in regs.items():
        c = sum(cost[v] for v in bl)
        ins = sum(w for s in bl for d, w in edges[s].items() if d in hotset and find(d) == r)
        out = sum(w for s in bl for d, w in edges[s].items()) - ins
        ent = sum(runs[v] for v in bl) - ins	# execucoes que entram de fora (aprox.)
        loop = any(d in bl and d <= s for s in bl for d in edges[s])
        stats.append((c, r, bl, ins, out, ent, loop))
    stats.sort(reverse=True)
    print('regioes: %d (min-edge %.2f, max %d instr SH4)' % (len(regs), a.min_edge, a.max_sh4))
    print('transicoes conhecidas saindo de bloco quente: %.1f M; ficam dentro da regiao: %.1f%%'
          % (trans / 1e6, 100.0 * inside / max(trans, 1)))
    acc = 0
    marks = [0.5, 0.8, 0.9]
    for i, (c, r, bl, ins, out, ent, loop) in enumerate(stats):
        acc += c
        while marks and acc >= marks[0] * tot_cost:
            print('  %d regioes cobrem %.0f%% do custo total' % (i + 1, marks[0] * 100))
            marks.pop(0)
    print()
    print('  #  custo%  blocos  SH4  ARM(B)  dentro/entrada  laco  cabeca')
    for i, (c, r, bl, ins, out, ent, loop) in enumerate(stats[:a.top]):
        sh4 = sum(info[v][1] for v in bl)
        arm = sum(info[v][2] for v in bl)
        head = max(bl, key=lambda v: runs[v] - sum(edges[s][v] for s in bl if s != v))
        print('%3d  %5.2f  %6d  %4d  %6d  %8.1f  %s  %08X' % (
            i + 1, 100.0 * c / tot_cost, len(bl), sh4, arm, ins / max(ent, 1),
            'sim ' if loop else 'nao ', head))
    for i, (c, r, bl, ins, out, ent, loop) in enumerate(stats[:a.show]):
        print()
        print('regiao %d: %.2f%% do custo' % (i + 1, 100.0 * c / tot_cost))
        print('  bloco     SH4  ARM(B)  execucoes  custo%  fim    -> destinos (fracao das saidas; * = fora da regiao)')
        for v in sorted(bl):
            sb, ni, hb, ops = info[v]
            k, _ = branch_info(v, ops)
            if v in condw:
                k = 'cond'
            out = sum(edges[v].values())
            ds = ' '.join('%s%08X:%.2f' % ('' if d in hotset and find(d) == r else '*', d, w / out)
                          for d, w in edges[v].most_common(3) if out)
            print('  %08X %4d  %6d  %9d  %5.2f  %-5s  %s' % (v, ni, hb, runs[v], 100.0 * cost[v] / tot_cost, k, ds))


if __name__ == '__main__':
    main()
