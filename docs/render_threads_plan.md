# Plano futuro: mais threads de render (round robin de frames)

Status: **plano futuro (2026-10-10)** — fora do `current_plan.md`; não executar sem
pedido. Itens: `tech_debits.md` 4.131 (teste de viabilidade) e 4.130 (DOA2, o caso que
motiva). Teste isolado: `tools/gl_two_threads.c`.

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
