# 022_copia

> Gerado por `tools/sdk_find.py`. Jogos: 9 · variantes (sequências normalizadas distintas): 5 · tempo perf somado: 1.93% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C073E10` | 67 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C1AC4B4` | 67 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1AC668` | 75 | 0.00% |
| Grandia II (USA) | `8C0704E8` | 69 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C33DB68` | 74 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C0619A4` | 69 | 0.00% |
| Macross M3 | `8C1FDD08` | 74 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C163170` | 78 | 1.93% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C370CEC` | 74 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C073E10`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C073E10  2FE6  mov.l r14,@-r15
  8C073E12  2FD6  mov.l r13,@-r15
  8C073E14  2FB6  mov.l r11,@-r15
  8C073E16  4F22  sts.l PR,@-r15
  8C073E18  7FE8  add ##-24,r15
  8C073E1A  D31E  mov.l @([8C073E94]),r3
  8C073E1C  430B  jsr @r3
  8C073E1E  64F3  mov r15,r4
  8C073E20  9031  mov.w @([8C073E86]),r0
  8C073E22  66F3  mov r15,r6
  8C073E24  DB1C  mov.l @([8C073E98]),r11
  8C073E26  760C  add ##12,r6
  8C073E28  6363  mov r6,r3
  8C073E2A  6263  mov r6,r2
  8C073E2C  7308  add ##8,r3
  8C073E2E  7204  add ##4,r2
  8C073E30  E700  mov ##0x00,r7
  8C073E32  6E73  mov r7,r14
  8C073E34  2672  mov.l r7,@r6
  8C073E36  1F21  mov.l r2,@(4,r15)
  8C073E38  2272  mov.l r7,@r2
  8C073E3A  1F32  mov.l r3,@(8,r15)
  8C073E3C  2372  mov.l r7,@r3
  8C073E3E  0DBE  mov.l @(R0,r11),r13
  8C073E40  7004  add ##4,r0
  8C073E42  04BE  mov.l @(R0,r11),r4
  8C073E44  4D15  cmp/pl r13
  8C073E46  8F18  bf.s 8C073E7A
  8C073E48  6543  mov r4,r5
  8C073E4A  911D  mov.w @([8C073E88]),r1
  8C073E4C  E040  mov ##0x40,r0
  8C073E4E  045E  mov.l @(R0,r5),r4
  8C073E50  2448  tst r4,r4
  8C073E52  8B01  bf 8C073E58
  ...
  8C073E58  6043  mov r4,r0
  8C073E5A  8806  cmp/eq ##0x06,R0
  8C073E5C  8901  bt 8C073E62
  8C073E5E  4411  cmp/pz r4
  8C073E60  8901  bt 8C073E66
  ...
  8C073E66  E401  mov ##0x01,r4
  8C073E68  6043  mov r4,r0
  8C073E6A  4008  shll2 r0
  8C073E6C  036E  mov.l @(R0,r6),r3
  8C073E6E  7E01  add ##1,r14
  8C073E70  3ED3  cmp/ge r13,r14
  8C073E72  7301  add ##1,r3
  8C073E74  0636  mov.l r3,@(R0,r6)
  8C073E76  8FE9  bf.s 8C073E4C
  8C073E78  351C  add r1,r5
  8C073E7A  53F1  mov.l @(4,r15),r3
  8C073E7C  6232  mov.l @r3,r2
  8C073E7E  2228  tst r2,r2
  8C073E80  890C  bt 8C073E9C
  8C073E82  A012  bra 8C073EAA
  8C073E84  EE01  mov ##0x01,r14
  ...
  8C073EAA  9047  mov.w @([8C073F3C]),r0
  8C073EAC  D325  mov.l @([8C073F44]),r3
  8C073EAE  0BE6  mov.l r14,@(R0,r11)
  8C073EB0  430B  jsr @r3
  8C073EB2  64F3  mov r15,r4
  8C073EB4  60E3  mov r14,r0
  8C073EB6  7F18  add ##24,r15
  8C073EB8  4F26  lds.l @r15+,PR
  8C073EBA  6BF6  mov.l @r15+,r11
  8C073EBC  6DF6  mov.l @r15+,r13
  8C073EBE  000B  rts
  8C073EC0  6EF6  mov.l @r15+,r14
```

## Evolution 2 - Far Off Promise (USA) `8C1AC668`

Dump: `/mnt/1TB/dcbat/20261007-151251_Evolution_2_-_Far_Off_Promise__USA__/jit-16237.txt`

```
  8C1AC668  2FE6  mov.l r14,@-r15
  8C1AC66A  2FD6  mov.l r13,@-r15
  8C1AC66C  2FB6  mov.l r11,@-r15
  8C1AC66E  4F22  sts.l PR,@-r15
  8C1AC670  7FE8  add ##-24,r15
  8C1AC672  D316  mov.l @([8C1AC6CC]),r3
  8C1AC674  430B  jsr @r3
  8C1AC676  64F3  mov r15,r4
  8C1AC678  9022  mov.w @([8C1AC6C0]),r0
  8C1AC67A  66F3  mov r15,r6
  8C1AC67C  DB14  mov.l @([8C1AC6D0]),r11
  8C1AC67E  760C  add ##12,r6
  8C1AC680  6363  mov r6,r3
  8C1AC682  6263  mov r6,r2
  8C1AC684  7308  add ##8,r3
  8C1AC686  7204  add ##4,r2
  8C1AC688  E700  mov ##0x00,r7
  8C1AC68A  6E73  mov r7,r14
  8C1AC68C  2672  mov.l r7,@r6
  8C1AC68E  1F21  mov.l r2,@(4,r15)
  8C1AC690  2272  mov.l r7,@r2
  8C1AC692  1F32  mov.l r3,@(8,r15)
  8C1AC694  2372  mov.l r7,@r3
  8C1AC696  0DBE  mov.l @(R0,r11),r13
  8C1AC698  7004  add ##4,r0
  8C1AC69A  04BE  mov.l @(R0,r11),r4
  8C1AC69C  4D15  cmp/pl r13
  8C1AC69E  8F23  bf.s 8C1AC6E8
  8C1AC6A0  6543  mov r4,r5
  8C1AC6A2  910E  mov.w @([8C1AC6C2]),r1
  8C1AC6A4  E040  mov ##0x40,r0
  8C1AC6A6  045E  mov.l @(R0,r5),r4
  8C1AC6A8  2448  tst r4,r4
  8C1AC6AA  8B01  bf 8C1AC6B0
  ...
  8C1AC6B0  6043  mov r4,r0
  8C1AC6B2  8806  cmp/eq ##0x06,R0
  8C1AC6B4  8901  bt 8C1AC6BA
  8C1AC6B6  4411  cmp/pz r4
  8C1AC6B8  890C  bt 8C1AC6D4
  8C1AC6BA  A00C  bra 8C1AC6D6
  8C1AC6BC  E402  mov ##0x02,r4
  ...
  8C1AC6D4  E401  mov ##0x01,r4
  8C1AC6D6  6043  mov r4,r0
  8C1AC6D8  4008  shll2 r0
  8C1AC6DA  036E  mov.l @(R0,r6),r3
  8C1AC6DC  7E01  add ##1,r14
  8C1AC6DE  3ED3  cmp/ge r13,r14
  8C1AC6E0  7301  add ##1,r3
  8C1AC6E2  0636  mov.l r3,@(R0,r6)
  8C1AC6E4  8FDE  bf.s 8C1AC6A4
  8C1AC6E6  351C  add r1,r5
  8C1AC6E8  53F1  mov.l @(4,r15),r3
  8C1AC6EA  6232  mov.l @r3,r2
  8C1AC6EC  2228  tst r2,r2
  8C1AC6EE  8901  bt 8C1AC6F4
  8C1AC6F0  A007  bra 8C1AC702
  8C1AC6F2  EE01  mov ##0x01,r14
  8C1AC6F4  52F2  mov.l @(8,r15),r2
  8C1AC6F6  6322  mov.l @r2,r3
  8C1AC6F8  2338  tst r3,r3
  8C1AC6FA  8901  bt 8C1AC700
  8C1AC6FC  A001  bra 8C1AC702
  8C1AC6FE  EE02  mov ##0x02,r14
  ...
  8C1AC702  9047  mov.w @([8C1AC794]),r0
  8C1AC704  D325  mov.l @([8C1AC79C]),r3
  8C1AC706  0BE6  mov.l r14,@(R0,r11)
  8C1AC708  430B  jsr @r3
  8C1AC70A  64F3  mov r15,r4
  8C1AC70C  60E3  mov r14,r0
  8C1AC70E  7F18  add ##24,r15
  8C1AC710  4F26  lds.l @r15+,PR
  8C1AC712  6BF6  mov.l @r15+,r11
  8C1AC714  6DF6  mov.l @r15+,r13
  8C1AC716  000B  rts
  8C1AC718  6EF6  mov.l @r15+,r14
```

## Grandia II (USA) `8C0704E8`

Dump: `/mnt/1TB/dcbat/20261002-192949_Grandia_II__USA__/jit-534412.txt`

```
  8C0704E8  2FE6  mov.l r14,@-r15
  8C0704EA  2FD6  mov.l r13,@-r15
  8C0704EC  2FB6  mov.l r11,@-r15
  8C0704EE  4F22  sts.l PR,@-r15
  8C0704F0  7FE8  add ##-24,r15
  8C0704F2  D316  mov.l @([8C07054C]),r3
  8C0704F4  430B  jsr @r3
  8C0704F6  64F3  mov r15,r4
  8C0704F8  9022  mov.w @([8C070540]),r0
  8C0704FA  66F3  mov r15,r6
  8C0704FC  DB14  mov.l @([8C070550]),r11
  8C0704FE  760C  add ##12,r6
  8C070500  6363  mov r6,r3
  8C070502  6263  mov r6,r2
  8C070504  7308  add ##8,r3
  8C070506  7204  add ##4,r2
  8C070508  E700  mov ##0x00,r7
  8C07050A  6E73  mov r7,r14
  8C07050C  2672  mov.l r7,@r6
  8C07050E  1F21  mov.l r2,@(4,r15)
  8C070510  2272  mov.l r7,@r2
  8C070512  1F32  mov.l r3,@(8,r15)
  8C070514  2372  mov.l r7,@r3
  8C070516  0DBE  mov.l @(R0,r11),r13
  8C070518  7004  add ##4,r0
  8C07051A  04BE  mov.l @(R0,r11),r4
  8C07051C  4D15  cmp/pl r13
  8C07051E  8F23  bf.s 8C070568
  8C070520  6543  mov r4,r5
  8C070522  910E  mov.w @([8C070542]),r1
  8C070524  E040  mov ##0x40,r0
  8C070526  045E  mov.l @(R0,r5),r4
  8C070528  2448  tst r4,r4
  8C07052A  8B01  bf 8C070530
  8C07052C  A013  bra 8C070556
  8C07052E  E400  mov ##0x00,r4
  8C070530  6043  mov r4,r0
  8C070532  8806  cmp/eq ##0x06,R0
  8C070534  8901  bt 8C07053A
  8C070536  4411  cmp/pz r4
  8C070538  890C  bt 8C070554
  ...
  8C070554  E401  mov ##0x01,r4
  8C070556  6043  mov r4,r0
  8C070558  4008  shll2 r0
  8C07055A  036E  mov.l @(R0,r6),r3
  8C07055C  7E01  add ##1,r14
  8C07055E  3ED3  cmp/ge r13,r14
  8C070560  7301  add ##1,r3
  8C070562  0636  mov.l r3,@(R0,r6)
  8C070564  8FDE  bf.s 8C070524
  8C070566  351C  add r1,r5
  8C070568  53F1  mov.l @(4,r15),r3
  8C07056A  6232  mov.l @r3,r2
  8C07056C  2228  tst r2,r2
  8C07056E  8901  bt 8C070574
  8C070570  A007  bra 8C070582
  8C070572  EE01  mov ##0x01,r14
  ...
  8C070582  9047  mov.w @([8C070614]),r0
  8C070584  D325  mov.l @([8C07061C]),r3
  8C070586  0BE6  mov.l r14,@(R0,r11)
  8C070588  430B  jsr @r3
  8C07058A  64F3  mov r15,r4
  8C07058C  60E3  mov r14,r0
  8C07058E  7F18  add ##24,r15
  8C070590  4F26  lds.l @r15+,PR
  8C070592  6BF6  mov.l @r15+,r11
  8C070594  6DF6  mov.l @r15+,r13
  8C070596  000B  rts
  8C070598  6EF6  mov.l @r15+,r14
```

## King of Fighters The - Evolution (USA) (EnJaEsPt) `8C33DB68`

Dump: `/mnt/1TB/dcbat/20261007-164133_King_of_Fighters_The_-_Evolution__USA___/jit-113430.txt`

```
  8C33DB68  2FE6  mov.l r14,@-r15
  8C33DB6A  2FD6  mov.l r13,@-r15
  8C33DB6C  2FB6  mov.l r11,@-r15
  8C33DB6E  4F22  sts.l PR,@-r15
  8C33DB70  7FE8  add ##-24,r15
  8C33DB72  D316  mov.l @([8C33DBCC]),r3
  8C33DB74  430B  jsr @r3
  8C33DB76  64F3  mov r15,r4
  8C33DB78  9022  mov.w @([8C33DBC0]),r0
  8C33DB7A  66F3  mov r15,r6
  8C33DB7C  DB14  mov.l @([8C33DBD0]),r11
  8C33DB7E  760C  add ##12,r6
  8C33DB80  6363  mov r6,r3
  8C33DB82  6263  mov r6,r2
  8C33DB84  7308  add ##8,r3
  8C33DB86  7204  add ##4,r2
  8C33DB88  E700  mov ##0x00,r7
  8C33DB8A  6E73  mov r7,r14
  8C33DB8C  2672  mov.l r7,@r6
  8C33DB8E  1F21  mov.l r2,@(4,r15)
  8C33DB90  2272  mov.l r7,@r2
  8C33DB92  1F32  mov.l r3,@(8,r15)
  8C33DB94  2372  mov.l r7,@r3
  8C33DB96  0DBE  mov.l @(R0,r11),r13
  8C33DB98  7004  add ##4,r0
  8C33DB9A  04BE  mov.l @(R0,r11),r4
  8C33DB9C  4D15  cmp/pl r13
  8C33DB9E  8F23  bf.s 8C33DBE8
  8C33DBA0  6543  mov r4,r5
  8C33DBA2  910E  mov.w @([8C33DBC2]),r1
  8C33DBA4  E040  mov ##0x40,r0
  8C33DBA6  045E  mov.l @(R0,r5),r4
  8C33DBA8  2448  tst r4,r4
  8C33DBAA  8B01  bf 8C33DBB0
  8C33DBAC  A013  bra 8C33DBD6
  8C33DBAE  E400  mov ##0x00,r4
  8C33DBB0  6043  mov r4,r0
  8C33DBB2  8806  cmp/eq ##0x06,R0
  8C33DBB4  8901  bt 8C33DBBA
  8C33DBB6  4411  cmp/pz r4
  8C33DBB8  890C  bt 8C33DBD4
  ...
  8C33DBD4  E401  mov ##0x01,r4
  8C33DBD6  6043  mov r4,r0
  8C33DBD8  4008  shll2 r0
  8C33DBDA  036E  mov.l @(R0,r6),r3
  8C33DBDC  7E01  add ##1,r14
  8C33DBDE  3ED3  cmp/ge r13,r14
  8C33DBE0  7301  add ##1,r3
  8C33DBE2  0636  mov.l r3,@(R0,r6)
  8C33DBE4  8FDE  bf.s 8C33DBA4
  8C33DBE6  351C  add r1,r5
  8C33DBE8  53F1  mov.l @(4,r15),r3
  8C33DBEA  6232  mov.l @r3,r2
  8C33DBEC  2228  tst r2,r2
  8C33DBEE  8901  bt 8C33DBF4
  8C33DBF0  A007  bra 8C33DC02
  8C33DBF2  EE01  mov ##0x01,r14
  8C33DBF4  52F2  mov.l @(8,r15),r2
  8C33DBF6  6322  mov.l @r2,r3
  8C33DBF8  2338  tst r3,r3
  8C33DBFA  8901  bt 8C33DC00
  ...
  8C33DC00  6E73  mov r7,r14
  8C33DC02  9047  mov.w @([8C33DC94]),r0
  8C33DC04  D325  mov.l @([8C33DC9C]),r3
  8C33DC06  0BE6  mov.l r14,@(R0,r11)
  8C33DC08  430B  jsr @r3
  8C33DC0A  64F3  mov r15,r4
  8C33DC0C  60E3  mov r14,r0
  8C33DC0E  7F18  add ##24,r15
  8C33DC10  4F26  lds.l @r15+,PR
  8C33DC12  6BF6  mov.l @r15+,r11
  8C33DC14  6DF6  mov.l @r15+,r13
  8C33DC16  000B  rts
  8C33DC18  6EF6  mov.l @r15+,r14
```

## Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) `8C163170`

Dump: `/mnt/1TB/dcbat_off/20261007-190751_Napple_Tale_-_Arsia_in_Daydream__Japan__/jit-6592.txt`

```
  8C163170  2FE6  mov.l r14,@-r15
  8C163172  2FD6  mov.l r13,@-r15
  8C163174  2FB6  mov.l r11,@-r15
  8C163176  4F22  sts.l PR,@-r15
  8C163178  7FE8  add ##-24,r15
  8C16317A  D316  mov.l @([8C1631D4]),r3
  8C16317C  430B  jsr @r3
  8C16317E  64F3  mov r15,r4
  8C163180  9022  mov.w @([8C1631C8]),r0
  8C163182  66F3  mov r15,r6
  8C163184  DB14  mov.l @([8C1631D8]),r11
  8C163186  760C  add ##12,r6
  8C163188  6363  mov r6,r3
  8C16318A  6263  mov r6,r2
  8C16318C  7308  add ##8,r3
  8C16318E  7204  add ##4,r2
  8C163190  E700  mov ##0x00,r7
  8C163192  6E73  mov r7,r14
  8C163194  2672  mov.l r7,@r6
  8C163196  1F21  mov.l r2,@(4,r15)
  8C163198  2272  mov.l r7,@r2
  8C16319A  1F32  mov.l r3,@(8,r15)
  8C16319C  2372  mov.l r7,@r3
  8C16319E  0DBE  mov.l @(R0,r11),r13
  8C1631A0  7004  add ##4,r0
  8C1631A2  04BE  mov.l @(R0,r11),r4
  8C1631A4  4D15  cmp/pl r13
  8C1631A6  8F23  bf.s 8C1631F0
  8C1631A8  6543  mov r4,r5
  8C1631AA  910E  mov.w @([8C1631CA]),r1
  8C1631AC  E040  mov ##0x40,r0
  8C1631AE  045E  mov.l @(R0,r5),r4
  8C1631B0  2448  tst r4,r4
  8C1631B2  8B01  bf 8C1631B8
  8C1631B4  A013  bra 8C1631DE
  8C1631B6  E400  mov ##0x00,r4
  8C1631B8  6043  mov r4,r0
  8C1631BA  8806  cmp/eq ##0x06,R0
  8C1631BC  8901  bt 8C1631C2
  8C1631BE  4411  cmp/pz r4
  8C1631C0  890C  bt 8C1631DC
  8C1631C2  A00C  bra 8C1631DE
  8C1631C4  E402  mov ##0x02,r4
  ...
  8C1631DC  E401  mov ##0x01,r4
  8C1631DE  6043  mov r4,r0
  8C1631E0  4008  shll2 r0
  8C1631E2  036E  mov.l @(R0,r6),r3
  8C1631E4  7E01  add ##1,r14
  8C1631E6  3ED3  cmp/ge r13,r14
  8C1631E8  7301  add ##1,r3
  8C1631EA  0636  mov.l r3,@(R0,r6)
  8C1631EC  8FDE  bf.s 8C1631AC
  8C1631EE  351C  add r1,r5
  8C1631F0  53F1  mov.l @(4,r15),r3
  8C1631F2  6232  mov.l @r3,r2
  8C1631F4  2228  tst r2,r2
  8C1631F6  8901  bt 8C1631FC
  8C1631F8  A007  bra 8C16320A
  8C1631FA  EE01  mov ##0x01,r14
  8C1631FC  52F2  mov.l @(8,r15),r2
  8C1631FE  6322  mov.l @r2,r3
  8C163200  2338  tst r3,r3
  8C163202  8901  bt 8C163208
  8C163204  A001  bra 8C16320A
  8C163206  EE02  mov ##0x02,r14
  8C163208  6E73  mov r7,r14
  8C16320A  9047  mov.w @([8C16329C]),r0
  8C16320C  D325  mov.l @([8C1632A4]),r3
  8C16320E  0BE6  mov.l r14,@(R0,r11)
  8C163210  430B  jsr @r3
  8C163212  64F3  mov r15,r4
  8C163214  60E3  mov r14,r0
  8C163216  7F18  add ##24,r15
  8C163218  4F26  lds.l @r15+,PR
  8C16321A  6BF6  mov.l @r15+,r11
  8C16321C  6DF6  mov.l @r15+,r13
  8C16321E  000B  rts
  8C163220  6EF6  mov.l @r15+,r14
```
