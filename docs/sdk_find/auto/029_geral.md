# 029_geral

> Gerado por `tools/sdk_find.py`. Jogos: 7 · variantes (sequências normalizadas distintas): 2 · tempo perf somado: 1.45% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C170B20` | 30 | 0.00% |
| Dead or Alive 2 (USA) | `8C0FDE70` | 30 | 0.30% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C121030` | 30 | 0.31% |
| Power Stone (USA) | `0C0E1960` | 30 | 0.00% |
| Project Justice (USA) | `0C147E90` | 30 | 0.00% |
| Shenmue (USA) (Disc 1) | `0C1D2120` | 30 | 0.33% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C1DFCF0` | 31 | 0.51% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C170B20`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C170B20  D30F  mov.l @([8C170B60]),r3
  8C170B22  4415  cmp/pl r4
  8C170B24  8900  bt 8C170B28
  ...
  8C170B28  6635  mov.w @r3+,r6
  8C170B2A  F3FD  fschg
  8C170B2C  6231  mov.w @r3,r2
  8C170B2E  73FE  add ##-2,r3
  8C170B30  3643  cmp/ge r4,r6
  8C170B32  3648  sub r4,r6
  8C170B34  8900  bt 8C170B38
  ...
  8C170B38  0029  movt r0
  8C170B3A  5731  mov.l @(4,r3),r7
  8C170B3C  6463  mov r6,r4
  8C170B3E  4418  shll8 r4
  8C170B40  3623  cmp/ge r2,r6
  8C170B42  4409  shlr2 r4
  8C170B44  2361  mov.w r6,@r3
  8C170B46  374C  add r4,r7
  8C170B48  8900  bt 8C170B4C
  8C170B4A  1372  mov.l r7,@(8,r3)
  8C170B4C  F179  fmov.s @r7+,fr1
  8C170B4E  F379  fmov.s @r7+,fr3
  8C170B50  F579  fmov.s @r7+,fr5
  8C170B52  F779  fmov.s @r7+,fr7
  8C170B54  F979  fmov.s @r7+,fr9
  8C170B56  FB79  fmov.s @r7+,fr11
  8C170B58  FD79  fmov.s @r7+,fr13
  8C170B5A  FF79  fmov.s @r7+,fr15
  8C170B5C  000B  rts
  8C170B5E  F3FD  fschg
```

## Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) `8C1DFCF0`

Dump: `/mnt/1TB/dcbat_off/20261007-192124_Shenmue_II__Europe___En_Fr_De_Es___Disc_/jit-12387.txt`

```
  8C1DFCF0  D30F  mov.l @([8C1DFD30]),r3
  8C1DFCF2  4415  cmp/pl r4
  8C1DFCF4  8900  bt 8C1DFCF8
  8C1DFCF6  E401  mov ##0x01,r4
  8C1DFCF8  6635  mov.w @r3+,r6
  8C1DFCFA  F3FD  fschg
  8C1DFCFC  6231  mov.w @r3,r2
  8C1DFCFE  73FE  add ##-2,r3
  8C1DFD00  3643  cmp/ge r4,r6
  8C1DFD02  3648  sub r4,r6
  8C1DFD04  8900  bt 8C1DFD08
  ...
  8C1DFD08  0029  movt r0
  8C1DFD0A  5731  mov.l @(4,r3),r7
  8C1DFD0C  6463  mov r6,r4
  8C1DFD0E  4418  shll8 r4
  8C1DFD10  3623  cmp/ge r2,r6
  8C1DFD12  4409  shlr2 r4
  8C1DFD14  2361  mov.w r6,@r3
  8C1DFD16  374C  add r4,r7
  8C1DFD18  8900  bt 8C1DFD1C
  8C1DFD1A  1372  mov.l r7,@(8,r3)
  8C1DFD1C  F179  fmov.s @r7+,fr1
  8C1DFD1E  F379  fmov.s @r7+,fr3
  8C1DFD20  F579  fmov.s @r7+,fr5
  8C1DFD22  F779  fmov.s @r7+,fr7
  8C1DFD24  F979  fmov.s @r7+,fr9
  8C1DFD26  FB79  fmov.s @r7+,fr11
  8C1DFD28  FD79  fmov.s @r7+,fr13
  8C1DFD2A  FF79  fmov.s @r7+,fr15
  8C1DFD2C  000B  rts
  8C1DFD2E  F3FD  fschg
```
