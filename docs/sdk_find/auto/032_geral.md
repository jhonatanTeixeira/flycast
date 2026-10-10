# 032_geral

> Gerado por `tools/sdk_find.py`. Jogos: 8 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 1.27% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C06985C` | 50 | 1.27% |
| Evolution - The World of Sacred Device (USA) | `8C19B0FC` | 50 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1752D4` | 50 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C32570C` | 50 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C11DAB4` | 50 | 0.00% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C39BE6C` | 50 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C199168` | 50 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C27FEDC` | 50 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C06985C`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C06985C  2FE6  mov.l r14,@-r15
  8C06985E  2FD6  mov.l r13,@-r15
  8C069860  4F22  sts.l PR,@-r15
  8C069862  7FF4  add ##-12,r15
  8C069864  E008  mov ##0x08,r0
  8C069866  D31A  mov.l @([8C0698D0]),r3
  8C069868  2F42  mov.l r4,@r15
  8C06986A  1F51  mov.l r5,@(4,r15)
  8C06986C  FF47  fmov.s fr4,@(R0,r15)
  8C06986E  430B  jsr @r3
  8C069870  6D63  mov r6,r13
  8C069872  DE18  mov.l @([8C0698D4]),r14
  8C069874  E300  mov ##0x00,r3
  8C069876  D018  mov.l @([8C0698D8]),r0
  8C069878  E440  mov ##0x40,r4
  8C06987A  2ED9  and r13,r14
  8C06987C  3E36  cmp/hi r3,r14
  8C06987E  24D9  and r13,r4
  8C069880  0E29  movt r14
  8C069882  3436  cmp/hi r3,r4
  8C069884  62E3  mov r14,r2
  8C069886  4208  shll2 r2
  8C069888  012E  mov.l @(R0,r2),r1
  8C06988A  E008  mov ##0x08,r0
  8C06988C  0429  movt r4
  8C06988E  410B  jsr @r1
  8C069890  F4F6  fmov.s @(R0,r15),fr4
  8C069892  E320  mov ##0x20,r3
  8C069894  55F1  mov.l @(4,r15),r5
  8C069896  E200  mov ##0x00,r2
  8C069898  2D39  and r3,r13
  8C06989A  3D26  cmp/hi r2,r13
  8C06989C  0029  movt r0
  8C06989E  4000  shll r0
  8C0698A0  3E0C  add r0,r14
  8C0698A2  D00E  mov.l @([8C0698DC]),r0
  8C0698A4  4E08  shll2 r14
  8C0698A6  03EE  mov.l @(R0,r14),r3
  8C0698A8  430B  jsr @r3
  8C0698AA  64F2  mov.l @r15,r4
  8C0698AC  D20D  mov.l @([8C0698E4]),r2
  8C0698AE  D30C  mov.l @([8C0698E0]),r3
  8C0698B0  420B  jsr @r2
  8C0698B2  6432  mov.l @r3,r4
  8C0698B4  7F0C  add ##12,r15
  8C0698B6  4F26  lds.l @r15+,PR
  8C0698B8  D10B  mov.l @([8C0698E8]),r1
  8C0698BA  6DF6  mov.l @r15+,r13
  8C0698BC  412B  jmp @r1
  8C0698BE  6EF6  mov.l @r15+,r14
```
