# Débitos Técnicos — flycast2026 (fork metallic77 @ 603814c9f)

> **Compactado em 2026-10-08.** Só ficam os débitos **ainda abertos** e as lições
> aprendidas dos resolvidos. A tabela completa (dezenas de itens com números, A/B e
> desmontagens) está em `docs/arquivo/tech_debits_2026-09-13_a_2026-10-08.md` — as
> referências "item 4.xx" dos outros docs apontam para lá. Itens novos continuam a
> numeração (próximo livre: **5.9** / **4.133**).
>
> Status: `não investigado` · `confirmado` · `parcial` · `aceito` · `opt-in`.
> Ao resolver um item, remova a linha e, se houver lição, acrescente um bullet abaixo.
> Cronologia: `docs/history.md`. Em andamento: `docs/current_plan.md`.

## Débitos abertos

### Bugs / jogos

| # | Item | Status |
|---|------|--------|
| 4.114 | `SH4ThrownException` não tratada → SIGABRT (Napple, Shenmue, SA2). `try/catch` em `dc_run`/`rdv_BlockCheckFail` não pegou; com tier2 OFF some | confirmado, sem fix |
| 4.115 | Teto de tempo de frame com ~100% dupes: Macross M3 ~145 ms, TR Chronicles ~147 ms (EGG era o tier2) — família frame-wait/CHD. **TR Chronicles não é espera: ver 4.121** | não corrigido |
| 4.121 | TR Chronicles (WinCE/MMU) **saturado, não esperando**: thread de emulação a ~95% de um core, 26% em `bm_GetCodeByVAddr` + 12% em `mmu_full_lookup` (todo despacho cai na busca lenta por vaddr). É o item 1.5 (MMU) — captura `dcbat_off` 2026-10-07 | confirmado, sem fix |
| 4.116 | Áudio estourado (saturação) + vozes baixas (Grandia II, Napple) | não investigado |
| 4.117 | Memory card "falta de espaço" (Napple, Shenmue II) + crash do Napple ao salvar | não investigado |
| 4.118 | MvC2 (glitches, 33-45 fps) e Project Justice (118 dupes) — suspeita de regressão | não investigado |
| 4.94 | `asndynmt` crasha após escolher personagem (tier2 OFF) | não investigado |
| 4.95 | `meltyb` trava (tela preta) quando a luta começa (tier2 OFF) | não investigado |
| 4.93 | `asndynmt`: triângulo laranja/vértices errados (padrão `region-bad-ftrv-delayslot`; só com tier2) | parked |
| 4.30 | Soulcalibur congela no boot pelo ES com flycast2026 | não investigado |
| 4.31 | Upstream oficial roda o MBAA sem glitches (a ~34 fps); os glitches do nosso eram em boa parte dupes (4.110) — reconferir | não investigado |
| 4.22/4.66 | Sonic Shuffle ~10 fps: laço de busca linear (1,25M exec/s); `scan_loop_skip` escrito mas nunca casou | confirmado, sem fix |
| — | Evolution 1 não abre: CHD com codec zstd (`cdzs`), libchdr do fork sem zstd | confirmado |
| 4.128 | Skies of Arcadia Disc 2 não boota (`SIGSEGV ... was not in vram`, exit 133): validar o CHD com o core upstream; se bom, `FC_JIT_TRACE` | não investigado |
| 4.129 | Le Mans: `declared_fps=7.5` (devia reportar 30), áudio quebrado (batida em loop) em cold boot, load/menus lentos | não investigado |
| — | Re-rodar a bateria DC com o core **sem tier2** (removido em 2026-10-10; skill `bateria`) para fechar as linhas de `game_status.md` medidas com tier2 ON (cvs2, MvC2, SA2, Shenmue II...) | pendente |
| — | `ggxx`/`ggxxsla`/`sa2`: savestates V12 antigos precisam ser regenerados pelo usuário | pendente |

### Performance

| # | Item | Status |
|---|------|--------|
| 4.119 | HLE `memset`/`ocbp` (5.4) **não validado**: sem `FC_STATE_HASH`; tempo não é bit-exato por construção (laço inteiro com um só `UpdateSystem` → interrupções atrasadas; o HLE do DOA2 faz ENTER por bloco); o "Le Mans `core_p95` 95→58 ms" veio de cold boot (cenas diferentes) e a rodada seguinte com o mesmo HLE deu p95 150 ms + `declared_fps` 7,5 + áudio em loop, sem separar do HLE. Teto do ganho: memset 2-6%, ocbp ≤1% da emu | validar |
| 4.125 | `hle_fn` **relocável**: reconhece a função por bytes em qualquer endereço (base = vaddr − offset da entrada; literais lidos da RAM; `Span` com máscara). Validado idêntico em Napple/DOA2/Shenmue II (o `FC_STATE_HASH` compara o **estado/saída** — RAM/VRAM/ARAM/`Sh4Context`/TA + tempo emulado — nas cenas medidas, **não** que a implementação seja igual à SH4; caminhos não exercitados são cobertos pelas recusas `bail` e por testar mais de uma cena). **B1 parcial:** strips com luz difusa (`r8 == 0`, grupo 005) nativos — bit-exato DOA2 (916) + Shenmue II (355). Falta: `r8` ímpar (grupo 004), variantes Power Stone/callback; `lightxf`/`stripemit` em Evolution 1/2, RE CV, Skies e o laço do DOA2 em MvC2/Power Stone/Project Justice (bytes idênticos no dump, savestate não executa). `docs/native_sdk_code.md` | parcial (infra + B1 r8==0 done) |
| 4.126 | Espera de fim de quadro que **lê o TMU0** (grupo 009 do `sdk_find`, mesma função em 10 jogos de DC): gira até uma interrupção zerar uma flag ou estourar o timeout; o idle fast-forward não pega porque o 4.43 exclui laço que lê hardware. EGG ~9,6% da emu. Pular com segurança = avançar até o próximo evento sabendo o valor do TCNT0 nesse ponto. `docs/sdk_find/pseudo/espera_de_quadro_com_timeout_pseudo.cpp` | confirmado, sem fix |
| 4.127 | Código quente do JIT não cabe na L1I e carrega peso morto: 90% do tempo em blocos = 80-160 KB (Shenmue II 345 KB) para 32 KB de L1I; 11-15% de cada bloco quente é caminho frio e 1-4% literal; blocos espalhados (DOA2 196 páginas de 4 KB); Shenmue executa a checagem de código em 24 dos 79 blocos mais quentes. Plano em `docs/jit_hot_path.md` **2026-10-11:** medido com contadores na thread da emulação (Shenmue II): **10 M faltas de L1I por segundo emulado** (~7-9% do tempo esperando código), iTLB 0,1 M/s. D1 (cauda fria) cortou 5% (10,5 → 10,0 M), VEL +0,8. Próximo: D2/D3 | D1 feito, D2-D5 pendentes |
| 5.8 | Spill do HLE do DOA2: o cluster `8C101BC4..C4E` (~15% da emu) guarda e recarrega todo o estado a cada fronteira de bloco do JIT — rendeu só ~1%. **Fechamento do laço externo tentado (2026-10-10, branch `wip/5.8-doa2-loop-closure`):** blocos `C52/C5C/C66/C6C/C78` nativos, ciclos exatos; **bit-exato no DOA2** mas **diverge no Shenmue II** (2 bytes de float em RAM no frame 60, `8C2E8900`; TA/VRAM/ctx batem). Falta achar a causa antes de mergear | parcial (DOA2 ok, Shenmue II diverge) |
| 4.130 | **DOA2 apresenta ~30-36 fps de um jogo de 60 fps** (é 60: `ta[1v≈50 2v≈1]`, um frame de TA por vblank). O core **descarta ~40%** dos frames (`dropped_slot_busy` ~360 de ~950): a fila de 1 slot está ocupada porque o render do frame anterior não terminou. Não é o teto de frameskip (`frameskip_budget=0` descarta igual) nem a GPU/fill (320x240 só subiu 33,7→36,1 fps; é **draw-call-bound**, 4.37). O render custa `process ~5ms + render ~14ms ≈ 19ms`; o intervalo do jogo é 16,7 ms **emulados** = 16,7/VEL em wall (~19 ms a 87%, **encolhe** com o VEL). **Daí a inversão:** VEL↑ → intervalo em wall menor → render fica para trás → mais descartes → fps apresentado cai rumo a 30 (e a ~50% de descarte o padrão é regular → "mais fluido"). Teto é a apresentação, não a emulação **2026-10-11:** com as render threads (4.131) + profundidade no vértice (lição 4.133) o DOA2 mostra **44 dos ~50 frames** (descartes 650 → 54, frame p99 59 → 45 ms), mas VEL 90,6 → 82% (4.132) | parcial |
| 4.131 | **Dois renderers em round robin são viáveis na Mali** (teste isolado `tools/gl_two_threads.c`, 2026-10-10, `perfmax`): 640 draws/frame pequenos (troca de textura/uniform/blend, FBO 640×480, 2 frames em voo) — 1 thread **88,8 frames/s** (17,9 µs de CPU por draw, thread a 100%); 2 threads com contextos compartilhados **155,2 frames/s** (1,75×; 18,6/19,7 µs por draw, 98% cada); independentes 149,4. O driver (Bifrost r13p0) não serializa contextos. Só ajuda quando o render é limitado pela CPU do driver: **DOA2** (4.130: render ~19 ms > intervalo de 16,7 ms → descarta ~40% dos frames; com 2 renderers cada frame tem ~33 ms) e MvC2; não ajuda com a emulação no limite (Shenmue II medido em 2026-10-10: GPU ~30% ocupada, render 28 de 60 ms). Plano futuro: `docs/render_threads_plan.md`. **Implementado e ligado por padrão (2026-10-11, `rt_render.cpp`; `FC_RT=0` volta ao render antigo para A/B):** 2 workers com contexto compartilhado sem superfície, Process em ordem numa cadeia de fences, render em paralelo, texturas versionadas no upload, sampler por worker, RTT/framebuffer na principal, apresenta o frame pronto mais novo. A **emulação entrega o frame direto a um worker livre** (antes ficava na fila até a main voltar ao `rend_single_frame`, p95 9 ms, com a emulação esperando o Process dele) e o **gate da fila descarta o frame se não há worker livre** (espera da emu 1,2 → 0,3 ms/frame, abaixo da base); emulação sozinha no núcleo 3. Hash idêntico ao render atual: MvC2, DOA2, Napple (RTT). **MvC2: 40 → 52 fps novos, frame p99 55 → 40 ms, VEL 100%.** Shenmue II: empate (emu-bound). **DOA2: 27 → 35 fps novos, mas VEL 89 → 83%: não é espera, é o 4.132** (a GPU passa de ~45% a ~70% e o `power_allocator` corta o clock da CPU) | implementado, padrão |
| 4.132 | **O R36 corta o clock da CPU por temperatura/potência durante o jogo** (2026-10-11): `soc-thermal` usa `power_allocator` (liga a 70 °C, alvo 85 °C) e divide um orçamento de potência entre CPU e GPU. DOA2 esfriado < 63 °C antes de cada rodada, s 4-26: base CPU ~1478 MHz (1512 quase sempre), GPU ~45%, 71-80 °C; rt CPU **~1344 MHz** (cai a 1008-1296 a toda hora), GPU ~70%, ~79 °C → VEL 89,2 → 83,2%. Em rodadas seguidas sem esfriar, o device já começa a 79 °C e a CPU fica em **1,0-1,2 GHz mesmo na base**. Consequências: (1) **toda A/B de VEL precisa partir da mesma temperatura** (`tools/rtabt.sh`: espera < 63 °C e loga clock/temp/GPU por segundo); (2) **trabalho de GPU custa clock de CPU**: GPU mais eficiente por frame = emulação mais rápida. DOA2 gasta ~16 ms de GPU por frame (45% a 27 fps) — não medido ainda por resolução/modifier volume (o "não é fill" do 4.130 era tempo de CPU do driver) **Correção (2026-10-11, rodada mais fria):** clock rt 1485 vs base 1512 MHz (-1,8%), GPU 74 vs 45%, e a VEL caiu igual (90,6 → 86,2%): o corte de clock explica só parte; o resto deve ser disputa de memória (DRAM compartilhada CPU/GPU) — hipótese, não medido. **A VEL é confiável:** `FC_SYNC_STATS` agora dá a velocidade por ciclos do SH4, vblanks, áudio gerado e áudio entregue — batem em 0,05% (DOA2 86,74/86,71/86,74/86,74%) **Com a GPU aliviada (4.133) e a CPU do render -24% (opacos ordenados, lição 4.134), a VEL do rt não se mexeu:** DOA2 rt 85,0 → 84,4% (ruído), base 90,6%, clock 1454-1478 MHz. Então não é a CPU do render nem só o clock: a emulação (núcleo 3, ~99% ocupada, mesma distribuição no `perf`) roda mais devagar quando o jogo desenha 46 em vez de 27 frames/s. **Mecanismo medido (2026-10-11, `tools/rtpmu.sh`):** mesmo build, 1 thread de render, clock 1512 MHz nos dois, só muda quantos frames são desenhados (`FC_RENDER_BUDGET_MS=36`): 21,5 × 36,5 frames/s → VEL 90,7 × 86,7%. Na thread da emulação, por segundo emulado: **instruções iguais** (668 × 665 M), **faltas no L2 iguais** (4,4 × 4,5 M), **ciclos +5%** (1404 × 1474 M, IPC 0,476 × 0,451). A emulação faz o mesmo trabalho e as mesmas idas à RAM; cada ida demora mais (~15 ciclos) porque o render move mais dados pela mesma RAM. Alvos no código: menos bytes por frame no render (vértices inteiros por frame, texturas, blit do rt) e menos faltas de L2 na emulação (4,5 M/s; 4.127) | medido, sem fix |
| 4.122 | Shenmue II: AICA/ARM7 >8% da emu (`FastControlBlock` 4,2% + `AICA_Sample32` 2,4% + `StreamStep`); 5-9% de amostras sem símbolo (`?`) na emu em vários jogos — identificar. **2026-10-10, save pesado com o cfg oficial (DSP ligado): `FC_REND_SPLIT` dá `aica_arm7_ms_per_frame` 23,3 ms (callback do agendador, 47,5 chamadas/frame) num frame de ~60 ms (~39%)** — conferir se é o DSP ligado (o `r_shenmue2.cfg` antigo o desligava) **2026-10-11 (fase 1a do `docs/mmap_moe_mtp_plan.md`):** com DSP ligado o core forçava `NoBatch=1` e a mixagem completa + DSP rodavam por amostra na emu thread — era isso os ~23 ms/frame. Agora o DSP roda no render do som em lote (`aica_render_block`: `MIXS[ISEL]` por amostra, `dsp_step`, EFREG): **Shenmue II com DSP 57,0 → 73,9% de VEL** (DSP desligado 73,5%), 16,3 → 20,7 fps novos. Estado: 2 rodadas iguais; áudio com o mesmo nível do caminho por amostra. `FC_AICA_NOBATCH=1` volta ao antigo. Resta ARM7 + controle (~8 ms/frame): fase 3 | DSP resolvido; ARM7 aberto |
| 4.20/4.34 | DOA2/Zombie/Shenmue: teto é o throughput do SH4 (~116M instr/s × ~11 ciclos ARM). Sobra a *moldura* de blocos minúsculos no laço de vértices; superblocos dariam ≤~5% | confirmado, teto estimado |
| 4.37 | Render DC: draw no driver Mali domina (~34 µs/draw, ~640 draws); quebras de lote são trocas reais de textura → só atlas/menos draws ajudaria | medido, sem fix |
| 4.112 | Fps do jogo: o core declara só o refresh (60) ou metade (30). Critério: fração dos frames que o jogo produz (thread de emulação, sem RTT) em exatamente 1 vblank emulado — 60→30 com <5%, 30→60 com >25%, confirmado em 2 janelas de 1 s. A média de vblanks/frame (1ª versão, limiar 1,6) declarava o DOA2 (~40 fps emulados) como 30 e o travava em 28 fps. O **ritmo é do core**, sem opção: com alvo 30 o `retro_run` dorme até o prazo de 1/30 s (fim do `retro_run`; fora em fast-forward; `FC_CORE_PACING=0` só para A/B); com alvo 60 não segura (o vsync já dá o ritmo; o pacing do frontend nos de 60 custou fps: KOF Evo 59,3 → 57,8, CvS2 com o report antigo travava em 55). `retrorun_loop_declared_fps=false` no device. Medido (cfg `false`, savestate, 20 s): Napple 30 / VEL 100,1 / 30,2 novos / 0 dupes / overruns 490 → 119; KOF Evo 60 / 100 / 59,0; Shenmue II 30 / 72,6 (empate). **Atenção na medição:** em jogo de 30, o `core_*`/`active_frame_*` do benchmark passa a incluir o sono do ritmo (~33 ms); o custo de trabalho fica no `FC_SYNC_STATS`. Core md5 `ea43aa08` | done (validar jogando) |
| 4.97 | Napple: giro do laço do frontend com dupes abaixo de 95% (mailbox); VEL real só pelo áudio | parcial (4.110 mitigou) |
| 4.98 | `FC_JIT_DUMP` completo derruba o Napple (74,8 → 23,8%); usar o `_LITE` | aceito |
| 4.9 | Regalloc FPU S16-S31 custa ~3% de core em 2D curto (push/pop caller-saved) | aceito |
| 4.17 | `RuntimeBlockInfo::hash()` lê bytes e só metade do bloco → `idle_hash` pode casar errado | confirmado, sem fix |
| 4.79 | `flycast2026_frameskip_budget` (33%) medido só no Shenmue | validar em mais jogos |
| 4.103 | Morton na GPU (`FC_TEX_GPU_MORTON`) validado no MvC2; falta bateria Naomi e mslug6 | opt-in |
| 4.51 | `jit_armv8_a` correto (idêntico) mas mais lento — código quente dobrou | guardado |
| 4.72 | Isolar por padrão o tradeoff do acesso compacto (kofnw −2-3%) | ideia / aceito |

### Nunca investigados (baixa prioridade; auditoria de 2026-09-13)

`UpdateSystem` ~7.400×/frame (1.2) · `bm_GetStaleBlock` linear (1.4) · MMU
`mmu_full_lookup` O(64) (1.5) · CD-DA no loop do AICA (2.4) · DSP da AICA (2.5) ·
`FlushCache` ARM7 (2.6) · `WriteSample`/`audio_batch_cb` (2.7) ·
`CaclulateSpritePlane` (3.4) · buscas lineares `ctx_list`/`spg_line_sched` (3.8) ·
`std::map` em `TexParameteri` (4.3) · `vidx_sort` com malloc/frame (4.5) ·
conversão de textura por texel (4.7) · upload de VBO/IBO todo frame (4.8).

## Lições aprendidas (itens resolvidos ou descartados)

- **Som "estourado" sem DSP: faltava `>> 4` no caminho em lote (2026-10-11).** Voz que só
  manda som para o DSP (DISDL = 0), com DSP desligado, toca direto com `oDsp >> 4` (caminho por
  amostra e `flyinghead/flycast`). O render em lote do 4.44 fazia `oLeft = oRight = oDsp`, sem o
  `>> 4`: essas vozes saíam 16× mais altas. Corrigido junto com a fase 1a (falta o usuário ouvir;
  pode ser o "som estourado" do Grandia II, 4.116, e do Shenmue II sem DSP).

- **Opacos agrupados por estado (4.134, 2026-10-11).** O lote de strips vizinhas iguais
  quase não pegava no DOA2 (497 de 643 draws com 1 strip; 359 trocas de textura): o jogo
  alterna texturas. Nos opacos a ordem não decide o que fica na frente (teste de Z), então
  os trechos com Z `GREATER`/`GEQUAL` e escrita de Z são ordenados por estado (estável);
  strip com outro modo é barreira. DOA2: **draws 643 → 406, CPU dos opacos 9,9 → 5,9 ms,
  render 16,7 → 12,7 ms por frame**; frame p95 34,9 → 32,1 ms; imagem ≤1 pixel diferente.
  `FC_OP_SORT=0` volta à ordem original (A/B).

- **`gl_FragDepth` desligava o descarte antecipado da Mali (4.133, 2026-10-11).** O caminho
  GLES3 escrevia `gl_FragDepth = log2(1 + 1e5·w)/34` em todo pixel (e no modifier volume):
  com isso a Mali não descarta pixel escondido antes do shader e cada camada opaca era
  texturizada e só depois jogada fora. Medido por eliminação (`FC_SKIP_LIST`; o
  `GL_EXT_disjoint_timer_query` da Mali não mede trechos dentro do frame): DOA2 16,6 ms
  de GPU por frame, opacos ~10, modifier volumes ~3,3, translúcidos ~1,2; a 320×240 6,7
  ms (custo de pixel — o "não é fill" do 4.130 era tempo de CPU do driver). Agora a
  profundidade sai do vertex shader, linear no 1/w com escala por frame (o caminho GLES2
  que já existia): **16,6 → 7,2 ms** (-57%), GPU 45 → 20% no mesmo fps. Imagem: ≤3 pixels
  diferentes por frame no DOA2, ~13 no MvC2, igual a olho nu no Shenmue II (o usuário
  aceita: mudou a técnica, muda pixel). O `flyinghead/flycast` atual também escreve
  `gl_FragDepth` no GLES.

### Método e medição
- Medir antes de otimizar: SQ inline em todo store (+6% pior), `timeslice` 4x, skip de
  Translucent, FPSCR nativo, HLE do DOA2 (~1%) e `div32` (empate) não deram ganho
  apesar de "óbvios" no código.
- `perf` agregado ≠ caminho crítico. Decidir por CPU **por thread** + split do frame.
  Idle loop legítimo pode dominar o `perf` e cortá-lo não mudar o fps (mslug6).
- "Descartado" vale só para o cenário medido (anti-SMC: descartado no 1º teste,
  +19% no Shenmue depois). Instrumentação esquecida infla A/B (+8% real era +3,2%).
- VEL% = áudio/(tempo×44100), não o contador de frames; esperar o render sobe fps e
  derruba VEL. Dupes inflam o contador e causam artefatos (mbaa).
- Rodar como o ES (`perfmax performance`, cfg oficial, retrorun3 com vsync+thread);
  mesma cena/savestate; pausa entre rodadas pesadas (térmico). Benchmark de 40s
  perde cena tardia → janela rolante. Savestate velho pode gravar o glitch.
- Validar JIT com `FC_STATE_HASH`+`FC_RTC_FIXED`+`FC_INPUT_NEUTRAL` e um controle
  com bug proposital. Build incremental após header amplo mente → `make clean`
  igual ao build + `.o`=0.
- Dumper completo derruba o jogo; usar `_LITE` + perf. `bm_WriteBlockMap`/gdb
  servem para inspecionar blocos sem rebuild; endereço de amostra do perf em região
  JIT pode vir com 2 bits baixos errados.

- **Priorizar por tempo amostrado, nunca por proporção estática.** O
  `padroes_ineficiencia.txt` (`consolidar.py`/`cluster_blocks.py`) pesava
  instr host/SH4 × nº de blocos parecidos — mede quanto o padrão *aparece*, não
  quanto *roda*; o filtro "≥3 jogos" ainda descartava os laços quentes (específicos
  de cada jogo). Pelo `perf` (7 capturas `dcbat_off`, % da emu): `tas.b` 0,0 em
  todos; `ocbp` ≤1; laço em si mesmo 2-6; `div1` ≤2,6; epílogo `lds.l PR`+`rts`
  1,5-7; blocos com FPU 9-43 (a maior categoria nos jogos <100%); fora do JIT
  26-46. "Sem FPU ⇒ não é 3D" era artefato do filtro. Aposentado como fonte de
  prioridade (2026-10-08).
- `jit_lite_report.py` dividia por 999 Hz mas o `rr_capture.sh` grava com
  `perf -F 299` → "% de um core" saía ~3,3× menor. Corrigido (`--freq`, padrão
  299; lê `.gz` direto). Emu real nas capturas `dcbat_off`: EGG 36, MvC2 57,
  Shenmue 72, DOA2 83, Napple 83, Shenmue II 89, TR Chronicles 95%. Conclusões
  antigas que usaram esse % merecem segundo olhar.
- `getenv("FC_MORTON_LOG")` dentro do `SetGPState` (~640×/frame) custava 1,4% da
  thread de render no Shenmue II (o `rr_capture.sh` exporta muitas `FC_*`).
  Cacheado em `static`. Nada de `getenv` em caminho quente.

### JIT / CPU
- **FMV Sofdec: clock reduzido só durante o vídeo (4.123, 2026-10-09).** A FMV do
  Evolution 1 rodava a ~78% com a emu saturada nos blocos da Sofdec — mas não era
  decodificação: o player **gira em espera** (laço que o idle não pega). Travar o
  declarado em 30 piorou (VEL 80 → 50: a emu espera cada frame ser consumido).
  O que funcionou: com o conversor YUV do PVR ativo (só a Sofdec usa), o stub
  `intc_sched` repõe 2/3 dos ciclos por fatia (`g_sh4CycleRefill`, equivale ao
  clock 1,5 só no vídeo, sem recompilar) → Evolution 1 VEL 78,9 → 100,2%, vídeo
  24 → 30 quadros/s, frame p99 39,5 → 19,8 ms; RE CV FMV 25-30 quadros/s (~83% →
  ~96%). Fora de FMV idêntico (DOA2, hash 956 frames). `FC_FMV_CLOCK=0` desliga;
  `FC_FPS_LOG=1` mostra `yuv=` (quadros de vídeo/s).
- **Link direto do despacho dinâmico (4.120, ex-5.7 `FC_DYN_CACHE`), padrão desde
  2026-10-08.** A 1ª versão não fazia o que prometia: o acerto terminava no mesmo
  `Br x15` indireto, com 5 loads em vez de 1, e a falha (~40%) somava 3 stores — o
  VEL 88,8→82,1% era real. Reimplementado com o mecanismo de link dos blocos
  estáticos: fim `BET_Dynamic*` em 3 estados de mesmo tamanho (A: stub de link;
  B: `cmp pc,#ligado` + `b` direto; C: lookup na `fpcb` após o 1º alvo diferente),
  invalidação por `pre_refs`/`Relink`. **Bug achado no caminho (pré-existente,
  também nos estáticos):** `rdv_LinkBlock` compila o alvo, a compilação pode limpar
  o cache de código, e o bloco de origem morto era religado no endereço já
  reaproveitado → corrompia o 1º bloco novo (DOA2 travava na BIOS). Agora revalida
  o dono do código após compilar e o descarte faz `RemRef` nos alvos. Bit-exato
  (DOA2 967 + Shenmue II 582 frames, hash completo); DOA2 35,6→37,2 fps novos (VEL
  88,4/88,1), Shenmue II VEL 72,8→73,6; boot ok em MvC2, Shenmue, Sonic Shuffle,
  RE CV, TR Chronicles, cvs2 (Naomi), kofxi (AW). `FC_NO_DYN_LINK=1` desliga (A/B).
- Clock do SH4 fixo em `d10` (nominal), sem core option (2026-10-08): d10 mediu melhor
  que o d12 antigo (Le Mans 16,1 → 18,8 fps, demais sem perda, 4.39); override por jogo
  em `lut_games.sh4clock` (4.42). Regra: opção provada boa vira comportamento.
- O gargalo típico é throughput do SH4 (~9-11 ciclos host/instr, IPC ~0,45; L1I na
  FMV, DRAM no DOA2). Ganho vem de baratear a instrução, não de achar um bug.
- Travessia JIT→C++ domina o custo de fallbacks (`lds FPSCR` nativo = sem ganho);
  o que ganha é eliminar a chamada (`stc.l SR` +3%, `tas.b`, `div32`).
- Acesso compacto à RAM (`x13`+`uxtw`) e stubs sem salvar o que não precisa: FMV
  71→83%. Stubs de SQ só nos sites reescritos (nunca check inline em todo store).
- Stub chamado com `Bl` precisa salvar o LR; slot de rewrite precisa de NOP até o
  tamanho fixo; rewrite não pode assumir `x0` = endereço.
- Anti-SMC: stores em página de código via stub + bitmap; leituras dobradas pelo
  SSA precisam ser invalidadas por escrita.
- **Threads nos jogos de DC (2026-10-10, varredura dos dumps em `/mnt/1TB`):** existe
  uma biblioteca de threads com **troca de contexto completa** (preempção por timer
  não confirmada) ligada em 10 jogos
  (Napple `8C16BB28…8C16BD74`, EGG, Grandia II, RE CV, Evolution 1 e 2, Le Mans,
  Macross M3, PSO v2, KOF Evo; endereço muda por jogo, bytes iguais). Cria a pilha da
  thread (entrada r6, arg r7, SR, FPSCR), cede por "exceção falsa" (`ldc SSR/SPC` com
  o PR e salta para o escalonador), salva tudo (r0-r14, bancos, 32 floats, GBR, MAC,
  SPC/SSR), escolhe a próxima em C e volta com `rte`. Custo medido: ~0,5% da emu
  (Napple, EGG; só esses têm amostras). TR Chronicles usa o kernel do WinCE (SPC/SSR
  próprios). Os `tas.b` do SDK são try-lock, não espera. Os "mutex" de verdade são
  `ldc SR` mascarando interrupção (`8C0084xx`, 21 jogos). Detalhe e a análise de
  paralelizar no ARM64 em `docs/sh4_threading_model.md`.
- Idle: assinaturas por **forma**, exigindo desvio para trás e apertado; laço de
  espera encadeado e de contagem precisam de avanço até o evento; checar "endereço é
  RAM" para não pular timer de hardware; varredura (`strlen`) não é espera.
- `sh4clock`: d12 (padrão antigo) cobrava 1,2×; d10 deu ganho geral; d8 piorou a maioria → d10 fixo (override por jogo no Le Mans).
- Despacho: tabela FPCB custa ~1-2%; inline cache só vale com invalidação em **toda**
  escrita da tabela (incluindo `bm_AddBlock`/`bm_ResetTempCache`) — opt-in.
- HLE por assinatura nativo (`hle_fn.cpp`): mesma ordem/fusão de float, mesma
  contabilidade de ciclos, hash idêntico; o ganho some se o estado ainda é
  guardado/recarregado a cada fronteira de bloco.
- Compute shader na GPU para função chamada por vértice/ida-e-volta: 14× pior.
- Tier2 (regiões quentes): bom no laço de vértices do DOA2/Shenmue II, mas quebra
  carts Naomi, EGG, Shenmue, SA2 e jogos com thread cooperativa → aposentado.
  Lições: ligar só após o handover BIOS→jogo; `pref` no slot de `jcond` clobbera o
  `decision`; MMIO dentro de região → bail preciso; saída não pode gravar T que a
  região não usa; gate do `safe_point` (1/64) e fila SPSC fora da emu thread.
- **Remoção do tier2 (2026-10-10):** código morto "OFF por padrão" ainda *roda*:
  com a opção desligada o `tier2_on_block_added` ainda marcava o handover e
  reservava 1 MB da cauda do code cache (`tier2_code_reserve = T2_AREA`) e subia a
  thread do worker (que ficava ociosa, sem amostras). Mesmo assim a remoção é
  neutra: `FC_STATE_HASH` (RAM/VRAM/ARAM/ctx) idêntico em DOA2 (1241 quadros),
  Shenmue II (397) e Napple (760), cold boot com `FC_RTC_FIXED`+`FC_INPUT_NEUTRAL`
  — a área liberada só muda a *capacidade* do cache, não a emulação. Validar antes
  de comemorar "é só código morto" pagou: confirmou que os ganchos eram inertes.
- **`hle_fn` relocável (2026-10-10, 4.125):** código de SDK é o mesmo em vários
  jogos, então casar por **bytes** (não por endereço) vale para todos. O `Span`
  guarda os bytes na base canônica e o match vai em `base + (addr − canonBase)`;
  a base do jogo sai de `vaddr − entryOff[entrada]`, e `ENTER`/`BAIL`/literais
  viram `base + offset`. Match completo (todos os spans) evita falso positivo;
  casa em qualquer endereço onde a função apareça. Migração do Napple/DOA2 e
  Shenmue II idênticas por `FC_STATE_HASH` — a relocação não muda o resultado,
  só onde a função é reconhecida. **Mas o gancho só instala quando o bloco de
  entrada é compilado:** savestate em cena que não roda a função (Evolution 1,
  MvC2) não mostra o log — falta a cena, não é bug.
- Exceção de FPU (`SR.FD`) no decode clobbera `next_pc`; checar em runtime.
- Savestate: validar contagens lidas (board_count lixo) e abortar o load limpo.
- Hipótese "N% do tempo ⇒ ganho grande" falhou várias vezes; o teto real vem do
  tráfego de contexto e do L1I.

### Render / textura
- **Translúcidos per-strip: strips profundos quebrados + sem escrita de Z (4.124,
  2026-10-09).** O chão do Evolution 1 sumia (também no metallic77): chão translúcido
  em strips longos; a chave do sort (ponto mais distante) desenhava-o antes de um
  polígono preto quase coplanar, e o Z escrito pelo translúcido ordenado reprovava
  o chão. Correção: strip com 1/W max > 1,5× o min vira pedaços de 2 triângulos
  (índice par, mesmo buffer, primitive restart junta de novo) e translúcido
  ordenado não escreve Z (como o flycast atual). Custo ~0: render 10,9 → 11,0 ms,
  506 draws, 29,7/29,5 fps. Per-triangle também corrigia, mas custava 30 → 25,9
  fps. Usuário: Napple "bem menos erros, mais coerente, mesmo fps".
  `FC_SORT_HYBRID=0` desliga (A/B). Descartados no caminho: recorte por W, modifier
  volume, culling, plano de fundo (recuo 1e-6 do upstream aplicado assim mesmo).
- Cada draw custa ~24-34 µs no Mali: batching por primitive restart (+7%), um upload
  de index buffer por lista, `glDrawRangeElements` (KOF Evo 31→54 fps).
- **O render pode virar o teto da APRESENTAÇÃO sem ser o teto da emulação (4.130,
  2026-10-10).** No DOA2 (jogo de 60 fps) o core apresenta só ~30-36/s: descarta
  ~40% dos frames porque o render (~19 ms: process 5 + render 14) não cabe no
  intervalo de 16,7 ms emulados, que em **wall** é 16,7/VEL — quanto mais rápido o
  emulador, menor o intervalo, mais descarta. O 4.37 ("DOA2 limitado pela emulação")
  valia com o emu mais lento; acelerar o emu **inverteu** o gargalo. Lição: medir o
  `dropped_slot_busy`/`new_fps` além do VEL — VEL alto com fps apresentado caindo é
  sinal de apresentação, não de emulação.
- `GetTexture` dominou mslug6/samsptk: shader de paleta bilinear (20→59 fps),
  `TextureUpscale` não inicializado, invalidação por página de 4 KB, skip de
  re-upload idêntico (default ON).
- Mexer em pipeline de textura exige validação **visual**; skip de Translucent e
  cache de estado do GLES já corromperam/quebraram frame e foram revertidos.
- `glInvalidateFramebuffer` ajuda GPU-bound, custa em CPU-bound.
- No-wait da emu derrubou a cauda (p99 100→11-39 ms) mas expõe corrida na VRAM
  (sprites 2D): wait **por página** em leitura de textura resolve sem o `re.Wait`
  cego (que custava 19% no Shenmue II).
- Em Mali r13p0: `usampler2D`+`texelFetch` na mesma unidade de um `sampler2D`
  invalida; `GL_ALPHA` sobe 4× mais barato que `GL_R8`; shader precisa de
  `precision`.

### Fila, pacing e frontend (retrorun)
- Presenter em thread + vsync (FIFO ≥95%, mailbox abaixo) tirou o swap síncrono do
  caminho; fila de 2 e espera com prazo = **2× o intervalo medido entre frames
  novos** (prazo fixo gerou 40% de dupes).
- Pacer determinístico e skip por orçamento relativo oscilam/pioram; orçamento de
  frameskip absoluto e histerese funcionam.
- O core reporta fps natural em vblanks emulados (janela de 1 s, estabilizado).
- Hotkeys no benchmark: `RETRORUN_BENCHMARK_ALLOW_HOTKEYS`; som: WSOLA no backend SDL
  (reamostrar desafina); catálogo por jogo exige `flycast_retrorun_*` exportados em
  `link.T` e o prefixo `flycast2026_`.

### Áudio / disco / boot
- AICA/ARM7 era desprezível no Shenmue 1 mas 12-15% no Shenmue II: controle exato na
  emu thread, mixagem em thread própria (PCM idêntico).
- CHD: descompressão com leitura antecipada fora da emu (~12% da CPU).
- GD-ROM Naomi: carregar/descriptografar o ROM.BIN por segmento (22 s → 1 s).
- DC memory card por jogo (`per_content_vmus`).

### Infra de device
- O disco `/` do device enche (dumps, `~/.debug` do perf); mandar tudo para `/roms2`.
- `cp` sobre core com hardlink altera todos os nomes: `cp` temporário + `mv`.
- Toolchain `*-13` e `make clean` com os mesmos argumentos; objetos de outro
  `platform=` ficam parados (`find . -name '*.o' -delete`).
