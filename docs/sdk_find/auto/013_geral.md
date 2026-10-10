# 013_geral

> Gerado por `tools/sdk_find.py`. Jogos: 15 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 4.03% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C1F9A46` | 26 | 0.00% |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C0618FA` | 26 | 0.03% |
| Evolution - The World of Sacred Device (USA) | `8C163F26` | 26 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C16FE0E` | 26 | 0.00% |
| Grandia II (USA) | `8C038202` | 26 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C31B772` | 26 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C21E132` | 26 | 0.00% |
| Macross M3 | `8C1D3986` | 26 | 0.00% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C178FBA` | 26 | 2.65% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C113F1A` | 26 | 1.35% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C37B646` | 26 | 0.00% |
| Project Justice (USA) | `0C2C2E62` | 26 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C1787D2` | 26 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C25762E` | 26 | 0.00% |
| Sonic Adventure 2 (USA) (EnJaFrDeEs) | `8C110D36` | 26 | 0.00% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C1F9A46`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C1F9A46  4F22  sts.l PR,@-r15
  8C1F9A48  D33F  mov.l @([8C1F9B48]),r3
  8C1F9A4A  6143  mov r4,r1
  8C1F9A4C  E564  mov ##0x64,r5
  8C1F9A4E  430B  jsr @r3
  8C1F9A50  6053  mov r5,r0
  8C1F9A52  6103  mov r0,r1
  8C1F9A54  4108  shll2 r1
  8C1F9A56  4108  shll2 r1
  8C1F9A58  D33C  mov.l @([8C1F9B4C]),r3
  8C1F9A5A  4108  shll2 r1
  8C1F9A5C  4100  shll r1
  8C1F9A5E  430B  jsr @r3
  8C1F9A60  6053  mov r5,r0
  8C1F9A62  D23A  mov.l @([8C1F9B4C]),r2
  8C1F9A64  6143  mov r4,r1
  8C1F9A66  6303  mov r0,r3
  8C1F9A68  420B  jsr @r2
  8C1F9A6A  6053  mov r5,r0
  8C1F9A6C  4008  shll2 r0
  8C1F9A6E  4F26  lds.l @r15+,PR
  8C1F9A70  4008  shll2 r0
  8C1F9A72  4008  shll2 r0
  8C1F9A74  4000  shll r0
  8C1F9A76  000B  rts
  8C1F9A78  303C  add r3,r0
```
