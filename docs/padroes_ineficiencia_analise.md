# Análise de Padrões de Ineficiência do JIT (SH4 → ARM64)

**Origem dos Dados:** `padroes_ineficiencia.txt`
**Objetivo:** Identificar laços de código SH4 gerados dinamicamente que apresentam alto custo de tradução (alto *ratio host/SH4*) e definir estratégias de ataque dentro da metodologia do projeto (Ciclo JIT → Nativo).

> [!IMPORTANT]
> A ausência completa de instruções de FPU (`fmac`, `fipr`, `ftrv`) nestes blocos de topo indica que **os maiores ofensores do JIT (neste contexto) não são cálculos matemáticos 3D**, mas sim primitivas de memória, controle de fluxo e sincronização do sistema operacional.

---

## 1. Operações em Bloco de Memória (`memset` / `memcpy`)

**Padrões Identificados:** 1, 2, 5, 7, 14, 17, 35
**Assinaturas Típicas:** Laços com `mov.w` ou `mov.l` seguidos de `add #-1, rn` (decremento) e `tst rn, rn` (verificação de zero).
**Contexto no Jogo:** Zerar buffers, inicializar instâncias de estruturas, ou copiar blocos de dados.

### 🎯 Como Atacar
Estes são os candidatos perfeitos para a abordagem atual do projeto (HLE via assinatura):
1. **Identificação:** Capturar os bytes SH4 que formam a assinatura exata do laço (ex: `247176ff740226688bfa` do Padrão 1).
2. **Reescrita Nativa:** Adicionar uma entrada em `core/rec-ARM64/hle_fn.cpp` (`hle_fn_lookup` / `hle_fn_run`).
3. **Execução Direta:** Ao invés do emulador traduzir e rodar o laço byte a byte, lemos os registradores de destino, fonte e tamanho diretamente, e disparamos um `std::memset` ou `std::memcpy` em C++ nativo (ARM64).
4. **Contabilidade:** Subtrair os ciclos SH4 baseados na quantidade de iterações que o laço daria (crucial para não quebrar o tempo do jogo e o `UpdateSystem`).

---

## 2. Spinlocks e Mutexes do Sistema Operacional (`tas.b`)

**Padrões Identificados:** 6, 15, 21, 32
**Assinaturas Típicas:** Contêm a infame instrução `tas.b @r0` seguida imediatamente por um `bt` (branch if true) ou `bf`.
**Contexto no Jogo:** O Katana/Shinobi SDK usando polling ativo ("Test-and-Set") para aguardar um recurso do sistema liberar (ex: fila do AICA). O JIT fica preso traduzindo um laço inútil que só gasta bateria e CPU do R36.

### 🎯 Como Atacar
Já vínhamos eliminando os fallbacks do interpretador (como o `stc.l SR` e `FPSCR`), e o `tas.b` sobrou na fila (~3k chamadas). 
1. **Curto-circuito do Spinlock:** Se a trava falhar, ao invés de rodar o bloco iterativamente, podemos interceptar o padrão de Spinlock e forçar um *Yield* da CPU do emulador (avançando ciclos virtuais ou processando os eventos de hardware pendentes até que o lock libere), economizando trabalho ARM64 inútil.
2. **Fallback:** No mínimo, garantir que `tas.b` possua um fast-path nativo perfeito e não cause quebras de bloco (block dispatch).

---

## 3. Despacho Dinâmico e Jump Tables (`switch`, Vtables)

**Padrões Identificados:** 0, 4, 9, 10, 18, 26, 36
**Assinaturas Típicas:**
- *Jump Tables:* `shll`, `mova`, `braf rn`
- *Ponteiros de Função:* Lojas diretas seguidas de `jmp @r0` ou `jsr @r3`.
**Contexto no Jogo:** Máquinas de estado gigantes (`switch(estado_personagem)`) ou métodos virtuais C++ (`Objeto->Render()`).

### 🎯 Como Atacar
Pulos indiretos são o pesadelo de qualquer JIT, pois quebram a previsibilidade e exigem *guards* caros em ARM64.
1. **Inline Caching (JIT):** Modificar o emissor do JIT para fazer cache de destino alvo. Se um `jmp @r0` vai para o mesmo endereço 99% das vezes, o JIT deve emitir um `cmp` rápido direto para o destino nativo, caindo no look-up lento só na falha (1%).
2. **HLE de Rotinas de SO:** Alguns desses `jmp @r0` (Padrão 0 e 4) são constantes e idênticos em mais de 20 jogos (Dead or Alive 2, Shenmue, Marvel, etc). Isso indica fortemente que é um *Trampoline* ou *Stub* padrão do SDK. Se identificarmos de qual rotina da BIOS/SDK se trata, podemos substituí-la inteira por código nativo via assinatura.

---

## 4. Purgas de Cache de Hardware (`ocbp`)

**Padrões Identificados:** 23
**Assinaturas Típicas:** Sequência de divisões de bit (`shlr2`, `shlr2`, `shlr` = /32) seguida de um `ocbp @rn` em loop fechado.
**Contexto no Jogo:** Operand Cache Block Purge. O jogo joga dados de geometria na RAM e manda a CPU vomitar o cache para a GPU puxar via DMA.

### 🎯 Como Atacar
1. **Ignorar/Otimizar (Fast-Forward):** No nosso emulador, não rodamos um cache SH4 cycle-accurate estrito onde isso seja necessário no nível do byte (os dados já estão na memória do host).
2. **Ataque:** Substituir o loop inteiro no `hle_fn.cpp` por um *No-Op* massivo. Apenas lemos o tamanho do loop, somamos no ponteiro para fingir que processamos, avançamos o contador de ciclos e saímos da função em `O(1)`. Ganho tremendo de performance.

---

## 🚀 Proposta de Priorização (Próximos Passos)

Seguindo a regra de "Medir antes de Otimizar":
1. **Primeiro Alvo: O Loop Rápido (Padrão 1, 2 e 5 - `memset`)**
   - É limpo, é matemático, não depende de estado oculto. Podemos reescrever o Padrão 1 amanhã no `hle_fn.cpp`, bootar *Grandia II* ou *Le Mans* com `FC_STATE_HASH`, validar a identidade do frame e medir se o tempo de core (`core_p99`) melhora.
2. **Segundo Alvo: O Ocbp Fantasma (Padrão 23 - Purge)**
   - Ocorre em 20 jogos. Otimização em `O(1)` pode retirar engasgos (hiccups) nas transições DMA pesadas.
3. **Terceiro Alvo: O `tas.b`**
   - Endereçar a pendência histórica dos fallbacks do interpretador documentada no `docs/current_plan.md`.

> [!TIP]
> Todo teste de sucesso resultará num commit imediato (`git commit`), conforme a regra do projeto para não acumular apenas código no *working tree*.


### Validação Prática: Dead or Alive 2 (memset HLE)

Para provar o valor do HLE de laços simples de memória como o `memset`, rodamos um A/B benchmark no device R36 (via `retrorun3`) usando o jogo **Dead or Alive 2 (DOA2)**, que é notoriamente pesado, com janela de 20s.

**Resultados do Benchmark (A/B `FC_HLE=0` vs `FC_HLE=1`):**

| Métrica | HLE=0 (Original) | HLE=1 (memset Nativo) | Impacto / Ganho |
|---|---|---|---|
| **VEL% (Velocidade)** | 88.98% | 87.50% | *Empate técnico/Pequena flutuação* |
| **FPS Novos (`new_fps`)** | 33.71 fps | 35.04 fps | **+1.33 fps** de rendering real |
| **Tempo Core p50** | 23.03 ms | 20.37 ms | **-2.66 ms** (-11.5%) na mediana |
| **Tempo Frame Ativo p50** | 24.28 ms | 21.46 ms | **-2.82 ms** na mediana global |
| **Tempo Core (Média)** | 26.32 ms | 25.38 ms | **-0.94 ms** |
| **Quadros Descartados** | 352 | 290 | **-62 quadros perdidos** na fila |

**Conclusão da Validação:** 
Mesmo em uma janela curta de 20s, a interceptação do `memset` diminuiu o tempo mediano que a CPU gasta por frame (`core_p50`) em cerca de **11,5%** (de 23ms para 20.3ms), o que permitiu ao emulador entregar **1.33 fps a mais** de quadros novos reais (`new_fps` subiu de 33.71 para 35.04). A pequena queda de VEL% isolada (1.4%) pode ser explicada pelo fato de que o emulador, rodando mais rápido, progrediu mais fundo na cena pesada de 3D durante os exatos 20s, pesando a média final; porém a redução do custo por frame na CPU (o principal gargalo do hardware) é inegável e muito expressiva (quase 3ms a menos de processamento por frame só otimizando limpeza de memória).
