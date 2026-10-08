# Plano de melhoria do JIT ARM64 — técnicas do backend x86 aplicáveis

> Síntese de `docs/x86jit.md` (backend x86 32-bit, `core/rec-x86/`) e
> `docs/arm64jit.md` (backend ARM64, `core/rec-ARM64/`, o que roda no device
> alvo deste projeto), cruzando as duas arquiteturas pra achar técnicas do x86
> que endereçam achados REAIS já medidos nesta sessão (ver `docs/tech_debits.md`
> itens 1.1, 1.6, 1.7, 4.9). Não é uma lista de ideias soltas — cada item abaixo
> está ancorado num achado medido ou numa técnica já comprovada (rodando há anos
> no x86), nunca em "parece que ia ajudar".
>
> **Regra de ouro deste plano, reforçada pela própria sessão:** a tentativa de
> fast-path de Store Queue (item 1.7) foi implementada, testada, e **revertida**
> por regredir ~6% — a leitura de código sugeria que ia funcionar, a medição
> real mostrou o contrário. Nenhum item aqui deve ser considerado "pronto" até
> ser medido no device, com o mesmo protocolo (`retrorun3 --benchmark`, savestate
> determinístico, A/B limpo sem instrumentação residual) já estabelecido nesta
> sessão. Ver `CLAUDE.md` e `docs/history.md` pro histórico completo dessa
> disciplina.

---

## Resumo executivo

| # | Item | Endereça achado | Esforço | Risco | Prioridade |
|---|------|------------------|---------|-------|------------|
| 1 | Stubs de memória compartilhados + rewrite por ponteiro de chamada | Item 1.7 (Store Queue, achado central da sessão) + item 4.9 (custo de `GenCallRuntime`) | Alto | Médio (mitigado por padrão já comprovado no x86) | **✅ Feito (2026-09-15, escopo reduzido) — ganho real mas pequeno, ver nota no item abaixo** |
| 2 | `PushCallerSaved`/`PopCallerSaved` sem varrer `reg_alloced` inteiro | Item 4.9 (custo pago em ~7.400 chamadas/frame mesmo quando não há nada a salvar) | Baixo | Baixo | Alta |
| 3 | Cache inline de 1 entrada pra saltos dinâmicos, antes do lookup em `fpcb` | Nenhum item específico — hipótese nova baseada em técnica comprovada no x86 | Médio | Médio (depende de característica do Cortex-A53 não medida ainda) | Média |
| 4 | Compilação em duas camadas ("staging") | Item 1.6 (compilação de bloco, já **descartado como dominante**, ~1,2% do tempo) | Alto | Baixo retorno esperado | Baixa — não recomendado priorizar |
| — | `CheckBlock`: ARM64 já é mais enxuto que o x86 nesse ponto | Item 1.1 (já descartado) | — | — | Nenhuma ação — ver nota abaixo |

---

## Item 1 — Stubs de memória compartilhados (a técnica que resolveria o item 1.7 de verdade)

> **Status (2026-09-15): implementado e medido — ver `docs/tech_debits.md`
> item 1.7 e `docs/history.md` 2026-09-15 pros números completos.**
> A versão que foi de fato implementada é **mais estreita** do que a proposta
> original abaixo (passos 1-4): em vez de trocar o corpo de
> `GenWriteMemoryFast`/`GenReadMemoryFast` pra chamar um stub `mem_stub_fast`
> (o que teria custo — 1 `Bl` a mais — em TODO acesso de memória, inclusive o
> caso comum), só `ngen_Rewrite` foi alterado: continua regenerando
> `GenWriteMemorySlow` in-place no caso genérico (como já fazia), mas quando o
> fault é de Store Queue (`acc>>26==0x38`) chama um novo `GenCallStubAddr()`
> que aponta pro stub compartilhado `sq_write_stub` (gerado uma vez em
> `GenMemStubs()`). O caminho comum (`GenWriteMemoryFast` inline) fica 100%
> intocado — nem o "1 instrução a mais" do plano original foi pago. Isso evita
> de vez o trade-off não-medido da seção "Trade-off a medir" abaixo (o custo
> do stub genérico no caso comum), ao preço de resolver só o caso de Store
> Queue especificamente, não o caso genérico read/write — que continua
> reescrito in-place como sempre foi. **Resultado medido: ganho real e
> reprodutível (~1-3% em `core_average`, até ~6-8% em `core_p50`/frame
> típico) em kofnw e Metal Slug 6, sem regressão — mas pequeno, e não explica
> o gap grande relatado pelo usuário em Metal Slug 6 (cauda pesada/p95-p99
> quase não muda — suspeita agora é renderização, não memória).**

### O problema medido (recapitulando item 1.7)

O achado central desta sessão: o loop mais executado de pelo menos dois jogos
(MBAA, kofnw) é um `shop_writem` de 32 bits dentro de um loop de preenchimento
de Store Queue, e **cada uma das 16 palavras por burst paga uma chamada
`GenCallRuntime`→`_vmem_WriteMem32` completa** (salto pra fora do buffer JIT e
volta), porque o endereço de Store Queue não é RAM real — o fastmem otimista
falha (SIGSEGV) na primeira execução e o `ngen_Rewrite` do ARM64 reescreve
*aquele call-site específico* para sempre chamar a função genérica dali em
diante.

A tentativa de corrigir isso (`docs/tech_debits.md` item 1.7, `docs/history.md`
2026-09-14) foi: adicionar uma checagem inline (`addr>>26==0x38`) dentro de
`GenWriteMemoryFast`, gravando direto em `sq_buffer` quando batesse. **Resultado
medido: ~+6% mais lento**, não mais rápido — porque a checagem foi emitida **em
todo write de 32 bits do jogo inteiro**, não só nos de Store Queue (ver seção
4.7 de `docs/arm64jit.md` pro post-mortem completo).

### Por que o x86 não tem esse problema — e como

O backend x86 (`docs/x86jit.md`, seções 4.2-4.4) resolve exatamente o mesmo
problema (SH4 Store Queue, mesmo endereço físico, mesma checagem
`addr>>26==0x38`) com uma arquitetura fundamentalmente diferente:

- O caminho "rápido" de acesso a memória **nunca é inlinado por instrução**. É
  sempre um `call` para um **stub compartilhado, gerado uma única vez** no boot
  (`ngen_init()`/`gen_hande()`, `mem_code[modo][leitura|escrita][tamanho]`  —
  3×2×5 combinações, cada uma um pedaço de código gerado só uma vez e reusado
  por **todos** os blocos compilados depois).
- Quando o "modo rápido" (`mem_code[0]`) falha (SIGSEGV), o `ngen_Rewrite` do
  x86 **não regenera lógica nenhuma** no call-site — ele só troca o **alvo do
  `call`** (4 bytes, o campo de offset relativo de um `call rel32`) pra apontar
  pro stub certo: `mem_code[1]` (Store Queue) se `acc>>26==0x38`, senão
  `mem_code[2]` (genérico/seguro).
- Resultado: **o custo extra da especialização de Store Queue fica
  inteiramente contido no(s) call-site(s) que de fato acessam SQ** — nenhum
  outro write de 32 bits do jogo paga overhead nenhum a mais, porque a decisão
  "que stub chamar" foi resolvida **uma vez, em runtime, no primeiro fault**, e
  gravada permanentemente como qual alvo o `call` aponta — não como uma
  checagem repetida a cada execução.

### Por que isso não existia no ARM64 (e por que a tentativa anterior não podia ter funcionado desse jeito)

O ARM64 (`docs/arm64jit.md`, seção 4.3-4.4) **inlina** a sequência de fastmem
diretamente em cada call-site (`Ubfx`+`Add`+`Str`, exatamente 3 instruções —
`write_memory_rewrite_size=3`). O `ngen_Rewrite` do ARM64 **regenera essas 3
instruções in-place**, no mesmo espaço fixo, porque não há "stub" nenhum pra
apontar — é tudo local. Isso trava qualquer tentativa de adicionar lógica nova
ali: **não tem orçamento de tamanho de código pra caber uma checagem +
desvio + store direto** dentro de 3 instruções (por isso a tentativa anterior
teve que inlinar a checagem ANTES do trecho de 3 instruções, afetando todo
call-site em vez de só o que precisava).

### A correção proposta (adaptação do padrão x86 pro ARM64)

1. **Gerar, uma única vez, no init do dynarec** (análogo a `ngen_init()`/
   `gen_hande()`), um pequeno conjunto de stubs ARM64 compartilhados — pelo
   menos: `mem_stub_fast[tamanho]` (o que hoje é inlinado em
   `GenWriteMemoryFast`/`GenReadMemoryFast`), `mem_stub_sq[tamanho]` (o
   equivalente ARM64 do modo 1 do x86 — grava direto em `sq_buffer`), e
   `mem_stub_slow[tamanho]` (o que já existe, `GenWriteMemorySlow`/
   `GenCallRuntime`).
2. **Trocar o corpo de `GenWriteMemoryFast`/`GenReadMemoryFast`** para emitir
   um `Bl`/`Blr` pro stub `mem_stub_fast[tamanho]` em vez de inlinar
   `Ubfx`+`Add`+`Str` diretamente. O "orçamento fixo" agora é **1 instrução**
   (`Bl` relativo, como `GenCallRuntime` já faz — ver seção 7 de
   `docs/arm64jit.md`), não 3 — mais folga, não menos.
3. **Trocar `ngen_Rewrite`** pra, em vez de regenerar `GenWriteMemorySlow`
   in-place, só **patchar o campo de offset do `Bl`** (instrução de 4 bytes,
   largura fixa — trivial de reescrever um campo imediato sem afetar
   instruções vizinhas) pra apontar pro stub de SQ (se `addr>>26==0x38`) ou pro
   stub genérico (caso contrário).
4. **Nenhum outro call-site é tocado.** Só o que efetivamente faultar uma vez
   entra no jogo de reescrita — exatamente a garantia que faltava na tentativa
   anterior.

### Trade-off a medir (não assumir)

Isso troca "3 instruções inline, zero chamada, no caso comum" por "1 instrução
(`Bl`), mais o custo de entrar/sair do stub compartilhado (`Ret`)" **mesmo no
caso comum** (RAM normal, que é a maioria dos acessos do jogo). Ou seja, essa
mudança pode ela mesma ter um custo pro caso comum que a versão atual
(inlinada) não tem — o x86 aceita esse custo porque o call em si já é barato
lá (arquitetura CISC, call/ret nativos e bem previstos) e porque ganha
MUITO em flexibilidade de correção. **Não presumir que o mesmo trade-off vale
no Cortex-A53** — é exatamente o tipo de intuição que já se provou errada uma
vez nesta sessão (regra de ouro no topo deste documento). **Medir com o mesmo
protocolo A/B antes de considerar pronto**, comparando pelo menos: (a) caso
comum (jogo sem Store Queue pesada, ex. cena calma do Shenmue) pra ver se a
indireção extra pesa, e (b) caso Store Queue pesado (kofnw/mbaa) pra confirmar
que resolve o item 1.7 sem reintroduzir a regressão de 2D do item 4.9 por
outro caminho.

**Escopo/esforço:** este é o item de maior escopo do plano — envolve criar
infraestrutura nova (geração de stub, gerenciamento do buffer onde eles vivem,
adaptação de `ngen_Rewrite`), comparável ou maior que a extensão de regalloc
do item 4.9. Recomendação: implementar isolado num branch/commit próprio,
testar exaustivamente (múltiplos jogos, sessão de jogo longa, não só
benchmark curto — lição do item de crash do master, `docs/history.md`
2026-09-14/15) antes de considerar estável.

---

## Item 2 — `PushCallerSaved`/`PopCallerSaved` mais barato mesmo quando vazio

### O que os dois backends têm em comum

Tanto x86 (`FreezeXMM`/`ThawXMM`, `docs/x86jit.md` seção 3) quanto ARM64
(`PushCallerSaved`/`PopCallerSaved`, `docs/arm64jit.md` seção 3.4) fazem a
mesma coisa em espírito: salvar só os registradores caller-saved que estão
**de fato vivos** naquele ponto do bloco, não os 4 (x86) ou 16 (ARM64) sempre.
Isso já é bom design nos dois — evita `stp`/`ldp` desnecessário quando nada
precisa ser salvo.

### A diferença que pode importar

- **x86:** `FreezeXMM` itera um **array fixo de 4 elementos**
  (`xmm_alloc_regs[]`) chamando `SpanNRegfIntr` pra cada um — custo constante
  pequeno, independente de quantos registradores SH4 estão de fato alocados.
- **ARM64:** `PushCallerSaved` itera **`reg_alloced` inteiro** (um
  `std::map<Sh4RegType, reg_alloc>`, `core/hw/sh4/dyna/ssa_regalloc.h:582`) —
  toda vez, mesmo que nenhum `S16`-`S31` esteja alocado. Isso significa que
  **mesmo quando o `vlist` final fica vazio** (nenhum `Stp`/`Ldp` emitido), o
  custo de **montar a lista** (iterar o map, checar `IsFloat`, checar o range)
  ainda é pago — e isso acontece em **toda** `GenCallRuntime`, inclusive
  `UpdateSystem` (~7.400×/frame, item 1.2 de `tech_debits.md`).

### Melhoria proposta

Manter, no `Arm64RegAlloc`, um contador/bitmask incremental de "quantos
`S16`-`S31` estão atualmente alocados e vivos", atualizado nos mesmos pontos
onde `reg_alloced` já é modificado (alocação/spill/flush — `ssa_regalloc.h`),
em vez de recalcular varrendo o mapa inteiro a cada `GenCallRuntime`. Se o
contador for zero, `PushCallerSaved`/`PopCallerSaved` retornam imediatamente
sem nem tocar em `reg_alloced`. Isso não muda nenhum comportamento — só evita
trabalho redundante no caminho que já é comprovadamente quente (item 4.9).

**Esforço:** baixo — é uma mudança contida dentro de `Arm64RegAlloc`/
`ssa_regalloc.h`, sem alterar a lógica de alocação em si, só adicionar
contabilidade incremental. **Risco:** baixo, mas ainda exige o mesmo cuidado
de medir antes/depois (mesmo protocolo) — ganho esperado é pequeno e pode não
ser mensurável frente ao ruído normal de benchmark; vale medir mesmo assim,
já que o custo de implementar é pequeno.

---

## Item 3 — Cache inline de 1 entrada pra saltos dinâmicos (hipótese nova, não comprovada)

### O que o x86 faz que o ARM64 não faz

Pra blocos com destino dinâmico (`BET_DynamicJump/Call/Ret` — RTS, saltos
calculados, retornos de sub-rotina), o x86 (`docs/x86jit.md` seção 5, "cache
de 1 entrada por bloco") faz, **antes** de cair no lookup genérico:

```asm
cmp [pc_dinamico_atual], pBranchBlock->addr   ; é o mesmo destino de antes?
je  pBranchBlock->code                         ; se sim, salta direto
; senão, cai no stub de resolução genérico
```

O ARM64 (`docs/arm64jit.md` seção 5.2) vai **direto** pra tabela `fpcb`
(`Ubfx`+`Ldr`+`Br`, indexação O(1), sem branch condicional prévio nenhum).

### Por que isso pode importar no Cortex-A53 especificamente

`Br`/`Blr` com registrador (salto **indireto**) depende do preditor de
salto indireto da CPU pra não estolar o pipeline — e essas estruturas
costumam ter capacidade bem mais limitada (menos entradas, mais aliasing)
que o preditor de branch **condicional direto** numa CPU pequena/in-order
como o Cortex-A53. Se um `RTS`/salto dinâmico típico de jogo SH4
majoritariamente volta pro **mesmo lugar** repetidamente (padrão comum:
sub-rotina chamada de um só call-site, loop com salto calculado sempre pro
mesmo destino), um `cmp`+`b.eq` **direto** e bem previsto pode ser mais barato
que sempre pagar um salto indireto através da tabela `fpcb`, mesmo esta sendo
O(1) algoritmicamente.

### Por que isso é hipótese, não recomendação

**Não foi medido.** Diferente dos itens 1 e 2 (ancorados em achados já
confirmados desta sessão), este item vem só da comparação estrutural entre os
dois backends — é plausível, não comprovado. Antes de implementar, valeria a
pena instrumentar (ou usar `perf` com eventos de branch-miss, se o Cortex-A53
expuser esse contador — `perf list` neste device só mostrou
`branch-instructions`/`cycles`/`instructions`/`bus-cycles` genéricos, sem
evento de misprediction específico — ver se existe via evento bruto) pra
confirmar que saltos dinâmicos via `fpcb` de fato geram mispredictions
mensuráveis nos jogos-alvo antes de investir na implementação.

**Esforço:** médio. **Risco:** médio — mexe em código de dispatch usado por
todo bloco com salto dinâmico, região sensível (mesma classe de risco do
item 1, ainda que menor escopo).

---

## Item 4 — Compilação em duas camadas ("staging") — não recomendado priorizar

O x86 tem um mecanismo de "compilar barato primeiro, reotimizar depois de N
execuções" (`docs/x86jit.md` seção 7, `staging_runs`/`block->runs`). O ARM64
não tem — compila cada bloco "completo" de uma vez (`docs/arm64jit.md` seção 2
já observa isso: não há dois estágios).

**Por que não priorizar:** o item 1.6 de `tech_debits.md` já mediu que
compilação de bloco é **~1,2% do tempo total**, "real mas não dominante".
Tiered compilation reduziria esse número — mas 1,2% já é pequeno o bastante
que mesmo uma redução de 50% nesse custo não move a agulha no orçamento geral
de frame. Fica registrado aqui por completude de comparação entre os
backends, não como algo a implementar nesta fase.

---

## Nota: `CheckBlock` — o ARM64 já é mais enxuto que o x86 nesse ponto específico

Vale registrar o oposto de uma "melhoria a importar": comparando as duas
implementações de `CheckBlock` (anti-SMC), o **ARM64 já é mais eficiente**
que o x86 no caso comum. O x86 (`docs/x86jit.md` seção 6.2) emite o checksum
**incondicionalmente em todo bloco**, mesmo quando a proteção de página
(`read_only=true`) já deveria tornar esse checksum redundante — só troca o
alvo de falha pra uma versão com `int3` (assert de "nunca deveria disparar").
O ARM64 (`docs/arm64jit.md` seção 6) já faz `if (!mmu_enabled() &&
!force_checks) return;` **sem emitir nenhuma instrução** pra blocos
`read_only` — um no-op real, não um checksum que nunca deveria disparar.
Isso já bate com o resultado medido do item 1.1 (`CheckBlock` descartado como
causa de lentidão) — não há nada a "importar" do x86 aqui, e mexer nesse
ponto do ARM64 pra imitar o x86 seria, pelo que os dois documentos mostram,
uma regressão de design, não uma melhoria.

---

## Como usar este documento

Ordem recomendada de ataque, seguindo a mesma disciplina desta sessão inteira
(medir antes, medir depois, reverter se não bater): item 2 primeiro (baixo
risco/esforço, ganho pequeno mas praticamente garantido), depois item 1 (o
que realmente resolve o achado central da sessão, mas exige o maior cuidado de
implementação e teste), item 3 só depois de confirmar a hipótese de branch
misprediction com dado real, item 4 fora de escopo por ora.

Qualquer implementação a partir daqui deve: (a) ser medida com
`retrorun3 --benchmark`, mesmo savestate/cena de referência já estabelecidos
(kofnw/mbaa pra 2D, Shenmue pra 3D); (b) ser validada visualmente, não só por
FPS (lição do batching de draw calls, que quase quebrou renderização antes);
(c) ter os números — bons ou ruins — registrados em `docs/tech_debits.md` e
`docs/history.md`, sem exceção, mesmo se o resultado for negativo (como o
item 1.7 já demonstrou ser tão valioso de registrar quanto um resultado
positivo).
