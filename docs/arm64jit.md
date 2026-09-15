# Backend JIT ARM64 (`core/rec-ARM64/`) — Documentação de Referência

> Documentação técnica densa do dynarec ARM64 do flycast, gerada por leitura completa
> de `core/rec-ARM64/rec_arm64.cpp` (2316 linhas), `core/rec-ARM64/arm64_regalloc.h`,
> `core/hw/sh4/dyna/ssa_regalloc.h` (algoritmo genérico de regalloc compartilhado) e
> `core/rec-ARM64/ngen_arm64.S`. Reflete o estado do código **após** as mudanças desta
> sessão de otimização (regalloc estendido de FPU, tentativa revertida de fast-path de
> Store Queue) — não é uma descrição do fork "original". Ver `docs/tech_debits.md`
> (itens 1.1, 1.6, 1.7, 4.9) e `docs/history.md` (entradas de 2026-09-14) para o
> histórico de medições que fundamentam as afirmações de performance abaixo.
>
> Convenção de citação: `arquivo:linha` refere-se sempre ao estado atual do arquivo no
> working tree desta branch (`flycast2021-metallic77-base`), não ao commit base
> `603814c9f`.

## 0. Contexto: VIXL como "assembler as a library"

O backend não monta bytes ARM64 na mão — usa a **VIXL** (`core/deps/vixl/`, biblioteca
da ARM/Google originalmente escrita para o simulador V8) como camada de emissão de
instruções. `Arm64Assembler` (a classe central deste backend, `rec_arm64.cpp:174`)
**herda diretamente de `vixl::aarch64::MacroAssembler`**:

```cpp
// core/rec-ARM64/rec_arm64.cpp:174
class Arm64Assembler : public MacroAssembler
```

Isso significa que todo método capitalizado chamado sem qualificação dentro do backend
(`Str()`, `Ldr()`, `Add()`, `Cmp()`, `B()`, `Bind()`, `Mov()`, `Tbz()`, `Fmov()`, `Ld1()`,
`Csinc()` etc.) é herdado da VIXL, não escrito por este projeto. A VIXL cuida de:
- Seleção de encoding (ex.: `Mov` com imediato grande vira `movz`/`movk` automaticamente
  quando a VIXL constrói via `MacroAssembler` de alto nível, ao contrário do `Assembler`
  de baixo nível que exige o encoding exato).
- `Label`/`Bind()`/resolução de offsets relativos (branches, `Ldr` PC-relative de
  literais).
- `CPURegList`/`PushCPURegList`/`PopCPURegList` (usado por `PushCallerSaved`, seção 3).
- No caminho `CallRuntime`/`TailCallRuntime` nativos da VIXL (usados só em 2 lugares
  frios do arquivo, ver seção 6) — carregam o endereço absoluto de 64 bits num
  registrador escrátario e fazem `Blr`/`Br`, ao contrário do `GenCallRuntime` próprio
  do backend (seção 7) que usa `Bl` com offset relativo de 26 bits.

Este documento não descreve a VIXL em si — só como/onde ela é chamada.

---

## 1. Mapa geral dos arquivos

| Arquivo | Papel |
|---|---|
| `core/rec-ARM64/rec_arm64.cpp` | Arquivo principal. Define `Arm64Assembler` (codegen completo: switch de `shop_*`, acesso a memória, chamada de runtime, linking de bloco, mainloop, `CheckBlock` anti-SMC) e as funções `extern "C"`/globais que a interface genérica do dynarec (`hw/sh4/dyna/ngen.h`) espera: `ngen_Compile`, `ngen_CC_Start/Param/Call/Finish`, `ngen_Rewrite`, `ngen_mainloop`, `ngen_init`, `ngen_ResetBlocks`, `ngen_AllocateBlock`, `ngen_HandleException`. |
| `core/rec-ARM64/arm64_regalloc.h` | Especialização ARM64 do alocador de registrador: define os enums `eReg`/`eFReg` (mapeamento nome→código de registrador físico ARM64), os **pools de registradores alocáveis** (`alloc_regs`/`alloc_fregs`, o coração do item 4.9), e a classe `Arm64RegAlloc` que implementa os hooks virtuais (`Preload`/`Writeback`/`Preload_FPU`/`Writeback_FPU`) exigidos pela base genérica, mais `PushCallerSaved`/`PopCallerSaved` (novos nesta sessão). |
| `core/hw/sh4/dyna/ssa_regalloc.h` | Algoritmo genérico de alocação de registrador, **compartilhado** entre backends (ARM64 usa; outros podem usar `OLD_REGALLOC`/`hw/sh4/dyna/regalloc.h`, um alocador mais simples baseado em live-range, não documentado aqui pois o ARM64 não o usa por padrão). Templated em `<nreg_t, nregf_t, bool explode_spans>` — ARM64 instancia como `RegAlloc<eReg, eFReg>` (sem `EXPLODE_SPANS`, a flag de spans FPU 64-bit exploded em 2×32-bit está `#define`ada como comentário em `rec_arm64.cpp:31`, ou seja, desativada). É aqui que mora o algoritmo real: versionamento SSA, alocação por-operando (source antes de dest), heurística de spill "farthest next use". |
| `core/rec-ARM64/ngen_arm64.S` | Pontos de entrada em assembly puro (GAS), sem VIXL — stubs de baixo nível para: religar blocos (`ngen_LinkBlock_*_stub`), cold-path de "bloco não encontrado" (`ngen_FailedToFindBlock_{mmu,nommu}`), cold-path de falha de `CheckBlock` (`ngen_blockcheckfail`), e `context_switch_aarch64` (troca de contexto completa, usada pelo suporte a MMU/save-state de registrador, não pelo caminho comum). |
| `core/deps/vixl/aarch64/*` | Biblioteca de terceiros (ARM/Google). Só consumida via API pública (`MacroAssembler`, `VRegister`, `CPURegList`, `Label`, `Operand`, `MemOperand`). Não documentada aqui. |
| `core/hw/sh4/dyna/driver.cpp` | Não é parte de `rec-ARM64/`, mas é o "cliente" imediato: `rdv_CompilePC`, `rdv_LinkBlock`, `rdv_BlockCheckFail`, `rdv_FailedToFindBlock` — o lado C++ chamado pelos stubs `.S` e pelo dispatcher do mainloop. Documentado aqui na medida em que fecha o ciclo de linking/dispatch (seção 5). |

---

## 2. Pipeline de compilação de bloco

Entrada: `RuntimeBlockInfo* block` já populado pelo front-end (decodificação SH4 →
SHIL/IR, `block->oplist` é a lista de `shil_opcode`). Ponto de entrada global:

```cpp
// core/rec-ARM64/rec_arm64.cpp:2114
void ngen_Compile(RuntimeBlockInfo* block, bool force_checks, bool reset, bool staging, bool optimise)
{
    verify(emit_FreeSpace() >= 16 * 1024);
    compiler = new Arm64Assembler();
    compiler->ngen_Compile(block, force_checks, reset, staging, optimise);
    delete compiler;
    compiler = NULL;
}
```

`Arm64Assembler` é instanciado **por bloco** (não é reutilizado), com o buffer de
escrita apontando para o cursor atual do code cache (`emit_GetCCPtr()`,
`Arm64Assembler(void*)` em `rec_arm64.cpp:184`). O método de instância
`ngen_Compile` (`rec_arm64.cpp:308-1002`) faz, em ordem:

1. **`CheckBlock(force_checks, block)`** (`rec_arm64.cpp:312`) — preâmbulo anti-SMC,
   ver seção 6. Só emite código se `mmu_enabled() || force_checks`; senão é um no-op
   (`rec_arm64.cpp:1965-1966`).
2. **`regalloc.DoAlloc(block)`** (`rec_arm64.cpp:315`) — roda o alocador de
   registrador **inteiro** antes de emitir qualquer instrução do corpo do bloco (não é
   intercalado op-a-op com a emissão de código de dados; é intercalado com a emissão de
   código de *alocação/preload/writeback*, que sim acontece dentro do loop de opcodes —
   ver seção 3).
3. **Preâmbulo de scheduler/interrupção** (`rec_arm64.cpp:317-338`): decrementa um
   contador de ciclos (`cycle_counter` global se MMU ligado, ou registrador dedicado
   `w27` se não) pelo custo estimado do bloco (`block->guest_cycles`); se estourou o
   timeslice, chama `arm64_intc_sched` (gerado pelo mainloop, ver seção 5) para
   verificar interrupções pendentes antes de continuar.
4. **Loop principal sobre `block->oplist`** (`rec_arm64.cpp:340-993`):
   ```cpp
   for (size_t i = 0; i < block->oplist.size(); i++) {
       shil_opcode& op = block->oplist[i];
       regalloc.OpBegin(&op, i);      // aloca registradores físicos p/ esta op
       switch (op.op) { /* ~50 casos shop_* */ default: shil_chf[op.op](&op); }
       regalloc.OpEnd(&op);            // flush/writeback conforme necessário
   }
   regalloc.Cleanup();
   ```
   Cada `shop_*` handler chama `regalloc.MapRegister()`/`MapVRegister()` para obter o
   `Register`/`VRegister` físico já alocado por `OpBegin` e emite as instruções VIXL
   correspondentes. Um `default` cai em `shil_chf[op.op]` — uma tabela de handlers C++
   genéricos (fora do escopo deste doc) para opcodes sem tradução direta.
5. **`RelinkBlock(block)`** (`rec_arm64.cpp:996-999,1176-1296`) — emite o código de
   *fim* de bloco (branch condicional/incondicional/dinâmico para o próximo bloco SH4),
   guardando o offset onde começa (`block->relink_offset`) para poder ser **regravado**
   depois, quando o bloco de destino for conhecido (ver seção 5).
6. **`Finalize()`** (`rec_arm64.cpp:1001,1298-1337`) — `FinalizeCode()` (VIXL resolve
   labels pendentes), grava `block->code`/`block->host_code_size`, avança o cursor do
   code cache (`emit_Skip`), e limpa i-cache/d-cache (`vmem_platform_flush_cache`,
   necessário em ARM64 por ter caches de instrução separadas — código recém-escrito
   como dados precisa ser explicitamente invalidado do I-cache antes de ser executado).

Do lado de fora (`core/hw/sh4/dyna/driver.cpp:206-248`, `rdv_CompilePC`), o ciclo
completo é: aloca `RuntimeBlockInfo` (`ngen_AllocateBlock`, que também garante que o
mainloop global já foi gerado — `generate_mainloop()`), `rbi->Setup(pc, fpscr)` decodifica
SH4→SHIL, decide se o bloco é "temp" (SMC hotspot, vai para um code cache separado
`TempCodeCache` sem otimização), chama `ngen_Compile`, e por fim `bm_AddBlock(rbi)`
registra o bloco no block manager (hash map + tabela `fpcb`, seção 5).

**Diferença "staging"/`optimise`:** `do_opts = !rbi->temp_block` (`driver.cpp:232`) é
passado como `optimise` para `ngen_Compile`. Isso controla, no nível de codegen, se o
fastmem (seção 4) é tentado ou não — blocos "temp" (marcados como SMC hotspot após
`CheckBlock` falhar repetidamente, ver seção 6) compilam sempre pelo caminho lento,
sem fastmem, provavelmente para não gastar o orçamento fixo de 3 instruções em código
que será descartado logo (esses blocos não são cacheados permanentemente).

---

## 3. Alocação de registrador

### 3.1 Algoritmo geral (`core/hw/sh4/dyna/ssa_regalloc.h`)

É um alocador **baseado em SSA**, não um linear-scan clássico com live-ranges
pré-computados globalmente. A cada bloco compilado:

```cpp
// core/hw/sh4/dyna/ssa_regalloc.h:38-51
void DoAlloc(RuntimeBlockInfo* block, const nreg_t* regs_avail, const nregf_t* regsf_avail)
{
    this->block = block;
    SSAOptimizer optim(block);
    optim.AddVersionPass();       // renomeia toda definição de reg SH4 p/ uma "versão" única (SSA)
    ...
    while (*regs_avail  != (nreg_t)-1)  host_gregs.push_back(*regs_avail++);
    while (*regsf_avail != (nregf_t)-1) host_fregs.push_back(*regsf_avail++);
}
```

`AddVersionPass()` (em `ssa.h`, fora do escopo deste doc) atribui um **número de
versão** a cada escrita de um registrador SH4 lógico dentro do bloco — a mesma técnica
de renomeação de SSA clássica, usada aqui não para otimizações de dataflow completas,
mas para que o alocador saiba, **olhando só a versão**, se duas leituras do "mesmo"
registrador lógico realmente se referem ao mesmo valor (permitindo decisões de
liveness precisas sem análise de dataflow completa — ex.: `UsesReg`/`DefsReg`,
`ssa_regalloc.h:549-576`, comparam `reg` **e** `version`).

A alocação acontece **op a op**, não perlifespan pré-computado:

- **`OpBegin`** (`ssa_regalloc.h:53-124`): antes de emitir código para uma `shil_opcode`,
  chama `AllocSourceReg` para `rs1`/`rs2`/`rs3` (garante que os operandos fonte estão em
  registrador físico, fazendo *preload* se necessário) e depois `AllocDestReg` para
  `rd`/`rd2`. Casos especiais fazem *flush* total antes de continuar: `shop_ifb`
  (fallback pro interpretador — precisa que o estado SH4 esteja coerente na memória,
  pois o handler interpretado lê direto do contexto), `shop_sync_sr`/`shop_sync_fpscr`
  (sincronizam SR/FPSCR e os bancos de registrador dependentes), e (só com MMU ligada)
  `shop_readm`/`shop_writem`/`shop_pref` (acesso a memória pode gerar exceção MMU que
  precisa do estado SH4 100% coerente na memória para tratar corretamente).
- **`AllocSourceReg`/`AllocDestReg`** (`ssa_regalloc.h:356-465`): se o registrador
  lógico já está mapeado (`reg_alloced.find`), reusa; senão retira um registrador
  físico livre de `host_gregs`/`host_fregs` (dois `std::deque`, um int outro float) —
  se vazio, dispara **spill** (`SpillReg`). Destinos marcam `write_back` conforme
  `NeedsWriteBack` decidir (ver abaixo).
- **`OpEnd`** (`ssa_regalloc.h:126-157`): processa `pending_flushes` (registradores que
  foram *spillados como destino* durante esta op, mas cuja eviction do mapa foi adiada
  até o fim da op — comentário explícito em `SpillReg`, `ssa_regalloc.h:530-534`, para
  não confundir o caso em que o mesmo registrador físico é simultaneamente origem e
  destino da op atual). No fim do bloco inteiro, força `FlushAllRegs(false)` (escreve
  de volta tudo que estiver "dirty" sem necessariamente evictar do mapa — write-back
  final).

### 3.2 Heurística de spill: "próximo uso mais distante" (Belady-like)

```cpp
// core/hw/sh4/dyna/ssa_regalloc.h:467-542 (SpillReg, resumido)
for (auto const& reg : reg_alloced) {
    if (IsFloat(reg.first) != freg) continue;
    ...
    int first_use = /* menor índice de op subsequente que usa este reg físico */;
    if (first_use == -1) { spilled_reg = reg.first; break; }        // nunca mais usado -> spill imediato, ótimo
    if (first_use > latest_use) { latest_use = first_use; spilled_reg = reg.first; } // senão, o de uso MAIS distante
}
```

Isto é essencialmente o algoritmo ótimo offline de Belady ("farthest-in-the-future"),
aplicado localmente dentro do bloco (que é sempre pequeno — um "basic block" SH4, tipicamente
poucas dezenas de opcodes) — abordagem clássica de dynarecs de alta qualidade (LuaJIT
usa uma variante semelhante). Não há spill para pilha explícita neste código: "spillar"
aqui significa **escrever de volta (`Writeback`/`Writeback_FPU`) para o contexto SH4 em
memória** (`p_sh4rcb->cntx`, via `x28`), não empilhar em `sp`. O "slot de spill" de cada
registrador lógico SH4 é sempre o mesmo — seu campo fixo dentro da struct de contexto —
não há alocação dinâmica de slots de spill.

`NeedsWriteBack` (`ssa_regalloc.h:397-422`) decide, olhando para frente no bloco, se um
valor recém-definido *precisa* ser escrito de volta à memória antes do fim do bloco
(porque será lido por `shop_ifb`/sync/MMU-access, ou porque nunca mais é redefinido
antes do fim do bloco — nesse caso o write-back final do bloco cobre) — isso evita
write-backs redundantes quando um registrador é definido e redefinido várias vezes
dentro do mesmo bloco sem nunca ser observado por fora do regalloc entre uma definição
e outra.

### 3.3 Pools de registrador físico — estado ATUAL (pós item 4.9)

```cpp
// core/rec-ARM64/arm64_regalloc.h:40-41
static eReg  alloc_regs[]  = { W19, W20, W21, W22, W23, W24, W25, W26, (eReg)-1 };
static eFReg alloc_fregs[] = { S16, S17, S18, S19, S20, S21, S22, S23, S24, S25, S26, S27,
                                S28, S29, S30, S31, S8, S9, S10, S11, S12, S13, S14, S15, (eFReg)-1 };
```

| Classe | Registradores | Contagem | ABI ARM64 |
|---|---|---|---|
| Inteiro (`eReg`, mapeia `shop_*` com operandos `r32i`) | `W19`–`W26` | **8** | todos **callee-saved** |
| Float/FPU (`eFReg`, mapeia FPU SH4 — a maioria dos opcodes de um jogo Dreamcast) | `S16`–`S31` + `S8`–`S15` | **24** | `S16`–`S31` **caller-saved** (16, novos nesta sessão); `S8`–`S15` **callee-saved** (8, originais do fork) |

O pool inteiro **nunca mudou nesta sessão** — permanece nos 8 originais. A mudança real
(commit `262f55605`, "rec-ARM64: estende regalloc de FPU de 8 para 24 registradores
físicos") foi **só no pool de float**, triplicando de 8 para 24. Por que só float:

- **Tentativa revertida:** estender `alloc_regs` para incluir `W9`–`W15` também foi
  implementada e **causou crash** (SIGSEGV, `"was not in vram"`) em uso interativo.
  Causa raiz: `W9`–`W15` (e `W0`–`W2`, `W8`, `X9`, `X10`, `X15`) são usados como
  **scratch hardcoded** em dezenas de pontos do próprio codegen deste arquivo — ex.
  `w9`/`w10`/`w11` em `shop_swaplb` (`rec_arm64.cpp:440-442`), `shop_rocr`/`rocl`
  (`566-624`), `shop_shld`/`shad` (`663-669`), `x9`/`x10` em `shop_fipr`/`ftrv`/`frswap`
  (`941-977`), `w0`/`w1`/`w2` em quase toda op de carry (`shop_adc`/`sbc`/`negc`,
  `493-568`) etc. Incluir esses registradores no pool do alocador gera **concorrência
  destrutiva**: o alocador pode colocar uma variável SH4 viva exatamente no registrador
  que o handler de uma outra op usa como temporário local, e o handler sobrescreve o
  valor sem o alocador saber. Contagem real de uso hardcoded no arquivo (grep
  `core/rec-ARM64/rec_arm64.cpp`, exclui o pool de alocação): `w0`×64, `w1`×36, `w9`×2,
  `w10`×15, `w11`×13, `x9`×24, `x10`×6, `x15`×4 — volume alto demais para "limpar" com
  segurança sem uma auditoria completa e reescrita desses handlers. **Revertido — só os
  16 floats extras seguem ativos.**
- Os registradores float extras (`S16`–`S31`) **não** têm esse problema porque o
  código deste arquivo **não usa nenhum `s16`–`s31` como scratch hardcoded em lugar
  nenhum** — os únicos floats usados como scratch são `s0`/`s1`/`v0`–`v7` (ex.
  `shop_fsrra`, `938-953`, `shop_ftrv`, `955-969`), todos fora da faixa `S16-S31`.
  Fazia sentido estender por aqui porque **o Dreamcast é massivamente dependente de
  FPU** (transformações 3D, produto interno via `shop_fipr`, etc.) — mais registradores
  físicos de float reduzem diretamente o spill em blocos com muita aritmética de ponto
  flutuante.

### 3.4 `PushCallerSaved`/`PopCallerSaved` — o preço dos 16 floats extras

Como `S16`–`S31` são **caller-saved** pela ABI AArch64 (ao contrário de `S8`–`S15`,
callee-saved — ou seja, uma função chamada (`GenCallRuntime`) tem a obrigação, pela
ABI, de preservá-los se estiver usando-os, já que a função chamada pode destruí-los
livremente), estender o pool para essa faixa exige salvar/restaurar explicitamente
qualquer um deles que esteja **atualmente alocado a uma variável SH4 viva** em volta de
toda chamada de runtime:

```cpp
// core/rec-ARM64/rec_arm64.cpp:2265-2295
void Arm64RegAlloc::PushCallerSaved()
{
    CPURegList vlist(CPURegister::kVRegister, 64, 0);
    for (auto const& it : reg_alloced)
        if (IsFloat(it.first)) {
            eFReg hreg = (eFReg)it.second.host_reg;
            if (hreg >= S16 && hreg <= S31)
                vlist.Combine(VRegister::GetDRegFromCode(hreg));
        }
    if ((vlist.GetCount() % 2) != 0) vlist.Combine(d7);   // padding p/ pares (stp/ldp, alinhamento)
    if (!vlist.IsEmpty()) assembler->PushCPURegList(vlist);
}
// PopCallerSaved é o espelho exato, com PopCPURegList
```

Note: a checagem é **dinâmica** (`reg_alloced`, o mapa vivo do alocador *naquele ponto
exato do bloco*) — não é um push/pop fixo de todos os 16 sempre; só os que estão de
fato alocados e vivos naquele ponto do bloco são empurrados. Ainda assim, isso roda
**dentro de `GenCallRuntime`** (seção 7), ou seja, em **toda** chamada de runtime
emitida pelo backend — inclusive as de altíssima frequência (`UpdateSystem`, chamado
pelo mainloop a cada timeslice, medido em ~7.400×/frame no item 1.2 de
`tech_debits.md`; e os slow-paths de leitura/escrita de memória, seção 4).

### 3.5 Medições reais desta sessão (item 4.9 de `tech_debits.md`)

| Jogo/cena | Métrica | Antes (8 floats) | Depois (24 floats) | Delta |
|---|---|---|---|---|
| Shenmue (3D, boot real, `--benchmark 90 --benchmark-warmup 90`) | FPS médio | 26,87 | 28,35 | **+5,5%** |
| Shenmue | `core_average` | 26,10 ms | 25,47 ms | -0,63 ms |
| Shenmue | `video_average` | 11,11 ms | 9,78 ms | -1,33 ms |
| KOF Neowave/`kofnw` (2D, savestate, A/B limpo, sem confound de instrumentação) | `core_average` | 11,730 ms | 12,106 ms | **+3,2% (mais lento)** |
| `kofnw` | `core_frames`/30s | 1512 | 1475 | **-2,4%** |
| `kofnw` | `video_average` | 8,108 ms | 8,217 ms | ~ruído (renderer não foi tocado) |

**Leitura importante:** a primeira medição de 2D deu um número pior (+8,0%/-5,0%), mas
metade da regressão aparente era **a própria instrumentação `chrono` de profiling**
(8 arquivos com `perfinstr_*`) que ainda estava no binário quando o A/B foi feito —
depois de removê-la e refazer o build do zero (`make clean`, necessário por causa de um
`.o` de `TexCache.h` não recompilado corretamente por dependência de header mal
rastreada pelo Makefile), o número real caiu para +3,2%/-2,4%. **A regressão em 2D é
real, mas menor do que a primeira medição sugeriu.**

**Hipótese arquitetural para a assimetria 3D-ganha/2D-perde (não confirmada por
instrumentação direta, mas consistente com o padrão observado):** jogos 2D como o
KOF/MBAA tendem a ter blocos SH4 **menores e mais numerosos** (mais branches de lógica
de jogo, menos aritmética de transformação 3D por bloco) e **mais tráfego de I/O
mapeado em memória** (paleta, VRAM, registradores do PVR, por sprite) — ou seja, pagam
a taxa de `PushCallerSaved`/`PopCallerSaved` com frequência igual ou maior que o 3D,
mas sem o benefício proporcional de menos spill (blocos curtos raramente precisavam de
mais de 8 registradores float mesmo antes da extensão). O item 1.7 (seção 4 abaixo,
achado do Store Queue) reforça essa hipótese diretamente: o loop mais executado do
MBAA é uma sequência de `shop_writem` de 32 bits chamando `GenCallRuntime` repetidamente
— exatamente o tipo de hot path onde o overhead por chamada mais se acumula.

**Estado da decisão:** em aberto (`docs/current_plan.md`, item 3) — reverter, manter, ou
reduzir o pool estendido para um meio-termo, ainda não decidido com o usuário. **O
código atual no working tree mantém a extensão ativa** (24 floats, sem
`PushCallerSaved`/`PopCallerSaved`), refletido neste documento.

### 3.6 Registradores host reservados/fixos (nunca alocáveis)

Além dos pools de `alloc_regs`/`alloc_fregs`, o backend reserva estaticamente, para uso
próprio do codegen (não disponíveis ao regalloc em nenhuma circunstância):

| Registrador | Papel fixo |
|---|---|
| `x28` | Ponteiro base do contexto SH4 (`p_sh4rcb->cntx`) — **e também** base da região de fastmem (seção 4.2), já que contexto e espaço de endereço do guest ficam contíguos na mesma região mapeada. |
| `w29`/`x29` | "next_pc" — valor do PC de destino calculado no fim do bloco, lido pelo dispatcher (`no_update`, seção 5). |
| `w27`/`x27` | Contador de ciclos do timeslice (quando MMU desligada) — substitui a global `cycle_counter` usada quando MMU ligada. |
| `lr`/`x30` | Link register — usado normalmente pela convenção `Bl`/`Ret` em `GenCallRuntime`/mainloop. |
| `w0`-`w2`, `x0`-`x2`, `w8`, `w9`-`w11`, `x9`, `x10`, `x15` | Scratch hardcoded, espalhado pelo switch de `shop_*` (ver 3.3). |
| `s0`, `s1`, `v0`-`v7` | Scratch float hardcoded (`shop_fsrra`, `shop_fipr`, `shop_ftrv`, `shop_frswap`, `ngen_BinaryFop`). |
| `w0`-`w7`/`x0`-`x7`/`s0`-`s7` | Registradores de argumento de chamada (`call_regs`/`call_regs64`/`call_fregs`, populados em `Arm64Assembler::Arm64Assembler()`, `rec_arm64.cpp:186-211` — convenção AAPCS64 padrão). |

---

## 4. Acesso a memória — `GenReadMemory`/`GenWriteMemory`

Esta é a parte mais importante do backend em termos de volume de execução — o achado
central desta sessão (Store Queue, item 1.7) mostra que um `shop_writem` compilado pelo
caminho lento, executado em loop apertado, é o hot path dominante em pelo menos dois
jogos (MBAA 71% self-time, kofnw ~40-51%).

### 4.1 Estrutura em cascata

`GenReadMemory`/`GenWriteMemory` (`rec_arm64.cpp:1526-1552` e `1747-1775`) tentam, em
ordem, até uma das três estratégias funcionar:

```cpp
// GenWriteMemory, resumido
void GenWriteMemory(const shil_opcode& op, size_t opid, bool optimise)
{
    if (GenWriteMemoryImmediate(op)) return;          // 1) endereço constante em tempo de compilação
    GenMemAddr(op, call_regs[0]);                       // computa endereço em w0 (caso geral)
    ... shil_param_to_host_reg(op.rs2, ...) ...          // coloca o dado a escrever em w1/x1
    if (optimise && GenWriteMemoryFast(op, opid)) return; // 2) fastmem (nvmem)
    GenWriteMemorySlow(size);                            // 3) chamada de runtime genérica
}
```

O caminho de leitura é análogo (`GenReadMemory`). A ordem importa: **imediato primeiro**
(mais barato — endereço conhecido em tempo de compilação permite resolver o *handler*
ou o *ponteiro direto* uma única vez, sem nenhuma instrução de cálculo de endereço em
runtime), depois **fastmem** (endereço computado em runtime, mas acesso direto sem
chamada de função), só then **slow/call genérico**.

### 4.2 `GenWriteMemoryImmediate`/`GenReadMemoryImmediate` — endereço constante

```cpp
// core/rec-ARM64/rec_arm64.cpp:1777 (write) / 1554 (read)
bool GenWriteMemoryImmediate(const shil_opcode& op)
{
    if (!op.rs1.is_imm()) return false;
    ...
    bool isram = false;
    void* ptr = _vmem_write_const(addr, isram, size > 4 ? 4 : size);
    if (isram) {
        Ldr(x0, reinterpret_cast<uintptr_t>(ptr));  // ponteiro HOST direto, resolvido em tempo de compilação
        Str(reg2, MemOperand(x0));                    // store direto, zero overhead de dispatch
    } else {
        // não é RAM: ptr é o ENDEREÇO DA FUNÇÃO HANDLER, também resolvido em compile-time
        Mov(w0, addr);
        GenCallRuntime((void (*)())ptr);               // chama o handler específico já conhecido
    }
    return true;
}
```

Só se aplica quando `op.rs1.is_imm()` — o SH4 usa isso tipicamente para acesso a
registradores/endereços fixos (modo de endereçamento `@(disp,PC)`/estruturas globais
conhecidas do jogo). `_vmem_write_const`/`_vmem_read_const` (`core/hw/mem/_vmem.cpp`)
resolvem, no momento da compilação, se o endereço cai em RAM real (`isram`, caso em que
devolvem um ponteiro host direto para aquele byte) ou em uma região com handler
registrado (devolvem o *ponteiro da função handler* diretamente, sem precisar de
nenhuma tabela de dispatch em runtime). É estritamente mais rápido que fastmem quando
aplicável: zero instruções de cálculo de endereço, e no caso RAM, zero chamada de
função também.

Com MMU (`FullMMU`) ligada, há ainda uma tradução de página em tempo de compilação
(`mmu_data_translation<MMU_TT_DWRITE/DREAD>`, `rec_arm64.cpp:1785-1811`/`1561-1588`),
restrita a permanecer dentro da mesma página de 4KB do início do bloco (ou do fim, para
write) — se a tradução falhar ou cruzar página, cai para os caminhos seguintes.

### 4.3 `GenWriteMemoryFast`/`GenReadMemoryFast` — fastmem via `nvmem`

Este é o mecanismo mais sofisticado do backend: mapeia **o espaço de endereço SH4
inteiro** (32 bits completos, confirmado em log do device: `"nvmem is enabled, with
addr space of size 4GB"`, `core/hw/mem/_vmem.cpp:482`) diretamente na memória virtual
do processo host, permitindo que um acesso de memória do jogo emulado vire **um único
load/store nativo do host**, sem cálculo de tradução nem chamada de função — o preço é
que endereços sem RAM real por trás geram SIGSEGV, tratado via handler de sinal +
patch de código em runtime.

```cpp
// core/rec-ARM64/rec_arm64.cpp:1912-1954 (write; read é simétrico em 1703-1745)
bool GenWriteMemoryFast(const shil_opcode& op, size_t opid)
{
    if (!_nvmem_enabled() || (mmu_enabled() && !vmem32_enabled()))
        return false;

    Instruction *start_instruction = GetCursorAddress<Instruction *>();

    // AVISO: o código de rewrite depende de haver 1 ou 2 ops antes do acesso.
    // Atualizar ngen_Rewrite (e possivelmente write_memory_rewrite_size) se mudar isto.
    if (!_nvmem_4gb_space()) {
        Ubfx(x7, *call_regs64[0], 0, 29);              // trunca p/ janela de 512MB (RAM_SIZE)
        Add(x7, x7, sizeof(Sh4Context), LeaveFlags);
    } else {
        Add(x7, *call_regs64[0], sizeof(Sh4Context), LeaveFlags); // espaço de 4GB completo, sem máscara
    }

    switch (size) {
        case 1: Strb(w1, MemOperand(x28, x7)); break;
        case 2: Strh(w1, MemOperand(x28, x7)); break;
        case 4: Str(w1,  MemOperand(x28, x7)); break;
        case 8: Str(x1,  MemOperand(x28, x7)); break;
    }
    EnsureCodeSize(start_instruction, write_memory_rewrite_size);  // sempre exatamente 3 instruções
    return true;
}
```

**Layout de memória:** `x28` (o ponteiro de contexto SH4, seção 3.6) e a região de
fastmem são **a mesma alocação contígua** — o contexto (`Sh4Context`) fica no início, e
o endereço de guest `0x00000000` do jogo corresponde a `x28 + sizeof(Sh4Context)`.
Por isso o cálculo de endereço é só `Add(x7, addr, sizeof(Sh4Context))` (ou, se o host
não suporta reservar os 4GB completos de espaço de endereço — `!_nvmem_4gb_space()`,
fallback para uma janela de 512MB — `Ubfx` trunca o endereço para 29 bits primeiro).
O acesso final é sempre `MemOperand(x28, x7)` — **um único load/store indexado por
registrador**, sem chamada de função nenhuma no caso comum.

**O "orçamento fixo de 3 instruções":**

```cpp
// core/rec-ARM64/rec_arm64.cpp:1956-1961
void EnsureCodeSize(Instruction *start_instruction, int code_size)
{
    while (GetCursorAddress<Instruction *>() - start_instruction < code_size * kInstructionSize)
        Nop();
    verify(GetCursorAddress<Instruction *>() - start_instruction == code_size * kInstructionSize);
}
// rec_arm64.cpp:2108-2109
const int read_memory_rewrite_size = 3;   // ubfx, add, ldr
const int write_memory_rewrite_size = 3;  // ubfx, add, str
```

No caso de 512MB (`Ubfx`+`Add`+`Ldr/Str` = 3 instruções reais), o orçamento é exato.
No caso de 4GB (`Add`+`Ldr/Str` = 2 instruções reais), sobra 1 slot, preenchido com
`Nop()`. Em ambos os casos, a sequência final ocupa **exatamente 12 bytes** (3×4), e o
`verify()` (que aborta o processo se falso — `core/types.h:429`) garante isso em
build de debug/release (não há `NO_VERIFY` definido neste fork). Esse tamanho fixo é
**necessário** para o mecanismo de rewrite abaixo funcionar: o patch em runtime precisa
saber exatamente onde a sequência começa e termina para sobrescrevê-la sem afetar o
código vizinho (nenhum branch/patch de tamanho variável — reescrever o buffer JIT
*in-place*, sem deslocar instruções subsequentes, é a única opção viável sem re-linkar
todo o bloco).

### 4.4 `ngen_Rewrite` — o handler de SIGSEGV que reescreve código

Quando um acesso fastmem cai numa região do espaço de 4GB **sem RAM real por trás**
(registradores de hardware mapeados em memória, região sem handler simples, etc.), o
load/store gera SIGSEGV. O handler de sinal do processo
(`core/libretro/common.cpp:198-369`, `context_segfault`) intercepta, e no caminho
ARM64 dynarec (`HOST_CPU==CPU_ARM64`, `common.cpp:348-352`):

```cpp
// core/libretro/common.cpp:348-352
else if (dyna_cde && ngen_Rewrite(ctx.pc, 0, 0))
{
    context_to_segfault(&ctx, segfault_ctx);   // devolve o contexto (agora com pc já reescrito) p/ retomar execução
}
```

```cpp
// core/rec-ARM64/rec_arm64.cpp:2178-2211
bool ngen_Rewrite(unat& host_pc, unat, unat)
{
    u32 *code_ptr = (u32 *)CC_RX2RW(host_pc);
    u32 armv8_op = *code_ptr;
    // decodifica QUAL instrução Ldr/Str faltou (tabela de máscaras armv8_mem_ops,
    // rec_arm64.cpp:2146-2177) -> descobre size + is_read
    ...
    // volta 1 (espaço 4GB) ou 2 (espaço 512MB) instruções -- para o INÍCIO
    // da sequência reservada de 3 instruções
    u32 *code_rewrite = code_ptr - 1 - (!_nvmem_4gb_space() ? 1 : 0);
    Arm64Assembler *assembler = new Arm64Assembler(code_rewrite);  // NOVO assembler, buffer = local exato
    if (is_read)  assembler->GenReadMemorySlow(size);
    else          assembler->GenWriteMemorySlow(size);
    assembler->Finalize(true);   // true = "rewrite": não avança block->code, só reflush icache
    delete assembler;
    host_pc = (unat)CC_RW2RX(code_rewrite);   // reexecuta a partir do início da sequência reescrita
    return true;
}
```

Ponto sutil e importante: o `Arm64Assembler` construído aqui é **uma instância nova**,
com seu próprio `Arm64RegAlloc regalloc(this)` **vazio** (`reg_alloced` default-vazio,
já que nenhum `DoAlloc`/`OpBegin` roda neste caminho). Isso significa que, dentro deste
`GenWriteMemorySlow`/`GenReadMemorySlow` de rewrite, a chamada interna a
`GenCallRuntime` (seção 7) sempre encontra `PushCallerSaved`/`PopCallerSaved` com
`reg_alloced` vazio → **nenhum push/pop é emitido** (`vlist.IsEmpty()` verdadeiro) — o
corpo final é só `Bl <handler>` (mais `Sxtb`/`Sxth` para leituras de 1/2 bytes), que
cabe folgado no orçamento de 3 instruções (`EnsureCodeSize` no fim de
`GenReadMemorySlow`/`GenWriteMemorySlow`, `rec_arm64.cpp:1132`/`1173`, preenche o resto
com `Nop`). **Depois da primeira falta, o site fica permanentemente reescrito para o
caminho lento** — não há tentativa de voltar para fastmem depois.

**Nota de risco não confirmada (achado desta análise, não medido):** o mesmo
`EnsureCodeSize(..., write_memory_rewrite_size=3)` também protege
`GenWriteMemorySlow`/`GenReadMemorySlow` quando chamado **durante a compilação normal
do bloco** (não via rewrite) — isto é, quando `GenWriteMemoryFast` retorna `false`
(fastmem indisponível: MMU com paginação sem `vmem32_enabled()`, ou `optimise=false`
em blocos "temp"/SMC-hotspot). Nesse caminho o `Arm64Assembler` **é o mesmo objeto que
está compilando o bloco inteiro**, com `regalloc.reg_alloced` potencialmente **não
vazio** — se, naquele ponto exato do bloco, houver registradores `S16`-`S31`
alocados e vivos, `PushCallerSaved` dentro do `GenCallRuntime` interno emitiria `Stp`
reais (até 8 pares), o que estouraria o orçamento fixo de 3 instruções e faria o
`verify()` final de `EnsureCodeSize` abortar o processo. **Isso não foi observado como
crash nesta sessão** — provavelmente porque, nas condições de teste reais (nvmem
sempre ligado no device, `FullMMU` tipicamente desligado, `optimise=true` na grande
maioria dos blocos), esse caminho combinado (slow-path *inline*, não via rewrite, com
float caller-saved vivo) é raro ou nunca exercitado. Fica registrado aqui como
**dívida técnica não confirmada** — ver seção 8.

### 4.5 `GenWriteMemorySlow`/`GenReadMemorySlow` + `GenCallRuntime` — caminho genérico

Último recurso: chama a função de runtime genérica por tamanho (`WriteMem8/16/32/64`,
`ReadMem8/16/32/64`, ou as variantes `*NoEx<T>` com tratamento de exceção MMU) via
`GenCallRuntime` — ver seção 7 para o mecanismo completo de chamada. Argumentos
(endereço em `w0`/dado em `w1`) já foram colocados pelo chamador (`GenReadMemory`/
`GenWriteMemory`) antes de decidir qual dos 3 caminhos usar.

### 4.6 Investigação do Store Queue (item 1.7) — o que domina o profile real

Achado desta sessão via `perf`+`gdb` ao vivo (não é leitura de código, é medição
direta — ver `docs/history.md`, entradas de 2026-09-14 "Mistério do `SH4_TCB`
RESOLVIDO" e seguintes):

- **O que é:** SH4 Store Queues (SQ), região P4 `0xE0000000`-`0xE3FFFFFF`. É um
  recurso de *hardware* do SH4 real: dois buffers de 8 palavras (32 bits) cada, que o
  jogo preenche com instruções `MOV.L` individuais e depois "descarrega" de uma vez
  com `PREF` (prefetch) — usado tipicamente para transferências em burst (ex.: enviar
  geometria/textura para o barramento de vídeo).
- **Padrão medido ao vivo (gdb, breakpoint em `_vmem_WriteMem32`, dado real):** 16
  escritas sequenciais em `0xE01C0460`...`0xE01C049C` (passo de 4 bytes, todas
  `data=0x0` — um "fast clear" das duas SQs), seguidas de 1 escrita numa página de RAM
  normal sob rastreamento de dirty-page (`vramlock`). Repete em loop.
- **Por que cada palavra individual passa pelo slow-path:** cada `MOV.L` que enche uma
  SQ é uma instrução `shop_writem` de 32 bits **isolada** no SHIL — o compilador não
  tem como saber que ela faz parte de um "burst" até ver o `PREF` que efetivamente
  dispara o flush (`shop_pref`, `rec_arm64.cpp:811-853`, que sim já tem um fast-path
  dedicado: checa `addr>>26==0x38` e chama `do_sqw_mmu`/`do_sqw_nommu` diretamente,
  gravando o buffer de 64 bytes de uma vez — `sq_buffer`, `core/hw/sh4/sh4_if.h:378`).
  As 16 escritas *individuais* que preenchem o buffer, porém, **não** passam por
  fastmem: a região `0xE0xxxxxx` não tem RAM real por trás no mapeamento de 4GB, então
  cada uma delas sofre SIGSEGV na primeira execução, é reescrita por `ngen_Rewrite`
  para o caminho lento, e a partir daí **toda execução subsequente desse bloco paga uma
  chamada `GenCallRuntime`→`_vmem_WriteMem32` completa por palavra** (salto para fora
  do buffer JIT e volta) — 16 chamadas de função para preencher o que semanticamente é
  um único store de 64 bytes num buffer fixo.
- **Confirmado como trabalho real, não stall:** sampling comparando `cycles` vs.
  `instructions` no `perf` mostrou o hot spot ficando **mais** dominante em
  `instructions` (não menos) — ou seja, não é espera de memória/cache miss, é volume
  real de instruções executadas.
- **Domínio no profile:** `SH4_TCB+offset` (o endereço específico deste loop dentro do
  buffer JIT) com **51-71% de self-time**, dependendo do jogo (kofnw ~40-51%, MBAA até
  71%/54% self) — o hot spot isolado mais dominante encontrado no projeto inteiro.

### 4.7 Tentativa de fix (implementada, testada, **revertida**) e a lição

**Implementação tentada** (`GenWriteMemoryFast`, antes do caminho de fastmem
existente): checagem inline `addr>>26==0x38` (o mesmo teste que `shop_pref` já faz)
para detectar endereço de Store Queue **em todo write de 32 bits**, e nesse caso gravar
direto em `sq_buffer` (offset fixo relativo a `x28`), pulando `GenCallRuntime`/
`_vmem_WriteMem32` inteiramente para esses casos.

**Resultado medido (não hipotético — benchmark oficial, 2 rodadas):**

| | `core_average` (kofnw) |
|---|---|
| Antes do fix (só regalloc+batching) | 12,070 ms |
| Com fast-path de SQ, rodada 1 | 12,762 ms |
| Com fast-path de SQ, rodada 2 | 12,837 ms |
| **Delta** | **~+6% mais lento** (consistente nas 2 rodadas — não é ruído) |

**Causa provável da regressão (análise pós-mortem, registrada em `docs/history.md`):**
a checagem nova (`Lsr`/`Cmp`/`B`) foi implementada **inline dentro de `GenWriteMemoryFast`,
sempre emitida**, rodando em **todo** write de 32 bits do jogo inteiro — não só nos de
Store Queue. Escritas de 32 bits para RAM comum são numericamente muito mais frequentes
que as de SQ; o imposto pago no caso comum (3 instruções extras: `Lsr`+`Cmp`+`B`, em
*todo* store de 32 bits, mesmo os que nunca são SQ) superou a economia nas poucas (mas
quentes) escritas de SQ que de fato se beneficiavam. **Revertido imediatamente.**

**Lição explícita do projeto (regra de ouro se provando na prática de novo):** a leitura
de código sugeria "checagem barata e bem prevista pelo branch predictor" — a intuição
não se sustentou na medição real. Uma abordagem mais cirúrgica, que só afetasse os call
sites que **já precisam de rewrite** (isto é, detectar o padrão só dentro de
`ngen_Rewrite`, como um stub compartilhado de destino, sem tocar o caminho comum de
fastmem que roda para todo o resto do jogo) tem uma chance real de funcionar, porque o
custo extra ficaria restrito exatamente aos sites que já pagam o slow-path mesmo — mas
isso exige mexer em infraestrutura de signal handler/code-patching mais arriscada e
**não foi tentada** ainda (avaliada como fora do escopo desta sessão).

---

## 5. Linking/dispatch de blocos

O dynarec **não** faz uma tabela de saltos por hash lookup em toda transição de bloco —
usa **linking direto** ("patch-on-first-execution", técnica clássica de dynarecs de
threaded-code): o código de fim de bloco (emitido por `RelinkBlock`) começa apontando
para um stub genérico de "ainda não linkado"; na primeira execução, esse stub resolve o
bloco de destino e **reescreve o código do bloco de origem** para um branch direto — a
partir daí, transições entre esses dois blocos específicos não pagam mais nenhum
overhead de dispatch.

### 5.1 `RelinkBlock` — código de fim de bloco, por tipo

```cpp
// core/rec-ARM64/rec_arm64.cpp:1176-1296
u32 RelinkBlock(RuntimeBlockInfo *block)
{
    switch (block->BlockType) {
    case BET_StaticJump: case BET_StaticCall:
        if (block->pBranchBlock == NULL)
            GenCallRuntime(ngen_LinkBlock_Generic_stub);   // ainda não linkado -> stub
        else
            GenBranch(block->pBranchBlock->code);            // já linkado -> branch direto
        break;
    case BET_Cond_0: case BET_Cond_1:
        // Cmp + B condicional -> GenBranch(pBranchBlock) ou stub cond_Branch
        // + fallthrough -> GenBranch(pNextBlock) ou stub cond_Next
        break;
    case BET_DynamicJump: case BET_DynamicCall: case BET_DynamicRet:
        // Indexação DIRETA na tabela fpcb (ver 5.2), sem stub de link algum --
        // blocos dinâmicos (destino só conhecido em runtime, ex. RTS) sempre
        // passam por essa tabela, nunca ficam "linkados" estaticamente.
        break;
    case BET_DynamicIntr: case BET_StaticIntr:
        GenCallRuntime(UpdateINTC);
        GenBranch(*arm64_no_update);
        break;
    }
    return GetBuffer()->GetCursorOffset() - start_offset;
}
```

`block->pBranchBlock`/`pNextBlock` são ponteiros para `RuntimeBlockInfo` já resolvidos
(preenchidos por `bm_AddBlock`/`rdv_LinkBlock`, ver 5.3) — **se não-nulos**, o codegen
emite um `GenBranch` direto (`B`/`Bl` com offset relativo, resolvido para o endereço de
código real do bloco vizinho). Se ainda nulos (bloco vizinho nunca foi compilado/visto),
cai no stub genérico.

### 5.2 Blocos dinâmicos — tabela `fpcb` (dispatch O(1) sem stub)

```cpp
// core/rec-ARM64/rec_arm64.cpp:1250-1273 (fim de bloco dinâmico, sem MMU)
Str(w29, sh4_context_mem_operand(&next_pc));
Sub(x2, x28, offsetof(Sh4RCB, cntx));
Ubfx(w1, w29, 1, 24);                    // 24 bits do PC (bit 0 descartado -- alinhamento de 2)
Ldr(x15, MemOperand(x2, x1, LSL, 3));    // lookup direto: fpcb[(pc>>1) & mask]
Br(x15);
```

`fpcb` ("fast program-counter block", `core/hw/sh4/sh4_if.h:374`,
`void* fpcb[FPCB_SIZE]`, `FPCB_SIZE = RAM_SIZE_MAX/2`) é um **array direto-mapeado**
(não hash map) de ponteiros de código, indexado pelos bits baixos do PC do guest —
`FPCA(x)` em `core/hw/sh4/dyna/blockmanager.cpp:40`:
`((DynarecCodeEntryPtr&)sh4rcb.fpcb[(x>>1)&FPCB_MASK])`. Todo bloco recém-compilado
grava seu ponteiro nessa tabela (`bm_AddBlock`). Se a entrada aponta para
`ngen_FailedToFindBlock` (valor sentinela — bloco nunca compilado, ou invalidado por
SMC), o `Br` salta direto para o stub de recompilação (seção 5.4) — **sem branch
condicional explícito no código gerado**: o "cache miss" é tratado deixando a própria
entrada da tabela apontar para o handler de miss, então o `Br x15` sempre funciona,
seja para código real ou para o stub. Essa é a técnica de dispatch usada tanto pelo
mainloop principal (`no_update`, `rec_arm64.cpp:1350-1371`) quanto por qualquer bloco
dinâmico — é o caminho mais "quente" de dispatch entre blocos SH4.

### 5.3 Resolução de link — `rdv_LinkBlock` (`driver.cpp`) + stubs `.S`

```asm
; core/rec-ARM64/ngen_arm64.S:1-24
ngen_LinkBlock_cond_Branch_stub:
    mov w1, #1
    b ngen_LinkBlock_Shared_stub
ngen_LinkBlock_cond_Next_stub:
    mov w1, #0
    b ngen_LinkBlock_Shared_stub
ngen_LinkBlock_Generic_stub:
    mov w1, w29                  ; pc do djump, caso precise
ngen_LinkBlock_Shared_stub:
    sub x0, lr, #4                ; endereço da instrução `bl` que chamou este stub
    bl rdv_LinkBlock               ; C++, devolve endereço RX pronto pra branch
    br x0
```

`rdv_LinkBlock` (`core/hw/sh4/dyna/driver.cpp:319-393`) recebe o **endereço de retorno
do `bl`** (isto é, o próprio ponto do buffer JIT onde o stub foi chamado) e:
1. Descobre a qual `RuntimeBlockInfo` esse ponto pertence (`bm_GetBlock2`).
2. Resolve o `next_pc` alvo conforme o tipo de bloco (estático/condicional/dinâmico via
   `w1`, passado pelos stubs acima).
3. `rdv_FindOrCompile()` — consulta `fpcb`, compila sob demanda se ainda não existir
   (`rdv_CompilePC`, seção 2).
4. **Se não é MMU nem bloco "stale":** atualiza os ponteiros `pBranchBlock`/`pNextBlock`
   do `RuntimeBlockInfo` de origem, registra a referência cruzada (`AddRef`, usado para
   invalidação em cascata quando um bloco é descartado), e chama
   `rbi->Relink()` — que **regrava o final do bloco de origem**, agora com o ponteiro
   já resolvido, para não passar pelo stub de novo:
   ```cpp
   // core/rec-ARM64/rec_arm64.cpp:2236-2246
   u32 DynaRBI::Relink() {
       Arm64Assembler *compiler = new Arm64Assembler((u8*)this->code + this->relink_offset);
       u32 code_size = compiler->RelinkBlock(this);   // agora pBranchBlock/pNextBlock != NULL -> emite branch direto
       compiler->Finalize(true);                        // true = rewrite in-place
       delete compiler;
       return code_size;
   }
   ```
5. Retorna o endereço RX do bloco de destino já resolvido — o `br x0` do stub salta pra
   lá **desta vez**; da próxima vez que o bloco de origem executar, o link já está
   direto (patch permanente, feito uma única vez).

### 5.4 Miss total — `ngen_FailedToFindBlock_{mmu,nommu}` (`ngen_arm64.S:26-37`)

Quando `fpcb`/dispatcher encontra a sentinela (nenhum link ainda existe e o bloco em si
nunca foi visto): `mov w0, w29 / bl rdv_FailedToFindBlock / br x0` — chama
`rdv_FailedToFindBlock` (`driver.cpp:255-265`, que por sua vez chama `rdv_CompilePC`) e
salta para o código recém-compilado.

---

## 6. Proteção anti-SMC — `CheckBlock`

Todo bloco JIT-compilado, opcionalmente, começa com um preâmbulo que verifica se o
código-fonte SH4 do qual ele foi derivado ainda é idêntico ao que está atualmente na
RAM do guest — proteção contra **self-modifying code** (comum em jogos que descompactam
código/dados em runtime, ou trocam overlays).

```cpp
// core/rec-ARM64/rec_arm64.cpp:1963-2038
void CheckBlock(bool force_checks, RuntimeBlockInfo* block)
{
    if (!mmu_enabled() && !force_checks)
        return;                          // <<-- caminho comum: NO-OP, zero overhead

    Label blockcheck_fail;
    if (mmu_enabled()) { /* compara next_pc salvo vs esperado */ }
    if (force_checks) {
        // compara byte a byte (8/4/2 por vez) o código-fonte SH4 atual em RAM
        // contra uma cópia constante do código no momento da compilação
        // (Ldr x10 do buffer atual vs Ldr x11 = *(u64*)ptr, imediato embutido no código)
    }
    Label blockcheck_success;
    B(&blockcheck_success);
    Bind(&blockcheck_fail);
    Ldr(w0, block->addr);
    TailCallRuntime(ngen_blockcheckfail);   // VIXL nativo -- Mov 64-bit + Br, não GenCallRuntime
    Bind(&blockcheck_success);
    ...
}
```

`force_checks` é `block_check` em `driver.cpp:234`
(`rbi->read_only ? false : IsOnRam(rbi->addr)`) — ou seja, **é ligado por padrão para
todo bloco cujo código-fonte está em RAM gravável** (não ROM/flash), justamente porque
só esse código pode ser modificado em runtime. É por isso que, apesar do "caminho
comum" citado acima ser um no-op quando `!force_checks`, **na prática a maioria dos
blocos de um jogo real passa por esse preâmbulo em toda execução** — só blocos
marcados `read_only` (código conhecido como imutável) o pulam.

Em caso de falha (`ngen_blockcheckfail`, `ngen_arm64.S:39-43`: `bl rdv_BlockCheckFail /
br x0`), o bloco é descartado e recompilado do zero contra o conteúdo atual da RAM
(`driver.cpp:287-308`) — se isso acontece repetidamente para o mesmo endereço
(`blockcheck_failures > 5`), o endereço é marcado como "SMC hotspot"
(`smc_hotspots`), e blocos futuros ali vão para o code cache temporário sem otimização
(`rbi->temp_block`, ver seção 2).

**Resultado do teste A/B desta sessão (item 1.1 de `tech_debits.md`):** forçando
`block_check=false` incondicionalmente em `driver.cpp:234`, `emuThread`/`core_average`
ficaram **idênticos** ao baseline (26,186ms vs ~26ms — diferença dentro do ruído).
**Descartado como causa de lentidão** — o custo do preâmbulo `CheckBlock` (comparação
de poucas palavras de 4/8 bytes, tipicamente 1-3 blocos de 8 bytes por bloco SH4 médio)
é desprezível frente ao custo do corpo do bloco em si. Mudança revertida após o teste
(o código atual tem `CheckBlock` ativo normalmente).

---

## 7. `GenCallRuntime` — chamada de runtime genérica

O mecanismo central usado por praticamente todo caminho "lento" do backend (memória
slow-path, `shop_ifb`, `shop_sync_sr`/`fpscr`, canonical calls `ngen_CC_Call`, `PREF`
sem MMU, dispatcher de interrupção no mainloop):

```cpp
// core/rec-ARM64/rec_arm64.cpp:1477-1488
template <typename R, typename... P>
void GenCallRuntime(R (*function)(P...))
{
    regalloc.PushCallerSaved();
    ptrdiff_t offset = reinterpret_cast<uintptr_t>(function)
                      - reinterpret_cast<uintptr_t>(CC_RW2RX(GetBuffer()->GetStartAddress<void*>()));
    verify(offset >= -128*1024*1024 && offset <= 128*1024*1024);  // alcance do `bl` relativo (26 bits, ±128MB)
    verify((offset & 3) == 0);
    Label function_label;
    BindToOffset(&function_label, offset);
    Bl(&function_label);
    regalloc.PopCallerSaved();
}
```

- **`Bl` relativo, não `Blr` com registrador:** diferente do `CallRuntime`/
  `TailCallRuntime` nativos da VIXL (usados só em `CheckBlock`, seção 6, para os
  caminhos frios de falha), que fazem `Mov` de um endereço absoluto de 64 bits para um
  registrador escrátario e então `Blr`/`Br` (várias instruções, sem limite de alcance),
  `GenCallRuntime` calcula o **offset relativo** entre o endereço da função-alvo e o
  início do buffer JIT (assumindo que o código do runtime C++ está dentro de ±128MB do
  code cache — verificado explicitamente) e emite um único `Bl` — **1 instrução**,
  bem mais barato, mas restrito a esse alcance.
- **`PushCallerSaved()`/`PopCallerSaved()` em volta de TODA chamada** — esse é o
  mecanismo introduzido nesta sessão junto com a extensão do regalloc de FPU (seção
  3.4). Antes da extensão (só `S8`-`S15`, todos callee-saved), `GenCallRuntime` não
  precisava salvar nada — qualquer registrador do pool de alocação já sobrevivia a uma
  chamada de função pela própria ABI. Com `S16`-`S31` no pool, isso deixou de ser
  verdade, e o push/pop explícito ficou necessário para correção — **mas passou a
  custar em toda chamada**, mesmo nas de altíssima frequência.
- **Por que isso é o coração do achado de performance desta sessão (item 4.9):** o
  mainloop chama `UpdateSystem` a cada timeslice (medido em ~7.400×/frame, item 1.2) —
  isso sozinho já é ~7.400 pares de push/pop por frame, mesmo quando nenhum
  `S16`-`S31` está de fato vivo naquele ponto (nesse caso o `vlist` fica vazio e
  `PushCPURegList`/`PopCPURegList` nem são chamados — mas ainda assim há o custo de
  **construir e varrer** o `CPURegList`/iterar `reg_alloced` em `PushCallerSaved`/
  `PopCallerSaved` a cada chamada, ainda que não emita nenhuma instrução ARM64).
  Some a isso o loop de Store Queue (seção 4.6), que chama `GenCallRuntime` uma vez por
  palavra (até 16×/burst) — a combinação explica por que o custo por chamada de
  `GenCallRuntime` importa tanto mais em 2D (blocos pequenos, muitos writes de I/O,
  pouca aritmética 3D para "pagar" o custo com menos spill) do que em 3D.

**`GenCall`/`GenBranchRuntime`/`GenBranch`** (`rec_arm64.cpp:1490-1524`) são variantes
sem push/pop — usadas para chamar código **dentro do próprio dynarec** (o mainloop
gerado, `arm64_intc_sched`/`arm64_no_update`), onde não há regalloc ativo ou onde o
chamado já sabe lidar com quaisquer registradores vivos por convenção própria (o
mainloop salva/restaura seu próprio conjunto fixo de registradores callee-saved no
prólogo/epílogo, `rec_arm64.cpp:1377-1386`/`1449-1458`).

---

## 8. Outras técnicas notáveis e dívidas técnicas observadas

- **`EXPLODE_SPANS` desativado** (`rec_arm64.cpp:31`, comentado por padrão) — existe um
  modo alternativo de regalloc/codegen (visível em vários `#ifdef EXPLODE_SPANS` pelo
  arquivo, ex. `shop_mov64`, `shop_fsca`, `GenReadMemory`/`GenWriteMemory` para size==8)
  que trataria pares de registrador SH4 de 64 bits como dois floats de 32 bits
  "explodidos" independentes no regalloc, em vez de sempre ler/escrever via memória
  (`shil_param_to_host_reg`/`host_reg_to_shil_param` com `x15`/`x1` de 64 bits). Não
  usado neste fork — o comentário em `arm64_regalloc.h:79-81` (`#error EXPLODE_SPANS
  not supported with ssa regalloc`) mostra que esse modo é **incompatível** com o
  regalloc SSA atual (só funcionaria com `OLD_REGALLOC`). Valores de 64 bits (`shop_mov64`,
  `shop_fsca`, leituras/escritas de 8 bytes) sempre passam pela memória de contexto via
  `x15`/`x1`, nunca ficam em par de registradores físicos.
- **`shil_param_to_host_reg`/`host_reg_to_shil_param`** (`rec_arm64.cpp:2040-2095`) são
  o ponto de conversão genérico entre "parâmetro SHIL" (pode ser imediato, registrador
  alocado, ou registrador não-alocado que precisa ir para memória) e "registrador host
  físico específico" — usados extensivamente por `ngen_CC_Call`/`GenReadMemory`/
  `GenWriteMemory`/`shop_mov64`. Centraliza a lógica de "está alocado? em qual
  classe?" que senão se repetiria em cada handler.
- **`sh4_context_mem_operand`** (`rec_arm64.cpp:1085-1090`) limita offsets de contexto a
  16380 bytes (`offset <= 16380`, múltiplo de 4) — o comentário `FIXME 64-bit regs need
  multiple of 8 up to 32760`no próprio código sinaliza uma limitação conhecida e não
  resolvida: o encoding `Ldr`/`Str` de offset imediato do ARM64 tem alcance diferente
  para acesso de 32 vs 64 bits, e este helper não diferencia os dois casos — depende de
  `MemOperand`/VIXL rejeitar (ou a VIXL fazer fallback automático para outro encoding se
  o offset não couber, comportamento não auditado aqui) caso a struct `Sh4Context`
  cresça além desse limite.
- **Risco não confirmado de estouro do orçamento de `EnsureCodeSize`** (seção 4.4) —
  achado desta análise: `GenWriteMemorySlow`/`GenReadMemorySlow`, quando chamados
  *inline* durante compilação normal (não via `ngen_Rewrite`) com `PushCallerSaved`
  não-vazio, poderiam em tese estourar o orçamento fixo de 3 instruções e abortar via
  `verify()`. Não observado como crash real nas condições de teste desta sessão
  (nvmem sempre ligado, MMU completa tipicamente desligada) — registrado como dívida
  técnica a validar antes de mexer nesse caminho de novo.
- **`GenMainloop`** (`rec_arm64.cpp:1339-1470`) gera o mainloop **uma vez por processo**
  (`generate_mainloop()`, guardado por `mainloop != nullptr`) e o salva num ponteiro C
  global (`mainloop`) — todo o dispatch entre blocos e o próprio loop de interrupção
  ficam **dentro do buffer JIT**, não em C++ nativo: `no_update` (dispatcher via `fpcb`),
  `intc_sched` (checagem de timeslice/interrupção, chamado por todo bloco via o
  preâmbulo da seção 2.3), e o prólogo/epílogo de troca de contexto (salva/restaura
  `x19`-`x28`, `s8`-`s15`, `x29`/`x30` — a convenção AAPCS64 completa de callee-saved,
  já que o mainloop é chamado a partir de C++ normal e precisa devolver esses
  registradores intactos ao retornar).
- **`ngen_ResetBlocks`/`restarting`** (`rec_arm64.cpp:91-104`) — quando a configuração
  de MMU muda em runtime, o mainloop precisa ser **regenerado do zero** (o código
  gerado é especializado estaticamente para `mmu_enabled()` em vários pontos — ex.
  contador de ciclo em `cycle_counter` global vs. `w27`). Isso é feito derrubando a CPU
  do loop atual (`CpuRunning=0`) e deixando `ngen_mainloop` perceber e regenerar
  (`generate_mainloop()` de novo) antes de re-entrar.
- **`do_sqw_nommu_area_3`** (`rec_arm64.cpp:2298-2315`) — função `naked` em assembly
  puro embutido via `__asm__` inline (não VIXL), usando `ld2`/`st2` (load/store
  estruturado NEON de 2 registradores de 128 bits) para copiar as duas Store Queues
  (16 bytes cada via `v0`/`v1` como par `D`) direto para a RAM de destino calculada a
  partir do endereço — o caminho **rápido** de `PREF` sem MMU (contraste direto com o
  slow-path por-palavra do item 1.7: o *flush* da SQ já é tão otimizado quanto possível,
  o gargalo está inteiramente no *preenchimento* palavra-a-palavra antes do flush).
- **Sem infraestrutura de profiling própria** (reafirmando o que o `CLAUDE.md` já
  documenta) — este backend não tem nenhum contador/hook de instrumentação embutido;
  toda medição desta sessão veio de `retrorun3 --benchmark`, `perf` externo, ou
  instrumentação manual `chrono` temporária (sempre revertida depois, nunca deixada no
  código de produção — ver "Lição registrada" na seção 4.7).

---

## 9. Referência rápida

**Pools de registrador físico (estado atual):**
- Inteiro: `W19`-`W26` (8, todos callee-saved) — `arm64_regalloc.h:40`.
- Float: `S16`-`S31` (16, caller-saved, com push/pop condicional) + `S8`-`S15` (8,
  callee-saved) = 24 total — `arm64_regalloc.h:41`.

**Cascata de acesso a memória (mais barato → mais caro):**
1. `GenReadMemoryImmediate`/`GenWriteMemoryImmediate` — endereço constante, ponteiro/
   handler resolvido em compile-time.
2. `GenReadMemoryFast`/`GenWriteMemoryFast` — fastmem via `nvmem`, 3 instruções fixas,
   pode faultar → `ngen_Rewrite` reescreve para (3).
3. `GenReadMemorySlow`/`GenWriteMemorySlow` — `GenCallRuntime` para `ReadMemNN`/
   `WriteMemNN`, com push/pop de caller-saved.

**Achados-chave desta sessão (ver `docs/tech_debits.md` para status oficial):**
- Item 1.1 (`CheckBlock` anti-SMC): **descartado** — sem efeito mensurável.
- Item 1.6 (compilação de bloco): **confirmado, impacto pequeno** (~1,2% do tempo).
- Item 1.7 (Store Queue slow-path): **diagnóstico confirmado** (dominante no profile),
  **fix tentado e revertido** (regrediu ~6%).
- Item 4.9 (regalloc FPU estendido): **confirmado** — +5,5% fps em 3D (Shenmue),
  +3,2% `core_average` (pior) em 2D (kofnw). Decisão de manter/reverter/ajustar em
  aberto.
