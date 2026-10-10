# Código de SDK nos jogos de Dreamcast: identificar e nativizar

Status: **levantamento (2026-10-10)**, nada implementado. Item aberto: `tech_debits.md`
4.125. Irmão: `docs/sh4_threading_model.md` (biblioteca de threads, IMASK, `tas.b`).

Base: os 60 dumps do JIT em `/mnt/1TB` (`dcbat/`, `dcbat_off/`), `tools/sh4dis.py`,
`tools/sh4_ctx_scan.py` (`find` = sequência de opcodes em todos os dumps) e o `perf`
dos 7 jogos de `dcbat_off`.

## 1. Resumo

- Os jogos de DC **ligam as bibliotecas do SDK dentro do executável**. A mesma função
  aparece com os **mesmos bytes** em vários jogos, cada um num endereço. Isso é o que
  denuncia código de SDK: código do jogo não se repete entre estúdios.
- O try-lock do DOA2 (`tas.b`) envolve os **comandos do barramento Maple** (controle,
  VMU, vibração). Não é quente.
- **Achado principal:** as funções que já nativizamos **não são do Napple nem do DOA2,
  são de biblioteca**:
  - `lightxf` + `stripemit` (Napple) existem em **Evolution 1, Evolution 2, RE Code:
    Veronica e Skies of Arcadia**;
  - o laço de vértices do DOA2 existe em **MvC2 e Shenmue II** (Shenmue II é o jogo
    mais lento do projeto, ~73%).
  - Hoje o `hle_fn` só instala no **endereço fixo** do Napple/DOA2, então esses outros
    jogos não ganham nada. Tornar o `hle_fn` relocável é o próximo passo (seção 5).
  - **Comparação da função inteira** (`docs/sdk_blocks/`, só o trecho que o jogo
    executou na sessão): `lightxf`, `stripemit` e o laço do DOA2 são **idênticos** em
    todos os jogos da tabela 3.1 (ex.: `stripemit` 134 de 134 opcodes nos 5 jogos).
    Falta conferir o literal pool (constantes), que o dump não guarda.

## 2. Como identificar o SDK

Não precisamos (nem devemos usar) o SDK vazado da Sega: o objetivo é **reconhecer**
as funções e reescrever o comportamento, que é o que o `hle_fn` já faz (validado bit
a bit com `FC_STATE_HASH`). Os métodos, do mais barato ao mais caro:

1. **Repetição entre jogos (dumps).** Blocos com os mesmos opcodes em N jogos = biblioteca.
   `sh4_ctx_scan.py find "<opcodes>"` responde em minutos sobre os 60 dumps. O
   `/mnt/1TB/consolidar.py` / `cluster_blocks.py` agrupam blocos parecidos em massa
   (mas a prioridade vem do tempo amostrado, nunca do ratio estático — lição em
   `tech_debits.md`).
2. **Chamadas da BIOS.** O SDK chama a BIOS por vetores fixos na RAM baixa: `r0 =
   @(0x8C0000BC)` (GD-ROM), `0x8C0000B0` (sysinfo), `0x8C0000B4` (fonte), `0x8C0000B8`
   (flashrom), `0x8C0000E0` (misc), com `r6` = grupo e `r7` = função. Ex.: DOA2
   `8C129BB4` = `r6=0, r7=3, jmp @(0x8C0000BC)` (inicialização do sistema do GD-ROM).
   O stub `mov.l r7; mov.l r0; mov.l @r0,r0; jmp @r0` (`D702 D003 6002 402B`) está
   em **20 jogos**, inclusive o TR Chronicles (WinCE).
3. **Constantes de protocolo e registradores de hardware.** Números que só fazem sentido
   para um periférico. Ex.: códigos de comando Maple em `r5` (seção 3.2); endereços
   MMIO lidos/escritos (Maple `A05F6Cxx`, GD-ROM `A05F70xx`, PVR/TA `A05F8xxx`, AICA
   `A07xxxxx`, DMAC `FFA0xxxx`, TMU `FFD8xxxx`).
4. **Strings de versão na RAM.** As bibliotecas guardam texto de versão (a Sofdec do RE
   CV: `mwPly/mwSfd 1.30, CRI SFD 1.14, MPV 1.14, MPS 1.14, ADXT 5.58, SJ 5.50`, lidas
   pelo socket com `mem`). Fontes: socket (`tools/fc_ctrl.py`, comando `mem`) ou o
   savestate (`*.rrstate.auto` contém a RAM de 16 MB). Dá o nome e a versão do módulo.
5. **Documentação aberta do hardware.** O KallistiOS (SDK livre do DC) e a documentação
   da comunidade descrevem syscalls, Maple, GD-ROM e PVR. Servem para dar nome ao que
   a função faz, sem copiar código de ninguém.

## 3. O que já foi identificado

Listagem SH4 de cada família em cada jogo, com a comparação contra o jogo de
referência: **`docs/sdk_blocks/`** (um arquivo por família, gerado por
`tools/sdk_blocks_doc.py`). Variantes vistas: a biblioteca de threads do **Le Mans**
difere (15 de 277 opcodes iguais: outra versão ou trecho deslocado) e a do **EGG** em
parte (244 de 269); a seção crítica do PSO v2 em parte (76 de 93).

| Módulo | Evidência | Jogos | Quente? |
|---|---|---|---|
| Transformação + luz de vértices (`lightxf`) e emissor de strips (`stripemit`) — biblioteca gráfica da Sega | mesmos bytes (prefixo) | Napple, Evolution 1/2, RE CV, Skies | **sim** (nativo no Napple) |
| Laço de vértices T&L + clamp (tipo DOA2) | mesmos bytes (prefixo) | DOA2, MvC2, Shenmue II | **sim** (~45% do JIT do DOA2) |
| Decodificador Sofdec (CRI MPV 1.14) | strings + mapa (`fmv_plan.md`) | RE CV, Evolution 1, … | sim, na FMV |
| `memset16/32`, `ocbp` | padrões 1/2/5/23 | 7-20 jogos | pouco (4.119) |
| Biblioteca de threads | `sh4_threading_model.md` | 10 jogos | não (~0,5%) |
| Comandos Maple com try-lock | `tas.b` + códigos Maple | 19 jogos | não (`tas.b` 0,0%) |
| Stubs de syscall da BIOS | vetor `0x8C0000BC` etc. | 20 jogos | não |
| Seção crítica (IMASK = 15) | `8C0083F8`, área de sistema | 21 jogos | não |

### 3.1 Endereços das funções nativas em outros jogos (prefixo)

| Função | Jogo | Endereço(s) |
|---|---|---|
| `lightxf` | Napple (nativo hoje) | `8C14DDC0` (+ `8C152AEC`) |
| | Evolution 1 | `8C1C9B6C` |
| | Evolution 2 | `8C1BCF40` |
| | RE Code: Veronica | `8C1C00A0` |
| | Skies of Arcadia (disco 1) | `8C2CA500`, `8C2CE794` |
| `stripemit` | Napple (nativo hoje) | `8C14D440` |
| | Evolution 1 | `8C1C91EC` |
| | Evolution 2 | `8C1BC5C0` |
| | RE Code: Veronica | `8C1BF720` |
| | Skies of Arcadia | `8C2C9B80` |
| laço do DOA2 (`F6E9 6763 F28D F270 F79D 7640`) | DOA2 (nativo hoje) | `8C101BC4` |
| | MvC2 | `8C12AA84` (cabeças `8C12AA7C/82`, `8C12B452`) |
| | Shenmue II | `8C1D8D24`, `8C1D9E62` (duas cópias) |

### 3.2 O try-lock do DOA2 = comandos Maple

DOA2 `8C136034…8C1362FE` (mesma estrutura em 19 jogos):

```
tas.b @trava ; bt pegou ; (ocupada: sai)
pegou: bsr  funcao_N           ; monta o pedido e chama o despachante 8C1353B0
       mov.b #0,@trava         ; solta
funcao_N: ... mov #cmd,r5 ; bsr 8C1353B0
```

Os códigos em `r5` batem com os comandos do barramento Maple: `0x01` pedir informação
do dispositivo, `0x0A` informação da mídia, `0x0B` ler bloco, `0x0C` gravar bloco,
`0x0E` ajustar condição (vibração/LCD/som do VMU). O despachante indexa uma tabela de
estruturas de 44 bytes por porta (`r4 × 44`, base em `0x8C31007C`). A trava impede
dois pedidos simultâneos ao mesmo dispositivo (ex.: o jogo pedindo leitura do VMU de
dentro de uma interrupção). Não é quente e não espera.

Possível ligação com o 4.117 (memory card "falta de espaço" no Napple/Shenmue II): é
essa camada (`0x0A` = informação da mídia, de onde vem o espaço livre) que conversa
com o VMU emulado.

## 4. O que nativizar (e o que não)

**Vale:** funções de **cálculo** quentes e autocontidas — T&L de vértice, emissores de
geometria, IDCT/compensação de movimento, cópias de memória grandes. Uma versão nativa
de função de biblioteca vale para **todo jogo que usa a mesma versão**.

**Não vale:** funções de **serviço** (Maple, GD-ROM, threads, syscalls). Não são quentes;
trocar a temporização de E/S tem risco (o jogo espera respostas em ciclos certos);
quando o jogo **espera** por elas, o caminho é o idle fast-forward, não reescrever.

## 5. Como nativizar código de SDK (generalizar o `hle_fn`)

Hoje (`core/rec-ARM64/hle_fn.cpp`): cada função tem uma lista de `Span {endereço fixo,
bytes}`; o JIT, ao compilar o bloco de entrada **naquele endereço** e com os bytes
batendo, chama a versão nativa. As saídas para o JIT (`ENTER`/`BAIL`) e as constantes
lidas por `mov.l @(disp,PC)` (ex.: `lit = 0x8C14DE08`) também são endereços fixos.

Para valer em qualquer jogo com a mesma biblioteca:

1. **Casar por bytes, não por endereço.** Ao compilar um bloco, um hash dos primeiros
   opcodes escolhe candidatos; confirma com a comparação completa dos `Span`
   (deslocamentos relativos à entrada, não endereços absolutos).
2. **Base relativa.** `ENTER`/`BAIL` e as constantes PC-relativas viram `base + offset`.
   As constantes do literal pool são lidas da RAM do jogo na hora (endereço da tabela
   de luz, escala, etc. mudam por jogo).
3. **Variantes.** Versões diferentes do SDK = bytes diferentes num trecho. Cada variante
   é uma entrada; a comparação completa garante que só casa onde é idêntico.
4. **Mesma contabilidade de ciclos** e mesma ordem de float (`fp-contract=off`) — já é
   assim; não muda com a relocação.
5. **Validar por jogo:** `FC_STATE_HASH` + `FC_RTC_FIXED` + `FC_INPUT_NEUTRAL`,
   frame a frame idêntico; só então o A/B de 2 rodadas.

## 6. Próximos passos (não executados)

1. Conferir o literal pool de `lightxf`, `stripemit` e do laço do DOA2 nos jogos da
   tabela 3.1 (o código já bate; ver `docs/sdk_blocks/`), lendo a RAM pelo socket.
2. `hle_fn` relocável (seção 5), mantendo o Napple e o DOA2 bit-exatos.
3. Shenmue II e MvC2 com o laço do DOA2; Evolution 1/2, RE CV e Skies com
   `lightxf`/`stripemit`. Ordem por ganho esperado: Shenmue II (mais lento) primeiro.
4. Catálogo: para cada função nativa nova, rodar `sh4_ctx_scan.py find` com o prefixo
   para saber em quantos jogos ela vale antes de escrever.

## 7. Reproduzir

```bash
# em quantos jogos uma sequência de opcodes aparece (prefixo de uma função)
python3 tools/sh4_ctx_scan.py find "FFFB FFEB FFDB FFCB 2F86 6346 6546" /mnt/1TB/dcbat*/*/jit-*.txt
# ler a função num jogo
python3 tools/sh4dis.py /mnt/1TB/dcbat_off/*Dead*/jit-*.txt 8C135F00 8C136400
# constantes resolvidas pelo JIT (linhas O: readm de endereço fixo = literal pool)
awk '$1=="B" && $3=="8C129BB4"{p=1} p&&/^O /{print} p&&/^H /{exit}' <jit-*.txt>
```
