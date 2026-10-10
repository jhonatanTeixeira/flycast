# 054_copia

> Gerado por `tools/sdk_find.py`. Jogos: 15 · variantes (sequências normalizadas distintas): 11 · tempo perf somado: 0.80% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C17B3F4` | 95 | 0.00% |
| Dead or Alive 2 (USA) | `8C1077A4` | 85 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C110F34` | 58 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C113948` | 58 | 0.00% |
| Grandia II (USA) | `8C0130D0` | 68 | 0.00% |
| Grandia II (USA) | `8C3890A8` | 49 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C170F18` | 85 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C0107BC` | 65 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C010E84` | 95 | 0.00% |
| Macross M3 | `8C013808` | 78 | 0.00% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C129D98` | 85 | 0.80% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C039124` | 95 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `AC00EE30` | 78 | 0.00% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C12BB60` | 95 | 0.00% |
| Project Justice (USA) | `0C1502AC` | 95 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C132FC8` | 95 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C01189C` | 95 | 0.00% |
| Sonic Adventure 2 (USA) (EnJaFrDeEs) | `8C0C1668` | 85 | 0.00% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C17B3F4`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C17B3F4  2FD6  mov.l r13,@-r15
  8C17B3F6  3450  cmp/eq r5,r4
  8C17B3F8  2FC6  mov.l r12,@-r15
  8C17B3FA  2FB6  mov.l r11,@-r15
  8C17B3FC  8954  bt 8C17B4A8
  8C17B3FE  E200  mov ##0x00,r2
  8C17B400  3626  cmp/hi r2,r6
  8C17B402  8B51  bf 8C17B4A8
  8C17B404  6743  mov r4,r7
  8C17B406  6B63  mov r6,r11
  8C17B408  275B  or r5,r7
  8C17B40A  6C63  mov r6,r12
  8C17B40C  4B01  shlr r11
  8C17B40E  3452  cmp/hs r5,r4
  8C17B410  276B  or r6,r7
  8C17B412  8D24  bt.s 8C17B45E
  8C17B414  4C09  shlr2 r12
  8C17B416  E301  mov ##0x01,r3
  8C17B418  2378  tst r7,r3
  8C17B41A  8B16  bf 8C17B44A
  8C17B41C  E103  mov ##0x03,r1
  8C17B41E  2718  tst r1,r7
  8C17B420  8B09  bf 8C17B436
  8C17B422  66C3  mov r12,r6
  8C17B424  6743  mov r4,r7
  8C17B426  6356  mov.l @r5+,r3
  8C17B428  76FF  add ##-1,r6
  8C17B42A  2668  tst r6,r6
  8C17B42C  2732  mov.l r3,@r7
  8C17B42E  8FFA  bf.s 8C17B426
  8C17B430  7704  add ##4,r7
  8C17B432  A039  bra 8C17B4A8
  8C17B434  0009  nop
  8C17B436  66B3  mov r11,r6
  8C17B438  6743  mov r4,r7
  8C17B43A  6355  mov.w @r5+,r3
  8C17B43C  76FF  add ##-1,r6
  8C17B43E  2668  tst r6,r6
  8C17B440  2731  mov.w r3,@r7
  8C17B442  8FFA  bf.s 8C17B43A
  8C17B444  7702  add ##2,r7
  8C17B446  A02F  bra 8C17B4A8
  8C17B448  0009  nop
  8C17B44A  6053  mov r5,r0
  8C17B44C  6743  mov r4,r7
  8C17B44E  6304  mov.b @r0+,r3
  8C17B450  76FF  add ##-1,r6
  8C17B452  2668  tst r6,r6
  8C17B454  2730  mov.b r3,@r7
  8C17B456  8FFA  bf.s 8C17B44E
  8C17B458  7701  add ##1,r7
  8C17B45A  A025  bra 8C17B4A8
  8C17B45C  0009  nop
  8C17B45E  E201  mov ##0x01,r2
  8C17B460  6043  mov r4,r0
  8C17B462  2278  tst r7,r2
  8C17B464  6D53  mov r5,r13
  8C17B466  306C  add r6,r0
  8C17B468  8F17  bf.s 8C17B49A
  8C17B46A  3D6C  add r6,r13
  8C17B46C  E103  mov ##0x03,r1
  8C17B46E  2718  tst r1,r7
  8C17B470  8B09  bf 8C17B486
  8C17B472  66C3  mov r12,r6
  8C17B474  67D3  mov r13,r7
  8C17B476  6503  mov r0,r5
  8C17B478  77FC  add ##-4,r7
  8C17B47A  6372  mov.l @r7,r3
  8C17B47C  4610  dt r6
  8C17B47E  8FFB  bf.s 8C17B478
  8C17B480  2536  mov.l r3,@-r5
  8C17B482  A011  bra 8C17B4A8
  8C17B484  0009  nop
  8C17B486  66B3  mov r11,r6
  8C17B488  65D3  mov r13,r5
  8C17B48A  6703  mov r0,r7
  8C17B48C  75FE  add ##-2,r5
  8C17B48E  6351  mov.w @r5,r3
  8C17B490  4610  dt r6
  8C17B492  8FFB  bf.s 8C17B48C
  8C17B494  2735  mov.w r3,@-r7
  8C17B496  A007  bra 8C17B4A8
  8C17B498  0009  nop
  8C17B49A  67D3  mov r13,r7
  8C17B49C  6503  mov r0,r5
  8C17B49E  77FF  add ##-1,r7
  8C17B4A0  6370  mov.b @r7,r3
  8C17B4A2  4610  dt r6
  8C17B4A4  8FFB  bf.s 8C17B49E
  8C17B4A6  2534  mov.b r3,@-r5
  8C17B4A8  6BF6  mov.l @r15+,r11
  8C17B4AA  6043  mov r4,r0
  8C17B4AC  6CF6  mov.l @r15+,r12
  8C17B4AE  000B  rts
  8C17B4B0  6DF6  mov.l @r15+,r13
```

## Dead or Alive 2 (USA) `8C1077A4`

Dump: `/mnt/1TB/dcbat_off/20261007-192359_Dead_or_Alive_2__USA__/jit-14513.txt`

```
  8C1077A4  2FD6  mov.l r13,@-r15
  8C1077A6  3450  cmp/eq r5,r4
  8C1077A8  2FC6  mov.l r12,@-r15
  8C1077AA  2FB6  mov.l r11,@-r15
  8C1077AC  8954  bt 8C107858
  8C1077AE  E200  mov ##0x00,r2
  8C1077B0  3626  cmp/hi r2,r6
  8C1077B2  8B51  bf 8C107858
  8C1077B4  6743  mov r4,r7
  8C1077B6  6B63  mov r6,r11
  8C1077B8  275B  or r5,r7
  8C1077BA  6C63  mov r6,r12
  8C1077BC  4B01  shlr r11
  8C1077BE  3452  cmp/hs r5,r4
  8C1077C0  276B  or r6,r7
  8C1077C2  8D24  bt.s 8C10780E
  8C1077C4  4C09  shlr2 r12
  8C1077C6  E301  mov ##0x01,r3
  8C1077C8  2378  tst r7,r3
  8C1077CA  8B16  bf 8C1077FA
  8C1077CC  E103  mov ##0x03,r1
  8C1077CE  2718  tst r1,r7
  8C1077D0  8B09  bf 8C1077E6
  8C1077D2  66C3  mov r12,r6
  8C1077D4  6743  mov r4,r7
  8C1077D6  6356  mov.l @r5+,r3
  8C1077D8  76FF  add ##-1,r6
  8C1077DA  2668  tst r6,r6
  8C1077DC  2732  mov.l r3,@r7
  8C1077DE  8FFA  bf.s 8C1077D6
  8C1077E0  7704  add ##4,r7
  8C1077E2  A039  bra 8C107858
  8C1077E4  0009  nop
  ...
  8C1077FA  6053  mov r5,r0
  8C1077FC  6743  mov r4,r7
  8C1077FE  6304  mov.b @r0+,r3
  8C107800  76FF  add ##-1,r6
  8C107802  2668  tst r6,r6
  8C107804  2730  mov.b r3,@r7
  8C107806  8FFA  bf.s 8C1077FE
  8C107808  7701  add ##1,r7
  8C10780A  A025  bra 8C107858
  8C10780C  0009  nop
  8C10780E  E201  mov ##0x01,r2
  8C107810  6043  mov r4,r0
  8C107812  2278  tst r7,r2
  8C107814  6D53  mov r5,r13
  8C107816  306C  add r6,r0
  8C107818  8F17  bf.s 8C10784A
  8C10781A  3D6C  add r6,r13
  8C10781C  E103  mov ##0x03,r1
  8C10781E  2718  tst r1,r7
  8C107820  8B09  bf 8C107836
  8C107822  66C3  mov r12,r6
  8C107824  67D3  mov r13,r7
  8C107826  6503  mov r0,r5
  8C107828  77FC  add ##-4,r7
  8C10782A  6372  mov.l @r7,r3
  8C10782C  4610  dt r6
  8C10782E  8FFB  bf.s 8C107828
  8C107830  2536  mov.l r3,@-r5
  8C107832  A011  bra 8C107858
  8C107834  0009  nop
  8C107836  66B3  mov r11,r6
  8C107838  65D3  mov r13,r5
  8C10783A  6703  mov r0,r7
  8C10783C  75FE  add ##-2,r5
  8C10783E  6351  mov.w @r5,r3
  8C107840  4610  dt r6
  8C107842  8FFB  bf.s 8C10783C
  8C107844  2735  mov.w r3,@-r7
  8C107846  A007  bra 8C107858
  8C107848  0009  nop
  8C10784A  67D3  mov r13,r7
  8C10784C  6503  mov r0,r5
  8C10784E  77FF  add ##-1,r7
  8C107850  6370  mov.b @r7,r3
  8C107852  4610  dt r6
  8C107854  8FFB  bf.s 8C10784E
  8C107856  2534  mov.b r3,@-r5
  8C107858  6BF6  mov.l @r15+,r11
  8C10785A  6043  mov r4,r0
  8C10785C  6CF6  mov.l @r15+,r12
  8C10785E  000B  rts
  8C107860  6DF6  mov.l @r15+,r13
```

## Evolution - The World of Sacred Device (USA) `8C110F34`

Dump: `/mnt/1TB/dcbat/20261007-151806_Evolution_-_The_World_of_Sacred_Device__/jit-24035.txt`

```
  8C110F34  2FD6  mov.l r13,@-r15
  8C110F36  3450  cmp/eq r5,r4
  8C110F38  2FC6  mov.l r12,@-r15
  8C110F3A  2FB6  mov.l r11,@-r15
  8C110F3C  8954  bt 8C110FE8
  8C110F3E  E200  mov ##0x00,r2
  8C110F40  3626  cmp/hi r2,r6
  8C110F42  8B51  bf 8C110FE8
  8C110F44  6743  mov r4,r7
  8C110F46  6B6D  extu.w r6,r11
  8C110F48  275B  or r5,r7
  8C110F4A  6C63  mov r6,r12
  8C110F4C  4B01  shlr r11
  8C110F4E  3452  cmp/hs r5,r4
  8C110F50  276B  or r6,r7
  8C110F52  8D24  bt.s 8C110F9E
  8C110F54  4C09  shlr2 r12
  8C110F56  E301  mov ##0x01,r3
  8C110F58  2378  tst r7,r3
  8C110F5A  8B16  bf 8C110F8A
  8C110F5C  E103  mov ##0x03,r1
  8C110F5E  2718  tst r1,r7
  8C110F60  8B09  bf 8C110F76
  ...
  8C110F76  66B3  mov r11,r6
  8C110F78  6743  mov r4,r7
  8C110F7A  6355  mov.w @r5+,r3
  8C110F7C  76FF  add ##-1,r6
  8C110F7E  2668  tst r6,r6
  8C110F80  2731  mov.w r3,@r7
  8C110F82  8FFA  bf.s 8C110F7A
  8C110F84  7702  add ##2,r7
  8C110F86  A02F  bra 8C110FE8
  8C110F88  0009  nop
  ...
  8C110F9E  E201  mov ##0x01,r2
  8C110FA0  6043  mov r4,r0
  8C110FA2  2278  tst r7,r2
  8C110FA4  6D53  mov r5,r13
  8C110FA6  306C  add r6,r0
  8C110FA8  8F17  bf.s 8C110FDA
  8C110FAA  3D6C  add r6,r13
  8C110FAC  E103  mov ##0x03,r1
  8C110FAE  2718  tst r1,r7
  8C110FB0  8B09  bf 8C110FC6
  8C110FB2  66C3  mov r12,r6
  8C110FB4  67D3  mov r13,r7
  8C110FB6  6503  mov r0,r5
  8C110FB8  77FC  add ##-4,r7
  8C110FBA  6372  mov.l @r7,r3
  8C110FBC  4610  dt r6
  8C110FBE  8FFB  bf.s 8C110FB8
  8C110FC0  2536  mov.l r3,@-r5
  8C110FC2  A011  bra 8C110FE8
  8C110FC4  0009  nop
  ...
  8C110FE8  6BF6  mov.l @r15+,r11
  8C110FEA  6043  mov r4,r0
  8C110FEC  6CF6  mov.l @r15+,r12
  8C110FEE  000B  rts
  8C110FF0  6DF6  mov.l @r15+,r13
```

## Evolution 2 - Far Off Promise (USA) `8C113948`

Dump: `/mnt/1TB/dcbat/20261007-151251_Evolution_2_-_Far_Off_Promise__USA__/jit-16237.txt`

```
  8C113948  2FD6  mov.l r13,@-r15
  8C11394A  3450  cmp/eq r5,r4
  8C11394C  2FC6  mov.l r12,@-r15
  8C11394E  2FB6  mov.l r11,@-r15
  8C113950  8954  bt 8C1139FC
  8C113952  E200  mov ##0x00,r2
  8C113954  3626  cmp/hi r2,r6
  8C113956  8B51  bf 8C1139FC
  8C113958  6743  mov r4,r7
  8C11395A  6B63  mov r6,r11
  8C11395C  275B  or r5,r7
  8C11395E  6C63  mov r6,r12
  8C113960  4B01  shlr r11
  8C113962  3452  cmp/hs r5,r4
  8C113964  276B  or r6,r7
  8C113966  8D24  bt.s 8C1139B2
  8C113968  4C09  shlr2 r12
  8C11396A  E301  mov ##0x01,r3
  8C11396C  2378  tst r7,r3
  8C11396E  8B16  bf 8C11399E
  8C113970  E103  mov ##0x03,r1
  8C113972  2718  tst r1,r7
  8C113974  8B09  bf 8C11398A
  ...
  8C11398A  66B3  mov r11,r6
  8C11398C  6743  mov r4,r7
  8C11398E  6355  mov.w @r5+,r3
  8C113990  76FF  add ##-1,r6
  8C113992  2668  tst r6,r6
  8C113994  2731  mov.w r3,@r7
  8C113996  8FFA  bf.s 8C11398E
  8C113998  7702  add ##2,r7
  8C11399A  A02F  bra 8C1139FC
  8C11399C  0009  nop
  ...
  8C1139B2  E201  mov ##0x01,r2
  8C1139B4  6043  mov r4,r0
  8C1139B6  2278  tst r7,r2
  8C1139B8  6D53  mov r5,r13
  8C1139BA  306C  add r6,r0
  8C1139BC  8F17  bf.s 8C1139EE
  8C1139BE  3D6C  add r6,r13
  8C1139C0  E103  mov ##0x03,r1
  8C1139C2  2718  tst r1,r7
  8C1139C4  8B09  bf 8C1139DA
  8C1139C6  66C3  mov r12,r6
  8C1139C8  67D3  mov r13,r7
  8C1139CA  6503  mov r0,r5
  8C1139CC  77FC  add ##-4,r7
  8C1139CE  6372  mov.l @r7,r3
  8C1139D0  4610  dt r6
  8C1139D2  8FFB  bf.s 8C1139CC
  8C1139D4  2536  mov.l r3,@-r5
  8C1139D6  A011  bra 8C1139FC
  8C1139D8  0009  nop
  ...
  8C1139FC  6BF6  mov.l @r15+,r11
  8C1139FE  6043  mov r4,r0
  8C113A00  6CF6  mov.l @r15+,r12
  8C113A02  000B  rts
  8C113A04  6DF6  mov.l @r15+,r13
```

## Grandia II (USA) `8C0130D0`

Dump: `/mnt/1TB/dcbat/20261002-192949_Grandia_II__USA__/jit-534412.txt`

```
  8C0130D0  2FD6  mov.l r13,@-r15
  8C0130D2  3450  cmp/eq r5,r4
  8C0130D4  2FC6  mov.l r12,@-r15
  8C0130D6  2FB6  mov.l r11,@-r15
  8C0130D8  8954  bt 8C013184
  8C0130DA  E200  mov ##0x00,r2
  8C0130DC  3626  cmp/hi r2,r6
  8C0130DE  8B51  bf 8C013184
  8C0130E0  6743  mov r4,r7
  8C0130E2  6B63  mov r6,r11
  8C0130E4  275B  or r5,r7
  8C0130E6  6C63  mov r6,r12
  8C0130E8  4B01  shlr r11
  8C0130EA  3452  cmp/hs r5,r4
  8C0130EC  276B  or r6,r7
  8C0130EE  8D24  bt.s 8C01313A
  8C0130F0  4C09  shlr2 r12
  8C0130F2  E301  mov ##0x01,r3
  8C0130F4  2378  tst r7,r3
  8C0130F6  8B16  bf 8C013126
  8C0130F8  E103  mov ##0x03,r1
  8C0130FA  2718  tst r1,r7
  8C0130FC  8B09  bf 8C013112
  8C0130FE  66C3  mov r12,r6
  8C013100  6743  mov r4,r7
  8C013102  6356  mov.l @r5+,r3
  8C013104  76FF  add ##-1,r6
  8C013106  2668  tst r6,r6
  8C013108  2732  mov.l r3,@r7
  8C01310A  8FFA  bf.s 8C013102
  8C01310C  7704  add ##4,r7
  8C01310E  A039  bra 8C013184
  8C013110  0009  nop
  8C013112  66B3  mov r11,r6
  8C013114  6743  mov r4,r7
  8C013116  6355  mov.w @r5+,r3
  8C013118  76FF  add ##-1,r6
  8C01311A  2668  tst r6,r6
  8C01311C  2731  mov.w r3,@r7
  8C01311E  8FFA  bf.s 8C013116
  8C013120  7702  add ##2,r7
  8C013122  A02F  bra 8C013184
  8C013124  0009  nop
  ...
  8C01313A  E201  mov ##0x01,r2
  8C01313C  6043  mov r4,r0
  8C01313E  2278  tst r7,r2
  8C013140  6D53  mov r5,r13
  8C013142  306C  add r6,r0
  8C013144  8F17  bf.s 8C013176
  8C013146  3D6C  add r6,r13
  8C013148  E103  mov ##0x03,r1
  8C01314A  2718  tst r1,r7
  8C01314C  8B09  bf 8C013162
  8C01314E  66C3  mov r12,r6
  8C013150  67D3  mov r13,r7
  8C013152  6503  mov r0,r5
  8C013154  77FC  add ##-4,r7
  8C013156  6372  mov.l @r7,r3
  8C013158  4610  dt r6
  8C01315A  8FFB  bf.s 8C013154
  8C01315C  2536  mov.l r3,@-r5
  8C01315E  A011  bra 8C013184
  8C013160  0009  nop
  ...
  8C013184  6BF6  mov.l @r15+,r11
  8C013186  6043  mov r4,r0
  8C013188  6CF6  mov.l @r15+,r12
  8C01318A  000B  rts
  8C01318C  6DF6  mov.l @r15+,r13
```

## Grandia II (USA) `8C3890A8`

Dump: `/mnt/1TB/dcbat/20261002-192949_Grandia_II__USA__/jit-534412.txt`

```
  8C3890A8  2FD6  mov.l r13,@-r15
  8C3890AA  3450  cmp/eq r5,r4
  8C3890AC  2FC6  mov.l r12,@-r15
  8C3890AE  2FB6  mov.l r11,@-r15
  8C3890B0  8954  bt 8C38915C
  8C3890B2  E200  mov ##0x00,r2
  8C3890B4  3626  cmp/hi r2,r6
  8C3890B6  8B51  bf 8C38915C
  8C3890B8  6743  mov r4,r7
  8C3890BA  6B63  mov r6,r11
  8C3890BC  275B  or r5,r7
  8C3890BE  6C63  mov r6,r12
  8C3890C0  4B01  shlr r11
  8C3890C2  3452  cmp/hs r5,r4
  8C3890C4  276B  or r6,r7
  8C3890C6  8D24  bt.s 8C389112
  8C3890C8  4C09  shlr2 r12
  8C3890CA  E301  mov ##0x01,r3
  8C3890CC  2378  tst r7,r3
  8C3890CE  8B16  bf 8C3890FE
  ...
  8C3890FE  6053  mov r5,r0
  8C389100  6743  mov r4,r7
  8C389102  6304  mov.b @r0+,r3
  8C389104  76FF  add ##-1,r6
  8C389106  2668  tst r6,r6
  8C389108  2730  mov.b r3,@r7
  8C38910A  8FFA  bf.s 8C389102
  8C38910C  7701  add ##1,r7
  8C38910E  A025  bra 8C38915C
  8C389110  0009  nop
  8C389112  E201  mov ##0x01,r2
  8C389114  6043  mov r4,r0
  8C389116  2278  tst r7,r2
  8C389118  6D53  mov r5,r13
  8C38911A  306C  add r6,r0
  8C38911C  8F17  bf.s 8C38914E
  8C38911E  3D6C  add r6,r13
  ...
  8C38914E  67D3  mov r13,r7
  8C389150  6503  mov r0,r5
  8C389152  77FF  add ##-1,r7
  8C389154  6370  mov.b @r7,r3
  8C389156  4610  dt r6
  8C389158  8FFB  bf.s 8C389152
  8C38915A  2534  mov.b r3,@-r5
  8C38915C  6BF6  mov.l @r15+,r11
  8C38915E  6043  mov r4,r0
  8C389160  6CF6  mov.l @r15+,r12
  8C389162  000B  rts
  8C389164  6DF6  mov.l @r15+,r13
```

## King of Fighters The - Evolution (USA) (EnJaEsPt) `8C170F18`

Dump: `/mnt/1TB/dcbat/20261007-164133_King_of_Fighters_The_-_Evolution__USA___/jit-113430.txt`

```
  8C170F18  2FD6  mov.l r13,@-r15
  8C170F1A  3450  cmp/eq r5,r4
  8C170F1C  2FC6  mov.l r12,@-r15
  8C170F1E  2FB6  mov.l r11,@-r15
  8C170F20  8954  bt 8C170FCC
  8C170F22  E200  mov ##0x00,r2
  8C170F24  3626  cmp/hi r2,r6
  8C170F26  8B51  bf 8C170FCC
  8C170F28  6743  mov r4,r7
  8C170F2A  6B63  mov r6,r11
  8C170F2C  275B  or r5,r7
  8C170F2E  6C63  mov r6,r12
  8C170F30  4B01  shlr r11
  8C170F32  3452  cmp/hs r5,r4
  8C170F34  276B  or r6,r7
  8C170F36  8D24  bt.s 8C170F82
  8C170F38  4C09  shlr2 r12
  8C170F3A  E301  mov ##0x01,r3
  8C170F3C  2378  tst r7,r3
  8C170F3E  8B16  bf 8C170F6E
  8C170F40  E103  mov ##0x03,r1
  8C170F42  2718  tst r1,r7
  8C170F44  8B09  bf 8C170F5A
  8C170F46  66C3  mov r12,r6
  8C170F48  6743  mov r4,r7
  8C170F4A  6356  mov.l @r5+,r3
  8C170F4C  76FF  add ##-1,r6
  8C170F4E  2668  tst r6,r6
  8C170F50  2732  mov.l r3,@r7
  8C170F52  8FFA  bf.s 8C170F4A
  8C170F54  7704  add ##4,r7
  8C170F56  A039  bra 8C170FCC
  8C170F58  0009  nop
  8C170F5A  66B3  mov r11,r6
  8C170F5C  6743  mov r4,r7
  8C170F5E  6355  mov.w @r5+,r3
  8C170F60  76FF  add ##-1,r6
  8C170F62  2668  tst r6,r6
  8C170F64  2731  mov.w r3,@r7
  8C170F66  8FFA  bf.s 8C170F5E
  8C170F68  7702  add ##2,r7
  8C170F6A  A02F  bra 8C170FCC
  8C170F6C  0009  nop
  ...
  8C170F82  E201  mov ##0x01,r2
  8C170F84  6043  mov r4,r0
  8C170F86  2278  tst r7,r2
  8C170F88  6D53  mov r5,r13
  8C170F8A  306C  add r6,r0
  8C170F8C  8F17  bf.s 8C170FBE
  8C170F8E  3D6C  add r6,r13
  8C170F90  E103  mov ##0x03,r1
  8C170F92  2718  tst r1,r7
  8C170F94  8B09  bf 8C170FAA
  8C170F96  66C3  mov r12,r6
  8C170F98  67D3  mov r13,r7
  8C170F9A  6503  mov r0,r5
  8C170F9C  77FC  add ##-4,r7
  8C170F9E  6372  mov.l @r7,r3
  8C170FA0  4610  dt r6
  8C170FA2  8FFB  bf.s 8C170F9C
  8C170FA4  2536  mov.l r3,@-r5
  8C170FA6  A011  bra 8C170FCC
  8C170FA8  0009  nop
  8C170FAA  66B3  mov r11,r6
  8C170FAC  65D3  mov r13,r5
  8C170FAE  6703  mov r0,r7
  8C170FB0  75FE  add ##-2,r5
  8C170FB2  6351  mov.w @r5,r3
  8C170FB4  4610  dt r6
  8C170FB6  8FFB  bf.s 8C170FB0
  8C170FB8  2735  mov.w r3,@-r7
  8C170FBA  A007  bra 8C170FCC
  8C170FBC  0009  nop
  8C170FBE  67D3  mov r13,r7
  8C170FC0  6503  mov r0,r5
  8C170FC2  77FF  add ##-1,r7
  8C170FC4  6370  mov.b @r7,r3
  8C170FC6  4610  dt r6
  8C170FC8  8FFB  bf.s 8C170FC2
  8C170FCA  2534  mov.b r3,@-r5
  8C170FCC  6BF6  mov.l @r15+,r11
  8C170FCE  6043  mov r4,r0
  8C170FD0  6CF6  mov.l @r15+,r12
  8C170FD2  000B  rts
  8C170FD4  6DF6  mov.l @r15+,r13
```

## Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) `8C0107BC`

Dump: `/mnt/1TB/dcbat/20261007-163759_Le_Mans_24_Hours__Europe___En_Fr_De_Es_I/jit-109461.txt`

```
  8C0107BC  2FD6  mov.l r13,@-r15
  8C0107BE  3450  cmp/eq r5,r4
  8C0107C0  2FC6  mov.l r12,@-r15
  8C0107C2  2FB6  mov.l r11,@-r15
  8C0107C4  8954  bt 8C010870
  8C0107C6  E200  mov ##0x00,r2
  8C0107C8  3626  cmp/hi r2,r6
  8C0107CA  8B51  bf 8C010870
  8C0107CC  6743  mov r4,r7
  8C0107CE  6B63  mov r6,r11
  8C0107D0  275B  or r5,r7
  8C0107D2  6C63  mov r6,r12
  8C0107D4  4B01  shlr r11
  8C0107D6  3452  cmp/hs r5,r4
  8C0107D8  276B  or r6,r7
  8C0107DA  8D24  bt.s 8C010826
  8C0107DC  4C09  shlr2 r12
  8C0107DE  E301  mov ##0x01,r3
  8C0107E0  2378  tst r7,r3
  8C0107E2  8B16  bf 8C010812
  8C0107E4  E103  mov ##0x03,r1
  8C0107E6  2718  tst r1,r7
  8C0107E8  8B09  bf 8C0107FE
  8C0107EA  66C3  mov r12,r6
  8C0107EC  6743  mov r4,r7
  8C0107EE  6356  mov.l @r5+,r3
  8C0107F0  76FF  add ##-1,r6
  8C0107F2  2668  tst r6,r6
  8C0107F4  2732  mov.l r3,@r7
  8C0107F6  8FFA  bf.s 8C0107EE
  8C0107F8  7704  add ##4,r7
  8C0107FA  A039  bra 8C010870
  8C0107FC  0009  nop
  ...
  8C010826  E201  mov ##0x01,r2
  8C010828  6043  mov r4,r0
  8C01082A  2278  tst r7,r2
  8C01082C  6D53  mov r5,r13
  8C01082E  306C  add r6,r0
  8C010830  8F17  bf.s 8C010862
  8C010832  3D6C  add r6,r13
  8C010834  E103  mov ##0x03,r1
  8C010836  2718  tst r1,r7
  8C010838  8B09  bf 8C01084E
  8C01083A  66C3  mov r12,r6
  8C01083C  67D3  mov r13,r7
  8C01083E  6503  mov r0,r5
  8C010840  77FC  add ##-4,r7
  8C010842  6372  mov.l @r7,r3
  8C010844  4610  dt r6
  8C010846  8FFB  bf.s 8C010840
  8C010848  2536  mov.l r3,@-r5
  8C01084A  A011  bra 8C010870
  8C01084C  0009  nop
  ...
  8C010862  67D3  mov r13,r7
  8C010864  6503  mov r0,r5
  8C010866  77FF  add ##-1,r7
  8C010868  6370  mov.b @r7,r3
  8C01086A  4610  dt r6
  8C01086C  8FFB  bf.s 8C010866
  8C01086E  2534  mov.b r3,@-r5
  8C010870  6BF6  mov.l @r15+,r11
  8C010872  6043  mov r4,r0
  8C010874  6CF6  mov.l @r15+,r12
  8C010876  000B  rts
  8C010878  6DF6  mov.l @r15+,r13
```

## Macross M3 `8C013808`

Dump: `/mnt/1TB/dcbat/20261006-081557_Macross_M3_/jit-5965.txt`

```
  8C013808  2FD6  mov.l r13,@-r15
  8C01380A  3450  cmp/eq r5,r4
  8C01380C  2FC6  mov.l r12,@-r15
  8C01380E  2FB6  mov.l r11,@-r15
  8C013810  8954  bt 8C0138BC
  8C013812  E200  mov ##0x00,r2
  8C013814  3626  cmp/hi r2,r6
  8C013816  8B51  bf 8C0138BC
  8C013818  6743  mov r4,r7
  8C01381A  6B63  mov r6,r11
  8C01381C  275B  or r5,r7
  8C01381E  6C63  mov r6,r12
  8C013820  4B01  shlr r11
  8C013822  3452  cmp/hs r5,r4
  8C013824  276B  or r6,r7
  8C013826  8D24  bt.s 8C013872
  8C013828  4C09  shlr2 r12
  8C01382A  E301  mov ##0x01,r3
  8C01382C  2378  tst r7,r3
  8C01382E  8B16  bf 8C01385E
  8C013830  E103  mov ##0x03,r1
  8C013832  2718  tst r1,r7
  8C013834  8B09  bf 8C01384A
  8C013836  66C3  mov r12,r6
  8C013838  6743  mov r4,r7
  8C01383A  6356  mov.l @r5+,r3
  8C01383C  76FF  add ##-1,r6
  8C01383E  2668  tst r6,r6
  8C013840  2732  mov.l r3,@r7
  8C013842  8FFA  bf.s 8C01383A
  8C013844  7704  add ##4,r7
  8C013846  A039  bra 8C0138BC
  8C013848  0009  nop
  8C01384A  66B3  mov r11,r6
  8C01384C  6743  mov r4,r7
  8C01384E  6355  mov.w @r5+,r3
  8C013850  76FF  add ##-1,r6
  8C013852  2668  tst r6,r6
  8C013854  2731  mov.w r3,@r7
  8C013856  8FFA  bf.s 8C01384E
  8C013858  7702  add ##2,r7
  8C01385A  A02F  bra 8C0138BC
  8C01385C  0009  nop
  ...
  8C013872  E201  mov ##0x01,r2
  8C013874  6043  mov r4,r0
  8C013876  2278  tst r7,r2
  8C013878  6D53  mov r5,r13
  8C01387A  306C  add r6,r0
  8C01387C  8F17  bf.s 8C0138AE
  8C01387E  3D6C  add r6,r13
  8C013880  E103  mov ##0x03,r1
  8C013882  2718  tst r1,r7
  8C013884  8B09  bf 8C01389A
  8C013886  66C3  mov r12,r6
  8C013888  67D3  mov r13,r7
  8C01388A  6503  mov r0,r5
  8C01388C  77FC  add ##-4,r7
  8C01388E  6372  mov.l @r7,r3
  8C013890  4610  dt r6
  8C013892  8FFB  bf.s 8C01388C
  8C013894  2536  mov.l r3,@-r5
  8C013896  A011  bra 8C0138BC
  8C013898  0009  nop
  8C01389A  66B3  mov r11,r6
  8C01389C  65D3  mov r13,r5
  8C01389E  6703  mov r0,r7
  8C0138A0  75FE  add ##-2,r5
  8C0138A2  6351  mov.w @r5,r3
  8C0138A4  4610  dt r6
  8C0138A6  8FFB  bf.s 8C0138A0
  8C0138A8  2735  mov.w r3,@-r7
  8C0138AA  A007  bra 8C0138BC
  8C0138AC  0009  nop
  ...
  8C0138BC  6BF6  mov.l @r15+,r11
  8C0138BE  6043  mov r4,r0
  8C0138C0  6CF6  mov.l @r15+,r12
  8C0138C2  000B  rts
  8C0138C4  6DF6  mov.l @r15+,r13
```

## Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) `AC00EE30`

Dump: `/mnt/1TB/dcbat_off/20261007-190751_Napple_Tale_-_Arsia_in_Daydream__Japan__/jit-6592.txt`

```
  AC00EE30  2FD6  mov.l r13,@-r15
  AC00EE32  3450  cmp/eq r5,r4
  AC00EE34  2FC6  mov.l r12,@-r15
  AC00EE36  2FB6  mov.l r11,@-r15
  AC00EE38  8954  bt AC00EEE4
  AC00EE3A  E200  mov ##0x00,r2
  AC00EE3C  3626  cmp/hi r2,r6
  AC00EE3E  8B51  bf AC00EEE4
  AC00EE40  6743  mov r4,r7
  AC00EE42  6B6D  extu.w r6,r11
  AC00EE44  275B  or r5,r7
  AC00EE46  6C63  mov r6,r12
  AC00EE48  4B01  shlr r11
  AC00EE4A  3452  cmp/hs r5,r4
  AC00EE4C  276B  or r6,r7
  AC00EE4E  8D24  bt.s AC00EE9A
  AC00EE50  4C09  shlr2 r12
  AC00EE52  E301  mov ##0x01,r3
  AC00EE54  2378  tst r7,r3
  AC00EE56  8B16  bf AC00EE86
  AC00EE58  E103  mov ##0x03,r1
  AC00EE5A  2718  tst r1,r7
  AC00EE5C  8B09  bf AC00EE72
  AC00EE5E  66C3  mov r12,r6
  AC00EE60  6743  mov r4,r7
  AC00EE62  6356  mov.l @r5+,r3
  AC00EE64  76FF  add ##-1,r6
  AC00EE66  2668  tst r6,r6
  AC00EE68  2732  mov.l r3,@r7
  AC00EE6A  8FFA  bf.s AC00EE62
  AC00EE6C  7704  add ##4,r7
  AC00EE6E  A039  bra AC00EEE4
  AC00EE70  0009  nop
  AC00EE72  66B3  mov r11,r6
  AC00EE74  6743  mov r4,r7
  AC00EE76  6355  mov.w @r5+,r3
  AC00EE78  76FF  add ##-1,r6
  AC00EE7A  2668  tst r6,r6
  AC00EE7C  2731  mov.w r3,@r7
  AC00EE7E  8FFA  bf.s AC00EE76
  AC00EE80  7702  add ##2,r7
  AC00EE82  A02F  bra AC00EEE4
  AC00EE84  0009  nop
  ...
  AC00EE9A  E201  mov ##0x01,r2
  AC00EE9C  6043  mov r4,r0
  AC00EE9E  2278  tst r7,r2
  AC00EEA0  6D53  mov r5,r13
  AC00EEA2  306C  add r6,r0
  AC00EEA4  8F17  bf.s AC00EED6
  AC00EEA6  3D6C  add r6,r13
  AC00EEA8  E103  mov ##0x03,r1
  AC00EEAA  2718  tst r1,r7
  AC00EEAC  8B09  bf AC00EEC2
  AC00EEAE  66C3  mov r12,r6
  AC00EEB0  67D3  mov r13,r7
  AC00EEB2  6503  mov r0,r5
  AC00EEB4  77FC  add ##-4,r7
  AC00EEB6  6372  mov.l @r7,r3
  AC00EEB8  4610  dt r6
  AC00EEBA  8FFB  bf.s AC00EEB4
  AC00EEBC  2536  mov.l r3,@-r5
  AC00EEBE  A011  bra AC00EEE4
  AC00EEC0  0009  nop
  AC00EEC2  66B3  mov r11,r6
  AC00EEC4  65D3  mov r13,r5
  AC00EEC6  6703  mov r0,r7
  AC00EEC8  75FE  add ##-2,r5
  AC00EECA  6351  mov.w @r5,r3
  AC00EECC  4610  dt r6
  AC00EECE  8FFB  bf.s AC00EEC8
  AC00EED0  2735  mov.w r3,@-r7
  AC00EED2  A007  bra AC00EEE4
  AC00EED4  0009  nop
  ...
  AC00EEE4  6BF6  mov.l @r15+,r11
  AC00EEE6  6043  mov r4,r0
  AC00EEE8  6CF6  mov.l @r15+,r12
  AC00EEEA  000B  rts
  AC00EEEC  6DF6  mov.l @r15+,r13
```

## Sonic Adventure 2 (USA) (EnJaFrDeEs) `8C0C1668`

Dump: `/mnt/1TB/dcbat/20261007-163225_Sonic_Adventure_2__USA___EnJaFrDeEs__/jit-101721.txt`

```
  8C0C1668  2FD6  mov.l r13,@-r15
  8C0C166A  3450  cmp/eq r5,r4
  8C0C166C  2FC6  mov.l r12,@-r15
  8C0C166E  2FB6  mov.l r11,@-r15
  8C0C1670  8954  bt 8C0C171C
  8C0C1672  E200  mov ##0x00,r2
  8C0C1674  3626  cmp/hi r2,r6
  8C0C1676  8B51  bf 8C0C171C
  8C0C1678  6743  mov r4,r7
  8C0C167A  6B63  mov r6,r11
  8C0C167C  275B  or r5,r7
  8C0C167E  6C63  mov r6,r12
  8C0C1680  4B01  shlr r11
  8C0C1682  3452  cmp/hs r5,r4
  8C0C1684  276B  or r6,r7
  8C0C1686  8D24  bt.s 8C0C16D2
  8C0C1688  4C09  shlr2 r12
  8C0C168A  E301  mov ##0x01,r3
  8C0C168C  2378  tst r7,r3
  8C0C168E  8B16  bf 8C0C16BE
  8C0C1690  E103  mov ##0x03,r1
  8C0C1692  2718  tst r1,r7
  8C0C1694  8B09  bf 8C0C16AA
  8C0C1696  66C3  mov r12,r6
  8C0C1698  6743  mov r4,r7
  8C0C169A  6356  mov.l @r5+,r3
  8C0C169C  76FF  add ##-1,r6
  8C0C169E  2668  tst r6,r6
  8C0C16A0  2732  mov.l r3,@r7
  8C0C16A2  8FFA  bf.s 8C0C169A
  8C0C16A4  7704  add ##4,r7
  8C0C16A6  A039  bra 8C0C171C
  8C0C16A8  0009  nop
  8C0C16AA  66B3  mov r11,r6
  8C0C16AC  6743  mov r4,r7
  8C0C16AE  6355  mov.w @r5+,r3
  8C0C16B0  76FF  add ##-1,r6
  8C0C16B2  2668  tst r6,r6
  8C0C16B4  2731  mov.w r3,@r7
  8C0C16B6  8FFA  bf.s 8C0C16AE
  8C0C16B8  7702  add ##2,r7
  8C0C16BA  A02F  bra 8C0C171C
  8C0C16BC  0009  nop
  8C0C16BE  6053  mov r5,r0
  8C0C16C0  6743  mov r4,r7
  8C0C16C2  6304  mov.b @r0+,r3
  8C0C16C4  76FF  add ##-1,r6
  8C0C16C6  2668  tst r6,r6
  8C0C16C8  2730  mov.b r3,@r7
  8C0C16CA  8FFA  bf.s 8C0C16C2
  8C0C16CC  7701  add ##1,r7
  8C0C16CE  A025  bra 8C0C171C
  8C0C16D0  0009  nop
  8C0C16D2  E201  mov ##0x01,r2
  8C0C16D4  6043  mov r4,r0
  8C0C16D6  2278  tst r7,r2
  8C0C16D8  6D53  mov r5,r13
  8C0C16DA  306C  add r6,r0
  8C0C16DC  8F17  bf.s 8C0C170E
  8C0C16DE  3D6C  add r6,r13
  8C0C16E0  E103  mov ##0x03,r1
  8C0C16E2  2718  tst r1,r7
  8C0C16E4  8B09  bf 8C0C16FA
  8C0C16E6  66C3  mov r12,r6
  8C0C16E8  67D3  mov r13,r7
  8C0C16EA  6503  mov r0,r5
  8C0C16EC  77FC  add ##-4,r7
  8C0C16EE  6372  mov.l @r7,r3
  8C0C16F0  4610  dt r6
  8C0C16F2  8FFB  bf.s 8C0C16EC
  8C0C16F4  2536  mov.l r3,@-r5
  8C0C16F6  A011  bra 8C0C171C
  8C0C16F8  0009  nop
  ...
  8C0C170E  67D3  mov r13,r7
  8C0C1710  6503  mov r0,r5
  8C0C1712  77FF  add ##-1,r7
  8C0C1714  6370  mov.b @r7,r3
  8C0C1716  4610  dt r6
  8C0C1718  8FFB  bf.s 8C0C1712
  8C0C171A  2534  mov.b r3,@-r5
  8C0C171C  6BF6  mov.l @r15+,r11
  8C0C171E  6043  mov r4,r0
  8C0C1720  6CF6  mov.l @r15+,r12
  8C0C1722  000B  rts
  8C0C1724  6DF6  mov.l @r15+,r13
```
