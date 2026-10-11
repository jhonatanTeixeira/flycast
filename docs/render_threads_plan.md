# Plano futuro: mais threads de render (round robin de frames)

Status: **implementado e ligado por padrão (2026-10-11)**; `FC_RT=0` volta ao render antigo (A/B) — fora do `current_plan.md`. Itens:
`tech_debits.md` 4.131 (teste de viabilidade) e 4.130 (DOA2, o caso que motiva).
Teste isolado: `tools/gl_two_threads.c`.

**Feito (etapa 2, fundação):** `core/rend/gles/rt_proto.cpp` — o core pega o
display/contexto EGL do frontend (corrente na thread principal, `eglGetCurrentDisplay`/
`GetCurrentContext` via `dlopen`, sem linkar EGL) e cria um **2º contexto EGL
compartilhado** com ele, faz corrente **sem superfície** (`EGL_KHR_surfaceless_context`,
sem pbuffer) e desenha/lê num FBO próprio. Opt-in `FC_RT=1`, chamado no `glesrend::Init`;
não toca o render normal. Testado no device (DOA2): contexto ok, FBO completo,
clear/readback correto.

**Feito (etapas 3-5, `core/rend/gles/rt_render.cpp`, `FC_RT=2`):**

- **Workers:** 2 threads, cada uma com contexto compartilhado sem superfície, 2 FBOs de
  saída, `glsm`/`glcache`/estado do render `thread_local` (`_pvrrc`, `vtx_sort_base`,
  programas compilados por worker). As funções GLES3 (fence, sampler, blit) vêm por
  `dlsym`: o build é GLES2 e os wrappers do `glsm` para elas são vazios.
- **Process serializado, render em paralelo:** o `Process` (TA → listas, sobe texturas)
  roda um de cada vez, na ordem dos frames, numa **cadeia de fences** (cada Process
  espera o fence do anterior, para ver as texturas que ele subiu). O fim do Process
  libera o slot da fila (`rt_release_after_process`): o próximo frame já pode entrar
  enquanto este desenha.
- **Texturas versionadas:** textura lida por um frame que ainda não terminou na GPU
  ganha nome GL novo **no momento do upload** (`UploadToGPU`); o nome antigo é apagado
  quando o fence daquele frame passar. Versionar antes do `Update` deixava o HUD do MvC2
  preto (o `Update` pula o upload quando os bytes da VRAM não mudaram).
- **Sampler objects por worker** no lugar de `glTexParameter` (o parâmetro é do objeto
  de textura, compartilhado entre contextos).
- **Paleta, tabela de fog e registradores do PVR:** cópia por frame (versão atômica +
  snapshot dos registradores no fim do Process).
- **RTT e framebuffer** rodam na thread principal, como hoje, entrando na cadeia de fences.
- **Apresentação:** a thread principal mostra o frame pronto **mais novo** (blit para o
  FBO do frontend) e descarta os mais velhos; frame atrasado (id ≤ o último mostrado)
  é descartado sem mostrar.
- **Afinidade:** emulação no núcleo 3, workers nos núcleos 1-2, principal em 0-2
  (`FC_RT_PIN`, ligada por padrão com 4 núcleos).
- **Validação:** `FC_RT_HASH=<arquivo>` grava o hash dos pixels por id de frame (todos os
  pedidos de render desde a carga do savestate); `tools/rt_hash_compare.py` compara.
  `FC_RT_DUMP=<id> FC_RT_DUMP_N=<n>` grava PPMs. Com `FC_RTC_FIXED=1 FC_INPUT_NEUTRAL=1
  FC_EMU_WAIT_RE=1`: **MvC2 952/952 frames idênticos, DOA2 546/546**.

**Medido (2026-10-10, savestate, `perfmax`):**

| Jogo | Modo | VEL% | fps novos | descartes | core p50/p95/p99 (ms) |
|---|---|---|---|---|---|
| MvC2 | atual | 100 | 38,8 | 34% | 19,4 / 44,5 / 51,0 |
| MvC2 | `FC_RT=2` | 100,2 | **53,4** | **0** | **13,7 / 29,7 / 33,3** |
| DOA2 | atual | 90,6 | 27,0 | 48% | 33,8 / 43,9 / 64,6 |
| DOA2 | `FC_RT=2` | 73,5 | 33,2 | — | — |
| DOA2 | `FC_RT=2` sem espera do Process | 83,5-87,8 | 30,5-31,4 | — | — |

- **MvC2 é o caso que fecha:** 60 fps de jogo quase sem descarte, com a VEL igual.
  Underruns subiram de 1 para 13 (a olhar).
- **DOA2 perde VEL:** com a espera da emulação pelo Process (`g_emuWaitRe`, padrão,
  protege a VRAM dos sprites 2D) a emulação espera mais, porque agora todo frame é
  processado (antes ~48% eram descartados sem Process). Sem a espera
  (`FC_EMU_WAIT_RE=0`) volta para 83,5-87,8%. **Falta:** o Process ler um snapshot da
  VRAM/TA em vez de a emulação esperar por ele, e descartar no despacho os frames que
  não vão ser mostrados.

**2026-10-11 — espera eliminada, padrão para todos:**

- **Entrega direta:** a emulação, logo depois de enfileirar (e do `palette_update`), entrega
  o frame a um worker livre (`rt_dispatch_pending`); os workers também puxam a fila ao
  ficarem livres. Antes o frame esperava a main voltar ao `rend_single_frame` (no ritmo do
  frontend): latência fila→worker p95 9 ms → 0,05 ms.
- **Gate estrito:** sem worker livre, o `g_rendBusyUntilUs` fica "ocupado" e o
  `QueueRender` descarta o frame (como a liberação antecipada do render antigo). Estimar o
  fim do desenho deixava passar frames quando ele atrasava. Espera da emulação pelo Process
  1,2 → 0,3 ms/frame (base 0,4).
- **Núcleo 3 só da emulação:** as outras threads do processo (apresentação/áudio do
  frontend) saem dele.
- Sem opção por jogo (pedido do usuário): ligado para todos; `FC_RT=0` só para A/B.

| Jogo | Modo | VEL% | fps novos | frame p50/p95/p99 (ms) | underruns |
|---|---|---|---|---|---|
| MvC2 | base | 99,8 | 40,1 | 20,3 / 45,5 / 55,0 | 0 |
| MvC2 | rt | 99,9 | **52,0** | **16,6 / 34,7 / 39,5** | 1 |
| Shenmue II | base | 56,0 | 15,5 | 56,7 / 76,8 / 88,4 | 33 |
| Shenmue II | rt | 55,8 | 16,0 | 57,0 / 75,7 / 89,7 | 25 |
| DOA2 (esfriado) | base | 89,2 | 27,0 | 35,7 / 44,6 / 62,0 | 11 |
| DOA2 (esfriado) | rt | 83,2 | 35,3 | 22,1 / 50,8 / 59,7 | 18 |

Hash idêntico ao render antigo: MvC2, DOA2, Napple (RTT, 1003/1003).

**A queda do DOA2 não é espera** (a emulação fica ~99% ocupada nos dois modos, mesma
distribuição de tempo no `perf`): é o **4.132** — a GPU sobe de ~45% para ~70% e o
`power_allocator` do RK3326 corta o clock da CPU (1478 → 1344 MHz médios). Para mostrar os
~50 frames que o DOA2 produz, o que precisa cair é o **custo de GPU por frame** (~16 ms):
próximo passo é medir onde ele vai (resolução, modifier volumes, translúcidos, shader).

## 1. A ideia

Hoje um frame do Dreamcast passa por uma thread de render só: `process` (TA → listas de
polígonos/vértices, CPU) e `render` (chamadas GL, CPU do driver Mali), e só então o
próximo frame começa. Com **N renderers em round robin**, o frame `k` vai para o
renderer `k mod N`, cada um com seu contexto GLES (compartilhando texturas e buffers)
desenhando num FBO próprio, e a thread principal apresenta os FBOs **na ordem**.
Os renderers trabalham ao mesmo tempo em núcleos diferentes da CPU.

Não deixa a GPU mais rápida: deixa a **CPU entregar comandos mais rápido**. Um frame sai a
cada `max(CPU do render, GPU)`; com 2 renderers, a parte da CPU por frame cai pela metade.

## 2. O que já está medido

| Medida | Resultado |
|---|---|
| Viabilidade no driver (teste isolado, 640 draws/frame pequenos, `perfmax`) | 1 thread **88,8 frames/s** (17,9 µs de CPU por draw, thread a 100%); 2 threads com contextos compartilhados **155,2** (1,75×, 18,6/19,7 µs por draw, 98% cada); independentes 149,4. O driver Bifrost r13p0 não serializa contextos. |
| GPU do R36 | Mali-G31, o driver expõe **1 núcleo** (`core_mask 0x1`), 400/480/520 MHz (520 no `performance`). Ociosa na maior parte do tempo nos jogos medidos. |
| **DOA2** (4.130) | Jogo de 60; render `process ~5 + render ~14 ≈ 19 ms` > intervalo de 16,7 ms → a fila de 1 slot fica ocupada e o core **descarta ~40% dos frames** → apresenta 30-36 fps. Draw-call-bound (baixar a resolução quase não muda). **Caso ideal**: com 2 renderers cada frame tem ~33 ms para os ~19 ms. |
| MvC2 | Thread principal ~42% de um core, quase tudo no driver; ~640 draws × ~34 µs. Candidato. |
| Shenmue II (2026-10-10) | GPU ~30% ocupada, render 6,2 + 21,6 ms de ~60 ms; o limite é a emulação (SH4 + ARM7 do som). **Não ajuda.** |

Regra para cada jogo: **ajuda se `process + render` por frame > intervalo do jogo** e a GPU
não está cheia (`/sys/devices/platform/ff400000.gpu/utilisation` < ~80%). Medir com
`FC_REND_SPLIT` + `FC_SYNC_STATS` (`dropped_slot_busy`) + a ocupação da GPU.

## 3. Desenho

### 3.1 Contextos

- No `retro_run`, o core já tem o contexto do frontend corrente: pega
  `eglGetCurrentDisplay()`/`eglGetCurrentContext()` e cria N contextos **compartilhados**
  com ele, um por thread de render.
- Superfície: **sem superfície** (`EGL_KHR_surfaceless_context`, disponível no R36) e
  desenho só em FBO. **Nunca pbuffer**: pbuffer no GBM derrubou o kernel 4.4 do R36
  (oops, GPU travada até o reboot — 2026-10-10).
- O que **não** é compartilhado entre contextos e precisa existir em cada um: FBO, VAO,
  estado de GL. **Programas são compartilhados, mas os valores de uniform fazem parte do
  programa** → cada renderer compila os seus programas (ou regrava todos os uniforms por
  draw), senão um renderer sobrescreve o uniform do outro.
- Se o frontend não permitir, o retrorun (fork nosso) cria os contextos compartilhados
  e entrega ao core.

### 3.2 Fluxo de um frame

1. Emulação fecha o frame do TA e o põe na fila (já existe: `tactx`, `rend_start_render`,
   fila de render 4.16). A fila passa a ter N slots, um por renderer.
2. O despachante entrega o frame `k` ao renderer `k mod N` (que precisa estar livre).
3. O renderer faz `process` + atualização de texturas (seção 3.3) + chamadas GL no seu FBO
   e cria um fence.
4. A thread principal, no `retro_run`, espera o fence do **próximo frame na ordem**, copia
   o FBO dele para o FBO do frontend (ou o usa como textura de apresentação) e chama o
   `video_cb`.
5. Latência: +1 frame com N = 2 (o frame `k+1` é montado enquanto `k` ainda desenha).

### 3.3 Estado compartilhado (o difícil)

- **Cache de texturas** (`core/rend/TexCache.cpp`, `gltex.cpp`): entre o frame `k` e o
  `k+1` a VRAM pode mudar e invalidar texturas que o `k` ainda está usando. Soluções:
  - **versionar**: quando uma textura muda e há frame em voo usando a versão antiga,
    criar outro nome GL (cópia na escrita); a antiga é liberada quando o fence do último
    frame que a usou passar;
  - as atualizações de textura de cada frame rodam **na ordem dos frames** (o renderer
    `k+1` só começa a fase de texturas depois que o `k` terminou a dele), e as chamadas
    de desenho em paralelo.
- **Render-to-texture** (`isRTT`): se o frame `k+1` lê o resultado de um RTT do `k`, ele
  espera o fence do `k`. Frames com RTT perdem o paralelismo (são minoria).
- **Frames de framebuffer** (`isRenderFramebuffer`) e cópias para a VRAM: serializar.
- **Paleta, tabela de fog, registradores do PVR**: precisam ser capturados com o frame
  (parte já vem no `rend_context`); o que hoje é lido do estado global na hora do render
  vira cópia por frame.

### 3.4 Ritmo e apresentação

- O core declara 30/60 e dorme até o prazo no `retro_run` (4.112): com N renderers a
  apresentação continua 1 frame por `retro_run`, na ordem; só muda que o frame já está
  pronto mais cedo.
- Se o renderer do próximo frame atrasar, repetir o último frame (como hoje) — sem
  quebrar a ordem.

## 4. Etapas

1. **Classificar os jogos**: acrescentar a ocupação da GPU ao `rr_capture.sh`/bench
   (amostra de 1 s do `utilisation`) e listar quais jogos têm `process + render` maior que
   o intervalo (DOA2 já tem).
2. **Protótipo sem paralelismo**: um segundo contexto compartilhado desenhando o frame
   num FBO, com a thread principal apresentando — saída **idêntica** ao renderer atual
   (hash dos pixels do FBO por frame, `glReadPixels`).
3. **Programas por contexto** e **versionamento de texturas**.
4. **N = 2 em paralelo**, com serialização de RTT/framebuffer.
5. **A/B** no DOA2 e no MvC2 (fps apresentado, `dropped_slot_busy`, VEL%, frame time
   p50/p95/p99) e conferência de imagem pelo usuário.

## 5. Validação

- Imagem: hash dos pixels de cada frame **igual** ao do renderer único (exceto a
  latência de +1 frame), em DOA2, MvC2, Napple (RTT) e um jogo com framebuffer.
- O estado da emulação não muda: `FC_STATE_HASH` idêntico (o render só afeta a emulação
  via RTT/cópias para a VRAM, que ficam serializados).

## 6. Riscos

- Driver Mali com vários contextos: o teste isolado passou; o pbuffer derrubou o kernel —
  manter o caminho sem superfície/janela.
- Memória: N FBOs + texturas versionadas (895 MB de RAM no R36).
- Frontend: mudança no retrorun para os contextos compartilhados; reset do contexto de
  hardware do libretro precisa recriar os N contextos.
- Ganho limitado pela GPU de 1 núcleo: quando a GPU passar de ~80% de ocupação, mais
  renderers não ajudam.

**2026-10-11 — GPU por frame (4.133):** o `gl_FragDepth` em todo pixel desligava o descarte
antecipado da Mali; com a profundidade no vértice o DOA2 cai de 16,6 para 7,2 ms de GPU por
frame. Com as threads: 44 fps novos (base 27), GPU 42%, VEL 82% (base 90,6%) — o que sobra é
CPU do render (~640 draws/frame) nas 2 threads. Próximo: menos draws por frame.

