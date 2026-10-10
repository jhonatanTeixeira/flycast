# 020_copia

> Gerado por `tools/sdk_find.py`. Jogos: 5 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 2.03% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C219FCC` | 34 | 0.00% |
| Dead or Alive 2 (USA) | `8C12F68C` | 34 | 0.12% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C23B94C` | 34 | 0.00% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C1A91AC` | 34 | 1.91% |
| Project Justice (USA) | `0C2DB3EC` | 34 | 0.00% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C219FCC`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C219FCC  D52D  mov.l @([8C21A084]),r5
  8C219FCE  E600  mov ##0x00,r6
  8C219FD0  6263  mov r6,r2
  8C219FD2  D72B  mov.l @([8C21A080]),r7
  8C219FD4  5352  mov.l @(8,r5),r3
  8C219FD6  3232  cmp/hs r3,r2
  8C219FD8  8918  bt 8C21A00C
  8C219FDA  6063  mov r6,r0
  8C219FDC  4000  shll r0
  8C219FDE  6363  mov r6,r3
  8C219FE0  303C  add r3,r0
  8C219FE2  5258  mov.l @(32,r5),r2
  8C219FE4  4008  shll2 r0
  8C219FE6  4000  shll r0
  8C219FE8  302C  add r2,r0
  8C219FEA  3040  cmp/eq r4,r0
  8C219FEC  8B0A  bf 8C21A004
  8C219FEE  6363  mov r6,r3
  8C219FF0  4300  shll r3
  8C219FF2  6063  mov r6,r0
  8C219FF4  330C  add r0,r3
  8C219FF6  5258  mov.l @(32,r5),r2
  8C219FF8  4308  shll2 r3
  8C219FFA  4300  shll r3
  8C219FFC  332C  add r2,r3
  8C219FFE  6131  mov.w @r3,r1
  8C21A000  2179  and r7,r1
  8C21A002  2311  mov.w r1,@r3
  8C21A004  5352  mov.l @(8,r5),r3
  8C21A006  7601  add ##1,r6
  8C21A008  3632  cmp/hs r3,r6
  8C21A00A  8BE6  bf 8C219FDA
  8C21A00C  000B  rts
  8C21A00E  0009  nop
```
