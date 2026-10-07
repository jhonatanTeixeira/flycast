# Plano: flycast standalone — frontend SDL próprio (absorver o retrorun)

> Documento de planejamento. **Só plano por enquanto — nada de código.**
> Decisão registrada em 2026-10-07.

## 1. Objetivo e decisão

Criar, **dentro deste fork**, um **frontend SDL** que faz o que o `retrorun3`
faz hoje no device — mas **chamando o core direto pelo código**, sem
`dlopen` da `libretro.so`.

Decisões do usuário (2026-10-07):
- **O alvo libretro continua existindo** (`.so`) — o **retrorun** e o
  **RetroArch** seguem usando. Não perdemos o caminho atual.
- O frontend novo é um **copy-paste adaptado** das funcionalidades do SDL do
  retrorun (em vez de chamar a `.so`, chama o core direto).
- O frontend deve implementar, no mínimo:
  1. **WSOLA** (áudio com tom preservado — 4.113);
  2. **Benchmark** (JSON, contadores, janela rolante) + harness de captura;
  3. **As melhorias de SDL que o projeto fez** (apresentação em thread
     FIFO/mailbox, `RETRORUN_PRESENT_DEPTH`, vsync, wait de frame — 4.105/4.110);
  4. **Frameskip adaptativo** — que **ainda não existe no SDL do nosso retrorun**
     (hoje é `#ifndef RR_PLATFORM_SDL`, ou seja, compilado fora).
- **Menu do emu** deve expor **todas as configs do emu** (as `settings` do core).
- **Escopo enxuto:** **sem RetroAchievements** nem "firulas" (decorações/bezel,
  backend go2, netplay). Só o **essencial do R36**.
- Fica como **plano** por enquanto (fases abaixo).

Motivação: um dono só do stack (core + apresentação + áudio + input + config +
medição), acesso **direto** ao core (settings, savestate, hooks de timing) para
as features que o libretro não expressa bem, e menos atrito de build/deploy.

**Não** é objetivo: reescrever a emulação. O core fica.

## 2. Estado atual

| peça | onde | o que é |
|---|---|---|
| **Nosso core** | este repo (`core/`, 143 `.cpp`) | emulação DC/Naomi/Atomiswave, dynarec ARM64/tier2, render GLES. **Só libretro** (sem frontend). Build: `Makefile`/`Makefile.common` → `flycast_libretro.so`. |
| **Glue libretro** | `core/libretro/libretro.cpp` (4142 linhas) | mapeia o core para a API libretro: `retro_run`, callbacks de vídeo/áudio/input, core options, serialize (savestate). **Fica** (retrorun + RetroArch). |
| **Frontend atual** | fork `dreams/retrorun` (~34k linhas) | SDL2/KMSDRM, apresentação em thread, áudio (SDL + rate control + **WSOLA**), input, config + catálogo por jogo, savestate, menu, benchmark, RetroAchievements. |
| **Standalone do upstream** | device `/opt/flycastsa/flycast` (v2.6-9) | app SDL2 + imgui do flyinghead. **É o master: ~10x mais lento** no R36 (`CLAUDE.md`). Referência de UX, não de base. |

O core do fork **já expõe** uma API de app (`core/emulator.h`):
`dc_init / dc_reset / dc_start / dc_run / dc_term / dc_stop / dc_request_reset /
dc_is_running`. Hoje só a glue libretro a usa.

## 3. Inventário do retrorun (o que puxar, com foco no SDL)

| módulo | linhas | o que faz | puxar? |
|---|---|---|---|
| `platform/` (SDL) | ~3.3k | KMSDRM, **apresentação em thread** (FIFO/mailbox, `RETRORUN_PRESENT_DEPTH`, vsync) | **sim** |
| `video/` | ~5.4k | escala, tate, aspecto, pixel-perfect, shader, OSD/FPS | sim (**sem decorações/bezel**) |
| `audio/` | ~1.6k | SDL_QueueAudio, stable buffer, áudio em thread, **rate control + WSOLA** | **sim** |
| `input/` | ~2.2k | SDL gamepad, mapeamento, hotkeys, rumble, analógico→digital | **sim** |
| `config/` | ~5k | `retrorun.cfg` + **catálogo por jogo** (product number → perfil) | **sim** |
| `core/` | ~5.6k | loader libretro, gestão de savestate/slot, disk control | parcial (só saves/disk) |
| `diagnostics/` | ~1k | **benchmark** (JSON, janela rolante), perf, logger | **sim** |
| `menu/` | ~1k | menu in-game (load/save, volume, brilho, device, aspecto…) | **sim** (+ settings do emu) |
| `services/` | ~2k | RetroAchievements (rcheevos), file browser | **não** (só file browser, se precisar) |
| `go2/` | ~6.7k | backend ODROID-GO2 (DRM + OpenAL) | **não** (R36 usa SDL) |

Fora do retrorun, hoje: `rr_capture.sh` + envs `FC_*` (captura), `dreamcast.sh`/
`naomi.sh`/`atomiswave.sh` + `es_settings.cfg` (ES), `KILLIT`/`killer_daemon`.

## 4. O que o core já entrega (não reimplementar)

- Emulação + `settings` (`core/types.h`), savestate (`core/serialize.cpp`),
  renderer GLES (`core/rend/`), AICA (`core/hw/aica/`), input state.
- A glue libretro já resolve: **core options** (mapa opções → `settings`),
  **serialize**, **callbacks** de áudio/vídeo/input, ciclo `retro_run` (inclui o
  **wait de frame** 4.110 e o dump do JIT).

## 5. Arquitetura escolhida — frontend SDL próprio, core in-process

```
flycast/
  core/                      # emulação (fica)
  core/libretro/             # glue libretro (fica; alvo .so separado)
  frontend/                  # NOVO: frontend SDL (adaptado do retrorun)
    main.cpp                 # loop, conteúdo, integração com o core
    platform_sdl.cpp         # KMSDRM + apresentação em thread
    audio/                   # SDL + rate control + WSOLA (4.113)
    input/  video/  menu/  config/  diagnostics/  services/
  # alvos de build:
  #   flycast_libretro.so     (retrorun + RetroArch)   <-- mantido
  #   flycast                 (standalone, core in-process)
```

**Como o frontend fala com o core** (pontos de integração a definir):

| aspecto | hoje (glue libretro) | no frontend direto |
|---|---|---|
| conteúdo | `retro_load_game(info)` → path + `dc_init` | setar o path + `dc_init`/`dc_start` |
| loop | `retro_run()` | `dc_run()` |
| vídeo | core chama `video_cb` (frame no FBO) | hook de fim de frame; frontend lê o FBO e apresenta |
| áudio | AICA → `WriteSample` → `audio_batch_cb` | sink direto (frontend provê o destino) |
| input | core lê via `input_cb` | setar o estado do controle |
| **settings** | core options → `settings` | **acesso direto a `settings`** (base do menu) |
| savestate | `retro_serialize`/`unserialize` | `core/serialize.cpp` direto |

Duas sub-abordagens (decidir na fase 1):
- **B1 — reaproveitar a glue**: compilar as funções `retro_*` no binário e
  chamá-las in-process. Chega rápido a um standalone funcional.
- **B2 — chamadas diretas ao core**: substituir a glue por `dc_*` + acesso
  direto a `settings`/serialize. Mais limpo; é o que permite o **menu com todas
  as configs**.

Sugestão: começar **B1** (rápido, reusa o que já funciona) e migrar para **B2**
onde o menu/hooks exigem.

## 6. Features a portar (copy-paste adaptado do SDL do retrorun)

1. **Áudio + WSOLA** — `src/audio/audio_backend_sdl2.cpp` e
   `audio_rate_control.h` (rate control + `AudioTimeStretch`). Já validado no
   device (4.113).
2. **Apresentação em thread** — `src/platform/platform_sdl.cpp`: KMSDRM,
   FIFO/mailbox, `RETRORUN_PRESENT_DEPTH`, vsync, e a integração com o **wait de
   frame** do core (4.105/4.110). Precisa manter "apresentado × novos × dupes".
3. **Frameskip adaptativo** — hoje **ausente no SDL** (todo o bloco é
   `#ifndef RR_PLATFORM_SDL` em `main.cpp:1430-1692` e `video.cpp:1199`). Portar
   e religar no frontend novo, com a **base de fps correta** (o retrorun usa o
   refresh de 60 pra tudo DC — ver 4.112; num jogo de 30 fps a base tem que ser
   30). É o item que o usuário citou como "ainda não arrumamos no SDL".
4. **Benchmark** — `src/diagnostics/benchmark.*` (JSON, janela rolante,
   contadores, timings) + o harness de captura (`FC_JIT_DUMP`, `FC_SYNC_STATS`,
   `perf`) que hoje vive no `rr_capture.sh`.
5. **Input** — `src/input/` (SDL gamepad, mapeamento, hotkeys, rumble).
6. **Config + catálogo por jogo** — `src/config/`.
7. **Menu** — `src/menu/`, **estendido** para expor **todas as `settings` do
   emu** (o que hoje é feito por core options).

## 7. Fases e marcos

| fase | entrega | critério de aceite |
|---|---|---|
| **0** | Confirmar escopo (seção 9) + decidir B1 × B2 | você confirma |
| **1** | `frontend/` compilando e **bootando um jogo** (vídeo+input+áudio+config), core in-process | Napple abre e roda com som |
| **2** | Apresentação em thread (FIFO/mailbox, depth, vsync) + **WSOLA** | mesmo comportamento do `retrorun3` (4.105/4.110/4.113) |
| **3** | **Frameskip adaptativo** portado (com base de fps correta) | fps/suavidade ≥ retrorun3; sem regressão |
| **4** | Saves (SRAM/savestate, auto, slots) + menu (subconjunto) | load/save no device |
| **5** | **Benchmark** nativo + captura (substituir `rr_capture.sh`/`FC_*`) | JSON equivalente |
| **6** | **Menu com todas as configs do emu** (settings direto) + config/catálogo + ES | ES lança o binário novo |
| **7** | **A/B** contra `retrorun3` nas mesmas cenas | fps/VEL/underruns/frame time ≥; veredito do usuário |

## 8. Riscos e mitigação

- **Regressão de performance** ao mexer no timing → A/B sempre, mesma cena;
  manter o `retrorun3` instalado como fallback.
- **Perder o que a glue libretro dava de graça** → manter a `.so` (retrorun/
  RetroArch) e começar pelo B1.
- **Savestate divergente** entre `.so` e standalone → mesmo `serialize.cpp`,
  mesmo formato; validar com `FC_STATE_HASH`.
- **Escopo grande** → fases independentes, cada uma commitável e testável.
- **Frameskip**: religar sem a base de fps certa repete o problema do retrorun
  (4.112) — tratar junto.

## 9. Decisões

Registradas:
- ✅ **Manter o libretro** (retrorun + RetroArch).
- ✅ **Frontend SDL novo**, core **direto pelo código** (sem `.so`).
- ✅ **Copy-paste adaptado** das funcionalidades do SDL do retrorun.
- ✅ Absorver: **WSOLA, benchmark, melhorias de SDL, frameskip adaptativo**.
- ✅ **Menu do emu com todas as configs** (plano).
- ✅ **Escopo enxuto:** sem RetroAchievements, sem decorações/bezel, sem go2,
  sem netplay — só o essencial do R36.

Abertas (propostas para OK):
1. **B1 × B2** na fase 1 — *proposto:* começar **B1** (glue in-process) e migrar
   para **B2** (chamadas diretas) onde o menu/hooks exigirem.
2. **Nome/instalação** — *proposto:* binário `flycast` no `/usr/local/bin`,
   convivendo com o `flycastsa` do upstream; o ES passa a oferecê-lo ao lado do
   `retrorun3` durante a transição (o usuário escolhe qual lançar).

## 10. Regras do projeto que valem aqui

- **Medir antes de otimizar**; toda hipótese validada no device.
- **Código não se reverte — se corrige**; nada de voltar atrás como "solução".
- **Commitar a cada marco** validado (código + docs).
- **Mesma cena** em qualquer A/B; sempre fps **e** frame time, p50/p95/p99 **e**
  média, e a **VEL%**.
- Registrar achados em `tech_debits.md` e a sessão em `history.md`.
