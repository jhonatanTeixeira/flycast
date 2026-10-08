# Nível 2: patches de velocidade por jogo (plano guardado)

2026-09-24. Plano B, e base para outras ideias, caso o nível 2 em tempo de
execução (segunda thread, `docs/current_plan.md`) não vingue. As duas coisas
podem conviver: o patch cobre as cenas perfiladas desde o primeiro frame e o
perfil em tempo de execução cobre o resto.

## Origem

O experimento `FC_TIER2=1` (item 4.57 de `docs/tech_debits.md`) já é um
"patch" feito à mão. A região do laço de vértices do DOA2 foi gerada offline
por `tools/tier2_gen.py --emu doa2`, compilada no core como
`core/rec-ARM64/tier2_doa2.S` e ligada por gancho nos blocos do JIT antigo.
Resultado: jogo inteiro IDÊNTICO, velocidade do jogo 86 → 95%, e na
avaliação jogando "o mais jogável até agora", sem os stutters de antes.

## O conceito

Uma **pasta onde se jogam os patches**, que o core lê no boot do jogo.

- **Pasta:** no diretório de sistema do libretro, por exemplo
  `/roms2/bios/dc/flycast_patches/`. Todo arquivo ali é lido; jogo sem patch
  roda normal.
- **Arquivo de patch:** texto pequeno, gerado offline, **sem código de
  máquina**. Por região:
  - nome (ex.: `doa2_vertices`);
  - blocos de entrada (ex.: 8C101BC2, 8C101BC4);
  - lista de blocos (endereço + tamanho);
  - instruções SH4 esperadas de cada bloco (ou hash).
  Um arquivo pode ter várias regiões (ex.: as 18 que dão 50% do Shenmue II).
- **No core:**
  1. No boot, lê todos os arquivos da pasta.
  2. Quando o JIT antigo compila um bloco que é entrada de alguma região,
     confere se o SH4 na RAM bate com o patch.
  3. Se bate, o gerador em C++ compila a região ali (dezenas de µs, uma vez)
     e põe o gancho.
  4. Se não bate (outra versão/região do jogo, outro jogo, código mudado),
     ignora em silêncio.
  5. Se o jogo reescreve o código, a proteção de página derruba os blocos e
     a conferência é refeita na recompilação.
- **Ferramenta:** `tools/tier2_pack.py` (a fazer): pega um dump
  (`FC_JIT_DUMP`), escolhe as regiões quentes até uma cobertura (ex.: 80% do
  custo, via `tools/region_study.py`) e escreve o arquivo. Cada patch passa
  pelo `state_compare` no jogo antes de ir para a pasta.

## Por que é atraente

- Custo zero em tempo de execução: sem segunda thread, sem compilar durante
  o jogo, sem stutter de compilação.
- Offline dá para gastar tempo: otimização mais pesada e até ajuste à mão
  das regiões mais quentes (a versão à mão do DOA2 fez 1,85× contra 1,74× do
  gerador).
- Validado antes de chegar no device (harness IDÊNTICO com ciclos +
  `state_compare`).
- Seguro: só liga com o SH4 exato.

## Limites

- Por jogo e por versão do jogo.
- Só cobre o que foi perfilado; cena fora do dump fica no JIT antigo.
- Código de máquina compilado no `.so` não escala (um `.S` por jogo); por
  isso o patch descreve a região e o gerador C++ compila no device.

## Dependências (em comum com o nível 2 em tempo de execução)

1. Gerador em C++/VIXL portando as regras do `tier2_gen.py`, com fault
   seguro dentro da região.
2. Gancho genérico (no lugar do gancho fixo do DOA2) e leitor da pasta.
3. `tier2_pack.py` e o primeiro patch de verdade (DOA2, depois Shenmue II).
