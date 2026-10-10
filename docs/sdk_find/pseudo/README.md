# Pseudo-C++ das funções mais valiosas da varredura

Escritos em 2026-10-10 a partir dos grupos de `docs/sdk_find/descoberta.md`, lendo os
dumps do JIT de cada jogo (sem jogar de novo). Pseudo-código: não compila no core; é a
base do `hle_fn` relocável (4.125). Mesma API dos `docs/sdk_blocks/*_pseudo.cpp`.
Nenhum foi validado com `FC_STATE_HASH` ainda. Os números de grupo vêm da varredura de
2026-10-10 e podem mudar se ela for regenerada: o arquivo identifica a função pelos
endereços e bytes, não pelo número.

| Arquivo | Grupo | O que é | Jogos | Peso medido | Variantes reais |
|---|---|---|---|---|---|
| `strips_transformados_e_iluminados_pseudo.cpp` | 004 | Strips sem luz (A), strips com luz direcional/pontual (B) e triângulos com luz (C) da biblioteca dos jogos de luta | DOA2, MvC2, CvS2, Power Stone, Project Justice, Shenmue II | DOA2 6,3% (B), MvC2 2,5% (A) | Power Stone: PCW de fim de strip `0xFFFFFFFF`; CvS2: literal pools embutidos deslocados |
| `strips_com_luz_difusa_pseudo.cpp` | 005 | Modo `r8 == 0` da mesma rotina: strips com luz difusa + ambiente | DOA2, MvC2, CvS2, Power Stone, Shenmue II | **Shenmue II 6,0%**, DOA2 2,6% | Power Stone (fim de strip); Shenmue II 2ª cópia chama uma 2ª luz por vértice |
| `vertices_com_cor_argb_pseudo.cpp` | 007 | Vértices com cor ARGB empacotada (strips e triângulos), 64 bytes por vértice | MvC2, DOA2, CvS2 | **MvC2 6,1%** | nenhuma |
| `lista_de_malhas_com_culling_pseudo.cpp` | 011 | Percorre as malhas do modelo: culling por esfera, QACR, cabeçalhos do TA e chama os laços de vértice (005/033) | DOA2, MvC2, CvS2, Power Stone, Project Justice, Shenmue II | DOA2 2,1%, Shenmue II 1,6%, MvC2 1,3% | Shenmue II (desvio para cópia alternativa + 2ª luz), Power Stone (planos e TSP) |
| `idct_do_macrobloco_pseudo.cpp` | 010 | IDCT 8×8 em float do macrobloco do decodificador de vídeo (Sofdec/MPV) | Napple, EGG, Evolution 1/2, Grandia II, KOF Evolution, PSO v2 | Napple 5,4% | nenhuma (369 de 369 opcodes) |
| `cabecalho_de_poligono_pseudo.cpp` | 012 | Monta o cabeçalho de polígono do TA a partir do estado de render (provável `kmProcessVertexRenderState` da Kamui2) | 13 jogos | EGG 4,2% | nenhuma (só 2 ponteiros por jogo, lidos da RAM) |
| `espera_de_quadro_com_timeout_pseudo.cpp` | 009 | **Espera** de fim de quadro: gira lendo o TMU0 até uma interrupção zerar uma flag ou estourar o timeout | 10 jogos | EGG ~9,6% (com as folhas) | nenhuma de código (literal pool em 2 posições) |
| `servidor_de_32_slots_cronometrado_pseudo.cpp` | 008 | Servidor periódico cronometrado pelo TMU que varre 32 slots (suspeita: servidor ADX da CRI) | 8 jogos + Le Mans | Napple 5,9% (no laço ocioso) | 2 layouts (slot 0x8C × 0x1BC bytes, campo +0x50 × +0x58) |

## O que a leitura mudou

- **004, 005 e o laço já nativo do DOA2 são modos da mesma função**, escolhidos por r8 na
  entrada (+000): `r8 == 0` → luz difusa (005); `r8` ímpar → sem luz (004 A); `r8` par
  ≠ 0 → clamp por tabela (o `8C101BC4` nativo, r8 = ponteiro da tabela). O 011 é quem
  chama esses laços. O nativo relocável dessa biblioteca deve despachar por r8 no +000.
- **008 e 009 não são cópia de memória** (rótulo automático errado): são **espera**.
  O 009 lê o timer TMU0, por isso o idle fast-forward não o pula (4.43 exclui laço que lê
  hardware) → item 4.126. O 008 roda dentro do laço ocioso do Napple enquanto o jogo
  espera o próximo quadro; o peso dele é tempo de espera.
- **010 é a IDCT da Sofdec/MPV** em outra versão que a do RE CV (`fmv_plan.md`): a mesma
  em 7 jogos, então uma versão nativa vale para as FMVs de todos eles.
- **012 (Kamui2)**: ~230 ciclos espalhados por 44 blocos curtos do JIT por chamada; no
  nativo vira uma chamada só — o caso em que nativizar rende mais que o tamanho sugere.

## Correção feita na revisão

O `strips_com_luz_difusa_pseudo.cpp` saiu com a paridade de r8 trocada (dizia r8 par →
sem luz). Conferido na desmontagem do DOA2 (`8C101928: tst #1,r0; bf 8C101930`): `tst`
liga T com o bit 0 zerado e `bf` desvia com T = 0, então r8 **ímpar** → sem luz. Corrigido.
