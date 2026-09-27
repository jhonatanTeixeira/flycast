# Plano — tier2 adaptativo (knobs inteligentes)

> Apartado do `current_plan.md` por pedido (2026-09-27). Retomar depois.
> Contexto: `docs/tech_debits.md` 4.82 (branch following) e 4.83 (tier2 inteiro
> na thread, fila SPSC + batching + park). Lição de controle: 5.2 (baseline
> EWMA relativo disparava ao contrário da intenção).

## Ideia A — batch / intervalo de wake adaptativo (retorno pequeno)

O que é: hoje `T2QBATCH=64` (blocos por sino) e o wake do pipeline a cada 4096
polls (~110/s) são fixos. Adaptá-los ao estado da fila.

- **Sinal correto = taxa de chegada de blocos + nível da fila**, NÃO o fps.
- **Controlador:** AIMD — se a fila fica quase vazia entre wakes, aumenta o lote
  (menos sinos); se enche/descarta, diminui. Ou "acorda quando `fila ≥ N` OU
  `T` ms", com N/T da taxa observada.
- **Por que é baixa prioridade:** o sino custa ~1-2 µs; a 110/s = ~0,1-0,2 ms/s.
  O ganho de fps é ~0. Só evita **descarte de bloco** (fila cheia) e reduz a
  **latência de formação**. Não gastar rodadas de A/B aqui sem evidência.

## Ideia B — agressividade do tier2 adaptativa (aqui está o valor)

O que adaptar: **quais/quantes regiões formar** — hoje é fixo (limiar de calor,
`group_has_loop`, `check_regions` desfaz região de baixo reúso *depois* de
instalar). Adaptativo = decidir **antes** de instalar.

- **Sinais:** VEL (velocidade do jogo), custo da emu thread (`g_tier2EmuUs`),
  **reúso da região** (blocos por entrada). Se VEL < 100% e a emu é o gargalo →
  mais agressivo (mais regiões, limiar menor). Se VEL = 100% (render-bound) ou
  reúso baixo → recua.
- **Ganho esperado:** resolve o Shenmue sozinho — detecta reúso baixo e **não
  forma** a região (hoje forma, instala e descarta, pagando o custo). É o item
  "limiar de reúso antes de instalar", mas dinâmico.

## Armadilhas (obrigatórias no desenho)

1. **Oscilação:** realimentação com sinal ruidoso oscila (lição 5.2). Adaptação
   **lenta** (janelas de segundos, não frames) + **histerese**.
2. **Limiar absoluto**, não relativo: `VEL < 90%`, `reúso < X` — nunca "melhor
   que a média recente" (foi o que quebrou no 5.2).
3. **Nunca realimentar pelo fps** — ruidoso demais. Usar VEL/custo/reúso.
4. **Determinismo:** qualquer decisão adaptativa muda o código gerado; validar
   com `state_compare` (tier2 on×off) como no resto.

## Pré-requisito: contadores (barato, sem eles não há como controlar)

Adicionar ao dump do `FC_IDLE_FF_STATS` (ou `FC_TIER2_STATE`):
- blocos **descartados por fila cheia** (`t2QHead - t2QTail >= T2Q`);
- nº de **wakes** do worker e **tamanho médio do lote** drenado;
- **latência de formação** (tempo entre um bloco virar quente e a região instalar);
- **reúso** por região (já existe no log: "blocos por entrada") e tempo de vida.

## Ordem sugerida

1. **Contadores** (acima) — barato, habilita todo o resto.
2. **Adaptativo por reúso + VEL** (Ideia B) — é o que move o Shenmue. Decidir
   antes de instalar (substituir/complementar `check_regions`).
3. **Batch adaptativo** (Ideia A) — só se os contadores mostrarem descarte ou
   formação atrasada.

## Método de validação

A/B com fps + VEL + p50/p95/p99 (como sempre), `state_compare` para correção, e
a bateria (`docs/batery/`) como validador final. Cuidado com cena: usar
savestate fixa; o Shenmue avança conteúdo entre rodadas.
