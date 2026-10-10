# 047_geral

> Gerado por `tools/sdk_find.py`. Jogos: 11 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 0.90% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C07D578` | 41 | 0.88% |
| Evolution - The World of Sacred Device (USA) | `8C1B7A9C` | 41 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C185890` | 41 | 0.00% |
| Grandia II (USA) | `8C03C648` | 41 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C3497CC` | 41 | 0.00% |
| Macross M3 | `8C17D954` | 41 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C115D74` | 41 | 0.02% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C38732C` | 41 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C17BEE4` | 41 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C25B3F8` | 41 | 0.00% |
| Sonic Adventure 2 (USA) (EnJaFrDeEs) | `8C113540` | 41 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C07D578`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C07D578  D542  mov.l @([8C07D684]),r5
  8C07D57A  9081  mov.w @([8C07D680]),r0
  8C07D57C  6352  mov.l @r5,r3
  8C07D57E  D442  mov.l @([8C07D688]),r4
  8C07D580  023E  mov.l @(R0,r3),r2
  8C07D582  7004  add ##4,r0
  8C07D584  D142  mov.l @([8C07D690]),r1
  8C07D586  2422  mov.l r2,@r4
  8C07D588  6352  mov.l @r5,r3
  8C07D58A  023E  mov.l @(R0,r3),r2
  8C07D58C  7004  add ##4,r0
  8C07D58E  1421  mov.l r2,@(4,r4)
  8C07D590  6352  mov.l @r5,r3
  8C07D592  023E  mov.l @(R0,r3),r2
  8C07D594  7004  add ##4,r0
  8C07D596  1422  mov.l r2,@(8,r4)
  8C07D598  6352  mov.l @r5,r3
  8C07D59A  D53C  mov.l @([8C07D68C]),r5
  8C07D59C  023E  mov.l @(R0,r3),r2
  8C07D59E  1423  mov.l r2,@(12,r4)
  8C07D5A0  6352  mov.l @r5,r3
  8C07D5A2  6232  mov.l @r3,r2
  8C07D5A4  1424  mov.l r2,@(16,r4)
  8C07D5A6  6352  mov.l @r5,r3
  8C07D5A8  5231  mov.l @(4,r3),r2
  8C07D5AA  1427  mov.l r2,@(28,r4)
  8C07D5AC  6352  mov.l @r5,r3
  8C07D5AE  5233  mov.l @(12,r3),r2
  8C07D5B0  9367  mov.w @([8C07D682]),r3
  8C07D5B2  1428  mov.l r2,@(32,r4)
  8C07D5B4  6212  mov.l @r1,r2
  8C07D5B6  2238  tst r3,r2
  8C07D5B8  8D06  bt.s 8C07D5C8
  8C07D5BA  6652  mov.l @r5,r6
  ...
  8C07D5C8  5062  mov.l @(8,r6),r0
  8C07D5CA  1405  mov.l r0,@(20,r4)
  8C07D5CC  6252  mov.l @r5,r2
  8C07D5CE  5124  mov.l @(16,r2),r1
  8C07D5D0  1416  mov.l r1,@(24,r4)
  8C07D5D2  000B  rts
  8C07D5D4  0009  nop
```
