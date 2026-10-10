# 015_float

> Gerado por `tools/sdk_find.py`. Jogos: 19 · variantes (sequências normalizadas distintas): 9 · tempo perf somado: 2.38% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C170400` | 23 | 0.00% |
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C170B70` | 45 | 0.00% |
| Dead or Alive 2 (USA) | `8C0FD750` | 23 | 0.00% |
| Dead or Alive 2 (USA) | `8C0FDEC0` | 45 | 0.19% |
| Dead or Alive 2 (USA) | `8C110FC0` | 20 | 0.02% |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C066AF8` | 71 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C197E2C` | 71 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C173ABC` | 71 | 0.00% |
| Grandia II (USA) | `8C04C2D0` | 71 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C018F68` | 19 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C3226B4` | 71 | 0.00% |
| Macross M3 | `8C17D6B4` | 71 | 0.00% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C120910` | 23 | 0.35% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C121080` | 45 | 0.60% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C10B3AC` | 71 | 0.00% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C391484` | 71 | 0.00% |
| Power Stone (USA) | `0C0E1240` | 23 | 0.00% |
| Power Stone (USA) | `0C0E19B0` | 64 | 0.00% |
| Project Justice (USA) | `0C147770` | 23 | 0.00% |
| Project Justice (USA) | `0C147BB0` | 32 | 0.00% |
| Project Justice (USA) | `0C147EE0` | 45 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C195C74` | 71 | 0.00% |
| Shenmue (USA) (Disc 1) | `0C1D1A00` | 25 | 0.08% |
| Shenmue (USA) (Disc 1) | `0C1D2170` | 64 | 0.34% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C1DFB80` | 19 | 0.23% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C1DFD40` | 64 | 0.56% |
| Skies of Arcadia (USA) (Disc 1) | `8C27C4C4` | 71 | 0.00% |
| Sonic Adventure 2 (USA) (EnJaFrDeEs) | `8C118BDC` | 71 | 0.00% |
| Tomb Raider Chronicles (USA) | `8C01288C` | 48 | 0.01% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C170400`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C170400  2448  tst r4,r4
  8C170402  8B01  bf 8C170408
  ...
  8C170408  A01A  bra 8C170440
  8C17040A  0009  nop
  ...
  8C170440  FBFD  frchg
  8C170442  F049  fmov.s @r4+,fr0
  8C170444  F149  fmov.s @r4+,fr1
  8C170446  F249  fmov.s @r4+,fr2
  8C170448  F349  fmov.s @r4+,fr3
  8C17044A  F449  fmov.s @r4+,fr4
  8C17044C  F549  fmov.s @r4+,fr5
  8C17044E  F649  fmov.s @r4+,fr6
  8C170450  F749  fmov.s @r4+,fr7
  8C170452  F849  fmov.s @r4+,fr8
  8C170454  F949  fmov.s @r4+,fr9
  8C170456  FA49  fmov.s @r4+,fr10
  8C170458  FB49  fmov.s @r4+,fr11
  8C17045A  FC49  fmov.s @r4+,fr12
  8C17045C  FD49  fmov.s @r4+,fr13
  8C17045E  FE49  fmov.s @r4+,fr14
  8C170460  FF49  fmov.s @r4+,fr15
  8C170462  000B  rts
  8C170464  FBFD  frchg
```

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C170B70`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C170B70  D30E  mov.l @([8C170BAC]),r3
  8C170B72  FBFD  frchg
  8C170B74  F3FD  fschg
  8C170B76  5132  mov.l @(8,r3),r1
  8C170B78  6232  mov.l @r3,r2
  8C170B7A  6713  mov r1,r7
  8C170B7C  7140  add ##64,r1
  8C170B7E  F1EB  fmov.s fr14,@-r1
  8C170B80  662F  exts.w r2,r6
  8C170B82  F1CB  fmov.s fr12,@-r1
  8C170B84  6229  swap.w r2,r2
  8C170B86  F1AB  fmov.s fr10,@-r1
  8C170B88  622F  exts.w r2,r2
  8C170B8A  F18B  fmov.s fr8,@-r1
  8C170B8C  3267  cmp/gt r6,r2
  8C170B8E  F16B  fmov.s fr6,@-r1
  8C170B90  0029  movt r0
  8C170B92  F14B  fmov.s fr4,@-r1
  8C170B94  7601  add ##1,r6
  8C170B96  F12B  fmov.s fr2,@-r1
  8C170B98  7740  add ##64,r7
  8C170B9A  F10B  fmov.s fr0,@-r1
  8C170B9C  8F00  bf.s 8C170BA0
  8C170B9E  1372  mov.l r7,@(8,r3)
  8C170BA0  2448  tst r4,r4
  8C170BA2  8F05  bf.s 8C170BB0
  8C170BA4  2361  mov.w r6,@r3
  8C170BA6  F3FD  fschg
  8C170BA8  000B  rts
  8C170BAA  FBFD  frchg
  ...
  8C170BB0  E104  mov ##0x04,r1
  8C170BB2  0483  pref @r4
  8C170BB4  2418  tst r1,r4
  8C170BB6  8912  bt 8C170BDE
  ...
  8C170BDE  F049  fmov.s @r4+,fr0
  8C170BE0  F249  fmov.s @r4+,fr2
  8C170BE2  F449  fmov.s @r4+,fr4
  8C170BE4  F649  fmov.s @r4+,fr6
  8C170BE6  F849  fmov.s @r4+,fr8
  8C170BE8  FA49  fmov.s @r4+,fr10
  8C170BEA  FC49  fmov.s @r4+,fr12
  8C170BEC  FE49  fmov.s @r4+,fr14
  8C170BEE  F3FD  fschg
  8C170BF0  000B  rts
  8C170BF2  FBFD  frchg
```

## Dead or Alive 2 (USA) `8C110FC0`

Dump: `/mnt/1TB/dcbat_off/20261007-192359_Dead_or_Alive_2__USA__/jit-14513.txt`

```
  8C110FC0  FBFD  frchg
  8C110FC2  F049  fmov.s @r4+,fr0
  8C110FC4  F149  fmov.s @r4+,fr1
  8C110FC6  F249  fmov.s @r4+,fr2
  8C110FC8  F349  fmov.s @r4+,fr3
  8C110FCA  F449  fmov.s @r4+,fr4
  8C110FCC  F549  fmov.s @r4+,fr5
  8C110FCE  F649  fmov.s @r4+,fr6
  8C110FD0  F749  fmov.s @r4+,fr7
  8C110FD2  F849  fmov.s @r4+,fr8
  8C110FD4  F949  fmov.s @r4+,fr9
  8C110FD6  FA49  fmov.s @r4+,fr10
  8C110FD8  FB49  fmov.s @r4+,fr11
  8C110FDA  FC49  fmov.s @r4+,fr12
  8C110FDC  FD49  fmov.s @r4+,fr13
  8C110FDE  FE49  fmov.s @r4+,fr14
  8C110FE0  FF49  fmov.s @r4+,fr15
  8C110FE2  FBFD  frchg
  8C110FE4  000B  rts
  8C110FE6  0009  nop
```

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C066AF8`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C066AF8  D023  mov.l @([8C066B88]),r0
  8C066AFA  E701  mov ##0x01,r7
  8C066AFC  D223  mov.l @([8C066B8C]),r2
  8C066AFE  2072  mov.l r7,@r0
  8C066B00  D123  mov.l @([8C066B90]),r1
  8C066B02  2262  mov.l r6,@r2
  8C066B04  D023  mov.l @([8C066B94]),r0
  8C066B06  D324  mov.l @([8C066B98]),r3
  8C066B08  2042  mov.l r4,@r0
  8C066B0A  2152  mov.l r5,@r1
  8C066B0C  2342  mov.l r4,@r3
  8C066B0E  D123  mov.l @([8C066B9C]),r1
  8C066B10  7440  add ##64,r4
  8C066B12  7120  add ##32,r1
  8C066B14  F019  fmov.s @r1+,fr0
  8C066B16  F119  fmov.s @r1+,fr1
  8C066B18  F219  fmov.s @r1+,fr2
  8C066B1A  F319  fmov.s @r1+,fr3
  8C066B1C  F419  fmov.s @r1+,fr4
  8C066B1E  F519  fmov.s @r1+,fr5
  8C066B20  F619  fmov.s @r1+,fr6
  8C066B22  F719  fmov.s @r1+,fr7
  8C066B24  F47B  fmov.s fr7,@-r4
  8C066B26  F46B  fmov.s fr6,@-r4
  8C066B28  71C0  add ##-64,r1
  8C066B2A  F45B  fmov.s fr5,@-r4
  8C066B2C  F44B  fmov.s fr4,@-r4
  8C066B2E  F43B  fmov.s fr3,@-r4
  8C066B30  F42B  fmov.s fr2,@-r4
  8C066B32  F41B  fmov.s fr1,@-r4
  8C066B34  F40B  fmov.s fr0,@-r4
  8C066B36  F019  fmov.s @r1+,fr0
  8C066B38  F119  fmov.s @r1+,fr1
  8C066B3A  F219  fmov.s @r1+,fr2
  8C066B3C  F319  fmov.s @r1+,fr3
  8C066B3E  F419  fmov.s @r1+,fr4
  8C066B40  F519  fmov.s @r1+,fr5
  8C066B42  F619  fmov.s @r1+,fr6
  8C066B44  F719  fmov.s @r1+,fr7
  8C066B46  F47B  fmov.s fr7,@-r4
  8C066B48  F46B  fmov.s fr6,@-r4
  8C066B4A  F45B  fmov.s fr5,@-r4
  8C066B4C  F44B  fmov.s fr4,@-r4
  8C066B4E  F43B  fmov.s fr3,@-r4
  8C066B50  F42B  fmov.s fr2,@-r4
  8C066B52  F41B  fmov.s fr1,@-r4
  8C066B54  F40B  fmov.s fr0,@-r4
  8C066B56  F04D  fneg fr0 
  8C066B58  E014  mov ##0x14,r0
  8C066B5A  F54D  fneg fr5 
  8C066B5C  F40A  fmov.s fr0,@r4
  8C066B5E  F457  fmov.s fr5,@(R0,r4)
  8C066B60  FBFD  frchg
  8C066B62  F049  fmov.s @r4+,fr0
  8C066B64  F149  fmov.s @r4+,fr1
  8C066B66  F249  fmov.s @r4+,fr2
  8C066B68  F349  fmov.s @r4+,fr3
  8C066B6A  F449  fmov.s @r4+,fr4
  8C066B6C  F549  fmov.s @r4+,fr5
  8C066B6E  F649  fmov.s @r4+,fr6
  8C066B70  F749  fmov.s @r4+,fr7
  8C066B72  F849  fmov.s @r4+,fr8
  8C066B74  F949  fmov.s @r4+,fr9
  8C066B76  FA49  fmov.s @r4+,fr10
  8C066B78  FB49  fmov.s @r4+,fr11
  8C066B7A  FC49  fmov.s @r4+,fr12
  8C066B7C  FD49  fmov.s @r4+,fr13
  8C066B7E  FE49  fmov.s @r4+,fr14
  8C066B80  FF49  fmov.s @r4+,fr15
  8C066B82  000B  rts
  8C066B84  FBFD  frchg
```

## King of Fighters The - Evolution (USA) (EnJaEsPt) `8C018F68`

Dump: `/mnt/1TB/dcbat/20261007-164133_King_of_Fighters_The_-_Evolution__USA___/jit-113430.txt`

```
  8C018F68  FBFD  frchg
  8C018F6A  F049  fmov.s @r4+,fr0
  8C018F6C  F149  fmov.s @r4+,fr1
  8C018F6E  F249  fmov.s @r4+,fr2
  8C018F70  F349  fmov.s @r4+,fr3
  8C018F72  F449  fmov.s @r4+,fr4
  8C018F74  F549  fmov.s @r4+,fr5
  8C018F76  F649  fmov.s @r4+,fr6
  8C018F78  F749  fmov.s @r4+,fr7
  8C018F7A  F849  fmov.s @r4+,fr8
  8C018F7C  F949  fmov.s @r4+,fr9
  8C018F7E  FA49  fmov.s @r4+,fr10
  8C018F80  FB49  fmov.s @r4+,fr11
  8C018F82  FC49  fmov.s @r4+,fr12
  8C018F84  FD49  fmov.s @r4+,fr13
  8C018F86  FE49  fmov.s @r4+,fr14
  8C018F88  FF49  fmov.s @r4+,fr15
  8C018F8A  000B  rts
  8C018F8C  FBFD  frchg
```

## Power Stone (USA) `0C0E19B0`

Dump: `/mnt/1TB/dcbat/20261007-160119_Power_Stone__USA__/jit-75898.txt`

```
  0C0E19B0  D30E  mov.l @([0C0E19EC]),r3
  0C0E19B2  FBFD  frchg
  0C0E19B4  F3FD  fschg
  0C0E19B6  5132  mov.l @(8,r3),r1
  0C0E19B8  6232  mov.l @r3,r2
  0C0E19BA  6713  mov r1,r7
  0C0E19BC  7140  add ##64,r1
  0C0E19BE  F1EB  fmov.s fr14,@-r1
  0C0E19C0  662F  exts.w r2,r6
  0C0E19C2  F1CB  fmov.s fr12,@-r1
  0C0E19C4  6229  swap.w r2,r2
  0C0E19C6  F1AB  fmov.s fr10,@-r1
  0C0E19C8  622F  exts.w r2,r2
  0C0E19CA  F18B  fmov.s fr8,@-r1
  0C0E19CC  3267  cmp/gt r6,r2
  0C0E19CE  F16B  fmov.s fr6,@-r1
  0C0E19D0  0029  movt r0
  0C0E19D2  F14B  fmov.s fr4,@-r1
  0C0E19D4  7601  add ##1,r6
  0C0E19D6  F12B  fmov.s fr2,@-r1
  0C0E19D8  7740  add ##64,r7
  0C0E19DA  F10B  fmov.s fr0,@-r1
  0C0E19DC  8F00  bf.s 0C0E19E0
  0C0E19DE  1372  mov.l r7,@(8,r3)
  0C0E19E0  2448  tst r4,r4
  0C0E19E2  8F05  bf.s 0C0E19F0
  0C0E19E4  2361  mov.w r6,@r3
  0C0E19E6  F3FD  fschg
  0C0E19E8  000B  rts
  0C0E19EA  FBFD  frchg
  ...
  0C0E19F0  E104  mov ##0x04,r1
  0C0E19F2  0483  pref @r4
  0C0E19F4  2418  tst r1,r4
  0C0E19F6  8912  bt 0C0E1A1E
  0C0E19F8  F3FD  fschg
  0C0E19FA  F049  fmov.s @r4+,fr0
  0C0E19FC  F149  fmov.s @r4+,fr1
  0C0E19FE  F249  fmov.s @r4+,fr2
  0C0E1A00  F349  fmov.s @r4+,fr3
  0C0E1A02  F449  fmov.s @r4+,fr4
  0C0E1A04  F549  fmov.s @r4+,fr5
  0C0E1A06  F649  fmov.s @r4+,fr6
  0C0E1A08  F749  fmov.s @r4+,fr7
  0C0E1A0A  F849  fmov.s @r4+,fr8
  0C0E1A0C  F949  fmov.s @r4+,fr9
  0C0E1A0E  FA49  fmov.s @r4+,fr10
  0C0E1A10  FB49  fmov.s @r4+,fr11
  0C0E1A12  FC49  fmov.s @r4+,fr12
  0C0E1A14  FD49  fmov.s @r4+,fr13
  0C0E1A16  FE49  fmov.s @r4+,fr14
  0C0E1A18  FF49  fmov.s @r4+,fr15
  0C0E1A1A  000B  rts
  0C0E1A1C  FBFD  frchg
  0C0E1A1E  F049  fmov.s @r4+,fr0
  0C0E1A20  F249  fmov.s @r4+,fr2
  0C0E1A22  F449  fmov.s @r4+,fr4
  0C0E1A24  F649  fmov.s @r4+,fr6
  0C0E1A26  F849  fmov.s @r4+,fr8
  0C0E1A28  FA49  fmov.s @r4+,fr10
  0C0E1A2A  FC49  fmov.s @r4+,fr12
  0C0E1A2C  FE49  fmov.s @r4+,fr14
  0C0E1A2E  F3FD  fschg
  0C0E1A30  000B  rts
  0C0E1A32  FBFD  frchg
```

## Project Justice (USA) `0C147BB0`

Dump: `/mnt/1TB/dcbat/20261007-160314_Project_Justice__USA__/jit-78773.txt`

```
  0C147BB0  FFFB  fmov.s fr15,@-r15
  0C147BB2  FFEB  fmov.s fr14,@-r15
  0C147BB4  FFDB  fmov.s fr13,@-r15
  0C147BB6  FFCB  fmov.s fr12,@-r15
  0C147BB8  F049  fmov.s @r4+,fr0
  0C147BBA  F149  fmov.s @r4+,fr1
  0C147BBC  F249  fmov.s @r4+,fr2
  0C147BBE  F349  fmov.s @r4+,fr3
  0C147BC0  F449  fmov.s @r4+,fr4
  0C147BC2  F549  fmov.s @r4+,fr5
  0C147BC4  F649  fmov.s @r4+,fr6
  0C147BC6  F749  fmov.s @r4+,fr7
  0C147BC8  F849  fmov.s @r4+,fr8
  0C147BCA  F949  fmov.s @r4+,fr9
  0C147BCC  FA49  fmov.s @r4+,fr10
  0C147BCE  FB49  fmov.s @r4+,fr11
  0C147BD0  FC49  fmov.s @r4+,fr12
  0C147BD2  FD49  fmov.s @r4+,fr13
  0C147BD4  FE49  fmov.s @r4+,fr14
  0C147BD6  FF49  fmov.s @r4+,fr15
  0C147BD8  FBFD  frchg
  0C147BDA  F1FD  ftrv xmtrx,fv0
  0C147BDC  F5FD  ftrv xmtrx,fv4
  0C147BDE  F9FD  ftrv xmtrx,fv8
  0C147BE0  FDFD  ftrv xmtrx,fv12
  0C147BE2  FBFD  frchg
  0C147BE4  FCF9  fmov.s @r15+,fr12
  0C147BE6  FDF9  fmov.s @r15+,fr13
  0C147BE8  FEF9  fmov.s @r15+,fr14
  0C147BEA  FFF9  fmov.s @r15+,fr15
  0C147BEC  000B  rts
  0C147BEE  0009  nop
```

## Shenmue (USA) (Disc 1) `0C1D1A00`

Dump: `/mnt/1TB/dcbat_off/20261007-191903_Shenmue__USA___Disc_1__/jit-11043.txt`

```
  0C1D1A00  2448  tst r4,r4
  0C1D1A02  8B01  bf 0C1D1A08
  0C1D1A04  D40A  mov.l @([0C1D1A30]),r4
  0C1D1A06  5442  mov.l @(8,r4),r4
  0C1D1A08  A01A  bra 0C1D1A40
  0C1D1A0A  0009  nop
  ...
  0C1D1A40  FBFD  frchg
  0C1D1A42  F049  fmov.s @r4+,fr0
  0C1D1A44  F149  fmov.s @r4+,fr1
  0C1D1A46  F249  fmov.s @r4+,fr2
  0C1D1A48  F349  fmov.s @r4+,fr3
  0C1D1A4A  F449  fmov.s @r4+,fr4
  0C1D1A4C  F549  fmov.s @r4+,fr5
  0C1D1A4E  F649  fmov.s @r4+,fr6
  0C1D1A50  F749  fmov.s @r4+,fr7
  0C1D1A52  F849  fmov.s @r4+,fr8
  0C1D1A54  F949  fmov.s @r4+,fr9
  0C1D1A56  FA49  fmov.s @r4+,fr10
  0C1D1A58  FB49  fmov.s @r4+,fr11
  0C1D1A5A  FC49  fmov.s @r4+,fr12
  0C1D1A5C  FD49  fmov.s @r4+,fr13
  0C1D1A5E  FE49  fmov.s @r4+,fr14
  0C1D1A60  FF49  fmov.s @r4+,fr15
  0C1D1A62  000B  rts
  0C1D1A64  FBFD  frchg
```

## Tomb Raider Chronicles (USA) `8C01288C`

Dump: `/mnt/1TB/dcbat_off/20261007-192238_Tomb_Raider_Chronicles__USA__/jit-13420.txt`

```
  8C01288C  0002  stc SR,r0
  8C01288E  D117  mov.l @([8C0128EC]),r1
  8C012890  2109  and r0,r1
  8C012892  410E  ldc r1,SR
  8C012894  745C  add ##92,r4
  8C012896  745C  add ##92,r4
  8C012898  6246  mov.l @r4+,r2
  8C01289A  6346  mov.l @r4+,r3
  8C01289C  E100  mov ##0x00,r1
  8C01289E  416A  lds r1,FPSCR
  8C0128A0  F049  fmov.s @r4+,fr0
  8C0128A2  F149  fmov.s @r4+,fr1
  8C0128A4  F249  fmov.s @r4+,fr2
  8C0128A6  F349  fmov.s @r4+,fr3
  8C0128A8  F449  fmov.s @r4+,fr4
  8C0128AA  F549  fmov.s @r4+,fr5
  8C0128AC  F649  fmov.s @r4+,fr6
  8C0128AE  F749  fmov.s @r4+,fr7
  8C0128B0  F849  fmov.s @r4+,fr8
  8C0128B2  F949  fmov.s @r4+,fr9
  8C0128B4  FA49  fmov.s @r4+,fr10
  8C0128B6  FB49  fmov.s @r4+,fr11
  8C0128B8  FC49  fmov.s @r4+,fr12
  8C0128BA  FD49  fmov.s @r4+,fr13
  8C0128BC  FE49  fmov.s @r4+,fr14
  8C0128BE  FF49  fmov.s @r4+,fr15
  8C0128C0  FBFD  frchg
  8C0128C2  F049  fmov.s @r4+,fr0
  8C0128C4  F149  fmov.s @r4+,fr1
  8C0128C6  F249  fmov.s @r4+,fr2
  8C0128C8  F349  fmov.s @r4+,fr3
  8C0128CA  F449  fmov.s @r4+,fr4
  8C0128CC  F549  fmov.s @r4+,fr5
  8C0128CE  F649  fmov.s @r4+,fr6
  8C0128D0  F749  fmov.s @r4+,fr7
  8C0128D2  F849  fmov.s @r4+,fr8
  8C0128D4  F949  fmov.s @r4+,fr9
  8C0128D6  FA49  fmov.s @r4+,fr10
  8C0128D8  FB49  fmov.s @r4+,fr11
  8C0128DA  FC49  fmov.s @r4+,fr12
  8C0128DC  FD49  fmov.s @r4+,fr13
  8C0128DE  FE49  fmov.s @r4+,fr14
  8C0128E0  FF49  fmov.s @r4+,fr15
  8C0128E2  426A  lds r2,FPSCR
  8C0128E4  435A  lds r3,FPUL
  8C0128E6  400E  ldc r0,SR
  8C0128E8  000B  rts
  8C0128EA  0009  nop
```
