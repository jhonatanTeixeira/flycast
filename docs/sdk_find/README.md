# Descoberta automática de funções repetidas (Dreamcast)

Gerado por `tools/sdk_find.py` (2026-10-10) a partir dos dumps do JIT que já existem
em `/mnt/1TB` (`dcbat_off/`, `dcbat/`), **sem jogar de novo**. Só Dreamcast (ROMs
`.chd`): 21 jogos, um dump cada (o que tem amostras do `perf`; senão o maior).
Ranking: `descoberta.md`. Listagens dos 60 primeiros grupos: `auto/`. Contexto e plano:
`docs/native_sdk_code.md` (4.125). Famílias analisadas à mão: `docs/sdk_blocks/`.

## Algoritmo

1. **Funções.** Mapa endereço→opcode (o bloco compilado mais recente ganha) e descida
   recursiva a partir dos alvos de `bsr` e dos inícios de bloco que sobraram: segue
   `bra`/`bt`/`bf` e os slots, para em `rts`/`rte`/`jmp`/`braf`, e chamadas continuam
   depois do slot. Cada endereço pertence a uma função só (2ª entrada ou cauda
   compartilhada não viram função nova). Mínimo de 8 instruções. 50.561 funções.
2. **Normalização.** O opcode com os campos de deslocamento zerados (literal
   PC-relativo, `mova`, desvios): o mesmo código em outro endereço fica igual;
   registradores e imediatos ficam (têm significado).
3. **Iguais:** hash da sequência normalizada.
4. **Parecidas:** índice invertido de 6-gramas normalizados (ignora os que aparecem em
   mais de 60 funções: prólogos, epílogos) → pares candidatos → **contenção**
   (interseção / menor lado) ≥ 0,8. Contenção, não Jaccard, porque o dump só tem o
   trecho que cada jogo executou: uma função inteira precisa casar com um pedaço dela.
   Segunda visão, imune à ordem das instruções: por bloco, o multiconjunto das
   operações **SHIL** do dump (registradores sem versão, endereços mascarados) →
   contenção ≥ 0,85 com ≥ 3 blocos.
5. **Grupos:** união dos pares; só grupos com 2+ jogos (2.798 grupos). Ranking: grupos
   que já têm versão nativa primeiro, depois o tempo do `perf` somado (% da thread de
   emulação), depois o número de jogos.

## Como ler

- **Tempo perf** só existe para 7 jogos (DOA2, EGG, MvC2, Napple, Shenmue, Shenmue II,
  TR Chronicles); nos outros a coluna conta 0 mesmo que a função rode muito lá.
- **Já nativos** (`nativo_*`): o tempo deles roda em C++, fora dos blocos do JIT, e
  aparece ~0. A busca reencontrou os três sozinha: `stripemit` e `lightxf` nos mesmos 5
  jogos de `docs/sdk_blocks/`, e o laço do DOA2 com MvC2, Shenmue II, Power Stone e
  Project Justice.
- **Variantes** = sequências normalizadas distintas no grupo (versões do SDK, trechos
  executados diferentes).
- **Nome** = rótulo automático pelo tipo de instrução (`matriz` = `ftrv`,
  `produto_escalar` = `fipr`, `copia` = laço com leitura e escrita, `switch` = `braf`,
  `contexto` = `rte`/SSR...). Dar nome de verdade exige ler a listagem.

## Primeiros achados (não nativizados ainda)

| Grupo | Jogos | Onde pesa |
|---|---|---|
| 004 T&L (`ftrv`+`fipr`+`fdiv`) | CvS2, DOA2, MvC2, Power Stone, Project Justice, Shenmue II | DOA2 6,3%, MvC2 2,5% |
| 005 T&L | CvS2, DOA2, MvC2, Power Stone, Shenmue II | **Shenmue II 6,0%**, DOA2 2,6% |
| 007 matriz + `fdiv` + `pref` | CvS2, DOA2, MvC2 | **MvC2 6,1%** |
| 008 cópia | 8 jogos (Napple, Evolution 1/2, Grandia II, RE CV…) | Napple 5,9% |
| 009 cópia | 10 jogos | EGG 5,4% |
| 010 matriz (região quente `8C1C1xxx` do Napple) | 7 jogos | Napple 5,4% |
| 011 matriz + `fdiv` + `pref` | DOA2, MvC2, Shenmue II, CvS2, Power Stone, Project Justice | DOA2 2,1%, Shenmue II 1,5% |
| 012 `switch` (512 instr.) | 13 jogos | EGG 4,2% |
| 013 rotina curta (26 instr.) | 15 jogos | MvC2 2,7%, Napple 1,4% |

Aparecem duas "famílias" de biblioteca: uma nos jogos de luta/Sega AM
(DOA2, MvC2, CvS2, Power Stone, Project Justice, Shenmue II) e outra nos RPG/aventura
(Napple, Evolution 1/2, Grandia II, RE CV, Skies, KOF Evolution, Macross, PSO, EGG).

## Limites

- Só o código **executado** nas sessões gravadas; função que não rodou não aparece.
- **Encadeamento:** a união de pares é transitiva; funções parecidas entre si (ex.: as
  4-5 variantes de uma mesma rotina em cada jogo) caem num grupo só. O grupo 006 (21
  jogos, ~8 funções por jogo) é isso. Um agrupamento por representante (sem
  transitividade) separaria.
- O literal pool (constantes) não entra na comparação.
- Regenerar: `SDK_FIND_CACHE=<dir> python3 tools/sdk_find.py docs/sdk_find
  /mnt/1TB/dcbat_off/* /mnt/1TB/dcbat/*` (o cache guarda os relatórios do `perf`).
