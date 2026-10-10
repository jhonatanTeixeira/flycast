#!/usr/bin/env python3
"""Gera docs/sdk_blocks/: um documento por familia de bloco de SDK repetida entre
jogos, com a listagem SH4 de cada jogo (docs/native_sdk_code.md).

  sdk_blocks_doc.py <saida_dir> <jit-*.txt...>

Para cada familia: acha a assinatura (opcodes) nos blocos compilados de cada dump,
calcula a janela da funcao a partir do ponto achado e lista os opcodes conhecidos da
janela (uniao de todos os blocos do dump). Um jogo usa o primeiro dump da lista em
que a familia aparece (passe dcbat_off antes de dcbat). A coluna "iguais" compara,
nos deslocamentos conhecidos dos dois lados, com o jogo de referencia.
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sh4dis  # noqa: E402

# nome do arquivo, titulo, assinaturas, deslocamento da assinatura dentro da janela,
# tamanho da janela, modo ('func' = uma janela por ocorrencia; 'span' = uma janela
# do primeiro ao ultimo achado do jogo), referencia (trecho do nome do jogo), texto
FAMILIES = [
    dict(file='luz_e_transformacao_de_vertices.md',
         title='Luz e transformação de vértices (`lightxf`)',
         sigs=['FFFB FFEB FFDB FFCB 2F86 6346 6546'], off=0, size=0x124, mode='func',
         ref='Napple',
         text='Transforma posição e normal de cada vértice (`ftrv`), soma a luz direcional '
              'de cada luz ativa (N.L), calcula 1/z e a posição de tela e grava 32 bytes '
              'por vértice na Store Queue. **Já nativa no Napple** (`hle_fn.cpp`, '
              '`8C14DDC0`). Biblioteca gráfica do SDK: a mesma função aparece em outros '
              'jogos.'),
    dict(file='emissor_de_strips.md',
         title='Emissor de strips de triângulo (`stripemit`)',
         sigs=['2F86 2F96 2FA6 2FB6 2FC6 D057 6243 7206'], off=0, size=0x178, mode='func',
         ref='Napple',
         text='Lê os registros de vértice que a `lightxf` gravou e monta as strips para o '
              'TA: cabeçalho por strip, u/v (int16 → float × escala) e duas rajadas de 32 '
              'bytes na Store Queue por vértice. Roda com `FPSCR.SZ=1` (os `fmov` movem '
              'pares). **Já nativa no Napple** (`8C14D440`).'),
    dict(file='vertices_com_clamp.md',
         title='Laço de vértices com clamp de cor (tipo DOA2)',
         sigs=['F6E9 6763 F28D F270 F79D 7640'], off=8, size=0xA8, mode='func',
         ref='Dead_or_Alive',
         text='Por vértice: carrega 3 pares, `ftrv` com a matriz, `fipr` (luz), `fdiv` '
              '(1/w), clamp de cor por tabela (`fcmp/gt` + `ftrc`), 4 pares + cabeçalho '
              'de 32 bytes na Store Queue e `pref`. ~45% do JIT do DOA2. **Já nativo no '
              'DOA2** (`8C101BC4`). Também em MvC2 e Shenmue II.'),
    dict(file='comandos_maple_com_trava.md',
         title='Comandos Maple com trava (`tas.b`)',
         sigs=['4F22 6032 401B 8907'], off=0x102, size=0, mode='span', tail=0x60,
         ref='Dead_or_Alive',
         text='Funções do SDK que mandam comandos ao barramento Maple (controle, VMU, '
              'vibração). Cada uma pega a trava com `tas.b` (se ocupada, sai sem esperar), '
              'chama o despachante com o código do comando em `r5` (`0x01` informação do '
              'dispositivo, `0x0A` informação da mídia, `0x0B` ler bloco, `0x0C` gravar '
              'bloco, `0x0E` ajustar condição) e solta a trava. Não é quente (`tas.b` 0,0% '
              'da emu).'),
    dict(file='chamadas_da_bios.md',
         title='Chamadas da BIOS (syscalls por vetor)',
         sigs=['D702 D003 6002 402B'], off=0, size=0x0A, mode='func',
         ref='Dead_or_Alive',
         text='Stub que carrega o número da função em `r7`, o vetor da BIOS em `r0` '
              '(`0x8C0000BC` GD-ROM, `0x8C0000B0` sysinfo, `0x8C0000B4` fonte, '
              '`0x8C0000B8` flashrom, `0x8C0000E0` misc), lê o endereço e salta. Os '
              'valores ficam no literal pool (endereço entre colchetes), que o dump do '
              'JIT não guarda; as linhas `O` do dump mostram o endereço lido.'),
    dict(file='biblioteca_de_threads.md',
         title='Biblioteca de threads (criar, ceder, trocar contexto)',
         sigs=['403E 002A 404E'], off=0x74, size=0x2D8, mode='func',
         ref='Napple',
         text='Cria a pilha da thread (entrada, argumento, SR, FPSCR), cede a vez fingindo '
              'uma exceção (`ldc SSR/SPC`), salva o contexto inteiro (r0-r14, bancos, 32 '
              'floats, GBR, MAC, SPC/SSR), escolhe a próxima em C e volta com `rte`. '
              'Detalhe em `docs/sh4_threading_model.md`.'),
    dict(file='secao_critica_imask.md',
         title='Seção crítica por máscara de interrupção (IMASK = 15)',
         sigs=['0002 4009 4009 C90F 2F02 0002'], off=0x0A, size=0xE8, mode='func',
         ref='Dead_or_Alive',
         text='Guarda o IMASK atual, põe IMASK = 15 (`or #0xF0` + `ldc SR`), chama a função '
              'protegida e restaura o IMASK. É o "mutex" do Dreamcast (um núcleo). Fica na '
              'área de sistema (`8C0083F8`), no mesmo endereço em todos os jogos.'),
    dict(file='memset_e_ocbp.md',
         title='Laços de `memset` (16 bits) e `ocbp`',
         sigs=['2471 76FF 7402 2668 8BFA', '2471 75FF 7402 2558 8BFA',
               '4509 4509 4501 7501 04A3 4510 8FFC 7420'], off=0, size=0x10, mode='func',
         ref='Napple',
         text='Laços curtos de preencher memória (`mov.w` + contador) e de purgar a cache '
              'de dados por linha (`ocbp`, 32 bytes por volta). HLE escrito e **não '
              'validado** (`tech_debits.md` 4.119).'),
]


def game_name(path):
    rom = os.path.join(os.path.dirname(path), 'rom.txt')
    if os.path.exists(rom):
        name = os.path.basename(open(rom, errors='ignore').read().strip())
        if name:
            return os.path.splitext(name)[0]
    d = path.replace('\\', '/').split('/')[-2]
    d = d.split('_', 1)[1] if re.match(r'\d{8}-', d) else d
    return re.sub(r'_+', ' ', d).strip()


def load(path):
    """Blocos na ordem de compilacao [(seq, pc, ops)] + endereco -> [(seq, opcode)].

    O mesmo endereco pode ter codigo diferente ao longo da sessao (overlays): cada
    janela usa, em cada endereco, o bloco compilado mais perto no tempo do bloco
    em que a assinatura casou."""
    blocks, mem, pc = [], {}, None
    for line in open(path, errors='ignore'):
        if line.startswith('B '):
            pc = int(line.split()[2], 16)
        elif line.startswith('G ') and pc is not None:
            ops = [int(x, 16) for x in line.split()[1:]]
            seq = len(blocks)
            blocks.append((seq, pc, ops))
            for i, op in enumerate(ops):
                mem.setdefault(pc + 2 * i, []).append((seq, op))
    return blocks, mem


def at(mem, a, seq):
    v = mem.get(a)
    if not v:
        return None
    return min(v, key=lambda x: abs(x[0] - seq))[1]


def find(blocks, sig):
    """{endereco: seq do primeiro bloco em que a assinatura aparece}"""
    s = [int(x, 16) for x in sig.split()]
    out = {}
    for seq, pc, ops in blocks:
        for i in range(len(ops) - len(s) + 1):
            if ops[i:i + len(s)] == s:
                out.setdefault(pc + 2 * i, seq)
    return out


def windows(fam, hits):
    if fam['mode'] == 'span':
        return [(hits[0] - fam['off'], hits[-1] + fam['tail'])]
    return [(h - fam['off'], h - fam['off'] + fam['size']) for h in hits]


def listing(mem, lo, hi, seq):
    lines, gap = [], False
    for a in range(lo, hi, 2):
        op = at(mem, a, seq)
        if op is None:
            if not gap:
                lines.append('  ...       (não compilado nesta sessão)')
            gap = True
            continue
        gap = False
        lines.append('  %08X  %04X  %s' % (a, op, sh4dis.dis(op, a)))
    return lines


def main():
    outdir, files = sys.argv[1], sys.argv[2:]
    sh4dis.load_table()
    os.makedirs(outdir, exist_ok=True)
    # familia -> jogo -> (dump, [(lo, hi, opcodes)])
    found = {f['file']: {} for f in FAMILIES}
    for path in files:
        g = game_name(path)
        need = [f for f in FAMILIES if g not in found[f['file']]]
        if not need:
            continue
        blocks, mem = load(path)
        for f in need:
            found_ = {}
            for s in f['sigs']:
                for h, sq in find(blocks, s).items():
                    found_[h] = min(sq, found_.get(h, sq))
            hits = sorted(found_)
            if not hits:
                continue
            ws = []
            for (lo, hi), h in zip(windows(f, hits), hits if f['mode'] == 'func' else hits[:1]):
                sq = found_[h]
                ops = {a: at(mem, a, sq) for a in range(lo, hi, 2) if a in mem}
                ws.append((lo, hi, ops, listing(mem, lo, hi, sq)))
            found[f['file']][g] = (path, ws)
        sys.stderr.write('lido %s\n' % g)

    index = []
    for f in FAMILIES:
        games = found[f['file']]
        ref = next((g for g in games if f['ref'] in g.replace(' ', '_')), None)
        ref_ops = games[ref][1][0][2] if ref else {}
        ref_lo = games[ref][1][0][0] if ref else 0
        out = ['# %s' % f['title'], '',
               '> Gerado por `tools/sdk_blocks_doc.py` a partir dos dumps do JIT em '
               '`/mnt/1TB` (2026-10-10). Índice: `README.md`. Contexto: '
               '`docs/native_sdk_code.md`.', '',
               f['text'], '',
               'Assinatura (opcodes): ' + ' · '.join('`%s`' % s for s in f['sigs']), '',
               'Referência da comparação: **%s**.' % (ref or '—'), '',
               '| Jogo | Endereço(s) | Iguais à referência |', '|---|---|---|']
        for g in sorted(games):
            path, ws = games[g]
            best = (-1, 0, 0)
            for lo, _, ops, _ in ws:   # a janela mais parecida com a referencia
                same = diff = 0
                for a, op in ops.items():
                    r = ref_ops.get(ref_lo + (a - lo))
                    if r is None:
                        continue
                    same += r == op
                    diff += r != op
                if (same, -diff) > (best[0], -best[1]):
                    best = (same, diff, lo)
            same, diff = max(best[0], 0), best[1]
            cmp_ = 'referência' if g == ref else (
                '%d de %d opcodes' % (same, same + diff) if same + diff else 'sem trecho em comum')
            out.append('| %s | %s | %s |' % (g, ', '.join('`%08X`' % w[0] for w in ws), cmp_))
        out.append('')
        out.append('"Iguais" conta só os deslocamentos compilados nas duas sessões (o dump '
                   'guarda o que o jogo executou); o literal pool (constantes) não entra.')
        for g in sorted(games):
            path, ws = games[g]
            out += ['', '## %s' % g, '', 'Dump: `%s`' % path, '']
            for lo, hi, _, lines in ws:
                out += ['```', '; %08X-%08X' % (lo, hi)] + lines + ['```', '']
        open(os.path.join(outdir, f['file']), 'w').write('\n'.join(out).rstrip() + '\n')
        index.append('| [%s](%s) | %d | %s |' % (f['title'], f['file'], len(games),
                                                  ', '.join(sorted(games))))

    open(os.path.join(outdir, 'README.md'), 'w').write('\n'.join([
        '# Blocos de SDK repetidos entre jogos', '',
        'Um documento por família de bloco que aparece com os mesmos opcodes em vários '
        'jogos (código de biblioteca do SDK). Cada um traz a listagem SH4 de cada jogo, '
        'tirada dos dumps do JIT. Contexto e plano: `docs/native_sdk_code.md` '
        '(4.125) e `docs/sh4_threading_model.md`.', '',
        'Regenerar (dcbat_off antes de dcbat; jogo usa o primeiro dump em que aparece):', '',
        '```bash',
        'python3 tools/sdk_blocks_doc.py docs/sdk_blocks /mnt/1TB/dcbat_off/*/jit-*.txt \\',
        '    $(ls -r /mnt/1TB/dcbat/*/jit-*.txt)',
        '```', '',
        'Cada família tem um arquivo irmão `<família>_pseudo.cpp`: pseudo-C++ (não compila '
        'no core) que resolve todos os jogos da família, com `if/else` onde eles diferem. '
        'É escrito à mão (não é gerado) e é a base do `hle_fn` relocável (4.125). '
        'Variantes reais: threads do EGG (sem FPSCR inicial), seção crítica do PSO v2 '
        '(chamadas a mais); o Maple do Shenmue só muda a ordem das instruções.', '',
        '| Família | Jogos | Onde |', '|---|---|---|'] + index) + '\n')


if __name__ == '__main__':
    main()
