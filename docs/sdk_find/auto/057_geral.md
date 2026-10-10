# 057_geral

> Gerado por `tools/sdk_find.py`. Jogos: 11 · variantes (sequências normalizadas distintas): 2 · tempo perf somado: 0.72% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C07E1DC` | 35 | 0.70% |
| Evolution - The World of Sacred Device (USA) | `8C1B86D0` | 35 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1864C4` | 35 | 0.00% |
| Grandia II (USA) | `8C03D318` | 38 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C34A49C` | 35 | 0.00% |
| Macross M3 | `8C17E624` | 35 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C116A40` | 38 | 0.02% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C387FF8` | 35 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C17CBB4` | 38 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C25C0C4` | 38 | 0.00% |
| Sonic Adventure 2 (USA) (EnJaFrDeEs) | `8C11420C` | 35 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C07E1DC`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C07E1DC  2FE6  mov.l r14,@-r15
  8C07E1DE  DE3A  mov.l @([8C07E2C8]),r14
  8C07E1E0  D33A  mov.l @([8C07E2CC]),r3
  8C07E1E2  D13C  mov.l @([8C07E2D4]),r1
  8C07E1E4  D23A  mov.l @([8C07E2D0]),r2
  8C07E1E6  2149  and r4,r1
  8C07E1E8  2FD6  mov.l r13,@-r15
  8C07E1EA  65E2  mov.l @r14,r5
  8C07E1EC  56E2  mov.l @(8,r14),r6
  8C07E1EE  2539  and r3,r5
  8C07E1F0  D339  mov.l @([8C07E2D8]),r3
  8C07E1F2  251B  or r1,r5
  8C07E1F4  D139  mov.l @([8C07E2DC]),r1
  8C07E1F6  2349  and r4,r3
  8C07E1F8  2629  and r2,r6
  8C07E1FA  6D12  mov.l @r1,r13
  8C07E1FC  263B  or r3,r6
  8C07E1FE  935D  mov.w @([8C07E2BC]),r3
  8C07E200  23D8  tst r13,r3
  8C07E202  8F06  bf.s 8C07E212
  8C07E204  57E1  mov.l @(4,r14),r7
  8C07E206  905A  mov.w @([8C07E2BE]),r0
  8C07E208  2D08  tst r0,r13
  8C07E20A  8905  bt 8C07E218
  ...
  8C07E218  9253  mov.w @([8C07E2C2]),r2
  8C07E21A  2529  and r2,r5
  8C07E21C  D231  mov.l @([8C07E2E4]),r2
  8C07E21E  2E52  mov.l r5,@r14
  8C07E220  1E71  mov.l r7,@(4,r14)
  8C07E222  1E62  mov.l r6,@(8,r14)
  8C07E224  6322  mov.l @r2,r3
  8C07E226  2342  mov.l r4,@r3
  8C07E228  6DF6  mov.l @r15+,r13
  8C07E22A  000B  rts
  8C07E22C  6EF6  mov.l @r15+,r14
```

## Grandia II (USA) `8C03D318`

Dump: `/mnt/1TB/dcbat/20261002-192949_Grandia_II__USA__/jit-534412.txt`

```
  8C03D318  2FE6  mov.l r14,@-r15
  8C03D31A  DE4A  mov.l @([8C03D444]),r14
  8C03D31C  D34A  mov.l @([8C03D448]),r3
  8C03D31E  D14C  mov.l @([8C03D450]),r1
  8C03D320  D24A  mov.l @([8C03D44C]),r2
  8C03D322  2149  and r4,r1
  8C03D324  2FD6  mov.l r13,@-r15
  8C03D326  65E2  mov.l @r14,r5
  8C03D328  56E2  mov.l @(8,r14),r6
  8C03D32A  2539  and r3,r5
  8C03D32C  D349  mov.l @([8C03D454]),r3
  8C03D32E  251B  or r1,r5
  8C03D330  D149  mov.l @([8C03D458]),r1
  8C03D332  2349  and r4,r3
  8C03D334  2629  and r2,r6
  8C03D336  6D12  mov.l @r1,r13
  8C03D338  263B  or r3,r6
  8C03D33A  937D  mov.w @([8C03D438]),r3
  8C03D33C  23D8  tst r13,r3
  8C03D33E  8F06  bf.s 8C03D34E
  8C03D340  57E1  mov.l @(4,r14),r7
  8C03D342  907A  mov.w @([8C03D43A]),r0
  8C03D344  2D08  tst r0,r13
  8C03D346  8905  bt 8C03D354
  ...
  8C03D34E  9175  mov.w @([8C03D43C]),r1
  8C03D350  A002  bra 8C03D358
  8C03D352  251B  or r1,r5
  8C03D354  9273  mov.w @([8C03D43E]),r2
  8C03D356  2529  and r2,r5
  8C03D358  D241  mov.l @([8C03D460]),r2
  8C03D35A  2E52  mov.l r5,@r14
  8C03D35C  1E71  mov.l r7,@(4,r14)
  8C03D35E  1E62  mov.l r6,@(8,r14)
  8C03D360  6322  mov.l @r2,r3
  8C03D362  2342  mov.l r4,@r3
  8C03D364  6DF6  mov.l @r15+,r13
  8C03D366  000B  rts
  8C03D368  6EF6  mov.l @r15+,r14
```
