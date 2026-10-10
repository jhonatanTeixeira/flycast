# Plano: FMVs (vídeo Sofdec) leves no Dreamcast

Status: **mitigado em 2026-10-09** (clock reduzido durante o vídeo levou o Evolution 1 a 100% e o RE CV a ~96%, ver `tech_debits.md` Lições, 4.123); o codec nativo continua opção se algum jogo não chegar a 100%. Antes:  **planejado** (o tier2 foi aposentado em 2026-10-07 e não ajudava nas FMVs: ~8% da faixa do decodificador; o caminho é o codec nativo por assinatura).

## 1. Medição que motivou (2026-10-02)

Abertura do Resident Evil: Code Veronica, captura `rr_capture.sh` (perf 299 Hz +
dumper leve), janela de 22 s do vídeo:

| Onde a thread de emulação gasta o tempo | % |
|---|---|
| `8C209000–8C20C000` (provável decodificador MPEG da Sofdec) | 40,5 |
| `8C1A6000–8C1A9FFF` (provável bitstream / ADX) | ~13,5 |
| `8C1CF/8C1D2/8C1D3` | ~8,4 |

- Thread de emulação a **98% de um núcleo** durante o vídeo. Na tela de título
  (fundo em vídeo), a mesma faixa ainda come 24%.
- Usuário: vídeo a ~45 fps no contador, poucos hiccups, **som desafinado** em
  alguns momentos (o controle de taxa do retrorun estica o áudio quando a
  emulação não acompanha).
- As regiões do tier2 dentro da faixa somam só ~8% (#10, #11, #13, #16, #17, #18).
  A maior parte roda em blocos tier1 (bloco mais caro `8C20A5B2`, 4,75%, 62
  instruções).

## 2. Por que não é "só tocar o vídeo"

O emulador não toca arquivos: quem lê o `.SFD` do GD-ROM e decodifica é o
próprio jogo, com a biblioteca **Sofdec da CRI** rodando no SH4. Para usar um
codec nativo é preciso interceptar a Sofdec. Como é a mesma biblioteca em dezenas
de jogos de DC, uma interceptação por assinatura (versão da biblioteca) vale para
todos que usam a mesma versão.

## 3. Arquitetura escolhida: codec nativo + entender o player

Decidido com o usuário: usar um codec nativo e entender a estrutura do player da
Sofdec (não só trocar funções quentes, nem só tocar por fora com o jogo
congelado).

1. **Ler o player (análise estática).** Dump completo do JIT durante o vídeo
   (`FC_JIT_DUMP` + `tools/sh4dis.py`): identificar a API do player (início,
   pedido de quadro, fim, estado), o demux do `.SFD` (MPEG-PS com ADX em
   private stream), o VLC/IDCT/compensação de movimento e onde os quadros saem
   (conversor YUV do PVR, `YUV_Block8x8`).
2. **Assinatura da versão da Sofdec** pelos bytes SH4 das funções da API, como no
   `hle_fn.cpp`. Uma tabela por versão.
3. **Decodificador nativo** em outra thread: vídeo MPEG-1 com **pl_mpeg** (MIT,
   um header), áudio **ADX** com decodificador nosso (formato simples).
   Alternativa ao pl_mpeg: libmpeg2 (GPL v2+).
4. **Entregar ao jogo** os quadros e o estado onde o player espera, para que o
   jogo siga o fluxo dele (fim do vídeo, pulo com Start, vídeo de fundo com
   coisas desenhadas por cima).
5. **Validar** jogo a jogo: imagem, som, sincronia, fim do vídeo, pulo.

**Licença:** o flycast é GPL v2. Serve MIT, BSD, LGPL 2.1+, GPL v2+. Não servem
Apache 2.0 nem (L)GPL v3. pl_mpeg (MIT) e um ADX próprio estão ok.

**Jogos de teste:**
- **Evolution 1**: FMV depois dos logos (cold boot, sem savestate). 2026-10-09: emu
  98-101%, 75% nos blocos `8C213F82..8C214ED0`, ~78% de velocidade, o jogo
  redesenha o quadro em todo vblank (4.123).
- **Resident Evil: Code Veronica**: FMV logo depois dos logos; título com fundo
  em vídeo.
- **Elemental Gimmick Gear (EGG)**: FMV logo no início.
- **Grandia II**: FMV depois do PRESS START. Pode ser dirigido pelo socket
  (`docs/ctrl_socket.md`, skill `controlar-jogo`).

**Plano B (barato, se o A emperrar):** detectar a abertura do `.SFD` (LBA →
nome pelo sistema de arquivos do CHD), congelar o jogo, tocar o arquivo por
fora, despausar e apertar Start. Só serve para vídeos de tela cheia que deixam
pular.

## 4. Antes: FMVs pelo tier2 (reconhecer o codec e rodar nativo)

Pedido do usuário: tentar primeiro pelo tier2. Codec não tem o laço que o tier2
procura; a ideia é o tier2 **reconhecer as funções do codec** (assinatura nos
bytes SH4, como o `hle_fn.cpp` do Napple) e rodar uma versão nativa exata. Serve
para todo jogo com a mesma versão da Sofdec.

**Medição base (RE CV, 20 s em cima do vídeo, `fmv_ab.sh`):**

| Rodada | VEL% | fps | ft méd | p50 | p95 | p99 | core | underruns |
|---|---|---|---|---|---|---|---|---|
| tier2 ligado | 75,8 | 45,4 | 22,0 | 21,3 | 26,0 | 27,7 | 21,2 | 16 |
| tier2 desligado | 79,3 | 47,5 | 21,0 | 21,2 | 23,9 | 25,4 | 20,2 | 14 |

O tier2 de hoje não ajuda no vídeo.

**Versões na RAM do RE CV:** mwPly/mwSfd 1.30, CRI SFD 1.14, MPV 1.14 (vídeo),
MPS 1.14 (demux), ADXT 5.58, SJ 5.50 (junho de 1999).

**Mapa (RAM lida pelo socket, `mem`, e desmontada com `tools/sh4raw.py`):**
- **IDCT do macrobloco** `8C20A374…8C20AA28`: ~25% da thread de emulação no
  vídeo. Ponto flutuante com `ftrv` (matriz da IDCT no XMTRX), 6 blocos por
  macrobloco (`mov #6,r7`), pula bloco não codificado pelo CBP (`shll r6; bf`),
  borboleta, arredondamento, `ftrc` e grava `short` menos o bias. O laço
  `8C20A5B2` (8 voltas) sai pelo despachante a cada volta no tier1, com os
  registradores de float indo e voltando do contexto a cada instrução.
- VLC `8C209A52`/`8C209EC4`: ~5%. Compensação de movimento
  `8C20AE98`, `8C20B0CE…8C20B254`: ~6%.

**Outra versão da IDCT (2026-10-10, `docs/sdk_find/pseudo/idct_do_macrobloco_pseudo.cpp`):**
a IDCT float do macrobloco em Napple, EGG, Evolution 1/2, Grandia II, KOF Evolution e PSO
v2 é a mesma função (369 de 369 opcodes; Napple `8C1C1658`, Evolution 1 `8C213F34`),
diferente da do RE CV acima. Uma versão nativa dela vale para as FMVs dos 7 jogos.

**Próximo:** IDCT do macrobloco nativa e exata (mesma ordem de operações de
float do JIT: `ftrv` = `fmul` + 3 `fmla`), por assinatura da MPV 1.14,
validada com `FC_STATE_HASH`. Depois compensação de movimento e VLC.
