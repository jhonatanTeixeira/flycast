# Shenmue — plano para os próximos cortes

> Estado em 2026-09-19. Cena de referência: savestate do usuário
> (`/roms2/dreamcast/Shenmue (USA) (Disc 1).fc2021-rrstate.auto`), cena
> pesada de ~20 fps. Esse save **é reproduzível** entre rodadas (23,2 / 23,3 /
> 23,6 fps no mesmo binário), ao contrário do save do mslug6.

## Onde estamos

| etapa | fps | velocidade do jogo | frame na tela p50 / p95 / p99 |
|---|---|---|---|
| início do dia | 17,2 | 57% | 57,9 / 59,7 / 62,4 ms |
| + retrorun com apresentação em thread (fork) | 20,3–21,2 | 68–71% | 47 / 53 / 58 ms |
| + stores em página de código (item 4.19) | **23,3** | **78%** | 42,4 / 47,3 / 51,9 ms |

O jogo desenha a 30 fps (intervalo emulado de 33,3 ms). Meta: 33 ms por frame.

## Mapa do frame hoje

- **Thread de emulação é o gargalo:** a main thread espera por ela ~17 ms/frame
  (`rsWait`). `perf`: ~80% dessa thread é código gerado pelo JIT (AICA ~8%,
  ARM7 ~2%, TA ~1%).
- **Dentro do JIT:** o laço de transformação de vértices do jogo (`0C1EDxxx`:
  `ftrv`, `fipr`, `fdiv`, `fmov.s`) é ~48% das instruções host. É trabalho
  real, não espera — sem idle skip possível.
- **GPU:** ~35 ms/frame (`FC_GL_FINISH`), majoritariamente fill-rate
  (14,7 ms a 320x240). Já roda em paralelo graças ao retrorun em thread, mas
  vira o teto (~28 fps) assim que a emulação ficar abaixo de ~35 ms.
- **Main thread:** `Process` ~4,6 ms + envio GL ~20 ms (586 draw calls ×
  ~35 µs, custo do driver Mali JM). Tem folga hoje.

## Passo 1 — acesso à memória no JIT (estimativa +5 a 8%, vale para todo jogo)

Visto no ARM64 gerado (`FC_DUMP_BLOCK`) dos blocos quentes:

| acesso | hoje | alvo |
|---|---|---|
| store de float | `mov w0,wA` · `fmov w1,sD` · `add x7,x0,#0x1c0` · `str w1,[x28,x7]` · `nop` | `add x7,xA,#0x1c0` · `str sD,[x28,x7]` · `nop` |
| load de float | `mov w0,wA` · `add x1,x0,#0x1c0` · `ldr w0,[x28,x1]` · `nop` · `fmov sD,w0` | `add x1,xA,#0x1c0` · `ldr sD,[x28,x1]` · `nop` |
| store/load inteiro | idem com `mov` extra para `w0`/`w1` | registrador alocado direto |

~2 instruções a menos por acesso: ~15% das instruções do laço de vértices.

**O que muda e onde está o risco:** o formato do acesso rápido é reescrito pelo
tratador de fault (`ngen_Rewrite`) quando o acesso cai fora da RAM. Hoje o
endereço já está em `w0` e o dado em `w1` **antes** da vaga reescrevível; no
formato novo, a vaga de 3 instruções passa a carregar isso:
`[mov w0,wA][fmov w1,sD | mov w1,wD][bl alvo]` (store) e
`[mov w0,wA][bl ReadMemN][fmov sD,w0 | mov wD,w0]` (load). O `ngen_Rewrite`
precisa decodificar o registrador de endereço (do `add`) e o de dado (do
`ldr/str`, incluindo as formas SIMD/FP). Todos os alvos seguem esperando
`w0`/`w1`: stub de Store Queue (item 1.7), stubs de página de código (4.19),
`WriteMemN`/`ReadMemN`. Só o layout de 4 GB (o do device); o de 512 MB fica
como está.

**Validação obrigatória:** toggle por variável de ambiente para A/B no mesmo
binário; bateria dos 11 jogos (Shenmue, kofxi, MBAA, kofnw, mslug6, ggxxsla,
gwing2, sfz3ugd, Ikaruga, capsnk, meltybld 2 min) sem crash; conferência
visual do usuário. Esta é a classe de mudança que quebrou kofxi/MBAA no
item 4.19 — não pular a bateria.

## Passo 2 — flag T no fim do bloco (pequeno, baixo risco)

Blocos condicionais gravam T no contexto, copiam duas vezes, gravam de novo e
**releem da memória** para decidir o salto (`RelinkBlock`: `ldr w11,[x28,#268]`
logo após `str w23,[x28,#268]`). ~4 instruções por bloco condicional; os blocos
do laço são pequenos (10–20 instruções SH4), então o custo fixo pesa.

## Passo 3 — GPU: fill-rate (necessário quando a emulação passar de ~35 ms)

- `glInvalidateFramebuffer` para depth/stencil no fim do frame — item 1 do
  `rendering_improvement_plan.md`, nunca implementado, recomendação oficial da
  ARM para GPU tile-based (evita escrever depth/stencil de volta na memória).
- Medir antes/depois com `FC_GL_FINISH` (tempo de GPU isolado) e
  `RETRORUN_PRESENT_STATS` (cadência na tela).
- Se ainda faltar: custo de fragment dos punch-through (`discard` desliga o
  descarte antecipado de pixels na Mali) e do shader de fog/alpha.

## Passo 4 — envio GL (só se a main thread virar gargalo)

586 draw calls × ~35 µs ≈ 20 ms/frame. Hoje há folga; o batching do item 4.2
já existe. Candidato: item 6 do plano de renderização (`PP_SameGPUState`
compara 32 bits quando só 6 importam, perdendo fusões).

## Último recurso — HLE do laço de vértices

Reconhecer a rotina de transformação do Shenmue por assinatura (como o kernel
do kofxi, item 4.14) e trocar por NEON escrito à mão. Específico de um jogo e
arriscado; só se os passos gerais não bastarem. Não é "fazer na GPU": no
Dreamcast o T&L é código do jogo no SH4 e o resultado é usado na hora pela CPU
(clipping/descarte).

## Método (vale para todos os passos)

- A/B sempre no mesmo binário com variável de ambiente, mesma cena.
- Shenmue: apresentação em thread **sem** vsync para comparar (o vsync
  quantiza e esconde diferenças pequenas); a configuração de jogo real é com
  vsync (mailbox automático abaixo de 95% de velocidade).
- Reportar frame e fps em p50/p95/p99 + média, velocidade do jogo (amostras
  de áudio/s), e `rsWait` para saber se a emulação continua sendo o gargalo.
