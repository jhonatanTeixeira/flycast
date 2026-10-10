# Plano futuro: refatorar o backend x64 para o mesmo pé do ARM64

Status: **plano futuro (2026-10-10)** — **não está no `current_plan.md`**; não executar
sem pedido. Pré-requisitos desejáveis: etapa 0 (remoção do tier2) e etapa A (`hle_fn`
relocável) do plano atual, para portar o desenho final e não um intermediário.

## 1. Por que

- **Ciclo de validação no PC.** Hoje todo `FC_STATE_HASH`, dump do JIT e A/B depende
  do R36. Um core x64 no mesmo pé roda no desktop em segundos: validar nativo bit a bit,
  rodar baterias de hash em dezenas de jogos e testar layout de código antes de ir ao
  device.
- **Standalone** (`docs/standalone_plan.md`) e builds desktop do core.
- **Correção**, não só velocidade: várias mudanças do ARM64 corrigem comportamento
  (anti-SMC, checagem de FPU desabilitada em runtime, link dinâmico com checagem de
  bloco vivo) e o x64 não tem nenhuma.

## 2. Situação

`core/rec-x64/rec_x64.cpp` (~2.200 linhas, Xbyak) é o backend original do fork
metallic77; `core/rec-ARM64/rec_arm64.cpp` (~4.100 linhas, vixl) recebeu todo o trabalho
do projeto. Referências no código (`grep`) em 2026-10-10:

| Recurso | Camada | ARM64 | x64 |
|---|---|---|---|
| Melhorias do decoder/SHIL (`stc.l SR`, `tas.b`, `lds FPSCR` nativos; assinaturas de idle) | compartilhada (`decoder.cpp`) | sim | **sim** (mesmo SHIL) |
| Ritmo 30/60 no core, sh4clock d10, socket de controle, render (sort de translúcidos etc.) | compartilhada | sim | **sim** |
| Idle fast-forward / pulo de laço de atraso, varredura e `dt` (`idle_fastforward`, `delay_skip`, `scan_skip`, `dt_skip`) | o decoder marca, **o backend emite a chamada** | sim | **não** (as marcas são ignoradas) |
| Clock reduzido na FMV (`g_sh4CycleRefill` no `intc_sched`, 4.123) | backend | sim | não |
| Link direto do despacho dinâmico (`dyn_link_pc`, fim de bloco de 3 estados, 4.120) | backend + driver | sim | não |
| Funções nativas por assinatura (`hle_fn`: lightxf, stripemit, DOA2) | backend (gancho) + C++ com NEON | sim | não |
| `div32` nativo, stub de SQ (`pref` → TA), stubs anti-SMC de store + bitmap | backend | sim | não |
| Checagem de FPU desabilitada em runtime (4.92) | backend | sim | não |
| Acesso compacto à RAM (base + `uxtw`, 4.27) | backend | sim | equivalente próprio (vmem) |
| Diagnóstico: dump do JIT (`B/G/E/O/H`), mapa do `perf`, `FC_BLOCK_PROF`, `FC_JIT_TRACE`, `FC_IFB_COUNT` | backend | sim (`FC_*` 66 referências) | não (0) |
| `ftrv`/`fipr` | backend | `fmul` + 3 `fmla` **fundidas**; `fipr` em pares | **sem fusão** (`mulps`+`addps`; a versão com `vfmadd` está em `#if 0` "rounding problems") |

A última linha importa: os dois backends **não dão o mesmo resultado de float**, então
um hash do ARM64 não é comparável com um do x64 hoje.

## 3. Plano

### Fase 1 — fatorar o que não é do backend

Tirar do `rec_arm64.cpp` a lógica que não depende do ARM64 e deixá-la num arquivo comum
do dynarec (ex.: `core/hw/sh4/dyna/ngen_common.cpp`), com uma interface pequena que cada
backend implementa (chamar o runtime, desconto de ciclos, desvio para o despachante):
- decisão das chamadas de pulo (idle FF, atraso, varredura, `dt`) na entrada do bloco;
- busca da função nativa (`hle_fn_lookup`) e a convenção de retorno;
- o escritor do dump do JIT e do mapa do `perf` (as linhas `B/G/E/O` são comuns; o `H`
  leva os bytes do host);
- a variável de recarga de ciclos (`g_sh4CycleRefill`) e o protocolo do link dinâmico.
Assim, "mesmo pé" deixa de ser cópia mantida à mão nos dois backends.

### Fase 2 — portar, nesta ordem

1. **Correção:** stubs anti-SMC de store + bitmap; checagem de FPU desabilitada em
   runtime; link dinâmico com a checagem de bloco vivo (o bug do DOA2 na BIOS, 4.120).
2. **Esperas:** emitir as chamadas de idle FF / atraso / varredura / `dt` que o decoder
   já marca; `intc_sched` com `g_sh4CycleRefill`.
3. **Velocidade:** link direto do despacho dinâmico (fim de bloco de tamanho fixo, 3
   estados), `div32`, stub de SQ.
4. **Diagnóstico:** dump do JIT e mapa do `perf`; `tools/jit_lite_report.py`,
   `sdk_find.py` e `jit_hotset.py` passam a reconhecer x86-64 nos bytes `H` (hoje
   assumem ARM64).
5. **Nativo:** o `hle_fn` (hoje NEON) ganha versão portátil — escalar com `fmaf`
   explícito onde o JIT funde, ou SSE/FMA3 — mantendo a mesma semântica do backend em
   que roda.

### Fase 3 — semântica de float única

Definir a semântica de referência de `ftrv`/`fipr`/`fmac` (a do ARM64: `ftrv` = `fmul`
da coluna 0 + 3 multiplicações-somas fundidas; `fipr` = produtos sem fusão somados em
pares) e implementar igual no x64 com FMA3 (`vfmadd231ps`), revisitando o "rounding
problems" do `#if 0`. Com isso o mesmo savestate dá **o mesmo hash** no PC e no R36, e o
PC vira o validador oficial do nativo.

### Fase 4 — caminho quente

As fases do `docs/jit_hot_path.md` (quente/frio, versão por página, arena quente)
portadas depois de validadas no ARM64. O x64 do desktop tem cache maior; o ganho lá é
menor, mas manter o mesmo layout facilita depurar.

## 4. Validação

- Por item: `FC_STATE_HASH` idêntico **antes × depois** no próprio x64 (nada pode mudar
  o resultado além da fase 3, que muda de propósito).
- Depois da fase 3: hash **x64 = ARM64** no mesmo savestate (DOA2, Shenmue II, Napple) —
  o critério de "mesmo pé".
- Bateria de boot no desktop com os jogos do `game_status.md`.

## 5. Riscos

- Xbyak × vixl: o fim de bloco de tamanho fixo e o `Relink` precisam de codificações de
  tamanho fixo no x86 (`jmp rel32`, `nop` de preenchimento).
- ABI (System V × Windows) nos stubs e chamadas ao runtime.
- FMA3 ausente em CPUs x86 antigas: sem ela, cair na semântica sem fusão e marcar o
  hash como não comparável.
- O x64 usa o vmem/fastmem do fork de outra forma que o acesso compacto do ARM64;
  portar o comportamento (anti-SMC), não a implementação.
