# 025_geral

> Gerado por `tools/sdk_find.py`. Jogos: 10 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 1.74% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C06192E` | 13 | 1.63% |
| Evolution - The World of Sacred Device (USA) | `8C163F5A` | 13 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C16FE42` | 13 | 0.00% |
| Grandia II (USA) | `8C038236` | 13 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C31B7A6` | 13 | 0.00% |
| Macross M3 | `8C1D39BA` | 13 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C113F4E` | 13 | 0.11% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C37B67A` | 13 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C178806` | 13 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C257662` | 13 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C06192E`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C06192E  E07F  mov ##0x7F,r0
  8C061930  6243  mov r4,r2
  8C061932  2049  and r4,r0
  8C061934  E564  mov ##0x64,r5
  8C061936  0057  mul.l r5,r0
  8C061938  E3F9  mov ##0xF9,r3
  8C06193A  423D  shld r3,r2
  8C06193C  001A  sts MACL,r0
  8C06193E  0257  mul.l r5,r2
  8C061940  403D  shld r3,r0
  8C061942  021A  sts MACL,r2
  8C061944  000B  rts
  8C061946  302C  add r2,r0
```
