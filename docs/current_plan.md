# Plano atual

> **Compactado em 2026-10-08.** Só o que está **em andamento ou na fila**, em ordem
> de prioridade. Concluído vai para `docs/history.md` (marcos) e `docs/tech_debits.md`
> (lições); o texto antigo deste arquivo e o plano da bateria de 2026-10-07 estão em
> `docs/arquivo/`. Status: `pendente` · `in progress` · `done` · `bloqueado`.
> Estado por jogo: `docs/game_status.md`. Débitos abertos: `docs/tech_debits.md`.

## Estado de base (2026-10-08)

- Core oficial `flycast2026` com funções nativas por assinatura (`hle_fn.cpp`),
  espera do frame com prazo = 2× intervalo medido (4.110), core reporta fps natural
  (4.112), `div32` nativo. **Tier2 aposentado** (opt-in, OFF no `retrorun.cfg`).
- Link direto do despacho dinâmico é padrão (4.120, `FC_NO_DYN_LINK=1` só para A/B).
  Opt-in ainda não promovido: `FC_TEX_GPU_MORTON` (4.103). HLE `memset`/`ocbp` (5.4) ligado mas não validado (4.119).
- **Prioridade = tempo amostrado** (bloco × amostras do `perf`), nunca proporção
  estática; `padroes_ineficiencia.txt` aposentado como fonte de prioridade.
- Método vigente no DC: **dump leve do JIT → análise estática → nativo por
  assinatura → `FC_STATE_HASH` → A/B** (skill `jit-nativo`).

## P0 — crashes e travas (parar aqui antes de otimizar)

| # | Item | Status |
|---|------|--------|
| 0.1 | `SH4ThrownException` não tratada (Shenmue no loading, SA2 após logos; Napple suspeito). SA2 e Shenmue só crasham com tier2 ON → **reconferir com OFF** e, se sumir, fechar; senão pegar o `throw` que escapa (backtrace em `samples.txt.gz`). Robustez: converter em `Do_Exception(epc,0x180,0x100)` (tentativa anterior não validada) — 4.114 | pendente |
| 0.2 | Skies Disc 2 não boota (`SIGSEGV ... was not in vram`, exit 133): validar o CHD com o core upstream; se bom, `FC_JIT_TRACE` | pendente |
| 0.3 | Teto de frame ~90-150 ms com ~100% dupes: Macross M3, PSO após load. EGG já resolvido. Olhar `rs.Wait`/timeout, CHD (`FC_CHD_PREFETCH`), AICA thread; comparar com upstream — 4.115 | pendente |
| 0.3b | **TR Chronicles não espera, está saturado** (emu ~95% de um core): 26% `bm_GetCodeByVAddr` + 12% `mmu_full_lookup` → lookup de bloco por vaddr com MMU (ex-1.5 "nunca investigado") — 4.121 | pendente |
| 0.4 | `asndynmt` crasha após escolher personagem; `meltyb` trava quando a luta começa (tier2 OFF) — 4.94/4.95 | pendente |

## P1 — estraga a experiência

| # | Item | Status |
|---|------|--------|
| 1.1 | Áudio estourado + vozes baixas (Grandia II, Napple): A/B `RETRORUN_AUDIO_TIME_STRETCH=0/1`, mixer do core, overruns altíssimos (8278 / 12550) — 4.116 | pendente |
| 1.2 | Memory card "sem espaço" (Napple, Shenmue II) + crash ao salvar: conferir `per_content_vmus` e o nome do VMU por jogo — 4.117 | pendente |
| 1.3 | MvC2 (glitches, 33-45 fps) e Project Justice (118 dupes, "passa de 60") — regressão? Re-testar com tier2 OFF e casar apresentado × novos × dupes — 4.118 | pendente |
| 1.4 | Le Mans: `declared_fps=7.5` (o core devia reportar 30), áudio quebrado em cold boot, load/menus lentos | pendente |
| 1.5 | Sonic Shuffle ~7-15 fps: laço de busca linear; dump leve → HLE por assinatura | pendente |
| 1.6 | Ritmo no fps do jogo é **comportamento do core** (sem opção): declara só 30/60 e, com alvo 30, o `retro_run` dorme até o prazo de 1/30 s. `retrorun_loop_declared_fps` voltou a `false` (nos de 60 só custava fps). Napple 30,2 fps novos, KOF Evo 59,0, Shenmue II empate — confirmar jogando — 4.112 | done (validar sensação) |
| 1.7 | Soulcalibur congela no boot pelo ES (4.30); Evolution 1 (CHD zstd) não abre | não investigado |

## P2 — otimização (ciclo `jit-nativo`)

- **Validar antes de manter/promover:** HLE `memset`/`ocbp` com `FC_STATE_HASH` +
  rever o `UpdateSystem` único por laço (4.119).

- **Alvos nativos (por tempo amostrado):** blocos com FPU são a maior categoria nos
  jogos <100% (Shenmue II 28%, Shenmue 24%, DOA2 43% da emu) → laços de vértices do
  **Shenmue II / Shenmue**; **spill do HLE do DOA2** (cluster `8C101BC4..C4E` ~15% da
  emu, rendeu ~1% por guardar/recarregar estado a cada fronteira — 5.6); AICA/ARM7
  no Shenmue II (>8%, 4.122); IDCT da Sofdec (`docs/fmv_plan.md`). `tas.b`, `ocbp`,
  `div1` e jmp `@rn` pesam ≤3% cada — não são alvo.
- **Despacho:** stubs/despachante 2-9% e blocos com `rts`/`jsr` 10-25% da emu; o link
  direto (4.120) só pega saltos monomórficos — `rts` polimórfico segue no lookup
  (próximo passo possível: pilha de retorno para `rts`).
- **MvC2 (VEL 99,5%):** emulação não é o limite; glitches/dupes são apresentação e
  render (thread principal ~42% de um core, quase tudo no driver Mali) — não o JIT.
- **Render (Mali):** ~640 draws × ~34 µs; só atlas/menos draws ajuda (4.37). Pool de
  RTT (item 3 do antigo plano de render) segue possível, sem medição que o priorize.
- **Morton na GPU:** rodar a bateria Naomi + mslug6 antes de promover (4.103).
- **DOA2/Zombie/Shenmue:** teto é o throughput do SH4 (4.20/4.34); só baratear o
  JIT por instrução (menos tráfego de contexto) move o número.
- **Standalone SDL:** plano futuro em `docs/standalone_plan.md` — não executar agora.

## Pendências de método/infra

- Regenerar os savestates de `ggxx`, `ggxxsla` e `sa2` (V12 antigo) e re-testar.
- Re-rodar a bateria DC com **tier2 OFF** (skill `bateria`) para fechar as linhas
  de `game_status.md` marcadas com tier2 ON (cvs2, MvC2, SA2, Shenmue II...).
