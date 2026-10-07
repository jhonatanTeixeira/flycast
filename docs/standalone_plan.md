# Plano: flycast **standalone** (absorver o retrorun no nosso emu)

> Documento de planejamento. **Nada de código ainda** — primeiro decidir o
> caminho. Estado: proposta para revisão (2026-10-07).

## 1. Objetivo

Transformar o nosso fork do flycast (hoje **só core libretro**) num **app
standalone** — um binário que faz *tudo* que o `retrorun3` faz hoje no device,
sem depender do fork do retrorun. Ou seja: "puxar as funcionalidades do retrorun
para dentro do nosso emu".

Motivação:
- Um **dono só** do stack (core + apresentação + áudio + input + config +
  medição) em vez de dois repos/projetos com filosofias diferentes.
- Acesso **direto** ao core (settings, savestate, hooks de timing) — sem a
  camada libretro no meio — para as features que o libretro não expressa bem
  (ex.: o wait de frame, o áudio com WSOLA, o dump do JIT).
- Reduzir o atrito de build/deploy (hoje: rebuildar core **e** retrorun).

**Não** é objetivo (agora): reescrever a emulação. O core fica.

## 2. Estado atual

| peça | onde | o que é |
|---|---|---|
| **Nosso core** | este repo (`core/`, 143 `.cpp`) | emulação DC/Naomi/Atomiswave, dynarec ARM64/tier2, render GLES. **Só libretro** (sem frontend). Build: `Makefile`/`Makefile.common` → `flycast_libretro.so`. |
| **Glue libretro** | `core/libretro/libretro.cpp` (4142 linhas) | mapeia o core para a API libretro: `retro_run`, callbacks de vídeo/áudio/input, core options, serialize (savestate). |
| **Frontend** | fork `dreams/retrorun` (~34k linhas) | SDL2/KMSDRM, apresentação em thread, áudio (SDL + rate control + **WSOLA**), input, config + catálogo por jogo, savestate, menu, benchmark, RetroAchievements. |
| **Standalone do upstream** | device `/opt/flycastsa/flycast` (v2.6-9) | o app SDL2 + imgui do flyinghead. **É o master: ~10x mais lento** no R36 (ver `CLAUDE.md`). Serve de referência de UX, não de base. |

O core do fork **já expõe** uma API de app (`core/emulator.h`):
`dc_init / dc_reset / dc_start / dc_run / dc_term / dc_stop / dc_request_reset /
dc_is_running`. Hoje só a glue libretro a usa.

## 3. Inventário do retrorun (o que "puxar")

| módulo | linhas | o que faz | essencial p/ o R36? |
|---|---|---|---|
| `platform/` (SDL) | ~3.3k | KMSDRM, **apresentação em thread** (FIFO/mailbox, `RETRORUN_PRESENT_DEPTH`, vsync) | **sim** |
| `video/` | ~5.4k | escala, tate, aspecto, pixel-perfect, decorações/bezel, shader, OSD/FPS | quase tudo |
| `audio/` | ~1.6k | SDL_QueueAudio, stable buffer, áudio em thread, **rate control + WSOLA** (4.113) | **sim** |
| `input/` | ~2.2k | SDL gamepad, mapeamento, hotkeys, rumble, analógico→digital | **sim** |
| `config/` | ~5k | `retrorun.cfg` + **catálogo por jogo** (product number → perfil) | **sim** |
| `core/` | ~5.6k | loader libretro, gestão de savestate/slot, disk control | **sim** |
| `diagnostics/` | ~1k | **benchmark** (JSON, janela rolante), perf, logger | **sim** (medição) |
| `menu/` | ~1k | menu in-game (load/save, volume, brilho, device, aspecto…) | parcial |
| `services/` | ~2k | **RetroAchievements** (rcheevos), file browser | opcional |
| `go2/` | ~6.7k | backend ODROID-GO2 (DRM + OpenAL) | **não** (o R36 usa SDL) |

Fora do retrorun, hoje:
- `rr_capture.sh` + envs `FC_*` (dump JIT, sync stats, perf) — captura da bateria.
- `dreamcast.sh`/`naomi.sh`/`atomiswave.sh` + `es_settings.cfg` — integração ES.
- `KILLIT`/`killer_daemon` — integração do ES com o standalone.

## 4. O que o core do fork já entrega (para não reimplementar)

- Emulação + `settings` (`core/types.h`), savestate (`core/serialize.cpp`),
  renderer GLES (`core/rend/`), AICA (`core/hw/aica/`), input state.
- A glue libretro já resolve: **core options** (mapa de opções → `settings`),
  **serialize** (tamanho/estado), **callbacks** de áudio/vídeo/input, ciclo
  `retro_run` (inclui o **wait de frame** 4.110 e o dump do JIT).

Consequência prática: um standalone pode (a) **compilar a glue libretro dentro
do binário** e falar libretro internamente, ou (b) **chamar o core direto**.

## 5. Opções de arquitetura

### A. Vendor do frontend + core estático — **1 binário** *(recomendada como base)*
Mover o frontend do retrorun para dentro do fork (ex.: `frontend/`) e linkar a
glue libretro **estaticamente** no mesmo binário `flycast`.
- **+** Reaproveita 100% do retrorun (já validado no device) e do core; um só
  build/repo; risco baixo; dá pra começar já.
- **−** Mantém a indireção libretro; carrega ~34k linhas de frontend.
- **Custo:** baixo/médio (é "mudança de casa", não reescrita).

### B. Frontend novo minimalista chamando `dc_*` direto — **standalone "de verdade"**
Escrever um frontend pequeno (SDL2) que chama `dc_init/dc_run/...` e o
serialize direto, sem libretro.
- **+** Limpo, pequeno, sem indireção; controle total do timing/áudio.
- **−** Reimplementar input, core options, savestate, menu, benchmark; perde o
  que a glue libretro já resolve; esforço alto.
- **Custo:** alto.

### C. Portar o standalone do upstream (`core/sdl` + `core/ui`) para o fork
- **+** App maduro (imgui, settings, shaders, achievements), mantido lá.
- **−** O fork diverge **2015** do upstream; a API (`emulator.h`, `settings`,
  `rend`) mudou muito. É um merge grande e arriscado, com risco de perder as
  otimizações do fork.
- **Custo:** muito alto / provavelmente inviável de forma incremental.

### D. Só juntar os dois no repo do flycast (2 componentes, 1 build)
Trazer o frontend do retrorun para um subdir e buildar junto, ainda como
`.so` + frontend.
- **+** Passo intermediário natural do A.
- **−** Continua 2 peças; não é "standalone".

**Recomendação:** começar pelo **A** (chega rápido num binário único e
funcional), e **evoluir para o B** só onde a camada libretro atrapalha (timing,
áudio, hooks de diagnóstico) — absorvendo as peças do retrorun aos poucos, sem
big-bang. **Não** ir pelo C.

## 6. Arquitetura proposta (base A)

```
flycast/
  core/                      # emulação (fica)
  core/libretro/             # glue libretro (fica, agora linkada estática)
  frontend/                  # NOVO: frontend do retrorun, adaptado
    main.cpp                 # loop, load do core (agora in-process)
    platform_sdl.cpp         # KMSDRM + apresentação em thread
    audio/                   # SDL + rate control + WSOLA (4.113)
    input/  video/  menu/  config/  diagnostics/  services/
  standalone/                # NOVO: alvo de build -> binário `flycast`
```

- Build: um alvo (Makefile, ou CMake novo) que compila `core/` + `core/libretro/`
  + `frontend/` → **`flycast`** (binário). O `.so` libretro continua sendo gerado
  para RetroArch (não perdemos o caminho atual).
- O frontend deixa de `dlopen` o `.so` e passa a chamar a glue **in-process**
  (mesmas funções `retro_*`, agora linkadas).
- Config unificada: `retrorun.cfg` + opções do flycast viram uma só; o
  **catálogo por jogo** (product number) é preservado.

## 7. Fases e marcos

| fase | entrega | critério de aceite |
|---|---|---|
| **0** | Decisões (seção 10) + inventário congelado (o que é essencial) | você confirma o escopo |
| **1** | `frontend/` compilando junto, **bootando um jogo** (vídeo+input+áudio+config) | Napple abre e roda com som |
| **2** | Apresentação em thread (FIFO/mailbox, depth, vsync) + **áudio com WSOLA** | mesmo comportamento do `retrorun3` (4.105/4.110/4.113) |
| **3** | Saves (SRAM/savestate, auto, slots, screenshot) + menu in-game (subconjunto) | load/save funcionando no device |
| **4** | Config unificada + catálogo por jogo + integração ES (`*.sh`, `KILLIT`) | ES lança o binário novo como lança o retrorun3 |
| **5** | **Benchmark/captura/diagnóstico** nativos (substituir `rr_capture.sh`/`FC_*`) | JSON igual, medição equivalente |
| **6** | Trocar a glue libretro por chamadas diretas onde vale (timing/áudio/hooks) | sem regressão; menos indireção |
| **7** | **A/B** contra `retrorun3` nas mesmas cenas | fps/VEL/underruns/frame time ≥; veredito do usuário |

## 8. Riscos e mitigação

- **Perder features "de graça"** (core options, achievements, netplay?) →
  manter a glue libretro na base (A) e absorver aos poucos.
- **Savestate divergente** entre `.so` e standalone → mesmo `serialize.cpp`,
  mesmo formato; validar com `FC_STATE_HASH`.
- **Regressão de performance** (frontend novo mexendo no timing) → A/B sempre,
  mesma cena; manter o `retrorun3` instalado como fallback durante a transição.
- **Escopo grande** → fases independentes; cada uma commitável e testável.
- **Build quebrando o `.so`** → manter os dois alvos (core libretro + standalone).

## 9. Validação e medição

- **Mesma cena/savestate**; A/B standalone × `retrorun3`.
- Sempre: frame time (`active_*`) p50/p95/p99 **e** média, fps (novos ×
  apresentados × dupes), **VEL%**, underruns/overruns.
- Confirmar o que a rodada mediu; o número final é o que o usuário vê/ouve.
- `FC_STATE_HASH` para garantir que o core não mudou (o frontend não deve alterar
  a emulação).

## 10. Decisões abertas (preciso do seu OK)

1. **"Standalone" = ?**
   (a) 1 binário **sem** libretro (opção B), ou
   (b) 1 binário **mantendo** a glue libretro por dentro (opção A, recomendada)?
2. **Escopo:** absorver **tudo** (achievements, decorações, go2) ou só o
   **essencial do R36** (apresentação + áudio + input + config + saves +
   benchmark)?
3. **RetroArch:** continuamos gerando o `.so` libretro em paralelo, ou o
   standalone vira o único alvo?
4. **Transição:** mantenho o `retrorun3` instalado como fallback até o A/B fechar?

## 11. Regras do projeto que valem aqui

- **Medir antes de otimizar**; toda hipótese validada no device.
- **Código não se reverte — se corrige**; nada de voltar atrás como "solução".
- **Commitar a cada marco** validado (código + docs).
- **Mesma cena** em qualquer A/B; sempre fps **e** frame time, p50/p95/p99 **e**
  média, e a **VEL%**.
- Registrar achados em `tech_debits.md` e a sessão em `history.md`.
