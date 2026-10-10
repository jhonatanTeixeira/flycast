# 034_matriz_produto_escalar_divisao

> Gerado por `tools/sdk_find.py`. Jogos: 5 · variantes (sequências normalizadas distintas): 3 · tempo perf somado: 1.22% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C17C990` | 59 | 0.00% |
| Dead or Alive 2 (USA) | `8C101F00` | 73 | 0.71% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C12ADC0` | 73 | 0.06% |
| Power Stone (USA) | `0C0E7BE0` | 72 | 0.00% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C1D9060` | 59 | 0.45% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C17C990`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C17C990  6042  mov.l @r4,r0
  8C17C992  C801  tst ##1,R0
  8C17C994  8F0E  bf.s 8C17C9B4
  8C17C996  F449  fmov.s @r4+,fr4
  8C17C998  6046  mov.l @r4+,r0
  8C17C99A  304C  add r4,r0
  8C17C99C  F409  fmov.s @r0+,fr4
  8C17C99E  F509  fmov.s @r0+,fr5
  8C17C9A0  F609  fmov.s @r0+,fr6
  8C17C9A2  F79D  fldi1 fr7
  8C17C9A4  F209  fmov.s @r0+,fr2
  8C17C9A6  F009  fmov.s @r0+,fr0
  8C17C9A8  F5FD  ftrv xmtrx,fv4
  8C17C9AA  F109  fmov.s @r0+,fr1
  8C17C9AC  F38D  fldi0 fr3
  8C17C9AE  7420  add ##32,r4
  8C17C9B0  A00A  bra 8C17C9C8
  8C17C9B2  6203  mov r0,r2
  8C17C9B4  F549  fmov.s @r4+,fr5
  8C17C9B6  F649  fmov.s @r4+,fr6
  8C17C9B8  F79D  fldi1 fr7
  8C17C9BA  F249  fmov.s @r4+,fr2
  8C17C9BC  F049  fmov.s @r4+,fr0
  8C17C9BE  F5FD  ftrv xmtrx,fv4
  8C17C9C0  F149  fmov.s @r4+,fr1
  8C17C9C2  F38D  fldi0 fr3
  8C17C9C4  6243  mov r4,r2
  8C17C9C6  7428  add ##40,r4
  8C17C9C8  FB8D  fldi0 fr11
  8C17C9CA  F3ED  fipr fv12,fv0
  8C17C9CC  0483  pref @r4
  8C17C9CE  74E0  add ##-32,r4
  8C17C9D0  F71D  flds fr7,FPUL
  8C17C9D2  005A  sts FPUL,r0
  8C17C9D4  F79D  fldi1 fr7
  8C17C9D6  F743  fdiv fr4,fr7
  8C17C9D8  F40D  fsts FPUL,fr4
  8C17C9DA  F3B5  fcmp/gt fr11,fr3
  8C17C9DC  F8ED  fipr fv12,fv8
  8C17C9DE  F2FC  fmov fr15,fr2
  8C17C9E0  8B1A  bf 8C17CA18
  8C17C9E2  F230  fadd fr3,fr2
  8C17C9E4  2888  tst r8,r8
  8C17C9E6  F38D  fldi0 fr3
  8C17C9E8  8D18  bt.s 8C17CA1C
  8C17C9EA  E101  mov ##0x01,r1
  8C17C9EC  2818  tst r1,r8
  8C17C9EE  8F0E  bf.s 8C17CA0E
  8C17C9F0  FB3D  ftrc fr11, FPUL
  ...
  8C17CA0E  F29D  fldi1 fr2
  8C17CA10  F41D  flds fr4,FPUL
  8C17CA12  005A  sts FPUL,r0
  8C17CA14  000B  rts
  8C17CA16  3C06  cmp/hi r0,r12
  8C17CA18  F41D  flds fr4,FPUL
  8C17CA1A  005A  sts FPUL,r0
  8C17CA1C  F38D  fldi0 fr3
  8C17CA1E  000B  rts
  8C17CA20  3C06  cmp/hi r0,r12
```

## Dead or Alive 2 (USA) `8C101F00`

Dump: `/mnt/1TB/dcbat_off/20261007-192359_Dead_or_Alive_2__USA__/jit-14513.txt`

```
  8C101F00  6042  mov.l @r4,r0
  8C101F02  C801  tst ##1,R0
  8C101F04  8F0E  bf.s 8C101F24
  8C101F06  F449  fmov.s @r4+,fr4
  8C101F08  6046  mov.l @r4+,r0
  8C101F0A  304C  add r4,r0
  8C101F0C  F409  fmov.s @r0+,fr4
  8C101F0E  F509  fmov.s @r0+,fr5
  8C101F10  F609  fmov.s @r0+,fr6
  8C101F12  F79D  fldi1 fr7
  8C101F14  F209  fmov.s @r0+,fr2
  8C101F16  F009  fmov.s @r0+,fr0
  8C101F18  F5FD  ftrv xmtrx,fv4
  8C101F1A  F109  fmov.s @r0+,fr1
  8C101F1C  F38D  fldi0 fr3
  8C101F1E  7420  add ##32,r4
  8C101F20  A00A  bra 8C101F38
  8C101F22  6203  mov r0,r2
  8C101F24  F549  fmov.s @r4+,fr5
  8C101F26  F649  fmov.s @r4+,fr6
  8C101F28  F79D  fldi1 fr7
  8C101F2A  F249  fmov.s @r4+,fr2
  8C101F2C  F049  fmov.s @r4+,fr0
  8C101F2E  F5FD  ftrv xmtrx,fv4
  8C101F30  F149  fmov.s @r4+,fr1
  8C101F32  F38D  fldi0 fr3
  8C101F34  6243  mov r4,r2
  8C101F36  7428  add ##40,r4
  8C101F38  FB8D  fldi0 fr11
  8C101F3A  F3ED  fipr fv12,fv0
  8C101F3C  0483  pref @r4
  8C101F3E  74E0  add ##-32,r4
  8C101F40  F71D  flds fr7,FPUL
  8C101F42  005A  sts FPUL,r0
  8C101F44  F79D  fldi1 fr7
  8C101F46  F743  fdiv fr4,fr7
  8C101F48  F40D  fsts FPUL,fr4
  8C101F4A  F3B5  fcmp/gt fr11,fr3
  8C101F4C  F8ED  fipr fv12,fv8
  8C101F4E  F2FC  fmov fr15,fr2
  8C101F50  8B1A  bf 8C101F88
  8C101F52  F230  fadd fr3,fr2
  8C101F54  2888  tst r8,r8
  8C101F56  F38D  fldi0 fr3
  8C101F58  8D18  bt.s 8C101F8C
  8C101F5A  E101  mov ##0x01,r1
  8C101F5C  2818  tst r1,r8
  8C101F5E  8F0E  bf.s 8C101F7E
  8C101F60  FB3D  ftrc fr11, FPUL
  8C101F62  E17F  mov ##0x7F,r1
  8C101F64  717F  add ##127,r1
  8C101F66  005A  sts FPUL,r0
  8C101F68  7137  add ##55,r1
  8C101F6A  3017  cmp/gt r1,r0
  8C101F6C  4008  shll2 r0
  8C101F6E  8B0B  bf 8C101F88
  8C101F70  008E  mov.l @(R0,r8),r0
  8C101F72  405A  lds r0,FPUL
  8C101F74  F30D  fsts FPUL,fr3
  8C101F76  F41D  flds fr4,FPUL
  8C101F78  005A  sts FPUL,r0
  8C101F7A  000B  rts
  8C101F7C  3C06  cmp/hi r0,r12
  8C101F7E  F29D  fldi1 fr2
  8C101F80  F41D  flds fr4,FPUL
  8C101F82  005A  sts FPUL,r0
  8C101F84  000B  rts
  8C101F86  3C06  cmp/hi r0,r12
  8C101F88  F41D  flds fr4,FPUL
  8C101F8A  005A  sts FPUL,r0
  8C101F8C  F38D  fldi0 fr3
  8C101F8E  000B  rts
  8C101F90  3C06  cmp/hi r0,r12
```

## Power Stone (USA) `0C0E7BE0`

Dump: `/mnt/1TB/dcbat/20261007-160119_Power_Stone__USA__/jit-75898.txt`

```
  0C0E7BE0  6042  mov.l @r4,r0
  0C0E7BE2  C801  tst ##1,R0
  0C0E7BE4  8F0E  bf.s 0C0E7C04
  0C0E7BE6  F449  fmov.s @r4+,fr4
  0C0E7BE8  6046  mov.l @r4+,r0
  0C0E7BEA  304C  add r4,r0
  0C0E7BEC  F409  fmov.s @r0+,fr4
  0C0E7BEE  F509  fmov.s @r0+,fr5
  0C0E7BF0  F609  fmov.s @r0+,fr6
  0C0E7BF2  F79D  fldi1 fr7
  0C0E7BF4  F209  fmov.s @r0+,fr2
  0C0E7BF6  F009  fmov.s @r0+,fr0
  0C0E7BF8  F5FD  ftrv xmtrx,fv4
  0C0E7BFA  F109  fmov.s @r0+,fr1
  0C0E7BFC  F38D  fldi0 fr3
  0C0E7BFE  7420  add ##32,r4
  0C0E7C00  A00A  bra 0C0E7C18
  0C0E7C02  6203  mov r0,r2
  0C0E7C04  F549  fmov.s @r4+,fr5
  0C0E7C06  F649  fmov.s @r4+,fr6
  0C0E7C08  F79D  fldi1 fr7
  0C0E7C0A  F249  fmov.s @r4+,fr2
  0C0E7C0C  F049  fmov.s @r4+,fr0
  0C0E7C0E  F5FD  ftrv xmtrx,fv4
  0C0E7C10  F149  fmov.s @r4+,fr1
  0C0E7C12  F38D  fldi0 fr3
  0C0E7C14  6243  mov r4,r2
  0C0E7C16  7428  add ##40,r4
  0C0E7C18  FB8D  fldi0 fr11
  0C0E7C1A  F3ED  fipr fv12,fv0
  0C0E7C1C  0483  pref @r4
  0C0E7C1E  74E0  add ##-32,r4
  0C0E7C20  F71D  flds fr7,FPUL
  0C0E7C22  005A  sts FPUL,r0
  0C0E7C24  F79D  fldi1 fr7
  0C0E7C26  F743  fdiv fr4,fr7
  0C0E7C28  F40D  fsts FPUL,fr4
  0C0E7C2A  F3B5  fcmp/gt fr11,fr3
  0C0E7C2C  F8ED  fipr fv12,fv8
  0C0E7C2E  F2FC  fmov fr15,fr2
  0C0E7C30  8B19  bf 0C0E7C66
  0C0E7C32  F230  fadd fr3,fr2
  0C0E7C34  2888  tst r8,r8
  0C0E7C36  F38D  fldi0 fr3
  0C0E7C38  8D17  bt.s 0C0E7C6A
  0C0E7C3A  4811  cmp/pz r8
  0C0E7C3C  8F0E  bf.s 0C0E7C5C
  0C0E7C3E  FB3D  ftrc fr11, FPUL
  0C0E7C40  E17F  mov ##0x7F,r1
  0C0E7C42  717F  add ##127,r1
  0C0E7C44  005A  sts FPUL,r0
  0C0E7C46  7137  add ##55,r1
  0C0E7C48  3017  cmp/gt r1,r0
  0C0E7C4A  4008  shll2 r0
  0C0E7C4C  8B0B  bf 0C0E7C66
  0C0E7C4E  008E  mov.l @(R0,r8),r0
  0C0E7C50  405A  lds r0,FPUL
  0C0E7C52  F30D  fsts FPUL,fr3
  0C0E7C54  F41D  flds fr4,FPUL
  0C0E7C56  005A  sts FPUL,r0
  0C0E7C58  000B  rts
  0C0E7C5A  3C06  cmp/hi r0,r12
  0C0E7C5C  F29D  fldi1 fr2
  0C0E7C5E  F41D  flds fr4,FPUL
  0C0E7C60  005A  sts FPUL,r0
  0C0E7C62  000B  rts
  0C0E7C64  3C06  cmp/hi r0,r12
  0C0E7C66  F41D  flds fr4,FPUL
  0C0E7C68  005A  sts FPUL,r0
  0C0E7C6A  F38D  fldi0 fr3
  0C0E7C6C  000B  rts
  0C0E7C6E  3C06  cmp/hi r0,r12
```
