# Plano Atual — Instrumentação de Performance do flycast2021

> O que está sendo trabalhado agora, em ordem de execução. Cada item vira uma
> entrada em `docs/tech_debits.md` quando investigado, e o resultado/decisão vai
> pra `docs/history.md` com timestamp. Origem: top-8 do `docs/profiling_plan.md`.

Status possíveis: `pendente` · `in progress` · `done` · `bloqueado`

## Pré-requisito já concluído

- [x] **done** — Build cross-compile (aarch64) do flycast2021 funcionando, sem
  crash, smoke-testado no device. (ver `docs/history.md`, 2026-09-13)

## Fila de instrumentação (ordem recomendada)

1. **done** (2026-09-13) — Disambiguado: instrumentado `rend_single_frame`/
   `rend_frame` (`core/hw/pvr/Renderer_if.cpp`) com `chrono` separando
   `rsWait`/`process`/`render`, e `rend_start_render` medindo o intervalo entre
   chamadas consecutivas (`emuThread`, proxy do custo por-frame da `emu_thread`).
   Log a cada 60 frames via `NOTICE_LOG(PVR, ...)`.
   **Resultado:** em trechos calmos, `emuThread≈33ms` domina completamente
   (`process`+`render` juntos <1ms) — a `emu_thread` (SH4/AICA/ARM7) sozinha já
   consome quase todo o orçamento de 30fps, ANTES de qualquer renderização. Em
   trechos pesados (neve), `render` sobe de ~0,65ms pra 7-16ms (cascata
   sort→`SetGPState`, seção 4.2 confirmada como fator secundário) e `emuThread`
   sobe pra 42-63ms. **Prioridade atualizada: seções 1 (SH4/dynarec) e 2
   (AICA/ARM7) do profiling_plan agora são a prioridade nº1, não mais empatadas
   com 3-4.** Ver `docs/history.md` pros números completos.
   **Bug conhecido, não corrigido ainda:** os contadores de geometria
   (`verts/idx/tr`, item 3 abaixo) saíram sempre zero nesse mesmo run — ponto de
   leitura em `rend_single_frame` precisa de correção antes do próximo teste.

2. **done** (2026-09-13) — Instrumentado `ta_parse_vdrc` (decode + 3x
   `make_index` + `fix_texture_bleeding`), `GenSorted`/`stable_sort` (nunca
   disparou — ver correção abaixo), `SetGPState` (contagem+tempo) e, após
   perceber que a config real é per-strip sorting, corrigido pra instrumentar
   `SortPParams` (o sort realmente ativo) e o `glDrawElements` dentro de
   `DrawList` (isolando submissão GPU de setup de estado).
   **Resultado, no pico (`render=25587us`):**
   - `process`≈4,1ms (decode≈2,4ms + make_index≈1,8ms) — real, mas secundário
   - `SortPParams`≈0,33ms (622 PolyParams/chamada) — irrelevante
   - `SetGPState`≈2us/chamada × 622 ≈ **1,2ms** — irrelevante
   - **`glDrawElements`≈24us/chamada × 622 ≈ 15ms — domina, ~60% do `render`
     total**
   **Conclusão: o gargalo do lado de renderização é overhead de submissão por
   draw call no driver Mali (glDrawElements), multiplicado por ~500-600 strips
   separadas por frame — não custo de CPU montando estado.** Isso é o achado
   mais acionável até agora: aponta pra **reduzir contagem de draw calls
   (batching)** como a otimização de maior potencial no lado de renderização.
   Documentado em `docs/tech_debits.md` (itens 4.1 corrigido pra "não
   aplicável", 4.2 confirmado e quantificado).
   **Correção de rota registrada:** a config real deste device usa per-strip
   sorting (`SortPParams`+`DrawList<Translucent,true>`), não per-triângulo
   (`GenSorted`/`DrawSorted`) — instrumentação inicial mirou a função errada até
   isso ser percebido pelos logs vazios de `PERF_SORT`.

3. **pendente** — Contadores de geometria por frame: `verts.used()`, `idx.used()`,
   `global_param_tr.used()`, logados por frame (ou por N frames) pra montar o
   histograma "tamanho de display list × tempo de frame".

4. **done, resultado negativo** (2026-09-13) — Teste A/B do `CheckBlock` anti-SMC:
   `block_check=false` incondicional em `core/hw/sh4/dyna/driver.cpp:234`, mesmo
   cenário de referência. **Resultado: sem efeito mensurável** — `emuThread` e
   `core_average` (26,186ms) idênticos ao baseline instrumentado. Item 1.1 do
   profiling_plan **descartado** como causa (`docs/tech_debits.md`). Código
   revertido para o original. O gargalo de ~33ms da `emu_thread` continua sem
   explicação — próximo item abaixo ataca isso diretamente.

5. **done, resultado negativo** (2026-09-13) — Separado `arm_mainloop` de
   `libAICA_TimeStep` em `core/hw/arm7/arm7.cpp:1535-1543` com dois acumuladores
   `chrono`. **Resultado: `arm_mainloop=0-1us`, `libAICA_TimeStep=0us` por
   chamada** (~44.100 chamadas/s ⇒ ~1ms total de ARM7+AICA por segundo real,
   distribuído entre ~30 frames — desprezível frente aos 33ms/frame da
   `emu_thread`). **Seção 2 (AICA/ARM7) inteira descartada** como causa
   (`docs/tech_debits.md`, itens 2.1/2.2/2.3). O gargalo é 100% SH4
   (interpretador/dynarec) + scheduler.

## Pivô após itens 4 e 5 (ambos negativos)

Com `CheckBlock` anti-SMC (§1.1) e AICA/ARM7 (§2) descartados, os candidatos
restantes da seção 1 (`UpdateSystem` ~7.400×/frame, `sh4_sched_ffts` O(n)) são do
tipo "morte por mil cortes" — nenhum isoladamente parece capaz de explicar 33ms.
Isso levanta uma hipótese nova, não prevista no `profiling_plan.md` original:
**pode não haver um "bug" único — pode ser que o Cortex-A53 @ 1,5GHz não tenha
throughput bruto suficiente pra interpretar/JIT-executar a carga de SH4 do
Shenmue a tempo de 30fps**, e a alavanca real seja qualidade do código gerado
pelo JIT (menos instruções ARM64 por opcode SH4 traduzido), não uma linha
específica errada.

**Decisão a tomar com o usuário antes de continuar:** seguir instrumentando os
itens individuais restantes (6/7/8 abaixo, todos de baixo risco de serem a causa
principal) ou pivotar pra profiling com `perf`/`simpleperf` anexado à
`emu_thread` durante a cena de referência, pra ver a distribuição real de tempo
por função dentro do interpretador/dynarec SH4 — provavelmente mais eficiente do
que continuar testando candidatos um a um.

6. **done, resultado negativo** (2026-09-14) — Instrumentado `tactx_Alloc`
   (`core/hw/pvr/ta_ctx.cpp:209`) contando alocações vindas do pool vs. `new`.
   **Resultado: só 2 `new` nas primeiras 60 chamadas (aquecimento), zero depois
   disso em toda a janela de 90s medida.** Pool de 2 nunca esgota nesta cena —
   item 3.6 descartado (`docs/tech_debits.md`).

7. **done, resultado negativo** (2026-09-14) — `sh4_sched_ffts`
   (`core/hw/sh4/sh4_sched.cpp:42`) instrumentado. **Resultado:
   `sch_list.size()≈10-11`, custo/chamada ~0-0,5us, total ~1,3ms/s** —
   descartado (`docs/tech_debits.md`).

8. **done, resultado negativo** (2026-09-14) — `TexCache::CollectCleanup`
   (`core/rend/TexCache.h:787`) e o loop de refresh de uniforms
   (`core/rend/gles/gles.cpp:868-879`) instrumentados. **Resultado: ambos
   crescem com o tamanho do cache/nº de shaders ao longo da sessão (1→20us e
   11→50-60us respectivamente), mas mesmo no pico são irrisórios.** Descartados
   (`docs/tech_debits.md`).

## Todos os 8 itens da fila original concluídos (2026-09-14)

Ver `docs/tech_debits.md` pra tabela completa. Resumo: **só o item 4.2
(overhead de `glDrawElements` × contagem de draw calls) sobreviveu como causa
confirmada e quantificada** de agravamento em cena pesada (~15ms dos ~25ms de
`render`). Todos os outros 7 itens (checkBlock anti-SMC, AICA/ARM7, TA_context
pool, sh4_sched_ffts, TexCache cleanup, shader uniform loop, GenSorted) foram
testados individualmente e descartados por medição direta.

## Item 9 (novo, fora da fila original): throughput real do SH4 + compilação de bloco — CONCLUÍDO (2026-09-14)

Empacotados num único ciclo de build+teste (a pedido do usuário, pra não
gastar um ciclo de ~10min por item): (a) instrumentado `rdv_CompilePC`
(`core/hw/sh4/dyna/driver.cpp:234`, item 1.6 da fila original) e (b) uma
medição NOVA de throughput real — `SH4_SPEED_RATIO`, piggyback em
`sh4_sched_ffts` (`core/hw/sh4/sh4_sched.cpp`), comparando ciclos SH4
simulados (`sh4_sched_now64()`) por segundo real vs. `SH4_MAIN_CLOCK` (200MHz).

**Resultado:**
- Compilação de bloco: ~1,2% do tempo total (1,95s de 159s), sem thrashing de
  code cache (`clearCacheCount` parado em 2). Real, mas não dominante.
- **`SH4_SPEED_RATIO`≈0,98-1,0 em trechos calmos (tempo real correto), mas cai
  pra 0,69-0,74 em trechos pesados** — o Cortex-A53 genuinamente não consegue
  simular todas as instruções SH4 que o jogo precisa executar quando a cena
  fica complexa. **Não é bug — é limite de throughput do host durante picos de
  carga do próprio jogo.**

**Isso fecha a investigação candidato-a-candidato.** Ver a seção "Conclusão
final" em `docs/tech_debits.md` pro resumo completo: dois problemas reais e
independentes (draw calls demais no lado de renderização + throughput de SH4
insuficiente em cena pesada no lado de CPU), nenhum "vilão único" escondido.

## Decisões em aberto (perguntar antes de assumir)

- Qual mecanismo de log/timer usar no flycast2021 pra essa instrumentação: portar
  um subconjunto do `fc_profiler` do flyinghead/flycast atual (mais reutilizável,
  mais trabalho) vs. um logger simples ad-hoc só pra este projeto (mais rápido,
  descartável). Ainda não decidido.
- Cena/savestate de referência pra todos os testes A/B: usar a intro real
  (águia + neve, ~100s warmup necessário) como já validado, ou tentar conseguir um
  savestate salvo exatamente no início da neve pra encurtar os testes (o usuário já
  indicou que consegue gerar esse savestate jogando manualmente, se pedido).

## Item 10 (novo, 2026-09-14 sessão seguinte) — Regressão em 2D com regalloc estendido (item 4.9)

**pendente** — Usuário reportou (jogando interativamente, sem benchmark formal)
que o regalloc ARM64 estendido (`S16-S31`, ver item 4.9 em `tech_debits.md`)
melhorou pouco jogos 3D (consistente com +5,5% já medido em Shenmue) mas
**piorou jogos 2D (MBAA)**. Hipótese de causa (não confirmada): `GenCallRuntime`
agora faz push/pop de pares de D-regs em toda chamada, inclusive `UpdateSystem`
(chamado ~7.400x/frame, item 1.2) e slow-paths de `ReadMem*`/`WriteMem*` — custo
pago em todo bloco SH4, mas só compensado nos blocos com muita pressão de
registrador (típico de 3D, não de 2D com blocos curtos/muito branch).

**Decisão a tomar com o usuário antes de agir:**
1. Conseguir ROM/caminho do MBAA pra rodar `retrorun3 --benchmark` (mesmo
   protocolo do `bench_mine.sh`) e ter número real antes/depois no 2D.
2. E/ou instrumentar contagem de `GenCallRuntime`/frame + nº médio de regs
   empurrados por chamada, comparando MBAA vs Shenmue, pra confirmar a
   hipótese acima antes de decidir entre reverter, ajustar o tamanho do pool
   estendido, ou uma correção mais cirúrgica (ex.: não estender regalloc em
   blocos pequenos, ou evitar salvar regs que não atravessam a chamada).

**done (2026-09-14, mesma sessão)** — Trocado MBAA por **kofnw** (KOF Neowave,
Naomi, 2D, já tinha savestate pronto). Benchmark de 30s (5s warmup) rodado com
o binário do Gemini já deployado (regalloc estendido): `core_average=12,67ms`,
`video_average=8,20ms`, ~47,9fps médio. Ver `docs/history.md` pro detalhe
completo, incluindo a descoberta de infra (`retrorun_auto_load = true` no
`.cfg` era o que faltava pra carregar o auto-savestate em benchmark — não era
o modo benchmark que bloqueava).

**pendente, próximo passo imediato** — Rebuild local revertendo
`arm64_regalloc.h`/`rec_arm64.cpp` pro estado pré-item-4.9 (8 floats físicos,
sem `PushCallerSaved`/`PopCallerSaved`), deploy no device, e repetir o
EXATO mesmo comando/savestate/duração pra ter o par comparável (baseline vs
Gemini) no kofnw. Só com os dois números lado a lado dá pra confirmar (ou
não) a regressão relatada e quantificá-la.

## Fila de ataque priorizada (2026-09-14, pós-perf limpo + combate real)

Consolidação de tudo que foi medido até agora (candidato-a-candidato + `perf`)
numa lista única, ranqueada por impacto medido/estimado. **Item 0 concluído**
— recapturado `perf` no binário limpo (sem instrumentação `chrono`) e sem
precisar o usuário jogar: o savestate do kofnw já começa com a IA adversária
soltando um especial elaborado logo de cara (determinístico), só precisou
iniciar a captura logo após o load em vez de esperar 8s. Números abaixo já
são confiáveis. Ver `docs/history.md` pra tabela comparativa completa.

### 0. ~~Pré-requisito~~ — CONCLUÍDO (2026-09-14)
`perf` recapturado limpo + combate real. Achado principal: `SH4_TCB`
continua dominando (~40-51% self-time) mesmo em combate ativo, não só idle —
ver item 1 abaixo, que foi REORDENADO pra topo da fila por causa disso.

### 1. `SH4_TCB` dominante — DIAGNOSTICADO, fix v1 REVERTIDO, fix v2 CORRIGIDO (ganho pequeno)
**Concluído (2026-09-14):** não é busy-wait/idle. Via `perf`+gdb ao vivo
(dump de `/proc/pid/mem` + desmontagem + breakpoint lendo endereço/dado
reais), confirmado que o que domina é enchimento de Store Queue —
8+8 palavras (`MOV.L` individuais) escritas antes do `PREF`, cada uma
pagando uma chamada `GenCallRuntime`→`_vmem_WriteMem32` completa pra fazer
o equivalente a um store num buffer fixo. Confirmado com `cycles` vs
`instructions` sampling que é trabalho real (fica MAIS dominante em
instructions, não menos — não é stall). Ver item 1.7 em `tech_debits.md`.

**Fix v1 (2026-09-14, revertido):** checagem inline em `GenWriteMemoryFast`
(detecta endereço de SQ, grava direto, pula a chamada). Piorou ~6%
(`core_average` 12,07→12,76-12,84ms) porque a checagem roda em todo write
de 32 bits, não só SQ — revertido.

**Fix v2 (2026-09-15, aplicado):** inspirado no backend x86 (stubs
compartilhados gerados uma vez + patch só do call target, ver
`docs/x86jit.md`/`docs/arm64jit_improvement_plan.md` item 1). Só mexe em
`ngen_Rewrite` (roda uma vez por call site, só após fault real) — detecta
fault de SQ pelo endereço real (`host_context_t::x0`/`ctx.x0`, novo) e
redireciona aquele call site pra um stub compartilhado (`sq_write_stub`,
`GenMemStubs()`). `GenWriteMemoryFast` fica intocado — zero custo no
caminho comum. **Medido (`retrorun3 --benchmark 60 --benchmark-warmup 15`,
2 rodadas/lado, mesmo savestate, só trocando `.so`):** kofnw `core_average`
-2,0% (11,40→11,17ms), Metal Slug 6 `core_average` -1,4% (21,30→21,00ms),
`core_frames`/60s +0,8~1,0% nos dois. Ganho real e reprodutível (mesma
direção nas 2 rodadas), sem crash, sem regressão. **Mas pequeno, e
concentrado no p50 — a cauda pesada (p95/p99) não melhora
proporcionalmente**, então isso NÃO explica o gap grande que o usuário
reportou em Metal Slug 6 vs PPSSPP na mesma cena (10-15fps vs liso).
Deployado no device como binário ativo. Ver item 1.7 em `tech_debits.md`
pra números completos. **Próximo suspeito pra explicar a cauda pesada:
renderização/draw calls em cenas com muitos sprites, não memória.**

### 2. Batching de draw calls (item 4.2, tech_debits.md) — maior impacto confirmado do lado de render
`glDrawElements` custa ~15ms dos ~25ms de `render` em cena pesada do Shenmue
(3D), por causa de ~500-600 draw calls/frame fragmentados. **Já confirmado e
quantificado, pronto pra atacar.** ⚠️ Ver risco conhecido em `tech_debits.md`
item 4.2 — uma tentativa anterior do Gemini mexendo perto disso ganhou
velocidade mas quebrou a renderização dos personagens no Shenmue; validar
visualmente, não só por FPS.

### 3. Decisão sobre o regalloc estendido (item 4.9) — dado limpo já em mãos
Número final limpo: Shenmue (3D) +5,5% fps / kofnw (2D) core_average +3,2%,
frames -2,4%. Opções: reverter, manter (aceitar a troca), ou tentar reduzir o
pool de `S16-S31` pra um meio-termo que preserve parte do ganho 3D com menos
tax em 2D. Decisão do usuário.

### 4. Slow-path de escrita de memória (`_vmem_WriteMem32`, `do_sqw_mmu`+`WriteMemBlock_nommu_sq`)
**Confirmado com número limpo:** ~3,5% dos ciclos totais, estável entre a
captura idle (1,89%+1,64%) e a captura em combate real (1,87%+1,66%) — não é
artefato de instrumentação nem de cena parada. Achado NOVO, não estava em
`tech_debits.md`. Consistente com a hipótese do item 4.9 sobre jogos 2D
baterem mais em I/O mapeado em memória (paleta/VRAM/PVR regs) via o slow-path
de `GenCallRuntime`. Pronto pra decidir se vale otimizar o fast-path desses
acessos.

### 5. AICA/ARM7 em jogos 2D (`AicaUpdate`, `AICA_Sample32`, `ARM7_TCB`)
**Corrigido com número limpo:** ~5,6% no profile em combate real sem o
confound da instrumentação (era ~7,6% antes, inflado pela própria
instrumentação `chrono` que rodava dentro de `aicaarm::run`). Ainda maior que
"desprezível" (item 2.3 de `tech_debits.md` foi medido só no Shenmue, onde
era ~1ms/s) — mas bem menor do que a primeira leitura sugeria. Vale reabrir
item 2.3 especificamente pra jogos 2D/audio-heavy, com prioridade moderada
(não é mais candidato a "achado enorme", é um ~5,6% real).

### 6. Throughput bruto do SH4 em cena pesada (Shenmue) — não é bug, é teto de hardware
`SH4_SPEED_RATIO` cai a 0,69-0,74 em cena pesada (neve) — confirmado, mas só
mitigável via qualidade geral do código gerado pelo JIT (menos instruções
ARM64 por opcode SH4), não uma linha específica. Prioridade mais baixa por
ser esforço alto/retorno incerto por unidade de trabalho, mas é o único jeito
de fechar esse gap especificamente pra Shenmue em cena pesada.

### Itens do profiling_plan.md original ainda não tocados (baixa prioridade, sem evidência nova a favor ou contra)
1.4, 1.5, 2.4-2.7, 3.5, 3.8, 4.3, 4.5, 4.7, 4.8 — ver `docs/tech_debits.md`
pra descrição de cada um. Ficam de fora da fila ativa a menos que o item 0
(recaptura `perf`) traga evidência nova apontando pra algum deles.
