# Plano atual

> **Zerado em 2026-10-10.** O plano anterior está em
> `docs/arquivo/current_plan_2026-10-08_a_2026-10-10.md`; os itens abertos dele (crashes,
> áudio, memory card, MvC2, Le Mans, Skies Disc 2...) estão em `docs/tech_debits.md`.
> Este plano cobre **três frentes de desempenho do Dreamcast**: nativizar funções de
> biblioteca do SDK em todos os jogos que as usam, limpar laços de espera que o idle
> fast-forward não pega, e deixar só o código quente na cache de instruções.
> Status: `pendente` · `in progress` · `done` · `bloqueado`.

## Regras de execução (valem para todo item)

1. **Implementar e ver no que deu**: sem rodada de medição antes; o A/B depois responde.
2. **Validar bit a bit antes de medir**: `FC_STATE_HASH` + `FC_RTC_FIXED` +
   `FC_INPUT_NEUTRAL`, tier2 OFF, frame a frame **idêntico** ao core anterior, em cada
   jogo que o item toca. Nativo e layout de código não podem mudar o resultado.
3. **A/B de 2 rodadas** (savestate, cfg de debug — nunca o oficial —, `perfmax`):
   VEL%, fps novos, dupes, frame time p50/p95/p99; anunciar antes o que cada rodada mede.
4. **Commit a cada item validado**; números em `history.md`, linha do jogo em
   `game_status.md`, lição em `tech_debits.md`.
5. Build: flags explícitas do `CLAUDE.md`, `-j2`; `make clean` com os mesmos argumentos
   depois de mexer em header amplo.

Referências: `docs/native_sdk_code.md` (método), `docs/sdk_find/` (varredura + ranking),
`docs/sdk_find/pseudo/` e `docs/sdk_blocks/*_pseudo.cpp` (o que cada função faz, por
jogo), `docs/jit_hot_path.md` (cache de instruções), `docs/sh4_threading_model.md`.

## Ordem

| Etapa | O quê | Por quê nesta posição |
|---|---|---|
| **0** | Remoção do tier2 | aposentado (opt-in, OFF); tira ~3.000 linhas e os ganchos do emissor antes das etapas A e D mexerem nele, e libera a área reservada do cache para a D3 |
| **A** | `hle_fn` relocável | pré-requisito de todas as nativizações em mais de um jogo |
| **B1 + C1** | T&L da biblioteca dos jogos de luta + espera de quadro do TMU | maiores pesos medidos (Shenmue II/DOA2 ~6%, EGG ~9,6%) |
| **D1 + D2** | quente/frio no emissor + versão por página | genéricos, todos os jogos, sem mudar o código do corpo |
| **B2, B4, B5** | cor ARGB (MvC2), cabeçalho Kamui2 (13 jogos), IDCT da Sofdec (7 jogos) | alto peso, funções fechadas |
| **D3** | arena quente | depende de D1 |
| **B3, B6, C2-C4, D4** | malhas com culling, memset/ocbp, demais esperas, menos bytes por instrução | maior esforço ou ganho menos certo |

## 0 — Remoção do tier2

O tier2 foi aposentado em 2026-10-07 (opt-in, `flycast2026_tier2 = disabled`): o ganho
não compensava os bugs de correção (4.93 etc.). O caminho quente (D) e as nativizações
(A/B) substituem o que ele tentava. Pedido do usuário: remover o código.

| # | Item | Status |
|---|------|--------|
| 0.1 | Remover `core/rec-ARM64/tier2.cpp` (~3.000 linhas) e `tier2_doa2.S` do build (`Makefile.common`) e do repositório. | pendente |
| 0.2 | Tirar os ganchos: `rec_arm64.cpp` (`tier2_entry_for` no `ngen_Compile` e demais, 36 referências), `driver.cpp` (`tier2_code_reserve`), `blockmanager.cpp`, `decoder.cpp`, `sh4_interpreter.cpp`, `sh4_interrupts.cpp`, `Renderer_if.cpp`, `nullDC.cpp`, `libretro.cpp`/`common.cpp` (opção e variáveis `FC_TIER2_*`). O `hle_fn.cpp` usa `t2_last_pc` (zera o estado do tier2 antes do `UpdateSystem`): remover junto. | pendente |
| 0.3 | Remover a core option `flycast2026_tier2` (`libretro_core_options.h`). A linha nos cfgs do device fica sem efeito; tirar dos cfgs só com o pedido do usuário (cfg oficial). Ferramentas e docs que citam `FC_TIER2_*`/regiões do tier2 (`jit_lite_report.py --log`, skills) atualizadas. | pendente |
| 0.4 | Validação: com o tier2 já OFF, a remoção é neutra → `FC_STATE_HASH` idêntico (DOA2, Shenmue II, Napple) e boot dos jogos da bateria; `make clean` com os argumentos do build (headers amplos mudam). Lição e números do tier2 continuam em `tech_debits.md`/`docs/arquivo/`. | pendente |

## A — Infraestrutura: `hle_fn` relocável (4.125)

| # | Item | Status |
|---|------|--------|
| A.1 | Reconhecer função por **bytes em qualquer endereço**: no `ngen_Compile`, hash dos primeiros opcodes do bloco → candidatos → comparação completa dos trechos (offsets relativos à entrada). Tabela de funções com variantes (uma entrada por versão do SDK). | pendente |
| A.2 | **Base relativa**: `ENTER`/`BAIL`, entradas extras (cabeça de laço) e literais PC-relativos viram `base + offset`; constantes do literal pool lidas da RAM do jogo na hora. | pendente |
| A.3 | Migrar `lightxf`, `stripemit` e o laço do DOA2 para a forma relocável; `FC_STATE_HASH` idêntico no Napple e no DOA2 (não pode regredir). | pendente |
| A.4 | Ligar nos outros jogos (código já idêntico, `docs/sdk_blocks/`): `lightxf`/`stripemit` em Evolution 1/2, RE CV, Skies; laço do DOA2 em MvC2, Shenmue II, Power Stone, Project Justice. Hash por jogo + A/B no Shenmue II e no Evolution 1. | pendente |

## B — Nativizações (por peso medido; pseudo pronto em `docs/sdk_find/pseudo/`)

| # | Item | Jogos | Peso | Status |
|---|------|-------|------|--------|
| B1 | **Rotina de T&L da biblioteca dos jogos de luta**, despacho por r8 no +000: `r8 == 0` strips com luz difusa (`strips_com_luz_difusa`), r8 ímpar strips sem luz e r8 com luzes direcional/pontual e triângulos (`strips_transformados_e_iluminados`), r8 par ≠ 0 clamp (já nativo). Variantes: Power Stone (PCW de fim de strip), Shenmue II (2ª luz por callback). | DOA2, MvC2, CvS2, Power Stone, Project Justice, Shenmue II | Shenmue II 6,0%, DOA2 6,3% + 2,6%, MvC2 2,5% | pendente |
| B2 | Vértices com cor ARGB (`vertices_com_cor_argb`: strips e triângulos) | MvC2, DOA2, CvS2 | MvC2 6,1% | pendente |
| B3 | Lista de malhas com culling (`lista_de_malhas_com_culling`): chama os laços do B1 por `bsr`; variantes Shenmue II (cópia alternativa + 2ª luz) e Power Stone | 6 jogos de luta + Shenmue II | DOA2 2,1%, Shenmue II 1,6%, MvC2 1,3% | pendente |
| B4 | Cabeçalho de polígono da Kamui2 (`cabecalho_de_poligono`): ~230 ciclos em 44 blocos curtos → 1 chamada | 13 jogos | EGG 4,2% | pendente |
| B5 | IDCT do macrobloco da Sofdec/MPV (`idct_do_macrobloco`); ler as matrizes A/B pela RAM; validar numa FMV (Evolution 1, Napple). Depois, a do RE CV (`fmv_plan.md`) | 7 jogos (+ RE CV) | Napple 5,4% (fora de FMV); FMV hoje a 100% com o clock reduzido (4.123) | pendente |
| B6 | Validar o HLE de `memset`/`ocbp` com `FC_STATE_HASH` e corrigir o `UpdateSystem` único por laço (parar na volta em que o JIT pararia) | 7-20 jogos | ≤ 6% (Le Mans) | pendente (4.119) |
| B7 | Fechar o laço externo do HLE do DOA2 (spill de estado a cada fronteira) | DOA2 (+ B1) | cluster ~15% rendeu ~1% | pendente (5.8) |

## C — Laços de espera que o idle fast-forward não pega

| # | Item | Jogos | Peso | Status |
|---|------|-------|------|--------|
| C1 | **Espera de fim de quadro lendo o TMU0** (`espera_de_quadro_com_timeout`): reconhecer por assinatura (cabeça do laço + bytes das folhas `tmr/dif/cnv`) e pular até `min(próximo evento do agendador, prazo do timeout calculado pelo TCNT0)`. O TCNT0 sai do tempo do agendador, então pular o tempo mantém o timer coerente; o cuidado do 4.43 (laço que lê hardware) é respeitado por calcular o prazo do próprio timer. | 10 jogos | EGG ~9,6% | pendente (4.126) |
| C2 | Servidor de 32 slots no laço ocioso do Napple (`servidor_de_32_slots_cronometrado`, laço `8C1368D4`): ver se o servidor só trabalha quando o TMU passa de um limiar; se sim, o laço ocioso inteiro pula até o próximo evento como em C1. Senão, nativo da varredura. | 8 jogos + Le Mans | Napple 5,9% | pendente |
| C3 | Espera da Sofdec na FMV: achar o laço de espera (captura leve com `FC_FMV_CLOCK=0`) e trocar o clock reduzido (4.123) por assinatura de idle fast-forward | jogos com Sofdec | FMV | pendente |
| C4 | **Varredura de esperas** no `sdk_find`: laços para trás cujo corpo só lê memória/MMIO (flag, TMU, registrador do PVR/GD) e compara, em todos os jogos; lista com peso do `perf` → novos itens C | 21 jogos | — | pendente |

## D — Caminho quente do JIT (`docs/jit_hot_path.md`, 4.127)

| # | Item | Status |
|---|------|--------|
| D1 | Separar quente/frio no emissor: área fria por fatia de 1 MB do cache; falha da checagem de código, fim de fatia (`intc_sched`), misses do fim de bloco e literais vão para o frio. Mesmo código executado → hash idêntico. | pendente |
| D2 | Versão por página no lugar da checagem byte a byte dos blocos em página não protegida (`ro=0`): 3 instruções na entrada; o frio compara os bytes só quando a versão muda. Alvo: Shenmue (24 dos 79 blocos mais quentes). | pendente |
| D3 | Arena quente: amostrar no `intc_sched` o bloco onde a fatia termina, promover os quentes para uma área contígua em ordem de cadeia (salto provável vira queda direta), com decaimento e saída quando enche. | pendente |
| D4 | Menos bytes por instrução SH4: T direto no desvio; registradores do SH4 em registradores do host dentro do bloco. | pendente |
| D5 | (opcional) Lado C++: agrupar as funções quentes do core (`hot`/`-freorder-functions` ou perfil do device). | pendente |

Nas rodadas de A/B do D, medir também `L1-icache-load-misses` e `iTLB-load-misses` da
thread de emulação (`perf stat`), nas mesmas rodadas.
