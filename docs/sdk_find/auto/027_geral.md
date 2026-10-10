# 027_geral

> Gerado por `tools/sdk_find.py`. Jogos: 3 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 1.67% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C06667C` | 58 | 1.67% |
| Evolution - The World of Sacred Device (USA) | `8C1676F4` | 58 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C17366C` | 58 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C06667C`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C06667C  D34B  mov.l @([8C0667AC]),r3
  8C06667E  6243  mov r4,r2
  8C066680  4400  shll r4
  8C066682  2FE6  mov.l r14,@-r15
  8C066684  342C  add r2,r4
  8C066686  4408  shll2 r4
  8C066688  4F22  sts.l PR,@-r15
  8C06668A  6532  mov.l @r3,r5
  8C06668C  6552  mov.l @r5,r5
  8C06668E  354C  add r4,r5
  8C066690  D447  mov.l @([8C0667B0]),r4
  8C066692  5552  mov.l @(8,r5),r5
  8C066694  6242  mov.l @r4,r2
  8C066696  6152  mov.l @r5,r1
  8C066698  3120  cmp/eq r2,r1
  8C06669A  8B04  bf 8C0666A6
  8C06669C  D145  mov.l @([8C0667B4]),r1
  8C06669E  5251  mov.l @(4,r5),r2
  8C0666A0  6012  mov.l @r1,r0
  8C0666A2  3020  cmp/eq r2,r0
  8C0666A4  8924  bt 8C0666F0
  8C0666A6  6352  mov.l @r5,r3
  8C0666A8  DE43  mov.l @([8C0667B8]),r14
  8C0666AA  2432  mov.l r3,@r4
  8C0666AC  E3C0  mov ##0xC0,r3
  8C0666AE  907A  mov.w @([8C0667A6]),r0
  8C0666B0  D140  mov.l @([8C0667B4]),r1
  8C0666B2  5251  mov.l @(4,r5),r2
  8C0666B4  2122  mov.l r2,@r1
  8C0666B6  64E2  mov.l @r14,r4
  8C0666B8  62E2  mov.l @r14,r2
  8C0666BA  044E  mov.l @(R0,r4),r4
  8C0666BC  5052  mov.l @(8,r5),r0
  8C0666BE  2439  and r3,r4
  8C0666C0  D33F  mov.l @([8C0667C0]),r3
  8C0666C2  C93F  and ##63,R0
  8C0666C4  240B  or r0,r4
  8C0666C6  906E  mov.w @([8C0667A6]),r0
  8C0666C8  0246  mov.l r4,@(R0,r2)
  8C0666CA  D23C  mov.l @([8C0667BC]),r2
  8C0666CC  5453  mov.l @(12,r5),r4
  8C0666CE  2249  and r4,r2
  8C0666D0  3236  cmp/hi r3,r2
  8C0666D2  8B03  bf 8C0666DC
  ...
  8C0666DC  5356  mov.l @(24,r5),r3
  8C0666DE  D139  mov.l @([8C0667C4]),r1
  8C0666E0  4329  shlr16 r3
  8C0666E2  9061  mov.w @([8C0667A8]),r0
  8C0666E4  2132  mov.l r3,@r1
  8C0666E6  62E2  mov.l @r14,r2
  8C0666E8  D337  mov.l @([8C0667C8]),r3
  8C0666EA  0246  mov.l r4,@(R0,r2)
  8C0666EC  430B  jsr @r3
  8C0666EE  64E2  mov.l @r14,r4
  8C0666F0  E001  mov ##0x01,r0
  8C0666F2  4F26  lds.l @r15+,PR
  8C0666F4  000B  rts
  8C0666F6  6EF6  mov.l @r15+,r14
```
