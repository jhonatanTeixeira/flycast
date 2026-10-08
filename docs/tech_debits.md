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
| 4.115 | Teto de tempo de frame com ~100% dupes: Macross M3 ~145 ms, TR Chronicles ~147 ms (EGG era o tier2) — família frame-wait/CHD | não corrigido |
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
| 4.20/4.34 | DOA2/Zombie/Shenmue: teto é o throughput do SH4 (~116M instr/s × ~11 ciclos ARM). Sobra a *moldura* de blocos minúsculos no laço de vértices; superblocos dariam ≤~5% | confirmado, teto estimado |
| 4.37 | Render DC: draw no driver Mali domina (~34 µs/draw, ~640 draws); quebras de lote são trocas reais de textura → só atlas/menos draws ajudaria | medido, sem fix |
| 4.112 | Retrorun usa o refresh declarado como fps máx. O core já reporta o fps natural; o frameskip adaptativo do SDL compila mas não dispara (mede só trabalho, não o pacing) | parcial |
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

### JIT / CPU
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
