# Débitos Técnicos — flycast2026 (fork metallic77 @ 603814c9f)

> **Compactado em 2026-10-08.** Só ficam os débitos **ainda abertos** e as lições
> aprendidas dos resolvidos. A tabela completa (dezenas de itens com números, A/B e
> desmontagens) está em `docs/arquivo/tech_debits_2026-09-13_a_2026-10-08.md` — as
> referências "item 4.xx" dos outros docs apontam para lá. Itens novos continuam a
> numeração (próximo livre: **5.8** / **4.119**).
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
| — | `ggxx`/`ggxxsla`/`sa2`: savestates V12 antigos precisam ser regenerados pelo usuário | pendente |

### Performance

| # | Item | Status |
|---|------|--------|
| 4.119 | HLE `memset`/`ocbp` (5.4) **não validado**: sem `FC_STATE_HASH`; tempo não é bit-exato por construção (laço inteiro com um só `UpdateSystem` → interrupções atrasadas; o HLE do DOA2 faz ENTER por bloco); o "Le Mans `core_p95` 95→58 ms" veio de cold boot (cenas diferentes) e a rodada seguinte com o mesmo HLE deu p95 150 ms + `declared_fps` 7,5 + áudio em loop, sem separar do HLE. Teto do ganho: memset 2-6%, ocbp ≤1% da emu | validar |
| 4.120 | Inline cache (`FC_DYN_CACHE`, 5.7): frames +11% mas **VEL 88,8 → 82,1%** — é o padrão "mais fps, jogo mais lento". VEL foi descartada sem prova; conferir `new_fps`/dupes (`FC_SYNC_STATS`) + VEL antes de promover | validar |
| 4.122 | Shenmue II: AICA/ARM7 >8% da emu (`FastControlBlock` 4,2% + `AICA_Sample32` 2,4% + `StreamStep`); 5-9% de amostras sem símbolo (`?`) na emu em vários jogos — identificar | não investigado |
| 4.20/4.34 | DOA2/Zombie/Shenmue: teto é o throughput do SH4 (~116M instr/s × ~11 ciclos ARM). Sobra a *moldura* de blocos minúsculos no laço de vértices; superblocos dariam ≤~5% | confirmado, teto estimado |
| 4.37 | Render DC: draw no driver Mali domina (~34 µs/draw, ~640 draws); quebras de lote são trocas reais de textura → só atlas/menos draws ajudaria | medido, sem fix |
| 4.112 | Fps do jogo: o core declara só o refresh (60) ou metade (30), histerese 1,6/1,3 vblanks/frame em 2 janelas de 1 s. O **ritmo é do core**, sem opção: com alvo 30 o `retro_run` dorme até o prazo de 1/30 s (fim do `retro_run`; fora em fast-forward; `FC_CORE_PACING=0` só para A/B); com alvo 60 não segura (o vsync já dá o ritmo; o pacing do frontend nos de 60 custou fps: KOF Evo 59,3 → 57,8, CvS2 com o report antigo travava em 55). `retrorun_loop_declared_fps=false` no device. Medido (cfg `false`, savestate, 20 s): Napple 30 / VEL 100,1 / 30,2 novos / 0 dupes / overruns 490 → 119; KOF Evo 60 / 100 / 59,0; Shenmue II 30 / 72,6 (empate). **Atenção na medição:** em jogo de 30, o `core_*`/`active_frame_*` do benchmark passa a incluir o sono do ritmo (~33 ms); o custo de trabalho fica no `FC_SYNC_STATS`. Core md5 `ea43aa08` | done (validar jogando) |
| 4.97 | Napple: giro do laço do frontend com dupes abaixo de 95% (mailbox); VEL real só pelo áudio | parcial (4.110 mitigou) |
| 4.98 | `FC_JIT_DUMP` completo derruba o Napple (74,8 → 23,8%); usar o `_LITE` | aceito |
| 4.9 | Regalloc FPU S16-S31 custa ~3% de core em 2D curto (push/pop caller-saved) | aceito |
| 4.17 | `RuntimeBlockInfo::hash()` lê bytes e só metade do bloco → `idle_hash` pode casar errado | confirmado, sem fix |
| 4.79 | `flycast2026_frameskip_budget` (33%) medido só no Shenmue | validar em mais jogos |
| 4.103 | Morton na GPU (`FC_TEX_GPU_MORTON`) validado no MvC2; falta bateria Naomi e mslug6 | opt-in |
| 4.51 | `jit_armv8_a` correto (idêntico) mas mais lento — código quente dobrou | guardado |
| 4.72 | Isolar por padrão o tradeoff do acesso compacto (kofnw −2-3%) | ideia / aceito |
| 4.111 | `div32u/s` ainda não suportados no tier2 (rejeita a região) | tier2 aposentado |
| — | Tier2 (4.73-4.76): aposentado, opt-in OFF. Causas raiz abertas: 4.74 (laço com store no delay slot), 4.76 (chão sumido esporádico) | aposentado |

### Nunca investigados (baixa prioridade; auditoria de 2026-09-13)

`UpdateSystem` ~7.400×/frame (1.2) · `bm_GetStaleBlock` linear (1.4) · MMU
`mmu_full_lookup` O(64) (1.5) · CD-DA no loop do AICA (2.4) · DSP da AICA (2.5) ·
`FlushCache` ARM7 (2.6) · `WriteSample`/`audio_batch_cb` (2.7) ·
`CaclulateSpritePlane` (3.4) · buscas lineares `ctx_list`/`spg_line_sched` (3.8) ·
`std::map` em `TexParameteri` (4.3) · `vidx_sort` com malloc/frame (4.5) ·
conversão de textura por texel (4.7) · upload de VBO/IBO todo frame (4.8).

## Lições aprendidas (itens resolvidos ou descartados)

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
- Idle: assinaturas por **forma**, exigindo desvio para trás e apertado; laço de
  espera encadeado e de contagem precisam de avanço até o evento; checar "endereço é
  RAM" para não pular timer de hardware; varredura (`strlen`) não é espera.
- `sh4clock` d12 cobrava 1,2× (d10 padrão; override por jogo no Le Mans).
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
- Exceção de FPU (`SR.FD`) no decode clobbera `next_pc`; checar em runtime.
- Savestate: validar contagens lidas (board_count lixo) e abortar o load limpo.
- Hipótese "N% do tempo ⇒ ganho grande" falhou várias vezes; o teto real vem do
  tráfego de contexto e do L1I.

### Render / textura
- Cada draw custa ~24-34 µs no Mali: batching por primitive restart (+7%), um upload
  de index buffer por lista, `glDrawRangeElements` (KOF Evo 31→54 fps).
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
