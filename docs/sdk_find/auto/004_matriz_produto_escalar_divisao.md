# 004_matriz_produto_escalar_divisao

> Gerado por `tools/sdk_find.py`. Jogos: 6 · variantes (sequências normalizadas distintas): 10 · tempo perf somado: 9.77% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C17C3D0` | 141 | 0.00% |
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C17FDF0` | 283 | 0.00% |
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C180150` | 129 | 0.00% |
| Dead or Alive 2 (USA) | `8C101930` | 141 | 0.26% |
| Dead or Alive 2 (USA) | `8C102A80` | 203 | 6.28% |
| Dead or Alive 2 (USA) | `8C102DE0` | 192 | 0.33% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C12A7F0` | 141 | 2.52% |
| Power Stone (USA) | `0C0E7610` | 138 | 0.00% |
| Project Justice (USA) | `0C150C00` | 140 | 0.00% |
| Project Justice (USA) | `0C1546BC` | 140 | 0.00% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C1D8A90` | 138 | 0.22% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C1DC360` | 142 | 0.16% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C17C3D0`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C17C3D0  6346  mov.l @r4+,r3
  8C17C3D2  F29D  fldi1 fr2
  8C17C3D4  F3FD  fschg
  8C17C3D6  F38D  fldi0 fr3
  8C17C3D8  F62B  fmov.s fr2,@-r6
  8C17C3DA  7620  add ##32,r6
  8C17C3DC  F62A  fmov.s fr2,@r6
  8C17C3DE  6042  mov.l @r4,r0
  8C17C3E0  6E43  mov r4,r14
  8C17C3E2  C801  tst ##1,R0
  8C17C3E4  7420  add ##32,r4
  8C17C3E6  8B02  bf 8C17C3EE
  8C17C3E8  5EE1  mov.l @(4,r14),r14
  8C17C3EA  74E8  add ##-24,r4
  8C17C3EC  3E4C  add r4,r14
  8C17C3EE  F4E9  fmov.s @r14+,fr4
  8C17C3F0  F6E8  fmov.s @r14,fr6
  8C17C3F2  7E10  add ##16,r14
  8C17C3F4  F79D  fldi1 fr7
  8C17C3F6  F0E8  fmov.s @r14,fr0
  8C17C3F8  F5FD  ftrv xmtrx,fv4
  8C17C3FA  F79D  fldi1 fr7
  8C17C3FC  F743  fdiv fr4,fr7
  8C17C3FE  4310  dt r3
  8C17C400  6046  mov.l @r4+,r0
  8C17C402  8D53  bt.s 8C17C4AC
  8C17C404  6E46  mov.l @r4+,r14
  8C17C406  C801  tst ##1,R0
  8C17C408  6263  mov r6,r2
  8C17C40A  8D03  bt.s 8C17C414
  8C17C40C  3E4C  add r4,r14
  8C17C40E  6E43  mov r4,r14
  8C17C410  7418  add ##24,r4
  8C17C412  7EF8  add ##-8,r14
  8C17C414  F8E9  fmov.s @r14+,fr8
  8C17C416  7420  add ##32,r4
  8C17C418  0483  pref @r4
  8C17C41A  FAE8  fmov.s @r14,fr10
  8C17C41C  7E10  add ##16,r14
  8C17C41E  FB9D  fldi1 fr11
  8C17C420  F672  fmul fr7,fr6
  8C17C422  F60B  fmov.s fr0,@-r6
  8C17C424  F572  fmul fr7,fr5
  8C17C426  F66B  fmov.s fr6,@-r6
  8C17C428  F9FD  ftrv xmtrx,fv8
  8C17C42A  F64B  fmov.s fr4,@-r6
  8C17C42C  74E0  add ##-32,r4
  8C17C42E  2622  mov.l r2,@r6
  8C17C430  F0E8  fmov.s @r14,fr0
  8C17C432  4310  dt r3
  8C17C434  0683  pref @r6
  8C17C436  7638  add ##56,r6
  8C17C438  6263  mov r6,r2
  8C17C43A  FB9D  fldi1 fr11
  8C17C43C  FB83  fdiv fr8,fr11
  8C17C43E  6046  mov.l @r4+,r0
  8C17C440  8D1A  bt.s 8C17C478
  8C17C442  6E46  mov.l @r4+,r14
  8C17C444  C801  tst ##1,R0
  8C17C446  8D03  bt.s 8C17C450
  8C17C448  3E4C  add r4,r14
  8C17C44A  6E43  mov r4,r14
  8C17C44C  7418  add ##24,r4
  8C17C44E  7EF8  add ##-8,r14
  8C17C450  7420  add ##32,r4
  8C17C452  F4E9  fmov.s @r14+,fr4
  8C17C454  0483  pref @r4
  8C17C456  F6E8  fmov.s @r14,fr6
  8C17C458  7E10  add ##16,r14
  8C17C45A  F79D  fldi1 fr7
  8C17C45C  FAB2  fmul fr11,fr10
  8C17C45E  74E0  add ##-32,r4
  8C17C460  F9B2  fmul fr11,fr9
  8C17C462  F60B  fmov.s fr0,@-r6
  8C17C464  F6AB  fmov.s fr10,@-r6
  8C17C466  F5FD  ftrv xmtrx,fv4
  8C17C468  F68B  fmov.s fr8,@-r6
  8C17C46A  7540  add ##64,r5
  8C17C46C  2622  mov.l r2,@r6
  8C17C46E  F0E8  fmov.s @r14,fr0
  8C17C470  0683  pref @r6
  8C17C472  AFC2  bra 8C17C3FA
  8C17C474  7638  add ##56,r6
  ...
  8C17C478  4015  cmp/pl r0
  8C17C47A  6246  mov.l @r4+,r2
  8C17C47C  8B0E  bf 8C17C49C
  8C17C47E  C880  tst ##128,R0
  8C17C480  63E3  mov r14,r3
  8C17C482  890B  bt 8C17C49C
  8C17C484  6023  mov r2,r0
  8C17C486  6263  mov r6,r2
  8C17C488  4221  shar r2
  8C17C48A  6E46  mov.l @r4+,r14
  8C17C48C  C801  tst ##1,R0
  8C17C48E  8DDF  bt.s 8C17C450
  8C17C490  3E4C  add r4,r14
  8C17C492  6E43  mov r4,r14
  8C17C494  7418  add ##24,r4
  8C17C496  AFDB  bra 8C17C450
  8C17C498  7EF8  add ##-8,r14
  ...
  8C17C49C  742C  add ##44,r4
  8C17C49E  0483  pref @r4
  8C17C4A0  74D0  add ##-48,r4
  8C17C4A2  F48C  fmov fr8,fr4
  8C17C4A4  7520  add ##32,r5
  8C17C4A6  A016  bra 8C17C4D6
  8C17C4A8  F6AC  fmov fr10,fr6
  ...
  8C17C4AC  4015  cmp/pl r0
  8C17C4AE  6246  mov.l @r4+,r2
  8C17C4B0  8B0E  bf 8C17C4D0
  8C17C4B2  C880  tst ##128,R0
  8C17C4B4  63E3  mov r14,r3
  8C17C4B6  890B  bt 8C17C4D0
  8C17C4B8  6023  mov r2,r0
  8C17C4BA  6263  mov r6,r2
  8C17C4BC  4221  shar r2
  8C17C4BE  6E46  mov.l @r4+,r14
  8C17C4C0  C801  tst ##1,R0
  8C17C4C2  8DA7  bt.s 8C17C414
  8C17C4C4  3E4C  add r4,r14
  8C17C4C6  6E43  mov r4,r14
  8C17C4C8  7418  add ##24,r4
  8C17C4CA  AFA3  bra 8C17C414
  8C17C4CC  7EF8  add ##-8,r14
  ...
  8C17C4D0  742C  add ##44,r4
  8C17C4D2  0483  pref @r4
  8C17C4D4  74D0  add ##-48,r4
  8C17C4D6  F672  fmul fr7,fr6
  8C17C4D8  6263  mov r6,r2
  8C17C4DA  4221  shar r2
  8C17C4DC  F572  fmul fr7,fr5
  8C17C4DE  F60B  fmov.s fr0,@-r6
  8C17C4E0  74F8  add ##-8,r4
  8C17C4E2  F66B  fmov.s fr6,@-r6
  8C17C4E4  7520  add ##32,r5
  8C17C4E6  F64B  fmov.s fr4,@-r6
  8C17C4E8  2622  mov.l r2,@r6
  8C17C4EA  F3FD  fschg
  8C17C4EC  0683  pref @r6
  8C17C4EE  000B  rts
  8C17C4F0  7620  add ##32,r6
```

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C17FDF0`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C17FDF0  4811  cmp/pz r8
  8C17FDF2  8901  bt 8C17FDF8
  8C17FDF4  A116  bra 8C180024
  8C17FDF6  0009  nop
  8C17FDF8  6346  mov.l @r4+,r3
  8C17FDFA  F08D  fldi0 fr0
  8C17FDFC  7640  add ##64,r6
  8C17FDFE  F18D  fldi0 fr1
  8C17FE00  F3FD  fschg
  8C17FE02  F60B  fmov.s fr0,@-r6
  8C17FE04  F60B  fmov.s fr0,@-r6
  8C17FE06  F3FD  fschg
  8C17FE08  6042  mov.l @r4,r0
  8C17FE0A  6E43  mov r4,r14
  8C17FE0C  C801  tst ##1,R0
  8C17FE0E  DD08  mov.l @([8C17FE30]),r13
  8C17FE10  8F03  bf.s 8C17FE1A
  8C17FE12  7420  add ##32,r4
  8C17FE14  5EE1  mov.l @(4,r14),r14
  8C17FE16  74E8  add ##-24,r4
  8C17FE18  3E4C  add r4,r14
  8C17FE1A  D006  mov.l @([8C17FE34]),r0
  8C17FE1C  F3FD  fschg
  8C17FE1E  6DD2  mov.l @r13,r13
  8C17FE20  FC09  fmov.s @r0+,fr12
  8C17FE22  FE09  fmov.s @r0+,fr14
  8C17FE24  F4E9  fmov.s @r14+,fr4
  8C17FE26  7420  add ##32,r4
  8C17FE28  F6E9  fmov.s @r14+,fr6
  8C17FE2A  A012  bra 8C17FE52
  8C17FE2C  F3FD  fschg
  ...
  8C17FE38  D01A  mov.l @([8C17FEA4]),r0
  8C17FE3A  7620  add ##32,r6
  8C17FE3C  6DD2  mov.l @r13,r13
  8C17FE3E  6163  mov r6,r1
  8C17FE40  FC09  fmov.s @r0+,fr12
  8C17FE42  7540  add ##64,r5
  8C17FE44  FE09  fmov.s @r0+,fr14
  8C17FE46  7440  add ##64,r4
  8C17FE48  F4E9  fmov.s @r14+,fr4
  8C17FE4A  7650  add ##80,r6
  8C17FE4C  F6E9  fmov.s @r14+,fr6
  8C17FE4E  F3FD  fschg
  8C17FE50  0183  pref @r1
  8C17FE52  F87C  fmov fr7,fr8
  8C17FE54  2DD8  tst r13,r13
  8C17FE56  F9E9  fmov.s @r14+,fr9
  8C17FE58  8D28  bt.s 8C17FEAC
  8C17FE5A  FAE9  fmov.s @r14+,fr10
  8C17FE5C  4D01  shlr r13
  8C17FE5E  D212  mov.l @([8C17FEA8]),r2
  8C17FE60  8B18  bf 8C17FE94
  8C17FE62  8521  mov.w @(2,r2),R0
  8C17FE64  F38D  fldi0 fr3
  8C17FE66  8802  cmp/eq ##0x02,R0
  8C17FE68  E174  mov ##0x74,r1
  8C17FE6A  8F23  bf.s 8C17FEB4
  8C17FE6C  312C  add r2,r1
  8C17FE6E  F79D  fldi1 fr7
  8C17FE70  2DD8  tst r13,r13
  8C17FE72  710C  add ##12,r1
  8C17FE74  8F01  bf.s 8C17FE7A
  8C17FE76  F019  fmov.s @r1+,fr0
  8C17FE78  F5FD  ftrv xmtrx,fv4
  8C17FE7A  F119  fmov.s @r1+,fr1
  8C17FE7C  6023  mov r2,r0
  8C17FE7E  F219  fmov.s @r1+,fr2
  8C17FE80  7018  add ##24,r0
  8C17FE82  F8ED  fipr fv12,fv8
  8C17FE84  F109  fmov.s @r0+,fr1
  8C17FE86  F209  fmov.s @r0+,fr2
  8C17FE88  F309  fmov.s @r0+,fr3
  8C17FE8A  F08D  fldi0 fr0
  8C17FE8C  F0B5  fcmp/gt fr11,fr0
  8C17FE8E  F0BC  fmov fr11,fr0
  8C17FE90  8F53  bf.s 8C17FF3A
  8C17FE92  2DD8  tst r13,r13
  8C17FE94  E0B0  mov ##0xB0,r0
  8C17FE96  8960  bt 8C17FF5A
  ...
  8C17FF3A  FD1E  fmac fr0,fr1,fr13
  8C17FF3C  E0B0  mov ##0xB0,r0
  8C17FF3E  FE2E  fmac fr0,fr2,fr14
  8C17FF40  8D0B  bt.s 8C17FF5A
  8C17FF42  FF3E  fmac fr0,fr3,fr15
  ...
  8C17FF5A  D231  mov.l @([8C180020]),r2
  8C17FF5C  F3FD  fschg
  8C17FF5E  F79D  fldi1 fr7
  8C17FF60  F743  fdiv fr4,fr7
  8C17FF62  0483  pref @r4
  8C17FF64  74E0  add ##-32,r4
  8C17FF66  F829  fmov.s @r2+,fr8
  8C17FF68  6163  mov r6,r1
  8C17FF6A  4121  shar r1
  8C17FF6C  FA29  fmov.s @r2+,fr10
  8C17FF6E  F9D2  fmul fr13,fr9
  8C17FF70  6042  mov.l @r4,r0
  8C17FF72  FAE2  fmul fr14,fr10
  8C17FF74  F2E9  fmov.s @r14+,fr2
  8C17FF76  FBF2  fmul fr15,fr11
  8C17FF78  DD10  mov.l @([8C17FFBC]),r13
  8C17FF7A  4310  dt r3
  8C17FF7C  F6AB  fmov.s fr10,@-r6
  8C17FF7E  6E43  mov r4,r14
  8C17FF80  F68B  fmov.s fr8,@-r6
  8C17FF82  76F8  add ##-8,r6
  8C17FF84  F672  fmul fr7,fr6
  8C17FF86  F62B  fmov.s fr2,@-r6
  8C17FF88  F572  fmul fr7,fr5
  8C17FF8A  F66B  fmov.s fr6,@-r6
  8C17FF8C  8D1A  bt.s 8C17FFC4
  8C17FF8E  F64B  fmov.s fr4,@-r6
  8C17FF90  C801  tst ##1,R0
  8C17FF92  2662  mov.l r6,@r6
  8C17FF94  8900  bt 8C17FF98
  8C17FF96  AF4F  bra 8C17FE38
  8C17FF98  0683  pref @r6
  ...
  8C17FFC4  6046  mov.l @r4+,r0
  8C17FFC6  2612  mov.l r1,@r6
  8C17FFC8  4015  cmp/pl r0
  8C17FFCA  0683  pref @r6
  8C17FFCC  7620  add ##32,r6
  8C17FFCE  8B1D  bf 8C18000C
  8C17FFD0  C880  tst ##128,R0
  8C17FFD2  DD0C  mov.l @([8C180004]),r13
  8C17FFD4  891A  bt 8C18000C
  8C17FFD6  6346  mov.l @r4+,r3
  8C17FFD8  6163  mov r6,r1
  8C17FFDA  6042  mov.l @r4,r0
  8C17FFDC  6E43  mov r4,r14
  8C17FFDE  6DD2  mov.l @r13,r13
  8C17FFE0  C801  tst ##1,R0
  8C17FFE2  D009  mov.l @([8C180008]),r0
  8C17FFE4  7540  add ##64,r5
  8C17FFE6  8900  bt 8C17FFEA
  8C17FFE8  AF2C  bra 8C17FE44
  8C17FFEA  FC09  fmov.s @r0+,fr12
  ...
  8C18000C  74FC  add ##-4,r4
  8C18000E  F3FD  fschg
  8C180010  7540  add ##64,r5
  8C180012  0683  pref @r6
  8C180014  000B  rts
  8C180016  7620  add ##32,r6
  ...
  8C180024  6346  mov.l @r4+,r3
  8C180026  F29D  fldi1 fr2
  8C180028  F3FD  fschg
  8C18002A  F39D  fldi1 fr3
  8C18002C  F62B  fmov.s fr2,@-r6
  8C18002E  7620  add ##32,r6
  8C180030  F62A  fmov.s fr2,@r6
  8C180032  6042  mov.l @r4,r0
  8C180034  6E43  mov r4,r14
  8C180036  C801  tst ##1,R0
  8C180038  7420  add ##32,r4
  8C18003A  8B02  bf 8C180042
  8C18003C  5EE1  mov.l @(4,r14),r14
  8C18003E  74E8  add ##-24,r4
  8C180040  3E4C  add r4,r14
  8C180042  F4E9  fmov.s @r14+,fr4
  8C180044  F6E8  fmov.s @r14,fr6
  8C180046  7E10  add ##16,r14
  8C180048  F79D  fldi1 fr7
  8C18004A  F0E8  fmov.s @r14,fr0
  8C18004C  F5FD  ftrv xmtrx,fv4
  8C18004E  F79D  fldi1 fr7
  8C180050  F743  fdiv fr4,fr7
  8C180052  4310  dt r3
  8C180054  6046  mov.l @r4+,r0
  8C180056  8D53  bt.s 8C180100
  8C180058  6E46  mov.l @r4+,r14
  8C18005A  C801  tst ##1,R0
  8C18005C  6263  mov r6,r2
  8C18005E  8D03  bt.s 8C180068
  8C180060  3E4C  add r4,r14
  8C180062  6E43  mov r4,r14
  8C180064  7418  add ##24,r4
  8C180066  7EF8  add ##-8,r14
  8C180068  F8E9  fmov.s @r14+,fr8
  8C18006A  7420  add ##32,r4
  8C18006C  0483  pref @r4
  8C18006E  FAE8  fmov.s @r14,fr10
  8C180070  7E10  add ##16,r14
  8C180072  FB9D  fldi1 fr11
  8C180074  F672  fmul fr7,fr6
  8C180076  F60B  fmov.s fr0,@-r6
  8C180078  F572  fmul fr7,fr5
  8C18007A  F66B  fmov.s fr6,@-r6
  8C18007C  F9FD  ftrv xmtrx,fv8
  8C18007E  F64B  fmov.s fr4,@-r6
  8C180080  74E0  add ##-32,r4
  8C180082  2622  mov.l r2,@r6
  8C180084  F0E8  fmov.s @r14,fr0
  8C180086  4310  dt r3
  8C180088  0683  pref @r6
  8C18008A  7638  add ##56,r6
  8C18008C  6263  mov r6,r2
  8C18008E  FB9D  fldi1 fr11
  8C180090  FB83  fdiv fr8,fr11
  8C180092  6046  mov.l @r4+,r0
  8C180094  8D1A  bt.s 8C1800CC
  8C180096  6E46  mov.l @r4+,r14
  8C180098  C801  tst ##1,R0
  8C18009A  8D03  bt.s 8C1800A4
  8C18009C  3E4C  add r4,r14
  8C18009E  6E43  mov r4,r14
  8C1800A0  7418  add ##24,r4
  8C1800A2  7EF8  add ##-8,r14
  8C1800A4  7420  add ##32,r4
  8C1800A6  F4E9  fmov.s @r14+,fr4
  8C1800A8  0483  pref @r4
  8C1800AA  F6E8  fmov.s @r14,fr6
  8C1800AC  7E10  add ##16,r14
  8C1800AE  F79D  fldi1 fr7
  8C1800B0  FAB2  fmul fr11,fr10
  8C1800B2  74E0  add ##-32,r4
  8C1800B4  F9B2  fmul fr11,fr9
  8C1800B6  F60B  fmov.s fr0,@-r6
  8C1800B8  F6AB  fmov.s fr10,@-r6
  8C1800BA  F5FD  ftrv xmtrx,fv4
  8C1800BC  F68B  fmov.s fr8,@-r6
  8C1800BE  7540  add ##64,r5
  8C1800C0  2622  mov.l r2,@r6
  8C1800C2  F0E8  fmov.s @r14,fr0
  8C1800C4  0683  pref @r6
  8C1800C6  AFC2  bra 8C18004E
  8C1800C8  7638  add ##56,r6
  ...
  8C1800CC  4015  cmp/pl r0
  8C1800CE  6246  mov.l @r4+,r2
  8C1800D0  8B0E  bf 8C1800F0
  8C1800D2  C880  tst ##128,R0
  8C1800D4  63E3  mov r14,r3
  8C1800D6  890B  bt 8C1800F0
  8C1800D8  6023  mov r2,r0
  8C1800DA  6263  mov r6,r2
  8C1800DC  4221  shar r2
  8C1800DE  6E46  mov.l @r4+,r14
  8C1800E0  C801  tst ##1,R0
  8C1800E2  8DDF  bt.s 8C1800A4
  8C1800E4  3E4C  add r4,r14
  8C1800E6  6E43  mov r4,r14
  8C1800E8  7418  add ##24,r4
  8C1800EA  AFDB  bra 8C1800A4
  8C1800EC  7EF8  add ##-8,r14
  ...
  8C1800F0  742C  add ##44,r4
  8C1800F2  0483  pref @r4
  8C1800F4  74D0  add ##-48,r4
  8C1800F6  F48C  fmov fr8,fr4
  8C1800F8  7520  add ##32,r5
  8C1800FA  A016  bra 8C18012A
  8C1800FC  F6AC  fmov fr10,fr6
  ...
  8C180100  4015  cmp/pl r0
  8C180102  6246  mov.l @r4+,r2
  8C180104  8B0E  bf 8C180124
  8C180106  C880  tst ##128,R0
  8C180108  63E3  mov r14,r3
  8C18010A  890B  bt 8C180124
  8C18010C  6023  mov r2,r0
  8C18010E  6263  mov r6,r2
  8C180110  4221  shar r2
  8C180112  6E46  mov.l @r4+,r14
  8C180114  C801  tst ##1,R0
  8C180116  8DA7  bt.s 8C180068
  8C180118  3E4C  add r4,r14
  8C18011A  6E43  mov r4,r14
  8C18011C  7418  add ##24,r4
  8C18011E  AFA3  bra 8C180068
  8C180120  7EF8  add ##-8,r14
  ...
  8C180124  742C  add ##44,r4
  8C180126  0483  pref @r4
  8C180128  74D0  add ##-48,r4
  8C18012A  F672  fmul fr7,fr6
  8C18012C  6263  mov r6,r2
  8C18012E  4221  shar r2
  8C180130  F572  fmul fr7,fr5
  8C180132  F60B  fmov.s fr0,@-r6
  8C180134  74F8  add ##-8,r4
  8C180136  F66B  fmov.s fr6,@-r6
  8C180138  7520  add ##32,r5
  8C18013A  F64B  fmov.s fr4,@-r6
  8C18013C  2622  mov.l r2,@r6
  8C18013E  F3FD  fschg
  8C180140  0683  pref @r6
  8C180142  000B  rts
  8C180144  7620  add ##32,r6
```

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C180150`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C180150  4811  cmp/pz r8
  8C180152  8901  bt 8C180158
  ...
  8C180158  6946  mov.l @r4+,r9
  8C18015A  E303  mov ##0x03,r3
  8C18015C  F08D  fldi0 fr0
  8C18015E  7640  add ##64,r6
  8C180160  F18D  fldi0 fr1
  8C180162  F3FD  fschg
  8C180164  F60B  fmov.s fr0,@-r6
  8C180166  F60B  fmov.s fr0,@-r6
  8C180168  F3FD  fschg
  8C18016A  6042  mov.l @r4,r0
  8C18016C  6E43  mov r4,r14
  8C18016E  C801  tst ##1,R0
  8C180170  DD07  mov.l @([8C180190]),r13
  8C180172  8F03  bf.s 8C18017C
  8C180174  7420  add ##32,r4
  8C180176  5EE1  mov.l @(4,r14),r14
  8C180178  74E8  add ##-24,r4
  8C18017A  3E4C  add r4,r14
  8C18017C  D005  mov.l @([8C180194]),r0
  8C18017E  F3FD  fschg
  8C180180  6DD2  mov.l @r13,r13
  8C180182  FC09  fmov.s @r0+,fr12
  8C180184  FE09  fmov.s @r0+,fr14
  8C180186  F4E9  fmov.s @r14+,fr4
  8C180188  7420  add ##32,r4
  8C18018A  F6E9  fmov.s @r14+,fr6
  8C18018C  A011  bra 8C1801B2
  8C18018E  F3FD  fschg
  ...
  8C180198  D01A  mov.l @([8C180204]),r0
  8C18019A  7620  add ##32,r6
  8C18019C  6DD2  mov.l @r13,r13
  8C18019E  6163  mov r6,r1
  8C1801A0  FC09  fmov.s @r0+,fr12
  8C1801A2  7540  add ##64,r5
  8C1801A4  FE09  fmov.s @r0+,fr14
  8C1801A6  7440  add ##64,r4
  8C1801A8  F4E9  fmov.s @r14+,fr4
  8C1801AA  7650  add ##80,r6
  8C1801AC  F6E9  fmov.s @r14+,fr6
  8C1801AE  F3FD  fschg
  8C1801B0  0183  pref @r1
  8C1801B2  F87C  fmov fr7,fr8
  8C1801B4  2DD8  tst r13,r13
  8C1801B6  F9E9  fmov.s @r14+,fr9
  8C1801B8  8D28  bt.s 8C18020C
  8C1801BA  FAE9  fmov.s @r14+,fr10
  8C1801BC  4D01  shlr r13
  8C1801BE  D212  mov.l @([8C180208]),r2
  8C1801C0  8B18  bf 8C1801F4
  8C1801C2  8521  mov.w @(2,r2),R0
  8C1801C4  F38D  fldi0 fr3
  8C1801C6  8802  cmp/eq ##0x02,R0
  8C1801C8  E174  mov ##0x74,r1
  8C1801CA  8F23  bf.s 8C180214
  8C1801CC  312C  add r2,r1
  8C1801CE  F79D  fldi1 fr7
  8C1801D0  2DD8  tst r13,r13
  8C1801D2  710C  add ##12,r1
  8C1801D4  8F01  bf.s 8C1801DA
  8C1801D6  F019  fmov.s @r1+,fr0
  8C1801D8  F5FD  ftrv xmtrx,fv4
  8C1801DA  F119  fmov.s @r1+,fr1
  8C1801DC  6023  mov r2,r0
  8C1801DE  F219  fmov.s @r1+,fr2
  8C1801E0  7018  add ##24,r0
  8C1801E2  F8ED  fipr fv12,fv8
  8C1801E4  F109  fmov.s @r0+,fr1
  8C1801E6  F209  fmov.s @r0+,fr2
  8C1801E8  F309  fmov.s @r0+,fr3
  8C1801EA  F08D  fldi0 fr0
  8C1801EC  F0B5  fcmp/gt fr11,fr0
  8C1801EE  F0BC  fmov fr11,fr0
  8C1801F0  8F53  bf.s 8C18029A
  8C1801F2  2DD8  tst r13,r13
  8C1801F4  E0B0  mov ##0xB0,r0
  8C1801F6  8960  bt 8C1802BA
  ...
  8C18029A  FD1E  fmac fr0,fr1,fr13
  8C18029C  E0B0  mov ##0xB0,r0
  8C18029E  FE2E  fmac fr0,fr2,fr14
  8C1802A0  8D0B  bt.s 8C1802BA
  8C1802A2  FF3E  fmac fr0,fr3,fr15
  ...
  8C1802BA  D221  mov.l @([8C180340]),r2
  8C1802BC  F3FD  fschg
  8C1802BE  F79D  fldi1 fr7
  8C1802C0  F743  fdiv fr4,fr7
  8C1802C2  0483  pref @r4
  8C1802C4  74E0  add ##-32,r4
  8C1802C6  F829  fmov.s @r2+,fr8
  8C1802C8  6163  mov r6,r1
  8C1802CA  4121  shar r1
  8C1802CC  FA29  fmov.s @r2+,fr10
  8C1802CE  F9D2  fmul fr13,fr9
  8C1802D0  6042  mov.l @r4,r0
  8C1802D2  FAE2  fmul fr14,fr10
  8C1802D4  F2E9  fmov.s @r14+,fr2
  8C1802D6  FBF2  fmul fr15,fr11
  8C1802D8  DD10  mov.l @([8C18031C]),r13
  8C1802DA  4310  dt r3
  8C1802DC  F6AB  fmov.s fr10,@-r6
  8C1802DE  6E43  mov r4,r14
  8C1802E0  F68B  fmov.s fr8,@-r6
  8C1802E2  76F8  add ##-8,r6
  8C1802E4  F672  fmul fr7,fr6
  8C1802E6  F62B  fmov.s fr2,@-r6
  8C1802E8  F572  fmul fr7,fr5
  8C1802EA  F66B  fmov.s fr6,@-r6
  8C1802EC  8D1A  bt.s 8C180324
  8C1802EE  F64B  fmov.s fr4,@-r6
  8C1802F0  C801  tst ##1,R0
  8C1802F2  2662  mov.l r6,@r6
  8C1802F4  8900  bt 8C1802F8
  8C1802F6  AF4F  bra 8C180198
  8C1802F8  0683  pref @r6
  ...
  8C180324  2612  mov.l r1,@r6
  8C180326  4910  dt r9
  8C180328  0683  pref @r6
  8C18032A  7620  add ##32,r6
  8C18032C  F3FD  fschg
  8C18032E  7540  add ##64,r5
  8C180330  0683  pref @r6
  8C180332  76E0  add ##-32,r6
  8C180334  8D02  bt.s 8C18033C
  8C180336  7670  add ##112,r6
  8C180338  AF17  bra 8C18016A
  8C18033A  E303  mov ##0x03,r3
  8C18033C  000B  rts
  8C18033E  76D0  add ##-48,r6
```

## Dead or Alive 2 (USA) `8C102A80`

Dump: `/mnt/1TB/dcbat_off/20261007-192359_Dead_or_Alive_2__USA__/jit-14513.txt`

```
  8C102A80  4811  cmp/pz r8
  8C102A82  8901  bt 8C102A88
  ...
  8C102A88  6346  mov.l @r4+,r3
  8C102A8A  F08D  fldi0 fr0
  8C102A8C  7640  add ##64,r6
  8C102A8E  F18D  fldi0 fr1
  8C102A90  F3FD  fschg
  8C102A92  F60B  fmov.s fr0,@-r6
  8C102A94  F60B  fmov.s fr0,@-r6
  8C102A96  F3FD  fschg
  8C102A98  6042  mov.l @r4,r0
  8C102A9A  6E43  mov r4,r14
  8C102A9C  C801  tst ##1,R0
  8C102A9E  DD83  mov.l @([8C102CAC]),r13
  8C102AA0  8F03  bf.s 8C102AAA
  8C102AA2  7420  add ##32,r4
  8C102AA4  5EE1  mov.l @(4,r14),r14
  8C102AA6  74E8  add ##-24,r4
  8C102AA8  3E4C  add r4,r14
  8C102AAA  D07E  mov.l @([8C102CA4]),r0
  8C102AAC  F3FD  fschg
  8C102AAE  6DD2  mov.l @r13,r13
  8C102AB0  FC09  fmov.s @r0+,fr12
  8C102AB2  FE09  fmov.s @r0+,fr14
  8C102AB4  F4E9  fmov.s @r14+,fr4
  8C102AB6  7420  add ##32,r4
  8C102AB8  F6E9  fmov.s @r14+,fr6
  8C102ABA  A00E  bra 8C102ADA
  8C102ABC  F3FD  fschg
  ...
  8C102AC0  D078  mov.l @([8C102CA4]),r0
  8C102AC2  7620  add ##32,r6
  8C102AC4  6DD2  mov.l @r13,r13
  8C102AC6  6163  mov r6,r1
  8C102AC8  FC09  fmov.s @r0+,fr12
  8C102ACA  7540  add ##64,r5
  8C102ACC  FE09  fmov.s @r0+,fr14
  8C102ACE  7440  add ##64,r4
  8C102AD0  F4E9  fmov.s @r14+,fr4
  8C102AD2  7650  add ##80,r6
  8C102AD4  F6E9  fmov.s @r14+,fr6
  8C102AD6  F3FD  fschg
  8C102AD8  0183  pref @r1
  8C102ADA  F87C  fmov fr7,fr8
  8C102ADC  2DD8  tst r13,r13
  8C102ADE  F9E9  fmov.s @r14+,fr9
  8C102AE0  8D24  bt.s 8C102B2C
  8C102AE2  FAE9  fmov.s @r14+,fr10
  8C102AE4  4D01  shlr r13
  8C102AE6  D270  mov.l @([8C102CA8]),r2
  8C102AE8  8B18  bf 8C102B1C
  ...
  8C102AEC  F38D  fldi0 fr3
  8C102AEE  8802  cmp/eq ##0x02,R0
  8C102AF0  E174  mov ##0x74,r1
  8C102AF2  8F1F  bf.s 8C102B34
  8C102AF4  312C  add r2,r1
  8C102AF6  F79D  fldi1 fr7
  8C102AF8  2DD8  tst r13,r13
  8C102AFA  710C  add ##12,r1
  8C102AFC  8F01  bf.s 8C102B02
  8C102AFE  F019  fmov.s @r1+,fr0
  8C102B00  F5FD  ftrv xmtrx,fv4
  8C102B02  F119  fmov.s @r1+,fr1
  8C102B04  6023  mov r2,r0
  8C102B06  F219  fmov.s @r1+,fr2
  8C102B08  7018  add ##24,r0
  8C102B0A  F8ED  fipr fv12,fv8
  8C102B0C  F109  fmov.s @r0+,fr1
  8C102B0E  F209  fmov.s @r0+,fr2
  8C102B10  F309  fmov.s @r0+,fr3
  8C102B12  F08D  fldi0 fr0
  8C102B14  F0B5  fcmp/gt fr11,fr0
  8C102B16  F0BC  fmov fr11,fr0
  8C102B18  8F4F  bf.s 8C102BBA
  8C102B1A  2DD8  tst r13,r13
  8C102B1C  E0B0  mov ##0xB0,r0
  8C102B1E  895C  bt 8C102BDA
  8C102B20  4D01  shlr r13
  8C102B22  600C  extu.b r0,r0
  8C102B24  8F53  bf.s 8C102BCE
  8C102B26  320C  add r0,r2
  8C102B28  AFE0  bra 8C102AEC
  8C102B2A  8521  mov.w @(2,r2),R0
  ...
  8C102B34  F019  fmov.s @r1+,fr0
  8C102B36  2DD8  tst r13,r13
  8C102B38  F119  fmov.s @r1+,fr1
  8C102B3A  F041  fsub fr4,fr0
  8C102B3C  F219  fmov.s @r1+,fr2
  8C102B3E  F151  fsub fr5,fr1
  8C102B40  F79D  fldi1 fr7
  8C102B42  7118  add ##24,r1
  8C102B44  FB8D  fldi0 fr11
  8C102B46  8F01  bf.s 8C102B4C
  8C102B48  F261  fsub fr6,fr2
  ...
  8C102B4C  F8ED  fipr fv12,fv8
  8C102B4E  FC8D  fldi0 fr12
  8C102B50  F0ED  fipr fv12,fv0
  8C102B52  FCB5  fcmp/gt fr11,fr12
  8C102B54  FC18  fmov.s @r1,fr12
  8C102B56  8937  bt 8C102BC8
  8C102B58  FC35  fcmp/gt fr3,fr12
  8C102B5A  8B35  bf 8C102BC8
  8C102B5C  8803  cmp/eq ##0x03,R0
  8C102B5E  F31D  flds fr3,FPUL
  8C102B60  8D13  bt.s 8C102B8A
  8C102B62  F37D  FSRRA fr3
  ...
  8C102B8A  71BC  add ##-68,r1
  8C102B8C  F218  fmov.s @r1,fr2
  8C102B8E  71F8  add ##-8,r1
  8C102B90  F00D  fsts FPUL,fr0
  8C102B92  FB32  fmul fr3,fr11
  8C102B94  FC18  fmov.s @r1,fr12
  8C102B96  F302  fmul fr0,fr3
  8C102B98  7104  add ##4,r1
  8C102B9A  FC2E  fmac fr0,fr2,fr12
  8C102B9C  F218  fmov.s @r1,fr2
  8C102B9E  6023  mov r2,r0
  8C102BA0  F03C  fmov fr3,fr0
  8C102BA2  FC2E  fmac fr0,fr2,fr12
  8C102BA4  F0BC  fmov fr11,fr0
  8C102BA6  7018  add ##24,r0
  8C102BA8  FBC3  fdiv fr12,fr11
  8C102BAA  F29D  fldi1 fr2
  8C102BAC  FC25  fcmp/gt fr2,fr12
  8C102BAE  F109  fmov.s @r0+,fr1
  8C102BB0  F209  fmov.s @r0+,fr2
  8C102BB2  8F01  bf.s 8C102BB8
  8C102BB4  F309  fmov.s @r0+,fr3
  8C102BB6  F0BC  fmov fr11,fr0
  8C102BB8  2DD8  tst r13,r13
  8C102BBA  FD1E  fmac fr0,fr1,fr13
  8C102BBC  E0B0  mov ##0xB0,r0
  8C102BBE  FE2E  fmac fr0,fr2,fr14
  8C102BC0  8D0B  bt.s 8C102BDA
  8C102BC2  FF3E  fmac fr0,fr3,fr15
  8C102BC4  A004  bra 8C102BD0
  8C102BC6  4D01  shlr r13
  8C102BC8  2DD8  tst r13,r13
  8C102BCA  E0B0  mov ##0xB0,r0
  8C102BCC  8D05  bt.s 8C102BDA
  8C102BCE  4D01  shlr r13
  8C102BD0  600C  extu.b r0,r0
  8C102BD2  8FFC  bf.s 8C102BCE
  8C102BD4  320C  add r0,r2
  8C102BD6  AF89  bra 8C102AEC
  8C102BD8  8521  mov.w @(2,r2),R0
  8C102BDA  D231  mov.l @([8C102CA0]),r2
  8C102BDC  F3FD  fschg
  8C102BDE  F79D  fldi1 fr7
  8C102BE0  F743  fdiv fr4,fr7
  8C102BE2  0483  pref @r4
  8C102BE4  74E0  add ##-32,r4
  8C102BE6  F829  fmov.s @r2+,fr8
  8C102BE8  6163  mov r6,r1
  8C102BEA  4121  shar r1
  8C102BEC  FA29  fmov.s @r2+,fr10
  8C102BEE  F9D2  fmul fr13,fr9
  8C102BF0  6042  mov.l @r4,r0
  8C102BF2  FAE2  fmul fr14,fr10
  8C102BF4  F2E9  fmov.s @r14+,fr2
  8C102BF6  FBF2  fmul fr15,fr11
  8C102BF8  DD2C  mov.l @([8C102CAC]),r13
  8C102BFA  4310  dt r3
  8C102BFC  F6AB  fmov.s fr10,@-r6
  8C102BFE  6E43  mov r4,r14
  8C102C00  F68B  fmov.s fr8,@-r6
  8C102C02  76F8  add ##-8,r6
  8C102C04  F672  fmul fr7,fr6
  8C102C06  F62B  fmov.s fr2,@-r6
  8C102C08  F572  fmul fr7,fr5
  8C102C0A  F66B  fmov.s fr6,@-r6
  8C102C0C  8D16  bt.s 8C102C3C
  8C102C0E  F64B  fmov.s fr4,@-r6
  8C102C10  C801  tst ##1,R0
  8C102C12  2662  mov.l r6,@r6
  8C102C14  8900  bt 8C102C18
  8C102C16  AF53  bra 8C102AC0
  8C102C18  0683  pref @r6
  ...
  8C102C3C  6046  mov.l @r4+,r0
  8C102C3E  2612  mov.l r1,@r6
  8C102C40  4015  cmp/pl r0
  8C102C42  0683  pref @r6
  8C102C44  7620  add ##32,r6
  8C102C46  8B18  bf 8C102C7A
  8C102C48  C880  tst ##128,R0
  8C102C4A  DD18  mov.l @([8C102CAC]),r13
  8C102C4C  8915  bt 8C102C7A
  8C102C4E  6346  mov.l @r4+,r3
  8C102C50  6163  mov r6,r1
  8C102C52  6042  mov.l @r4,r0
  8C102C54  6E43  mov r4,r14
  8C102C56  6DD2  mov.l @r13,r13
  8C102C58  C801  tst ##1,R0
  8C102C5A  D012  mov.l @([8C102CA4]),r0
  8C102C5C  7540  add ##64,r5
  8C102C5E  8900  bt 8C102C62
  8C102C60  AF34  bra 8C102ACC
  8C102C62  FC09  fmov.s @r0+,fr12
  ...
  8C102C7A  74FC  add ##-4,r4
  8C102C7C  F3FD  fschg
  8C102C7E  7540  add ##64,r5
  8C102C80  0683  pref @r6
  8C102C82  000B  rts
  8C102C84  7620  add ##32,r6
```

## Dead or Alive 2 (USA) `8C102DE0`

Dump: `/mnt/1TB/dcbat_off/20261007-192359_Dead_or_Alive_2__USA__/jit-14513.txt`

```
  8C102DE0  4811  cmp/pz r8
  8C102DE2  8901  bt 8C102DE8
  ...
  8C102DE8  6946  mov.l @r4+,r9
  8C102DEA  E303  mov ##0x03,r3
  8C102DEC  F08D  fldi0 fr0
  8C102DEE  7640  add ##64,r6
  8C102DF0  F18D  fldi0 fr1
  8C102DF2  F3FD  fschg
  8C102DF4  F60B  fmov.s fr0,@-r6
  8C102DF6  F60B  fmov.s fr0,@-r6
  8C102DF8  F3FD  fschg
  8C102DFA  6042  mov.l @r4,r0
  8C102DFC  6E43  mov r4,r14
  8C102DFE  C801  tst ##1,R0
  8C102E00  DD72  mov.l @([8C102FCC]),r13
  8C102E02  8F03  bf.s 8C102E0C
  8C102E04  7420  add ##32,r4
  8C102E06  5EE1  mov.l @(4,r14),r14
  8C102E08  74E8  add ##-24,r4
  8C102E0A  3E4C  add r4,r14
  8C102E0C  D06D  mov.l @([8C102FC4]),r0
  8C102E0E  F3FD  fschg
  8C102E10  6DD2  mov.l @r13,r13
  8C102E12  FC09  fmov.s @r0+,fr12
  8C102E14  FE09  fmov.s @r0+,fr14
  8C102E16  F4E9  fmov.s @r14+,fr4
  8C102E18  7420  add ##32,r4
  8C102E1A  F6E9  fmov.s @r14+,fr6
  8C102E1C  A00D  bra 8C102E3A
  8C102E1E  F3FD  fschg
  8C102E20  D068  mov.l @([8C102FC4]),r0
  8C102E22  7620  add ##32,r6
  8C102E24  6DD2  mov.l @r13,r13
  8C102E26  6163  mov r6,r1
  8C102E28  FC09  fmov.s @r0+,fr12
  8C102E2A  7540  add ##64,r5
  8C102E2C  FE09  fmov.s @r0+,fr14
  8C102E2E  7440  add ##64,r4
  8C102E30  F4E9  fmov.s @r14+,fr4
  8C102E32  7650  add ##80,r6
  8C102E34  F6E9  fmov.s @r14+,fr6
  8C102E36  F3FD  fschg
  8C102E38  0183  pref @r1
  8C102E3A  F87C  fmov fr7,fr8
  8C102E3C  2DD8  tst r13,r13
  8C102E3E  F9E9  fmov.s @r14+,fr9
  8C102E40  8D24  bt.s 8C102E8C
  8C102E42  FAE9  fmov.s @r14+,fr10
  8C102E44  4D01  shlr r13
  8C102E46  D260  mov.l @([8C102FC8]),r2
  8C102E48  8B18  bf 8C102E7C
  ...
  8C102E4C  F38D  fldi0 fr3
  8C102E4E  8802  cmp/eq ##0x02,R0
  8C102E50  E174  mov ##0x74,r1
  8C102E52  8F1F  bf.s 8C102E94
  8C102E54  312C  add r2,r1
  8C102E56  F79D  fldi1 fr7
  8C102E58  2DD8  tst r13,r13
  8C102E5A  710C  add ##12,r1
  8C102E5C  8F01  bf.s 8C102E62
  8C102E5E  F019  fmov.s @r1+,fr0
  8C102E60  F5FD  ftrv xmtrx,fv4
  8C102E62  F119  fmov.s @r1+,fr1
  8C102E64  6023  mov r2,r0
  8C102E66  F219  fmov.s @r1+,fr2
  8C102E68  7018  add ##24,r0
  8C102E6A  F8ED  fipr fv12,fv8
  8C102E6C  F109  fmov.s @r0+,fr1
  8C102E6E  F209  fmov.s @r0+,fr2
  8C102E70  F309  fmov.s @r0+,fr3
  8C102E72  F08D  fldi0 fr0
  8C102E74  F0B5  fcmp/gt fr11,fr0
  8C102E76  F0BC  fmov fr11,fr0
  8C102E78  8F4F  bf.s 8C102F1A
  8C102E7A  2DD8  tst r13,r13
  8C102E7C  E0B0  mov ##0xB0,r0
  8C102E7E  895C  bt 8C102F3A
  8C102E80  4D01  shlr r13
  8C102E82  600C  extu.b r0,r0
  8C102E84  8F53  bf.s 8C102F2E
  8C102E86  320C  add r0,r2
  8C102E88  AFE0  bra 8C102E4C
  8C102E8A  8521  mov.w @(2,r2),R0
  ...
  8C102E94  F019  fmov.s @r1+,fr0
  8C102E96  2DD8  tst r13,r13
  8C102E98  F119  fmov.s @r1+,fr1
  8C102E9A  F041  fsub fr4,fr0
  8C102E9C  F219  fmov.s @r1+,fr2
  8C102E9E  F151  fsub fr5,fr1
  8C102EA0  F79D  fldi1 fr7
  8C102EA2  7118  add ##24,r1
  8C102EA4  FB8D  fldi0 fr11
  8C102EA6  8F01  bf.s 8C102EAC
  8C102EA8  F261  fsub fr6,fr2
  ...
  8C102EAC  F8ED  fipr fv12,fv8
  8C102EAE  FC8D  fldi0 fr12
  8C102EB0  F0ED  fipr fv12,fv0
  8C102EB2  FCB5  fcmp/gt fr11,fr12
  8C102EB4  FC18  fmov.s @r1,fr12
  8C102EB6  8937  bt 8C102F28
  8C102EB8  FC35  fcmp/gt fr3,fr12
  8C102EBA  8B35  bf 8C102F28
  8C102EBC  8803  cmp/eq ##0x03,R0
  8C102EBE  F31D  flds fr3,FPUL
  8C102EC0  8D13  bt.s 8C102EEA
  8C102EC2  F37D  FSRRA fr3
  ...
  8C102EEA  71BC  add ##-68,r1
  8C102EEC  F218  fmov.s @r1,fr2
  8C102EEE  71F8  add ##-8,r1
  8C102EF0  F00D  fsts FPUL,fr0
  8C102EF2  FB32  fmul fr3,fr11
  8C102EF4  FC18  fmov.s @r1,fr12
  8C102EF6  F302  fmul fr0,fr3
  8C102EF8  7104  add ##4,r1
  8C102EFA  FC2E  fmac fr0,fr2,fr12
  8C102EFC  F218  fmov.s @r1,fr2
  8C102EFE  6023  mov r2,r0
  8C102F00  F03C  fmov fr3,fr0
  8C102F02  FC2E  fmac fr0,fr2,fr12
  8C102F04  F0BC  fmov fr11,fr0
  8C102F06  7018  add ##24,r0
  8C102F08  FBC3  fdiv fr12,fr11
  8C102F0A  F29D  fldi1 fr2
  8C102F0C  FC25  fcmp/gt fr2,fr12
  8C102F0E  F109  fmov.s @r0+,fr1
  8C102F10  F209  fmov.s @r0+,fr2
  8C102F12  8F01  bf.s 8C102F18
  8C102F14  F309  fmov.s @r0+,fr3
  8C102F16  F0BC  fmov fr11,fr0
  8C102F18  2DD8  tst r13,r13
  8C102F1A  FD1E  fmac fr0,fr1,fr13
  8C102F1C  E0B0  mov ##0xB0,r0
  8C102F1E  FE2E  fmac fr0,fr2,fr14
  8C102F20  8D0B  bt.s 8C102F3A
  8C102F22  FF3E  fmac fr0,fr3,fr15
  8C102F24  A004  bra 8C102F30
  8C102F26  4D01  shlr r13
  8C102F28  2DD8  tst r13,r13
  8C102F2A  E0B0  mov ##0xB0,r0
  8C102F2C  8D05  bt.s 8C102F3A
  8C102F2E  4D01  shlr r13
  8C102F30  600C  extu.b r0,r0
  8C102F32  8FFC  bf.s 8C102F2E
  8C102F34  320C  add r0,r2
  8C102F36  AF89  bra 8C102E4C
  8C102F38  8521  mov.w @(2,r2),R0
  8C102F3A  D221  mov.l @([8C102FC0]),r2
  8C102F3C  F3FD  fschg
  8C102F3E  F79D  fldi1 fr7
  8C102F40  F743  fdiv fr4,fr7
  8C102F42  0483  pref @r4
  8C102F44  74E0  add ##-32,r4
  8C102F46  F829  fmov.s @r2+,fr8
  8C102F48  6163  mov r6,r1
  8C102F4A  4121  shar r1
  8C102F4C  FA29  fmov.s @r2+,fr10
  8C102F4E  F9D2  fmul fr13,fr9
  8C102F50  6042  mov.l @r4,r0
  8C102F52  FAE2  fmul fr14,fr10
  8C102F54  F2E9  fmov.s @r14+,fr2
  8C102F56  FBF2  fmul fr15,fr11
  8C102F58  DD1C  mov.l @([8C102FCC]),r13
  8C102F5A  4310  dt r3
  8C102F5C  F6AB  fmov.s fr10,@-r6
  8C102F5E  6E43  mov r4,r14
  8C102F60  F68B  fmov.s fr8,@-r6
  8C102F62  76F8  add ##-8,r6
  8C102F64  F672  fmul fr7,fr6
  8C102F66  F62B  fmov.s fr2,@-r6
  8C102F68  F572  fmul fr7,fr5
  8C102F6A  F66B  fmov.s fr6,@-r6
  8C102F6C  8D16  bt.s 8C102F9C
  8C102F6E  F64B  fmov.s fr4,@-r6
  8C102F70  C801  tst ##1,R0
  8C102F72  2662  mov.l r6,@r6
  8C102F74  8900  bt 8C102F78
  8C102F76  AF53  bra 8C102E20
  8C102F78  0683  pref @r6
  ...
  8C102F9C  2612  mov.l r1,@r6
  8C102F9E  4910  dt r9
  8C102FA0  0683  pref @r6
  8C102FA2  7620  add ##32,r6
  8C102FA4  F3FD  fschg
  8C102FA6  7540  add ##64,r5
  8C102FA8  0683  pref @r6
  8C102FAA  76E0  add ##-32,r6
  8C102FAC  8D02  bt.s 8C102FB4
  8C102FAE  7670  add ##112,r6
  8C102FB0  AF23  bra 8C102DFA
  8C102FB2  E303  mov ##0x03,r3
  8C102FB4  000B  rts
  8C102FB6  76D0  add ##-48,r6
```

## Power Stone (USA) `0C0E7610`

Dump: `/mnt/1TB/dcbat/20261007-160119_Power_Stone__USA__/jit-75898.txt`

```
  0C0E7610  6346  mov.l @r4+,r3
  0C0E7612  F29D  fldi1 fr2
  0C0E7614  F3FD  fschg
  0C0E7616  F38D  fldi0 fr3
  0C0E7618  F62B  fmov.s fr2,@-r6
  0C0E761A  7620  add ##32,r6
  0C0E761C  F62A  fmov.s fr2,@r6
  0C0E761E  6042  mov.l @r4,r0
  0C0E7620  6E43  mov r4,r14
  0C0E7622  C801  tst ##1,R0
  0C0E7624  7420  add ##32,r4
  0C0E7626  8B02  bf 0C0E762E
  0C0E7628  5EE1  mov.l @(4,r14),r14
  0C0E762A  74E8  add ##-24,r4
  0C0E762C  3E4C  add r4,r14
  0C0E762E  F4E9  fmov.s @r14+,fr4
  0C0E7630  F6E8  fmov.s @r14,fr6
  0C0E7632  7E10  add ##16,r14
  0C0E7634  F79D  fldi1 fr7
  0C0E7636  F0E8  fmov.s @r14,fr0
  0C0E7638  F5FD  ftrv xmtrx,fv4
  0C0E763A  F79D  fldi1 fr7
  0C0E763C  F743  fdiv fr4,fr7
  0C0E763E  4310  dt r3
  0C0E7640  6046  mov.l @r4+,r0
  0C0E7642  8D51  bt.s 0C0E76E8
  0C0E7644  6E46  mov.l @r4+,r14
  0C0E7646  C801  tst ##1,R0
  0C0E7648  6263  mov r6,r2
  0C0E764A  8D03  bt.s 0C0E7654
  0C0E764C  3E4C  add r4,r14
  0C0E764E  6E43  mov r4,r14
  0C0E7650  7418  add ##24,r4
  0C0E7652  7EF8  add ##-8,r14
  0C0E7654  F8E9  fmov.s @r14+,fr8
  0C0E7656  7420  add ##32,r4
  0C0E7658  0483  pref @r4
  0C0E765A  FAE8  fmov.s @r14,fr10
  0C0E765C  7E10  add ##16,r14
  0C0E765E  FB9D  fldi1 fr11
  0C0E7660  F672  fmul fr7,fr6
  0C0E7662  F60B  fmov.s fr0,@-r6
  0C0E7664  F572  fmul fr7,fr5
  0C0E7666  F66B  fmov.s fr6,@-r6
  0C0E7668  F9FD  ftrv xmtrx,fv8
  0C0E766A  F64B  fmov.s fr4,@-r6
  0C0E766C  74E0  add ##-32,r4
  0C0E766E  2622  mov.l r2,@r6
  0C0E7670  F0E8  fmov.s @r14,fr0
  0C0E7672  4310  dt r3
  0C0E7674  0683  pref @r6
  0C0E7676  7638  add ##56,r6
  0C0E7678  6263  mov r6,r2
  0C0E767A  FB9D  fldi1 fr11
  0C0E767C  FB83  fdiv fr8,fr11
  0C0E767E  6046  mov.l @r4+,r0
  0C0E7680  8D1A  bt.s 0C0E76B8
  0C0E7682  6E46  mov.l @r4+,r14
  0C0E7684  C801  tst ##1,R0
  0C0E7686  8D03  bt.s 0C0E7690
  0C0E7688  3E4C  add r4,r14
  0C0E768A  6E43  mov r4,r14
  0C0E768C  7418  add ##24,r4
  0C0E768E  7EF8  add ##-8,r14
  0C0E7690  7420  add ##32,r4
  0C0E7692  F4E9  fmov.s @r14+,fr4
  0C0E7694  0483  pref @r4
  0C0E7696  F6E8  fmov.s @r14,fr6
  0C0E7698  7E10  add ##16,r14
  0C0E769A  F79D  fldi1 fr7
  0C0E769C  FAB2  fmul fr11,fr10
  0C0E769E  74E0  add ##-32,r4
  0C0E76A0  F9B2  fmul fr11,fr9
  0C0E76A2  F60B  fmov.s fr0,@-r6
  0C0E76A4  F6AB  fmov.s fr10,@-r6
  0C0E76A6  F5FD  ftrv xmtrx,fv4
  0C0E76A8  F68B  fmov.s fr8,@-r6
  0C0E76AA  7540  add ##64,r5
  0C0E76AC  2622  mov.l r2,@r6
  0C0E76AE  F0E8  fmov.s @r14,fr0
  0C0E76B0  0683  pref @r6
  0C0E76B2  AFC2  bra 0C0E763A
  0C0E76B4  7638  add ##56,r6
  ...
  0C0E76B8  4015  cmp/pl r0
  0C0E76BA  6246  mov.l @r4+,r2
  0C0E76BC  8B0C  bf 0C0E76D8
  0C0E76BE  C880  tst ##128,R0
  0C0E76C0  63E3  mov r14,r3
  0C0E76C2  8909  bt 0C0E76D8
  0C0E76C4  6023  mov r2,r0
  0C0E76C6  E2FF  mov ##0xFF,r2
  0C0E76C8  6E46  mov.l @r4+,r14
  0C0E76CA  C801  tst ##1,R0
  0C0E76CC  8DE0  bt.s 0C0E7690
  0C0E76CE  3E4C  add r4,r14
  0C0E76D0  6E43  mov r4,r14
  0C0E76D2  7418  add ##24,r4
  0C0E76D4  AFDC  bra 0C0E7690
  0C0E76D6  7EF8  add ##-8,r14
  0C0E76D8  742C  add ##44,r4
  0C0E76DA  0483  pref @r4
  0C0E76DC  74D0  add ##-48,r4
  0C0E76DE  F48C  fmov fr8,fr4
  0C0E76E0  7520  add ##32,r5
  0C0E76E2  A014  bra 0C0E770E
  0C0E76E4  F6AC  fmov fr10,fr6
  ...
  0C0E76E8  4015  cmp/pl r0
  0C0E76EA  6246  mov.l @r4+,r2
  0C0E76EC  8B0C  bf 0C0E7708
  0C0E76EE  C880  tst ##128,R0
  0C0E76F0  63E3  mov r14,r3
  0C0E76F2  8909  bt 0C0E7708
  0C0E76F4  6023  mov r2,r0
  0C0E76F6  E2FF  mov ##0xFF,r2
  0C0E76F8  6E46  mov.l @r4+,r14
  0C0E76FA  C801  tst ##1,R0
  0C0E76FC  8DAA  bt.s 0C0E7654
  0C0E76FE  3E4C  add r4,r14
  0C0E7700  6E43  mov r4,r14
  0C0E7702  7418  add ##24,r4
  0C0E7704  AFA6  bra 0C0E7654
  0C0E7706  7EF8  add ##-8,r14
  0C0E7708  742C  add ##44,r4
  0C0E770A  0483  pref @r4
  0C0E770C  74D0  add ##-48,r4
  0C0E770E  F672  fmul fr7,fr6
  0C0E7710  E2FF  mov ##0xFF,r2
  0C0E7712  F572  fmul fr7,fr5
  0C0E7714  F60B  fmov.s fr0,@-r6
  0C0E7716  74F8  add ##-8,r4
  0C0E7718  F66B  fmov.s fr6,@-r6
  0C0E771A  7520  add ##32,r5
  0C0E771C  F64B  fmov.s fr4,@-r6
  0C0E771E  2622  mov.l r2,@r6
  0C0E7720  F3FD  fschg
  0C0E7722  0683  pref @r6
  0C0E7724  000B  rts
  0C0E7726  7620  add ##32,r6
```

## Project Justice (USA) `0C150C00`

Dump: `/mnt/1TB/dcbat/20261007-160314_Project_Justice__USA__/jit-78773.txt`

```
  0C150C00  2888  tst r8,r8
  0C150C02  6083  mov r8,r0
  0C150C04  8B00  bf 0C150C08
  ...
  0C150C08  C801  tst ##1,R0
  0C150C0A  8B01  bf 0C150C10
  ...
  0C150C10  6346  mov.l @r4+,r3
  0C150C12  F29D  fldi1 fr2
  0C150C14  F3FD  fschg
  0C150C16  F38D  fldi0 fr3
  0C150C18  F62B  fmov.s fr2,@-r6
  0C150C1A  7620  add ##32,r6
  0C150C1C  F62A  fmov.s fr2,@r6
  0C150C1E  6042  mov.l @r4,r0
  0C150C20  6E43  mov r4,r14
  0C150C22  C801  tst ##1,R0
  0C150C24  7420  add ##32,r4
  0C150C26  8B02  bf 0C150C2E
  ...
  0C150C2E  F4E9  fmov.s @r14+,fr4
  0C150C30  F6E8  fmov.s @r14,fr6
  0C150C32  7E10  add ##16,r14
  0C150C34  F79D  fldi1 fr7
  0C150C36  F0E8  fmov.s @r14,fr0
  0C150C38  F5FD  ftrv xmtrx,fv4
  0C150C3A  F79D  fldi1 fr7
  0C150C3C  F743  fdiv fr4,fr7
  0C150C3E  4310  dt r3
  0C150C40  6046  mov.l @r4+,r0
  0C150C42  8D53  bt.s 0C150CEC
  0C150C44  6E46  mov.l @r4+,r14
  0C150C46  C801  tst ##1,R0
  0C150C48  6263  mov r6,r2
  0C150C4A  8D03  bt.s 0C150C54
  0C150C4C  3E4C  add r4,r14
  0C150C4E  6E43  mov r4,r14
  0C150C50  7418  add ##24,r4
  0C150C52  7EF8  add ##-8,r14
  0C150C54  F8E9  fmov.s @r14+,fr8
  0C150C56  7420  add ##32,r4
  0C150C58  0483  pref @r4
  0C150C5A  FAE8  fmov.s @r14,fr10
  0C150C5C  7E10  add ##16,r14
  0C150C5E  FB9D  fldi1 fr11
  0C150C60  F672  fmul fr7,fr6
  0C150C62  F60B  fmov.s fr0,@-r6
  0C150C64  F572  fmul fr7,fr5
  0C150C66  F66B  fmov.s fr6,@-r6
  0C150C68  F9FD  ftrv xmtrx,fv8
  0C150C6A  F64B  fmov.s fr4,@-r6
  0C150C6C  74E0  add ##-32,r4
  0C150C6E  2622  mov.l r2,@r6
  0C150C70  F0E8  fmov.s @r14,fr0
  0C150C72  4310  dt r3
  0C150C74  0683  pref @r6
  0C150C76  7638  add ##56,r6
  0C150C78  6263  mov r6,r2
  0C150C7A  FB9D  fldi1 fr11
  0C150C7C  FB83  fdiv fr8,fr11
  0C150C7E  6046  mov.l @r4+,r0
  0C150C80  8D1A  bt.s 0C150CB8
  0C150C82  6E46  mov.l @r4+,r14
  0C150C84  C801  tst ##1,R0
  0C150C86  8D03  bt.s 0C150C90
  0C150C88  3E4C  add r4,r14
  0C150C8A  6E43  mov r4,r14
  0C150C8C  7418  add ##24,r4
  0C150C8E  7EF8  add ##-8,r14
  0C150C90  7420  add ##32,r4
  0C150C92  F4E9  fmov.s @r14+,fr4
  0C150C94  0483  pref @r4
  0C150C96  F6E8  fmov.s @r14,fr6
  0C150C98  7E10  add ##16,r14
  0C150C9A  F79D  fldi1 fr7
  0C150C9C  FAB2  fmul fr11,fr10
  0C150C9E  74E0  add ##-32,r4
  0C150CA0  F9B2  fmul fr11,fr9
  0C150CA2  F60B  fmov.s fr0,@-r6
  0C150CA4  F6AB  fmov.s fr10,@-r6
  0C150CA6  F5FD  ftrv xmtrx,fv4
  0C150CA8  F68B  fmov.s fr8,@-r6
  0C150CAA  7540  add ##64,r5
  0C150CAC  2622  mov.l r2,@r6
  0C150CAE  F0E8  fmov.s @r14,fr0
  0C150CB0  0683  pref @r6
  0C150CB2  AFC2  bra 0C150C3A
  0C150CB4  7638  add ##56,r6
  ...
  0C150CB8  4015  cmp/pl r0
  0C150CBA  6246  mov.l @r4+,r2
  0C150CBC  8B0E  bf 0C150CDC
  0C150CBE  C880  tst ##128,R0
  0C150CC0  63E3  mov r14,r3
  0C150CC2  890B  bt 0C150CDC
  0C150CC4  6023  mov r2,r0
  0C150CC6  6263  mov r6,r2
  0C150CC8  4221  shar r2
  0C150CCA  6E46  mov.l @r4+,r14
  0C150CCC  C801  tst ##1,R0
  0C150CCE  8DDF  bt.s 0C150C90
  0C150CD0  3E4C  add r4,r14
  0C150CD2  6E43  mov r4,r14
  0C150CD4  7418  add ##24,r4
  0C150CD6  AFDB  bra 0C150C90
  0C150CD8  7EF8  add ##-8,r14
  ...
  0C150CDC  742C  add ##44,r4
  0C150CDE  0483  pref @r4
  0C150CE0  74D0  add ##-48,r4
  0C150CE2  F48C  fmov fr8,fr4
  0C150CE4  7520  add ##32,r5
  0C150CE6  A016  bra 0C150D16
  0C150CE8  F6AC  fmov fr10,fr6
  ...
  0C150CEC  4015  cmp/pl r0
  0C150CEE  6246  mov.l @r4+,r2
  0C150CF0  8B0E  bf 0C150D10
  0C150CF2  C880  tst ##128,R0
  0C150CF4  63E3  mov r14,r3
  0C150CF6  890B  bt 0C150D10
  0C150CF8  6023  mov r2,r0
  0C150CFA  6263  mov r6,r2
  0C150CFC  4221  shar r2
  0C150CFE  6E46  mov.l @r4+,r14
  0C150D00  C801  tst ##1,R0
  0C150D02  8DA7  bt.s 0C150C54
  0C150D04  3E4C  add r4,r14
  0C150D06  6E43  mov r4,r14
  0C150D08  7418  add ##24,r4
  0C150D0A  AFA3  bra 0C150C54
  0C150D0C  7EF8  add ##-8,r14
  ...
  0C150D16  F672  fmul fr7,fr6
  0C150D18  6263  mov r6,r2
  0C150D1A  4221  shar r2
  0C150D1C  F572  fmul fr7,fr5
  0C150D1E  F60B  fmov.s fr0,@-r6
  0C150D20  74F8  add ##-8,r4
  0C150D22  F66B  fmov.s fr6,@-r6
  0C150D24  7520  add ##32,r5
  0C150D26  F64B  fmov.s fr4,@-r6
  0C150D28  2622  mov.l r2,@r6
  0C150D2A  F3FD  fschg
  0C150D2C  0683  pref @r6
  0C150D2E  000B  rts
  0C150D30  7620  add ##32,r6
```

## Project Justice (USA) `0C1546BC`

Dump: `/mnt/1TB/dcbat/20261007-160314_Project_Justice__USA__/jit-78773.txt`

```
  0C1546BC  A151  bra 0C154962
  0C1546BE  0009  nop
  ...
  0C154962  6346  mov.l @r4+,r3
  0C154964  F29D  fldi1 fr2
  0C154966  F3FD  fschg
  0C154968  F39D  fldi1 fr3
  0C15496A  F62B  fmov.s fr2,@-r6
  0C15496C  7620  add ##32,r6
  0C15496E  F62A  fmov.s fr2,@r6
  0C154970  6042  mov.l @r4,r0
  0C154972  6E43  mov r4,r14
  0C154974  C801  tst ##1,R0
  0C154976  7420  add ##32,r4
  0C154978  8B02  bf 0C154980
  ...
  0C154980  F4E9  fmov.s @r14+,fr4
  0C154982  F6E8  fmov.s @r14,fr6
  0C154984  7E10  add ##16,r14
  0C154986  F79D  fldi1 fr7
  0C154988  F0E8  fmov.s @r14,fr0
  0C15498A  F5FD  ftrv xmtrx,fv4
  0C15498C  F79D  fldi1 fr7
  0C15498E  F743  fdiv fr4,fr7
  0C154990  4310  dt r3
  0C154992  6046  mov.l @r4+,r0
  0C154994  8D52  bt.s 0C154A3C
  0C154996  6E46  mov.l @r4+,r14
  0C154998  C801  tst ##1,R0
  0C15499A  6263  mov r6,r2
  0C15499C  8D03  bt.s 0C1549A6
  0C15499E  3E4C  add r4,r14
  0C1549A0  6E43  mov r4,r14
  0C1549A2  7418  add ##24,r4
  0C1549A4  7EF8  add ##-8,r14
  0C1549A6  F8E9  fmov.s @r14+,fr8
  0C1549A8  7420  add ##32,r4
  0C1549AA  0483  pref @r4
  0C1549AC  FAE8  fmov.s @r14,fr10
  0C1549AE  7E10  add ##16,r14
  0C1549B0  FB9D  fldi1 fr11
  0C1549B2  F672  fmul fr7,fr6
  0C1549B4  F60B  fmov.s fr0,@-r6
  0C1549B6  F572  fmul fr7,fr5
  0C1549B8  F66B  fmov.s fr6,@-r6
  0C1549BA  F9FD  ftrv xmtrx,fv8
  0C1549BC  F64B  fmov.s fr4,@-r6
  0C1549BE  74E0  add ##-32,r4
  0C1549C0  2622  mov.l r2,@r6
  0C1549C2  F0E8  fmov.s @r14,fr0
  0C1549C4  4310  dt r3
  0C1549C6  0683  pref @r6
  0C1549C8  7638  add ##56,r6
  0C1549CA  6263  mov r6,r2
  0C1549CC  FB9D  fldi1 fr11
  0C1549CE  FB83  fdiv fr8,fr11
  0C1549D0  6046  mov.l @r4+,r0
  0C1549D2  8D19  bt.s 0C154A08
  0C1549D4  6E46  mov.l @r4+,r14
  0C1549D6  C801  tst ##1,R0
  0C1549D8  8D03  bt.s 0C1549E2
  0C1549DA  3E4C  add r4,r14
  0C1549DC  6E43  mov r4,r14
  0C1549DE  7418  add ##24,r4
  0C1549E0  7EF8  add ##-8,r14
  0C1549E2  7420  add ##32,r4
  0C1549E4  F4E9  fmov.s @r14+,fr4
  0C1549E6  0483  pref @r4
  0C1549E8  F6E8  fmov.s @r14,fr6
  0C1549EA  7E10  add ##16,r14
  0C1549EC  F79D  fldi1 fr7
  0C1549EE  FAB2  fmul fr11,fr10
  0C1549F0  74E0  add ##-32,r4
  0C1549F2  F9B2  fmul fr11,fr9
  0C1549F4  F60B  fmov.s fr0,@-r6
  0C1549F6  F6AB  fmov.s fr10,@-r6
  0C1549F8  F5FD  ftrv xmtrx,fv4
  0C1549FA  F68B  fmov.s fr8,@-r6
  0C1549FC  7540  add ##64,r5
  0C1549FE  2622  mov.l r2,@r6
  0C154A00  F0E8  fmov.s @r14,fr0
  0C154A02  0683  pref @r6
  0C154A04  AFC2  bra 0C15498C
  0C154A06  7638  add ##56,r6
  0C154A08  4015  cmp/pl r0
  0C154A0A  6246  mov.l @r4+,r2
  0C154A0C  8B0E  bf 0C154A2C
  0C154A0E  C880  tst ##128,R0
  0C154A10  63E3  mov r14,r3
  0C154A12  890B  bt 0C154A2C
  0C154A14  6023  mov r2,r0
  0C154A16  6263  mov r6,r2
  0C154A18  4221  shar r2
  0C154A1A  6E46  mov.l @r4+,r14
  0C154A1C  C801  tst ##1,R0
  0C154A1E  8DE0  bt.s 0C1549E2
  0C154A20  3E4C  add r4,r14
  0C154A22  6E43  mov r4,r14
  0C154A24  7418  add ##24,r4
  0C154A26  AFDC  bra 0C1549E2
  0C154A28  7EF8  add ##-8,r14
  ...
  0C154A2C  742C  add ##44,r4
  0C154A2E  0483  pref @r4
  0C154A30  74D0  add ##-48,r4
  0C154A32  F48C  fmov fr8,fr4
  0C154A34  7520  add ##32,r5
  0C154A36  A016  bra 0C154A66
  0C154A38  F6AC  fmov fr10,fr6
  ...
  0C154A3C  4015  cmp/pl r0
  0C154A3E  6246  mov.l @r4+,r2
  0C154A40  8B0E  bf 0C154A60
  0C154A42  C880  tst ##128,R0
  0C154A44  63E3  mov r14,r3
  0C154A46  890B  bt 0C154A60
  0C154A48  6023  mov r2,r0
  0C154A4A  6263  mov r6,r2
  0C154A4C  4221  shar r2
  0C154A4E  6E46  mov.l @r4+,r14
  0C154A50  C801  tst ##1,R0
  0C154A52  8DA8  bt.s 0C1549A6
  0C154A54  3E4C  add r4,r14
  0C154A56  6E43  mov r4,r14
  0C154A58  7418  add ##24,r4
  0C154A5A  AFA4  bra 0C1549A6
  0C154A5C  7EF8  add ##-8,r14
  ...
  0C154A60  742C  add ##44,r4
  0C154A62  0483  pref @r4
  0C154A64  74D0  add ##-48,r4
  0C154A66  F672  fmul fr7,fr6
  0C154A68  6263  mov r6,r2
  0C154A6A  4221  shar r2
  0C154A6C  F572  fmul fr7,fr5
  0C154A6E  F60B  fmov.s fr0,@-r6
  0C154A70  74F8  add ##-8,r4
  0C154A72  F66B  fmov.s fr6,@-r6
  0C154A74  7520  add ##32,r5
  0C154A76  F64B  fmov.s fr4,@-r6
  0C154A78  2622  mov.l r2,@r6
  0C154A7A  F3FD  fschg
  0C154A7C  0683  pref @r6
  0C154A7E  000B  rts
  0C154A80  7620  add ##32,r6
```

## Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) `8C1D8A90`

Dump: `/mnt/1TB/dcbat_off/20261007-192124_Shenmue_II__Europe___En_Fr_De_Es___Disc_/jit-12387.txt`

```
  8C1D8A90  6346  mov.l @r4+,r3
  8C1D8A92  F29D  fldi1 fr2
  8C1D8A94  F3FD  fschg
  8C1D8A96  F38D  fldi0 fr3
  8C1D8A98  F62B  fmov.s fr2,@-r6
  8C1D8A9A  7620  add ##32,r6
  8C1D8A9C  F62A  fmov.s fr2,@r6
  8C1D8A9E  6042  mov.l @r4,r0
  8C1D8AA0  6E43  mov r4,r14
  8C1D8AA2  C801  tst ##1,R0
  8C1D8AA4  7420  add ##32,r4
  8C1D8AA6  8B02  bf 8C1D8AAE
  8C1D8AA8  5EE1  mov.l @(4,r14),r14
  8C1D8AAA  74E8  add ##-24,r4
  8C1D8AAC  3E4C  add r4,r14
  8C1D8AAE  F4E9  fmov.s @r14+,fr4
  8C1D8AB0  F6E8  fmov.s @r14,fr6
  8C1D8AB2  7E10  add ##16,r14
  8C1D8AB4  F79D  fldi1 fr7
  8C1D8AB6  F0E8  fmov.s @r14,fr0
  8C1D8AB8  F5FD  ftrv xmtrx,fv4
  8C1D8ABA  F79D  fldi1 fr7
  8C1D8ABC  F743  fdiv fr4,fr7
  8C1D8ABE  4310  dt r3
  8C1D8AC0  6046  mov.l @r4+,r0
  8C1D8AC2  8D53  bt.s 8C1D8B6C
  8C1D8AC4  6E46  mov.l @r4+,r14
  8C1D8AC6  C801  tst ##1,R0
  8C1D8AC8  6263  mov r6,r2
  8C1D8ACA  8D03  bt.s 8C1D8AD4
  8C1D8ACC  3E4C  add r4,r14
  8C1D8ACE  6E43  mov r4,r14
  8C1D8AD0  7418  add ##24,r4
  8C1D8AD2  7EF8  add ##-8,r14
  8C1D8AD4  F8E9  fmov.s @r14+,fr8
  8C1D8AD6  7420  add ##32,r4
  8C1D8AD8  0483  pref @r4
  8C1D8ADA  FAE8  fmov.s @r14,fr10
  8C1D8ADC  7E10  add ##16,r14
  8C1D8ADE  FB9D  fldi1 fr11
  8C1D8AE0  F672  fmul fr7,fr6
  8C1D8AE2  F60B  fmov.s fr0,@-r6
  8C1D8AE4  F572  fmul fr7,fr5
  8C1D8AE6  F66B  fmov.s fr6,@-r6
  8C1D8AE8  F9FD  ftrv xmtrx,fv8
  8C1D8AEA  F64B  fmov.s fr4,@-r6
  8C1D8AEC  74E0  add ##-32,r4
  8C1D8AEE  2622  mov.l r2,@r6
  8C1D8AF0  F0E8  fmov.s @r14,fr0
  8C1D8AF2  4310  dt r3
  8C1D8AF4  0683  pref @r6
  8C1D8AF6  7638  add ##56,r6
  8C1D8AF8  6263  mov r6,r2
  8C1D8AFA  FB9D  fldi1 fr11
  8C1D8AFC  FB83  fdiv fr8,fr11
  8C1D8AFE  6046  mov.l @r4+,r0
  8C1D8B00  8D1A  bt.s 8C1D8B38
  8C1D8B02  6E46  mov.l @r4+,r14
  8C1D8B04  C801  tst ##1,R0
  8C1D8B06  8D03  bt.s 8C1D8B10
  8C1D8B08  3E4C  add r4,r14
  8C1D8B0A  6E43  mov r4,r14
  8C1D8B0C  7418  add ##24,r4
  8C1D8B0E  7EF8  add ##-8,r14
  8C1D8B10  7420  add ##32,r4
  8C1D8B12  F4E9  fmov.s @r14+,fr4
  8C1D8B14  0483  pref @r4
  8C1D8B16  F6E8  fmov.s @r14,fr6
  8C1D8B18  7E10  add ##16,r14
  8C1D8B1A  F79D  fldi1 fr7
  8C1D8B1C  FAB2  fmul fr11,fr10
  8C1D8B1E  74E0  add ##-32,r4
  8C1D8B20  F9B2  fmul fr11,fr9
  8C1D8B22  F60B  fmov.s fr0,@-r6
  8C1D8B24  F6AB  fmov.s fr10,@-r6
  8C1D8B26  F5FD  ftrv xmtrx,fv4
  8C1D8B28  F68B  fmov.s fr8,@-r6
  8C1D8B2A  7540  add ##64,r5
  8C1D8B2C  2622  mov.l r2,@r6
  8C1D8B2E  F0E8  fmov.s @r14,fr0
  8C1D8B30  0683  pref @r6
  8C1D8B32  AFC2  bra 8C1D8ABA
  8C1D8B34  7638  add ##56,r6
  ...
  8C1D8B38  4015  cmp/pl r0
  8C1D8B3A  6246  mov.l @r4+,r2
  8C1D8B3C  8B0E  bf 8C1D8B5C
  8C1D8B3E  C880  tst ##128,R0
  8C1D8B40  63E3  mov r14,r3
  8C1D8B42  890B  bt 8C1D8B5C
  8C1D8B44  6023  mov r2,r0
  8C1D8B46  6263  mov r6,r2
  8C1D8B48  4221  shar r2
  8C1D8B4A  6E46  mov.l @r4+,r14
  8C1D8B4C  C801  tst ##1,R0
  8C1D8B4E  8DDF  bt.s 8C1D8B10
  8C1D8B50  3E4C  add r4,r14
  8C1D8B52  6E43  mov r4,r14
  8C1D8B54  7418  add ##24,r4
  8C1D8B56  AFDB  bra 8C1D8B10
  8C1D8B58  7EF8  add ##-8,r14
  ...
  8C1D8B5C  742C  add ##44,r4
  8C1D8B5E  0483  pref @r4
  8C1D8B60  74D0  add ##-48,r4
  8C1D8B62  F48C  fmov fr8,fr4
  8C1D8B64  7520  add ##32,r5
  8C1D8B66  A016  bra 8C1D8B96
  8C1D8B68  F6AC  fmov fr10,fr6
  ...
  8C1D8B6C  4015  cmp/pl r0
  8C1D8B6E  6246  mov.l @r4+,r2
  8C1D8B70  8B0E  bf 8C1D8B90
  8C1D8B72  C880  tst ##128,R0
  8C1D8B74  63E3  mov r14,r3
  8C1D8B76  890B  bt 8C1D8B90
  8C1D8B78  6023  mov r2,r0
  8C1D8B7A  6263  mov r6,r2
  8C1D8B7C  4221  shar r2
  8C1D8B7E  6E46  mov.l @r4+,r14
  8C1D8B80  C801  tst ##1,R0
  8C1D8B82  8DA7  bt.s 8C1D8AD4
  8C1D8B84  3E4C  add r4,r14
  8C1D8B86  6E43  mov r4,r14
  8C1D8B88  7418  add ##24,r4
  8C1D8B8A  AFA3  bra 8C1D8AD4
  8C1D8B8C  7EF8  add ##-8,r14
  ...
  8C1D8B96  F672  fmul fr7,fr6
  8C1D8B98  6263  mov r6,r2
  8C1D8B9A  4221  shar r2
  8C1D8B9C  F572  fmul fr7,fr5
  8C1D8B9E  F60B  fmov.s fr0,@-r6
  8C1D8BA0  74F8  add ##-8,r4
  8C1D8BA2  F66B  fmov.s fr6,@-r6
  8C1D8BA4  7520  add ##32,r5
  8C1D8BA6  F64B  fmov.s fr4,@-r6
  8C1D8BA8  2622  mov.l r2,@r6
  8C1D8BAA  F3FD  fschg
  8C1D8BAC  0683  pref @r6
  8C1D8BAE  000B  rts
  8C1D8BB0  7620  add ##32,r6
```

## Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) `8C1DC360`

Dump: `/mnt/1TB/dcbat_off/20261007-192124_Shenmue_II__Europe___En_Fr_De_Es___Disc_/jit-12387.txt`

```
  8C1DC360  4811  cmp/pz r8
  8C1DC362  8901  bt 8C1DC368
  8C1DC364  A104  bra 8C1DC570
  8C1DC366  0009  nop
  ...
  8C1DC570  6346  mov.l @r4+,r3
  8C1DC572  F29D  fldi1 fr2
  8C1DC574  F3FD  fschg
  8C1DC576  F39D  fldi1 fr3
  8C1DC578  F62B  fmov.s fr2,@-r6
  8C1DC57A  7620  add ##32,r6
  8C1DC57C  F62A  fmov.s fr2,@r6
  8C1DC57E  6042  mov.l @r4,r0
  8C1DC580  6E43  mov r4,r14
  8C1DC582  C801  tst ##1,R0
  8C1DC584  7420  add ##32,r4
  8C1DC586  8B02  bf 8C1DC58E
  ...
  8C1DC58E  F4E9  fmov.s @r14+,fr4
  8C1DC590  F6E8  fmov.s @r14,fr6
  8C1DC592  7E10  add ##16,r14
  8C1DC594  F79D  fldi1 fr7
  8C1DC596  F0E8  fmov.s @r14,fr0
  8C1DC598  F5FD  ftrv xmtrx,fv4
  8C1DC59A  F79D  fldi1 fr7
  8C1DC59C  F743  fdiv fr4,fr7
  8C1DC59E  4310  dt r3
  8C1DC5A0  6046  mov.l @r4+,r0
  8C1DC5A2  8D53  bt.s 8C1DC64C
  8C1DC5A4  6E46  mov.l @r4+,r14
  8C1DC5A6  C801  tst ##1,R0
  8C1DC5A8  6263  mov r6,r2
  8C1DC5AA  8D03  bt.s 8C1DC5B4
  8C1DC5AC  3E4C  add r4,r14
  8C1DC5AE  6E43  mov r4,r14
  8C1DC5B0  7418  add ##24,r4
  8C1DC5B2  7EF8  add ##-8,r14
  8C1DC5B4  F8E9  fmov.s @r14+,fr8
  8C1DC5B6  7420  add ##32,r4
  8C1DC5B8  0483  pref @r4
  8C1DC5BA  FAE8  fmov.s @r14,fr10
  8C1DC5BC  7E10  add ##16,r14
  8C1DC5BE  FB9D  fldi1 fr11
  8C1DC5C0  F672  fmul fr7,fr6
  8C1DC5C2  F60B  fmov.s fr0,@-r6
  8C1DC5C4  F572  fmul fr7,fr5
  8C1DC5C6  F66B  fmov.s fr6,@-r6
  8C1DC5C8  F9FD  ftrv xmtrx,fv8
  8C1DC5CA  F64B  fmov.s fr4,@-r6
  8C1DC5CC  74E0  add ##-32,r4
  8C1DC5CE  2622  mov.l r2,@r6
  8C1DC5D0  F0E8  fmov.s @r14,fr0
  8C1DC5D2  4310  dt r3
  8C1DC5D4  0683  pref @r6
  8C1DC5D6  7638  add ##56,r6
  8C1DC5D8  6263  mov r6,r2
  8C1DC5DA  FB9D  fldi1 fr11
  8C1DC5DC  FB83  fdiv fr8,fr11
  8C1DC5DE  6046  mov.l @r4+,r0
  8C1DC5E0  8D1A  bt.s 8C1DC618
  8C1DC5E2  6E46  mov.l @r4+,r14
  8C1DC5E4  C801  tst ##1,R0
  8C1DC5E6  8D03  bt.s 8C1DC5F0
  8C1DC5E8  3E4C  add r4,r14
  8C1DC5EA  6E43  mov r4,r14
  8C1DC5EC  7418  add ##24,r4
  8C1DC5EE  7EF8  add ##-8,r14
  8C1DC5F0  7420  add ##32,r4
  8C1DC5F2  F4E9  fmov.s @r14+,fr4
  8C1DC5F4  0483  pref @r4
  8C1DC5F6  F6E8  fmov.s @r14,fr6
  8C1DC5F8  7E10  add ##16,r14
  8C1DC5FA  F79D  fldi1 fr7
  8C1DC5FC  FAB2  fmul fr11,fr10
  8C1DC5FE  74E0  add ##-32,r4
  8C1DC600  F9B2  fmul fr11,fr9
  8C1DC602  F60B  fmov.s fr0,@-r6
  8C1DC604  F6AB  fmov.s fr10,@-r6
  8C1DC606  F5FD  ftrv xmtrx,fv4
  8C1DC608  F68B  fmov.s fr8,@-r6
  8C1DC60A  7540  add ##64,r5
  8C1DC60C  2622  mov.l r2,@r6
  8C1DC60E  F0E8  fmov.s @r14,fr0
  8C1DC610  0683  pref @r6
  8C1DC612  AFC2  bra 8C1DC59A
  8C1DC614  7638  add ##56,r6
  ...
  8C1DC618  4015  cmp/pl r0
  8C1DC61A  6246  mov.l @r4+,r2
  8C1DC61C  8B0E  bf 8C1DC63C
  8C1DC61E  C880  tst ##128,R0
  8C1DC620  63E3  mov r14,r3
  8C1DC622  890B  bt 8C1DC63C
  8C1DC624  6023  mov r2,r0
  8C1DC626  6263  mov r6,r2
  8C1DC628  4221  shar r2
  8C1DC62A  6E46  mov.l @r4+,r14
  8C1DC62C  C801  tst ##1,R0
  8C1DC62E  8DDF  bt.s 8C1DC5F0
  8C1DC630  3E4C  add r4,r14
  8C1DC632  6E43  mov r4,r14
  8C1DC634  7418  add ##24,r4
  8C1DC636  AFDB  bra 8C1DC5F0
  8C1DC638  7EF8  add ##-8,r14
  ...
  8C1DC63C  742C  add ##44,r4
  8C1DC63E  0483  pref @r4
  8C1DC640  74D0  add ##-48,r4
  8C1DC642  F48C  fmov fr8,fr4
  8C1DC644  7520  add ##32,r5
  8C1DC646  A016  bra 8C1DC676
  8C1DC648  F6AC  fmov fr10,fr6
  ...
  8C1DC64C  4015  cmp/pl r0
  8C1DC64E  6246  mov.l @r4+,r2
  8C1DC650  8B0E  bf 8C1DC670
  8C1DC652  C880  tst ##128,R0
  8C1DC654  63E3  mov r14,r3
  8C1DC656  890B  bt 8C1DC670
  8C1DC658  6023  mov r2,r0
  8C1DC65A  6263  mov r6,r2
  8C1DC65C  4221  shar r2
  8C1DC65E  6E46  mov.l @r4+,r14
  8C1DC660  C801  tst ##1,R0
  8C1DC662  8DA7  bt.s 8C1DC5B4
  8C1DC664  3E4C  add r4,r14
  8C1DC666  6E43  mov r4,r14
  8C1DC668  7418  add ##24,r4
  8C1DC66A  AFA3  bra 8C1DC5B4
  8C1DC66C  7EF8  add ##-8,r14
  ...
  8C1DC670  742C  add ##44,r4
  8C1DC672  0483  pref @r4
  8C1DC674  74D0  add ##-48,r4
  8C1DC676  F672  fmul fr7,fr6
  8C1DC678  6263  mov r6,r2
  8C1DC67A  4221  shar r2
  8C1DC67C  F572  fmul fr7,fr5
  8C1DC67E  F60B  fmov.s fr0,@-r6
  8C1DC680  74F8  add ##-8,r4
  8C1DC682  F66B  fmov.s fr6,@-r6
  8C1DC684  7520  add ##32,r5
  8C1DC686  F64B  fmov.s fr4,@-r6
  8C1DC688  2622  mov.l r2,@r6
  8C1DC68A  F3FD  fschg
  8C1DC68C  0683  pref @r6
  8C1DC68E  000B  rts
  8C1DC690  7620  add ##32,r6
```
