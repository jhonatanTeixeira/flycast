# Histórico — Projeto de otimização flycast2021 → flycast2026 (R36 / RK3326)

> Log cronológico, **compactado em 2026-10-08**: uma linha de resultado por marco,
> com o número do item em `docs/tech_debits.md` (onde estão o detalhe e os números).
> O texto integral de 2026-09-13 a 2026-10-08 (tabelas A/B completas, desmontagens,
> hipóteses refutadas passo a passo) está em
> **`docs/arquivo/history_2026-09-13_a_2026-10-08.md`**; quando outro doc diz "ver
> `history.md` <data>" para tabelas, é lá que elas estão.
>
> Daqui pra frente: adicionar **uma entrada curta por sessão/marco** no fim, com
> data (e hora se houver mais de uma no dia), o resultado medido e o item do
> `tech_debits.md`. O detalhe vai no `tech_debits.md`, não aqui.

## Lições de método (consolidadas)

As que custaram caro e se repetiram; as regras derivadas estão no `CLAUDE.md`.

- **`perf` mostra onde a CPU gasta, não o que limita o frame.** Várias leituras
  erradas vieram de tratar `SH4_TCB` alto como "o JIT é o gargalo" (mslug6 era
  cache de textura; DOA2 parecia render e era emu). Quem decide é a amostragem de
  CPU **por thread** (`/proc/<pid>/task/*/stat`) + split do frame (`FC_REND_SPLIT`,
  `FC_TA_SPLIT`).
- **Instrumentação deixada no código contamina A/B.** A regressão 2D do regalloc
  parecia +8% e era +3,2%; metade era `chrono` esquecido. Instrumentação é opt-in
  por variável de ambiente, custo zero desligada.
- **Build incremental mente.** `.o` velho após editar header amplo (`types.h`,
  `blockmanager.h`, `host_context.h`) já deu regressão de ~9× e crash
  determinístico (inline cache, 2026-10-08). `make clean` com os mesmos argumentos
  + `find . -name '*.o' | wc -l` = 0.
- **fps apresentado engana.** A velocidade real é a do áudio (VEL%) e os frames
  **novos** (`new_fps`); dupes inflam o contador (Napple "30→60" e "340 fps" eram
  repetidos; artefatos do mbaa eram dupes). Esperar o render sobe o fps e derruba a
  VEL (DOA2 23→33 fps, 74→55%).
- **Mesma cena nas duas rodadas.** Várias "regressões" eram cena diferente (kofevo,
  Shenmue II, Le Mans, samsptk). Savestate + `perfmax performance` (condição do ES),
  pausa entre rodadas pesadas (térmico dobrou um número uma vez).
- **Testar pelo caminho real (ES/cfg oficial)** antes de concluir — cfg de teste
  própria já enviesou um diagnóstico de crash.
- **"Descartado" fora do caminho crítico não é descartado.** O anti-SMC (1.1) foi
  descartado em 09-13 e era +19% no Shenmue em 09-19.
- **Hipótese de "X% do custo ⇒ ganho grande" falhou várias vezes** (FPSCR nativo,
  HLE do laço do DOA2, SQ inline): mesma travessia JIT→C++ ou mesmo tráfego de
  estado nos dois lados. Medir antes de prometer.
- **Determinismo para validar JIT:** `FC_STATE_HASH` + `FC_RTC_FIXED` +
  `FC_INPUT_NEUTRAL` (tier2 off), comparar com `tools/state_compare.py`; um controle
  com bug proposital prova que o caminho é exercido.

## Linha do tempo

### 2026-09-13 — início, baseline e fila de 8 itens

- Device: R36 (RK3326, A53 ×4 @1,5 GHz, Mali-G31), Debian 13 no userspace. Fork
  `metallic77/flycast` @ `603814c9f` diverge do flyinghead em **2015**.
- Baseline Shenmue (intro): 26,9 fps, core 26,1 ms; clock de GPU só mexe no vídeo →
  não é GPU-bound; um core a 100% (single-thread).
- Master (flyinghead) cross-compilado: frames 26→100 ms e SIGSEGV no dispatch de
  bloco. Decisão: não perseguir o master.
- Build aarch64: bugs do Makefile (`CXX ?=`, `CC_AS`, `-lGLESv2`) → comando fixo do
  `CLAUDE.md`. Auditoria estática → `profiling_plan.md`/`tech_debits.md`.
- Fila de 8 itens medida no device: **só 4.2 sobreviveu** (`glDrawElements` ×
  ~600 draws ≈ 15 ms dos 25 ms de render em cena pesada). Descartados: anti-SMC
  (depois reaberto), AICA/ARM7, pool de TA_context, `sh4_sched_ffts`, TexCache
  cleanup, uniforms, GenSorted (não ativo: device usa per-strip).
- Item 9: compilação de bloco ~1,2% do tempo; SH4 cai a ~70% de tempo real em cena
  pesada → teto de throughput do JIT.

### 2026-09-14 — regalloc, batching, `perf` e Store Queue

- Regalloc FPU estendido S16-S31 (Gemini): Shenmue +5,5%; W9-W15 revertidos (scratch
  hardcoded → crash); mudanças em `gles.*` revertidas (quebravam personagens).
  2D regrediu **+3,2%** de core no kofnw (medição limpa, 4.9).
- `perf` instalado no device (call-graph dwarf); `gdb` também.
- **Batching por primitive restart (4.2):** Shenmue 26,9 → 28,8 fps (+7%), kofnw e
  mbaa ~+7%, e glitches do MBAA corrigidos de bônus. Validado visualmente.
- "Spin" do `SH4_TCB` no MBAA = laço de escrita: 16 stores em **Store Queue** +
  `pref` (gdb ao vivo), via `_vmem_WriteMem32` reescrito pelo `ngen_Rewrite`.
  Fast-path inline de SQ em todo store: **+6% mais lento**, revertido (1.7).
- Correção de premissa: o master é ~10× mais lento, não só crash; `perf` do
  standalone sugere cadeia funda tipo interpretador (não confirmado).
- Docs `x86jit.md`, `arm64jit.md`, `arm64jit_improvement_plan.md`.

### 2026-09-15 — SQ por stub, mslug6 e SH4_TIMESLICE

- Stub compartilhado de SQ só nos sites reescritos (1.7 v2): kofnw −2%, mslug6
  −1,4% de core. Ganho real, pequeno.
- mslug6: o quente é `bl intc_sched` no fim de blocos minúsculos (laço do jogo
  `8C05B3A4`), push/pop vazio ali → item 2 do plano não se aplica (4.10).
  `bm_WriteBlockMap` chamado ao vivo via gdb.
- `sh4_timeslice` como opção do core (1x-8x): sem ganho (4.11).
- `glInvalidateFramebuffer` (5.1): mslug6 +2-3% core (CPU-bound), mbaa melhor na
  sensação (GPU-bound). Mantido.

### 2026-09-16 — Translucent skip, FPSCR, `stc.l SR` e o cache de textura do mslug6

- Skip de Translucent sob spike (5.2/5.3): v1 corrompia frame (revertido), v2-v2.4
  sem ganho e com glitch → opt-in desligado.
- `lds Rn,FPSCR` (o fallback nº1, ~196/frame): auditoria interna + pesquisa externa
  (flycast master, redream, Dolphin, PPSSPP) + plano → implementado com fim de
  bloco forçado: **sem ganho** (mesma travessia JIT→C++). Mantido por decisão do
  usuário. Docs `fpscr_*`.
- **`stc.l SR,@-Rn` nativo** (`DM_WriteMSRF`): fallback 3,4M → 0; mbaa +3,2% fps,
  p99 −6%.
- **mslug6 = cache de textura (4.12/4.13):** `GetTexture` 41 ms/frame, miss 70% por
  VRAM suja invalidada por página de 4 KB (~30 texturas mortas por escrita). Idle
  loop legítimo; idle skip cortou 48% das instruções host sem mudar o fps.
  Ferramentas: `FC_BLOCK_PROF`, `FC_WATCH_ADDR`, `FC_IDLE_OPS`, `FC_REND_SPLIT`,
  `FC_TA_SPLIT`, `FC_IFB_COUNT`, `FC_FPSCR_STATS`.

### 2026-09-18 — crash do meltybld/capsnk e kofxi

- `sq_write_stub` não salvava o LR → SP subia até o guard page (1.10). `die()` agora
  loga arquivo/linha; `ngen_Rewrite` falha limpo quando não reconhece.
- kofxi: 75% das instruções host no SO cooperativo trocando contexto em vazio
  (4.14); 98% das paletizadas com bilinear fora da GPU (4.15).

### 2026-09-19 — idle fast-forward, apresentação em thread, anti-SMC

- **Idle fast-forward (4.14):** kofxi 49,7 → 59,2 fps e 83 → 100%; MBAA 83 → 100%;
  kofnw 94 → 99%. Fila de render esperando em vez de descartar (4.16).
- Shenmue: GPU 35 ms em série com o envio GL; o swap do frontend serializa CPU e
  GPU (4.18). **Fork `jhonatanTeixeira/retrorun`** com apresentação em thread +
  vsync/FIFO-mailbox: Shenmue 17 → 21 fps, 57 → 71%.
- Anti-SMC é o próximo gargalo do Shenmue (`FC_NO_BLOCK_CHECK` +19%). **Stores em
  página de código via stub com bitmap (4.19)**, depois de um crash no alias de
  store constante: Shenmue 23,3 fps / 77,7%, ligado por padrão após 11 jogos.

### 2026-09-22 — samsptk 20 → 59 fps, DOA2 e KOF Evo

- samsptk: 61% do tempo em converter+subir 3 texturas 512² 4bpp bilineares. **Shader
  de paleta bilinear portado do master (4.15): 20 → 59,4 fps**, neutro em 8 jogos.
- DOA2 (60 fps nativo, `req_native_fps`): emu thread saturada → teto ~74%; render
  640 draws como 2º gargalo; esperar o render sobe fps e derruba VEL (4.20).
  **Medir core + retrorun juntos; velocidade = áudio.**
- Evolution 1 não abre: CHD com zstd (`cdzs`). KOF Evo: `glDrawRangeElements`
  (4.21). mslug6 é reproduzível na média (4.22). MBAA glitches (4.23).
- Toolchain: só `aarch64-linux-gnu-*-13` existe nesta máquina.

### 2026-09-23 — teto de SH4, acesso compacto à RAM, flycast2026

- FMV para em L1I, DOA2 em DRAM; ~9 ciclos host por instrução SH4 (4.24-4.26).
- **Acesso compacto à RAM (`x13` base, 4.27) + 64 bits + `ZeroExtendLoadPass` +
  trampolins enxutos:** FMV do RE CV 71 → 83%; Soulcalibur 88 → 93%; kofnw −2-3%
  aceito (memória `project_kofnw_accepted_tradeoff`). Bug latente 4.28.
- `glDrawRangeElements`: KOF Evo 31 → 54 fps.
- **Renomeado para `flycast2026`** (prefixo `flycast2026_`); `flycast_libretro.so`
  passa a ser o upstream oficial v2.7-42.

### 2026-09-24 — fila, `x0` do rewrite, idle por assinatura, clock por jogo, AICA

- Tag `flycast2026-estavel-2026-09-24`. Rewrite usava `x0` como endereço (4.32):
  DOA2 73 → 81%, Shenmue 76 → 80%. Um upload de index buffer por lista (4.35): KOF
  Evo 58,8 fps a 100%. Stub `pref`→TA (4.36), margem do render (4.37).
- Avanço até o evento nos laços de espera (4.38/4.43): DOA2 → 87,6%, Shenmue →
  83%, **Shenmue II 68 → 84%**. Conta fecha: ~116M instr SH4/s × ~11 ciclos ARM.
- Le Mans (4.39-4.42): `sh4clock` padrão d12 cobrava 1,2× → **d10**; pulo do laço de
  atraso; detector espera×varredura; override de clock por jogo → **30 fps a 100%**.
- Barramento medido: teto 5,33 GB/s, 1 núcleo lê 2,2 GB/s; DRAM ~165-200 ns.
- **Som em thread própria (4.44):** controle na emu, mixagem na thread; PCM
  idêntico. Shenmue II 56,5 → 60,6%.

### 2026-09-24/25 — estudo do JIT, `jit_armv8_a` e o nascimento do tier2

- `FC_JIT_DUMP` + `jit_study.md` (4.45): 44-60% das instruções host são overhead;
  L1I estoura. Despacho evitável ~2,5-3,5% (4.46). PMU: IPC 0,43 (4.47).
  Determinismo por `FC_STATE_HASH` (4.48). `rts` previsível (4.49).
- **`jit_armv8_a`** (backend novo, 4.51-4.53): idêntico ao antigo nos 3 jogos, mas
  mais lento (código quente dobrou). Guardado.
- **Tier2 (regiões quentes):** à mão 1,85× no laço do DOA2 (4.55) → gerador
  offline (4.56) → injetado (4.57, DOA2 86 → 95%) → em runtime em C++/VIXL (4.58)
  → automático (4.59) → segunda thread + autochecagem (4.60).
- Tier2 virou opção do core (4.65); bug: a opção nunca ligava a formação
  automática (4.67); ligada de verdade, Sonic Shuffle piorou 55% → grupos sem laço
  exigem 4× mais calor (4.69).
- Varredura de configs: quase tudo já no ajuste rápido; `timeslice 2x` sem ganho.
- Achado: **o ES roda `perfmax performance`**, SSH não (4.70). Tier2 com glitches
  em vários jogos → desligado por padrão de código cooperativo (4.73-4.75).

### 2026-09-26 — Shenmue, bateria de 27 jogos, savestates velhos, Le Mans

- Chão sumido do Shenmue: o save gravou o glitch; tier2 determinístico nesse
  state. Autochecagem por replay no interpretador opt-in (4.76).
- Bateria de 27 jogos com savestate + 94 imagens (`game_status.md`, `docs/batery/`).
- ggxx/ggxxsla/sa2: crash era `board_count` lixo de savestate antigo (guarda) e
  região do tier2 lendo MMIO (desfeita no ponto seguro).
- Shenmue 13,2 → 26,6 fps (assinatura `bios-wait-flag` exige laço para trás);
  orçamento de frameskip `flycast2026_frameskip_budget` (33%); mslug6 estriado =
  padrão `region-bad-store-burst`.
- **Le Mans 15 → 30 fps:** laço de espera encadeado de 4 blocos
  (`chained-wait-loop-getter-cmp`). Assinaturas renomeadas pela forma, não pelo jogo.

### 2026-09-27 — tier2 em thread, MvC2/CvS2, modelo no-wait

- Branch following estilo Dolphin (DOA2 +3,6 VEL); tier2 inteiro na thread
  (435 → 83 µs/frame na emu, 4.83); o custo no Shenmue era o call por fatia (gate
  1/64). Plano de topologia em `thread_separation_plan.md`.
- **MvC2/CvS2 crash:** `pref` no delay slot de `jcond` clobberava o registrador de
  decisão (4.86) + self-healing genérico (bail para o tier1 no 1º fault).
- Pacer determinístico: negativo, opt-in (`frame_pacing_plan.md`).
- **Modelo no-wait (emu nunca espera):** core p99 100 → 11-39 ms, VEL ~100% nos não
  emu-bound; custo: dupes.
- Morton na GPU (opt-in) + glitch de sprite = corrida na VRAM do no-wait →
  `re.Wait` curto; regrediu o Shenmue II 19% → **wait por página de VRAM (4.87)**.
  As "regressões" do kofevo/Shenmue II eram leitura de cena.

### 2026-09-28 — bateria Naomi cold boot

- Inspeção visual um a um (`game_status.md`): 6 não bootavam.
- GD-ROM Naomi: descriptografar o ROM.BIN inteiro → **lazy loading por segmento**
  portado do upstream: cvs2 22 s → 1 s (4.89).
- Crash do cvs2 com tier2: exceção de FPU no decode clobberava `next_pc` no
  `rdv_LinkBlock` (4.91).
- Tier2 travava o cold boot dos carts → gatilho de fim de boot = **handover
  BIOS→jogo** (1º bloco em `0x0C020000`) (4.92). Triângulo laranja do asndynmt
  (padrão `region-bad-ftrv-delayslot`, 4.93) desfeito junto e ainda pendente.
- **Bateria Naomi com tier2 OFF:** 17/19 jogáveis; com ON quase todos quebravam →
  tier2 é perda líquida no Naomi. `RETRORUN_BENCHMARK_KEEP_RUNNING`.

### 2026-09-29 — janela rolante e VMU por jogo

- `RETRORUN_BENCHMARK_ROLLING=1` (últimos 40 s, grava ao fechar). VMU por jogo no DC.

### 2026-10-01 — Napple Tale e a mudança de método

- "340 fps / som desafinado" = emu < 100% + laço do frontend girando com dupes +
  áudio reamostrado (4.97). Dump completo derruba o jogo (4.98).
- Tier2 estava desligado em silêncio no DC desde 09-28 (4.99).
- **Dumper leve `FC_JIT_DUMP_LITE` (4.100)** + `jit_lite_report.py` + `sh4dis.py`.
- **Método novo (vigente):** dump do JIT → análise estática → reescrever em nativo
  → plugar por assinatura SH4. **`lightxf` do Napple nativo (4.101):** bit-exato,
  VEL 67,7 → 72,2%.

### 2026-10-02 — sincronização emu↔render, emissor de strips, captura pelo ES

- lightxf na GPU: 14× mais lenta e divergente → GPU só para o que vai à tela (4.102).
- `sync_emu_render.md` (mapa das esperas, plano em 4 fases). Morton na GPU de
  verdade, opt-in (4.103). `FC_SYNC_STATS`: 35-42% dos frames descartados (4.104).
  Espera com prazo + `RETRORUN_PRESENT_DEPTH=2` (4.105).
- **Emissor de strips do Napple (`8C14D440`) nativo (4.106):** VEL 77 → 87%.
- Instalado como oficial (core `037a942b`, retrorun `2703a59`).
- `tools/rr_capture.sh` no `dreamcast.sh` (uma pasta por sessão em `/roms2/dcbat/`);
  `RETRORUN_BENCHMARK_ALLOW_HOTKEYS`. Crashes do tier2 no DC (4.107-4.109):
  Grandia II e Macross bootam.
- Socket de controle `FC_CTRL_PORT` + `tools/fc_ctrl.py` (`ctrl_socket.md`);
  `FC_INPUT_SCRIPT`. FMV: IDCT da Sofdec ~25% da emu (`fmv_plan.md`).

### 2026-10-06 — fps medido do jogo, `div32` nativo

- Grandia II 50 fps "lento" = dupes do prazo fixo de 20 ms → **espera 2× o
  intervalo medido entre frames novos (4.110)**: dupes 40% → 0,08%; mbaa
  "mais perfeito". Skill `rodar-debug`.
- **`div32u/s` inline no ARM64 (4.111):** bit-exato; empate no Napple (0,8%).

### 2026-10-07 — áudio, fps natural, catálogo do RetroRun, tier2 aposentado

- **WSOLA no backend SDL do retrorun (4.113):** som deixa de desafinar abaixo de
  100%. "Corrigiu" (usuário).
- **Core reporta o fps natural do jogo** (vblanks emulados/frame em janela de 1 s,
  4.112); frameskip compila no SDL.
- Product number via `flycast_retrorun_get_product_number_v1` +
  `core_variant_v1` (`link.T` liberado); catálogo do retrorun mapeia para
  `flycast2026_*`; perfil do Napple (`HDR-0079`). Skill `cross-compile-r36`.
- **A/B tier2 pelo ES: EGG, Shenmue e SA2 só quebram com ON → tier2 aposentado
  (opt-in, OFF por padrão).** TR Chronicles preso com OFF também (família
  frame-wait/CHD). `padroes_ineficiencia_analise.md`.
- HLE `memset`/`ocbp`: bug do `next_pc` corrigido (vaddr do bloco); Le Mans
  `core_p95` 95 → 58 ms (5.4). `tas.b` nativo (fallback → 0).

### 2026-10-08 — HLE do DOA2 e inline cache do despacho

- **HLE do laço de vértices do DOA2 (`8C101BC4`, 5.6):** bit-exato (2 pares, 0
  diferenças), mas **~1%** — o spill de estado a cada fronteira de bloco come o
  ganho (~300 vs ~390 instr/vértice).
- **Inline cache do despacho (`FC_DYN_CACHE`, 5.7):** crash era `.o` velho; 6× mais
  lento era invalidação incompleta (a tabela `fpcb` muda sem discard) → bump da
  geração em toda escrita. Bit-exato; frames +11%, core −7%, p99 −16%. VEL do
  retrorun não confiável aqui. Opt-in.
- **Deploy oficial:** build do HEAD (HLE do DOA2 + inline cache opt-in)
  deployado como core oficial do device
  (`~/.config/retroarch/cores/flycast2026_libretro.so`, md5
  `dceb2a4cfd691bf21c6d4a6c137f518a`), backup em
  `.bak-pre-dyncache-20261008` (build anterior de 2026-10-06); boot DOA2
  validado (exit=0, 0 SIGSEGV). `FC_DYN_CACHE` segue **desligado por padrão**.
- `history.md` compactado (este arquivo); original em `docs/arquivo/`.

### 2026-10-08 (noite) — revisão: prioridade por tempo amostrado

- Revisão (sem mudar código) dos commits de JIT recentes contra as 7 capturas
  `dcbat_off` (com `samples.txt.gz`): `tas.b` 0,0% da emu, `ocbp` ≤1%, `memset`
  2-6%; blocos com FPU são a maior categoria nos jogos <100%. O ranking estático do
  `padroes_ineficiencia.txt` foi aposentado como fonte de prioridade (lição em
  `tech_debits.md`, Método).
- HLE `memset`/`ocbp` e `FC_DYN_CACHE` voltam a "validar" (4.119, 4.120). TR
  Chronicles é MMU saturada, não frame-wait (4.121). AICA no Shenmue II (4.122).
- Fixes pequenos: `getenv` por draw no `SetGPState` cacheado; `jit_lite_report.py`
  usa `--freq` (padrão 299, o `-F` do `rr_capture.sh`) e lê `.gz`.
- **A/B `retrorun_loop_declared_fps` false × true** (savestate, 20 s + 8 s,
  `perfmax`, core oficial; `/roms2/dcbat/ab_ldf_*`): CvS2 new_fps 58,4 → 55,6 (o
  core declara 55 e o pacing trava nisso), KOF Evo 59,3 → 57,0, Shenmue II empate
  (VEL 73,5 × 73,0). Não liga global; o ganho do Napple não generaliza (4.112).
- **Fps declarado só 30/60 + `loop_declared_fps=true` no device** (4.112): o core
  declara o refresh ou metade, com histerese; CvS2 e KOF Evo agora declaram 60
  (CvS2 55,6 → 57,4 fps novos com `true`), Shenmue II e Napple 30. Core oficial
  trocado (md5 `39bb9db6`), backups `*-20261008` em `/roms2/backups/`.
- **Ritmo de 30 fps passa a ser do core, não opção do retrorun** (4.112): com
  alvo 30 o `retro_run` dorme até o prazo; nos de 60 não segura. Napple 30,2
  fps novos com `loop_declared_fps=false`, KOF Evo de volta a 59,0. Cfg do
  device volta a `false`; core oficial md5 `ea43aa08`.
- **Opções provadas boas viram comportamento** (levantamento de todas as `FC_*` e
  core options): quase todas já eram padrão sem variável. Faltava o clock do SH4:
  core option `sh4clock` removida, clock fixo em `d10` (1,0; override por jogo do
  `lut_games` continua). KOF Evo idêntico (59,0 novos, VEL 100). Linha
  `flycast2026_sh4clock` tirada dos cfgs do device; core md5 `7583b53d`.
  `FC_TEX_GPU_MORTON` (4.103) e `FC_DYN_CACHE` (4.120) seguem opt-in até validar.
- **Link direto do despacho dinâmico (4.120)** reimplementado sobre o link dos
  blocos estáticos (o `FC_DYN_CACHE` antigo não fazia branch direto) e padrão.
  Corrigido bug de religar bloco morto após limpeza do cache (corrompia código).
  Bit-exato; DOA2 +4% fps novos, Shenmue II +0,8 VEL. Classificador 30/60 trocado
  para fração de frames de 1 vblank (o DOA2 era declarado 30). Core md5 `0295f22b`.

### 2026-10-09 — FMV do Evolution 1: não é pacing, é a Sofdec saturando a emu

- `FC_FPS_LOG=1` (novo, opt-in): por segundo, frames do TA por vblanks gastos,
  frames de framebuffer, RTT, trocas de página e o fps declarado. Na FMV o jogo
  desenha pelo TA em todo vblank (declarado 60, certo), mas a emu faz só 44-50
  vblanks/s (~78%), saturada na decodificação da Sofdec (4.123) → caminho é o
  `fmv_plan.md`.
- Teste "travar FMV em 30" (`FC_FORCE_TARGET_FPS=30`, diagnóstico): VEL da FMV
  80,0 → 50,1%. A emulação fica presa ao consumo de cada frame; não vira padrão.
- **FMV a 100%** (4.123): o player da Sofdec gira em espera; com o conversor YUV
  ativo o JIT repõe 2/3 dos ciclos por fatia (clock 1,5 só no vídeo).
  Evolution 1 78,9 → 100,2% (vídeo 24 → 30 quadros/s), RE CV FMV ~96%; DOA2
  idêntico por hash fora de FMV. Usuário: vídeo perfeito, áudio certo. Core md5
  no device atualizado; backup `*-pre-fmvclock-20261009`.
- **Chão faltando no Evolution 1 (4.124):** o chão é translúcido em strips longos;
  o sort per-strip (chave = ponto mais distante) desenha um polígono preto curto
  por cima. Per-triangle corrige (30 → 25,9 fps). Ferramentas novas: pick de
  polígonos por pixel (`FC_DBG_PICK`) e chaves de render para isolar culpados.
- **Sort dos translúcidos (4.124):** strips profundos quebrados em pedaços de 2
  triângulos + translúcido ordenado sem escrita de Z. Chão do Evolution 1 completo,
  Napple com bem menos defeitos (usuário), custo ~0 (render +0,1 ms). Core oficial
  md5 `5525c6b0`, backup `*-pre-trsort-20261009`.

### 2026-10-10 — threads nos jogos de DC (varredura dos dumps)

- Pedido do usuário: achar nos dumps de `/mnt/1TB` a lógica de threads. Varredura de
  60 dumps por `rte`/`ldc SSR/SPC`/bancos/`tas.b`: biblioteca de troca de contexto
  completa em 10 jogos (mesmos bytes); ~0,5% da emu onde há amostra. Os "spinlocks
  `tas.b`" do `padroes_ineficiencia_analise.md` são try-lock do SDK (corrigido lá).
  Lição em `tech_debits.md`. `sh4clock`: d12 era o padrão antigo, d10 ganho geral,
  d8 piorou a maioria (lição corrigida).

- **Código de SDK (mesmo dia):** o try-lock do DOA2 envolve os comandos Maple (r5 =
  0x01/0x0A/0x0B/0x0C/0x0E; 19 jogos). As funções já nativas são de biblioteca:
  `lightxf`/`stripemit` em Evolution 1/2, RE CV e Skies; o laço do DOA2 em MvC2 e
  Shenmue II (prefixo). `hle_fn` só instala em endereço fixo → 4.125,
  `docs/native_sdk_code.md`.
- **`docs/sdk_blocks/`** (gerado por `tools/sdk_blocks_doc.py`): um documento por
  família de bloco de SDK com a listagem de cada jogo. `lightxf`, `stripemit` e o laço
  do DOA2 idênticos nos outros jogos (no trecho executado); threads do Le Mans e do EGG
  são variantes.
- **Varredura automática (`tools/sdk_find.py`, `docs/sdk_find/`):** 21 jogos de DC dos
  dumps existentes, 50.561 funções, 2.798 grupos em 2+ jogos. Funções disjuntas,
  contenção em vez de Jaccard (o dump é parcial), SHIL como visão sem ordem.
  Reencontrou `lightxf`/`stripemit`/laço do DOA2; novos alvos de T&L no Shenmue II,
  MvC2 e DOA2 e cópias de memória em 8-10 jogos.
- **Pseudo das 8 mais valiosas (`docs/sdk_find/pseudo/`):** T&L dos jogos de luta
  (004/005/007/011; 004, 005 e o laço nativo do DOA2 são modos de uma rotina, despacho
  por r8), IDCT da Sofdec em 7 jogos (010), cabeçalho de polígono Kamui2 em 13 jogos
  (012), e 008/009 que eram espera, não cópia (009 lê o TMU0 → 4.126). Paridade de r8
  corrigida na revisão. Nada validado com FC_STATE_HASH ainda.
- **Plano `jit_hot_path` (4.127):** medição nos dumps + `perf` (`tools/jit_hotset.py`):
  50% do tempo em blocos cabe na L1I (8-25 KB; Shenmue II 72 KB), 90% pede 80-345 KB;
  frio + literais ~15-20% de cada bloco quente; empacotar só o quente corta ~25-33% das
  linhas tocadas. Fases em `docs/jit_hot_path.md`.
- **`current_plan.md` zerado** (anterior em `docs/arquivo/current_plan_2026-10-08_a_2026-10-10.md`;
  itens abertos conferidos no `tech_debits.md`, novos 4.128 Skies Disc 2, 4.129 Le Mans,
  5.8 spill do DOA2). Plano novo: A `hle_fn` relocável → B nativizações do SDK → C
  esperas fora do idle FF → D caminho quente do JIT.
- Plano: etapa 0 = **remoção do tier2** (pedido do usuário; ~3.000 linhas + ganchos em 11
  arquivos, neutra porque já está OFF).
- **Plano futuro `docs/refactor_x64.md`** (fora do `current_plan`): o x64 é o backend
  original (nenhum `FC_*`, sem idle FF, `hle_fn`, link dinâmico, anti-SMC) e faz
  `ftrv`/`fipr` sem fusão — hash não comparável com o ARM64. Fases: fatorar o comum,
  portar (correção → esperas → velocidade → diagnóstico → nativo), semântica de float
  única (hash PC = R36), caminho quente.

## 2026-10-10 — Etapa 0 do plano: remoção do tier2 (neutra, validada)

Pedido do usuário (plano 0): tirar o segundo compilador de regiões, aposentado em
2026-10-07 (opt-in OFF). Removidos `core/rec-ARM64/tier2.cpp` (3.073 linhas),
`tier2_doa2.S` (641) e `tools/tier2_gen.py` (910); ganchos em 11 arquivos
(`rec_arm64.cpp` com `tier2_entry_for`/`t2_doa2_*`/`t2_no_update`/`t2_last_pc`,
`driver.cpp` `tier2_code_reserve`, `blockmanager.cpp`, `decoder.cpp`
`coop_kernel_detect`/`region_bad_pattern`, `sh4_interpreter.cpp` `tier2_poll`,
`sh4_interrupts.cpp`, `Renderer_if.cpp`, `nullDC.cpp`, `libretro.cpp`/`common.cpp`);
core option `flycast2026_tier2` e as variáveis `FC_TIER2_*`. Ferramentas
(`jit_lite_report.py --log`, `dcbat_consolidar.py`, README do harness) e 5 skills
atualizadas.

**Validação (core novo `/home/ark/flycast_tier2rm.so` × oficial de 2026-10-09):**
`FC_STATE_HASH`+`FC_RTC_FIXED`+`FC_INPUT_NEUTRAL`, cold boot (cfg oficial, tier2
desligado), frame a frame **idêntico** — DOA2 1241 quadros, Shenmue II 397, Napple
760, 0 diferenças (RAM/VRAM/ARAM/ctx a cada 60). Boot ok em MvC2, cvs2 (Naomi) e
kofxi (Atomiswave). Detalhe e lição no `tech_debits.md`; números do tier2 seguem em
`docs/arquivo/`.

## 2026-10-10 — Etapa A: `hle_fn` relocável (4.125)

As funções nativas são de **biblioteca do SDK** (mesmos bytes em vários jogos), mas
o `hle_fn` só instalava no endereço fixo do Napple/DOA2. Agora casa por **bytes em
qualquer endereço**: `Span` na base canônica, match em `base + (addr − canonBase)`,
base = `vaddr − entryOff[entrada]`; `ENTER`/`BAIL`/literais PC-relativos viram
`base + offset` (literal pool lido da RAM). Match completo de todos os spans (sem
falso positivo). Só `hle_fn.cpp` mudou; ganchos do JIT intactos.

**Validado (FC_STATE_HASH+FC_RTC_FIXED+FC_INPUT_NEUTRAL):** Napple 760 quadros e
DOA2 1249 idênticos ao core anterior (HLE ainda instala na base canônica); **Shenmue
II** (savestate) com o laço do DOA2 na base `8C1D8D22`: HLE on × off **idênticos**
(283 quadros) — o Shenmue II agora usa o nativo. MvC2 e Evolution 1 têm os bytes
idênticos (confirmado no dump), mas o savestate não executa a função (falta cena).
Falta A/B de 2 rodadas no Shenmue II e ligar as demais cenas (4.125).

## 2026-10-10 — Etapa B1 (parcial): strips com luz difusa (r8 == 0) nativos

Modo `r8 == 0` da rotina de T&L da biblioteca dos jogos de luta/AM2 (grupo 005 do
`sdk_find`; pseudo `docs/sdk_find/pseudo/strips_com_luz_difusa_pseudo.cpp`) nativizado
no `hle_fn.cpp` (`stripluz_run`, layout DOA2 = DOA2/MvC2/CvS2/Shenmue II 8C1D8A80).
Reconhecido por bytes (entrada `8C101920` + cabeças A/B); o `Span` ganhou máscara
opcional. `r8 != 0` recusa (o 004 e o clamp seguem no JIT / `doa2_run`).

**Validado** (`FC_STATE_HASH`+`FC_RTC_FIXED`+`FC_INPUT_NEUTRAL`, savestate): HLE on ×
off **idênticos** — DOA2 916 quadros, Shenmue II 355. `stripluz` instala nos dois.
Shenmue II no B1 levemente melhor (VEL 70,7 → 71,8, p99 88 → 78 ms); DOA2 confundido
(cenas divergem). Falta o `r8` ímpar (grupo 004) e as variantes Power Stone/callback.
Item 4.125.

## 2026-10-10 — Render threads (4.131): fundação do 2º contexto EGL

Início da implementação do `docs/render_threads_plan.md` (2 renderers em round robin
para o DOA2/MvC2, render-bound). Etapa 2, fundação: `core/rend/gles/rt_proto.cpp` pega o
display/contexto EGL do frontend (via `dlopen("libEGL.so")`, sem linkar EGL, como o
`hle_gpu.cpp`) e cria um **2º contexto EGL compartilhado** com ele, faz corrente **sem
superfície** (`EGL_KHR_surfaceless_context`; sem pbuffer — pbuffer no GBM derrubou o
kernel) e desenha/lê num FBO próprio. Opt-in `FC_RT=1`, no `glesrend::Init`; não toca o
render normal. Testado no device (DOA2): contexto ok, FBO completo, clear/readback =
64,128,191,255. Falta desenhar o frame real (reset do `glcache` por contexto) e
apresentar. 4.131.
- **Teste de 2 renderers na Mali (4.131 — saiu como 4.130 no commit, número já usado pelo DOA2; `tools/gl_two_threads.c`):** 1 thread 88,8
  frames/s → 2 threads com contextos compartilhados 155,2 (1,75×), custo por draw igual;
  o driver não serializa. A 1ª versão do teste usava pbuffer no GBM e deu oops no kernel
  (GPU travada até o reboot); com janela GBM, todos os modos limpos.
- **Shenmue II, ocupação da GPU (savestate pesado, cfg oficial + auto_load):** GPU
  18-37% (média ~30%) a 520 MHz, render 6,2 + 21,6 ms de ~60 ms, VEL 54,5%, 15,2 fps
  novos → o limite é a emulação, não a GPU (a afirmação "Shenmue é GPU-bound" vinha do
  Shenmue 1 em 09/2026). ARM7 da AICA ~23 ms/frame (4.122). Dados em
  `/roms2/dcbat/gpu_shenmue2/`.
- **Plano futuro `docs/render_threads_plan.md`** (round robin de renderers): caso ideal
  DOA2 (4.130, render ~19 ms > 16,7 ms, descarta ~40%), MvC2 candidato, Shenmue II não.
  Desenho: contextos compartilhados sem superfície (nunca pbuffer), programas por
  contexto, texturas versionadas, RTT/framebuffer serializados, apresentação em ordem.
  O 4.131 (teste de viabilidade) foi renumerado (colidia com o 4.130 do DOA2) e corrigido.

## 2026-10-10 (noite) — Render threads (4.131): round robin implementado (`FC_RT=2`)

Continuação da fundação do DeepSeek (`rt_proto.cpp`, `FC_RT=1`). `core/rend/gles/rt_render.cpp`:
2 workers com contexto EGL compartilhado sem superfície, cada um com 2 FBOs de saída;
estado do render `thread_local` (`_pvrrc`, `glsm`, `glcache`, sorter, programas por
worker); funções GLES3 por `dlsym`. Process em ordem numa cadeia de fences, render em
paralelo; texturas versionadas (nome novo no `UploadToGPU` quando um frame em voo ainda
lê a antiga); sampler objects por worker; paleta/fog/registradores copiados por frame;
RTT e framebuffer na thread principal; apresentação do frame pronto mais novo;
afinidade (emu núcleo 3, workers 1-2). Validação por hash de pixels por id de frame
(`FC_RT_HASH`, `tools/rt_hash_compare.py`): **MvC2 952/952 e DOA2 546/546 idênticos**.
- Bugs no caminho: estado do `glsm` zerado no worker (SIGSEGV no `BindTexture`,
  `glsm_thread_state_init`); `stencil_present` falso sem superfície; textura subida num
  contexto invisível no outro (cadeia de fences); **HUD do MvC2 preto**: versionar antes
  do `Update` criava nome novo vazio quando o `Update` pulava o upload (bytes iguais,
  `FC_TEX_SKIP_UNCHANGED`) — versionar só no upload de verdade.
- **MvC2:** VEL 100 → 100,2%, fps novos 38,8 → 53,4, descartes 34% → 0, core p50/p95/p99
  19,4/44,5/51,0 → 13,7/29,7/33,3 ms; underruns 1 → 13.
- **DOA2:** fps novos 27 → 33,2, mas VEL 90,6 → 73,5%: com todo frame processado, a
  emulação espera mais o Process (`g_emuWaitRe`). Sem a espera 83,5%, com afinidade
  87,8%. Próximo: snapshot da VRAM no lugar da espera.

## 2026-10-11 — Render threads ligadas por padrão; o R36 corta o clock da CPU pela GPU (4.131, 4.132)

- **DOA2 perdia VEL com as threads (90,6 → 73,5%)** porque o frame ficava na fila esperando
  um worker (a main só despacha no ritmo do frontend) e a emulação esperava o Process dele.
  Correções: a emulação entrega o frame direto a um worker livre (`rt_dispatch_pending`),
  o gate da fila descarta o frame se não há worker livre, e o núcleo 3 fica só com a
  emulação. Espera da emulação 1,2 → 0,3 ms/frame; latência fila→worker p95 9 → 0,05 ms.
- **Ligado para todos** (pedido do usuário: sem `if` por jogo; uma opção do core + LUT foi
  feita e descartada antes do commit). `FC_RT=0` volta ao render antigo para A/B.
- Hash idêntico ao render antigo: MvC2, DOA2, Napple Tale (RTT). MvC2 40 → 52 fps novos, p99
  55 → 40 ms, VEL 100%. Shenmue II empata.
- **DOA2 ainda perde VEL (89,2 → 83,2%, rodadas esfriadas) e não é espera:** `perf` mostra a
  emulação ~99% ocupada nos dois modos, mesma distribuição; o clock da CPU é que cai (1478
  → 1344 MHz médios), porque o `power_allocator` do RK3326 (liga a 70 °C) divide a potência
  entre CPU e GPU e a GPU subiu de ~45% para ~70%. Em rodadas seguidas sem esfriar, até a
  base roda a 1,0-1,2 GHz (4.132). Novo script `tools/rtabt.sh` (esfria < 63 °C, loga clock/temp/GPU).
- A GPU **não** está saturada no DOA2 base (~45%), mas gasta ~16 ms por frame: o caminho para
  mostrar os ~50 frames/s que o jogo produz é baixar o custo de GPU por frame.
- **A VEL não é frágil:** o `FC_SYNC_STATS` ganhou `vel_sh4_cycles`, `vel_vblank`,
  `vel_audio_generated` e `vel_audio_delivered` (mesma janela, contados no core). No DOA2
  batem em 0,05% (base 86,74/86,71/86,74/86,74%; rt 81,79/81,76/81,80/81,80%); o core não
  perde áudio. A VEL do retrorun mede outra janela (20 s após 8 s de warmup), mesma diferença.
- **Correção do 4.132:** numa rodada fria o clock rt caiu só 1,8% (1485 vs 1512 MHz) e a VEL
  caiu 4,4 pontos mesmo assim — o corte térmico é parte; o resto deve ser memória
  compartilhada CPU/GPU (GPU 45 → 74%). Hipótese, falta medir.

## 2026-10-11 — DOA2: o render gastava GPU à toa com `gl_FragDepth` (4.133)

- O usuário: "não é produtivo pôr a culpa no hardware; tem algo ineficiente no render".
  Medido por eliminação (`FC_SKIP_LIST`, nova; o timer de GPU da Mali deu ~0,3 ms para
  qualquer trecho, até vazio, e foi descartado): DOA2 16,6 ms de GPU por frame, opacos
  ~10 ms, 320×240 6,7 ms → custo de pixel. Causa: todo shader de pixel GLES3 escrevia
  `gl_FragDepth` (log2), o que desliga o descarte antecipado de pixel escondido da Mali.
- Correção: profundidade no vertex shader (linear no 1/w, escala por frame — o caminho
  GLES2 que já existia). DOA2 **16,6 → 7,2 ms de GPU por frame**, GPU 45 → 20% no mesmo
  fps. Imagem ≤3 px/frame diferentes no DOA2, ~13 no MvC2, igual a olho nu no Shenmue II.
- DOA2 rt + profundidade no vértice: **44 fps novos** (base 27), descartes 54 (base 650),
  frame p50/p95/p99 19,4/34,4/45,6 (base 34,9/44,2/58,7), GPU 42%; VEL 82% (base 90,6%):
  agora é a CPU do render (~640 draws/frame em 2 threads) que tira clock da emulação.
- `FC_SYNC_STATS`: a janela de VEL recomeça na carga do savestate (o relógio emulado
  saltava e dava 313%).

## 2026-10-11 — Opacos agrupados por estado: menos draws (4.134)

- `FC_REND_SPLIT` no DOA2: 643 draws/frame, 497 com uma strip só, 359 trocas de textura;
  lotes quebrados por textura (184/frame), `tsp` (168), `isp` (114). O jogo alterna texturas
  nos opacos e a fusão de vizinhas iguais não pega.
- Os opacos com Z `GREATER`/`GEQUAL` e escrita de Z são ordenados por estado antes do lote
  (estável; outros modos ficam como barreira). DOA2: draws 643 → 406, opacos 9,9 → 5,9 ms,
  render 16,7 → 12,7 ms de CPU por frame; imagem ≤1 pixel diferente (16/19 frames iguais).
- Com as threads: fps novos 45,5 iguais, frame p95/p99 34,9/43,5 → 32,1/42,4 ms, **VEL igual
  (85,0 → 84,4%, ruído)**. Corrige o que foi dito antes: a perda de VEL do rt (base 90,6%)
  não é a CPU do render. A emulação fica mais lenta quando o jogo desenha mais frames;
  hipótese aberta: memória compartilhada (4.132).
- `FC_REND_SPLIT` derruba a VEL (DOA2 ~54%): os números de render dele valem para comparar,
  a VEL não.
- **Leitura corrigida pelo usuário:** os 90,6% do render antigo eram velocidade inflada — ele
  descartava 47% dos frames de um jogo de 60 (27 fps novos) e desenhar frames comprovadamente
  deixa a emulação mais lenta. Com as threads o DOA2 desenha 95% do que produz: 84,4% é a
  velocidade honesta e passa a ser a referência. Regra: VEL só se lê junto com os descartes.

## 2026-10-11 — Por que desenhar mais frames deixa a emulação mais lenta (4.132)

- Mesmo build, 1 thread de render, clock 1512 MHz cravado, só a taxa de desenho muda
  (`FC_RENDER_BUDGET_MS=36` limita a ~21 frames/s): VEL 90,7% (21,5 desenhados/s) × 86,7%
  (36,5/s). Contadores de hardware só na thread da emulação (`tools/rtpmu.sh` +
  `tools/rtpmu.py`), por segundo emulado: instruções 668 × 665 M (iguais), faltas no L2 4,4 ×
  4,5 M (iguais), ciclos 1404 × 1474 M (+5%), IPC 0,476 × 0,451; faults e trocas de contexto
  desprezíveis. Mesmo trabalho e mesmas idas à RAM; cada ida fica ~15 ciclos mais cara com o
  tráfego do render. Ciclos +5% ≈ VEL −4,4%.
- Alvos: menos bytes por frame no render; menos faltas de L2 na emulação (4.127).

## 2026-10-11 — Caminho quente do JIT, fase D1: cauda fria (4.127)

- `rec_arm64.cpp`: fim de fatia (`intc_sched`), falha da checagem de código e exceção de
  FPU saem do meio do bloco para uma cauda fria depois do fim do bloco (`EmitColdTail`). A
  entrada do bloco troca o `b.pl` tomado (pulando ~6 instruções) por `b.mi` não tomado.
  `FC_JIT_COLD=0` volta ao layout antigo.
- Bug achado no caminho: o `rdv_LinkBlock` encolhia o `host_code_size` para terminar no
  relink; a cauda saía do bloco e o `rdv_DoInterrupts` (que acha o bloco pelo endereço de
  retorno da chamada ao `intc_sched`) dava ponteiro nulo. Agora não encolhe.
- Estado idêntico (`FC_STATE_HASH`, comparado por ciclo emulado — `tools/sthcmp.py`; a
  numeração de frames pode deslocar 1 entre rodadas): DOA2 966/966, Napple 942/942, Shenmue
  II 771 (RAM/VRAM/ARAM/ctx iguais em todos).
- Shenmue II (contadores na thread da emulação, `tools/rtpmu.sh`): VEL 54,3 → 55,1%, ciclos
  por segundo emulado −1%, **faltas de L1I 10,5 → 10,0 M por segundo emulado**. Esse número
  (~7-9% do tempo esperando código) é o alvo da D3 (arena quente).

## 2026-10-11 — Shenmue II: os ~16 fps eram cfg de debug velho (DSP ligado)

- Os testes de hoje no Shenmue II usaram `r_shenmue2_gpu.cfg` (cópia do oficial do dia 10,
  com `flycast2026_enable_dsp = enabled`); o oficial está com o DSP desligado desde o dia 10.
  Novos cfgs de debug = oficial + `retrorun_auto_load = true`: `/home/ark/r_cur_{doa2,mvc2,
  shenmue2,napple}.cfg`. As comparações A/B de hoje continuam válidas (mesmo cfg dos dois
  lados), o nível absoluto do Shenmue II não.
- Shenmue II, mesma cena, DSP desligado: core oficial VEL 70,8%, 21,3 fps apresentados (19,9
  novos), frame p50/p95/p99 44,0/63,7/80,5; build atual 73,5%, 22,1 (20,6 novos),
  42,4/65,6/76,4. Sem regressão; bate com os ~22 fps do DeepSeek.
- `tools/rtabt.sh` aceita `CORE=<.so>` para medir outro core.

## 2026-10-11 — Plano mmap/MoE/MTP; fase 1a: DSP no render do som em lote (4.122)

- Plano novo `docs/mmap_moe_mtp_plan.md` (técnicas inspiradas em LLM + dual core do Dolphin):
  fase 1 ganhos rápidos (DSP em lote, parar de limpar o cache por uma falha de checagem), fase 2
  arena quente bloco a bloco (a D3 por limpeza ficou desligada, `FC_JIT_HOT=1` só diagnóstico:
  travava 0,15-1 s e mudava as fronteiras dos blocos), fase 3 ARM7 em thread própria 1 quantum
  adiantado. Cache de código em disco descartado (ponteiros absolutos por execução).
- **DSP em lote:** DSP ligado forçava `NoBatch=1` → mixagem + DSP por amostra na emu thread
  (~23 ms/frame). Agora o render do som (thread já existente do 4.44) acumula `MIXS[ISEL]` por
  amostra, roda o `dsp_step` e soma o EFREG; `dyndirty` atômico; a fila é drenada antes do
  `FC_STATE_HASH`. Shenmue II com DSP, mesma cena: **VEL 57,0 → 73,9%**, fps 17,1/16,3 →
  22,3/20,7 (apresentados/novos), frame p50/p95/p99 55,7/78,2/92,1 → 42,0/67,5/87,3. Estado: 2
  rodadas em lote idênticas (762 frames); áudio lote × lote idêntico, lote × por amostra com o
  mesmo nível (RMS ±2%) e as vozes deslocadas até ~30 amostras (o controle já é por bloco de 32).
- **Bug achado:** o render em lote tocava a voz só-DSP com `oDsp` em vez de `oDsp >> 4` (16×
  mais alto) — candidato ao "som estourado" sem DSP. Corrigido.
- Cfg de debug com DSP: `/home/ark/r_cur_shenmue2_dsp.cfg`.

## 2026-10-11 — Fase 1b: limpezas do cache contadas — nada a corrigir agora

- `FC_SYNC_STATS` ganhou `cache_clears` por motivo (falha de checagem, cheio/boot, externa,
  reset, arena) e o tempo total. DOA2, Shenmue II, MvC2 e Napple (~50 s cada, savestate): **0
  limpezas por falha de checagem**; só as 4 da partida (boot, load, 2 resets), 4-9 ms no total
  com o cache quase vazio. Os 120-140 ms medidos antes eram a limpeza com o cache cheio (a D3
  por limpeza). A troca do `rdv_BlockCheckFail` por descarte por página fica para quando um
  jogo mostrar limpezas durante o jogo.

## 2026-10-11 — Fase 2 (MoE): arena quente bloco a bloco — funciona, não ganha

- Promoção bloco a bloco para o `TempCodeCache` (1 MB, ocioso sem MMU): seleção pelas amostras
  do `intc_sched` (95% das amostras, até 1500 blocos; `FC_JIT_HOT_PCT`/`FC_JIT_HOT_MAX`), fila em
  ordem de cadeia, descarta e recompila no mesmo endereço/`fpu_cfg`, 1 ms por ponto seguro
  (`rdv_DoInterrupts`), troca de época quando a arena enche. **Estado idêntico** a
  `FC_JIT_HOT=0` (DOA2 494, Shenmue II 532, Napple 536 frames). DOA2 1615 blocos/281 KB,
  Shenmue II 2074/352 KB.
- **Piorou** (Shenmue II, `rtpmu.sh`): sem arena VEL 71,1%, L1I 7,3 M/s emulado, iTLB 0,12 M;
  arena 95% 66,5% / 9,3 M / 0,19 M; arena 50% (140 blocos) 69,9% / 8,7 M / 0,11 M. Desligada por
  padrão.
- `perf record -e r01` (falta de L1I) na thread da emulação: 76,5% no código do JIT do SH4,
  5,4% `hle_fn_run`, ~4% kernel, ~2,7% ARM7. O JIT já põe junto o que roda junto (compila na
  primeira execução); o limite é o tamanho do código quente (250-350 KB contra 32 KB). Próximo
  alvo da etapa D: D4, menos bytes por instrução SH4.
- Nota de método: no `sthash.sh` a primeira rodada de cada par carrega o jogo a frio e avança
  menos tempo emulado; o estado vale (comparado nos frames em comum), a velocidade não.

## 2026-10-11 — Fase 3, portão 3.0: o SH4 não toca nos registradores do som; escreve muito na ARAM

- Contadores `sh4_aica_por_s_emulado` no `FC_SYNC_STATS` (registradores por tipo, ARAM, DMA G2)
  e o diagnóstico `FC_ARAM_COUNT=1` (ARM7/DSP num apelido da ARAM em 0x20000000; as visões do
  SH4 ficam `PROT_NONE`, cada acesso vira chamada do handler da área 0 e é contado).
- Por segundo emulado: registradores = 0 nos 4 jogos; ARAM DOA2 44.298 escritas / 62 leituras,
  MvC2 44.316 / 192, Shenmue II 0, Napple pouco (DMA G2 3,6/s). Desenho do dual core
  ajustado: escritas do SH4 na ARAM em fila aplicada no início do quantum seguinte; leituras
  drenam.
- Usuário sobre o DOA2: "é outro jogo já desde ontem, chegamos a 50 fps e visual mais fluido".

## 2026-10-11 — Fase 3: dual core do som (ARM7 + vozes em thread) — Shenmue II +13 VEL

- **Feito:** o ARM7 + o controle das vozes saem da thread de emulação para uma thread
  própria, um quantum (32 amostras) por vez, com **folga de N quanta** (`FC_AICA_DUAL=N`,
  `FC_AICA_DUAL_CPU` = núcleo; padrão fora do 3). A emu entrega as escritas do SH4 na ARAM
  por fila (aplicadas no início do quantum seguinte), entrega a interrupção do SPU em T_{k+N}
  e dispara o quantum; **leituras de ARAM e pontos de sincronia (registradores, DMA G2,
  savestate, hash) drenam**. Commit `32d5a04c9`.
- **Bug corrigido no caminho (despertar perdido):** o `notify` da fila do mix fora da trava
  podia cair entre o worker conferir a fila e dormir → `aica_mix_sync` esperava para sempre
  (hang de 2 quadros no N=32). Os `notify` passaram para dentro da trava (`sgc_if.cpp`).
- **Medido** (`tools/rtabt.sh`, mesmo savestate, 20 s + 8 s, `perfmax`, rodadas curtas):

  | Jogo (cfg) | modo | VEL% | apres | novos | core p50/p95/p99 | und |
  |---|---|---|---|---|---|---|
  | Shenmue II (DSP off) | sync | 72,8 | 22,0 | 20,36 | 41,4/61,6/75,7 | 26 |
  | | **dual** | **86,4** | 25,9 | 24,26 | **33,6**/57,2/71,4 | 21 |
  | Shenmue II (DSP on) | sync | 73,0 | 21,9 | 19,26 | 41,1/66,3/79,2 | 23 |
  | | **dual** | **85,8** | 26,0 | 24,07 | **33,9**/56,2/75,6 | 19 |
  | DOA2 | sync | 84,8 | 49,9 | 45,89 | 17,3/28,6/41,9 | 17 |
  | | dual | 85,2 | 50,0 | 45,98 | 17,0/31,2/42,6 | 20 |
  | MvC2 | sync | 100,0 | 59,8 | 53,65 | 8,6/29,9/34,6 | 3 |
  | | dual | 100,0 | 59,4 | 55,34 | 9,3/31,2/36,5 | 2 |

- **Leitura:** ganho grande e limpo no **Shenmue II** (+13,6 pts com DSP off, +12,8 com DSP
  on), igual nos dois (a fase 1a já tirou o DSP da emu; o dual tira o ARM7 + controle). O
  Shenmue II quase não toca a ARAM (portão 3.0 = 0), então o dual quase não drena (28
  drenagens, 33 ms). **DOA2/MvC2 neutros:** as **leituras de ARAM** do SH4 (62/s e 192/s)
  fazem drenar e o custo come a folga (DOA2 2,4 s de espera, MvC2 11,4 s), sem derrubar a
  VEL.
- **Passo 1 (reduzir as drenagens) medido:** `lag=4` corta o custo por espera ~6-18× mas
  multiplica as esperas (DOA2: drain 2431 → 2153 ms, mesma VEL 85,2; MvC2 ~11,5 → 10,5 s,
  VEL 100). **Pinning no núcleo 0 piora** o DOA2 (85,2 → 81,3). As drenagens **não derrubam
  a VEL** — comem folga. Mantido `lag=32` (melhor para o Shenmue II, o alvo). Fix real das
  drenagens exigiria a thread do som com visão própria da ARAM (trabalho futuro).
- **Validação:** `FC_STATE_HASH` bit-exato (Shenmue II N=32×N=32 826 quadros e sync×N=32 792,
  DOA2 952, MvC2 1238 — 0 diferenças); **áudio bit-idêntico** ao síncrono (RMS/pico iguais,
  `dif rms 0`, defasagem 0 amostras em 5 trechos).
- **Opt-in** (`FC_AICA_DUAL=0` volta ao síncrono). Ligar por padrão fica a critério do
  usuário (o ganho é do Shenmue II; DOA2/MvC2 neutros).
- **Deploy (2026-10-11, pedido do usuário: "oficial com opt-in ligado nas configs"):** o core
  oficial `flycast2026_libretro.so` = o build do dual core (md5 `30c5e1a1`; backup
  `fea67b12` em `/roms2/backups/flycast2026_libretro.so.bak-pre-dualcore-20261011`). O opt-in
  ficou **ligado no `rr_capture.sh`** (o wrapper que o `dreamcast.sh`/`naomi.sh`/
  `atomiswave.sh` usam no `retrorun3`): `FC_AICA_DUAL=${FC_AICA_DUAL:-32}
  FC_AICA_DUAL_CPU=${FC_AICA_DUAL_CPU:--1}` (backup `rr_capture.sh.bak-pre-dualcore-20261011`);
  dá para desligar por jogo com `FC_AICA_DUAL=0`. Smoke com o core oficial + env (MvC2):
  dual ativo (30.295 quanta), sem crash nem watchdog.
- **Instrumentação desligada do caminho do ES (2026-10-11, pedido do usuário):** os três
  launch scripts (`dreamcast.sh`/`naomi.sh`/`atomiswave.sh`, branch `retrorun3`) chamavam
  `rr_capture.sh`, que liga `FC_JIT_DUMP`/`FC_JIT_DUMP_LITE`/`FC_PERF_MAP`/`FC_SYNC_STATS`,
  roda `--benchmark 40` e um `perf record` **em todo boot** (criava uma pasta em
  `/roms2/dcbat/`). Agora chamam **`/usr/local/bin/retrorun3` direto**, só com os env de
  apresentação + `FC_AICA_DUAL` (backups `.bak-pre-instr-off-20261011`). `retrorun_auto_load =
  false` já estava na config oficial. Smoke pelo caminho novo: nenhuma pasta de captura criada,
  sem benchmark no log, jogo sobe. `rr_capture.sh` fica só como ferramenta (não é mais chamado
  pelo ES).
- **Revertido no mesmo dia (pedido do usuário: "ligue a instrumentação de volta"):** os três
  launch scripts voltaram a chamar `/home/ark/rr_capture.sh` (a versão com captura). O
  `FC_AICA_DUAL` continua ligado (o `rr_capture.sh` exporta `FC_AICA_DUAL=32
  FC_AICA_DUAL_CPU=-1`).
- **Limpeza dos dados de instrumentação do device (pedido do usuário):** removidos
  `/roms2/dcbat/*` (capturas, 1,1 GB), `/roms2/jitdump_napple*` (148 MB), `/roms2/dcbase`,
  `/roms2/cold`, `/roms2/ab`, os arquivos de medição de `/roms2/dumps` (json/log/cfg/data/gl) e
  de `/home/ark` (420 `.log`, 93 `.json`, `.data`/`.txt`/`.gz`/`.map`/`.pcm`/`.raw`). **Movidos**
  os 49 cores de teste (`/home/ark/*.so` + `/roms2/dumps/*.so`) para
  `/roms2/backups/old_test_cores/`. **Preservados** `/roms2/dumps/savestates_backup` (1,0 GB de
  savestates), os cfgs/scripts do `/home/ark` e `dcbat_tools`. `/` 7,1 → 6,5 GB usados; `/roms2`
  ~1,5 GB liberados.

## 2026-10-11 — Regressões do deploy: RT causa glitches; crash no stub da SQ

- **Relato do usuário (jogando com o deploy novo):** crashes aleatórios em Daytona USA, EGG,
  MvC2 (após um tempo) e SA2; glitches visuais em Napple Tale e Crazy Taxi (canto da tela no
  menu de pause) e no Shenmue (Ryo e o caderno); Sega Rally (WinCE) não boota.
- **Os dumps de captura guardaram os crashes** (`/roms2/dcbat/20261011-*`): EGG, Daytona,
  MvC2 e SA2 com `exit 133`/SIGSEGV/DEBUGBREAK. **Todos o mesmo:** a instrução que falta é
  `stp x2,x3,[x13]` (`A9000DA2`) dentro do `ta_sq_stub` ("SQ->TA direto", 4.36), com
  `acc=ctx.x0=0xe0000000` (a Store Queue) e o destino `ta_tad.thd_data` fora de qualquer mapa
  -> o `thd_data`/`thd_root` da TA ficou inválido antes. `ngen_Rewrite` só reescreve `ldr/str`
  de fastmem; não reconhece `stp` -> fault genuíno -> `die()`. **O stub é vítima; não é o HLE**
  (o HLE evita esse stub, usa `TAWriteSQ`). Item 4.135.
- **Bissecção (pedido do usuário, via env no `rr_capture.sh` do ES):**
  - `FC_HLE=0` (HLE off) + `FC_RT=0` (render threads off) -> o usuário testou;
  - `FC_RT=2` (RT on) de volta -> **os glitches voltaram** -> **o RT é a causa dos glitches**
    (usuário). Item 4.131 atualizado.
- **Estado final do ES:** `FC_AICA_DUAL=32` (dual core do som LIGADO), `FC_HLE=1` (HLE ligado),
  `FC_RT=0` (render threads DESLIGADO). Backups do `rr_capture.sh` a cada passo
  (`.bak-pre-hleoff-20261011`, `.bak-pre-rt-off-20261011`, `.bak-pre-rt-on-20261011`,
  `.bak-pre-hleon-rtoff-20261011`).
- **Achado do dual core no cold boot:** com `FC_AICA_DUAL=32`, a BIOS/driver lê a ARAM
  ~170.000x/s no boot (no gameplay era 192x/s) e cada leitura drena -> 1,95 M drenagens em
  15 s (8,2 s de espera; VEL 88,6% no MvC2 cold boot). Custo do dual core na inicialização
  dos jogos com esse padrão; `FC_AICA_DUAL=0` no boot se incomodar.
- **Pendente:** os crashes (4.135) — com `FC_RT=0` ainda não confirmado se somem; e o Sega
  Rally (WinCE/MMU, provavelmente pré-existente).
- **Shenmue: o glitch (Ryo, caderno de notas) é do HLE, não do RT** (usuário, 2026-10-11).
  O HLE é relocável (casa por bytes) e instala no Shenmue; a validação por hash
  (Napple/DOA2/Shenmue II) **não cobriu essa cena**. O usuário lembra que uma sessão anterior
  **já tinha percebido algo errado com o HLE, mas ele pediu para seguir** — fica registrado que
  a ressalva existia e foi adiada a pedido; a causa apareceu no Shenmue. Item 4.125.
- **Split por sistema (pedido do usuário):** Naomi e Atomiswave rodam com
  `FC_HLE=0 FC_RT=0 FC_AICA_DUAL=0` (o `env` do `naomi.sh`/`atomiswave.sh`); o Dreamcast
  segue com `FC_AICA_DUAL=32 FC_HLE=1 FC_RT=0` (defaults do `rr_capture.sh`). Smoke (Naomi,
  `capsnk`): AICA off (0 quanta), HLE off (0 instalações), sem crash.

## 2026-10-11 — GL por sistema: Naomi/Atomiswave no render antigo

- **Contexto:** o usuário reportou que o **RT quebrou tudo** (precisa de reparo) e pediu para
  **desligar as mudanças de GL só no Naomi/Atomiswave** (o Dreamcast mantém). O split anterior
  por `env` (HLE/AICA off no Naomi/Atomis) foi **revertido** — quebrou o MBAA; a lista
  Naomi+Atomis estava perfeita com `FC_HLE=1 FC_AICA_DUAL=32`.
- **Feito (código, `gl_legacy()` em `gles.cpp`):** true quando `settings.System !=
  DC_PLATFORM_DREAMCAST` (ou `FC_GL_LEGACY=1/0` para forçar). Gateia duas mudanças recentes:
  - **4.133** (profundidade no vertex shader, sem `gl_FragDepth`): no legacy, o shader volta a
    `vpos.z = vpos.w` (vertex) + `gl_FragDepth = log2(1+w)/34` (fragment), via
    `#define VERTEX_DEPTH 0` injetado em `findGLVersion`.
  - **4.134** (opacos ordenados por estado): `op_sort_enabled()` devolve false no legacy.
  - **Não toca o RT** (4.131), a pedido.
- **Deploy:** core oficial `f3851b68`; backup `flycast2026_libretro.so.bak-pre-gllegacy-20261011`.
  Smoke (MBAA Naomi e MvC2 DC): sem crash, sem erro de shader.
- **Pendente:** validar com o usuário jogando (Naomi/Atomis) que os glitches/crashes somem;
  e o RT (reparos).


## 2026-10-21 — Deploy no ROCKNIX (device novo): core flycast2026 + retrorun do projeto

- **Device reflashado para ROCKNIX official 20260901** (`root@192.168.0.14`, senha `rock`).
  Muda tudo em relação ao ArkOS: `/` é **squashfs ro**, só `/storage` é gravável; os cores
  do RetroArch vêm do overlay **`/storage/cores` → `/tmp/cores`**; o ES (serviço
  `essway.service`) usa `runemu.sh` e o `start_retrorun.sh` da imagem, com binário
  `/usr/bin/retrorun-sdl2` e core `/usr/lib/libretro/flycast2021_libretro.so` fixos
  (não dá para sobrescrever).
- **Deploy feito:**
  - core do projeto em `/storage/cores/flycast2026_libretro.so` (+ `.info`, aparece em
    `/tmp/cores` e no RetroArch);
  - nosso retrorun em `/storage/bin/retrorun-sdl2`;
  - wrapper `/storage/.config/retrorun/es-launcher.sh`: o `<command>` de
    dreamcast/naomi/atomiswave passou a chamá-lo; ele roteia `--core=retrorun2026` para
    o nosso retrorun + core e **delega todo o resto ao `/usr/bin/runemu.sh`** original;
  - `es_systems.cfg`: `flycast2026` adicionado como core do RetroArch e emulador
    `retrorun: flycast2026` (`core retrorun2026`) nos 3 sistemas (backup
    `.bak-pre-flycast2026-20261021-231335`);
  - `retrorun.cfg`: bloco `flycast2026_*` (o cfg da imagem só tinha `flycast_*`/
    `flycast2021_*`, que o nosso core não lê) e o wrapper exporta o estado validado
    `FC_RT=0 FC_HLE=1 FC_AICA_DUAL=32` (history 2026-10-11).
- **Build do retrorun para o ROCKNIX:** a imagem **não tem headers de dev**. Sysroot novo
  montado com headers de SDL2/curl de `.deb` (amd64; são portáveis) + png/zlib/GLES locais
  e libs aarch64 copiadas do device; `_real_SDL_config.h` do Debian x86_64 tem
  `HAVE_IMMINTRIN_H` e quebra o build aarch64 (remover). **O `libGLESv2.so.2` do Rocknix é
  só o shim `libmali-hook` e não exporta símbolo nenhum**; linkar com
  `--unresolved-symbols=ignore-all` faz o ld resolver os `gl*` como ABS e gera PLT
  `*ABS*@plt` — o binário crasha na primeira chamada GL (SIGSEGV determinístico em
  `retrorun-sdl2+0x10fb0`, "was not in vram"). Solução: linkar `-l:libmali.so.1`
  (o blob real, presente no device) — DT_NEEDED `libmali.so.1`.
- **Smoke:** `ggxx` (Naomi) boota e roda com nosso retrorun+core, sem crash; rodada de
  15 s com o ES ainda renderizando atrás deu core 20,4 ms, VEL 81% (não serve como A/B —
  falta medir com o ES oculto). Wrapper testado ponta a ponta (dispatch para
  `retrorun2026`). Logs em `/storage/test_rr_ggxx.*`, `/storage/test_wrapper.log`.
- **Memory card por jogo (pedido do usuário):** `flycast2026_per_content_vmus = VMU A1`
  no `retrorun.cfg` (backup `.bak-pre-vmu-*`). O retrorun entrega o cfg via
  `RETRO_ENVIRONMENT_GET_VARIABLE`, e o core grava `<jogo>.A1.bin` no **save dir** do
  frontend. `VMU A1` = 1 card por jogo; `All VMUs` = 4 (A1/B1/C1/D1).
  Também posto no `retroarch-core-options.cfg` (`flycast2026_per_content_vmus = "VMU A1"`).
- **Save tem que ficar na pasta da ROM (pedido do usuário, layout do ArkOS):** o `-s` do
  retrorun é o que vira `GET_SAVE_DIRECTORY` do core (VMU per-game, `.srm` e savestates).
  O wrapper passava `/storage/.config/retrorun/saves`, então o Napple Tale "perdeu" o save:
  o core **não apagou nada** — o card antigo continuava em
  `/storage/roms/dreamcast/<jogo>.A1.bin` (md5 `e31fa77b…`, 18/10) e ele criou um card
  novo/vazio no diretório novo. **Fix:** `es-launcher.sh` passa `-s "$(dirname "$ROM")"`.
  Provado escondendo o card e vendo o core criar um novo **na pasta da ROM**; card antigo
  restaurado (md5 idêntico). As cópias que eu tinha feito em
  `/storage/.config/retrorun/saves/` foram removidas (não pode ter save em dois lugares).
  `retroarch.cfg` já tem `savefiles_in_content_dir = "true"` para os outros sistemas.
- **Pendente:** o usuário validar na tela pelo ES (menu Emulador/Core); comparar VEL com a
  linha de `game_status.md`; reintroduzir o `rr_capture.sh`/benchmark no caminho do ES se
  for medir sério.


## 2026-10-22 — ROCKNIX: menu do retrorun, auditoria do cfg e BIOS do Atomiswave

- **Menu do retrorun:** `retrorun_alternative_input_mode = true` no
  `/storage/.config/retrorun/retrorun.cfg` (backup `.bak-pre-altinput-20261022-195219`).
  Com isso o menu é **Select+X** (Select+Y é toggle de FPS; Select+A pausa;
  Select+B screenshot; Select+Start sai; Select+R1/L1 save/load state).
  **Pendente:** o usuário confirmar na tela.
- **Auditoria do bloco `flycast2026_*`:** era cópia do template genérico RK3326 (todos os
  valores iguais ao default do core). Corrigidos os valores inválidos
  `anisotropic_filtering = off` → `disabled` e `texupscale = off` → `disabled`.
  O `frameskip_budget = 33` chegou a ser adicionado e foi **removido em seguida** a pedido
  do usuário (o core volta ao default). Nenhum opt-in do projeto além do `FC_RT`.
- **kofxi (Atomiswave) "crashava" no boot:** o wrapper `es-launcher.sh` passava
  `-d /storage/.config/retrorun/system`, cujo `dc/` não tem BIOS. O core abortava com
  `hw/naomi/naomi_cart.cpp:243 Error: cannot load BIOS. Exiting` e
  `Cannot load kofxi.zip: error 2` (ENOENT do `awbios.zip`). Os BIOS estavam em
  `/storage/roms/bios/dc/` (`awbios.zip`, `naomi.zip`, `naomi2.zip`). **Fix:**
  `SYSTEM_DIR="/storage/roms/bios"` no wrapper (o core appenda `/dc`); backup em
  `es-launcher.sh.bak-bios-20261022-201658`.
  - **Validado:** kofxi pelo wrapper rodou 30 s sem erro de BIOS, áudio ratio 1.016
    (~30 fps, velocidade cheia); Napple Tale (DC) 20 s, 640x480@30, ratio 1.004, sem novo
    OOM do Mali. O `nothing to save in srm file!` no shutdown é normal no Atomiswave
    (usa `.nvmem` na pasta da ROM).

## 2026-10-31 (tarde) — MMU/despacho (4.121): instrumentação, caches e o que o TR Chronicles realmente faz

- **Instrumentação nova (tudo opt-in, custo zero desligado):**
  - `FC_MMU_DIAG=1`: reparte o custo da tradução MMU (caminho LRU / hash / WinCE /
    miss), ciclos por caminho, e conta chamadas do despacho com MMU + quantas caem na
    faixa física direta (P1/P2/P4). Sai no `FC_SYNC_STATS` como `mmu_diag` e
    `dispatch_diag`.
  - `skip_diag` no `FC_SYNC_STATS`: ciclos pulados por cada truque (`idle_ff`,
    `delay`, `scan`, `dt`) — é o que mostra que a VEL% pode estar inflada.
  - `FC_INITPROF`/`FC_TINYPROF=<Hz>`: amostrador de PC do host por `SIGPROF` (o device
    não tem `perf`). Grava PCs crus + `/proc/self/maps` em `/tmp/tinyprof-<pid>.txt`;
    `tools/tinyprof.py <core.so> <arquivo> [--so-dir DIR]` separa por módulo (core,
    libc, driver Mali) e simboliza (com os `.so` copiados do device).
  - `memv ADDR LEN` no socket de controle: lê a RAM por endereço **virtual** (os jogos
    WinCE moram em 0x00xxxxxx e o `mem` só via P1).
  - `ctrl_socket_init()` também no `retro_run` (antes só em `UpdateInputState`, então
    jogo que não lê o controle — TR — nunca subia o socket).
- **Cache de tradução multi-entrada (`FC_ITLB`, padrão ligado) e cache de despacho
  vaddr→código (`FC_VDISP`, padrão ligado)** em `fastmmu.cpp`/`blockmanager.cpp`:
  512 e 4096 entradas diretas, com geração invalidada em flush da MMU, `LDTLB`
  (`UTLB_Sync`), troca de ASID, `bm_Reset`, `bm_ResetCache`, `bm_ResetTempCache` e
  `bm_DiscardBlock`. **Meta alcançada em taxa de acerto:** iTLB 99,97% (7,59 M
  lookups), cache de despacho 93,3%.
- **Mas o TR Chronicles não melhorou — e o diagnóstico do 4.121 muda:**
  - Com os caches ligados o TR **trava mais cedo**: VEL 1,8-2,1% (reproduzível, 3
    rodadas, 1 quadro novo, 7.166 despachos de bloco mas 7,55 M traduções de dados);
    com `FC_ITLB=0 FC_VDISP=0` volta a VEL 20,4%, igual ao core do device
    (21,4 / 23,75 / 18,5%). Ou seja: **os caches mudam o comportamento do jogo** —
    não é só taxa de acerto, tem invalidação faltando (o `FC_ITLB_VERIFY` não pega:
    a busca "completa" consulta as mesmas tabelas UTLB/hash, então uma entrada velha
    é consistente consigo mesma; só comparar com a tabela de páginas do WinCE na RAM
    pegaria).
  - **No estado saudável (20%), o TR não está "saturado" pelo despacho**: 78% das
    amostras de PC caem num único endereço de biblioteca (`8C024704`, o retorno de um
    callback do "system library") — ele gira no kernel/biblioteca do WinCE
    (malloc/lock: `8C01734C..8C0176D2`, `8C02CC40..6E`, `01E42E34`) e faz 1,5 quadros
    novos/s, tela preta. O socket de controle (que só subia em `UpdateInputState`)
    nunca apareceu, indício de que o jogo não chega a ler o controle. O `FC_FB_DUMP`
    não serve de prova do que está na tela (também sai preto no Napple, que funciona).
  - O TR precisa do **BIOS real** (`dc_boot.bin`) para comparar com o core oficial
    (o oficial sai com "Unable to find bios"; nosso fork usa reios/HLE). **O device não
    tem `dc_boot.bin` em lugar nenhum.**

## 2026-10-31 (noite) — NGBC (Atomiswave) a 40-45/60: medido, não é CPU; 25% dos quadros estala ~40 ms

- **Cena:** savestate do usuário (`ngbc.fc2021-rrstate.auto`), ES/cfg oficial, `perfmax`.
  Baselines (rodadas de 60-130 s, `FC_SYNC_STATS`): `new_fps` 39,5 / **44,8** (a rodada de
  26/10), VEL 97,8-99,4%, `vblank` 59,9/s, descartes de apresentação 834/4443.
- **O processo NÃO está saturado de CPU** (por thread, `ps -L`): emulação ~40% de um
  núcleo, frontend ~15%, áudio ~3%, Mali backend ~2% — ~66% de um núcleo no total. E os
  ciclos de bloco executados são **52 M/s emulados** (26% do clock de 200 MHz): os outros
  ~98 M/s de relógio emulado são **pulo do idle fast-forward** (`skip_diag`:
  `idle_ff_cycles` 5,10 G em 51,6 s emulados). Ou seja: a VEL% do NGBC é inflada pelo
  skip; o jogo trabalha 1/4 do tempo.
- **A distribuição de intervalo entre quadros novos é o problema:**
  `p50 = 16,68 ms` (exatamente 1 quadro), `p95 = 43-45 ms`, `p99 = 46-49 ms`
  (no benchmark: core p50 9,8 ms, p95 39,5, p99 45). ~25% dos quadros custam ~2,5 quadros
  de tempo — é a cauda, não a média.
- **A/B de configuração de apresentação/ciclo (mesma cena):**

  | config | new_fps | descartes |
  |---|---|---|
  | ES (threaded present + mailbox, vsync) | 39,5 | 834 |
  | + `FC_IDLE_FF=0` (sem idle skip) | **43,2** | **1** |
  | apresentação bloqueante (`RETRORUN_SDL_THREADED_PRESENT=0`) | 34,3 | 915 |
  | sem vsync e sem threaded present | 26,6 | — |
  | `internal_resolution = 320x240` | 33,5 | — |

  O `FC_IDLE_FF=0` é o único que **melhora** (e elimina os descartes): o idle skip faz a
  emulação correr à frente da apresentação e o frame é descartado. **Ressalva:**
  o build instrumentado desta sessão deu 32,7-34,7 fps (2 × 80 s) na mesma cena, então
  parte da diferença entre linhas da tabela pode ser variação de cena — o que é
  mecanístico (não depende de cena) é a contagem de descartes e o formato da
  distribuição (p50 16,6 ms / p95 43-45 ms).
- **Perfil de CPU do host (`FC_TINYPROF=300`, processo inteiro, 12,8 k amostras):**
  core 51,8% (JIT `SH4_TCB` 27,0%, `AWCartridge::decrypt` 1,7%, `SRdecode` 1,5%,
  `ReadMem_area0` 1,0%, resto espalhado em <1%), **driver Mali 23,8%**, **libc 19,2%**
  (as amostras caem no `svc`/`ioctl` e em futex — parte é artefato de entrega do sinal a
  threads bloqueadas). No código gerado, os 4 blocos mais quentes são dois pares de
  laços minúsculos (`8C025C48/6A` e `8C052CFA/D24`, 4 instruções, 30-36 instruções host
  por volta, 30,8 M e 22,9 M voltas na sessão) = **32% do trabalho host do JIT**; e a
  divisão 64 bits do runtime (`div0u` + 32 × `rotcl/div1`, `8C1238FE`/`8C123ABC`) é
  **21% dos ciclos emulados executados**.
- **Conclusão:** o NGBC não está limitado pela emulação do SH4 (26% do clock), nem pela
  GPU (24% do processo). O que falta é achar o **estalo de ~40 ms em ~25% dos quadros**
  (candidatos: upload de textura/VRAM, DMA G2 do AICA, leitura do cartucho com
  `AWCartridge::decrypt`, ou a cadeia de apresentação). Ferramenta pronta para isso:
  `FC_TINYPROF` + `tools/tinyprof.py` (falta atribuir por thread: hoje o `SIGPROF` cai
  em qualquer thread, e thread bloqueada em `svc` também é amostrada).

## 2026-10-31 (noite, 2) — método do projeto: perf + dump do JIT no TR; despacho inline P1/P2/P4

- **`perf` de volta no device** (não havia nos repos: instalado do cache do apt,
  `linux-perf 6.12.107` + `libopencsd1` + `libdebuginfod1t64` + `libbabeltrace1` +
  `libtraceevent1` + `libjson-c5`; com PATH completo o `dpkg` funciona).
- **Pipeline (método do projeto)**: `FC_JIT_DUMP_LITE=1` + `perf record -F 299 -k mono -p <pid>`
  → `jit_lite_report.py <dump> <samples> --tcb-off 0x38ea48` resolve amostra→bloco (o perf
  só vê o blob `SH4_TCB`). Ferramenta nova: `tools/jit_hot_functions.py` agrupa os blocos
  em **funções** (entrada = alvo de `bsr` / bloco sem fall-through) e soma o tempo por função.
- **TR Chronicles, thread de emulação (131 s, 38,7 mil amostras, 99% de um núcleo):**
  `codigo do JIT` só **31,8%**; fora do JIT **65,8%**, dos quais
  **`bm_GetCodeByVAddr` 28,0% + `mmu_full_lookup` 22,4% = 50,4% em despacho+tradução**.
  O bloco mais caro é 0,49% e os 30 maiores somam 6,5% — **não existe "função quente" para
  nativizar**: o custo é o despacho, 16 M por segundo emulado (laço minúsculo do kernel WinCE).
- **`vmem32` está ligado** (fastmem com MMU): o acesso a *dados* já é load direto, então os
  22,4% de `mmu_full_lookup` são **tradução de fetch de instrução** (27% dos despachos; os
  outros 73% caem em P1/P2/P4, onde a tradução é identidade).
- **Fix medido (seguro, sem mudar semântica): despacho inline no `no_update` do JIT** —
  para `va` par e `fast_reg_lut[va>>29] != 0` o JIT faz `fpcb[(va>>1)&FPCB_MASK]` inline
  (mesma ordem do C: ímpar → syscall/caminho C). Resultado no mesmo perfil:
  **`bm_GetCodeByVAddr` 28,0% → 13,6%**, total despacho+tradução **50,5% → 41,1%**
  (o resto migra para o despachante inline, que aparece como "JIT sem bloco").
  VEL do TR 18,3-19,9% (era 18,5-23,8% — o ganho é de CPU, não de VEL nesta cena travada).
- **Bug achado na cache de tradução (`FC_ITLB`, desligada):** ela devolvia
  `*tlb_entry_ret = <ponteiro do TLB_Entry>` e o `wince_resolve_address` **reusa
  `UTLB[CCN_MMUCR.URC]`** a cada resolução; o `vmem32_map_mmu` lê `SZ/PPN` desse ponteiro,
  então mapeava a página errada. Corrigido guardando **cópia** do `TLB_Entry` (fica).
  Mesmo com a correção a cache **não compensa**: 59 M lookups com 99,58% de acerto e VEL
  10,2% (× 19,9% sem ela) — o caminho do jogo muda e fica mais lento. **Fica desligada.**
- **Conclusão para o TR:** o gargalo é a taxa de despacho (blocos minúsculos do kernel do
  WinCE em laço). Próximos passos com o dump: (a) HLE por assinatura do laço
  `8C01734C..8C0176D2` (heap/validação, ~20 blocos por volta) e `8C02CC40..6E`
  (`free`/bins) — é o que transforma ~20 despachos em 1 chamada nativa; (b) o
  `01E42E34` (xor/xor/or/tst, 6,3 M voltas, função pura) é HLE trivial; (c) o
  `000B87E6` (aritmética 64 bits com `subc`).

## 2026-10-31 (noite, 3) — TR Chronicles: o laço exato onde ele não passa (ring trace + leitura de RAM)

- **Ferramenta nova:** `FC_JIT_TRACE_RING=1` (ring buffer das últimas 4096 execuções de bloco,
  reescrito a cada volta) — o trace normal começa no boot e o travamento fica de fora.
  `tools/jit_ring_loop.py` acha o **período do laço** e desmonta o ciclo (com o dump).
- **Resultado (TR, cena travada, 1,5 quadro/s):** laço de **29 blocos**, repetindo sem saída:
  `8C017544 → 8C01755E → 8C0175BC → 8C017604 → 8C01761C → 8C02CC40 (seqlock) →
   8C017628 → 8C01764C/6B2/6C2 → 8C01238E..8C01245E (7 blocos de 2-4 instr) → 8C017544`.
  Dentro dele há uma chamada para **código de usuário em `00005BC0`**
  (`r0 = *r4; *r4 = r5; rts` = troca atômica) — o kernel chamando o processo.
- **Estado lido da RAM (via `mem` no socket de controle), congelado × avançando:**
  - **congelados:** `*(8C129D14) = 7` (contador), `*(8C131560) = 7` + `*(8C131564) = 8C897248`
    (cabeça + ponteiro de uma fila/tabela de 7 entradas), `*(8C133BA0) = 0x4C4B4 = 312500`,
    `*(8C133BA4) = 5` (escala);
  - **avançando:** `*(8C129888)` = contador de **ms emulados** a **1000 × VEL** (medido: 28400 →
    29525 → 30525 em 5 s, com VEL ~22%).
  - `8C02CC40` lê esses dois (seqlock: `if (a == tick)` / `if (*r4 == escala)`) e calcula
    `((312500 - tick) * 5) >> 16` — ou seja, é **aritmética de prazo/tempo restante**.
- **Leitura:** o laço espera uma mudança de estado (`*(8C131564)`/contador 7 — o "dreno" da
  fila) que **outra thread ou interrupção** deveria provocar; no nosso emulador isso não
  acontece. Só o relógio de ms anda. `312500` ms = **312,5 s de tempo emulado** de prazo — a
  VEL 20% isso é ~26 min de parede (e explica por que "parece travado" e por que o 2021, mais
  rápido, chega na tela de apresentação).
- **Próximo passo (instrumento já existe):** `FC_WATCH_ADDR=8C131564` (amostra por quadro) e/ou
  watch nas duas palavras para provar que ninguém escreve; depois achar (estaticamente, no dump)
  o código que deveria escrever e por que ele não roda (thread/interrupção não escalonada).

## 2026-10-31 (noite, 4) — TR Chronicles: boot parqueado após 124 quadros; o laço progride (7↔3)

- **Os dois cores bootam igual:** log em DEBUG mostra `Did not load bios, using reios` +
  `Enabling Full MMU ... for Windows CE game` + `Enabling Full MMU support` **no nosso e no
  2021**. Não é diferença de BIOS/MMU.
- **Nosso core para em exatamente 124 quadros novos** (todas as rodadas: 124) — é o fim do
  splash/primeira fase do WinCE; depois fica no laço de 29 blocos (4.121b).
- **O laço NÃO é deadlock:** `FC_WATCH_ADDR=8C131560` (amostra por quadro) mostra o contador
  da fila oscilando **7↔3, ~4 mudanças/s** (47 em 750 quadros) enquanto o laço roda ~33 mil
  voltas/s — ou seja, o emulador gasta o orçamento inteiro *observando* um estado que outra
  thread muda devagar.
- O `8C02CC40` calcula `((*(8C133BA0)=312500 − tick) * 5) >> 16` (tempo restante) contra o
  relógio de ms `*(8C129888)`, que anda certo (1000 × VEL). 312500 ms = **312 s de tempo
  emulado** de espera → a VEL 20% são ~26 min de parede. É por isso que "parece travado" e
  por que o core 2021 (uns 30% mais rápido na apresentação) chega na tela.
- `FC_NO_FDCLEAR=1` (única diferença semântica achada no caminho de exceção de FPU vs o base
  2021) **não muda**: 124 quadros, VEL 22,0% (× 18,3-24% de ruído).
- **Próximo passo (ferramenta a portar):** `print_wince_syscall` do `wince.h` do upstream —
  imprime **nome de API e método** de cada syscall WinCE (nosso fork só tem o `#ifdef`
  `TRACE_WINCE_SYSCALLS` vazio). Com ele se vê qual chamada antecede o parque. Em paralelo,
  `FC_WATCH_ADDR=8C131564` para pegar a transição exata.

## 2026-10-31 (noite, 5) — TR Chronicles: causa por bissecção = guarda do FPSCR (commit 13); boota com fim de bloco

- **Device novo de teste: `ark@192.168.0.11`** (dArkOS RE, mesmos scripts em `/home/ark`).
- **Revisão do `012d949d8` (despacho inline P1/P2/P4):** semântica igual ao `mmu_instruction_translation`
  do `FAST_MMU` — correto, mas não muda o boot (é CPU, não o defeito).
- **Critério objetivo:** o 2021 base produz quadros sempre (988 apresentados/105 s, VEL 28,5%); o nosso
  faz 124 quadros novos e para. Desligar todas as chaves `FC_*` (HLE, dyn link, compact, stubs, skips,
  cold, FMV clock, codepage, SMC, AICA dual) **não** muda: defeito em código sem chave. As opções do
  cfg (`flycast2026_*` × `flycast2021_*`) são equivalentes.
- **Bissecção** (worktree `flycast-wt-2ea`, build ~40 s): 101/35/15/14 param em 123 apresentados;
  **11 e 12 bootam** (666/683, VEL 26%). Culpado: **`8e8b3c80c`** — guarda PR/SZ do FPSCR (o bloco
  deixa de terminar após `lds FPSCR`).
- **No HEAD, `FC_FPSCR_BLOCKEND=1`** (diag: fim de bloco após escrita de FPSCR): 363 quadros novos em
  85 s e o usuário viu **a FMV da Eidos (~12 fps) e a tela de PRESS START** — o 2021 base não mostra
  essa FMV.
- **Guarda com RM:** o decoder também fixa o RM (`RoundToZero` escolhe `cvt_i2f_z/_n`), e a guarda só
  olhava PR/SZ. Agora compara `FPSCR_BLOCK_CFG_MASK` (SZ|PR|RM) nos dois backends — correto, mas o TR
  segue em 124 com ela: não é a causa.
- Regalloc conferido: `shop_sync_fpscr` faz flush duro de fr/xf (não sobra float velho em registrador).
- **Próximo:** `FC_FPSCR_BLOCKEND=lo-hi` para separar kernel (`8C…`, troca de contexto de FPU
  `8C01282C`/`8C012894`) de usuário (libm `0009DACE`/`000961C8`) e ler o ARM64 do bloco culpado;
  depois testar o Sega Rally 2 (também WinCE).

