# Backend JIT x86 (32-bit) — `core/rec-x86/`

> Documentação de referência técnica, gerada por leitura completa do código-fonte em
> `core/rec-x86/` (e da interface com `core/hw/sh4/dyna/`). Não é tutorial — é
> referência densa para reuso por outro engenheiro/IA sem precisar reler o
> código-fonte.
>
> **Contexto importante**: este backend é selecionado por `WITH_DYNAREC=x86`
> (`HOST_CPU=CPU_X86`, ver `Makefile:960-962`) e gera código para x86 **32-bit**
> (modo protegido, sem REX/x64). **Não é o backend usado no device alvo deste
> projeto** (R36 / ARM64 — ver `core/rec-ARM64/`). É o backend usado em builds
> desktop 32-bit (Linux/Windows x86, alguns cores libretro). Ainda assim, é o
> "parente" mais próximo em estilo de código do backend x64
> (`core/rec-x64/rec_x64.cpp`) — mas **não compartilha implementação** com ele: o
> x64 usa uma abordagem completamente diferente (não usa este emissor de
> opcodes/matcher). O emissor aqui tem vestígios de suporte a X64 nunca
> terminado (ver seção 8).

---

## 1. Mapa geral dos arquivos

| Arquivo | Papel |
|---|---|
| `rec_x86_driver.cpp` | Driver principal: `ngen_Compile` (compila um `RuntimeBlockInfo` inteiro), `ngen_init`/`gen_hande` (stubs de acesso a memória compartilhados), `ngen_Rewrite` (handler de patch pós-SIGSEGV), `DynaRBI::Relink` (linking/relinking de blocos), detecção de CPU (SSE/MMX), call-site cache (desativado). |
| `rec_x86_il.cpp` | `ngen_opcode`: o "grande switch" que traduz cada `shop_*` do SHIL/IR para sequências de `x86e->Emit(...)`. Contém `shop_readm`/`shop_writem` (a parte mais crítica), aritmética inteira, FPU escalar/vetorial (SSE), e helpers genéricos (`ngen_Bin`, `ngen_fp_bin`, `ngen_Unary`) e a "convenção de chamada canônica" (`ngen_CC_*`) usada pelo fallback em C. |
| `rec_x86_ngen.h` | Header de interface: declara `x86_reg_alloc` (especialização de `RegAlloc<x86_reg,x86_reg>` do frontend), os externs compartilhados entre `driver.cpp`/`il.cpp` (`x86e`, `cycle_counter`, `loop_no_update`, flags de SIMD, stubs de linking). |
| `x86_emitter.h` / `x86_emitter.cpp` | Camada de emissão: enum `x86_reg` (todas as classes de registrador x86, incluindo campos já preparados para x64 — não usados aqui), `x86_block` (buffer de código + índice + patches + labels), construção de `x86_mrm_t` (ModRM/SIB/disp) a partir de combinações `(base, index, scale, disp)`, aplicação de patches/relocations (`ApplyPatches`, `GetExterns`). |
| `x86_matcher.h` | Conversão de um parâmetro C++ (`x86_reg`, `x86_mrm_t`, `u32` imm, `x86_Label*`, `x86_ptr_imm`) em um `encoded_type` com uma classe de encoding (`pg_R0`, `pg_REG`, `pg_IMM_S8`... `pg_ModRM`), e `Match_opcode`: varre a lista de encodings possíveis de uma `x86_opcode_class` e escolhe a primeira que "contém" os tipos dos parâmetros reais. |
| `x86_op_encoder.h` | Definição dos "modos de encoding" x86 (`enc_param_slash_r`, `enc_param_plus_r`, `enc_param_memrel_32`, etc.) e o encoder genérico `x86_encode_opcode_tmpl<...>` (template C++ instanciado uma vez por combinação de encoding — "compila" a lógica de emissão de bytes em tempo de compilação do emissor). Também define as macros `OP`/`OP_0`/`OP_1_rm`/`OP_2`/`s_r`/`s_d` etc. usadas para descrever cada opcode. |
| `x86_op_classes.h` | Só declara `enum x86_opcode_class { #include "generated_class_names.h" ... op_count };` — o enum de todas as classes de opcode (`op_mov32`, `op_add32`, `op_addss`, ...). |
| `x86_op_table.h` | Monta as duas tabelas finais: `all_opcodes[]` (todas as variantes de encoding de todos os opcodes, `#include "generated_descriptors.h"`) e `x86_opcode_list[op_count+1]` (ponteiro para o primeiro elemento de cada classe dentro de `all_opcodes[]`, `#include "generated_indexes.h"`). |
| `generated_class_names.h` / `generated_class_names_string.h` | Lista simples de identificadores `op_aaa, op_aad, ...` (primeiro) e a mesma lista como strings `"op_aaa", "op_aad", ...` (usada só para debug/disasm via `DissasmClass`). **Gerados automaticamente** — não há gerador visível neste repositório (nem em `tools/` nem em subdiretórios; é um artefato "congelado"). |
| `generated_descriptors.h` | Uma entrada `OP(...)`/`s_LIST_END` por variante de encoding de cada opcode, terminada por `s_LIST_END`. Ex.: `op_add16` tem 5 variantes (imm8 sinalizado, imm16 direto em EAX/AX, imm16 genérico, reg→r/m, r/m→reg) — é basicamente uma tabela extraída do manual de opcodes x86 (estilo Intel SDM). |
| `generated_indexes.h` | Array de ponteiros `&all_opcodes[N]` — um por classe de opcode — usado para indexar diretamente em `all_opcodes` sem fazer busca linear pelo nome da classe. |
| `rec_x86_asm.cpp` | Trivial: calcula `gas_offs`, `cpurun_offset`, `nextpc_offset` (offsets de campos de `Sh4RCB`) para serem consumidos pelo assembly (`.S`) via `extern`. |
| `rec_lin86_asm.S` | Pontos de entrada em assembly GAS (sintaxe Intel) para Linux/ELF: `ngen_mainloop` (laço principal do dynarec), stubs de linking de bloco (`ngen_LinkBlock_*_stub`), `ngen_FailedToFindBlock_`, `ngen_blockcheckfail`/`ngen_blockcheckfail2`. |
| `rec_win86_asm.S` | Mesma coisa que `rec_lin86_asm.S`, mas com underscore-prefixed symbols (convenção COFF/Windows) e pequenas diferenças (ex.: lê `PC` de `[0xA0000000]` fixo em vez de via `p_sh4rcb`+offset — ver seção 8). |

### Como se encaixam

```
core/hw/sh4/dyna/driver.cpp (genérico, todos os backends)
        │  recSh4_Init() -> ngen_init()             [uma vez]
        │  rdv_CompilePC() -> RuntimeBlockInfo::Setup()  (decode SH4 -> SHIL)
        │                   -> ngen_Compile()        [rec_x86_driver.cpp]
        ▼
rec_x86_driver.cpp: ngen_Compile()
        │  reg.DoAlloc(...)              [core/hw/sh4/dyna/regalloc.h, compartilhado]
        │  CheckBlock(...)               [checksum anti-SMC por opcode]
        │  scheduler (cycle_counter)
        │  for each shil_opcode:
        │        ngen_opcode(...)  ──────────────────► rec_x86_il.cpp
        │                                                  │
        │                                                  ▼
        │                                        x86e->Emit(op_xxx, ...)
        │                                                  │
        │                                                  ▼
        │                                     x86_block::Emit (x86_emitter.cpp)
        │                                                  │
        │                                                  ▼
        │                                Match_opcode (x86_matcher.h) percorre
        │                                x86_opcode_list[op_xxx] e escolhe a
        │                                primeira variante compatível
        │                                                  │
        │                                                  ▼
        │                                x86_encode_opcode_tmpl<...> (x86_op_encoder.h)
        │                                escreve bytes reais no buffer
        │  block->Relink()                [emite o "rodapé" de linking do bloco]
        ▼
código x86 executável no CodeCache, chamado a partir de ngen_mainloop (.S)
```

---

## 2. Pipeline de compilação de bloco

Ponto de entrada: `ngen_Compile()` em `core/rec-x86/rec_x86_driver.cpp:270-347`, chamado por
`rdv_CompilePC()` (`core/hw/sh4/dyna/driver.cpp:206-248`) depois que
`RuntimeBlockInfo::Setup()` já rodou `dec_DecodeBlock` (decodificador SH4 → lista de
`shil_opcode`, arquivo `decoder.cpp`, não documentado aqui em detalhe) e
`AnalyseBlock()` (otimizador SSA, `ssa.cpp`).

Passo a passo dentro de `ngen_Compile`:

1. **Setup do emissor** (`rec_x86_driver.cpp:278-285`): cria um `x86_block` temporário
   (`x86e`, variável global do backend) apontando diretamente para o ponteiro de
   escrita atual do code cache (`emit_GetCCPtr()`), com `do_realloc=false` — ou seja,
   **não há realocação**: o buffer é o próprio code cache linear, e o backend
   escreve direto nele. `block->code` é fixado nesse mesmo endereço — é o endereço
   final de execução do bloco, conhecido antes mesmo de gerar o código.

2. **Alocação de registradores** (`rec_x86_driver.cpp:292`): `reg.DoAlloc(block,
   alloc_regs, xmm_alloc_regs)` — roda o algoritmo de linear-scan compartilhado do
   frontend (`core/hw/sh4/dyna/regalloc.h`) sobre a lista *inteira* do bloco antes
   de emitir qualquer instrução. Ver seção 3.

3. **Cabeçalho do bloco** (`rec_x86_driver.cpp:294-317`):
   - `mov ECX, block->addr` seguido de `CheckBlock(...)`: checksum anti-SMC por
     opcode (seção 6).
   - Scheduler de ciclos: `sub cycle_counter, block->guest_cycles; jns no_up; call
     intc_sched; no_up:` — decrementa um contador global de ciclos "gastos" pelo
     bloco; se ficar negativo, chama o handler de interrupções/scheduler antes de
     continuar. Esse é o mecanismo de "fatiamento de tempo" do laço principal
     (`SH4_TIMESLICE 448`, ver seção 5).
   - Contadores de profiling/staging: `block->staging_runs--` (se `staging==true`)
     e `block->runs++` (sempre) — usados pela heurística de otimização em dois
     estágios (ver seção 7, "staging/optimise").

4. **Corpo do bloco** (`rec_x86_driver.cpp:319-332`): itera `block->oplist` (o
   array de `shil_opcode` do IR) em ordem, e para cada opcode:
   ```cpp
   op->host_offs = x86e->x86_indx;   // guarda offset do código gerado (debug/dissasm)
   reg.OpBegin(op, i);               // pode emitir Preload()/Preload_FPU() (mov reg físico <- slot)
   ngen_opcode(block, op, x86e, staging, optimise);   // rec_x86_il.cpp: switch(op->op)
   reg.OpEnd(op);                    // pode emitir Writeback()/Writeback_FPU()
   ```
   `ngen_opcode` é o "switch" gigante em `rec_x86_il.cpp:250-1293` que traduz cada
   `shop_*` (add, sub, fmac, readm, writem, jdyn, ...) numa sequência de
   `x86e->Emit(...)`. Opcodes sem tradução nativa caem no `default:` e são
   despachados para o **fallback canônico em C** via `shil_chf[op->op](op)`
   (`rec_x86_il.cpp:1289-1291`) — ver seção 7.

5. **Rodapé de linking** (`rec_x86_driver.cpp:334-337`):
   ```cpp
   block->relink_offset = x86e->x86_indx;
   block->relink_data = 0;
   x86e->x86_indx += block->Relink();
   ```
   `block->Relink()` (implementado em `DynaRBI::Relink`, `rec_x86_driver.cpp:349-520`)
   gera, **num sub-`x86_block` separado que escreve diretamente no mesmo buffer**
   (a partir de `code + relink_offset`), o trecho de código que decide para onde
   pular ao fim do bloco (chamada de stub de linking, já que `pBranchBlock`/
   `pNextBlock` ainda são nulos na primeira compilação — ver seção 5). O tamanho
   retornado é somado ao cursor do `x86e` externo só para reservar o espaço
   corretamente (o conteúdo já foi escrito pelo sub-bloco).

6. **Finalização** (`rec_x86_driver.cpp:339-346`): `x86e->Generate()` (aplica
   patches de label pendentes — branches internas do bloco), grava
   `host_code_size`/`host_opcodes` no `RuntimeBlockInfo`, `emit_Skip(...)` avança o
   cursor real do code cache pelo tamanho gerado, e destrói o `x86_block`
   temporário.

Note que **todo o processo é single-pass por opcode** (não há um passo de
"scheduling" de instruções x86 nem peephole — a única otimização estrutural vem do
regalloc e de decisões condicionais dentro do próprio `ngen_opcode`, como o
"read-ahead" comentado dentro de `shop_readm`, ver seção 7).

---

## 3. Alocação de registrador

O algoritmo em si **não é específico do x86** — vive em
`core/hw/sh4/dyna/regalloc.h` (`RegAlloc<nreg_t,nregf_t,explode_spans=true>`,
template compartilhado por vários backends) e é parametrizado com `nreg_t =
nregf_t = x86_reg` para este backend (`x86_reg_alloc : RegAlloc<x86_reg,x86_reg>`,
`rec_x86_ngen.h:48-56`).

### Registradores físicos disponíveis (`rec_x86_driver.cpp:95-97`)

```cpp
x86_reg alloc_regs[]     = { EBX, EBP, ESI, EDI, NO_REG };
x86_reg xmm_alloc_regs[] = { XMM7, XMM6, XMM5, XMM4, NO_REG };
f32 DECL_ALIGN(16) thaw_regs[4];
```

- **4 registradores inteiros de 32 bits**: EBX, EBP, ESI, EDI. Note que EAX, ECX,
  EDX e ESP **nunca** entram no pool do regalloc — são reservados como scratch
  fixo: ECX/EDX carregam endereço/valor nas chamadas de leitura/escrita de
  memória (convenção `fastcall`/`DYNACALL`, ver seção 4), EAX é scratch geral e
  valor de retorno de chamadas C, ESP é a pilha real do host.
- Os 4 registradores escolhidos (EBX/EBP/ESI/EDI) são **todos callee-saved** na
  convenção cdecl x86 — ou seja, sobrevivem automaticamente a qualquer `call`
  para uma função C (`ReadMem32`, `WriteMem32`, fallback canônico, etc.) sem
  nenhum código extra do JIT. Essa escolha é deliberada: evita ter que
  salvar/restaurar registradores inteiros ao redor de cada chamada.
- **4 registradores XMM**: XMM7..XMM4 (XMM0..XMM3 ficam de fora — usados como
  scratch pelo próprio codegen, ex. em `shop_fmac`, `shop_ftrv`, no protocolo de
  leitura/escrita de memória float, etc.).
- **Nenhum spill para pilha real do x86**: quando o linear-scan do frontend
  precisa de mais spans simultâneos do que registradores físicos disponíveis
  (`RegAlloc::SplitSpans`, `regalloc.h:927-971`), ele corta o *span* do SHIL em
  dois (preload/writeback intermediários), mas cada span sempre acaba mapeado
  para um registrador físico real (`nreg`/`nregf`) — o "spill" nesse desenho é
  para a memória de contexto do SH4 (`Sh4cntx`, via `GetRegPtr`), não para a
  pilha nativa. `Preload`/`Writeback` (implementados no backend, ver abaixo) é
  exatamente esse mecanismo.

### Ponte RegAlloc ↔ x86

O backend implementa apenas 4 métodos virtuais exigidos pelo template
(`rec_x86_driver.cpp:100-116`):

```cpp
void x86_reg_alloc::Preload(u32 reg, x86_reg nreg)      { x86e->Emit(op_mov32, nreg, GetRegPtr(reg)); }
void x86_reg_alloc::Writeback(u32 reg, x86_reg nreg)     { x86e->Emit(op_mov32, GetRegPtr(reg), nreg); }
void x86_reg_alloc::Preload_FPU(u32 reg, x86_reg nreg)   { x86e->Emit(op_movss, nreg, GetRegPtr(reg)); }
void x86_reg_alloc::Writeback_FPU(u32 reg, x86_reg nreg) { x86e->Emit(op_movss, GetRegPtr(reg), nreg); }
```

`GetRegPtr(reg)` resolve para um endereço dentro do contexto SH4 (`Sh4cntx`) —
i.e., preload é um `mov reg32, [Sh4cntx.r[n]]` e writeback o inverso. Essas
chamadas são inseridas automaticamente pelo `RegAlloc::OpBegin`/`OpEnd`
(`regalloc.h:1035-1079`) no início/fim do *span* de vida de cada registrador
virtual, e é isso que `ngen_Compile` invoca a cada opcode (seção 2, passo 4).

### XMM e chamadas de função: Freeze/Thaw

Diferente dos GPRs escolhidos (callee-saved), **XMM não tem noção de
callee-saved na convenção cdecl usada aqui** — qualquer `call` para uma função C
(inclusive os stubs de leitura/escrita de memória) pode destruir XMM4-7 livremente
do ponto de vista da ABI. Como o regalloc pode ter valores SH4 "vivos" alocados
nesses registradores atravessando a chamada, o backend implementa manualmente
save/restore ao redor de **toda** chamada externa:

```cpp
// rec_x86_driver.cpp:121-146
void x86_reg_alloc::FreezeXMM()
{
    x86_reg* fpreg = xmm_alloc_regs;
    f32* slpc = thaw_regs;
    while (*fpreg != -1)
    {
        if (SpanNRegfIntr(current_opid, *fpreg))   // registrador está "vivo" agora?
            x86e->Emit(op_movss, slpc++, *fpreg);  // salva em thaw_regs[] (estático, 16-byte aligned)
        fpreg++;
    }
}

void x86_reg_alloc::ThawXMM() { /* espelho: recarrega de thaw_regs[] */ }
```

`SpanNRegfIntr(opid, nreg)` (`regalloc.h:997-1007`) conta quantos *spans* ativos
naquele `opid` estão mapeados para aquele registrador físico — ou seja,
`Freeze`/`Thaw` só salvam os XMM que **realmente** têm um valor SH4 vivo
atravessando aquele ponto, não os 4 sempre. `ngen_CC_Call` (chamada canônica,
`rec_x86_il.cpp:184-189`) e o codegen de `shop_readm`/`shop_writem`
(`rec_x86_il.cpp:363-365`, `:535-537`) envolvem toda `call` externa com
`reg.FreezeXMM(); ...; reg.ThawXMM();`. **Nota**: `thaw_regs` é um buffer
**global único de 4 floats** (não uma pilha) — funciona porque o dynarec é
single-threaded e não há chamadas de memória reentrantes/aninhadas dentro do
mesmo opcode, mas é frágil a mudanças futuras que reintroduzam reentrância.

### Particularidades adicionais

- Interseção de spans inteiros vs. float é tratada de forma totalmente
  independente (dois pools de registradores físicos, duas listas de spans) —
  não há conflito de alocação entre GPR e XMM.
  - Registradores vetoriais SH4 (`FMT_V2`/`FMT_V4`/`FMT_V16`, usados por
  `fipr`/`ftrv`/`frswap`/matrizes) são tratados especialmente por
  `RegAlloc::InsertRegs` (`regalloc.h:388-407`): quando `count()==1 ||
  count()>2` o registrador entra "inteiro" na alocação; para `FMT_V2`/`FMT_F64`
  ele é "explodido" em registradores `FMT_F32` individuais (`explode_spans=true`,
  o default do template). Isso explica por que `shop_ftrv`/`shop_fipr` fazem
  `verify(!reg.IsAllocAny(...))` nos operandos vetoriais grandes — vetores
  FV/matrizes XMTRX nunca são mapeados para registrador físico individual, só
  acessados via ponteiro direto (`op->rs1.reg_ptr()`) no contexto.
- Fallback de emergência: se `mapg`/`mapf` são chamados para um `shil_param` que
  não está de fato alocado, o código morre (`die("map must return value")`) — há
  bastante `verify(reg.IsAllocg(...))`/`verify(reg.IsAllocf(...))` espalhado no
  `rec_x86_il.cpp` como assert de sanidade antes de cada tradução.

---

## 4. Acesso a memória (`shop_readm`/`shop_writem`)

Esta é a parte mais elaborada do backend. Existem **três caminhos completamente
diferentes** para uma leitura/escrita de memória gerada pelo JIT, escolhidos em
**dois momentos distintos**: (a) em tempo de compilação do bloco, se o endereço é
imediato conhecido (`op->rs1.is_imm()`); (b) via os **stubs compartilhados**
`mem_code[...]`, se o endereço só é conhecido em runtime; e (c) via *rewrite*
pós-fault, quando o caminho "rápido" de (b) se prova errado para aquele
call-site específico.

### 4.1 Caso endereço imediato conhecido em compile-time

`shop_readm`/`shop_writem` primeiro tentam `_vmem_read_const(addr, isram, size)`
(`rec_x86_il.cpp:270-334` e `:454-489`). Se o endereço cai em RAM (`isram==true`),
o acesso é **inlinado diretamente** como um `mov`/`movsx`/`movss` para/de um
ponteiro absoluto do host — sem nenhuma chamada de função:

```cpp
// rec_x86_il.cpp:278-284 (leitura imediata, tamanho 1/2/4)
if (size==1)      x86e->Emit(op_movsx8to32, EAX, ptr);
else if (size==2) x86e->Emit(op_movsx16to32, EAX, ptr);
else if (size==4) x86e->Emit(op_mov32, EAX, ptr);
```

Se **não** é RAM (registrador de hardware / área mapeada especial), o endereço
vira `mov ECX, imm` e cai no mesmo mecanismo de chamada de função do caso
dinâmico (variável `fuct` guarda o ponteiro da função de acesso resolvida por
`_vmem_read_const`, reaproveitado mais abaixo).

Há também um bloco de otimização especulativa **morto** (`#if 0` /
`OPTIMIZATION_GRAVEYARD`, `rec_x86_il.cpp:285-315`) que tentava usar um
"whitelist" de valores lidos por PC-relative load como se fossem constantes,
comparando o valor atual contra o valor visto em compile-time e só invalidando
(`int3`) se divergisse — comentário do próprio autor: *"this is a pretty good
sieve, but its not perfect [...] Maybe a mix of both?"*. Está desabilitado e não
é chamado (ver seção 8).

### 4.2 Caso dinâmico — stubs compartilhados (`mem_code[3][2][5]`)

Quando o endereço só é conhecido em runtime (registrador SH4, não imediato), o
JIT **não inlina** a sequência de acesso a memória em cada opcode. Em vez
disso, carrega o endereço em **ECX** (mais `rs3`, se houver deslocamento de
base, `rec_x86_il.cpp:336-349`) e faz um `call` para um **stub compartilhado e
pré-compilado uma única vez**, indexado por `mem_code[modo][w][tamanho]`:

```cpp
// rec_x86_driver.cpp:696
void* mem_code[3][2][5];
```

- **Dimensão 1 — `modo` (3 valores)**:
  - `0`: caminho **rápido**. Se `_nvmem_enabled()` (mapeamento de memória virtual
    "nativo": a RAM do SH4 e seus espelhos de 29 bits estão realmente mapeados
    de forma contígua no espaço de endereço do processo host, apontados por
    `virt_ram_base`), o stub faz o acesso **inline, sem nenhuma outra
    chamada**: mascara o endereço (`and ECX, 0x1FFFFFFF`) e acessa
    `[ECX + virt_ram_base]` diretamente. Se `_nvmem_enabled()` for falso em
    runtime, o gerador cai automaticamente no mesmo código do modo `2`
    (fallback "geral" via C) — mas o **próprio corpo do stub `mem_code[0][1][*]`
    é gerado mesmo assim e nunca é usado** para escritas (ver seção 8).
  - `1`: caso **Store Queue** (SQ). Só existe para escrita (`w==1`) de 32/64
    bits (`ngen_init` pula a geração para os outros casos — ver `if (m==1 &&
    (sz<=SZ_16 || w==0)) continue;`, `rec_x86_driver.cpp:724`). Detalhado em
    4.3.
  - `2`: caminho **geral/seguro** — sempre chama a função C real
    (`ReadMem8/16/32/64`, `WriteMem8/16/32/64`, ou as variantes `_vmem_*` se
    `NO_MMU` estiver definido — não é o caso neste fork por padrão, ver
    `core/build.h:118`, comentado). É o único caminho que sabe lidar
    corretamente com MMU, registradores de hardware mapeados em memória (holly,
    PVR, AICA, etc.) e todas as áreas não-RAM.

- **Dimensão 2 — `w` (2 valores)**: 0 = leitura, 1 = escrita.
- **Dimensão 3 — `tamanho`/`Lsz` (5 valores)**: `SZ_8, SZ_16, SZ_32I, SZ_32F,
  SZ_64F` — 32 bits inteiro e 32 bits float são **tamanhos distintos** aqui
  porque o valor entra/sai por um registrador diferente (EAX/EDX vs.
  XMM0/XMM1), mesmo a função C subjacente (`ReadMem32`/`WriteMem32`) sendo a
  mesma para os dois (`rwm[w][sz]` reaproveita o mesmo ponteiro de função para
  `SZ_32I` e `SZ_32F` — `rec_x86_driver.cpp:565-573`).

**Geração dos stubs** — `ngen_init()` (`rec_x86_driver.cpp:703-740`), chamado
**uma única vez**, em `recSh4_Init()` (`core/hw/sh4/dyna/driver.cpp:463`), no
início físico do code cache:

```cpp
void ngen_init(void)
{
    if (mem_code_end != 0) return;      // guarda de idempotência
    ...
    mem_code_base = (size_t)emit_GetCCPtr();
    for (int sz=0; sz<5; sz++)
     for (int w=0; w<2; w++)
      for (int m=0; m<3; m++)
      {
          if (m==1 && (sz<=SZ_16 || w==0)) continue;   // SQ só p/ store 32/64
          mem_code[m][w][sz] = emit_GetCCPtr();
          gen_hande(w, sz, m);
      }
    mem_code_end = (size_t)emit_GetCCPtr();
    x86e->Generate();
    ...
    emit_SetBaseAddr();   // esses bytes nunca são "limpos" por recSh4_ClearCache()
}
```

Como `emit_SetBaseAddr()` marca `LastAddr_min` logo após os stubs, e
`recSh4_ClearCache()` sempre faz `LastAddr = LastAddr_min;`, **os stubs
sobrevivem para sempre** a qualquer reset/limpeza do code cache — ficam
permanentemente no início do buffer executável, e todos os blocos compilados
depois (mesmo após N limpezas de cache) continuam apontando para os mesmos
endereços de stub. `ngen_ResetBlocks()` (`rec_x86_driver.cpp:698-701`, chamado
por `bm_ResetCache()` a cada limpeza de cache) apenas zera a variável estática
`mem_code_end` — mas como **nada mais no fluxo normal chama `ngen_init()` de
novo** depois do boot (única chamada em `driver.cpp:463`), esse reset é hoje
vestigial/sem efeito (ver seção 8).

**`gen_hande(w, sz, mode)`** (`rec_x86_driver.cpp:560-692`) é o gerador do corpo
de cada stub. Estrutura (o parâmetro de endereço sempre chega em **ECX**, por
convenção do call-site em `rec_x86_il.cpp`):

```cpp
// modo 0 + nvmem: acesso direto, sem chamada de função (rec_x86_driver.cpp:584-615)
x86e->Emit(op_mov32, EAX, ECX);
x86e->Emit(op_and32, ECX, 0x1FFFFFFF);
x86_mrm_t buff = x86_mrm(ECX, virt_ram_base);
// leitura: mov AL/AX/EAX, [ECX+virt_ram_base]   (ou movss XMM0 para float)
// escrita: mov [ECX+virt_ram_base], DL/DX/EDX   (ou movss ..., XMM0)
```

Note o `mov EAX, ECX` **antes** de mascarar — o valor original (não mascarado)
fica preservado em EAX propositalmente, para ser usado pelo mecanismo de
*rewrite* (4.4) caso esse acesso "rápido" na verdade page-faulte.

```cpp
// modo == 1 (Store Queue), rec_x86_driver.cpp:616-642
verify(w==1);
x86e->Emit(op_mov32, EAX, ECX);
x86e->Emit(op_and32, ECX, 0x3f);        // offset dentro dos 64 bytes da SQ
x86e->Emit(op_shr32, EAX, 26);
x86e->Emit(op_cmp32, EAX, 0x38);        // confirma área 0x38xxxxxx (P4 store queue)
... op_je l; op_int3; MarkLabel(l);     // trap se a área não bater (não deveria acontecer)
x86e->Emit(op_mov32, x86_mrm(ECX, sq_both), EDX);    // grava direto no buffer de SQ do contexto
```

```cpp
// modo == 2 (geral), rec_x86_driver.cpp:643-691
// alinhamento de pilha (16 bytes) em não-Windows; empacota XMM0/XMM1 em ESP
// se for escrita float; chama rwm[w][sz] (ReadMem*/WriteMem*); desempacota
// resultado de EAX/EDX para XMM0/XMM1 se for leitura float; restaura ESP.
```

**Onde o SHIL efetivamente chama cada modo**: interessante notar uma
**assimetria deliberada** entre leitura e escrita:

```cpp
// rec_x86_il.cpp:364 — shop_readm dinâmico: sempre modo 0 (rápido, com fallback interno a nvmem-off)
x86e->Emit(op_call, x86_ptr_imm(mem_code[0][0][Lsz]));

// rec_x86_il.cpp:536 — shop_writem dinâmico: sempre modo 2 (geral/seguro)
x86e->Emit(op_call, x86_ptr_imm(mem_code[2][1][Lsz]));
```

Ou seja: **leituras dinâmicas sempre tentam o caminho rápido primeiro** (e
dependem do mecanismo de *rewrite* em 4.4 para se corrigir caso o endereço não
seja RAM); **escritas dinâmicas sempre vão direto pelo caminho seguro/geral**,
nunca tentam o `mem_code[0][1][*]` "rápido" — apesar desse stub existir e ser
gerado por `ngen_init`. Isso implica que **os stubs `mem_code[0][1][*]` (escrita
rápida via nvmem) são código morto/nunca referenciado pelo tradutor SHIL** — ver
seção 8.

### 4.3 Store Queue como caso especial (`mode==1`)

A Store Queue do SH4 (área `0xE0000000`-`0xE3FFFFFF`/física `0x38xxxxxx`) é um
buffer de 2×32 bytes mapeado no contexto (`sq_both`, macro para
`sh4rcb.sq_buffer`, `core/hw/sh4/sh4_mmr.h:13`) que só é **descarregado** para a
RAM de verdade quando o software SH4 executa a instrução `pref` sobre aquele
endereço (ver `shop_pref`, `rec_x86_il.cpp:1159-1190`, que chama
`do_sqw_mmu`/`do_sqw_nommu`). Por isso o *store* na própria SQ é tratado à parte
de todo o mecanismo de leitura/escrita "normal": não faz sentido rotear pela
função `WriteMem32` genérica (que trataria como MMIO/RAM comum), nem pelo
caminho nvmem direto (a SQ não é RAM espelhada em `virt_ram_base`). O stub de
modo 1 apenas mascara os 6 bits baixos do endereço (`& 0x3f`, offset dentro dos
64 bytes das duas SQs) e grava direto no buffer do contexto — o mais barato
possível, já que escritas em SQ são extremamente frequentes em código de
transformação/transferência em massa (ex. envio de listas de polígonos para a
GPU). O `int3` no caminho de "área errada" é uma proteção de depuração: o
tradutor SHIL só deveria gerar chamadas a esse stub quando já sabe (por
contexto de decodificação) que o acesso é de fato a uma SQ.

### 4.4 `ngen_Rewrite` — correção pós-fault do caminho rápido

O mecanismo mais sutil do backend. Quando o modo 0 (nvmem) tenta acessar um
endereço que **não** está de fato espelhado como RAM real no processo (ex.:
registrador de hardware, VRAM write-protegida, endereço fora dos mirrors
mapeados), a instrução `mov`/`movss` dentro do stub `mem_code[0][w][sz]` gera um
**page fault real do host** (SIGSEGV/`EXCEPTION_ACCESS_VIOLATION`). Isso é
capturado pelo handler de sinal específico da plataforma
(`core/libretro/common.cpp`, função `signal_handler`/`ExceptionHandler`), que
tenta, em ordem: `bm_RamWriteAccess` (escrita numa página RAM protegida por
anti-SMC — seção 6), `VramLockedWrite`, `BM_LockedWrite`, e por fim, se nada
bateu:

```cpp
// core/libretro/common.cpp:334-342 (caminho Linux/Unix, HOST_CPU==CPU_X86)
if (ngen_Rewrite((size_t&)ctx.pc, *(size_t*)ctx.esp, ctx.eax))
{
    ctx.esp += 4;           // desfaz o push do "call" que já ocorreu (será refeito)
    ctx.ecx = ctx.eax;      // restaura ECX = endereço original completo (não mascarado)
    context_to_segfault(&ctx, segfault_ctx);
}
```

`ngen_Rewrite` (`rec_x86_driver.cpp:742-795`) recebe:
- `addr` (= `ctx.pc`, o EIP no momento do fault — está **dentro** de um dos
  stubs `mem_code[0][w][sz]`, já que é lá que a instrução de acesso direto
  mora);
- `retadr` (= topo da pilha no momento do fault = endereço de retorno do
  `call mem_code[0][w][sz]` que o bloco SH4 tinha acabado de executar);
- `acc` (= EAX no momento do fault = o endereço **original, não mascarado**,
  porque `gen_hande` sempre faz `mov EAX, ECX` antes do `and` — ver 4.2).

```cpp
bool ngen_Rewrite(size_t& addr, size_t retadr, size_t acc)
{
    if (addr >= mem_code_base && addr < mem_code_end)   // faultou dentro de um stub?
    {
        u32 ca = *(u32*)(retadr-4) + retadr;   // recupera o alvo do "call rel32" original
        // aponta um x86_block temporário para o próprio call-site (retadr-5,
        // 5 = tamanho do opcode `call rel32`), dentro do BLOCO SH4 que chamou:
        x86e->x86_buff = (u8*)retadr - 5;
        for (int i=0;i<5;i++) for (int w=0;w<2;w++)
        {
            if ((u32)mem_code[0][w][i] == ca)     // achou qual stub "rápido" foi chamado
            {
                if ((acc>>26) == 0x38)            // endereço é área de Store Queue?
                    x86e->Emit(op_call, x86_ptr_imm(mem_code[1][w][i]));  // repatcha p/ SQ
                else
                    x86e->Emit(op_call, x86_ptr_imm(mem_code[2][w][i]));  // repatcha p/ geral
                x86e->Generate();
                addr = retadr - 5;   // reexecuta a partir do call (agora corrigido)
                return true;
            }
        }
        die("Failed to match the code :(\n");
    }
    return false;
}
```

Ou seja: **o call-site específico dentro daquele bloco compilado é reescrito
permanentemente** (self-modifying code sobre o *próprio* código do dynarec) para
apontar para o stub seguro (modo 2) ou o stub de SQ (modo 1), dependendo do tipo
de endereço que causou a falha — e a execução é retomada bem no início daquele
`call` (que agora vai pro destino corrigido). Isso significa que **o primeiro
acesso "errado" custa uma falha de página completa**, mas todos os acessos
subsequentes àquele mesmo call-site (mesma instrução SH4, dentro daquele bloco
compilado) já usam o caminho correto direto — é uma forma de **cache de
polimorfismo inline de 1 nível, por instrução, auto-corretiva**, sem custo
nenhum em runtime além do primeiro erro. Isso é o que torna o modo 0
(`_nvmem_enabled()`) seguro para ser tentado sempre "otimisticamente" nas
leituras dinâmicas (seção 4.2) mesmo sem o compilador saber estaticamente se o
endereço será RAM ou hardware.

**Diferença fundamental frente a "inlinar tudo por instrução"**: se cada
`shop_readm`/`shop_writem` emitisse seu próprio código de decisão RAM-vs-MMIO
inline (branch + chamada opcional), (a) o código gerado por bloco seria maior
(pior para a I-cache — relevante no Cortex-A53 alvo, mas esse backend é x86);
(b) qualquer patch de correção teria que reconstruir a lógica por opcode; (c)
não haveria um ponto único para trocar a estratégia global (nvmem
ligado/desligado) sem recompilar todo bloco já gerado. Com stubs
compartilhados, **um único bloco de código** (gerado uma vez) é reusado por
milhares de instruções de load/store em todos os blocos compilados depois —
tanto o `call` de 5 bytes quanto a lógica dentro do stub são muito menores que
replicar a decisão em cada site, e o *rewrite* funciona uniformemente porque só
há 2×5 endereços de stub "rápido" possíveis para comparar (o loop em
`ngen_Rewrite`).

---

## 5. Linking/dispatch de blocos

### Laço principal (`ngen_mainloop`, `rec_lin86_asm.S:82-144`)

```asm
ngen_mainloop:
    push esi/esi/esi/esi/edi/ebp/ebx      ; alinhamento 16B + salva callee-saved
    mov ecx, [p_sh4rcb + nextpc_offset]   ; ecx = pc inicial
    mov cycle_counter, 448                ; SH4_TIMESLICE
    lea eax, no_update
    mov loop_no_update, eax               ; guarda endereço de "no_update" p/ os blocos usarem
    lea eax, intc_sched_offs
    mov intc_sched, eax
no_update:
    mov esi, ecx           ; esi preserva o "próximo pc" através da chamada
    call bm_GetCodeByVAddr ; eax = ponteiro executável do bloco (ou stub de "não encontrado")
    jmp eax                 ; salta pro bloco

intc_sched_offs:
    add cycle_counter, 448
    call UpdateSystem       ; processa scheduler/temporizadores
    cmp eax, 0
    jnz do_iter
    ret                      ; sinaliza "pare de rodar"
do_iter:
    pop ecx
    call rdv_DoInterrupts
    mov ecx, eax
    cmp [p_sh4rcb + cpurun_offset], 0
    jz cleanup
    jmp no_update
```

`loop_no_update`/`intc_sched` são ponteiros globais (`extern void*
loop_no_update; extern void* intc_sched;`, `rec_x86_driver.cpp:36-37`)
resolvidos **em runtime** para dentro do próprio `ngen_mainloop`, e usados pelos
blocos compilados (`x86e->Emit(op_jmp, x86_ptr_imm(loop_no_update));`) para
"voltar ao trampolim" sem re-executar o prólogo do laço inteiro. Todo bloco
compilado espera encontrar o **próximo PC em ECX** ao terminar (convenção fixa),
que é exatamente o que `no_update:` consome via `mov esi, ecx`.

### `bm_GetCodeByVAddr`/`bm_GetCode` — como um bloco é localizado

(`core/hw/sh4/dyna/blockmanager.cpp:44-108`)

```cpp
#define FPCA(x) ((DynarecCodeEntryPtr&)sh4rcb.fpcb[(x>>1)&FPCB_MASK])
static DynarecCodeEntryPtr bm_GetCode(u32 addr) { return FPCA(addr); }
```

`fpcb` é uma tabela plana indexada por `(addr>>1) & FPCB_MASK` — **uma tabela
de dispatch direta por endereço físico**, sem hashing nem busca: cada entrada
guarda diretamente o ponteiro de código executável daquele endereço (ou
`ngen_FailedToFindBlock` se ainda não compilado — `bm_vmem_pagefill`,
`blockmanager.cpp:227-233`, preenche a tabela inteira com esse valor sentinela
na inicialização). `bm_GetCodeByVAddr` só adiciona a etapa de tradução MMU
(quando ativa) por cima de `bm_GetCode`. Esse desenho (tabela de ponteiros
direta indexada por PC) é o que permite ao stub em assembly (`call
bm_GetCodeByVAddr; jmp eax`) resolver e desviar para qualquer bloco em O(1),
sem envolver nenhum código C++ pesado no caminho comum (uma vez que o bloco já
existe).

### Linking direto: patch do "rodapé" de cada bloco

Todo bloco termina com um trecho gerado por `DynaRBI::Relink()`
(`rec_x86_driver.cpp:349-520`) que depende do `BlockType` decodificado
(`BET_StaticJump`, `BET_Cond_0/1`, `BET_DynamicJump`, `BET_StaticCall`, etc. —
ver `core/hw/sh4/dyna/decoder.h:8-31`). Na **primeira compilação**,
`pBranchBlock`/`pNextBlock` ainda são nulos (o bloco alvo pode nem existir
ainda), então o rodapé emitido é uma **chamada para um stub de linking**:

```cpp
// caso BET_StaticJump/BET_StaticCall (rec_x86_driver.cpp:485-493)
if (pBranchBlock)
    x86e->Emit(op_jmp, x86_ptr_imm(pBranchBlock->code));       // já linkado: jmp direto
else
    x86e->Emit(op_call, x86_ptr_imm(ngen_LinkBlock_Generic_stub)); // ainda não: stub
```

Os stubs (`rec_lin86_asm.S:20-63`) são pequenos trampolins que descobrem, a
partir do próprio endereço de retorno da `call`, qual instrução precisa ser
re-patchada:

```asm
ngen_LinkBlock_Shared_stub:
    pop ecx          ; ecx = endereço de retorno = logo após o "call" no bloco
    sub ecx, 5        ; ecx = endereço do próprio opcode "call" (5 bytes: E8 rel32)
    call rdv_LinkBlock  ; (ecx=code, edx=dpc) -> eax = ponteiro executável do alvo
    jmp eax             ; salta pro alvo IMEDIATAMENTE (não espera o próximo laço)

ngen_LinkBlock_cond_Next_stub:  mov edx, 0; jmp ngen_LinkBlock_Shared_stub
ngen_LinkBlock_cond_Branch_stub: mov edx, 1; jmp ngen_LinkBlock_Shared_stub
ngen_LinkBlock_Generic_stub:
    mov edx, [p_sh4rcb + gas_offs]   ; edx = Sh4cntx.jdyn (alvo dinâmico calculado pelo bloco)
    jmp ngen_LinkBlock_Shared_stub
```

`rdv_LinkBlock` (`core/hw/sh4/dyna/driver.cpp:319-394`) resolve `next_pc` a
partir do `BlockType`/`dpc`, garante que o bloco alvo existe (compila se
necessário, via `rdv_FindOrCompile`), registra a referência cruzada
(`pBranchBlock`/`pNextBlock`, `AddRef`/`RemRef` para o grafo de predecessores
usado em invalidação), e então **repatcha o próprio rodapé do bloco de
origem**, chamando de novo `rbi->Relink()` — só que dessa vez com
`pBranchBlock`/`pNextBlock` já preenchidos, então o código gerado agora é um
`jmp` **direto** para o endereço do bloco alvo, sem stub:

```cpp
// driver.cpp:384-386
u32 ncs = rbi->relink_offset + rbi->Relink();
verify(rbi->host_code_size >= ncs);
rbi->host_code_size = ncs;
```

Isso é escrita direta sobre código já em execução (o `call` que trouxe a
execução até aqui está sendo substituído por um `jmp`, no mesmo slot de bytes,
reservado com folga — `x86_size=512` no `x86_block` temporário criado dentro de
`Relink()`). Da próxima vez que esse bloco for executado e alcançar esse mesmo
ponto, o salto já é direto — **sem nenhuma chamada a `bm_GetCodeByVAddr` nem a
`rdv_LinkBlock`**, e sem sequer passar pelo trampolim `ngen_mainloop`. Isso é o
clássico "block chaining"/"direct linking" de dynarecs: o primeiro salto entre
dois blocos paga o custo de lookup uma vez; todos os subsequentes são
literalmente um `jmp rel32` nativo.

Blocos condicionais (`BET_Cond_0/1`) têm **dois** rodapés de linking
independentes — um para o caminho "branch tomado" e outro para "não tomado" —
cada um podendo estar linkado ou não independentemente
(`ngen_LinkBlock_cond_Next_stub`/`ngen_LinkBlock_cond_Branch_stub`, que só
diferem em `edx=0`/`edx=1` para indicar ao `rdv_LinkBlock` qual dos dois ramos
resolver).

Blocos dinâmicos (`BET_DynamicJump/Call/Ret`) não têm alvo fixo — o PC alvo é
computado em runtime e colocado em `Sh4cntx.jdyn` pelo próprio opcode
`shop_jdyn` (`rec_x86_il.cpp:568-581`). Mesmo assim, há uma tentativa de
"cache de 1 entrada" *inline*: se já existe `pBranchBlock` de uma execução
anterior, o código gerado compara o PC dinâmico atual contra aquele endereço
conhecido (`cmp GetRegPtr(reg_pc_dyn), pBranchBlock->addr; je pBranchBlock->code`)
e só cai no stub genérico se divergir — uma forma barata de *inline caching*
para alvos dinâmicos que na prática quase sempre saltam para o mesmo lugar
(loops via `jmp`/`bsr` calculado, tabelas de despacho, etc.):

```cpp
// rec_x86_driver.cpp:460-475
if (pBranchBlock)
{
    x86e->Emit(op_cmp32, GetRegPtr(reg_pc_dyn), pBranchBlock->addr);
    x86e->Emit(op_je, x86_ptr_imm(pBranchBlock->code));
    x86e->Emit(op_call, x86_ptr_imm(ngen_LinkBlock_Generic_stub));
}
```

### Cache de call-site (retorno de função) — implementado, porém **desativado**

`rec_x86_driver.cpp:76-235` implementa um mecanismo bastante elaborado de
**call-site cache** (`csc_push`/`csc_pop`/`csc_fail`, array `csc[64]`) — a ideia
é: ao executar um `BET_*Call`, empilhar o endereço de retorno esperado num
pequeno array (`csc_push`, indexado por hash do PC ou por um índice
rotativo/`csc_sidx` conforme `csc_mode`); ao executar o `BET_DynamicRet`
correspondente, comparar o PC de retorno real contra o topo esperado
(`csc_pop`) e, se bater, pular direto pro bloco de retorno sem passar pelo
`ngen_LinkBlock_Generic_stub`. Há inclusive contadores de estatística
(`ret_hit`, `ret_all`, `ret_stc`) e um cache "estático" de 1 entrada por bloco
(`block->csc_RetCache`). **Porém**, as duas únicas chamadas desse mecanismo
estão comentadas em `DynaRBI::Relink`:

```cpp
// rec_x86_driver.cpp:420-423 e :454-457
if (BlockType == BET_StaticCall || BlockType == BET_DynamicCall)
{
    //csc_push(this);
}
case BET_DynamicRet:
{
    //csc_pop(this);
}
```

Ou seja: hoje, `BET_DynamicRet` é tratado exatamente como qualquer outro salto
dinâmico (cai no `ngen_LinkBlock_Generic_stub`/cache de 1 entrada genérico
descrito acima) — todo o mecanismo de call-site cache é **código morto
alcançável apenas se alguém descomentar essas duas linhas**. Ver seção 8.

---

## 6. Proteção anti-SMC / invalidação de bloco

Duas camadas independentes e redundantes, ambas ativas ao mesmo tempo para
blocos residentes em RAM:

### 6.1 Proteção de página do host (mecanismo primário, quando nvmem ativo)

Ao terminar de compilar um bloco, `RuntimeBlockInfo::SetProtectedFlags()`
(`core/hw/sh4/dyna/blockmanager.cpp:569-598`) decide `read_only`:

- Blocos fora de RAM (ROM/BIOS/IP.BIN em `0x0c000000`) **nunca** são protegidos
  (`read_only=false` sempre — não faz sentido proteger memória que o jogo não
  pode escrever de qualquer forma nesse fork/hardware).
- Blocos em RAM só viram `read_only=true` se **nenhuma** página que o bloco
  ocupa já estiver marcada em `unprotected_pages[]` (heurística: uma vez que
  uma página apanhou uma escrita, ela fica "desistida" — todo bloco futuro
  compilado nela nasce já `read_only=false`, para não pagar o custo de
  proteger/desproteger repetidamente uma página "quente" de SMC).
- Se `read_only=true`, a página real do host é colocada em modo
  **somente-leitura** via `mprotect` (`bm_LockPage`, `blockmanager.cpp:270-288`,
  chama `mem_region_lock` sobre os *mirrors* relevantes de `virt_ram_base`), e
  o bloco é registrado em `blocks_per_page[pagina]` (um `std::set` por página
  física de RAM) para permitir localizar rapidamente todos os blocos afetados
  quando aquela página apanhar uma escrita.

Quando o jogo de fato escreve numa página protegida, o host gera um SIGSEGV; o
handler de sinal (`core/libretro/common.cpp`) chama `bm_RamWriteAccess(addr)`
(`blockmanager.cpp:600-620`):

```cpp
void bm_RamWriteAccess(u32 addr)
{
    addr &= RAM_MASK;
    unprotected_pages[addr / PAGE_SIZE] = true;   // marca a página como "suja" p/ sempre
    bm_UnlockPage(addr);                           // mprotect de volta pra RW
    for (auto& block : blocks_per_page[addr / PAGE_SIZE])
        bm_DiscardBlock(block);                    // invalida TODOS os blocos daquela página
}
```

A escrita então é **refeita** (o handler retorna `EXCEPTION_CONTINUE_EXECUTION`
sem alterar a instrução) — o segundo attempt já encontra a página destravada e
segue normalmente. Todos os blocos que ocupavam aquela página são descartados
de uma vez (`bm_DiscardBlock`), forçando recompilação na próxima execução.

### 6.2 Checksum por instrução no cabeçalho do bloco (mecanismo secundário/paranóico)

Independente de `read_only`, **todo bloco** paga um checksum inline no seu
próprio cabeçalho — `CheckBlock()` (`rec_x86_driver.cpp:249-268`), chamado
incondicionalmente logo no início de `ngen_Compile` (`rec_x86_driver.cpp:299`):

```cpp
void CheckBlock(RuntimeBlockInfo* block, x86_ptr_imm place)
{
    s32 sz = block->sh4_code_size;
    u32 sa = block->addr;
    while (sz > 0)
    {
        void* ptr = (void*)GetMemPtr(sa, 4);
        if (ptr)
        {
            if (sz == 2) x86e->Emit(op_cmp16, ptr, *(u16*)ptr);
            else         x86e->Emit(op_cmp32, ptr, *(u32*)ptr);   // *(u32*)ptr é lido AGORA (compile-time!)
            x86e->Emit(op_jne, place);
        }
        sz -= 4; sa += 4;
    }
}
```

Note o `*(u32*)ptr` — o valor comparado é um **snapshot tirado no instante da
compilação**, embutido como imediato na instrução `cmp`. Ou seja: em runtime,
sempre que o bloco é executado, ele compara os bytes SH4 atuais contra o
snapshot que tinha em compile-time — **um `cmp` (imm32) + `jne` por palavra de
32 bits do bloco, toda vez que o bloco roda**, independente de já estar
protegido por página. É uma segunda linha de defesa contra SMC (cobre casos
onde a proteção de página não é aplicável ou não pôde ser usada), mas também
**custo puro em runtime para blocos que já são protegidos por página** (onde
teoricamente nunca deveria disparar).

O alvo do `jne` diferencia exatamente esse caso:

```cpp
// rec_x86_driver.cpp:297-299
x86e->Emit(op_mov32, ECX, block->addr);
CheckBlock(block, force_checks ? x86_ptr_imm(ngen_blockcheckfail)
                                : x86_ptr_imm(ngen_blockcheckfail2));
```

`force_checks` vem de `rdv_CompilePC`:
```cpp
// core/hw/sh4/dyna/driver.cpp:234-235
bool block_check = rbi->read_only ? false : IsOnRam(rbi->addr);
ngen_Compile(rbi, block_check, ...);
```

- **`read_only == true`** (protegido por página) → `force_checks=false` →
  `ngen_blockcheckfail2` (`rec_lin86_asm.S:159-165`), que executa **`int3`
  antes** de chamar `rdv_BlockCheckFail` — ou seja, é tratado como "isso não
  deveria nunca disparar" (trap de depuração/assert), já que a proteção de
  página deveria ter invalidado o bloco antes de qualquer escrita chegar até
  aqui.
- **`read_only == false` e está em RAM** → `force_checks=true` →
  `ngen_blockcheckfail` (sem `int3`) — aqui o checksum **é** o mecanismo real de
  detecção (cobre exatamente os casos onde a página não está protegida: páginas
  "quentes" já desistidas em 6.1, MMU habilitada, etc.).
- Fora de RAM (ROM) → `IsOnRam==false` → também `force_checks=false`, mas
  nesse caso `CheckBlock` normalmente não emite nada útil (`GetMemPtr` tende a
  não achar ponteiro de RAM direto para ROM não espelhada, então o `if (ptr)`
  pula).

`rdv_BlockCheckFail` (`core/hw/sh4/dyna/driver.cpp:287-308`) descarta o bloco
(`bm_DiscardBlock`) e recompila; com MMU habilitada, também rastreia
`blockcheck_failures` por bloco e, acima de 5 falhas repetidas, marca o endereço
como **hotspot de SMC** (`smc_hotspots`, um `std::unordered_set<u32>`) — blocos
futuros compilados naquele endereço então são direcionados para o **temp code
cache** (`TempCodeCache`, buffer separado de 1MB, só existe quando `!NO_MMU`) em
vez do cache principal, uma área que é limpa com muito mais frequência e
independentemente do cache principal (`clear_temp_cache`) — isolando o custo de
recompilação repetida de código "notoriamente instável" (ex. trampolins de
JIT-de-JIT ou stubs auto-modificáveis de jogos) sem forçar limpeza do cache
principal inteiro.

---

## 7. Otimizações e técnicas notáveis

- **Stubs de memória compartilhados + auto-correção via fault (seção 4)** — a
  técnica mais sofisticada do backend: código gerado uma única vez, reusado por
  todos os blocos, com correção per-call-site via `ngen_Rewrite` em vez de
  branch condicional inline. Ver seção 4.4.
- **Direct block chaining com repatch em runtime (seção 5)** — rodapé de bloco
  trocado de "call para stub de resolução" para "jmp direto" assim que o alvo é
  conhecido, eliminando indireção repetida.
- **Callee-saved GPRs escolhidos deliberadamente** (EBX/EBP/ESI/EDI) para que
  chamadas a funções C não exijam nenhum save/restore de inteiros — só XMM
  precisa do mecanismo manual `FreezeXMM`/`ThawXMM` (seção 3), e mesmo esse só
  salva os registradores que estão de fato "vivos" no ponto de chamada
  (`SpanNRegfIntr`), não os 4 sempre.
- **Sistema de matching de opcode por "contenção de conjunto"** (`x86_matcher.h`,
  `ENC_PARAM_CONTAINS`) — cada operando real é classificado no menor
  "grupo/threshold" que o descreve (ex. um imediato que cabe em `s8` vira
  `pg_IMM_S8`), e a tabela de variantes de cada opcode (`generated_descriptors.h`)
  é ordenada da forma de encoding mais compacta para a mais genérica; o primeiro
  `match` na varredura linear é sempre a codificação x86 mais curta possível
  para aqueles operandos (ex.: `add reg, imm8` em vez de `add reg, imm32`
  quando o imediato cabe em 8 bits com sinal) — economia de tamanho de código
  automática, sem lógica explícita no `rec_x86_il.cpp`.
- **Renomeação implícita de operando em operações binárias** (`ngen_Bin`,
  `rec_x86_il.cpp:8-46`): quando `rs2` está mapeado para o mesmo registrador
  físico que `rd` (que vai ser sobrescrito), o valor de `rs2` é primeiro salvo
  em EAX antes do `mov rd, rs1` — evita ler um operando já corrompido. O mesmo
  padrão aparece em `shop_adc` (troca os operandos em vez de copiar, quando
  possível) e em `ngen_fp_bin`.
- **"Staging" de blocos com contagem de execuções** (`staging_runs`,
  `block->runs`) — `ngen_Compile` recebe flags `staging`/`optimise`
  independentes; blocos "staging" são compilados de forma mais barata/menos
  otimizada primeiro e reavaliados, e só requalificam para compilação otimizada
  depois de um número de execuções (`rdv_CompilePC`:
  `rbi->staging_runs=do_opts?100:-100;`) — uma forma simples de compilação em
  camadas (tiered compilation), embora bem mais rudimentar que JITs modernos.
- **`shop_fseteq` trata NaN corretamente via flags da FPU** (`rec_x86_il.cpp:1132-1157`):
  usa `ucomiss` + `lahf` + `test AH,0x44` + `setnp` para tratar
  especificamente o caso "unordered" (NaN) na igualdade de ponto flutuante —
  comentário no código: *"We want to take in account the 'unordered' case on
  the fpu"*.
- **`shop_ftrv`/`shop_fipr` com dois caminhos conforme SSE3** (`sse_3` detectado
  em runtime via `DetectCpuFeatures`): usa `haddps` (SSE3) quando disponível, ou
  uma sequência `movhlps`/`shufps`/`addps` mais longa em CPUs sem SSE3 — trade-off
  runtime detectado uma vez no início da execução.
- **`shop_cvt_f2i_t` com clamp explícito de overflow** (`rec_x86_il.cpp:1210-1222`):
  `cvttss2si` seguido de `cmp`+`cmovge` para saturar em `0x7fffffff` quando o
  float de entrada excede o range representável — replica o comportamento
  definido do SH4 (que a instrução SSE crua não replica sozinha).
- **Convenção de chamada canônica genérica** (`ngen_CC_Start/Param/Call/Finish`,
  `rec_x86_il.cpp:100-193`) — usada pelo fallback em C (`shil_chf[]`, gerado a
  partir de `core/hw/sh4/dyna/shil_canonical.h` com `SHIL_MODE=1`/`3`) para
  qualquer `shop_*` que não tenha tradução nativa no switch de
  `rec_x86_il.cpp`. Empilha argumentos (`push`, incluindo valores XMM via
  `sub esp,4; movss [esp],xmm`), chama a função C real (`ngen_CC_Call`, que
  também faz `FreezeXMM`/`ThawXMM` ao redor), e recupera o retorno de
  EAX/EDX/ST(0) dependendo do `CanonicalParamType`. É o mesmo padrão de "menor
  esforço para cobertura completa do IR" que outros backends usam — qualquer
  opcode SHIL passa a funcionar (ainda que lentamente) assim que existe sua
  versão canônica em C, sem exigir codegen nativo imediato.

---

## 8. Limitações / dívidas técnicas observadas

- **Backend não usado no hardware alvo deste projeto.** `WITH_DYNAREC=x86` gera
  código x86 32-bit; o device R36/ARM64 usa `core/rec-ARM64/`. Esta
  documentação é útil como referência de desenho geral e para comparação com o
  backend ARM64 (ver resumo final), não como alvo de otimização direta do
  projeto de performance descrito em `CLAUDE.md`.
- **Cache de call-site de retorno (`csc_push`/`csc_pop`) implementado por
  inteiro, porém desativado** (`rec_x86_driver.cpp:422`, `:456`, chamadas
  comentadas). Todo o código de suporte (`csc[64]`, `csc_hash`, `csc_fail`,
  contadores `ret_hit`/`ret_all`/`ret_stc`, `RuntimeBlockInfo::csc_RetCache`)
  permanece compilado e ocupando espaço/mantendo estado, mas nunca é exercitado
  em runtime pelo fluxo atual — `BET_DynamicRet` cai no caminho genérico de
  salto dinâmico como qualquer outro.
- **`x86_block_externs`/`DynaRBI::Relocate()` são código morto.** `reloc_info` é
  sempre setado para `0` em `ngen_Compile` (`rec_x86_driver.cpp:275`) e nunca
  recebe um `x86_block::GetExterns()` de fato; `Relocate()` (que despacharia
  para `reloc_info->Apply(...)`, e causaria null-deref se chamado) **nunca é
  invocado em lugar nenhum do código-base** (confirmado por busca em todo
  `core/`). O grande comentário de desenho em `core/hw/sh4/dyna/ngen.h:1-41`
  (esquema de "staging buffer" + "steady-state buffer" com GC/relocalização de
  blocos) descreve uma funcionalidade que, pelo menos para este backend, **não
  foi implementada** — só o "esqueleto" da interface (`Relink`/`Relocate`
  virtuais) existe.
- **Stub de escrita rápida via nvmem nunca é usado.** `mem_code[0][1][*]` (modo
  0 = rápido, w=1 = escrita) é gerado por `ngen_init` para todos os 5 tamanhos,
  mas `shop_writem` sempre chama `mem_code[2][1][Lsz]` (modo geral) —
  `rec_x86_il.cpp:536`. O corpo de código gerado para essas variantes ocupa
  espaço no code cache permanentemente sem jamais ser alcançado por uma
  chamada.
- **`ngen_ResetBlocks()` é hoje um no-op efetivo neste backend.** Zera
  `mem_code_end` (`rec_x86_driver.cpp:698-701`), mas como `ngen_init()` só é
  chamado uma vez, no boot (`core/hw/sh4/dyna/driver.cpp:463`), e nenhum outro
  ponto do código volta a chamar `ngen_init()` depois de uma limpeza de cache,
  esse reset não tem efeito observável no fluxo atual — os stubs continuam
  válidos e nunca são regenerados (o que é correto/desejado na prática, dado
  que `emit_SetBaseAddr()` protege essa região de `LastAddr` mesmo após um
  clear, mas levanta a questão de por que o reset existe se nada o consome).
- **Otimização especulativa de leitura constante está morta** (`#if 0` +
  `OPTIMIZATION_GRAVEYARD` em `rec_x86_il.cpp:285-315`, dentro de
  `shop_readm`). O comentário original do autor já documenta a limitação:
  *"this is a pretty good sieve, but its not perfect [...] Maybe a mix of both
  ?"* — a ideia (usar valor lido em compile-time como palpite, validar/invalidar
  em runtime) nunca foi finalizada.
- **`shop_div32s`/`shop_div32u`/`shop_div32p2` comentados e não implementados
  nativamente** (`rec_x86_il.cpp:1256-1284`, dentro de um bloco `/* TODO Update
  this according to new canonical implementation ... */`). Caem no fallback
  canônico via `shil_chf[]` — funcionalmente corretos, mas mais lentos que uma
  tradução nativa `idiv`/`div`.
- **Vestígios de suporte a x64 nunca terminados no emissor.** `x86_emitter.h`
  define registradores de 64 bits (`R0q..R15q`), variantes REX de 8 bits, e
  `x86_op_encoder.h` tem `encode_rex(...)` completo sob `#ifdef X64` — mas
  `X64` nunca é definido neste diretório (o backend x64 real,
  `core/rec-x64/rec_x64.cpp`, é uma implementação **totalmente separada** que
  não usa este emissor/matcher). Ou seja, esse emissor foi desenhado para
  eventualmente suportar as duas arquiteturas com o mesmo código, mas o suporte
  a 64 bits foi abandonado aqui e refeito do zero em outro lugar — duplicação
  de esforço de engenharia entre os dois backends x86/x64.
- **Origem dos arquivos `generated_*.h` não rastreável no repositório.** São
  claramente gerados por uma ferramenta externa (comentário `//auto generated`
  no topo de cada um), mas nenhum script gerador está presente em nenhum lugar
  deste checkout — não há como regenerá-los ou entender de onde vieram as
  ~2300 linhas de `generated_descriptors.h` sem uma fonte externa ao repo.
- **`rec_win86_asm.S` diverge de `rec_lin86_asm.S` de forma sutil e frágil**: lê
  o PC inicial via `mov ecx,[0xA0000000]` (endereço **fixo/hardcoded**, não via
  `p_sh4rcb`+offset como a variante Linux faz, `rec_lin86_asm.S:96-98`) — depende
  de uma convenção de layout de memória que não é validada em compile-time; e
  referencia `_cpurun_offset` (`rec_win86_asm.S:109`) que **não é declarado
  `.globl`** nem importado em nenhum `.extern`/símbolo visível nesse arquivo
  (só `_p_sh4rcb`, `_gas_offs`, etc. são listados nas diretivas `.globl` do
  topo) — se este arquivo ainda é de fato usado/buildado, é candidato a bug de
  link ou símbolo resolvido "por sorte" via outra unidade de tradução.
- **Checksum `CheckBlock` roda incondicionalmente em todo bloco, mesmo quando
  redundante com a proteção de página** (seção 6.2) — custo de runtime real
  (N/4 pares `cmp`+`jne` por bloco, toda execução) que hoje parece existir só
  como rede de segurança para os casos `read_only==false`; para os blocos
  `read_only==true` (a maioria do código "quente" e estável de um jogo em RAM
  protegida), o checksum inteiro é, na prática, trabalho descartável (o `jne`
  aponta pra um `int3` que não deveria dispar). Não medido neste documento —
  é só uma observação estática; ver `docs/tech_debits.md` para registrar se for
  o caso de instrumentar (nota: este achado é sobre o backend x86, que não
  roda no device alvo deste projeto — o valor dessa observação é para quem for
  comparar com o equivalente no backend ARM64).
