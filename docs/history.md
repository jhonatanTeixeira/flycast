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
