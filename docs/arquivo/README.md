# Arquivo — documentos compactados e planos obsoletos

Nada aqui é lido por rotina. Serve para consultar números, tabelas e o caminho de
raciocínio quando um doc ativo aponta para "ver history/tech_debits <item>".

| Arquivo | O que é |
|---|---|
| `history_2026-09-13_a_2026-10-08.md` | `history.md` integral |
| `tech_debits_2026-09-13_a_2026-10-08.md` | tabela completa de todos os itens (1.x a 5.7) |
| `game_status_2026-09-16_a_2026-10-08.md` | baterias, tabelas e imagens por jogo |
| `current_plan_2026-09-13_a_2026-10-08.md` | plano atual integral (fila de 8 itens, tier2, etc.) |
| `current_plan_2026-10-08_a_2026-10-10.md` | plano compactado de 2026-10-08 (P0 crashes, P1 experiência, P2 otimização); zerado em 2026-10-10 — itens abertos estão em `tech_debits.md` |
| `planos_obsoletos/` | planos e auditorias já executados, refutados ou superados |

## `planos_obsoletos/` — por que cada um saiu

- `profiling_plan` — auditoria estática de 2026-09-13; fila executada (só 4.2 sobreviveu).
- `arm64jit_improvement_plan` — itens feitos (stub de SQ), medidos sem ganho ou
  superados (despacho → 5.7; tiering → tier2 aposentado).
- `fpscr_jit_code_audit`, `fpscr_native_translation_plan`, `sh4_fpscr_external_research`
  — implementado e medido: sem ganho; lição em tech_debits.
- `gles_code_audit`, `rendering_improvement_plan` — itens feitos (invalidate,
  paleta, draws) ou refutados; os que sobram estão em `current_plan` P2.
  (`docs/mali_gles_best_practices.md` fica ativo como referência.)
- `texcache_vram_invalidation_plan` — diagnóstico refutado pela própria medição.
- `tier2_adaptive_plan`, `tier2_selfhealing_plan`, `tier2_patches`, `t2_tests_plan`,
  `tier2_na_bateria_2026-10-07` — tier2 aposentado em 2026-10-07.
- `thread_separation_plan` — topologia já aplicada (AICA e CHD em thread, tier2
  em thread e depois aposentado).
- `frame_pacing_plan` — pacer determinístico medido negativo; o modelo no-wait +
  espera com prazo (4.110) o substituiu.
- `shenmue_plan` — estado de 2026-09-19, superado.
- `jit_armv8_a_context_audit` — backend `jit_armv8_a` guardado.
- `bateria_2026-10-07_plan` — plano da bateria; itens migrados para `current_plan.md`.

## Docs ativos que sobraram em `docs/`

`arm64jit.md`, `x86jit.md`, `jit_study.md`, `mali_gles_best_practices.md`
(referência) · `sync_emu_render.md` (referência do mecanismo; fases 1-3 e a espera
com prazo já feitas) · `padroes_ineficiencia_analise.md`, `fmv_plan.md`,
`standalone_plan.md`, `sh4_threading_model.md`, `native_sdk_code.md` + `sdk_blocks/` (planos vivos) · `ctrl_socket.md` (ferramenta) ·
`current_plan.md`, `tech_debits.md`, `history.md`, `game_status.md`.
