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
