# Plano — Self-healing do tier2 (região ruim → fallback pro tier1)

> Origem: crash do tier2 no MvC2 e no CvS2 (item 4.86 do `tech_debits.md`,
> sessão 2026-09-27). Objetivo do usuário: **se a região gerar um crash, ela
> faz fallback pro tier1 ou desfaz sua modificação**, sem derrubar o jogo.
> Contexto de topologia: `docs/thread_separation_plan.md`.

## 0. O problema em uma frase

Uma região compilada pode **corromper estado** (RAM/TA/SQ) sem dar fault
detectável, e quando dá fault o desfazimento é **tardio** (próximo ponto
seguro) — então a região continua executando e corrompendo. Não existe hoje uma
rede de segurança que cubra as regiões de vértice/SQ.

## 1. O que já existe (e onde falha)

| Mecanismo | O que faz | Buraco |
|---|---|---|
| `tier2_fault` | emula o acesso fora da RAM e agenda o desfazimento (`scUnhookId`) | **não aborta a região**; ela segue até o ponto seguro |
| `tier2_safe_point` | desfaz a região marcada (`unhook` + `slowMem`) | só roda no ponto seguro (`&63`); tarde demais |
| `tier2_selfcheck` | replay no interpretador + compara contexto, desfaz se divergir | **recusa** região com `hasPref`/`hasWrite` (`tier2.cpp:2443`) — o exato caso do MvC2/CvS2 |
| `tier2_disable_for_pattern` | padrão de código conhecido → desliga tier2 no jogo | precisa de assinatura; reativo e grosseiro |
| `check_regions` (bpe) | desfaz região de baixo reúso | não pega região "quente e errada" |

## 2. Mecanismos candidatos (ranqueados)

### A. Abortar a região no 1º fault — fallback imediato pro tier1 (recomendado)

**Ideia:** quando `tier2_fault` detecta um acesso fora da RAM dentro de uma
região, em vez de emular e continuar, a execução **sai da região na hora** e o
bloco que deu fault é reexecutado pelo **JIT normal** (tier1), que tem o
caminho lento correto. A região é marcada morta e desfeita.

**Por que é o certo:** é literalmente o "fallback pro tier1" pedido; fecha a
janela de corrupção no primeiro sinal de problema; o JIT normal já sabe lidar
com o acesso (fastmem + `ngen_Rewrite` + stubs).

**Como implementar (esboço):**
1. No `tier2_fault`, localizar a região pelo PC e o **bloco** que deu fault
   (mapear host-PC → vaddr do bloco; a região já guarda `blocks`/faixas de
   código).
2. Escrever no contexto: `ctx.pc = vaddr do bloco que deu fault`, salvar todos
   os registradores SH4 vivos (como o `exitStub` faz) e `SR.T`.
3. Redirecionar o PC do host para um **epílogo comum da região** (bail) que
   restaura o estado do host e faz `Ret()` de volta pro despachante da emu
   thread. O despachante roda o bloco pelo JIT normal → correção.
4. `unhook` imediato (ou no ponto seguro) + `badBlocks.insert(bloco)`.

**Ponto delicado:** o epílogo comum precisa existir. Se não existir, emitir um
`bailStub` por região (como o `exitStub`, mas sem destino fixo — o destino é o
`ctx.pc` que o handler escreve). É a parte a validar primeiro.

### B. Estender a verificação neutra para `hasPref`/`hasWrite`

Hoje o replay só é seguro em região de leitura pura. Para cobrir escrita/SQ,
salvar/restaurar também o estado da TA/SQ (`ta_tad`, `sq_buffer`, `ta_fsm`) e
comparar RAM+contexto. Mais caro e mais complexo, mas pega corrupção silenciosa
(sem fault). É o item 2 do `current_plan.md` ("só regiões verificadas").
**Complementar ao A, não substituto.**

### C. Tornar não-fatal o `die()` do `YUV_data`

`pvr_mem.cpp:145` mata o processo, mas a linha seguinte já chama `YUV_init()`
(o autor queria recuperar). Trocar `die()` por `WARN_LOG` + `YUV_init()` para
o jogo **sobreviver** ao stream corrompido (um frame glitchado). É descrash
imediato, mas **mascara** o bug — usar só como rede de último recurso, nunca
como "a correção".

### D. Corrigir os bugs pontuais do `tier2_fault`

- Load de grupo com offset imediato (`ldr wt, [xn, #imm]`,
  `insn & 0xFFC00000 == 0xB9400000`) não é decodificado → `return false`
  (`tier2.cpp:2811`) → SIGSEGV segue pro `ngen_Rewrite`. Adicionar o caso.
- Auditar `sqCall` (`tier2.cpp:931`): o `decision` (w15/w16) é scratch do host e
  **não é salvo** em volta da chamada `do_sqw_nommu`; se o slot de atraso de um
  `jcond` contém `pref`/call, o desvio pode usar decisão clobberada. Verificar.

## 3. Ordem sugerida

1. **D** primeiro (bugs pontuais baratos; podem ser a causa direta).
2. **A** em seguida (a rede de segurança que o usuário pediu).
3. **B** depois (cobre corrupção silenciosa).
4. **C** só se sobrar crash mesmo com A+B (último recurso).

## 4. Validação

- **Alvo:** MvC2 e CvS2 não crasham, com tier2 on, no mesmo ponto (boot/intro e
  com savestate de luta).
- **Métrica:** fps + VEL + p50/p95/p99, A/B tier2 on×off, mesmo savestate/cena.
- **Correção:** `state_compare` (tier2 on×off) + bateria (`docs/batery/`).
- **Não regredir:** DOA2 (região de vértice que hoje ganha VEL +3,6) e Shenmue.
- **Registrar:** `tech_debits.md` (status) + `history.md` (timestamp).

## 5. Riscos

- O bail (A) reexecuta o bloco pelo JIT normal: garante correção, mas paga o
  custo de sair/entrar; em região que faulta muito, pode virar ping-pong
  (mitigar: `badBlocks` impede a região de se reformar com aquele bloco).
- Redirecionar o PC no signal handler é sensível: validar que o epílogo não
  depende de registradores que o handler não restaurou.
- Sempre com `state_compare`: qualquer caminho de fallback muda o timing de
  compilação, mas **não** pode mudar o estado emulado.

## 6. Referências

- `docs/tech_debits.md` 4.86 — o achado do crash (região de vértice/SQ).
- `docs/thread_separation_plan.md` — topologia e invariantes.
- `core/rec-ARM64/tier2.cpp` — `tier2_fault` (2730), `tier2_safe_point` (2608),
  `tier2_selfcheck` (2472), `exitStub` (970), `sqCall` (931).
- `core/hw/pvr/pvr_mem.cpp:141` — `YUV_data` (a `die()`).
- `docs/t2_tests_plan.md` — o desenho "um VIXL na thread 2" e o risco do
  fallback interpretado.
