# 001_nativo_doa2

> Gerado por `tools/sdk_find.py`. Jogos: 5 · variantes (sequências normalizadas distintas): 7 · tempo perf somado: 18.78% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Dead or Alive 2 (USA) | `8C10192C` | 103 | 17.41% |
| Dead or Alive 2 (USA) | `8C10232C` | 101 | 1.11% |
| Dead or Alive 2 (USA) | `8C102330` | 127 | 0.08% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C12A7EC` | 103 | 0.01% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C12B1E0` | 232 | 0.00% |
| Power Stone (USA) | `0C0E8010` | 128 | 0.00% |
| Project Justice (USA) | `0C154A9C` | 126 | 0.00% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C1D8A8C` | 85 | 0.17% |

## Dead or Alive 2 (USA) `8C10192C`

Dump: `/mnt/1TB/dcbat_off/20261007-192359_Dead_or_Alive_2__USA__/jit-14513.txt`

```
  8C10192C  A13B  bra 8C101BA6
  8C10192E  0009  nop
  ...
  8C101BA6  E27F  mov ##0x7F,r2
  8C101BA8  6346  mov.l @r4+,r3
  8C101BAA  76E0  add ##-32,r6
  8C101BAC  6042  mov.l @r4,r0
  8C101BAE  727F  add ##127,r2
  8C101BB0  F3FD  fschg
  8C101BB2  7237  add ##55,r2
  8C101BB4  C801  tst ##1,R0
  8C101BB6  6E43  mov r4,r14
  8C101BB8  8F03  bf.s 8C101BC2
  8C101BBA  7420  add ##32,r4
  8C101BBC  5EE1  mov.l @(4,r14),r14
  8C101BBE  74E8  add ##-24,r4
  8C101BC0  3E4C  add r4,r14
  8C101BC2  F4E9  fmov.s @r14+,fr4
  8C101BC4  F6E9  fmov.s @r14+,fr6
  8C101BC6  6763  mov r6,r7
  8C101BC8  F28D  fldi0 fr2
  8C101BCA  F270  fadd fr7,fr2
  8C101BCC  F79D  fldi1 fr7
  8C101BCE  7640  add ##64,r6
  8C101BD0  F38D  fldi0 fr3
  8C101BD2  7520  add ##32,r5
  8C101BD4  F0E9  fmov.s @r14+,fr0
  8C101BD6  F5FD  ftrv xmtrx,fv4
  8C101BD8  FB8D  fldi0 fr11
  8C101BDA  4310  dt r3
  8C101BDC  6046  mov.l @r4+,r0
  8C101BDE  8D3D  bt.s 8C101C5C
  8C101BE0  61E3  mov r14,r1
  8C101BE2  F3ED  fipr fv12,fv0
  8C101BE4  C801  tst ##1,R0
  8C101BE6  6E46  mov.l @r4+,r14
  8C101BE8  8F10  bf.s 8C101C0C
  8C101BEA  F79D  fldi1 fr7
  8C101BEC  F743  fdiv fr4,fr7
  8C101BEE  0483  pref @r4
  8C101BF0  F3B5  fcmp/gt fr11,fr3
  8C101BF2  3E4C  add r4,r14
  8C101BF4  FF1D  flds fr15,FPUL
  8C101BF6  F8ED  fipr fv12,fv8
  8C101BF8  0E83  pref @r14
  8C101BFA  8F1D  bf.s 8C101C38
  8C101BFC  F20D  fsts FPUL,fr2
  8C101BFE  F230  fadd fr3,fr2
  8C101C00  F38D  fldi0 fr3
  8C101C02  A010  bra 8C101C26
  8C101C04  FB3D  ftrc fr11, FPUL
  ...
  8C101C08  74E4  add ##-28,r4
  8C101C0A  F79D  fldi1 fr7
  8C101C0C  F743  fdiv fr4,fr7
  8C101C0E  7418  add ##24,r4
  8C101C10  F3B5  fcmp/gt fr11,fr3
  8C101C12  FF1D  flds fr15,FPUL
  8C101C14  F8ED  fipr fv12,fv8
  8C101C16  6E43  mov r4,r14
  8C101C18  7EE0  add ##-32,r14
  8C101C1A  0483  pref @r4
  8C101C1C  8F0C  bf.s 8C101C38
  8C101C1E  F20D  fsts FPUL,fr2
  8C101C20  F230  fadd fr3,fr2
  8C101C22  FB3D  ftrc fr11, FPUL
  8C101C24  F38D  fldi0 fr3
  8C101C26  005A  sts FPUL,r0
  8C101C28  3027  cmp/gt r2,r0
  8C101C2A  4008  shll2 r0
  8C101C2C  8B05  bf 8C101C3A
  8C101C2E  F3FD  fschg
  8C101C30  F386  fmov.s @(R0,r8),fr3
  8C101C32  A002  bra 8C101C3A
  8C101C34  F3FD  fschg
  ...
  8C101C38  F38D  fldi0 fr3
  8C101C3A  F018  fmov.s @r1,fr0
  8C101C3C  F672  fmul fr7,fr6
  8C101C3E  F62B  fmov.s fr2,@-r6
  8C101C40  F572  fmul fr7,fr5
  8C101C42  F60B  fmov.s fr0,@-r6
  8C101C44  2338  tst r3,r3
  8C101C46  F66B  fmov.s fr6,@-r6
  8C101C48  F64B  fmov.s fr4,@-r6
  8C101C4A  8D02  bt.s 8C101C52
  8C101C4C  2672  mov.l r7,@r6
  8C101C4E  F4E9  fmov.s @r14+,fr4
  8C101C50  AFB8  bra 8C101BC4
  8C101C52  0683  pref @r6
  ...
  8C101C5C  6763  mov r6,r7
  8C101C5E  4721  shar r7
  8C101C60  4015  cmp/pl r0
  8C101C62  8FD1  bf.s 8C101C08
  8C101C64  F3ED  fipr fv12,fv0
  8C101C66  C880  tst ##128,R0
  8C101C68  F79D  fldi1 fr7
  8C101C6A  89CD  bt 8C101C08
  8C101C6C  6346  mov.l @r4+,r3
  8C101C6E  6046  mov.l @r4+,r0
  8C101C70  6E46  mov.l @r4+,r14
  8C101C72  C801  tst ##1,R0
  8C101C74  F743  fdiv fr4,fr7
  8C101C76  8BCA  bf 8C101C0E
  8C101C78  AFBA  bra 8C101BF0
  8C101C7A  0483  pref @r4
```

## Dead or Alive 2 (USA) `8C10232C`

Dump: `/mnt/1TB/dcbat_off/20261007-192359_Dead_or_Alive_2__USA__/jit-14513.txt`

```
  8C10232C  A125  bra 8C10257A
  8C10232E  0009  nop
  ...
  8C10257A  E27F  mov ##0x7F,r2
  8C10257C  E303  mov ##0x03,r3
  8C10257E  6946  mov.l @r4+,r9
  8C102580  76E0  add ##-32,r6
  8C102582  6042  mov.l @r4,r0
  8C102584  727F  add ##127,r2
  8C102586  F3FD  fschg
  8C102588  7237  add ##55,r2
  8C10258A  C801  tst ##1,R0
  8C10258C  6E43  mov r4,r14
  8C10258E  8F03  bf.s 8C102598
  8C102590  7420  add ##32,r4
  8C102592  5EE1  mov.l @(4,r14),r14
  8C102594  74E8  add ##-24,r4
  8C102596  3E4C  add r4,r14
  8C102598  F4E9  fmov.s @r14+,fr4
  8C10259A  F6E9  fmov.s @r14+,fr6
  8C10259C  6763  mov r6,r7
  8C10259E  F28D  fldi0 fr2
  8C1025A0  F270  fadd fr7,fr2
  8C1025A2  F79D  fldi1 fr7
  8C1025A4  7640  add ##64,r6
  8C1025A6  F38D  fldi0 fr3
  8C1025A8  7520  add ##32,r5
  8C1025AA  F0E9  fmov.s @r14+,fr0
  8C1025AC  F5FD  ftrv xmtrx,fv4
  8C1025AE  FB8D  fldi0 fr11
  8C1025B0  4310  dt r3
  8C1025B2  6046  mov.l @r4+,r0
  8C1025B4  8D3C  bt.s 8C102630
  8C1025B6  61E3  mov r14,r1
  8C1025B8  F3ED  fipr fv12,fv0
  8C1025BA  C801  tst ##1,R0
  8C1025BC  6E46  mov.l @r4+,r14
  8C1025BE  8F0F  bf.s 8C1025E0
  8C1025C0  F79D  fldi1 fr7
  8C1025C2  F743  fdiv fr4,fr7
  8C1025C4  0483  pref @r4
  8C1025C6  F3B5  fcmp/gt fr11,fr3
  8C1025C8  3E4C  add r4,r14
  8C1025CA  FF1D  flds fr15,FPUL
  8C1025CC  F8ED  fipr fv12,fv8
  8C1025CE  0E83  pref @r14
  8C1025D0  8F1C  bf.s 8C10260C
  8C1025D2  F20D  fsts FPUL,fr2
  8C1025D4  F230  fadd fr3,fr2
  8C1025D6  F38D  fldi0 fr3
  8C1025D8  A00F  bra 8C1025FA
  8C1025DA  FB3D  ftrc fr11, FPUL
  8C1025DC  74E4  add ##-28,r4
  8C1025DE  F79D  fldi1 fr7
  8C1025E0  F743  fdiv fr4,fr7
  8C1025E2  7418  add ##24,r4
  8C1025E4  F3B5  fcmp/gt fr11,fr3
  8C1025E6  FF1D  flds fr15,FPUL
  8C1025E8  F8ED  fipr fv12,fv8
  8C1025EA  6E43  mov r4,r14
  8C1025EC  7EE0  add ##-32,r14
  8C1025EE  0483  pref @r4
  8C1025F0  8F0C  bf.s 8C10260C
  8C1025F2  F20D  fsts FPUL,fr2
  8C1025F4  F230  fadd fr3,fr2
  8C1025F6  FB3D  ftrc fr11, FPUL
  8C1025F8  F38D  fldi0 fr3
  8C1025FA  005A  sts FPUL,r0
  8C1025FC  3027  cmp/gt r2,r0
  8C1025FE  4008  shll2 r0
  8C102600  8B05  bf 8C10260E
  8C102602  F3FD  fschg
  8C102604  F386  fmov.s @(R0,r8),fr3
  8C102606  A002  bra 8C10260E
  8C102608  F3FD  fschg
  ...
  8C10260C  F38D  fldi0 fr3
  8C10260E  F018  fmov.s @r1,fr0
  8C102610  F672  fmul fr7,fr6
  8C102612  F62B  fmov.s fr2,@-r6
  8C102614  F572  fmul fr7,fr5
  8C102616  F60B  fmov.s fr0,@-r6
  8C102618  2338  tst r3,r3
  8C10261A  F66B  fmov.s fr6,@-r6
  8C10261C  F64B  fmov.s fr4,@-r6
  8C10261E  8D02  bt.s 8C102626
  8C102620  2672  mov.l r7,@r6
  8C102622  F4E9  fmov.s @r14+,fr4
  8C102624  AFB9  bra 8C10259A
  8C102626  0683  pref @r6
  ...
  8C102630  6763  mov r6,r7
  8C102632  4721  shar r7
  8C102634  4910  dt r9
  8C102636  8DD1  bt.s 8C1025DC
  8C102638  F3ED  fipr fv12,fv0
  8C10263A  6E46  mov.l @r4+,r14
  8C10263C  C801  tst ##1,R0
  8C10263E  D303  mov.l @([8C10264C]),r3
  8C102640  F79D  fldi1 fr7
  8C102642  8FCE  bf.s 8C1025E2
  8C102644  F743  fdiv fr4,fr7
  8C102646  AFBE  bra 8C1025C6
  8C102648  0483  pref @r4
```

## Dead or Alive 2 (USA) `8C102330`

Dump: `/mnt/1TB/dcbat_off/20261007-192359_Dead_or_Alive_2__USA__/jit-14513.txt`

```
  8C102330  6946  mov.l @r4+,r9
  8C102332  F29D  fldi1 fr2
  8C102334  F3FD  fschg
  8C102336  F38D  fldi0 fr3
  8C102338  E303  mov ##0x03,r3
  8C10233A  F62B  fmov.s fr2,@-r6
  8C10233C  7620  add ##32,r6
  8C10233E  F62A  fmov.s fr2,@r6
  8C102340  6042  mov.l @r4,r0
  8C102342  6E43  mov r4,r14
  8C102344  C801  tst ##1,R0
  8C102346  7420  add ##32,r4
  8C102348  8B02  bf 8C102350
  ...
  8C102350  F4E9  fmov.s @r14+,fr4
  8C102352  F6E8  fmov.s @r14,fr6
  8C102354  7E10  add ##16,r14
  8C102356  F79D  fldi1 fr7
  8C102358  F0E8  fmov.s @r14,fr0
  8C10235A  F5FD  ftrv xmtrx,fv4
  8C10235C  F79D  fldi1 fr7
  8C10235E  F743  fdiv fr4,fr7
  8C102360  4310  dt r3
  8C102362  6046  mov.l @r4+,r0
  8C102364  8D4C  bt.s 8C102400
  8C102366  6E46  mov.l @r4+,r14
  8C102368  C801  tst ##1,R0
  8C10236A  6263  mov r6,r2
  8C10236C  8D03  bt.s 8C102376
  8C10236E  3E4C  add r4,r14
  8C102370  6E43  mov r4,r14
  8C102372  7418  add ##24,r4
  8C102374  7EF8  add ##-8,r14
  8C102376  F8E9  fmov.s @r14+,fr8
  8C102378  7420  add ##32,r4
  8C10237A  0483  pref @r4
  8C10237C  FAE8  fmov.s @r14,fr10
  8C10237E  7E10  add ##16,r14
  8C102380  FB9D  fldi1 fr11
  8C102382  F672  fmul fr7,fr6
  8C102384  F60B  fmov.s fr0,@-r6
  8C102386  F572  fmul fr7,fr5
  8C102388  F66B  fmov.s fr6,@-r6
  8C10238A  F9FD  ftrv xmtrx,fv8
  8C10238C  F64B  fmov.s fr4,@-r6
  8C10238E  74E0  add ##-32,r4
  8C102390  2622  mov.l r2,@r6
  8C102392  F0E8  fmov.s @r14,fr0
  8C102394  4310  dt r3
  8C102396  0683  pref @r6
  8C102398  7638  add ##56,r6
  8C10239A  6263  mov r6,r2
  8C10239C  FB9D  fldi1 fr11
  8C10239E  FB83  fdiv fr8,fr11
  8C1023A0  6046  mov.l @r4+,r0
  8C1023A2  8D19  bt.s 8C1023D8
  8C1023A4  6E46  mov.l @r4+,r14
  8C1023A6  C801  tst ##1,R0
  8C1023A8  8D03  bt.s 8C1023B2
  8C1023AA  3E4C  add r4,r14
  8C1023AC  6E43  mov r4,r14
  8C1023AE  7418  add ##24,r4
  8C1023B0  7EF8  add ##-8,r14
  8C1023B2  7420  add ##32,r4
  8C1023B4  F4E9  fmov.s @r14+,fr4
  8C1023B6  0483  pref @r4
  8C1023B8  F6E8  fmov.s @r14,fr6
  8C1023BA  7E10  add ##16,r14
  8C1023BC  F79D  fldi1 fr7
  8C1023BE  FAB2  fmul fr11,fr10
  8C1023C0  74E0  add ##-32,r4
  8C1023C2  F9B2  fmul fr11,fr9
  8C1023C4  F60B  fmov.s fr0,@-r6
  8C1023C6  F6AB  fmov.s fr10,@-r6
  8C1023C8  F5FD  ftrv xmtrx,fv4
  8C1023CA  F68B  fmov.s fr8,@-r6
  8C1023CC  7540  add ##64,r5
  8C1023CE  2622  mov.l r2,@r6
  8C1023D0  F0E8  fmov.s @r14,fr0
  8C1023D2  0683  pref @r6
  8C1023D4  AFC2  bra 8C10235C
  8C1023D6  7638  add ##56,r6
  8C1023D8  4910  dt r9
  8C1023DA  6263  mov r6,r2
  8C1023DC  8D08  bt.s 8C1023F0
  8C1023DE  4221  shar r2
  8C1023E0  C801  tst ##1,R0
  8C1023E2  E303  mov ##0x03,r3
  8C1023E4  8DE5  bt.s 8C1023B2
  8C1023E6  3E4C  add r4,r14
  8C1023E8  6E43  mov r4,r14
  8C1023EA  7418  add ##24,r4
  8C1023EC  AFE1  bra 8C1023B2
  8C1023EE  7EF8  add ##-8,r14
  8C1023F0  7430  add ##48,r4
  8C1023F2  0483  pref @r4
  8C1023F4  74D0  add ##-48,r4
  8C1023F6  F48C  fmov fr8,fr4
  8C1023F8  7520  add ##32,r5
  8C1023FA  A010  bra 8C10241E
  8C1023FC  F6AC  fmov fr10,fr6
  ...
  8C102400  4910  dt r9
  8C102402  6263  mov r6,r2
  8C102404  8D08  bt.s 8C102418
  8C102406  4221  shar r2
  8C102408  C801  tst ##1,R0
  8C10240A  E303  mov ##0x03,r3
  8C10240C  8DB3  bt.s 8C102376
  8C10240E  3E4C  add r4,r14
  8C102410  6E43  mov r4,r14
  8C102412  7418  add ##24,r4
  8C102414  AFAF  bra 8C102376
  8C102416  7EF8  add ##-8,r14
  8C102418  7430  add ##48,r4
  8C10241A  0483  pref @r4
  8C10241C  74D0  add ##-48,r4
  8C10241E  F672  fmul fr7,fr6
  8C102420  F60B  fmov.s fr0,@-r6
  8C102422  F572  fmul fr7,fr5
  8C102424  F66B  fmov.s fr6,@-r6
  8C102426  7520  add ##32,r5
  8C102428  F64B  fmov.s fr4,@-r6
  8C10242A  74F8  add ##-8,r4
  8C10242C  2622  mov.l r2,@r6
  8C10242E  F3FD  fschg
  8C102430  0683  pref @r6
  8C102432  000B  rts
  8C102434  7620  add ##32,r6
```

## Marvel vs. Capcom 2 - New Age of Heroes (Europe) `8C12B1E0`

Dump: `/mnt/1TB/dcbat_off/20261007-191632_Marvel_vs__Capcom_2_-_New_Age_of_Heroes_/jit-9882.txt`

```
  8C12B1E0  2888  tst r8,r8
  8C12B1E2  6083  mov r8,r0
  8C12B1E4  8B00  bf 8C12B1E8
  ...
  8C12B1E8  C801  tst ##1,R0
  8C12B1EA  8B01  bf 8C12B1F0
  8C12B1EC  A125  bra 8C12B43A
  8C12B1EE  0009  nop
  8C12B1F0  6946  mov.l @r4+,r9
  8C12B1F2  F29D  fldi1 fr2
  8C12B1F4  F3FD  fschg
  8C12B1F6  F38D  fldi0 fr3
  8C12B1F8  E303  mov ##0x03,r3
  8C12B1FA  F62B  fmov.s fr2,@-r6
  8C12B1FC  7620  add ##32,r6
  8C12B1FE  F62A  fmov.s fr2,@r6
  8C12B200  6042  mov.l @r4,r0
  8C12B202  6E43  mov r4,r14
  8C12B204  C801  tst ##1,R0
  8C12B206  7420  add ##32,r4
  8C12B208  8B02  bf 8C12B210
  8C12B20A  5EE1  mov.l @(4,r14),r14
  8C12B20C  74E8  add ##-24,r4
  8C12B20E  3E4C  add r4,r14
  8C12B210  F4E9  fmov.s @r14+,fr4
  8C12B212  F6E8  fmov.s @r14,fr6
  8C12B214  7E10  add ##16,r14
  8C12B216  F79D  fldi1 fr7
  8C12B218  F0E8  fmov.s @r14,fr0
  8C12B21A  F5FD  ftrv xmtrx,fv4
  8C12B21C  F79D  fldi1 fr7
  8C12B21E  F743  fdiv fr4,fr7
  8C12B220  4310  dt r3
  8C12B222  6046  mov.l @r4+,r0
  8C12B224  8D4C  bt.s 8C12B2C0
  8C12B226  6E46  mov.l @r4+,r14
  8C12B228  C801  tst ##1,R0
  8C12B22A  6263  mov r6,r2
  8C12B22C  8D03  bt.s 8C12B236
  8C12B22E  3E4C  add r4,r14
  8C12B230  6E43  mov r4,r14
  8C12B232  7418  add ##24,r4
  8C12B234  7EF8  add ##-8,r14
  8C12B236  F8E9  fmov.s @r14+,fr8
  8C12B238  7420  add ##32,r4
  8C12B23A  0483  pref @r4
  8C12B23C  FAE8  fmov.s @r14,fr10
  8C12B23E  7E10  add ##16,r14
  8C12B240  FB9D  fldi1 fr11
  8C12B242  F672  fmul fr7,fr6
  8C12B244  F60B  fmov.s fr0,@-r6
  8C12B246  F572  fmul fr7,fr5
  8C12B248  F66B  fmov.s fr6,@-r6
  8C12B24A  F9FD  ftrv xmtrx,fv8
  8C12B24C  F64B  fmov.s fr4,@-r6
  8C12B24E  74E0  add ##-32,r4
  8C12B250  2622  mov.l r2,@r6
  8C12B252  F0E8  fmov.s @r14,fr0
  8C12B254  4310  dt r3
  8C12B256  0683  pref @r6
  8C12B258  7638  add ##56,r6
  8C12B25A  6263  mov r6,r2
  8C12B25C  FB9D  fldi1 fr11
  8C12B25E  FB83  fdiv fr8,fr11
  8C12B260  6046  mov.l @r4+,r0
  8C12B262  8D19  bt.s 8C12B298
  8C12B264  6E46  mov.l @r4+,r14
  8C12B266  C801  tst ##1,R0
  8C12B268  8D03  bt.s 8C12B272
  8C12B26A  3E4C  add r4,r14
  8C12B26C  6E43  mov r4,r14
  8C12B26E  7418  add ##24,r4
  8C12B270  7EF8  add ##-8,r14
  8C12B272  7420  add ##32,r4
  8C12B274  F4E9  fmov.s @r14+,fr4
  8C12B276  0483  pref @r4
  8C12B278  F6E8  fmov.s @r14,fr6
  8C12B27A  7E10  add ##16,r14
  8C12B27C  F79D  fldi1 fr7
  8C12B27E  FAB2  fmul fr11,fr10
  8C12B280  74E0  add ##-32,r4
  8C12B282  F9B2  fmul fr11,fr9
  8C12B284  F60B  fmov.s fr0,@-r6
  8C12B286  F6AB  fmov.s fr10,@-r6
  8C12B288  F5FD  ftrv xmtrx,fv4
  8C12B28A  F68B  fmov.s fr8,@-r6
  8C12B28C  7540  add ##64,r5
  8C12B28E  2622  mov.l r2,@r6
  8C12B290  F0E8  fmov.s @r14,fr0
  8C12B292  0683  pref @r6
  8C12B294  AFC2  bra 8C12B21C
  8C12B296  7638  add ##56,r6
  8C12B298  4910  dt r9
  8C12B29A  6263  mov r6,r2
  8C12B29C  8D08  bt.s 8C12B2B0
  8C12B29E  4221  shar r2
  8C12B2A0  C801  tst ##1,R0
  8C12B2A2  E303  mov ##0x03,r3
  8C12B2A4  8DE5  bt.s 8C12B272
  8C12B2A6  3E4C  add r4,r14
  8C12B2A8  6E43  mov r4,r14
  8C12B2AA  7418  add ##24,r4
  8C12B2AC  AFE1  bra 8C12B272
  8C12B2AE  7EF8  add ##-8,r14
  8C12B2B0  7430  add ##48,r4
  8C12B2B2  0483  pref @r4
  8C12B2B4  74D0  add ##-48,r4
  8C12B2B6  F48C  fmov fr8,fr4
  8C12B2B8  7520  add ##32,r5
  8C12B2BA  A010  bra 8C12B2DE
  8C12B2BC  F6AC  fmov fr10,fr6
  ...
  8C12B2C0  4910  dt r9
  8C12B2C2  6263  mov r6,r2
  8C12B2C4  8D08  bt.s 8C12B2D8
  8C12B2C6  4221  shar r2
  8C12B2C8  C801  tst ##1,R0
  8C12B2CA  E303  mov ##0x03,r3
  8C12B2CC  8DB3  bt.s 8C12B236
  8C12B2CE  3E4C  add r4,r14
  8C12B2D0  6E43  mov r4,r14
  8C12B2D2  7418  add ##24,r4
  8C12B2D4  AFAF  bra 8C12B236
  8C12B2D6  7EF8  add ##-8,r14
  ...
  8C12B2DE  F672  fmul fr7,fr6
  8C12B2E0  F60B  fmov.s fr0,@-r6
  8C12B2E2  F572  fmul fr7,fr5
  8C12B2E4  F66B  fmov.s fr6,@-r6
  8C12B2E6  7520  add ##32,r5
  8C12B2E8  F64B  fmov.s fr4,@-r6
  8C12B2EA  74F8  add ##-8,r4
  8C12B2EC  2622  mov.l r2,@r6
  8C12B2EE  F3FD  fschg
  8C12B2F0  0683  pref @r6
  8C12B2F2  000B  rts
  8C12B2F4  7620  add ##32,r6
  ...
  8C12B43A  E27F  mov ##0x7F,r2
  8C12B43C  E303  mov ##0x03,r3
  8C12B43E  6946  mov.l @r4+,r9
  8C12B440  76E0  add ##-32,r6
  8C12B442  6042  mov.l @r4,r0
  8C12B444  727F  add ##127,r2
  8C12B446  F3FD  fschg
  8C12B448  7237  add ##55,r2
  8C12B44A  C801  tst ##1,R0
  8C12B44C  6E43  mov r4,r14
  8C12B44E  8F03  bf.s 8C12B458
  8C12B450  7420  add ##32,r4
  8C12B452  5EE1  mov.l @(4,r14),r14
  8C12B454  74E8  add ##-24,r4
  8C12B456  3E4C  add r4,r14
  8C12B458  F4E9  fmov.s @r14+,fr4
  8C12B45A  F6E9  fmov.s @r14+,fr6
  8C12B45C  6763  mov r6,r7
  8C12B45E  F28D  fldi0 fr2
  8C12B460  F270  fadd fr7,fr2
  8C12B462  F79D  fldi1 fr7
  8C12B464  7640  add ##64,r6
  8C12B466  F38D  fldi0 fr3
  8C12B468  7520  add ##32,r5
  8C12B46A  F0E9  fmov.s @r14+,fr0
  8C12B46C  F5FD  ftrv xmtrx,fv4
  8C12B46E  FB8D  fldi0 fr11
  8C12B470  4310  dt r3
  8C12B472  6046  mov.l @r4+,r0
  8C12B474  8D3C  bt.s 8C12B4F0
  8C12B476  61E3  mov r14,r1
  8C12B478  F3ED  fipr fv12,fv0
  8C12B47A  C801  tst ##1,R0
  8C12B47C  6E46  mov.l @r4+,r14
  8C12B47E  8F0F  bf.s 8C12B4A0
  8C12B480  F79D  fldi1 fr7
  8C12B482  F743  fdiv fr4,fr7
  8C12B484  0483  pref @r4
  8C12B486  F3B5  fcmp/gt fr11,fr3
  8C12B488  3E4C  add r4,r14
  8C12B48A  FF1D  flds fr15,FPUL
  8C12B48C  F8ED  fipr fv12,fv8
  8C12B48E  0E83  pref @r14
  8C12B490  8F1C  bf.s 8C12B4CC
  8C12B492  F20D  fsts FPUL,fr2
  8C12B494  F230  fadd fr3,fr2
  8C12B496  F38D  fldi0 fr3
  8C12B498  A00F  bra 8C12B4BA
  8C12B49A  FB3D  ftrc fr11, FPUL
  8C12B49C  74E4  add ##-28,r4
  8C12B49E  F79D  fldi1 fr7
  8C12B4A0  F743  fdiv fr4,fr7
  8C12B4A2  7418  add ##24,r4
  8C12B4A4  F3B5  fcmp/gt fr11,fr3
  8C12B4A6  FF1D  flds fr15,FPUL
  8C12B4A8  F8ED  fipr fv12,fv8
  8C12B4AA  6E43  mov r4,r14
  8C12B4AC  7EE0  add ##-32,r14
  8C12B4AE  0483  pref @r4
  8C12B4B0  8F0C  bf.s 8C12B4CC
  8C12B4B2  F20D  fsts FPUL,fr2
  8C12B4B4  F230  fadd fr3,fr2
  8C12B4B6  FB3D  ftrc fr11, FPUL
  8C12B4B8  F38D  fldi0 fr3
  8C12B4BA  005A  sts FPUL,r0
  8C12B4BC  3027  cmp/gt r2,r0
  8C12B4BE  4008  shll2 r0
  8C12B4C0  8B05  bf 8C12B4CE
  8C12B4C2  F3FD  fschg
  8C12B4C4  F386  fmov.s @(R0,r8),fr3
  8C12B4C6  A002  bra 8C12B4CE
  8C12B4C8  F3FD  fschg
  ...
  8C12B4CE  F018  fmov.s @r1,fr0
  8C12B4D0  F672  fmul fr7,fr6
  8C12B4D2  F62B  fmov.s fr2,@-r6
  8C12B4D4  F572  fmul fr7,fr5
  8C12B4D6  F60B  fmov.s fr0,@-r6
  8C12B4D8  2338  tst r3,r3
  8C12B4DA  F66B  fmov.s fr6,@-r6
  8C12B4DC  F64B  fmov.s fr4,@-r6
  8C12B4DE  8D02  bt.s 8C12B4E6
  8C12B4E0  2672  mov.l r7,@r6
  8C12B4E2  F4E9  fmov.s @r14+,fr4
  8C12B4E4  AFB9  bra 8C12B45A
  8C12B4E6  0683  pref @r6
  ...
  8C12B4F0  6763  mov r6,r7
  8C12B4F2  4721  shar r7
  8C12B4F4  4910  dt r9
  8C12B4F6  8DD1  bt.s 8C12B49C
  8C12B4F8  F3ED  fipr fv12,fv0
  8C12B4FA  6E46  mov.l @r4+,r14
  8C12B4FC  C801  tst ##1,R0
  8C12B4FE  D303  mov.l @([8C12B50C]),r3
  8C12B500  F79D  fldi1 fr7
  8C12B502  8FCE  bf.s 8C12B4A2
  8C12B504  F743  fdiv fr4,fr7
  8C12B506  AFBE  bra 8C12B486
  8C12B508  0483  pref @r4
```

## Power Stone (USA) `0C0E8010`

Dump: `/mnt/1TB/dcbat/20261007-160119_Power_Stone__USA__/jit-75898.txt`

```
  0C0E8010  6946  mov.l @r4+,r9
  0C0E8012  F29D  fldi1 fr2
  0C0E8014  F3FD  fschg
  0C0E8016  F38D  fldi0 fr3
  0C0E8018  E303  mov ##0x03,r3
  0C0E801A  F62B  fmov.s fr2,@-r6
  0C0E801C  7620  add ##32,r6
  0C0E801E  F62A  fmov.s fr2,@r6
  0C0E8020  6042  mov.l @r4,r0
  0C0E8022  6E43  mov r4,r14
  0C0E8024  C801  tst ##1,R0
  0C0E8026  7420  add ##32,r4
  0C0E8028  8B02  bf 0C0E8030
  0C0E802A  5EE1  mov.l @(4,r14),r14
  0C0E802C  74E8  add ##-24,r4
  0C0E802E  3E4C  add r4,r14
  0C0E8030  F4E9  fmov.s @r14+,fr4
  0C0E8032  F6E8  fmov.s @r14,fr6
  0C0E8034  7E10  add ##16,r14
  0C0E8036  F79D  fldi1 fr7
  0C0E8038  F0E8  fmov.s @r14,fr0
  0C0E803A  F5FD  ftrv xmtrx,fv4
  0C0E803C  F79D  fldi1 fr7
  0C0E803E  F743  fdiv fr4,fr7
  0C0E8040  4310  dt r3
  0C0E8042  6046  mov.l @r4+,r0
  0C0E8044  8D4C  bt.s 0C0E80E0
  0C0E8046  6E46  mov.l @r4+,r14
  0C0E8048  C801  tst ##1,R0
  0C0E804A  6263  mov r6,r2
  0C0E804C  8D03  bt.s 0C0E8056
  0C0E804E  3E4C  add r4,r14
  0C0E8050  6E43  mov r4,r14
  0C0E8052  7418  add ##24,r4
  0C0E8054  7EF8  add ##-8,r14
  0C0E8056  F8E9  fmov.s @r14+,fr8
  0C0E8058  7420  add ##32,r4
  0C0E805A  0483  pref @r4
  0C0E805C  FAE8  fmov.s @r14,fr10
  0C0E805E  7E10  add ##16,r14
  0C0E8060  FB9D  fldi1 fr11
  0C0E8062  F672  fmul fr7,fr6
  0C0E8064  F60B  fmov.s fr0,@-r6
  0C0E8066  F572  fmul fr7,fr5
  0C0E8068  F66B  fmov.s fr6,@-r6
  0C0E806A  F9FD  ftrv xmtrx,fv8
  0C0E806C  F64B  fmov.s fr4,@-r6
  0C0E806E  74E0  add ##-32,r4
  0C0E8070  2622  mov.l r2,@r6
  0C0E8072  F0E8  fmov.s @r14,fr0
  0C0E8074  4310  dt r3
  0C0E8076  0683  pref @r6
  0C0E8078  7638  add ##56,r6
  0C0E807A  6263  mov r6,r2
  0C0E807C  FB9D  fldi1 fr11
  0C0E807E  FB83  fdiv fr8,fr11
  0C0E8080  6046  mov.l @r4+,r0
  0C0E8082  8D19  bt.s 0C0E80B8
  0C0E8084  6E46  mov.l @r4+,r14
  0C0E8086  C801  tst ##1,R0
  0C0E8088  8D03  bt.s 0C0E8092
  0C0E808A  3E4C  add r4,r14
  0C0E808C  6E43  mov r4,r14
  0C0E808E  7418  add ##24,r4
  0C0E8090  7EF8  add ##-8,r14
  0C0E8092  7420  add ##32,r4
  0C0E8094  F4E9  fmov.s @r14+,fr4
  0C0E8096  0483  pref @r4
  0C0E8098  F6E8  fmov.s @r14,fr6
  0C0E809A  7E10  add ##16,r14
  0C0E809C  F79D  fldi1 fr7
  0C0E809E  FAB2  fmul fr11,fr10
  0C0E80A0  74E0  add ##-32,r4
  0C0E80A2  F9B2  fmul fr11,fr9
  0C0E80A4  F60B  fmov.s fr0,@-r6
  0C0E80A6  F6AB  fmov.s fr10,@-r6
  0C0E80A8  F5FD  ftrv xmtrx,fv4
  0C0E80AA  F68B  fmov.s fr8,@-r6
  0C0E80AC  7540  add ##64,r5
  0C0E80AE  2622  mov.l r2,@r6
  0C0E80B0  F0E8  fmov.s @r14,fr0
  0C0E80B2  0683  pref @r6
  0C0E80B4  AFC2  bra 0C0E803C
  0C0E80B6  7638  add ##56,r6
  0C0E80B8  4910  dt r9
  0C0E80BA  E2FF  mov ##0xFF,r2
  0C0E80BC  8908  bt 0C0E80D0
  0C0E80BE  C801  tst ##1,R0
  0C0E80C0  E303  mov ##0x03,r3
  0C0E80C2  8DE6  bt.s 0C0E8092
  0C0E80C4  3E4C  add r4,r14
  0C0E80C6  6E43  mov r4,r14
  0C0E80C8  7418  add ##24,r4
  0C0E80CA  AFE2  bra 0C0E8092
  0C0E80CC  7EF8  add ##-8,r14
  ...
  0C0E80D0  7430  add ##48,r4
  0C0E80D2  0483  pref @r4
  0C0E80D4  74D0  add ##-48,r4
  0C0E80D6  F48C  fmov fr8,fr4
  0C0E80D8  7520  add ##32,r5
  0C0E80DA  A010  bra 0C0E80FE
  0C0E80DC  F6AC  fmov fr10,fr6
  ...
  0C0E80E0  4910  dt r9
  0C0E80E2  E2FF  mov ##0xFF,r2
  0C0E80E4  8908  bt 0C0E80F8
  0C0E80E6  C801  tst ##1,R0
  0C0E80E8  E303  mov ##0x03,r3
  0C0E80EA  8DB4  bt.s 0C0E8056
  0C0E80EC  3E4C  add r4,r14
  0C0E80EE  6E43  mov r4,r14
  0C0E80F0  7418  add ##24,r4
  0C0E80F2  AFB0  bra 0C0E8056
  0C0E80F4  7EF8  add ##-8,r14
  ...
  0C0E80F8  7430  add ##48,r4
  0C0E80FA  0483  pref @r4
  0C0E80FC  74D0  add ##-48,r4
  0C0E80FE  F672  fmul fr7,fr6
  0C0E8100  F60B  fmov.s fr0,@-r6
  0C0E8102  F572  fmul fr7,fr5
  0C0E8104  F66B  fmov.s fr6,@-r6
  0C0E8106  7520  add ##32,r5
  0C0E8108  F64B  fmov.s fr4,@-r6
  0C0E810A  74F8  add ##-8,r4
  0C0E810C  2622  mov.l r2,@r6
  0C0E810E  F3FD  fschg
  0C0E8110  0683  pref @r6
  0C0E8112  000B  rts
  0C0E8114  7620  add ##32,r6
```

## Project Justice (USA) `0C154A9C`

Dump: `/mnt/1TB/dcbat/20261007-160314_Project_Justice__USA__/jit-78773.txt`

```
  0C154A9C  A136  bra 0C154D0C
  0C154A9E  0009  nop
  ...
  0C154D0C  6946  mov.l @r4+,r9
  0C154D0E  F29D  fldi1 fr2
  0C154D10  F3FD  fschg
  0C154D12  F39D  fldi1 fr3
  0C154D14  E303  mov ##0x03,r3
  0C154D16  F62B  fmov.s fr2,@-r6
  0C154D18  7620  add ##32,r6
  0C154D1A  F62A  fmov.s fr2,@r6
  0C154D1C  6042  mov.l @r4,r0
  0C154D1E  6E43  mov r4,r14
  0C154D20  C801  tst ##1,R0
  0C154D22  7420  add ##32,r4
  0C154D24  8B02  bf 0C154D2C
  ...
  0C154D2C  F4E9  fmov.s @r14+,fr4
  0C154D2E  F6E8  fmov.s @r14,fr6
  0C154D30  7E10  add ##16,r14
  0C154D32  F79D  fldi1 fr7
  0C154D34  F0E8  fmov.s @r14,fr0
  0C154D36  F5FD  ftrv xmtrx,fv4
  0C154D38  F79D  fldi1 fr7
  0C154D3A  F743  fdiv fr4,fr7
  0C154D3C  4310  dt r3
  0C154D3E  6046  mov.l @r4+,r0
  0C154D40  8D4C  bt.s 0C154DDC
  0C154D42  6E46  mov.l @r4+,r14
  0C154D44  C801  tst ##1,R0
  0C154D46  6263  mov r6,r2
  0C154D48  8D03  bt.s 0C154D52
  0C154D4A  3E4C  add r4,r14
  0C154D4C  6E43  mov r4,r14
  0C154D4E  7418  add ##24,r4
  0C154D50  7EF8  add ##-8,r14
  0C154D52  F8E9  fmov.s @r14+,fr8
  0C154D54  7420  add ##32,r4
  0C154D56  0483  pref @r4
  0C154D58  FAE8  fmov.s @r14,fr10
  0C154D5A  7E10  add ##16,r14
  0C154D5C  FB9D  fldi1 fr11
  0C154D5E  F672  fmul fr7,fr6
  0C154D60  F60B  fmov.s fr0,@-r6
  0C154D62  F572  fmul fr7,fr5
  0C154D64  F66B  fmov.s fr6,@-r6
  0C154D66  F9FD  ftrv xmtrx,fv8
  0C154D68  F64B  fmov.s fr4,@-r6
  0C154D6A  74E0  add ##-32,r4
  0C154D6C  2622  mov.l r2,@r6
  0C154D6E  F0E8  fmov.s @r14,fr0
  0C154D70  4310  dt r3
  0C154D72  0683  pref @r6
  0C154D74  7638  add ##56,r6
  0C154D76  6263  mov r6,r2
  0C154D78  FB9D  fldi1 fr11
  0C154D7A  FB83  fdiv fr8,fr11
  0C154D7C  6046  mov.l @r4+,r0
  0C154D7E  8D19  bt.s 0C154DB4
  0C154D80  6E46  mov.l @r4+,r14
  0C154D82  C801  tst ##1,R0
  0C154D84  8D03  bt.s 0C154D8E
  0C154D86  3E4C  add r4,r14
  0C154D88  6E43  mov r4,r14
  0C154D8A  7418  add ##24,r4
  0C154D8C  7EF8  add ##-8,r14
  0C154D8E  7420  add ##32,r4
  0C154D90  F4E9  fmov.s @r14+,fr4
  0C154D92  0483  pref @r4
  0C154D94  F6E8  fmov.s @r14,fr6
  0C154D96  7E10  add ##16,r14
  0C154D98  F79D  fldi1 fr7
  0C154D9A  FAB2  fmul fr11,fr10
  0C154D9C  74E0  add ##-32,r4
  0C154D9E  F9B2  fmul fr11,fr9
  0C154DA0  F60B  fmov.s fr0,@-r6
  0C154DA2  F6AB  fmov.s fr10,@-r6
  0C154DA4  F5FD  ftrv xmtrx,fv4
  0C154DA6  F68B  fmov.s fr8,@-r6
  0C154DA8  7540  add ##64,r5
  0C154DAA  2622  mov.l r2,@r6
  0C154DAC  F0E8  fmov.s @r14,fr0
  0C154DAE  0683  pref @r6
  0C154DB0  AFC2  bra 0C154D38
  0C154DB2  7638  add ##56,r6
  0C154DB4  4910  dt r9
  0C154DB6  6263  mov r6,r2
  0C154DB8  8D08  bt.s 0C154DCC
  0C154DBA  4221  shar r2
  0C154DBC  C801  tst ##1,R0
  0C154DBE  E303  mov ##0x03,r3
  0C154DC0  8DE5  bt.s 0C154D8E
  0C154DC2  3E4C  add r4,r14
  0C154DC4  6E43  mov r4,r14
  0C154DC6  7418  add ##24,r4
  0C154DC8  AFE1  bra 0C154D8E
  0C154DCA  7EF8  add ##-8,r14
  0C154DCC  7430  add ##48,r4
  0C154DCE  0483  pref @r4
  0C154DD0  74D0  add ##-48,r4
  0C154DD2  F48C  fmov fr8,fr4
  0C154DD4  7520  add ##32,r5
  0C154DD6  A010  bra 0C154DFA
  0C154DD8  F6AC  fmov fr10,fr6
  ...
  0C154DDC  4910  dt r9
  0C154DDE  6263  mov r6,r2
  0C154DE0  8D08  bt.s 0C154DF4
  0C154DE2  4221  shar r2
  0C154DE4  C801  tst ##1,R0
  0C154DE6  E303  mov ##0x03,r3
  0C154DE8  8DB3  bt.s 0C154D52
  0C154DEA  3E4C  add r4,r14
  0C154DEC  6E43  mov r4,r14
  0C154DEE  7418  add ##24,r4
  0C154DF0  AFAF  bra 0C154D52
  0C154DF2  7EF8  add ##-8,r14
  ...
  0C154DFA  F672  fmul fr7,fr6
  0C154DFC  F60B  fmov.s fr0,@-r6
  0C154DFE  F572  fmul fr7,fr5
  0C154E00  F66B  fmov.s fr6,@-r6
  0C154E02  7520  add ##32,r5
  0C154E04  F64B  fmov.s fr4,@-r6
  0C154E06  74F8  add ##-8,r4
  0C154E08  2622  mov.l r2,@r6
  0C154E0A  F3FD  fschg
  0C154E0C  0683  pref @r6
  0C154E0E  000B  rts
  0C154E10  7620  add ##32,r6
```

## Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) `8C1D8A8C`

Dump: `/mnt/1TB/dcbat_off/20261007-192124_Shenmue_II__Europe___En_Fr_De_Es___Disc_/jit-12387.txt`

```
  8C1D8A8C  A13B  bra 8C1D8D06
  8C1D8A8E  0009  nop
  ...
  8C1D8D06  E27F  mov ##0x7F,r2
  8C1D8D08  6346  mov.l @r4+,r3
  8C1D8D0A  76E0  add ##-32,r6
  8C1D8D0C  6042  mov.l @r4,r0
  8C1D8D0E  727F  add ##127,r2
  8C1D8D10  F3FD  fschg
  8C1D8D12  7237  add ##55,r2
  8C1D8D14  C801  tst ##1,R0
  8C1D8D16  6E43  mov r4,r14
  8C1D8D18  8F03  bf.s 8C1D8D22
  8C1D8D1A  7420  add ##32,r4
  ...
  8C1D8D22  F4E9  fmov.s @r14+,fr4
  8C1D8D24  F6E9  fmov.s @r14+,fr6
  8C1D8D26  6763  mov r6,r7
  8C1D8D28  F28D  fldi0 fr2
  8C1D8D2A  F270  fadd fr7,fr2
  8C1D8D2C  F79D  fldi1 fr7
  8C1D8D2E  7640  add ##64,r6
  8C1D8D30  F38D  fldi0 fr3
  8C1D8D32  7520  add ##32,r5
  8C1D8D34  F0E9  fmov.s @r14+,fr0
  8C1D8D36  F5FD  ftrv xmtrx,fv4
  8C1D8D38  FB8D  fldi0 fr11
  8C1D8D3A  4310  dt r3
  8C1D8D3C  6046  mov.l @r4+,r0
  8C1D8D3E  8D3D  bt.s 8C1D8DBC
  8C1D8D40  61E3  mov r14,r1
  8C1D8D42  F3ED  fipr fv12,fv0
  8C1D8D44  C801  tst ##1,R0
  8C1D8D46  6E46  mov.l @r4+,r14
  8C1D8D48  8F10  bf.s 8C1D8D6C
  8C1D8D4A  F79D  fldi1 fr7
  ...
  8C1D8D68  74E4  add ##-28,r4
  8C1D8D6A  F79D  fldi1 fr7
  8C1D8D6C  F743  fdiv fr4,fr7
  8C1D8D6E  7418  add ##24,r4
  8C1D8D70  F3B5  fcmp/gt fr11,fr3
  8C1D8D72  FF1D  flds fr15,FPUL
  8C1D8D74  F8ED  fipr fv12,fv8
  8C1D8D76  6E43  mov r4,r14
  8C1D8D78  7EE0  add ##-32,r14
  8C1D8D7A  0483  pref @r4
  8C1D8D7C  8F0C  bf.s 8C1D8D98
  8C1D8D7E  F20D  fsts FPUL,fr2
  8C1D8D80  F230  fadd fr3,fr2
  8C1D8D82  FB3D  ftrc fr11, FPUL
  8C1D8D84  F38D  fldi0 fr3
  8C1D8D86  005A  sts FPUL,r0
  8C1D8D88  3027  cmp/gt r2,r0
  8C1D8D8A  4008  shll2 r0
  8C1D8D8C  8B05  bf 8C1D8D9A
  8C1D8D8E  F3FD  fschg
  8C1D8D90  F386  fmov.s @(R0,r8),fr3
  8C1D8D92  A002  bra 8C1D8D9A
  8C1D8D94  F3FD  fschg
  ...
  8C1D8D98  F38D  fldi0 fr3
  8C1D8D9A  F018  fmov.s @r1,fr0
  8C1D8D9C  F672  fmul fr7,fr6
  8C1D8D9E  F62B  fmov.s fr2,@-r6
  8C1D8DA0  F572  fmul fr7,fr5
  8C1D8DA2  F60B  fmov.s fr0,@-r6
  8C1D8DA4  2338  tst r3,r3
  8C1D8DA6  F66B  fmov.s fr6,@-r6
  8C1D8DA8  F64B  fmov.s fr4,@-r6
  8C1D8DAA  8D02  bt.s 8C1D8DB2
  8C1D8DAC  2672  mov.l r7,@r6
  8C1D8DAE  F4E9  fmov.s @r14+,fr4
  8C1D8DB0  AFB8  bra 8C1D8D24
  8C1D8DB2  0683  pref @r6
  ...
  8C1D8DBC  6763  mov r6,r7
  8C1D8DBE  4721  shar r7
  8C1D8DC0  4015  cmp/pl r0
  8C1D8DC2  8FD1  bf.s 8C1D8D68
  8C1D8DC4  F3ED  fipr fv12,fv0
  8C1D8DC6  C880  tst ##128,R0
  8C1D8DC8  F79D  fldi1 fr7
  8C1D8DCA  89CD  bt 8C1D8D68
  8C1D8DCC  6346  mov.l @r4+,r3
  8C1D8DCE  6046  mov.l @r4+,r0
  8C1D8DD0  6E46  mov.l @r4+,r14
  8C1D8DD2  C801  tst ##1,R0
  8C1D8DD4  F743  fdiv fr4,fr7
  8C1D8DD6  8BCA  bf 8C1D8D6E
```
