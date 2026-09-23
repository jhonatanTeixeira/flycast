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
| **MBAA** | **60** (bench 2026-09-19) | a reavaliar | era câmera lenta (83%); agora 100% de velocidade — reavaliar jogando |
| **kofnw** | 45-50 → bench 57 | **ruim** | quase lá — hicups; velocidade 94%→99% em 2026-09-19 |
| **kofxi** | **60 constante** (visto jogando, 2026-09-19) | boa | era câmera lenta (83%); idle skip + fila de render + retrorun vsync/thread |
| **Skies of Arcadia** | drops pra 24 | pontual | falta um toque |
| **Shenmue** | 18-30; cena pesada 17 → **23-25** | melhorou muito | retrorun thread (+24%) + stores em página de código (+16%, opt-in) |
| **mslug6** | 30 (20 em boss) | consistente nos boss | resto do jogo full speed |
| **Dead or Alive 2** | 25-30 (bench 23) | — | diagnosticado 2026-09-22: **emu-bound (SH4 saturado, 74%)**, render 2º gargalo; 60fps nativo |
| **Giga Wing 2** (`gwing2`) | — | **não é cauda** | **jogável** (antes não era); percebe-se o frameskip |
| **samsptk** | **~59** (medido 2026-09-22) | boa (p99/p50=1,29x) | **full speed**; era 20fps por conversão de paleta na CPU — corrigido (GPU bilinear), aguarda validação visual |

## Detalhe

### SFZ3UGD — jogável
55 fps. Savestate criado nesta sessão. O frameskip do retrorun3 mais as
otimizações dão conta. **Pendência não-performance:** o jogo demora muito para
carregar (custo de boot/descompressão, categoria própria — ver `tech_debits.md`
sobre cold-start: zip/inflate/crc + JIT frio, ~26% num profile inicial).

### ggxxsla — jogável
55 fps com cauda longa boa. O frameskip do retrorun3 deixa o gameplay quase
liso. Um dos melhores resultados.

### MBAA — 60 fps no benchmark desde 2026-09-19 (reavaliar jogando)
**Atualização:** o MBAA **rodava em câmera lenta** — 83% da velocidade real,
medido pelas amostras de áudio por segundo, com 202 underruns de áudio em
60s. O jogo usa o mesmo kernel de tarefas cooperativo do kofxi e ganhou o
mesmo idle fast-forward (`tech_debits.md` 4.14/4.16): **59,9 fps, 100% de
velocidade, 6 underruns**. A hipótese abaixo (hicups fora da simulação) estava
certa sobre a CPU plana, mas o que se sentia provavelmente era a lentidão
constante + estalos de áudio. Precisa de nova avaliação jogando.

#### Avaliação anterior (2026-09-16)
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

**Resolvido em 2026-09-19:** 59,2 fps e 100% de velocidade (antes 83% —
rodava em câmera lenta, o "apresenta-se lento"). Ver `tech_debits.md` 4.14.

**Medido em 2026-09-18 (savestate, 30s):** 49,7 fps, frame p50/p95/p99 =
19,9/22,0/25,8ms — cauda curta, bate com o "sem hicup". **Limitado por
throughput da emulação da CPU, não por textura nem GPU:** 75% do trabalho do
JIT é o sistema de tarefas do próprio jogo trocando de contexto em vazio
enquanto espera o vblank (`tech_debits.md` 4.14). É o candidato mais claro do
projeto a um *idle skip*, e o fork já tem o mecanismo pra isso.

### Skies of Arcadia — falta um toque
Roda bem, exceto por trechos pontuais que derrubam para **24 fps**. Não
investigado: não sabemos ainda se o gargalo desses trechos é o mesmo do
mslug6 (upload de textura) ou outro. É um candidato barato para o próximo
corte, porque o problema é localizado.

### Shenmue — melhorou muito
**Medido em 2026-09-19 (savestate novo do usuário, cena de ~20fps):** 17,2
fps, jogo a 57,7% de velocidade. **Não é CPU emulada** (ela sobra). É render:
GPU ~35ms/frame (majoritariamente fill-rate) + envio GL ~20ms (586 draw
calls), rodando **em série** porque o present do frontend espera a GPU
terminar cada frame. Ver `tech_debits.md` 4.18. A pendência de savestate
abaixo está resolvida para esta cena.

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

### Giga Wing 2 — jogável, mas dá pra perceber o frameskip
Romset MAME: **`gwing2`** (confirmado no device em 2026-09-19; tem savestate).

**Antes não era jogável; hoje é.** Mas não está liso, e o sintoma é de um
tipo diferente do resto da lista: **não são hicups** (picos isolados de frame
time). O que se percebe é o **frameskip do retrorun3 atuando de forma
constante** — ou seja, o emulador está consistentemente abaixo do alvo e o
frontend descarta frames com regularidade para manter o ritmo.

**Por que essa distinção importa:** separa a lista em dois problemas
diferentes, que pedem soluções diferentes:

- **Limitado por cauda** (MBAA, kofnw): a média é boa, os picos é que
  estragam. Atacar p95/p99 — e no caso do MBAA a evidência aponta para fora
  da simulação (ver acima).
- **Limitado por throughput** (Giga Wing 2, mslug6 nas áreas de boss): o
  frame inteiro é caro de forma consistente, não há pico a remover. Só
  melhora tornando o trabalho por frame mais barato.

Giga Wing 2 é um shmup com muita coisa na tela (bullet hell), o que é
consistente com o perfil do mslug6 nos boss: muitas partículas/sprites ⇒
muita textura e muito draw call. **Ainda não medido** — vale rodar com
savestate e o `FC_TA_SPLIT`/`FC_REND_SPLIT` para ver se o mapa do frame bate
com o do mslug6 (upload de textura + submissão GL) ou se é outra coisa.

### Dead or Alive 2 — diagnosticado: emu-bound (SH4), não render-bound
**Medido 2026-09-22** (savestate, `--benchmark 20`): 23,1 fps, jogo a **73,9%**
de velocidade, 186 underruns. **É 60fps nativo** (confirmado:
`req_native_fps=59,92`, sem RTT). **Gargalo primário: throughput de emulação
SH4** — a emu thread está **saturada em ~1 core** (103%, `SH4_TCB` 39% self +
`ta_vtx_data32` + ARM7), produzindo ~40 frames reais/s onde o jogo pede 60. O
render (640 draw calls, 22,5ms, CPU-bound não-GPU) é o **segundo** gargalo:
descarta os frames que não cabem, então a tela mostra ~21fps. Remover o render
não levaria a 60fps (teto de 74% é do SH4, como Shenmue em cena pesada).
vsync/threaded present não mudam (4 combos, todas ~74%).

**Armadilha de medição (usuário viu na tela):** `FC_AUTOSKIP=0` sobe os frames
apresentados de 23,1 para 32,6 fps mas a **velocidade do jogo CAI de 74% para
54,5%** (áudio) — "passa de 30fps mas fica mais lento". A métrica de fps do
frontend conta frames apresentados, não velocidade de jogo. Ver
`docs/history.md` 2026-09-22 e item 4.20.

25-30 fps no nosso branch. **Só roda bem via RetroArch com o fork
`flycast_extreme`** — não se sabe o que esse fork tem que faz diferença aqui.
(A versão no device é um build **32-bit ARM** de 2020, que não roda no
`retrorun3` 64-bit — comparar exige RetroArch32.)

**Por que este caso é especialmente valioso:** é o único jogo da lista onde
existe uma implementação de referência que comprovadamente roda melhor no
MESMO hardware. Isso permite comparação de código dirigida, em vez de
investigação às cegas — exatamente a ferramenta que o `CLAUDE.md` já autoriza
("comparação de código entre os dois é uma ferramenta de investigação
legítima") e que já valeu duas vezes nesta sessão: a pesquisa no
`flyinghead/flycast` master e no `redream` guiou o trabalho de FPSCR, e olhar
o upstream mostrou que ele **não** resolveu a invalidação de textura de forma
diferente — o que economizou construir a solução errada.

**Antes de investigar, checar:** (a) se `flycast_extreme` está no device como
core (dá para medir A/B direto, mesmo savestate, como já fizemos com o
`flycast2021_libretro.so` original); (b) se é fork público com código
disponível. Sem uma das duas, vira adivinhação.

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
4b. **Giga Wing 2** — limitado por throughput, não por cauda. Medir primeiro
   (savestate + `FC_TA_SPLIT`/`FC_REND_SPLIT`) para ver se o mapa do frame é
   o mesmo do mslug6; se for, os dois andam juntos com o mesmo trabalho.
5. **Upload de textura (~21,5ms no mslug6)** — maior item isolado, mas as
   alavancas baratas já foram testadas e refutadas (realocação, redundância,
   formato — ver `rendering_improvement_plan.md` item 4). O que sobra é
   reduzir a quantidade de chamadas: atlas de textura, mudança arquitetural.
6. **Tempo de carregamento (SFZ3UGD)** — categoria própria, não é por-frame.

### Fora da ordem: comparar com `flycast_extreme` (Dead or Alive 2)
Não entra na lista por prioridade porque não é uma frente técnica, é um
**atalho de método**: quando existe outra implementação rodando melhor no
mesmo hardware, ler o que ela faz diferente costuma custar menos que
descobrir do zero. Vale disparar assim que der para confirmar se o core está
no device e/ou se o código é público.

### samsptk (Samurai Shodown 6 / Samurai Spirits 6) — full speed após fix
**Medido 2026-09-22.** Baseline: `core_average` 47,5ms, ~20,5 fps, cauda
**curta** (`core_p95` 53,5 / `core_p99` 59,2ms). **Não era JIT** — `perf`
mostrava conversão de paleta 4bpp na CPU (`convPAL4_TW`, ~19%) como maior bloco
isolado; 84% das conversões eram de paletizadas **bilineares**, fora do caminho
de paleta na GPU. São **3 texturas de 512×512 reconvertidas quase todo frame**
(+2 de 1024×1024), ~61% do frame.

**Corrigido:** portado `palettePixelBilinear` (`pp_Palette == 2`) do master +
`IsGpuHandledPaletted` aceitar `FilterMode <= 1` em GLES3. Resultado A/B no mesmo
binário: **20,0 → 59,4 fps**, `core_average` 48,9 → 12,1ms, underruns de áudio
195 → 2, `dq_filter_updates` 3.070 → 0. Neutro nos outros 8 jogos testados;
bateria de 10 jogos sem crash. Escape hatch: `FC_NO_GPU_PAL_BILINEAR=1`.
**Aguarda validação visual do usuário** (mudança de pipeline de textura tem
histórico de glitch neste projeto).

### Evolution - The World of Sacred Device — não inicia (CHD em zstd)
**Diagnosticado 2026-09-22.** Não é crash do emulador: o CHD foi gerado por
um `chdman` recente com codec **`cdzs` (CD + Zstandard)** no header
(`cdzs cdzl cdfl`); todos os outros CHDs de DC do device usam `cdlz cdzl cdfl`.
O `libchdr` deste fork (`core/deps/libchdr`) só conhece zlib/lzma/flac, então
o disco abre como `NoDisk` e `nullDC.cpp` (~linha 453) desliga o HLE e tenta a
BIOS real — daí a mensagem enganosa *"Unable to find bios in /roms2/bios/dc/"*.
(Este fork não lê `dc.zip`; só `dc_boot.bin`/`dc_bios.bin`. Todo jogo de DC no
device roda com HLE BIOS, que é o default.) Evolution 2 usa `cdlz` e não é
afetado. Correções possíveis: recomprimir o CHD (`chdman`) ou portar o codec
zstd pro `libchdr`.

### KOF Evolution (DC) — 60 fps fora da chuva; ~54 fps na chuva desde 2026-09-23 (era 30)
**Medido 2026-09-22** (savestate da chuva): jogo a **100% de velocidade**, mas
a tela mostra **30,4 fps** (core p50/p95/p99 31,6/34,6/41,7ms). O jogo é 60fps;
o render passa um pouco de 16,7ms e cai para metade. Ver `tech_debits.md` 4.21.

### MBAA — glitches (2026-09-22)
Fixos no savestate slot 2: blocos no retrato e lixo tipo texto no fundo.
Intermitente: retrato some e fica só o contorno (em algumas rodadas).
Ver `tech_debits.md` 4.23.

**Atualização 2026-09-23:** com `glDrawRangeElements` a chuva vai a **~54 fps**
(render 18,7 → 13,9ms), com o jogo a ~92% de velocidade nessa cena (a fila
agora espera o render; ver `tech_debits.md` 4.29). Reavaliar jogando.

### Soulcalibur — congela no boot pelo ES (2026-09-23)
Com o flycast2026 lançado pelo ES (boot do zero). Pelo savestate roda. Ver
`tech_debits.md` 4.30.
