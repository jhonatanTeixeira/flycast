# O tier2 na bateria DC+Naomi de 2026-10-07 — fez diferença? quebrou laços? dá pra HLE?

Base: bateria pelo ES (core `961f9a55`, `retrorun3` `53040f37`), **tier2 ligado**
(`flycast2026_tier2 = enabled`). Fonte: os logs de tier2 + os dumps de JIT de
`/roms2/dcbat/20261007-15xx…16xx_<jogo>/` (consolidados no host) + o clustering dos
blocos SH4.

## 1. O que o tier2 FEZ (números)

| | total |
|---|---|
| regiões formadas | **721** |
| regiões **com laço** (`volta@` = aresta de volta) | **495** |
| regiões "ok" (quentes, formadas e mantidas) | **414** |
| `BAIL` (acessou MMIO → desfeita) | 68 |
| grupos **recusados** (não formou região) | 66 |
| **`iNimp`** (executou dados) | **8** |

Formou região em **quase todos** os jogos (DOA2 46, Grandia II 67, Le Mans 76, Shenmue II
67, Skies D1 60, Soulcalibur 45…). Ficaram em **0** (não chegaram a rodar código de jogo):
`cvs2`, `Skies Disc 2` (crash), `Sonic Shuffle`, `TR Chronicles` (tela preta).

**Leitura:** o tier2 **não é inerte** — ele forma regiões de verdade, e a maioria
(495/721) tem **laço de verdade** (`volta@`). Não é só "juntar 2-3 blocos retos".

## 2. Fez DIFERENÇA (a favor ou contra)?

O que os dados desta bateria permitem dizer:
- **Formou e manteve** 414 regiões quentes → em teoria está emitindo código mais
  eficiente nesses pontos.
- Mas os **efeitos colaterais medidos** nesta bateria são todos **negativos**:
  - **SA2**: crash por `iNimp` **só com o tier2 ligado** (A/B feito: ON → exit 134;
    OFF → sem crash). O tier2 é a causa.
  - **Napple/Shenmue**: também `iNimp` (2 cada) — mesma família (região executando dados).
  - **MvC2**: o usuário sentiu **regressão + glitches** (a suspeita é o tier2).
- **O que falta para fechar:** o **A/B tier2 ON×OFF nesta bateria** (mesma cena) não foi
  rodado. Sem ele, não dá pra dizer se as 414 regiões quentes ganham mais do que os
  efeitos colaterais custam. **Próximo passo natural.**

Histórico que ajuda a interpretar: no **Naomi** o tier2 já foi medido como **perda
líquida** (bateria 2026-09-28: quase todos quebravam com tier2 ON); no **DC 3D** ele
ajuda (4.106: Napple VEL 77→87%). Ou seja: o sinal provavelmente **depende do sistema**.

## 3. Matou laços de verdade?

**Sim, em 3 jogos.** O `iNimp` (instrução ilegal = o jogo executando **dados**) é a
assinatura do **4.74**: quando a região mantém um laço com store no delay slot (ou
condição equivalente), o fluxo sai do trilho e o PC vai parar em dado. Nesta bateria isso
aconteceu em **SA2 (crash), Shenmue e Napple** — e no SA2 o A/B provou que é o tier2.

Ou seja: o tier2 **pegou laços de verdade (495)** e, em alguns casos, **quebrou** esses
laços (8 `iNimp`). Não é "não fez nada" — é "fez, e às vezes quebrou".

## 4. Esses laços poderiam estar no HLE em vez do tier2?

**Boa parte sim.** Cruzando os blocos que estão **dentro de regiões do tier2** com os
**padrões recorrentes** (assinatura de bytes SH4, ≥5 ocorrências e ≥3 jogos):
**700 padrões recorrentes aparecem dentro de regiões do tier2** (de 3954 blocos em
região). Os mais espalhados:

| jogos | ocorr | instr | bytes SH4 (assinatura) | leitura |
|---|---|---|---|---|
| 16 | 17 | 5 | `25407501626235228bfa` | laço `mov.w r5,@r4` / `add` / `tst` / `bf` (cópia de words) |
| 16 | 19 | 4 | `7d013dc38ff87e04` | laço com `add #1,r13` / `cmp/ge` / `bf.s` / `add #1,r14` |
| 16 | 19 | 4 | `65f3e6044b0b64e3` | `mov r15,r5` / `mov #4,r6` / `jsr` / `mov r14,r4` |
| 16 | 20 | 5 | `7701205037628ffb7001` | laço de varredura (`add #1,r7` / `mov.b` / `cmp` / `bf`) |
| 14 | 14 | 5 | `25427504636235328bfa` | variante da cópia de words |

**Conclusão:** os laços que o tier2 captura são, em boa parte, **os mesmos padrões
recorrentes** que dariam pra **reescrever em nativo e plugar por assinatura no HLE**
(`hle_fn_lookup`, que casa por `memcmp` dos bytes SH4 — a assinatura já está no relatório).
E o HLE tem uma vantagem sobre o tier2 nesses casos: **não recompila região** (sem o custo
de emissão/guarda) e **não tem a classe de bug do 4.74** (executar dados) — porque o código
é escrito à mão, com os ciclos e a fronteira de interrupção explícitos.

Ou seja, a resposta à pergunta: **o tier2 pegou laços reais; em alguns ele quebrou; e
esses laços recorrentes são justamente os melhores candidatos a HLE** (assinatura estável,
aparecem em muitos jogos, e o HLE não tem o risco do tier2).

## 5. Próximos passos propostos

1. **A/B tier2 ON×OFF** em 3-4 jogos da bateria (mesma cena/savestate) → responder de
   verdade "fez diferença?" com fps/VEL/p50/p95/p99 + a sensação.
2. **Achar a região do SA2** que dispara o `iNimp` (bisseção nas regiões #1-#17 do log) e
   rejeitá-la (como o 4.74 faz com `slotStore`) — mata o crash do SA2.
3. **Escolher 1-2 laços recorrentes** do relatório e reescrever em nativo (HLE por
   assinatura) — o de cópia de words (`25407501…`) é o mais espalhado e o mais simples.
