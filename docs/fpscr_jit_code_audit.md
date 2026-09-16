# Auditoria: seria seguro nativizar `lds Rn,FPSCR` no JIT ARM64? (flycast, fork metallic77)

> Gerado por subagente de investigação (read-only, sem medição), 2026-09-16.
> Motivado pelo achado de contagem por-opcode do fallback pro interpretador
> (`shop_ifb`, `core/rec-ARM64/rec_arm64.cpp:35-76`) em gameplay real de Shenmue:
> `lds <REG_N>,FPSCR` (0x4X6A) é o opcode de maior volume (~196 hits/frame,
> 484.809 em 2476 frames), seguido por `lds.l @<REG_N>+,FPSCR` (~21/frame),
> `div1` (~62/frame) e `tas.b`. Este documento cobre só os dois opcodes de
> FPSCR — é investigação, **nenhum código foi alterado**.

---

## 1. Contexto — por que estes opcodes existem no fallback

`core/hw/sh4/sh4_opcode_list.cpp` define cada opcode SH4 numa tabela
(`sh4_opcodelistentry`, layout em `core/hw/sh4/sh4_opcode_list.h:41-56`) com,
entre outros, dois campos relevantes:

- `rec_oph` — ponteiro pra uma função escrita à mão que decodifica ESSE opcode
  específico direto pra SHIL (ex.: as funções `sh4dec(...)` de
  `core/hw/sh4/dyna/decoder.cpp`).
- `decode` — um `u64` empacotado (`dec_Fill()`, `sh4_opcode_list.cpp:33-36`)
  que descreve o opcode de forma genérica, consumido por `dec_generic()`
  (`core/hw/sh4/dyna/decoder.cpp:676-948`) — o "tradutor genérico" pro grosso
  dos opcodes regulares (aritmética, load/store, etc.).

As duas linhas da tabela em questão:

```
core/hw/sh4/sh4_opcode_list.cpp:240:
{0, i0100_nnnn_0110_0110, Mask_n, 0x4066, FWritesFPSCR, "lds.l @<REG_N>+,FPSCR", 1,1,CO, fix_none},
core/hw/sh4/sh4_opcode_list.cpp:275:
{0, i0100_nnnn_0110_1010, Mask_n, 0x406A, FWritesFPSCR, "lds <REG_N>,FPSCR",     1,1,CO, fix_none},
```

Ambas têm `rec_oph=0` (primeiro campo) **e** nenhum sufixo `dec_XXX(...)`
(logo `decode=0`, já que os campos não-inicializados ficam 0 em agregados
C++). Comparar com a leitura oposta (FPSCR→registrador), que TEM decode:

```
core/hw/sh4/sh4_opcode_list.cpp:266:
{0, i0000_nnnn_0110_1010, Mask_n, 0x006A, UsesFPU, "sts FPSCR,<REG_N>", 1,3,CO, fix_none, dec_ST(PRM_SREG)},
```

`sts FPSCR,Rn` (ler FPSCR pra um GPR) tem `decode = dec_ST(PRM_SREG)`
(`sh4_opcode_list.cpp:91`: `dec_Fill(DM_UnaryOp,PRM_RN,d,shop_mov32)`) e por
isso passa por `dec_generic()` normalmente — é uma cópia sem efeito colateral
de modo, trivialmente nativizável, e já está nativizada. **A assimetria é
direcional: ler FPSCR é seguro e já é nativo; ESCREVER FPSCR é o caso sem
tradução.** Isso já é uma pista de que o problema não é "ninguém teve tempo
de escrever o codegen", é que escrever FPSCR tem uma implicação que ler não
tem (seção 2).

No loop de decodificação (`core/hw/sh4/dyna/decoder.cpp:1024-1039`):

```cpp
if (state.ngen.OnlyDynamicEnds || !OpDesc[op]->rec_oph)
{
    if (state.ngen.InterpreterFallback || !dec_generic(op))
    {
        dec_fallback(op);                                    // shop_ifb
        ...
        if (OPCODE_SETFPSCR(OpDesc[op]->type) && !state.cpu.is_delayslot)
        {
            dec_End(state.cpu.rpc+2,BET_StaticJump,false);    // <- força fim de bloco
        }
    }
}
```

Pra `lds Rn,FPSCR`: `rec_oph` nulo → entra no `if`; `dec_generic()` retorna
`false` na primeira linha (`decoder.cpp:679-680`: `if (OpDesc[op]->decode==0)
return false;`) porque `decode` é 0 → cai em `dec_fallback(op)`
(`decoder.cpp:88-101`, emite `shop_ifb`). Como `OPCODE_SETFPSCR(type)` é
verdadeiro (`type = FWritesFPSCR = UsesFPU|WritesFPSCR`,
`sh4_opcode_list.h:26`) e a instrução não está num delay slot, o decoder
**força o fim do bloco imediatamente**, com `BET_StaticJump` pro endereço
logo depois da própria instrução (`state.cpu.rpc+2`). Isso é central pra
seção 4 — é o mecanismo de segurança que já existe hoje.

---

## 2. O que `FPR64`/`FSZ64` são e por que são invariantes de compile-time do bloco

`state_t::cpu` (`core/hw/sh4/dyna/decoder.h:58-63`) guarda, entre outros
campos do decoder (não da CPU real):

```cpp
struct
{
   bool FPR64; //64 bit FPU opcodes
   bool FSZ64; //64 bit FPU moves
   bool RoundToZero;
   u32 rpc;
   bool is_delayslot;
} cpu;
```

Esses dois bits vêm de `FPSCR.PR` (precisão: simples vs dupla pras opcodes
aritméticas de FPU) e `FPSCR.SZ` (tamanho de transferência: se `fmov`
movimenta registrador único ou par), confirmados no layout de bits de
`fpscr_t` em `core/hw/sh4/sh4_if.h:212-280` (`PR` bit 19, `SZ` bit 20, `FR`
bit 21, nessa ordem LSB-first).

**Eles são escritos em exatamente 2 lugares em todo o código** (grep
`FPR64`/`FSZ64` em `core/`, sem outra ocorrência):

```
core/hw/sh4/dyna/decoder.cpp:954:  state.cpu.FPR64=fpu_cfg.PR;
core/hw/sh4/dyna/decoder.cpp:955:  state.cpu.FSZ64=fpu_cfg.SZ;
```

dentro de `state_Setup()`, chamada **uma única vez**, no início de
`dec_DecodeBlock()` (`decoder.cpp:970-973`):

```cpp
bool dec_DecodeBlock(RuntimeBlockInfo* rbi, u32 max_cycles)
{
    blk = rbi;
    state_Setup(blk->vaddr, blk->fpu_cfg);   // <- FPR64/FSZ64 fixados aqui, só aqui
```

`blk->fpu_cfg` vem de `RuntimeBlockInfo::Setup(u32 rpc, fpscr_t rfpu_cfg)`
(`core/hw/sh4/dyna/driver.cpp:155-182`, `fpu_cfg=rfpu_cfg;` na linha 182), que
por sua vez é chamado de `rdv_CompilePC()` com o **FPSCR ao vivo da CPU no
momento da compilação do bloco**:

```
core/hw/sh4/dyna/driver.cpp:216:  if (!rbi->Setup(pc,fpscr))
```

Ou seja: `FPR64`/`FSZ64` são um retrato do FPSCR real tirado no instante em
que aquele bloco é compilado, e depois disso **nada no resto da decodificação
daquele bloco os atualiza** — com uma exceção parcial e reveladora:

```
core/hw/sh4/dyna/decoder.cpp:301-307  (fschg)
Emit(shop_xor,reg_fpscr,reg_fpscr,mk_imm(1<<20));
state.cpu.FSZ64=!state.cpu.FSZ64;
```

`fschg` (que alterna só o bit `SZ`) **atualiza `state.cpu.FSZ64` no meio da
decodificação**, e isso é seguro precisamente porque o efeito é conhecido em
tempo de compilação (é sempre um XOR de um bit fixo — não depende de valor de
registrador) e o loop de `dec_DecodeBlock` é uma única passada linear e
sequencial pelo fluxo de instruções (`decoder.cpp:980-1058`, `for(;;)` com
`state.cpu.rpc+=2` a cada opcode): o decoder pode "seguir junto" com o efeito
porque ele mesmo está emitindo o efeito, na ordem certa, sem ambiguidade.

Já `frchg` (que alterna o bit `FR`, banco de registrador) **não** atualiza
nenhum campo em `state.cpu`:

```
core/hw/sh4/dyna/decoder.cpp:309-316
Emit(shop_xor,reg_fpscr,reg_fpscr,mk_imm(1<<21));
Emit(shop_mov32,reg_old_fpscr,reg_fpscr);
Emit(shop_frswap,regv_xmtrx,regv_fmtrx,regv_xmtrx,0,rmn,regv_fmtrx);
```

— consistente: `FR` seleciona qual banco físico (`FR0-15` vs `XF0-15`) é
usado pelas instruções de matriz, não muda `FPR64`/`FSZ64` nem a forma como
o restante do bloco precisa ser decodificado (pareamento de registrador,
`FRN_SZ`/`FRM_SZ`), então não há nada pro decoder atualizar.

**`lds Rn,FPSCR`/`lds.l @Rn+,FPSCR` são categoricamente diferentes de
`fschg`/`frchg`:** eles escrevem um valor de 32 bits **lido de um registrador
em tempo de EXECUÇÃO** (`fpscr.full = r[n]`, seção 3) — o novo estado de
`PR`/`SZ` não é conhecido em tempo de compilação, e portanto **não existe
como o decoder poderia "seguir junto" atualizando `state.cpu.FPR64`/`FSZ64`
do jeito que faz pro `fschg`**. Essa é a resposta central da pergunta 1: sim,
escrever FPSCR muda como o resto do bloco precisaria ser interpretado (via
`FPR64`/`FSZ64`), e isso É uma invariante de compile-time — decidida uma vez
no início do bloco (`decoder.cpp:954-955`) — que hoje só pode mudar de forma seg
ura dentro do MESMO bloco quando a mudança é um efeito fixo conhecido em tempo
de compilação (`fschg`). Pra um valor arbitrário vindo de registrador, a única
forma segura que o código atual usa é **terminar o bloco ali** (seção 1) e
deixar o PRÓXIMO bloco recapturar o FPSCR real do zero em `state_Setup()`.

Onde `FPR64`/`FSZ64` são efetivamente *usados* pelo decoder, pra deixar claro
o que quebraria se ficassem obsoletos dentro do bloco:

- `decoder.cpp:695-696` — `dec_generic()` recusa TODA tradução nativa de
  opcode de FPU (`op>=0xF000`) quando `state.cpp.FPR64` é verdadeiro:
  `if (state.cpu.FPR64 /*|| state.cpu.FSZ64*/) return false;` (cai pro
  fallback do interpretador pra qualquer FPU op de precisão dupla, não só
  FPSCR).
- `decoder.cpp:698-699` — decide se a transferência é de 64 bits
  (`transfer_64`) quando o parâmetro é `PRM_FRN_SZ`/`PRM_FRM_SZ` e
  `FSZ64` é verdadeiro.
- `decoder.cpp:448-459` e `decoder.cpp:464-475` (`dec_param()`, casos
  `PRM_FRN_SZ`/`PRM_FRM_SZ`) — decide se `GetN(op)`/`GetM(op)` seleciona um
  registrador único (`reg_fr_0+n`) ou um par banked (`regv_dr_0+n/2` /
  `regv_xd_0+n/2`) **com base em `state.cpu.FSZ64`**. Isso é escolha de QUAL
  registrador shil o resto do bloco referencia — gravado permanentemente no
  SHIL/código ARM64 gerado pra aquele bloco.

Se `state.cpu.FPR64`/`FSZ64` ficarem obsoletos (porque um `lds Rn,FPSCR` no
meio do bloco mudou o valor real mas o decoder continuou com o valor antigo),
qualquer opcode de FPU decodificado DEPOIS, no mesmo bloco, usa a
classificação errada de forma silenciosa — não há checagem de consistência
em nenhum desses três pontos.

---

## 3. O que `LDS Rn,FPSCR` realmente faz (implementação do interpretador)

`core/hw/sh4/interpr/sh4_opcodes.cpp:2083-2089`:

```cpp
//lds <REG_N>,FPSCR
sh4op(i0100_nnnn_0110_1010)
{
	u32 n = GetN(op);
	fpscr.full = r[n];
	UpdateFPSCR();
}
```

E a variante `.l` com pós-incremento, `sh4_opcodes.cpp:2056-2064`:

```cpp
//lds.l @<REG_N>+,FPSCR
sh4op(i0100_nnnn_0110_0110)
{
	u32 n = GetN(op);
	ReadMemU32(fpscr.full,r[n]);
	UpdateFPSCR();
	r[n] += 4;
}
```

Nas duas, o corpo é curto: sobrescreve os 32 bits inteiros de `fpscr` (não só
um campo) e chama `UpdateFPSCR()`. **Não há troca de banco de GPR nem
invalidação de cache de código aqui** — isso é feito, condicionalmente, DENTRO
de `UpdateFPSCR()`:

`core/hw/sh4/sh4_core_regs.cpp:148-154`:

```cpp
//called when fpscr is changed and we must check for reg banks etc..
void UpdateFPSCR()
{
	if (fpscr.FR != old_fpscr.FR)
		ChangeFP(); // FPU bank change

   old_fpscr = fpscr;
   setHostRoundingMode();
}
```

Três efeitos possíveis, cada um condicional/relativamente barato no
interpretador:

1. **Troca de banco FR↔XF, só se o bit `FR` mudou.** `ChangeFP()`
   (`sh4_core_regs.cpp:29-42`) faz `std::swap` de dois arrays de 16 `f32`
   (`Sh4cntx.xffr[0..15]` ↔ `[16..31]`) — 16 trocas de 32 bits, custo trivial
   pro interpretador, mas é um efeito sobre o ARMAZENAMENTO FÍSICO dos
   registradores, não sobre decodificação.
2. **`old_fpscr = fpscr`** — só contabilidade, pra próxima chamada poder
   comparar `FR` de novo.
3. **`setHostRoundingMode()`** (`sh4_core_regs.cpp:69-141`), só se `RM` ou
   `DN` mudaram desde a última chamada (cacheado em `old_rm`/`old_dn` file-
   -static, linhas 66-67): programa o `FPCR` real da CPU ARM64 via `asm
   volatile` (`sh4_core_regs.cpp:120-137`, `MRS`/`MSR FPCR`) pra bater com o
   modo de arredondamento e flush-to-zero de denormais que o SH4 emulado
   pediu. Efeito sobre o HOST, não sobre o bloco JIT.

**Nenhum desses três efeitos invalida blocos de código já compilados**, e
nenhum aciona `bm_ResetCache`/`recSh4_ClearCache`
(`core/hw/sh4/dyna/driver.cpp:65-79`) — grep confirma que `UpdateFPSCR` não
chama nada em `blockmanager.*`. **O único efeito que interage com o JIT é o
que `UpdateFPSCR()` NÃO faz explicitamente: nada ali atualiza `FPR64`/`FSZ64`
do decoder** — esses só existem no lado do JIT (`decoder.h`/`decoder.cpp`), e
`UpdateFPSCR()` é código do interpretador/runtime, chamado tanto pelo
interpretador puro quanto (via `shop_ifb`) de dentro do JIT — ele sincroniza
o ESTADO REAL da CPU emulada, não o estado de decodificação de um bloco já em
andamento. É exatamente essa lacuna — real FPSCR atualizado, decoder
`FPR64`/`FSZ64` não — que o fim de bloco forçado (seção 1) existe pra tapar.

---

## 4. Mecanismos relacionados existentes

### 4.1 `shop_sync_fpscr` — existe na tabela SHIL e no codegen, mas nunca é emitido

`shop_sync_fpscr` está definido em `core/hw/sh4/dyna/shil_canonical.h:218-227`:

```cpp
shil_opc(sync_fpscr)
shil_canonical
(
void, f1, (),
	UpdateFPSCR();
)
shil_compile
(
	shil_cf(f1);
)
shil_opc_end()
```

e tem codegen ARM64 pronto, `core/rec-ARM64/rec_arm64.cpp:520-522`:

```cpp
case shop_sync_fpscr:
    GenCallRuntime(UpdateFPSCR);
    break;
```

(idêntico em espírito ao vizinho `shop_sync_sr` → `GenCallRuntime(UpdateSR)`,
linha 517-519, e replicado nos backends x64/x86 —
`core/rec-x64/rec_x64.cpp:505-506`, `core/rec-x86/rec_x86_il.cpp:854-868`.)

O regalocador e o otimizador SSA também já sabem tratar essa opcode
corretamente, tratando-a como uma barreira que consome/produz TODO o estado
de FPU:

- `core/hw/sh4/dyna/regalloc.h:375` — `IsFlushOp()` trata `shop_sync_fpscr`
  igual a `shop_sync_sr`/`shop_ifb` (opcode que exige flush antes de rodar).
- `core/hw/sh4/dyna/regalloc.h:492-517` — no caminho `fp=true` (disparado por
  `shop_sync_fpscr`), faz flush de `reg_fpscr`, `reg_old_fpscr` **e todos os
  32 registradores `reg_fr_0..reg_xf_15`** (loop de 16 + comentário implícito
  de FR/XF) antes/depois da chamada — ou seja, o regalocador já assume que
  `UpdateFPSCR()` pode fazer o `ChangeFP()` (troca de banco inteiro) e invalida
  tudo que poderia ter mudado.
- `core/hw/sh4/dyna/ssa.h:191-199` e `:364-368`, `core/hw/sh4/dyna/ssa_regalloc.h:74-77,409`
  — o otimizador SSA também reconhece `shop_sync_fpscr` explicitamente e
  invalida as versões conhecidas de `reg_fpscr`/`reg_old_fpscr`/todo o range
  `reg_fr_0..reg_xf_15` no propagador de constantes e no rastreador de
  versões.

**Porém:** `grep -rn "Emit(shop_sync_fpscr" core/` não retorna nada — **não
existe nenhum ponto em `decoder.cpp` (nem em nenhum outro lugar) que de fato
emita essa opcode**. Comparar com `shop_sync_sr`, que É emitido, duas vezes,
pelas instruções que escrevem SR:

```
core/hw/sh4/dyna/decoder.cpp:246 (rte):     Emit(shop_sync_sr);
core/hw/sh4/dyna/decoder.cpp:292 (ldc Rn,SR): Emit(shop_sync_sr);
```

**Conclusão da pergunta 2:** `shop_sync_fpscr` é infraestrutura completa e
correta (codegen + regalloc + SSA) só que **órfã** — parece ter sido
preparada (aqui ou já upstream, antes do fork divergir) pra uma tradução
nativa de `lds Rn,FPSCR`/`.l` que nunca foi conectada no decoder. Ela reaproveita
exatamente a peça que falta (chamar `UpdateFPSCR()` sem passar pelo
interpretador completo de opcode), mas sozinha **não resolve** o problema de
`FPR64`/`FSZ64` obsoletos da seção 2 — ela sincroniza o estado FÍSICO
(bancos FR/XF, FPCR do host), não o estado de DECODIFICAÇÃO do bloco. Usá-la
troca "chamar o handler de opcode completo do interpretador" por "chamar só
`UpdateFPSCR()`" — economiza o dispatch pesado do `shop_ifb` (seção 4.2), mas
não evita, por si só, a necessidade de lidar com `FPR64`/`FSZ64` obsoletos.

### 4.2 Custo do caminho atual (`shop_ifb`) — pra dimensionar o que se ganharia

Codegen do fallback, `core/rec-ARM64/rec_arm64.cpp:426-455`: grava `next_pc`
se a opcode precisar (`OPCODE_NEEDPC` — não é o caso de `lds`/`lds.l FPSCR`,
`type=FWritesFPSCR` não tem `ReadsPC`, `sh4_opcode_list.h:26`), põe o opcode
bruto de 16 bits no primeiro registrador de argumento, e chama
`OpDesc[op.rs3._imm]->oph` via `GenCallRuntime` — ou seja, chama o
interpretador de opcode-único inteiro (a mesma função usada pro interpretador
puro), com toda a indireção de function pointer que isso implica.

Antes dessa chamada, o regalocador já é obrigado a fazer flush de um
conjunto amplo e **opcode-agnóstico** de spans, `core/hw/sh4/dyna/regalloc.h:438-463`:
`reg_r0`, `reg_sr_T`, `reg_sr_status`, `reg_fpscr`, e todo o intervalo
`reg_gbr..reg_fpul` — o mesmo custo de flush é pago pra `lds Rn,FPSCR` quanto
seria pra qualquer outra opcode `shop_ifb`, porque o regalocador não sabe
quais registradores a opcode específica realmente toca (só sabe que É um
`shop_ifb`).

Some a isso o fim de bloco forçado (seção 1): cada `lds Rn,FPSCR` real em
jogo também trunca a árvore de blocos ali, impedindo qualquer fusão/otimização
entre o código antes e depois dele, e fazendo o dispatcher resolver o próximo
bloco (`rdv_FindOrCompile`/`bm_GetCodeByVAddr`, `driver.cpp:308-313`,
`blockmanager.cpp:53-60`) a cada ocorrência.

### 4.3 Não existe checagem de `fpu_cfg` no despacho de blocos — nem flag de invalidação dedicada

O ponto de dispatch de blocos compilados, `bm_GetCode()`
(`core/hw/sh4/dyna/blockmanager.cpp:44-49`):

```cpp
#define FPCA(x) ((DynarecCodeEntryPtr&)sh4rcb.fpcb[(x>>1)&FPCB_MASK])
static DynarecCodeEntryPtr DYNACALL bm_GetCode(u32 addr)
{
	DynarecCodeEntryPtr rv = FPCA(addr);
	return rv;
}
```

é uma tabela indexada **só pelo endereço físico**. `fpu_cfg` (o retrato do
FPSCR capturado na compilação, seção 2) fica guardado em
`RuntimeBlockInfo::fpu_cfg` (`blockmanager.h:43`) mas **não faz parte de
nenhuma chave de lookup** — não há comparação entre o `fpu_cfg` com que um
bloco foi compilado e o FPSCR real no momento em que ele é despachado de
novo. `grep -rn "fpu_cfg" core/hw/sh4/dyna/blockmanager.cpp` não retorna
nada.

Ou seja: **não existe, hoje, nenhum mecanismo de invalidação de bloco
dedicado a mudança de modo de FPU** (nem um flag tipo `smc_hotspots`, nem
`bm_...` genérico) — o `bm_...`/`smc_hotspots` mencionados no pedido são
mecanismos de **self-modifying code** (`driver.cpp:50,72-79,222`,
`RuntimeBlockInfo::hash()`, `driver.cpp:127-153`), ortogonais a FPSCR. A
segurança de `FPR64`/`FSZ64` hoje não vem de invalidar blocos — vem de
**nunca deixar um bloco existir cuja premissa de FPSCR possa ficar errada em
tempo de execução**: todo caminho que escreve FPSCR e não tem tradução nativa
força fim de bloco (seção 1) e toda recompilação de bloco recaptura o FPSCR
ao vivo (`driver.cpp:216`, seção 2). É um design "nunca fica velho" em vez de
"detecta e invalida quando fica velho".

### 4.4 O precedente mais próximo de "fim de bloco condicionado a um valor calculado em runtime": `BET_Cond_0`/`BET_Cond_1`

`bt`/`bf`/`bt.s`/`bf.s` (branches condicionais por `sr.T`) já terminam bloco
de forma **condicionada a um valor só conhecido em runtime** — mas o
mecanismo é especificamente amarrado à semântica de branch/T-flag, não é uma
condição genérica:

- Tipos de fim de bloco: `core/hw/sh4/dyna/decoder.h:26-27` —
  `BET_Cond_0`/`BET_Cond_1` = `"sr.T==0/1 -> BranchBlock else NextBlock"`.
- Emissão: `decoder.cpp:165-187` — `bt`/`bf` chamam só `dec_End(...,
  BET_Cond_0/1, false)`; `bt.s`/`bf.s` primeiro emitem
  `Emit(shop_jcond,reg_pc_dyn,reg_sr_T)` (marcando `blk->has_jcond=true`) e
  só depois `dec_End`.
- Epílogo (código que decide de fato o desvio, gerado por bloco),
  `core/rec-ARM64/rec_arm64.cpp:1289-1336`: lê `Sh4cntx.jdyn`
  (se `has_jcond`, linha 1297) ou `sr.T` direto da memória de contexto
  (linha 1299, caso contrário), compara (`Cmp`) contra `block->BlockType &
  1`, e desvia pro `pBranchBlock` ou `pNextBlock` (ou pros stubs de link
  lazy quando o alvo ainda não foi compilado).

Isso mostra que a INFRAESTRUTURA de "terminar bloco baseado num valor
calculado durante a execução do próprio bloco" já existe e funciona
(`BET_Cond_0/1` + `shop_jcond`). Mas ela está codificada especificamente em
torno de `sr.T`/`jdyn` (o campo lido no epílogo é hardcoded pra um dos dois,
`rec_arm64.cpp:1296-1299`) — **não é uma condição genérica reutilizável sem
plumbing novo**. Adaptar isso pra "terminar bloco só se os bits PR/SZ do novo
FPSCR mudaram" precisaria de: um novo campo de condição (ou reaproveitar
`jdyn` com uma convenção nova) e uma nova opcode/variante de emissão — não é
um "já dá pra usar direto".

Um detalhe que sugere que esse tipo de checagem já foi pelo menos cogitado
por quem projetou `fpscr_t`: existe um campo de conveniência não utilizado em
lugar nenhum do código, `core/hw/sh4/sh4_if.h:267-278`:

```cpp
struct
{
    u32 _nil   : 19;  // RM..cfpuerr
    u32 PR_SZ  : 2;   // bits 19-20 = exatamente PR+SZ juntos
    u32 nilz   : 11;
};
```

`grep -rn "PR_SZ" core/` só retorna a própria definição — **nunca é lido em
nenhum lugar**. É exatamente o par de bits que decide `FPR64`/`FSZ64`, pronto
pra comparação em uma única leitura de 2 bits, mas não há nenhum consumidor.

---

## 5. Avaliação de risco

### 5.1 Tradução nativa "ingênua" (escrever `fpscr` e continuar o bloco): **RISCO ALTO**

Se `lds Rn,FPSCR` ganhasse um `rec_oph`/`decode` que simplesmente emitisse
`Emit(shop_mov32, reg_fpscr, rsN)` (+ opcionalmente reviver
`Emit(shop_sync_fpscr)`, seção 4.1) e deixasse o decoder continuar
decodificando o resto do bloco normalmente (sem forçar fim de bloco), isso
quebraria diretamente a invariante estabelecida na seção 2:
`state.cpu.FPR64`/`FSZ64` continuariam com o valor capturado no início do
bloco (`decoder.cpp:954-955`), agora **obsoleto**, e qualquer instrução de
FPU decodificada depois, no MESMO bloco, usaria a classificação errada em
`decoder.cpp:695-699` (gate de precisão dupla) e `decoder.cpp:449-476`
(seleção de registrador único vs par banked) — produzindo código ARM64 que
lê/escreve o registrador físico errado, ou aplica aritmética de precisão
errada, de forma **permanente e silenciosa** (gravado no bloco compilado, sem
nenhum re-check em runtime — seção 4.3 confirma que não existe invalidação
por mudança de FPSCR). Não é um crash — é corrupção de estado de FPU
(posições, física, ou pior, sinais de áudio/render derivados de cálculo de
ponto flutuante errado), exatamente a classe de bug mais cara de diagnosticar
que o projeto já documentou (`docs/tech_debits.md` item 5.2 — postmortem do
skip de Translucent, corrupção visual sem crash claro).

Isso vale mesmo que, na prática, a maioria das chamadas reais de
`lds Rn,FPSCR` em Shenmue não mude `PR`/`SZ` (plausível — jogos costumam
reescrever o mesmo valor de FPSCR repetidamente, ou só mexer em `RM`/`DN`) —
o risco não é "acontece sempre", é "quando acontecer, é silencioso e
indetectável sem teste dirigido", e o volume medido (484.809 hits) garante
MUITAS oportunidades pra um dos casos ruins acontecer em algum jogo, mesmo
que raro em Shenmue especificamente.

### 5.2 Caminho mais seguro (nativizar só quando o modo de precisão não muda): **RISCO MÉDIO**

O formato concreto, com base no que existe:

1. Emitir a escrita de fato: `Emit(shop_mov32, reg_fpscr, rsN)` (registrador)
   ou o equivalente de leitura de memória pra `.l`.
2. Calcular, em SHIL, se os bits que importam mudaram: mascarar o novo valor
   com a máscara de `PR|SZ` (exatamente o campo `PR_SZ` de 2 bits ocioso,
   `sh4_if.h:271/275`) e comparar contra a constante conhecida em
   compile-time (`state.cpu.FPR64`/`FSZ64` atuais) — ex.:
   `Emit(shop_and, tmp, rsN, mk_imm(PR_SZ_MASK))` +
   `Emit(shop_seteq/setne, cond, tmp, mk_imm(valor_atual_PR_SZ))`.
3. Reaproveitar (com plumbing novo — seção 4.4) o padrão `BET_Cond_0/1` +
   `shop_jcond` pra terminar o bloco (com exatamente o `dec_End(state.cpu.rpc+2,
   BET_StaticJump, false)` que já roda hoje, seção 1) **só** quando `cond`
   indica mudança; caso contrário, continuar decodificando o resto do bloco
   normalmente — o `FPR64`/`FSZ64` do decoder continuam válidos porque, por
   construção, não mudaram.
4. Em ambos os ramos, chamar `UpdateFPSCR()` (via `shop_sync_fpscr` revivido,
   seção 4.1) pra manter banco FR/XF e modo de arredondamento do host
   corretos — isso é necessário independente do PR/SZ terem mudado (o `FR`
   pode mudar junto, e `RM`/`DN` sempre podem mudar).

Por que ainda é MÉDIO e não BAIXO:

- **Sem precedente direto:** a seção 4.4 mostra que "terminar bloco
  condicionado a um valor calculado em runtime" existe, mas hardcoded pra
  `sr.T`/`jdyn` — extrapolar pra uma condição arbitrária de FPSCR é
  engenharia nova (novo campo de contexto ou nova convenção pra `jdyn`, novo
  caminho no epílogo do ARM64), não reuso direto.
- **`shop_sync_fpscr` sozinho não é grátis:** ainda é uma chamada de runtime
  completa (`GenCallRuntime(UpdateFPSCR)`, seção 4.1) com o mesmo padrão de
  flush amplo que `shop_ifb` já paga pros registradores de FPU
  (`regalloc.h:492-517`, flush de `reg_fpscr`+`reg_old_fpscr`+ os 32
  `reg_fr_0..reg_xf_15`) — o ganho real do caminho rápido é evitar (a) o
  dispatch indireto pro handler de opcode completo do interpretador e (b) o
  fim de bloco forçado, não eliminar a chamada de runtime inteira.
- **Superfície de teste:** qualquer bug aqui é da mesma classe silenciosa da
  seção 5.1 (só que só se manifesta se a lógica de comparação tiver um erro
  de máscara/sinal) — precisaria de teste dirigido comparando saída
  interpretador-puro vs JIT pra blocos que de fato alternam PR/SZ no meio do
  bloco, não só medição de fps (a disciplina "medir antes de otimizar" do
  projeto vale aqui pra CORREÇÃO, não só performance).

### 5.3 Alternativa de risco mais baixo, mas mais modesta: baratear o `shop_ifb` existente sem mudar semântica

Sem tocar em `FPR64`/`FSZ64` nem no fim de bloco (zero risco de correção,
porque o comportamento fica byte-a-byte igual ao de hoje): tornar o flush de
`regalloc.h:438-463` sensível à opcode específica em vez de sempre
flushar `reg_r0`+`reg_sr_T`+`reg_sr_status`+`reg_fpscr`+todo `reg_gbr..reg_fpul`
pra QUALQUER `shop_ifb` — hoje esse flush é opcode-agnóstico (olha só
`op->op==shop_ifb`, nunca `op->rs3._imm` pra decidir o QUE flushar,
`regalloc.h:455-463` vs. o `switch(OpDesc[op->rs3._imm]->mask)` que vem
logo depois e só decide outra coisa). Isso não elimina a chamada de
interpretador nem o fim de bloco, só reduz o custo de setup ao redor dela.
Ganho por-ocorrência provavelmente menor que as opções 5.1/5.2, mas risco de
correção desprezível — não muda semântica de FPSCR nenhuma. Não avaliado em
profundidade aqui (fora do escopo da pergunta), citado só como piso de
comparação.

---

## 6. Recomendação

**Não nativizar `lds Rn,FPSCR`/`lds.l @Rn+,FPSCR` no formato "ingênuo"
(5.1) — risco alto, confirmado por leitura de código, não hipotético:** a
invariante que seria violada (`FPR64`/`FSZ64` fixados uma vez por bloco,
seção 2) é ativamente o que mantém a tradução de FPU correta hoje em TODO o
resto do JIT, e não há nenhuma rede de segurança em runtime (seção 4.3) que
pegaria o erro depois.

**O caminho "nativizar só o caso comum, com branch de fallback pro
comportamento atual quando PR/SZ mudam" (5.2) é tecnicamente viável e tem
peças reais já no código** (`shop_sync_fpscr` órfão pronto pra reviver, seção
4.1; `PR_SZ` como máscara pronta, seção 4.4; `BET_Cond_0/1` como precedente
de forma, não de conteúdo, seção 4.4) — mas é **risco médio, não baixo**, e
exige plumbing novo no decoder E no epílogo do backend ARM64 (e, se for pra
manter os três backends consistentes, também x64/x86). Antes de implementar,
valeria:

1. Confirmar com os próprios dados já coletados (contagem por-opcode) se dá
   pra estimar, ainda que aproximadamente, que fração dos ~196 hits/frame de
   `lds Rn,FPSCR` em Shenmue de fato muda `PR`/`SZ` vs. só re-escreve o mesmo
   valor ou só mexe em `RM`/`DN` — isso está diretamente disponível
   instrumentando (não medindo, só logando) o valor antigo vs. novo de
   `fpscr.full` nos hits reais de `shop_ifb` pra esse opcode específico, sem
   precisar terminar a investigação de código primeiro. Se a fração de
   "muda modo de verdade" for baixíssima, o caminho rápido citado em 5.2
   recupera quase todo o ganho de volume: a maioria dos hits.
2. Só então avaliar se o ganho esperado (eliminar dispatch indireto +
   flush amplo + fim de bloco, seção 4.2, pra essa fração) justifica o custo
   de engenharia de 5.2 (novo mecanismo de fim de bloco condicional
   genérico) frente à alternativa mais barata e de risco desprezível de 5.3.

Em qualquer caminho escolhido, qualquer mudança de comportamento aqui precisa
de teste dirigido comparando resultado numérico (não só fps/crash) entre
interpretador puro e JIT em cenas que de fato alternam precisão FPU no meio
de um bloco (bibliotecas de matemática que usam `lds Rn,FPSCR` cirurgicamente
ao redor de uma função são o caso de uso mais plausível pra isso acontecer de
verdade) — a métrica "sem crash" não cobre esse tipo de bug.
