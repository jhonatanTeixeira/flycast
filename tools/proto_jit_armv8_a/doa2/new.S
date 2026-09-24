// Regiao do nivel 2 escrita a mao: laco de vertices do DOA2 (12 blocos,
// 8C101BC2/8C101BC4 .. 8C101C4E), mesma semantica SH4 do JIT atual, bit a bit.
//
// Regras (cada uma e algo que um gerador faz so com o que ve na compilacao):
//  1. Ciclos: guarda por ponto de entrada com o custo do caminho mais longo ate
//     a proxima guarda (BC2/BC4: 23; C4E: 2+23); passou, os blocos originais
//     so subtraem (sem desvio); falhou, sai ANTES de executar para o bloco do
//     JIT antigo, que faz a checagem exata. Equivale a checar por bloco.
//  2. SH4 em callee-saved: r0 w19 r1 w20 r2 w21 r3 w22 r4 w23 r5 w24 r6 w25
//     r14 w26; fr8-10,fr12-15 (so leitura no laco) em s8-s10,s12-s15.
//     fr0-7 s16-s23, fr11 s24, fpul w14, XMTRX v4-v7 (caller-saved).
//     x27 base da memoria, x28 contexto, w29 ciclos.
//  3. A chamada da SQ (pref no TA) e chamada C completa: nada vivo em
//     caller-saved atravessa; fpul vai ao contexto antes; XMTRX (so leitura)
//     volta com um ld1. A carga de dr4 do C4E vai para depois da chamada
//     (a descarga da SQ so le a propria SQ; nao escreve RAM do guest).
//  4. T nao mora em registrador: vira o desvio; o valor de T em cada saida e
//     conhecido (C5C/C52: 1; guarda do C4E: 0).
//  5. r7 = r6 da cabeca da volta = r6 - 32 depois dos 4 fmov @-r6; nao ocupa
//     registrador. Os 4 fmov @-r6 viram uma base e 4 stp (guarda de wrap).
	.text
	.align 4
	.global new_run
	.global sq_stub
	.global g_membase
	.global g_sink
	.global g_cycles

// fipr: (a0*b0 + a1*b1) + (a2*b2 + a3*b3), ordem do JIT atual (fmul + faddp)
.macro FIPR d, a0, a1, a2, a3, b0, b1, b2, b3
	fmul s0, \a0, \b0
	fmul s1, \a1, \b1
	fadd s0, s0, s1
	fmul s1, \a2, \b2
	fmul s2, \a3, \b3
	fadd s1, s1, s2
	fadd \d, s0, s1
.endm
// par de floats: @reg (SZ=1)
.macro LDP2 a, b, reg
	add x0, x27, \reg, uxtw
	ldp \a, \b, [x0]
.endm
// pref de RAM (caso comum); SQ vai para o caminho frio que salva o que esta
// vivo em caller-saved (fr0-7, fr11, fpul) e recarrega a XMTRX
.macro PREF_RAM reg, id
	lsr w0, \reg, #26
	cmp w0, #0x38
	b.eq .Lsq_\id
	prfm pldl1keep, [x27, \reg, uxtw]
.Lret_\id:
	.pushsection .text.cold, "ax"
.Lsq_\id:
	sub sp, sp, #48
	stp s16, s17, [sp]
	stp s18, s19, [sp, #8]
	stp s20, s21, [sp, #16]
	stp s22, s23, [sp, #24]
	str s24, [sp, #32]
	str w14, [sp, #36]
	mov w0, \reg
	bl sq_stub
	ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x28]
	ldp s16, s17, [sp]
	ldp s18, s19, [sp, #8]
	ldp s20, s21, [sp, #16]
	ldp s22, s23, [sp, #24]
	ldr s24, [sp, #32]
	ldr w14, [sp, #36]
	add sp, sp, #48
	b .Lret_\id
	.popsection
.endm

new_run:
	stp x29, x30, [sp, #-160]!
	stp x19, x20, [sp, #16]
	stp x21, x22, [sp, #32]
	stp x23, x24, [sp, #48]
	stp x25, x26, [sp, #64]
	stp x27, x28, [sp, #80]
	stp d8, d9, [sp, #96]
	stp d10, d11, [sp, #112]
	stp d12, d13, [sp, #128]
	stp d14, d15, [sp, #144]
	mov x28, x0
	mov w29, w1
	add x27, x28, #0x1c0
	ldp w19, w20, [x28, #128]
	ldp w21, w22, [x28, #136]
	ldp w23, w24, [x28, #144]
	ldr w25, [x28, #152]
	ldr w26, [x28, #184]
	ldp s8, s9, [x28, #96]
	ldr s10, [x28, #104]
	ldp s12, s13, [x28, #112]
	ldp s14, s15, [x28, #120]
	ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x28]

// ---- 8C101BC2 (entrada): fmov @r14+,dr4 e cai no corpo do BC4
L_BC2:
	cmp w29, #23
	b.lt X_guard_BC2
	LDP2 s20, s21, w26
	add w26, w26, #8
	b L_BC4_body

// ---- 8C101BC4 (entrada vinda de fora)
L_BC4:
	cmp w29, #23
	b.lt X_guard_BC4
	ldp s20, s21, [x28, #80]		// fr4,fr5 do contexto (o BC2/C4E carregam)
L_BC4_body:
	sub w29, w29, #7
	LDP2 s22, s23, w26			// fmov @r14+,dr6
	add w26, w26, #8
	movi d0, #0
	fadd s18, s23, s0			// fldi0 fr2 ; fadd fr7,fr2
	fmov s23, #1.0				// fldi1 fr7
	add w25, w25, #64
	fmov s19, wzr				// fldi0 fr3
	add w24, w24, #32
	LDP2 s16, s17, w26			// fmov @r14+,dr0
	add w26, w26, #8
	fmul v0.4s, v4.4s, v20.s[0]		// ftrv xmtrx,fv4
	fmla v0.4s, v5.4s, v21.s[0]
	fmla v0.4s, v6.4s, v22.s[0]
	fmla v0.4s, v7.4s, v23.s[0]
	mov s20, v0.s[0]
	mov s21, v0.s[1]
	mov s22, v0.s[2]
	mov s23, v0.s[3]
	fmov s24, wzr				// fldi0 fr11
	ldr w19, [x27, w23, uxtw]		// mov.l @r4+,r0
	add w23, w23, #4
	mov w20, w26				// (slot) mov r14,r1
	subs w22, w22, #1			// dt r3
	b.eq X_C5C				// bt/s 8C101C5C

// ---- 8C101BE2
	sub w29, w29, #3
	FIPR s19, s16, s17, s18, s19, s12, s13, s14, s15	// fipr fv12,fv0
	ldr w26, [x27, w23, uxtw]		// mov.l @r4+,r14
	add w23, w23, #4
	fmov s23, #1.0				// (slot) fldi1 fr7
	tbnz w19, #0, L_C0C			// tst #1,r0 ; bf/s 8C101C0C

// ---- 8C101BEC (r0 par)
	sub w29, w29, #4
	fdiv s23, s23, s20			// fdiv fr4,fr7
	PREF_RAM w23, bec4			// pref @r4
	add w26, w26, w23			// add r4,r14
	PREF_RAM w26, bec14			// pref @r14 (antes do fipr: s1 e caller-saved)
	fmov w14, s15				// flds fr15,fpul
	FIPR s1, s8, s9, s10, s24, s16, s17, s18, s19	// fipr fv0,fv8 -> fr11
	fmov s3, s1
	fmov s18, s15				// (slot) fsts fpul,fr2
	fcmp s19, s24				// fcmp/gt fr11,fr3 (fr11 antigo)
	fmov s24, s3
	b.le L_C38				// bf/s 8C101C38
	sub w29, w29, #1			// 8C101BFE, cai no corpo comum do C20/C26
	b L_C20

// ---- 8C101C0C (r0 impar)
L_C0C:
	sub w29, w29, #5
	fdiv s23, s23, s20
	add w23, w23, #24
	sub w26, w23, #32			// mov r4,r14 ; add #-32,r14
	PREF_RAM w23, c0c4
	fmov w14, s15
	FIPR s1, s8, s9, s10, s24, s16, s17, s18, s19
	fmov s3, s1
	fmov s18, s15
	fcmp s19, s24
	fmov s24, s3
	b.le L_C38

// ---- 8C101C20 (e 8C101BFE + 8C101C26: mesmas ops)
L_C20:
	sub w29, w29, #4
	fadd s18, s18, s19			// fadd fr3,fr2
	fcvtzs w14, s24				// ftrc fr11,fpul
	fmov s19, wzr				// fldi0 fr3
	lsl w19, w14, #2			// sts fpul,r0 ; shll2 r0
	cmp w14, w21				// cmp/gt r2,r0
	b.le L_C3A				// bf 8C101C3A
// ---- 8C101C2E: fschg ; fmov.s @(r0,r8),fr3 ; bra ; fschg
	sub w29, w29, #1
	ldr w0, [x28, #160]			// r8 (so leitura, rara)
	add w0, w0, w19
	ldr s19, [x27, w0, uxtw]
	b L_C3A

// ---- 8C101C38: fldi0 fr3 + corpo do C3A (mesmos 3 ciclos)
L_C38:
	fmov s19, wzr
// ---- 8C101C3A
L_C3A:
	sub w29, w29, #3
	LDP2 s16, s17, w20			// fmov @r1,dr0
	fmul s22, s22, s23			// fmul fr7,fr6
	fmul s21, s21, s23			// fmul fr7,fr5
	cmp w25, #32				// 4x fmov @-r6 numa base (sem wrap)
	b.lo X_wrap
	sub w25, w25, #32
	add x0, x27, w25, uxtw
	stp s18, s19, [x0, #24]			// fmov dr2,@-r6
	stp s16, s17, [x0, #16]			// fmov dr0,@-r6
	stp s22, s23, [x0, #8]			// fmov dr6,@-r6
	stp s20, s21, [x0]			// fmov dr4,@-r6
	sub w1, w25, #32			// (slot) mov.l r7,@r6 ; r7 = r6 da cabeca
	str w1, [x0]
	cbz w22, X_C52				// tst r3,r3 ; bt/s 8C101C52

// ---- 8C101C4E: fmov @r14+,dr4 ; bra 8C101BC4 ; (slot) pref @r6
	cmp w29, #25
	b.lt X_guard_C4E
	sub w29, w29, #2
	lsr w0, w25, #26
	cmp w0, #0x38
	b.ne L_c4e_ram
	str w14, [x28, #260]			// fpul vivo na saida
	mov w0, w25
	bl sq_stub
	ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x28]
L_c4e_dr4:
	LDP2 s20, s21, w26
	add w26, w26, #8
	b L_BC4_body
L_c4e_ram:
	prfm pldl1keep, [x27, w25, uxtw]
	str w14, [x28, #260]
	b L_c4e_dr4

// ---- saidas: grava o que mudou e devolve o pc do bloco seguinte
// fr0-7 e fr11 sempre recalculados no caminho ate a saida; fpul ja esta no
// contexto (C4E) ou em w14 (C52, guarda do C4E); r7 = r6 da cabeca.
X_C5C:						// saiu no BC4: T=1, r7 = r6 - 64
	sub w1, w25, #64
	str w1, [x28, #156]
	mov w2, #1
	mov w0, #0x1C5C
	b X_common
X_C52:						// T=1, r7 = w1 (calculado no C3A)
	str w1, [x28, #156]
	str w14, [x28, #260]
	mov w2, #1
	mov w0, #0x1C52
	b X_common
X_guard_C4E:					// antes do C4E: T=0 (bt nao tomado)
	str w1, [x28, #156]
	str w14, [x28, #260]
	mov w2, #0
	mov w0, #0x1C4E
	b X_common
X_guard_BC2:					// nada executado
	mov w0, #0x1BC2
	b X_nochange
X_guard_BC4:
	mov w0, #0x1BC4
	b X_nochange
X_wrap:
	brk #1					// r6 < 32: fora do alcance deste prototipo
X_common:
	stp w19, w20, [x28, #128]
	stp w21, w22, [x28, #136]
	stp w23, w24, [x28, #144]
	str w25, [x28, #152]
	str w26, [x28, #184]
	str w2, [x28, #276]
	stp s16, s17, [x28, #64]
	stp s18, s19, [x28, #72]
	stp s20, s21, [x28, #80]
	stp s22, s23, [x28, #88]
	str s24, [x28, #108]
X_nochange:
	movk w0, #0x8C10, lsl #16
	adrp x9, g_cycles
	str w29, [x9, :lo12:g_cycles]
	ldp d14, d15, [sp, #144]
	ldp d12, d13, [sp, #128]
	ldp d10, d11, [sp, #112]
	ldp d8, d9, [sp, #96]
	ldp x27, x28, [sp, #80]
	ldp x25, x26, [sp, #64]
	ldp x23, x24, [sp, #48]
	ldp x21, x22, [sp, #32]
	ldp x19, x20, [sp, #16]
	ldp x29, x30, [sp], #160
	ret

// stub da SQ (os dois lados usam): copia os 32 bytes da fila para o "TA"
sq_stub:
	adrp x3, g_membase
	ldr x3, [x3, :lo12:g_membase]
	and w0, w0, #0xFFFFFFE0
	add x3, x3, x0
	ldp q0, q1, [x3]
	adrp x3, g_sink
	add x3, x3, :lo12:g_sink
	ldr x2, [x3]
	stp q0, q1, [x2], #32
	str x2, [x3]
	ret

	.data
	.align 3
g_membase: .quad 0
g_sink: .quad 0
g_cycles: .word 0
