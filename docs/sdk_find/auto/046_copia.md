# 046_copia

> Gerado por `tools/sdk_find.py`. Jogos: 21 · variantes (sequências normalizadas distintas): 3 · tempo perf somado: 0.93% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C0083F8` | 123 | 0.00% |
| Dead or Alive 2 (USA) | `8C0083F8` | 123 | 0.16% |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C0083F8` | 123 | 0.03% |
| Evolution - The World of Sacred Device (USA) | `8C0083F8` | 127 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C0083F8` | 123 | 0.00% |
| Grandia II (USA) | `8C0083F8` | 123 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C0083F8` | 127 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C0083F8` | 123 | 0.00% |
| Macross M3 | `8C0083F8` | 123 | 0.00% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C0083F8` | 123 | 0.15% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C0083F8` | 123 | 0.04% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C008474` | 78 | 0.00% |
| Power Stone (USA) | `8C0083F8` | 123 | 0.00% |
| Project Justice (USA) | `8C0083F8` | 123 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C0083F8` | 123 | 0.00% |
| Shenmue (USA) (Disc 1) | `8C0083F8` | 123 | 0.09% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C0083F8` | 123 | 0.24% |
| Skies of Arcadia (USA) (Disc 1) | `8C0083F8` | 123 | 0.00% |
| Sonic Adventure 2 (USA) (EnJaFrDeEs) | `8C0083F8` | 123 | 0.00% |
| Soulcalibur (USA) | `8C0083F8` | 123 | 0.00% |
| Tomb Raider Chronicles (USA) | `8C0083F8` | 123 | 0.22% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C0083F8`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C0083F8  4F22  sts.l PR,@-r15
  8C0083FA  7FD8  add ##-40,r15
  8C0083FC  D315  mov.l @([8C008454]),r3
  8C0083FE  430B  jsr @r3
  8C008400  0009  nop
  8C008402  0002  stc SR,r0
  8C008404  4009  shlr2 r0
  8C008406  4009  shlr2 r0
  8C008408  C90F  and ##15,R0
  8C00840A  2F02  mov.l r0,@r15
  8C00840C  0002  stc SR,r0
  8C00840E  9320  mov.w @([8C008452]),r3
  8C008410  2039  and r3,r0
  8C008412  CBF0  or ##240,R0
  8C008414  400E  ldc r0,SR
  8C008416  D210  mov.l @([8C008458]),r2
  8C008418  420B  jsr @r2
  8C00841A  0009  nop
  8C00841C  1F09  mov.l r0,@(36,r15)
  8C00841E  60F2  mov.l @r15,r0
  8C008420  C90F  and ##15,R0
  8C008422  4008  shll2 r0
  8C008424  4008  shll2 r0
  8C008426  0302  stc SR,r3
  8C008428  9213  mov.w @([8C008452]),r2
  8C00842A  2329  and r2,r3
  8C00842C  203B  or r3,r0
  8C00842E  400E  ldc r0,SR
  8C008430  50F9  mov.l @(36,r15),r0
  8C008432  8804  cmp/eq ##0x04,R0
  8C008434  8B03  bf 8C00843E
  8C008436  E209  mov ##0x09,r2
  8C008438  1F28  mov.l r2,@(32,r15)
  8C00843A  A011  bra 8C008460
  8C00843C  0009  nop
  ...
  8C008460  0002  stc SR,r0
  8C008462  4009  shlr2 r0
  8C008464  4009  shlr2 r0
  8C008466  C90F  and ##15,R0
  8C008468  2F02  mov.l r0,@r15
  8C00846A  0002  stc SR,r0
  8C00846C  9332  mov.w @([8C0084D4]),r3
  8C00846E  2039  and r3,r0
  8C008470  CBF0  or ##240,R0
  8C008472  400E  ldc r0,SR
  8C008474  54F8  mov.l @(32,r15),r4
  8C008476  D318  mov.l @([8C0084D8]),r3
  8C008478  430B  jsr @r3
  8C00847A  0009  nop
  8C00847C  60F2  mov.l @r15,r0
  8C00847E  C90F  and ##15,R0
  8C008480  4008  shll2 r0
  8C008482  4008  shll2 r0
  8C008484  0202  stc SR,r2
  8C008486  9325  mov.w @([8C0084D4]),r3
  8C008488  2239  and r3,r2
  8C00848A  202B  or r2,r0
  8C00848C  400E  ldc r0,SR
  8C00848E  54F8  mov.l @(32,r15),r4
  8C008490  D312  mov.l @([8C0084DC]),r3
  8C008492  430B  jsr @r3
  8C008494  0009  nop
  8C008496  E401  mov ##0x01,r4
  8C008498  D211  mov.l @([8C0084E0]),r2
  8C00849A  420B  jsr @r2
  8C00849C  0009  nop
  8C00849E  D311  mov.l @([8C0084E4]),r3
  8C0084A0  430B  jsr @r3
  8C0084A2  0009  nop
  8C0084A4  1F07  mov.l r0,@(28,r15)
  8C0084A6  E300  mov ##0x00,r3
  8C0084A8  1F31  mov.l r3,@(4,r15)
  8C0084AA  E200  mov ##0x00,r2
  8C0084AC  1F23  mov.l r2,@(12,r15)
  8C0084AE  D30D  mov.l @([8C0084E4]),r3
  8C0084B0  430B  jsr @r3
  8C0084B2  0009  nop
  8C0084B4  1F06  mov.l r0,@(24,r15)
  8C0084B6  55F6  mov.l @(24,r15),r5
  8C0084B8  54F7  mov.l @(28,r15),r4
  8C0084BA  D30B  mov.l @([8C0084E8]),r3
  8C0084BC  430B  jsr @r3
  8C0084BE  0009  nop
  8C0084C0  1F05  mov.l r0,@(20,r15)
  8C0084C2  54F5  mov.l @(20,r15),r4
  8C0084C4  D309  mov.l @([8C0084EC]),r3
  8C0084C6  430B  jsr @r3
  8C0084C8  0009  nop
  8C0084CA  1F04  mov.l r0,@(16,r15)
  8C0084CC  E300  mov ##0x00,r3
  8C0084CE  1F32  mov.l r3,@(8,r15)
  8C0084D0  A011  bra 8C0084F6
  8C0084D2  0009  nop
  ...
  8C0084F0  51F2  mov.l @(8,r15),r1
  8C0084F2  7101  add ##1,r1
  8C0084F4  1F12  mov.l r1,@(8,r15)
  8C0084F6  931B  mov.w @([8C008530]),r3
  8C0084F8  52F2  mov.l @(8,r15),r2
  8C0084FA  3233  cmp/ge r3,r2
  8C0084FC  8BF8  bf 8C0084F0
  8C0084FE  53F1  mov.l @(4,r15),r3
  8C008500  7301  add ##1,r3
  8C008502  1F31  mov.l r3,@(4,r15)
  8C008504  9215  mov.w @([8C008532]),r2
  8C008506  51F1  mov.l @(4,r15),r1
  8C008508  3123  cmp/ge r2,r1
  8C00850A  8B01  bf 8C008510
  ...
  8C008510  D208  mov.l @([8C008534]),r2
  8C008512  51F4  mov.l @(16,r15),r1
  8C008514  3122  cmp/hs r2,r1
  8C008516  8902  bt 8C00851E
  8C008518  53F3  mov.l @(12,r15),r3
  8C00851A  2338  tst r3,r3
  8C00851C  89C7  bt 8C0084AE
  8C00851E  D206  mov.l @([8C008538]),r2
  8C008520  6322  mov.l @r2,r3
  8C008522  E400  mov ##0x00,r4
  8C008524  430B  jsr @r3
  8C008526  0009  nop
  8C008528  7F28  add ##40,r15
  8C00852A  4F26  lds.l @r15+,PR
  8C00852C  000B  rts
  8C00852E  0009  nop
```

## Evolution - The World of Sacred Device (USA) `8C0083F8`

Dump: `/mnt/1TB/dcbat/20261007-151806_Evolution_-_The_World_of_Sacred_Device__/jit-24035.txt`

```
  8C0083F8  4F22  sts.l PR,@-r15
  8C0083FA  7FD8  add ##-40,r15
  8C0083FC  D315  mov.l @([8C008454]),r3
  8C0083FE  430B  jsr @r3
  8C008400  0009  nop
  8C008402  0002  stc SR,r0
  8C008404  4009  shlr2 r0
  8C008406  4009  shlr2 r0
  8C008408  C90F  and ##15,R0
  8C00840A  2F02  mov.l r0,@r15
  8C00840C  0002  stc SR,r0
  8C00840E  9320  mov.w @([8C008452]),r3
  8C008410  2039  and r3,r0
  8C008412  CBF0  or ##240,R0
  8C008414  400E  ldc r0,SR
  8C008416  D210  mov.l @([8C008458]),r2
  8C008418  420B  jsr @r2
  8C00841A  0009  nop
  8C00841C  1F09  mov.l r0,@(36,r15)
  8C00841E  60F2  mov.l @r15,r0
  8C008420  C90F  and ##15,R0
  8C008422  4008  shll2 r0
  8C008424  4008  shll2 r0
  8C008426  0302  stc SR,r3
  8C008428  9213  mov.w @([8C008452]),r2
  8C00842A  2329  and r2,r3
  8C00842C  203B  or r3,r0
  8C00842E  400E  ldc r0,SR
  8C008430  50F9  mov.l @(36,r15),r0
  8C008432  8804  cmp/eq ##0x04,R0
  8C008434  8B03  bf 8C00843E
  ...
  8C00843E  50F9  mov.l @(36,r15),r0
  8C008440  8801  cmp/eq ##0x01,R0
  8C008442  8902  bt 8C00844A
  8C008444  50F9  mov.l @(36,r15),r0
  8C008446  8803  cmp/eq ##0x03,R0
  8C008448  8B08  bf 8C00845C
  ...
  8C00845C  E106  mov ##0x06,r1
  8C00845E  1F18  mov.l r1,@(32,r15)
  8C008460  0002  stc SR,r0
  8C008462  4009  shlr2 r0
  8C008464  4009  shlr2 r0
  8C008466  C90F  and ##15,R0
  8C008468  2F02  mov.l r0,@r15
  8C00846A  0002  stc SR,r0
  8C00846C  9332  mov.w @([8C0084D4]),r3
  8C00846E  2039  and r3,r0
  8C008470  CBF0  or ##240,R0
  8C008472  400E  ldc r0,SR
  8C008474  54F8  mov.l @(32,r15),r4
  8C008476  D318  mov.l @([8C0084D8]),r3
  8C008478  430B  jsr @r3
  8C00847A  0009  nop
  8C00847C  60F2  mov.l @r15,r0
  8C00847E  C90F  and ##15,R0
  8C008480  4008  shll2 r0
  8C008482  4008  shll2 r0
  8C008484  0202  stc SR,r2
  8C008486  9325  mov.w @([8C0084D4]),r3
  8C008488  2239  and r3,r2
  8C00848A  202B  or r2,r0
  8C00848C  400E  ldc r0,SR
  8C00848E  54F8  mov.l @(32,r15),r4
  8C008490  D312  mov.l @([8C0084DC]),r3
  8C008492  430B  jsr @r3
  8C008494  0009  nop
  8C008496  E401  mov ##0x01,r4
  8C008498  D211  mov.l @([8C0084E0]),r2
  8C00849A  420B  jsr @r2
  8C00849C  0009  nop
  8C00849E  D311  mov.l @([8C0084E4]),r3
  8C0084A0  430B  jsr @r3
  8C0084A2  0009  nop
  8C0084A4  1F07  mov.l r0,@(28,r15)
  8C0084A6  E300  mov ##0x00,r3
  8C0084A8  1F31  mov.l r3,@(4,r15)
  8C0084AA  E200  mov ##0x00,r2
  8C0084AC  1F23  mov.l r2,@(12,r15)
  8C0084AE  D30D  mov.l @([8C0084E4]),r3
  8C0084B0  430B  jsr @r3
  8C0084B2  0009  nop
  8C0084B4  1F06  mov.l r0,@(24,r15)
  8C0084B6  55F6  mov.l @(24,r15),r5
  8C0084B8  54F7  mov.l @(28,r15),r4
  8C0084BA  D30B  mov.l @([8C0084E8]),r3
  8C0084BC  430B  jsr @r3
  8C0084BE  0009  nop
  8C0084C0  1F05  mov.l r0,@(20,r15)
  8C0084C2  54F5  mov.l @(20,r15),r4
  8C0084C4  D309  mov.l @([8C0084EC]),r3
  8C0084C6  430B  jsr @r3
  8C0084C8  0009  nop
  8C0084CA  1F04  mov.l r0,@(16,r15)
  8C0084CC  E300  mov ##0x00,r3
  8C0084CE  1F32  mov.l r3,@(8,r15)
  8C0084D0  A011  bra 8C0084F6
  8C0084D2  0009  nop
  ...
  8C0084F0  51F2  mov.l @(8,r15),r1
  8C0084F2  7101  add ##1,r1
  8C0084F4  1F12  mov.l r1,@(8,r15)
  8C0084F6  931B  mov.w @([8C008530]),r3
  8C0084F8  52F2  mov.l @(8,r15),r2
  8C0084FA  3233  cmp/ge r3,r2
  8C0084FC  8BF8  bf 8C0084F0
  8C0084FE  53F1  mov.l @(4,r15),r3
  8C008500  7301  add ##1,r3
  8C008502  1F31  mov.l r3,@(4,r15)
  8C008504  9215  mov.w @([8C008532]),r2
  8C008506  51F1  mov.l @(4,r15),r1
  8C008508  3123  cmp/ge r2,r1
  8C00850A  8B01  bf 8C008510
  ...
  8C008510  D208  mov.l @([8C008534]),r2
  8C008512  51F4  mov.l @(16,r15),r1
  8C008514  3122  cmp/hs r2,r1
  8C008516  8902  bt 8C00851E
  8C008518  53F3  mov.l @(12,r15),r3
  8C00851A  2338  tst r3,r3
  8C00851C  89C7  bt 8C0084AE
  8C00851E  D206  mov.l @([8C008538]),r2
  8C008520  6322  mov.l @r2,r3
  8C008522  E400  mov ##0x00,r4
  8C008524  430B  jsr @r3
  8C008526  0009  nop
  8C008528  7F28  add ##40,r15
  8C00852A  4F26  lds.l @r15+,PR
  8C00852C  000B  rts
  8C00852E  0009  nop
```

## Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) `8C008474`

Dump: `/mnt/1TB/dcbat/20261007-084025_Phantasy_Star_Online_Ver__2__USA___EnJaF/jit-2303.txt`

```
  8C008474  54F8  mov.l @(32,r15),r4
  8C008476  D318  mov.l @([8C0084D8]),r3
  8C008478  430B  jsr @r3
  8C00847A  0009  nop
  8C00847C  60F2  mov.l @r15,r0
  8C00847E  C90F  and ##15,R0
  8C008480  4008  shll2 r0
  8C008482  4008  shll2 r0
  8C008484  0202  stc SR,r2
  8C008486  9325  mov.w @([8C0084D4]),r3
  8C008488  2239  and r3,r2
  8C00848A  202B  or r2,r0
  8C00848C  400E  ldc r0,SR
  8C00848E  54F8  mov.l @(32,r15),r4
  8C008490  D312  mov.l @([8C0084DC]),r3
  8C008492  430B  jsr @r3
  8C008494  0009  nop
  8C008496  E401  mov ##0x01,r4
  8C008498  D211  mov.l @([8C0084E0]),r2
  8C00849A  420B  jsr @r2
  8C00849C  0009  nop
  8C00849E  D311  mov.l @([8C0084E4]),r3
  8C0084A0  430B  jsr @r3
  8C0084A2  0009  nop
  8C0084A4  1F07  mov.l r0,@(28,r15)
  8C0084A6  E300  mov ##0x00,r3
  8C0084A8  1F31  mov.l r3,@(4,r15)
  8C0084AA  E200  mov ##0x00,r2
  8C0084AC  1F23  mov.l r2,@(12,r15)
  8C0084AE  D30D  mov.l @([8C0084E4]),r3
  8C0084B0  430B  jsr @r3
  8C0084B2  0009  nop
  8C0084B4  1F06  mov.l r0,@(24,r15)
  8C0084B6  55F6  mov.l @(24,r15),r5
  8C0084B8  54F7  mov.l @(28,r15),r4
  8C0084BA  D30B  mov.l @([8C0084E8]),r3
  8C0084BC  430B  jsr @r3
  8C0084BE  0009  nop
  8C0084C0  1F05  mov.l r0,@(20,r15)
  8C0084C2  54F5  mov.l @(20,r15),r4
  8C0084C4  D309  mov.l @([8C0084EC]),r3
  8C0084C6  430B  jsr @r3
  8C0084C8  0009  nop
  8C0084CA  1F04  mov.l r0,@(16,r15)
  8C0084CC  E300  mov ##0x00,r3
  8C0084CE  1F32  mov.l r3,@(8,r15)
  8C0084D0  A011  bra 8C0084F6
  8C0084D2  0009  nop
  ...
  8C0084F0  51F2  mov.l @(8,r15),r1
  8C0084F2  7101  add ##1,r1
  8C0084F4  1F12  mov.l r1,@(8,r15)
  8C0084F6  931B  mov.w @([8C008530]),r3
  8C0084F8  52F2  mov.l @(8,r15),r2
  8C0084FA  3233  cmp/ge r3,r2
  8C0084FC  8BF8  bf 8C0084F0
  8C0084FE  53F1  mov.l @(4,r15),r3
  8C008500  7301  add ##1,r3
  8C008502  1F31  mov.l r3,@(4,r15)
  8C008504  9215  mov.w @([8C008532]),r2
  8C008506  51F1  mov.l @(4,r15),r1
  8C008508  3123  cmp/ge r2,r1
  8C00850A  8B01  bf 8C008510
  ...
  8C008510  D208  mov.l @([8C008534]),r2
  8C008512  51F4  mov.l @(16,r15),r1
  8C008514  3122  cmp/hs r2,r1
  8C008516  8902  bt 8C00851E
  8C008518  53F3  mov.l @(12,r15),r3
  8C00851A  2338  tst r3,r3
  8C00851C  89C7  bt 8C0084AE
  8C00851E  D206  mov.l @([8C008538]),r2
  8C008520  6322  mov.l @r2,r3
  8C008522  E400  mov ##0x00,r4
  8C008524  430B  jsr @r3
  8C008526  0009  nop
  8C008528  7F28  add ##40,r15
  8C00852A  4F26  lds.l @r15+,PR
  8C00852C  000B  rts
  8C00852E  0009  nop
```
