---
name: jit-nativo
description: Ciclo de otimização do flycast deste repositório no Dreamcast — dump leve do JIT, leitura do código gerado (análise estática), reescrever a função/trecho quente em nativo e plugar por assinatura dos bytes SH4 (hle_fn.cpp), validando por FC_STATE_HASH antes de medir. Use sempre que for otimizar uma função quente do JIT, "achar o gargalo no dump", "reescrever essa função em nativo", "plugar a correção por assinatura", ou continuar o trabalho da lightxf/emissor de strips/laço de vértices do DOA2/Shenmue.
---

# Otimizar uma função quente do JIT em nativo

Método vigente no Dreamcast (pedido do usuário; ver `history.md` 2026-10-01 e
`tech_debits.md` 4.100/4.101/4.106). A ideia central: **não** fazer profiling em
volta do tier2 — **dumpar o código que o JIT gera, ler como análise estática,
reescrever o trecho ruim em nativo e plugar a correção por identificação da
função** (assinatura dos bytes SH4). Casos de referência: `lightxf` (4.101) e o
emissor de strips do Napple (4.106).

Antes de rodar/medir no device, leia `rodar-games` (backup de core, logs, `perfmax`).

## 0. Onde estão os dumps (device)

O `tools/rr_capture.sh` (instalado em `/home/ark/rr_capture.sh`, chamado pelo
`dreamcast.sh`) cria **uma pasta por sessão**:

```
/roms2/dcbat/<AAAAMMDD-HHMMSS>_<jogo>/
```

com `export FC_JIT_DUMP="$D" FC_JIT_DUMP_LITE=1 FC_PERF_MAP=1 FC_SYNC_STATS=1`.
Dentro: `jit-*.txt` (o dump), `samples.txt.gz` (`perf`), `sync-stats`,
`bench.json`, `live.log`, `exit_code.txt`. Dumps manuais antigos ficaram em
`/roms2/jitdump_napple*/`.

**Regra de ouro do dumper:** dump serve para a **distribuição** do código quente,
**nunca** para tempo/VEL. Métricas sempre de uma rodada **limpa** separada (o
`FC_JIT_DUMP` completo derrubou o Napple de VEL 74,8% → 23,8% — 4.98).

## 1. Dumpar

```bash
# device, cold boot ou savestate, entre perfmax/perfnorm:
FC_JIT_DUMP=/roms2/dcbat/<jogo> FC_JIT_DUMP_LITE=1 FC_PERF_MAP=1 \
  retrorun3 --benchmark 40 --benchmark-warmup 5 "<rom>"
# (o rr_capture.sh já faz isso + perf record -F 299 -k mono)
```

Custo do modo leve: ~2,4 pts de VEL. `FC_JIT_DUMP_LITE=1` grava só o código na
compilação (SH4/SHIL/**ARM64** por bloco) + linhas `t <mono> <realtime>`.

## 2. Ler (análise estática)

- `tools/jit_lite_report.py` — resolve amostra do `perf` → bloco (pelo endereço e
  instante; o cache de código é o array `SH4_TCB` dentro do `.so`) e **separa
  regiões do tier2 de stubs**. Opções `--window`, `--at`, `--log`,
  `--tcb-off 0x3a2568`.
- `tools/sh4dis.py` — desmonta o SH4 de uma faixa a partir do dump (usa a tabela
  de opcodes do próprio flycast).

Objetivo: achar a **função/laço quente** e **entender o algoritmo** (o que ele
faz de fato, não só onde está quente). Ex. Napple: laço externo por strip + laço
interno de 1 bloco por vértice, 2 rajadas de 32 B na SQ por vértice, `FPSCR.SZ=1`.

## 3. Reescrever em nativo (`core/rec-ARM64/hle_fn.cpp`)

- Tabela de entradas `{vaddr, id=(func<<8)|entry, sig, name}` em `hle_fn_lookup`;
  `hle_fn_run(cycles, id)` retorna bit 32 = tratou, e o `w27` (ciclos) nos 32 bits
  baixos.
- **Mesmas operações de float na ordem/fusão do JIT**: NEON com `fp-contract=off`
  (`ftrv` = `fmul` + 3 `fmla`; `fipr` = produtos sem fusão, soma em pares
  `(p0+p1)+(p2+p3)`; `fmac` fundido). Isso é o que garante resultado **bit a bit
  idêntico** — a Mali/`fma` fundido mudaria decisões do jogo (ver 4.102).
- **Mesma contabilidade de ciclos**: desconta os ciclos de cada bloco do JIT na
  mesma fronteira e, quando a fatia acaba, faz o que o `intc_sched` faz (fatia +
  `UpdateSystem` + `rdv_DoInterrupts_pc`) — interrupções caem no **mesmo ponto**.
- Cuidado com **ciclos pré-calculados** (a 1ª `lightxf` recalculava por fronteira
  e custava o mesmo que o JIT; a 1ª condição do caminho rápido esqueceu o custo
  fixo da varredura → `UpdateSystem` atrasado → leitura de TMU 2 ticks diferente —
  o hash pegou).

## 4. Plugar por assinatura

O gancho é emitido em `ngen_Compile`, depois da checagem de ciclos: o JIT, ao
compilar o **bloco de entrada com os bytes SH4 batendo**, chama a versão nativa e
cai num `arm64_no_update` (o `next_pc` vai pro `w29`). Se a assinatura não bate ou
o caminho rápido não cobre o caso, **BAIL** para a entrada do bloco do JIT
correspondente (nunca resultado diferente). `FC_HLE=0` desliga; `FC_HLE_LOG=1` loga.

> Distinção: isso é o **`hle_fn`** (substitui a função por assinatura de bytes).
> O **tier2** tem o mecanismo irmão de **padrões** (`idle_ff_sigs` — ex.
> `chained-wait-loop-getter-cmp`, `wait-flag-task-loop`; `region_bad_sigs`/
> `tier2_bad_pattern` — ex. exclusão de bloco que estriava o mslug6). Mesma ideia
> de "identificar o padrão no jogo", camadas diferentes.

## 5. Validar (antes de medir)

`FC_STATE_HASH` + `FC_RTC_FIXED` + `FC_INPUT_NEUTRAL` com **tier2 desligado**:
hash completo (RAM/VRAM/ARAM/ctx) em **todo** frame, `FC_HLE=0` × ligado,
**idênticos**. Ex.: `lightxf` 560 frames + 380 com hash completo; emissor de
strips 402 frames. Sem isso, não meça.

## 6. Medir (A/B)

Mesma cena/savestate, `perfmax performance`, 2 rodadas por lado, e a tabela
completa: **fps + VEL% + frame time p50/p95/p99 + média + underruns**. Referência:
emissor de strips do Napple → VEL 77,0 → 87,0%, fps 23,1 → 26,1, e a thread de
emulação JIT 81% → 47%. Feche a pergunta aberta ao usuário sobre o que ele viu na
tela. **Commite o marco** (código + docs) assim que validado.

## 7. Registro e alvos abertos

- `docs/tech_debits.md`: novo item com o ganho medido. `docs/history.md`: entrada
  com timestamp. `docs/current_plan.md`: estado do alvo.
- **Alvos abertos do DC:** laços de vértice do DOA2/Shenmue II/Shenmue (a base
  dumpada mostra o laço de vértices → SQ dominando nos quatro — DOA2 região tier2
  #1 = 14% da emu; Shenmue II 7,5%); controle do AICA; **IDCT da Sofdec**
  (`docs/fmv_plan.md`).
