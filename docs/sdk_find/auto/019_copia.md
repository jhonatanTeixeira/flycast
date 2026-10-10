# 019_copia

> Gerado por `tools/sdk_find.py`. Jogos: 5 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 2.15% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C069590` | 119 | 2.15% |
| Evolution - The World of Sacred Device (USA) | `8C19AE30` | 119 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C175008` | 119 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C198E9C` | 119 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C27FC10` | 119 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C069590`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C069590  2FE6  mov.l r14,@-r15
  8C069592  2FD6  mov.l r13,@-r15
  8C069594  2FC6  mov.l r12,@-r15
  8C069596  2FB6  mov.l r11,@-r15
  8C069598  2F96  mov.l r9,@-r15
  8C06959A  2F86  mov.l r8,@-r15
  8C06959C  4F22  sts.l PR,@-r15
  8C06959E  9098  mov.w @([8C0696D2]),r0
  8C0695A0  3F0C  add r0,r15
  8C0695A2  6853  mov r5,r8
  8C0695A4  6B83  mov r8,r11
  8C0695A6  7B01  add ##1,r11
  8C0695A8  9C94  mov.w @([8C0696D4]),r12
  8C0695AA  E500  mov ##0x00,r5
  8C0695AC  9793  mov.w @([8C0696D6]),r7
  8C0695AE  4B21  shar r11
  8C0695B0  6E83  mov r8,r14
  8C0695B2  6653  mov r5,r6
  8C0695B4  36B3  cmp/ge r11,r6
  8C0695B6  6DF3  mov r15,r13
  8C0695B8  7EFF  add ##-1,r14
  8C0695BA  3CFC  add r15,r12
  8C0695BC  8D4A  bt.s 8C069654
  8C0695BE  37FC  add r15,r7
  8C0695C0  6263  mov r6,r2
  8C0695C2  4208  shll2 r2
  8C0695C4  6342  mov.l @r4,r3
  8C0695C6  6153  mov r5,r1
  8C0695C8  4108  shll2 r1
  8C0695CA  4100  shll r1
  8C0695CC  4200  shll r2
  8C0695CE  323C  add r3,r2
  8C0695D0  D343  mov.l @([8C0696E0]),r3
  8C0695D2  31CC  add r12,r1
  8C0695D4  430B  jsr @r3
  8C0695D6  E008  mov ##0x08,r0
  8C0695D8  6163  mov r6,r1
  8C0695DA  5342  mov.l @(8,r4),r3
  8C0695DC  4108  shll2 r1
  8C0695DE  6953  mov r5,r9
  8C0695E0  331C  add r1,r3
  8C0695E2  6231  mov.w @r3,r2
  8C0695E4  4908  shll2 r9
  8C0695E6  39DC  add r13,r9
  8C0695E8  6353  mov r5,r3
  8C0695EA  2921  mov.w r2,@r9
  8C0695EC  6263  mov r6,r2
  8C0695EE  5042  mov.l @(8,r4),r0
  8C0695F0  4208  shll2 r2
  8C0695F2  4308  shll2 r3
  8C0695F4  301C  add r1,r0
  8C0695F6  8501  mov.w @(2,r0),R0
  8C0695F8  7501  add ##1,r5
  8C0695FA  337C  add r7,r3
  8C0695FC  8191  mov.w R0,@(2,r9)
  8C0695FE  7601  add ##1,r6
  8C069600  5141  mov.l @(4,r4),r1
  8C069602  321C  add r1,r2
  8C069604  6153  mov r5,r1
  8C069606  6022  mov.l @r2,r0
  8C069608  4108  shll2 r1
  8C06960A  62E3  mov r14,r2
  8C06960C  4208  shll2 r2
  8C06960E  2302  mov.l r0,@r3
  8C069610  4100  shll r1
  8C069612  6342  mov.l @r4,r3
  8C069614  4200  shll r2
  8C069616  31CC  add r12,r1
  8C069618  323C  add r3,r2
  8C06961A  D331  mov.l @([8C0696E0]),r3
  8C06961C  430B  jsr @r3
  8C06961E  E008  mov ##0x08,r0
  8C069620  69E3  mov r14,r9
  8C069622  5342  mov.l @(8,r4),r3
  8C069624  4908  shll2 r9
  8C069626  6153  mov r5,r1
  8C069628  339C  add r9,r3
  8C06962A  36B3  cmp/ge r11,r6
  8C06962C  4108  shll2 r1
  8C06962E  6231  mov.w @r3,r2
  8C069630  31DC  add r13,r1
  8C069632  6353  mov r5,r3
  8C069634  2121  mov.w r2,@r1
  8C069636  62E3  mov r14,r2
  8C069638  5042  mov.l @(8,r4),r0
  8C06963A  4208  shll2 r2
  8C06963C  4308  shll2 r3
  8C06963E  309C  add r9,r0
  8C069640  8501  mov.w @(2,r0),R0
  8C069642  337C  add r7,r3
  8C069644  7501  add ##1,r5
  8C069646  8111  mov.w R0,@(2,r1)
  8C069648  5141  mov.l @(4,r4),r1
  8C06964A  321C  add r1,r2
  8C06964C  6022  mov.l @r2,r0
  8C06964E  2302  mov.l r0,@r3
  8C069650  8FB6  bf.s 8C0695C0
  8C069652  7EFF  add ##-1,r14
  8C069654  9040  mov.w @([8C0696D8]),r0
  8C069656  6583  mov r8,r5
  8C069658  943E  mov.w @([8C0696D8]),r4
  8C06965A  75FE  add ##-2,r5
  8C06965C  0FC6  mov.l r12,@(R0,r15)
  8C06965E  903C  mov.w @([8C0696DA]),r0
  8C069660  0F76  mov.l r7,@(R0,r15)
  8C069662  903B  mov.w @([8C0696DC]),r0
  8C069664  0FD6  mov.l r13,@(R0,r15)
  8C069666  BE25  bsr 8C0692B4
  8C069668  34FC  add r15,r4
  8C06966A  9138  mov.w @([8C0696DE]),r1
  8C06966C  3F1C  add r1,r15
  8C06966E  4F26  lds.l @r15+,PR
  8C069670  68F6  mov.l @r15+,r8
  8C069672  69F6  mov.l @r15+,r9
  8C069674  6BF6  mov.l @r15+,r11
  8C069676  6CF6  mov.l @r15+,r12
  8C069678  6DF6  mov.l @r15+,r13
  8C06967A  000B  rts
  8C06967C  6EF6  mov.l @r15+,r14
```
