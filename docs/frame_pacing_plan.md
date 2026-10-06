# Plano — Pacing de frames / estabilidade da cauda

> Origem: feedback do usuário 2026-09-27 (MvC2/CvS2 descraschados rodam suaves
> nos menus/load, mas a luta tem VEL < 100% e cauda de hicups). Objetivo:
> **distribuir os frames igualmente e estabilizar o frame time**, buscando o
> melhor tradeoff fps × VEL. Não é "maximizar fps" nem "maximizar VEL" — é o
> joelho da curva.

## 1. O que foi medido (MvC2, savestate de luta, tier2 on)

`FC_REND_SPLIT=1` + `FC_IDLE_FF_STATS=1` (benchmark 15s):

| Métrica | Valor | Leitura |
|---|---|---|
| `game_interval_ms_ema` | 16,52 | o jogo pede 60 fps (16,7ms) |
| `emu_frame_interval` | 18,5ms | a emu produz ~54 fps |
| `process_ms` (TA parse) | 3,67 | main thread |
| `render_ms` (GL submit) | 5,00 | main thread |
| `video_p50` (present) | 13,9 | **frontend + swap, o maior item** |
| `render_work_ms_ema` | 10,53 | Process+Render |
| `rsWait_ms` | 5,90 | main espera a emu |
| `dropped_frames_rqueue_busy` | 321 | de ~908 frames do jogo (~35%) |
| `waited_for_render` | 2 | quase nunca espera |
| `hiccups` / `hiccup_rate` | 49 / **8,16%** | frames > 2× a mediana (48,7ms) |
| `frame_median_ms` | 24,37 | mediana do `retro_run` |

**Diagnóstico:** o ciclo da main thread (`process` + `render` + `present`) ≈
22,6ms > intervalo do jogo (16,5ms). O jogo produz 60 fps, a main thread só
apresenta ~35, e o descarte é **reativo** (quando a `rqueue` está ocupada) →
espaçamento irregular → sensação de hicup. A VEL (~96%) já está boa porque o
descarte não bloqueia a emu; o problema é a **distribuição**, não a velocidade.

## 2. Ideia central

**Desacoplar taxa de apresentação da taxa do jogo.** Em vez de apresentar 60 e
descartar quando a fila enche (irregular), apresentar **1 a cada N frames do
jogo** (`pacerDiv`), com N escolhido para a main thread caber folgada — os
frames apresentados ficam **igualmente espaçados**. A VEL não muda (a emu roda
igual); só a apresentação fica even.

- N=1 → 60 fps (só se a main thread sustentar).
- N=2 → 30 fps even.
- N=3 → 20 fps even. Etc.

Nunca apresentar taxa "quebrada" (ex.: 45) — em 60Hz ela vira 60/30 alternado,
que é justamente a irregularidade a evitar.

## 2b. RESULTADO do pacer (2026-09-27): NEGATIVO — pular frames piora

Implementado `g_pacerDiv` (pula 1 a cada N frames) + adaptação por descarte
residual, em `ta_ctx.cpp` (`FC_PACER`, `FC_PACER_DIV`). Medido no MvC2 (save):

| Config | core_frames | core_p50 | core_p95 | active_p95 |
|---|---|---|---|---|
| off | 517 | 10,4ms | 33,7ms | 46,8ms |
| div=2 | 423 | 13,9ms | 54,8ms | 68,9ms |
| div=3 | 286 | 33,7ms | 96,2ms | 102,7ms |

**Pular frames piora tudo.** A adaptação também **oscilou** (div 1↔2; lição 5.2).
A leitura: a main thread fica **ociosa** (p50 10,4ms por frame, 517 frames em
15s = ~5,4s de trabalho em 15s). Não é CPU-bound. Pular só faz a main thread
**esperar mais** no `rs.Wait` (o próximo frame demora N× o intervalo), sem
reduzir o custo por frame apresentado — a cauda (p95/p99) acompanha o intervalo.
**A premissa "limitar o frame" está furada**: o frame time não melhora, só cai o
número de frames. Fica opt-in (`FC_PACER=1`), default off.

**Pivô:** o gargalo real não é o número de frames, é o **custo por frame
apresentado** (process+render+present) e o **motivo do descarte** (321 frames
descartados com a main thread ociosa — a `rqueue` está ocupada quando o próximo
frame chega, mesmo com a main thread parada). Próximo passo: instrumentar o
descarte (por que a `rqueue` está ocupada com a main ociosa) e atacar isso, não
o número de frames.

## 2c. Orçamento por TEMPO (2026-09-27) — espaça mas não melhora a cauda

Implementado `g_renderBudgetUs` (`ta_ctx.cpp`): se o frame chega antes do
orçamento desde o último renderizado, **descarta a renderização e segue**
(não espera → VEL 100%). Fixo (`FC_RENDER_BUDGET_MS`) e adaptativo (descarte
residual → aumenta o intervalo). Medido no MvC2 (save):

| Budget | frames | VEL% | core_p50 | core_p95 | active_p50 |
|---|---|---|---|---|---|
| off | 519 | 95,8 | 10,4ms | 32,1ms | 24,3ms |
| 16ms | 468 | 95,9 | 10,7ms | 40,7ms | 24,7ms |
| 20ms | 381 | 95,9 | 25,0ms | 43,6ms | 38,9ms |
| 25ms | 350 | 95,9 | 27,9ms | 60,6ms | 42,0ms |

- O budget **espaça** (active_p50 = B) mas **não melhora a cauda** (p95 piora,
  pois o frame time passa a ser = B).
- **VEL fica ~96% em TODOS** — para o MvC2 o limite é a **emu (SH4)**, não a
  espera do render. O "não esperar" não muda a VEL aqui.
- **Adaptativo ficou em 0** para MvC2 e Shenmue (`render_budget_ms 0.00`):
  `render_work (10,5/12,4ms) < intervalo do jogo (16,5/33ms)` → o render **cabe**,
  não há descarte residual → não sobe. (Shenmue é 30fps: intervalo 33ms.)

**Pivô 2:** a cauda de ~100ms (`core_p99=100,4ms` em TODAS as configs) é
exatamente o **timeout do `rs.Wait(100)`** (`Renderer_if.cpp:456`; o `rs` só é
sinalizado quando o frame é enfileirado, `:682`). Ou seja: o hicup é a main
thread **esperando o timeout inteiro** — sinal de stall da emu (JIT? CHD?
texture?), não de pacing. Próximo: instrumentar o timeout (contar/quando) e a
fonte do stall.

**Retrorun (fork vizinho):** o adaptive frameskip é **por dívida**
(`frameDebt`/`debtRatioEma` → `skipPeriod` → `skipNextVideoFrame` em
`core_video_refresh`) e **gated por `retrorun_loop_declared_fps`** (`main.cpp:1412`),
que está **false** no cfg do device → nunca ativa. Mesmo ligando, `skipped_adaptive=0`
no MvC2 (a dívida não chega ao limiar).

## 3. Como escolher N (adaptação)

Sinal: **descarte residual** (fila ocupada) depois do pacer. Com o pacer ativo,
o descarte reativo deve cair a ~0. Regra (janelas de ~120 frames, histerese):

- Ainda descarta (residual > 5% da janela) → `pacerDiv++` (a main thread não
  cabe; aperta).
- Sem descarte residual por uma janela e `pacerDiv > 1` → `pacerDiv--` (probe:
  tenta apresentar mais).
- Adaptação **lenta** (lição 5.2: controle por sinal ruidoso oscila) e **nunca**
  realimentar por fps.

`FC_PACER=0` desliga (A/B). `FC_PACER_DIV=N` fixa N (diagnóstico).

## 4. Reduzir o trabalho da main thread (a outra metade)

O pacer esconde a irregularidade; reduzir o `present`/`render` aumenta o N que
cabe (mais fps). Itens já mapeados:

- `video_p50=13,9ms` (present) — frontend/retrorun3 + swap; é o maior item.
  Ver o que o retrorun3 faz em `video_cb` (vsync? cópia?); o core não controla,
  mas dá pra medir e talvez reduzir resolução de swap / evitar glFinish.
- `render_ms=5,0` — batching de draw call (item 4.2; `rs_drawcall` 2,1ms,
  `rs_opaque` 2,0, `rs_transl` 2,4). Batching sobe o N que cabe.
- `process_ms=3,67` — TA parse; `make_index`/`fix_texture_bleeding` (item 4.1).

## 5. Cauda longa (os hicups de ~100ms)

`core_p99=100ms`, `active_frame_p99=103ms`, `hiccup_rate=8%`. Fonte ainda não
isolada. Candidatos: stall de GL (upload de textura/shader), leitura de CHD,
formação/compilação do tier2 na emu thread (sem `FC_TIER2_FULL_THREAD`), burst
de JIT. **Próximo passo:** instrumentar `retro_run` para logar o frame > 40ms
com o split (wait/process/render/present) e identificar o culpado.

## 6. Método de validação

- A/B no **mesmo savestate** (`retrorun_auto_load=true`), fps + **VEL%** +
  `core_p50/p95/p99` + `active_frame_p50/p95/p99` + `hiccup_rate` +
  `dropped/duplicated` + underruns de áudio.
- Alvo: **reduzir `hiccup_rate` e a cauda (p95/p99)** mantendo VEL e a média
  de fps; a média sozinha não vale.
- Sensação: o usuário avalia no device (a tela é o juiz final).

## Resultado (2026-10-06) — o prazo da main vira o fps MEDIDO do jogo

A fase 3 (`FC_FRAME_WAIT_MS` fixo em 20 ms) resolveu os 60 fps, mas **não** os 30: o
prazo de 20 ms < o intervalo de 33 ms, então a main estourava o prazo e devolvia
repetido, e o retrorun reapresentava (Grandia II: ~50/s com 40% de dupes, apesar da
VEL 100%). Correção (4.110): o core mede o intervalo entre frames novos (EMA) e a main
espera o frame real por 2× o intervalo — **30 fps apresenta 30, 60 apresenta 60**.
Detalhe e tabela em `docs/sync_emu_render.md` §10. Isto **encerra o item**: o pacing
deixou de ser um prazo fixo e passou a acompanhar o jogo (que alterna) sozinho.
- Registrar em `tech_debits.md` (status) e `history.md` (timestamp).

## 7. Ordem

1. **Pacer determinístico** (`pacerDiv` + adaptação) — o item desta sessão.
2. Instrumentar a cauda (frame > 40ms com split) e atacar a fonte.
3. Reduzir `present`/`render` (mais N cabe).

## 8. Referências

- `core/hw/pvr/ta_ctx.cpp` — `QueueRender` (pacer entra aqui), `g_queueDrops`,
  `g_queueBusyDrops`, `g_rendWorkUsEma`, `g_rendIntervalCyclesEma`.
- `core/hw/pvr/spg.cpp` — `SH4FastEnough` (85% em 4 vblanks).
- `core/libretro/libretro.cpp` — `g_lastFrameTimeMs`, `g_hicCount`,
  `g_measuredFps`, `FC_IDLE_FF_STATS`, `FC_REND_SPLIT`.
- `core/hw/pvr/Renderer_if.cpp` — `rend_frame`, liberação antecipada,
  `g_rendBusyUntilUs`.
- `docs/rendering_improvement_plan.md` — batching (item 4.2).
- `docs/tech_debits.md` 4.29 (espera × descarte), 5.2 (controle por fps oscila).
