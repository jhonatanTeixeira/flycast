# t2_tests — experimento: um único VIXL na thread 2

> Branch `t2_tests` (a partir de `dfbc6c350`). Experimento do usuário, só no
> **DOA2**. Não é para virar o padrão; é para medir uma arquitetura diferente.

## Desenho alvo

```
T1 (emu):   memoria -> decoder -> SHIL -> backend (SSA+regalloc+seleção) -> codigo pre-VIXL
                                                     |
                                      [SPSC + batching + park]  (fila + batch + sino)
                                                     v
T2 (worker):                         tier2 (otimiza: corta idle, funde blocos, regalloc entre
                                     blocos) -> VIXL (ÚNICO) -> ARM64 -> instala no T1
```

Diferença do design atual: hoje há **dois** VIXL (o do bloco, no T1, e o da
região, no T2) e o tier2 lê a **SHIL**. Aqui: **um** VIXL (no T2) e o tier2 lê o
**código pré-VIXL gerado pelo backend** (a saída do backend), aplicando as mesmas
otimizações que ele aplica hoje (cortar idle loops, fundir blocos, etc.).

## Handoff

Reusar o mecanismo já construído (4.83): **fila SPSC sem lock + batching +
park (futex)**. Muda só o *payload*: em vez do `RuntimeBlockInfoPtr`, vai o
**pré-VIXL do bloco** (ops após `ssa_Compile` + metadados de bloco).

## Questão de fundo (resolver cedo)

O T1 precisa **executar** o bloco na primeira vez. Se o T1 não emite (sem VIXL),
ele não tem o que rodar até o T2 entregar → **fallback = interpretador**
(`Sh4_int_Step`) enquanto a compilação não chega. É o custo desse desenho e o
que a medição no DOA2 vai mostrar (boot/picos com muito bloco novo).

## Passos

1. **Payload pré-VIXL:** definir o que atravessa a fila (a lista de ops otimizada
   do bloco + vaddr/BlockType/Branch/Next/cycles/host-code-range). Reusar o
   `sampleBuf`-style SPSC (u32/ponteiro) ou uma variante.
2. **T1 sem emissão (experimento):** no `rdv_CompilePC`, após `ssa_Compile`,
   empurrar o payload e **não** chamar o VIXL; marcar o bloco como "pendente".
3. **Fallback:** bloco pendente → `Sh4_int_Step` até o T2 instalar (flag no bloco).
4. **T2 = tier2 + VIXL:** o tier2 forma a região a partir dos payloads, otimiza
   (idle/merge/regalloc) e emite pelo VIXL; instala no ponto seguro (como hoje).
5. **Medir no DOA2:** fps/VEL/p50/p95/p99 + `state_compare` (correção) + o custo
   do fallback interpretado (contador de blocos pendentes).

## Riscos

- Fallback interpretado caro (A53) → o jogo engasga no arranque e em cena nova.
- O pré-VIXL do backend já tem regalloc/contexto por bloco; "fundir" exige
  desfazer isso (o mesmo problema que motivou o tier2 trabalhar na SHIL).
- Sem o bloco compilado no T1, o fallback de região desfeita some.
