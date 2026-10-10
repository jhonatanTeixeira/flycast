# 026_matriz

> Gerado por `tools/sdk_find.py`. Jogos: 6 · variantes (sequências normalizadas distintas): 8 · tempo perf somado: 1.73% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C170EF0` | 20 | 0.00% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C121400` | 20 | 0.10% |
| Power Stone (USA) | `0C0E1D30` | 20 | 0.00% |
| Project Justice (USA) | `0C148260` | 20 | 0.00% |
| Shenmue (USA) (Disc 1) | `0C0941CC` | 128 | 0.13% |
| Shenmue (USA) (Disc 1) | `0C0942EC` | 143 | 0.57% |
| Shenmue (USA) (Disc 1) | `0C09442C` | 114 | 0.00% |
| Shenmue (USA) (Disc 1) | `0C1D2500` | 20 | 0.02% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C04E22C` | 126 | 0.06% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C04E340` | 139 | 0.54% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C04E474` | 115 | 0.09% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C1DFF90` | 19 | 0.22% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C170EF0`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C170EF0  445A  lds r4,FPUL
  8C170EF2  F4FD  FSCA FPUL, dr4
  8C170EF4  F18D  fldi0 fr1
  8C170EF6  F38D  fldi0 fr3
  8C170EF8  F78D  fldi0 fr7
  8C170EFA  F05C  fmov fr5,fr0
  8C170EFC  F24C  fmov fr4,fr2
  8C170EFE  F24D  fneg fr2 
  8C170F00  F60C  fmov fr0,fr6
  8C170F02  F1FD  ftrv xmtrx,fv0
  8C170F04  F58D  fldi0 fr5
  8C170F06  F5FD  ftrv xmtrx,fv4
  8C170F08  F3FD  fschg
  8C170F0A  F10C  fmov fr0,fr1
  8C170F0C  F32C  fmov fr2,fr3
  8C170F0E  F94C  fmov fr4,fr9
  8C170F10  FB6C  fmov fr6,fr11
  8C170F12  F3FD  fschg
  8C170F14  000B  rts
  8C170F16  0009  nop
```

## Shenmue (USA) (Disc 1) `0C0941CC`

Dump: `/mnt/1TB/dcbat_off/20261007-191903_Shenmue__USA___Disc_1__/jit-11043.txt`

```
  0C0941CC  2F86  mov.l r8,@-r15
  0C0941CE  2F96  mov.l r9,@-r15
  0C0941D0  2FA6  mov.l r10,@-r15
  0C0941D2  2FB6  mov.l r11,@-r15
  0C0941D4  2FC6  mov.l r12,@-r15
  0C0941D6  6943  mov r4,r9
  0C0941D8  2FD6  mov.l r13,@-r15
  0C0941DA  2FE6  mov.l r14,@-r15
  0C0941DC  6C53  mov r5,r12
  0C0941DE  4F22  sts.l PR,@-r15
  0C0941E0  F3FD  fschg
  0C0941E2  6A96  mov.l @r9+,r10
  0C0941E4  C7EE  mova @([0C0945A0]),R0
  0C0941E6  5BA5  mov.l @(20,r10),r11
  0C0941E8  F109  fmov.s @r0+,fr1
  0C0941EA  6EB3  mov r11,r14
  0C0941EC  58B1  mov.l @(4,r11),r8
  0C0941EE  F309  fmov.s @r0+,fr3
  0C0941F0  2888  tst r8,r8
  0C0941F2  F509  fmov.s @r0+,fr5
  0C0941F4  F709  fmov.s @r0+,fr7
  0C0941F6  F909  fmov.s @r0+,fr9
  0C0941F8  FB09  fmov.s @r0+,fr11
  0C0941FA  FD09  fmov.s @r0+,fr13
  0C0941FC  FF09  fmov.s @r0+,fr15
  0C0941FE  8D50  bt.s 0C0942A2
  0C094200  F3FD  fschg
  0C094202  6DB3  mov r11,r13
  0C094204  7D20  add ##32,r13
  0C094206  F4D9  fmov.s @r13+,fr4
  0C094208  F5D9  fmov.s @r13+,fr5
  0C09420A  F6D8  fmov.s @r13,fr6
  0C09420C  F79D  fldi1 fr7
  0C09420E  F5FD  ftrv xmtrx,fv4
  0C094210  F3FD  fschg
  0C094212  FD4C  fmov fr4,fr13
  0C094214  FF6C  fmov fr6,fr15
  0C094216  F3FD  fschg
  0C094218  54B4  mov.l @(16,r11),r4
  0C09421A  2448  tst r4,r4
  0C09421C  8902  bt 0C094224
  0C09421E  D02C  mov.l @([0C0942D0]),r0
  0C094220  400B  jsr @r0
  0C094222  0009  nop
  0C094224  54B3  mov.l @(12,r11),r4
  0C094226  2448  tst r4,r4
  0C094228  8911  bt 0C09424E
  0C09422A  445A  lds r4,FPUL
  0C09422C  F4FD  FSCA FPUL, dr4
  0C09422E  F18D  fldi0 fr1
  0C094230  F38D  fldi0 fr3
  0C094232  F78D  fldi0 fr7
  0C094234  F05C  fmov fr5,fr0
  0C094236  F24C  fmov fr4,fr2
  0C094238  F24D  fneg fr2 
  0C09423A  F60C  fmov fr0,fr6
  0C09423C  F1FD  ftrv xmtrx,fv0
  0C09423E  F58D  fldi0 fr5
  0C094240  F5FD  ftrv xmtrx,fv4
  0C094242  F3FD  fschg
  0C094244  F10C  fmov fr0,fr1
  0C094246  F32C  fmov fr2,fr3
  0C094248  F94C  fmov fr4,fr9
  0C09424A  FB6C  fmov fr6,fr11
  0C09424C  F3FD  fschg
  0C09424E  54B2  mov.l @(8,r11),r4
  0C094250  2448  tst r4,r4
  0C094252  8902  bt 0C09425A
  0C094254  D01F  mov.l @([0C0942D4]),r0
  0C094256  400B  jsr @r0
  0C094258  0009  nop
  0C09425A  7B14  add ##20,r11
  0C09425C  F79D  fldi1 fr7
  0C09425E  F4B9  fmov.s @r11+,fr4
  0C094260  F5B9  fmov.s @r11+,fr5
  0C094262  F474  fcmp/eq fr7,fr4_SD_F
  0C094264  8F04  bf.s 0C094270
  0C094266  F6B8  fmov.s @r11,fr6
  0C094268  F574  fcmp/eq fr7,fr5_SD_F
  0C09426A  8B01  bf 0C094270
  0C09426C  F674  fcmp/eq fr7,fr6_SD_F
  0C09426E  8902  bt 0C094276
  0C094270  D019  mov.l @([0C0942D8]),r0
  0C094272  400B  jsr @r0
  0C094274  0009  nop
  0C094276  6DA3  mov r10,r13
  0C094278  7D18  add ##24,r13
  0C09427A  DB18  mov.l @([0C0942DC]),r11
  0C09427C  60D2  mov.l @r13,r0
  0C09427E  64A3  mov r10,r4
  0C094280  7A0C  add ##12,r10
  0C094282  00BE  mov.l @(R0,r11),r0
  0C094284  400B  jsr @r0
  0C094286  F4A8  fmov.s @r10,fr4
  0C094288  2008  tst r0,r0
  0C09428A  890A  bt 0C0942A2
  0C09428C  7D04  add ##4,r13
  0C09428E  D014  mov.l @([0C0942E0]),r0
  0C094290  400B  jsr @r0
  0C094292  F4D8  fmov.s @r13,fr4
  0C094294  55EF  mov.l @(60,r14),r5
  0C094296  D113  mov.l @([0C0942E4]),r1
  0C094298  2558  tst r5,r5
  0C09429A  8B00  bf 0C09429E
  ...
  0C09429E  410B  jsr @r1
  0C0942A0  6483  mov r8,r4
  0C0942A2  4C10  dt r12
  0C0942A4  8F9D  bf.s 0C0941E2
  0C0942A6  F3FD  fschg
  0C0942A8  C7BD  mova @([0C0945A0]),R0
  0C0942AA  F109  fmov.s @r0+,fr1
  0C0942AC  F309  fmov.s @r0+,fr3
  0C0942AE  F509  fmov.s @r0+,fr5
  0C0942B0  F709  fmov.s @r0+,fr7
  0C0942B2  F909  fmov.s @r0+,fr9
  0C0942B4  FB09  fmov.s @r0+,fr11
  0C0942B6  FD09  fmov.s @r0+,fr13
  0C0942B8  FF09  fmov.s @r0+,fr15
  0C0942BA  F3FD  fschg
  0C0942BC  4F26  lds.l @r15+,PR
  0C0942BE  6EF6  mov.l @r15+,r14
  0C0942C0  6DF6  mov.l @r15+,r13
  0C0942C2  6CF6  mov.l @r15+,r12
  0C0942C4  6BF6  mov.l @r15+,r11
  0C0942C6  6AF6  mov.l @r15+,r10
  0C0942C8  69F6  mov.l @r15+,r9
  0C0942CA  000B  rts
  0C0942CC  68F6  mov.l @r15+,r8
```

## Shenmue (USA) (Disc 1) `0C0942EC`

Dump: `/mnt/1TB/dcbat_off/20261007-191903_Shenmue__USA___Disc_1__/jit-11043.txt`

```
  0C0942EC  2F86  mov.l r8,@-r15
  0C0942EE  2F96  mov.l r9,@-r15
  0C0942F0  2FA6  mov.l r10,@-r15
  0C0942F2  2FB6  mov.l r11,@-r15
  0C0942F4  2FC6  mov.l r12,@-r15
  0C0942F6  6943  mov r4,r9
  0C0942F8  2FD6  mov.l r13,@-r15
  0C0942FA  2FE6  mov.l r14,@-r15
  0C0942FC  6C53  mov r5,r12
  0C0942FE  4F22  sts.l PR,@-r15
  0C094300  F3FD  fschg
  0C094302  6A96  mov.l @r9+,r10
  0C094304  C7A6  mova @([0C0945A0]),R0
  0C094306  5BA5  mov.l @(20,r10),r11
  0C094308  F109  fmov.s @r0+,fr1
  0C09430A  6EB3  mov r11,r14
  0C09430C  58B1  mov.l @(4,r11),r8
  0C09430E  F309  fmov.s @r0+,fr3
  0C094310  2888  tst r8,r8
  0C094312  F509  fmov.s @r0+,fr5
  0C094314  F709  fmov.s @r0+,fr7
  0C094316  F909  fmov.s @r0+,fr9
  0C094318  FB09  fmov.s @r0+,fr11
  0C09431A  FD09  fmov.s @r0+,fr13
  0C09431C  FF09  fmov.s @r0+,fr15
  0C09431E  8D5F  bt.s 0C0943E0
  0C094320  F3FD  fschg
  0C094322  6DB3  mov r11,r13
  0C094324  7D20  add ##32,r13
  0C094326  F4D9  fmov.s @r13+,fr4
  0C094328  F5D9  fmov.s @r13+,fr5
  0C09432A  F6D8  fmov.s @r13,fr6
  0C09432C  F79D  fldi1 fr7
  0C09432E  F5FD  ftrv xmtrx,fv4
  0C094330  F3FD  fschg
  0C094332  FD4C  fmov fr4,fr13
  0C094334  FF6C  fmov fr6,fr15
  0C094336  F3FD  fschg
  0C094338  54B4  mov.l @(16,r11),r4
  0C09433A  2448  tst r4,r4
  0C09433C  8902  bt 0C094344
  0C09433E  D033  mov.l @([0C09440C]),r0
  0C094340  400B  jsr @r0
  0C094342  0009  nop
  0C094344  54B3  mov.l @(12,r11),r4
  0C094346  2448  tst r4,r4
  0C094348  8911  bt 0C09436E
  0C09434A  445A  lds r4,FPUL
  0C09434C  F4FD  FSCA FPUL, dr4
  0C09434E  F18D  fldi0 fr1
  0C094350  F38D  fldi0 fr3
  0C094352  F78D  fldi0 fr7
  0C094354  F05C  fmov fr5,fr0
  0C094356  F24C  fmov fr4,fr2
  0C094358  F24D  fneg fr2 
  0C09435A  F60C  fmov fr0,fr6
  0C09435C  F1FD  ftrv xmtrx,fv0
  0C09435E  F58D  fldi0 fr5
  0C094360  F5FD  ftrv xmtrx,fv4
  0C094362  F3FD  fschg
  0C094364  F10C  fmov fr0,fr1
  0C094366  F32C  fmov fr2,fr3
  0C094368  F94C  fmov fr4,fr9
  0C09436A  FB6C  fmov fr6,fr11
  0C09436C  F3FD  fschg
  0C09436E  54B2  mov.l @(8,r11),r4
  0C094370  2448  tst r4,r4
  0C094372  8902  bt 0C09437A
  0C094374  D026  mov.l @([0C094410]),r0
  0C094376  400B  jsr @r0
  0C094378  0009  nop
  0C09437A  7B14  add ##20,r11
  0C09437C  F79D  fldi1 fr7
  0C09437E  F4B9  fmov.s @r11+,fr4
  0C094380  F5B9  fmov.s @r11+,fr5
  0C094382  F474  fcmp/eq fr7,fr4_SD_F
  0C094384  8F04  bf.s 0C094390
  0C094386  F6B8  fmov.s @r11,fr6
  0C094388  F574  fcmp/eq fr7,fr5_SD_F
  0C09438A  8B01  bf 0C094390
  0C09438C  F674  fcmp/eq fr7,fr6_SD_F
  0C09438E  8902  bt 0C094396
  0C094390  D020  mov.l @([0C094414]),r0
  0C094392  400B  jsr @r0
  0C094394  0009  nop
  0C094396  6DA3  mov r10,r13
  0C094398  7D18  add ##24,r13
  0C09439A  DB1F  mov.l @([0C094418]),r11
  0C09439C  60D2  mov.l @r13,r0
  0C09439E  64A3  mov r10,r4
  0C0943A0  7A0C  add ##12,r10
  0C0943A2  00BE  mov.l @(R0,r11),r0
  0C0943A4  400B  jsr @r0
  0C0943A6  F4A8  fmov.s @r10,fr4
  0C0943A8  2008  tst r0,r0
  0C0943AA  8919  bt 0C0943E0
  0C0943AC  6DD2  mov.l @r13,r13
  0C0943AE  E504  mov ##0x04,r5
  0C0943B0  3D50  cmp/eq r5,r13
  0C0943B2  890A  bt 0C0943CA
  0C0943B4  C7DA  mova @([0C094720]),R0
  0C0943B6  700C  add ##12,r0
  0C0943B8  F0AB  fmov.s fr10,@-r0
  0C0943BA  F09B  fmov.s fr9,@-r0
  0C0943BC  F08B  fmov.s fr8,@-r0
  0C0943BE  6403  mov r0,r4
  0C0943C0  D016  mov.l @([0C09441C]),r0
  0C0943C2  400B  jsr @r0
  0C0943C4  F4A8  fmov.s @r10,fr4
  0C0943C6  2008  tst r0,r0
  0C0943C8  890A  bt 0C0943E0
  0C0943CA  7A10  add ##16,r10
  0C0943CC  D014  mov.l @([0C094420]),r0
  0C0943CE  400B  jsr @r0
  0C0943D0  F4A8  fmov.s @r10,fr4
  0C0943D2  55EF  mov.l @(60,r14),r5
  0C0943D4  D113  mov.l @([0C094424]),r1
  0C0943D6  2558  tst r5,r5
  0C0943D8  8B00  bf 0C0943DC
  ...
  0C0943DC  410B  jsr @r1
  0C0943DE  6483  mov r8,r4
  0C0943E0  4C10  dt r12
  0C0943E2  8F8E  bf.s 0C094302
  0C0943E4  F3FD  fschg
  0C0943E6  C76E  mova @([0C0945A0]),R0
  0C0943E8  F109  fmov.s @r0+,fr1
  0C0943EA  F309  fmov.s @r0+,fr3
  0C0943EC  F509  fmov.s @r0+,fr5
  0C0943EE  F709  fmov.s @r0+,fr7
  0C0943F0  F909  fmov.s @r0+,fr9
  0C0943F2  FB09  fmov.s @r0+,fr11
  0C0943F4  FD09  fmov.s @r0+,fr13
  0C0943F6  FF09  fmov.s @r0+,fr15
  0C0943F8  F3FD  fschg
  0C0943FA  4F26  lds.l @r15+,PR
  0C0943FC  6EF6  mov.l @r15+,r14
  0C0943FE  6DF6  mov.l @r15+,r13
  0C094400  6CF6  mov.l @r15+,r12
  0C094402  6BF6  mov.l @r15+,r11
  0C094404  6AF6  mov.l @r15+,r10
  0C094406  69F6  mov.l @r15+,r9
  0C094408  000B  rts
  0C09440A  68F6  mov.l @r15+,r8
```

## Shenmue (USA) (Disc 1) `0C09442C`

Dump: `/mnt/1TB/dcbat_off/20261007-191903_Shenmue__USA___Disc_1__/jit-11043.txt`

```
  0C09442C  2F86  mov.l r8,@-r15
  0C09442E  2F96  mov.l r9,@-r15
  0C094430  2FA6  mov.l r10,@-r15
  0C094432  2FB6  mov.l r11,@-r15
  0C094434  2FC6  mov.l r12,@-r15
  0C094436  6943  mov r4,r9
  0C094438  2FD6  mov.l r13,@-r15
  0C09443A  2FE6  mov.l r14,@-r15
  0C09443C  6C53  mov r5,r12
  0C09443E  4F22  sts.l PR,@-r15
  0C094440  F3FD  fschg
  0C094442  6A96  mov.l @r9+,r10
  0C094444  C756  mova @([0C0945A0]),R0
  0C094446  5BA5  mov.l @(20,r10),r11
  0C094448  F109  fmov.s @r0+,fr1
  0C09444A  6EB3  mov r11,r14
  0C09444C  58B1  mov.l @(4,r11),r8
  0C09444E  F309  fmov.s @r0+,fr3
  0C094450  2888  tst r8,r8
  0C094452  F509  fmov.s @r0+,fr5
  0C094454  F709  fmov.s @r0+,fr7
  0C094456  F909  fmov.s @r0+,fr9
  0C094458  FB09  fmov.s @r0+,fr11
  0C09445A  FD09  fmov.s @r0+,fr13
  0C09445C  FF09  fmov.s @r0+,fr15
  0C09445E  8D69  bt.s 0C094534
  0C094460  F3FD  fschg
  0C094462  6DB3  mov r11,r13
  0C094464  7D20  add ##32,r13
  0C094466  F4D9  fmov.s @r13+,fr4
  0C094468  F5D9  fmov.s @r13+,fr5
  0C09446A  F6D8  fmov.s @r13,fr6
  0C09446C  F79D  fldi1 fr7
  0C09446E  F5FD  ftrv xmtrx,fv4
  0C094470  F3FD  fschg
  0C094472  FD4C  fmov fr4,fr13
  0C094474  FF6C  fmov fr6,fr15
  0C094476  F3FD  fschg
  0C094478  54B4  mov.l @(16,r11),r4
  0C09447A  2448  tst r4,r4
  0C09447C  8910  bt 0C0944A0
  ...
  0C0944A0  54B3  mov.l @(12,r11),r4
  0C0944A2  2448  tst r4,r4
  0C0944A4  8911  bt 0C0944CA
  0C0944A6  445A  lds r4,FPUL
  0C0944A8  F4FD  FSCA FPUL, dr4
  0C0944AA  F18D  fldi0 fr1
  0C0944AC  F38D  fldi0 fr3
  0C0944AE  F78D  fldi0 fr7
  0C0944B0  F05C  fmov fr5,fr0
  0C0944B2  F24C  fmov fr4,fr2
  0C0944B4  F24D  fneg fr2 
  0C0944B6  F60C  fmov fr0,fr6
  0C0944B8  F1FD  ftrv xmtrx,fv0
  0C0944BA  F58D  fldi0 fr5
  0C0944BC  F5FD  ftrv xmtrx,fv4
  0C0944BE  F3FD  fschg
  0C0944C0  F10C  fmov fr0,fr1
  0C0944C2  F32C  fmov fr2,fr3
  0C0944C4  F94C  fmov fr4,fr9
  0C0944C6  FB6C  fmov fr6,fr11
  0C0944C8  F3FD  fschg
  0C0944CA  54B2  mov.l @(8,r11),r4
  0C0944CC  2448  tst r4,r4
  0C0944CE  8912  bt 0C0944F6
  ...
  0C0944F6  7B14  add ##20,r11
  0C0944F8  F79D  fldi1 fr7
  0C0944FA  F4B9  fmov.s @r11+,fr4
  0C0944FC  F5B9  fmov.s @r11+,fr5
  0C0944FE  F474  fcmp/eq fr7,fr4_SD_F
  0C094500  8F04  bf.s 0C09450C
  0C094502  F6B8  fmov.s @r11,fr6
  0C094504  F574  fcmp/eq fr7,fr5_SD_F
  0C094506  8B01  bf 0C09450C
  0C094508  F674  fcmp/eq fr7,fr6_SD_F
  0C09450A  8902  bt 0C094512
  ...
  0C094512  64A3  mov r10,r4
  0C094514  7A0C  add ##12,r10
  0C094516  BDED  bsr 0C0940F4
  0C094518  F4A8  fmov.s @r10,fr4
  0C09451A  2008  tst r0,r0
  0C09451C  890A  bt 0C094534
  0C09451E  7A10  add ##16,r10
  0C094520  D010  mov.l @([0C094564]),r0
  0C094522  400B  jsr @r0
  0C094524  F4A8  fmov.s @r10,fr4
  0C094526  55EF  mov.l @(60,r14),r5
  0C094528  D10F  mov.l @([0C094568]),r1
  0C09452A  2558  tst r5,r5
  0C09452C  8B00  bf 0C094530
  ...
  0C094530  410B  jsr @r1
  0C094532  6483  mov r8,r4
  0C094534  4C10  dt r12
  0C094536  8F84  bf.s 0C094442
  0C094538  F3FD  fschg
  0C09453A  C719  mova @([0C0945A0]),R0
  0C09453C  F109  fmov.s @r0+,fr1
  0C09453E  F309  fmov.s @r0+,fr3
  0C094540  F509  fmov.s @r0+,fr5
  0C094542  F709  fmov.s @r0+,fr7
  0C094544  F909  fmov.s @r0+,fr9
  0C094546  FB09  fmov.s @r0+,fr11
  0C094548  FD09  fmov.s @r0+,fr13
  0C09454A  FF09  fmov.s @r0+,fr15
  0C09454C  F3FD  fschg
  0C09454E  4F26  lds.l @r15+,PR
  0C094550  6EF6  mov.l @r15+,r14
  0C094552  6DF6  mov.l @r15+,r13
  0C094554  6CF6  mov.l @r15+,r12
  0C094556  6BF6  mov.l @r15+,r11
  0C094558  6AF6  mov.l @r15+,r10
  0C09455A  69F6  mov.l @r15+,r9
  0C09455C  000B  rts
  0C09455E  68F6  mov.l @r15+,r8
```

## Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) `8C04E22C`

Dump: `/mnt/1TB/dcbat_off/20261007-192124_Shenmue_II__Europe___En_Fr_De_Es___Disc_/jit-12387.txt`

```
  8C04E22C  2F86  mov.l r8,@-r15
  8C04E22E  2F96  mov.l r9,@-r15
  8C04E230  2FA6  mov.l r10,@-r15
  8C04E232  2FB6  mov.l r11,@-r15
  8C04E234  2FC6  mov.l r12,@-r15
  8C04E236  6943  mov r4,r9
  8C04E238  2FD6  mov.l r13,@-r15
  8C04E23A  2FE6  mov.l r14,@-r15
  8C04E23C  6C53  mov r5,r12
  8C04E23E  4F22  sts.l PR,@-r15
  8C04E240  F3FD  fschg
  8C04E242  6A96  mov.l @r9+,r10
  8C04E244  C7E6  mova @([8C04E5E0]),R0
  8C04E246  5BA5  mov.l @(20,r10),r11
  8C04E248  F109  fmov.s @r0+,fr1
  8C04E24A  6EB3  mov r11,r14
  8C04E24C  58BA  mov.l @(40,r11),r8
  8C04E24E  F309  fmov.s @r0+,fr3
  8C04E250  2888  tst r8,r8
  8C04E252  F509  fmov.s @r0+,fr5
  8C04E254  F709  fmov.s @r0+,fr7
  8C04E256  F909  fmov.s @r0+,fr9
  8C04E258  FB09  fmov.s @r0+,fr11
  8C04E25A  FD09  fmov.s @r0+,fr13
  8C04E25C  FF09  fmov.s @r0+,fr15
  8C04E25E  8D4D  bt.s 8C04E2FC
  8C04E260  F3FD  fschg
  8C04E262  6DB3  mov r11,r13
  8C04E264  7D04  add ##4,r13
  8C04E266  F4D9  fmov.s @r13+,fr4
  8C04E268  F5D9  fmov.s @r13+,fr5
  8C04E26A  F6D8  fmov.s @r13,fr6
  8C04E26C  F79D  fldi1 fr7
  8C04E26E  F5FD  ftrv xmtrx,fv4
  8C04E270  F3FD  fschg
  8C04E272  FD4C  fmov fr4,fr13
  8C04E274  FF6C  fmov fr6,fr15
  8C04E276  F3FD  fschg
  8C04E278  54B6  mov.l @(24,r11),r4
  8C04E27A  2448  tst r4,r4
  8C04E27C  8902  bt 8C04E284
  8C04E27E  D02A  mov.l @([8C04E328]),r0
  8C04E280  400B  jsr @r0
  8C04E282  0009  nop
  8C04E284  54B5  mov.l @(20,r11),r4
  8C04E286  2448  tst r4,r4
  8C04E288  8911  bt 8C04E2AE
  8C04E28A  445A  lds r4,FPUL
  8C04E28C  F4FD  FSCA FPUL, dr4
  8C04E28E  F18D  fldi0 fr1
  8C04E290  F38D  fldi0 fr3
  8C04E292  F78D  fldi0 fr7
  8C04E294  F05C  fmov fr5,fr0
  8C04E296  F24C  fmov fr4,fr2
  8C04E298  F24D  fneg fr2 
  8C04E29A  F60C  fmov fr0,fr6
  8C04E29C  F1FD  ftrv xmtrx,fv0
  8C04E29E  F58D  fldi0 fr5
  8C04E2A0  F5FD  ftrv xmtrx,fv4
  8C04E2A2  F3FD  fschg
  8C04E2A4  F10C  fmov fr0,fr1
  8C04E2A6  F32C  fmov fr2,fr3
  8C04E2A8  F94C  fmov fr4,fr9
  8C04E2AA  FB6C  fmov fr6,fr11
  8C04E2AC  F3FD  fschg
  8C04E2AE  54B4  mov.l @(16,r11),r4
  8C04E2B0  2448  tst r4,r4
  8C04E2B2  8902  bt 8C04E2BA
  8C04E2B4  D01D  mov.l @([8C04E32C]),r0
  8C04E2B6  400B  jsr @r0
  8C04E2B8  0009  nop
  8C04E2BA  7B1C  add ##28,r11
  8C04E2BC  F79D  fldi1 fr7
  8C04E2BE  F4B9  fmov.s @r11+,fr4
  8C04E2C0  F5B9  fmov.s @r11+,fr5
  8C04E2C2  F474  fcmp/eq fr7,fr4_SD_F
  8C04E2C4  8F04  bf.s 8C04E2D0
  8C04E2C6  F6B8  fmov.s @r11,fr6
  8C04E2C8  F574  fcmp/eq fr7,fr5_SD_F
  8C04E2CA  8B01  bf 8C04E2D0
  8C04E2CC  F674  fcmp/eq fr7,fr6_SD_F
  8C04E2CE  8902  bt 8C04E2D6
  8C04E2D0  D017  mov.l @([8C04E330]),r0
  8C04E2D2  400B  jsr @r0
  8C04E2D4  0009  nop
  8C04E2D6  6DA3  mov r10,r13
  8C04E2D8  7D18  add ##24,r13
  8C04E2DA  DB16  mov.l @([8C04E334]),r11
  8C04E2DC  60D2  mov.l @r13,r0
  8C04E2DE  64A3  mov r10,r4
  8C04E2E0  7A0C  add ##12,r10
  8C04E2E2  00BE  mov.l @(R0,r11),r0
  8C04E2E4  400B  jsr @r0
  8C04E2E6  F4A8  fmov.s @r10,fr4
  8C04E2E8  2008  tst r0,r0
  8C04E2EA  8907  bt 8C04E2FC
  8C04E2EC  7D04  add ##4,r13
  8C04E2EE  D012  mov.l @([8C04E338]),r0
  8C04E2F0  400B  jsr @r0
  8C04E2F2  F4D8  fmov.s @r13,fr4
  8C04E2F4  D111  mov.l @([8C04E33C]),r1
  8C04E2F6  6112  mov.l @r1,r1
  8C04E2F8  410B  jsr @r1
  8C04E2FA  6483  mov r8,r4
  8C04E2FC  4C10  dt r12
  8C04E2FE  8FA0  bf.s 8C04E242
  8C04E300  F3FD  fschg
  8C04E302  C7B7  mova @([8C04E5E0]),R0
  8C04E304  F109  fmov.s @r0+,fr1
  8C04E306  F309  fmov.s @r0+,fr3
  8C04E308  F509  fmov.s @r0+,fr5
  8C04E30A  F709  fmov.s @r0+,fr7
  8C04E30C  F909  fmov.s @r0+,fr9
  8C04E30E  FB09  fmov.s @r0+,fr11
  8C04E310  FD09  fmov.s @r0+,fr13
  8C04E312  FF09  fmov.s @r0+,fr15
  8C04E314  F3FD  fschg
  8C04E316  4F26  lds.l @r15+,PR
  8C04E318  6EF6  mov.l @r15+,r14
  8C04E31A  6DF6  mov.l @r15+,r13
  8C04E31C  6CF6  mov.l @r15+,r12
  8C04E31E  6BF6  mov.l @r15+,r11
  8C04E320  6AF6  mov.l @r15+,r10
  8C04E322  69F6  mov.l @r15+,r9
  8C04E324  000B  rts
  8C04E326  68F6  mov.l @r15+,r8
```

## Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) `8C04E340`

Dump: `/mnt/1TB/dcbat_off/20261007-192124_Shenmue_II__Europe___En_Fr_De_Es___Disc_/jit-12387.txt`

```
  8C04E340  2F86  mov.l r8,@-r15
  8C04E342  2F96  mov.l r9,@-r15
  8C04E344  2FA6  mov.l r10,@-r15
  8C04E346  2FB6  mov.l r11,@-r15
  8C04E348  2FC6  mov.l r12,@-r15
  8C04E34A  6943  mov r4,r9
  8C04E34C  2FD6  mov.l r13,@-r15
  8C04E34E  2FE6  mov.l r14,@-r15
  8C04E350  6C53  mov r5,r12
  8C04E352  4F22  sts.l PR,@-r15
  8C04E354  F3FD  fschg
  8C04E356  6A96  mov.l @r9+,r10
  8C04E358  C7A1  mova @([8C04E5E0]),R0
  8C04E35A  5BA5  mov.l @(20,r10),r11
  8C04E35C  F109  fmov.s @r0+,fr1
  8C04E35E  6EB3  mov r11,r14
  8C04E360  58BA  mov.l @(40,r11),r8
  8C04E362  F309  fmov.s @r0+,fr3
  8C04E364  2888  tst r8,r8
  8C04E366  F509  fmov.s @r0+,fr5
  8C04E368  F709  fmov.s @r0+,fr7
  8C04E36A  F909  fmov.s @r0+,fr9
  8C04E36C  FB09  fmov.s @r0+,fr11
  8C04E36E  FD09  fmov.s @r0+,fr13
  8C04E370  FF09  fmov.s @r0+,fr15
  8C04E372  8D5A  bt.s 8C04E42A
  8C04E374  F3FD  fschg
  8C04E376  6DB3  mov r11,r13
  8C04E378  7D04  add ##4,r13
  8C04E37A  F4D9  fmov.s @r13+,fr4
  8C04E37C  F5D9  fmov.s @r13+,fr5
  8C04E37E  F6D8  fmov.s @r13,fr6
  8C04E380  F79D  fldi1 fr7
  8C04E382  F5FD  ftrv xmtrx,fv4
  8C04E384  F3FD  fschg
  8C04E386  FD4C  fmov fr4,fr13
  8C04E388  FF6C  fmov fr6,fr15
  8C04E38A  F3FD  fschg
  8C04E38C  54B6  mov.l @(24,r11),r4
  8C04E38E  2448  tst r4,r4
  8C04E390  8902  bt 8C04E398
  8C04E392  D031  mov.l @([8C04E458]),r0
  8C04E394  400B  jsr @r0
  8C04E396  0009  nop
  8C04E398  54B5  mov.l @(20,r11),r4
  8C04E39A  2448  tst r4,r4
  8C04E39C  8911  bt 8C04E3C2
  8C04E39E  445A  lds r4,FPUL
  8C04E3A0  F4FD  FSCA FPUL, dr4
  8C04E3A2  F18D  fldi0 fr1
  8C04E3A4  F38D  fldi0 fr3
  8C04E3A6  F78D  fldi0 fr7
  8C04E3A8  F05C  fmov fr5,fr0
  8C04E3AA  F24C  fmov fr4,fr2
  8C04E3AC  F24D  fneg fr2 
  8C04E3AE  F60C  fmov fr0,fr6
  8C04E3B0  F1FD  ftrv xmtrx,fv0
  8C04E3B2  F58D  fldi0 fr5
  8C04E3B4  F5FD  ftrv xmtrx,fv4
  8C04E3B6  F3FD  fschg
  8C04E3B8  F10C  fmov fr0,fr1
  8C04E3BA  F32C  fmov fr2,fr3
  8C04E3BC  F94C  fmov fr4,fr9
  8C04E3BE  FB6C  fmov fr6,fr11
  8C04E3C0  F3FD  fschg
  8C04E3C2  54B4  mov.l @(16,r11),r4
  8C04E3C4  2448  tst r4,r4
  8C04E3C6  8902  bt 8C04E3CE
  8C04E3C8  D024  mov.l @([8C04E45C]),r0
  8C04E3CA  400B  jsr @r0
  8C04E3CC  0009  nop
  8C04E3CE  7B1C  add ##28,r11
  8C04E3D0  F79D  fldi1 fr7
  8C04E3D2  F4B9  fmov.s @r11+,fr4
  8C04E3D4  F5B9  fmov.s @r11+,fr5
  8C04E3D6  F474  fcmp/eq fr7,fr4_SD_F
  8C04E3D8  8F04  bf.s 8C04E3E4
  8C04E3DA  F6B8  fmov.s @r11,fr6
  8C04E3DC  F574  fcmp/eq fr7,fr5_SD_F
  8C04E3DE  8B01  bf 8C04E3E4
  8C04E3E0  F674  fcmp/eq fr7,fr6_SD_F
  8C04E3E2  8902  bt 8C04E3EA
  8C04E3E4  D01E  mov.l @([8C04E460]),r0
  8C04E3E6  400B  jsr @r0
  8C04E3E8  0009  nop
  8C04E3EA  6DA3  mov r10,r13
  8C04E3EC  7D18  add ##24,r13
  8C04E3EE  DB1D  mov.l @([8C04E464]),r11
  8C04E3F0  60D2  mov.l @r13,r0
  8C04E3F2  64A3  mov r10,r4
  8C04E3F4  7A0C  add ##12,r10
  8C04E3F6  00BE  mov.l @(R0,r11),r0
  8C04E3F8  400B  jsr @r0
  8C04E3FA  F4A8  fmov.s @r10,fr4
  8C04E3FC  2008  tst r0,r0
  8C04E3FE  8914  bt 8C04E42A
  8C04E400  6DD2  mov.l @r13,r13
  8C04E402  E504  mov ##0x04,r5
  8C04E404  3D50  cmp/eq r5,r13
  8C04E406  8908  bt 8C04E41A
  8C04E408  65E3  mov r14,r5
  8C04E40A  7AF4  add ##-12,r10
  8C04E40C  64A3  mov r10,r4
  8C04E40E  7A0C  add ##12,r10
  8C04E410  D015  mov.l @([8C04E468]),r0
  8C04E412  400B  jsr @r0
  8C04E414  F4A8  fmov.s @r10,fr4
  8C04E416  2008  tst r0,r0
  8C04E418  8907  bt 8C04E42A
  8C04E41A  7A10  add ##16,r10
  8C04E41C  D013  mov.l @([8C04E46C]),r0
  8C04E41E  400B  jsr @r0
  8C04E420  F4A8  fmov.s @r10,fr4
  8C04E422  D113  mov.l @([8C04E470]),r1
  8C04E424  6112  mov.l @r1,r1
  8C04E426  410B  jsr @r1
  8C04E428  6483  mov r8,r4
  8C04E42A  4C10  dt r12
  8C04E42C  8F93  bf.s 8C04E356
  8C04E42E  F3FD  fschg
  8C04E430  C76B  mova @([8C04E5E0]),R0
  8C04E432  F109  fmov.s @r0+,fr1
  8C04E434  F309  fmov.s @r0+,fr3
  8C04E436  F509  fmov.s @r0+,fr5
  8C04E438  F709  fmov.s @r0+,fr7
  8C04E43A  F909  fmov.s @r0+,fr9
  8C04E43C  FB09  fmov.s @r0+,fr11
  8C04E43E  FD09  fmov.s @r0+,fr13
  8C04E440  FF09  fmov.s @r0+,fr15
  8C04E442  F3FD  fschg
  8C04E444  4F26  lds.l @r15+,PR
  8C04E446  6EF6  mov.l @r15+,r14
  8C04E448  6DF6  mov.l @r15+,r13
  8C04E44A  6CF6  mov.l @r15+,r12
  8C04E44C  6BF6  mov.l @r15+,r11
  8C04E44E  6AF6  mov.l @r15+,r10
  8C04E450  69F6  mov.l @r15+,r9
  8C04E452  000B  rts
  8C04E454  68F6  mov.l @r15+,r8
```

## Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) `8C04E474`

Dump: `/mnt/1TB/dcbat_off/20261007-192124_Shenmue_II__Europe___En_Fr_De_Es___Disc_/jit-12387.txt`

```
  8C04E474  2F86  mov.l r8,@-r15
  8C04E476  2F96  mov.l r9,@-r15
  8C04E478  2FA6  mov.l r10,@-r15
  8C04E47A  2FB6  mov.l r11,@-r15
  8C04E47C  2FC6  mov.l r12,@-r15
  8C04E47E  6943  mov r4,r9
  8C04E480  2FD6  mov.l r13,@-r15
  8C04E482  2FE6  mov.l r14,@-r15
  8C04E484  6C53  mov r5,r12
  8C04E486  4F22  sts.l PR,@-r15
  8C04E488  F3FD  fschg
  8C04E48A  6A96  mov.l @r9+,r10
  8C04E48C  C754  mova @([8C04E5E0]),R0
  8C04E48E  5BA5  mov.l @(20,r10),r11
  8C04E490  F109  fmov.s @r0+,fr1
  8C04E492  6EB3  mov r11,r14
  8C04E494  58BA  mov.l @(40,r11),r8
  8C04E496  F309  fmov.s @r0+,fr3
  8C04E498  2888  tst r8,r8
  8C04E49A  F509  fmov.s @r0+,fr5
  8C04E49C  F709  fmov.s @r0+,fr7
  8C04E49E  F909  fmov.s @r0+,fr9
  8C04E4A0  FB09  fmov.s @r0+,fr11
  8C04E4A2  FD09  fmov.s @r0+,fr13
  8C04E4A4  FF09  fmov.s @r0+,fr15
  8C04E4A6  8D66  bt.s 8C04E576
  8C04E4A8  F3FD  fschg
  8C04E4AA  6DB3  mov r11,r13
  8C04E4AC  7D04  add ##4,r13
  8C04E4AE  F4D9  fmov.s @r13+,fr4
  8C04E4B0  F5D9  fmov.s @r13+,fr5
  8C04E4B2  F6D8  fmov.s @r13,fr6
  8C04E4B4  F79D  fldi1 fr7
  8C04E4B6  F5FD  ftrv xmtrx,fv4
  8C04E4B8  F3FD  fschg
  8C04E4BA  FD4C  fmov fr4,fr13
  8C04E4BC  FF6C  fmov fr6,fr15
  8C04E4BE  F3FD  fschg
  8C04E4C0  54B6  mov.l @(24,r11),r4
  8C04E4C2  2448  tst r4,r4
  8C04E4C4  8910  bt 8C04E4E8
  ...
  8C04E4E8  54B5  mov.l @(20,r11),r4
  8C04E4EA  2448  tst r4,r4
  8C04E4EC  8911  bt 8C04E512
  8C04E4EE  445A  lds r4,FPUL
  8C04E4F0  F4FD  FSCA FPUL, dr4
  8C04E4F2  F18D  fldi0 fr1
  8C04E4F4  F38D  fldi0 fr3
  8C04E4F6  F78D  fldi0 fr7
  8C04E4F8  F05C  fmov fr5,fr0
  8C04E4FA  F24C  fmov fr4,fr2
  8C04E4FC  F24D  fneg fr2 
  8C04E4FE  F60C  fmov fr0,fr6
  8C04E500  F1FD  ftrv xmtrx,fv0
  8C04E502  F58D  fldi0 fr5
  8C04E504  F5FD  ftrv xmtrx,fv4
  8C04E506  F3FD  fschg
  8C04E508  F10C  fmov fr0,fr1
  8C04E50A  F32C  fmov fr2,fr3
  8C04E50C  F94C  fmov fr4,fr9
  8C04E50E  FB6C  fmov fr6,fr11
  8C04E510  F3FD  fschg
  8C04E512  54B4  mov.l @(16,r11),r4
  8C04E514  2448  tst r4,r4
  8C04E516  8912  bt 8C04E53E
  ...
  8C04E53E  7B1C  add ##28,r11
  8C04E540  F79D  fldi1 fr7
  8C04E542  F4B9  fmov.s @r11+,fr4
  8C04E544  F5B9  fmov.s @r11+,fr5
  8C04E546  F474  fcmp/eq fr7,fr4_SD_F
  8C04E548  8F04  bf.s 8C04E554
  8C04E54A  F6B8  fmov.s @r11,fr6
  8C04E54C  F574  fcmp/eq fr7,fr5_SD_F
  8C04E54E  8B01  bf 8C04E554
  8C04E550  F674  fcmp/eq fr7,fr6_SD_F
  8C04E552  8902  bt 8C04E55A
  8C04E554  D013  mov.l @([8C04E5A4]),r0
  8C04E556  400B  jsr @r0
  8C04E558  0009  nop
  8C04E55A  64A3  mov r10,r4
  8C04E55C  7A0C  add ##12,r10
  8C04E55E  BE07  bsr 8C04E170
  8C04E560  F4A8  fmov.s @r10,fr4
  8C04E562  2008  tst r0,r0
  8C04E564  8907  bt 8C04E576
  8C04E566  7A10  add ##16,r10
  8C04E568  D00F  mov.l @([8C04E5A8]),r0
  8C04E56A  400B  jsr @r0
  8C04E56C  F4A8  fmov.s @r10,fr4
  8C04E56E  D10F  mov.l @([8C04E5AC]),r1
  8C04E570  6112  mov.l @r1,r1
  8C04E572  410B  jsr @r1
  8C04E574  6483  mov r8,r4
  8C04E576  4C10  dt r12
  8C04E578  8F87  bf.s 8C04E48A
  8C04E57A  F3FD  fschg
  8C04E57C  C718  mova @([8C04E5E0]),R0
  8C04E57E  F109  fmov.s @r0+,fr1
  8C04E580  F309  fmov.s @r0+,fr3
  8C04E582  F509  fmov.s @r0+,fr5
  8C04E584  F709  fmov.s @r0+,fr7
  8C04E586  F909  fmov.s @r0+,fr9
  8C04E588  FB09  fmov.s @r0+,fr11
  8C04E58A  FD09  fmov.s @r0+,fr13
  8C04E58C  FF09  fmov.s @r0+,fr15
  8C04E58E  F3FD  fschg
  8C04E590  4F26  lds.l @r15+,PR
  8C04E592  6EF6  mov.l @r15+,r14
  8C04E594  6DF6  mov.l @r15+,r13
  8C04E596  6CF6  mov.l @r15+,r12
  8C04E598  6BF6  mov.l @r15+,r11
  8C04E59A  6AF6  mov.l @r15+,r10
  8C04E59C  69F6  mov.l @r15+,r9
  8C04E59E  000B  rts
  8C04E5A0  68F6  mov.l @r15+,r8
```

## Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) `8C1DFF90`

Dump: `/mnt/1TB/dcbat_off/20261007-192124_Shenmue_II__Europe___En_Fr_De_Es___Disc_/jit-12387.txt`

```
  8C1DFF90  445A  lds r4,FPUL
  8C1DFF92  F4FD  FSCA FPUL, dr4
  8C1DFF94  F18D  fldi0 fr1
  8C1DFF96  F38D  fldi0 fr3
  8C1DFF98  F78D  fldi0 fr7
  8C1DFF9A  F05C  fmov fr5,fr0
  8C1DFF9C  F24C  fmov fr4,fr2
  8C1DFF9E  F24D  fneg fr2 
  8C1DFFA0  F60C  fmov fr0,fr6
  8C1DFFA2  F1FD  ftrv xmtrx,fv0
  8C1DFFA4  F58D  fldi0 fr5
  8C1DFFA6  F5FD  ftrv xmtrx,fv4
  8C1DFFA8  F3FD  fschg
  8C1DFFAA  F10C  fmov fr0,fr1
  8C1DFFAC  F32C  fmov fr2,fr3
  8C1DFFAE  F94C  fmov fr4,fr9
  8C1DFFB0  FB6C  fmov fr6,fr11
  8C1DFFB2  000B  rts
  8C1DFFB4  F3FD  fschg
```
