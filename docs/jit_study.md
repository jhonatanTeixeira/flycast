# Estudo do código gerado pelo JIT do SH4 (ARM64)

2026-09-25. Base para o desenho do `jit_armv8_a` — um JIT à parte, sem
descartar os existentes (`rec-ARM64`, `rec-x64`, ...). Este documento é **só
o estudo**: não muda nada no JIT atual.

## Método

- `FC_JIT_DUMP=<dir>` (novo, `core/rec-ARM64/rec_arm64.cpp`,
  `blockmanager.cpp`, `driver.cpp`), junto com `FC_BLOCK_PROF=1`. O arquivo
  `<dir>/jit-<pid>.txt` recebe:
  - **por bloco compilado:** código SH4, cada op SHIL com o trecho ARM64 que
    ela gerou (carga de registrador / corpo / descarga), fim da entrada, fim
    das ops, início da ligação e os bytes ARM64;
  - **execuções:** de cada bloco a cada segundo emulado (`R`, na emu thread) e
    as finais dos blocos descartados (`D`, destrutor);
  - **faults reescritos:** endereço do guest de cada acesso que caiu fora da
    RAM (`W`), o que diz a região (vídeo/som/sistema);
  - **escritas em página de código** (`P`) e **limpezas do cache** (`Z`).
- `tools/jit_study.py jit-<pid>.txt flycast_libretro.so [--top N] [--block VADDR]`
  desmonta tudo de uma vez (`aarch64-linux-gnu-objdump`) e atribui cada
  instrução do host a uma categoria. `--block` mostra um bloco anotado op por op.
- **Contagem:** instruções estáticas × execuções. Bloco é linha reta, então cada
  op roda uma vez por execução. Não conta o caminho frio do agendador nem o
  contador do próprio profiler. Na saída condicional conta só um dos dois
  desvios. É um limite superior: desvios internos raros, como o caminho não-SQ
  do `pref`, contam como executados.
- **O que não conta:** C++ chamado pela emu thread (agendador, AICA/ARM7, TA,
  handlers). Instrução não é ciclo: a sessão anterior mediu IPC ~0,5 e clock
  real ~1,3 GHz na emu thread (`tech_debits.md` 4.24).
- **Jogos:** Shenmue II (save pesado), DOA2, Shenmue 1 e MBAA, cada um com o
  savestate de sempre, uma rodada (~10 s emulados).

## Resultado geral

| | Shenmue II | DOA2 | Shenmue 1 | MBAA |
|---|---|---|---|---|
| instr SH4 executadas | 1066 M | 1694 M | 1774 M | 1537 M |
| instr ARM64 por instr SH4 | 3,74 | 4,05 | 3,70 | 3,57 |
| **corpo das ops (o que o jogo pediu)** | 53% | 56% | 56% | 40% |
| **overhead do flycast** | **47%** | **44%** | **45%** | **60%** |
| carga + descarga de registrador | 27% | 27% | 26% | 28% |
| saída/ligação de bloco | 14% | 11% | 12% | 21% |
| entrada (ciclos/agendador) | 6% | 6% | 6% | 10% |
| instr SH4 por bloco executado | 8,5 | 8,9 | 9,0 | 5,6 |
| saídas condicionais / estáticas / dinâmicas | 65/18/17% | 71/21/8% | 73/16/12% | 75/14/11% |
| código quente p/ 80% da execução | **119 KB** | 34 KB | 31 KB | 39 KB |
| código quente p/ 90% | 277 KB | 78 KB | 96 KB | 84 KB |

L1I do A53: 32 KB, 2 vias.

## O que o jogo realmente faz

Classes de instrução SH4, pesadas pelas execuções.

**Shenmue II:**

| Classe | Parte | ARM64 por instr SH4 |
|---|---|---|
| FPU | 35% | 3,4 |
| ALU | 31% | 2,3 |
| load | 13% | 3,2 |
| desvio | 12% | 2,3 |
| store | 5% | 3,2 |
| sistema | 2,4% | 3,0 |
| `pref` | 1,3% | 12,4 |

**MBAA:** ALU 43%, desvio 21%, load 17%, store 8%, FPU 6%.

**Memória, por região** (execuções de `readm`/`writem`):
- **RAM principal:** 83–98% dos acessos. Custa ~2 instruções ARM64 por acesso
  (`add x13, x28, #0x1c0` recalculado a cada vez + `ldr/str [x13, wN, uxtw]`).
  O fastmem compacto já está perto do mínimo.
- **Vídeo, via fila SQ para o TA:** 10% (Shenmue II), 24% (DOA2), 16%
  (Shenmue 1), 1,5% (MBAA) dos acessos. Mais o `pref` que descarrega a fila:
  12 instruções ARM64 + chamada ao stub do TA. No DOA2 o `pref` sozinho é
  **9,3% de todas as instruções do host**.
- **Som:** o SH4 praticamente não toca no AICA. Só 4–7 pontos de acesso aos
  registradores, com execuções ~0. O som é trabalho do ARM7 e do mixer.
- **Sistema:** OCRAM, registradores P4, Holly, Maple e GD-ROM somam <0,5% dos
  acessos. Nos 2D aparecem mais pontos (Maple, GD-ROM), mas com poucas
  execuções.

## O que é inventado pelo flycast

Achados medidos, em ordem de peso.

1. **Carga e descarga de registradores: 26–28% das instruções.** O alocador
   (SSA por bloco) grava cada resultado no contexto logo depois da op
   (write-through). A descarga (672 M no Shenmue II) é maior que a carga
   (399 M). Cada bloco começa carregando do contexto o que usa. Com blocos de
   5–9 instruções SH4, quase todo registrador vai à memória e volta a cada
   poucas instruções. Descargas redundantes dentro do mesmo bloco são poucas
   (~6 M): o custo não é gravar duas vezes, é **não manter o registrador entre
   blocos**.
2. **Flag T e `jdyn` pela memória: os campos que mais trafegam** em todos os
   jogos. Por exemplo, Shenmue II: `jdyn` 126 M, `sr.T` 113 M; MBAA: `sr.T`
   366 M.
   - O `cmp`/`cset` grava o T no contexto.
   - O `jcond` copia T para `jdyn` e grava.
   - A saída **relê** `jdyn` da memória para decidir o desvio: 94–207 M de
     "recargas fora do alocador".
   - No SH4 o T é um bit; aqui vira um load/store por comparação.
3. **Saída de bloco: 11–21%.** A saída condicional são ~4 instruções (`ldr`,
   `cmp`, `b.ne`, `b`). A dinâmica são 5, lendo a tabela `FPCB` de 128 MB
   indexada pelo PC. Custo medido na seção "Tabela de despacho": ~2,5–3,5% do
   tempo, mais pela previsão do desvio indireto que pela tabela.
4. **Entrada: ~2 instruções por bloco** (`subs w27` + `b.pl`), mais a checagem
   anti-SMC nos blocos em RAM desprotegida. Pouco por bloco, mas com blocos de
   5–9 instruções SH4 vira 6–10%.
5. **`ftrv` recarrega a matriz XMTRX a cada chamada:** 13 instruções, sendo 5
   `ld1` + 1 `st1`. A conta em si são 4 (`fmul` + 3 `fmla`). O `fipr` é
   parecido (7). No DOA2, `ftrv` + `fipr` = 9% do host.
6. **`pref`: 12 instruções + chamada**, com a checagem SQ inline e o ponteiro
   `do_sqw` carregado do contexto a cada vez.
7. **Blocos pequenos:** 45% das execuções no Shenmue II são de blocos com ≤4
   instruções SH4. Todo o custo fixo acima (entrada, carga, descarga, saída) é
   por bloco. O laço de vértices do Shenmue II (`8C1D8BDA`–`8C1D8C84`) é picado
   em ~7 blocos por vértice, em parte por `bf +0`, um desvio que só pula uma
   instrução.

## Cache de código e cache de instruções

- **Cache de código (15 MB): não estoura.** 1,9–2,4 MB gerados por sessão. As 8
  limpezas são todas de boot do BIOS e carga de savestate, nenhuma durante o
  jogo. Recompilações: 30–175 endereços, 79–522 compilações extras (SMC),
  pouco.
- **Escritas em página de código:** 50–83 pontos, mas **4108 no Shenmue 1**.
  Ele escreve dados em páginas que também têm código; cada ponto passa uma vez
  pelo fault + reescrita.
- **O buffer que estoura de verdade é o L1I (32 KB):**
  - **Shenmue II** precisa de **119 KB** de código para cobrir 80% do que
    executa, e 277 KB para 90%.
  - **DOA2, Shenmue 1 e MBAA** ficam em 31–39 KB para 80%, logo acima do L1I.
  - Como o código ARM64 tem ~3,7 instruções de 4 bytes por instrução SH4 de 2
    bytes (~7×), o que cabia no I-cache de 16 KB do SH4 transborda aqui. Bate
    com o L1I medido em 4.24 (FMV do RE CV).

## Tabela de despacho (FPCB) e desvio indireto — medido

2026-09-25, Shenmue II, save pesado.

- **Volume:** `FC_JIT_DUMP` também grava a sequência de destinos das saídas
  dinâmicas (`dyn-<pid>.bin`). São 14 M saídas em 7 s emulados, ~2 M por
  segundo emulado e **~1,2 M por segundo real** a 60%: **82% `rts`**, 17%
  `jsr @Rn` e 1% `jmp @Rn`.
- **Destinos:** 4.903 distintos; 945 cobrem 90% das saídas.
- **Localidade na tabela:** 715 páginas de 4 KB tocadas, 127 cobrem 90%. Só
  com a tabela, um TLB de 512 entradas erraria 0,6% das saídas, o L1D 4,2%, o
  L2 0,1%. O device não tem huge pages: o kernel não tem THP, tudo é 4 KB.

**Custo da tabela** (`tools/fpcb_bench.c`): replay de 8 M saídas reais, com
carga dependente como `ldr`+`br`, na tabela de 128 MB contra uma tabela
compacta com os mesmos destinos (38 KB).

| Cenário | FPCB | Compacta | Diferença |
|---|---|---|---|
| Só a tabela | 14,0 ns | 8,3 ns | **5,7 ns** |
| + 16 leituras de RAM em 1 MB | 213,2 ns | 204,7 ns | 8,5 ns |
| + 16 leituras de RAM em 16 MB | 352,8 ns | 336,9 ns | 16 ns |

As 16 leituras por saída imitam os ~16 acessos à RAM do guest que o
Shenmue II faz entre duas saídas dinâmicas.

**Custo do desvio indireto** (`tools/dispatch_branch_bench.c` +
`tools/jit_dyn_offsets.py`): `ret` posto no endereço real do bloco-alvo no
cache de código, `blr` pela sequência real.

| Cenário | Custo por saída |
|---|---|
| Destino fixo | 5,4 ns |
| Destinos reais, código compacto | 15,5 ns (**~10 ns de previsão errada**) |
| Destinos reais, layout real | 18,8 ns (+3,3 ns de layout, I-TLB/L1I) |

**Conclusão:** o despacho custa ~20–30 ns a mais que o ideal por saída
dinâmica: ~6–16 ns da tabela e ~10–13 ns da previsão do desvio. A 1,2 M
saídas/s, isso é **~2,5–3,5% do tempo da emu thread**.
- **A hipótese de que a tabela seria cara não se confirma:** o layout de
  128 MB responde por ~1–2%.
- **Os microbenchmarks são limite inferior:** isolados, não dividem TLB, BTB
  e caches com o resto do JIT.
- **A parte evitável é o `rts`**, 82% das saídas: o SH4 volta sempre para
  depois do `jsr`/`bsr`. Espelhar a chamada do guest numa chamada do host
  (`bl`/`ret`) deixa o preditor de retorno do A53 acertar, e dispensa a
  tabela no caso comum.

## Ciclos: contadores do A53 na emu thread (passo 1) — medido

2026-09-25, `tools/pmu_game.sh` (perf stat -t na emu thread + perf record,
~14 s por grupo de eventos, cena do savestate).

| | Shenmue II | DOA2 |
|---|---|---|
| clock real | 1,28 GHz | 1,25 GHz |
| IPC | 0,43 | 0,44 |
| **fila vazia por falta no L1I (0xE1)** | **21,0%** dos ciclos | **19,1%** |
| **espera de load que faltou (0xE7)** | **17,6%** | **13,0%** |
| fila vazia, outros (previsão de desvio etc., 0xE0) | 8,2% | 6,0% |
| dependência de endereço (0xE5) | 5,6% | 7,4% |
| dependência geral (0xE4) | 3,9% | 3,5% |
| dependência FP/NEON (0xE6) | 3,6% | 7,3% |
| store e micro-TLB de instrução | 1,3% | 1,3% |
| sobra (trabalho útil) | ~39% | ~42% |
| L1I refill / 1000 instr | 7,6 | 5,6 |
| L2 refill / 1000 instr | 5,9 | 6,4 |
| desvios errados / 1000 instr | 13,1 (indiretos 2,0) | 8,4 (indiretos 1,4) |

**Onde o tempo vai** (amostragem de ciclos, Shenmue II):
- **JIT (`SH4_TCB`):** 65,7%.
- **Descompressão do CHD na emu thread:** 14,8% (`LzmaDec_DecodeReal2`
  9,7%, `ecc_compute_bytes` 4,2%, `crc16` 0,9%). **Achado fora do JIT:** o
  Shenmue II lê do disco sem parar nessa cena.
- **Som:** AICA ~8%, ARM7 ~3%.
- **DOA2:** JIT 69,5%, CHD ~1%.

**Faltas no L1I:** 83% (Shenmue II) e 67% (DOA2) das paradas por falta no
L1I são no código do JIT.

**Espera de load:** 72% e 49% delas.

**No tempo do JIT, Shenmue II:**

| Parte do tempo do JIT | Parte |
|---|---|
| esperando instrução (L1I) | ~26% |
| esperando dado | ~19% |
| executando (inclui dependências e desvios errados) | ~55% |

**Consequência:** cortar as instruções de overhead ganha sobre os ~55%. A
densidade do código e agrupar o código quente atacam os ~26% de L1I. As
esperas de dado são do jogo (RAM emulada) e só mudam com layout ou prefetch.

## Validação por comparação exata (passo 5) — funciona

- **Ferramentas:**
  - `FC_STATE_HASH=<arq>`: hash dos dados do TA por pedido de render e, a cada
    `FC_STATE_HASH_EVERY` pedidos, de RAM, VRAM, RAM de som e contexto do SH4;
    `L` marca a carga do savestate;
  - `FC_AUDIO_DUMP` recomeça na carga;
  - `FC_RTC_FIXED=<s>`: o RTC não entra no savestate e começava na hora do
    host, a única fonte de variação encontrada;
  - `FC_INPUT_NEUTRAL=1`: controle parado;
  - `tools/state_compare.py`: alinha pelo ciclo emulado e sai com código 1 se
    houver diferença.
- **Resultado:** com RTC fixo, duas rodadas do mesmo savestate saem
  **idênticas** em Shenmue II, DOA2 e MBAA (TA de todo frame, RAM, VRAM,
  RAM de som, contexto e PCM), com render em thread e sem.
- **Diferenças inofensivas ignoradas:** um pedido de render vazio a mais numa
  das rodadas do DOA2, e o primeiro frame depois da carga no modo sem thread
  (leva sobra do boot).
- **Sensibilidade conferida:**
  - `FC_NO_ZX_LOAD=1` (otimização neutra) → IDÊNTICOS;
  - `FC_IDLE_FF=0` (muda o tempo emulado) → DIFERENTES.
- **Consequência:** o `jit_armv8_a` pode ser validado automaticamente contra o
  JIT atual, jogo por jogo.

## Desvios e chamadas (passo 4) — medido

`FC_JIT_DUMP` agora também conta, por bloco, as saídas condicionais (tomado
ou caiu no próximo, com destino e próximo) e mantém uma pilha-sombra de
chamada e retorno (`bsr`/`jsr` empilham o `NextBlock`, `rts` confere).
Análise: `tools/jit_branch_study.py`.

| | Shenmue II | DOA2 | Shenmue 1 | MBAA |
|---|---|---|---|---|
| **`rts` que volta ao endereço empilhado** | **99,97%** | 99,70% | 98,75% | 99,80% |
| desvio curto p/ frente (pula ≤4 instr) | 33% | 27% | 44% | 19% |
| laço (volta ao início do bloco) | 17% | 11% | 28% | 48% |
| um lado ≥90% das vezes | 58% | 35% | 67% | 86% |
| um lado ≥99% | 34% | 13% | 32% | 52% |

**O que isso diz para o desenho:**
- **Retorno como `ret` do host:** acertaria quase sempre. No Shenmue 1 a
  pilha chega a encher, sinal de chamadas que não retornam (troca de tarefa):
  é preciso um caminho de volta para o despacho normal quando o endereço não
  bate.
- **Execução condicional:** absorve 19–44% das saídas condicionais.
- **Laços compilados como laço:** 11–48%.
- **Seguir o lado quente:** funciona bem em MBAA e Shenmue, menos no DOA2.

## Protótipo (passo 2) — medido

`tools/proto_jit_armv8_a/`: o laço de vértices do Shenmue II (11 blocos,
`8C1D8BDA`–`8C1D8C82`, ~20% do trabalho do JIT nesse jogo) sobre estado real
capturado do jogo (`FC_CAPTURE_BLOCK`).
- **Versão atual:** o código ARM64 que o JIT de hoje gerou, com as saídas como
  ficam depois de ligadas.
- **Versão `jit_armv8_a`, escrita à mão:**
  - registradores do SH4 fixos (r0–r6, r14 em w19–w26; fr em s16–s31;
    XMTRX em v4–v7);
  - T em registrador;
  - os `bf +0` e o `bt.s` que pula o prefixo como `csel`/`fcsel`;
  - a volta inteira num bloco com duas saídas e uma checagem de ciclos.
- **Equivalência:** registradores, T, FPUL, memória da SQ e dados enviados ao
  TA **idênticos** nos dois estados capturados.

| Estado | Atual | `jit_armv8_a` | Ganho |
|---|---|---|---|
| strip de 14 vértices | 3036 ns | 1781 ns | **1,70×** |
| strip de 2 vértices | 322 ns | 220 ns | 1,46× |
| tamanho do código do laço | 2740 bytes | 724 bytes | **3,8× menor** |

**Limites:**
- **Cache quente:** não mede o ganho de L1I, que no jogo é ~26% do tempo do
  JIT e o código 3,8× menor ataca diretamente.
- **SQ:** no emulador, o código atual paga trampolins nas escritas da SQ.
- **Estado por entrada:** a versão nova carrega e descarrega o estado a cada
  entrada; num JIT real isso sai do caminho quente.

Os três pontos favorecem o `jit_armv8_a` no jogo real.

## Onde o contexto precisa estar em memória (passo 3)

Ver `docs/jit_armv8_a_context_audit.md`. Pontos principais:
- **Sem MMU, os caminhos de memória não leem o banco SH4.** Sem flush; o
  custo é preservar os registradores fixos pelo ABI.
- **Flush e reload completos:** ifb, HLE (padrão no device), interrupção,
  exceção, `sync_sr`/`sync_fpscr` e compilação.
- **MMU:** passar para o JIT antigo.
- **ABI:** só x19–x28 e a metade baixa de v8–v15 são preservados; o banco
  inteiro não cabe.

## Implicações para o `jit_armv8_a`

Insumos de desenho, não decisões:

- **Arquivo de registradores do SH4 fixo em registradores do host.**
  - **Inteiros:** o ARM64 tem 31 registradores inteiros, e o SH4 usa
    r0–r15 + T + pr + gbr + macl/mach + fpul (~22).
  - **Ponto flutuante:** fr0–15 cabem em 16 dos 32 registradores NEON, e o
    XMTRX em mais 4 como vetores 4S.
  - **Efeito:** elimina a maior parte dos 26–28% de carga/descarga, e o `ftrv`
    cai de 13 para 4 instruções.
  - **Contexto em memória:** só em chamada C++, exceção e savestate.
- **T num registrador (ou nos flags NZCV) e saída condicional direto nele:**
  some o tráfego de `sr.T`/`jdyn`, o maior de todos.
- **Blocos maiores:** traços ou superblocos que atravessam desvios
  condicionais quentes, e execução condicional para `bf/bt +0`. Isso corta a
  entrada e a saída fixas por bloco, que somam 17–31%.
- **Densidade de código para o L1I:** menos bytes por instrução SH4, e código
  quente agrupado (blocos ligados em sequência na memória). O Shenmue II é o
  caso crítico.
- **`pref`/SQ inline para o stub do TA, com o ponteiro em registrador fixo:**
  12 instruções → ~5.
- **Retorno previsível:** `jsr`/`bsr` do guest como chamada do host e `rts`
  como `ret` com checagem do endereço, ou uma pilha de retorno própria. Tira
  ~2–3% do despacho e a maior parte das leituras da FPCB. A tabela em si
  pode ficar.
- **Continua valendo:** fastmem compacto (~2 instruções por acesso à RAM) e os
  truques de espera (avanço até o evento).

## Como reproduzir

```
# no device (core com FC_JIT_DUMP)
mkdir -p ~/jitdump
~/q_run.sh j_jogo /roms2/<sistema> "<rom>" "FC_JIT_DUMP=/home/ark/jitdump FC_BLOCK_PROF=1"
# no PC
python3 tools/jit_study.py jit-<pid>.txt flycast_libretro.so --top 30
python3 tools/jit_study.py jit-<pid>.txt flycast_libretro.so --block 8C1D8C04
```

## Regiões quentes para o nível 2 (2026-09-24)

`tools/region_study.py jit-<pid>.txt [--min-edge P] [--max-sh4 N] [--show N]`
monta o grafo de blocos do dump (arestas condicionais pelos contadores `C`,
`bra`/`bsr`/queda decodificados do `G`; `jmp`/`jsr`/`rts` sem aresta) e junta
os blocos quentes (90% do custo = execuções × instr ARM do JIT antigo) por
arestas dominantes, com teto de tamanho. Resultado estável variando
`--min-edge` 0,1-0,5 e `--max-sh4` 256-1024.

| | Shenmue II | DOA2 |
|---|---|---|
| blocos quentes (90% do custo) | 1524 | 372 |
| regiões que cobrem 50% / 80% / 90% | 18 / 183 / 672 | 1 / 21 / 133 |
| transições de bloco quente que ficam dentro da região | 96% | 97% |
| fim de bloco `jsr`+`rts` (sem aresta estática) | 26,6% | 10,6% |

**A maior região dos dois jogos é o laço de vértices** (transformação,
iluminação, 1/w, clamp, tabela de cor, 32 bytes na Store Queue, `pref` pro
TA), feito de blocos de 3-30 instruções SH4 separados por desvios curtos
para frente:
- **DOA2:** laço interno de 10 blocos (8C101BC4-8C101C4E), 10,6 M voltas,
  **~45% do custo do JIT** (a região com o laço externo, 52%); ~55 instr SH4
  e ~1,5 KB (~390 instr) de ARM do JIT antigo por volta.
- **Shenmue II:** 11 blocos 8C1D8BDA-8C1D8C5A (os do protótipo, item 4.50),
  ~18% do custo (região 24%), 2 vértices por volta.
- A 2ª do Shenmue II (8,2%, 8C0D8EA0) é uma função curta chamada ~1 M vezes,
  com `jsr` dentro: alvo de embutir, não de laço.

Primeiro alvo do nível 2: o laço interno do DOA2 (maior fatia de um jogo numa
região só), com o do Shenmue II já escrito à mão em `tools/proto_jit_armv8_a/`.

