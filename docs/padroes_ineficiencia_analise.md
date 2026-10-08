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


---

## 5. Epílogos de Função e Retornos (`rts` / Stack Pops)

**Origem dos Dados:** Varredura adicional em 287 MB de logs do JIT consolidados (`dcbat_consolidado.txt`).
**Padrões Identificados:** Blocos contendo desempilhamento de pilha e a instrução `rts` (Return from Subroutine). Ex: `4F26000B0009` (`lds.l @r15+,PR` -> `rts` -> `nop`).
**Contexto no Jogo:** O final (epílogo) de toda e qualquer função chamada no código C/C++ padrão do jogo. Aparecem sistematicamente como o gargalo mais repetido entre mais de 20 jogos (chegando a *ratios* astronômicos de 15.0 a 18.0 host/SH4 para blocos de apenas 3 instruções).

### 🎯 O Problema
Esses blocos são formados por duas ações que geram um volume gigantesco de código ARM64 para cada instrução SH4:
1. **O Custo do `rts`:** Assim como as Jump Tables, um `rts` é um pulo indireto (ele pula para o endereço dinâmico contido em `PR`). Como o JIT não sabe o destino durante a compilação, ele é forçado a injetar a rotina lenta de fechamento do bloco para consultar o *Program Counter* na tabela de hash/dispatch do emulador e só então saltar.
2. **Desempilhamento Sub-Ótimo:** Várias instruções em sequência lendo da pilha (`lds.l @r15+, PR`, `mov.l @r15+, r14`, etc.). Cada instrução gera código independente com overhead de verificação de memória (MMU) na CPU ARM64.

### 🎯 Como Atacar (Via Otimização de JIT)
Diferente dos laços `memset` ou de locks do SO, os epílogos são minúsculos e dispersos, tornando o ataque via HLE (`hle_fn.cpp`) inviável. A solução deve ocorrer no **Tier 2 do JIT**:

1. **Implementar Inline RAS (Return Address Stack):**
   Fazer o JIT manter uma minúscula pilha de endereços de retorno (buffer local). Quando o JIT emite um `bsr` (Branch Subroutine), ele guarda o PC no RAS nativo. Quando o `rts` é emitido, em vez de gerar código de *lookup* caro, ele gera uma checagem rápida (`cmp`) do `PR` atual contra o topo do RAS. Se o valor bater, um salto estático nativo (fast-path) é realizado, burlando 99% da ineficiência do retorno.
2. **Fusão de Cargas (Load Coalescing - ARM64 `ldp`):**
   Otimizar o motor de tradução do JIT para reconhecer múltiplos desempilhamentos em sequência (`@r15+`) e emitir menos instruções na CPU ARM64 fundindo as cargas em uma instrução `ldp` (Load Pair) onde for seguro, economizando ciclos de busca e acesso à memória do hospedeiro.


> [!NOTE]
> **Validação em Larga Escala (`dcbat_consolidado_off.txt`)**
> Uma varredura posterior no log bruto secundário (de 287 MB) focando estritamente em blocos maiores (≥ 5 instruções) reconfirmou que a ineficiência de contexto não se limita a blocos pequenos. Dos 15 maiores ofensores identificados na nova bateria, 14 eram epílogos massivos (ratios de até 9.0 em blocos de 10 instruções).
> Exemplo de padrão universal dominante encontrado na varredura:
> ```assembly
>   lds.l @r15+,PR
>   mov.l @r15+,r8
>   mov.l @r15+,r9
>   ... (até r13)
>   rts
>   mov.l @r15+,r14
> ```
> O único bloco não-epílogo neste Top 15 foi o padrão de despacho virtual (`jmp @r0`). Essa redundância entre diferentes sessões e tamanhos de bloco crava definitivamente que a implementação do **Inline RAS (Return Address Stack)** e da **Fusão de Cargas (Load Coalescing)** são as otimizações mais vitais que o compilador Tier 2 precisa receber.
