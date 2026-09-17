# Status por jogo — o que já roda e o que falta

> Avaliação **jogando**, feita pelo usuário no device (R36/RK3326), em
> 2026-09-16, com o core depois das otimizações desta data (caminho de paleta
> na GPU + fixes de JIT). É observação de gameplay real, não benchmark — onde
> houver número medido por benchmark, está marcado como tal.
>
> "Cauda longa" = os picos de frame time (p95/p99), que é o que se sente como
> hicup mesmo quando a média está boa. "retrorun salva" = o frameskip
> adaptativo do frontend compensa parte do problema.

## Resumo

| Jogo | FPS observado | Cauda longa | Veredito |
|---|---|---|---|
| **SFZ3UGD** | 55 | boa | **jogável**, só demora muito pra carregar |
| **ggxxsla** | 55 | boa | **jogável**, gameplay quase liso |
| **MBAA** | 45-50 | **ruim** | quase lá — a cauda impede |
| **kofnw** | 45-50 | **ruim** | quase lá — hicups |
| **kofxi** | 43 | boa | **surpresa** — não era esperado rodar |
| **Skies of Arcadia** | drops pra 24 | pontual | falta um toque |
| **Shenmue** | 18-30 | melhorou muito | neve agora bem consistente a 30 |
| **mslug6** | 30 (20 em boss) | consistente nos boss | resto do jogo full speed |

## Detalhe

### SFZ3UGD — jogável
55 fps. Savestate criado nesta sessão. O frameskip do retrorun3 mais as
otimizações dão conta. **Pendência não-performance:** o jogo demora muito para
carregar (custo de boot/descompressão, categoria própria — ver `tech_debits.md`
sobre cold-start: zip/inflate/crc + JIT frio, ~26% num profile inicial).

### ggxxsla — jogável
55 fps com cauda longa boa. O frameskip do retrorun3 deixa o gameplay quase
liso. Um dos melhores resultados.

### MBAA — quase lá, travado pela cauda
45-50 fps, mas **cauda longa ruim** com hicups. O retrorun quase salva; a cauda
impede. Tem glitches de renderização **que não foram introduzidos por nós**
(já existiam).

**O que sabemos tecnicamente e não bate direto:** no benchmark com savestate o
MBAA é o **mais estável** dos três medidos — razão `p99/p50` de **1,4x**
(kofnw 3,8x, neve do Shenmue 2,4x), distribuição quase plana de tempo de CPU.
Ou seja, **os hicups que se sentem no MBAA provavelmente não vêm da
simulação** — apontam para apresentação/áudio/pacing, que é uma frente que
nunca investigamos. É o jogo onde a diferença entre "o que o benchmark mede" e
"o que se sente" é maior, e por isso o mais informativo para essa frente.

### kofnw — quase lá, travado pela cauda
45-50 fps com cauda longa ruim causando hicups. O fix de paleta na GPU já
melhorou bastante a cauda aqui (`core_p95` -45%, `core_p99` -54% em benchmark
com savestate), mas ainda incomoda jogando.

### kofxi — surpresa positiva
Não era esperado chegar perto de rodar; hoje faz **43 fps quase constantes**.
Apresenta-se lento mas **sem muitos hicups**. Tem glitches de renderização em
algumas partes que **já existiam antes** das nossas mudanças.

### Skies of Arcadia — falta um toque
Roda bem, exceto por trechos pontuais que derrubam para **24 fps**. Não
investigado: não sabemos ainda se o gargalo desses trechos é o mesmo do
mslug6 (upload de textura) ou outro. É um candidato barato para o próximo
corte, porque o problema é localizado.

### Shenmue — melhorou muito
18-30 fps. **A cena da neve melhorou muito com o caminho de paleta na GPU**,
com bem menos drops e 30 fps bastante consistente — era a cena mais
problemática do projeto inteiro.

**Pendência de infraestrutura:** o sistema de savestate está quebrado para este
jogo, o que impede medição confiável. Já registrado em
`docs/fpscr_native_translation_plan.md`: sem savestate, o `core_p99` do mesmo
binário variou 34ms → 62ms entre duas rodadas, porque o jogo boota do zero e a
cena deriva. **Qualquer conclusão sobre a cauda do Shenmue é ruído até isso ser
resolvido.**

### mslug6 — full speed fora dos boss
**30 fps (full speed) na maior parte do jogo.** Cai para **20 fps só nas áreas
de boss com muitas partículas**, com cauda longa consistente ali. O retrorun
ajuda mas não fecha.

Foi o jogo mais medido da sessão: fps +35% e `core_p99` -57% com o fix da
paleta. O que sobra nas cenas de boss, pelo último mapa do frame:
upload de textura ~21,5ms + submissão GL (`render`) ~17,3ms.

## Para onde olhar, por prioridade

1. **Apresentação/áudio/pacing** — a frente nunca investigada, e a única
   hipótese que explica o MBAA (CPU plana, hicups sentidos). Provavelmente
   também é o que separa "45-50 fps com hicup" de "liso" no kofnw.
2. **`render` (submissão GL), ~17,3ms/frame no mslug6** — segundo maior item
   do frame e **nunca medido nessa cena**. O que existe é de 2026-09-13 no
   Shenmue, onde `glDrawElements` × contagem de draw calls dominou. Corte
   barato, hipótese já catalogada (item 6 do `rendering_improvement_plan.md`:
   `PP_SameGPUState` compara 32 bits quando só 6 importam, perdendo fusões).
3. **Savestate do Shenmue** — não é otimização, é **pré-requisito de medição**.
   Enquanto não existir, o 3D pesado não é medível de forma confiável.
4. **Skies of Arcadia** — drops localizados, ainda não diagnosticados. Barato
   de investigar justamente por serem localizados.
5. **Upload de textura (~21,5ms no mslug6)** — maior item isolado, mas as
   alavancas baratas já foram testadas e refutadas (realocação, redundância,
   formato — ver `rendering_improvement_plan.md` item 4). O que sobra é
   reduzir a quantidade de chamadas: atlas de textura, mudança arquitetural.
6. **Tempo de carregamento (SFZ3UGD)** — categoria própria, não é por-frame.
