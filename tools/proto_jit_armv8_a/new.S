// Prototipo jit_armv8_a do laco de vertices do Shenmue II (8C1D8BDA..8C1D8C82),
// escrito a mao com a mesma semantica SH4 dos 11 blocos do JIT atual.
//   r0 w19  r1 w20  r2 w21  r3 w22  r4 w23  r5 w24  r6 w25  r14 w26
//   T w15   fpul w14   ciclos w12   base da memoria x27   contexto x28
//   frN = s(16+N)   XMTRX = v4..v7 (xf0-3, xf4-7, xf8-11, xf12-15)
// sq_stub so suja x0, x2, x3, v0, v1, x30.
	.text
	.align 4
	.global new_run
	.global sq_stub
	.global g_membase
	.global g_sink

// fipr f3 <- fv0 . fv12, na mesma ordem do JIT atual: (p0+p1)+(p2+p3)
.macro FIPR_F3
	fmul s0, s16, s28
	fmul s1, s17, s29
	fadd s0, s0, s1
	fmul s1, s18, s30
	fmul s2, s19, s31
	fadd s1, s1, s2
	fadd s19, s0, s1
.endm
// ld/st de par de floats (fmov com SZ=1): endereco = base + reg
.macro LDP2 a, b, reg
	add x0, x27, \reg, uxtw
	ldp \a, \b, [x0]
.endm
.macro STP2 a, b, reg
	add x0, x27, \reg, uxtw
	stp \a, \b, [x0]
.endm
// pref @reg: SQ -> stub (TA), senao prfm
.macro PREF reg
	lsr w0, \reg, #26
	cmp w0, #0x38
	b.ne .Lnsq\@
	mov w0, \reg
	bl sq_stub
	b .Ldone\@
.Lnsq\@:
	prfm pldl1keep, [x27, \reg, uxtw]
.Ldone\@:
.endm
// ftrv fvN: XMTRX * fvN, mesmo fmul/fmla do JIT atual
.macro FTRV a, b, c, d, va, vb, vc, vd
	fmul v0.4s, v4.4s, \va\().s[0]
	fmla v0.4s, v5.4s, \vb\().s[0]
	fmla v0.4s, v6.4s, \vc\().s[0]
	fmla v0.4s, v7.4s, \vd\().s[0]
	mov \a, v0.s[0]
	mov \b, v0.s[1]
	mov \c, v0.s[2]
	mov \d, v0.s[3]
.endm

new_run:
	stp x29, x30, [sp, #-96]!
	stp x19, x20, [sp, #16]
	stp x21, x22, [sp, #32]
	stp x23, x24, [sp, #48]
	stp x25, x26, [sp, #64]
	stp x27, x28, [sp, #80]
	mov x28, x0
	mov w12, w1
	add x27, x28, #0x1c0
	ldp w19, w20, [x28, #128]
	ldp w21, w22, [x28, #136]
	ldp w23, w24, [x28, #144]
	ldr w25, [x28, #152]
	ldr w26, [x28, #184]
	ldr w14, [x28, #260]
	ldr w15, [x28, #276]
	ldp s16, s17, [x28, #64]
	ldp s18, s19, [x28, #72]
	ldp s20, s21, [x28, #80]
	ldp s22, s23, [x28, #88]
	ldp s24, s25, [x28, #96]
	ldp s26, s27, [x28, #104]
	ldp s28, s29, [x28, #112]
	ldp s30, s31, [x28, #120]
	ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x28]

L_8BDA:
	subs w12, w12, #86			// ciclos da volta inteira, uma checagem
	b.mi X_sched
	ldr w19, [x27, w23, uxtw]		// r0 = @r4+
	add w23, w23, #4
	sub w22, w22, #1			// dt r3
	cmp w22, #0
	cset w15, eq
	fmov w14, s31				// flds fr15, fpul
	fmov s23, #1.0				// fldi1 fr7
	fdiv s23, s23, s20			// fr7 = 1/fr4
	ldr w20, [x27, w23, uxtw]		// r1 = @r4+
	add w23, w23, #4
	FIPR_F3					// delay slot do bt.s
	cbnz w15, X_8CB4
// 8BEA: tst #1,r0 ; add r4,r1 ; bt.s 8BF8 ; fsts fpul,fr2
	tst w19, #1
	cset w15, eq
	add w20, w20, w23
	fmov s18, w14
	sub w0, w23, #8				// 8BF2 (T=0): r1 = r4-8 ; r4 += 24
	add w1, w23, #24
	csel w20, w20, w0, eq
	csel w23, w23, w1, eq
// 8BF8
	fmov s16, wzr				// fldi0 fr0
	fcmp s19, s16				// fcmp/gt fr0,fr3
	cset w15, gt
	LDP2 s16, s17, w26			// fr0,fr1 = @r14
	add w23, w23, #32
	LDP2 s24, s25, w20			// fr8,fr9 = @r1+
	add w20, w20, #8
	fadd s0, s18, s19			// bf +0: fadd fr3,fr2 so se T=1
	cmp w15, #0
	fcsel s18, s0, s18, ne
// 8C06
	fmov s19, wzr				// fldi0 fr3
	sub w22, w22, #1			// dt r3
	cmp w22, #0
	cset w15, eq
	sub w25, w25, #8
	STP2 s18, s19, w25			// fmov dr2,@-r6
	fsub s18, s18, s18
	PREF w23
	sub w23, w23, #32
	LDP2 s26, s27, w20			// fr10,fr11 = @r1+
	add w20, w20, #8
	fmul s22, s22, s23
	sub w25, w25, #8
	STP2 s16, s17, w25			// fmov dr0,@-r6
	fadd s18, s18, s27
	fmov s27, #1.0
	fmul s21, s21, s23
	LDP2 s16, s17, w20			// fr0,fr1 = @r1+
	add w20, w20, #8
	add w24, w24, #32
	sub w25, w25, #8
	STP2 s22, s23, w25			// fmov dr6,@-r6
	FTRV s24, s25, s26, s27, v24, v25, v26, v27
	sub w25, w25, #8
	STP2 s20, s21, w25			// fmov dr4,@-r6
	str w21, [x27, w25, uxtw]		// mov.l r2,@r6
	mov w21, w25
	PREF w25
	add w25, w25, #64
	ldr w19, [x27, w23, uxtw]		// r0 = @r4+
	add w23, w23, #4
	ldr w26, [x27, w23, uxtw]		// r14 = @r4+
	add w23, w23, #4
	fmov w14, s31
	FIPR_F3
	fmov s27, #1.0
	fdiv s27, s27, s24			// delay slot do bt.s
	cbnz w15, X_8C84
// 8C3E: tst #1,r0 ; add r4,r14 ; bt.s 8C4C ; fsts fpul,fr2
	tst w19, #1
	cset w15, eq
	add w26, w26, w23
	fmov s18, w14
	sub w0, w23, #8				// 8C46 (T=0): r14 = r4-8 ; r4 += 24
	add w1, w23, #24
	csel w26, w26, w0, eq
	csel w23, w23, w1, eq
// 8C4C
	fmov s16, wzr
	fcmp s19, s16
	cset w15, gt
	LDP2 s16, s17, w20			// fr0,fr1 = @r1
	add w23, w23, #32
	LDP2 s20, s21, w26			// fr4,fr5 = @r14+
	add w26, w26, #8
	fadd s0, s18, s19			// bf +0
	cmp w15, #0
	fcsel s18, s0, s18, ne
// 8C5A
	fmov s19, wzr
	sub w25, w25, #8
	STP2 s18, s19, w25
	fsub s18, s18, s18
	PREF w23
	sub w23, w23, #32
	LDP2 s22, s23, w26			// fr6,fr7 = @r14+
	add w26, w26, #8
	fmul s26, s26, s27
	sub w25, w25, #8
	STP2 s16, s17, w25
	fadd s18, s18, s23
	fmov s23, #1.0
	fmul s25, s25, s27
	LDP2 s16, s17, w26			// fr0,fr1 = @r14+
	add w26, w26, #8
	add w24, w24, #32
	sub w25, w25, #8
	STP2 s26, s27, w25			// fmov dr10,@-r6
	FTRV s20, s21, s22, s23, v20, v21, v22, v23
	sub w25, w25, #8
	STP2 s24, s25, w25			// fmov dr8,@-r6
	str w21, [x27, w25, uxtw]
	mov w21, w25
	PREF w25
	add w25, w25, #64			// delay slot do bra
	b L_8BDA

X_sched:
	mov w0, #0xFFFF
	b X_common
X_8CB4:
	mov w0, #0x8CB4
	movk w0, #0x8C1D, lsl #16
	b X_common
X_8C84:
	mov w0, #0x8C84
	movk w0, #0x8C1D, lsl #16
X_common:
	stp w19, w20, [x28, #128]
	stp w21, w22, [x28, #136]
	stp w23, w24, [x28, #144]
	str w25, [x28, #152]
	str w26, [x28, #184]
	str w14, [x28, #260]
	str w15, [x28, #276]
	stp s16, s17, [x28, #64]
	stp s18, s19, [x28, #72]
	stp s20, s21, [x28, #80]
	stp s22, s23, [x28, #88]
	stp s24, s25, [x28, #96]
	stp s26, s27, [x28, #104]
	stp s28, s29, [x28, #112]
	stp s30, s31, [x28, #120]
	ldp x27, x28, [sp, #80]
	ldp x25, x26, [sp, #64]
	ldp x23, x24, [sp, #48]
	ldp x21, x22, [sp, #32]
	ldp x19, x20, [sp, #16]
	ldp x29, x30, [sp], #96
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
