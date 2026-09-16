# Plano: nativizar `LDS Rn,FPSCR` / `LDS.L @Rn+,FPSCR` no JIT ARM64

> Síntese de `docs/fpscr_jit_code_audit.md` (auditoria interna deste fork) +
> `docs/sh4_fpscr_external_research.md` (pesquisa externa: flycast atual,
> redream, Dolphin, PPSSPP). Mesmo padrão de investigação já usado pra
> renderização (`docs/rendering_improvement_plan.md`) — dois agentes,
> um de código e um externo, cruzados aqui. **Nada foi implementado ainda.**

## Contexto (o que motivou isso)

Instrumentação desta sessão (`FC_IFB_COUNT`, `core/rec-ARM64/rec_arm64.cpp`)
mediu o fallback pro interpretador por opcode em Shenmue real (90s+90s,
2476 frames). `lds Rn,FPSCR` foi o opcode de maior volume, de longe:
**~196 hits/frame** (484.809 no total), seguido por `lds.l @Rn+,FPSCR`
(~21/frame) e `div1` (~62/frame). Cada hit paga o custo total de uma
chamada de runtime (sair do buffer JIT, chamar C++, voltar).

## O achado central: as duas investigações convergiram na mesma resposta

**Não existe um jeito de compilar isso como um "mov" contínuo, sem terminar
o bloco, com segurança.** Isso não é uma limitação deste fork especificamente
— é o mesmo resultado a que chegaram, de forma independente, o **flycast
atual** (`flyinghead/flycast`, ~10 anos de evolução a mais que este fork) e o
**redream** (outro JIT de SH4 totalmente independente). A causa raiz (achada
pela auditoria interna, `fpscr_jit_code_audit.md` §2): `FPR64`/`FSZ64` —
os campos que decidem COMO o decodificador traduz instruções de ponto
flutuante **daqui pra frente no mesmo bloco** — só são lidos do FPSCR real
uma vez, no INÍCIO da compilação do bloco (`decoder.cpp:954-955`,
`state_Setup()`). Se `LDS Rn,FPSCR` escrevesse um valor novo e o bloco
continuasse sendo decodificado com o `FPR64`/`FSZ64` ANTIGO, qualquer
instrução FP depois, no mesmo bloco, seria compilada com a precisão errada
— corrupção silenciosa, sem crash, do tipo mais perigoso de debugar.

**A solução real, usada tanto pelo flycast atual quanto pelo redream:**

1. Compilar a escrita em si nativamente (é só um `mov`/`store` — a parte
   cara não é a escrita, é o que vem DEPOIS dela no bloco).
2. **Terminar o bloco imediatamente ali** (`dec_End(pc+2, BET_StaticJump,
   false)` no flycast atual — mecanismo que ESSE FORK JÁ TEM e já usa
   pra outros casos, ver `decoder.cpp:1029-1037` na auditoria interna).
3. A próxima instrução SH4 vira o início de um bloco NOVO, compilado do
   zero, que lê o FPSCR real (já atualizado) na hora — sem risco de
   decisão de codegen desatualizada.

Isso é **exatamente o oposto** de como este fork (e o flycast atual, e o
redream) já tratam `FRCHG`/`FSCHG` — que só invertem 1 bit com efeito
**conhecido em tempo de compilação** (não um valor de runtime arbitrário)
e por isso continuam no mesmo bloco sem terminá-lo (`decoder.cpp:306`,
`FSZ64` já é atualizado assim neste fork hoje). A distinção que separa os
dois casos: efeito previsível em compile-time (`FRCHG`/`FSCHG`) vs. valor
de runtime desconhecido até a instrução executar (`LDS Rn,FPSCR`) — e
`LDS Rn,FPSCR` está inequivocamente no segundo grupo.

### Por que isso simplifica a recomendação da auditoria interna

A auditoria interna (`fpscr_jit_code_audit.md`) tinha sugerido, como
caminho de "risco médio", um atalho condicional — nativizar só quando os
bits PR/SZ não mudam, com fallback só quando mudam — o que exigiria
infraestrutura nova (um fim-de-bloco condicionado a um valor de runtime
arbitrário, diferente do `BET_Cond_0/1` existente que só olha `sr.T`/
`jdyn`). **A pesquisa externa mostra que os projetos reais não fazem essa
distinção condicional — terminam o bloco INCONDICIONALMENTE, sempre.** É
mais simples de implementar (reusa o `dec_End`/`BET_StaticJump` que já
existe, sem inventar mecanismo novo) e é o que dois projetos maduros e
independentes escolheram fazer. Não precisa do atalho condicional pra ter
o ganho principal (eliminar a chamada de interpretador na maioria dos
casos) com risco bem mais baixo.

### Precedente fora do SH4 confirma o padrão, não um atalho mais barato

Dolphin (PowerPC, JIT de 20+ anos) trata o bit equivalente mais "leve"
(`GQR`, decide forma de código mas com valor tipicamente constante por
call-site) com constant-propagation nativa — mas o bit mais parecido com o
problema completo do FPSCR (`HID2`/paired-single-enable) **cai 100% no
fallback pro interpretador**, nem tenta nativizar. PPSSPP (MIPS) consegue
nativizar sem terminar bloco, mas só porque os bits que importam lá mapeiam
DIRETO pro registro de controle de FPU do host (`FPCR`/`MXCSR`) — não
mudam a FORMA do código gerado, diferente do PR/SZ do SH4. Ou seja: não
existe, em nenhum dos quatro projetos maduros pesquisados, um atalho mais
barato que "termina o bloco" pro tipo específico de bit que o SH4 tem.

## Avaliação de risco final

**Baixo-médio**, e mais baixo que a estimativa inicial da auditoria interna
(que não tinha ainda o dado de que dois projetos reais já validam esse
caminho em produção). Reduz pra reusar mecanismo já existente
(`dec_End`/`BET_StaticJump`), sem inventar nada novo na infraestrutura de
decodificação/regalloc/SSA.

## Esboço de implementação (não feito ainda)

1. Em `core/hw/sh4/sh4_opcode_list.cpp`, dar um `decode` (`dec_Fill(...)`)
   pras duas entradas de `lds Rn,FPSCR`/`lds.l @Rn+,FPSCR` (linhas 240 e
   275) — hoje são `0`. Precisa de um `DecMode` novo (ou reaproveitar
   `shop_sync_fpscr`, que a auditoria interna achou **já totalmente
   construído no codegen ARM64/x64/x86, regalloc e SSA, mas nunca emitido
   em lugar nenhum** — `grep "Emit(shop_sync_fpscr"` não acha nada,
   `fpscr_jit_code_audit.md` §4).
2. Emitir a escrita (`shop_mov32` pro campo de estado do FPSCR, ou
   `shop_sync_fpscr` se ele já fizer o que precisa — checar o que
   `UpdateFPSCR()` faz, `sh4_core_regs.cpp:148-154`, antes de decidir).
3. Forçar fim de bloco imediatamente depois, igual ao flycast atual —
   `dec_End(state.cpu.rpc + 2, BET_StaticJump, false)`.
4. Build limpo (`make clean` com as MESMAS flags do build — ver lição
   nova no `CLAUDE.md` desta sessão), deploy, sanity check sem crash em
   kofnw/Metal Slug 6/Shenmue antes de qualquer medição.
5. Medir com o protocolo já estabelecido (frame time + fps, sempre os
   dois, sempre p50/p95/p99 + média — regra nova do `CLAUDE.md`) em
   Shenmue especificamente, já que é onde o achado apareceu.

## Correção (v2) — a premissa do plano acima estava furada

**O erro central deste documento:** ele tratou "terminar o bloco" como o
mecanismo NOVO a ser implementado. Mas este fork **já terminava o bloco
depois de toda escrita de FPSCR desde 2015** — `decoder.cpp:1034-1037`
(`OPCODE_SETFPSCR(...) && !is_delayslot → dec_End(rpc+2, BET_StaticJump)`),
presente no commit base `603814c9f`, dentro do caminho do fallback, que era
exatamente por onde esses opcodes passavam. Nem a auditoria interna nem a
pesquisa externa registraram isso, e a síntese citou essa linha como "o
mecanismo existe e pode ser reusado" sem perceber que ela **já estava sendo
aplicada nesses exatos opcodes**.

Consequência: a v1 não eliminou custo nenhum. Ela trocou
`shop_ifb → GenCallRuntime(handler_do_interpretador)` por
`shop_mov32 nativo + shop_sync_fpscr → GenCallRuntime(UpdateFPSCR)` —
**mesma travessia JIT→C++, mesmo corte de bloco**, só movendo dois acessos
de memória pra dentro do bloco JIT. Resultado medido: neutro no Shenmue,
levemente negativo no kofnw. Coerente.

### O dado que faltava

Contadores `FC_FPSCR_STATS` (novos, `sh4_core_regs.cpp`), medindo o que
cada escrita de FPSCR realmente muda:

| | kofnw (2D) | Shenmue (3D) |
|---|---|---|
| Escritas de FPSCR / frame | **9.628** | 92 |
| PR/SZ inalterado | **100,0%** (0 de 11,9M) | 11,1% |
| No-op total (nada mudou) | **60,1%** | ~0% |
| `FR` mudou (troca de banco) | 39,9% | 33% |
| `RM`/`DN` mudou | 0% | 20% |

Ou seja: no kofnw o jogo usa `LDS Rn,FPSCR` como **troca de banco de
registradores FP**, nunca pra mudar precisão — então os ~9.628 cortes de
bloco por frame (a única razão de o corte existir) eram 100% desperdiçados.
E 60% das chamadas a `UpdateFPSCR()` não faziam literalmente nada
(`ChangeFP()` só roda se `FR` mudou, e `setHostRoundingMode()` tem cache
próprio de `RM`/`DN`) — pagavam só a travessia da chamada.

### O que a v2 faz

Duas coisas que a v1 não fazia, ambas no `shop_sync_fpscr` do backend ARM64:

1. **Guard inline de no-op** — compara `fpscr` com `old_fpscr` em 3
   instruções e pula a chamada inteira quando nada mudou. Comprovado em
   medição: no-ops que chegavam em `UpdateFPSCR` caíram de **7.166.131 → 2**.
2. **Guard de PR/SZ em runtime** — em vez de terminar o bloco sempre, compara
   os bits PR/SZ com o valor de compile-time e só sai pro dispatcher no
   caminho frio onde eles realmente mudaram (`dec_write_fpscr()` em
   `decoder.cpp` passa o esperado em `rs1` e o PC de retomada em `rs2`).
   É a forma do `ir_assert_eq` do redream, já citada na pesquisa externa e
   descartada pela síntese original com base na premissa furada.

Sair do bloco no meio é seguro porque `shop_sync_fpscr` agora força
writeback de **todos** os registradores vivos (`ssa_regalloc.h`), a mesma
garantia que `shop_ifb` já exigia. `write_back` não desaloca o registrador,
então o resto do bloco continua usando os valores em registrador — é
bem mais barato que um fim de bloco (que obriga o bloco seguinte a
recarregar tudo da memória, além do dispatch).

Backends sem o guard (x64, x86, ARM32, cpp) mantêm o comportamento antigo
via a flag nova `ngen_features::FpscrGuard` — correto, só mais lento, sem
bug latente.

### Resultado medido (kofnw, 2 rodadas por lado, mesma cena)

| Métrica | Baseline | v1 (sem guard) | **v2 (com guard)** | v2 vs base (R1 / R2) |
|---|---|---|---|---|
| fps | 50,60 | 49,47 | **51,15 / 51,48** | **+1,1% / +1,7%** |
| `core_average` | 11,539 | 11,976 | **11,315 / 11,174** | **-1,9% / -3,3%** |
| `core_p50` | 10,000 | 10,225 | **9,932 / 9,854** | -0,7% / -2,3% |
| `core_p95` | 20,948 | 21,745 | **20,692 / 20,788** | -1,2% / -1,1% |
| `core_p99` | 39,456 | 43,181 | **38,673 / 36,244** | -2,0% / -2,7% |
| `active_frame_p95` | 29,295 | 30,067 | **28,850 / 28,640** | -1,5% / -2,2% |
| `active_frame_p99` | 46,889 | 50,977 | **45,460 / 44,605** | -3,0% / -1,9% |

**Ganho real e reprodutível**: as 8 métricas melhoraram nas 2 rodadas, mesma
direção — diferente do resultado misto/ruidoso da v1.

**Caminho frio validado em execução real:** no Shenmue, PR/SZ muda em 88,9%
das escritas (141.828 vezes em 60s), então o guard dispara e sai do bloco de
verdade — o jogo rodou correto, sem crash. Não é código morto.

## Decisão (v1, superada pela v2 acima)

**Implementado em 2026-09-16.** Build limpo, deploy, zero crash em 2 rodadas
de 90s+90s no Shenmue (~484k execuções reais do opcode mais comum sem
incidente) — a parte de correctness/segurança do plano se confirmou.

**Resultado de performance: sem ganho medido.** A/B mesma cena, mesmo
protocolo (ver `docs/tech_debits.md` item 1.8 pra tabela completa
frame-time+fps em p50/p95/p99+média): fps -0,2%, `core_average` +0,5%,
sinal misto em todos os percentis, cauda (p95/p99) levemente pior. Não bate
com a expectativa de ganho que a investigação (auditoria + pesquisa
externa) projetava.

**Por quê, em retrospecto:** tanto o caminho antigo (fallback pro
interpretador via `shop_ifb`) quanto o novo (mov nativo + `shop_sync_fpscr`)
terminam pagando a MESMA travessia de fronteira JIT→C++ via
`GenCallRuntime` (push/pop caller-saved + `BLR` + volta) — a mudança troca
"uma chamada que decodifica `Rn` e lê `r[n]` da memória dentro do handler
do interpretador" por "mov nativo + uma chamada um pouco mais magra
(`UpdateFPSCR` sozinho)", mas não elimina a travessia em si, que é o custo
que realmente domina. O plano não tinha identificado essa equivalência
antes de medir — é o tipo de coisa que só a medição real revela (ver regra
de ouro do `CLAUDE.md`).

**Decisão do usuário: manter implementado mesmo sem ganho isolado.**
Motivo: o código é correto e seguro (mesmo padrão usado há anos em 2 JITs
SH4 maduros e independentes), e a escrita de FPSCR agora sendo nativa pode
beneficiar algum fix futuro que se aproveite disso indiretamente, mesmo sem
ganho próprio hoje. Diferente do skip-Translucent (que era opt-in e foi
mantido desligado por padrão), esta mudança é incondicional — faz parte do
comportamento padrão do JIT a partir de agora.

### Shenmue: por que o número dele não decide nada aqui

Par back-to-back (90s+90s, guard vs baseline rodados em sequência):
fps +0,7%, `core_average` -1,5%, `core_p50` -4,7%, `core_p95` -0,9%,
`active_frame_p50` -1,2%, `active_frame_p95` -0,2% — mas `core_p99` +21,3%
e `active_frame_p99` +15,9%.

**A cauda do Shenmue não é medível neste setup.** O `core_p99` do MESMO
binário baseline deu 34,247ms numa rodada e 61,936ms noutra no mesmo dia
(+81%), porque o Shenmue boota do zero e, rodando mais rápido, avança mais
durante os 90s de warmup e cai numa cena diferente (a cutscene de neve, bem
mais pesada). Um delta de +21% está dentro desse ruído. Por isso o número
que vale é o do kofnw, que usa savestate e garante cena idêntica.

Registrado como pendência honesta: **para medir Shenmue de forma confiável
falta um savestate** (como o que o kofnw já tem), salvo num ponto fixo de
gameplay. Sem isso, qualquer conclusão sobre a cauda do Shenmue é ruído.

### Possível refinamento futuro (não implementado)

O guard só é necessário se existir alguma instrução de FP DEPOIS da escrita
de FPSCR no mesmo bloco — é só por causa delas que PR/SZ importa. Um
pós-passe no fim de `dec_DecodeBlock()` poderia varrer a `oplist` e
desativar o guard (limpar `rs1`) nos blocos em que nenhuma op de FP segue a
escrita, eliminando tanto a comparação quanto a saída de bloco nesses casos.
Também vale investigar fazer a saída fria do guard usar block linking
(`GenBranch(pBranchBlock->code)`) em vez do dispatcher genérico
(`arm64_no_update`), que é o que o fim de bloco antigo usava e é mais barato
— relevante para jogos como o Shenmue, onde PR/SZ muda de verdade em 88,9%
das escritas e portanto o caminho frio é o comum.
