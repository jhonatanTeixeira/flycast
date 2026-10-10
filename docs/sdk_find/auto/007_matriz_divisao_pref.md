# 007_matriz_divisao_pref

> Gerado por `tools/sdk_find.py`. Jogos: 3 · variantes (sequências normalizadas distintas): 2 · tempo perf somado: 7.69% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C1891C0` | 93 | 0.00% |
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C1892D0` | 91 | 0.00% |
| Dead or Alive 2 (USA) | `8C109CC0` | 93 | 0.73% |
| Dead or Alive 2 (USA) | `8C109DE0` | 91 | 0.02% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C1304E0` | 93 | 6.13% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C130600` | 91 | 0.81% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C1891C0`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C1891C0  6346  mov.l @r4+,r3
  8C1891C2  7620  add ##32,r6
  8C1891C4  F3FD  fschg
  8C1891C6  6042  mov.l @r4,r0
  8C1891C8  C801  tst ##1,R0
  8C1891CA  6E43  mov r4,r14
  8C1891CC  8F03  bf.s 8C1891D6
  8C1891CE  7420  add ##32,r4
  8C1891D0  5EE1  mov.l @(4,r14),r14
  8C1891D2  74E8  add ##-24,r4
  8C1891D4  3E4C  add r4,r14
  8C1891D6  F4E9  fmov.s @r14+,fr4
  8C1891D8  F6E8  fmov.s @r14,fr6
  8C1891DA  61E3  mov r14,r1
  8C1891DC  F79D  fldi1 fr7
  8C1891DE  7110  add ##16,r1
  8C1891E0  84E7  mov.b @(7,r14),R0
  8C1891E2  58E2  mov.l @(8,r14),r8
  8C1891E4  4015  cmp/pl r0
  8C1891E6  608C  extu.b r8,r0
  8C1891E8  8F26  bf.s 8C189238
  8C1891EA  F5FD  ftrv xmtrx,fv4
  ...
  8C189238  7420  add ##32,r4
  8C18923A  405A  lds r0,FPUL
  8C18923C  4819  shlr8 r8
  8C18923E  608C  extu.b r8,r0
  8C189240  0483  pref @r4
  8C189242  4819  shlr8 r8
  8C189244  FB2D  float FPUL,fr11
  8C189246  74E0  add ##-32,r4
  8C189248  F79D  fldi1 fr7
  8C18924A  F743  fdiv fr4,fr7
  8C18924C  405A  lds r0,FPUL
  8C18924E  FA2D  float FPUL,fr10
  8C189250  608C  extu.b r8,r0
  8C189252  405A  lds r0,FPUL
  8C189254  4819  shlr8 r8
  8C189256  F92D  float FPUL,fr9
  8C189258  485A  lds r8,FPUL
  8C18925A  76F8  add ##-8,r6
  8C18925C  F82D  float FPUL,fr8
  8C18925E  F018  fmov.s @r1,fr0
  8C189260  6063  mov r6,r0
  8C189262  F38D  fldi0 fr3
  8C189264  FBF2  fmul fr15,fr11
  8C189266  8F03  bf.s 8C189270
  8C189268  F60B  fmov.s fr0,@-r6
  ...
  8C189270  4021  shar r0
  8C189272  F28D  fldi0 fr2
  8C189274  4310  dt r3
  8C189276  F672  fmul fr7,fr6
  8C189278  8900  bt 8C18927C
  8C18927A  6063  mov r6,r0
  8C18927C  F572  fmul fr7,fr5
  8C18927E  F66B  fmov.s fr6,@-r6
  8C189280  7540  add ##64,r5
  8C189282  F64B  fmov.s fr4,@-r6
  8C189284  FAE2  fmul fr14,fr10
  8C189286  2602  mov.l r0,@r6
  8C189288  0683  pref @r6
  8C18928A  6E43  mov r4,r14
  8C18928C  5EE1  mov.l @(4,r14),r14
  8C18928E  7640  add ##64,r6
  8C189290  6046  mov.l @r4+,r0
  8C189292  F9D2  fmul fr13,fr9
  8C189294  F62B  fmov.s fr2,@-r6
  8C189296  F8C2  fmul fr12,fr8
  8C189298  F62B  fmov.s fr2,@-r6
  8C18929A  3E4C  add r4,r14
  8C18929C  F6AB  fmov.s fr10,@-r6
  8C18929E  7E04  add ##4,r14
  8C1892A0  F68B  fmov.s fr8,@-r6
  8C1892A2  8D08  bt.s 8C1892B6
  8C1892A4  0683  pref @r6
  8C1892A6  C801  tst ##1,R0
  8C1892A8  7640  add ##64,r6
  8C1892AA  8D94  bt.s 8C1891D6
  8C1892AC  7404  add ##4,r4
  8C1892AE  6E43  mov r4,r14
  8C1892B0  7418  add ##24,r4
  8C1892B2  AF90  bra 8C1891D6
  8C1892B4  7EF8  add ##-8,r14
  8C1892B6  7640  add ##64,r6
  8C1892B8  4015  cmp/pl r0
  8C1892BA  8F03  bf.s 8C1892C4
  8C1892BC  C880  tst ##128,R0
  8C1892BE  8F82  bf.s 8C1891C6
  8C1892C0  6346  mov.l @r4+,r3
  8C1892C2  74FC  add ##-4,r4
  8C1892C4  F3FD  fschg
  8C1892C6  74FC  add ##-4,r4
  8C1892C8  000B  rts
  8C1892CA  76E0  add ##-32,r6
```

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C1892D0`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C1892D0  6946  mov.l @r4+,r9
  8C1892D2  7620  add ##32,r6
  8C1892D4  E303  mov ##0x03,r3
  8C1892D6  F3FD  fschg
  8C1892D8  6042  mov.l @r4,r0
  8C1892DA  C801  tst ##1,R0
  8C1892DC  6E43  mov r4,r14
  8C1892DE  8F03  bf.s 8C1892E8
  8C1892E0  7420  add ##32,r4
  8C1892E2  5EE1  mov.l @(4,r14),r14
  8C1892E4  74E8  add ##-24,r4
  8C1892E6  3E4C  add r4,r14
  8C1892E8  F4E9  fmov.s @r14+,fr4
  8C1892EA  F6E8  fmov.s @r14,fr6
  8C1892EC  61E3  mov r14,r1
  8C1892EE  F79D  fldi1 fr7
  8C1892F0  7110  add ##16,r1
  8C1892F2  84E7  mov.b @(7,r14),R0
  8C1892F4  58E2  mov.l @(8,r14),r8
  8C1892F6  4015  cmp/pl r0
  8C1892F8  608C  extu.b r8,r0
  8C1892FA  8F26  bf.s 8C18934A
  8C1892FC  F5FD  ftrv xmtrx,fv4
  ...
  8C18934A  7420  add ##32,r4
  8C18934C  405A  lds r0,FPUL
  8C18934E  4819  shlr8 r8
  8C189350  608C  extu.b r8,r0
  8C189352  0483  pref @r4
  8C189354  4819  shlr8 r8
  8C189356  FB2D  float FPUL,fr11
  8C189358  74E0  add ##-32,r4
  8C18935A  F79D  fldi1 fr7
  8C18935C  F743  fdiv fr4,fr7
  8C18935E  405A  lds r0,FPUL
  8C189360  FA2D  float FPUL,fr10
  8C189362  608C  extu.b r8,r0
  8C189364  405A  lds r0,FPUL
  8C189366  4819  shlr8 r8
  8C189368  F92D  float FPUL,fr9
  8C18936A  485A  lds r8,FPUL
  8C18936C  76F8  add ##-8,r6
  8C18936E  F82D  float FPUL,fr8
  8C189370  F018  fmov.s @r1,fr0
  8C189372  6063  mov r6,r0
  8C189374  F38D  fldi0 fr3
  8C189376  FBF2  fmul fr15,fr11
  8C189378  8F03  bf.s 8C189382
  8C18937A  F60B  fmov.s fr0,@-r6
  ...
  8C189382  4021  shar r0
  8C189384  F28D  fldi0 fr2
  8C189386  4310  dt r3
  8C189388  F672  fmul fr7,fr6
  8C18938A  8900  bt 8C18938E
  8C18938C  6063  mov r6,r0
  8C18938E  F572  fmul fr7,fr5
  8C189390  F66B  fmov.s fr6,@-r6
  8C189392  7540  add ##64,r5
  8C189394  F64B  fmov.s fr4,@-r6
  8C189396  FAE2  fmul fr14,fr10
  8C189398  2602  mov.l r0,@r6
  8C18939A  0683  pref @r6
  8C18939C  6E43  mov r4,r14
  8C18939E  5EE1  mov.l @(4,r14),r14
  8C1893A0  7640  add ##64,r6
  8C1893A2  6046  mov.l @r4+,r0
  8C1893A4  F9D2  fmul fr13,fr9
  8C1893A6  F62B  fmov.s fr2,@-r6
  8C1893A8  F8C2  fmul fr12,fr8
  8C1893AA  F62B  fmov.s fr2,@-r6
  8C1893AC  3E4C  add r4,r14
  8C1893AE  F6AB  fmov.s fr10,@-r6
  8C1893B0  7E04  add ##4,r14
  8C1893B2  F68B  fmov.s fr8,@-r6
  8C1893B4  8D08  bt.s 8C1893C8
  8C1893B6  0683  pref @r6
  8C1893B8  C801  tst ##1,R0
  8C1893BA  7640  add ##64,r6
  8C1893BC  8D94  bt.s 8C1892E8
  8C1893BE  7404  add ##4,r4
  8C1893C0  6E43  mov r4,r14
  8C1893C2  7418  add ##24,r4
  8C1893C4  AF90  bra 8C1892E8
  8C1893C6  7EF8  add ##-8,r14
  8C1893C8  74FC  add ##-4,r4
  8C1893CA  4910  dt r9
  8C1893CC  7640  add ##64,r6
  8C1893CE  8F83  bf.s 8C1892D8
  8C1893D0  E303  mov ##0x03,r3
  8C1893D2  F3FD  fschg
  8C1893D4  000B  rts
  8C1893D6  76E0  add ##-32,r6
```
