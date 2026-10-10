# 041_cache

> Gerado por `tools/sdk_find.py`. Jogos: 4 · variantes (sequências normalizadas distintas): 2 · tempo perf somado: 1.03% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C0685DC` | 15 | 0.00% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C02DEA8` | 15 | 1.03% |
| Power Stone (USA) | `0C0100B0` | 14 | 0.00% |
| Project Justice (USA) | `0C010080` | 15 | 0.00% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C0685DC`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C0685DC  E01F  mov ##0x1F,r0
  8C0685DE  6007  not r0,r0
  8C0685E0  2409  and r0,r4
  8C0685E2  6043  mov r4,r0
  8C0685E4  4509  shlr2 r5
  8C0685E6  4509  shlr2 r5
  8C0685E8  4501  shlr r5
  8C0685EA  0093  ocbi @r0
  8C0685EC  7020  add ##32,r0
  8C0685EE  4510  dt r5
  8C0685F0  8BFB  bf 8C0685EA
  8C0685F2  0009  nop
  8C0685F4  00A3  ocbp @r0
  8C0685F6  000B  rts
  8C0685F8  0009  nop
```

## Power Stone (USA) `0C0100B0`

Dump: `/mnt/1TB/dcbat/20261007-160119_Power_Stone__USA__/jit-75898.txt`

```
  0C0100B0  E01F  mov ##0x1F,r0
  0C0100B2  6007  not r0,r0
  0C0100B4  2409  and r0,r4
  0C0100B6  6043  mov r4,r0
  0C0100B8  4509  shlr2 r5
  0C0100BA  4509  shlr2 r5
  0C0100BC  4501  shlr r5
  0C0100BE  00A3  ocbp @r0
  0C0100C0  7020  add ##32,r0
  0C0100C2  4510  dt r5
  0C0100C4  8BFB  bf 0C0100BE
  0C0100C6  0009  nop
  0C0100C8  000B  rts
  0C0100CA  0009  nop
```
