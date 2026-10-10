# 035_divisao

> Gerado por `tools/sdk_find.py`. Jogos: 8 · variantes (sequências normalizadas distintas): 7 · tempo perf somado: 1.15% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C068EEC` | 32 | 0.04% |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C068F38` | 32 | 1.11% |
| Evolution - The World of Sacred Device (USA) | `8C19A78C` | 33 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C19A7D8` | 33 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C174964` | 30 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1749B0` | 32 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C3244BC` | 37 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C324D9C` | 38 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C11C864` | 32 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C11D144` | 32 | 0.00% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C39A9B4` | 37 | 0.00% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C39B4FC` | 38 | 0.00% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C39D300` | 31 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C1987F8` | 33 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C198844` | 33 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C27F56C` | 38 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C27F5B8` | 37 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C27FF6C` | 32 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C068EEC`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C068EEC  FFFB  fmov.s fr15,@-r15
  8C068EEE  4F22  sts.l PR,@-r15
  8C068EF0  7FFC  add ##-4,r15
  8C068EF2  D07A  mov.l @([8C0690DC]),r0
  8C068EF4  2F42  mov.l r4,@r15
  8C068EF6  4408  shll2 r4
  8C068EF8  D379  mov.l @([8C0690E0]),r3
  8C068EFA  FF4C  fmov fr4,fr15
  8C068EFC  430B  jsr @r3
  8C068EFE  044E  mov.l @(R0,r4),r4
  8C068F00  62F2  mov.l @r15,r2
  8C068F02  2228  tst r2,r2
  8C068F04  8905  bt 8C068F12
  8C068F06  D377  mov.l @([8C0690E4]),r3
  8C068F08  430B  jsr @r3
  8C068F0A  0009  nop
  8C068F0C  D276  mov.l @([8C0690E8]),r2
  8C068F0E  A005  bra 8C068F1C
  8C068F10  2202  mov.l r0,@r2
  ...
  8C068F1C  F49D  fldi1 fr4
  8C068F1E  F44D  fneg fr4 
  8C068F20  F4F5  fcmp/gt fr15,fr4
  8C068F22  8B02  bf 8C068F2A
  8C068F24  F34C  fmov fr4,fr3
  8C068F26  A001  bra 8C068F2C
  8C068F28  F3F3  fdiv fr15,fr3
  ...
  8C068F2C  D370  mov.l @([8C0690F0]),r3
  8C068F2E  F33A  fmov.s fr3,@r3
  8C068F30  7F04  add ##4,r15
  8C068F32  4F26  lds.l @r15+,PR
  8C068F34  000B  rts
  8C068F36  FFF9  fmov.s @r15+,fr15
```

## Evolution - The World of Sacred Device (USA) `8C19A78C`

Dump: `/mnt/1TB/dcbat/20261007-151806_Evolution_-_The_World_of_Sacred_Device__/jit-24035.txt`

```
  8C19A78C  FFFB  fmov.s fr15,@-r15
  8C19A78E  4F22  sts.l PR,@-r15
  8C19A790  7FFC  add ##-4,r15
  8C19A792  D07A  mov.l @([8C19A97C]),r0
  8C19A794  2F42  mov.l r4,@r15
  8C19A796  4408  shll2 r4
  8C19A798  D379  mov.l @([8C19A980]),r3
  8C19A79A  FF4C  fmov fr4,fr15
  8C19A79C  430B  jsr @r3
  8C19A79E  044E  mov.l @(R0,r4),r4
  8C19A7A0  62F2  mov.l @r15,r2
  8C19A7A2  2228  tst r2,r2
  8C19A7A4  8905  bt 8C19A7B2
  8C19A7A6  D377  mov.l @([8C19A984]),r3
  8C19A7A8  430B  jsr @r3
  8C19A7AA  0009  nop
  8C19A7AC  D276  mov.l @([8C19A988]),r2
  8C19A7AE  A005  bra 8C19A7BC
  8C19A7B0  2202  mov.l r0,@r2
  ...
  8C19A7BC  F49D  fldi1 fr4
  8C19A7BE  F44D  fneg fr4 
  8C19A7C0  F4F5  fcmp/gt fr15,fr4
  8C19A7C2  8B02  bf 8C19A7CA
  8C19A7C4  F34C  fmov fr4,fr3
  8C19A7C6  A001  bra 8C19A7CC
  8C19A7C8  F3F3  fdiv fr15,fr3
  8C19A7CA  F39D  fldi1 fr3
  8C19A7CC  D370  mov.l @([8C19A990]),r3
  8C19A7CE  F33A  fmov.s fr3,@r3
  8C19A7D0  7F04  add ##4,r15
  8C19A7D2  4F26  lds.l @r15+,PR
  8C19A7D4  000B  rts
  8C19A7D6  FFF9  fmov.s @r15+,fr15
```

## Evolution 2 - Far Off Promise (USA) `8C174964`

Dump: `/mnt/1TB/dcbat/20261007-151251_Evolution_2_-_Far_Off_Promise__USA__/jit-16237.txt`

```
  8C174964  FFFB  fmov.s fr15,@-r15
  8C174966  4F22  sts.l PR,@-r15
  8C174968  7FFC  add ##-4,r15
  8C17496A  D07A  mov.l @([8C174B54]),r0
  8C17496C  2F42  mov.l r4,@r15
  8C17496E  4408  shll2 r4
  8C174970  D379  mov.l @([8C174B58]),r3
  8C174972  FF4C  fmov fr4,fr15
  8C174974  430B  jsr @r3
  8C174976  044E  mov.l @(R0,r4),r4
  8C174978  62F2  mov.l @r15,r2
  8C17497A  2228  tst r2,r2
  8C17497C  8905  bt 8C17498A
  8C17497E  D377  mov.l @([8C174B5C]),r3
  8C174980  430B  jsr @r3
  8C174982  0009  nop
  8C174984  D276  mov.l @([8C174B60]),r2
  8C174986  A005  bra 8C174994
  8C174988  2202  mov.l r0,@r2
  ...
  8C174994  F49D  fldi1 fr4
  8C174996  F44D  fneg fr4 
  8C174998  F4F5  fcmp/gt fr15,fr4
  8C17499A  8B02  bf 8C1749A2
  ...
  8C1749A2  F39D  fldi1 fr3
  8C1749A4  D370  mov.l @([8C174B68]),r3
  8C1749A6  F33A  fmov.s fr3,@r3
  8C1749A8  7F04  add ##4,r15
  8C1749AA  4F26  lds.l @r15+,PR
  8C1749AC  000B  rts
  8C1749AE  FFF9  fmov.s @r15+,fr15
```

## King of Fighters The - Evolution (USA) (EnJaEsPt) `8C3244BC`

Dump: `/mnt/1TB/dcbat/20261007-164133_King_of_Fighters_The_-_Evolution__USA___/jit-113430.txt`

```
  8C3244BC  FFFB  fmov.s fr15,@-r15
  8C3244BE  4F22  sts.l PR,@-r15
  8C3244C0  7FFC  add ##-4,r15
  8C3244C2  D07B  mov.l @([8C3246B0]),r0
  8C3244C4  2F42  mov.l r4,@r15
  8C3244C6  4408  shll2 r4
  8C3244C8  D37A  mov.l @([8C3246B4]),r3
  8C3244CA  FF4C  fmov fr4,fr15
  8C3244CC  430B  jsr @r3
  8C3244CE  044E  mov.l @(R0,r4),r4
  8C3244D0  62F2  mov.l @r15,r2
  8C3244D2  2228  tst r2,r2
  8C3244D4  8905  bt 8C3244E2
  8C3244D6  D378  mov.l @([8C3246B8]),r3
  8C3244D8  430B  jsr @r3
  8C3244DA  0009  nop
  8C3244DC  D277  mov.l @([8C3246BC]),r2
  8C3244DE  A005  bra 8C3244EC
  8C3244E0  2202  mov.l r0,@r2
  8C3244E2  D177  mov.l @([8C3246C0]),r1
  8C3244E4  410B  jsr @r1
  8C3244E6  0009  nop
  8C3244E8  D374  mov.l @([8C3246BC]),r3
  8C3244EA  2302  mov.l r0,@r3
  8C3244EC  F49D  fldi1 fr4
  8C3244EE  F44D  fneg fr4 
  8C3244F0  F4F5  fcmp/gt fr15,fr4
  8C3244F2  8B02  bf 8C3244FA
  8C3244F4  F34C  fmov fr4,fr3
  8C3244F6  A001  bra 8C3244FC
  8C3244F8  F3F3  fdiv fr15,fr3
  ...
  8C3244FC  D371  mov.l @([8C3246C4]),r3
  8C3244FE  F33A  fmov.s fr3,@r3
  8C324500  7F04  add ##4,r15
  8C324502  4F26  lds.l @r15+,PR
  8C324504  000B  rts
  8C324506  FFF9  fmov.s @r15+,fr15
```

## King of Fighters The - Evolution (USA) (EnJaEsPt) `8C324D9C`

Dump: `/mnt/1TB/dcbat/20261007-164133_King_of_Fighters_The_-_Evolution__USA___/jit-113430.txt`

```
  8C324D9C  FFFB  fmov.s fr15,@-r15
  8C324D9E  4F22  sts.l PR,@-r15
  8C324DA0  7FFC  add ##-4,r15
  8C324DA2  D07A  mov.l @([8C324F8C]),r0
  8C324DA4  2F42  mov.l r4,@r15
  8C324DA6  4408  shll2 r4
  8C324DA8  D379  mov.l @([8C324F90]),r3
  8C324DAA  FF4C  fmov fr4,fr15
  8C324DAC  430B  jsr @r3
  8C324DAE  044E  mov.l @(R0,r4),r4
  8C324DB0  62F2  mov.l @r15,r2
  8C324DB2  2228  tst r2,r2
  8C324DB4  8905  bt 8C324DC2
  8C324DB6  D377  mov.l @([8C324F94]),r3
  8C324DB8  430B  jsr @r3
  8C324DBA  0009  nop
  8C324DBC  D276  mov.l @([8C324F98]),r2
  8C324DBE  A005  bra 8C324DCC
  8C324DC0  2202  mov.l r0,@r2
  8C324DC2  D176  mov.l @([8C324F9C]),r1
  8C324DC4  410B  jsr @r1
  8C324DC6  0009  nop
  8C324DC8  D373  mov.l @([8C324F98]),r3
  8C324DCA  2302  mov.l r0,@r3
  8C324DCC  F49D  fldi1 fr4
  8C324DCE  F44D  fneg fr4 
  8C324DD0  F4F5  fcmp/gt fr15,fr4
  8C324DD2  8B02  bf 8C324DDA
  8C324DD4  F34C  fmov fr4,fr3
  8C324DD6  A001  bra 8C324DDC
  8C324DD8  F3F3  fdiv fr15,fr3
  8C324DDA  F39D  fldi1 fr3
  8C324DDC  D370  mov.l @([8C324FA0]),r3
  8C324DDE  F33A  fmov.s fr3,@r3
  8C324DE0  7F04  add ##4,r15
  8C324DE2  4F26  lds.l @r15+,PR
  8C324DE4  000B  rts
  8C324DE6  FFF9  fmov.s @r15+,fr15
```

## Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) `8C39D300`

Dump: `/mnt/1TB/dcbat/20261007-084025_Phantasy_Star_Online_Ver__2__USA___EnJaF/jit-2303.txt`

```
  8C39D300  FFFB  fmov.s fr15,@-r15
  8C39D302  4F22  sts.l PR,@-r15
  8C39D304  7FFC  add ##-4,r15
  8C39D306  D024  mov.l @([8C39D398]),r0
  8C39D308  2F42  mov.l r4,@r15
  8C39D30A  4408  shll2 r4
  8C39D30C  D323  mov.l @([8C39D39C]),r3
  8C39D30E  FF4C  fmov fr4,fr15
  8C39D310  430B  jsr @r3
  8C39D312  044E  mov.l @(R0,r4),r4
  8C39D314  62F2  mov.l @r15,r2
  8C39D316  2228  tst r2,r2
  8C39D318  8905  bt 8C39D326
  ...
  8C39D326  D120  mov.l @([8C39D3A8]),r1
  8C39D328  410B  jsr @r1
  8C39D32A  0009  nop
  8C39D32C  D31D  mov.l @([8C39D3A4]),r3
  8C39D32E  2302  mov.l r0,@r3
  8C39D330  F49D  fldi1 fr4
  8C39D332  F44D  fneg fr4 
  8C39D334  F4F5  fcmp/gt fr15,fr4
  8C39D336  8B02  bf 8C39D33E
  8C39D338  F34C  fmov fr4,fr3
  8C39D33A  A001  bra 8C39D340
  8C39D33C  F3F3  fdiv fr15,fr3
  ...
  8C39D340  D31A  mov.l @([8C39D3AC]),r3
  8C39D342  F33A  fmov.s fr3,@r3
  8C39D344  7F04  add ##4,r15
  8C39D346  4F26  lds.l @r15+,PR
  8C39D348  000B  rts
  8C39D34A  FFF9  fmov.s @r15+,fr15
```

## Skies of Arcadia (USA) (Disc 1) `8C27FF6C`

Dump: `/mnt/1TB/dcbat/20261007-162126_Skies_of_Arcadia__USA___Disc_1__/jit-98449.txt`

```
  8C27FF6C  FFFB  fmov.s fr15,@-r15
  8C27FF6E  4F22  sts.l PR,@-r15
  8C27FF70  7FFC  add ##-4,r15
  8C27FF72  D024  mov.l @([8C280004]),r0
  8C27FF74  2F42  mov.l r4,@r15
  8C27FF76  4408  shll2 r4
  8C27FF78  D323  mov.l @([8C280008]),r3
  8C27FF7A  FF4C  fmov fr4,fr15
  8C27FF7C  430B  jsr @r3
  8C27FF7E  044E  mov.l @(R0,r4),r4
  8C27FF80  62F2  mov.l @r15,r2
  8C27FF82  2228  tst r2,r2
  8C27FF84  8905  bt 8C27FF92
  ...
  8C27FF92  D120  mov.l @([8C280014]),r1
  8C27FF94  410B  jsr @r1
  8C27FF96  0009  nop
  8C27FF98  D31D  mov.l @([8C280010]),r3
  8C27FF9A  2302  mov.l r0,@r3
  8C27FF9C  F49D  fldi1 fr4
  8C27FF9E  F44D  fneg fr4 
  8C27FFA0  F4F5  fcmp/gt fr15,fr4
  8C27FFA2  8B02  bf 8C27FFAA
  8C27FFA4  F34C  fmov fr4,fr3
  8C27FFA6  A001  bra 8C27FFAC
  8C27FFA8  F3F3  fdiv fr15,fr3
  8C27FFAA  F39D  fldi1 fr3
  8C27FFAC  D31A  mov.l @([8C280018]),r3
  8C27FFAE  F33A  fmov.s fr3,@r3
  8C27FFB0  7F04  add ##4,r15
  8C27FFB2  4F26  lds.l @r15+,PR
  8C27FFB4  000B  rts
  8C27FFB6  FFF9  fmov.s @r15+,fr15
```
