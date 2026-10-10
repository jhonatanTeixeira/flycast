# Plano: caminho quente do JIT (só código quente na cache de instruções do ARM)

Status: **planejado (2026-10-10)**. Item `tech_debits.md` 4.127. Ordem de execução:
implementar uma fase, validar bit a bit (`FC_STATE_HASH`), A/B de 2 rodadas, commitar —
sem rodada de medição antes (preferência do usuário: implementar e ver no que deu).

## 1. Problema

O A53 do R36 tem **32 KB de L1I** (e uma L2 compartilhada, tamanho a confirmar no device).
A CPU sempre executa da cache; código fora dela custa ~15-20 ciclos se vier da L2 e
~250-300 ciclos (~200 ns) se vier da DRAM. O JIT gera ~5× mais bytes que o SH4 (16 bits
por instrução → ~20 bytes de ARM64), e cada bloco carrega junto do código quente:

- **caminhos frios** (quase nunca executam): falha da checagem de código alterado
  (recompilar) e fim de fatia (`intc_sched` → `UpdateSystem`);
- **literal pool** do vixl no fim do bloco (dado no meio do código);
- **checagem de código alterado executada** em todo bloco de página não protegida
  (`ro=0`): 4 instruções + 8 bytes de constante para cada 8 bytes de SH4.

E os blocos ficam no cache de código **na ordem em que foram compilados**: blocos que
rodam juntos ficam espalhados, cada um ocupando linhas de 64 bytes e páginas de 4 KB
próprias (o A53 tem micro-TLB de instrução pequeno).

## 2. Medição (dumps existentes, `perf` dos 7 jogos com amostras, 2026-10-10)

Conjunto quente = blocos do JIT, ordenados por tempo amostrado, que somam 50% / 90% do
tempo da thread de emulação **dentro de blocos**. Script: `tools/jit_hotset.py`.

| Jogo | 50%: blocos / bytes | 90%: blocos / bytes | Frio | Literais | Checagem exec. | Linhas 64 B tocadas (90%) | Só o quente (90%) | Páginas 4 KB (90%) |
|---|---|---|---|---|---|---|---|---|
| DOA2 | 35 / 10,6 K | 631 / 130,9 K | 11,5% | 2,0% | 0,7% | 155,6 K | 113,2 K | 196 |
| Shenmue II | 305 / 72,5 K | 2142 / 344,5 K | 14,6% | 1,9% | 0,3% | 428,6 K | 287,5 K | 421 |
| Shenmue | 79 / 20,7 K | 806 / 156,1 K | 13,2% | 3,6% | **3,8%** (9,3% nos 50%) | 185,2 K | 129,8 K | 281 |
| Napple | 115 / 25,2 K | 671 / 102,5 K | 15,4% | 0,7% | 0,6% | 114,6 K | 86,0 K | 93 |
| MvC2 | 43 / 8,4 K | 595 / 105,6 K | 13,8% | 3,2% | 1,8% | 129,4 K | 87,7 K | 142 |
| EGG | 38 / 9,2 K | 486 / 87,6 K | 13,5% | 4,0% | 2,1% | 101,9 K | 72,2 K | 113 |

(TR Chronicles roda com MMU, prólogo diferente; fora da tabela.)

Leitura:
- **Metade do tempo cabe na L1I** (8-25 KB) em quase todos; o Shenmue II já não cabe (72 KB).
- **90% do tempo pede 80-160 KB** (Shenmue II 345 KB): vive na L2 e disputa a L1I.
- **~15-20% de cada bloco quente é peso morto** (frio + literais). Somado à
  fragmentação em linhas de 64 B, empacotar só o quente corta **~25-33% das linhas
  tocadas** (DOA2 155,6 → 113,2 K; Shenmue II 428,6 → 287,5 K) e o número de páginas.
- **Shenmue**: 24 dos 79 blocos mais quentes estão em página não protegida e executam
  a checagem de código a cada entrada (9,3% dos bytes desses blocos).
- Fora dos blocos (C++: handlers de memória, `UpdateSystem`, TA, áudio) roda o resto da
  thread (30-55%): ele disputa a mesma L1I.

Não sabemos ainda quanto do tempo é **espera por instrução** (falta de L1I/iTLB): a
lição da FMV ("L1I na FMV") diz que existe; o A/B de cada fase responde com os
contadores do `perf` (`L1-icache-load-misses`, `iTLB-load-misses`) nas mesmas rodadas.

## 3. Fases

### Fase 1 — separar quente e frio no emissor (todos os blocos, sem mudar comportamento)

Onde: `core/rec-ARM64/rec_arm64.cpp` (`ngen_Compile`, `CheckBlock`, fim de bloco,
`FinalizeCode`), `core/hw/sh4/dyna/driver.cpp` (alocação do cache).

- **Área fria** dentro do cache de código, ao alcance de desvio condicional (`b.cond`
  alcança ±1 MB): dividir o cache em fatias de 1 MB; em cada fatia o código quente cresce
  do início e o frio cresce do fim; quando se encontram, passa para a próxima fatia.
  (Alternativa mais simples: `b.cond` para um trampolim frio de 1 instrução no fim do
  bloco — economiza menos.)
- Vão para a área fria:
  - falha da checagem de código (`blockcheck_fail` → `ngen_blockcheckfail`);
  - fim de fatia: hoje `b.pl corpo; bl intc_sched; cbnz; mov; movk; str; b` →
    `b.mi frio_k` com o resto no frio (o caminho quente perde 1 desvio tomado);
  - caminhos de erro/miss do fim de bloco (link dinâmico que errou etc.);
  - **literais**: as constantes que hoje o vixl põe no fim do bloco vão para a área fria
    da fatia (o `ldr` literal alcança ±1 MB), com codificação manual do `ldr`.
- O bloco quente fica contíguo e sem dados no meio. Mesmas instruções executadas no
  caminho normal (menos um desvio), mesmos ciclos do SH4 → **`FC_STATE_HASH` idêntico**.
- Cuidado: `Relink`/`rdv_LinkBlock` reescrevem o fim do bloco em tamanho fixo (12
  instruções no link dinâmico); o tamanho fixo continua, só o miss vai para o frio. O
  dump do JIT (`B`/`E`/`H`) e o `FC_PERF_MAP` precisam registrar o pedaço frio.

### Fase 2 — tirar a checagem de código executada dos blocos quentes

Onde: `CheckBlock` (`rec_arm64.cpp`), `SetProtectedFlags`/`unprotected_pages`
(`blockmanager.cpp`), stub anti-SMC dos stores.

- Hoje um bloco em página não protegida (`ro=0`: página com dado escrito com
  frequência ao lado de código) compara **todos os bytes do SH4** a cada entrada.
- Troca: **versão por página**. Cada página de 4 KB da RAM tem um contador; todo
  store que cai numa página com código (o stub anti-SMC já intercepta esses stores) o
  incrementa. O bloco guarda a versão da compilação e, na entrada, faz
  `ldr w, [versão_da_página]; cmp w, #v; b.ne frio` — 3 instruções em vez de 4 por 8
  bytes, e sem constantes de 8 bytes. Se a versão mudou, o frio compara os bytes (como
  hoje) e, se o código for o mesmo, só atualiza a versão (dado escrito ao lado, não SMC).
- Alvo direto: Shenmue (9,3% dos bytes quentes), EGG, MvC2.

### Fase 3 — arena quente com os blocos em ordem de execução

Onde: `driver.cpp` (alocação/recompilação), `blockmanager.cpp` (`bm_*`, `Relink`), reuso
do cache principal (todo ele livre desde a remoção do tier2, 2026-10-10).

- **Quem é quente, sem custo por bloco:** amostrar no `intc_sched` (uma vez por fatia,
  ~448 ciclos do SH4) o bloco onde a fatia acabou (`next_pc`) e somar num contador do
  bloco. Sem instrução extra no caminho quente (o `FC_BLOCK_PROF` custa 4 por entrada).
- **Promoção:** bloco que passa de um limiar é recompilado na **arena quente** (uma área
  contígua de ~128-256 KB), junto com os sucessores ligados (`pBranchBlock`/`pNextBlock`)
  que também estão quentes, em ordem de cadeia: o salto mais provável vira queda direta
  para o próximo bloco. Os links antigos são refeitos (`Relink`) para apontar à cópia.
- **Limite:** a arena tem tamanho fixo; quando enche, os menos quentes (contador com
  decaimento) saem e voltam a apontar para a cópia normal.
- **Reset do cache** (`recSh4_ClearCache`) zera a arena junto.

### Fase 4 — menos bytes por instrução SH4 (genérico)

Encolhe tudo, quente incluído (exemplo medido: 43 instruções ARM64 para um laço de 4
SH4, `8C16BB32` do Napple):
- **T direto no desvio**: hoje `cmp; cset; str T; ldr T; cmp; b.ne` → `cmp; b.cond`
  quando o T só é usado pelo desvio do fim do bloco (gravar o T no contexto só no frio /
  na saída).
- **Registradores do SH4 em registradores do host dentro do bloco**: gravar no
  contexto só na saída do bloco e antes de chamadas ao runtime.
Maior esforço; depois das fases 1-3, que não mudam o código gerado do corpo.

### Fase 5 (opcional) — lado C++

O resto da thread (30-55%) também disputa a L1I: compilar o core com o perfil do device
(`-fprofile-use`/AutoFDO a partir do `perf`) ou ao menos agrupar as funções quentes
(`__attribute__((hot))`, `-freorder-functions`) para elas ficarem juntas.

## 4. Validação de cada fase

1. Build com as flags do `CLAUDE.md` (`-j2`, `make clean` com os mesmos argumentos se
   mexer em header amplo).
2. `FC_STATE_HASH` + `FC_RTC_FIXED` + `FC_INPUT_NEUTRAL`: frame a frame
   **idêntico** ao core atual (DOA2, Shenmue II, Napple). Fases 1-3 só mudam o lugar do
   código; qualquer diferença é bug.
3. A/B de 2 rodadas (savestate, cfg de debug, `perfmax`): VEL%, fps novos, frame time
   p50/p95/p99, dupes; nas mesmas rodadas, `perf stat` de `L1-icache-load-misses` e
   `iTLB-load-misses` da thread de emulação. Jogos: DOA2, Shenmue II, Napple (+ Shenmue
   na fase 2).
4. Commit do marco; números em `history.md` e `game_status.md`.

## 5. Riscos

- **Alcance de desvio** (`b.cond`/`ldr` literal ±1 MB): resolvido pelas fatias de 1 MB;
  `bl`/`b` (±128 MB) cobrem o cache inteiro (15 MB).
- **Relink e descarte de blocos** com a parte fria separada: o `bm_DiscardBlock` tem que
  liberar as duas partes; o link dinâmico (4.120) reescreve só o fim quente.
- **Ganho incerto**: se a espera por instrução for pequena, as fases 1 e 3 empatam; a
  fase 2 ganha no Shenmue de qualquer jeito (corta instruções executadas), e a fase 4
  corta trabalho em todos.
