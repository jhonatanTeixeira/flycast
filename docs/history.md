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
- `history.md` compactado (este arquivo); original em `docs/arquivo/`.
