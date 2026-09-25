// GERADO por tools/tier2_gen.py --emu doa2 (nao editar a mao): regiao do nivel 2
// --emu doa2 --entries 8C101BC4 <dump do DOA2, captura s3000> core/rec-ARM64/tier2_doa2.S 8C101BC2 8C101BC4 8C101BE2 8C101BEC 8C101BFE 8C101C0C 8C101C20 8C101C26 8C101C2E 8C101C38 8C101C3A 8C101C4E
	.text
	.align 4
	.global t2_doa2_begin
	.hidden t2_doa2_begin
t2_doa2_begin:
	.global t2_doa2_E_8C101BC2
	.hidden t2_doa2_E_8C101BC2
t2_doa2_E_8C101BC2:
region_begin:
E_8C101BC2:
	add x13, x28, #0x1c0
	ldr w15, [x28, #276]
	ldr s16, [x28, #64]
	ldr s17, [x28, #68]
	ldr s10, [x28, #104]
	ldr s24, [x28, #108]
	ldr s11, [x28, #112]
	ldr s12, [x28, #116]
	ldr s13, [x28, #120]
	ldr s14, [x28, #124]
	ldr s18, [x28, #72]
	ldr s19, [x28, #76]
	ldr s20, [x28, #80]
	ldr s21, [x28, #84]
	ldr s22, [x28, #88]
	ldr s23, [x28, #92]
	ldr s8, [x28, #96]
	ldr s9, [x28, #100]
	ldr w14, [x28, #260]
	ldr w19, [x28, #128]
	ldr w20, [x28, #132]
	ldr w26, [x28, #184]
	ldr w9, [x28, #136]
	ldr w21, [x28, #140]
	ldr w22, [x28, #144]
	ldr w23, [x28, #148]
	ldr w24, [x28, #152]
	ldr w25, [x28, #156]
	ldr w10, [x28, #160]
	ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x28]
	cmp w27, #23
	b.lt .LR1
	b B_8C101BC2
	.global t2_doa2_E_8C101BC4
	.hidden t2_doa2_E_8C101BC4
t2_doa2_E_8C101BC4:
E_8C101BC4:
	add x13, x28, #0x1c0
	ldr w15, [x28, #276]
	ldr s16, [x28, #64]
	ldr s17, [x28, #68]
	ldr s10, [x28, #104]
	ldr s24, [x28, #108]
	ldr s11, [x28, #112]
	ldr s12, [x28, #116]
	ldr s13, [x28, #120]
	ldr s14, [x28, #124]
	ldr s18, [x28, #72]
	ldr s19, [x28, #76]
	ldr s20, [x28, #80]
	ldr s21, [x28, #84]
	ldr s22, [x28, #88]
	ldr s23, [x28, #92]
	ldr s8, [x28, #96]
	ldr s9, [x28, #100]
	ldr w14, [x28, #260]
	ldr w19, [x28, #128]
	ldr w20, [x28, #132]
	ldr w26, [x28, #184]
	ldr w9, [x28, #136]
	ldr w21, [x28, #140]
	ldr w22, [x28, #144]
	ldr w23, [x28, #148]
	ldr w24, [x28, #152]
	ldr w25, [x28, #156]
	ldr w10, [x28, #160]
	ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x28]
	cmp w27, #23
	b.lt .LR2
	b B_8C101BC4
B_8C101BC2:
	sub w27, w27, #7
	add x0, x13, w26, uxtw
	ldp s20, s21, [x0]
	add w26, w26, #8
	add x0, x13, w26, uxtw
	ldp s22, s23, [x0]
	add w26, w26, #8
	mov w25, w24
	fmov s1, wzr
	fadd s18, s23, s1
	fmov s23, #1.0
	add w24, w24, #64
	fmov s19, wzr
	add w23, w23, #32
	add x0, x13, w26, uxtw
	ldp s16, s17, [x0]
	add w26, w26, #8
	fmul v0.4s, v4.4s, v20.s[0]
	fmla v0.4s, v5.4s, v21.s[0]
	fmla v0.4s, v6.4s, v22.s[0]
	fmla v0.4s, v7.4s, v23.s[0]
	mov s20, v0.s[0]
	mov s21, v0.s[1]
	mov s22, v0.s[2]
	mov s23, v0.s[3]
	fmov s24, wzr
	sub w21, w21, #1
	cmp w21, #0
	cset w15, eq
	ldr w19, [x13, w22, uxtw]
	add w22, w22, #4
	mov w20, w26
	cbnz w15, .LX3
B_8C101BE2:
	sub w27, w27, #3
	fmul s0, s16, s11
	fmul s1, s17, s12
	fadd s0, s0, s1
	fmul s1, s18, s13
	fmul s2, s19, s14
	fadd s1, s1, s2
	fadd s19, s0, s1
	tst w19, #0x1
	cset w15, eq
	ldr w26, [x13, w22, uxtw]
	add w22, w22, #4
	fmov s23, #1.0
	cbnz w15, B_8C101BEC
B_8C101C0C:
	sub w27, w27, #5
	fdiv s23, s23, s20
	add w22, w22, #24
	lsr w0, w22, #26
	cmp w0, #0x38
	b.eq .Lsq4
	prfm pldl1keep, [x13, w22, uxtw]
.Lpd5:
	fcmp s19, s24
	cset w15, gt
	fmov w14, s14
	fmul s0, s8, s16
	fmul s1, s9, s17
	fadd s0, s0, s1
	fmul s1, s10, s18
	fmul s2, s24, s19
	fadd s1, s1, s2
	fadd s24, s0, s1
	sub w26, w22, #32
	fmov s18, w14
	cbnz w15, B_8C101C20
B_8C101C38:
	sub w1, w24, #32
	lsr w0, w1, #26
	cmp w0, #0x38
	b.ne .LX6
	and w2, w1, #0x3f
	cmp w2, #32
	b.hi .LX6
	sub x16, x28, #0x40
	add x16, x16, x2
	sub w27, w27, #3
	fmov s19, wzr
	add x0, x13, w20, uxtw
	ldp s16, s17, [x0]
	fmul s22, s22, s23
	sub w24, w24, #8
	stp s18, s19, [x16, #24]
	fmul s21, s21, s23
	sub w24, w24, #8
	stp s16, s17, [x16, #16]
	tst w21, w21
	cset w15, eq
	sub w24, w24, #8
	stp s22, s23, [x16, #8]
	sub w24, w24, #8
	stp s20, s21, [x16, #0]
	str w25, [x16, #0]
	cbnz w15, .LX7
B_8C101C4E:
	cmp w27, #25
	b.lt .LX8
	sub w27, w27, #2
	lsr w0, w24, #26
	cmp w0, #0x38
	b.ne .Lram11
	str w14, [x28, #260]
	mov w0, w24
	sub x9, x28, #0x48
	ldr x9, [x9]
	sub x1, x28, #0x40
	blr x9
	add x13, x28, #0x1c0
	ldr w14, [x28, #260]
	ldr w9, [x28, #136]
	ldr w10, [x28, #160]
	ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x28]
.Lpd10:
	add x0, x13, w26, uxtw
	ldp s20, s21, [x0]
	add w26, w26, #8
B_8C101BC4:
	sub w27, w27, #7
	add x0, x13, w26, uxtw
	ldp s22, s23, [x0]
	add w26, w26, #8
	mov w25, w24
	fmov s1, wzr
	fadd s18, s23, s1
	fmov s23, #1.0
	add w24, w24, #64
	fmov s19, wzr
	add w23, w23, #32
	add x0, x13, w26, uxtw
	ldp s16, s17, [x0]
	add w26, w26, #8
	fmul v0.4s, v4.4s, v20.s[0]
	fmla v0.4s, v5.4s, v21.s[0]
	fmla v0.4s, v6.4s, v22.s[0]
	fmla v0.4s, v7.4s, v23.s[0]
	mov s20, v0.s[0]
	mov s21, v0.s[1]
	mov s22, v0.s[2]
	mov s23, v0.s[3]
	fmov s24, wzr
	sub w21, w21, #1
	cmp w21, #0
	cset w15, eq
	ldr w19, [x13, w22, uxtw]
	add w22, w22, #4
	mov w20, w26
	cbnz w15, .LX12
	b B_8C101BE2
B_8C101BEC:
	sub w27, w27, #4
	lsr w0, w22, #26
	cmp w0, #0x38
	b.eq .Lsq13
	prfm pldl1keep, [x13, w22, uxtw]
.Lpd14:
	fdiv s23, s23, s20
	fcmp s19, s24
	cset w15, gt
	add w26, w26, w22
	lsr w0, w26, #26
	cmp w0, #0x38
	b.eq .Lsq15
	prfm pldl1keep, [x13, w26, uxtw]
.Lpd16:
	fmov w14, s14
	fmul s0, s8, s16
	fmul s1, s9, s17
	fadd s0, s0, s1
	fmul s1, s10, s18
	fmul s2, s24, s19
	fadd s1, s1, s2
	fadd s24, s0, s1
	fmov s18, w14
	cbz w15, B_8C101C38
B_8C101BFE:
	sub w27, w27, #1
	fadd s18, s18, s19
	fmov s19, wzr
	fcvtzs w14, s24
B_8C101C26:
	sub w27, w27, #4
	cmp w14, w9
	cset w15, gt
	lsl w19, w14, #2
	cbnz w15, B_8C101C2E
B_8C101C3A:
	sub w1, w24, #32
	lsr w0, w1, #26
	cmp w0, #0x38
	b.ne .LX17
	and w2, w1, #0x3f
	cmp w2, #32
	b.hi .LX17
	sub x16, x28, #0x40
	add x16, x16, x2
	sub w27, w27, #3
	add x0, x13, w20, uxtw
	ldp s16, s17, [x0]
	fmul s22, s22, s23
	sub w24, w24, #8
	stp s18, s19, [x16, #24]
	fmul s21, s21, s23
	sub w24, w24, #8
	stp s16, s17, [x16, #16]
	tst w21, w21
	cset w15, eq
	sub w24, w24, #8
	stp s22, s23, [x16, #8]
	sub w24, w24, #8
	stp s20, s21, [x16, #0]
	str w25, [x16, #0]
	cbnz w15, .LX18
	b B_8C101C4E
B_8C101C20:
	sub w27, w27, #4
	fadd s18, s18, s19
	fcvtzs w14, s24
	fmov s19, wzr
	cmp w14, w9
	cset w15, gt
	lsl w19, w14, #2
	cbz w15, B_8C101C3A
B_8C101C2E:
	sub w27, w27, #1
	add w0, w10, w19
	ldr s19, [x13, w0, uxtw]
	b B_8C101C3A
	.p2align 4
region_hot_end:
.LR1:
	adrp x0, t2_resume_8C101BC2
	ldr x0, [x0, :lo12:t2_resume_8C101BC2]
	br x0
.LR2:
	adrp x0, t2_resume_8C101BC4
	ldr x0, [x0, :lo12:t2_resume_8C101BC4]
	br x0
.LX3:
	str w14, [x28, #260]
	str s16, [x28, #64]
	str s17, [x28, #68]
	str s18, [x28, #72]
	str s19, [x28, #76]
	str s20, [x28, #80]
	str s21, [x28, #84]
	str s22, [x28, #88]
	str s23, [x28, #92]
	str s24, [x28, #108]
	str w19, [x28, #128]
	str w20, [x28, #132]
	str w21, [x28, #140]
	str w22, [x28, #144]
	str w23, [x28, #148]
	str w24, [x28, #152]
	str w25, [x28, #156]
	str w26, [x28, #184]
	mov w1, #1
	str w1, [x28, #276]
	mov w29, #0x1C5C
	movk w29, #0x8C10, lsl #16
	str w29, [x28, #264]
	adrp x0, t2_no_update
	ldr x0, [x0, :lo12:t2_no_update]
	br x0
.Lsq4:
	str s16, [x28, #64]
	str s17, [x28, #68]
	str s24, [x28, #108]
	str s18, [x28, #72]
	str s19, [x28, #76]
	str s20, [x28, #80]
	str s21, [x28, #84]
	str s22, [x28, #88]
	str s23, [x28, #92]
	mov w0, w22
	sub x9, x28, #0x48
	ldr x9, [x9]
	sub x1, x28, #0x40
	blr x9
	add x13, x28, #0x1c0
	ldr s16, [x28, #64]
	ldr s17, [x28, #68]
	ldr s24, [x28, #108]
	ldr s18, [x28, #72]
	ldr s19, [x28, #76]
	ldr s20, [x28, #80]
	ldr s21, [x28, #84]
	ldr s22, [x28, #88]
	ldr s23, [x28, #92]
	ldr w9, [x28, #136]
	ldr w10, [x28, #160]
	ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x28]
	b .Lpd5
.LX6:
	str w14, [x28, #260]
	str s16, [x28, #64]
	str s17, [x28, #68]
	str s18, [x28, #72]
	str s19, [x28, #76]
	str s20, [x28, #80]
	str s21, [x28, #84]
	str s22, [x28, #88]
	str s23, [x28, #92]
	str s24, [x28, #108]
	str w19, [x28, #128]
	str w20, [x28, #132]
	str w21, [x28, #140]
	str w22, [x28, #144]
	str w23, [x28, #148]
	str w24, [x28, #152]
	str w25, [x28, #156]
	str w26, [x28, #184]
	str w15, [x28, #276]
	mov w29, #0x1C38
	movk w29, #0x8C10, lsl #16
	str w29, [x28, #264]
	adrp x0, t2_no_update
	ldr x0, [x0, :lo12:t2_no_update]
	br x0
.LX7:
	str w14, [x28, #260]
	str s16, [x28, #64]
	str s17, [x28, #68]
	str s18, [x28, #72]
	str s19, [x28, #76]
	str s20, [x28, #80]
	str s21, [x28, #84]
	str s22, [x28, #88]
	str s23, [x28, #92]
	str s24, [x28, #108]
	str w19, [x28, #128]
	str w20, [x28, #132]
	str w21, [x28, #140]
	str w22, [x28, #144]
	str w23, [x28, #148]
	str w24, [x28, #152]
	str w25, [x28, #156]
	str w26, [x28, #184]
	mov w1, #1
	str w1, [x28, #276]
	mov w29, #0x1C52
	movk w29, #0x8C10, lsl #16
	str w29, [x28, #264]
	adrp x0, t2_no_update
	ldr x0, [x0, :lo12:t2_no_update]
	br x0
.LX8:
	str w14, [x28, #260]
	str s16, [x28, #64]
	str s17, [x28, #68]
	str s18, [x28, #72]
	str s19, [x28, #76]
	str s20, [x28, #80]
	str s21, [x28, #84]
	str s22, [x28, #88]
	str s23, [x28, #92]
	str s24, [x28, #108]
	str w19, [x28, #128]
	str w20, [x28, #132]
	str w21, [x28, #140]
	str w22, [x28, #144]
	str w23, [x28, #148]
	str w24, [x28, #152]
	str w25, [x28, #156]
	str w26, [x28, #184]
	str w15, [x28, #276]
	mov w29, #0x1C4E
	movk w29, #0x8C10, lsl #16
	str w29, [x28, #264]
	adrp x0, t2_no_update
	ldr x0, [x0, :lo12:t2_no_update]
	br x0
.Lram11:
	prfm pldl1keep, [x13, w24, uxtw]
	b .Lpd10
.LX12:
	str w14, [x28, #260]
	str s16, [x28, #64]
	str s17, [x28, #68]
	str s18, [x28, #72]
	str s19, [x28, #76]
	str s20, [x28, #80]
	str s21, [x28, #84]
	str s22, [x28, #88]
	str s23, [x28, #92]
	str s24, [x28, #108]
	str w19, [x28, #128]
	str w20, [x28, #132]
	str w21, [x28, #140]
	str w22, [x28, #144]
	str w23, [x28, #148]
	str w24, [x28, #152]
	str w25, [x28, #156]
	str w26, [x28, #184]
	mov w1, #1
	str w1, [x28, #276]
	mov w29, #0x1C5C
	movk w29, #0x8C10, lsl #16
	str w29, [x28, #264]
	adrp x0, t2_no_update
	ldr x0, [x0, :lo12:t2_no_update]
	br x0
.Lsq13:
	str s16, [x28, #64]
	str s17, [x28, #68]
	str s24, [x28, #108]
	str s18, [x28, #72]
	str s19, [x28, #76]
	str s20, [x28, #80]
	str s21, [x28, #84]
	str s22, [x28, #88]
	str s23, [x28, #92]
	mov w0, w22
	sub x9, x28, #0x48
	ldr x9, [x9]
	sub x1, x28, #0x40
	blr x9
	add x13, x28, #0x1c0
	ldr s16, [x28, #64]
	ldr s17, [x28, #68]
	ldr s24, [x28, #108]
	ldr s18, [x28, #72]
	ldr s19, [x28, #76]
	ldr s20, [x28, #80]
	ldr s21, [x28, #84]
	ldr s22, [x28, #88]
	ldr s23, [x28, #92]
	ldr w9, [x28, #136]
	ldr w10, [x28, #160]
	ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x28]
	b .Lpd14
.Lsq15:
	str w15, [x28, #276]
	str s16, [x28, #64]
	str s17, [x28, #68]
	str s24, [x28, #108]
	str s18, [x28, #72]
	str s19, [x28, #76]
	str s20, [x28, #80]
	str s21, [x28, #84]
	str s22, [x28, #88]
	str s23, [x28, #92]
	mov w0, w26
	sub x9, x28, #0x48
	ldr x9, [x9]
	sub x1, x28, #0x40
	blr x9
	add x13, x28, #0x1c0
	ldr w15, [x28, #276]
	ldr s16, [x28, #64]
	ldr s17, [x28, #68]
	ldr s24, [x28, #108]
	ldr s18, [x28, #72]
	ldr s19, [x28, #76]
	ldr s20, [x28, #80]
	ldr s21, [x28, #84]
	ldr s22, [x28, #88]
	ldr s23, [x28, #92]
	ldr w9, [x28, #136]
	ldr w10, [x28, #160]
	ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x28]
	b .Lpd16
.LX17:
	str w14, [x28, #260]
	str s16, [x28, #64]
	str s17, [x28, #68]
	str s18, [x28, #72]
	str s19, [x28, #76]
	str s20, [x28, #80]
	str s21, [x28, #84]
	str s22, [x28, #88]
	str s23, [x28, #92]
	str s24, [x28, #108]
	str w19, [x28, #128]
	str w20, [x28, #132]
	str w21, [x28, #140]
	str w22, [x28, #144]
	str w23, [x28, #148]
	str w24, [x28, #152]
	str w25, [x28, #156]
	str w26, [x28, #184]
	str w15, [x28, #276]
	mov w29, #0x1C3A
	movk w29, #0x8C10, lsl #16
	str w29, [x28, #264]
	adrp x0, t2_no_update
	ldr x0, [x0, :lo12:t2_no_update]
	br x0
.LX18:
	str w14, [x28, #260]
	str s16, [x28, #64]
	str s17, [x28, #68]
	str s18, [x28, #72]
	str s19, [x28, #76]
	str s20, [x28, #80]
	str s21, [x28, #84]
	str s22, [x28, #88]
	str s23, [x28, #92]
	str s24, [x28, #108]
	str w19, [x28, #128]
	str w20, [x28, #132]
	str w21, [x28, #140]
	str w22, [x28, #144]
	str w23, [x28, #148]
	str w24, [x28, #152]
	str w25, [x28, #156]
	str w26, [x28, #184]
	mov w1, #1
	str w1, [x28, #276]
	mov w29, #0x1C52
	movk w29, #0x8C10, lsl #16
	str w29, [x28, #264]
	adrp x0, t2_no_update
	ldr x0, [x0, :lo12:t2_no_update]
	br x0
	.global t2_doa2_end
	.hidden t2_doa2_end
t2_doa2_end:
	.section .rodata
	.align 2
	.global t2_doa2_code
	.hidden t2_doa2_code
t2_doa2_code:
	.word 12
	.word 0x8C101BC2, 16
	.hword 0xf4e9, 0xf6e9, 0x6763, 0xf28d, 0xf270, 0xf79d, 0x7640, 0xf38d, 0x7520, 0xf0e9, 0xf5fd, 0xfb8d, 0x4310, 0x6046, 0x8d3d, 0x61e3
	.word 0x8C101BC4, 15
	.hword 0xf6e9, 0x6763, 0xf28d, 0xf270, 0xf79d, 0x7640, 0xf38d, 0x7520, 0xf0e9, 0xf5fd, 0xfb8d, 0x4310, 0x6046, 0x8d3d, 0x61e3
	.hword 0
	.word 0x8C101BE2, 5
	.hword 0xf3ed, 0xc801, 0x6e46, 0x8f10, 0xf79d
	.hword 0
	.word 0x8C101BEC, 9
	.hword 0xf743, 0x0483, 0xf3b5, 0x3e4c, 0xff1d, 0xf8ed, 0x0e83, 0x8f1d, 0xf20d
	.hword 0
	.word 0x8C101BFE, 4
	.hword 0xf230, 0xf38d, 0xa010, 0xfb3d
	.word 0x8C101C0C, 10
	.hword 0xf743, 0x7418, 0xf3b5, 0xff1d, 0xf8ed, 0x6e43, 0x7ee0, 0x0483, 0x8f0c, 0xf20d
	.word 0x8C101C20, 7
	.hword 0xf230, 0xfb3d, 0xf38d, 0x005a, 0x3027, 0x4008, 0x8b05
	.hword 0
	.word 0x8C101C26, 4
	.hword 0x005a, 0x3027, 0x4008, 0x8b05
	.word 0x8C101C2E, 4
	.hword 0xf3fd, 0xf386, 0xa002, 0xf3fd
	.word 0x8C101C38, 11
	.hword 0xf38d, 0xf018, 0xf672, 0xf62b, 0xf572, 0xf60b, 0x2338, 0xf66b, 0xf64b, 0x8d02, 0x2672
	.hword 0
	.word 0x8C101C3A, 10
	.hword 0xf018, 0xf672, 0xf62b, 0xf572, 0xf60b, 0x2338, 0xf66b, 0xf64b, 0x8d02, 0x2672
	.word 0x8C101C4E, 3
	.hword 0xf4e9, 0xafb8, 0x0683
	.hword 0
