# 048_geral

> Gerado por `tools/sdk_find.py`. Jogos: 9 · variantes (sequências normalizadas distintas): 11 · tempo perf somado: 0.89% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C071004` | 25 | 0.00% |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C071DDA` | 19 | 0.00% |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C0726AE` | 23 | 0.00% |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C075140` | 21 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C1A9698` | 25 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C1AA46E` | 19 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C1AAD42` | 23 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C1AD874` | 21 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1A984C` | 25 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1AA622` | 24 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1AAEF6` | 28 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1ADA28` | 21 | 0.00% |
| Grandia II (USA) | `8C06C078` | 29 | 0.00% |
| Grandia II (USA) | `8C06D70C` | 21 | 0.00% |
| Grandia II (USA) | `8C06E4AA` | 27 | 0.00% |
| Grandia II (USA) | `8C071AF4` | 21 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C3396F8` | 29 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C33AD8C` | 21 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C33BB2A` | 27 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C33F174` | 21 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C05C342` | 147 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C063A24` | 21 | 0.00% |
| Macross M3 | `8C1F86BC` | 29 | 0.00% |
| Macross M3 | `8C1FAE58` | 21 | 0.00% |
| Macross M3 | `8C1FBBF6` | 27 | 0.00% |
| Macross M3 | `8C1FFD88` | 21 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C15ED00` | 29 | 0.27% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C160394` | 26 | 0.17% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C161132` | 27 | 0.24% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C16477C` | 21 | 0.21% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C36B6A0` | 29 | 0.00% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C36DE3C` | 21 | 0.00% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C36EBDA` | 27 | 0.00% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C372D6C` | 21 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C071004`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C071004  2FE6  mov.l r14,@-r15
  8C071006  4F22  sts.l PR,@-r15
  8C071008  7FFC  add ##-4,r15
  8C07100A  D336  mov.l @([8C0710E4]),r3
  8C07100C  6E43  mov r4,r14
  8C07100E  430B  jsr @r3
  8C071010  E506  mov ##0x06,r5
  8C071012  2008  tst r0,r0
  8C071014  8903  bt 8C07101E
  8C071016  B1EC  bsr 8C0713F2
  8C071018  64E3  mov r14,r4
  8C07101A  8801  cmp/eq ##0x01,R0
  8C07101C  8B04  bf 8C071028
  ...
  8C071028  B00A  bsr 8C071040
  8C07102A  64E3  mov r14,r4
  8C07102C  2F02  mov.l r0,@r15
  8C07102E  B07D  bsr 8C07112C
  8C071030  64E3  mov r14,r4
  8C071032  B0B4  bsr 8C07119E
  8C071034  64E3  mov r14,r4
  8C071036  60F2  mov.l @r15,r0
  8C071038  7F04  add ##4,r15
  8C07103A  4F26  lds.l @r15+,PR
  8C07103C  000B  rts
  8C07103E  6EF6  mov.l @r15+,r14
```

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C071DDA`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C071DDA  2FE6  mov.l r14,@-r15
  8C071DDC  4F22  sts.l PR,@-r15
  8C071DDE  7FFC  add ##-4,r15
  8C071DE0  B240  bsr 8C072264
  8C071DE2  6E43  mov r4,r14
  8C071DE4  8801  cmp/eq ##0x01,R0
  8C071DE6  8B04  bf 8C071DF2
  ...
  8C071DF2  B00A  bsr 8C071E0A
  8C071DF4  64E3  mov r14,r4
  8C071DF6  2F02  mov.l r0,@r15
  8C071DF8  B276  bsr 8C0722E8
  8C071DFA  64E3  mov r14,r4
  8C071DFC  B31A  bsr 8C072434
  8C071DFE  64E3  mov r14,r4
  8C071E00  60F2  mov.l @r15,r0
  8C071E02  7F04  add ##4,r15
  8C071E04  4F26  lds.l @r15+,PR
  8C071E06  000B  rts
  8C071E08  6EF6  mov.l @r15+,r14
```

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C0726AE`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C0726AE  2FE6  mov.l r14,@-r15
  8C0726B0  4F22  sts.l PR,@-r15
  8C0726B2  7FFC  add ##-4,r15
  8C0726B4  D329  mov.l @([8C07275C]),r3
  8C0726B6  6E43  mov r4,r14
  8C0726B8  430B  jsr @r3
  8C0726BA  E505  mov ##0x05,r5
  8C0726BC  2008  tst r0,r0
  8C0726BE  8903  bt 8C0726C8
  8C0726C0  B5BD  bsr 8C07323E
  8C0726C2  64E3  mov r14,r4
  8C0726C4  8801  cmp/eq ##0x01,R0
  8C0726C6  8B04  bf 8C0726D2
  ...
  8C0726D2  B008  bsr 8C0726E6
  8C0726D4  64E3  mov r14,r4
  8C0726D6  2F02  mov.l r0,@r15
  8C0726D8  B5DC  bsr 8C073294
  8C0726DA  64E3  mov r14,r4
  8C0726DC  60F2  mov.l @r15,r0
  8C0726DE  7F04  add ##4,r15
  8C0726E0  4F26  lds.l @r15+,PR
  8C0726E2  000B  rts
  8C0726E4  6EF6  mov.l @r15+,r14
```

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C075140`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C075140  2FE6  mov.l r14,@-r15
  8C075142  4F22  sts.l PR,@-r15
  8C075144  7FFC  add ##-4,r15
  8C075146  D340  mov.l @([8C075248]),r3
  8C075148  6E43  mov r4,r14
  8C07514A  430B  jsr @r3
  8C07514C  E505  mov ##0x05,r5
  8C07514E  2008  tst r0,r0
  8C075150  8B04  bf 8C07515C
  ...
  8C07515C  B00A  bsr 8C075174
  8C07515E  64E3  mov r14,r4
  8C075160  B059  bsr 8C075216
  8C075162  64E3  mov r14,r4
  8C075164  2F02  mov.l r0,@r15
  8C075166  B033  bsr 8C0751D0
  8C075168  64E3  mov r14,r4
  8C07516A  60F2  mov.l @r15,r0
  8C07516C  7F04  add ##4,r15
  8C07516E  4F26  lds.l @r15+,PR
  8C075170  000B  rts
  8C075172  6EF6  mov.l @r15+,r14
```

## Evolution 2 - Far Off Promise (USA) `8C1AA622`

Dump: `/mnt/1TB/dcbat/20261007-151251_Evolution_2_-_Far_Off_Promise__USA__/jit-16237.txt`

```
  8C1AA622  2FE6  mov.l r14,@-r15
  8C1AA624  4F22  sts.l PR,@-r15
  8C1AA626  7FFC  add ##-4,r15
  8C1AA628  B240  bsr 8C1AAAAC
  8C1AA62A  6E43  mov r4,r14
  8C1AA62C  8801  cmp/eq ##0x01,R0
  8C1AA62E  8B04  bf 8C1AA63A
  8C1AA630  E000  mov ##0x00,r0
  8C1AA632  7F04  add ##4,r15
  8C1AA634  4F26  lds.l @r15+,PR
  8C1AA636  000B  rts
  8C1AA638  6EF6  mov.l @r15+,r14
  8C1AA63A  B00A  bsr 8C1AA652
  8C1AA63C  64E3  mov r14,r4
  8C1AA63E  2F02  mov.l r0,@r15
  8C1AA640  B276  bsr 8C1AAB30
  8C1AA642  64E3  mov r14,r4
  8C1AA644  B31A  bsr 8C1AAC7C
  8C1AA646  64E3  mov r14,r4
  8C1AA648  60F2  mov.l @r15,r0
  8C1AA64A  7F04  add ##4,r15
  8C1AA64C  4F26  lds.l @r15+,PR
  8C1AA64E  000B  rts
  8C1AA650  6EF6  mov.l @r15+,r14
```

## Evolution 2 - Far Off Promise (USA) `8C1AAEF6`

Dump: `/mnt/1TB/dcbat/20261007-151251_Evolution_2_-_Far_Off_Promise__USA__/jit-16237.txt`

```
  8C1AAEF6  2FE6  mov.l r14,@-r15
  8C1AAEF8  4F22  sts.l PR,@-r15
  8C1AAEFA  7FFC  add ##-4,r15
  8C1AAEFC  D329  mov.l @([8C1AAFA4]),r3
  8C1AAEFE  6E43  mov r4,r14
  8C1AAF00  430B  jsr @r3
  8C1AAF02  E505  mov ##0x05,r5
  8C1AAF04  2008  tst r0,r0
  8C1AAF06  8903  bt 8C1AAF10
  8C1AAF08  B5BD  bsr 8C1ABA86
  8C1AAF0A  64E3  mov r14,r4
  8C1AAF0C  8801  cmp/eq ##0x01,R0
  8C1AAF0E  8B04  bf 8C1AAF1A
  8C1AAF10  E000  mov ##0x00,r0
  8C1AAF12  7F04  add ##4,r15
  8C1AAF14  4F26  lds.l @r15+,PR
  8C1AAF16  000B  rts
  8C1AAF18  6EF6  mov.l @r15+,r14
  8C1AAF1A  B008  bsr 8C1AAF2E
  8C1AAF1C  64E3  mov r14,r4
  8C1AAF1E  2F02  mov.l r0,@r15
  8C1AAF20  B5DC  bsr 8C1ABADC
  8C1AAF22  64E3  mov r14,r4
  8C1AAF24  60F2  mov.l @r15,r0
  8C1AAF26  7F04  add ##4,r15
  8C1AAF28  4F26  lds.l @r15+,PR
  8C1AAF2A  000B  rts
  8C1AAF2C  6EF6  mov.l @r15+,r14
```

## Grandia II (USA) `8C06C078`

Dump: `/mnt/1TB/dcbat/20261002-192949_Grandia_II__USA__/jit-534412.txt`

```
  8C06C078  2FE6  mov.l r14,@-r15
  8C06C07A  4F22  sts.l PR,@-r15
  8C06C07C  7FFC  add ##-4,r15
  8C06C07E  D33C  mov.l @([8C06C170]),r3
  8C06C080  6E43  mov r4,r14
  8C06C082  430B  jsr @r3
  8C06C084  E506  mov ##0x06,r5
  8C06C086  2008  tst r0,r0
  8C06C088  8903  bt 8C06C092
  8C06C08A  B45E  bsr 8C06C94A
  8C06C08C  64E3  mov r14,r4
  8C06C08E  8801  cmp/eq ##0x01,R0
  8C06C090  8B04  bf 8C06C09C
  ...
  8C06C09C  B00E  bsr 8C06C0BC
  8C06C09E  64E3  mov r14,r4
  8C06C0A0  2F02  mov.l r0,@r15
  8C06C0A2  B2EB  bsr 8C06C67C
  8C06C0A4  64E3  mov r14,r4
  8C06C0A6  B335  bsr 8C06C714
  8C06C0A8  64E3  mov r14,r4
  8C06C0AA  B477  bsr 8C06C99C
  8C06C0AC  64E3  mov r14,r4
  8C06C0AE  B4BE  bsr 8C06CA2E
  8C06C0B0  64E3  mov r14,r4
  8C06C0B2  60F2  mov.l @r15,r0
  8C06C0B4  7F04  add ##4,r15
  8C06C0B6  4F26  lds.l @r15+,PR
  8C06C0B8  000B  rts
  8C06C0BA  6EF6  mov.l @r15+,r14
```

## Grandia II (USA) `8C06D70C`

Dump: `/mnt/1TB/dcbat/20261002-192949_Grandia_II__USA__/jit-534412.txt`

```
  8C06D70C  2FE6  mov.l r14,@-r15
  8C06D70E  4F22  sts.l PR,@-r15
  8C06D710  7FFC  add ##-4,r15
  8C06D712  B335  bsr 8C06DD80
  8C06D714  6E43  mov r4,r14
  8C06D716  8801  cmp/eq ##0x01,R0
  8C06D718  8B04  bf 8C06D724
  ...
  8C06D724  B00C  bsr 8C06D740
  8C06D726  64E3  mov r14,r4
  8C06D728  2F02  mov.l r0,@r15
  8C06D72A  E040  mov ##0x40,r0
  8C06D72C  00EE  mov.l @(R0,r14),r0
  8C06D72E  8802  cmp/eq ##0x02,R0
  8C06D730  8B01  bf 8C06D736
  8C06D732  B370  bsr 8C06DE16
  8C06D734  64E3  mov r14,r4
  8C06D736  60F2  mov.l @r15,r0
  8C06D738  7F04  add ##4,r15
  8C06D73A  4F26  lds.l @r15+,PR
  8C06D73C  000B  rts
  8C06D73E  6EF6  mov.l @r15+,r14
```

## Grandia II (USA) `8C06E4AA`

Dump: `/mnt/1TB/dcbat/20261002-192949_Grandia_II__USA__/jit-534412.txt`

```
  8C06E4AA  2FE6  mov.l r14,@-r15
  8C06E4AC  4F22  sts.l PR,@-r15
  8C06E4AE  7FFC  add ##-4,r15
  8C06E4B0  D33D  mov.l @([8C06E5A8]),r3
  8C06E4B2  6E43  mov r4,r14
  8C06E4B4  430B  jsr @r3
  8C06E4B6  E505  mov ##0x05,r5
  8C06E4B8  2008  tst r0,r0
  8C06E4BA  8903  bt 8C06E4C4
  8C06E4BC  B034  bsr 8C06E528
  8C06E4BE  64E3  mov r14,r4
  8C06E4C0  8801  cmp/eq ##0x01,R0
  8C06E4C2  8B04  bf 8C06E4CE
  ...
  8C06E4CE  B00C  bsr 8C06E4EA
  8C06E4D0  64E3  mov r14,r4
  8C06E4D2  B0ED  bsr 8C06E6B0
  8C06E4D4  64E3  mov r14,r4
  8C06E4D6  2F02  mov.l r0,@r15
  8C06E4D8  B032  bsr 8C06E540
  8C06E4DA  64E3  mov r14,r4
  8C06E4DC  B0A8  bsr 8C06E630
  8C06E4DE  64E3  mov r14,r4
  8C06E4E0  60F2  mov.l @r15,r0
  8C06E4E2  7F04  add ##4,r15
  8C06E4E4  4F26  lds.l @r15+,PR
  8C06E4E6  000B  rts
  8C06E4E8  6EF6  mov.l @r15+,r14
```

## Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) `8C05C342`

Dump: `/mnt/1TB/dcbat/20261007-163759_Le_Mans_24_Hours__Europe___En_Fr_De_Es_I/jit-109461.txt`

```
  8C05C342  D215  mov.l @([8C05C398]),r2
  8C05C344  6613  mov r1,r6
  8C05C346  6413  mov r1,r4
  8C05C348  E000  mov ##0x00,r0
  8C05C34A  32FC  add r15,r2
  8C05C34C  2200  mov.b r0,@r2
  8C05C34E  7604  add ##4,r6
  8C05C350  D712  mov.l @([8C05C39C]),r7
  8C05C352  7414  add ##20,r4
  8C05C354  6362  mov.l @r6,r3
  8C05C356  37FC  add r15,r7
  8C05C358  5061  mov.l @(4,r6),r0
  8C05C35A  7120  add ##32,r1
  8C05C35C  2732  mov.l r3,@r7
  8C05C35E  D510  mov.l @([8C05C3A0]),r5
  8C05C360  1701  mov.l r0,@(4,r7)
  8C05C362  35FC  add r15,r5
  8C05C364  5362  mov.l @(8,r6),r3
  8C05C366  5063  mov.l @(12,r6),r0
  8C05C368  1732  mov.l r3,@(8,r7)
  8C05C36A  1703  mov.l r0,@(12,r7)
  8C05C36C  6342  mov.l @r4,r3
  8C05C36E  5041  mov.l @(4,r4),r0
  8C05C370  2532  mov.l r3,@r5
  8C05C372  1501  mov.l r0,@(4,r5)
  8C05C374  5042  mov.l @(8,r4),r0
  8C05C376  1502  mov.l r0,@(8,r5)
  8C05C378  D20A  mov.l @([8C05C3A4]),r2
  8C05C37A  F018  fmov.s @r1,fr0
  8C05C37C  32FC  add r15,r2
  8C05C37E  F20A  fmov.s fr0,@r2
  8C05C380  5DB3  mov.l @(12,r11),r13
  8C05C382  2DD8  tst r13,r13
  8C05C384  8B01  bf 8C05C38A
  8C05C386  A105  bra 8C05C594
  8C05C388  0009  nop
  8C05C38A  B477  bsr 8C05CC7C
  8C05C38C  64E3  mov r14,r4
  8C05C38E  B4BE  bsr 8C05CD0E
  8C05C390  64E3  mov r14,r4
  8C05C392  60F2  mov.l @r15,r0
  8C05C394  7F04  add ##4,r15
  8C05C396  4F26  lds.l @r15+,PR
  8C05C398  000B  rts
  8C05C39A  6EF6  mov.l @r15+,r14
  ...
  8C05C594  54B5  mov.l @(20,r11),r4
  8C05C596  2448  tst r4,r4
  8C05C598  8903  bt 8C05C5A2
  8C05C59A  D534  mov.l @([8C05C66C]),r5
  8C05C59C  D034  mov.l @([8C05C670]),r0
  8C05C59E  400B  jsr @r0
  8C05C5A0  35FC  add r15,r5
  8C05C5A2  54B6  mov.l @(24,r11),r4
  8C05C5A4  2448  tst r4,r4
  8C05C5A6  8903  bt 8C05C5B0
  8C05C5A8  D530  mov.l @([8C05C66C]),r5
  8C05C5AA  D031  mov.l @([8C05C670]),r0
  8C05C5AC  400B  jsr @r0
  8C05C5AE  35FC  add r15,r5
  8C05C5B0  D130  mov.l @([8C05C674]),r1
  8C05C5B2  F18D  fldi0 fr1
  8C05C5B4  6013  mov r1,r0
  8C05C5B6  7014  add ##20,r0
  8C05C5B8  F008  fmov.s @r0,fr0
  8C05C5BA  E000  mov ##0x00,r0
  8C05C5BC  6303  mov r0,r3
  8C05C5BE  6403  mov r0,r4
  8C05C5C0  F014  fcmp/eq fr1,fr0_SD_F
  8C05C5C2  8F05  bf.s 8C05C5D0
  8C05C5C4  6503  mov r0,r5
  8C05C5C6  7118  add ##24,r1
  8C05C5C8  F018  fmov.s @r1,fr0
  8C05C5CA  F014  fcmp/eq fr1,fr0_SD_F
  8C05C5CC  8B00  bf 8C05C5D0
  8C05C5CE  E501  mov ##0x01,r5
  8C05C5D0  615C  extu.b r5,r1
  8C05C5D2  2118  tst r1,r1
  8C05C5D4  8906  bt 8C05C5E4
  8C05C5D6  D127  mov.l @([8C05C674]),r1
  8C05C5D8  F08D  fldi0 fr0
  8C05C5DA  711C  add ##28,r1
  8C05C5DC  F118  fmov.s @r1,fr1
  8C05C5DE  F104  fcmp/eq fr0,fr1_SD_F
  8C05C5E0  8B00  bf 8C05C5E4
  8C05C5E2  E401  mov ##0x01,r4
  8C05C5E4  614C  extu.b r4,r1
  8C05C5E6  2118  tst r1,r1
  8C05C5E8  8906  bt 8C05C5F8
  8C05C5EA  D122  mov.l @([8C05C674]),r1
  8C05C5EC  F09D  fldi1 fr0
  8C05C5EE  7120  add ##32,r1
  8C05C5F0  F118  fmov.s @r1,fr1
  8C05C5F2  F104  fcmp/eq fr0,fr1_SD_F
  8C05C5F4  8B00  bf 8C05C5F8
  8C05C5F6  E301  mov ##0x01,r3
  8C05C5F8  613C  extu.b r3,r1
  8C05C5FA  2118  tst r1,r1
  8C05C5FC  8927  bt 8C05C64E
  8C05C5FE  D31D  mov.l @([8C05C674]),r3
  8C05C600  E400  mov ##0x00,r4
  8C05C602  F09D  fldi1 fr0
  8C05C604  6133  mov r3,r1
  8C05C606  7104  add ##4,r1
  8C05C608  F118  fmov.s @r1,fr1
  8C05C60A  6543  mov r4,r5
  8C05C60C  F104  fcmp/eq fr0,fr1_SD_F
  8C05C60E  8F06  bf.s 8C05C61E
  8C05C610  6143  mov r4,r1
  8C05C612  7308  add ##8,r3
  8C05C614  F138  fmov.s @r3,fr1
  8C05C616  F08D  fldi0 fr0
  8C05C618  F104  fcmp/eq fr0,fr1_SD_F
  8C05C61A  8B00  bf 8C05C61E
  8C05C61C  E101  mov ##0x01,r1
  8C05C61E  611C  extu.b r1,r1
  8C05C620  2118  tst r1,r1
  8C05C622  8906  bt 8C05C632
  8C05C624  D113  mov.l @([8C05C674]),r1
  8C05C626  F08D  fldi0 fr0
  8C05C628  710C  add ##12,r1
  8C05C62A  F118  fmov.s @r1,fr1
  8C05C62C  F104  fcmp/eq fr0,fr1_SD_F
  8C05C62E  8B00  bf 8C05C632
  8C05C630  E501  mov ##0x01,r5
  8C05C632  615C  extu.b r5,r1
  8C05C634  2118  tst r1,r1
  8C05C636  8906  bt 8C05C646
  8C05C638  D10E  mov.l @([8C05C674]),r1
  8C05C63A  F08D  fldi0 fr0
  8C05C63C  7110  add ##16,r1
  8C05C63E  F118  fmov.s @r1,fr1
  8C05C640  F104  fcmp/eq fr0,fr1_SD_F
  8C05C642  8B00  bf 8C05C646
  8C05C644  E401  mov ##0x01,r4
  8C05C646  614C  extu.b r4,r1
  8C05C648  2118  tst r1,r1
  8C05C64A  8900  bt 8C05C64E
  8C05C64C  E001  mov ##0x01,r0
  8C05C64E  600C  extu.b r0,r0
  8C05C650  2008  tst r0,r0
  8C05C652  8913  bt 8C05C67C
  8C05C654  D008  mov.l @([8C05C678]),r0
  8C05C656  400B  jsr @r0
  8C05C658  54BA  mov.l @(40,r11),r4
  8C05C65A  E000  mov ##0x00,r0
  8C05C65C  A057  bra 8C05C70E
  8C05C65E  1B0A  mov.l r0,@(40,r11)
```

## Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) `8C160394`

Dump: `/mnt/1TB/dcbat_off/20261007-190751_Napple_Tale_-_Arsia_in_Daydream__Japan__/jit-6592.txt`

```
  8C160394  2FE6  mov.l r14,@-r15
  8C160396  4F22  sts.l PR,@-r15
  8C160398  7FFC  add ##-4,r15
  8C16039A  B335  bsr 8C160A08
  8C16039C  6E43  mov r4,r14
  8C16039E  8801  cmp/eq ##0x01,R0
  8C1603A0  8B04  bf 8C1603AC
  8C1603A2  E000  mov ##0x00,r0
  8C1603A4  7F04  add ##4,r15
  8C1603A6  4F26  lds.l @r15+,PR
  8C1603A8  000B  rts
  8C1603AA  6EF6  mov.l @r15+,r14
  8C1603AC  B00C  bsr 8C1603C8
  8C1603AE  64E3  mov r14,r4
  8C1603B0  2F02  mov.l r0,@r15
  8C1603B2  E040  mov ##0x40,r0
  8C1603B4  00EE  mov.l @(R0,r14),r0
  8C1603B6  8802  cmp/eq ##0x02,R0
  8C1603B8  8B01  bf 8C1603BE
  8C1603BA  B370  bsr 8C160A9E
  8C1603BC  64E3  mov r14,r4
  8C1603BE  60F2  mov.l @r15,r0
  8C1603C0  7F04  add ##4,r15
  8C1603C2  4F26  lds.l @r15+,PR
  8C1603C4  000B  rts
  8C1603C6  6EF6  mov.l @r15+,r14
```
