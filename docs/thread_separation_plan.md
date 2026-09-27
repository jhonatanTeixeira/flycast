# Plano — Separação de threads (topologia de execução)

> Origem: discussão de 2026-09-27 sobre "modelo distribuído" por TOC
> (Goldratt) para o RK3326 (Cortex-A53 quad-core @ 1,5 GHz). Este doc é o
> guarda-chuva da topologia de threads; os planos específicos ficam nos docs
> referenciados (`arm64jit_improvement_plan.md`, `tier2_adaptive_plan.md`,
> `rendering_improvement_plan.md`). Método do projeto: **medir antes de
> otimizar** e validar com `state_compare` — ver `CLAUDE.md`.

## 0. Resumo executivo (a decisão)

A topologia correta **não é uma linha de 3 estágios** (JIT → tier2 → execução).
É **2 threads quentes + N helpers parkeados**:

- **2 quentes** (sempre runnable, uma por core): **emu** (execução SH4 + JIT por
  bloco + scheduler) e **main/GL** (parse da TA + submissão GL + present).
- **N parkeadas** (0 CPU em regime): **tier2 worker**, **AICA render worker**,
  **pool do CHD** (e, quando o jogo usa, loader de textura custom e rede).

O critério de separação **não é estágio de pipeline** — é **sensibilidade a
latência + propriedade de recurso**:

- No **caminho crítico** (o resultado é preciso *antes* da próxima entrada ser
  conhecida) → fica no core quente.
- **Cold / adiável / especulativo** → vira pulmão numa thread parkeada.

O JIT **por bloco** está no caminho crítico (o bloco precisa existir antes da
primeira execução dele) → fica fundido com a execução na emu thread. A
**formação de região** é cold/especulativa → worker. Essa é a divisão que faz o
"bastão + pulmão" funcionar; o resto é ilusão de ganho.

## 1. Fatos medidos que restringem o desenho

Nenhuma decisão deste plano vale sem estes números (todos já medidos no projeto):

1. **O gargalo é execução serial, não compilação.** Compilação de bloco =
   **~1,2%** do tempo total (1,95 s de 159 s — `current_plan.md`, item 9).
   `SH4_TCB` = **40–51%** de self-time; `SH4_SPEED_RATIO` cai a **0,69–0,74** em
   cena pesada (itens 1 e 6). Ou seja: T0/T1/T2 (input/decode/codegen) somam
   ~1% do trabalho.
2. **O segundo gargalo é submissão de draw call** no Mali: `glDrawElements`
   ~15 ms dos ~25 ms de `render` em cena pesada (`current_plan.md`, item 4.2).
3. **O GL já roda em thread separada.** `ThreadedRendering` é **padrão
   `enabled`** (`core/libretro/libretro_core_options.h:671`, "Highly
   recommended"): a emu thread só monta a display list e enfileira
   (`QueueRender`, `core/hw/pvr/ta_ctx.cpp:133`); a main thread faz `Process` +
   `Render` (`rend_single_frame`, `core/hw/pvr/Renderer_if.cpp:443`).
4. **O acoplamento emu↔GL é de profundidade 1.** A fila tem **um slot** e a emu
   thread espera em `re.Wait()` quando o render estoura o intervalo do jogo
   (liberação antecipada, `Renderer_if.cpp:324-405`). Thread separada não é "de
   graça": só é, enquanto `Process+Render ≤ intervalo do jogo`.
5. **O bastão/pulmão já existe e já foi medido.** Fila SPSC sem lock + batching
   (`T2QBATCH=64`) + park em futex (`core/rec-ARM64/tier2.cpp:1657`). Custo na
   emu thread: **435 → 83 µs/frame** (Shenmue), 283 → 68 (DOA2) — `history.md`
   4.83. O `safe_point` chamado por fatia custava **~0,9 fps**, corrigido com
   gate `&63` (4.84).
6. **Região ajuda DOA2 e atrapalha Shenmue.** Branch following: DOA2 VEL
   93,6 → 96,3%; Shenmue 27,3 off × 26,6 on (overhead de gerir região de baixo
   reúso) — `history.md` 4.82. O fix é decidir **antes** de instalar
   (`tier2_adaptive_plan.md`, Ideia B), não remover o tier2.

## 2. Topologia alvo

| Thread | Quente? | Propriedade (recurso) | IPC com as outras | Onde no código |
|---|---|---|---|---|
| **emu** | sim | estado SH4 (`ctx`), code cache, scheduler | TA queue + `rs`/`re` (render); SPSC (tier2) | `libretro.cpp:221`, `driver.cpp` |
| **main/GL** | sim | contexto GL, framebuffer, present | `rs`/`re`, slot de 1 | `Renderer_if.cpp:443`, `gles.cpp` |
| **tier2 worker** | não (park) | codegen de região, `workerBlocks` | SPSC + futex, lote 64 | `tier2.cpp:1657` |
| **AICA render** | não (park) | mixagem de áudio | fila + park | `sgc_if.cpp:1565` |
| **CHD pool** | não (park) | descompressão de imagem | fila + park (4 workers) | `imgread/chd.cpp:43` |

Condicionais (só quando o jogo usa): loader de textura custom
(`core/rend/CustomTexture.h:49`), rede Pico (`network/picoppp.cpp:1040`), M3
comm do Naomi (`naomi/naomi_m3comm.cpp:266`).

**Contagem:** tipicamente **5** (2 quentes + tier2 + AICA + CHD). Em 4 cores
isso só é seguro porque as parkeadas ficam em **0 CPU** em regime — é a regra do
pulmão (seção 3).

## 3. Invariantes (regras da separação)

Quebrar qualquer uma destas anula o ganho e/ou quebra o determinismo:

1. **No máximo 1 thread runnable por core em regime.** Helper acordado que
   compete com a emu thread é pior que helper inexistente. (Oversubscription em
   A53 + bouncing de cache line é regressão, não ganho.)
2. **Buffer limitado, sempre.** Fila cheia → descarta (fire-and-forget), nunca
   bloqueia o produtor. Fila de render: profundidade 1 (latência). Tier2: SPSC
   de tamanho fixo.
3. **Zero lock no caminho quente.** `bm_GetBlock2` é chamado em
   `rdv_DoInterrupts` (saída de bloco) — por isso o worker **não** toca o
   `blkmap` e mantém `workerBlocks`/`workerByCode` próprios (`history.md` 4.83).
4. **Payload = só trabalho cold.** O pulmão não carrega o que a emu thread
   precisa agora (execução, codegen por bloco). Carrega região, textura, CHD.
5. **Park em futex, 0 CPU.** Espera ativa em helper rouba o core do gargalo.
6. **Determinismo.** Toda publicação/instalação acontece em ponto seguro;
   qualquer decisão adaptativa muda o código gerado e **tem** de passar por
   `state_compare` (tier2 on×off). Timing emulado não pode depender de quando o
   worker acordou.
7. **Sinal de controle ≠ fps.** Adaptar por VEL/custo/reúso/estado de fila.
   Realimentar por fps oscila (lição 5.2 do `tech_debits.md`).

## 4. Gap analysis — o que já existe × o que falta

**Já existe (não refazer):**
- emu e main/GL separadas com TA queue + `rs`/`re` (ThreadedRendering).
- tier2 worker com SPSC + batching + futex, custo emu 435→83 µs.
- gate `&63` no `tier2_safe_point` (call por fatia).
- AICA e CHD parkeados.
- no-lock no caminho quente do block manager.

**Falta / a verificar:**
- [ ] **Confirmar `flycast2026_threaded_rendering=enabled` no device** (cfgs em
  `~/.config/retroarch/retroarch-core-options.cfg` e `~/.config/retrorun.cfg`).
  Se estiver off, o GL está inline na emu thread — maior alavanca isolada e
  explica "GL come 20% do core".
- [ ] **Medir o stall emu↔render**: quanto a emu thread perde em `re.Wait()`
  por frame, por jogo (instrumentar o `re.Wait` de `QueueRender`).
- [ ] **Contadores do pulmão do tier2** (`tier2_adaptive_plan.md`, pré-requisito):
  descartes por fila cheia, nº de wakes, tamanho médio de lote, latência de
  formação, reúso por região.
- [ ] **Decisão adaptativa antes de instalar** (Ideia B do tier2_adaptive).
- [ ] **Reduzir o trabalho do render** (batching de draw call, item 4.2) — é o
  que libera a emu thread do `re.Wait`, mais do que qualquer thread nova.
- [ ] **Elevar o core quente**: qualidade do ARM64 gerado / overhead por bloco
  (`arm64jit_improvement_plan.md`; `UpdateSystem` ~7.400×/frame; SQ).

## 5. Itens de trabalho (ordem de execução)

Cada item com método de A/B e status. Status: `pendente` · `in progress` ·
`done` · `descartado`.

> **Auditoria 2026-09-27 (sessão atual):** itens 1,2,3,5 = `done`; item 4 =
> `parcial`; item 6 = aberto. Ver §5b (achados novos: no-wait, AICA futex,
> `mprotect`, present do retrorun).

1. **Verificar threaded_rendering no device** — `done`. Device = `enabled`
   (`retrorun.cfg` + `retroarch-core-options.cfg`); default `enabled`
   (`libretro_core_options.h:671`).
2. **Instrumentar o stall do render** — `done`. `g_emuReWaitUs`/`g_emuReWaits`
   (`Renderer_if.cpp:707`) + `g_rendWaitUs`. Foi o que provou que o hicup era o
   `rs.Wait(100)` (não o `re.Wait`) → levou ao modelo no-wait (§4b).
3. **Contadores do pulmão do tier2** — `done`. `FC_TIER2_STATE`
   (`tier2_dump_state`) loga `entradas/blocos_exec/nblocos/pref/write/checked` +
   `pacer/render_budget` no `FC_IDLE_FF_STATS`.
4. **Adaptativo por reúso+VEL, decidir antes de instalar** — `parcial`. Hoje
   `check_regions` (`FC_TIER2_CHECK_ENTRIES`/`CHECK_BPE`) decide **depois** de
   instalar. O "antes" (Ideia B) **não** foi feito.
5. **Batching de draw call** — `done (opt-out)`. `PP_SameGPUState` +
   `canBatch` (`gldraw.cpp:311/341`), one-upload (default on).
6. **Qualidade do JIT / overhead por bloco** — `aberto`
   (`arm64jit_improvement_plan.md`).

### 5b. Achados novos da auditoria (2026-09-27)

- **Modelo no-wait** (feito): emu nunca espera (`g_emuNeverWaits`); a main
  thread faz dequeue não-bloqueante e devolve duplicado. Cauda `core_p99`
  100→11-39ms, VEL ~100% em CvS2/DOA2. Custo: `duplicated_frames` ~1/3.
- **AICA→futex** (feito): o `aica_block_acquire`/`aica_mix_sync` girava em
  `yield` (perf: `__sched_yield` ~20% do CPU da emu). Convertido pra
  mutex+condvar (park 0 CPU). Invariantes 2 e 5.
- **Item 2 — `mprotect`**: `_vmem_protect_vram` → `mem_region_lock` = 1
  `mprotect` por região, chamado 3× (P0/P1/P2) + mirror por lock de textura.
  `perf -a`: `__mprotect` = **2,07%**. Real, modesto; reduzir é arriscado
  (o lock é o que detecta escrita na VRAM — evita textura suja por frame).
- **Item 3 — present/GL (o maior, ~14ms)**: o GL (`Process`+`Render`) **já**
  está na main thread (ThreadedRendering). O que falta é tirar o **present**
  (`SDL_RenderCopyEx`+`RenderPresent`, ~14ms) da main. O retrorun **tem** worker
  de vídeo (`video_worker_loop`), mas no R36 ele está desligado por plataforma:
  `videoMultithreadRequested()` retorna **`false` no `RR_PLATFORM_SDL`**
  (`globals.cpp:464`) e `supportsVideoMultithread()` só cobre RG552/RG353. Além
  disso o frontend **apresenta até duplicado** (o `return` que pulava dupe está
  comentado, `video/video.cpp:925`). **Fix:** mudança no **fork do retrorun**
  (habilitar o worker no SDL e/ou pular o present de duplicado) — não é core.
  Build: `make PLATFORM=linux-sdl CXX=aarch64-linux-gnu-g++-13` (feasível, mas é
  o frontend inteiro → risco).

## 6. Anti-padrões (o que NÃO fazer)

1. **Linha síncrona T1 JIT → T2 tier2 → T3 execução.** T1 e T2 são a mesma
   categoria (codegen); separar o JIT por bloco da execução põe o compilador no
   caminho crítico → a emu thread ou espera (VEL cai) ou cai no interpretador
   (`Sh4_int_Step`, caro na A53 — risco já registrado em `t2_tests_plan.md`).
   Teto de ganho ~1,2%, risco alto.
2. **Bastão síncrono no caminho crítico.** Baton-passing só vale para trabalho
   adiável; para o caminho crítico é stall.
3. **Fila ilimitada ou lock no quente.** Ver invariantes 2 e 3.
4. **Realimentar controle por fps.** Oscila (5.2).
5. **Adicionar thread "para paralelizar" sem antes provar que o gargalo é
   divisível.** O gargalo (execução SH4) é serial e stateful; paralelizá-lo
   quebra o estado/determinismo.
6. **Oversubscription.** 7–8 threads runnable em 4 cores = regressão por cache
   coherence, mesmo com futex (o futex só salva se a thread dorme de verdade).

## 7. Questões abertas

- Vale manter o tier2 como opção do core ou ele deve virar sempre-ligado com o
  adaptativo decidindo caso a caso? (Medição atual: ajuda DOA2, atrapalha
  Shenmue — depende do jogo.)
- O pool do CHD (4 workers) chega a acordar todos ao mesmo tempo em load? Se
  sim, isso compete com a emu thread no arranque.
- O AICA render worker e a emu thread disputam o mesmo core em jogos
  audio-heavy? (Item 5 da fila priorizada: ~5,6% no profile 2D.)

## 8. Validação

- **Métrica:** frame time (`active_frame_*`) + fps, sempre **p50/p95/p99 e
  média**, mais **VEL%** (`audio_frames / (duration * 44100)`), sempre os dois
  lados de um A/B no **mesmo savestate/cena**.
- **Correção:** `state_compare` (tier2 on×off) e a bateria (`docs/batery/`)
  como validador final.
- **Device:** `sudo perfmax performance <rom>` … `sudo perfnorm`; duas rodadas
  rápidas + pergunta aberta ao usuário (protocolo do `CLAUDE.md`).
- **Registrar:** todo achado em `docs/tech_debits.md` com status; toda sessão em
  `docs/history.md` com timestamp.

## 9. Referências

- `docs/current_plan.md` — itens 9 (compilação 1,2%), 1/6 (SH4_TCB/SPEED_RATIO),
  4.2 (draw calls).
- `docs/history.md` — 4.82 (branch following), 4.83 (tier2 na thread), 4.84
  (safe_point por fatia).
- `docs/tier2_adaptive_plan.md` — contadores do pulmão e Ideia B.
- `docs/t2_tests_plan.md` — o desenho "um VIXL na thread 2" e o risco do
  fallback interpretado.
- `docs/arm64jit_improvement_plan.md` — elevar o core quente.
- `docs/rendering_improvement_plan.md` — reduzir o trabalho do render.
- `docs/tech_debits.md` — status de cada achado; lição 5.2 (controle por fps).
- Código: `libretro.cpp:221` (emu), `Renderer_if.cpp:443` (`rend_single_frame`),
  `ta_ctx.cpp:133` (`QueueRender`), `tier2.cpp:1657` (worker), `sgc_if.cpp:1565`
  (AICA), `imgread/chd.cpp:43` (CHD),
  `libretro_core_options.h:671` (ThreadedRendering default).
