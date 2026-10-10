# 024_copia

> Gerado por `tools/sdk_find.py`. Jogos: 20 · variantes (sequências normalizadas distintas): 5 · tempo perf somado: 1.75% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C1FBA80` | 25 | 0.00% |
| Dead or Alive 2 (USA) | `8C129E20` | 25 | 0.15% |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C099F80` | 23 | 0.08% |
| Evolution - The World of Sacred Device (USA) | `8C1F1B40` | 23 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1CF880` | 13 | 0.00% |
| Grandia II (USA) | `8C0F91E0` | 23 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C39EE20` | 23 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C022500` | 15 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C22F4C0` | 25 | 0.00% |
| Macross M3 | `8C13607E` | 8 | 0.00% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C17AF00` | 25 | 0.94% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C158E40` | 23 | 0.02% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C3C2EA0` | 23 | 0.00% |
| Power Stone (USA) | `0C0FDDBE` | 8 | 0.00% |
| Project Justice (USA) | `0C2C4D20` | 25 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C1E8BE0` | 23 | 0.00% |
| Shenmue (USA) (Disc 1) | `0C0488E0` | 23 | 0.24% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C1DF1F8` | 8 | 0.06% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C21B480` | 23 | 0.26% |
| Skies of Arcadia (USA) (Disc 1) | `8C2DC7BE` | 8 | 0.00% |
| Sonic Adventure 2 (USA) (EnJaFrDeEs) | `8C14D1DE` | 8 | 0.00% |
| Soulcalibur (USA) | `8C23C020` | 13 | 0.00% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C1FBA80`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C1FBA80  6043  mov r4,r0
  8C1FBA82  205B  or r5,r0
  8C1FBA84  206B  or r6,r0
  8C1FBA86  C807  tst ##7,R0
  8C1FBA88  8B09  bf 8C1FBA9E
  8C1FBA8A  F3FD  fschg
  8C1FBA8C  4609  shlr2 r6
  8C1FBA8E  4601  shlr r6
  8C1FBA90  F059  fmov.s @r5+,fr0
  8C1FBA92  4610  dt r6
  8C1FBA94  F40A  fmov.s fr0,@r4
  8C1FBA96  8FFB  bf.s 8C1FBA90
  8C1FBA98  7408  add ##8,r4
  8C1FBA9A  000B  rts
  8C1FBA9C  F3FD  fschg
  8C1FBA9E  C803  tst ##3,R0
  8C1FBAA0  8B07  bf 8C1FBAB2
  8C1FBAA2  4609  shlr2 r6
  8C1FBAA4  6056  mov.l @r5+,r0
  8C1FBAA6  4610  dt r6
  8C1FBAA8  2402  mov.l r0,@r4
  8C1FBAAA  8FFB  bf.s 8C1FBAA4
  8C1FBAAC  7404  add ##4,r4
  8C1FBAAE  000B  rts
  8C1FBAB0  0009  nop
```

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C099F80`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C099F80  6043  mov r4,r0
  8C099F82  205B  or r5,r0
  8C099F84  206B  or r6,r0
  8C099F86  C807  tst ##7,R0
  8C099F88  8B09  bf 8C099F9E
  8C099F8A  F3FD  fschg
  8C099F8C  4609  shlr2 r6
  8C099F8E  4601  shlr r6
  8C099F90  F059  fmov.s @r5+,fr0
  8C099F92  4610  dt r6
  8C099F94  F40A  fmov.s fr0,@r4
  8C099F96  8FFB  bf.s 8C099F90
  8C099F98  7408  add ##8,r4
  8C099F9A  000B  rts
  8C099F9C  F3FD  fschg
  8C099F9E  4609  shlr2 r6
  8C099FA0  6056  mov.l @r5+,r0
  8C099FA2  4610  dt r6
  8C099FA4  2402  mov.l r0,@r4
  8C099FA6  8FFB  bf.s 8C099FA0
  8C099FA8  7404  add ##4,r4
  8C099FAA  000B  rts
  8C099FAC  0009  nop
```

## Evolution 2 - Far Off Promise (USA) `8C1CF880`

Dump: `/mnt/1TB/dcbat/20261007-151251_Evolution_2_-_Far_Off_Promise__USA__/jit-16237.txt`

```
  8C1CF880  6043  mov r4,r0
  8C1CF882  205B  or r5,r0
  8C1CF884  206B  or r6,r0
  8C1CF886  C807  tst ##7,R0
  8C1CF888  8B09  bf 8C1CF89E
  ...
  8C1CF89E  4609  shlr2 r6
  8C1CF8A0  6056  mov.l @r5+,r0
  8C1CF8A2  4610  dt r6
  8C1CF8A4  2402  mov.l r0,@r4
  8C1CF8A6  8FFB  bf.s 8C1CF8A0
  8C1CF8A8  7404  add ##4,r4
  8C1CF8AA  000B  rts
  8C1CF8AC  0009  nop
```

## Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) `8C022500`

Dump: `/mnt/1TB/dcbat/20261007-163759_Le_Mans_24_Hours__Europe___En_Fr_De_Es_I/jit-109461.txt`

```
  8C022500  6043  mov r4,r0
  8C022502  205B  or r5,r0
  8C022504  206B  or r6,r0
  8C022506  C807  tst ##7,R0
  8C022508  8B09  bf 8C02251E
  ...
  8C02251E  C803  tst ##3,R0
  8C022520  8B07  bf 8C022532
  8C022522  4609  shlr2 r6
  8C022524  6056  mov.l @r5+,r0
  8C022526  4610  dt r6
  8C022528  2402  mov.l r0,@r4
  8C02252A  8FFB  bf.s 8C022524
  8C02252C  7404  add ##4,r4
  8C02252E  000B  rts
  8C022530  0009  nop
```

## Macross M3 `8C13607E`

Dump: `/mnt/1TB/dcbat/20261006-081557_Macross_M3_/jit-5965.txt`

```
  8C13607E  4609  shlr2 r6
  8C136080  6056  mov.l @r5+,r0
  8C136082  4610  dt r6
  8C136084  2402  mov.l r0,@r4
  8C136086  8FFB  bf.s 8C136080
  8C136088  7404  add ##4,r4
  8C13608A  000B  rts
  8C13608C  0009  nop
```
