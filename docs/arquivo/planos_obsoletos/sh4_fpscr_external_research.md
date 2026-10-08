# Pesquisa externa: `LDS Rn,FPSCR` nativo em JITs de SH4 (e precedentes em outras CPUs)

> Pesquisa pontual, não uma auditoria deste fork. Objetivo: descobrir se existe uma
> forma **conhecida e segura** de compilar `LDS Rn,FPSCR` nativamente (sem cair no
> fallback pro interpretador), olhando pra como o flycast atual (`flyinghead/flycast`),
> outros emuladores SH4, e JITs maduros de outras CPUs com o mesmo problema
> arquitetural (modo de FPU que muda em runtime e afeta como código FP subsequente
> deveria ter sido compilado) resolvem isso. Todo achado abaixo tem fonte citada
> (URL + trecho real de código/manual, não parafraseado de memória). Onde não achei
> informação definitiva, digo isso explicitamente em vez de especular.

## Resumo executivo

`LDS Rn,FPSCR` **não é compilado como um simples "mov" contínuo em nenhuma das
implementações reais examinadas** (flycast atual, redream). As duas terminam o
bloco JIT imediatamente após a instrução — a escrita em si até é nativa (um
`mov`/`store` de verdade), mas o bloco sempre acaba ali, e a próxima instrução SH4
vira o início de um bloco novo, recompilado do zero lendo o FPSCR real naquele
momento. Isso é diferente de como as mesmas implementações tratam `FRCHG`/`FSCHG`
(que **continuam no mesmo bloco**, sem terminá-lo) — a distinção entre os dois casos
é o ponto central da pesquisa (seção 5).

Precedentes em outras arquiteturas (Dolphin/PowerPC, PPSSPP/MIPS) mostram que a
resposta "certa" depende de **se o bit controla como o decodificador escolhe qual
código nativo gerar** (caso do SH4 PR/SZ, e do "paired-single" do PowerPC) — nesse
caso os dois projetos que achei (`flycast` atual e `redream`) sempre terminam o
bloco — **ou se o bit só controla como o hardware/FPU deveria arredondar/tratar os
resultados** (caso do MIPS RM/FCSR, e parcialmente do FPSCR.RM do SH4), que dá pra
mapear direto pro registro de controle da FPU do host, sem precisar terminar bloco
nenhum. `LDS Rn,FPSCR` do SH4 mexe nos dois tipos de bit ao mesmo tempo (PR/SZ
*e* RM/DN/Cause/Enable/Flag), então cai no caso mais restritivo.

---

## 1. O que FPSCR controla (fonte oficial)

Fonte primária: **Hitachi/Renesas, *SuperH RISC engine SH-4 Programming Manual*,
documento `ADE-602-156D`, Rev. 5.0, 19/04/2001**, seção 6.3.2 (página 121 do PDF).
Página institucional do documento na Renesas:
https://www.renesas.com/en/document/mas/sh-4-software-manual?language=en&r=1055161
— cópia acessível usada nesta pesquisa (mesmo conteúdo, mesmo número de documento e
revisão, confirmado nas primeiras páginas do PDF):
https://0x04.net/~mwk/doc/sh/e602156_sh4.pdf

Layout de bits (32 bits, valor inicial `H'0004_0001`):

```
31                     22 21 20 19 18 17          12 11        7 6        2 1  0
        —                  FR SZ PR DN     Cause        Enable      Flag   RM
```

- **FR — Floating-point register bank** (bit 21). `FR=0`: `FR0–FR15` mapeiam pro
  banco físico 0, `XF0–XF15` pro banco 1. `FR=1`: invertido. Instrução dedicada:
  `FRCHG` (`~FPSCR.FR → FPSCR.FR`, seção 9.41 do manual, título oficial "FR-bit
  CHanGe").
- **SZ — Transfer size mode** (bit 20). `SZ=0`: `FMOV` transfere 32 bits. `SZ=1`:
  `FMOV` transfere um par de registros de 32 bits (64 bits), habilitando as formas
  de par (`FMOV DRm/XDm,DRn/XDn`, `FMOV DRm/XDm,@Rn` etc.). Instrução dedicada:
  `FSCHG` (`~FPSCR.SZ → FPSCR.SZ`, seção 9.42, "SZ-bit CHanGe").
- **PR — Precision mode** (bit 19). `PR=0`: instruções de ponto flutuante
  executam em precisão simples. `PR=1`: executam em precisão dupla (instruções de
  suporte gráfico ficam indefinidas nesse modo). O manual é explícito: **"Do not
  set SZ and PR to 1 simultaneously; this setting is reserved. [SZ, PR = 11]:
  Reserved (FPU operation instruction is undefined.)"**
- **DN — Denormalization mode** (bit 18), **Cause/Enable/Flag** (bits 12-17,
  7-11, 2-6 — campos de exceção de FPU) e **RM — Rounding mode** (bits 0-1,
  `00`=round-to-nearest, `01`=round-to-zero, `10`/`11` reservados) completam o
  registrador.

**Correção a um detalhe da premissa da tarefa:** `PR` e `FR` são bits
*diferentes* e controlam coisas diferentes — `FRCHG` alterna o bit `FR` (banco de
registradores), não o `PR` (precisão). Quem alterna `SZ` (tamanho de transferência
do `FMOV`) é `FSCHG`. **Não existe instrução dedicada de 1 bit para alternar `PR`**
— a única forma de mudar `PR` é via `LDS Rn,FPSCR` (ou `LDS.L @Rn+,FPSCR`, ou
`STC`/escrita indireta), carregando o registrador inteiro com um valor arbitrário
vindo de um GPR em runtime. Essa assimetria (bits com toggle dedicado de 1 ciclo
vs. bits que só mudam via carga geral) é exatamente o que explica por que os JITs
tratam `FRCHG`/`FSCHG` diferente de `LDS Rn,FPSCR` (seção 5).

**Custo em hardware real, citado do próprio manual** (seção 6.6.1, "Bank
Switching and FPU Operation Instructions", pg. 125): *"When the LDC instruction is
used on FPSCR, this instruction expends 4 to 5 cycles in order to maintain the FPU
state. With the FRCHG instruction, an FPSCR.FR bit modification can be performed
in one cycle."* (nota: o texto do manual chama a instrução de "LDC" aqui por
descuido — a mnemônica oficial listada na tabela de opcodes e na tabela de timing é
`LDS`, opcode `0100mmmm01101010`.) A tabela 8.3 (Execution Cycles) confirma:
entrada #215 `LDS Rm,FPSCR` — categoria `CO`, issue rate 1, **latência 4**, trava o
estágio `F1` por 3 ciclos a partir do ciclo 3; entrada #217 `LDS.L @Rm+,FPSCR` —
latência `1/4`. Ou seja: **mesmo em hardware, escrever FPSCR não é um "mov" — é
uma operação que serializa o pipeline da FPU pra manter o estado consistente**,
diferente de `FRCHG`/`FSCHG` que são transformações de 1 bit previsíveis.

---

## 2. Como o flycast atual (flyinghead/flycast, master) trata isso

Código lido diretamente de `core/hw/sh4/dyna/decoder.cpp` do repositório
`flyinghead/flycast` (branch `master`, via
`https://raw.githubusercontent.com/flyinghead/flycast/master/core/hw/sh4/dyna/decoder.cpp`,
baixado e inspecionado nesta sessão):

```c
//ldc.l <REG_N>,FPSCR      <-- comentário do próprio flycast; mnemônica real é LDS (ver seção 1)
sh4dec(i0100_nnnn_0110_1010)
{
	Emit(shop_mov32, reg_fpscr, mk_regi(reg_r0 + GetN(op)));
	Emit(shop_sync_fpscr);
	if (!state.cpu.is_delayslot)
		dec_End(state.cpu.rpc + 2, BET_StaticJump, false);
}

//ldc.l @<REG_N>+,FPSCR
sh4dec(i0100_nnnn_0110_0110)
{
	shil_param rn = mk_regi(reg_r0 + GetN(op));
	state.info.has_readm = true;
	Emit(shop_readm, reg_fpscr, rn, shil_param(), 4);
	Emit(shop_add, rn, rn, mk_imm(4));
	Emit(shop_sync_fpscr);
	if (!state.cpu.is_delayslot)
		dec_End(state.cpu.rpc + 2, BET_StaticJump, false);
}
```

Isso responde a pergunta central: **sim, o flycast atual já compila a escrita em
si nativamente** — `shop_mov32`/`shop_readm` são opcodes SHIL normais, compilados
pra `mov`/`load` de verdade nos backends ARM64/x86 (não passam pelo interpretador).
Mas **duas coisas acontecem junto, sempre**:

1. **`shop_sync_fpscr`** não é um "mov" — é uma chamada de função nativa embutida.
   Confirmado em `core/hw/sh4/dyna/shil_canonical.h` (mesmo repo,
   `https://raw.githubusercontent.com/flyinghead/flycast/master/core/hw/sh4/dyna/shil_canonical.h`):
   ```c
   shil_opc(sync_fpscr)
   shil_canonical
   (
   	void, f1, (Sh4Context *ctx),
   		Sh4Context::UpdateFPSCR(ctx);
   )
   shil_compile
   (
   	shil_cf_arg_sh4ctx();
   	shil_cf(f1);
   )
   shil_opc_end()
   ```
   Ou seja, o JIT emite uma chamada real (não um salto pro interpretador
   completo, mas uma chamada de função C++ `Sh4Context::UpdateFPSCR(ctx)` embutida
   no meio do código nativo gerado) que atualiza estado derivado do FPSCR — não
   consegui localizar a implementação de `UpdateFPSCR` no espelho público (não é
   um arquivo `.h`, deve estar num `.cpp` que não baixei; então não confirmo aqui
   exatamente o que ela faz internamente — só que ela existe, é chamada sempre
   após qualquer escrita de FPSCR, e é mais barata que um fallback completo pro
   interpretador). Achei evidência indireta de que o flycast atual também mapeia
   bits de arredondamento do FPSCR pro registro de controle da FPU do host: existe
   um método `Sh4Context::restoreHostRoundingMode()` declarado em `core/hw/sh4/sh4_if.h`
   (linha 207), chamado em `Sh4Recompiler::Run()` (`core/hw/sh4/dyna/driver.cpp`).
   Não tracei a implementação completa — fica registrado como pista, não como
   achado confirmado.

2. **`dec_End(state.cpu.rpc + 2, BET_StaticJump, false)`** — isso **termina o
   bloco JIT imediatamente**, incondicionalmente (só pulado se a instrução estiver
   num delay slot, caso em que quem termina o bloco é a instrução de branch que a
   precede). O mesmo `decoder.cpp` mostra que esse não é um caso isolado — existe
   uma checagem genérica pro caminho de fallback também:
   ```c
   else if (OpDesc[op]->SetFPSCR() && !state.cpu.is_delayslot)
   {
   	dec_End(state.cpu.rpc + 2, BET_StaticJump, false);
   }
   ```
   Confirmado em `core/hw/sh4/sh4_opcode_list.cpp`
   (`https://raw.githubusercontent.com/flyinghead/flycast/master/core/hw/sh4/sh4_opcode_list.cpp`)
   que a flag `FWritesFPSCR` está em exatamente 4 entradas: `lds Rm,FPSCR`,
   `lds.l @Rm+,FPSCR`, `frchg`, `fschg`. As duas primeiras (que mudam FPSCR a
   partir de um valor de runtime arbitrário) **sempre** terminam o bloco. `frchg`
   e `fschg` têm handler próprio (mostrado abaixo) e **não terminam o bloco**.

**O que acontece depois do bloco terminar:** a próxima instrução SH4 vira o
início de um bloco novo. Confirmado em `core/hw/sh4/dyna/driver.cpp`:
```c
static DynarecCodeEntryPtr compilePC(u32 blockcheck_failures)
{
	const u32 pc = Sh4cntx.pc;
	...
	RuntimeBlockInfo* rbi = sh4Dynarec->allocateBlock();
	if (!rbi->Setup(pc, Sh4cntx.fpscr))   // <-- lê o FPSCR REAL, agora, na hora de compilar
	...
```
Ou seja: **cada bloco é compilado especializado pro FPSCR vigente naquele
momento**, lido direto do contexto de CPU real (`Sh4cntx.fpscr`), não inferido
estaticamente. `RuntimeBlockInfo::fpu_cfg` guarda essa config (`blockmanager.h`
linha 25), e o decodificador usa isso pra decidir como traduzir cada instrução FP
subsequente (`core/hw/sh4/dyna/decoder.cpp`, `state_Setup`):
```c
static void state_Setup(u32 rpc, fpscr_t fpu_cfg)
{
	...
	state.cpu.FPR64 = fpu_cfg.PR;
	state.cpu.FSZ64 = fpu_cfg.SZ;
	state.cpu.RoundToZero = fpu_cfg.RM==1;
	...
```

**Contraste com `FRCHG`/`FSCHG`** (mesmo arquivo, mesmo repo):
```c
//fschg
sh4dec(i1111_0011_1111_1101)
{
	//fpscr.SZ is bit 20
	Emit(shop_xor,reg_fpscr,reg_fpscr,mk_imm(1<<20));
	state.cpu.FSZ64=!state.cpu.FSZ64;
}

//frchg
sh4dec(i1111_1011_1111_1101)
{
	Emit(shop_xor,reg_fpscr,reg_fpscr,mk_imm(1<<21));
	Emit(shop_mov32,reg_old_fpscr,reg_fpscr);
	shil_param rmn;
	Emit(shop_frswap,regv_xmtrx,regv_fmtrx,regv_xmtrx,0,rmn,regv_fmtrx);
}
```
Aqui **não tem `dec_End`** — o decodificador simplesmente inverte o bit que ele
mesmo rastreia em `state.cpu.FSZ64`/(implícito no banco pra `frchg`) e continua
decodificando o resto do bloco já especializado pro novo estado. Isso só é seguro
porque o efeito de `FRCHG`/`FSCHG` é **conhecido em tempo de compilação** (sempre
inverte o mesmo bit) — o decodificador consegue seguir esse estado estaticamente
sem precisar saber o valor real de nenhum registrador em runtime. Já `LDS
Rn,FPSCR` carrega um valor de um `Rn` qualquer — o decodificador não tem como
saber, em tempo de compilação, que `PR`/`SZ` vão valer depois daquela instrução,
então não tem como especializar corretamente o que vem depois dela no mesmo
bloco. Essa é a diferença estrutural entre os dois casos (retomo na seção 5).

**Lacuna que não consegui fechar com evidência direta:** o cache rápido de blocos
compilados (`bm_GetCode`/`FPCA`, em `core/hw/sh4/dyna/blockmanager.cpp`) parece
indexado **só por endereço**, sem `fpu_cfg` na chave — não vi comparação de
`fpu_cfg` nesse caminho rápido. Isso teoricamente deixaria uma brecha: se o mesmo
PC for reexecutado depois sob um FPSCR diferente (ex.: chegando por um branch, não
por fallthrough logo após o próprio `LDS Rn,FPSCR`), o bloco cacheado antigo
poderia ser reusado com a especialização errada. Não fui fundo o suficiente pra
confirmar se isso é tratado em outro lugar (ex.: nas condições de invalidação/SMC)
ou se é uma limitação aceita na prática (código SH4 gerado por compilador
raramente reentra o mesmo PC sob precisão diferente). Registro como pergunta em
aberto, não como bug confirmado — não tenho evidência de que isso cause problemas
reais no flycast atual.

---

## 3. Outros precedentes

### 3.1 redream (`inolen/redream`) — outro JIT SH4 independente, mesma conclusão

Fonte: `src/jit/frontend/sh4/sh4_frontend.c`, repositório `inolen/redream`
(`https://raw.githubusercontent.com/inolen/redream/master/src/jit/frontend/sh4/sh4_frontend.c`).

```c
static int sh4_frontend_is_terminator(struct jit_opdef *def) {
  /* stop emitting once a branch is hit */
  if (def->flags & SH4_FLAG_STORE_PC) {
    return 1;
  }

  /* if fpscr changed, stop as the compile-time assumptions may be invalid */
  if (def->flags & SH4_FLAG_STORE_FPSCR) {
    return 1;
  }

  return 0;
}
```

Mesmo padrão do flycast: uma escrita em FPSCR **termina o bloco** — o comentário
do próprio código explicita o motivo ("compile-time assumptions may be invalid").
`SH4_DOUBLE_PR`/`SH4_DOUBLE_SZ` são derivados do FPSCR real no início de cada
bloco (`ctx->fpscr & PR_MASK`/`SZ_MASK`), igual ao `fpu_cfg` do flycast.

**Redream vai um passo além do flycast** (na parte que consegui inspecionar): ele
insere uma **guarda em runtime no início do bloco**, não só no ponto de escrita.
Qualquer bloco que usou alguma otimização dependente do FPSCR (`use_fpscr`, setado
quando alguma instrução tem `SH4_FLAG_USE_FPSCR`) recebe um assert gerado no IR:

```c
/* if the block makes optimizations based on the fpscr state, assert that the
   run-time fpscr state matches the compile-time state */
if (use_fpscr) {
  ...
  struct ir_value *actual =
      ir_load_context(ir, offsetof(struct sh4_context, fpscr), VALUE_I32);
  actual = ir_and(ir, actual, ir_alloc_i32(ir, PR_MASK | SZ_MASK));
  struct ir_value *expected =
      ir_alloc_i32(ir, ctx->fpscr & (PR_MASK | SZ_MASK));
  ir_assert_eq(ir, actual, expected);
}
```

Isso é exatamente a técnica "guarda de compilação especulativa" que a tarefa
perguntou se existe: compila assumindo o modo vigente, mas insere uma checagem em
runtime pra pegar o caso em que a suposição não vale (ex.: o mesmo PC reentrado
sob outro FPSCR via branch — a lacuna que ficou em aberto pro flycast na seção
2). **Não consegui localizar/inspecionar a implementação de `ir_assert_eq`** (não
baixei o backend do redream) pra confirmar o que acontece exatamente quando o
assert falha (recompilação sob o modo certo? trap genérico?) — fica como detalhe
não confirmado, mas o padrão em si (assert de guarda + provável invalidação/
recompilação) é um precedente real e citável.

### 3.2 Dolphin (GameCube/Wii, PowerPC Gekko/Broadway) — dois casos distintos

O PowerPC não tem um bit equivalente exato ao `PR` do SH4 dentro do próprio
`FPSCR` (o `FPSCR` do PowerPC controla arredondamento/exceções, não decodificação
de instrução). O paralelo real com "modo que muda o significado de instruções FP
subsequentes" no PowerPC é o **registrador `GQR` (Graphics Quantization
Register)**, que controla o tipo/formato de dado das instruções `psq_l`/`psq_st`
("paired-single load/store quantized") — e, separadamente, o bit `PSE` de `HID2`
que liga/desliga o paired-single como um todo.

**Escrita em `GQR`: compilada nativamente, sem terminar bloco.** Fonte:
`Source/Core/Core/PowerPC/Jit64/Jit_SystemRegisters.cpp`
(`https://raw.githubusercontent.com/dolphin-emu/dolphin/master/Source/Core/Core/PowerPC/Jit64/Jit_SystemRegisters.cpp`),
função `Jit64::mtspr`:
```c
case SPR_GQR0:
case SPR_GQR0 + 1:
...
case SPR_GQR0 + 7:
  // These are safe to do the easy way, see the bottom of this function.
  break;
```
(o "bottom of this function" é um `MOV`/store direto pro campo de estado — sem
`FALLBACK_IF`, sem invalidar nada.)

**Quem lida com a especialização é o *consumidor* (`psq_l`/`psq_st`), não a
escrita.** Fonte: `Source/Core/Core/PowerPC/Jit64/Jit_LoadStorePaired.cpp`
(`https://raw.githubusercontent.com/dolphin-emu/dolphin/master/Source/Core/Core/PowerPC/Jit64/Jit_LoadStorePaired.cpp`):
```c
const bool gqrIsConstant = js.constantGqrValid[i];
if (gqrIsConstant)
{
  const u32 gqrValue = js.constantGqr[i] & 0xffff;
  int type = gqrValue & 0x7;
  ...
  GenQuantizedStore(true, static_cast<EQuantizeType>(type), (gqrValue & 0x3F00) >> 8);
}
else
{
  // não é constante: lê o GQR de verdade em runtime e despacha
  ...
  AND(32, R(RSCRATCH2), PPCSTATE_SPR(SPR_GQR0 + i));
  ...
}
```
`js.constantGqrValid[i]` vem de uma análise de **propagação de constante em tempo
de compilação do bloco**: se o compilador consegue provar que o `GQR[i]` usado por
essa instrução tem valor conhecido (normalmente porque um `mtspr` com valor
imediato apareceu antes, no mesmo bloco), ele gera código 100% especializado pro
tipo de quantização; senão, gera um caminho **genérico mas ainda nativo** que lê o
valor real do `GQR` e despacha em runtime (via uma tabela/switch dentro de
`GenQuantizedLoad`/`GenQuantizedStore`) — **sem nunca cair no interpretador**.
Essa é uma terceira técnica, distinta da "terminar bloco": **generalizar o código
nativo pra ler o modo em runtime**, quando dá pra fazer isso sem multiplicar
demais as variantes de código (aqui só são ~8 tipos de quantização, todos
tratáveis por uma função geradora comum).

Essa técnica não é diretamente portável pro par PR/SZ do SH4, porque lá a
"especialização" não é "que tipo de dado ler", é "qual layout de registro (par
FR/XF vs. registro único) e qual forma de instrução (single vs. double) o
restante da tradução em SHIL deveria ter gerado" — uma diferença estrutural bem
maior de shape de código, não só de uma constante numérica lida em runtime.
Registro isso como uma leitura minha da diferença arquitetural, não como algo que
achei documentado explicitamente em nenhuma fonte.

**Escrita em `HID2` (que inclui o bit `PSE`): cai inteiramente no fallback pro
interpretador**, igual ao que nosso fork faz hoje com `LDS Rn,FPSCR`. Confirmado
no mesmo `Jit_SystemRegisters.cpp`, no `switch` de `mtspr`: `HID2` **não aparece
em nenhum `case` explícito** — cai no `default: FALLBACK_IF(true);` do fim da
função. Ou seja, **mesmo um JIT tão maduro quanto o do Dolphin (20+ anos de
desenvolvimento) escolhe não compilar nativamente a escrita num registrador de
controle que liga/desliga um modo de decodificação de FPU** — trata exatamente
como o nosso fork trata `LDS Rn,FPSCR` hoje. Isso é evidência de que "sempre cair
no interpretador" para esse tipo especificamente arriscado de escrita **é, em si,
uma escolha de design legítima e usada por projetos de referência**, não
necessariamente uma lacuna a corrigir a qualquer custo.

Não achei (nem no código nem em busca por dev-blog/wiki do Dolphin) uma explicação
textual do motivo de `HID2` não ser tratado nativamente — é inferência minha, a
partir do padrão do código, que é porque mudar `PSE` afetaria como o restante do
bloco (e blocos futuros no mesmo endereço) deveriam ter sido compilados, o mesmo
problema estrutural do SH4. Sinalizo que essa parte é interpretação meu, não uma
citação direta de fonte que explique o "porquê".

### 3.3 PPSSPP (PSP, MIPS Allegrex) — caso oposto: FCR31 é compilado nativo, sem terminar bloco

Fonte: `Core/MIPS/IR/IRCompFPU.cpp`, repositório `hrydgard/ppsspp`
(`https://raw.githubusercontent.com/hrydgard/ppsspp/master/Core/MIPS/IR/IRCompFPU.cpp`),
função `Comp_mxc1` (que trata `mfc1`/`cfc1`/`mtc1`/`ctc1`):
```c
case 6: //ctc1
	if (fs == 31) {
		// Set rounding mode
		RestoreRoundingMode();
		ir.Write(IROp::FpCtrlFromReg, 0, rt);
		// TODO: Do the UpdateRoundingMode check at runtime?
		UpdateRoundingMode();
		ApplyRoundingMode();
	} else {
		INVALIDOP;
	}
	return;
```
`IROp::FpCtrlFromReg` é compilado nativamente pelos backends de IR (x86/ARM64) —
**sem terminar bloco, sem fallback**. O motivo é que os bits de `FCR31` que
importam pro PSP (modo de arredondamento + flush-to-zero, máscara `0x01000003`)
**não mudam como instruções FP subsequentes são decodificadas/traduzidas** — eles
só mudam como a **FPU de verdade do host** deveria arredondar. A técnica usada
(`Core/MIPS/MIPS.cpp`,
`https://raw.githubusercontent.com/hrydgard/ppsspp/master/Core/MIPS/MIPS.cpp`) é
mapear os bits do `FCR31` guest direto pros bits de controle da FPU real do host
(`MXCSR` em x86 via `_mm_setcsr`, `FPCR` em ARM64 via `msr fpcr`):
```c
void ApplyHostRoundingMode(const MIPSState *mips) {
	u32 fcr1Bits = mips->fcr31 & 0x01000003;
	if (fcr1Bits) {
		int rmode = fcr1Bits & 3;
		bool ftz = (fcr1Bits & 0x01000000) != 0;
#if PPSSPP_ARCH(ARM64)
		u64 fpcr = ARM64ReadFPCR();
		static const u8 lookup[4] = {0, 3, 1, 2};   // MIPS RM -> ARM64 FPCR RMode
		fpcr &= ~(3 << 22);
		fpcr |= ((u64)lookup[rmode] << 22);
		if (ftz) fpcr |= 1 << 24;
		ARM64WriteFPCR(fpcr);
#endif
	}
}
void RestoreHostRoundingMode() { /* limpa os bits de volta antes de voltar pro host code genérico */ }
```
Isso é elegante justamente porque **o hardware ARM64/x86 já tem, nativamente, os
mesmos graus de liberdade que o MIPS expõe em `FCR31`** (modos de arredondamento
IEEE-754 padrão + flush-to-zero) — então o JIT não precisa gerar código
diferente por modo, só precisa garantir que o *registro de controle da FPU do
host* esteja configurado igual ao guest antes de rodar o bloco, e restaurado
depois. Isso não existe pro par `PR`/`SZ` do SH4 porque `PR`/`SZ` não são "modo de
arredondamento" — são "quais registros e qual largura de dado a instrução
seguinte deveria ter operado", algo que a FPU do host não resolve sozinha
trocando um registro de controle; precisa mesmo de código nativo diferente.
(Curiosamente, isso sugere que o **bit `RM` do FPSCR do SH4** — que É puramente
modo de arredondamento, igual ao `FCR31` do MIPS — poderia em tese usar essa
mesma técnica de "mapear pro FPCR do host" independentemente de `PR`/`SZ`; achei
indício de que o flycast atual já faz algo parecido via
`Sh4Context::restoreHostRoundingMode()`, mas não confirmei os detalhes — ver
seção 2.)

---

## 4. Padrão geral de JIT pra "modo dinâmico que muda como o resto deveria ter sido compilado"

Juntando os quatro casos reais inspecionados (flycast SH4, redream SH4, Dolphin
PowerPC, PPSSPP MIPS), dá pra nomear duas técnicas canônicas distintas, cada
uma resolvendo um problema diferente — não é "uma técnica geral", são duas
famílias de solução pra dois problemas parecidos mas não idênticos:

**Técnica A — "a fronteira do bloco é a guarda" (usada por flycast e redream pro
par PR/SZ do SH4; a mesma ideia geral é citada informalmente em discussões sobre
troca de modo Thumb/ARM em JITs ARM32 e sobre x87-vs-SSE em JITs x86 antigos,
embora eu não tenha achado, nesta pesquisa, o código-fonte de um desses dois
últimos casos pra citar diretamente — menciono como analogia conhecida, não como
achado verificado nesta sessão):**
- O decodificador rastreia o modo **estaticamente**, em tempo de compilação, pra
  qualquer transição cujo efeito seja **previsível sem olhar um valor de
  runtime** (ex.: `FRCHG`/`FSCHG`, que sempre invertem o mesmo bit) — essas
  continuam no mesmo bloco, sem custo de terminação.
  Alicerce, se pertinente, para uma futura otimização (fora do escopo desta
  pesquisa): a mesma lógica que já existe pro sh4 aqui.
- Qualquer transição cujo efeito **dependa de um valor de runtime arbitrário**
  (registrador de GPR, memória) força o fim do bloco ali mesmo — não tenta
  "adivinhar" ou compilar uma versão genérica que sirva pros dois modos. O bloco
  seguinte é compilado do zero, sob demanda, lendo o modo real na hora.
- **Nenhum runtime branch/guard é estritamente necessário dentro do bloco que fez
  a mudança** — a proteção vem inteiramente de nunca ter havido a chance de gerar
  código mal-especializado, porque a decodificação simplesmente parou ali.
- Opcionalmente (redream faz isso, flycast não parece fazer, pelo que inspecionei),
  soma-se uma **guarda em runtime no início do próximo bloco**, pra cobrir o caso
  de reentrada por branch sob modo inesperado — esse é o componente que mais se
  parece com "guarda de compilação especulativa com fallback" no sentido que a
  tarefa perguntou.

**Técnica B — "mapeia pro registro de controle equivalente do host" (PPSSPP pro
FCR31/rounding mode; indício de uso parcial no flycast atual pro FPSCR.RM):**
- Só se aplica quando o bit controla **comportamento de arredondamento/exceção da
  FPU**, não **forma/layout do código gerado**. Nesse caso o hardware do host já
  resolve o problema — não precisa nem terminar bloco nem gerar variantes de
  código, só sincronizar o registro de controle da FPU real (MXCSR/FPCR) antes/depois
  de rodar código guest.

**Técnica C — "generaliza o código nativo, com constant-folding quando possível"
(Dolphin pro `GQR`):** quando o número de "modos" possíveis é pequeno e
tratável por uma função geradora comum (aqui, 8 tipos de quantização), dá pra
compilar um caminho nativo genérico que lê o modo em runtime e despacha, evitando
tanto o fallback quanto a explosão combinatória de terminar blocos o tempo todo —
mas com um pass de análise de constante em cima pra especializar quando possível.
Não parece ter sido aplicada por nenhum JIT de SH4 real ao par PR/SZ, provavelmente
porque a "forma" do código gerado (banco de registro FR/XF inteiro, largura de
dado) muda demais entre os dois modos pra caber numa função geradora comum barata
— mas é uma opção teórica que vale citar como existente no espaço de soluções.

**Onde `LDS Rn,FPSCR` do SH4 cai:** ele mexe em bits dos dois tipos ao mesmo
tempo — `PR`/`SZ`/`FR` (tipo "forma do código", exige Técnica A) e
`RM`/`DN`/`Cause`/`Enable`/`Flag` (tipo "controle de arredondamento/exceção",
poderia em tese usar Técnica B só pra essa parte). Como a mesma instrução escreve
os dois grupos de bits juntos, e o grupo `PR`/`SZ` é o que manda (é o que decide
que forma o resto do bloco deveria ter), **os dois JITs SH4 reais que
inspecionei tratam a instrução inteira com a Técnica A** — não tentam separar os
bits "seguros" dos "perigosos" dentro da mesma instrução.

---

## 5. Avaliação de segurança: dá pra compilar como "mov nativo simples", sem terminar o bloco?

**Não, com base na evidência coletada — nenhuma das duas implementações reais de
JIT SH4 examinadas (flycast atual, redream) faz isso, e ambas convergiram
independentemente na mesma solução (terminar o bloco), o que é um sinal forte de
que não é coincidência de projeto, é uma resposta a uma restrição real.**

Respondendo à pergunta específica da tarefa — "é comum `LDS Rn,FPSCR` ser seguida
imediatamente por instrução FP dependente de modo, dentro do mesmo bloco básico?":

- Não achei uma fonte que quantifique isso diretamente em código real de jogos SH4
  (não fui atrás de binários de jogo pra desmontar), então isto é inferência
  estrutural, não medição:
  `FRCHG`/`FSCHG` existem especificamente **porque** trocar de par/banco é comum o
  bastante no código quente (ex.: `FTRV`/multiplicação de matriz alternando entre
  banco de "fundo" e banco "visível", citado no próprio manual seção 6.6.1) pra
  justificar uma instrução dedicada de 1 ciclo. Isso é evidência indireta de que
  o *padrão contrário* também é plausível: `LDS Rn,FPSCR` tende a ser usado em
  pontos de fronteira mais "cirúrgicos" — troca de contexto, entrada/saída de
  rotina de precisão dupla, restauração de estado de FPU salvo — exatamente os
  lugares onde é mais natural que a instrução seguinte já dependa do novo modo
  (por exemplo, código gerado por compilador pra "entrar em modo double, fazer
  contas em double, sair de novo" tende a colocar a carga de `FPSCR` bem colada
  nas instruções que ela habilita). Não tenho uma métrica real disso — é raciocínio
  sobre o propósito da instrução, não uma contagem em disassembly de jogo.
- O próprio custo em hardware (4-5 ciclos, trava de pipeline — seção 1) é
  consistente com a instrução ser pensada como "operação cara e rara de fronteira
  de modo", não "hot loop toggle" — o que reforça que não é seguro assumir que o
  compilador escreveu `LDS Rn,FPSCR` num ponto qualquer sem instrução FP logo
  depois dependendo dela.

**Recomendação, com base nos precedentes reais (não invenção minha — é
literalmente o que flycast master e redream fazem):**

1. É seguro (e seria uma melhoria real sobre o fallback completo atual) compilar
   a **escrita bruta** nativamente — um `mov32`/`str` do valor do `Rn` pro campo
   de estado `fpscr` — exatamente como o flycast master faz com `shop_mov32`.
   Isso sozinho já tira o custo de sair do código JIT e entrar no interpretador
   completo pra fazer só uma cópia de 32 bits.
2. **Mas o bloco JIT precisa terminar logo depois**, igual ao `dec_End(...,
   BET_StaticJump, false)` do flycast master — sem essa parte, a ideia deixa de
   ser "o que o flycast master faz" e vira uma suposição não testada por nenhum
   dos dois projetos de referência encontrados. O próximo bloco (a partir da
   instrução seguinte) é recompilado do zero, lendo o FPSCR real no momento da
   compilação — isso já existe como conceito no fork (o padrão de "block ending"
   por `SetPC`/branches já é usado o tempo todo pelo dynarec pra outras
   instruções) e não exige inventar infraestrutura nova, só aplicar o padrão
   existente a mais um caso.
3. Se o fork quiser ir além do que o flycast master faz e cobrir a lacuna
   apontada na seção 2 (reentrada do mesmo PC sob FPSCR diferente via branch, não
   via fallthrough), a técnica do redream (assert de guarda no início de blocos
   que dependem de FPSCR, comparando o valor real com o assumido na compilação) é
   um precedente real e citável — mas não confirmei os detalhes de implementação
   dela (o que acontece quando o assert falha), então isso ficaria como extensão
   a investigar, não como receita pronta.
4. **Não acho seguro**, com a evidência que tenho, pular a etapa 2 (terminar o
   bloco) só porque a etapa 1 (o `mov` em si) é trivial de compilar — foi
   exatamente essa separação que os dois JITs SH4 reais examinados fizeram, e
   nenhum dos dois tentou "continuar no mesmo bloco, só cuidando pra não gerar
   código incompatível" pra este caso especificamente (viram um problema
   grande o bastante pra sempre terminar bloco, mesmo sendo dois times
   independentes).

---

## Fontes citadas

- Hitachi/Renesas, *SH-4 Programming Manual*, `ADE-602-156D` Rev. 5.0 (2001) —
  página institucional: https://www.renesas.com/en/document/mas/sh-4-software-manual?language=en&r=1055161
  — cópia usada nesta pesquisa: https://0x04.net/~mwk/doc/sh/e602156_sh4.pdf
  (seção 6.3.2 "Floating-Point Status/Control Register (FPSCR)", pg. 121; seção
  6.6.1 "Bank Switching and FPU Operation Instructions", pg. 125; seção 9.41
  `FRCHG`, 9.42 `FSCHG`; Tabela 8.3 "Execution Cycles", entradas #215/#217/#232/#233)
- `flyinghead/flycast` (master):
  - `core/hw/sh4/dyna/decoder.cpp` — https://raw.githubusercontent.com/flyinghead/flycast/master/core/hw/sh4/dyna/decoder.cpp
  - `core/hw/sh4/dyna/shil_canonical.h` — https://raw.githubusercontent.com/flyinghead/flycast/master/core/hw/sh4/dyna/shil_canonical.h
  - `core/hw/sh4/dyna/driver.cpp` — https://raw.githubusercontent.com/flyinghead/flycast/master/core/hw/sh4/dyna/driver.cpp
  - `core/hw/sh4/dyna/blockmanager.h`/`.cpp` — https://raw.githubusercontent.com/flyinghead/flycast/master/core/hw/sh4/dyna/blockmanager.h , https://raw.githubusercontent.com/flyinghead/flycast/master/core/hw/sh4/dyna/blockmanager.cpp
  - `core/hw/sh4/sh4_opcode_list.cpp` — https://raw.githubusercontent.com/flyinghead/flycast/master/core/hw/sh4/sh4_opcode_list.cpp
  - `core/hw/sh4/sh4_if.h` — https://raw.githubusercontent.com/flyinghead/flycast/master/core/hw/sh4/sh4_if.h
- `inolen/redream`:
  - `src/jit/frontend/sh4/sh4_frontend.c` — https://raw.githubusercontent.com/inolen/redream/master/src/jit/frontend/sh4/sh4_frontend.c
- `dolphin-emu/dolphin`:
  - `Source/Core/Core/PowerPC/Jit64/Jit_SystemRegisters.cpp` — https://raw.githubusercontent.com/dolphin-emu/dolphin/master/Source/Core/Core/PowerPC/Jit64/Jit_SystemRegisters.cpp
  - `Source/Core/Core/PowerPC/JitArm64/JitArm64_SystemRegisters.cpp` — https://raw.githubusercontent.com/dolphin-emu/dolphin/master/Source/Core/Core/PowerPC/JitArm64/JitArm64_SystemRegisters.cpp
  - `Source/Core/Core/PowerPC/Jit64/Jit_LoadStorePaired.cpp` — https://raw.githubusercontent.com/dolphin-emu/dolphin/master/Source/Core/Core/PowerPC/Jit64/Jit_LoadStorePaired.cpp
- `hrydgard/ppsspp`:
  - `Core/MIPS/IR/IRCompFPU.cpp` — https://raw.githubusercontent.com/hrydgard/ppsspp/master/Core/MIPS/IR/IRCompFPU.cpp
  - `Core/MIPS/MIPS.cpp` — https://raw.githubusercontent.com/hrydgard/ppsspp/master/Core/MIPS/MIPS.cpp

## O que fica em aberto (honestamente não investigado nesta sessão)

- Implementação real de `Sh4Context::UpdateFPSCR` e `restoreHostRoundingMode` no
  flycast master (só vi a declaração/uso, não o `.cpp` com o corpo).
- Implementação de `ir_assert_eq` no redream (não sei o que acontece exatamente
  quando a guarda falha — recompilação? invalidação de cache? trap?).
- Não fiz uma contagem real em disassembly de jogos SH4 pra confirmar com que
  frequência `LDS Rn,FPSCR` é seguida imediatamente por instrução FP dependente
  de modo — a seção 5 é inferência estrutural a partir do manual e do design das
  instruções, não uma medição.
- Não achei (nem busquei a fundo) o código-fonte de um JIT x86 antigo lidando com
  x87-vs-SSE nem de um JIT ARM32 lidando com troca Thumb/ARM pra citar
  diretamente — essas comparações na seção 4 ficam como analogia de conhecimento
  geral, não como achado desta pesquisa.
