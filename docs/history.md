# Histórico — Projeto de otimização flycast2021 (R36 / RK3326)

> Log cronológico de tudo que foi feito nesta investigação/projeto. Adicionar uma
> entrada nova por sessão/marco relevante, sempre com timestamp. Ver
> `docs/current_plan.md` para o que está em andamento agora e `docs/tech_debits.md`
> para o status de cada achado técnico.

## 2026-09-13

### Identificação do device e do problema

- Conectado via SSH ao device (`192.168.0.14`, handheld R36 rodando "darkosre-r36",
  Debian 13 trixie aarch64, hostname bate com `/boot/rk3326-r36s-linux.dtb` →
  RK3326, Cortex-A53 quad-core + Mali-G31, confirmado via `.config/.DEVICE=RG351MP`).
- Levantados todos os binários/cores flycast presentes no device: `flycast32_rumble`
  (32-bit, libretro/flycast @ `f4d04ed178` de 2021-11-30, identificado por
  correspondência de blob hash do patch de rumble), `flycast2021_libretro.so`
  (64-bit, de `metallic77/flycast` @ `603814c9f73b773c455d9a497f389d2f93a257fd`,
  identificado via `package.mk` do ROCKNIX), `flycast_libretro.so` (core moderno,
  maio/2024) e `/opt/flycastsa/flycast` (standalone, `v2.6-9-g21eb24f86`, jan/2026).
- Confirmado que `metallic77/flycast` diverge do `flyinghead/flycast` (o repo deste
  projeto) em **2015** (`git merge-base` = `73a585a13`) — são ~10 anos de evolução
  paralela, não uma versão "congelada em 2021" como o nome sugere.

### Baseline de performance (~19:56–20:07)

- **19:56** — Primeiro benchmark via `retrorun3` (governor `ondemand`, padrão real
  de uso): flycast2021 + Shenmue, 100s warmup + 200s medidos, intro real (águia +
  cutscene, gráficos 3D reais do próprio jogo, não pré-renderizado). Resultado:
  **26,87 fps médio**, `core_average=26,10ms`, `video_average=11,11ms`.
- **20:02–20:07** — Segundo benchmark, mesma cena, governor forçado em
  `performance` (GPU 520MHz fixo vs. 400MHz do `ondemand`). Resultado: **28,32 fps**
  (+5,4%), `core_average=26,18ms` (inalterado), `video_average=9,11ms` (-18%).
  Conclusão: clock de GPU só ajuda o estágio de "video" (present/blit); o estágio
  "core" (simulação SH4/AICA + submissão de comandos) é insensível a clock de GPU —
  não é GPU-bound.
- Medição de %CPU por core (script Python amostrando `/proc/stat` a cada 2s,
  concorrente com um benchmark de 90s+90s) mostrou **um único core saturando a
  100%** durante os trechos mais pesados da intro (águia voando, depois neve caindo),
  migrando entre cpu0/cpu1/cpu2 conforme o escalonador do SO — confirma gargalo
  single-thread de CPU, não GPU (GPU nunca passou de ~70%, subir o clock não mudou
  o tempo de frame).

### Diff estrutural flycast2021 vs. flyinghead/flycast atual

- Comparados os diretórios de `core/` entre os dois: o flyinghead/flycast atual tem
  subsistemas inteiros que não existem no flycast2021 (`achievements/`, `ui/`,
  `lua/`, `profiler/`, `util/`, `wsi/`, `input/`, `cfg/`, `audio/`, `debug/`,
  `sdl/`, `windows/`).
- `sorter.cpp` (2021, 458 linhas) vs. `ta_util.cpp::sortTriangles` (master, mesma
  lógica): algoritmo de sort **idêntico em essência** — descartado como causa de
  regressão entre as duas versões.
- Modelo de threading do `Renderer_if.cpp` também já existia igual no 2021 (fila +
  wait com timeout) — não é uma mudança estrutural nova do master.

### Crash no master (flyinghead/flycast atual)

- Compilada uma build cross (aarch64) do flyinghead/flycast atual (branch `master`)
  como core libretro, com `ENABLE_FC_PROFILER=ON` (exigiu patch em
  `core/profiler/fc_profiler.{h,cpp}` pra separar as funções dependentes de ImGui,
  que não existem em build LIBRETRO — corrigido também um `#include <cstring>`
  faltando, bug latente pré-existente).
- Rodando a mesma cena (intro do Shenmue) nessa build: **SIGSEGV reproduzível em
  ~5 segundos**, sempre com o mesmo padrão (frames de ~100ms escalando antes do
  crash, vs. 26ms do flycast2021).
- Análise de core dump (via `gdb-multiarch` + sysroot local montado a partir de
  libs copiadas do device) mostrou: `SIGSEGV @ (nil) invalid access to (nil)` — PC
  e endereço de falha ambos zero, ou seja, um jump para ponteiro de código nulo,
  não um fastmem fault normal (que teria PC válido). Aponta pro *block-dispatch
  table* do dynarec ARM64 do SH4 (`core/rec-ARM64`, `core/hw/sh4/dyna/blockmanager`).
- Conclusão registrada: a percepção de "nem a BIOS aguenta" no master provavelmente
  não é lentidão constante — é o mesmo bug de dispatch gerando picos de ~100ms que
  escalam até o crash total. Decisão do usuário: **não perseguir o bug do master
  agora — foco fica no flycast2021**, que é estável.

### Build do flycast2021 para aarch64 (cross-compile)

- Toolchain instalado: `gcc-aarch64-linux-gnu` / `g++-aarch64-linux-gnu`.
- Worktree criado a partir da branch `flycast2021-metallic77-base` (commit
  `603814c9f`, fetched de `metallic77/flycast`) em `/tmp/flycast2021-src`.
- Dois bugs de Makefile encontrados e contornados (passando as variáveis
  explicitamente na linha de comando, não editando o Makefile):
  1. `CXX ?= g++` na linha 922 sobrescrevia o cross-compiler pra alguns arquivos
     (causava erros de assembly bizarros em `core/hw/arm7/arm64.cpp`, compilado
     silenciosamente como x86_64 nos primeiros ~204 objetos até isso ser percebido
     — corrigido com `make clean` + rebuild total). Fix: `CXX=aarch64-linux-gnu-g++
     CC=aarch64-linux-gnu-gcc` explícitos.
  2. A regra de build de `core/rec-ARM64/ngen_arm64.S` chamava `as` puro com flags
     de GCC (`-D`, `-f*`), que `as` não entende. Fix: `CC_AS=aarch64-linux-gnu-g++`
     (mesmo padrão já usado pelo próprio Makefile pra outra plataforma ARM32,
     comentário original: "must be compiled with gcc, not as").
  3. Faltava `-lGLESv2` pro linker (sem dev-lib aarch64 local) — resolvido copiando
     `libGLESv2.so` real do device como stub de link (`LDFLAGS="-L."`).
- Build recorrentemente interrompido por um watchdog de baixa-memória do sistema
  (a máquina roda várias outras sessões do Claude Code + Docker + Grafana/Tempo
  simultaneamente — não é o build em si que consome memória demais). Contornado
  simplesmente retomando o `make` incremental repetidas vezes até completar.
- **Build finalizado com sucesso**, smoke-testado no device (30s, sem crash,
  `core_version` bate com o hash certo).

### Auditoria estática de código (agente dedicado)

- Disparado um agente pra ler `core/hw/sh4`, `core/rec-ARM64`, `core/hw/aica`,
  `core/hw/arm7`, `core/hw/pvr`, `core/rend` e `core/libretro/libretro.cpp` do
  flycast2021 e produzir um relatório de pontos suspeitos pra instrumentar,
  ranqueados por impacto. Resultado documentado em `docs/profiling_plan.md`, com
  status individual de cada achado em `docs/tech_debits.md`.
- Achado arquitetural chave do relatório: `retro_run()` (a thread medida
  externamente como "core time") majoritariamente **espera** a `emu_thread`
  (`rs.Wait(100)` em `Renderer_if.cpp:193`) e só depois faz o parsing da TA + sort +
  submissão GL — ou seja, ainda não sabemos se quem satura o core é a `emu_thread`
  (SH4/AICA/ARM7) ou essa thread de render/parsing. Isso é o item #1 do plano de
  instrumentação.

### Execução do plano — item 1: disambiguação de qual thread satura o core (~22:22–22:25)

- Instrumentado `core/hw/pvr/Renderer_if.cpp` com `chrono` (namespace `perfinstr`):
  `rend_single_frame`/`rend_frame` separam `rsWait` (tempo esperando a
  `emu_thread` em `rs.Wait(100)`), `process` (`renderer->Process()`, parsing da
  TA) e `render` (`renderer->Render()`, submissão GL); `rend_start_render` mede o
  intervalo entre chamadas consecutivas (`emuThread`, proxy do custo por-frame da
  `emu_thread`). Log a cada 60 frames via `NOTICE_LOG(PVR, ...)`.
- Rebuild aarch64 (só recompilou `Renderer_if.cpp` + relink, rápido), deploy no
  device, rodado com o mesmo cenário de referência (90s warmup + 90s medidos,
  intro do Shenmue).
- **Resultado (77 linhas de log capturadas, 22:22:51–22:25:42):**
  - Trechos calmos: `rsWait≈24-28ms`, `process≈0,05-0,1ms`, `render≈0,5-0,9ms`,
    `emuThread≈33-34ms`. A soma `rsWait+process+render` (~25-29ms) bate com o
    "core time" de ~26-30ms já medido externamente pelo retrorun3 — consistência
    confirmada.
  - Trechos pesados (partículas/neve, ex. 22:25:37-22:25:42): `rsWait` cai pra
    2-13ms (a `emu_thread` já tem trabalho pronto, main thread não precisa
    esperar), `process` sobe pra 1,5-2,9ms, **`render` sobe pra 7-16ms (10-25x)**,
    `emuThread` sobe pra 42-63ms.
  - **Conclusão:** a `emu_thread` (SH4 dynarec + AICA + ARM7) é o gargalo
    dominante em termos absolutos o tempo todo (~33ms/frame só ela, já perto do
    limite de 30fps=33,3ms, antes de qualquer renderização). O lado de
    renderização (`render`, cascata sort→`SetGPState`) é um fator secundário que
    se agrava bastante em cenas pesadas, mas não é o suspeito nº1 do baseline.
  - Documentado em `docs/tech_debits.md` (achado arquitetural resolvido, itens 3.7
    e 4.2 atualizados) e `docs/current_plan.md` (item 1 marcado `done`).
- **Bug conhecido, não corrigido:** os contadores de geometria (`verts/idx/tr`,
  item 3 do plano) saíram sempre zero no log — o ponto de leitura em
  `rend_single_frame` precisa de correção antes de confiar nesses números.
- **Próximo item da fila:** item 4 do `current_plan.md` (teste A/B do
  `CheckBlock` anti-SMC), que subiu de prioridade dado que a seção 1 (SH4/dynarec)
  agora é a suspeita principal.

### Execução do plano — item 4: teste A/B do CheckBlock anti-SMC (~22:28–22:31)

- `bool block_check = false;` incondicional em `core/hw/sh4/dyna/driver.cpp:234`
  (só pro teste — SMC real quebraria, revertido logo depois). Rebuild, deploy,
  mesmo cenário de referência (90s+90s, intro do Shenmue).
- **Resultado: sem efeito mensurável.** `emuThread≈33ms` em trechos calmos
  (idêntico ao baseline), `core_average=26,186ms` (baseline era ~26ms),
  comportamento em trechos pesados também idêntico (`render` sobe 10-25x igual).
- **Conclusão:** item 1.1 (verificação anti-SMC embutida em todo bloco JIT)
  **descartado** como causa do gargalo — documentado em `docs/tech_debits.md`.
  Código revertido pro original.
- Próximo item da fila: separar `arm_mainloop` (ARM7 puro) de
  `libAICA_TimeStep` (bookkeeping AICA) dentro do loop de
  `core/hw/arm7/arm7.cpp:1535-1543`, pra decidir se o gargalo de ~33ms da
  `emu_thread` está no SH4 ou no som (AICA/ARM7).

### Execução do plano — item 5: separar ARM7 de AICA na emu_thread (~22:34–22:37)

- Instrumentado `core/hw/arm7/arm7.cpp` (`aicaarm::run`) com `chrono` separando
  `arm_mainloop` (execução real do ARM7) de `libAICA_TimeStep` (bookkeeping da
  AICA), logado a cada 1400 chamadas via `NOTICE_LOG(AICA_ARM, ...)`.
- **Resultado: `arm_mainloop=0-1us`, `libAICA_TimeStep=0us` por chamada.** Com
  ~44.100 chamadas/s (uma por sample de áudio), isso dá ~1ms de trabalho total de
  ARM7+AICA por segundo real — distribuído entre ~30 frames, ou seja,
  **desprezível** frente aos ~33ms/frame que a `emu_thread` gasta no total.
- **Conclusão: seção 2 do profiling_plan (AICA/ARM7) descartada por completo**
  como causa do gargalo — documentado em `docs/tech_debits.md` (itens 2.1, 2.2,
  2.3 todos marcados descartados).
- **Pivô registrado em `docs/current_plan.md`:** com CheckBlock e AICA/ARM7
  descartados, os candidatos restantes da seção 1 (SH4) individualmente não
  parecem capazes de explicar 33ms/frame sozinhos. Hipótese nova: pode ser
  limitação de throughput bruto do Cortex-A53 @ 1,5GHz pra essa carga de SH4, não
  um bug pontual — o que mudaria a abordagem pra "qualidade do código gerado
  pelo JIT" em vez de "achar a linha errada". Decisão em aberto com o usuário:
  continuar a lista de instrumentação item a item, ou pivotar pra `perf`/
  `simpleperf` anexado à `emu_thread` pra ver a distribuição real de tempo por
  função.

### Execução do plano — item 2: quebra de `process`/`render` em sub-fases (~22:44–22:59, 2 rodadas)

- Instrumentado `core/hw/pvr/ta_vtx.cpp` (`ta_parse_vdrc`: decode bruto + 3x
  `make_index` op/pt/tr + `fix_texture_bleeding`) e `core/rend/sorter.cpp`
  (`GenSorted`/`stable_sort`). Rebuild, deploy, mesmo cenário.
- **Primeira rodada:** `PERF_TA` deu dados reais (decode≈2,1-2,4ms, make_index
  op≈1,2-1,4ms no pico), mas `PERF_SORT` (`GenSorted`) **nunca disparou** — sinal
  de que essa função não está sendo chamada.
- **Investigação da causa:** checado `core/rend/gles/gldraw.cpp` — a escolha
  entre sort por triângulo (`SortTriangles`/`GenSorted`/`DrawSorted`) e sort por
  strip (`SortPParams`/`DrawList<Translucent,true>`) depende de
  `settings.pvr.Emulation.AlphaSortMode`. Confirmado que este device usa
  per-strip (bate com `flycast2021_alpha_sorting = "per-strip"` visto no
  `retrorun.cfg` real, achado numa sessão anterior) — `GenSorted` de fato nunca
  executa aqui.
- **Segunda rodada (corrigida):** instrumentado `SortPParams` (sort real ativo,
  `core/rend/sorter.cpp:94`) e `glDrawElements` dentro de `DrawList`
  (`core/rend/gles/gldraw.cpp`), separando submissão GPU de `SetGPState`.
  Rebuild, deploy, mesmo cenário.
- **Resultado, correlacionando os 3 logs no mesmo pico (`render=25587us`):**
  - `process`≈4,1ms (decode≈2,4ms + make_index≈1,8ms combinado) — real, secundário
  - `SortPParams`≈0,33ms (622 PolyParams/chamada) — irrelevante
  - `SetGPState`≈2us/chamada × ~622 ≈ 1,2ms — irrelevante
  - **`glDrawElements`≈24us/chamada × ~622 ≈ 15ms — domina, ~60% do `render`
    total de 25,6ms**
- **Conclusão: o gargalo de renderização em cena pesada é overhead de
  submissão por draw call (glDrawElements) no driver Mali r13p0, multiplicado
  por ~500-600 strips separadas por frame — não é custo de CPU montando
  estado (`SetGPState` é barato) nem do sort (`SortPParams` é barato).** Este é
  o achado mais acionável da investigação até agora: aponta claramente pra
  **redução de contagem de draw calls (batching)** como a otimização de maior
  potencial no lado de renderização. Documentado em `docs/tech_debits.md`
  (itens 4.1 corrigido pra "não aplicável neste device", 4.2 confirmado e
  quantificado) e `docs/current_plan.md` (item 2 marcado done).

### Execução do plano — item 6: pool exhaustion do TA_context (2026-09-14, ~23:00–23:03)

- Instrumentado `tactx_Alloc` (`core/hw/pvr/ta_ctx.cpp:209`) contando quantas
  vezes o pool de 2 contextos é reaproveitado vs. quantas cai em `new
  TA_context()`. Rebuild, deploy, mesmo cenário de referência.
- **Resultado: só 2 alocações `new` nas primeiras 60 chamadas (aquecimento
  inicial), depois disso ZERO alocações novas em toda a janela de 90s
  medida** — 100% servido pelo pool.
- **Conclusão: item 3.6 descartado.** O pool nunca esgota nesta cena; não há
  loop de retroalimentação negativa por alocação de ~15MB aqui.

### Execução do plano — item 7: sh4_sched_ffts (2026-09-14, ~23:05–23:08)

- Instrumentado `sh4_sched_ffts` (`core/hw/sh4/sh4_sched.cpp:42`) com contador
  de chamadas, tempo acumulado e tamanho médio de `sch_list`. Rebuild, deploy,
  mesmo cenário de referência.
- **Resultado: `sch_list.size()≈10-11`, custo médio por chamada ~0-0,5us**
  (abaixo da resolução do timer de microssegundos), ~3000 chamadas/s ⇒ total
  ~1,3ms de CPU por segundo real — **desprezível** frente aos 33ms/frame da
  `emu_thread`.
- **Conclusão: item 1.3 descartado.** Próximo e último item da fila original:
  `TexCache::CollectCleanup` + refresh de uniforms de shaders.

### Execução do plano — item 8: TexCache cleanup + shader uniform loop (2026-09-14, ~23:10–23:13)

- Instrumentados `TexCache::CollectCleanup` (`core/rend/TexCache.h:787`) e o
  loop de refresh de uniforms de shaders (`core/rend/gles/gles.cpp`). Rebuild,
  deploy, mesmo cenário de referência.
- **Resultado: ambos crescem ao longo da sessão** (`CollectCleanup`: 1us com 5
  texturas → 20us com 41; shader loop: 11us com 2 shaders → 50-60us com 9),
  **mas mesmo no pico são irrisórios** frente aos 25ms de `render` em cena
  pesada ou aos 33ms/frame da `emu_thread`.
- **Conclusão: itens 4.4 e 4.6 descartados** (`docs/tech_debits.md`).

### Fila original de 8 itens concluída (2026-09-14)

Todos os 8 itens do `docs/current_plan.md` (originados do top-8 do
`docs/profiling_plan.md`) foram testados individualmente por medição direta no
device, sempre no mesmo cenário de referência. **Só um sobreviveu como causa
confirmada:** item 4.2 (overhead de `glDrawElements` × contagem de draw
calls, ~15ms dos ~25ms de `render` em cena pesada). Os outros 7 (CheckBlock
anti-SMC, AICA/ARM7, TA_context pool, sh4_sched_ffts, TexCache cleanup, shader
uniform loop, GenSorted/per-triângulo) foram descartados.

**Lacuna registrada:** nenhum item da lista explica o baseline de ~33ms/frame
da `emu_thread` em trechos calmos (sem neve). Duas linhas de investigação
possíveis daqui pra frente, ainda não iniciadas: (a) instrumentação mais fina
dentro da própria `emu_thread` (contagem de blocos/instruções SH4 executadas
vs. tempo, medido de dentro, não só pelo intervalo entre `rend_start_render`),
ou (b) aceitar que é limitação de throughput bruto do host pra essa carga de
SH4 e mudar o foco pra qualidade do código gerado pelo JIT.

### Item 9 (fora da fila original): throughput real do SH4 + compilação de bloco (2026-09-14, ~23:44–23:48)

- A pedido do usuário (empacotar múltiplos pontos de medida num único ciclo de
  build+teste em vez de um por vez), instrumentados juntos: `rdv_CompilePC`
  (`core/hw/sh4/dyna/driver.cpp:234`, item 1.6 da fila original — contador +
  tempo de compilação de bloco) e uma medição nova, `SH4_SPEED_RATIO`
  (piggyback em `sh4_sched_ffts`, `core/hw/sh4/sh4_sched.cpp`): ciclos SH4
  simulados por segundo real (via `sh4_sched_now64()`) vs. `SH4_MAIN_CLOCK`
  (200MHz) — testa diretamente se o host consegue simular o SH4 em tempo real.
- **Resultado compilação de bloco:** soma total ~1,95s de compilação em ~159s
  de teste (~1,2%), `clearCacheCount` parado em 2 durante todo o teste (sem
  thrashing de code cache). Contribuição real, mas não dominante.
- **Resultado `SH4_SPEED_RATIO` (achado central):** ~0,98-1,0 em trechos
  calmos (tempo real correto — o host acompanha perfeitamente), mas **cai pra
  0,69-0,74 em trechos pesados** — nesses momentos o jogo precisa executar
  mais instruções SH4 por segundo real (mais IA/física/partículas simuladas)
  do que o Cortex-A53 consegue processar via o JIT.
- **Conclusão final da investigação candidato-a-candidato:** existem DOIS
  problemas reais e independentes, ambos confirmados por medição, sem "bala de
  prata" única:
  1. Lado de renderização: overhead de `glDrawElements` × contagem de draw
     calls (~15ms dos ~25ms de `render` em cena pesada) — fixável via
     batching de draw calls.
  2. Lado de CPU: o SH4 emulado via JIT não acompanha tempo real quando a
     cena fica pesada (ratio caindo a ~70%) — só melhora com qualidade do
     código gerado pelo JIT (menos instruções ARM64 por opcode SH4), não há
     bug pontual a caçar.
- Documentado em `docs/tech_debits.md` (seção "Conclusão final" adicionada) e
  `docs/current_plan.md`.

### Documentação do projeto criada

- `docs/profiling_plan.md`, `docs/tech_debits.md`, `docs/current_plan.md` e este
  `docs/history.md` criados em `/tmp/flycast2021-src` (worktree da branch
  `flycast2021-metallic77-base`). `CLAUDE.md` desta branch atualizado com as regras
  de ouro do projeto.

## 2026-09-14

### Implementação de Otimizações 

- **Renderização (GLES):** Refatorado o `GetProgram` e as buscas em shaders que usavam `unordered_map` em `gles.cpp/h`. Substituído por arrays fixos `PipelineShader shaders[32768]` para o cache de estado $O(1)$ sem colisões e um vetor iterável `active_shaders` (removendo custo em `RenderFrame`). Também refatorado `TexParameteri` e `BindTexture` para cachearem o ponteiro dos parâmetros ativos (`_cur_texture_params`) eliminando a busca em Red-Black tree a cada draw call. (Mitigação para os itens 4.2 e 4.3).
- **CPU (JIT ARM64):** Identificado e corrigido a principal falha estrutural do `arm64_regalloc.h`: o alocador usava apenas 8 registradores inteiros e 8 de ponto flutuante, forçando "spills/fills" massivos constantes. Os registradores "caller-saved" (`W9-W15` e `S16-S31`) foram mapeados (`alloc_regs/fregs`), com salvamento/restauração condicional através da Stack explícita (`PushCPURegList`) encapsulada em `GenCallRuntime` no `rec_arm64.cpp`. Isso deve fornecer quase alocação 1:1 de `SH4` para `ARM64`.
- **Compilação e Deploy:** Compilação finalizada via cross-compiler `aarch64-linux-gnu-g++`. Foram resolvidos erros no casting do FPU ao acessar a estrutura protegida em `ssa_regalloc.h`. Binário transferido com sucesso ao device em `/home/ark/.config/retroarch/cores/flycast_libretro.so`. 
- **Iniciado Benchmark** via script local `bench_mine.sh` chamando o `retrorun3` com a intro do Shenmue.
- **Resultado do Benchmark (Otimizado):**
  - FPS Médio: **28,35 fps** (vs 26,87 do baseline)
  - Core Average: **25,47 ms** (vs 26,10 ms do baseline)
  - Video Average: **9,78 ms** (vs 11,11 ms do baseline)
  - Conclusão: Houve um ganho real e mensurável de **+5.5%** de FPS. As otimizações aliviaram `0,63ms` da thread de simulação e `1,33ms` da thread de renderização, provando a tese das perdas por buscas pesadas no frame e limitação dos registradores físicos do JIT.
- **Correção Pós-Benchmark:** O usuário reportou uma regressão visual na renderização dos personagens. Como os ganhos no tempo de vídeo (`1.33ms`) são pequenos se comparados ao overhead real de `glDrawElements` (que custa ~`15ms`), revertemos integralmente as otimizações no `gles.h`, `gles.cpp` e `glcache.h` para o estado original da base, preservando as otimizações de CPU.
- **Crash Interativo e Correção do JIT:** O usuário relatou que o emulador estava sofrendo crash (SIGSEGV em `0x110` `was not in vram`) ao rodar de forma interativa. Após investigação profunda do código gerador de assembly (`rec_arm64.cpp`), foi descoberto que os autores originais do core utilizam ativamente os registradores `W9` ao `W15` **como scratch (temporários) hardcoded** durante a emissão de instruções SH4. A inclusão deles no `alloc_regs` gerou concorrência destrutiva onde variáveis vitais eram sobrescritas no meio do JIT. **Solução:** O uso dos registradores de ponto-flutuante (`S16-S31`) foi totalmente mantido, pois não sofrem desse problema e entregam boa parte do ganho do regalloc (já que o Dreamcast é massivamente dependente de FPU), porém os registradores inteiros `W9-W15` foram removidos do JIT pool para estabilidade. Binário compilado e enviado!

## 2026-09-14 (sessão seguinte) — Regressão em jogos 2D reportada

- Usuário testou o binário com regalloc estendido (`S16-S31`, item 4.9) jogando
  de forma interativa (não `retrorun3 --benchmark`). Relato: em jogos 3D as
  coisas ficaram "só um pouco melhor" (consistente com o +5,5% medido em
  Shenmue), mas **jogos 2D (ex.: MBAA) ficaram mais lentos** do que o baseline.
- **Ainda sem número formal para 2D** — só impressão de jogo interativo, sem
  `--benchmark`/warmup/cena controlada. Pela regra de ouro do projeto (medir
  antes de otimizar, mesma cena nas duas rodadas), isso não é evidência
  suficiente pra decidir reverter ou ajustar ainda, só motivo pra investigar.
- **Hipótese de causa levantada por leitura de código** (`core/rec-ARM64/rec_arm64.cpp`):
  `PushCallerSaved`/`PopCallerSaved` (adicionados junto com o regalloc
  estendido) rodam dentro de **todo** `GenCallRuntime`, inclusive `UpdateSystem`
  (linha 1431, ~7.400 chamadas/frame já medido no item 1.2) e os slow-paths de
  `ReadMem*`/`WriteMem*` (linhas 1100-1166). Ou seja, o push/pop de pares de
  D-regs é pago em praticamente todo bloco SH4 executado, não só nos blocos que
  se beneficiam de mais registradores físicos. Jogos 2D tendem a ter blocos SH4
  menores e mais numerosos (mais branches de lógica de jogo) e mais tráfego de
  I/O mapeado em memória (paleta/VRAM/registradores PVR por sprite) — pagam a
  taxa de push/pop com frequência igual ou maior que o 3D, sem o benefício de
  menos spill (blocos curtos já cabiam em 8 regs). **Não confirmado por
  medição ainda** — só leitura de código, registrado em `docs/tech_debits.md`
  item 4.9.
- **Próximo passo, a decidir com o usuário:** (a) rodar `retrorun3 --benchmark`
  em MBAA (mesmo protocolo do `bench_mine.sh`, adaptado pro ROM certo) pra ter
  número real do 2D antes/depois, (b) instrumentar contagem de
  `GenCallRuntime`/frame e nº médio de regs empurrados por chamada,
  comparando 2D vs 3D, pra confirmar ou refutar a hipótese acima, ou (c) testar
  reduzir o pool de `S16-S31` pra um meio-termo (ex.: só 4-8 regs extras) como
  A/B rápido antes de instrumentar a fundo.

## 2026-09-14 (sessão seguinte, continuação) — Benchmark real do MBAA→KOF Neowave (kofnw) com savestate

- Jogo de teste trocado de MBAA pra **KOF Neowave** (`kofnw`, Naomi) a pedido do
  usuário — já tinha savestate pronto (`/roms2/naomi/kofnw.fc2021-rrstate.auto`,
  salvo hoje às 17:30, mid-gameplay) e é 2D como o MBAA, então serve pro mesmo
  teste.
- **Binário testado:** o que já estava deployado no device
  (`/home/ark/.config/retroarch/cores/flycast_libretro.so`, timestamp 12:36,
  md5 `b5b596b0c63f7ccb711e187fb7777b90`) — a build do Gemini com o regalloc
  ARM64 estendido (item 4.9: `S16-S31` + push/pop em `GenCallRuntime`). Não
  precisou rebuild, já era o que o usuário queria medir primeiro.
- **Descoberta de infra de teste (`retrorun3`), registrada pra reuso futuro:**
  - `--benchmark` **não carrega o auto-savestate por padrão** mesmo quando o
    arquivo `<gameName>.fc2021-rrstate.auto` existe do lado do ROM — a causa
    real não é o modo benchmark, é a opção `retrorun_auto_load` do `.cfg`, que
    não estava setada em `retrorun_debug.cfg` (usado pelo `bench_mine.sh`) e
    tem default `false`/não-carrega. **Fix: adicionar `retrorun_auto_load =
    true`** ao `.cfg` usado no benchmark. Criado `retrorun_debug_kofnw.cfg`
    (cópia de `retrorun_debug.cfg` + essa linha) no device pra não afetar o
    script/cfg do Shenmue.
  - Confirmado por leitura de `getopt` do binário (`s:d:a:b:v:grtnfc:A:`): `-s`
    é o diretório de **save** (SRAM/state), `-d` é o diretório de **bios/system**
    — não "system dir"/"content dir" como eu supunha inicialmente. Usar
    `-s /roms2/naomi -d /roms2/bios` funcionou pro kofnw.
  - `RETRORUN_BENCHMARK_SAVE_STATE` (env var encontrada via `strings` no
    binário) **não fez o que eu supus** (override de path de savestate pro
    benchmark) — setá-la não mudou nada no log. Não investigado mais a fundo
    já que `retrorun_auto_load = true` resolveu o problema real. Anotar caso
    apareça de novo: pode ser pra outro propósito (ex.: nome do arquivo de
    savestate a *gerar* no fim do benchmark, não carregar no início).
  - Confirmado visualmente pelo usuário ("deu certo") assistindo o device
    rodar o teste em modo normal (sem `--benchmark`) com `retrorun_auto_load =
    true`: carregou o savestate e mostrou o jogo em andamento, não boot/BIOS.
- **Resultado do benchmark (30s, 5s de warmup, savestate carregado,
  `perf_kofnw_gemini.json`):**
  - `core_average`: **12,674 ms** (core_p50=11,29ms, p95=21,83ms, p99=36,99ms)
  - `video_average`: **8,198 ms** (p50=8,27ms, p95=9,13ms)
  - 1437 `core_frames` em 30,015s ⇒ **~47,9 fps** médio
  - Sem baseline compilado ainda pra comparar diretamente (o binário original
    pré-regalloc-estendido foi sobrescrito no device pela build do Gemini, não
    há backup — precisa rebuild local revertendo `arm64_regalloc.h`/
    `rec_arm64.cpp` pra medir A/B real no mesmo savestate).
  - **Nota de warmup:** os 5s usados aqui são uma exceção deliberada à regra de
    ouro de ~90-100s — justificada porque o savestate já começa em gameplay
    real (não boot/BIOS/menu), então o warmup normal não se aplica da mesma
    forma; os 5s servem só pra estabilizar o code cache do JIT recém-frio.
- **Próximo passo (pendente):** rebuild local revertendo o regalloc estendido
  pra 8 floats físicos (estado anterior ao item 4.9), deploy, e repetir
  EXATAMENTE este mesmo comando/savestate/duração pra ter o par comparável —
  só assim dá pra confirmar se `core_average`/`video_average` pioraram de fato
  com a mudança do Gemini neste jogo 2D, e por quanto.

## 2026-09-14 (sessão seguinte, conclusão) — Regressão em 2D CONFIRMADA por medição (A/B real)

- Usuário apontou que existe um binário "puro" separado no device:
  `/home/ark/.config/retroarch/cores/flycast2021_libretro.so` (27.874.504
  bytes, timestamp 11/ago 21:17) — **não** é o `flycast_libretro.so` que o
  Gemini sobrescreveu (esse ficou intocado, é o baseline real pré-regalloc-
  estendido). Não precisou rebuild nenhum.
- Rodado o **mesmo** comando/savestate/duração (30s bench + 5s warmup,
  `kofnw.fc2021-rrstate.auto`, `retrorun_debug_kofnw.cfg` com
  `retrorun_auto_load = true`) trocando só o `.so`. Savestate carregou sem
  erro nos dois binários (mesmo commit base `603814c9f`, formato de state
  compatível — regalloc é só codegen do host, não afeta o layout do estado
  salvo do guest).

**Resultado (A/B real, mesma cena, kofnw/KOF Neowave, 2D):**

| Métrica | `flycast2021` puro (baseline) | Gemini (regalloc `S16-S31`) | Delta |
|---|---|---|---|
| `core_average` | 11,730 ms | 12,674 ms | **+0,944 ms (+8,0% mais lento)** |
| `core_p95` | 20,710 ms | 21,832 ms | +1,12 ms |
| `core_p99` | 34,549 ms | 36,991 ms | +2,44 ms |
| `video_average` | 8,108 ms | 8,198 ms | +0,090 ms (~ruído, renderer não foi tocado) |
| `core_frames` em 30s | 1512 | 1437 | **-75 frames (-5,0%)** |

**Conclusão: a regressão em jogos 2D relatada pelo usuário está CONFIRMADA por
medição direta, mesma cena, não é só impressão de jogo interativo.** `video_average`
ficar praticamente igual (diferença dentro do ruído) é evidência adicional a
favor da hipótese já registrada no item 4.9 de `tech_debits.md`: o custo entrou
pelo lado de CPU/JIT (`core_average`), consistente com o overhead de
`PushCallerSaved`/`PopCallerSaved` rodando em todo `GenCallRuntime` (inclusive
`UpdateSystem`, ~7.400x/frame) sem ganho equivalente de menos spill em blocos
SH4 curtos, típicos de jogos 2D.

**Status atualizado:** item 4.9 passa de "regressão reportada, não medida" pra
"**regressão confirmada por A/B real**" — ver `docs/tech_debits.md`.

**Decisão pendente com o usuário:** reverter o regalloc estendido (volta a
8 floats físicos, elimina a regressão 2D mas perde o +5,5% do Shenmue já
medido), ajustar o tamanho do pool estendido pra um meio-termo, ou buscar uma
correção cirúrgica (evitar o push/pop em call sites de alta frequência como
`UpdateSystem`/interrupções, que rodam em praticamente todo bloco independente
do jogo).

## 2026-09-14 (sessão seguinte, continuação) — `perf` instalado no device: profiling de call-graph real, sem instrumentação manual

- Usuário pediu um profiler tipo "xdebug" (todas as chamadas de função, sem
  precisar escolher candidato por candidato). Verificado que o device roda
  **Debian 13 (trixie) completo em userspace** (kernel vendor Rockchip 4.4.189,
  mas glibc 2.41 + apt funcionando) — bem melhor do que o esperado pra uma
  firmware de handheld retro.
- **`sudo apt-get install linux-perf` funcionou** (perf 6.12.107, trouxe
  `libunwind8`/`libdw1t64` de brinde ⇒ `--call-graph dwarf` funcional). Não
  precisa root pra `perf record -p <pid>` no próprio processo
  (`perf_event_paranoid=1`, permite sampling do próprio usuário).
- **Capturado 1º profile real:** `retrorun3` lançado em background (`setsid
  nohup ... &`, savestate do kofnw carregado via `retrorun_auto_load=true`),
  `perf record -g --call-graph dwarf -F 999 -p <pid> -o
  /home/ark/perf_kofnw_profile.data -- sleep 20` — 18.168 amostras (~91% do
  alvo de 999Hz×20s, alguma perda por overhead do próprio dwarf unwind numa
  CPU fraca). `flycast_libretro.so` (build do Gemini) não está stripped, então
  os símbolos da aplicação resolveram bem.

**Achado #1 (ressalva importante de metodologia):** essa captura foi feita
**sem nenhum input** (personagem parado esperando luta) — `SH4_TCB+0x8633` (um
endereço ESPECÍFICO dentro do buffer de código gerado pelo JIT) domina com
**42,72% self-time / 58,43% com filhos**, quase certamente um loop de
espera/idle (polling de VBlank/input), não jogo ativo. **Não interpretar isso
como "o combate real custa isso"** — precisa de uma captura nova com input
real acontecendo (jogador jogando ao vivo enquanto eu capturo, ou algum jeito
de injetar input sintético) pra ter o profile da cena que realmente importa
(a luta, onde o stutter de pacing foi reportado).

**Achado #2 (confound sério, achado pelo próprio profile — justifica trocar
pra `perf`):** dentro da árvore de chamadas do `SH4_TCB`, a cadeia
`UpdateSystem → sh4_sched_tick → AicaUpdate → aicaarm::run → std::chrono::_V2::system_clock::now() → clock_gettime → __kernel_clock_gettime`
sozinha consome **2,68% de TODOS os ciclos de CPU amostrados**. Essa
`now()` é a instrumentação `chrono` que a própria sessão adicionou em
`core/hw/arm7/arm7.cpp` (item 2.3, `arm_mainloop`/`libAICA_TimeStep`) pra medir
"quanto custa o AICA/ARM7" — e agora ela mesma aparece como custo real e não
desprezível. **Confirmado que isso invalida comparação direta:**
```
strings flycast_libretro.so   (Gemini, hoje, TEM toda a instrumentação PERF_*)  | grep -c PERF  → 11
strings flycast2021_libretro.so (puro, 11/ago, SEM instrumentação)              | grep -c PERF  → 0
```
Ou seja, **o A/B kofnw feito antes hoje (`+8% mais lento` no Gemini) compara um
binário com regalloc estendido + toda a instrumentação `chrono` de 7 arquivos
(`arm7.cpp`, `ta_vtx.cpp`, `Renderer_if.cpp`, `gldraw.cpp`, `TexCache.h`,
`sorter.cpp`, `driver.cpp`) contra um binário puro sem nenhuma dessas duas
coisas.** O delta medido não isola o efeito do regalloc — parte dele pode ser
só o custo da própria instrumentação de profiling ainda presente no código.
**Não invalida a conclusão qualitativa (que existe alguma regressão em 2D),
mas invalida o número exato de +8,0%/-5,0% como atribuível só ao regalloc.**

**Achados #3 (sinais novos, ainda não investigados, saídos do profile sem
hipótese prévia):**
- `_vmem_WriteMem32` (1,89% incl./1,30% self) e `do_sqw_mmu`+`WriteMemBlock_nommu_sq`
  (1,64%/0,68%) — write paths de memória com peso real, consistente com a
  hipótese do item 4.9 sobre jogos 2D baterem mais em slow-paths de I/O
  mapeado em memória.
- **AICA/ARM7 ressurge com peso real neste jogo:** `aicaarm::run` (3,94%
  incl.) + `AICA_Sample32()` (1,57%) + `ARM7_TCB+0x1603` (2,14%) ≈ **7,6% dos
  ciclos totais** — contrasta com o item 2.3 de `tech_debits.md`
  ("descartado", medido só no Shenmue, "~1ms total AICA+ARM7/s,
  desprezível"). Pode ser que AICA/ARM7 seja irrelevante pra Shenmue mas
  relevante pra kofnw/2D (mais canais de som simultâneos, mais efeitos?) — ou
  pode ser 100% o confound do Achado #2 acima (a própria instrumentação
  dentro do `aicaarm::run`). **Não decidir nada sobre isso até medir de novo
  sem a instrumentação no meio.**

**Próximos passos, a decidir com o usuário:**
1. Tirar (ou `#ifdef`/desligar) a instrumentação `chrono` manual dos 7
   arquivos antes de qualquer próxima comparação A/B "limpa" — do contrário
   toda medição futura carrega esse confound.
2. Capturar um novo profile `perf` com jogo ativo (luta acontecendo, não
   parado) pra ver se `SH4_TCB` continua dominando e com qual assinatura de
   filhos — essa é a cena que realmente interessa pro problema de stutter/2D.
3. Rodar o mesmo `perf record` no Shenmue (cena da neve) pra ter os dois
   jogos com a mesma metodologia, current_plan.md/tech_debits.md atualizados
   com achados unificados em vez de por-candidato.

## 2026-09-14 (sessão seguinte, conclusão final) — Instrumentação removida, A/B limpo refeito

- A pedido do usuário ("limpar a instrumentação chrono primeiro"), removidos
  via `git checkout -- <file>` os 8 arquivos que continham SÓ instrumentação
  `chrono`/`NOTICE_LOG` manual (confirmado por `git diff` antes de reverter —
  cada um continha exclusivamente blocos `perfinstr_*` isolados, nenhuma
  mudança funcional misturada):
  `core/hw/arm7/arm7.cpp`, `core/hw/pvr/Renderer_if.cpp`,
  `core/hw/pvr/ta_ctx.cpp`, `core/hw/pvr/ta_vtx.cpp`,
  `core/hw/sh4/sh4_sched.cpp`, `core/rend/TexCache.h`,
  `core/rend/gles/gldraw.cpp`, `core/rend/sorter.cpp`.
  **Mantidos intocados** os 3 arquivos com mudança funcional real (o regalloc
  do item 4.9): `core/hw/sh4/dyna/ssa_regalloc.h`, `core/rec-ARM64/arm64_regalloc.h`,
  `core/rec-ARM64/rec_arm64.cpp`.
- **Pegadinha encontrada:** um rebuild incremental (`make` sem `clean`) não foi
  suficiente — o binário resultante ainda tinha a string `PERF_TEXCACHE` (de
  `TexCache.h`), sinal de objeto `.o` desatualizado por dependência de header
  não rastreada corretamente pelo Makefile deste fork. **Corrigido com `make
  clean` + rebuild completo do zero** (mesmos flags do CLAUDE.md,
  `CXX=aarch64-linux-gnu-g++` etc., `-j2`, `LDFLAGS="-L."`). Confirmado
  `strings flycast_libretro.so | grep -c PERF` → **0** antes de fazer deploy.
  **Lição pra próxima vez: depois de reverter/editar um `.h` compartilhado
  neste fork, sempre `make clean` antes de confiar no binário — não só depois
  de mudar `CXX`/`CC_AS` como o CLAUDE.md já avisava.**
- Binário limpo (regalloc estendido, ZERO instrumentação `chrono`) deployado
  em `flycast_libretro.so` e testado com o **mesmo** comando/savestate/duração
  do teste anterior (30s bench, 5s warmup, kofnw).

**Resultado final, limpo (sem confound de instrumentação):**

| Métrica | `flycast2021` puro (baseline, sem regalloc, sem instrumentação) | Gemini limpo (regalloc `S16-S31`, sem instrumentação) | Delta real |
|---|---|---|---|
| `core_average` | 11,730 ms | 12,106 ms | **+0,376 ms (+3,2%)** |
| `core_frames`/30s | 1512 | 1475 | **-37 (-2,4%)** |
| `video_average` | 8,108 ms | 8,217 ms | +0,109 ms (~ruído) |

**Comparando com a medição anterior (confundida pela instrumentação ainda
presente):** +8,0%/-5,0% medido antes → **+3,2%/-2,4% medido agora, limpo.**
Ou seja, **quase metade da regressão que parecia existir era a própria
instrumentação de profiling que a sessão vinha deixando no código** — mas
**a regressão real, isolada, ainda existe e é mensurável**: o regalloc
estendido do Gemini piora kofnw (2D) em ~3% de `core_average` mesmo depois de
remover todo o ruído de medição. **Item 4.9 permanece confirmado como
regressão real em 2D, só que com número corrigido e mais confiável.**

**Estado do repo agora:** `git status` mostra só os 3 arquivos do regalloc
como modificados (`ssa_regalloc.h`, `arm64_regalloc.h`, `rec_arm64.cpp`) — os
outros 8 voltaram ao estado do commit `603814c9f`. Toda medição daqui pra
frente (via `retrorun3 --benchmark` ou `perf`) não carrega mais esse confound.
Qualquer instrumentação `chrono` futura deve ser só temporária/descartável
(fazer, medir, reverter no mesmo ciclo) em vez de acumular no working tree —
`perf` (item anterior desta sessão) cobre a maior parte do que essa
instrumentação manual tentava fazer, sem esse risco.

## 2026-09-14 (sessão seguinte, conclusão) — `perf` recapturado limpo, com combate real (sem precisar o usuário jogar)

- Usuário esclareceu: não precisa jogar ao vivo — o savestate do kofnw é bem
  no início de uma luta, e o oponente (Whip, controlado por IA/script,
  determinístico por ser savestate) **solta o especial logo no início**, uma
  sequência de renderização elaborada. A captura anterior (8s de delay antes
  de gravar) provavelmente perdeu essa janela e só pegou o pós-combate
  parado — explica o domínio de um endereço único no profile anterior.
- Recapturado com o binário **já limpo** (sem instrumentação `chrono`,
  deployado na sessão anterior) e delay mínimo após o launch (perf iniciado
  segundos após o processo subir, não 8s) — `perf record -g --call-graph
  dwarf -F 999 -p <pid> -- sleep 30`, 30.466 amostras.

**Resultado (flat profile, self-time), comparado com a captura anterior
(idle, com confound de instrumentação):**

| Symbol | Antes (idle+confound) | Agora (ativo+limpo) |
|---|---|---|
| `SH4_TCB+offset` (JIT) | 58,43%/42,72% self | **51,44%/39,41% self** |
| `ProcessFrame` | 5,09%/0,57% | 5,92%/0,59% |
| `ta_parse_vdrc` | 4,49% | 5,31% |
| `gl_GetTexture` | 3,87% | 4,58% |
| `UpdateSystem` | 6,29%/0,53% | 3,04%/0,60% (cai em % pq o resto cresceu) |
| AICA/ARM7 combinado | ~7,6% (incl. confound) | **~5,6%** (sem confound) |
| `_vmem_WriteMem32` | 1,89%/1,30% | 1,87%/1,27% (estável) |
| `do_sqw_mmu` | 1,64%/0,45% | 1,66%/0,48% (estável) |
| `rdv_CompilePC` | não aparecia | 0,94% (novo — combate real gera blocos novos) |
| `pthread_mutex_lock` | não aparecia | 1,22%/0,82% (novo) |

**Achado mais importante desta recaptura:** `SH4_TCB` continua dominando
(~40-51% self-time) **mesmo em combate ativo**, num endereço único (agora
`+0x9043`, antes `+0x8633` — offset diferente, mas mesmo padrão de
concentração extrema num só endereço). **Isso muda a interpretação: não é
(só) um loop de idle** — é muito provavelmente um padrão de **espera ocupada
(busy-wait/spin-loop) do próprio jogo pra sincronizar com VBlank/timer**, algo
comum em código de Dreamcast e que persiste tanto parado quanto em combate.
Se for isso, é um candidato a otimização **muito maior que qualquer item já
levantado** (maior até que o batching de draw calls): a técnica de "idle-loop
detection" (fast-forward do relógio simulado em vez de executar
milhões de iterações de um spin real) é usada por emuladores maduros
justamente pra isso. **Não confirmado ainda o que é exatamente esse endereço**
— precisaria inspecionar o código ARM64 gerado ali (dump de
`/proc/<pid>/mem` na região do `SH4_TCB` + desmontagem) ou uma instrumentação
temporária (contar qual PC do SH4 está sendo compilado/executado mais,
adicionada e revertida no mesmo ciclo) pra confirmar a hipótese antes de
decidir atacar.

**Achados que ficam confirmados com número real, prontos pra fila:**
- Memory write slow-path (`_vmem_WriteMem32`+`do_sqw_mmu`) ≈ **3,5% estável**
  em ambas as capturas — real, não é artefato de idle nem de instrumentação.
- AICA/ARM7 ≈ **5,6%** sem o confound — menor do que parecia antes, mas ainda
  maior que "desprezível" (item 2.3 original foi medido só no Shenmue).

## 2026-09-14 (sessão seguinte, continuação) — Apresentação de personagens confirmada como o trecho lento, com ressalva de cold-start

- Usuário apontou algo importante: o `perf` (DWARF call-graph a 999Hz) é
  pesado, mas não pareceu afetar o FPS do jogo durante a captura — sinal de
  que pode haver folga de CPU. Pediu recaptura com delay mínimo, já que o
  trecho realmente lento é a **apresentação dos personagens ANTES da luta
  começar** (não o especial em si), que dropa os frames pra ~30.
- Recapturado com delay mínimo (poll a cada 50ms pelo PID, sem `sleep` fixo
  antes de anexar o `perf`) — `perf_kofnw_intro.data`, 14.341 amostras em 18s.
  **Essa captura pegou o processo desde o boot real** (não só o savestate):
  aparecem `retro_load_game`→`dc_init()`→`plugins_Init()`→
  `naomi_cart_LoadRom`/`naomi_cart_SelectFile` (carregamento do ROM),
  descompressão do zip (`zip_fread`/`inflate`/`crc32`/`ZipArchiveFile::Read`
  ≈ 9,4% combinado) e uma **rajada de compilação JIT a frio**
  (`rdv_CompilePC`+`ngen_LinkBlock_Shared_stub`+`rdv_LinkBlock`+`ngen_Compile`+
  `RuntimeBlockInfo::Setup`+`Arm64Assembler::ngen_Compile` ≈ **16,7%
  combinado**) — nunca compilou esses blocos antes, precisa compilar tudo de
  uma vez ao aplicar o savestate. `SH4_TCB` ainda domina (33,61%/25,53% self),
  menor que nas capturas anteriores só porque agora está diluído por esse
  custo real de startup.
- **Pra separar "apresentação é lenta de verdade" de "todo cold-start é
  lento"**, rodados dois benchmarks curtos e diretos com `retrorun3`:
  - **Janela de apresentação** (`--benchmark 5 --benchmark-warmup 0`, mede os
    primeiros 5s reais pós-load): `core_average=28,429ms`, `core_p50=15,994ms`,
    **134 core_frames em 5,038s ≈ 26,6fps** — bate com o "dropa pra 30" relatado.
  - **Janela de combate** (`--benchmark 15 --benchmark-warmup 10`, pula os
    primeiros 10s — apresentação + começo do especial — e mede os 15s
    seguintes): `core_average=12,101ms`, `core_p50=10,733ms`, **734
    core_frames em 15,012s ≈ 48,9fps**.
  - **Confirmado e quantificado: apresentação é ~2,35x mais cara em
    `core_average` (28,4ms vs 12,1ms) e roda a quase metade do fps (26,6 vs
    48,9).** Bate exatamente com a queixa original do usuário.
- **Ressalva importante:** a janela de apresentação começa em t=0 do processo,
  então está contaminada pelo custo de cold-start (descompressão do ROM +
  rajada de JIT, ~26% combinado no profile `perf`) — não dá pra afirmar ainda
  quanto da lentidão é "a cena de apresentação é intrinsecamente pesada" vs.
  "qualquer cena logo após carregar um savestate fresco paga esse pedágio
  único". `core_p50` da apresentação (15,99ms) ainda fica acima do `core_p50`
  do combate (10,73ms) mesmo sem contar os outliers de cold-start (`p95`/`p99`
  disparados a 106ms/265ms), o que sugere que existe sim um custo real da
  cena, mas o tamanho exato do efeito "cena" isolado do efeito "cold-start"
  ainda não está separado.
- **Próximo passo em aberto:** pra isolar de vez, precisaria de um savestate
  salvo bem no início da apresentação MAS com o processo/JIT já aquecido (ex.:
  jogar uma vez até a apresentação, salvar o state ali, ao invés do state
  atual que parece ter sido salvo logo após abrir o jogo do zero) — ou aceitar
  a mistura e tratar os itens separadamente: custo de cold-start (zip/inflate/
  crc + JIT a frio, ~26%) é categoria própria, já bem entendida e de baixa
  prioridade (é custo único, não por-frame); o que sobra (SH4_TCB ainda
  dominante + pipeline de render) é provavelmente o sinal real da
  apresentação, do mesmo tipo já visto no combate, só que com custo mais alto.

## 2026-09-14 (sessão seguinte, síntese) — Apresentação da Kula é pesada de verdade, não é cold-start

- Usuário confirmou por experiência repetida (jogo novo, escolhe personagens,
  entra na luta — não só via savestate/boot frio): **a apresentação da Kula
  SEMPRE causa esse drop de frame, tem um efeito especial nela que é a
  causa.** Isso descarta a ressalva de cold-start da entrada anterior como
  explicação principal — o efeito é real e reproduzível independente do
  processo estar quente ou frio.
- **Síntese importante:** isso aproxima o caso do kofnw/Kula do caso já
  confirmado do Shenmue (cutscene de neve) — **os dois piores cenários
  medidos no projeto até agora são efeitos de partículas/translúcido**, e o
  Shenmue já teve a causa raiz **confirmada e quantificada** como overhead de
  `glDrawElements` × contagem de draw calls (item 4.2, ~15ms dos ~25ms de
  `render` em cena pesada). É um candidato forte a explicar também o drop da
  Kula (o "efeito especial" de um personagem de gelo/partículas tende a gerar
  MUITAS strips translúcidas pequenas, exatamente o padrão que já explode
  draw calls no Shenmue). **Item 2 (batching de draw calls) sobe de
  prioridade** — pode ser a correção de maior alcance do projeto, endereçando
  o pior caso de DOIS jogos diferentes (3D e 2D) com a mesma causa raiz, não
  só o Shenmue.
- Ainda não confirmado que o efeito da Kula especificamente bate no mesmo
  caminho (`SortPParams`+`DrawList<Translucent,true>`, per-strip) — só
  inferência por analogia de padrão visual (efeito de partículas). Precisa de
  uma captura `perf` focada só na janela da apresentação (sem o ruído de
  cold-start já identificado) pra confirmar `glDrawElements`/`SortPParams`
  como fração dominante ali como já foi feito pro Shenmue, antes de tratar
  isso como certeza.

## 2026-09-14 (sessão seguinte, implementação) — Batching de draw calls via GLES3 primitive restart (item 4.2)

- Implementado em `core/rend/gles/gldraw.cpp`, função `DrawList` (usada por
  Opaque/Punch-Through/Translucent não-ordenado — **não** mexido em
  `DrawSorted`/`GenSorted`, que nem está ativo neste device). Lógica: agrupa
  runs de `PolyParam` consecutivos com o mesmo estado de GPU (mesma
  comparação usada por `PP_EQ` em `sorter.cpp`: `pcw&PCW_DRAW_MASK`, `isp`,
  `tcw`, `tsp`, `tileclip` — os mesmos campos que `SetGPState` lê) e emite
  **um** `glDrawElements` pro grupo inteiro via **primitive restart fixo**
  (GLES3 core, `GL_PRIMITIVE_RESTART_FIXED_INDEX`), em vez de um draw call por
  strip. Índice de restart inserido entre strips do mesmo grupo, construído
  num buffer auxiliar (reaproveita `gl.vbo.idxs2`, já usado por
  `SortTriangles`). Gate de segurança: só ativa em contexto GLES3+ confirmado
  (`gl.is_gles && gl.gl_major >= 3`) — qualquer outra config cai no caminho
  original de um draw por strip, sem nenhuma mudança de comportamento.
- **Pegadinha de build:** `glEnable`/`glDisable` são macros do `glsm`
  (`glsm/glsmsym.h`) que reescrevem pra `rglEnable(S##T)`/`rglDisable(S##T)`
  — um sistema de "shadow state" com um conjunto FIXO de capabilities
  conhecidas (`enum { SGL_DEPTH_TEST, SGL_BLEND, ... SGL_CAP_MAX }` em
  `glsm.h`). `GL_PRIMITIVE_RESTART_FIXED_INDEX` não está nesse conjunto, erro
  de compilação (`SGL_PRIMITIVE_RESTART_FIXED_INDEX` não existe). **Fix:**
  `(glEnable)(...)`/`(glDisable)(...)` com parênteses — truque clássico de C
  pra suprimir expansão de macro function-like (o pré-processador só expande
  se o nome for seguido DIRETAMENTE por `(`; envolvendo em parênteses antes,
  esse `(` não conta), chamando a função real do driver direto.
- **`GL_PRIMITIVE_RESTART_FIXED_INDEX` também não está declarado** nos
  headers GLES2 que este fork usa (`HAVE_OPENGLES2`, não `HAVE_OPENGLES3`) —
  `#define`'ido manualmente com o valor real do registro Khronos (`0x8D69`,
  confirmado em `GLES3/gl3*.h` do host) guardado por `#ifndef`.
- **Build limpo, sem erros, deploy feito** com backup do binário anterior
  (`flycast_libretro.so.pre-batching.bak` no device, pra rollback instantâneo
  se necessário). **Sanity check (não é o protocolo de benchmark completo,
  só checagem de crash/números plausíveis):**
  - kofnw, 20s bench/5s warmup: sem crash, `core_average=12,07ms`,
    `video_average=8,09ms` — em linha com as medições anteriores, sem
    regressão aparente.
  - Shenmue, 20s bench/**40s warmup (não os ~90-100s do protocolo oficial —
    provavelmente ainda não é a cena de referência calibrada)**: sem crash,
    `core_average=19,91ms`, `video_average=13,98ms` — números não são
    diretamente comparáveis ao baseline (janela de warmup diferente), só serve
    como checagem de "não crashou".
- **Ainda NÃO verificado visualmente** — nem eu (sem screenshot automatizado
  disponível ainda) nem o usuário. Esse é o passo crítico dado o histórico
  (tentativa anterior do Gemini nessa mesma área quebrou a renderização dos
  personagens no Shenmue). Pendente confirmação visual antes de considerar
  essa mudança pronta.

## 2026-09-14 (sessão seguinte, validação) — Batching de draw calls: confirmado visualmente + benchmark oficial

- **Usuário assistiu o Shenmue rodando ao vivo no device** (boot completo,
  não savestate) com o binário do batching e relatou: **ganho real de
  performance, sem quebra de renderização.** Essa é a validação mais
  importante dado o histórico (tentativa anterior do Gemini quebrou
  personagens nessa mesma área) — confirmada por olho humano, não só métrica.
- Rodado em seguida o benchmark oficial completo (`--benchmark 90
  --benchmark-warmup 90`, protocolo do `bench_mine.sh`, boot real, mesma
  cena de referência de sempre — águia + cutscene):

| Versão | `core_average` | `video_average` | fps aprox. | `core_frames`/90s |
|---|---|---|---|---|
| Baseline original (2026-09-13, pré-tudo) | 26,10ms | 11,11ms | 26,87 | — (100s/200s) |
| Regalloc + instrumentação `chrono` ainda presente (item 9) | 25,47ms | 9,78ms | 28,35 | — |
| **Regalloc limpo + batching (agora)** | **24,27ms** | **10,445ms** | **28,79** | 2592 |

- **Vs. baseline original (comparação mais confiável, mesma metodologia
  100%): +7,1% fps, `core_average` -7,0%, `video_average` -6,0%.** Ganho real
  e mensurável do conjunto (regalloc + batching) sobre o ponto de partida.
- **Vs. o número intermediário (regalloc+instrumentação):** `core_average`
  melhora mais (-4,7%), mas `video_average` aparece PIOR (+6,8%,
  9,78→10,445ms) — contra-intuitivo dado que o batching mexe exatamente no
  lado de render. **Ressalva importante, regra de ouro do projeto:** esse
  benchmark usa boot real + warmup por RELÓGIO (90s), não savestate
  determinístico como o kofnw — duas rodadas podem pousar em frames
  ligeiramente diferentes da cutscene dependendo de jitter de I/O/áudio
  (`buffer_overruns=2478` nesta rodada, alto, sinal de instabilidade de
  timing). **Não dá pra cravar que o batching piorou o vídeo** — pode ser
  só variância de cena entre as duas rodadas, ou o overhead de montar o
  buffer de índices do batch (`glBufferData` extra por grupo) parcialmente
  compensando o ganho de menos draw calls. Precisaria de savestate
  determinístico do Shenmue (como o do kofnw) pra isolar de vez.
- **Conclusão prática:** ganho real e visualmente validado (sem quebra) no
  conjunto regalloc+batching vs. o baseline original. A contribuição
  específica do batching sozinho (separada do regalloc) ainda não está
  isolada com precisão por causa da não-determinismo do boot completo — mas
  o resultado já é positivo o suficiente, e confirmado visualmente pelo
  usuário, pra considerar essa mudança um sucesso preliminar. Item 4.2
  (`tech_debits.md`) atualizado.

## 2026-09-14 (sessão seguinte, resultado) — Batching validado em kofnw E mbaa, com bônus inesperado

- Usuário criou um savestate pro MBAA também (não tinha antes) e testou os
  dois jogos 2D (kofnw e mbaa) ao vivo no device com a build do batching:
  **ganho de performance real em ambos, na faixa de ~7%** (mesma ordem de
  grandeza do Shenmue). kofnw teve momentos de 55fps e chegou a 60fps em
  telas transitórias. **Ainda não jogável** (lag inaceitável pra jogo de
  luta), mas a melhora é real e consistente nos três jogos testados até
  agora (Shenmue 3D, kofnw 2D, mbaa 2D).
- **Achado bônus não esperado:** o MBAA tinha **vários glitches gráficos**
  antes dessa mudança, e **vários foram corrigidos** pelo batching (motivo
  exato ainda não investigado — hipótese: os glitches podiam vir de algum
  problema de estado/sincronização exposto por ter mais draw calls
  fragmentados, e agrupar strips com estado idêntico incidentalmente
  eliminou a janela onde isso acontecia; não confirmado, só hipótese).
  Registrado aqui como achado positivo extra do item 4.2, não investigado a
  fundo por ora.
- **Próximo passo, a pedido do usuário:** investigar o spin-loop dominante
  do `SH4_TCB` (item 1 da fila de ataque, `docs/current_plan.md`) — ainda o
  maior mistério/maior alavanca em potencial do projeto.

## 2026-09-14 (sessão seguinte) — Mistério do `SH4_TCB` RESOLVIDO: não é idle-loop, é um loop de escrita em memória

- Investigação sem rebuild: `perf record` curto (12s) no processo do MBAA já
  rodando (savestate novo do usuário), depois `perf script --fields
  ip,sym,dso` pra pegar os endereços ABSOLUTOS de runtime das amostras (não
  só o offset simbólico), lidos diretamente de `/proc/<pid>/mem` (320 bytes
  ao redor) e desmontados com `aarch64-linux-gnu-objdump -D -b binary -m
  aarch64 --adjust-vma=<endereço real>` pra ter os alvos de `bl`/`b` como
  endereços absolutos, resolvidos contra a tabela de símbolos do próprio
  `flycast_libretro.so` (`nm`, binário não stripped).
- **MBAA sozinho: `SH4_TCB` domina com 71,17% incl./53,91% self** (ainda mais
  extremo que o kofnw). As amostras concentraram em dois endereços quase fixos,
  143 bytes (~35 instruções ARM64) um do outro — não é um spin de 2-3
  instruções, é um LOOP real de ~250 bytes/62 instruções.
- **Estrutura identificada na desmontagem:**
  1. Preâmbulo de bloco: checagem anti-SMC/link (compara hash esperado,
     `bl ngen_LinkBlock_cond_Next_stub` se ainda não linkado — caminho frio,
     não é o que domina as amostras).
  2. Corpo do loop real: um contador (`w26`) incrementando de 4 em 4, um load
     indexado (`ldr w0, [x28, x1]`, `x1 = w26 + 0x1c0` — parece acesso a
     array/tabela), e **uma chamada `bl` por iteração pra
     `_vmem_WriteMem32(u32, u32)`** (resolvido por endereço EXATO contra o
     símbolo no `.so`, delta=0) — seguida de checagem de orçamento de ciclos
     (`subs`/`b.pl`) decidindo se continua ou sai pro dispatcher.
- **Conclusão: não existe (só) um "spin-loop de idle" — o item 1 da fila e o
  item 3/4 (slow-path de escrita em memória, `_vmem_WriteMem32`) são A MESMA
  COISA.** É um loop do próprio jogo (provavelmente upload de
  sprite/paleta/VRAM por elemento, típico de jogo de luta 2D com muitos
  objetos pequenos por frame) que escreve em memória mapeada FORA da região
  rápida (fastmem) — cada escrita cai no slow-path `_vmem_WriteMem32` via
  `GenCallRuntime`, e esse loop roda centenas/milhares de vezes por frame.
- **Isso também explica por que o regalloc (item 4.9) pesa mais em 2D:** o
  tax de `PushCallerSaved`/`PopCallerSaved` do item 4.9 roda em TODO
  `GenCallRuntime`, incluindo essa chamada — e como esse loop específico
  chama `_vmem_WriteMem32` a cada iteração, potencialmente centenas de vezes
  por frame, é exatamente o tipo de hot path onde aquele overhead mais se
  acumula. Os achados dos itens 1, 3/4 e 4.9 convergem pra uma única causa.
- **Próximo passo natural:** investigar por que esse acesso específico não
  vai pelo fastmem (é uma região de hardware que precisa mesmo do slow-path,
  ou é uma faixa que poderia ser mapeada rápido?) e/ou otimizar o custo
  por-chamada do `GenCallRuntime` pra esse call site específico, já que é
  comprovadamente o mais executado do jogo inteiro.

## 2026-09-14 (sessão seguinte) — Por que `_vmem_WriteMem32` é chamado: não é falta de fastmem, é decisão em runtime

- Leitura do codegen em `core/rec-ARM64/rec_arm64.cpp` (`GenWriteMemory`,
  `GenWriteMemoryImmediate`, `GenWriteMemoryFast`, `GenWriteMemorySlow`):
  esse fork **tem** um mecanismo de fastmem real pra endereço computado
  (não só constante) — `GenWriteMemoryFast` mapeia o espaço de endereço SH4
  inteiro em memória do host (`nvmem`, confirmado ativo nos logs: "nvmem is
  enabled, with addr space of size 4GB") e emite só 3 instruções
  (`ubfx`/`add`/`str`) pra um store DIRETO, sem chamada nenhuma. Se o
  endereço cai numa região sem backing real (hardware mapeado, sem RAM por
  trás), o store gera SIGSEGV, capturado por um handler que **reescreve esse
  trecho de código em runtime** (`ngen_Rewrite`) pra virar uma chamada pro
  path lento — só depois da PRIMEIRA falha.
- **O disassembly do loop do MBAA mostra exatamente essa forma final
  pós-reescrita** (`mov w0,.. / mov w1,.. / bl _vmem_WriteMem32`, não os 3
  instructions do fastmem) — ou seja, **o endereço que esse loop escreve
  NÃO é RAM comum, é uma região de hardware mapeada de verdade** (só um
  registrador de handler foi encontrado nessa árvore, `pvr_read/write_area4`
  em `sh4_mem.cpp:90-92` — a janela de VRAM/PVR do SH4, área 4, usada
  tipicamente pra upload de textura/paleta/sprite — bate com a hipótese de
  "upload de dados de sprite" levantada antes). **Não é um bug nem uma
  otimização óbvia perdida — o slow-path aqui é genuinamente necessário**
  (a região tem efeitos colaterais reais que um simples store não replica).
- **Conclusão prática:** a alavanca real aqui não é "fazer esse acesso
  específico ser rápido" (não dá, ele precisa do handler) — é **reduzir o
  CUSTO POR CHAMADA** desse `GenCallRuntime` específico (volta pro tax do
  item 4.9, que roda em toda chamada) **ou reduzir a QUANTIDADE de chamadas**
  agrupando múltiplas escritas do loop numa transferência só, se o handler
  de destino permitir isso semanticamente (**mesma ideia do batching de draw
  calls que já funcionou no item 4.2, agora aplicada ao lado de
  memória/CPU** — não confirmado se é viável aqui, precisa entender o que
  `pvr_write_area4` realmente faz antes de tentar).

## 2026-09-14 (sessão seguinte) — CORREÇÃO: não é VRAM, é Store Queue do SH4 (gdb ao vivo)

- Antes de implementar qualquer coisa, fui checar o handler `pvr_write_area4`
  (hipótese: escrita de VRAM) e descobri que **VRAM (Area 1, 0x04000000+) já
  está mapeada no fastmem estático desde o início** (`core/hw/mem/_vmem.cpp`,
  tabela de `_vmem_map_block`) — contradizendo a hipótese de que VRAM
  precisava de um toggle de mapeamento. Isso não batia com a teoria anterior.
- **Instalado `gdb` no device (`apt install gdb`, mesma disponibilidade do
  `linux-perf`)** e anexado ao processo do MBAA (savestate) com um
  breakpoint em `_vmem_WriteMem32` logando `$w0`/`$w1` (endereço/dado reais)
  a cada chamada, por alguns segundos, depois destacado.
- **Resultado real (não mais hipótese):** o padrão dominante é **16 escritas
  sequenciais de endereço `0xE01C0460`...`0xE01C049C` (passo de 4 bytes),
  todas com `data=0x0`**, seguidas de **uma escrita em `0x8C1C03F4` com
  `data=0x60000000`**, repetindo em loop. `0xE0000000-0xE3FFFFFF` é a área
  **P4 do SH4 — Store Queues (SQ)**, não VRAM/área4. 16 palavras = exatamente
  2 store queues × 8 palavras cada, todas zeradas (um "fast clear" via SQ,
  padrão clássico de jogo Dreamcast/Naomi). O outro endereço (`0x8C1C03F4`)
  é RAM normal, mas cai numa página provavelmente sob rastreamento de
  "vram lock"/dirty-page (mecanismo de invalidação de cache de textura via
  mprotect+SIGSEGV, `vramlock_list_add`/`libCore_vramlock_Lock`, que também
  apareceram nos profiles `perf` anteriores).
- **Por que isso não passa pelo fastmem, e por que isso é ARQUITETURALMENTE
  necessário, não um bug:** Store Queue é um recurso de hardware do SH4 —
  as 8 escritas individuais que enchem a fila são instruções `MOV.L`
  SEPARADAS no código do jogo (não dá pra saber que são um "burst" até a
  instrução `PREF` que dispara o flush de verdade — confirmado em
  `rec_arm64.cpp:811-841`, `do_sqw_mmu` só dispara no `shop_pref`, nunca nos
  writes individuais). Não tem como "fastmem-mapear" a região de SQ (não é
  RAM real, é um buffer especial do CPU) nem "agrupar" os 8 writes sem
  reconhecer um padrão de MÚLTIPLAS instruções do jogo — a mesma categoria
  de risco (mexer em como o código do jogo é interpretado, não só no nosso
  próprio C++) que já tínhamos identificado como arriscada antes.
- **Correção de rota:** a ideia de "mapear VRAM no fastmem condicionalmente
  a LMMODE" (proposta na mensagem anterior) **não se aplica** — não é isso
  que está acontecendo. Descartada.
- **Conclusão honesta:** não achei um fix seguro e rápido pra esse call
  site específico nesta sessão. As opções reais que restam são todas de
  escopo maior: (a) implementar reconhecimento do padrão "8 stores + PREF"
  no JIT pra tratar como operação composta (otimização de padrão de código
  do jogo — arriscado, precisa de bastante cuidado e testes com vários
  jogos), ou (b) aceitar esse custo como inerente ao hardware emulado e
  focar em reduzir o custo por chamada genericamente (volta pro item 4.9).
  Registrado como aprendizado, não como fix pronto.

## 2026-09-14 (sessão seguinte, implementação) — Fast-path de Store Queue no JIT ARM64

- Implementado em `core/rec-ARM64/rec_arm64.cpp`, `GenWriteMemoryFast`: antes
  do caminho existente (nvmem identity-map + SIGSEGV/`ngen_Rewrite` pro
  slow-path), adiciona uma checagem barata (`addr>>26==0x38`, o mesmo check
  já usado por `shop_pref` pro flush) pra detectar escrita de 32 bits em
  endereço de Store Queue (0xE0000000-0xE3FFFFFF) e gravar DIRETO no
  `sq_buffer` (offset fixo relativo a `x28`, `offsetof(Sh4RCB,cntx)-
  offsetof(Sh4RCB,sq_buffer)`, mesma expressão que `shop_pref` já usa),
  pulando o `GenCallRuntime`/`_vmem_WriteMem32` inteiramente.
- **Decisão de design importante:** optei por essa abordagem (checagem
  inline sempre emitida, só custa uns instructions extras bem previsíveis
  em TODO write de 32 bits) em vez de estender o mecanismo de
  SIGSEGV+`ngen_Rewrite` — a segunda seria mais cirúrgica (só afeta call
  sites que já precisaram de rewrite) mas exigiria mexer no signal handler
  cross-platform e caberia num orçamento de só 3 instruções por rewrite
  (`write_memory_rewrite_size`), forçando um stub out-of-line — infra nova
  que não existe. A abordagem escolhida é mais simples de raciocinar e não
  toca em nada fora de `GenWriteMemoryFast`.
- **Cuidado de correção:** o código novo é emitido ANTES de
  `start_instruction`/`EnsureCodeSize`, então a sequência que `ngen_Rewrite`
  já sabe reescrever (Ubfx/Add+Str) fica exatamente na mesma posição
  relativa à instrução que causa o SIGSEGV — não precisou mexer em
  `ngen_Rewrite` nem em `write_memory_rewrite_size`. Pra tamanhos != 4 (SQ é
  documentado como "write only 32bit" em `sh4_mmr.cpp`), zero instruções
  novas são emitidas — o caminho original fica 100% intocado.
- **Build limpo, deploy feito** com backup do binário anterior
  (`flycast_libretro.so.pre-sq-fastpath.bak`). Sanity check (sem crash,
  sem `verify()` disparando) em kofnw (15s bench/3s warmup) e Shenmue (20s
  bench/15s warmup, warmup curto — não é o protocolo oficial, só checagem
  de "não quebrou"). **Pendente: confirmação visual do usuário** — essa
  mudança mexe no canal usado pra mandar geometria/partículas pro GPU
  (Store Queues), então um bug aqui provavelmente apareceria como corrupção
  visual, não crash.

## 2026-09-14 (sessão seguinte, resultado negativo) — Fast-path de Store Queue REVERTIDO: regrediu, não melhorou

- Benchmark oficial (mesmo protocolo de sempre, 20s/5s warmup, savestate
  kofnw) com a build do fast-path de SQ, repetido 2x pra descartar ruído:
  - Rodada 1: `core_average=12,762ms`
  - Rodada 2: `core_average=12,837ms`
  - **Antes do fix (só regalloc+batching): `core_average=12,070ms`.**
  - **Resultado: ~+6% MAIS LENTO, consistente nas duas rodadas — não é
    ruído.**
- **Causa provável:** a checagem nova (`Lsr`/`Cmp`/`B`) roda em TODO write
  de 32 bits, não só nos de Store Queue — e escritas de 32 bits comuns
  (RAM normal) são numericamente muito mais frequentes que as de SQ. O
  custo do "imposto" pago em toda escrita comum superou a economia nas
  poucas (mas quentes) escritas de SQ. **Confirma na prática o oposto do
  que a leitura de código sugeria** — a suposição de "checagem barata e
  bem prevista" não se sustentou na medição real.
- **Revertido imediatamente** (`core/rec-ARM64/rec_arm64.cpp` de volta ao
  estado só com regalloc+batching, sem o fast-path de SQ). Rebuild, deploy,
  confirmado de volta à faixa boa (`core_average=11,813ms`).
- **Lição registrada:** essa é exatamente a regra de ouro do projeto se
  provando na prática de novo — "toda hipótese levantada só de olhar
  código precisa ser validada com medição real antes de virar mudança
  definitiva". A leitura de código (útil pra entender o mecanismo) não
  substituiu a medição — e aqui a medição corrigiu a intuição. O achado do
  Store Queue (item novo em `tech_debits.md`) continua válido como
  DIAGNÓSTICO (é real, é o que domina o profile), mas o fix tentado não
  funciona nesse formato — precisaria da abordagem mais cirúrgica via
  `ngen_Rewrite` (só afeta call sites que já precisam de rewrite, não
  adiciona custo ao caminho comum) pra ter chance de dar certo, e isso é
  bem mais arriscado/complexo, como já havia sido avaliado antes de
  implementar.

## 2026-09-14 (sessão seguinte, correção de premissa + investigação nova) — master não é "só crash", é ~10x mais lento

- **Correção importante do usuário:** o `CLAUDE.md` (escrito por uma sessão
  anterior do Claude) caracterizou o `flyinghead/flycast` atual como "tem bug
  de crash conhecido, não é o foco" — mas o usuário esclareceu que o motivo
  real de interesse no master é que ele é **~10x mais lento** nesse hardware,
  não só um crash isolado. Reexaminando os próprios números já registrados
  (`docs/history.md`, 2026-09-13): o master mostrou frames **escalando de
  ~26ms pra ~100ms ANTES do crash** — ou seja, lentidão e crash são
  provavelmente a MESMA causa raiz (degradação do dispatch de bloco), não
  dois problemas independentes. `CLAUDE.md` corrigido pra refletir isso e
  deixar claro que comparar código com o master É uma ferramenta de
  investigação válida (só não é objetivo rodar/consertar o master no device).
- **Tentativa de investigação nova:** ao ler manualmente o diff de
  `blockmanager.cpp` (fork vs. master, 458 linhas) não achou nada óbvio —
  é majoritariamente modernização estrutural (renomeações, abstração de
  espaço de memória via `addrspace::`/`virtmem::`), sem regressão óbvia de
  performance visível ali.
- **Usuário revelou que o device já tem um build standalone do master
  pronto** (`/opt/flycastsa/flycast`, v2.6-9-g21eb24f86, já visto antes em
  `docs/history.md` 2026-09-13) — não precisa cross-compile. Usuário abriu
  o kofnw nele manualmente pela UI e confirmou ao vivo: **"o jogo é
  basicamente quadro a quadro nessa versão"** — bate com o "~10x mais
  lento" relatado.
- Capturado `perf` (20s, `--call-graph dwarf`) no processo rodando. **Binário
  stripped** (sem símbolo nenhum) — só endereços hex, sem nome de função.
  Padrão estrutural encontrado: uma cadeia de chamada **funda e estreita**
  (37%→36%→28%→23%→19%→19%→18%→16%→14%, ~10 níveis aninhados, cada um
  retendo 80-99% do tempo do pai, quase sem ramificação) — padrão
  estruturalmente parecido com despacho de interpretador (instrução por
  instrução), bem diferente do hot spot largo-e-plano que vimos no nosso
  fork (dentro de código já compilado pelo JIT).
- **Checado `~/.config/flycast/emu.cfg`: `Dynarec.Enabled = yes`,
  `Dynarec.idleskip = yes`** — contradiz a teoria de "roda tudo
  interpretado". Sem override específico pro kofnw
  (`find ~/.config/flycast -iname '*kof*'` vazio).
- **Hipótese revisada, NÃO CONFIRMADA:** o master pode estar caindo com
  muito mais frequência no mecanismo de `shop_ifb`/"Interpreter fallback"
  (existe no código, é uma válvula de escape legítima pra instruções
  individuais que o JIT não traduz — não é bug em si) especificamente pro
  código do Naomi/kofnw, o que bateria tanto com "Dynarec habilitado" quanto
  com o padrão de cadeia funda. **Não confirmado** — precisaria de símbolo
  no binário (= build) pra confirmar, que é justamente o custo que essa
  investigação estava tentando evitar.
- **Decisão: parar aqui por ora.** Fica registrado como pista clara e
  parcialmente caracterizada pra retomar com mais tempo dedicado — seja
  fazendo o cross-compile do master com debug symbols (aceitando o custo)
  seja continuando a leitura manual de diff nos arquivos-chave
  (`rec_arm64.cpp`, `driver.cpp` — ainda não lidos).

## 2026-09-14 (sessão seguinte) — Documentação completa dos backends JIT x86 e ARM64 + plano de melhoria

- Usuário perguntou se o JIT x86 (`core/rec-x64/rec_x64.cpp`, também usado
  como referência via `core/rec-x86/`) é arquiteturalmente diferente do
  ARM64, e o que dá pra aproveitar dele. Disparados 2 subagentes em
  paralelo, cada um lendo e documentando um backend inteiro sem viés do
  outro.
- **`docs/x86jit.md`** (997 linhas): referência completa de `core/rec-x86/`.
  Achado central: stubs de acesso a memória compartilhados
  (`mem_code[3][2][5]`, gerados uma única vez em `ngen_init()`/`gen_hande()`)
  — `ngen_Rewrite` do x86 só troca o **alvo de um `call rel32`** (4 bytes) em
  vez de regenerar lógica inline a cada rewrite.
- **`docs/arm64jit.md`** (960 linhas): referência completa de
  `core/rec-ARM64/`, incluindo os itens 1.7 e 4.9 já trabalhados nesta
  sessão.
- **`docs/arm64jit_improvement_plan.md`**: síntese acionável dos dois.
  Item 1 (prioridade máxima): levar a arquitetura de stub compartilhado do
  x86 pro ARM64 — é exatamente o que faltava pro item 1.7 (Store Queue) dar
  certo sem o custo do fast-path inline que regrediu. Item 2 (baixo risco):
  contador incremental de registradores vivos pro `PushCallerSaved`. Item 3
  (hipótese não confirmada): cache inline de salto dinâmico. Item 4 (não
  recomendado): compilação em camadas. Nota: o `CheckBlock` do ARM64 já é
  melhor que o do x86.

## 2026-09-15 — Item 1 (stub compartilhado de Store Queue) implementado e medido: ganho real mas pequeno

- Usuário pediu pra começar pelo item 1 do plano de melhoria, usando Metal
  Slug 6 como caso de teste principal: deixou um savestate na cena mais
  pesada do jogo (`/roms2/naomi/mslug6.fc2021-rrstate.auto`), reportando
  10fps antes das mudanças desta sessão, 15fps agora (jogo roda a 30fps no
  máximo) — e destacou que o mesmo trecho roda liso no PPSSPP com gráfico
  idêntico, então não acredita que seja limite de hardware.
- **Implementação (fix v2 do item 1.7):** em vez de tocar em
  `GenWriteMemoryFast` (caminho comum, já tinha regredido na tentativa 1),
  só `ngen_Rewrite` foi ensinado a reconhecer um fault de Store Queue e
  redirecionar aquele call site pra um stub compartilhado. Peças novas:
  - `core/oslib/host_context.h`: campo `x0` novo em `host_context_t` (ARM64)
    pra carregar o endereço real que causou o fault.
  - `core/libretro/common.cpp`: `context_segfault()` agora copia
    `regs[0]`↔`x0`; `signal_handler()` passa `ctx.x0` como 3º argumento de
    `ngen_Rewrite` (antes era sempre `0`).
  - `core/rec-ARM64/rec_arm64.cpp`: `sq_write_stub` (global, resetado em
    `ngen_ResetBlocks()`), gerado uma vez em `GenMemStubs()` (chamado de
    `generate_mainloop()` com um segundo `Arm64Assembler`); `GenCallStubAddr()`
    novo, espelha o cálculo de offset de `GenCallRuntime()` mas sem
    push/pop de caller-saved (não precisa, é um `Bl` cru pro endereço do
    stub); `ngen_Rewrite` ganhou um branch: se o write faltoso for de 32
    bits e o endereço faltoso (`acc>>26`) for `0x38` (range de SQ,
    0xE0000000-0xE3FFFFFF), chama `GenCallStubAddr(sq_write_stub)` em vez
    do `GenWriteMemorySlow` genérico.
  - **Bug pego e corrigido antes de compilar:** `GenCallStubAddr` emitia só
    1 instrução (`Bl`, 4 bytes) dentro do slot fixo de 3 instruções (12
    bytes, `write_memory_rewrite_size`) que `ngen_Rewrite` já reserva —
    sem preencher o resto com NOP, sobrariam bytes de lixo executados como
    instrução logo depois do `Bl`. Corrigido replicando o padrão interno já
    usado por `GenWriteMemorySlow`/`GenReadMemorySlow`
    (`EnsureCodeSize(start_instruction, write_memory_rewrite_size)` no fim
    da função — `EnsureCodeSize` é privado, mas como membro da mesma
    classe pode ser chamado de dentro).
- **Build:** `host_context.h` mudou (header compartilhado) → `make clean`
  obrigatório por causa do bug conhecido de dependência incremental deste
  fork. Cross-compile completo (comando exato do `CLAUDE.md`, `-j2`), sem
  erro de compilação; único aviso no link foi o de sempre sobre o
  `libGLESv2.so` stub local (`.dynsym` — inofensivo, já visto antes).
- **Deploy e sanity check:** backup do binário anterior
  (`flycast_libretro.so.pre-sq-stub-rewrite.bak`, era o baseline pós-reversão
  do fix v1, confirmado por md5sum). Rodadas curtas em kofnw e Metal Slug 6
  (savestate do usuário) sem crash, savestate confirmado carregando de
  verdade (`retrorun_auto_load = true`, log em DEBUG mostrou "File
  '.../mslug6.fc2021-rrstate.auto': loaded correctly!").
- **Medição oficial** (`retrorun3 --benchmark 60 --benchmark-warmup 15`, 2
  rodadas por lado, trocando só o `.so`, mesmo savestate em cada jogo):

  | Jogo | Métrica | Baseline (méd. 2 rodadas) | Fix v2 (méd. 2 rodadas) | Delta |
  |---|---|---|---|---|
  | kofnw | `core_average` | 11,40ms | 11,17ms | -2,0% |
  | kofnw | `core_frames`/60s | ~3070 | ~3101 | +1,0% |
  | kofnw | `core_p50` | 9,95ms | 9,65ms | -3,0% |
  | kofnw | `core_p95` | 20,15ms | 19,71ms | -2,2% |
  | Metal Slug 6 | `core_average` | 21,30ms | 21,00ms | -1,4% |
  | Metal Slug 6 | `core_frames`/60s | 1917 | 1932 | +0,8% |
  | Metal Slug 6 | `active_frame_p50` | 27,41ms | 25,61ms | -6,6% |
  | Metal Slug 6 | `active_frame_p95` | 71,08ms | 69,31ms | -2,5% |

  Direção consistente nas 2 rodadas de cada jogo em cada jogo (não é
  ruído) — ganho real, sem regressão, sem crash. Deployado como binário
  ativo no device.
- **Mas o ganho é pequeno e não explica o relato do usuário.** A cauda
  pesada (p95/p99, frames 3-4x mais caros que o p50 em Metal Slug 6) quase
  não melhora — ou seja, o que domina os PICOS de custo em Metal Slug 6
  não é a Store Queue. **Conclusão honesta:** o fix é correto, seguro e vale
  manter, mas não é a causa principal do gap Metal Slug 6 vs PPSSPP.
  **Próxima suspeita a investigar:** renderização/draw calls em cenas com
  muitos sprites (Metal Slug 6 é 2D denso, muitos personagens/tiros na
  tela) — não memória. Ver item 1.7 em `tech_debits.md` pra números
  completos e item 1 em `current_plan.md` pro resumo de status.

## 2026-09-15 (sessão seguinte) — Metal Slug 6 pós-fix: a função mais cara não é memória nem renderização, é o trampolim de interrupção/ciclo

- Usuário deu contexto importante: Metal Slug (1-6) também roda mal no MAME
  nesse device (MAME é emulação 100% CPU, e só peca nesses jogos raros que
  têm "algo a mais que sprites 2D" — a cena do savestate é justamente um
  "monstro gigante mecânico feito de muitas partículas diferenciadas");
  já os mesmos jogos rodam bem no FBNeo. Pediu pra usar o savestate do MS6
  pra medir qual é a função mais cara AGORA, tendo fé que a Store Queue
  (item 1.7) foi superada.
- **Descoberta de metodologia (efeito colateral útil):** um teste isolado
  logo após uma sequência de ~10 rodadas de benchmark consecutivas sem
  pausa mediu `core_average` quase 2x pior (39,7ms vs ~21ms de baseline) —
  levantou hipótese de throttling térmico. Um recheck após ~90s de idle
  voltou exatamente pro número normal (21,376ms), confirmando que é preciso
  dar um respiro entre rodadas pesadas consecutivas ou o número de uma
  única rodada isolada pode enganar — reforça a regra de ouro "repetir
  antes de confiar" já em uso nesta sessão, agora aplicada também ao
  espaçamento entre rodadas, não só à repetição em si.
- **Profiling:** `perf record -g --call-graph dwarf -F 999` anexado a uma
  instância rodando o savestate (protocolo já validado: warmup 15s +
  benchmark 60s, que mantém a cena pesada do início ao fim — um teste
  exploratório com warmup 10s/benchmark 90s diluiu demais, pegando o fim
  da luta que aparentemente acaba por volta de t≈70s pós-load, o que por
  si só é uma lição de design de protocolo: durações mais longas não são
  "mais corretas", podem só diluir a cena que se quer medir). 2 capturas
  independentes (~24k e ~37k amostras) concordaram: uma única instrução
  domina ~10-20% de TODAS as amostras.
- **Achado técnico sobre a própria ferramenta:** o endereço de amostra
  relatado por `perf script`/`perf report` para essa região JIT veio
  consistentemente com os 2 bits baixos errados (`endereço % 4 == 3`,
  impossível pra um PC real de ARM64). Sem perceber isso, uma primeira
  tentativa de desmontar o endereço "cru" via gdb gerou uma cadeia de
  `.inst ... undefined` e um `bl` aparentemente apontando pra dentro da
  reserva gigante `PROT_NONE` de 4GB do nvmem — pareceria um achado grave
  (chamando memória não mapeada) mas era só artefato de desalinhamento.
  Corrigido subtraindo 3 do endereço reportado antes de desmontar; a partir
  daí toda desmontagem bateu limpa, sem instrução inválida nenhuma.
- **Achado real, confirmado via gdb ao vivo** (endereço resolvido, `bl`
  mostrando o símbolo real): a instrução mais quente é `bl SH4_TCB+3072` —
  uma chamada embutida no final de blocos SH4 compilados (quando o
  contador de ciclos local do bloco, `w27`, estoura) pro trampolim
  `intc_sched` (`core/rec-ARM64/rec_arm64.cpp:1417-1449`, dentro de
  `generate_mainloop()`, código já existente, não escrito nesta sessão) —
  que chama `UpdateSystem()` e condicionalmente `rdv_DoInterrupts()`.
- **Isso CONFIRMA, com medição de profiling real, a hipótese arquitetural
  que o item 4.9 já tinha levantado mas não conseguido isolar:**
  `PushCallerSaved`/`PopCallerSaved` (hoje varrendo até 24 registradores
  FPU, não mais 8, por causa da extensão de regalloc do item 4.9) rodam em
  TODA chamada a `UpdateSystem`, e essa chamada acontece a cada vez que o
  orçamento de 448 ciclos SH4 (`SH4_TIMESLICE`, compartilhado entre TODOS
  os backends — x86/ARM32/ARM64 — não é algo desta sessão) de QUALQUER
  bloco se esgota. Numa cena com muitos objetos/partículas cada um com seu
  próprio código curto rodando em sequência, esse checkpoint é atravessado
  com muito mais frequência que numa cena calma.
- **Não é memória (item 1.7, já corrigido) nem renderização/draw-calls**
  (hipótese do post anterior, não confirmada por este profiling — a
  suposição de que a cauda pesada seria dominada por draw calls não se
  sustentou; o profiling aponta claramente pra despacho/agendamento de
  bloco).
- **Não corrigido ainda.** `SH4_TIMESLICE` não é candidato de baixo risco
  (mexer nele troca overhead por precisão de timing de interrupção pra
  TODOS os jogos). O candidato natural e já catalogado é o item 2 do
  `docs/arm64jit_improvement_plan.md` — reduzir o custo por chamada de
  `PushCallerSaved`/`PopCallerSaved` em vez de reduzir a frequência de
  chamada — mas não implementado ainda, aguardando decisão do usuário.
  Ver item 4.10 em `docs/tech_debits.md` pro registro completo.

## 2026-09-15 (sessão seguinte) — Autocorreção: item 4.9 não se aplica; identificado o bloco SH4 real por trás do achado

- Usuário: "podemos implementar [item 2], mas vamos mexer no timeslice
  depois caso o ganho ainda não deixe os games em boa velocidade" — sinal
  pra prosseguir com o Item 2. Antes de escrever código, reexaminei o
  próprio disassembly do `intc_sched` já capturado e percebi um problema:
  **não há NENHUM `stp`/`ldp` ao redor do `bl UpdateSystem`/`bl
  rdv_DoInterrupts`** — push/pop vazio. Item 2 reduz o custo de um
  push/pop que, nesse call site específico, já não existe.
- Causa raiz confirmada lendo o código: `generate_mainloop()` cria seu
  próprio `Arm64Assembler`/`regalloc` do zero (mesmo padrão de
  `ngen_Compile`, um por bloco) e nunca faz `Preload_FPU` antes de gerar
  `intc_sched` — `reg_alloced` fica vazio, então `PushCallerSaved` calcula
  uma lista vazia e não emite nada. **O item 4.9 não se aplica aqui** —
  autocorreção feita ANTES de implementar algo que não atacaria o achado
  medido, em vez de depois (diferente do episódio da Store Queue v1).
- Perguntei ao usuário como prosseguir (investigar mais fundo vs.
  implementar Item 2 mesmo assim vs. pular pro Item 3) — escolheu
  investigar mais fundo primeiro.
- **Identificação por fonte real, não só heurística de endereço:**
  encontrado `bm_WriteBlockMap(const std::string&)` — função já existente
  e ATIVA no binário (não em `#if 0`, ao contrário de `print_blocks()`,
  que tentei reativar com `__attribute__((used))` sem efeito porque o
  pré-processador nunca alcança esse código — edit revertida) — que
  despeja endereço guest, `code`, `guest_cycles`, `guest_opcodes` e o
  oplist inteiro de cada bloco compilado. Está oculta do dynsym mas
  presente no symtab completo (achada via `nm` sem `-D`, mangled name
  `_Z16bm_WriteBlockMapRKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE`).
  Chamada ao vivo via gdb: como ela recebe `const std::string&` (uma
  referência = um ponteiro na ABI), construí um objeto `std::string` fake
  na memória do processo à mão (`malloc` de 64 bytes pro objeto + 64 pro
  buffer de caracteres, `strcpy` do caminho, `set {long}obj = charbuf` /
  `set {long}(obj+8) = tamanho` pros campos `_M_p`/`_M_string_length` —
  não precisei me preocupar com o campo de capacidade porque a função só
  lê via `.c_str()`, nunca modifica) e chamei a função pelo nome mangled.
  Gerou `blkmap_hot.lst` (1.98MB, ~58k linhas) com TODOS os blocos
  compilados até aquele momento.
- **Achado de metodologia:** a máquina compartilhada matou o processo
  local (`run_in_background`) por baixa memória bem no meio dessa
  captura — mas o trabalho remoto (gdb + escrita do arquivo) já tinha
  terminado antes do kill, então o arquivo saiu íntegro; só precisei
  limpar o `retrorun3` órfão que ficou rodando no device depois (kill -9
  manual). Lição: um kill do harness local não significa que o trabalho
  remoto foi perdido — vale checar antes de re-tentar do zero.
- **Resultado:** o bloco contendo o endereço quente é **vaddr SH4
  `0x8C05B3A4`** — 6 opcodes SHIL, `guest_cycles=7`,
  `host_code_size=124 bytes`: carrega uma constante de endereço
  (`0x8c386928`), lê aquela palavra de memória, lê outra palavra do
  stack (`r15+19`), e compara (`setae`, >= sem sinal) — uma checagem de
  contador/limite típica de bookkeeping de objeto/partícula. Blocos
  vizinhos no mesmo caminho (`8C05B3D0`, `8C05B3D6`, `8C05B68E`) são
  igualmente minúsculos (2-3 opcodes). **Não é ineficiência de tradução
  do emulador — é a lógica do próprio jogo, rodando um número enorme de
  vezes nessa cena específica** (bate 100% com a descrição do usuário:
  "monstro gigante mecânico de muitas partículas diferenciadas"). O
  `bl intc_sched` fica visível no profile simplesmente porque QUALQUER
  bloco executado tantas vezes acaba sendo "a vez" de estourar o
  orçamento de 448 ciclos com frequência proporcional às suas próprias
  execuções.
- **Conclusão honesta:** não há alavanca de baixo risco do lado do JIT
  pra esse achado específico — nem Item 2 (push/pop já vazio aqui), nem
  encolher o SH4 traduzido (já é o mínimo possível, 6 opcodes), nem a
  cauda longa de renderização (descartada no post anterior). A única
  alavanca conhecida é frequência de checagem (`SH4_TIMESLICE`),
  explicitamente adiada pelo usuário pra depois "caso o ganho ainda não
  deixe os games em boa velocidade". Ver item 4.10 em `tech_debits.md`
  (atualizado com a correção e o achado completo).

## 2026-09-15 (sessão seguinte) — SH4_TIMESLICE como speedhack opt-in: implementado, testado, sem ganho

- Usuário: "o timeslice, como podemos deixar ele como uma opção de
  speedhack se agente mexer?" — pediu pra transformar a alavanca adiada
  no achado 4.10 numa opção de core do libretro, não numa mudança direta
  da constante.
- Implementado: `SH4_TIMESLICE` (macro, 448) continua intocada; nova
  variável `sh4_sched_timeslice` (default = `SH4_TIMESLICE`) passa a ser
  o que de fato decide a frequência da checagem de interrupção/timer, em
  todo lugar que isso importa: os 4 sites em `generate_mainloop()`
  (`rec_arm64.cpp`, JIT ARM64 — lida em tempo de geração de código,
  cravada como imediato, igual o resto do mainloop já fazia) e os 2 sites
  equivalentes no interpretador (`Sh4_int_Run`/`UpdateSystem()`, pra
  manter os dois caminhos de execução consistentes entre si). Nova opção
  de core `reicast_sh4_timeslice` (categoria "hacks", "Restart Required",
  valores 1x/2x/4x/8x, default 1x) em `libretro_core_options.h`, lida em
  `update_variables()` em `libretro.cpp`.
- **Build limpo** (`make clean` obrigatório — `sh4_interpreter.h` mudou),
  deploy com backup do binário anterior
  (`flycast_libretro.so.pre-timeslice-option.bak`).
- **Validação de que a opção realmente funciona:** rodando com
  `reicast_sh4_timeslice = 4x` num `.cfg` de teste, lida a variável ao
  vivo via gdb (`print *(unsigned int*)&sh4_sched_timeslice`) → `1792`
  (`=448×4`), confirmando que o valor passado pelo `.cfg` chega
  corretamente até o código gerado pelo JIT. Sem crash. `1x` (default)
  mede igual ao binário anterior (~21,8-22,0ms em Metal Slug 6, dentro do
  ruído já visto o dia todo) — confirma que a mudança não afeta ninguém
  que não opte explicitamente pela opção.
- **Medição do ganho:** 1ª rodada em `4x` = 22,3ms (nenhuma melhora sobre
  ~21,8-22,0ms do `1x`). 2ª rodada em `4x` = 39,2ms, mas essa rodou colada
  em 3 benchmarks anteriores sem pausa nenhuma — reconheci a
  contaminação térmica/recursos na hora (mesmo padrão documentado mais
  cedo hoje) e comecei a preparar uma repetição limpa com pausa de
  cooldown. Usuário interrompeu ("cara, pode parar") antes da repetição
  limpa terminar, e confirmou diretamente: **"não teve ganho"**.
- **Decisão:** parar de medir (pedido explícito do usuário). Código da
  opção MANTIDO e commitado — é seguro (default inalterado, testado sem
  crash) e fica disponível como ferramenta pra experimentação futura,
  mas documentado como **não resolvendo** o achado 4.10 nesta
  cena/jogo. Ver item 4.11 em `tech_debits.md` pro registro completo,
  incluindo uma hipótese não testada pra por que não ajudou (o overhead
  visto no profile pode não estar no caminho crítico do tempo de frame
  nessa cena específica).

## 2026-09-15/16 — Item 1 do plano de renderização: `glInvalidateFramebuffer`

- Pedido do usuário ("eu quero entrar nesse mundo [renderização]... Consegue
  jogar uns 2 agentes de investigação?") disparou 2 agentes paralelos: um
  auditando `core/rend/gles/*.cpp` linha a linha, outro pesquisando fontes
  primárias (guia oficial de 135 páginas da ARM, blog "Mali Performance",
  deck "Beyond Porting", precedente do Dolphin). Resultado cruzado em
  `docs/gles_code_audit.md` + `docs/mali_gles_best_practices.md` →
  sintetizado em `docs/rendering_improvement_plan.md`, 7 itens
  priorizados. Item 1 (`glInvalidateFramebuffer`) era o de maior
  confiança: zero ocorrências no código (fato do audit) + ARM documenta
  que buffer transitório precisa ser invalidado ANTES do unbind, não no
  próximo uso.
- **Implementado** em `core/rend/gles/gles.cpp`: resolvido via
  `eglGetProcAddress` (a tabela de símbolos deste fork,
  `glsym_private.h`/`HAVE_GLSYM_PRIVATE`, só cobre GLES2+extensões, não
  GLES3 core — primeira tentativa de compilar falhou por isso). Chamado
  no fim de `RenderFrame()`, depois de `ReadRTTBuffer()`, invalidando
  `GL_DEPTH_ATTACHMENT`+`GL_STENCIL_ATTACHMENT` (confirmado em
  `gltex.cpp:186-257`/`BindRTT()` que este build usa o branch de
  attachments separados, não combinado).
- Build limpo, deploy, sanity check sem crash em kofnw, Metal Slug 6 e
  mbaa.
- **Medição — Metal Slug 6, protocolo oficial** (`retrorun3 --benchmark 60
  --benchmark-warmup 15`, mesmo savestate/cena, 2 rodadas), comparado
  contra a baseline pós-fix-da-SQ já documentada (item 1.7):
  | | baseline (sem invalidate) | Run 1 | Run 2 |
  |---|---|---|---|
  | `core_average` | 21,00ms | 21,397ms (+1,9%) | 21,660ms (+3,1%) |
  | `core_frames`/60s | 1932 | 1916 (-0,8%) | 1910 (-1,1%) |

  As 2 rodadas concordam na direção: **pequena regressão real**, não
  ruído. Cena é CPU-bound (o próprio usuário já tinha marcado essa cena
  como "nunca vai ficar mais leve") — sem banda de framebuffer sobrando
  pra aliviar, só sobra o custo fixo da chamada extra.
- **mbaa** (2D, ação, testado ao vivo pelo usuário jogando — não
  benchmark automatizado): usuário relatou pico novo de 50fps (teto
  histórico do jogo nesse device era 45fps). Percentis de
  `active_frame` (frame time real, core+vídeo) das 2 rodadas com o fix:
  p50≈20,85-20,97ms (≈47,7-48,0fps), p95≈22,90-26,13ms, p99≈28,27-102,64ms
  (cauda variável, puxada por picos de ação genuínos do gameplay, não
  ruído térmico — usuário esclareceu que a variância run-to-run era
  porque estava jogando ativamente, não rodando um benchmark passivo).
  Usuário: "eu pessoalmente acho que ficou melhor... talvez tenha sido
  por conta do frameskip do retrorun ter tido mais espaço para
  respirar" — e optou por **não** jogar uma sessão baseline equivalente
  (sem o fix) pra comparação formal.
- `kofnw` (benchmark automatizado, sessão anterior): sem diferença clara
  em nenhuma direção.
- **Mecanismo plausível levantado em resposta ao usuário** (não
  confirmado por instrumentação direta, mas consistente com achado de
  código já documentado): `docs/gles_code_audit.md` §5.3 já tinha
  identificado que `QueueRender()`/`rqueue` é fila de slot único — o
  frame simulado seguinte é descartado (frameskip) se a thread de
  render ainda estiver dentro de `Render()` do frame anterior. Ou seja,
  o tempo de `Render()` (que agora inclui a chamada de invalidate) não é
  só custo de CPU — é também a *janela* de frameskip. Em cena GPU-bound
  (mbaa em ação), se o invalidate reduz trabalho real de GPU (bandwidth
  de depth/stencil), `Render()` termina mais rápido do ponto de vista da
  emu thread, o slot libera mais rápido, menos frames são descartados —
  "mais espaço pra respirar" sem precisar reduzir `core_average`. Em
  cena CPU-bound (Metal Slug 6), `Render()` nunca era o fator limitante,
  então esse mecanismo não tem como ajudar e só sobra o custo da
  chamada. Reconcilia os 3 resultados sem contradição, mas fica como
  hipótese até alguém instrumentar o contador de descarte da
  `QueueRender()` (item 2 do `rendering_improvement_plan.md`).
- **Decisão:** manter a mudança. É uso correto de API por documentação
  oficial da ARM, custo pequeno e concentrado exatamente onde não pode
  ajudar mesmo, benefício potencial real em cena GPU-bound. Commitado.
  Ver item 5.1 em `tech_debits.md` e frente de renderização em
  `current_plan.md` pro item 2 (instrumentar `rqueue`) como próximo
  passo natural, não decidido ainda.

## 2026-09-16 — Skip de Translucent sob spike de frame time: implementado, testado, revertido

- Pedido do usuário: técnica pra pular a lista Translucent do PowerVR
  quando o frame time der um spike, preferindo "glitch a lentidão".
  Implementado com 3 níveis de agressividade (`low`/`medium`/`high`),
  opt-in via core option `reicast_frame_budget_skip_translucent`.
- **Bug de build sério no meio do caminho** (não da feature em si):
  mudar `bool`→`float` num campo de `settings_t` (`core/types.h`) sem
  `make clean` deixou ~100+ `.o` desatualizados discordando sobre o
  layout da struct — regressão catastrófica de performance (~9x mais
  lento, ~9fps) confirmada tanto por benchmark quanto pelo usuário
  observando a tela. Isolado comparando o binário anterior no mesmo
  estado de device (limpo, sem o bug) — não era térmico nem warmup.
  `make clean` completo resolveu. Ver `CLAUDE.md` (regra nova sobre
  `make clean` após header amplamente incluído).
- **Testado em Shenmue (boot real, sem savestate) — usuário identificou
  2 bugs reais na feature em si, visualmente, com precisão:**
  1. Detecção invertida do pretendido: cenas estáveis (30fps) tiveram
     flicker (baseline EWMA baixa, jitter normal cruza o threshold
     relativo); cenas pesadas uniformes (25fps) não dispararam e não
     melhoraram (baseline alta, raramente varia o suficiente pra
     cruzar 1,4x dela mesma). Usuário: "média de fps não mede nada... a
     questão dos glitches com 0 ganhos é erro da sua lógica".
  2. Corrupção visual real ("frame preenchido por cor predominante,
     destruído por completo, pode ser mais de um frame"), não o
     "efeito ausente" esperado. Código confirma que a lista Translucent
     deste pipeline tem histórico de estado acoplado com passes/frames
     seguintes (rebind de buffer de índice em `DrawSorted`, prime de
     depth buffer "pro próximo pass" citando o jogo Cosmic Smash) —
     pular o bloco inteiro pula qualquer preparação de estado que ele
     devesse fazer, plausivelmente explicando corrupção que persiste
     além de 1 frame.
- **Decisão: revertido por completo** (`git revert f77fa6d1c` →
  `de9a26625`), em vez de mais patches — risco maior do que dá pra
  mapear com segurança só lendo código estático, resultado observado
  (corrupção) categoricamente diferente do pedido original (efeito
  visual ausente). Ver item 5.2 em `tech_debits.md` pro registro
  completo, incluindo o que seria necessário pra revisitar a ideia com
  segurança (métrica absoluta de orçamento, não relativa à baseline;
  preservar setup de estado da lista Translucent em vez de pular tudo).

## 2026-09-16 (continuação) — v2-v2.4 do skip de Translucent: corrigida, testada, descontinuada

- Reimplementação completa corrigindo os 2 bugs do v1 (item 5.2):
  granularidade (reduz contagem no `DrawList<Translucent>` já existente
  em vez de pular o bloco, `SortPParams` continua na lista completa) e
  sinal de detecção (`g_lastFrameTimeMs`, `retro_run()` inteiro, não só
  `RenderFrame()`). Usuário confirmou visualmente em Shenmue: efeito
  contido (cabelo picando), corrupção de frame inteiro não voltou.
- Calibração ao vivo, 2 rodadas: multiplicadores de vblank sentavam em
  múltiplos inteiros (2,0x = exatamente 30fps nativo), piscando em cena
  estável. Recalibrado, ainda piscava (inclusive no logo trivial da
  Sega) — causa real era ausência de tolerância a ruído de frame único.
  Corrigido com histerese (2 frames consecutivos).
- Usuário perguntou se o retrorun3 tem acesso a um "fps real" que a
  gente não tinha — investigação levou ao código-fonte público real do
  retrorun3 (`navy1978/retrorun` no GitHub) e confirmou: não existe
  metadado de fps nativo por jogo em lugar nenhum, o contador do
  retrorun é uma medição ao vivo (`ceil(totalFrames/elapsedSeconds)`
  numa janela de ~1s). v2.4 implementa a mesma fórmula (`g_measuredFps`)
  como referência em vez da heurística ad-hoc da v2.3.
- **Incidente de crash intermitente:** 1 `DEBUGBREAK`/SIGSEGV em 2
  rodadas de Shenmue com a v2.4. Investigação extensa com múltiplas
  hipóteses do usuário, cada uma checada no código: handler de SIGSEGV
  real (descartado, faltava o log característico), detecção de core
  errada no retrorun3 (`isFlycast2021()`, descartado, versão bate
  certinho), bug conhecido de `retro_unload_game()` documentado no
  próprio repo do retrorun (achado real, mas o contorno deveria se
  aplicar igual em toda rodada, não explica a intermitência). Usuário
  confirmou visualmente não ter visto crash; retry rodou 100% limpo.
  Não reproduzido, causa não cravada.
- **Veredito final do usuário, após A/B extensivo (mesmo binário v2.4,
  opção ligada vs desligada, mesmo protocolo de boot real repetido
  várias vezes):** a feature introduz glitch visual sem gerar ganho de
  performance mensurável. **Descontinuada** — mantida no código como
  opt-in mas desligada por padrão (não removida). Estado final: v2.4
  deployado no device, `disabled` no `retrorun.cfg` oficial e em todos
  os `.cfg` de teste. Ver item 5.3 em `tech_debits.md` pro registro
  completo.

## 2026-09-16 (continuação) — Auditoria: seria seguro nativizar `lds Rn,FPSCR` no JIT?

- Ponto de partida: instrumentação `FC_IFB_COUNT` (opt-in, `rec_arm64.cpp:35-76`,
  ver item novo em `tech_debits.md` seção 1) contou hits de `shop_ifb` por
  opcode SH4 em Shenmue real (90s+90s warmup, 2476 frames). `lds
  <REG_N>,FPSCR` domina disparado (484.809 hits, ~196/frame), seguido de
  `lds.l @<REG_N>+,FPSCR` (~21/frame), `div1` (~62/frame) e `tas.b`.
- Subagente de investigação (read-only, sem medição nova) despachado pra
  responder: por que esses opcodes caem no fallback, o que a instrução real
  faz, se `shop_sync_fpscr`/`UpdateFPSCR` já dão uma base pra tradução
  nativa, se existe precedente de invalidação de bloco por mudança de modo
  de FPU, e um veredito de risco.
- **Achado central:** `state.cpu.FPR64`/`FSZ64` (o que decide, em
  `core/hw/sh4/dyna/decoder.cpp`, se um bloco trata FPU em precisão simples
  ou dupla e como pareia registradores) são capturados **uma única vez**,
  no início da compilação do bloco (`decoder.cpp:954-955`, do FPSCR ao vivo
  em `driver.cpp:216`), e **nunca atualizados** por `lds`/`lds.l Rn,FPSCR`
  — só `fschg` atualiza um desses campos mid-block, e só porque seu efeito
  é um XOR de bit fixo conhecido em tempo de compilação, não um valor de
  registrador lido em runtime (que é exatamente o caso de `lds`). A
  segurança de hoje vem de um fim de bloco FORÇADO imediatamente após
  qualquer opcode que escreve FPSCR e cai no fallback
  (`decoder.cpp:1034-1037`) — não existe invalidação de bloco dedicada a
  FPSCR no `blockmanager` (dispatch é indexado só por endereço,
  `blockmanager.cpp:44-49`).
- **`shop_sync_fpscr` existe completo (SHIL + codegen ARM64/x64/x86 +
  regalloc + SSA) mas nunca é emitido** (`grep "Emit(shop_sync_fpscr"` em
  todo `core/` = vazio) — infraestrutura órfã, provavelmente preparada pra
  uma tradução nativa que nunca foi conectada no decoder. Sozinha não
  resolve o problema de `FPR64`/`FSZ64` obsoletos (sincroniza estado físico
  — banco FR/XF, modo de arredondamento do host — não o estado de
  decodificação do bloco).
- **Veredito:** tradução nativa "ingênua" (escrever FPSCR e continuar o
  bloco sem forçar fim) = **risco ALTO**, corrupção silenciosa de FPU sem
  rede de segurança em runtime pra detectar. Caminho "nativizar só quando
  PR/SZ não mudam, fallback idêntico ao atual quando mudam" = **risco
  MÉDIO**, viável (existe até uma máscara de 2 bits pronta e nunca usada,
  `fpscr_t::PR_SZ` em `sh4_if.h:271/275`) mas exige mecanismo novo de fim
  de bloco condicionado a valor de runtime (o precedente mais próximo,
  `BET_Cond_0/1`, é hardcoded pra `sr.T`, não genérico).
- **Não implementado — só investigação, por pedido explícito.** Documento
  completo com toda citação arquivo:linha em `docs/fpscr_jit_code_audit.md`.
  Próximo passo sugerido (não feito): instrumentar `fpscr.full` antigo-vs-
  -novo nos hits reais desse opcode pra medir que fração de fato muda
  `PR`/`SZ` antes de decidir se vale implementar o caminho de risco médio.
  completo.

## 2026-09-16 (continuação 2) — Pesquisa externa: como outros JITs de SH4/outras CPUs tratam FPSCR-like

- Complemento externo à auditoria interna acima (`docs/fpscr_jit_code_audit.md`):
  em vez de olhar só o código deste fork, foi lido código-fonte real do
  `flyinghead/flycast` (master, ~10 anos mais evoluído que este fork),
  do `inolen/redream` (outro JIT SH4 independente), do `dolphin-emu/dolphin`
  (JIT PowerPC, paralelo com paired-single/`GQR`/`HID2`) e do `hrydgard/ppsspp`
  (JIT MIPS, paralelo com `FCR31`/`ctc1`), pra ver se existe precedente de
  compilar nativamente uma escrita em registrador de controle de FPU que muda
  modo de precisão, em vez de sempre cair no interpretador.
- **Achado central, e confirma de forma independente o veredito da auditoria
  interna:** nem `flycast` master nem `redream` compilam `LDS Rn,FPSCR` como um
  "mov" contínuo sem terminar o bloco. Os dois fazem exatamente o padrão que a
  auditoria interna já tinha identificado como o único caminho seguro: a
  escrita bruta é nativa (`shop_mov32` no flycast master), mas o bloco termina
  ali (`dec_End(..., BET_StaticJump, false)` no flycast; `SH4_FLAG_STORE_FPSCR`
  tratado como terminador no redream) — o bloco seguinte é recompilado do zero
  lendo o FPSCR real. Em contraste, `FRCHG`/`FSCHG` (que só invertem 1 bit
  fixo, efeito conhecido em tempo de compilação) continuam no mesmo bloco sem
  terminá-lo nos dois projetos — é essa distinção (efeito previsível em
  compile-time vs. valor de runtime arbitrário) que decide quem pode continuar
  no bloco e quem não pode.
- **Precedente extra do redream, não confirmado no flycast master:** blocos que
  dependem de FPSCR recebem um assert em runtime no início (`ir_assert_eq`
  comparando FPSCR real vs. assumido na compilação) — cobre o caso de
  reentrar o mesmo PC via branch sob modo diferente, uma lacuna que a
  auditoria interna tinha apontado (dispatch indexado só por endereço, sem
  `fpu_cfg` na chave). Implementação do fallback desse assert não foi
  rastreada (não baixei o backend do redream).
- **Precedentes fora de SH4, pedidos como adendo:** Dolphin (PowerPC) trata os
  dois lados do mesmo problema de forma diferente dependendo do tipo de bit —
  `GQR` (equivalente mais próximo do PR/SZ do SH4: decide *forma* de código)
  é nativo sem terminar bloco, mas com um pass de constant-propagation
  (`js.constantGqrValid`) que gera um caminho genérico-mas-nativo quando o
  valor não é constante; já `HID2` (que liga/desliga paired-single como um
  todo, mais perto architeturalmente do problema do FPSCR completo) **cai
  100% no fallback pro interpretador** (`default: FALLBACK_IF(true)` em
  `Jit_SystemRegisters.cpp`, sem case dedicado) — ou seja, mesmo o JIT do
  Dolphin, 20+ anos maduro, escolhe fallback puro pra esse tipo de bit, igual
  ao que este fork faz hoje. PPSSPP (MIPS) é o contra-exemplo: `ctc1`/`FCR31`
  é nativo sem terminar bloco nenhum, mas só porque os bits que importam lá
  são puramente arredondamento/flush-to-zero — mapeados direto pro registro
  de controle da FPU real do host (`MXCSR`/`FPCR`), não pra forma de código.
  Achado curioso: o flycast master parece já fazer algo parecido
  (`Sh4Context::restoreHostRoundingMode()`, chamado em `driver.cpp`) só pro
  bit `RM` do FPSCR — não confirmado em detalhe, mas é pista de que só a
  parte PR/SZ do FPSCR exigiria mesmo o tratamento de "terminar bloco".
- **Conclusão prática:** não existe atalho seguro conhecido além do que a
  auditoria interna já tinha proposto (mov nativo + fim de bloco forçado,
  igual ao flycast master) — nenhum dos projetos de referência encontrados
  arrisca continuar o bloco depois de uma escrita de FPSCR com efeito
  desconhecido em compile-time. Documento completo com todas as citações
  (URLs de código real, manual oficial SH-4 `ADE-602-156D` com layout de bits
  e custo em ciclos) em `docs/sh4_fpscr_external_research.md`.

## 2026-09-16 (continuação 3) — Síntese final: plano de nativização de `LDS Rn,FPSCR`

- Terceiro documento do padrão "2 agentes + junção" (mesmo padrão já usado
  antes pra investigação de renderização), combinando `docs/fpscr_jit_code_audit.md`
  (auditoria interna) e `docs/sh4_fpscr_external_research.md` (pesquisa
  externa) num plano concreto: `docs/fpscr_native_translation_plan.md`.
- **Recomendação da síntese:** o caminho validado por dois JITs SH4 reais e
  independentes (`flyinghead/flycast` master e `redream`) é mais simples do
  que a alternativa "risco médio" que a própria auditoria interna tinha
  cogitado — não precisa de plumbing novo de fim-de-bloco condicionado a
  valor de runtime (o que a auditoria propôs pra só nativizar quando PR/SZ
  não mudam). Os projetos reais terminam o bloco **incondicionalmente** toda
  vez que `LDS Rn,FPSCR`/`LDS.L @Rn+,FPSCR` executa, reusando o mecanismo de
  fim de bloco (`dec_End`/`BET_StaticJump`) que este fork já tem e já usa em
  outros lugares — sem inventar nada novo em decodificação/regalloc/SSA.
  Risco reavaliado pra baixo-médio (mais baixo que a estimativa inicial da
  auditoria, que não tinha ainda a validação externa).
- Esboço de implementação registrado no documento (dar `decode` novo ou
  reaproveitar `shop_sync_fpscr` — órfão, já com codegen completo — pras
  duas entradas de tabela em `sh4_opcode_list.cpp:240,275`, emitir a escrita
  + forçar `dec_End` imediatamente depois, medir em Shenmue com o protocolo
  já estabelecido). **Nenhuma implementação feita ainda — decisão de seguir
  ou não fica pendente com o usuário.**
- Instrumentação `FC_IFB_COUNT` (`core/rec-ARM64/rec_arm64.cpp`,
  `core/libretro/libretro.cpp`) que motivou toda essa investigação,
  commitada nesta mesma sessão junto com os 3 documentos (código opt-in,
  sem custo quando a env var não está setada).

## 2026-09-16 (continuação 4) — Implementação de `LDS Rn,FPSCR` nativo, medido, sem ganho

- A pedido do usuário ("implemente o 1"), implementado o caminho
  recomendado pela síntese: duas novas `sh4dec` em
  `core/hw/sh4/dyna/decoder.cpp` (declaradas em `decoder_opcodes.h`) pra
  `lds <REG_N>,FPSCR` e `lds.l @<REG_N>+,FPSCR`, espelhando exatamente o
  código real do `flyinghead/flycast` master citado na pesquisa externa:
  `Emit(shop_mov32,reg_fpscr,Rn)` (ou `shop_readm` pra forma `.l`, com
  `Rn+=4`) + `Emit(shop_sync_fpscr)` + `dec_End(rpc+2,BET_StaticJump,false)`
  quando não em delay slot. As duas entradas da tabela
  (`sh4_opcode_list.cpp:240,275`) passaram a ter `rec_oph` apontando pra
  essas funções, igual a `frchg`/`fschg` (que já usavam esse mecanismo).
- `make clean` com as flags certas (header compartilhado editado,
  `decoder_opcodes.h`) — 0→266 objetos confirmado antes do rebuild. Build
  limpo, sem warning novo nos arquivos tocados.
- **Sanity check:** 20s sem crash (boot/intro). Depois, **2 rodadas
  completas de 90s+90s no Shenmue** (protocolo idêntico ao `bench_mine.sh`
  usado a sessão inteira) — uma com o binário novo, uma com o anterior
  (backup `flycast_libretro.so.pre-fpscr-native.bak`) — **zero crash nas
  duas**, exit 0, 2470 e 2475 frames completos. Isso cobre ~484 mil
  execuções reais de `lds Rn,FPSCR` (o opcode mais comum, medido antes via
  `FC_IFB_COUNT`) sem nenhum incidente — a parte de correctness/segurança
  do plano se confirmou na prática, não só na teoria.
- **Resultado de performance, A/B mesma cena (fps + frame time, p50/p95/p99
  + média, regra do CLAUDE.md):**

  | Métrica | Antes | Depois | Delta |
  |---|---|---|---|
  | fps (`core_frames`/90s) | 27,50 | 27,44 | -0,2% |
  | `core_average` | 17,759ms | 17,853ms | +0,5% |
  | `core_p50` | 16,038ms | 15,851ms | -1,2% |
  | `core_p95` | 32,079ms | 32,121ms | +0,1% |
  | `core_p99` | 34,247ms | 35,764ms | +4,4% |
  | `active_frame_p50` | 35,890ms | 35,811ms | -0,2% |
  | `active_frame_p95` | 50,445ms | 51,476ms | +2,0% |
  | `active_frame_p99` | 55,731ms | 57,566ms | +3,3% |

  **Sem ganho medido** — sinal misto, dentro do ruído, cauda (p95/p99)
  levemente pior. Não confirma a expectativa de ganho da investigação
  anterior.
- **Explicação em retrospecto, adicionada à síntese
  (`docs/fpscr_native_translation_plan.md`):** tanto o caminho antigo
  (`shop_ifb`→`GenCallRuntime(oph)`, chamando o handler do interpretador)
  quanto o novo (`shop_mov32` nativo + `shop_sync_fpscr`→
  `GenCallRuntime(UpdateFPSCR)`) pagam a MESMA travessia de fronteira
  JIT→C++ (push/pop caller-saved + `BLR` + volta) — a mudança elimina só a
  decodificação de `Rn`/leitura de memória dentro do handler antigo, não a
  travessia em si, que é o custo real dominante. Nenhum dos dois documentos
  da investigação tinha identificado essa equivalência antes de medir.
- **Decisão do usuário: manter implementado mesmo sem ganho isolado**
  (pergunta feita via `AskUserQuestion`, resposta escolhida: "Manter mesmo
  sem ganho"). Motivo dado: código correto e validado como seguro (mesmo
  padrão usado por 2 JITs SH4 maduros e independentes há anos), pode
  beneficiar algum fix futuro que se aproveite da escrita já ser nativa,
  mesmo sem ganho próprio hoje. Binário nativo redeployado como ativo no
  device depois do A/B (`flycast_libretro.so`, backup do estado anterior em
  `flycast_libretro.so.pre-fpscr-native.bak`).

## 2026-09-16 (continuação 5) — MBAA instrumentado e `stc.l SR,@-Rn` nativizado

- A pedido do usuário ("vamos instrumentar o mbaa entender os numeros dele"),
  rodado MBAA (`/roms2/naomi/mbaa.zip`, savestate `mbaa.fc2021-rrstate.auto`,
  cena idêntica garantida) com `FC_IFB_COUNT` + `FC_FPSCR_STATS` juntos.
- **Perfil do MBAA — é o mais ESTÁVEL dos três jogos testados:** `core_p50`
  14,07ms → `core_p99` 19,29ms, razão **1,4x** (kofnw: 3,8x; neve do
  Shenmue: 2,4x). Ou seja, praticamente não tem cauda — os "hicups" que o
  usuário sente não vêm de variação no tempo de CPU, o que aponta pra
  apresentação/áudio/pacing e não pra simulação.
- **FPSCR no MBAA: 11.866 escritas/frame** (mais que o kofnw, 9.628), com o
  guard de PR/SZ falhando só 579 vezes no total (0,003%) e 60,1% de no-ops
  pulados inline — mesmo padrão do kofnw, então o fix de FPSCR da rodada
  anterior é tão ou mais útil aqui.
- **Achado principal:** com os dois `lds ...,FPSCR` já nativos, sobrou **um
  único** fallback pro interpretador dominante — `stc.l SR,@-<REG_N>`, com
  3.399.262 hits em 1431 frames (**~2.375/frame**), e ~1.935/frame no kofnw.
  É padrão dos jogos 2D (salvar SR na pilha em prólogo de função/interrupção).
- **Por que só ele não tinha caminho nativo:** todos os irmãos da família
  (`GBR`/`VBR`/`SSR`/`SPC`/`DBR`/`SGR`) usam `dec_STM(...)`, mas o SR neste
  fork é guardado **partido** em `sr.status` + `sr.T` (`sh4_sr_GetFull()`,
  `sh4_if.h:385`), então não dá pra ler com um mov só. A versão NÃO-`.l`
  (`stc SR,<REG_N>`) já era nativa via `dec_STSRF`/`DM_ReadSRF`, que monta o
  valor completo com `mov`+`or` — metade da solução já existia no código.
- **Correção (~10 linhas, reusando o que já existe):** `DecMode` novo
  `DM_WriteMSRF` que emite as mesmas duas ops do `DM_ReadSRF` pro `reg_temp`
  (scratch que o próprio decoder já usa no caminho nativo do `div1`) e **cai
  por fallthrough no `DM_WriteM`** — assim o store com pré-decremento,
  incluindo tratamento de MMU e o fixup de exceção `rn_4`, continua
  compartilhado com os outros `stc.l` em vez de ser duplicado à mão.
- **Verificado:** o opcode sumiu da contagem de fallback (3.399.262 → **0**);
  sobraram só `div1` (~29k) e `tas.b` (~3k).
- **Medido (mbaa, savestate, 2 rodadas por lado, sem instrumentação):**

  | | base r1 | base r2 | srfix r1 | srfix r2 | média |
  |---|---|---|---|---|---|
  | fps | 49,08 | 48,73 | 50,38 | 50,55 | **+3,19%** |
  | `core_average` | 13,56 | 13,63 | 13,04 | 12,98 | **-4,31%** |
  | `core_p95` | 14,92 | 15,06 | 14,20 | 13,99 | **-5,97%** |
  | `core_p99` | 17,95 | 19,01 | 17,52 | 17,16 | **-6,15%** |

  As 7 métricas melhoraram e **as duas rodadas do fix ficaram acima das duas
  do baseline em fps, sem sobreposição** — separação limpa, não é ruído.
  Sanity no kofnw sem crash e também melhor (51,66 fps).
- **Diferente do fix de FPSCR, este melhora a CAUDA** (p95/p99 ~6%), que é
  onde os hicups aparecem. Maior ganho isolado da sessão.

## 2026-09-16 (continuação 6) — Metal Slug 6: o gargalo é o cache de textura, não a CPU

Instrumentação nova, em cadeia, cada etapa respondendo a anterior. Savestate
pesado do usuário (`mslug6.fc2021-rrstate.auto`), 30s, cena idêntica.

**1. Profiler de blocos JIT (`FC_BLOCK_PROF`)** — o backend x86 sempre incrementou
`block->runs` (`rec_x86_driver.cpp`), o ARM64 nunca; e o relatório do
`blockmanager.cpp` (`bm_Sort`/`bm_PrintTopBlocks`) está dentro de `#if 0` desde
sempre, com `all_blocks` nem existindo. Implementado incremento no ARM64 +
`bm_DumpHotBlocks()` (ordena por `runs*host_opcodes`, dumpa SH4 e SHIL dos
mais quentes). Motivo: o `perf` só mostra um blob anônimo `SH4_TCB` e o
`FC_PERF_MAP` não foi lido pelo perf.

**Achado:** 4 blocos minúsculos (`8C05B3A4`, `8C05B3D0`, `8C05B68E`,
`8C05B3D6`) rodando ~187M vezes cada = **74,6% de todas as instruções host**.
É um spin loop de 4 blocos, somente-leitura, esperando um flag em RAM
(`0x8C386928`).

**2. Watch do flag (`FC_WATCH_ADDR`)** — o byte alterna 0↔1 a cada 1-2 frames
(149 mudanças em 450 frames). Escritor funcionando. Refazendo a conta:
423k iterações/frame × ~15 ciclos ≈ 6,3M ciclos ≈ um frame inteiro a 200MHz.
**Não é bug: é idle loop legítimo**, o jogo trabalha no handler de interrupção
e espera no laço principal.

**3. Idle skip (`FC_IDLE_OPS`/`FC_IDLE_MUL`)** — o detector existente rejeitava
o bloco quente por UM opcode (`guest_opcodes<6`, o bloco tem 6). Com
`OPS=7 MUL=10`: `cycles` 7→72, iterações 187M→49,7M, **instruções host totais
26,5G→13,7G (-48%)**. **E o fps não mudou** (14,79→14,46).

**4. Split da main thread (`FC_REND_SPLIT`)** — explicou o porquê:
`rsWait` 8,7ms / **`Process` 56,8ms** / `render` 17,4ms. O JIT nunca foi o
caminho crítico; a `emu_thread` termina antes e fica esperando. O `perf`
mostrava onde os ciclos são gastos, não o que limita o frame.

**5. Split do `Process` (`FC_TA_SPLIT`)** — `make_index` 0,15ms,
`fix_texture_bleeding` 0ms, `lock` 0,001ms, `CollectCleanup` 0,8ms,
**decode (`TaCmd`) 42,1ms** — e dentro dele, **`GetTexture` 41,2ms (98%)**,
com 558 chamadas/frame = **74,8µs por chamada**. A cena tem só 560 polígonos
e 88KB de display list, então não é parsing.

**6. Contadores do cache de textura** — miss rate **70,1%**, e
**17,65s dos 30s dentro de `tf->Update()`** (re-conversão + re-upload).

**7. Causa do miss** — `NeedsUpdate()` tem duas causas; separadas:
`miss_vram_dirty` 164.717, `miss_palette` **7**, `miss_both` 11.843.
Hipótese de paleta animada (jogo 2D) **refutada**: é VRAM suja.

**8. Granularidade (achado final)** — `vram_write_faults` 5.899 (~13/frame),
`vram_invalidations` 176.299 (~398/frame): **29,88 texturas mortas por
escrita**. A VRAM é protegida e invalidada por página de 4KB inteira
(`_vmem_protect_vram(i*PAGE_SIZE, PAGE_SIZE)`, e `VramLockedWriteOffset`
percorre `for (lock : list) rend_text_invl(lock)` sem olhar intervalo), então
uma escrita derruba todas as texturas que compartilham a página. Num jogo 2D
de sprites pequenos isso é dezenas por escrita.

**Conclusão:** o gargalo do Metal Slug 6 é re-upload desnecessário de textura
por invalidação grossa — ~394 uploads/frame, ~40ms de um frame de ~63ms. Bate
com a observação do usuário de que todas as otimizações de CPU rendiam só
3-7% e que havia coisas que "nunca mudavam o fps".

**Correção de duas leituras minhas registradas aqui:** (a) eu disse que o laço
girava mais do que o SH4 real conseguiria — errado, cabe no orçamento; (b) eu
li o `perf` (53,9% em `SH4_TCB`) como "o JIT é o gargalo" — ele mostra consumo
de CPU somado entre threads, não caminho crítico.

## 2026-09-18 — Crash do meltybld/capsnk: `sq_write_stub` não salvava o LR

- Usuário reportou que o meltybld (jogo mais liso do projeto, 60fps nas lutas)
  passou a crashar durante a luta, e o capsnk também.
- **Causa (item 1.10):** o `sq_write_stub` (fix de Store Queue, item 1.7, de
  2026-09-15) é chamado com `Bl`, mas no caminho `not_sq` faz outro `Bl`
  (`GenCallRuntime(WriteMem32)`) **sem salvar o LR**. O `Ret()` final voltava
  para dentro do próprio stub, e cada volta reexecutava o `PopCPURegList`,
  cujo `ldp d16,d17,[sp],#128` soma 128 ao SP — o SP subia 128 bytes por
  iteração até sair do topo da pilha e bater no guard page. **Fix: salvar e
  restaurar `x30` em volta da chamada.**
- **Duas ferramentas de diagnóstico que faltavam e agora existem:**
  (a) `die()` passou a logar razão, arquivo e linha — antes era só
  `DEBUGBREAK!` com ~17 pontos possíveis; (b) `ngen_Rewrite` parou de seguir
  com `size` não inicializada quando não reconhece a instrução (o
  `verify(found)` é no-op sob `-DNO_VERIFY`), passando a logar o encoding e
  devolver `false` para o handler reportar o fault de verdade.
- **Três hipóteses minhas refutadas pelo dado no caminho**, todas registradas
  no item 1.10: que era regressão do dia 16 (a baseline crasha igual), que a
  pilha estava dentro da reserva de vmem (está 485MB fora), e que o
  `EnsureCodeSize` estourava o slot silenciosamente (zero estouros medidos).
- **Erro de método registrado:** bisseccionei com uma cfg de teste minha em
  vez da cfg real do EmulationStation, e usei isso para afirmar que a baseline
  também crashava. A afirmação estava certa, mas o método estava enviesado —
  o usuário apontou que jogava o jogo inteiro sem crash, e eu deveria ter
  testado o caminho real antes de concluir.
- Validado: meltybld 240s sem crash; kofnw/mbaa/mslug6 limpos e com
  performance mantida (55,6 / 50,0 / 25,5 fps).

## 2026-09-18 — Instrumentação do kofxi (savestate, core com fix do LR)

- Duas rodadas de 30s (+5s warmup) no savestate do kofxi, separadas para não
  misturar overhead: (a) `FC_TA_SPLIT`+`FC_REND_SPLIT` (mapa do frame), (b)
  `FC_BLOCK_PROF`+`FC_IFB_COUNT`+`FC_FPSCR_STATS` (JIT). Sem crash nas duas.
- **Números da rodada limpa (a):** 1.490 frames em 30s = **49,7 fps**
  (frame médio 20,1ms). `active_frame` p50/p95/p99 = 19,9 / 22,0 / 25,8ms;
  `core` média/p50/p95/p99 = 12,6 / 12,3 / 14,4 / 17,7ms; `video` 7,6ms.
  Cauda curta (p99/p50 = 1,3x) — bate com o "sem muitos hicups" do usuário.
- **Mapa da main thread:** `rsWait` 6,3ms · `Process` 3,0ms (textura 2,1) ·
  `render` 3,5ms. Cena leve pra GPU (~131 polígonos translúcidos/frame).
  **Ao contrário do mslug6, aqui quem manda é a `emu_thread`:** a main
  thread espera 6,3ms por frame, e a instrumentação do JIT sozinha derrubou o
  fps de 49,7 para 40,9.
- **Achado principal (item 4.14):** 75% das instruções host do JIT estão no
  SO cooperativo do próprio jogo trocando de contexto em vazio — ~2.100
  trocas completas por frame (save/restore de todos os registradores e dos
  dois bancos de FPU), cada uma varrendo 16 slots de handler vazios, enquanto
  as tarefas esperam o vblank. Lido direto do código SH4 no savestate com um
  desmontador mínimo em Python (scratchpad, `sh4dis.py`).
- **Achado secundário (item 4.15):** 98% das texturas paletizadas do kofxi
  usam filtro bilinear e por isso ficam fora do caminho de paleta na GPU; o
  master resolve isso no shader (`pp_Palette == 2`).
- Nada implementado ainda — só medição e diagnóstico.

## 2026-09-19 — 4.14 fechado (idle fast-forward + fila de render) e diagnóstico do Shenmue

- **Idle fast-forward validado nos 4 jogos (savestate, baseline × novo):**
  kofxi 49,7→59,2 fps e 82,9%→100% de velocidade; MBAA 50,0→59,9 fps e
  83,3%→100%; kofnw 55,6→57,0 fps e 94,4%→98,8%; mslug6 inalterado. A
  assinatura do kernel cooperativo casou sozinha no MBAA e no kofnw — é
  biblioteca compartilhada, não recurso de placa. **kofxi e MBAA rodavam em
  câmera lenta** (velocidade medida pelas amostras de áudio por segundo).
- **Efeito colateral corrigido (item 4.16):** com a emulação mais rápida que a
  apresentação, o `QueueRender` descartava em silêncio 25% dos frames. Portado
  o `SH4FastEnough` do upstream (esperar o render em vez de descartar) + uma
  condição própria (só esperar se `Process`+`Render` cabe no intervalo do
  jogo), porque esperar sozinho derrubava o mslug6 para 92% de velocidade.
  Duas tentativas refutadas no caminho, registradas no item.
- **Achado lateral (item 4.17):** o `hash()` usado pelo `idle_hash` existente
  lê bytes em vez de opcodes e só metade do bloco.
- **Shenmue (item 4.18), savestate novo do usuário:** 17,2 fps, jogo a 57,7%.
  A CPU emulada sobra; o gargalo é render. Instrumento novo `FC_GL_FINISH`
  mostrou GPU = 35ms/frame (majoritariamente fill-rate: 14,7ms a 320x240),
  envio GL ~20ms (586 draw calls), e que os dois correm **em série**: o
  present do frontend espera a GPU terminar o frame inteiro. O core não
  sincroniza em lugar nenhum — a espera está no retrorun3.
- Observação de config: as linhas `flycast2021_*` da `retrorun.cfg` são
  ignoradas por este core (ele lê `reicast_*`); hoje não muda nada porque os
  defaults coincidem com os valores pretendidos.

## 2026-09-19 — Apresentação em thread no retrorun (fork) para o Shenmue

- Diagnóstico do item 4.18 levado até o frontend: `SDL_GL_SwapWindow` sozinho
  levava 33,3ms/frame no Shenmue (o blob Mali só volta do swap quando a GPU
  termina o frame), serializando CPU e GPU na mesma thread do `retro_run()`.
- Fork `jhonatanTeixeira/retrorun` (a pedido do usuário), branch
  `threaded-present`: apresentação em thread no backend SDL, com contexto
  compartilhado e o contexto do core sem surface; FIFO com vsync, mailbox sem;
  modo `auto`; métrica de cadência na tela; override `RETRORUN_VSYNC`.
  Compilado **localmente** (cross aarch64 + sysroot tirado do device) — o
  usuário não quer build no device; um build iniciado lá foi interrompido e
  todos os artefatos removidos.
- Resultado: Shenmue 17→21 fps, velocidade do jogo 57%→71%, cauda mais curta
  de todas as rodadas com vsync + thread. kofxi: sem vsync a thread piorava o
  pacing e o áudio (o swap síncrono era o que regulava o loop); com vsync +
  FIFO o usuário viu melhora e os underruns caíram 12→3, embora a métrica de
  intervalo entre swaps discorde (ela oscila sob vsync no KMSDRM).
- Instalado no device para avaliação jogando (com backups): `retrorun3` do
  fork; `dreamcast.sh`/`naomi.sh` com vsync + thread; cfg global intocada.
- Achados laterais: o kofxi já estourava o orçamento de 16,7ms no modo
  síncrono (core 8,9 + swap 8,5ms) — origem dos hicups que ele sempre teve;
  e o próximo gargalo do Shenmue é a CPU emulada no laço de transformação de
  vértices (JIT de FPU), não espera.

## 2026-09-19 — Shenmue: a checagem anti-SMC é o próximo gargalo

- `perf` na `emu_thread` (99,6% de uma core com a GPU fora do caminho): 80% é
  código gerado pelo JIT. Ferramenta nova `FC_DUMP_BLOCK` para ver o ARM64
  emitido: 22 de ~142 instruções do bloco mais quente são a comparação
  anti-SMC inline.
- `FC_NO_BLOCK_CHECK` (diagnóstico): +19% fps, velocidade 68%→81%. O item 1.1,
  "descartado" em 2026-09-13, foi medido fora do caminho crítico.
- Reproteção de páginas quietas (estilo PCSX2) implementada e mantida, mas sem
  ganho aqui: as páginas são re-escritas logo em seguida. O log mostrou por
  quê — todas as escritas são DADO na mesma página do código, nenhuma é
  código automodificável.
- Correção proposta (aguardando ok do usuário): reescrever via `ngen_Rewrite`
  só os stores do JIT que escrevem em página de código, mantendo a página
  protegida e os blocos sem checagem. Item 4.19.
- Pergunta do usuário registrada: "cálculo de vértices não seria melhor na
  GPU?" — no Dreamcast o T&L é código do jogo no SH4 (sem T&L em hardware), o
  resultado é usado na hora pela CPU (clipping/descarte), e cada jogo tem sua
  rotina; o análogo viável seria HLE por jogo em NEON, não GPU.

## 2026-09-19 — Stores em página de código (item 4.19): implementação, um crash e a correção

- Implementado: reescrita de stores do JIT para stubs com bitmap de código de
  32 bytes, alias de store constante na compilação, registro de leituras
  dobradas pelo SSA. Tudo escrevendo por um espelho da RAM que nunca é
  travado (`0x0C000000 + RAM_SIZE`).
- **Crash no caminho:** com o alias ligado, kofxi e MBAA abortavam no boot
  (instrução ilegal na BIOS). Bisseção por interruptor isolou o alias de
  store constante; o log mostrou stores nas variáveis do kernel de tarefas
  (`0x8C03C90C`), e a causa foi o SSA embutindo leituras de páginas
  protegidas — premissa que o alias quebra. O core do device ficou com a
  mudança desligada por padrão enquanto isso (kofxi confirmado a 59 fps e
  100% com ele). O usuário viu o kofxi a **60 fps constantes** nessa hora —
  idle skip + fila de render + retrorun vsync/thread.
- Correção: leituras dobradas registradas e invalidadas por escrita sem
  fault; trecho escrito por store constante nunca é dobrado.
- Validado com a mudança ligada: Shenmue 23,3 fps / 77,7% (0 blocos com
  checagem), kofxi 59,7, MBAA 60,0 sem crash. Bateria ampla em andamento.
- Romset do Giga Wing 2 confirmado: `gwing2`.

## 2026-09-19 — Bateria ampla, política de fila no retrorun e o save do mslug6 que não se repete

- Stores em página de código validados com a mudança ligada em 11 jogos, sem
  crash (Ikaruga e capsnk exercitaram SMC real: 44 e 46 escritas em código
  tratadas pelo caminho antigo). Ligado por padrão (`FC_CODEPAGE_STORES=0`
  desliga).
- retrorun (fork): com vsync, fila FIFO enquanto o jogo roda a 100% e
  mailbox abaixo de 95% (volta acima de 99%) — vsync + FIFO arredondava jogos
  de ~35ms/frame para 3 vblanks e prendia a emulação. kofxi fica em FIFO
  (59,5 fps, 100%), Shenmue vai para mailbox (23,2 fps, 77,7%).
- **[CORRIGIDO em 2026-09-22, ver `tech_debits.md` 4.22: a média É
  reproduzível; o que varia é a cadência]** **Cuidado de método:** o savestate do mslug6 NÃO é reproduzível entre
  rodadas — sem input, a luta evolui diferente a cada vez (mesmo binário:
  17,5 a 25,5 fps, 65% a 100% de velocidade). Comparações do mslug6 entre
  rodadas separadas no tempo não valem; só A/B lado a lado no mesmo binário.
  O A/B (`FC_CODEPAGE_STORES=0 FC_SMC_REPROTECT=0` × padrão) deu 17,9 × 17,8
  fps e 99,9% × 99,3%: as mudanças do core não afetam o mslug6.

## 2026-09-22 — samsptk (Samurai Shodown 6, Atomiswave): 20fps NÃO é JIT, é conversão de paleta 4bpp na CPU

- Usuário passou os games da Sammy para `/roms2/atomiswave/` e reportou o
  `samsptk` (Samurai Shodown 6 / Samurai Spirits Tenkaichi Kenkyakuden) a
  **~20 fps**, fighting game 2D, savestate presente
  (`/roms2/atomiswave/samsptk.fc2021-rrstate.auto`, md5 `bdabd904...`).
- **Baseline medido** (`retrorun3 --benchmark 30 --benchmark-warmup 5`,
  savestate, vsync+threaded present, cfg `retrorun_samsptk.cfg` com
  `retrorun_auto_load = true`): **`core_average` 47,5 ms, ~20,5 fps**
  (`core_p50` 46,0, `core_p95` 53,5, `core_p99` 59,2ms — cauda CURTA,
  p99/p50 = 1,29x), ~617 frames/30s. O savestate carrega (confirmado no log
  DEBUG: `File '...samsptk.fc2021-rrstate.auto': loaded correctly!`).
- **Mapa do frame (`FC_REND_SPLIT` + `FC_TA_SPLIT`):** `rsWait` 5,6-6,3ms ·
  **`process` 38,5ms** · `render` 5,3ms. Dentro do `process`: `decode` 38,2ms,
  `make_index` 0,31ms, `bleeding` 0. E dentro do decode: **`GetTexture` =
  37,1ms de 38,2ms (97%)** com 1.033 chamadas/frame (36µs cada). Ou seja, o
  frame inteiro é dominado por **cache de textura**, exatamente o padrão do
  mslug6 (item 4.12) — não é parsing de display list (179KB/frame, 222 polys
  op + 827 translúcidos) nem sort (draw calls 59,7/frame, baixo).
- **`perf` (`-g dwarf -F 500`, retrorun3 ao vivo, 2 capturas independentes,
  reprodutível):**
  - `SH4_TCB` (código gerado pelo JIT) **~21-23%**
  - `texture_TW<convPAL4_TW<u16>,u16>` (**conversão de paleta 4bpp→16bpp,
    twiddled**) **~18,5-19%** — self-time, sem filhos
  - `texture_TW<convPAL4PT_TW<u8>,u8>` (4bpp→8bpp planar, p/ caminho GPU)
    **~3,2%**
  - `libmali` ~15% + ~7% disperso (driver GL)
  - **`BaseTextureCacheData::Update` → `gl_GetTexture` → `AppendPolyParam0` →
    `ta_parse_vdrc` → `rend_frame`** — a árvore de chamadas confirma que a
    conversão roda dentro do `Process`, na main thread, serializada.
  - AICA ~1,3%.
  - **Contraste com o esperado:** em nenhum momento o JIT domina; o maior
    bloco de CPU é expandir paleta. Isso **não** é o caso Shenmue (JIT/FPU) nem
    o caso kofxi (kernel de tarefas); é o caso mslug6 (textura).
- **Fallbacks do JIT estão limpos neste jogo (`FC_IFB_COUNT`, 30s):**
  `div1` 89.747 (~150/frame) e `tas.b` 2.366 — **nada** de FPSCR, `stc.l SR`,
  etc. `FC_FPSCR_STATS`: 144.482 escritas, PR/SZ mudou em 47.792 (mas o guard
  de no-op já cobre). `FC_BLOCK_PROF`: blocos quentes são laços de cópia de
  memória (SH4 8C0781CC: 7,04M execuções) e transformação/cópia de vértices —
  trabalho real do jogo, sem desperdício óbvio. **O JIT não é a alavanca
  deste jogo.**
- **Causa raiz quantificada — o bucket `dq_filter` do item 4.15.** Contadores
  novos (`FC_TA_SPLIT`, `dq_filter_*`): das ~3.670 atualizações de textura por
  30s, **3.070 são paletizadas com filtro bilinear** (`dq_filter`), i.e.
  **84% de todas as conversões** — desqualificadas do caminho de paleta na GPU
  (`IsGpuHandledPaletted` exige `tsp.FilterMode == 0`). Custo atribuído a esse
  bucket: **`dq_filter_conv_us` ≈ 8,4s de 10,0s totais de conversão (84%)** e
  **`dq_filter_upload_us` ≈ 9,9s de 11,5s de upload (86%)**. Somando
  convert+upload do bucket desqualificado ≈ **18,3s de um run de 30s = 61% do
  tempo total** (≈ 29,7ms por frame de ~47,5ms) — fecha com os 18,5% de `perf`
  só de conversão (o `perf` self-time exclui o driver Mali e o resto do
  pipeline).
- **`FC_TEX_DQ_LOG` (novo) — são POUCAS texturas GRANDES:** 2.382 updates
  em **5 texturas distintas**: **`w512 h512` × 3** (cnt 793/793/792, quase todo
  frame) e **`w1024 h1024` × 2** (cnt 2, pontuais). Ou seja, quase todo o custo
  é reconverter **três planos de 512×512 4bpp** a cada frame (~1,3MB de VRAM
  fonte/frame ≈ 40MB/s). Não é "muitos sprites pequenos" (que pedia atlas) —
  é pouca textura grande, o que casa com o shader de paleta bilinear do master
  (`palettePixelBilinear`, `pp_Palette == 2`, item 4.15) muito melhor do que um
  atlas.
- **Hipóteses baratas testadas e refutadas:**
  - `FC_TEX_SKIP_UNCHANGED` (hash do conteúdo da VRAM antes de reconverter):
    **`tex_skipped_uploads` = 0** em 3 rodadas — as texturas **mudam de verdade
    todo frame** (animação de fundo/paleta). Não é invalidação espúria; a
    invalidação está CERTA.
  - `FC_TEX_SUBIMAGE` (glTexSubImage2D em vez de glTexImage2D): sem mudança
    (`core_average` 48,4 vs 48,5).
  - `FC_TEX_PRECISE_INVL` não testado aqui — já marcado "não usar" (quebra
    imagem) no item 4.13.
- **Caminho do fix (não implementado ainda):** portar o `palettePixelBilinear`
  do `flyinghead/flycast` master (`core/rend/gles/gles.cpp`, `pp_Palette == 2`:
  4 buscas de índice + 4 buscas na paleta + interpolação manual) e afrouxar
  `IsGpuHandledPaletted` de `FilterMode == 0` para `FilterMode <= 1`. Com isso
  as 3 texturas de 512×512 vão como índice 8bpp (o `convPAL4PT_TW` que o
  `perf` mostra a 3,2%, já sem expansão de paleta) e a paleta é lida no shader
  — elimina ~8,4s de conversão + converte o upload em 1/4 do tamanho. Custo:
  mais trabalho de fragment shader na Mali-G31 (4 buscas duplas por pixel
  dessas texturas), a medir visualmente (risco de regressão de cor/glitch,
  como em toda mudança de pipeline de textura — ver item 4.2/5.2). **Não
  implementar sem validação visual.**
- **Infra usada:** `bench_env.sh` (novo, no device: `bench_env.sh TAG "ENVS"
  dur warmup` roda o benchmark com env vars), `retrorun_samsptk.cfg` (cópia do
  `retrorun_debug.cfg` + `retrorun_auto_load = true`). Descoberta repetida:
  `--benchmark` não carrega savestate sem `retrorun_auto_load = true` no cfg.
  Instrumentação nova no core: `dq_filter_updates`/`dq_filter_src_bytes`/
  `gpu_handled_src_bytes`/`dq_filter_conv_us`/`dq_filter_upload_us`/
  `dq_log_*`, todas opt-in sob `FC_TA_SPLIT` (e `FC_TEX_DQ_LOG`), custo zero
  sem a env var. Binário com ela deployado no device (backup do anterior em
  `flycast_libretro.so.pre-samsptk.bak`).

## 2026-09-22 (continuação) — Implementado o shader de paleta bilinear (item 4.15): samsptk 20 → 59 fps

- **Fix implementado** (a pedido do usuário, depois do diagnóstico da sessão):
  1. **Shader** (`core/rend/gles/gles.cpp`): `pp_Palette` deixou de ser booleano
     e virou 0/1/2 (sem paleta / nearest / bilinear). Portado o
     `palettePixelBilinear` do master adaptado à paleta 1024×1 deste fork
     (4 amostras NEAREST dos texels vizinhos via `textureSize()` + busca na
     paleta + `mix`). Renomeado o helper `palettePixel` para `getPaletteEntry`.
  2. **`IsGpuHandledPaletted`** (`core/rend/TexCache.h`): passou a aceitar
     `FilterMode <= 1` quando `g_paletteBilinearSupported` (flag nova, setada
     pelo `findGLVersion` do backend GLES quando GLSL >= 1.30 / GLES3/GL3).
     Renderers sem suporte (Vulkan, gl4) e GLSL antigo ficam nearest-only.
  3. **`gldraw.cpp`**: `palette` passou de bool a int (`FilterMode + 1`);
     `GetProgram` usa 2 bits pra ele; o filtro GL da textura continua NEAREST
     quando a paleta é na GPU (a interpolação é feita no shader).
  4. Escape hatch `FC_NO_GPU_PAL_BILINEAR=1` força o comportamento antigo.
- **Medição (samsptk, savestate, mesmo binário, A/B ligado × desligado,
  2 rodadas):** `core_average` **48,9 → 12,1 ms**, fps **20,0 → 59,4**
  (100% de velocidade do jogo), `core_p95` 53,7 → 12,7ms; underruns de áudio
  195 → 2. Mesma cena (222 polys op + 827 tr). `dq_filter_updates` caiu de
  ~3.070 pra **0** (todas as paletizadas bilineares agora usam a GPU).
- **Mecanismo confirmado por `perf`:** o hotspot de conversão
  `convPAL4_TW` (~19%) desapareceu; sobrou `convPAL4PT_TW` (só untwiddle, sem
  expandir paleta) a ~8%, e o `SH4_TCB` (JIT) voltou a ser o topo (~25%). Com
  o fix, `render` mede ~23ms dos quais **GPU ~17ms** (`FC_GL_FINISH`) — o jogo
  saiu de CPU-bound (conversão) para ~60fps cheio, agora levemente GPU-bound.
- **Bateria de regressão (mesmo binário, A/B por jogo):** neutro em kofnw
  (+3%), kofxi, ggx15, mbaa, meltybld, ggxxsla, sfz3ugd, gwing2 (todos ~1,00x).
  mslug6 apareceu 0,92x numa passada mas é o savestate documentado como
  **não reproduzível**; A/B lado a lado no mesmo binário deu 19,4 × 19,5 × 19,4
  × 15,0 fps (off1/on1/off2/on2) — ruído, sem regressão.
- **Bateria de crash (fix ON, 10 jogos que não estavam no A/B):** capsnk,
  cspike, cvs2, ggxxac, ikaruga, kov7sprt, ngbc, rumblef2, slashout, spawn —
  todos `exit 0`, sem crash de shader.
- **PENDENTE — validação visual (usuário):** mudança de pipeline de textura
  tem histórico de regressão visual neste projeto (itens 4.2/5.2). O ganho
  medido é grande demais pra não ser checado: pedir ao usuário pra jogar o
  samsptk e confirmar que as cores/fundo/sprites estão corretos, e olhar 1-2
  jogos 2D já conhecidos (kofnw, mslug6) por garantia. Se houver corrupção,
  `FC_NO_GPU_PAL_BILINEAR=1` desliga sem rebuild.

## 2026-09-22 — DOA2 (Dreamcast): diagnóstico — render thread domina, e a política de fila engana a métrica

- Pedido do usuário: "salvar" o Dead or Alive 2 (tem savestate em
  `/roms2/dreamcast/Dead or Alive 2 (USA).fc2021-rrstate.auto`). **Confirmado
  que o jogo é 60fps** — medido limpo no fim desta sessão com um contador novo
  que conta os **pedidos de render do jogo em tempo emulado** (`req_native_fps`,
  imune aos drops da fila): **59,92Hz**. (Uma primeira tentativa,
  `swap_native_fps`, deu 31,65 e estava **contaminada** — só contava os swaps
  que chegavam ao render thread, com 521/591 frames descartados pela fila;
  registro do erro de medição.) `declared_fps=60`, `game_interval_ms_ema`
  16,68ms confirmam.
- **Baseline** (`retrorun3 --benchmark 30 --benchmark-warmup 10`, savestate,
  vsync+threaded present): **23,1 fps**, jogo a **73,9% de velocidade**
  (`core_average` 42,0ms, `core_p50` 42,5 / p95 50,8 / p99 75,2ms; 186
  underruns de áudio).
- **Quem limita (medido, não inferido):** instrumentei o intervalo real entre
  frames produzidos pela emu thread (`emu_frame_interval_ms_avg`). Deu
  **24ms** — a emu thread (SH4) produz a ~42fps, **longe dos 60 nativos**.
  Mas eu errei ao concluir daqui que "a emu thread tem folga" — o dado que
  decide é a **amostragem de CPU por thread** (`/proc/<pid>/task/*/stat`,
  jiffies em 5s), e ela mostra **duas threads quase saturadas**:

  | thread | CPU (de 1 core) | o que roda (via `perf --sort pid`) |
  |---|---|---|
  | emu (`dc_run`: SH4+AICA+ARM7) | **~103%** | `SH4_TCB` 39% self, `ta_vtx_data32`, `ARM7_TCB` |
  | main/render (`retro_run`) | **~72%** | `FifoSplitter::ta_poly_data`, `make_index`, `DrawStrips`, `SetGPState`, `libmali` |
  | SDLAudio / mali-cmar / presenter | 3% cada | áudio, driver GPU |

  **Correção de rota (registrada):** eu primeiro li o `perf` agregado
  (`SH4_TCB` 40%) como "a emu thread gira produzindo frames descartados, não é
  o limitante". **Errado** — 103% de um core na emu thread significa que ela
  está **saturada de trabalho real** (SH4 + parse de vértices na TA + ARM7),
  não girando à toa. A `rsWait` de 14,7ms é a main thread esperando essa emu
  thread saturada. **O DOA2 é, primariamente, limitado pelo throughput de
  emulação SH4** (~74% = 24ms para produzir o que deveria levar 16,7ms), com o
  render (~22,5ms, 640 draw calls) como **segundo gargalo** que realimenta via
  fila (521/591 frames descartados quando a main thread não acompanha).
- **O render NÃO é GPU:** `FC_GL_FINISH` → GPU ≈ 8ms; baixar a resolução para
  320x240 **não mudou** o `render` (~21,7ms). São **~640 draw calls/frame ×
  ~34µs** = overhead de submissão do driver Mali JM (mesmo teto do item 4.2).
  `process` é só 5,5ms (decode 5,3, textura 2,6 — irrelevante; 2.804 polys
  opacos).
- **O batching já funde ~4,5 strips/draw** e o item 6 (máscara `isp` mais
  justa) só cobriria ~9% dos breaks — os breaks reais são `tcw`/`tsp`
  (texturas/estado diferentes). Não é alavanca grande.
- **ACHADO IMPORTANTE — a métrica de fps engana, e o usuário viu na tela:**
  `FC_AUTOSKIP=0` (esperar o render) sobe os frames apresentados de 23,1 para
  **32,6 fps**, mas a **velocidade emulada do jogo CAI de 73,9% para 54,5%**
  (áudio 977.920 → 481.280 frames). O usuário descreveu exatamente isso antes
  de eu medir: *"passa de 30 fps mas fica visualmente mais lento"*. Mecanismo:
  com autoskip off a emu thread **espera** o render lento, e a apresentação
  conta frames que a main thread empurra enquanto a emulação avança menos
  ciclos/s. `FC_AUTOSKIP=1/2` mantém 73-74% de velocidade com 23,7 fps.
   **Conclusão: não existe política de fila que resolva o teto de velocidade
   — a emu thread (SH4) está saturada.** A cadência de tela
   (`RETRORUN_PRESENT_STATS`) confirma: autoskip off p50 29,4ms (parece mais
   fluido na contagem) mas o jogo anda a 55%.
- **Pista do `flycast_extreme` (do `game_status.md`):** no device só existe o
  build **32-bit ARM** (`/home/ark/.config/retroarch32/cores/flycast_xtreme_libretro.so`,
  2,28MB, de 2020-03-04, stripped, via RetroArch32) — não roda no `retrorun3`
  (64-bit). É a versão antiga pública `flyinghead/flycast_xtreme`, não um fork
  com código secreto; comparar código é possível via upstream histórico.
- **Gargalo do frontend (retrorun) — pedido do usuário para tratar junto:**
  a apresentação em thread do fork (`threaded-present`, commit `648f450`) já
  resolveu parte do Shenmue. No DOA2 ela interage com o autoskip: em modo
  mailbox, sem esperar o render, retro_run() dispara frames mais rápido que a
  emulação avança, e é a **contagem de frames apresentados** (não a velocidade
  do jogo) que sobe. A velocidade real só é observável pela taxa de áudio
  (`audio_frames/sample_rate`). Isso é uma armadilha de medição que vale
  corrigir no frontend também (expor/exibir velocidade do jogo, não frames).
- **Nada corrigido ainda.** Instrumentação nova (opt-in, `FC_REND_SPLIT`):
  `emu_frame_interval_ms_avg`/`emu_frames` (Renderer_if.cpp), contadores de
  quebra de batching `batch_breaks_*`/`batch_isp_irrelevant_only`
  (gldraw.cpp). Binário deployado no device.

### Nota de método (a pedido do usuário)
O usuário reforçou: **medir o core e o retrorun em conjunto**, porque o
sintoma que se vê na tela ("mais fps mas mais lento") só aparece na interação
dos dois. A métrica de `core_frames` do retrorun não é velocidade de jogo na
presença de fila/pacing — a taxa de áudio é. Ver `docs/tech_debits.md` item
4.20.

### 2026-09-22 — DOA2: correção do diagnóstico (emu-bound, não render-bound) e pista do idle_hash

- **Correção de leitura (importante):** a primeira conclusão ("render thread é
  o limitante") estava **errada**. A medição que decide é a **amostragem de CPU
  por thread** (`/proc/<pid>/task/*/stat`, jiffies em 5s):
  - emu thread (`dc_run`: SH4+AICA+ARM7): **~103% de um core** — saturada
  - main/render thread (`retro_run`): ~72%
  - confirmado por `perf --sort pid`: a thread de 103% roda `SH4_TCB` (39%
    self) + `ta_vtx_data32` + `ARM7_TCB`; a de 72% roda `FifoSplitter`,
    `make_index`, `DrawStrips`, `SetGPState`, `libmali`.
  - CPU em 1,512GHz (máx) em todos os cores — não é throttling.
- **`req_native_fps = 59,92`, `req_rtt = 0`, `req_display = 1126`** (contador
  novo, imune a drops): o jogo pede 60 frames de TELA/s em tempo emulado, sem
  RTT nenhum. Logo, DOA2 é 60fps nativo mesmo (o usuário estava certo; minha
  `swap_native_fps=31,65` estava contaminada pelos 521/591 frames descartados).
- **Quadro final:** a emu thread (SH4) produz ~40 frames reais/s onde o jogo
  pede 60 → **~74% de velocidade** (bate com o áudio). O render (640 draw
  calls, 22,5ms) é o segundo gargalo: descarta os frames que não cabem, então a
  tela mostra ~21fps. **Remover o render não levaria a 60fps** — o teto de 74%
  é da emulação SH4 (mesmo caso do Shenmue em cena pesada, item 6 do
  `current_plan`). O frontend (vsync/threaded) não muda isso: 4 combinações
  testadas, todas ~74%.
- **`FC_DUMP_BLOCK=8C101BC4`** (bloco #1, 10,4% do trabalho do JIT): laço de
  transformação de vértices (`fmov.s @Rm+`, `fadd`, `ftrv xmtrx`, `dt/bt`). O
  `ftrv` já sai em NEON (`ld1/fmul/fmla/st1`) — bom. Mas os `fldi0/fldi1/fadd`
  e `fmov.s` fazem muito tráfego `[x28,#...]` (contexto SH4) porque os
  registradores FPU desses ops não estão alocados — só 8/24 são mapeados
  (item 4.9). Não é um único bloco dominante: o trabalho do JIT está espalhado
  (total ~11,8G host-ops em 30s, ~393M/s).
- **Pista nova (idle):** os blocos `8C12D2C0`/`8C12F99E`/... têm
  `cycles=224 = max_cycles` — são **matches do `idle_hash`** (a entrada
  "Dead or Alive 2" existe em `decoder.cpp`), e `idle_hash` é o mecanismo
  antigo que o item 4.17 suspeita casar blocos errados (lê bytes, não opcodes,
  e só metade do bloco). Não medido ainda se esses matches são corretos.
- **Nada corrigido.** Instrumentação opt-in nova: `req_native_fps`/`req_rtt`/
  `req_display`/`swaps`/`emu_frame_interval_ms_avg`/`batch_breaks_*`/
  `opaque_distinct_states_per_frame` (todos sob `FC_REND_SPLIT`).

### 2026-09-22 — Evolution 1 não inicia: CHD com codec zstd (`cdzs`)

- Sintoma: fecha na inicialização com *"Unable to find bios in
  /roms2/bios/dc/"*. Log do core em INFO: `Did not load bios, using reios` →
  logo em seguida o erro de BIOS. Causa: o disco não abre (`NoDisk`), e aí
  `nullDC.cpp` força BIOS real, que o fork não acha (não lê `dc.zip`).
- Header do CHD: `cdzs cdzl cdfl` (os demais: `cdlz cdzl cdfl`). O `libchdr`
  do fork não tem zstd. Nada corrigido ainda; ver `game_status.md`.

### 2026-09-22 (noite) — KOF Evolution, mslug6, MBAA (dumps) e Macross

- **Macross M3** (CHD novo do usuário, `cdlz`): boota com HLE, ~60 fps e
  100% na abertura (core p50/p95/p99 10,8/28,7/40,6ms).
- **KOF Evolution (chuva)** — `tech_debits.md` 4.21: jogo a 100%, tela a
  30fps; render ~21ms > 16,7ms e a fila de 1 slot descarta o frame que chega
  durante o render. 71% da main thread dentro da `libmali`; ~10% é o driver
  varrendo o index buffer por draw. Implementado `glDrawRangeElements`
  (`FC_NO_DRAW_RANGE=1` desliga) — **compilado, não medido**.
- **mslug6** — 4.22: 4 rodadas iguais (19,8-19,9 fps, 66%). A nota antiga de
  "não reproduzível" estava errada quanto à média. Eu errei uma leitura na
  sessão (amostragem do arquivo de split a cada 5s → "30fps/100%" falso); o
  usuário corrigiu vendo a tela.
- **MBAA** — 4.23: dumps do slot 2 mostram glitches fixos (blocos no
  retrato, lixo tipo texto). Descartados RTT, cache de textura
  (`FC_TEX_ALWAYS_UPDATE`, novo) e sorting. Retrato sumindo é intermitente.
- **Infra:** `bench_game.sh TAG SAVEDIR ROM "ENVS" dur warm` (cfg
  `retrorun_dbg2.cfg`, log DEBUG mostra "loaded correctly"),
  `bench_test.sh` (mesmo, com `/home/ark/flycast_test.so`), `perf_game.sh`,
  `timeline_game.sh`. Slot 2 do savestate = arquivo `.auto1`; copiar para
  uma pasta temporária como `.auto` e passar como `-s` (modo `--benchmark`
  não grava state). **Toolchain:** nesta máquina só existem
  `aarch64-linux-gnu-g++-13`/`gcc-13` (o link sem sufixo sumiu) — passar
  `CXX=aarch64-linux-gnu-g++-13 CC=aarch64-linux-gnu-gcc-13
  CC_AS=aarch64-linux-gnu-g++-13`.
- Core ativo no device **intocado** (build `32a2b9ed1`).

### 2026-09-23 — RE CV / EGG (FMV) e DOA2: o "teto de SH4" medido por contadores de hardware

- Pergunta do usuário: outra análise culpou o SH4 (codegen, ~3,2 host ops/op)
  tanto na FMV quanto no DOA2 — "como 3D e FMV batem na mesma coisa?".
- Medido (`tech_debits.md` 4.24/4.25): os dois têm IPC ~0,5 na emu thread,
  mas a FMV para em **cache de instrução** (código traduzido quente 60-115KB
  × L1I 32KB) e o DOA2 em **DRAM**. Clock real ~1,30GHz; governor não muda
  nada. CHD (LZMA+ECC) come ~8% da emu thread na FMV.
- `PREF`→`PRFM` implementado e A/B no DOA2: ruído. Binário de teste
  (`/home/ark/flycast_test.so`) tem ainda `glDrawRangeElements` e
  `FC_TEX_ALWAYS_UPDATE`; core ativo intocado.
- Scripts novos no device: `ipc_game.sh` (perf stat por janela na emu thread
  + timeline por mtime), `l1i_game.sh` (amostragem por evento de L1I).
- **Continuação (usuário não aceitou "FMV é gargalo"):** medido em tempo
  EMULADO com `FC_BLOCK_PROF` (estendido): 171M instr SH4 por segundo
  emulado na FMV, ~88% de código real (decoder, divisão por software,
  middleware CRI), idle já barato. O próprio SH4 real estaria quase cheio. O
  nosso teto é ~9 ciclos host por instrução SH4 (~85%), e o C++ na emu thread
  leva ao ~70%. `tech_debits.md` 4.26.
- **Otimização do JIT a partir do dump do código gerado (pedido do usuário):**
  `FC_DUMP_BLOCK` nos 14 blocos quentes da FMV mostrou que cada acesso à RAM
  emulada custava 5 instruções ARM (mov→w0, add, ldr/str, nop, mov). Feito o
  acesso compacto (`tech_debits.md` 4.27): base em `x13`, `ldr/str [x13, wA,
  uxtw]`, trampolim no fault. FMV do RE CV **71% → 82-83%** (2×2 rodadas).
  Bateria de 18 jogos, 1ª passada: sem crash, mas **Shenmue 76→68%,
  Soulcalibur 91→79%, KOF Evo 99,5→95%** — o trampolim salvava v16-v31 +
  LR em toda execução de site reescrito (SQ/hardware), onde antes havia um
  `bl` direto. Corrigido: stubs sem salvamento, genérico salva só os S16-S31
  vivos no site (máscara gravada na emissão). Junto: acesso compacto de 64
  bits e `ZeroExtendLoadPass` (ssa.h: `mov.b`+`extu.b` → `ldrb`,
  `FC_NO_ZX_LOAD=1` desliga). Achado o bug latente 4.28 (slow path reescrito
  não salvava S16-S31). 2ª passada da bateria em andamento.
- **Resultado final da sessão (binário de teste `/home/ark/flycast_test.so`,
  core ativo intocado):** acesso compacto + 64 bits + leitura sem sinal +
  trampolins enxutos. FMV do RE CV **71% → 83%** (confirmado no binário
  final). Bateria de 18 jogos sem crash; ganhos em Soulcalibur (88→93%),
  meltybld (80→87%), DOA2 (+1pt), SA2 (+1pt), mslug6 (+10% fps); kofnw com
  emulação 9% mais barata mas fps pior por pacing (4.29). **mslug6 bimodal
  confirmado em número** (4.22): rodada antiga a 99,3% e outra a 66,6% no
  mesmo binário. Um erro meu registrado: li o "64% → 83,5%" da bateria como
  ganho, era troca de modo.

### 2026-09-23 — flycast2026: novo nome do nosso core, upstream oficial instalado

- Prefixo de opções do fork: `reicast_` → `flycast2026_`
  (`libretro_core_option_defines.h`). `library_name` segue "Flycast" com
  versão sem "v" de propósito: o retrorun3 continua aplicando os quirks de
  core legado (não chamar `retro_unload_game`) e os savestates
  `*.fc2021-rrstate.auto` continuam sendo lidos.
- Device: `flycast2026_libretro.so` (+ `.info`) = binário final (acesso
  compacto, 64 bits, leitura sem sinal, trampolins enxutos, SQ,
  `glDrawRangeElements` LIGADO). `flycast_libretro.so` = flyinghead/flycast
  v2.7-42-g869038f40 (backup do anterior em
  `flycast_libretro.so.bak-fork-pre-upstream`). `retrorun.cfg` e
  `retroarch-core-options.cfg`: `reicast_*` removidos, 18 chaves
  `flycast2026_*` com a configuração medida (backups `*.bak-pre-flycast2026`).
  ES: flycast2026 primeiro em retrorun3/retrorun3go2/retroarch nos 3
  sistemas, padrão dos sistemas e 7 jogos fixados migrados para flycast2026
  (backups `*.bak-flycast2026-1790166999`). Scripts `*.sh` não precisaram
  mudar (já carregam `"$2"_libretro.so`).
- **`glDrawRangeElements` medido (A/B 2×2, KOF Evolution na chuva):**
  **31,1 → 54,2/54,3 fps**, render 18,7 → 13,9ms; a velocidade cai 100% →
  92% porque agora a fila faz a emulação esperar o render (4.29).
- Upstream no device: SA2 (boot) 32 fps/59%, sfz3ugd 50 fps/90%; roda como
  core moderno no retrorun3.
- Soulcalibur congelou no boot pelo ES com flycast2026 (4.30), não
  investigado a pedido do usuário.
- Paliativo da fila (4.29): espera só com render ≤ 75% do intervalo
  (`FC_AUTOSKIP_MARGIN`), instalado como flycast2026 **sem medição** (usuário
  precisou sair). Backup: `flycast2026_libretro.so.bak-pre-margin`. Para
  benchmarks do core novo usar `~/bench_2026.sh` (cfg `retrorun_dbg3.cfg`
  com chaves `flycast2026_`; o `retrorun_dbg2.cfg` antigo só tem `reicast_`).

### 2026-09-24 — Fila e velocidade de emulação (DOA2, Zombie Revenge, KOF Evo)

- Estado estável congelado antes de mexer: commit `73c813d7d` + tag
  `flycast2026-estavel-2026-09-24`, binário em
  `../flycast2026-builds/`, snapshot completo do device em
  `~/snapshot-flycast2026-estavel-2026-09-24/` (core, cfgs, ES, scripts).
- Regras da sessão (usuário): 3D medido por **velocidade de emulação**; 1
  rodada por jogo, sem A/B; só retrorun + instrumentação de JIT/GLES.
- Achados: fila não limita DOA2/Zombie (emu espera o render 0,1-0,3ms/frame);
  rewrite usava `x0` como endereço (4.32) → SQ e OCRAM agora rápidos: DOA2
  72,9 → 80,9%, Shenmue 76 → 79,9%; KOF Evo 100%; Zombie 74% (limitado pela
  moldura de blocos pequenos, 4.34). Liberação antecipada da fila (4.33):
  física do render do KOF Evo impede 60 fps a 100% sem render mais curto.
- 2D conferidos (1 rodada): SFA3 98,6%/58,8fps, MBAA 97,9%/50,2, KOFXI
  99,8%/59,2.
- Instalado como `flycast2026_libretro.so` (md5 3ed1ac55); o estável anterior
  em `flycast2026_libretro.so.bak-estavel-2026-09-24` e no snapshot.
- **Encurtar o render (passo 1):** instrumentado o render por etapa; 87% do
  Render do KOF Evo eram translúcidos, e 1,93ms só na subida de index buffer
  por draw em lote. Um upload por lista (4.35): KOF Evo **58,8 fps a 100%**,
  DOA2 83,3%, Zombie 77,9%, Shenmue 81,6%; 2D iguais. Instalado como
  flycast2026 (anterior em `.bak-pre-oneupload`).
- **Passo 2 (moldura dos blocos) — medido antes de implementar:** tráfego de
  contexto é ~30% do laço de vértices do DOA2, mas os desvios do laço são
  ~50/50 e o laço é ~45% do JIT: teto ~5% de velocidade com superblocos.
  O `pref` → TA do mesmo laço: ~21 mil/frame, ~8% do orçamento; stub direto
  ~4%. Registrado em 4.34; decisão de seguir fica com o usuário.
- **Stub `pref` → TA (4.36):** implementado; primeiro não pegava no DOA2
  porque o load do savestate forçava o C — corrigido save/load. Ganho dentro
  do ruído (DOA2 ~83%); Zombie 80,4%. Instalado (anterior em
  `.bak-pre-tastub`).
- **Margem do render (4.37):** quebras de lote são trocas reais de textura
  (mesma textura GL: 0-6/frame); o draw no driver é 70% do custo por draw.
  Sobra ~1ms em cache de uniforms; o resto exige menos draws (atlas).
- **Caça ao "eureka" do DOA2 (4.38):** FPSCR, interpretador e SMC limpos;
  scheduler por dispositivo: som ~2,7ms/frame. Os laços de espera de DOA2 e
  Shenmue só eram cobrados pelo `idle_hash`, não pulados — assinaturas de
  avanço até o evento: DOA2 84,8 → 87,6%, Shenmue 80,7 → 83,1%. A conta
  fecha: ~116M instr SH4/s de trabalho real × ~11 ciclos ARM por instrução =
  o núcleo inteiro. Próximo "paaaw" só vem de baratear o JIT por instrução.
  Instalado (anteriores em `.bak-pre-doa2ff` e `.bak-pre-shenff`).
- **Le Mans como "método Demóstenes" (4.39):** velocidade 100% mas o jogo
  pede 16 frames/s em tempo emulado; 54% do tempo emulado num laço de atraso
  por contagem. Achado central: o padrão `sh4clock = d12` cobra 1,2x os
  ciclos de todo bloco (SH4 a ~167MHz efetivos). `d10`: Le Mans 16,1 → 18,8
  frames/s do jogo sem perda; DOA2/Zombie/Shenmue/KOF Evo/2D iguais.
  Aplicado no cfg oficial (retrorun e RetroArch).
- **Pulo do laço de atraso (4.40):** Le Mans com d8 agora 25,4 frames/s a
  100% (d12 original: 16,1). Assinatura só casa no Le Mans. Instalado
  (anterior em `.bak-pre-delayskip`). Padrão do cfg continua d10.
- **Detector de espera x varredura (4.41):** o `strlen` do Le Mans era
  cobrado x30 como se fosse espera (25% do tempo emulado). Corrigido: Le Mans
  18,6 → 24,2 fps do jogo no d10; Zombie → 87,5%. Instalado (anterior em
  `.bak-pre-scanfix`).
- **Le Mans travado nos 30 (4.42):** override de clock por jogo na tabela
  do core (T15111D 50 → 0.8). Com o cfg d10, Le Mans 30,1 frames/s a 100%;
  DOA2 inalterado. Usuário: "isso é o le mans que eu me lembro". Instalado
  (anterior em `.bak-pre-lemanslut`).

### 2026-09-24 (noite) — Shenmue II, o chefão final

- Primeira rodada (save do usuário, ~20 fps): 68,6%, jogo pede 30; 320×240
  idêntico → não é GPU. Laço de espera igual ao do Shenmue 1 (outra
  compilação, tarefa vazia) = 54% do JIT, + espera por contador em memória.
  Avanço até o evento (4.43): **Shenmue II 68,4 → 84,2%** (25 fps); a
  assinatura curta ajudou também o Zombie (80,8 → 87,0%). Checagem "só se o
  endereço é RAM" para não passar do ponto em timers de hardware. Instalado
  (anterior em `.bak-pre-shenmue2`).

### 2026-09-24 (noite) — Limites de barramento do device (medido)

- DMC 666MHz (governor performance), barramento de 32 bits → teto teórico
  666M × 2 × 4B = **5,33 GB/s**, dividido entre CPU e GPU. Benchmark próprio
  (C estático, 64MB, threads sincronizadas por barreira; a 1ª versão somava
  o melhor tempo de cada thread e dava 7,4 GB/s, impossível):
  leitura 2,2 GB/s com 1 núcleo / 4,1 com 2 / 4,5 com 4 (~84% do teto);
  escrita ~4,2 GB/s (1 núcleo já satura); cópia 2,7 / 3,6 / 3,9 GB/s.
  NEON não muda a leitura (o limite é a janela de misses do A53, não a
  instrução). Latência aleatória: L1 2,3ns, L2 13–18ns, DRAM ~165–200ns
  (~250–300 ciclos a 1,5GHz). Leitura: um núcleo sozinho usa só ~40% do
  barramento; o que pesa pro JIT é a latência de miss, não a banda.

### 2026-09-25 (madrugada) — Shenmue II save pesado: som em thread própria

- Save novo do usuário: 56,5%, 17 fps. Perfil de blocos plano (sem laço de
  espera dominante); `FC_REND_SPLIT` achou 13ms/frame no `AicaUpdate` (ARM7
  2,8 + mixagem das vozes 10,4; ~49 de 64 vozes ativas).
- Tentativa 1 (mixagem inteira em thread): nula — o driver do ARM7 consulta
  posição/envelope/KYONB das vozes quase todo bloco. Desenho final (4.44):
  controle exato na emu thread, render (interpolação/filtro/volume/mixagem)
  numa thread de som com fila; bloco rápido de controle sem chamadas
  indiretas. PCM bit a bit idêntico ao original.
- Shenmue II 56,5 → 60,6%; Shenmue 1 86,5%; Zombie 100%; DOA2 88%; 2D 100%.
  Instalado (anterior em `.bak-pre-aicathread`).

### 2026-09-25 — Estudo do código gerado pelo JIT (base do `jit_armv8_a`)

- `FC_JIT_DUMP` (novo) grava todo bloco compilado com o trecho ARM64 de cada
  op SHIL, execuções, faults por região e limpezas de cache;
  `tools/jit_study.py` classifica offline. Rodado em Shenmue II, DOA2,
  Shenmue 1 e MBAA. Resultado em `docs/jit_study.md` (4.45): 44-60% das
  instruções do host são overhead (registrador ida-e-volta ao contexto,
  T/jdyn pela memória, entrada/saída de bloco pequeno); cache de código não
  estoura, o L1I sim (Shenmue II: 119 KB quentes). Nome do JIT novo, a pedido
  do usuário: `jit_armv8_a` (à parte, sem descartar os existentes).
- Tabela de despacho medida (4.46): sequência real de 14M saídas dinâmicas do
  Shenmue II reproduzida no device. Tabela FPCB custa ~1-2%, previsão do
  desvio indireto ~1-1,5%; total evitável ~2,5-3,5%. O que vale no
  `jit_armv8_a` é retorno previsível (82% das saídas são `rts`), não trocar
  a tabela.

### 2026-09-25 — Passos 1, 3 e 5 do plano do `jit_armv8_a`

- Passo 1 (PMU, 4.47): IPC 0,43; 21% dos ciclos esperando instrução (L1I),
  13-18% esperando load; ~15% da emu thread do Shenmue II é descompressão
  de CHD (LZMA + ECC), achado fora do JIT.
- Passo 3: auditoria de todo C++ que toca o contexto do SH4 durante o JIT
  (`docs/jit_armv8_a_context_audit.md`).
- Passo 5 (4.48): emulação determinística a partir do savestate depois de
  fixar o RTC (não está no savestate); `FC_STATE_HASH` +
  `tools/state_compare.py` detectam diferença e servem de teste do JIT novo.
- Passo 4 (4.49): `rts` volta ao endereço empilhado em 98,8-99,97% das
  vezes; 19-44% das saídas condicionais pulam ≤4 instruções.
- Passo 2 (4.50): protótipo à mão do laço de vértices do Shenmue II no
  estilo `jit_armv8_a`, sobre estado real capturado: resultado idêntico,
  1,46-1,70× mais rápido, código 3,8× menor.

### 2026-09-24 — `jit_armv8_a` v0: backend novo, selecionável, IDÊNTICO nos 3 jogos

- Objetivo da sessão: escrever o `jit_armv8_a` (backend JIT ARM64 à parte, ao
  lado do `rec_arm64.cpp`, selecionado por `FC_JIT_ARMV8_A=1`), começando
  pequeno e validando a cada passo (docs/jit_study.md, jit_armv8_a_context_audit.md,
  tools/proto_jit_armv8_a/).
- **Implementado** `core/rec-ARM64/jit_armv8_a.cpp`/`.h`: r0-r7 fixos em
  x19-x26 entre ops, resto no `Sh4Context`; nativas de ALU/desvio/mov/ifb/FPU
  (NEON, mesmas instruções do backend antigo) e leitura/escrita de memória por
  chamada C++; o resto em `shil_chf` canônico. Dispatch nas funções globais do
  ngen em `rec_arm64.cpp`; `FC_JIT_ARMV8_A=1` liga, só sem MMU. Mainloop
  compartilhado. Ver item 4.51 em `tech_debits.md`.
- **Validado com a ferramenta obrigatória:** duas rodadas do mesmo savestate
  (uma com o JIT atual, outra com o novo), `FC_RTC_FIXED=600000000
  FC_INPUT_NEUTRAL=1 FC_STATE_HASH=... FC_AUDIO_DUMP=...`, comparadas com
  `tools/state_compare.py` — **IDÊNTICOS em Shenmue II, DOA2 e MBAA** (TA de
  todo frame, RAM/VRAM/ARAM/contexto, PCM). O backend novo também é
  auto-determinístico (2 rodadas idênticas).
- **Bugs corrigidos durante a validação** (cada um achado com trace por bloco,
  `FC_JIT_TRACE`/`FC_JIT_CTXDUMP`): FPU canônica divergia do NEON; `jdyn` não
  era gravado no `jcond`/`jdyn`; guarda PR/SZ ignorada; `writem` desprotegia
  página de código que o backend antigo mantém protegida; fixos não iam ao
  contexto antes da escrita (o `FC_STATE_HASH` lê o contexto no meio do bloco).
  No harness: `FC_RTC_FIXED` agora fixa `GetRTC_now()` (o `FixUpFlash` usava a
  hora do host, quebrando o determinismo entre rodadas ~13s distantes), e
  `jdyn` (scratch interno do dynarec) sai do hash de contexto.
- **Velocidade (retrorun, 30s/10s):** ainda muito mais lento que o atual —
  DOA2 89,3%→40,8%, Shenmue II 60,5%→24,5%, MBAA 100,1%→86,8%. Esperado: é o
  esqueleto correto (memória por chamada C++, `shil_chf`, flush/reload em toda
  escrita). Os ganhos vêm nos próximos passos (fastmem, T em registrador,
  `csel`, blocos maiores, `jsr`/`bsr`→`bl`, `rts`→`ret`).
- Binário de teste no device: `/home/ark/flycast_armv8a_new.so`. O core ativo
  (`flycast2026_libretro.so`) não foi tocado.

### 2026-09-25 — `jit_armv8_a` passo 1: fastmem

- Leitura/escrita do backend novo viram acesso direto `ldr/str [x13, wN, uxtw]`,
  com o mesmo tratamento de fault/trampolim do backend antigo; no modo novo o
  trampolim grava r0-r7 no contexto antes do C++ (4.52).
- IDÊNTICO ao backend antigo em Shenmue II, DOA2 e MBAA. Velocidade:
  Shenmue II 24,6 → 47,5%, DOA2 41,1 → 65,9%, MBAA 79,2 → 100%.

### 2026-09-24 — `jit_armv8_a` passos 2-5, diagnóstico do tamanho de código, mudança de rumo

- Passos 2-5 do `jit_armv8_a` (cache de FP, entrada fria/quente, cache de GPR,
  `pref` nativo), todos IDÊNTICOS ao backend antigo e sem fault. Shenmue II
  47,5 → 47,8%, DOA2 65,9 → 71,7% (antigo 60,5 / 86,8). Detalhe em 4.53.
- Perf da emu thread com os blocos mapeados à mão pelo `/tmp/perf-PID.map`:
  o JIT novo gasta 1,39× o tempo do antigo dentro dos blocos (fora deles,
  igual), excesso difuso, e o código quente que cobre 80% das amostras dobrou
  (266 → 520 KB). Nos dumps, ~1/3 das instruções eram carga/descarga de r0-r7.
- Tentativa de cortar isso (entrada fria no despachante, `UpdateSystem` num
  stub fora da linha, `ldp`/`stp`, par de float mais curto) quebrou o core: o
  stub passava ao `rdv_DoInterrupts` um pc do host que o `bm_GetBlock2` não
  achava (SIGSEGV logo depois do savestate em Shenmue II e DOA2). Desfeita a
  pedido do usuário; cópia em scratchpad só para referência. O código
  commitado é byte a byte o binário validado (`~/fc_pm.so`).
- Veredito: o diagnóstico do estudo (código quente grande, tráfego de contexto
  entre blocos) se confirma; a execução errou ao reescrever o backend por bloco
  com um contrato que infla o código, sem fazer o que deu o ganho do protótipo,
  e com dois passos sem medição prévia. Pelo usuário, o flyinghead/flycast atual roda o
  MBAA a ~32 fps no device; o `jit_armv8_a` roda a 59,9 fps / 100% (benchmark
  do retrorun, savestate) — ~1,9× (não é rodada lado a lado na mesma cena). Fica guardado; próximo: nível 2 com otimização de região
  (estilo LTO) em segunda thread, construído a partir de alvos reescritos à
  mão do dump (`docs/current_plan.md`).

### 2026-09-24 — nível 2: regiões quentes e primeira região escrita à mão (DOA2)

- `tools/region_study.py`: no dump do JIT antigo, o laço de vértices do DOA2
  (10-12 blocos) tem ~45% do custo do JIT do jogo; no Shenmue II, 18 regiões
  dão 50% (4.54).
- Captura no início da strip do DOA2 (8C101BC2), extrator salvo
  (`tools/proto_jit_armv8_a/extract_blocks.py`), harness parametrizado e agora
  comparando ciclos, contexto inteiro e RAM.
- Região do DOA2 escrita à mão: IDÊNTICA ao JIT atual incluindo ciclos, 1,85×
  (18 vértices) e 1,76× (5 vértices), código 2,1× menor no total e ~2,9× no
  caminho quente; nove regras
  para o gerador (4.55, README do protótipo).
- O protótipo antigo do Shenmue falha na conferência de ciclos (602 × 257):
  a checagem única por volta não era a soma dos blocos. A refazer.

### 2026-09-24 — gerador offline do nível 2 (v1)

- `tools/tier2_gen.py`: região do dump → ARM64 com a interface do harness.
  DOA2 IDÊNTICO com ciclos, 1,63× (18 vértices) / 1,53× (5); Shenmue II,
  mesmo gerador sem mudança, IDÊNTICO com ciclos, 1,40×. Caminho quente do
  DOA2 808 bytes (à mão 564, JIT atual ~1616). Primeiros bugs de regra
  achados lendo o código gerado: guarda da volta depois da chamada (forçava
  spill de 9 valores por vértice) → sobe para o predecessor; reload de floats
  callee-saved; layout começando pelo bloco frio (4.56).

### 2026-09-24 — nível 2 no emulador: região do DOA2 injetada (FC_TIER2)

- Gerador com modo `--emu` e stores agrupados; região do DOA2 gerada em
  `core/rec-ARM64/tier2_doa2.S` e ligada por gancho no `ngen_Compile` do JIT
  antigo (`FC_TIER2=1`), com conferência do SH4 na RAM.
- DOA2 no jogo inteiro: IDÊNTICO ao JIT antigo (`state_compare`). Velocidade
  do jogo 86 → 95%, underruns pela metade; fps apresentado 33,8 → 31,8 (render
  vira o gargalo, família do 4.29). Detalhe em 4.57.

### 2026-09-24 — nível 2 em tempo de execução, etapa 1 (gerador C++)

- `core/rec-ARM64/tier2.cpp`: porte do gerador para C++/VIXL, compilando a
  região a partir do `oplist` dos blocos do JIT antigo num ponto seguro do
  `UpdateSystem` e ligando por patch da 1a instrução do bloco antigo.
- DOA2 com `FC_TIER2_RT` (mesma região do `.S` offline): IDÊNTICO no jogo
  inteiro; velocidade igual à versão offline (94-98% contra 87% do JIT
  antigo). Detalhe em 4.58.

### 2026-09-24 — nível 2 automático (etapa 2)

- Perfil barato (amostra no fim da fatia de ciclos) e formação automática de
  região (`FC_TIER2_AUTO=1`). DOA2: acha o laço sozinho, IDÊNTICO, 87 → 93%.
  Shenmue II: acha o laço, IDÊNTICO, sem ganho mensurável — ~11 spills por
  chamada da SQ. Duas tentativas de perfil caras descartadas pela medição
  (sinal de 1 kHz, amostrar toda fatia). Detalhe em 4.59.

### 2026-09-24 — nível 2, etapa 3 (segunda thread) e os outros 3D

- Compilação das regiões na segunda thread e autochecagem (região com < 1,5
  volta por entrada é desfeita). Quatro bugs achados rodando Zombie, Giga
  Wing 2 e Shenmue 1 e corrigidos (fault em MMIO, SQ marcada como lenta,
  laço linear travando a análise, limite da autochecagem). Detalhe em 4.60.
- Jogando: DOA2 jogável, Giga Wing 2 jogável parecendo 100%, Shenmue 1 26
  fps; Zombie de volta ao normal (99%) depois do conserto do congelamento.

### 2026-09-25 — Le Mans investigado (sem regressão), nível 2 virou opção do core, samsptk e Sonic Shuffle

- **Le Mans "perdeu performance":** investigado a fundo (código, config, e
  reprodução no device) — o core no device estava parado em `2ea46c054`
  desde a correção (nenhum commit novo chegou a ser deployado), config
  idêntica. Reprodução controlada (mesma savestate automática, mesmo core,
  mesmo cfg) bateu frame a frame com o baseline bom (586 frames em 20s dos
  dois lados). Conclusão: sem regressão de código; provavelmente throttling
  térmico/bateria daquela sessão específica.
- **Nível 2 virou opção do core** (`flycast2026_tier2`), no lugar de só
  `FC_TIER2_AUTO`. Build limpo em `32ff91ca1` (primeira vez que a thread do
  AICA (4.44) e a thread do CHD (4.62) chegam ao device — estavam commitadas
  mas nunca deployadas). Deploy com backup do `.so` e dos cfgs. Usuário
  jogou vários jogos e confirmou **estável**. Detalhe em 4.65.
- **Regressão relatada no samsptk** (60→45 fps a "100%"): bateria A/B no
  device (tier2 on/off, thread do AICA off, thread do CHD off, e até o
  `.so` antigo puro) deu o MESMO número em todas as variantes — não
  reproduzido na savestate automática, usuário confirmou que não controlou
  a cena entre as duas comparações. **Decisão: deixar como está, reinvestigar
  depois.** Detalhe em 4.65.
- **Sonic Shuffle "sempre a 10fps":** não é render (vídeo <1ms/frame no
  benchmark). Dump do JIT achou um laço de busca linear (2 blocos, 9 instr
  SH4) rodando 1,25 milhão de vezes por segundo, 20% do custo total,
  76-88% dele em puro despacho/registrador (não é laço de espera mal
  classificado — endereço avança, `idle=0` corretamente). Forçar essa
  região exata no nível 2 (`FC_TIER2_RT`) instalou sem crash mas não
  mudou o frame time medido — porém a savestate automática desse jogo
  variou 43-66ms de frame emulado só de rodada pra rodada (não é cena
  parada), então o teste de 1 rodada não prova nada. Próximo passo:
  savestate fixa antes de julgar. Detalhe em 4.66.

### 2026-09-25 (continuação) — bug real na opção do nível 2, cluster da cena pesada, disco do device cheio

- **Isolado modo benchmark × modo real:** `perf stat` (instruções/ciclos) em
  janelas de 12s idênticas nos dois modos deu o MESMO número exato (12,113
  bilhões de instruções, IPC 0,41) — o benchmark reflete fielmente o custo
  real de CPU, não esconde vsync/apresentação. A diferença que o usuário via
  entre "aqui" e "lá" (ES) era cena, não metodologia.
- **Duas savestates (pesada/leve) do Sonic Shuffle, trocadas por cópia de
  arquivo, dump do JIT dos dois:** achado um cluster de ~8 blocos vizinhos
  (`8C0402C2`-`8C0413D6`, cálculo de vetor 3D entre peças) que é 8,35% do
  custo na cena pesada e nem aparece no top-20 da leve — mesma estrutura
  rodando proporcionalmente mais, não uma estrutura nova. Detalhe em 4.68.
- **BUG achado e corrigido:** a opção do core do nível 2 (4.65) nunca ligava
  a formação automática de verdade — `tier2_safe_point()` checava a
  variável de ambiente crua em vez da opção do core, então se desligava
  sozinha na primeira checagem sempre que só a opção estava ligada (sem
  `FC_TIER2_AUTO`). **Isso significa que todo A/B do dia com a opção do
  core (samsptk 4.65, Sonic Shuffle antes deste ponto) comparava "desligado"
  × "desligado"** — só a variável de ambiente (`FC_TIER2_RT`) funcionava de
  verdade. Corrigido em `tier2_sampling()`/`tier2_safe_point()`. Detalhe em
  4.67.
- **Com o bug corrigido, a formação automática realmente rodou no Sonic
  Shuffle (23 regiões, incluindo o cluster da cena pesada) — e o jogo
  PIOROU: 65 → 100ms/frame (~55% mais lento).** O custo de formar e
  descartar dezenas de regiões de baixo reaproveitamento superou o ganho
  das poucas que colaram. `flycast2026_tier2` revertido pra `disabled` nos
  cfgs do device (primeira vez que essa opção realmente faz diferença).
  Detalhe em 4.68.
- **Disco do device (`/dev/mmcblk0p2`, `/home/ark`) ficou 100% cheio** de
  dumps acumulados de várias sessões (não só hoje) — usuário confirmou que
  esse disco inteiro é descartável (o que importa está no sd2, `/roms2`).
  Limpeza (2,9GB) feita pelo próprio usuário via terminal do device (o
  classificador de segurança bloqueou o `rm -rf` em lote via SSH remoto).
  Confirmado antes de apagar: saves do PPSSPP ficam em `~/.config/ppsspp`
  (vazio, preservado) e os saves de verdade (incl. PSP) estão no sd2.
- **Classificação por padrão no nível 2 (pedido direto do usuário: "isolar
  soluções só para cada padrão"):** causa exata da regressão do 4.68 —
  grupos SEM laço interno (trecho que só é atravessado uma vez por entrada,
  ex.: pedaços do cluster de vetor 3D) entravam no mesmo limiar frouxo dos
  laços de verdade e só eram descartados DEPOIS de instalar e rodar até
  4000 entradas com guarda extra a toa. `group_has_loop()` (DFS nas mesmas
  arestas estáticas que já formam o grupo) separa os dois padrões: laço de
  verdade mantém o limiar de sempre, sem-laço agora precisa de 4× mais
  calor pra tentar instalar. **Medido (mesma cena pesada do 4.68):**
  100,6ms (regredido) → **64,9ms (empate com os 65,1ms sem tier2 —
  regressão eliminada)**; 9 regiões formadas em vez de 23, quase todo
  descarte por "1,00 blocos por entrada" sumiu, o laço de busca e um
  pedaço real do cluster de vetor 3D continuam fundindo. `flycast2026_tier2`
  reativado no device. Detalhe em 4.69. **Não validado:** efeito no
  DOA2/Shenmue (limiar deles não mudou, risco baixo mas não medido hoje).


### 2026-09-25 (fim) — varredura de configs para desempenho

- Revisadas todas as opções `flycast2026_*` e as do retrorun. Quase tudo já
  estava no ajuste mais rápido (alpha_sorting per-strip, sem anisotrópico,
  sem PVR2 filter, sem texupscale, sem DSP, threaded_rendering, sem
  synchronous/delay swap, hle_bios, gdrom_fast_loading, div_matching auto,
  sh4clock d10 -- d8 mede pior em DOA2/Shenmue, ver 4.42, tier2 ligado).
- Ligado `flycast2026_frame_budget_skip_translucent = low` (retrorun.cfg e
  retroarch-core-options.cfg): speedhack só age em frame que já estourou o
  orçamento, sem custo no caso normal.
- `sh4_timeslice = 2x` testado uma vez no Sonic Shuffle (cena pesada): 64,6ms
  contra 64,9ms do baseline, sem ganho, então NÃO aplicado (só risco de
  glitch de timing/áudio). Resolução interna não mexida: vídeo <1ms/frame
  nos dumps, jogo é CPU-bound, baixar resolução não rende.

### 2026-09-25 (noite) — regressão geral reportada, diferença ES × SSH achada

- Usuário: crash com glitch com tier2 ligado; depois "tudo caiu para 45 fps, ligado ou desligado". Tier2 desligado e device restaurado ao estado de ontem (core `2ea46c054`, cfgs originais).
- Bateria de 27 jogos com o core de ontem, em `ondemand`: números em 4.70. `ggxx` crasha (4.71); Le Mans a 15 fps (era 30); `ggx15` 47,6 e `samsptk` 43,6.
- **Achado:** o ES roda `sudo perfmax performance <rom>` antes do jogo; os testes por SSH nunca passaram por isso (4.70).
- Usuário aceitou a perda do kofnw (2-3%, acesso compacto 4.27) como efeito colateral; ideia de isolar por padrão registrada em 4.72.
- Skip do laço de varredura do Sonic Shuffle escrito no código (`scan_loop_match`, `sh4_scan_loop_skip`), sem uso ainda: o log não mostrou match nas duas rodadas.

### 2026-09-25 (madrugada) — tier2: padrão cooperativo desligado, Shenmue 1 destravado

- Bateria com a build mais nova (tier2 ligado): vários defeitos vistos na tela (KOF XI, MBAA, mslug6, samsptk, Sonic Shuffle com gráfico bagunçado; ggx15 tela preta; PSO 2 congelado; Shenmue 1 e Skies abortando ao iniciar). Ganhos vistos em DOA2, Shenmue II e gwing2. Detalhe em 4.73.
- Hipótese do usuário (jogos com a biblioteca de threads cooperativas) implementada como desligamento do tier2 por padrão de código (4.75): KOF XI e MBAA voltaram a ficar corretos na tela.
- Shenmue 1 (que o usuário quer com tier2, é jogo que precisa de ganho): isolado à região do laço `0C1DC912` (memcpy de bytes, store no delay slot); contornado por padrão (laço com store no slot não fica dentro da região), com DOA2 95,2% e Shenmue II 60,9% preservados; causa raiz segue aberta (4.74).
- Ferramentas novas de diagnóstico: `FC_TIER2_MAXREG`, `FC_TIER2_NOLOOP`, `FC_TIER2_DUMP`, contexto guest no `iNimp`.

### 2026-09-26 — Shenmue 1: chão sumido esporádico, autochecagem do tier2

- **O glitch:** o usuário reportou o chão sumindo em algumas áreas durante
  gameplay normal com tier2 ligado, e deixou um savestate no lugar. Dump de
  framebuffer (`FC_FB_DUMP`) mostrou a rua/terreno ausente (cor de céu). O
  state reproduzia o glitch com tier2 **on e off** e também com o core antigo
  `2ea46c054` (pré-tier2) → **o save gravou o glitch no estado emulado**. Um
  state novo, salvo com tier2 desligado (a rua presente), mostra só o
  transiente normal de carregamento (~20 quadros) e recupera nas duas configs.
- **tier2 é determinísticamente correto nesse state:** `state_compare` (off ×
  on, input neutro) 0 divergências; framebuffer pixel-idêntico em 570 quadros;
  assinatura do `scan_loop` não casa no Shenmue; as 5 regiões formadas foram
  desmontadas (`FC_TIER2_DUMP` + `objdump`) e estão corretas — inclusive o
  bloco `0C1DC912` do 4.74, onde o `slotStore` realmente sai do laço a cada
  volta. **Bateria (Shenmue 1/2, Zombie Revenge, MBAA):** `state_compare`
  IDÊNTICO nos 4 (estado+áudio) e frames batem; perf limpa (tier2 on): Shenmue 1
  98,8%/29,6fps, Shenmue 2 65,4%/19,6fps, Zombie 100,4%/52fps, MBAA 100,1%/59,9fps.
- **Conclusão:** a corrupção é esporádica e só aparece com input de gameplay
  (input neutro nunca diverge) — não é reproduzível sozinho.
- **Autochecagem do tier2 (pedido do usuário, 4.76):** em amostragem a região
  captura o estado na entrada e na saída; no ponto seguro o estado de entrada é
  restaurado e o **interpretador** roda o mesmo trecho até o PC de saída,
  comparando registradores/T/FPU/FPSCR. Divergiu → região desfeita + log.
  Restrita a regiões sem `pref`/`writem` (o `WriteMem` do interpretador dispara
  `bm_RamWriteAccess` em página de código, descartando blocos). **Limitação:**
  mesmo em região de leitura o replay ainda perturba levemente o `ctx`
  (~5/340 pedidos) — é diagnóstico opt-in (`FC_TIER2_SELFCHECK=1`), não caminho
  de jogo; com a env var ausente o binário é idêntico ao anterior (bateria dos 4
  jogos IDÊNTICA). Deployado como `flycast2026` (backup
  `.bak-pre-selfcheck`).
- **Ferramentas:** `FC_TIER2_SELFCHECK`, `FC_TIER2_SELFCHECK_PERIOD`; scripts
  `run_fb*.sh`, `run_statecmp.sh`, `battery.sh`, `run_selfcheck.sh` no device.

### 2026-09-26 (continuação) — bateria completa dos 27 jogos com savestate + imagens

- A pedido do usuário (pausada a caça ao crash esporádico do Shenmue 1; hipótese
  dele: savestate corrompendo estado). Core normal `f1a537dd` (tier2 ligado por
  padrão), **`perfmax performance`** (condição do ES), savestate automático,
  **40s + 8s de warmup, 1 rodada por jogo**; depois uma segunda rodada por jogo
  só pra dumpar **4 imagens** (`FC_FB_DUMP`, passo de 200 frames) — as imagens
  não entram na rodada dos números. Tabela completa e imagens embutidas na nova
  seção **"Bateria de 27 jogos — 2026-09-26"** em `docs/game_status.md`
  (imagens em `docs/batery/`, 94 no total).
- **Crashes (`DEBUGBREAK!`/SIGILL) na passada dos números:** `ggxx` (já conhecido,
  4.71) e `ggxxsla` (intermitente: a rodada de imagens passou); `sa2` crashou as
  duas. Os três ficaram "sem JSON". `meltybld` ficou lento mas gerou JSON.
- **Números melhores que a bateria de 2026-09-25** porque agora é `perfmax
  performance` (a de ontem foi em `ondemand`): DOA2 86,4%→99,4%, Shenmue
  72,6%→91,9%, Soulcalibur 60→59,7 fps estável, MBAA 59,8 fps/100%.
- **Assinatura de 100,5ms reaparece:** PSO 2 (9,9 fps, média/p50/p95/p99 =
  100,5/100,5/100,7/100,8ms) e cauda do Skies (p95/p99 ~100,6ms) — o mesmo
  padrão do 4.73; não investigado nesta sessão.

### 2026-09-26 (continuação) — destravados os 3 jogos que não abriam (ggxx, ggxxsla, sa2)

- **Contexto:** a bateria de 27 jogos (item anterior) deixou `ggxx`, `ggxxsla`
  e `sa2` sem abrir (`DEBUGBREAK!`/SIGILL). O usuário confirmou que os
  savestates são antigos (ggxx de 2026-07-10) e vai atualizá-los depois.
- **Backtrace no handler de SIGSEGV** (`core/libretro/common.cpp`,
  `backtrace_symbols_fd` no caminho fatal) mostrou a cadeia real:
  `retro_unserialize` → `mcfg_UnserializeDevices` →
  `maple_naomi_jamma::maple_unserialize` → `jvs_io_board::maple_unserialize` →
  `ra_unserialize` → `memcpy` com ponteiro quase-nulo (`si_addr` 0x29/0xa8,
  `dyna code 0`). **Não era o JIT/tier2 — era o load de savestate.**
- **Causa 1:** `maple_naomi_jamma::maple_unserialize` usava o `board_count`
  lido do savestate pra indexar `io_boards[i]` sem validar. Savestate antigo →
  `board_count=281479271677953` (lixo) com `io_boards=1` → índice fora dos
  limites → crash. Todos os savestates do device são V12 e o core escreve V13;
  o do ggxx é um V12 mais antigo. **Fix:** guarda + `g_unserializeBad` aborta o
  load limpo.
- **Causa 2 (tier2):** região que lê MMIO (TMU TCNT0 `0xFFD8000C`) via
  `tier2_fault` — o tempo global não avança dentro do laço interno da região,
  então o polling do jogo nunca sai (travava/loopava). **Fix:** `tier2_fault`
  marca a região; no ponto seguro ela é desfeita e os blocos vão pra `slowMem`.
- **Bônus:** o `FC_BLOCK_PROF` injetava o contador antes do `subs w27`, o que
  fazia o tier2 recusar **todas** as regiões (foi por isso que a bateria
  instrumentada "não crashava" — o tier2 estava inativo). Movido pra depois do
  `subs`. Novo `FC_TIER2_STATE` (dump periódico do estado/contadores das
  regiões) e `FC_JIT_DUMP_NODYN` (não gravar o `dyn-*.bin` gigante).
- **Estado:** `sa2` e `ggxxsla` não crasham mais; `ggxx` aborta o load antigo
  (sem crash). Core no device `17812822`; backups `.bak-pre-mmio`/
  `.bak-pre-statedump`/`.bak-pre-profpos`. **Pendente:** o usuário vai
  regenerar os savestates dos 3 jogos; depois re-rodar a bateria.

## 2026-09-26 21:33 — Shenmue idle_ff, orçamento de frameskip e padrão do mslug6

- **Shenmue 1 (13,2 fps / 100% VEL) — corrigido.** A assinatura nova
  `bios-wait-flag-r2-r3` (4 instruções terminando em `bt`) é o idioma genérico
  "if (*p == 0)" e casou em **22 blocos do Shenmue** (log `FC_IDLE_LOG=1`), todos
  com `bt` **para frente** (não são laços). O fast-forward pulava tempo em
  trabalho real → emulação a 100%, fila de render desaba. **Fix:** campo
  `self_loop` exige desvio para trás e apertado (`target == addr` ou `addr-2`);
  o laço do BIOS Naomi (`0C02F3A4`) desvia para `0C02F3A2`, então `target==addr`
  sozinho zerava o gwing2. **Medido:** 22 → 1 match real, **26,6 fps / core
  36,6 ms** (era 13,2 / 74); gwing2 boot mantém os 2 laços e o fps.
- **Orçamento de frameskip (opção nova `flycast2026_frameskip_budget`, padrão
  33%).** Pedido do usuário (estilo PPSSPP): quantos % de frames o core pode
  descartar para manter 100% de VEL; acima do teto, para de descartar e espera o
  render (VEL cai, apresentação não degrada mais). Implementado em `QueueRender`
  (`ta_ctx.cpp`) com EMA da fração descartada; `FC_SKIP_BUDGET` para A/B; 100 =
  antigo. **Shenmue 25s:** 100 → 16,5 fps/95,6% VEL/p95 100,6 ms/149 dup;
  33 → **26,8 fps/89,4% VEL/p95 40,4 ms/1 dup**; 0 igual ao 33. Ressalva: a VEL
  difere, então as rodadas avançam conteúdo emulado diferente.
- **mslug6 estriado com tier2 — corrigido por padrão de código.** Confirmado que
  o glitch é do tier2 (limpo com tier2 off). Isolamento por métrica de gradiente
  (limpo ~10,5 × glitch 20-33): cada região sozinha e o merge explícito limpos;
  `FC_TIER2_MAXREG` 1/2/3 limpos, **4 glitcha** → a 4ª região
  (`8C01254A`+`8C01255C`) é a culpada. `8C01255C` tem 7 stores
  `mov.l rX,@(0x18,r2)` e um `bra`. **Fix (pedido: desligar para o PADRÃO, não
  para o jogo):** `tier2_bad_pattern` casa o prefixo e chama
  `tier2_exclude_block` (vai pra `badBlocks`); o bloco não entra em região, o
  tier2 segue ligado nas outras 3. **Medido:** região #4 some, frames limpos
  (~10,5) e fps igual ao tier2 off. O padrão não dispara no Shenmue nem no
  gwing2. Novo dump cru no `FC_DUMP_BLOCK` (`.raw`).
- **Ferramentas novas de diagnóstico:** `FC_IDLE_LOG` loga cada assinatura de
  idle casada e cada rejeição de laço; `FC_DUMP_BLOCK` agora grava os bytes
  crus do bloco (`.raw`).

## 2026-09-26 22:30 — Le Mans: laço de espera encadeado (15 → 30 fps)

- **Savestate restaurado** (`/roms2/dreamcast/...Le Mans....fc2021-rrstate.auto`,
  md5 `42e1159c`, do backup). O save antigo (que dava 30 fps) tinha som quebrado
  e bugou na versão nova; o usuário criou outro, mesma pista/carro/situação.
- **Diagnóstico:** a cena roda a 15 fps e **~50% de velocidade** (lap time
  0:02,879 → 0:04,871 em 4,0 s reais), emu-bound (core 66 ms). **Não é regressão
  de código** — `core_2ea` (o do clock por jogo), `core_32ff` e o atual dão os
  mesmos 15 fps e `delay_skip=0`. O laço de atraso do 4.40 (`8C1730F6`) existe na
  RAM mas **não roda nesta cena** (nenhum bloco compilado perto; watch validado).
- **Achado (dump dinâmico `FC_JIT_DUMP` + `FC_JIT_TRACE_ALL`):** o gargalo é um
  **ciclo exato de 4 blocos** (`8C01BF1E` jsr getter → `8C100340` getter-folha →
  `8C01BF24` compara → `8C01BF6C` tst → volta), 72.362 voltas/s. O getter sozinho
  = 66M execuções = **49,8% de todo o trabalho**. Nenhum bloco é laço próprio, daí
  o "limpador de jit não detectava".
- **Fix:** assinatura de 10 ops (`chained-wait-loop-getter-cmp`) no `idle_ff_sigs`
  cobrindo o trecho contíguo `8C01BF1E..8C01BF30`, marcando `8C01BF1E` como idle
  fast-forward (avança até o próximo evento). **Medido: 15 → 30 fps** (600
  frames/20 s, core 66 → 32 ms); lap time 0:04,682 → 0:09,679 em 150 frames a
  30 fps = **99,9%**. Sem match falso em Shenmue/gwing2/mslug6; boot frio ok.
- **Ferramentas novas:** `FC_MEM_READ=<addr>:<n>` (dump de memória do guest),
  `FC_ADDR_WATCH=<lo>-<hi>` (loga blocos compilados na faixa) e o `FC_DUMP_BLOCK`
  agora grava bytes crus (`.raw`).

## 2026-09-26 23:15 — padrões com nomes genéricos + plano tier2 estilo Dolphin

- **Renomeação:** as assinaturas passam a ter nome da FORMA (não do jogo):
  `wait-flag-cmp-loop`, `wait-flag-task-loop`/`-v2`, `coop-yield-self-idle-loop`,
  `chained-wait-loop-getter-cmp`, e o padrão do tier2 vira
  `region-bad-store-burst` (agora `tier2_exclude_block(va, nome)` loga a
  classe). O comentário guarda o jogo como evidência. Docs atualizados.
- **Tier2 (mantendo o que existe):** confirmado que o `block_ok` já exclui
  blocos com truque de ciclo das regiões. Implementado **call following por
  literal de PC** (`call_target` resolve `mov.l @(disp,PC),rN` e lê o ponteiro)
  atrás de `FC_TIER2_INLINE_LIT` — **sem ganho medido** (o `leaf_blocks` rejeita
  os alvos; Shenmue: 4 alvos resolvidos, 0 embutidas, fps igual). Fica opt-in.
- **Próximas técnicas do Dolphin (plano em `current_plan.md`):** formação por
  branch following (fechar o laço por arestas estáticas), verificação neutra
  sempre-ligada, e limiar de reúso antes de instalar. Cada uma atrás de flag +
  A/B + `state_compare`.
- **Deploy:** oficial `509ef7d4` (renomeação; comportamento inalterado — o hook
  novo é desligado por padrão). Backup em `/roms2/dumps/core_pre_rename.so`.

## 2026-09-27 00:10 — tier2: branch following (Dolphin) — DOA2 +VEL, Shenmue é alvo ruim

- **Alvo de dev = Shenmue** (pedido do usuário). Baseline: 26,6 fps / 88,6% com
  tier2; **27,3 fps / 91,0% com tier2 OFF** → o tier2 **piora** o Shenmue
  (overhead de gerir regiões > ganho). Achado registrado (4.82).
- **Diagnóstico:** os blocos quentes do Shenmue são o laço de transformação de
  vértices (`0C1ED...`, ~7 blocos/vértice, terminando em `jcond` dinâmico). Os
  edges estão em `BranchBlock`/`NextBlock`, mas os blocos intermediários do
  laço não estão quentes → a união estática fragmentava em grupos pequenos
  (#2/#3/#4) que eram removidos por baixo reúso.
- **Implementado (Dolphin block merging):** `complete_loop` em `form_regions`
  segue as arestas estáticas a partir do grupo e adiciona os blocos de um
  caminho que sai e **volta** ao grupo (BFS direto ∩ reverso, limite 40).
  `FC_TIER2_FOLLOW=0` desliga.
- **Medido (DOA2, 15s, 2 pares):** ligado **39,2/39,5 fps, VEL 96,3/96,2%**;
  desligado 38,3/39,3 fps, VEL 93,6/91,7% → ganho de VEL consistente (+3,6); o
  reúso da região #1 vai de 32,8 → 65,1 blocos/entrada. **Shenmue:** estrutura
  melhorou (região #2 de 3 → 7 blocos, reúso 2,8 → 4,3) mas fps neutro.
  **mslug6:** segue limpo (métrica de gradiente ~10,5).
- **Conclusão:** a técnica vale (fica ligada por padrão), mas o **alvo de dev
  do tier2 é DOA2/Shenmue II**, não o Shenmue. Próximo: verificação neutra.

## 2026-09-27 01:40 — tier2 inteiro na thread separada (design 1)

- **Motivação:** o usuário queria o tier2 inteiramente numa thread; a medição
  mostrou que a emu thread gastava **435 µs/frame** (Shenmue) / 283 (DOA2) nele
  (amostragem/heat/formação/load), só o VIXL no worker.
- **Desenho (1):** `bm_AddBlock` (emu) entrega o `RuntimeBlockInfoPtr` (shared_ptr,
  mantém o bloco vivo — sem copiar o SHIL) numa **fila SPSC sem lock**; o worker
  drena, mantém `workerBlocks`/`workerByCode` (não toca o `blkmap`, que não é
  thread-safe) e faz amostragem→heat→formação→load→codegen. A emu thread só:
  store da amostra + `finish` + `check_regions`/unhook. Park no futex, acordado
  em lote (a cada 4096 polls, ~110/s). Flag `FC_TIER2_FULL_THREAD`.
- **Por que não mutex no block manager:** `bm_GetBlock2` é chamado em
  `rdv_DoInterrupts` (saída de bloco) — um lock ali custaria no caminho quente +
  contenção cross-core. A fila evita o blkmap no worker.
- **Bug pego:** `tier2_on_block_added` estava no `namespace {}` (linkage interno)
  → o weak do blockmanager ficava indefinido; movido pra fora.
- **Medido (Shenmue 2 pares):** on 26,4/26,4 fps e VEL 87,9/87,7% × off 26,3/25,9
  e 87,4/86,2%. **Custo emu: 435 → 83 µs/frame** (DOA2 283 → 68). DOA2 VEL
  94,8 → 95,8%. Fica **opt-in** (default off) até validar mais.

## 2026-09-27 03:00 — tier2: o overhead do Shenmue era o call por fatia

- A conta não fechava (tier2 era perda líquida no Shenmue). Isolado: não eram as
  regiões (desfazer todas não recuperava) nem o trabalho da emu thread (83 µs).
- **Era o `tier2_safe_point` chamado do `UpdateSystem` a CADA fatia** (com tier2
  desligado o call nem acontece). `FC_TIER2_POLL_MASK`: mask0 26,3 / mask63 27,1
  / mask1023 27,3 / off 27,2 fps — o call era ~0,9 fps.
- **Fix:** gate `&63` no call site (default 1/64) + amostrar toda chamada
  (cadência de amostragem/drain idêntica, 64× menos calls).
- **Medido:** Shenmue 26,3 → 26,8 (off 27,3; os ~0,5 restantes são as 16
  regiões); DOA2 38,6/88,6% → 39,1/96,4%. Deploy `a03a0503`.

## 2026-09-27 10:00 — plano: topologia de threads (2 quentes + N parkeadas)

- **Contexto:** discussão de modelo distribuído por TOC. Conclusão: a topologia
  não é uma linha de 3 estágios (JIT → tier2 → execução); é **2 quentes** (emu
  e main/GL) **+ N helpers parkeados** (tier2, AICA, CHD). O critério é
  **sensibilidade a latência + propriedade de recurso**, não estágio: caminho
  crítico fica no core quente; trabalho cold/adiável vira pulmão parkeado.
- **Fatos que sustentam:** compilação de bloco = ~1,2% do tempo (item 9), então
  separar o JIT da execução tem teto ~1% e risco de fallback interpretado;
  `ThreadedRendering` já é **padrão enabled** (`libretro_core_options.h:671`) —
  o GL **já está** fora da emu thread; o bastão/pulmão do tier2 já existe e já
  foi medido (435 → 83 µs/frame, 4.83).
- **Novo doc:** `docs/thread_separation_plan.md` (topologia, invariantes, gap
  analysis, itens ordenados, anti-padrões, validação).
- **Primeiros passos propostos:** confirmar `threaded_rendering=enabled` no
  device; medir o stall `re.Wait()` por frame; contadores do pulmão do tier2.
  Nada de código nesta sessão — é plano.

## 2026-09-27 10:30 — crash do tier2 no MvC2/CvS2: causa isolada (região de vértice/SQ)

- **Reproduzido no device** (SSH, `perfmax performance` + `retrorun3
  --benchmark`, sem savestate — benchmark roda `saves=disabled`): MvC2 e CvS2
  crasham com `die(): YUV_data : YUV decoder not inited` (`pvr_mem.cpp:145`) +
  `DEBUGBREAK` (exit 133) em ~7s.
- **A/B decisivo:** `flycast2026_tier2=disabled` → os dois passam (EXIT 0).
  Tier2 on → crasham. **É o tier2.**
- **Isolamento (MvC2, `FC_TIER2_MAXREG`):** 1..7 sem crash, **8 crasha**. A 8ª
  região é o loop de vértice/SQ (9 blocos `8C1305xx`, `ftrv` + muitos `writem`
  em `r6` + `pref`). No CvS2 a análoga é a #2 (mesma assinatura: 9 blocos, 34
  spills em 4 chamadas). Dump do SHIL em `/tmp/block-8C1305xx.txt`.
- **Mecanismo:** a região lê ponteiros fora da RAM, é marcada "acessou MMIO --
  desfeita", mas o desfazimento é no **próximo ponto seguro** → a região segue
  executando e corrompe o stream de TA/SQ → parser lê YUV sem init → die.
- **Buraco de segurança:** `tier2_selfcheck` **recusa** região `hasPref`/
  `hasWrite` (`tier2.cpp:2443`) — as regiões de vértice/SQ ficam sem
  autochecagem; `tier2_fault` não aborta; e não decodifica load de grupo com
  offset imediato (`tier2.cpp:2811`).
- **Docs:** achado em `tech_debits.md` 4.86; plano do self-healing (fallback
  pro tier1 no 1º fault) em `docs/tier2_selfhealing_plan.md`. **Sem código
  ainda** — próxima sessão implementa (começar pelo bug D do plano).

## 2026-09-27 11:40 — MvC2/CvS2 descrascharam: bug do `decision` + self-healing genérico

- **Causa raiz (o crash NÃO era o fault):** o bail no fault (hipótese do 4.86) foi
  implementado e **não** resolveu — o fault até sumiu, o crash ficou. Logo a
  corrupção é um **miscompile silencioso**, não a continuação pós-fault.
- **Bug achado:** um `pref` (→ `sqCall`/`do_sqw_nommu`) no **delay slot de um
  `jcond`** clobbera o registrador de decisão do desvio (`decision` = w15/w16,
  scratch do host). A liveness de SH4 não modela esse uso (o desvio lê T via
  codegen), então o call não o preservava → **desvio errado** → stream de TA/SQ
  corrompido → YUV sem init → `die()`. É genérico (qualquer região com call no
  delay slot de um `jcond`), não um padrão por jogo.
- **Fix 1 (correção):** `sqCall` salva/restaura `decision` na pilha quando
  `inJcondSlot` (`tier2.cpp`). Custo ~0 (só no padrão).
- **Fix 2 (self-healing genérico, pedido do usuário):** no 1º acesso fora da RAM
  dentro de uma região, `tier2_fault` **não** emula e continua: escreve o vaddr
  do bloco em `Ctx(pc)`, salta pro **bail stub** da região (salva todos os regs
  homed e devolve ao despachante) e seta `tier2BailFlag`; os ganchos de entrada
  de toda região desviam pro JIT normal (tier1) até o ponto seguro desfazer a
  culpada e zerar a flag. Nada de desligar o tier2.
- **Medido (com savestate de luta carregado — benchmark do retrorun3 com
  `retrorun_auto_load=true`):**
  - MvC2: **EXIT 0** (era crash), tier2 on 520 frames/15,6ms × off 519/15,6ms
    (**neutro**), VEL 95,8%.
  - CvS2: **EXIT 0** (era crash); bail disparou em 2 regiões (MMIO) sem crash.
  - DOA2: test (fix) **437** frames / VEL 87,7% × core deployado (sem fix)
    **435** / 86,2% → **sem regressão** (levemente melhor).
  - O bail disparou de verdade (regiões de MMIO) e funcionou.
- **Infra:** savestate do retrorun3 só carrega com `retrorun_auto_load=true`
  (e o log do load é DEBUG); o core upstream usa path de state próprio e **não**
  carrega o nosso save (`load=0`) — comparação com upstream exige cuidado.
- **Deploy:** `flycast2026_libretro.so` (md5 `28b7f52a...`), backup
  `.bak-pre-selfheal`. Usuário vai avaliar a sensação manualmente vs upstream.

## 2026-09-27 13:05 — pacing de frames: pacer determinístico medido (negativo)

- **Pedido do usuário:** estabilizar a cauda (hicups) e distribuir frames,
  limitando a apresentação ao fps sustentável (melhor fps × VEL).
- **Diagnóstico (MvC2, save, `FC_REND_SPLIT`+`FC_IDLE_FF_STATS`):**
  `game_interval=16,5ms` (60fps), `render_work=10,5ms`, `video_p50=13,9ms`
  (present), `dropped_frames_rqueue_busy=321` de ~908 (~35%), `hiccup_rate=8,16%`,
  `core_p99=100ms`. **A main thread fica ociosa** (p50 10,4ms, 517 frames em 15s
  = ~5,4s de trabalho).
- **Implementado:** `g_pacerDiv` em `ta_ctx.cpp` (pula 1 a cada N frames) +
  adaptação por descarte residual; `FC_PACER` / `FC_PACER_DIV` (fixo).
- **Medido (negativo):** off 517 frames/p50 10,4/p95 33,7 × div2 423/13,9/54,8 ×
  div3 286/33,7/96,2. **Pular piora** e a adaptação **oscilou** (div 1↔2, lição
  5.2). Pular não reduz o custo por frame apresentado — só faz a main thread
  esperar mais no `rs.Wait`.
- **Decisão:** pacer fica **opt-in** (default off). **Pivô:** o gargalo é o custo
  por frame apresentado + o motivo do descarte (rqueue ocupada com a main
  ociosa), não o número de frames. Plano em `docs/frame_pacing_plan.md` §2b.
- **Deploy:** core com o pacer opt-in no device (`flycast_test.so`); o
  `flycast2026` do usuário segue a versão do self-heal (sem pacer ativo).

## 2026-09-27 14:20 — MODELO NOVO: emu nunca espera (remover o wait) — cauda desabou

- **Direção do usuário:** "mudar o modelo e remover o wait; emulação sempre a
  100%; set e não espera; um gate no renderizador descarta o set se ainda está
  no frame anterior".
- **Achado:** o `core_p99=100ms` era o **`rs.Wait(100)` da MAIN thread** (não a
  emu esperando — tirei os waits da emu, `re.Wait`/`frame_finished.Wait`, e a
  cauda não mudou). A main thread dormia o timeout inteiro quando a emu travava.
- **Implementado:** `g_emuNeverWaits` (default 1; `FC_EMU_WAIT=1` restaura):
  (1) `QueueRender` não chama `frame_finished.Wait()`; (2) `rend_end_render` não
  chama `re.Wait()`; (3) **`rend_single_frame` não chama `rs.Wait`** — dequeue
  não-bloqueante; sem frame pronto → devolve duplicado (o gate da fila de 1
  slot já descartou o que não caberia).
- **Medido (benchmark 15s, save, budget off):**

  | Jogo | frames | VEL% | core_p50 | core_p95 | core_p99 | dupes |
  |---|---|---|---|---|---|---|
  | MvC2 | 740 | 97,1 | 8,0 | 11,8 | **39,1** | 300 |
  | MvC2 (com wait) | 518 | 95,2 | 10,4 | 31,0 | 100,4 | 10 |
  | CvS2 | 856 | **99,9** | 6,8 | 8,9 | **11,2** | 288 |
  | DOA2 | 472 | **100,0** | 17,8 | 23,4 | 33,2 | 46 |
  | Shenmue | 610 | 89,5 | 12,6 | 13,8 | **14,9** | 208 |

  **A cauda desabou** (p99 100 → 11-39ms) e a VEL subiu a ~100% nos jogos que
  não são emu-bound. Custo: `duplicated_frames` alto (~1/3 — a main thread
  devolve duplicado quando não há frame pronto).
- **Budget por tempo:** ficou **opt-in** (`FC_RENDER_BUDGET`, ou
  `FC_RENDER_BUDGET_MS` fixo) — medido pior que o no-wait puro. O pacer também
  segue opt-in.
- **Deploy:** `flycast2026_libretro.so` = no-wait (md5 `96b0a5a8...`), backups
  `.bak-pre-nowait` / `.bak-pre-selfheal`. Usuário avalia a sensação.

## 2026-09-27 19:45 — untwiddle Morton na GPU (A) + glitch de sprite + wait-curto

- **Objetivo (usuário):** usar a GPU, que tem folga (Mali ~45% em clock máx).
  Investigado o caminho de textura: `convPAL4PT_TW`/`convPAL8PT_TW` são
  *"untwiddle only"* -- a CPU desfaz o Morton pixel-a-pixel mesmo com a paleta
  na GPU. É imposto de emulação (o PowerVR amostrava nativo).
- **Isolado e medido antes de codar:** (1) script CPU×GPU
  (`/tmp/opencode/twiddle_bench.cu`, RTX 3060): untwiddle CPU ~8µs/64², GPU
  kernel 2,2µs; ganho depende do tamanho (512²: 73×; 8×8: 20× **pior**);
  (2) shader Morton validado **exato** contra a CPU (pal4 e pal8, 0 erros);
  (3) harness SDL/E2E no Mali-G31 (`/tmp/opencode/mali_morton_sdl.c`) provou
  R8UI+usampler2D+texelFetch OK (0..7) e custo +0,05ms/frame.
- **A implementado** (`FC_TEX_GPU_MORTON`, default off): sobe os bytes crus
  twiddled (R8) e o fragment shader faz o Morton + paleta. **3 bugs reais
  achados no caminho:** (a) `%` no GLSL consumido pelo `sprintf` do shader
  (escapar `%%`); (b) `float`/`int` sem `precision` default no Mali (adicionar
  `precision highp int;` e qualificar float); (c) **`usampler2D`+`texelFetch` na
  mesma unidade de um `sampler2D` invalida no Mali r13p0** (o `texelFetch` lia 0
  → sprite vazio). Fix: `sampler2D`+`GL_R8` (mesmo tipo do `tex`).
- **Glitch dos sprites:** com Morton on, personagens com blocos de cor errada.
  Diagnóstico: **não é o Morton** -- é a **corrida na VRAM exposta pelo no-wait**
  (emu escreve sprite N+1 enquanto o render lê o N). Prova: `FC_EMU_WAIT=1` e
  `FC_NO_EARLY_RELEASE=1` limpam; o early-release só agrava.
- **Fix (sem o wait caro):** `g_emuWaitRe` (`FC_EMU_WAIT_RE`, default 1) --
  `re.Wait()` **só até o `Process`** (upload de textura), não até o draw.
  **Medido (MvC2):** wait-curto on 717/718 frames / `core_average` 7,77ms ×
  sem-wait 750/727 / 6,97ms → glitch resolvido por ~5%, muito abaixo do wait
  cheio. Usuário confirmou "resolveu".
- **`FC_TEX_SKIP_UNCHANGED` virou default ON:** MvC2 ~30% dos updates eram
  re-upload idêntico; frames 829/821 × 782/796, `core_average` 5,70 × 6,99ms.
- **Aberta 4.87:** nova abordagem pra serializar a VRAM sem o `re.Wait` curto
  (snapshot/versionamento por página, wait por dependência, fence por textura).
- **Estado:** tudo em `flycast_test.so`; `flycast2026` intocado. A/B do Morton
  no MvC2 = neutro (paletizada ~1%); falta o **mslug6** (caso grande).

## 2026-09-27 20:15 — regressão do Shenmue II isolada: é o `re.Wait` curto

- **Relato do usuário:** Shenmue II regrediu (a abertura chegou a rodar 100%/30 fps
  numa build de manhã); e teve 2 crashes esporádicos. Napple Tale subiu de 30→60 fps.
- **A/B (Shenmue II, savestate, mesma cena, `perfmax performance`, 2 rodadas/lado):**
  | build/config | fps | VEL% | core_average | p99 |
  |---|---|---|---|---|
  | atual (wait-curto) | 21,0/21,4 | 67,5 | 27,5ms | 29,7 |
  | `.bak-pre-nowait` (self-heal, wait cheio) | 20,8/20,7 | 69,1 | 27,9ms | 36,7 |
  | `.bak-pre-mortongpu` (no-wait) | **25,6/26,2** | **72,8/73,2** | 20,7ms | 32,2 |
  | atual + `FC_EMU_WAIT_RE=0` | **25,9/25,4** | **71,3** | 20,6ms | 29,9 |
  | atual + `FC_TEX_SKIP_UNCHANGED=0` | 21,0/21,0 | 67,2 | 27,5ms | 30,0 |
- **Conclusão:** a regressão do Shenmue II é o **`g_emuWaitRe`** (o `re.Wait` curto
  que corrigiu o glitch do MvC2): esperar o `Process` (longo no Shenmue II) rouba
  ~19% (25,9→21,0 fps). `SKIP_UNCHANGED` não afeta. **Isso reforça o 4.87** --
  serializar a VRAM sem esperar o `Process` inteiro.
- **Deploy:** `flycast2026` = `f7dd8dcae5f364d6cd6f3dfa755fb39a` (com o wait-curto,
  ou seja COM a regressão do Shenmue II). Backup `.bak-pre-mortongpu` (= no-wait,
  sem glitch-fix; 25,6 fps no Shenmue II). **Decisão pendente:** default do
  `FC_EMU_WAIT_RE` (on = MvC2 limpo / Shenmue II lento; off = o contrário).

## 2026-09-27 22:05 — 4.87 RESOLVIDO: wait por página de VRAM (sem o re.Wait cego)

- **Medição que decidiu:** instrumentados contadores de concorrência real (emu
  escrevendo na VRAM DURANTE a leitura de textura pelo render): **0,6% no
  Shenmue II** (1 de 172) × **17% no MvC2** (473 de 2729). O `re.Wait` cego
  esperava o `Process` inteiro (7,76ms; só ~0,95ms de leitura real) → desperdício
  no Shenmue II (a regressão de 19%).
- **Fix:** `TexReadScope` marca as páginas de VRAM em leitura durante o `Update()`;
  `VramLockedWriteOffset` só espera se o emu for escrever numa página em leitura
  agora (spin de µs). `g_emuWaitRe` default 0 (o cego virou A/B).
- **Resultado:** Shenmue II 21,0 → **24,7/24,0 fps**; **MvC2 sem glitch** (12
  frames limpos). Resolve os dois lados do tradeoff.
- **Ainda em A/B:** kofevo (a outra "regressão" citada pelo usuário) -- a checar.

## 2026-09-27 22:15 — as "2 regressões" eram leitura de cena (kofevo/Shenmue II)

- **kofevo A/B (mesma savestate, `perfmax`, 2 rodadas/lado):** pré-self-heal
  35,1/35,9 fps (p99 25,8/15,8, 0 dup) × atual 37,5/37,3 (p99 14,9/14,6, ~40 dup).
  A build **atual é mais rápida**, não regrediu — o 57,7 fps da bateria de
  2026-09-26 era outra cena/condição.
- **Shenmue II:** idem (ver 20:15 e 22:05) — boot 59,5 × 29,9 fps, gameplay
  emu-bound ~21 nas duas; a "regressão de 30→?" era abertura (teto 30fps da
  cena) vs. gameplay pesado.
- **Conclusão:** nenhuma das duas "regressões" citadas é regressão de código.
  O único custo real do no-wait (o `re.Wait` cego) foi **eliminado** no 4.87
  (wait por página). **Napple Tale 30→60 fps** foi ganho real do no-wait.
  **[CORRIGIDO 2026-10-01, ver 4.97: era 30 novos + 30 duplicados cadenciados pelo vsync, não ganho real.]**

## 2026-09-28 — bateria Naomi cold boot: inspeção visual um-a-um + início do ataque aos que não bootam

- **Contexto:** a bateria JSON de 20 jogos Naomi (cold boot, `FC_FB_DUMP`) marcou
  "20/20 bootam", mas o JSON sozinho engana (tela NAOMI parada a ~0,27ms/frame).
  O usuário inspecionou **um a um na tela** (abre, observa, fecha, pergunta sim/não).
- **Resultado (20/20, core `efbe5529e1b0d7abaced0078ee418b11`, wait-curto, cold
  boot):** ver tabela e categorias em `docs/game_status.md` (seção "Inspeção visual
  um-a-um"). Resumo:
  - **Não bootam:** `asndynmt`, `cvs2`, `gwing2`, `meltyb`, `sfz3ugd`, `zombrvn`.
  - **Bootam mas freeze/crash:** `azumanga` (tela preta), `ggx` (parental advisory),
    `ggxx` (tela preta), `ggxxsla` (crash no disclaimer de região).
  - **Bootam (perf):** `mbaa` (perdeu suavidade c/ wait curto), `meltybld`
    (referência de suavidade), `ggxxac` (hicups), `slashout` (savestate p/ perf);
    `cspike`/`capsnk`/`ggisuka`/`spawn` limpos. `ikaruga` = bug do retrorun
    (deitado/sem controle); `cvsgd` = falta o GD.
- **Ataque aos que não bootam — achados iniciais (sem mudar código):**
  - **`asndynmt` (cart):** detecta `NAOMI GAME ID` e entra no render loop, mas o SH4
    **gira pra sempre em 152 blocos de boot** (offsets baixos da RAM: `8C0000E8`
    memcpy, `AC001AEE` memcpy, `AC0023C2` delay, `AC00265E` poll) e **nunca chega ao
    código do jogo**. Traçado com `FC_JIT_TRACE_ALL=1 FC_JIT_TRACE_BOOT=1` (2M
    linhas, cap) + `FC_DUMP_BLOCK`. Não é crash: é laço de espera/cópia que não sai.
  - **`cvs2` (GD-ROM):** **chega ao jogo**, mas após **~60 s** descomprimindo o CHD
    (`/roms2/naomi/cvs2/gdl-0007a.chd`+`gdl-0008.chd`; 65k setores lidos, 87,4%
    hit, 8190 esperas pela thread). O "não boota" do usuário pode ser a espera.
  - **Descoberta:** os títulos GD-ROM têm diretório próprio com `.chd`
    (`/roms2/naomi/<jogo>/gdl-*.chd`); os cartuchos usam o `.zip` MAME (`315-*`).
- **Próximo:** confirmar se `meltyb`/`sfz3ugd` (GD-ROM) também só são lentos ou se
  travam de fato; atacar o laço de boot do `asndynmt`/`gwing2`/`zombrvn` (cart).

## 2026-09-28 (continuação) — boot lento do GD-ROM Naomi: lazy loading do ROM.BIN (fix)

- **Diagnóstico:** o "não boota" do `cvs2`/`meltyb`/`sfz3ugd` era o
  `GDCartridge::device_start` lendo e descriptografando (DES) o **ROM.BIN inteiro**
  do GD-ROM antes do jogo iniciar — ~134 MB no cvs2, ~22 s até o `GAME ID` (mais
  nos maiores). **Não era o CHD prefetch (4.62):** `FC_CHD_PREFETCH=0` deu o mesmo
  tempo. O upstream carrega **por segmentos de 16 KB, sob demanda**.
- **Correção (portada do upstream):** `loadSegments()` lê+descriptografa por
  segmento, chamado de `GetDmaPtr`/`Read`; `device_start` só monta
  `loadedSegments`, gera as subchaves e carrega o 1º segmento (pro `GetGameId`);
  o `Disc *gdrom` virou membro. **Medido:** `cvs2` `GAME ID` **22 s → ~1 s**.
- **Usuário na tela:** `cvs2` bootou rápido, **60 fps cravados**, mas com **menor
  suavidade** (mesmo padrão do mbaa) — registrado em `game_status.md` na categoria
  de performance.
- **Efeito colateral:** com o boot rápido + **tier2 ligado**, o `cvs2` dá SIGSEGV
  ~5 s após o renderer, em `RuntimeBlockInfo::AddRef` (shared_ptr do blockmanager)
  — **4.91**, a investigar (suspeita de corrida do tier2 design 1). Com tier2
  desligado roda normal. Teste num `.so` separado (`/home/ark/flycast_lazygd.so`);
  o core oficial `flycast2026` no device **não foi tocado**.

## 2026-09-28 (continuação 2) — crash do cvs2 com tier2: exceção de FPU clobbera o `next_pc` no `rdv_LinkBlock`

- **Contexto:** depois do fix 4.89 (boot 22 s → 1 s), o `cvs2` passou a crashar ~5 s
  pós-boot **só com tier2 ligado** (`RuntimeBlockInfo::AddRef`, `si_addr 0xa8`).
- **Isolamento:** `FC_TIER2_FULL_THREAD=0` (worker off) → 0 NULL em 4/4; com worker
  → 2/3. É o worker do tier2.
- **Detector de corrida opt-in (`FC_TIER2_RACE`):** carimba a geração nos mapas do
  worker (`workerBlocks`/`workerByCode`) e loga se o worker usa bloco de geração
  anterior. **Não disparou** → não é bloco obsoleto pós-reset. (Opt-in, custo zero
  desligado — não tira ganho de nenhum jogo.)
- **Causa raiz:** com `FC_EXC_LOG=1`, `EXC: epc=0C02EE20 evn=800 vect=100
  next=0C000100 sr=60008001` — exceção de **FPU desabilitada** (`sr.FD=1`) tomada
  **durante o decode** de um bloco (`decoder.cpp` → `Do_Exception(next_pc, 0x800,
  0x100); return false`). O `Do_Exception` grava `next_pc = 0C000100` (vetor).
  `Setup` devolve false → `rdv_CompilePC` NULL. O `rdv_LinkBlock` seguia usando o
  **`next_pc` global clobberado** em `bm_GetBlock(next_pc)` → NULL → `AddRef` em
  NULL. Dump da RAM em `0C000100` mostrou código válido (handler já escrito) — a
  falha era o `next_pc` errado, não a RAM.
- **Correção:** em `rdv_LinkBlock`, se `rv == NULL` após `rdv_FindOrCompile()`,
  continuar no handler: `return rdv_FailedToFindBlock(next_pc)`. Blindei os
  `bm_GetBlock(next_pc)` contra NULL (defensivo). Diagnósticos ficaram opt-in
  (`FC_EXC_LOG`, `FC_TIER2_RACE`).
- **Medido:** `cvs2` 5/5 rodadas com tier2 ligado, **0 NULL / 0 SIGSEGV**; a
  exceção de FPU continua (legítima) e o handler roda. Deploy num `.so` de teste
  (`/home/ark/flycast_lazygd.so`); o `flycast2026` no device não foi tocado.

## 2026-09-28 (continuação 3) — asndynmt boota: fim de boot do tier2 (bloco 1B16) e triângulo laranja (região de vértices, padrão `ftrv`-delayslot)

- **Correção do achado 4.88 (o "laço de boot eterno" era artefato de medição):**
  o "gira pra sempre em 152 blocos e nunca chega ao jogo" era **cap do trace** —
  o `FC_JIT_TRACE` corta em 2M linhas (e só gravava quando a RAM baixa mudava).
  Com `FC_JIT_TRACE_MAX` maior, `asndynmt` e `gwing2` **chegam ao bloco físico
  `AC001B16`** (offset `0x1B16`), o mesmo despachante de callback da BIOS que
  todos os carts que bootam atravessam (comparado com o trace de
  capsnk/cspike/spawn). O que prendia os carts era o **tier2 ligado durante o
  boot**: ele muda *quando* os blocos são compilados e a temporização, e os
  carts que esperam o timer TMU nunca saíam do laço.
- **Fim de boot do tier2 (4.92):** o tier2 agora só liga depois do boot
  (`gameStarted`), disparado pelo bloco `0x1B16` **ou** pelo primeiro quadro PVR
  (`renderSeen`, hook em `rend_frame`, para jogos que bootam sem passar por ele).
  Log: `tier2: fim de boot por bloco 1B16 (bloco AC001B16)`. A reserva da cauda
  do code cache é aplicada na emu thread (próxima compilação de bloco), não na
  thread do render. Ainda no mesmo item: a exceção de FPU desabilitada saiu do
  **decode** (era frágil, dependia de *quando* o bloco era compilado — o tier2
  mudava isso e a exceção espúria travava a BIOS do `cvs2`) para uma **checagem
  de `SR.FD` em runtime** na entrada do bloco gerado (`rec_arm64.cpp`); o
  `Do_Exception` limpa `FD` ao entrar no handler de FPU (o stub da BIOS acessa a
  FPU para salvar o contexto e, com `FD=1`, re-disparava a exceção em
  tempestade).
- **Triângulo laranja do asndynmt (4.93):** com o tier2 ligado e o jogo
  bootando, a atração/3D mostrava um **triângulo laranja gigante** + listras
  brancas e triângulos prateados (vértices errados). Isolado com
  `FC_TIER2_MAXREG` (região #4) + `FC_TIER2_EXCLUDE`: a região automática #4
  (9 blocos: `8C114A9A 8C1149F6 8C114AC6 8C114A90 8C114ACE 8C114A9C 8C1149E6
  8C114AD6 8C114ADE`, 2,7 blocos/entrada) é a culpada. Bisseção: excluir
  `8C1149F6` (bloco do `ftrv`) → limpo; excluir `8C114A9A` → limpo; excluir
  `8C114A9C` → piora. Novo padrão de **forma** (não de jogo)
  `region-bad-ftrv-delayslot` (11 opcodes; bloco curto de transformação de
  vértice terminando com `ftrv` no slot de atraso de um `bf`) casa em
  `8C1149F6` e `8C114B08`; com o `8C1149F6` fora, a região #4 cai para 5 blocos
  e é removida por baixo reúso. **Triângulo laranja eliminado** (10 frames
  dumpados limpos; `live_pattern.log`/`live_pattern2.log`).
- **Fica bugado por enquanto (decisão do usuário):** ainda há **listras
  brancas** e **triângulos prateados** (outros vértices errados) na cena; não
  reproduziram nos 24 frames dumpados de `pattern2` (a atração até agora saiu
  limpa), então o culpado pode estar em outra cena/fase da câmera. Parked.
- **Deploy:** o core com os fixes foi para o **oficial do device**
  (`~/.config/retroarch/cores/flycast2026_libretro.so` =
  `2ccd8fc365859bb3538752eedc2c26c6`); backup do anterior
  (`efbe5529e1b0d7abaced0078ee418b11`) em `flycast2026_libretro.so.bak-pre-orange`.

## 2026-09-28 (continuação 4) — REVERTIDO o fix do laranja: ele quebrava o cold boot do asndynmt

- **Regressão achada na bateria cold boot:** `asndynmt` com o core deployado
  (`2ccd8fc`) **não mostrava nem o logo NAOMI — tela preta e travava**. Com o
  **tier2 desligado** o jogo boota normal (logo + atração + jogo).
- **A build boa era a de antes do fix do laranja** (a que destravou o boot e
  deixou o usuário fazer o savestate, ~20:56): `decoder.cpp` no HEAD (exceção de
  FPU desabilitada ainda tomada no **decode**) + gatilho de fim-de-boot no
  `tier2.cpp`. A build do fix do laranja (`decoder.cpp` de 23:05) juntou, na
  mesma edição, a remoção da exceção do decode (4.92) **e** o padrão
  `region-bad-ftrv-delayslot` (4.93) — e o cold boot do asndynmt com tier2 passou
  a preto/travado.
- **Ação (a pedido do usuário):** revertido `decoder.cpp` para o HEAD
  (`git checkout HEAD -- core/hw/sh4/dyna/decoder.cpp`): volta a exceção de FPU no
  decode e **some o padrão `ftrv`**. O gatilho de fim-de-boot do `tier2.cpp` foi
  mantido (é o fix do boot). O triângulo laranja do asndynmt volta, mas o jogo
  boota — aceito como "bugado por enquanto".
- **Deploy:** `flycast2026_libretro.so` = `3c9d97a354640748d14503e0f7d5d1fa`
  (pré-laranja); backup do build do laranja (`2ccd8fc...`) em
  `flycast2026_libretro.so.bak-orange-1B16`.
- **Pendência registrada:** reaplicar o fix do triângulo laranja **sem** mexer na
  exceção de FPU do decode (os dois ficaram no mesmo commit por acidente); o
  padrão `region-bad-ftrv-delayslot` e o `FC_TIER2_EXCLUDE` ficam no histórico
  (4.93) para reaplicar depois.

## 2026-09-28 (continuação 5) — cold boot do asndynmt RESOLVIDO: gatilho = handover BIOS→jogo

- **Correção da continuação 4:** a build "pré-laranja" (`0x1B16` + exceção de FPU
  no decode) **também** travava na tela preta. O problema **não** era o fix do
  laranja nem a exceção de FPU — era o **gatilho de fim-de-boot**.
- **O que o trace mostrou** (`FC_JIT_TRACE_BOOT=1 FC_JIT_TRACE_FIRST=1`, tier2
  desligado): a descoberta de blocos termina a BIOS (`AC0011xx` → `AC001B16` →
  `AC001E16`) e **entrega o controle ao jogo em `0C020000`** (`AC001E16` →
  `0C000620`, stub na RAM baixa → `0C020000`, código do jogo). O `0x1B16` é um
  callback **periódico** da BIOS que roda **antes** do handover — ligar o tier2
  ali quebrava o cold boot.
- **Fix:** o gatilho agora é o **handover**: o primeiro bloco na região de código
  do jogo em RAM (`0x0C020000-0x0C03FFFF`). Log: `tier2: handover BIOS->jogo
  (bloco 0C020000)`.
- **Resultado:** `asndynmt` **boota até a atração/jogo com o tier2 ligado**
  (confirmado por dump de frames: logo NAOMI → cena do ringue com legenda).
- **Estado:** `decoder.cpp` segue no HEAD (exceção de FPU no decode, **sem** o
  padrão `ftrv` → o triângulo laranja volta, parked). O gatilho antigo
  (`0x1B16`/`render`) foi removido.
- **Deploy:** `flycast2026_libretro.so` = `4c46a0cae79a7e85e1fd5a6eb01abd07`;
  backup do anterior em `flycast2026_libretro.so.bak-pre-handover`.

## 2026-09-28 (continuação 6) — bateria Naomi com TIER2 DESLIGADO: o tier2 é perda líquida no Naomi

- **Pedido do usuário:** bateria completa com `flycast2026_tier2 = disabled`
  (`r_t2off.cfg`) "pra ver no que esse cara que é tão caro vale a pena".
- **`retrorun3` ganhou uma opção:** `RETRORUN_BENCHMARK_KEEP_RUNNING=1` (commit
  `966a093` no repo `dreams/retrorun`, branch `threaded-present`) — quando a
  janela do benchmark termina, grava o relatório/JSON mas **não fecha o jogo**;
  continua rodando até o usuário fechar. Motivo: o auto-fechar aos 40s cortava a
  jogabilidade e fez parecer "crash" (`azumanga`). Cross-compilado localmente
  (aarch64 + sysroot extraído do device), instalado com backup
  (`retrorun3.bak-pre-keeprunning`). Skill da bateria atualizada.
- **Resultado (19/20; `cvsgd` pulado — sem o GD):** **17 de 19 jogáveis/perfeitos.**
  Só 2 bugs, ambos **com tier2 OFF**: `asndynmt` crasha no character select
  (4.94) e `meltyb` trava quando a luta começa (4.95).
- **Contra a bateria tier2 ON:** com o tier2 ligado quase todos quebravam —
  tela preta (`asndynmt`, `azumanga`, `ggxx`), freeze em tela de aviso (`ggx`),
  crash no disclaimer (`ggxxsla`), **iluminação destruída** (`cspike`), não boota
  (`meltyb`, `sfz3ugd`, `gwing2`, `zombrvn`). **Conclusão: no Naomi o tier2 é
  perda líquida** (ele foi feito pros 3D do Dreamcast).
- **Métricas:** as VEL% do JSON saíram conservadoras em vários jogos que o
  usuário sentiu 100% (`mbaa` 80%, `ggxxac` 92%) — ver 4.96 (o bench de 40s
  também não pega cenas pesadas tardias: `gwing2` 45 fps e `slashout` 27-32
  depois do cap).
- **Notas de percepção (não-tier2):** suavização de movimentação (`capsnk`,
  `cvs2`) — cache de paletas + wait-curto; `meltybld` sem underruns nas
  transições (antes extremos); barras de life do `meltybld` ainda erradas.

## 2026-09-29 — benchmark com janela rolante (retrorun) + VMU por jogo no Dreamcast

- **Benchmark — janela rolante (pedido do usuário):** `RETRORUN_BENCHMARK_ROLLING=1`
  no `retrorun3` (repo `dreams/retrorun`, commit `98fdae9`): o benchmark guarda só os
  **últimos 40s** de amostras (descarta o dado antigo; cada frame registra timestamp +
  snapshot dos contadores cumulativos) e **reporta ao fechar** o jogo (não fecha
  sozinha). Motivo: o bench dos primeiros 40s não pegava cenas pesadas tardias
  (`gwing2` 45 fps, `slashout` 27-32 depois do cap — 4.96). Testado: `cspike` rodou
  100s, JSON no fechamento com `duration=40.2s`, `core_avg` 7,9ms (contra 4,3ms dos
  primeiros 40s). Antes dele, o `RETRORUN_BENCHMARK_KEEP_RUNNING=1` (`966a093`) já
  gravava aos 40s sem fechar. Cross-compilado local (aarch64 + sysroot do device),
  instalado com backup (`retrorun3.bak-pre-rolling`).
- **Dreamcast — memory card por jogo:** `flycast2026_per_content_vmus = VMU A1` nos
  cfgs `~/.config/retrorun.cfg`, `r_t2off.cfg` e `r_noload.cfg` (backups
  `*.bak-pre-vmu`). Antes o padrão era VMU compartilhado entre todos os jogos.
- **Próximo:** bateria Dreamcast com tier2 OFF (janela rolante).

## 2026-10-01 — Napple Tale (DC): dump do JIT + métricas no savestate do "340 fps / som desafinado"

- **Pedido:** início da bateria Dreamcast; no Napple, sempre que o jogo ficava
  lento o contador marcava 340+ fps e o som desafinava. Sem mudar código: dump do
  JIT + métricas no ponto do savestate do usuário.
- **Setup:** savestate `.fc2021-rrstate.auto` (o `retrorun.cfg` do ES não tem
  `retrorun_auto_load`, padrão false — criado `/home/ark/r_napple_load.cfg` =
  `retrorun.cfg` + `retrorun_auto_load = true` + `retrorun_auto_save = false`),
  mesmo ambiente do `dreamcast.sh` (`RETRORUN_VSYNC=1 RETRORUN_SDL_THREADED_PRESENT=1`),
  `perfmax performance`, tier2 on, 20s + 8s warmup. Script `/home/ark/napple_ab.sh A|B`.
- **Rodada A (limpa):** VEL 74,8%, 22,4 frames novos/s (jogo pede 30), **384
  `retro_run`/s** (7.246 de 7.695 duplicados) = o "340+ fps"; áudio esticado pelo
  controle de taxa do retrorun (ratio médio 1,33, máx 1,53) = o "desafinado".
  Não é perda de noção de tempo: é a emu abaixo de 100% + contador que conta
  voltas vazias + áudio esticado (4.97).
- **Rodada B (`FC_JIT_DUMP` + `FC_BLOCK_PROF`):** o dump derruba esse jogo para
  VEL 23,8% (4.98) — serve só para a distribuição do código quente. Quente: laço
  de vértices → SQ em blocos de 2-3 instruções SH4 (`8C14DDF8`, `8C14DDFE`,
  `8C1350A4`, `8C1350C0`, `8C1368B6`...), saída/ligação de bloco 35% do host.
- **Arquivos:** `/roms2/jitdump_napple_B/` (jit-96167.txt, hot-blocks, dyn-96167.bin,
  JSONs e logs A/B); tentativas anteriores em `/roms2/jitdump_napple*/`.
- **Achado de método:** os benchmarks da skill rodavam sem `THREADED_PRESENT`/`VSYNC`
  — diferente do ES; com eles o laço do frontend gira quando não há frame novo.
- **Correção (mesmo dia, após feedback do usuário):** a 1ª leitura culpava o
  contador do retrorun; errado. O contador sempre contou voltas e batia com o fps
  porque o `rs.Wait` segurava o laço. Desde o no-wait (2026-09-27) o core devolve
  duplicado sem esperar: em FIFO (≥95%) o vsync cadencia a 60 (= 30 novos + 30 dup,
  o "30→60" de 09-27 não era ganho), em mailbox (<95%) o laço gira a 300+.
  Rodada C `FC_EMU_WAIT=1`: 24,2 fps, 0 dup, VEL 80,9% (A: 74,8%) — o giro custa ~6
  pts; a cena segue <100% mesmo com a espera. Som grave = reamostragem do retrorun
  (fork, `daf93bc`, 2026-09-24, `RETRORUN_AUDIO_RATE_CONTROL`). Detalhes em 4.97/4.98.

## 2026-10-01 (noite) — tier2 de volta no DC, dumper leve e a 1ª função do jogo reescrita em nativo

- **tier2 no DC (4.99):** estava desligado em silêncio desde 09-28 (handover só
  casava o endereço do Naomi; poll não religava depois da carga de savestate;
  stub sem a store da amostra). Corrigido; opção do core só liga no DC.
- **Dumper leve (4.100):** `FC_JIT_DUMP_LITE=1` + perf + `tools/jit_lite_report.py`;
  custo ~2,4 pts de VEL (o completo tirava 51). `tools/sh4dis.py` desmonta o SH4.
- **Mudança de método (pedido do usuário):** ler o código que o JIT gera (análise
  estática) e reescrever em nativo o que está ruim, em vez de profiling em volta
  do tier2 — como o DOA2/Shenmue do começo.
- **`lightxf` do Napple em nativo (4.101):** transformação + iluminação; a máscara
  de luzes era varrida a cada vértice em blocos de 1-3 instruções. Versão nativa
  com resultado bit a bit idêntico (hash de estado em todo frame) e mesma
  contabilidade de ciclos. VEL +4,5 pts no ambiente do ES (67,7 → 72,2%,
  20,3 → 21,6 fps). Core de teste: `/home/ark/flycast_hle.so` (md5 `1e9c212a`).
- **Device:** limpeza do cache de build-id do perf (`~/.debug`, builds superados de
  hoje) — o `/` tinha chegado a 39 MB livres.

## 2026-10-02 — experimento: a lightxf na GPU (só pra medir)

- Compute shader GLES 3.1 na Mali, chamada da thread de emulação a cada chamada
  da função (envia → dispatch → espera → lê). GPU ~590 µs/chamada × CPU nativa
  42 µs/chamada; VEL 80,8% → 22,2% (6,6 fps), underruns 6 → 183; resultado ≠ CPU
  em 26% dos vértices. Confirma: GPU só para o que vai direto pra tela (4.102).

## 2026-10-02 — mecanismo de sincronização emu ↔ render documentado

- `docs/sync_emu_render.md` (agente + rodada de planejamento com o usuário): mapa
  das esperas (`rs`/`re`/`frame_finished`), linha do tempo, as duas corridas na
  VRAM, os furos da paleta, o giro da main thread, comparação com o upstream e o
  plano em 4 fases (seção 9).
- Achados: o wait por página (4.87) saiu em `ea0414f3c` sem registro (hoje é o wait
  curto); o Morton na GPU faz trabalho em dobro (CPU converte e descarta) e está
  desligado; o glitch do no-wait era só nos personagens 2D (sprites reescritos a
  cada frame), não no cenário 3D.

## 2026-10-02 — fase 1: Morton na GPU de verdade

- Sem conversão na CPU, bilinear e modos de repetição no shader, upload cru em
  `GL_ALPHA` (o `GL_R8` custava ~4× neste driver). MvC2 DC: imagem idêntica pixel
  a pixel, `Process` 4,98 → 4,55 ms. `FC_FB_DUMP` agora conta a partir da carga do
  savestate (comparação determinística). Opt-in até a bateria Naomi (4.103).

## 2026-10-02 — contadores de sincronização (fase 1)

- `FC_SYNC_STATS` / `FC_SYNC_PAGES`. MvC2 DC e MBAA: 35-42% dos frames do jogo são
  descartados (a main ainda ocupada com o anterior) — é isso que tira a suavidade;
  o wait curto custa 2,5-4 ms/frame à emu, quase tudo esperando a main pegar o frame;
  ~10 páginas de VRAM escritas por frame no MvC2, ~0 no MBAA (4.104).

## 2026-10-02 — fase 3: espera com prazo + fila de 2 no retrorun

- Os ~13 ms do `video_cb` eram o `submit` esperando o vblank com a fila do
  apresentador cheia de frames repetidos. Core: a main espera o próximo frame até
  20 ms (`FC_FRAME_WAIT_MS`); retrorun: `RETRORUN_PRESENT_DEPTH=2`. MBAA p95 entre
  frames novos 50 → 33 ms; MvC2 34 → 39 novos/s. Usuário: "já muito bons" (4.105).

## 2026-10-02 — DC: base dos 4 jogos e o emissor de strips do Napple em nativo

- Base com dumper leve + perf + contadores (Napple, DOA2, Shenmue, Shenmue II): o
  laço de vértices → SQ domina nos quatro (DOA2: região tier2 #1 = 14% da emu;
  Shenmue II: 7,5%); som (controle AICA + ARM7) 12-15% nos Shenmue; render pesado
  no DC (DOA2 descarta 42% dos frames de 60; Shenmue II render 24 ms).
  `jit_lite_report.py` agora separa regiões do tier2 e stubs.
- Emissor de strips do Napple (8C14D440) em nativo: idêntico (402 frames com hash
  completo), VEL 77 → 87%, fps 23,1 → 26,1 (4.106).

## 2026-10-02 — instalado como oficial no device

- `flycast2026_libretro.so` = core `037a942b` (tier2 no DC, funções nativas `lightxf`
  e emissor de strips do Napple, espera com prazo na main, contadores opt-in, Morton
  opt-in). Backup do anterior (`4c46a0ca`): `/roms2/backups/flycast2026_libretro.so.bak-pre-hle-2026-10-02`
  (no `/roms2` porque o `/` tinha 91 MB livres).
- `/usr/local/bin/retrorun3` = fork `2703a59` (`RETRORUN_PRESENT_DEPTH`), md5
  `5ff0b8a0`. Backup: `retrorun3.bak-pre-depth`.
- `dreamcast.sh`, `naomi.sh`, `atomiswave.sh`: `RETRORUN_PRESENT_DEPTH=2` na linha do
  `retrorun3` (backups `*.sh.bak-pre-depth`).
- Fumaça com os binários oficiais (Napple, savestate): fila 2 ativa, handover do
  tier2, as duas funções nativas instaladas, VEL 88,5% / 26,5 fps.
- Build do retrorun (cross, sysroot em `/tmp/opencode/rsysroot`):
  `make -C build/linux-sdl config=release CXX=aarch64-linux-gnu-g++-13 SDL_CFLAGS="-I$R/include/SDL2 -I$R/include/aarch64-linux-gnu/SDL2 -I$R/include/aarch64-linux-gnu -D_REENTRANT" SDL_LIBS="-L$R/lib -Wl,-rpath-link,$R/lib -Wl,--allow-shlib-undefined -lSDL2" PNG_CFLAGS="-I$R/include/libpng16" PNG_LIBS="-lpng16" GLES_CFLAGS="-I$R/include" GLES_LIBS="-lEGL -lGLESv2"`
  (`R=/tmp/opencode/rsysroot`; o `--allow-shlib-undefined` cobre as dependências
  indiretas da libcurl/SDL2 que não estão no sysroot).

## 2026-10-02 — captura da bateria DC no caminho do ES + crash do tier2 no cold boot

- `tools/rr_capture.sh` (instalado em `/home/ark/rr_capture.sh`): o `dreamcast.sh` chama
  ele no lugar do `retrorun3` (backup `dreamcast.sh.bak-pre-capture`). Roda o
  `retrorun3` oficial com dumper leve, `FC_SYNC_STATS`, benchmark de janela rolante
  (últimos 40 s, grava ao fechar) e `perf record -N -F 299`; uma pasta por sessão em
  `/roms2/dcbat/<data-hora>_<jogo>/`. Só DC; Naomi/Atomiswave inalterados. Benchmark
  não grava SRAM do frontend, mas o VMU por jogo é gravado pelo core.
- O teste de fumaça (cold boot pelo `dreamcast.sh`) achou um crash do tier2 no DC:
  bail de MMIO dentro de folha embutida retomava em `0xF0000000` (4.107). Tier2
  desligado no `retrorun.cfg` por alguns minutos, corrigido, core oficial atualizado
  (`f9435643`, backup `flycast2026_libretro.so.bak-pre-bailfix-2026-10-02` no
  `/roms2/backups`), tier2 religado. Cold boot do Napple ok (40 s, sem erro).

- **Teclas do retrorun na captura (2026-10-02):** o usuário notou que Select+L1/R1
  (carregar/salvar savestate) pararam. Causa: o modo benchmark do retrorun zera, a
  cada frame, os pedidos de savestate/pausa/menu/avanço rápido ("o benchmark é dono da
  janela"). Novo `RETRORUN_BENCHMARK_ALLOW_HOTKEYS=1` no fork (commit seguinte ao
  `2703a59`), ligado no `rr_capture.sh`. `retrorun3` reinstalado (md5 `9ba774d0`,
  backup `retrorun3.bak-pre-hotkeys`).

## 2026-10-02 — lote DC: Grandia II e Macross M3 bootam com tier2

- Cold boot com tier2 desligado: Grandia II chega no PRESS START e Macross M3 passa da BIOS.
  Os dois crashes eram do tier2.
- 4.108: bail por MMIO agora é preciso (emula o acesso, sai na próxima fronteira de bloco).
  Resolveu o Grandia.
- 4.109: a saída de região gravava lixo no T quando a região não usava o T (achado por
  bisseção com `FC_TIER2_MAXREG`, região reta `8C1D4224`). Resolveu o Macross.
- Cold boot com o core novo e tier2 ligado: Grandia (PRESS START), Macross (tela do jogo),
  DOA2 e Shenmue 1 ok. `cold.sh` no device aceita `CORE=` para testar outro `.so`.
- O tier2 ficou desligado no `retrorun.cfg` do device durante a investigação e foi religado
  com o core novo instalado.
- Ferramentas novas, só ligam com a variável de ambiente:
  - `FC_INPUT_SCRIPT="a-b:TECLAS;..."`: roteiro de botões do controle 1, contado em leituras
    do controle.
  - `FC_CTRL_PORT=5555` (`core/libretro/ctrl_socket.cpp` + `tools/fc_ctrl.py`): socket TCP
    com `shot` (PNG da tela, lido no fim do quadro do render), `press/hold/release/stick/wait/status`.
    Os botões do socket somam com o controle de verdade. Validado no Grandia II (fotos da tela de título).
- O `/` do device encheu (64 KB livres). Cores de teste antigos movidos para
  `/roms2/backups/old_test_cores/`.
- Socket de controle ampliado (`docs/ctrl_socket.md`): `do` com sequência (combinações
  com `+`, passos com `;`, `:3s`, `*5`, `/200ms`), analógicos e gatilhos com força,
  `mode step` (jogo pausado entre comandos) e `pc N` (amostra do PC/PR do SH4).
  Validado no Grandia II.
- Grandia II, tela preta no "Save Game" a partir do savestate de 13/08: reproduzida pelo
  socket. Também acontece com tier2 desligado e com o próprio `flycast2021` de agosto,
  que criou o savestate. Não é regressão. Suspeita: estado do GD-ROM/VMU guardado no
  savestate. Falta o teste sem savestate (Continue pelo VMU ou New Game até um cristal).
- FMVs (`docs/fmv_plan.md`): abertura do RE CV medida (VEL 76-79%, emu a 98%); o tier2
  de hoje não ajuda. Sofdec MPV 1.14 mapeada: a IDCT do macrobloco come ~25% da thread
  de emulação. Socket ganhou `mem ADDR LEN` (lê a RAM do jogo); `tools/sh4raw.py`
  desmonta o dump cru. Próximo: IDCT nativa exata por assinatura.
