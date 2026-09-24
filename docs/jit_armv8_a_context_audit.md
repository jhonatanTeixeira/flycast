# `jit_armv8_a` — onde o contexto do SH4 precisa estar em memória

2026-09-25. Passo 3 do plano (`docs/jit_study.md`).

- **O que é:** auditoria só de leitura de todo código C++ que lê ou escreve o
  `Sh4Context` enquanto o código gerado roda. É o que o JIT novo precisa
  respeitar se mantiver o arquivo de registradores do SH4 fixo em
  registradores do host.
- **Referências:** `arquivo:linha` neste fork.
- **Especulação:** o que é inferência está marcado **[especulação]**.

## 0. Comportamento atual que define o contrato

- **Registradores fixos do JIT atual** (`rec_arm64.cpp`):
  - x28 = `&Sh4cntx`;
  - w29 = next_pc em trânsito;
  - w27 = contador de ciclos;
  - x13 = base do acesso compacto.
- **Alocação dentro do bloco:** GPRs em w19–w26, floats em s8–s31
  (`arm64_regalloc.h`). O `FlushAllRegs` roda no fim do bloco
  (`ssa_regalloc.h`, `OpEnd`).
- **Caminho lento de memória:** sem MMU, `readm`/`writem`/`pref` **não
  descarregam** registradores antes do C++ (`ssa_regalloc.h:60,404`, só com
  `mmu_enabled()`). O código de hoje já assume que os handlers de memória e
  MMIO não leem o banco SH4. O custo do JIT novo não está no flush: está em
  **preservar pelo ABI** os registradores fixos em cada chamada C++ (§14.7).
- **`Sh4cntx.pc` já fica velho entre blocos ligados.** Só é gravado nas
  saídas para o despachante.
- **`sh4_sched_next`:** C++ escreve nele no meio do bloco, então não pode ficar
  em registrador fixo.

## 1. Fallback para o interpretador (`shop_ifb` → `OpDesc[op]->oph`)

**Como é emitido** (`rec_arm64.cpp:640-672`):
- com NEEDPC, grava `next_pc`;
- chama o handler direto; com MMU, passa por `interpreter_fallback`
  (try/catch → `Do_Exception` → longjmp);
- o regalloc faz `FlushAllRegs(true)` antes (`ssa_regalloc.h:56-58`).

**Quais opcodes caem no fallback** (`decoder.cpp:1255-1266`,
`sh4_opcode_list.cpp`):
- **sempre:** `reios_trap`, `clrmac`, `mac.l`, `mac.w`, `div1`, `subv`,
  `addv`, `ldc.l @Rn+,SR`, `tas.b`, `tst/and/xor/or.b #imm,@(R0,GBR)`,
  `fcnvds`, `fcnvsd`;
- `trapa` e `sleep` (`dec_fallback`);
- **toda a FPU quando FPSCR.PR=1** no início do bloco
  (`decoder.cpp:907-913`);
- no jogo, na prática, sobram `div1` (dezenas por frame) e `tas.b`.

**O padrão:** os handlers usam as macros de `sh4_core.h` e podem tocar
qualquer campo. Os que alteram fluxo ou modo:
- **`ldc.l @Rn+,SR`:** `UpdateSR` (troca r0–r7 ↔ `r_bank`) + `UpdateINTC` →
  `Do_Interrupt`.
- **`trapa`:** `Do_Exception`.
- **`sleep`:** até 1000× `UpdateSystem_INTC`, um mainloop dentro do ifb.
- **`reios_trap`:** HLE, §9.
- **FPU:** lê `fpscr.PR/SZ` em execução; `frchg` e `lds FPSCR` do
  interpretador chamam `UpdateFPSCR` (troca fr ↔ xf).
- **`iNotImplemented`:** lança exceção C++ sem try no caminho sem MMU
  (problema que já existe hoje).

**O que o JIT novo precisa fazer:**
- descarregar tudo antes e recarregar tudo depois (incluindo `sr`, `fpscr`,
  `old_*`, `mac`, fr/xf);
- ou tratar os ops com PC/SR/FPSCR como saída de bloco + reentrada.

## 2. Caminhos lentos de memória e MMIO

**Sem MMU, `ReadMem*`/`WriteMem*` não tocam o banco SH4.** Exceções:
- `CCN_CCR_write` lê `curr_pc` (já velho hoje, só log; `ccn.cpp:87-111`);
- `CCN_MMUCR_write` → `ResetCache` → `CpuRunning=0` (raro);
- `write_INTC_IPR*` → `SRdecode` lê `sr.BL/IMASK` (`intc.cpp:17-39`). BL e
  IMASK só mudam via sync/ifb, que descarregam: é preciso manter essa
  invariante;
- TMU/DMAC/SCIF/SB/Holly escrevem `interrupt_pend` e `sh4_sched_next`.

**Frequência:** milhares de vezes por frame. **O que fazer:** nenhum flush.
Preservar os registradores fixos pelo ABI.

## 3. Agendador, interrupções e despacho

- **`intc_sched` → `UpdateSystem`** (a cada 448 ciclos, ~446 mil/s):
  - não lê registradores;
  - os callbacks escrevem `interrupt_pend` e `sh4_sched_next`, e `CpuRunning`
    via `dc_stop` no `Present`;
  - o JIT novo não precisa descarregar; só checar `CpuRunning` depois.
- **`rdv_DoInterrupts` / `UpdateINTC` (fim de bloco `BET_*Intr`, `rte`,
  `ldc SR`) → `Do_Interrupt`:**
  - lê sr, r15, vbr e o banco;
  - escreve ssr/spc/sgr, sr e pc;
  - `UpdateSR` troca r0–r7;
  - descarregar r0–r15, sr, old_sr, vbr, `r_bank` antes; recarregar
    r0–r7, sr, ssr/spc/sgr e pc depois.
- **`rdv_FailedToFindBlock` → `rdv_CompilePC`:**
  - lê `fpscr` inteiro e `sr.FD`;
  - se FD=1, chama `Do_Exception`;
  - se o cache for limpo, `CpuRunning=0`;
  - descarregar pelo menos fpscr/sr (mais seguro: tudo).
- **`rdv_BlockCheckFail`:** ClearCache, reinicia o mainloop.
- **`rdv_LinkBlock`:** só escreve next_pc.

## 4. Troca de modo SR/FPSCR e de bancos

**`UpdateSR`** (`sh4_core_regs.cpp:47-64`):
- `ChangeGPR` troca r0–r7 ↔ `r_bank`;
- escreve old_sr;
- `SRdecode`.

**`UpdateFPSCR`** (`:173-194`):
- `ChangeFP` troca xffr[0..15] ↔ xffr[16..31];
- escreve old_fpscr;
- ajusta o FPCR do host.

**Onde aparecem:**
- ops `sync_sr`/`sync_fpscr` (o regalloc descarrega os campos antes);
- ifb (`rte`, `ldc SR`, `lds FPSCR`, `frchg` do interpretador);
- `Do_Interrupt`/`Do_Exception`;
- reset.

**`frchg` nativo** (`decoder.cpp:524-531`) faz `shop_frswap` direto na memória
do contexto. **Opções para o JIT novo:**
- descarregar, chamar e recarregar em volta;
- ou fazer o swap nativo: permutação de registradores, e fr ↔ xf com NEON
  fixo vira troca de registradores.

O guard PR/SZ (`rec_arm64.cpp:772-786`) sai no meio do bloco: o caminho frio
precisa descarregar tudo.

## 5. Truques de espera deste fork

- **`sh4_delay_loop_skip(pc,cyc)`** (`sh4_sched.cpp:113-132`): lê e escreve
  **r4**, `sh4_sched_next` e memória. Descarregar r4 antes e recarregar
  depois, ou passar e devolver r4 por argumento.
- **`sh4_sched_idle_fastforward_if_ram(reg)`:** lê `r[reg]`. Descarregar ou
  passar o valor.
- **`sh4_sched_idle_fastforward`:** só `sh4_sched_next`.

## 6. Store Queue / TA

**Rotinas** (`do_sqw_nommu_area_3`, `do_sqw_nommu_full`, `TAWriteSQ`,
`ta_sq_stub`): nenhuma toca o banco SH4. **O que exigem:**
- sem flush;
- atenção ao que as rotinas sobrescrevem: `do_sqw_nommu_area_3` usa x0, x11,
  x12, v0, v1; o `ta_sq_stub` usa x2–x15.

## 7. Handler de falha e reescrita

**Handler** (`signal_handler`, `ngen_Rewrite*`, trampolins compactos):
- não toca o banco SH4, só x0/x2/pc do host;
- os trampolins salvam **só S16–S31 vivos** e assumem que os valores SH4
  estão em w19–w26, que o ABI preserva.

**Com registradores fixos:** todos os trampolins, stubs e reescritas precisam
salvar os registradores fixos que o ABI não preserva (x0–x18, v0–v7, v16–v31
e a metade alta de v8–v15).

## 8. MMU

- **Quando liga:** `mmu_enabled()` = `FullMMU && MMUCR.AT`. Só jogos WinCE,
  `ForceWinCE` e PBA Tour Bowling.
- **Como sai:** os caminhos com MMU usam longjmp (`ReadMemNoEx`,
  `interpreter_fallback`, `vmem32_handle_signal`, `bm_GetCodeByVAddr` com o
  hack WinCE que escreve r0/pc), e **isso perde os registradores fixos**.
- **Recomendação:** o JIT novo só roda com `!mmu_enabled()` e passa para o
  JIT antigo na troca de AT. O `CCN_MMUCR_write` já dispara `ResetCache`, o
  ponto natural para trocar de backend.

## 9. Savestate, reset, cheats, HLE

**Savestate, reset e cheats:**
- **Savestate:** grava e sobrescreve o `cntx` inteiro, fora do JIT (dc_stop +
  mutex do mainloop). Coberto se o mainloop carrega tudo na entrada e
  descarrega tudo na saída.
- **Reset:** fora do JIT.
- **Cheats:** só RAM, no vblank.

**HLE BIOS (reios), padrão no device:**
- **syscalls:** leem r4–r7 e escrevem r0;
- **`GDROM_G1_DMA_END` e `multi_xfer`:** reescrevem **r4 e pc**
  (`gdrom_hle.cpp:247-251,746-750`);
- **`reios_boot`:** reescreve o banco inteiro e o pc sem
  `UpdateSR`/`UpdateFPSCR`;
- **o que exigem:** flush completo, reload completo e pc do contexto (o bloco
  já termina em salto dinâmico).

## 10. Outros

- **`CpuRunning`** é escrito de fora: `dc_stop` (Present no callback spg, ou
  pela thread do frontend), `ngen_ResetBlocks` e `recSh4_Run`.
- **Recompilação do mainloop com o frame ainda na pilha**
  (`recSh4_ClearCache` → `ngen_ResetBlocks`). A saída acontece pelo
  `end_mainloop` da geração nova: o epílogo e o layout da pilha precisam ser
  idênticos entre gerações.
- **Ops do JIT que acessam o contexto em memória direto:** `fipr`, `ftrv`,
  `frswap`, `fsca`, `mov64`, a saída condicional (`sr.T`/`jdyn`) e o
  `CheckBlock`.

## 11. Campos escritos por C++ fora do fluxo do código gerado

| Campo | Quem | Quando |
|---|---|---|
| `interrupt_pend` | MMIO, callbacks, `SRdecode` | no meio do bloco |
| `CpuRunning` | `dc_stop`, `ngen_ResetBlocks` | no meio do bloco / outra thread |
| `sh4_sched_next` | `sh4_sched_request` (inclusive via MMIO), `UpdateSystem`, truques de espera | no meio do bloco e na entrada |
| `pc` | `rdv_*`, `Do_Interrupt`/`Do_Exception`, HLE, ifb | despacho, interrupção, ifb |
| sr, ssr, spc, sgr, old_sr, r0–r7/`r_bank` | `Do_Interrupt`/`Do_Exception` + `UpdateSR` | limites de bloco, ifb |
| r0, r4 (boot: r0–r15) | HLE, `sh4_delay_loop_skip` | ifb, entrada do bloco |
| fr/xf, fpscr, old_fpscr | `UpdateFPSCR`, FPU no ifb, boot HLE | sync_fpscr, ifb |
| mac | ifb `mac.*`/`clrmac` | ifb |

## 12. Resumo: o que o `jit_armv8_a` faz em cada caso

| Caminho | O que fazer |
|---|---|
| ifb genérico (`div1`, `tas.b`, `mac`, FPU com PR=1) | descarregar tudo antes, recarregar tudo depois |
| ifb que muda PC/SR/FPSCR (`ldc.l SR`, `trapa`, `sleep`) e HLE `reios_trap` | descarregar tudo, recarregar tudo, pc do contexto (ou saída de bloco) |
| `sync_sr` / `sync_fpscr` | descarregar os campos e recarregar os bancos, ou swap nativo |
| `UpdateSystem` | nada; checar `CpuRunning` |
| interrupção / exceção / bloco Intr | descarregar r0–r15, sr, old_sr, vbr, `r_bank`; recarregar r0–r7, sr, ssr/spc/sgr, pc |
| compilação (`rdv_CompilePC`) | descarregar tudo (FD=1 gera exceção; pode limpar o cache) |
| delay skip / idle ff | r4 ou `r[reg]` por argumento, ou descarregar e recarregar esse registrador |
| memória/MMIO sem MMU, SQ/TA, faults, trampolins | sem flush; **preservar os registradores fixos pelo ABI** |
| MMU | não usar o JIT novo (passar para o antigo) |
| entrada/saída do mainloop (savestate, reset, ResetCache) | carregar tudo na entrada, descarregar tudo na saída |

## 13. Restrição de ABI (AAPCS64)

- **O que o ABI preserva:** só x19–x28, e x27/x28 já estão em uso. Dos NEON,
  **só a metade baixa de 64 bits de v8–v15**.
- **O que o banco pretendido exige:** ~22 GPRs (r0–r15, T, pr, gbr, macl,
  mach, fpul) + 32 floats (fr/xf).
- **Consequência:**
  - não cabe nos registradores que o ABI preserva;
  - toda chamada C++ de alta frequência (MMIO, TA) precisa salvar e
    restaurar os registradores fixos que a chamada pode sobrescrever;
  - os stubs atuais assumem x0–x18 e v0–v7 mortos e precisam de revisão.
- **Desenhos possíveis:**
  - fixar em x19–x26 os registradores SH4 mais quentes (pelo estudo: T, r0,
    r4, r5, r14, r15, pr) e o resto nos que não são preservados, salvando só
    esses nas chamadas raras;
  - ou manter chamadas C++ fora do caminho quente (stubs próprios em asm que
    salvam só o necessário).
