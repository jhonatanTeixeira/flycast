# 028_copia

> Gerado por `tools/sdk_find.py`. Jogos: 2 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 1.58% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C0687D4` | 37 | 0.00% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C031CB6` | 37 | 1.58% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C0687D4`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C0687D4  2FE6  mov.l r14,@-r15
  8C0687D6  2FD6  mov.l r13,@-r15
  8C0687D8  2FC6  mov.l r12,@-r15
  8C0687DA  2FB6  mov.l r11,@-r15
  8C0687DC  2FA6  mov.l r10,@-r15
  8C0687DE  2F96  mov.l r9,@-r15
  8C0687E0  4F22  sts.l PR,@-r15
  8C0687E2  DD31  mov.l @([8C0688A8]),r13
  8C0687E4  4D0B  jsr @r13
  8C0687E6  0009  nop
  8C0687E8  DB31  mov.l @([8C0688B0]),r11
  8C0687EA  DE32  mov.l @([8C0688B4]),r14
  8C0687EC  D92F  mov.l @([8C0688AC]),r9
  8C0687EE  DA2B  mov.l @([8C06889C]),r10
  8C0687F0  A00A  bra 8C068808
  8C0687F2  6C03  mov r0,r12
  8C0687F4  4D0B  jsr @r13
  8C0687F6  0009  nop
  8C0687F8  6503  mov r0,r5
  8C0687FA  490B  jsr @r9
  8C0687FC  64C3  mov r12,r4
  8C0687FE  4B0B  jsr @r11
  8C068800  6403  mov r0,r4
  8C068802  6403  mov r0,r4
  8C068804  34E6  cmp/hi r14,r4
  8C068806  8902  bt 8C06880E
  8C068808  62A2  mov.l @r10,r2
  8C06880A  2228  tst r2,r2
  8C06880C  89F2  bt 8C0687F4
  8C06880E  4F26  lds.l @r15+,PR
  8C068810  69F6  mov.l @r15+,r9
  8C068812  6AF6  mov.l @r15+,r10
  8C068814  6BF6  mov.l @r15+,r11
  8C068816  6CF6  mov.l @r15+,r12
  8C068818  6DF6  mov.l @r15+,r13
  8C06881A  000B  rts
  8C06881C  6EF6  mov.l @r15+,r14
```
