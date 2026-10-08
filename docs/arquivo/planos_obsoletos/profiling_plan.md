# Plano de Profiling — flycast2021 (metallic77/flycast @ 603814c9f)

> Gerado em 2026-09-13 por auditoria estática de código (sem execução), encomendada
> especificamente para identificar onde instrumentar medição de performance antes de
> qualquer otimização. Ver `docs/tech_debits.md` para o rastreamento de status de cada
> achado e `docs/current_plan.md` para a ordem de execução ativa.

## Contexto assumido nesta auditoria

- Hardware alvo: RK3326, CPU quad-core Cortex-A53 @ 1.5GHz (aarch64), GPU Mali-G31
  (OpenGL ES 3.2, sem Vulkan utilizável).
- Medido via benchmark externo (retrorun3, instrumentação black-box na fronteira da
  API libretro): por frame, o tempo dentro de `retro_run()` ("core time") fica em
  ~26-30ms, e o tempo de apresentação/blit do frontend ("video time") fica em
  ~5-11ms. `threaded_video` é sempre false nesse frontend (present roda serializado
  após o core, não overlapped).
- `ThreadedRendering` (config::ThreadedRendering) está ativo por padrão: existe uma
  thread de emulação e uma thread de render separada, comunicando via fila/wait com
  timeout (ver `core/hw/pvr/Renderer_if.cpp`).
- Medição de %CPU por core (feita durante gameplay real, fora deste código) mostrou
  UM ÚNICO core saturando a 100% durante os trechos mais pesados de uma cena
  (partículas de neve, muita geometria), enquanto os outros 3 cores ficam ociosos —
  ou seja, o gargalo é single-thread em algum ponto da emulação de CPU (SH4/AICA) ou
  da montagem/submissão de geometria, não a GPU (GPU nunca passou de ~70% de uso, e
  um teste isolado de subir o clock da GPU não mudou o tempo de frame).
- O dynarec ARM64 usa `core/rec-ARM64/rec_arm64.cpp` + `core/rec-ARM64/ngen_arm64.S`
  (loop principal em assembly).

## Metodologia

Leitura direta de `core/libretro/libretro.cpp`, `core/nullDC.cpp`, `core/hw/sh4/*`
(interpretador, dynarec driver, block manager, scheduler), `core/rec-ARM64/*` (JIT),
`core/hw/aica/*`, `core/hw/arm7/*`, `core/hw/pvr/*` e `core/rend/*` (sorter, TexCache,
backend GLES), complementada por três sub-análises dedicadas (AICA/ARM7, PVR/TA,
rend/GLES) cujos achados foram cruzados com a leitura direta. Todos os arquivos
citados foram efetivamente lidos (não apenas grepados), exceto onde indicado como
"cross-reference via grep".

## Achado arquitetural central (contexto para todo o resto)

`retro_run()` (`core/libretro/libretro.cpp:1227-1274`) com `ThreadedRendering` ativo
faz apenas: poll de input, `rend_single_frame()` e `video_cb()`. Toda a emulação de
CPU roda numa thread separada (`emu_thread`, `libretro.cpp:217-246`) executando
`dc_run()` → `sh4_cpu.Run()` (`core/nullDC.cpp:479-482`) em loop **infinito e
ininterrupto** — é literalmente um único `while` dentro do dynarec
(`ngen_mainloop`, `core/rec-ARM64/rec_arm64.cpp:73-83`) que só sai quando o emulador
para/reseta/salva estado. Isso confirma: **o core que fica a 100% é, com altíssima
probabilidade, a `emu_thread`** (SH4 dynarec + AICA + ARM7 + bufferização crua do
TA), enquanto a thread que chama `retro_run()` gasta a maior parte do seu "core
time" (26-30ms medidos externamente) **bloqueada** em `rs.Wait(100)` dentro de
`rend_single_frame()` (`core/hw/pvr/Renderer_if.cpp:181-233`), usando um
`cResetEvent` real (mutex+condvar), não busy-wait — e só então faz o trabalho de
`Renderer::Process()` (parsing pesado do TA + sort) e `Renderer::Render()`
(submissão OpenGL ES).

Isso muda o diagnóstico esperado: **o parsing pesado do TA (decodificação de
vértice, cálculo de plano de sprite, sort de transparência, lookup/upload de
textura) NÃO roda na `emu_thread`** — ele roda na mesma thread que chama
`retro_run()`, disparado por `rend_frame()` (`Renderer_if.cpp:159-179`) →
`renderer->Process(ctx)` → `ta_parse_vdrc()` (`core/hw/pvr/ta_vtx.cpp:1545`). Isso
explica perfeitamente "GPU nunca passa de 70%, subir clock da GPU não muda nada": a
GPU fica esperando essa thread terminar de montar/ordenar os draw calls. **A
instrumentação #1 de toda esta auditoria é descobrir qual das duas threads é a que
satura o core** — o restante do relatório está organizado assumindo que ambas podem
contribuir, mas com pesos diferentes.

---

## 1. SH4 — interpretador, dynarec driver, block manager, JIT ARM64

### 1.1 [ALTO] Verificação inline de integridade (anti-SMC) executada em TODA execução de bloco compilado
**Arquivo:** `core/rec-ARM64/rec_arm64.cpp:1961-2020` (`CheckBlock`), disparado por
`core/hw/sh4/dyna/driver.cpp:234`
(`bool block_check = rbi->read_only ? false : IsOnRam(rbi->addr);`)

O que faz: para todo bloco cujo endereço esteja em RAM (ou seja, praticamente todo o
código do jogo, já que o Dreamcast carrega o executável inteiro para RAM antes de
rodar — só o BIOS roda de ROM), o JIT emite, dentro do próprio código nativo do
bloco, uma comparação byte-a-byte/word-a-word entre a cópia do opcode original
(capturada em tempo de compilação) e o conteúdo atual da memória, antes de executar
o bloco. Se mudou, invalida e recompila (`ngen_blockcheckfail`).

Por que é suspeito: isso não é um custo de compilação (raro) — é pago em toda e
qualquer execução do bloco, para sempre, proporcional ao tamanho do bloco
(`block->sh4_code_size`). Como o custo total do dynarec escala com "quantas
instruções SH4 o jogo executa" (que cresce com a quantidade de entidades/partículas/
lógica por frame — exatamente o padrão "muita geometria" do sintoma relatado), este
é um imposto universal, silencioso e proporcional ao trabalho real do jogo, embutido
em código de máquina sem símbolo de função — não aparece em profiling baseado em
`FC_PROFILE_SCOPE`, porque não há um call site em C++ para instrumentar.

Frequência esperada: por execução de bloco, ou seja, correlacionado diretamente com
"instruções SH4 executadas por segundo" (potencialmente dezenas de milhões/s).

Como instrumentar:
- **Teste A/B rápido (mais barato que instrumentação):** force temporariamente
  `block_check = false` incondicionalmente em `driver.cpp:234` e compare o tempo de
  frame na cena de neve/partículas.
- **Instrumentação real:** adicionar, em `CheckBlock` (`rec_arm64.cpp`), mais duas
  instruções ARM64 no caminho de sucesso (`blockcheck_success`) que incrementam um
  contador global `atomic<u64> g_blockChecksExecuted` e somam
  `block->sh4_code_size` a `atomic<u64> g_blockCheckBytesCompared` — reportar
  por segundo/por frame.
- **Alternativa sem tocar no JIT:** `perf record -p <tid da emu_thread> -g` (ou
  `simpleperf` no Android) e comparar a fração de samples cujo IP cai dentro do
  range `[CodeCache, CodeCache+CODE_SIZE)`.

### 1.2 [MÉDIO] Frequência de chamada de `UpdateSystem`/scheduler a partir do código JIT
**Arquivo:** `core/rec-ARM64/rec_arm64.cpp:1339-1442` (`GenMainloop`),
`SH4_TIMESLICE=448` (`core/hw/sh4/sh4_interpreter.h:57`)

O que faz: a cada 448 ciclos SH4 executados (SH4 roda a 200MHz ⇒ ~446.000
vezes/segundo ⇒ ~7.400 vezes por frame a 60fps), o código JIT salta para o label
`intc_sched`, que chama `UpdateSystem()` (runtime C++,
`core/hw/sh4/interpr/sh4_interpreter.cpp:171-178`). O corpo é barato
(`sh4_sched_next -= 448; if (<0) sh4_sched_tick(...)`), mas é uma chamada de
função real (não inline) cruzando a fronteira JIT↔C++, ~7.400 vezes por frame.

Por que é suspeito: não é O(n²) nem alocação, mas é "morte por mil cortes" — call
overhead + possível poluição de cache/branch-predictor repetido milhares de vezes
por frame.

Frequência: ~7.400×/frame (fixo, independente de carga de jogo).

Como instrumentar: contador de chamadas (incrementar dentro de `UpdateSystem()`) +
`chrono::high_resolution_clock` acumulado ao redor do corpo da função, reportado a
cada segundo.

### 1.3 [MÉDIO] `sh4_sched_ffts()` — busca linear a cada reagendamento de evento
**Arquivo:** `core/hw/sh4/sh4_sched.cpp:42-65`

```cpp
for (size_t i=0;i<sch_list.size();i++) {
    if (sh4_sched_remaining(i)<diff) { slot=i; diff=sh4_sched_remaining(i); }
}
```

O que faz: varre todos os handlers agendados (AICA, TMU×3, GD-ROM, maple, RTC,
SPG/scanline) toda vez que qualquer um se reagenda, para achar o próximo a
disparar — não é heap/priority-queue.

Por que é suspeito: `n` é pequeno (10-20 handlers), então o custo unitário é baixo,
mas é chamado de múltiplos subsistemas simultaneamente: SPG faz isso ~1x por
scanline (`spg_line_sched`, `core/hw/pvr/spg.cpp:64-170`, ~480-624×/frame), AICA faz
isso a cada `AICA_TICK` (~1.378×/s ⇒ ~23×/frame), mais TMU/maple/gdrom. Total
plausível: milhares de varreduras O(n) por frame.

Como instrumentar: contador de chamadas a `sh4_sched_ffts()` por frame + `chrono`
acumulado ao redor do `for`; logar `sch_list.size()` para confirmar a constante `n`.

### 1.4 [MÉDIO/BAIXO] `bm_GetStaleBlock` — busca linear reversa em `del_blocks`
**Arquivo:** `core/hw/sh4/dyna/blockmanager.cpp:150-165`

O(n) com `n` = blocos descartados no último segundo — em jogos com muito código
automodificável (SMC) ou com o cache de blocos sendo invalidado com frequência
(`recSh4_ClearCache`, disparado quando `emit_FreeSpace() < 16KB`), isso pode
crescer. Só é acionado no caminho de "link para bloco obsoleto" (raro em regime
estacionário, mas pode picar).

Como instrumentar: contador de chamadas + tamanho de `del_blocks` no momento da
chamada (histograma).

### 1.5 [BAIXO, condicional] MMU completo (`mmu_full_lookup`) — O(64) por acesso de memória
**Arquivo:** `core/hw/sh4/modules/mmu.cpp:338-379` (`UTLB[64]`, busca linear)

Só ativo quando `settings.dreamcast.FullMMU` está ligado (jogos WinCE/alguns Naomi).
Se ativo, vira O(64) por todo acesso de memória do jogo. Fora desse caso,
irrelevante.

Como instrumentar: logar 1x se `mmu_enabled()==true` para o jogo em teste; se sim,
contador de chamadas a `mmu_full_lookup` + tempo acumulado.

### 1.6 [BAIXO] Compilação de bloco (`rdv_CompilePC` → `dec_DecodeBlock` → `AnalyseBlock` → `ngen_Compile`/regalloc)
**Arquivos:** `core/hw/sh4/dyna/driver.cpp:190-230`, `core/hw/sh4/dyna/decoder.cpp`,
`core/hw/sh4/dyna/ssa_regalloc.h`

Custo real de compilação, mas amortizado sobre milhares de execuções por bloco em
regime estacionário — só vira hot path se houver muito SMC ou se o working set de
código exceder `CODE_SIZE` forçando `recSh4_ClearCache()` repetido.

Como instrumentar: contador de chamadas a `rdv_CompilePC`/`recSh4_ClearCache` por
segundo + `chrono` acumulado.

---

## 2. AICA (chip de som) + ARM7 (CPU de som) — mesma thread de emulação

Confirmado por build: no alvo (`HOST_CPU==CPU_ARM64`), tanto o ARM7 (`FEAT_AREC`)
quanto o DSP da AICA (`FEAT_DSPREC`) usam JIT (`core/build.h:233-246`), não
interpretador puro. O ponto de entrada único é `AicaUpdate`
(`core/hw/aica/aica.cpp:85-96`), agendado via o scheduler do SH4 a cada
`AICA_TICK=145125` ciclos SH4 = 32 amostras de áudio ⇒ ~1.378 disparos/s ⇒
~23/frame.

### 2.1 [MÉDIO] Varredura incondicional dos 64 canais AICA por lote de amostras
**Arquivo:** `core/hw/aica/sgc_if.cpp:1356-1374` (`AICA_Sample32`), `525-529`
(`StepAll`)

Não existe bitmask/lista de canais ativos — canais desligados ainda pagam a chamada
de função e os testes iniciais. Cada canal ativo, por amostra, dispara 6 chamadas
indiretas (function-pointer) por `Step()`: `StepAEG`, `StepFEG`, `StepStream`
(inclui decodificação ADPCM com número de iterações dependente do pitch,
`sgc_if.cpp:952-1007`) e `lfo.Step`.

Frequência: até 2.048 chamadas de `Step()` por invocação de `AICA_Sample32`
(~1.378×/s), escalando com número de canais realmente ativos (tipicamente 8-32 em
jogos DC).

Como instrumentar: contador "canais ativos processados" vs. "canais varridos" por
chamada; `FC_PROFILE_SCOPE_NAMED("AICA_MixChannels")` ao redor do loop de 64;
contador de iterações extras do decode ADPCM por lote de 32 amostras.

### 2.2 [MÉDIO] Fallback do interpretador ARM7 por instrução (LDM/STM multi-registro, SWI, etc.)
**Arquivo:** `core/hw/arm7/arm7.cpp:1933-1967` (emissão do fallback dentro do bloco
compilado), `362-373`+`arm-new.h` (corpo interpretado)

Instruções que o virtualizador ARM7→ARM64 não traduz nativamente (LDM/STM
multi-registro está sempre desabilitado e cai no fallback) emitem uma chamada real
(`BL arm_single_op`) a cada execução daquela instrução — pago toda vez que o
driver de som do jogo executa esse opcode, comum em prólogo/epílogo de função ARM7.

Como instrumentar: contador global incrementado no início de `arm_single_op`
(`arm7.cpp:362`), separado por opcode/classe se possível.

### 2.3 [MÉDIO] `arm_mainloop` não separável de `libAICA_TimeStep` na medição
**Arquivo:** `core/hw/arm7/arm7.cpp:1535-1543` (chamada),
`core/hw/arm7/arm64.cpp:483-527` (loop de despacho em assembly puro)

`aicaarm::run()` chama, 32× por invocação (~44.100×/s no total): `arm_mainloop`
(execução real do ARM7, pura asm, sem ponto de instrumentação interno possível) e
`libAICA_TimeStep()` (bookkeeping: 3 timers, flags de interrupção,
`update_arm_interrupts` com busca linear O(11) — `core/hw/aica/aica.cpp:42-64`).
Como estão sempre juntos na mesma chamada de fora, uma medição ingênua confunde
"tempo ARM7" com "tempo bookkeeping AICA".

Como instrumentar: separar as duas chamadas com `chrono`/timestamps individuais
dentro do loop em `arm7.cpp:1535-1543` (2 acumuladores diferentes) — **este é o
ponto mais direto para responder "quanto do core vai para ARM7 puro vs. AICA puro
vs. SH4"**.

### 2.4 [BAIXO/pontual] I/O de CD-DA disparado de dentro do loop de mixagem de áudio
**Arquivos:** `core/hw/aica/sgc_if.cpp:1385-1389`/`1466-1470` →
`core/hw/gdrom/gdromv3.cpp:73-102` (`libCore_CDDA_Sector`) →
`core/imgread/ImgReader.cpp:16-21` (`libGDR_ReadSector`)

A cada 1.176 amostras (~37,5×/s) quando uma faixa de CD-DA está tocando, o mixer
chama o leitor de imagem de disco, potencialmente fazendo I/O de arquivo dentro do
thread único de emulação. Candidato clássico a hitches periódicos — só relevante se
o jogo/cena usa música CD-DA.

Como instrumentar: `chrono` antes/depois da chamada + contador de leituras; reportar
percentil 99 (não média).

### 2.5 [BAIXO, condicional off-by-default] DSP da AICA (efeitos)
**Arquivo:** `core/hw/aica/dsp_arm64.cpp:244-460`

Se `settings.aica.DSPEnabled` (default off) estiver ligado: até 128 chamadas de
runtime por amostra de áudio, e força o desligamento do batching. Só relevante se o
usuário ligou emulação de DSP.

### 2.6 [BAIXO] `FlushCache()` do ARM7 — O(2M) por reset
**Arquivo:** `core/hw/arm7/arm7.cpp:2014-2019`, disparado por escrita em `ARMRST`

Zera uma tabela de 2.097.152 ponteiros toda vez que o jogo reseta o ARM7. Raro, mas
pode causar hitch pontual.

### 2.7 [BAIXO] Fronteira `WriteSample`/`audio_batch_cb`
**Arquivo:** `core/libretro/audiostream.cpp:10-23`

Com `ThreadedRendering=true` e `LimitFPS=true`, `audio_batch_cb()` roda dentro da
`emu_thread`, ~86×/s. O custo real fica do lado do frontend, mas é o ponto exato
onde termina "tempo do core" e começa "tempo do driver de áudio do frontend".

---

## 3. PVR / Tile Accelerator (`core/hw/pvr/`)

### 3.1 [ALTO] `ta_parse_vdrc()` — parsing completo da display list, na thread de render/main
**Arquivo:** `core/hw/pvr/ta_vtx.cpp:1545-1639`

Roda inteiro, 1x por frame renderizado, na mesma thread que chama `retro_run()`
(não na `emu_thread`). Decodifica byte-a-byte todo o stream de comandos TA
acumulado pelo jogo naquele frame, depois chama `make_index()` 3× (op/pt/tr) e
`fix_texture_bleeding()` 3×.

Por que é suspeito: consolida todo custo por-vértice/por-polígono da cena — cresce
linearmente com o número de partículas/polígonos submetidos.

Como instrumentar: `FC_PROFILE_SCOPE_NAMED` em torno da função inteira, e
sub-escopos para: (a) loop de decodificação bruta (1578-1579), (b) os 3
`make_index` (1593/1598/1602), (c) os 3 `fix_texture_bleeding` (1616-1619).

### 3.2 [ALTO] `make_index()` — custo por vértice, dominado pela lista translúcida em cenas de partículas
**Arquivo:** `core/hw/pvr/ta_vtx.cpp:1417-1495`

Por vértice: 2× `std::isnan` + 2× `fabsf` + comparações (`is_vertex_inf`,
1407-1412) + lógica de reparo de strip. Chamado 3×/frame (op/pt/tr); a lista `tr`
(onde caem sprites translúcidos de partículas) tende a dominar.

Como instrumentar: contador de vértices processados por chamada e tempo separado
por lista (op vs pt vs tr).

### 3.3 [MÉDIO, condicional a `screen_height`] `fix_texture_bleeding()` — recomputado do zero toda cena
**Arquivo:** `core/hw/pvr/ta_vtx.cpp:1497-1541`

Dois loops completos sobre vértices/índices de todo polígono texturizado, sem cache
entre frames, só ativo quando `screen_height > 480`.

Como instrumentar: logar `screen_height` na inicialização; se >480, medir tempo
das 3 chamadas (1616-1619).

### 3.4 [MÉDIO] `CaclulateSpritePlane()` — 2 divisões de float por sprite
**Arquivo:** `core/hw/pvr/ta_vtx.cpp:1220-1267`, chamado de `AppendSpriteVertexB`
(1287)

1× por sprite/partícula submetido — neve é tipicamente implementada como nuvem de
sprites translúcidos, então o número de chamadas escala diretamente com o número de
flocos.

Como instrumentar: contador global incrementado em `AppendSpriteVertexB` (1269),
reportado por frame; cruzar com tempo total de `ta_parse_vdrc`.

### 3.5 [MÉDIO] `GetTexture()` disparado por polígono/sprite com textura
**Arquivo:** chamadas em `ta_vtx.cpp:712,780,1174` → `core/rend/gles/gltex.cpp:361-390`
→ `TexCache.h:752-783` (hash lookup)

Com `STRIPS_AS_PPARAMS=1`, cada sprite/strip vira seu próprio `PolyParam`, cada um
chamando `GetTexture` de novo — cascata de lookups de hash por sprite. Já existem
contadores prontos (`TexCacheLookups`/`TexCacheHits`, `gltex.cpp:356-358`) só
faltando logá-los por frame.

### 3.6 [ALTO] `TA_context` — pool exhaustion sob backlog vira alocação de ~15MB em pleno gameplay
**Arquivos:** `core/hw/pvr/ta_ctx.cpp:209-244` (`tactx_Alloc`/`tactx_Recycle`),
`core/hw/pvr/ta_ctx.h:158,197-222` (`TA_DATA_SIZE=8MB` + `Alloc()`)

`tactx_Alloc()` usa um pool (`ctx_pool`) de no máximo 2 contextos reciclados; se
mais de 2 contextos estiverem em voo simultaneamente (exatamente o que acontece
quando o render fica para trás durante uma cena pesada — back-pressure), cai em
`new TA_context(); rv->Alloc();`, que faz ~8 malloc/posix_memalign separados
totalizando 8MB (tad) + 4MB (verts) + buffers de índice/parâmetros ≈ 13-15MB, com
falta de páginas no meio do frame. Candidato a loop de retroalimentação negativa
(fica lento → aloca mais → fica mais lento ainda).

Como instrumentar: contador incrementado no branch `if (!rv) { rv = new
TA_context(); ... }` (linha 221-225 de `ta_ctx.cpp`) + `chrono` ao redor de
`rv->Alloc()`; logar `ctx_pool.size()` por frame.

### 3.7 [BAIXO] Sincronização emu↔render (`rend_single_frame`, fila de 1 slot)
**Arquivo:** `core/hw/pvr/Renderer_if.cpp:181-233`, fila em
`core/hw/pvr/ta_ctx.cpp:109-201`

Usa `cResetEvent` real (mutex+condvar via libretro `slock`/`scond`), não busy-wait —
1-2 sincronizações por frame, não escalam com geometria. Recomendo medir mesmo
assim (é o item #1 do relatório): tempo bloqueado em `rs.Wait(100)` (linha 193) e
`re.Wait()` (linha 304) por frame.

### 3.8 [BAIXO] Busca linear em `ctx_list` / `spg_line_sched`
`core/hw/pvr/ta_ctx.cpp:246-283` (`tactx_Find`/`tactx_Pop`, O(n) com n tipicamente
1-3); `core/hw/pvr/spg.cpp:64-170` (chamado 1x/scanline, ~480-624×/frame, corpo
O(1) — custo fixo, não cresce com a cena).

---

## 4. `core/rend/` — sorting e backend GLES (thread de render/main)

Vale ler como conjunto, não pontos isolados — é a cadeia de amplificação mais forte
do relatório.

### 4.1 [ALTO] `GenSorted()` — sort por triângulo individual, recomputado do zero todo frame
**Arquivo:** `core/rend/sorter.cpp:205-366`

Decompõe cada `PolyParam` translúcido em triângulos individuais, calcula `minZ` por
triângulo e empilha em `std::vector<IndexTrig> lst` (`resize(vtx_count*4)`,
superalocação 4×). Em seguida, `std::stable_sort(lst.begin(), lst.end())` (linha
366) — em libstdc++, `stable_sort` tenta alocar um buffer temporário proporcional a
N via `operator new` internamente (não é in-place) — alocação escondida dentro da
chamada de sort, todo frame, além do O(n log n) de comparação.

Frequência: 1×/render-pass com autosort (tipicamente 1×/frame), custo ∝ nº de
triângulos translúcidos — neve/partículas caem quase sempre nessa lista.

Como instrumentar: `FC_PROFILE_SCOPE_NAMED("GenSorted")` na função inteira +
escopo isolado só na linha do `stable_sort`; contador de triângulos de entrada
(`aused`) e nº de grupos de desenho resultantes (`pidx_sort.size()`).

### 4.2 [ALTO] Efeito cascata: sort por triângulo fragmenta draw calls → `SetGPState()` pago quase por triângulo
**Arquivos:** `core/rend/sorter.cpp:191-196` (`PP_EQ`, merge pass) +
`core/rend/gles/gldraw.cpp:102-238` (`SetGPState`, chamada em 258/314)

Como a ordenação é por triângulo (granularidade fina, necessária para transparência
correta), em cenas com múltiplos materiais transparentes intercalados por
profundidade (partículas + água + HUD etc.) o merge por `PP_EQ` raramente agrupa
vizinhos — resultado: grupos de desenho de ~1 triângulo cada, cada um pagando o
custo completo de `SetGPState()` (lookup de shader via `unordered_map`, lookup de
parâmetros de textura via `std::map`, `glBindTexture`, `glBlendFunc`, `glCullFace`,
`glDepthFunc/Mask`, 2-5× `glTexParameteri`). Explicação mais plausível e concreta
para "GPU nunca passa de 70%, aumentar clock da GPU não ajuda": o gargalo é o setup
de estado por triângulo na CPU, não o desenho em si.

Como instrumentar: contador de chamadas a `SetGPState()` por lista
(opaco/PT/translúcido) por frame; histograma de "triângulos por grupo de desenho"
(`aused / pidx_sort.size()`); `FC_PROFILE_SCOPE_NAMED("SetGPState")`.

### 4.3 [MÉDIO] `std::map<GLuint, TextureParameters>` no hot path de `TexParameteri`
**Arquivo:** `core/rend/gles/glcache.h:186-215` (`_texture_params`, árvore
rubro-negra, não hash)

Chamado 2-5× por `SetGPState()` — com a fragmentação do item 4.2, potencialmente
milhares de lookups em árvore (baixa localidade de cache) por frame.

Como instrumentar: contar hits/misses reais do `if` interno vs. quantas vezes cai
no `glTexParameteri` de baixo.

### 4.4 [MÉDIO] Refresh de uniforms de TODOS os shaders já compilados na sessão, todo frame
**Arquivo:** `core/rend/gles/gles.cpp:868-879`, dentro de `RenderFrame()`

```cpp
for (const auto& it : gl.shaders) { glcache.UseProgram(it.second.program); ShaderUniforms.Set(&it.second); }
```

`gl.shaders` só cresce (nunca é podado) — este loop percorre todas as variantes de
shader já vistas na sessão inteira, não só as usadas no frame atual.

Como instrumentar: `FC_PROFILE_SCOPE_NAMED` nas linhas 875-879; logar
`gl.shaders.size()` a cada N frames.

### 4.5 [MÉDIO] `vidx_sort` local (não-`static`) — malloc/free por frame no caminho de sort
**Arquivo:** `core/rend/gles/gldraw.cpp` (`SortTriangles`, variável local
`std::vector<u32> vidx_sort`) vs. `core/rend/sorter.cpp` (`lst`/`pidx_sort` são
`static`, reaproveitados)

Diferente dos outros buffers do sorter, `vidx_sort` é recriado do zero a cada
`SortTriangles()` — malloc+free garantido todo frame com autosort ativo.

Como instrumentar: hook de `operator new`/`operator delete` filtrado por thread id
da render thread; ou teste A/B tornando a variável `static`.

### 4.6 [MÉDIO] `TexCache::CollectCleanup()` — varredura do `unordered_map` inteiro, todo frame, incondicional
**Arquivo:** `core/rend/TexCache.h:787-807`, chamado em
`core/rend/gles/gles.cpp:1088`

O `break` só ocorre ao achar 5 candidatos — se o cache tem centenas/milhares de
texturas e poucas estão "dirty há >120 frames", o loop varre o mapa inteiro todo
frame para não achar quase nada.

Como instrumentar: `FC_PROFILE_SCOPE_NAMED`; logar `cache.size()` junto com o
tempo, a cada N frames.

### 4.7 [BAIXO] Conversão de textura por-texel (`Update()`) e `PrintTextureName()`
**Arquivo:** `core/rend/TexCache.cpp:527-730` (conversão twiddled/paletted/VQ→RGBA,
O(w×h) por textura) e `381-402`+`744` (`PrintTextureName`, monta string
incondicionalmente antes de checar se o log está habilitado)

Conversão esperada/inevitável; só é hot se muitas texturas forem re-marcadas dirty
por frame. `PrintTextureName` é desperdício silencioso de baixo impacto.

**Descartado explicitamente:** `NeedsUpdate()` usa flag `dirty` via write-protect de
VRAM, não recalcula hash da textura por frame. `glCheck()`/`glGetError` é macro
vazia (`gles.h:38`). Post-processing está desabilitado no fork atual.

### 4.8 [BAIXO, esperado] Upload de VBO/IBO completo todo frame
**Arquivo:** `core/rend/gles/gles.cpp:941-956` (`glBufferData(...,
GL_STREAM_DRAW)`, técnica de orphaning)

Cópia de `pvrrc.verts.bytes()` bytes por frame — design razoável, custo cresce com
contagem de vértices da cena.

---

## 5. Ranking consolidado por probabilidade de impacto

**ALTO** (mais prováveis de explicar "1 core a 100% em cena de muita
geometria/partículas"):
1. §1.1 — Verificação inline anti-SMC em todo bloco SH4 executado
2. §4.1+§4.2 — Sort por triângulo + fragmentação de `SetGPState()`
3. §3.1+§3.2 — `ta_parse_vdrc`/`make_index`
4. §3.6 — Alocação de ~15MB sob backlog de `TA_context`

**MÉDIO:**
5. §4.3 — `std::map` em `TexParameteri`
6. §4.4 — Refresh de uniforms de todos os shaders
7. §4.5 — malloc/free de `vidx_sort` por frame
8. §4.6 — `TexCache::CollectCleanup` varrendo o cache inteiro
9. §2.1 — Varredura incondicional de 64 canais AICA
10. §2.2 — Fallback de interpretador ARM7 por instrução
11. §1.2 — Frequência de `UpdateSystem` (~7.400×/frame)
12. §1.3 — `sh4_sched_ffts` O(n) por reagendamento
13. §2.3 — Impossibilidade de separar tempo ARM7 puro de bookkeeping AICA
14. §3.3/§3.4/§3.5 — `fix_texture_bleeding`, `CaclulateSpritePlane`, `GetTexture`

**BAIXO** (raro, condicional, ou já descartado):
15. §1.4 — `bm_GetStaleBlock`
16. §1.5 — MMU completo
17. §2.4 — I/O de CD-DA
18. §2.5 — DSP da AICA
19. §2.6 — `FlushCache` ARM7
20. §2.7 — fronteira `WriteSample`/`audio_batch_cb`
21. §3.7/§3.8 — sincronização emu↔render, buscas lineares de custo fixo
22. §4.7/§4.8 — conversão de textura, upload de VBO (esperados)
23. Descartados: hash de textura por frame, `glGetError`, post-processing

---

## 6. Top 8 — ordem recomendada de instrumentação

1. **Disambiguar qual thread satura o core** — comparar TID/nome
   (`prctl(PR_SET_NAME)`) da `emu_thread` (`libretro.cpp:217`) vs. a thread que
   chama `retro_run()`. Envolver `rs.Wait(100)` (`Renderer_if.cpp:193`) e
   `re.Wait()` (`Renderer_if.cpp:304`) com timestamps para medir quanto tempo a
   thread de render fica bloqueada esperando a `emu_thread`.
2. **`FC_PROFILE_SCOPE_NAMED` em `ta_parse_vdrc`** (`ta_vtx.cpp:1545`) com
   sub-escopos para o loop de decodificação, os 3 `make_index` e os 3
   `fix_texture_bleeding`, mais `GenSorted`/`stable_sort` isolado
   (`sorter.cpp:205,366`) e `SetGPState` (`gldraw.cpp:102`) com contador de
   chamadas por lista.
3. **Contadores de geometria por frame** (`verts.used()`, `idx.used()`,
   `global_param_tr.used()`) para montar o histograma "tamanho de display list ×
   tempo de frame".
4. **Teste A/B do CheckBlock anti-SMC** (`rec_arm64.cpp:1961`, condição em
   `driver.cpp:234`).
5. **Separar `arm_mainloop` de `libAICA_TimeStep`** dentro do loop em
   `arm7.cpp:1535-1543` com `chrono`.
6. **Contador no branch de pool-exhaustion de `tactx_Alloc`**
   (`ta_ctx.cpp:221-225`) + `chrono` ao redor de `Alloc()`.
7. **`sh4_sched_ffts`** (`sh4_sched.cpp:47-54`) — contador de chamadas/frame +
   tempo acumulado.
8. **`TexCache::CollectCleanup`** (`TexCache.h:787`) e o loop de refresh de
   uniforms de todos os shaders (`gles.cpp:868-879`).
