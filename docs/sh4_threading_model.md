# Modelo de threads do SH4 nos jogos de Dreamcast

Status: **levantamento (2026-10-10)**. Nada implementado. Próximo passo é medir quanto
cada thread do jogo roda (seção 7) antes de decidir qualquer código.

Origem: pergunta do usuário ("o SH4 trabalhava com threads? dá para disparar essas
threads no ARM64 e deixar o jogo gerir as próprias threads?"). Base: varredura dos 60
dumps do JIT em `/mnt/1TB` (`dcbat/`, `dcbat_off/`) com `tools/sh4_ctx_scan.py`,
desmontagem com `tools/sh4dis.py` e amostras do `perf` (`jit_lite_report.py`).

## 1. Resumo

- O Dreamcast tem **um** SH4. Não existe sistema operacional na maioria dos jogos
  (Katana); o que há é biblioteca ligada no executável de cada jogo.
- **Existe uma biblioteca de threads de verdade**, com troca de contexto completa,
  em **10 jogos** (mesmos bytes, endereço diferente por jogo). As threads se
  **revezam** num núcleo só, nunca rodam ao mesmo tempo.
- **TR Chronicles** é jogo Windows CE: kernel do WinCE, threads próprias (e MMU, 4.121).
- Os "spinlocks `tas.b`" do `padroes_ineficiencia_analise.md` **não são espera**: são
  try-lock na entrada de funções de um módulo do SDK.
- A exclusão mútua de verdade no DC é **mascarar interrupções** pelo SR (IMASK = 15),
  numa rotina presente em 21 jogos.
- Custo da biblioteca de threads: ~0,5% da thread de emulação (Napple, EGG). O custo
  não é o ponto; o ponto (pergunta do usuário) é se dá para **paralelizar** — seção 6.

## 2. A biblioteca de threads (10 jogos)

Assinaturas (linhas `G` do dump):

| Peça | Opcodes | Significado |
|---|---|---|
| yield | `403E 002A 404E` | `ldc r0,SSR; sts PR,r0; ldc r0,SPC` |
| salvamento | `4F43 4F33 2FE6 2FE6 2FD6` | `stc.l SPC; stc.l SSR; mov.l r14,r14,r13…` |

Endereços (bloco do yield / bloco do salvamento):

| Jogo | yield | salvamento |
|---|---|---|
| Napple Tale | `8C16BB9A` | `8C16BC18` |
| Elemental Gimmick Gear | `8C077626` | `8C0776A4` |
| Grandia II | `8C0842FA` | `8C084378` |
| Resident Evil Code: Veronica | `8C1B5F26` | `8C1B5FA4` |
| Evolution 1 | `8C1B75C6` | `8C1B7644` |
| Evolution 2 | `8C184F72` | `8C184FF0` |
| Le Mans 24 Hours | `8C049C5E` | `8C049CDC` |
| Macross M3 | `8C1D2E7E` | `8C1D2EFC` |
| Phantasy Star Online v2 | `8C38629E` | `8C38631C` |
| KOF Evolution | `8C3490DE` | `8C34915C` |

**Não aparece** em DOA2, Shenmue, Shenmue II, MvC2, CvS2, Power Stone, Project
Justice, Skies of Arcadia, SA2, Soul Calibur (nos dumps que temos). Os jogos mais
lentos do projeto (DOA2, Shenmue II) não usam essa biblioteca.

### 2.1 Como funciona (Napple, `8C16BB28…8C16BDFE`)

1. **Criar thread** (`8C16BB28`): zera a pilha nova (`mov.l r2,@-r0` em laço) e grava
   no topo o quadro inicial: entrada (`r6`), argumento (`r7`), `SR` atual (`stc SR`) e
   `FPSCR` (`sts FPSCR`). Ou seja, a thread nasce como se tivesse sido interrompida
   na primeira instrução.
2. **Ceder a vez / yield** (`8C16BB88` → `8C16BB9A`): salva r0/r1, guarda o SR e
   **mascara interrupções** (`or` com a máscara, `ldc r0,SR`), põe o SR salvo em
   `SSR` e o endereço de retorno (`PR`) em `SPC` — finge uma exceção — grava o
   motivo/parâmetro e salta (`jmp @r0`) para o escalonador.
3. **Entrada pela interrupção** (`8C16BC08`): mesma porta, com `SSR`/`SPC` vindos de
   variáveis — é o caminho usado quando a troca é pedida de dentro de um tratador
   de interrupção.
4. **Salvar o contexto** (`8C16BC18`, `8C16BC62`): empilha FPUL, FPSCR, PR, MACL,
   MACH, GBR, SPC, SSR, r14…r0, os 8 registradores do banco (`stc rN_bank`) e os
   **dois bancos de float** (32 × `fmov.s`, trocando FR via FPSCR).
5. **Escolher a próxima** (`8C16BCBE…8C16BCD6`): chama código C (`jsr @r0`) que
   devolve o `r15` da próxima thread (`mov r0,r15`).
6. **Restaurar e voltar** (`8C16BCE0…8C16BD72`): desempilha tudo na ordem inversa
   e termina em `rte` — a CPU volta para o `SPC` da thread escolhida, com o `SSR`
   dela como SR.

Não confirmado: se existe **preempção por timer** (um tratador do TMU entrando pelo
caminho 3) ou se as threads só trocam quando cedem/bloqueiam. Dá para ver contando
quem chama o caminho 3 (seção 7).

## 3. `tas.b`: try-lock de um módulo do SDK (não é espera)

Mesmo padrão em ~20 jogos (DOA2 `8C136034/8C136086/8C1360E6/8C1362A0`; no Napple
fica ao lado da biblioteca de threads, `8C16B0D8/8C16B12A`):

```
mov.l @(trava),r3 ; sts.l PR,@-r15 ; mov.l @r3,r0
tas.b @r0         ; testa e marca (atômico no SH4)
bt    pegou       ; trava estava livre
...               ; ocupada: sai sem esperar (sem desvio para trás)
pegou: bsr funcao ; monta argumentos e chama o despachante (DOA2 8C1353B0,
                  ; Napple 8C16A454) com um código de comando em r5: 0x0A, 0x0B,
                  ; 0x0C, 0x0E
       mov #0,r1 ; mov.b r1,@trava   ; solta a trava
```

É a proteção de reentrada de um módulo (o módulo não foi identificado; os stubs
vizinhos `jmp @r0` por tabela, padrões 0/4, têm a forma das chamadas de sistema da
BIOS). `perf`: `tas.b` 0,0% da emu em todos os jogos medidos. O "ratio 9,8 host/SH4"
do documento de padrões é da instrução isolada, não de um laço.

## 4. Exclusão mútua de verdade: mascarar interrupções

Rotina em `8C0083F8…8C0084xx` (área de sistema; 21 jogos):

```
stc SR,r0 ; shlr2 ; shlr2 ; and #15,r0 ; mov.l r0,@r15   ; guarda o IMASK antigo
stc SR,r0 ; and máscara ; or #0xF0,r0 ; ldc r0,SR        ; IMASK = 15
jsr @função                                                ; trabalho protegido
... monta o IMASK antigo de volta ... ldc r0,SR
```

Num núcleo só isso basta: sem interrupção não há troca de thread nem tratador
mexendo no mesmo dado. É o "mutex" do DC. A biblioteca de threads usa o mesmo
recurso no yield (passo 2).

## 5. Outros achados da varredura

- `rte` em `8C00F400` (15 jogos): retorno do tratador de exceção/interrupção do SDK
  (área do VBR); Shenmue/Shenmue II têm o seu em `8C00FA20`.
- `ldc Rm_BANK` só no Shenmue (endereços `0Cxxxxxx`): código do jogo mexendo nos
  bancos de registrador — não analisado.
- TR Chronicles: `stc SPC/SSR` + `ldc SSR/SPC` + `rte` em `8C0121FA…8C01276A`
  (kernel do WinCE).
- `sleep` (`001B`): nenhum jogo usa. O SH4 nunca dorme; ociosidade é laço
  (o que o idle fast-forward pega, 4.14/4.38/4.43).

## 6. Dá para rodar as threads do jogo em núcleos do ARM64?

Ideia: o R36 tem 4 Cortex-A53 e a emulação usa um; se cada thread do jogo rodasse
num núcleo, o jogo se geriria sozinho em paralelo.

**De forma genérica, não** — quebra a semântica que o jogo assume:

1. **O jogo foi escrito para um núcleo.** As threads dessa biblioteca nunca rodam
   juntas; o código conta com isso.
2. **As seções críticas deixam de excluir.** Mascarar IMASK (seção 4) impede a troca
   num núcleo, mas não impede outro núcleo de estar rodando a outra thread agora →
   condição de corrida, corrupção, travas aleatórias. As seções estão espalhadas
   pelo código do jogo; não dá para consertar uma a uma.
3. **Hardware emulado único.** PVR/TA, Store Queue, GD-ROM, AICA, interrupções e o
   relógio de ciclos (`sh4_sched`) são compartilhados. Paralelizar exige serializar
   quase todo acesso a hardware, o que come o ganho; e é preciso decidir qual núcleo
   recebe o vblank.
4. **Modelo de memória.** O ARM64 é fracamente ordenado; o código do SH4 (um núcleo,
   em ordem) não tem barreira nenhuma. Cada acesso compartilhado precisaria de uma.
5. **Precedente.** Emuladores de consoles de um núcleo mantêm as threads do jogo numa
   thread do host (o PPSSPP faz HLE do kernel de threads do PSP, mas roda todas numa
   thread só). Mapear thread do jogo em thread do host é coisa de console que já era
   multinúcleo (Xbox 360/Xenia, PS3/RPCS3), onde o código do jogo já sincroniza de
   verdade.
6. **Alcance.** Só 10 jogos usam a biblioteca; DOA2/Shenmue II/MvC2 não.

**A versão viável: tirar trabalho isolado, por assinatura.** Se uma thread do jogo
(ou um trecho dela) faz trabalho autocontido — decodificar áudio/vídeo, descomprimir
dado lido do disco, montar geometria num buffer próprio — esse trabalho vira função
nativa rodando em outro núcleo do host, e o jogo continua vendo a thread dele
terminar no mesmo tempo emulado. É o caminho do `hle_fn` (validado bit a bit com
`FC_STATE_HASH`) e do "codec nativo em outra thread" do `fmv_plan.md`, só que em
paralelo. Caso a caso, mas sem quebrar a semântica de um núcleo.

## 7. Próximo passo: medir quanto cada thread roda

O teto do ganho é o tempo das threads que **não** são a principal. Se o jogo passa
95% numa thread, não há o que paralelizar; se passa 30% numa thread de streaming ou
decodificação, há alvo.

Diagnóstico proposto (opt-in, custo zero desligado):
- a cada `UpdateSystem`, ler na RAM do jogo o ponteiro da thread atual (a variável que
  a biblioteca grava na troca; no Napple, perto de `8C16BD84`, a confirmar) e somar os
  ciclos do intervalo naquela thread;
- registrar a entrada de cada thread (o `r6` da criação, passo 1) para dar nome a ela;
- contar trocas por caminho (yield × interrupção), o que responde a dúvida da
  preempção;
- saída: histograma de tempo emulado por thread + por função de entrada.

Rodadas: Napple (cold boot, cfg de debug) e Grandia II.

## 8. Reproduzir

```bash
# varredura geral (rte, SSR/SPC, SR, bancos, tas.b, sleep)
python3 tools/sh4_ctx_scan.py scan /mnt/1TB/dcbat_off/*/jit-*.txt /mnt/1TB/dcbat/*/jit-*.txt
# onde está a biblioteca de threads em cada jogo
python3 tools/sh4_ctx_scan.py find "403E 002A 404E" /mnt/1TB/dcbat*/*/jit-*.txt
# ler o código
python3 tools/sh4dis.py /mnt/1TB/dcbat_off/*Napple*/jit-*.txt 8C16BB00 8C16BE00
# tempo por bloco (só os dumps de dcbat_off têm samples.txt.gz)
python3 tools/jit_lite_report.py --top 100000 <dir>/jit-*.txt <dir>/samples.txt.gz
```
