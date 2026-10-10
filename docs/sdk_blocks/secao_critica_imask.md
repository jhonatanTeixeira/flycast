# Seção crítica por máscara de interrupção (IMASK = 15)

> Gerado por `tools/sdk_blocks_doc.py` a partir dos dumps do JIT em `/mnt/1TB` (2026-10-10). Índice: `README.md`. Contexto: `docs/native_sdk_code.md`.

Guarda o IMASK atual, põe IMASK = 15 (`or #0xF0` + `ldc SR`), chama a função protegida e restaura o IMASK. É o "mutex" do Dreamcast (um núcleo). Fica na área de sistema (`8C0083F8`), no mesmo endereço em todos os jogos.

Assinatura (opcodes): `0002 4009 4009 C90F 2F02 0002`

Referência da comparação: **Dead or Alive 2 (USA)**.

| Jogo | Endereço(s) | Iguais à referência |
|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C0083F8`, `8C008456` | 93 de 93 opcodes |
| Dead or Alive 2 (USA) | `8C0083F8`, `8C008456` | referência |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C0083F8`, `8C008456` | 93 de 93 opcodes |
| Evolution - The World of Sacred Device (USA) | `8C0083F8`, `8C008456` | 89 de 89 opcodes |
| Evolution 2 - Far Off Promise (USA) | `8C0083F8`, `8C008456`, `8C17230A` | 93 de 93 opcodes |
| Grandia II (USA) | `8C0083F8`, `8C008456`, `8C03B762` | 93 de 93 opcodes |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C0083F8`, `8C008456`, `8C310CBC`, `8C31DEE2` | 89 de 89 opcodes |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C0083F8`, `8C008456` | 93 de 93 opcodes |
| Macross M3 | `8C0083F8`, `8C008456`, `8C1C5B7A`, `8C1EBE88` | 93 de 93 opcodes |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C0083F8`, `8C008456` | 93 de 93 opcodes |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C0083F8`, `8C008456`, `8C108CAE`, `8C10C514` | 93 de 93 opcodes |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C0083F8`, `8C008456`, `8C3544BC`, `8C38D2E2` | 93 de 93 opcodes |
| Power Stone (USA) | `8C0083F8`, `8C008456` | 93 de 93 opcodes |
| Project Justice (USA) | `8C0083F8`, `8C008456` | 93 de 93 opcodes |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C0083F8`, `8C008456`, `8C173854`, `8C17AFFE` | 93 de 93 opcodes |
| Shenmue (USA) (Disc 1) | `0C1D6E3A`, `0C1D710E`, `0C1D7D96`, `8C0083F8`, `8C008456` | 93 de 93 opcodes |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C0083F8`, `8C008456` | 93 de 93 opcodes |
| Skies of Arcadia (USA) (Disc 1) | `8C0083F8`, `8C008456`, `8C25104C`, `8C25A512` | 93 de 93 opcodes |
| Sonic Adventure 2 (USA) (EnJaFrDeEs) | `8C0083F8`, `8C008456`, `8C0BF31C`, `8C11265A`, `8C12E5F0` | 93 de 93 opcodes |
| Soulcalibur (USA) | `8C0083F8`, `8C008456` | 93 de 93 opcodes |
| Tomb Raider Chronicles (USA) | `8C0083F8`, `8C008456` | 93 de 93 opcodes |
| cvs2 | `0C153D5E`, `0C1542E4` | 14 de 59 opcodes |

"Iguais" conta só os deslocamentos compilados nas duas sessões (o dump guarda o que o jogo executou); o literal pool (constantes) não entra.

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan)

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```


## Dead or Alive 2 (USA)

Dump: `/mnt/1TB/dcbat_off/20261007-192359_Dead_or_Alive_2__USA__/jit-14513.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```


## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!]

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```


## Evolution - The World of Sacred Device (USA)

Dump: `/mnt/1TB/dcbat/20261007-151806_Evolution_-_The_World_of_Sacred_Device__/jit-24035.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
  8C00843E  50F9  mov.l @(36,r15),r0
  8C008440  8801  cmp/eq ##0x01,R0
  8C008442  8902  bt 8C00844A
  8C008444  50F9  mov.l @(36,r15),r0
  8C008446  8803  cmp/eq ##0x03,R0
  8C008448  8B08  bf 8C00845C
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```


## Evolution 2 - Far Off Promise (USA)

Dump: `/mnt/1TB/dcbat/20261007-151251_Evolution_2_-_Far_Off_Promise__USA__/jit-16237.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```

```
; 8C17230A-8C1723F2
  8C17230A  FE77  fmov.s fr7,@(R0,r14)
  8C17230C  E038  mov ##0x38,r0
  8C17230E  FE67  fmov.s fr6,@(R0,r14)
  8C172310  E03C  mov ##0x3C,r0
  8C172312  FE47  fmov.s fr4,@(R0,r14)
  8C172314  0002  stc SR,r0
  8C172316  4009  shlr2 r0
  8C172318  4009  shlr2 r0
  8C17231A  C90F  and ##15,R0
  8C17231C  2F02  mov.l r0,@r15
  8C17231E  0002  stc SR,r0
  8C172320  2039  and r3,r0
  8C172322  CBF0  or ##240,R0
  8C172324  400E  ldc r0,SR
  8C172326  BAA7  bsr 8C171878
  8C172328  0009  nop
  8C17232A  D343  mov.l @([8C172438]),r3
  8C17232C  D441  mov.l @([8C172434]),r4
  8C17232E  430B  jsr @r3
  8C172330  0009  nop
  8C172332  60F2  mov.l @r15,r0
  8C172334  0202  stc SR,r2
  8C172336  937A  mov.w @([8C17242E]),r3
  8C172338  C90F  and ##15,R0
  8C17233A  4008  shll2 r0
  8C17233C  2239  and r3,r2
  8C17233E  4008  shll2 r0
  8C172340  202B  or r2,r0
  8C172342  400E  ldc r0,SR
  8C172344  D33D  mov.l @([8C17243C]),r3
  8C172346  430B  jsr @r3
  8C172348  E400  mov ##0x00,r4
  8C17234A  D23D  mov.l @([8C172440]),r2
  8C17234C  2202  mov.l r0,@r2
  8C17234E  7F08  add ##8,r15
  8C172350  4F26  lds.l @r15+,PR
  8C172352  FFF9  fmov.s @r15+,fr15
  8C172354  68F6  mov.l @r15+,r8
  8C172356  69F6  mov.l @r15+,r9
  8C172358  6AF6  mov.l @r15+,r10
  8C17235A  6BF6  mov.l @r15+,r11
  8C17235C  6CF6  mov.l @r15+,r12
  8C17235E  6DF6  mov.l @r15+,r13
  8C172360  000B  rts
  8C172362  6EF6  mov.l @r15+,r14
  8C172364  4F22  sts.l PR,@-r15
  8C172366  7FFC  add ##-4,r15
  8C172368  6343  mov r4,r3
  8C17236A  4300  shll r3
  8C17236C  6243  mov r4,r2
  8C17236E  332C  add r2,r3
  8C172370  4308  shll2 r3
  8C172372  D134  mov.l @([8C172444]),r1
  8C172374  4308  shll2 r3
  8C172376  D234  mov.l @([8C172448]),r2
  8C172378  4308  shll2 r3
  8C17237A  2F42  mov.l r4,@r15
  8C17237C  331C  add r1,r3
  8C17237E  2232  mov.l r3,@r2
  8C172380  D232  mov.l @([8C17244C]),r2
  8C172382  420B  jsr @r2
  8C172384  6433  mov r3,r4
  8C172386  61F2  mov.l @r15,r1
  8C172388  D331  mov.l @([8C172450]),r3
  8C17238A  D232  mov.l @([8C172454]),r2
  8C17238C  4108  shll2 r1
  8C17238E  313C  add r3,r1
  8C172390  2212  mov.l r1,@r2
  8C172392  7F04  add ##4,r15
  8C172394  4F26  lds.l @r15+,PR
  8C172396  000B  rts
  8C172398  0009  nop
  8C17239A  E500  mov ##0x00,r5
  8C17239C  D32E  mov.l @([8C172458]),r3
  8C17239E  E040  mov ##0x40,r0
  8C1723A0  2432  mov.l r3,@r4
  8C1723A2  E208  mov ##0x08,r2
  8C1723A4  E302  mov ##0x02,r3
  8C1723A6  1451  mov.l r5,@(4,r4)
  8C1723A8  E706  mov ##0x06,r7
  8C1723AA  1452  mov.l r5,@(8,r4)
  8C1723AC  E601  mov ##0x01,r6
  8C1723AE  1463  mov.l r6,@(12,r4)
  8C1723B0  1454  mov.l r5,@(16,r4)
  8C1723B2  1475  mov.l r7,@(20,r4)
  8C1723B4  1466  mov.l r6,@(24,r4)
  8C1723B6  1457  mov.l r5,@(28,r4)
  8C1723B8  1468  mov.l r6,@(32,r4)
  8C1723BA  1459  mov.l r5,@(36,r4)
  8C1723BC  145A  mov.l r5,@(40,r4)
  8C1723BE  142B  mov.l r2,@(44,r4)
  8C1723C0  147C  mov.l r7,@(48,r4)
  8C1723C2  145D  mov.l r5,@(52,r4)
  8C1723C4  145E  mov.l r5,@(56,r4)
  8C1723C6  143F  mov.l r3,@(60,r4)
  8C1723C8  0456  mov.l r5,@(R0,r4)
  8C1723CA  E044  mov ##0x44,r0
  8C1723CC  0456  mov.l r5,@(R0,r4)
  8C1723CE  E048  mov ##0x48,r0
  8C1723D0  0456  mov.l r5,@(R0,r4)
  8C1723D2  E050  mov ##0x50,r0
  8C1723D4  0456  mov.l r5,@(R0,r4)
  8C1723D6  E04C  mov ##0x4C,r0
  8C1723D8  0456  mov.l r5,@(R0,r4)
  8C1723DA  E054  mov ##0x54,r0
  8C1723DC  F49D  fldi1 fr4
  8C1723DE  0456  mov.l r5,@(R0,r4)
  8C1723E0  E058  mov ##0x58,r0
  8C1723E2  0456  mov.l r5,@(R0,r4)
  8C1723E4  E05C  mov ##0x5C,r0
  8C1723E6  E304  mov ##0x04,r3
  8C1723E8  0436  mov.l r3,@(R0,r4)
  8C1723EA  E060  mov ##0x60,r0
  8C1723EC  E203  mov ##0x03,r2
  8C1723EE  0426  mov.l r2,@(R0,r4)
  8C1723F0  E06C  mov ##0x6C,r0
```


## Grandia II (USA)

Dump: `/mnt/1TB/dcbat/20261007-152308_Grandia_II__USA__/jit-32936.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```

```
; 8C03B762-8C03B84A
  8C03B762  FE77  fmov.s fr7,@(R0,r14)
  8C03B764  E038  mov ##0x38,r0
  8C03B766  FE67  fmov.s fr6,@(R0,r14)
  8C03B768  E03C  mov ##0x3C,r0
  8C03B76A  FE47  fmov.s fr4,@(R0,r14)
  8C03B76C  0002  stc SR,r0
  8C03B76E  4009  shlr2 r0
  8C03B770  4009  shlr2 r0
  8C03B772  C90F  and ##15,R0
  8C03B774  2F02  mov.l r0,@r15
  8C03B776  0002  stc SR,r0
  8C03B778  2039  and r3,r0
  8C03B77A  CBF0  or ##240,R0
  8C03B77C  400E  ldc r0,SR
  8C03B77E  B9E9  bsr 8C03AB54
  8C03B780  0009  nop
  8C03B782  D343  mov.l @([8C03B890]),r3
  8C03B784  D441  mov.l @([8C03B88C]),r4
  8C03B786  430B  jsr @r3
  8C03B788  0009  nop
  8C03B78A  60F2  mov.l @r15,r0
  8C03B78C  0202  stc SR,r2
  8C03B78E  937A  mov.w @([8C03B886]),r3
  8C03B790  C90F  and ##15,R0
  8C03B792  4008  shll2 r0
  8C03B794  2239  and r3,r2
  8C03B796  4008  shll2 r0
  8C03B798  202B  or r2,r0
  8C03B79A  400E  ldc r0,SR
  8C03B79C  D33D  mov.l @([8C03B894]),r3
  8C03B79E  430B  jsr @r3
  8C03B7A0  E400  mov ##0x00,r4
  8C03B7A2  D23D  mov.l @([8C03B898]),r2
  8C03B7A4  2202  mov.l r0,@r2
  8C03B7A6  7F08  add ##8,r15
  8C03B7A8  4F26  lds.l @r15+,PR
  8C03B7AA  FFF9  fmov.s @r15+,fr15
  8C03B7AC  68F6  mov.l @r15+,r8
  8C03B7AE  69F6  mov.l @r15+,r9
  8C03B7B0  6AF6  mov.l @r15+,r10
  8C03B7B2  6BF6  mov.l @r15+,r11
  8C03B7B4  6CF6  mov.l @r15+,r12
  8C03B7B6  6DF6  mov.l @r15+,r13
  8C03B7B8  000B  rts
  8C03B7BA  6EF6  mov.l @r15+,r14
  8C03B7BC  4F22  sts.l PR,@-r15
  8C03B7BE  7FFC  add ##-4,r15
  8C03B7C0  6343  mov r4,r3
  8C03B7C2  4300  shll r3
  8C03B7C4  6243  mov r4,r2
  8C03B7C6  332C  add r2,r3
  8C03B7C8  4308  shll2 r3
  8C03B7CA  D134  mov.l @([8C03B89C]),r1
  8C03B7CC  4308  shll2 r3
  8C03B7CE  D234  mov.l @([8C03B8A0]),r2
  8C03B7D0  4308  shll2 r3
  8C03B7D2  2F42  mov.l r4,@r15
  8C03B7D4  331C  add r1,r3
  8C03B7D6  2232  mov.l r3,@r2
  8C03B7D8  D232  mov.l @([8C03B8A4]),r2
  8C03B7DA  420B  jsr @r2
  8C03B7DC  6433  mov r3,r4
  8C03B7DE  61F2  mov.l @r15,r1
  8C03B7E0  D331  mov.l @([8C03B8A8]),r3
  8C03B7E2  D232  mov.l @([8C03B8AC]),r2
  8C03B7E4  4108  shll2 r1
  8C03B7E6  313C  add r3,r1
  8C03B7E8  2212  mov.l r1,@r2
  8C03B7EA  7F04  add ##4,r15
  8C03B7EC  4F26  lds.l @r15+,PR
  8C03B7EE  000B  rts
  8C03B7F0  0009  nop
  8C03B7F2  E500  mov ##0x00,r5
  8C03B7F4  D32E  mov.l @([8C03B8B0]),r3
  8C03B7F6  E040  mov ##0x40,r0
  8C03B7F8  2432  mov.l r3,@r4
  8C03B7FA  E208  mov ##0x08,r2
  8C03B7FC  E302  mov ##0x02,r3
  8C03B7FE  1451  mov.l r5,@(4,r4)
  8C03B800  E706  mov ##0x06,r7
  8C03B802  1452  mov.l r5,@(8,r4)
  8C03B804  E601  mov ##0x01,r6
  8C03B806  1463  mov.l r6,@(12,r4)
  8C03B808  1454  mov.l r5,@(16,r4)
  8C03B80A  1475  mov.l r7,@(20,r4)
  8C03B80C  1466  mov.l r6,@(24,r4)
  8C03B80E  1457  mov.l r5,@(28,r4)
  8C03B810  1468  mov.l r6,@(32,r4)
  8C03B812  1459  mov.l r5,@(36,r4)
  8C03B814  145A  mov.l r5,@(40,r4)
  8C03B816  142B  mov.l r2,@(44,r4)
  8C03B818  147C  mov.l r7,@(48,r4)
  8C03B81A  145D  mov.l r5,@(52,r4)
  8C03B81C  145E  mov.l r5,@(56,r4)
  8C03B81E  143F  mov.l r3,@(60,r4)
  8C03B820  0456  mov.l r5,@(R0,r4)
  8C03B822  E044  mov ##0x44,r0
  8C03B824  0456  mov.l r5,@(R0,r4)
  8C03B826  E048  mov ##0x48,r0
  8C03B828  0456  mov.l r5,@(R0,r4)
  8C03B82A  E050  mov ##0x50,r0
  8C03B82C  0456  mov.l r5,@(R0,r4)
  8C03B82E  E04C  mov ##0x4C,r0
  8C03B830  0456  mov.l r5,@(R0,r4)
  8C03B832  E054  mov ##0x54,r0
  8C03B834  F49D  fldi1 fr4
  8C03B836  0456  mov.l r5,@(R0,r4)
  8C03B838  E058  mov ##0x58,r0
  8C03B83A  0456  mov.l r5,@(R0,r4)
  8C03B83C  E05C  mov ##0x5C,r0
  8C03B83E  E304  mov ##0x04,r3
  8C03B840  0436  mov.l r3,@(R0,r4)
  8C03B842  E060  mov ##0x60,r0
  8C03B844  E203  mov ##0x03,r2
  8C03B846  0426  mov.l r2,@(R0,r4)
  8C03B848  E06C  mov ##0x6C,r0
```


## King of Fighters The - Evolution (USA) (EnJaEsPt)

Dump: `/mnt/1TB/dcbat/20261007-164133_King_of_Fighters_The_-_Evolution__USA___/jit-113430.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
  8C00843E  50F9  mov.l @(36,r15),r0
  8C008440  8801  cmp/eq ##0x01,R0
  8C008442  8902  bt 8C00844A
  8C008444  50F9  mov.l @(36,r15),r0
  8C008446  8803  cmp/eq ##0x03,R0
  8C008448  8B08  bf 8C00845C
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```

```
; 8C310CBC-8C310DA4
  8C310CBC  9247  mov.w @([8C310D4E]),r2
  8C310CBE  6532  mov.l @r3,r5
  8C310CC0  EC00  mov ##0x00,r12
  8C310CC2  6EC3  mov r12,r14
  8C310CC4  045E  mov.l @(R0,r5),r4
  8C310CC6  0002  stc SR,r0
  8C310CC8  4009  shlr2 r0
  8C310CCA  4009  shlr2 r0
  8C310CCC  C90F  and ##15,R0
  8C310CCE  2F02  mov.l r0,@r15
  8C310CD0  0002  stc SR,r0
  8C310CD2  2029  and r2,r0
  8C310CD4  CBF0  or ##240,R0
  8C310CD6  400E  ldc r0,SR
  8C310CD8  2558  tst r5,r5
  8C310CDA  8B02  bf 8C310CE2
  ...       (não compilado nesta sessão)
  8C310CE2  D51D  mov.l @([8C310D58]),r5
  8C310CE4  E740  mov ##0x40,r7
  8C310CE6  66C3  mov r12,r6
  8C310CE8  6242  mov.l @r4,r2
  8C310CEA  3250  cmp/eq r5,r2
  8C310CEC  8B06  bf 8C310CFC
  8C310CEE  7601  add ##1,r6
  8C310CF0  3673  cmp/ge r7,r6
  8C310CF2  8FF9  bf.s 8C310CE8
  8C310CF4  7440  add ##64,r4
  ...       (não compilado nesta sessão)
  8C310CFC  D318  mov.l @([8C310D60]),r3
  8C310CFE  2452  mov.l r5,@r4
  8C310D00  2D42  mov.l r4,@r13
  8C310D02  430B  jsr @r3
  8C310D04  E500  mov ##0x00,r5
  8C310D06  E700  mov ##0x00,r7
  8C310D08  D216  mov.l @([8C310D64]),r2
  8C310D0A  6573  mov r7,r5
  8C310D0C  2FC6  mov.l r12,@-r15
  8C310D0E  6673  mov r7,r6
  8C310D10  420B  jsr @r2
  8C310D12  64D2  mov.l @r13,r4
  8C310D14  7F04  add ##4,r15
  8C310D16  D20E  mov.l @([8C310D50]),r2
  8C310D18  E054  mov ##0x54,r0
  8C310D1A  6322  mov.l @r2,r3
  8C310D1C  013E  mov.l @(R0,r3),r1
  8C310D1E  7101  add ##1,r1
  8C310D20  0316  mov.l r1,@(R0,r3)
  8C310D22  60F2  mov.l @r15,r0
  8C310D24  0302  stc SR,r3
  8C310D26  9212  mov.w @([8C310D4E]),r2
  8C310D28  C90F  and ##15,R0
  8C310D2A  4008  shll2 r0
  8C310D2C  2329  and r2,r3
  8C310D2E  4008  shll2 r0
  8C310D30  203B  or r3,r0
  8C310D32  400E  ldc r0,SR
  8C310D34  2EE8  tst r14,r14
  8C310D36  8903  bt 8C310D40
  ...       (não compilado nesta sessão)
  8C310D40  60E3  mov r14,r0
  8C310D42  7F04  add ##4,r15
  8C310D44  4F26  lds.l @r15+,PR
  8C310D46  6CF6  mov.l @r15+,r12
  8C310D48  6DF6  mov.l @r15+,r13
  8C310D4A  000B  rts
  8C310D4C  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C310D70  2FE6  mov.l r14,@-r15
  8C310D72  2FD6  mov.l r13,@-r15
  8C310D74  2FC6  mov.l r12,@-r15
  8C310D76  2FB6  mov.l r11,@-r15
  8C310D78  2FA6  mov.l r10,@-r15
  8C310D7A  4F22  sts.l PR,@-r15
  8C310D7C  7FF8  add ##-8,r15
  8C310D7E  0002  stc SR,r0
  8C310D80  9357  mov.w @([8C310E32]),r3
  8C310D82  6A43  mov r4,r10
  8C310D84  6D43  mov r4,r13
  8C310D86  4009  shlr2 r0
  8C310D88  4009  shlr2 r0
  8C310D8A  C90F  and ##15,R0
  8C310D8C  1F01  mov.l r0,@(4,r15)
  8C310D8E  0002  stc SR,r0
  8C310D90  2039  and r3,r0
  8C310D92  CBF0  or ##240,R0
  8C310D94  400E  ldc r0,SR
  8C310D96  D228  mov.l @([8C310E38]),r2
  8C310D98  420B  jsr @r2
  8C310D9A  64A3  mov r10,r4
  8C310D9C  6E03  mov r0,r14
  8C310D9E  2EE8  tst r14,r14
  8C310DA0  8B2F  bf 8C310E02
  8C310DA2  DC26  mov.l @([8C310E3C]),r12
```

```
; 8C31DEE2-8C31DFCA
  8C31DEE2  FE77  fmov.s fr7,@(R0,r14)
  8C31DEE4  E038  mov ##0x38,r0
  8C31DEE6  FE67  fmov.s fr6,@(R0,r14)
  8C31DEE8  E03C  mov ##0x3C,r0
  8C31DEEA  FE47  fmov.s fr4,@(R0,r14)
  8C31DEEC  0002  stc SR,r0
  8C31DEEE  4009  shlr2 r0
  8C31DEF0  4009  shlr2 r0
  8C31DEF2  C90F  and ##15,R0
  8C31DEF4  2F02  mov.l r0,@r15
  8C31DEF6  0002  stc SR,r0
  8C31DEF8  2039  and r3,r0
  8C31DEFA  CBF0  or ##240,R0
  8C31DEFC  400E  ldc r0,SR
  8C31DEFE  B9E9  bsr 8C31D2D4
  8C31DF00  0009  nop
  8C31DF02  D343  mov.l @([8C31E010]),r3
  8C31DF04  D441  mov.l @([8C31E00C]),r4
  8C31DF06  430B  jsr @r3
  8C31DF08  0009  nop
  8C31DF0A  60F2  mov.l @r15,r0
  8C31DF0C  0202  stc SR,r2
  8C31DF0E  937A  mov.w @([8C31E006]),r3
  8C31DF10  C90F  and ##15,R0
  8C31DF12  4008  shll2 r0
  8C31DF14  2239  and r3,r2
  8C31DF16  4008  shll2 r0
  8C31DF18  202B  or r2,r0
  8C31DF1A  400E  ldc r0,SR
  8C31DF1C  D33D  mov.l @([8C31E014]),r3
  8C31DF1E  430B  jsr @r3
  8C31DF20  E400  mov ##0x00,r4
  8C31DF22  D23D  mov.l @([8C31E018]),r2
  8C31DF24  2202  mov.l r0,@r2
  8C31DF26  7F08  add ##8,r15
  8C31DF28  4F26  lds.l @r15+,PR
  8C31DF2A  FFF9  fmov.s @r15+,fr15
  8C31DF2C  68F6  mov.l @r15+,r8
  8C31DF2E  69F6  mov.l @r15+,r9
  8C31DF30  6AF6  mov.l @r15+,r10
  8C31DF32  6BF6  mov.l @r15+,r11
  8C31DF34  6CF6  mov.l @r15+,r12
  8C31DF36  6DF6  mov.l @r15+,r13
  8C31DF38  000B  rts
  8C31DF3A  6EF6  mov.l @r15+,r14
  8C31DF3C  4F22  sts.l PR,@-r15
  8C31DF3E  7FFC  add ##-4,r15
  8C31DF40  6343  mov r4,r3
  8C31DF42  4300  shll r3
  8C31DF44  6243  mov r4,r2
  8C31DF46  332C  add r2,r3
  8C31DF48  4308  shll2 r3
  8C31DF4A  D134  mov.l @([8C31E01C]),r1
  8C31DF4C  4308  shll2 r3
  8C31DF4E  D234  mov.l @([8C31E020]),r2
  8C31DF50  4308  shll2 r3
  8C31DF52  2F42  mov.l r4,@r15
  8C31DF54  331C  add r1,r3
  8C31DF56  2232  mov.l r3,@r2
  8C31DF58  D232  mov.l @([8C31E024]),r2
  8C31DF5A  420B  jsr @r2
  8C31DF5C  6433  mov r3,r4
  8C31DF5E  61F2  mov.l @r15,r1
  8C31DF60  D331  mov.l @([8C31E028]),r3
  8C31DF62  D232  mov.l @([8C31E02C]),r2
  8C31DF64  4108  shll2 r1
  8C31DF66  313C  add r3,r1
  8C31DF68  2212  mov.l r1,@r2
  8C31DF6A  7F04  add ##4,r15
  8C31DF6C  4F26  lds.l @r15+,PR
  8C31DF6E  000B  rts
  8C31DF70  0009  nop
  8C31DF72  E500  mov ##0x00,r5
  8C31DF74  D32E  mov.l @([8C31E030]),r3
  8C31DF76  E040  mov ##0x40,r0
  8C31DF78  2432  mov.l r3,@r4
  8C31DF7A  E208  mov ##0x08,r2
  8C31DF7C  E302  mov ##0x02,r3
  8C31DF7E  1451  mov.l r5,@(4,r4)
  8C31DF80  E706  mov ##0x06,r7
  8C31DF82  1452  mov.l r5,@(8,r4)
  8C31DF84  E601  mov ##0x01,r6
  8C31DF86  1463  mov.l r6,@(12,r4)
  8C31DF88  1454  mov.l r5,@(16,r4)
  8C31DF8A  1475  mov.l r7,@(20,r4)
  8C31DF8C  1466  mov.l r6,@(24,r4)
  8C31DF8E  1457  mov.l r5,@(28,r4)
  8C31DF90  1468  mov.l r6,@(32,r4)
  8C31DF92  1459  mov.l r5,@(36,r4)
  8C31DF94  145A  mov.l r5,@(40,r4)
  8C31DF96  142B  mov.l r2,@(44,r4)
  8C31DF98  147C  mov.l r7,@(48,r4)
  8C31DF9A  145D  mov.l r5,@(52,r4)
  8C31DF9C  145E  mov.l r5,@(56,r4)
  8C31DF9E  143F  mov.l r3,@(60,r4)
  8C31DFA0  0456  mov.l r5,@(R0,r4)
  8C31DFA2  E044  mov ##0x44,r0
  8C31DFA4  0456  mov.l r5,@(R0,r4)
  8C31DFA6  E048  mov ##0x48,r0
  8C31DFA8  0456  mov.l r5,@(R0,r4)
  8C31DFAA  E050  mov ##0x50,r0
  8C31DFAC  0456  mov.l r5,@(R0,r4)
  8C31DFAE  E04C  mov ##0x4C,r0
  8C31DFB0  0456  mov.l r5,@(R0,r4)
  8C31DFB2  E054  mov ##0x54,r0
  8C31DFB4  F49D  fldi1 fr4
  8C31DFB6  0456  mov.l r5,@(R0,r4)
  8C31DFB8  E058  mov ##0x58,r0
  8C31DFBA  0456  mov.l r5,@(R0,r4)
  8C31DFBC  E05C  mov ##0x5C,r0
  8C31DFBE  E304  mov ##0x04,r3
  8C31DFC0  0436  mov.l r3,@(R0,r4)
  8C31DFC2  E060  mov ##0x60,r0
  8C31DFC4  E203  mov ##0x03,r2
  8C31DFC6  0426  mov.l r2,@(R0,r4)
  8C31DFC8  E06C  mov ##0x6C,r0
```


## Le Mans 24 Hours (Europe) (En,Fr,De,Es,It)

Dump: `/mnt/1TB/dcbat/20261007-163759_Le_Mans_24_Hours__Europe___En_Fr_De_Es_I/jit-109461.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```


## Macross M3

Dump: `/mnt/1TB/dcbat/20261007-153242_Macross_M3_/jit-35982.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```

```
; 8C1C5B7A-8C1C5C62
  8C1C5B7A  FE77  fmov.s fr7,@(R0,r14)
  8C1C5B7C  E038  mov ##0x38,r0
  8C1C5B7E  FE67  fmov.s fr6,@(R0,r14)
  8C1C5B80  E03C  mov ##0x3C,r0
  8C1C5B82  FE47  fmov.s fr4,@(R0,r14)
  8C1C5B84  0002  stc SR,r0
  8C1C5B86  4009  shlr2 r0
  8C1C5B88  4009  shlr2 r0
  8C1C5B8A  C90F  and ##15,R0
  8C1C5B8C  2F02  mov.l r0,@r15
  8C1C5B8E  0002  stc SR,r0
  8C1C5B90  2039  and r3,r0
  8C1C5B92  CBF0  or ##240,R0
  8C1C5B94  400E  ldc r0,SR
  8C1C5B96  B9E9  bsr 8C1C4F6C
  8C1C5B98  0009  nop
  8C1C5B9A  D343  mov.l @([8C1C5CA8]),r3
  8C1C5B9C  D441  mov.l @([8C1C5CA4]),r4
  8C1C5B9E  430B  jsr @r3
  8C1C5BA0  0009  nop
  8C1C5BA2  60F2  mov.l @r15,r0
  8C1C5BA4  0202  stc SR,r2
  8C1C5BA6  937A  mov.w @([8C1C5C9E]),r3
  8C1C5BA8  C90F  and ##15,R0
  8C1C5BAA  4008  shll2 r0
  8C1C5BAC  2239  and r3,r2
  8C1C5BAE  4008  shll2 r0
  8C1C5BB0  202B  or r2,r0
  8C1C5BB2  400E  ldc r0,SR
  8C1C5BB4  D33D  mov.l @([8C1C5CAC]),r3
  8C1C5BB6  430B  jsr @r3
  8C1C5BB8  E400  mov ##0x00,r4
  8C1C5BBA  D23D  mov.l @([8C1C5CB0]),r2
  8C1C5BBC  2202  mov.l r0,@r2
  8C1C5BBE  7F08  add ##8,r15
  8C1C5BC0  4F26  lds.l @r15+,PR
  8C1C5BC2  FFF9  fmov.s @r15+,fr15
  8C1C5BC4  68F6  mov.l @r15+,r8
  8C1C5BC6  69F6  mov.l @r15+,r9
  8C1C5BC8  6AF6  mov.l @r15+,r10
  8C1C5BCA  6BF6  mov.l @r15+,r11
  8C1C5BCC  6CF6  mov.l @r15+,r12
  8C1C5BCE  6DF6  mov.l @r15+,r13
  8C1C5BD0  000B  rts
  8C1C5BD2  6EF6  mov.l @r15+,r14
  8C1C5BD4  4F22  sts.l PR,@-r15
  8C1C5BD6  7FFC  add ##-4,r15
  8C1C5BD8  6343  mov r4,r3
  8C1C5BDA  4300  shll r3
  8C1C5BDC  6243  mov r4,r2
  8C1C5BDE  332C  add r2,r3
  8C1C5BE0  4308  shll2 r3
  8C1C5BE2  D134  mov.l @([8C1C5CB4]),r1
  8C1C5BE4  4308  shll2 r3
  8C1C5BE6  D234  mov.l @([8C1C5CB8]),r2
  8C1C5BE8  4308  shll2 r3
  8C1C5BEA  2F42  mov.l r4,@r15
  8C1C5BEC  331C  add r1,r3
  8C1C5BEE  2232  mov.l r3,@r2
  8C1C5BF0  D232  mov.l @([8C1C5CBC]),r2
  8C1C5BF2  420B  jsr @r2
  8C1C5BF4  6433  mov r3,r4
  8C1C5BF6  61F2  mov.l @r15,r1
  8C1C5BF8  D331  mov.l @([8C1C5CC0]),r3
  8C1C5BFA  D232  mov.l @([8C1C5CC4]),r2
  8C1C5BFC  4108  shll2 r1
  8C1C5BFE  313C  add r3,r1
  8C1C5C00  2212  mov.l r1,@r2
  8C1C5C02  7F04  add ##4,r15
  8C1C5C04  4F26  lds.l @r15+,PR
  8C1C5C06  000B  rts
  8C1C5C08  0009  nop
  8C1C5C0A  E500  mov ##0x00,r5
  8C1C5C0C  D32E  mov.l @([8C1C5CC8]),r3
  8C1C5C0E  E040  mov ##0x40,r0
  8C1C5C10  2432  mov.l r3,@r4
  8C1C5C12  E208  mov ##0x08,r2
  8C1C5C14  E302  mov ##0x02,r3
  8C1C5C16  1451  mov.l r5,@(4,r4)
  8C1C5C18  E706  mov ##0x06,r7
  8C1C5C1A  1452  mov.l r5,@(8,r4)
  8C1C5C1C  E601  mov ##0x01,r6
  8C1C5C1E  1463  mov.l r6,@(12,r4)
  8C1C5C20  1454  mov.l r5,@(16,r4)
  8C1C5C22  1475  mov.l r7,@(20,r4)
  8C1C5C24  1466  mov.l r6,@(24,r4)
  8C1C5C26  1457  mov.l r5,@(28,r4)
  8C1C5C28  1468  mov.l r6,@(32,r4)
  8C1C5C2A  1459  mov.l r5,@(36,r4)
  8C1C5C2C  145A  mov.l r5,@(40,r4)
  8C1C5C2E  142B  mov.l r2,@(44,r4)
  8C1C5C30  147C  mov.l r7,@(48,r4)
  8C1C5C32  145D  mov.l r5,@(52,r4)
  8C1C5C34  145E  mov.l r5,@(56,r4)
  8C1C5C36  143F  mov.l r3,@(60,r4)
  8C1C5C38  0456  mov.l r5,@(R0,r4)
  8C1C5C3A  E044  mov ##0x44,r0
  8C1C5C3C  0456  mov.l r5,@(R0,r4)
  8C1C5C3E  E048  mov ##0x48,r0
  8C1C5C40  0456  mov.l r5,@(R0,r4)
  8C1C5C42  E050  mov ##0x50,r0
  8C1C5C44  0456  mov.l r5,@(R0,r4)
  8C1C5C46  E04C  mov ##0x4C,r0
  8C1C5C48  0456  mov.l r5,@(R0,r4)
  8C1C5C4A  E054  mov ##0x54,r0
  8C1C5C4C  F49D  fldi1 fr4
  8C1C5C4E  0456  mov.l r5,@(R0,r4)
  8C1C5C50  E058  mov ##0x58,r0
  8C1C5C52  0456  mov.l r5,@(R0,r4)
  8C1C5C54  E05C  mov ##0x5C,r0
  8C1C5C56  E304  mov ##0x04,r3
  8C1C5C58  0436  mov.l r3,@(R0,r4)
  8C1C5C5A  E060  mov ##0x60,r0
  8C1C5C5C  E203  mov ##0x03,r2
  8C1C5C5E  0426  mov.l r2,@(R0,r4)
  8C1C5C60  E06C  mov ##0x6C,r0
```

```
; 8C1EBE88-8C1EBF70
  8C1EBE88  9247  mov.w @([8C1EBF1A]),r2
  8C1EBE8A  6532  mov.l @r3,r5
  8C1EBE8C  EC00  mov ##0x00,r12
  8C1EBE8E  6EC3  mov r12,r14
  8C1EBE90  045E  mov.l @(R0,r5),r4
  8C1EBE92  0002  stc SR,r0
  8C1EBE94  4009  shlr2 r0
  8C1EBE96  4009  shlr2 r0
  8C1EBE98  C90F  and ##15,R0
  8C1EBE9A  2F02  mov.l r0,@r15
  8C1EBE9C  0002  stc SR,r0
  8C1EBE9E  2029  and r2,r0
  8C1EBEA0  CBF0  or ##240,R0
  8C1EBEA2  400E  ldc r0,SR
  8C1EBEA4  2558  tst r5,r5
  8C1EBEA6  8B02  bf 8C1EBEAE
  ...       (não compilado nesta sessão)
  8C1EBEAE  D51D  mov.l @([8C1EBF24]),r5
  8C1EBEB0  E740  mov ##0x40,r7
  8C1EBEB2  66C3  mov r12,r6
  8C1EBEB4  6242  mov.l @r4,r2
  8C1EBEB6  3250  cmp/eq r5,r2
  8C1EBEB8  8B06  bf 8C1EBEC8
  8C1EBEBA  7601  add ##1,r6
  8C1EBEBC  3673  cmp/ge r7,r6
  8C1EBEBE  8FF9  bf.s 8C1EBEB4
  8C1EBEC0  7440  add ##64,r4
  ...       (não compilado nesta sessão)
  8C1EBEC8  D318  mov.l @([8C1EBF2C]),r3
  8C1EBECA  2452  mov.l r5,@r4
  8C1EBECC  2D42  mov.l r4,@r13
  8C1EBECE  430B  jsr @r3
  8C1EBED0  E500  mov ##0x00,r5
  8C1EBED2  E700  mov ##0x00,r7
  8C1EBED4  D216  mov.l @([8C1EBF30]),r2
  8C1EBED6  6573  mov r7,r5
  8C1EBED8  2FC6  mov.l r12,@-r15
  8C1EBEDA  6673  mov r7,r6
  8C1EBEDC  420B  jsr @r2
  8C1EBEDE  64D2  mov.l @r13,r4
  8C1EBEE0  7F04  add ##4,r15
  8C1EBEE2  D20E  mov.l @([8C1EBF1C]),r2
  8C1EBEE4  E054  mov ##0x54,r0
  8C1EBEE6  6322  mov.l @r2,r3
  8C1EBEE8  013E  mov.l @(R0,r3),r1
  8C1EBEEA  7101  add ##1,r1
  8C1EBEEC  0316  mov.l r1,@(R0,r3)
  8C1EBEEE  60F2  mov.l @r15,r0
  8C1EBEF0  0302  stc SR,r3
  8C1EBEF2  9212  mov.w @([8C1EBF1A]),r2
  8C1EBEF4  C90F  and ##15,R0
  8C1EBEF6  4008  shll2 r0
  8C1EBEF8  2329  and r2,r3
  8C1EBEFA  4008  shll2 r0
  8C1EBEFC  203B  or r3,r0
  8C1EBEFE  400E  ldc r0,SR
  8C1EBF00  2EE8  tst r14,r14
  8C1EBF02  8903  bt 8C1EBF0C
  ...       (não compilado nesta sessão)
  8C1EBF0C  60E3  mov r14,r0
  8C1EBF0E  7F04  add ##4,r15
  8C1EBF10  4F26  lds.l @r15+,PR
  8C1EBF12  6CF6  mov.l @r15+,r12
  8C1EBF14  6DF6  mov.l @r15+,r13
  8C1EBF16  000B  rts
  8C1EBF18  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C1EBF3C  2FE6  mov.l r14,@-r15
  8C1EBF3E  2FD6  mov.l r13,@-r15
  8C1EBF40  2FC6  mov.l r12,@-r15
  8C1EBF42  2FB6  mov.l r11,@-r15
  8C1EBF44  2FA6  mov.l r10,@-r15
  8C1EBF46  4F22  sts.l PR,@-r15
  8C1EBF48  7FF8  add ##-8,r15
  8C1EBF4A  0002  stc SR,r0
  8C1EBF4C  9357  mov.w @([8C1EBFFE]),r3
  8C1EBF4E  6A43  mov r4,r10
  8C1EBF50  6D43  mov r4,r13
  8C1EBF52  4009  shlr2 r0
  8C1EBF54  4009  shlr2 r0
  8C1EBF56  C90F  and ##15,R0
  8C1EBF58  1F01  mov.l r0,@(4,r15)
  8C1EBF5A  0002  stc SR,r0
  8C1EBF5C  2039  and r3,r0
  8C1EBF5E  CBF0  or ##240,R0
  8C1EBF60  400E  ldc r0,SR
  8C1EBF62  D228  mov.l @([8C1EC004]),r2
  8C1EBF64  420B  jsr @r2
  8C1EBF66  64A3  mov r10,r4
  8C1EBF68  6E03  mov r0,r14
  8C1EBF6A  2EE8  tst r14,r14
  8C1EBF6C  8B2F  bf 8C1EBFCE
  8C1EBF6E  DC26  mov.l @([8C1EC008]),r12
```


## Marvel vs. Capcom 2 - New Age of Heroes (Europe)

Dump: `/mnt/1TB/dcbat_off/20261007-191632_Marvel_vs__Capcom_2_-_New_Age_of_Heroes_/jit-9882.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```


## Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0)

Dump: `/mnt/1TB/dcbat_off/20261007-190751_Napple_Tale_-_Arsia_in_Daydream__Japan__/jit-6592.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```

```
; 8C108CAE-8C108D96
  8C108CAE  FE77  fmov.s fr7,@(R0,r14)
  8C108CB0  E038  mov ##0x38,r0
  8C108CB2  FE67  fmov.s fr6,@(R0,r14)
  8C108CB4  E03C  mov ##0x3C,r0
  8C108CB6  FE47  fmov.s fr4,@(R0,r14)
  8C108CB8  0002  stc SR,r0
  8C108CBA  4009  shlr2 r0
  8C108CBC  4009  shlr2 r0
  8C108CBE  C90F  and ##15,R0
  8C108CC0  2F02  mov.l r0,@r15
  8C108CC2  0002  stc SR,r0
  8C108CC4  2039  and r3,r0
  8C108CC6  CBF0  or ##240,R0
  8C108CC8  400E  ldc r0,SR
  8C108CCA  B9E9  bsr 8C1080A0
  8C108CCC  0009  nop
  8C108CCE  D343  mov.l @([8C108DDC]),r3
  8C108CD0  D441  mov.l @([8C108DD8]),r4
  8C108CD2  430B  jsr @r3
  8C108CD4  0009  nop
  8C108CD6  60F2  mov.l @r15,r0
  8C108CD8  0202  stc SR,r2
  8C108CDA  937A  mov.w @([8C108DD2]),r3
  8C108CDC  C90F  and ##15,R0
  8C108CDE  4008  shll2 r0
  8C108CE0  2239  and r3,r2
  8C108CE2  4008  shll2 r0
  8C108CE4  202B  or r2,r0
  8C108CE6  400E  ldc r0,SR
  8C108CE8  D33D  mov.l @([8C108DE0]),r3
  8C108CEA  430B  jsr @r3
  8C108CEC  E400  mov ##0x00,r4
  8C108CEE  D23D  mov.l @([8C108DE4]),r2
  8C108CF0  2202  mov.l r0,@r2
  8C108CF2  7F08  add ##8,r15
  8C108CF4  4F26  lds.l @r15+,PR
  8C108CF6  FFF9  fmov.s @r15+,fr15
  8C108CF8  68F6  mov.l @r15+,r8
  8C108CFA  69F6  mov.l @r15+,r9
  8C108CFC  6AF6  mov.l @r15+,r10
  8C108CFE  6BF6  mov.l @r15+,r11
  8C108D00  6CF6  mov.l @r15+,r12
  8C108D02  6DF6  mov.l @r15+,r13
  8C108D04  000B  rts
  8C108D06  6EF6  mov.l @r15+,r14
  8C108D08  4F22  sts.l PR,@-r15
  8C108D0A  7FFC  add ##-4,r15
  8C108D0C  6343  mov r4,r3
  8C108D0E  4300  shll r3
  8C108D10  6243  mov r4,r2
  8C108D12  332C  add r2,r3
  8C108D14  4308  shll2 r3
  8C108D16  D134  mov.l @([8C108DE8]),r1
  8C108D18  4308  shll2 r3
  8C108D1A  D234  mov.l @([8C108DEC]),r2
  8C108D1C  4308  shll2 r3
  8C108D1E  2F42  mov.l r4,@r15
  8C108D20  331C  add r1,r3
  8C108D22  2232  mov.l r3,@r2
  8C108D24  D232  mov.l @([8C108DF0]),r2
  8C108D26  420B  jsr @r2
  8C108D28  6433  mov r3,r4
  8C108D2A  61F2  mov.l @r15,r1
  8C108D2C  D331  mov.l @([8C108DF4]),r3
  8C108D2E  D232  mov.l @([8C108DF8]),r2
  8C108D30  4108  shll2 r1
  8C108D32  313C  add r3,r1
  8C108D34  2212  mov.l r1,@r2
  8C108D36  7F04  add ##4,r15
  8C108D38  4F26  lds.l @r15+,PR
  8C108D3A  000B  rts
  8C108D3C  0009  nop
  8C108D3E  E500  mov ##0x00,r5
  8C108D40  D32E  mov.l @([8C108DFC]),r3
  8C108D42  E040  mov ##0x40,r0
  8C108D44  2432  mov.l r3,@r4
  8C108D46  E208  mov ##0x08,r2
  8C108D48  E302  mov ##0x02,r3
  8C108D4A  1451  mov.l r5,@(4,r4)
  8C108D4C  E706  mov ##0x06,r7
  8C108D4E  1452  mov.l r5,@(8,r4)
  8C108D50  E601  mov ##0x01,r6
  8C108D52  1463  mov.l r6,@(12,r4)
  8C108D54  1454  mov.l r5,@(16,r4)
  8C108D56  1475  mov.l r7,@(20,r4)
  8C108D58  1466  mov.l r6,@(24,r4)
  8C108D5A  1457  mov.l r5,@(28,r4)
  8C108D5C  1468  mov.l r6,@(32,r4)
  8C108D5E  1459  mov.l r5,@(36,r4)
  8C108D60  145A  mov.l r5,@(40,r4)
  8C108D62  142B  mov.l r2,@(44,r4)
  8C108D64  147C  mov.l r7,@(48,r4)
  8C108D66  145D  mov.l r5,@(52,r4)
  8C108D68  145E  mov.l r5,@(56,r4)
  8C108D6A  143F  mov.l r3,@(60,r4)
  8C108D6C  0456  mov.l r5,@(R0,r4)
  8C108D6E  E044  mov ##0x44,r0
  8C108D70  0456  mov.l r5,@(R0,r4)
  8C108D72  E048  mov ##0x48,r0
  8C108D74  0456  mov.l r5,@(R0,r4)
  8C108D76  E050  mov ##0x50,r0
  8C108D78  0456  mov.l r5,@(R0,r4)
  8C108D7A  E04C  mov ##0x4C,r0
  8C108D7C  0456  mov.l r5,@(R0,r4)
  8C108D7E  E054  mov ##0x54,r0
  8C108D80  F49D  fldi1 fr4
  8C108D82  0456  mov.l r5,@(R0,r4)
  8C108D84  E058  mov ##0x58,r0
  8C108D86  0456  mov.l r5,@(R0,r4)
  8C108D88  E05C  mov ##0x5C,r0
  8C108D8A  E304  mov ##0x04,r3
  8C108D8C  0436  mov.l r3,@(R0,r4)
  8C108D8E  E060  mov ##0x60,r0
  8C108D90  E203  mov ##0x03,r2
  8C108D92  0426  mov.l r2,@(R0,r4)
  8C108D94  E06C  mov ##0x6C,r0
```

```
; 8C10C514-8C10C5FC
  8C10C514  9247  mov.w @([8C10C5A6]),r2
  8C10C516  6532  mov.l @r3,r5
  8C10C518  EC00  mov ##0x00,r12
  8C10C51A  6EC3  mov r12,r14
  8C10C51C  045E  mov.l @(R0,r5),r4
  8C10C51E  0002  stc SR,r0
  8C10C520  4009  shlr2 r0
  8C10C522  4009  shlr2 r0
  8C10C524  C90F  and ##15,R0
  8C10C526  2F02  mov.l r0,@r15
  8C10C528  0002  stc SR,r0
  8C10C52A  2029  and r2,r0
  8C10C52C  CBF0  or ##240,R0
  8C10C52E  400E  ldc r0,SR
  8C10C530  2558  tst r5,r5
  8C10C532  8B02  bf 8C10C53A
  ...       (não compilado nesta sessão)
  8C10C53A  D51D  mov.l @([8C10C5B0]),r5
  8C10C53C  E740  mov ##0x40,r7
  8C10C53E  66C3  mov r12,r6
  8C10C540  6242  mov.l @r4,r2
  8C10C542  3250  cmp/eq r5,r2
  8C10C544  8B06  bf 8C10C554
  8C10C546  7601  add ##1,r6
  8C10C548  3673  cmp/ge r7,r6
  8C10C54A  8FF9  bf.s 8C10C540
  8C10C54C  7440  add ##64,r4
  ...       (não compilado nesta sessão)
  8C10C554  D318  mov.l @([8C10C5B8]),r3
  8C10C556  2452  mov.l r5,@r4
  8C10C558  2D42  mov.l r4,@r13
  8C10C55A  430B  jsr @r3
  8C10C55C  E500  mov ##0x00,r5
  8C10C55E  E700  mov ##0x00,r7
  8C10C560  D216  mov.l @([8C10C5BC]),r2
  8C10C562  6573  mov r7,r5
  8C10C564  2FC6  mov.l r12,@-r15
  8C10C566  6673  mov r7,r6
  8C10C568  420B  jsr @r2
  8C10C56A  64D2  mov.l @r13,r4
  8C10C56C  7F04  add ##4,r15
  8C10C56E  D20E  mov.l @([8C10C5A8]),r2
  8C10C570  E054  mov ##0x54,r0
  8C10C572  6322  mov.l @r2,r3
  8C10C574  013E  mov.l @(R0,r3),r1
  8C10C576  7101  add ##1,r1
  8C10C578  0316  mov.l r1,@(R0,r3)
  8C10C57A  60F2  mov.l @r15,r0
  8C10C57C  0302  stc SR,r3
  8C10C57E  9212  mov.w @([8C10C5A6]),r2
  8C10C580  C90F  and ##15,R0
  8C10C582  4008  shll2 r0
  8C10C584  2329  and r2,r3
  8C10C586  4008  shll2 r0
  8C10C588  203B  or r3,r0
  8C10C58A  400E  ldc r0,SR
  8C10C58C  2EE8  tst r14,r14
  8C10C58E  8903  bt 8C10C598
  ...       (não compilado nesta sessão)
  8C10C598  60E3  mov r14,r0
  8C10C59A  7F04  add ##4,r15
  8C10C59C  4F26  lds.l @r15+,PR
  8C10C59E  6CF6  mov.l @r15+,r12
  8C10C5A0  6DF6  mov.l @r15+,r13
  8C10C5A2  000B  rts
  8C10C5A4  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C10C5C8  2FE6  mov.l r14,@-r15
  8C10C5CA  2FD6  mov.l r13,@-r15
  8C10C5CC  2FC6  mov.l r12,@-r15
  8C10C5CE  2FB6  mov.l r11,@-r15
  8C10C5D0  2FA6  mov.l r10,@-r15
  8C10C5D2  4F22  sts.l PR,@-r15
  8C10C5D4  7FF8  add ##-8,r15
  8C10C5D6  0002  stc SR,r0
  8C10C5D8  9357  mov.w @([8C10C68A]),r3
  8C10C5DA  6A43  mov r4,r10
  8C10C5DC  6D43  mov r4,r13
  8C10C5DE  4009  shlr2 r0
  8C10C5E0  4009  shlr2 r0
  8C10C5E2  C90F  and ##15,R0
  8C10C5E4  1F01  mov.l r0,@(4,r15)
  8C10C5E6  0002  stc SR,r0
  8C10C5E8  2039  and r3,r0
  8C10C5EA  CBF0  or ##240,R0
  8C10C5EC  400E  ldc r0,SR
  8C10C5EE  D228  mov.l @([8C10C690]),r2
  8C10C5F0  420B  jsr @r2
  8C10C5F2  64A3  mov r10,r4
  8C10C5F4  6E03  mov r0,r14
  8C10C5F6  2EE8  tst r14,r14
  8C10C5F8  8B2F  bf 8C10C65A
  8C10C5FA  DC26  mov.l @([8C10C694]),r12
```


## Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs)

Dump: `/mnt/1TB/dcbat/20261007-155901_Phantasy_Star_Online_Ver__2__USA___EnJaF/jit-73352.txt`

```
; 8C0083F8-8C0084E0
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
  8C00843E  D30B  mov.l @([8C00846C]),r3
  8C008440  D409  mov.l @([8C008468]),r4
  8C008442  9510  mov.w @([8C008466]),r5
  8C008444  430B  jsr @r3
  8C008446  0009  nop
  8C008448  D308  mov.l @([8C00846C]),r3
  8C00844A  D40A  mov.l @([8C008474]),r4
  8C00844C  D208  mov.l @([8C008470]),r2
  8C00844E  430B  jsr @r3
  8C008450  6522  mov.l @r2,r5
  8C008452  D209  mov.l @([8C008478]),r2
  8C008454  6323  mov r2,r3
  8C008456  2F22  mov.l r2,@r15
  8C008458  6232  mov.l @r3,r2
  8C00845A  6323  mov r2,r3
  8C00845C  2F22  mov.l r2,@r15
  8C00845E  7F04  add ##4,r15
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  8C008456  2F22  mov.l r2,@r15
  8C008458  6232  mov.l @r3,r2
  8C00845A  6323  mov r2,r3
  8C00845C  2F22  mov.l r2,@r15
  8C00845E  7F04  add ##4,r15
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```

```
; 8C3544BC-8C3545A4
  8C3544BC  9247  mov.w @([8C35454E]),r2
  8C3544BE  6532  mov.l @r3,r5
  8C3544C0  EC00  mov ##0x00,r12
  8C3544C2  6EC3  mov r12,r14
  8C3544C4  045E  mov.l @(R0,r5),r4
  8C3544C6  0002  stc SR,r0
  8C3544C8  4009  shlr2 r0
  8C3544CA  4009  shlr2 r0
  8C3544CC  C90F  and ##15,R0
  8C3544CE  2F02  mov.l r0,@r15
  8C3544D0  0002  stc SR,r0
  8C3544D2  2029  and r2,r0
  8C3544D4  CBF0  or ##240,R0
  8C3544D6  400E  ldc r0,SR
  8C3544D8  2558  tst r5,r5
  8C3544DA  8B02  bf 8C3544E2
  ...       (não compilado nesta sessão)
  8C3544E2  D51D  mov.l @([8C354558]),r5
  8C3544E4  E740  mov ##0x40,r7
  8C3544E6  66C3  mov r12,r6
  8C3544E8  6242  mov.l @r4,r2
  8C3544EA  3250  cmp/eq r5,r2
  8C3544EC  8B06  bf 8C3544FC
  8C3544EE  7601  add ##1,r6
  8C3544F0  3673  cmp/ge r7,r6
  8C3544F2  8FF9  bf.s 8C3544E8
  8C3544F4  7440  add ##64,r4
  ...       (não compilado nesta sessão)
  8C3544FC  D318  mov.l @([8C354560]),r3
  8C3544FE  2452  mov.l r5,@r4
  8C354500  2D42  mov.l r4,@r13
  8C354502  430B  jsr @r3
  8C354504  E500  mov ##0x00,r5
  8C354506  E700  mov ##0x00,r7
  8C354508  D216  mov.l @([8C354564]),r2
  8C35450A  6573  mov r7,r5
  8C35450C  2FC6  mov.l r12,@-r15
  8C35450E  6673  mov r7,r6
  8C354510  420B  jsr @r2
  8C354512  64D2  mov.l @r13,r4
  8C354514  7F04  add ##4,r15
  8C354516  D20E  mov.l @([8C354550]),r2
  8C354518  E054  mov ##0x54,r0
  8C35451A  6322  mov.l @r2,r3
  8C35451C  013E  mov.l @(R0,r3),r1
  8C35451E  7101  add ##1,r1
  8C354520  0316  mov.l r1,@(R0,r3)
  8C354522  60F2  mov.l @r15,r0
  8C354524  0302  stc SR,r3
  8C354526  9212  mov.w @([8C35454E]),r2
  8C354528  C90F  and ##15,R0
  8C35452A  4008  shll2 r0
  8C35452C  2329  and r2,r3
  8C35452E  4008  shll2 r0
  8C354530  203B  or r3,r0
  8C354532  400E  ldc r0,SR
  8C354534  2EE8  tst r14,r14
  8C354536  8903  bt 8C354540
  ...       (não compilado nesta sessão)
  8C354540  60E3  mov r14,r0
  8C354542  7F04  add ##4,r15
  8C354544  4F26  lds.l @r15+,PR
  8C354546  6CF6  mov.l @r15+,r12
  8C354548  6DF6  mov.l @r15+,r13
  8C35454A  000B  rts
  8C35454C  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C354570  2FE6  mov.l r14,@-r15
  8C354572  2FD6  mov.l r13,@-r15
  8C354574  2FC6  mov.l r12,@-r15
  8C354576  2FB6  mov.l r11,@-r15
  8C354578  2FA6  mov.l r10,@-r15
  8C35457A  4F22  sts.l PR,@-r15
  8C35457C  7FF8  add ##-8,r15
  8C35457E  0002  stc SR,r0
  8C354580  9357  mov.w @([8C354632]),r3
  8C354582  6A43  mov r4,r10
  8C354584  6D43  mov r4,r13
  8C354586  4009  shlr2 r0
  8C354588  4009  shlr2 r0
  8C35458A  C90F  and ##15,R0
  8C35458C  1F01  mov.l r0,@(4,r15)
  8C35458E  0002  stc SR,r0
  8C354590  2039  and r3,r0
  8C354592  CBF0  or ##240,R0
  8C354594  400E  ldc r0,SR
  8C354596  D228  mov.l @([8C354638]),r2
  8C354598  420B  jsr @r2
  8C35459A  64A3  mov r10,r4
  8C35459C  6E03  mov r0,r14
  8C35459E  2EE8  tst r14,r14
  8C3545A0  8B2F  bf 8C354602
  8C3545A2  DC26  mov.l @([8C35463C]),r12
```

```
; 8C38D2E2-8C38D3CA
  8C38D2E2  FE77  fmov.s fr7,@(R0,r14)
  8C38D2E4  E038  mov ##0x38,r0
  8C38D2E6  FE67  fmov.s fr6,@(R0,r14)
  8C38D2E8  E03C  mov ##0x3C,r0
  8C38D2EA  FE47  fmov.s fr4,@(R0,r14)
  8C38D2EC  0002  stc SR,r0
  8C38D2EE  4009  shlr2 r0
  8C38D2F0  4009  shlr2 r0
  8C38D2F2  C90F  and ##15,R0
  8C38D2F4  2F02  mov.l r0,@r15
  8C38D2F6  0002  stc SR,r0
  8C38D2F8  2039  and r3,r0
  8C38D2FA  CBF0  or ##240,R0
  8C38D2FC  400E  ldc r0,SR
  8C38D2FE  B9E9  bsr 8C38C6D4
  8C38D300  0009  nop
  8C38D302  D343  mov.l @([8C38D410]),r3
  8C38D304  D441  mov.l @([8C38D40C]),r4
  8C38D306  430B  jsr @r3
  8C38D308  0009  nop
  8C38D30A  60F2  mov.l @r15,r0
  8C38D30C  0202  stc SR,r2
  8C38D30E  937A  mov.w @([8C38D406]),r3
  8C38D310  C90F  and ##15,R0
  8C38D312  4008  shll2 r0
  8C38D314  2239  and r3,r2
  8C38D316  4008  shll2 r0
  8C38D318  202B  or r2,r0
  8C38D31A  400E  ldc r0,SR
  8C38D31C  D33D  mov.l @([8C38D414]),r3
  8C38D31E  430B  jsr @r3
  8C38D320  E400  mov ##0x00,r4
  8C38D322  D23D  mov.l @([8C38D418]),r2
  8C38D324  2202  mov.l r0,@r2
  8C38D326  7F08  add ##8,r15
  8C38D328  4F26  lds.l @r15+,PR
  8C38D32A  FFF9  fmov.s @r15+,fr15
  8C38D32C  68F6  mov.l @r15+,r8
  8C38D32E  69F6  mov.l @r15+,r9
  8C38D330  6AF6  mov.l @r15+,r10
  8C38D332  6BF6  mov.l @r15+,r11
  8C38D334  6CF6  mov.l @r15+,r12
  8C38D336  6DF6  mov.l @r15+,r13
  8C38D338  000B  rts
  8C38D33A  6EF6  mov.l @r15+,r14
  8C38D33C  4F22  sts.l PR,@-r15
  8C38D33E  7FFC  add ##-4,r15
  8C38D340  6343  mov r4,r3
  8C38D342  4300  shll r3
  8C38D344  6243  mov r4,r2
  8C38D346  332C  add r2,r3
  8C38D348  4308  shll2 r3
  8C38D34A  D134  mov.l @([8C38D41C]),r1
  8C38D34C  4308  shll2 r3
  8C38D34E  D234  mov.l @([8C38D420]),r2
  8C38D350  4308  shll2 r3
  8C38D352  2F42  mov.l r4,@r15
  8C38D354  331C  add r1,r3
  8C38D356  2232  mov.l r3,@r2
  8C38D358  D232  mov.l @([8C38D424]),r2
  8C38D35A  420B  jsr @r2
  8C38D35C  6433  mov r3,r4
  8C38D35E  61F2  mov.l @r15,r1
  8C38D360  D331  mov.l @([8C38D428]),r3
  8C38D362  D232  mov.l @([8C38D42C]),r2
  8C38D364  4108  shll2 r1
  8C38D366  313C  add r3,r1
  8C38D368  2212  mov.l r1,@r2
  8C38D36A  7F04  add ##4,r15
  8C38D36C  4F26  lds.l @r15+,PR
  8C38D36E  000B  rts
  8C38D370  0009  nop
  8C38D372  E500  mov ##0x00,r5
  8C38D374  D32E  mov.l @([8C38D430]),r3
  8C38D376  E040  mov ##0x40,r0
  8C38D378  2432  mov.l r3,@r4
  8C38D37A  E208  mov ##0x08,r2
  8C38D37C  E302  mov ##0x02,r3
  8C38D37E  1451  mov.l r5,@(4,r4)
  8C38D380  E706  mov ##0x06,r7
  8C38D382  1452  mov.l r5,@(8,r4)
  8C38D384  E601  mov ##0x01,r6
  8C38D386  1463  mov.l r6,@(12,r4)
  8C38D388  1454  mov.l r5,@(16,r4)
  8C38D38A  1475  mov.l r7,@(20,r4)
  8C38D38C  1466  mov.l r6,@(24,r4)
  8C38D38E  1457  mov.l r5,@(28,r4)
  8C38D390  1468  mov.l r6,@(32,r4)
  8C38D392  1459  mov.l r5,@(36,r4)
  8C38D394  145A  mov.l r5,@(40,r4)
  8C38D396  142B  mov.l r2,@(44,r4)
  8C38D398  147C  mov.l r7,@(48,r4)
  8C38D39A  145D  mov.l r5,@(52,r4)
  8C38D39C  145E  mov.l r5,@(56,r4)
  8C38D39E  143F  mov.l r3,@(60,r4)
  8C38D3A0  0456  mov.l r5,@(R0,r4)
  8C38D3A2  E044  mov ##0x44,r0
  8C38D3A4  0456  mov.l r5,@(R0,r4)
  8C38D3A6  E048  mov ##0x48,r0
  8C38D3A8  0456  mov.l r5,@(R0,r4)
  8C38D3AA  E050  mov ##0x50,r0
  8C38D3AC  0456  mov.l r5,@(R0,r4)
  8C38D3AE  E04C  mov ##0x4C,r0
  8C38D3B0  0456  mov.l r5,@(R0,r4)
  8C38D3B2  E054  mov ##0x54,r0
  8C38D3B4  F49D  fldi1 fr4
  8C38D3B6  0456  mov.l r5,@(R0,r4)
  8C38D3B8  E058  mov ##0x58,r0
  8C38D3BA  0456  mov.l r5,@(R0,r4)
  8C38D3BC  E05C  mov ##0x5C,r0
  8C38D3BE  E304  mov ##0x04,r3
  8C38D3C0  0436  mov.l r3,@(R0,r4)
  8C38D3C2  E060  mov ##0x60,r0
  8C38D3C4  E203  mov ##0x03,r2
  8C38D3C6  0426  mov.l r2,@(R0,r4)
  8C38D3C8  E06C  mov ##0x6C,r0
```


## Power Stone (USA)

Dump: `/mnt/1TB/dcbat/20261007-160119_Power_Stone__USA__/jit-75898.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```


## Project Justice (USA)

Dump: `/mnt/1TB/dcbat/20261007-160314_Project_Justice__USA__/jit-78773.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```


## Resident Evil - Code - Veronica (USA) (Disc 1)

Dump: `/mnt/1TB/dcbat/20261007-160515_Resident_Evil_-_Code_-_Veronica__USA___D/jit-82021.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```

```
; 8C173854-8C17393C
  8C173854  9247  mov.w @([8C1738E6]),r2
  8C173856  6532  mov.l @r3,r5
  8C173858  EC00  mov ##0x00,r12
  8C17385A  6EC3  mov r12,r14
  8C17385C  045E  mov.l @(R0,r5),r4
  8C17385E  0002  stc SR,r0
  8C173860  4009  shlr2 r0
  8C173862  4009  shlr2 r0
  8C173864  C90F  and ##15,R0
  8C173866  2F02  mov.l r0,@r15
  8C173868  0002  stc SR,r0
  8C17386A  2029  and r2,r0
  8C17386C  CBF0  or ##240,R0
  8C17386E  400E  ldc r0,SR
  8C173870  2558  tst r5,r5
  8C173872  8B02  bf 8C17387A
  ...       (não compilado nesta sessão)
  8C17387A  D51D  mov.l @([8C1738F0]),r5
  8C17387C  E740  mov ##0x40,r7
  8C17387E  66C3  mov r12,r6
  8C173880  6242  mov.l @r4,r2
  8C173882  3250  cmp/eq r5,r2
  8C173884  8B06  bf 8C173894
  8C173886  7601  add ##1,r6
  8C173888  3673  cmp/ge r7,r6
  8C17388A  8FF9  bf.s 8C173880
  8C17388C  7440  add ##64,r4
  ...       (não compilado nesta sessão)
  8C173894  D318  mov.l @([8C1738F8]),r3
  8C173896  2452  mov.l r5,@r4
  8C173898  2D42  mov.l r4,@r13
  8C17389A  430B  jsr @r3
  8C17389C  E500  mov ##0x00,r5
  8C17389E  E700  mov ##0x00,r7
  8C1738A0  D216  mov.l @([8C1738FC]),r2
  8C1738A2  6573  mov r7,r5
  8C1738A4  2FC6  mov.l r12,@-r15
  8C1738A6  6673  mov r7,r6
  8C1738A8  420B  jsr @r2
  8C1738AA  64D2  mov.l @r13,r4
  8C1738AC  7F04  add ##4,r15
  8C1738AE  D20E  mov.l @([8C1738E8]),r2
  8C1738B0  E054  mov ##0x54,r0
  8C1738B2  6322  mov.l @r2,r3
  8C1738B4  013E  mov.l @(R0,r3),r1
  8C1738B6  7101  add ##1,r1
  8C1738B8  0316  mov.l r1,@(R0,r3)
  8C1738BA  60F2  mov.l @r15,r0
  8C1738BC  0302  stc SR,r3
  8C1738BE  9212  mov.w @([8C1738E6]),r2
  8C1738C0  C90F  and ##15,R0
  8C1738C2  4008  shll2 r0
  8C1738C4  2329  and r2,r3
  8C1738C6  4008  shll2 r0
  8C1738C8  203B  or r3,r0
  8C1738CA  400E  ldc r0,SR
  8C1738CC  2EE8  tst r14,r14
  8C1738CE  8903  bt 8C1738D8
  ...       (não compilado nesta sessão)
  8C1738D8  60E3  mov r14,r0
  8C1738DA  7F04  add ##4,r15
  8C1738DC  4F26  lds.l @r15+,PR
  8C1738DE  6CF6  mov.l @r15+,r12
  8C1738E0  6DF6  mov.l @r15+,r13
  8C1738E2  000B  rts
  8C1738E4  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C173908  2FE6  mov.l r14,@-r15
  8C17390A  2FD6  mov.l r13,@-r15
  8C17390C  2FC6  mov.l r12,@-r15
  8C17390E  4F22  sts.l PR,@-r15
  8C173910  7FF8  add ##-8,r15
  8C173912  0002  stc SR,r0
  8C173914  9341  mov.w @([8C17399A]),r3
  8C173916  6C43  mov r4,r12
  8C173918  6D43  mov r4,r13
  8C17391A  4009  shlr2 r0
  8C17391C  4009  shlr2 r0
  8C17391E  C90F  and ##15,R0
  8C173920  1F01  mov.l r0,@(4,r15)
  8C173922  0002  stc SR,r0
  8C173924  2039  and r3,r0
  8C173926  CBF0  or ##240,R0
  8C173928  400E  ldc r0,SR
  8C17392A  D21C  mov.l @([8C17399C]),r2
  8C17392C  420B  jsr @r2
  8C17392E  64C3  mov r12,r4
  8C173930  6E03  mov r0,r14
  8C173932  2EE8  tst r14,r14
  8C173934  8B1B  bf 8C17396E
  8C173936  D31A  mov.l @([8C1739A0]),r3
  8C173938  65F3  mov r15,r5
  8C17393A  430B  jsr @r3
```

```
; 8C17AFFE-8C17B0E6
  8C17AFFE  FE77  fmov.s fr7,@(R0,r14)
  8C17B000  E038  mov ##0x38,r0
  8C17B002  FE67  fmov.s fr6,@(R0,r14)
  8C17B004  E03C  mov ##0x3C,r0
  8C17B006  FE47  fmov.s fr4,@(R0,r14)
  8C17B008  0002  stc SR,r0
  8C17B00A  4009  shlr2 r0
  8C17B00C  4009  shlr2 r0
  8C17B00E  C90F  and ##15,R0
  8C17B010  2F02  mov.l r0,@r15
  8C17B012  0002  stc SR,r0
  8C17B014  2039  and r3,r0
  8C17B016  CBF0  or ##240,R0
  8C17B018  400E  ldc r0,SR
  8C17B01A  B9E9  bsr 8C17A3F0
  8C17B01C  0009  nop
  8C17B01E  D343  mov.l @([8C17B12C]),r3
  8C17B020  D441  mov.l @([8C17B128]),r4
  8C17B022  430B  jsr @r3
  8C17B024  0009  nop
  8C17B026  60F2  mov.l @r15,r0
  8C17B028  0202  stc SR,r2
  8C17B02A  937A  mov.w @([8C17B122]),r3
  8C17B02C  C90F  and ##15,R0
  8C17B02E  4008  shll2 r0
  8C17B030  2239  and r3,r2
  8C17B032  4008  shll2 r0
  8C17B034  202B  or r2,r0
  8C17B036  400E  ldc r0,SR
  8C17B038  D33D  mov.l @([8C17B130]),r3
  8C17B03A  430B  jsr @r3
  8C17B03C  E400  mov ##0x00,r4
  8C17B03E  D23D  mov.l @([8C17B134]),r2
  8C17B040  2202  mov.l r0,@r2
  8C17B042  7F08  add ##8,r15
  8C17B044  4F26  lds.l @r15+,PR
  8C17B046  FFF9  fmov.s @r15+,fr15
  8C17B048  68F6  mov.l @r15+,r8
  8C17B04A  69F6  mov.l @r15+,r9
  8C17B04C  6AF6  mov.l @r15+,r10
  8C17B04E  6BF6  mov.l @r15+,r11
  8C17B050  6CF6  mov.l @r15+,r12
  8C17B052  6DF6  mov.l @r15+,r13
  8C17B054  000B  rts
  8C17B056  6EF6  mov.l @r15+,r14
  8C17B058  4F22  sts.l PR,@-r15
  8C17B05A  7FFC  add ##-4,r15
  8C17B05C  6343  mov r4,r3
  8C17B05E  4300  shll r3
  8C17B060  6243  mov r4,r2
  8C17B062  332C  add r2,r3
  8C17B064  4308  shll2 r3
  8C17B066  D134  mov.l @([8C17B138]),r1
  8C17B068  4308  shll2 r3
  8C17B06A  D234  mov.l @([8C17B13C]),r2
  8C17B06C  4308  shll2 r3
  8C17B06E  2F42  mov.l r4,@r15
  8C17B070  331C  add r1,r3
  8C17B072  2232  mov.l r3,@r2
  8C17B074  D232  mov.l @([8C17B140]),r2
  8C17B076  420B  jsr @r2
  8C17B078  6433  mov r3,r4
  8C17B07A  61F2  mov.l @r15,r1
  8C17B07C  D331  mov.l @([8C17B144]),r3
  8C17B07E  D232  mov.l @([8C17B148]),r2
  8C17B080  4108  shll2 r1
  8C17B082  313C  add r3,r1
  8C17B084  2212  mov.l r1,@r2
  8C17B086  7F04  add ##4,r15
  8C17B088  4F26  lds.l @r15+,PR
  8C17B08A  000B  rts
  8C17B08C  0009  nop
  8C17B08E  E500  mov ##0x00,r5
  8C17B090  D32E  mov.l @([8C17B14C]),r3
  8C17B092  E040  mov ##0x40,r0
  8C17B094  2432  mov.l r3,@r4
  8C17B096  E208  mov ##0x08,r2
  8C17B098  E302  mov ##0x02,r3
  8C17B09A  1451  mov.l r5,@(4,r4)
  8C17B09C  E706  mov ##0x06,r7
  8C17B09E  1452  mov.l r5,@(8,r4)
  8C17B0A0  E601  mov ##0x01,r6
  8C17B0A2  1463  mov.l r6,@(12,r4)
  8C17B0A4  1454  mov.l r5,@(16,r4)
  8C17B0A6  1475  mov.l r7,@(20,r4)
  8C17B0A8  1466  mov.l r6,@(24,r4)
  8C17B0AA  1457  mov.l r5,@(28,r4)
  8C17B0AC  1468  mov.l r6,@(32,r4)
  8C17B0AE  1459  mov.l r5,@(36,r4)
  8C17B0B0  145A  mov.l r5,@(40,r4)
  8C17B0B2  142B  mov.l r2,@(44,r4)
  8C17B0B4  147C  mov.l r7,@(48,r4)
  8C17B0B6  145D  mov.l r5,@(52,r4)
  8C17B0B8  145E  mov.l r5,@(56,r4)
  8C17B0BA  143F  mov.l r3,@(60,r4)
  8C17B0BC  0456  mov.l r5,@(R0,r4)
  8C17B0BE  E044  mov ##0x44,r0
  8C17B0C0  0456  mov.l r5,@(R0,r4)
  8C17B0C2  E048  mov ##0x48,r0
  8C17B0C4  0456  mov.l r5,@(R0,r4)
  8C17B0C6  E050  mov ##0x50,r0
  8C17B0C8  0456  mov.l r5,@(R0,r4)
  8C17B0CA  E04C  mov ##0x4C,r0
  8C17B0CC  0456  mov.l r5,@(R0,r4)
  8C17B0CE  E054  mov ##0x54,r0
  8C17B0D0  F49D  fldi1 fr4
  8C17B0D2  0456  mov.l r5,@(R0,r4)
  8C17B0D4  E058  mov ##0x58,r0
  8C17B0D6  0456  mov.l r5,@(R0,r4)
  8C17B0D8  E05C  mov ##0x5C,r0
  8C17B0DA  E304  mov ##0x04,r3
  8C17B0DC  0436  mov.l r3,@(R0,r4)
  8C17B0DE  E060  mov ##0x60,r0
  8C17B0E0  E203  mov ##0x03,r2
  8C17B0E2  0426  mov.l r2,@(R0,r4)
  8C17B0E4  E06C  mov ##0x6C,r0
```


## Shenmue (USA) (Disc 1)

Dump: `/mnt/1TB/dcbat_off/20261007-191903_Shenmue__USA___Disc_1__/jit-11043.txt`

```
; 0C1D6E3A-0C1D6F22
  0C1D6E3A  430B  jsr @r3
  0C1D6E3C  5445  mov.l @(20,r4),r4
  0C1D6E3E  6E03  mov r0,r14
  0C1D6E40  2EE8  tst r14,r14
  0C1D6E42  892E  bt 0C1D6EA2
  0C1D6E44  0002  stc SR,r0
  0C1D6E46  4009  shlr2 r0
  0C1D6E48  4009  shlr2 r0
  0C1D6E4A  C90F  and ##15,R0
  0C1D6E4C  2F02  mov.l r0,@r15
  0C1D6E4E  0002  stc SR,r0
  0C1D6E50  934A  mov.w @([0C1D6EE8]),r3
  0C1D6E52  2039  and r3,r0
  0C1D6E54  CBF0  or ##240,R0
  0C1D6E56  400E  ldc r0,SR
  0C1D6E58  6DC2  mov.l @r12,r13
  0C1D6E5A  E50E  mov ##0x0E,r5
  0C1D6E5C  66C2  mov.l @r12,r6
  0C1D6E5E  5DD3  mov.l @(12,r13),r13
  0C1D6E60  7640  add ##64,r6
  0C1D6E62  5DDF  mov.l @(60,r13),r13
  0C1D6E64  4D0B  jsr @r13
  0C1D6E66  E401  mov ##0x01,r4
  0C1D6E68  6D03  mov r0,r13
  0C1D6E6A  2DD8  tst r13,r13
  0C1D6E6C  890E  bt 0C1D6E8C
  0C1D6E6E  E050  mov ##0x50,r0
  0C1D6E70  1EDF  mov.l r13,@(60,r14)
  0C1D6E72  E204  mov ##0x04,r2
  0C1D6E74  0E25  mov.w r2,@(R0,r14)
  0C1D6E76  E048  mov ##0x48,r0
  0C1D6E78  E30A  mov ##0x0A,r3
  0C1D6E7A  0E35  mov.w r3,@(R0,r14)
  0C1D6E7C  62E2  mov.l @r14,r2
  0C1D6E7E  12E7  mov.l r14,@(28,r2)
  0C1D6E80  63C2  mov.l @r12,r3
  0C1D6E82  E040  mov ##0x40,r0
  0C1D6E84  5233  mov.l @(12,r3),r2
  0C1D6E86  012E  mov.l @(R0,r2),r1
  0C1D6E88  410B  jsr @r1
  0C1D6E8A  0009  nop
  0C1D6E8C  60F2  mov.l @r15,r0
  0C1D6E8E  0302  stc SR,r3
  0C1D6E90  922A  mov.w @([0C1D6EE8]),r2
  0C1D6E92  C90F  and ##15,R0
  0C1D6E94  4008  shll2 r0
  0C1D6E96  2329  and r2,r3
  0C1D6E98  4008  shll2 r0
  0C1D6E9A  203B  or r3,r0
  0C1D6E9C  400E  ldc r0,SR
  0C1D6E9E  2DD8  tst r13,r13
  0C1D6EA0  8B01  bf 0C1D6EA6
  0C1D6EA2  A001  bra 0C1D6EA8
  0C1D6EA4  E0F3  mov ##0xF3,r0
  0C1D6EA6  E000  mov ##0x00,r0
  0C1D6EA8  7F04  add ##4,r15
  0C1D6EAA  4F26  lds.l @r15+,PR
  0C1D6EAC  6CF6  mov.l @r15+,r12
  0C1D6EAE  6DF6  mov.l @r15+,r13
  0C1D6EB0  000B  rts
  0C1D6EB2  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
```

```
; 0C1D710E-0C1D71F6
  0C1D710E  430B  jsr @r3
  0C1D7110  5445  mov.l @(20,r4),r4
  0C1D7112  6E03  mov r0,r14
  0C1D7114  2EE8  tst r14,r14
  0C1D7116  893B  bt 0C1D7190
  0C1D7118  0002  stc SR,r0
  0C1D711A  4009  shlr2 r0
  0C1D711C  4009  shlr2 r0
  0C1D711E  C90F  and ##15,R0
  0C1D7120  2F02  mov.l r0,@r15
  0C1D7122  0002  stc SR,r0
  0C1D7124  9373  mov.w @([0C1D720E]),r3
  0C1D7126  2039  and r3,r0
  0C1D7128  CBF0  or ##240,R0
  0C1D712A  400E  ldc r0,SR
  0C1D712C  6CD2  mov.l @r13,r12
  0C1D712E  5CC3  mov.l @(12,r12),r12
  0C1D7130  5CCC  mov.l @(48,r12),r12
  0C1D7132  4C0B  jsr @r12
  0C1D7134  0009  nop
  0C1D7136  6C03  mov r0,r12
  0C1D7138  2CC8  tst r12,r12
  0C1D713A  891E  bt 0C1D717A
  0C1D713C  E050  mov ##0x50,r0
  0C1D713E  1ECF  mov.l r12,@(60,r14)
  0C1D7140  E204  mov ##0x04,r2
  0C1D7142  0E25  mov.w r2,@(R0,r14)
  0C1D7144  E03A  mov ##0x3A,r0
  0C1D7146  63D2  mov.l @r13,r3
  0C1D7148  023D  mov.w @(R0,r3),r2
  0C1D714A  2228  tst r2,r2
  0C1D714C  890A  bt 0C1D7164
  ...       (não compilado nesta sessão)
  0C1D7164  E048  mov ##0x48,r0
  0C1D7166  E106  mov ##0x06,r1
  0C1D7168  0E15  mov.w r1,@(R0,r14)
  0C1D716A  63E2  mov.l @r14,r3
  0C1D716C  13E7  mov.l r14,@(28,r3)
  0C1D716E  62D2  mov.l @r13,r2
  0C1D7170  E040  mov ##0x40,r0
  0C1D7172  5323  mov.l @(12,r2),r3
  0C1D7174  013E  mov.l @(R0,r3),r1
  0C1D7176  410B  jsr @r1
  0C1D7178  0009  nop
  0C1D717A  60F2  mov.l @r15,r0
  0C1D717C  0202  stc SR,r2
  0C1D717E  9346  mov.w @([0C1D720E]),r3
  0C1D7180  C90F  and ##15,R0
  0C1D7182  4008  shll2 r0
  0C1D7184  2239  and r3,r2
  0C1D7186  4008  shll2 r0
  0C1D7188  202B  or r2,r0
  0C1D718A  400E  ldc r0,SR
  0C1D718C  2CC8  tst r12,r12
  0C1D718E  8B01  bf 0C1D7194
  ...       (não compilado nesta sessão)
  0C1D7194  E000  mov ##0x00,r0
  0C1D7196  7F04  add ##4,r15
  0C1D7198  4F26  lds.l @r15+,PR
  0C1D719A  6CF6  mov.l @r15+,r12
  0C1D719C  6DF6  mov.l @r15+,r13
  0C1D719E  000B  rts
  0C1D71A0  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
```

```
; 0C1D7D96-0C1D7E7E
  ...       (não compilado nesta sessão)
  0C1D7DA0  0002  stc SR,r0
  0C1D7DA2  4009  shlr2 r0
  0C1D7DA4  4009  shlr2 r0
  0C1D7DA6  C90F  and ##15,R0
  0C1D7DA8  2F02  mov.l r0,@r15
  0C1D7DAA  0002  stc SR,r0
  0C1D7DAC  938F  mov.w @([0C1D7ECE]),r3
  0C1D7DAE  2039  and r3,r0
  0C1D7DB0  CBF0  or ##240,R0
  0C1D7DB2  400E  ldc r0,SR
  0C1D7DB4  62E2  mov.l @r14,r2
  0C1D7DB6  E04E  mov ##0x4E,r0
  0C1D7DB8  07ED  mov.w @(R0,r14),r7
  0C1D7DBA  65D3  mov r13,r5
  0C1D7DBC  5323  mov.l @(12,r2),r3
  0C1D7DBE  5131  mov.l @(4,r3),r1
  0C1D7DC0  410B  jsr @r1
  0C1D7DC2  64C3  mov r12,r4
  0C1D7DC4  6C03  mov r0,r12
  0C1D7DC6  2CC8  tst r12,r12
  0C1D7DC8  8913  bt 0C1D7DF2
  0C1D7DCA  E202  mov ##0x02,r2
  0C1D7DCC  1ECF  mov.l r12,@(60,r14)
  0C1D7DCE  E050  mov ##0x50,r0
  0C1D7DD0  1ED7  mov.l r13,@(28,r14)
  0C1D7DD2  0E25  mov.w r2,@(R0,r14)
  0C1D7DD4  E04A  mov ##0x4A,r0
  0C1D7DD6  0EB5  mov.w r11,@(R0,r14)
  0C1D7DD8  E048  mov ##0x48,r0
  0C1D7DDA  0EB5  mov.w r11,@(R0,r14)
  0C1D7DDC  1EA6  mov.l r10,@(24,r14)
  0C1D7DDE  63E2  mov.l @r14,r3
  0C1D7DE0  13E7  mov.l r14,@(28,r3)
  0C1D7DE2  62E2  mov.l @r14,r2
  0C1D7DE4  E040  mov ##0x40,r0
  0C1D7DE6  5323  mov.l @(12,r2),r3
  0C1D7DE8  013E  mov.l @(R0,r3),r1
  0C1D7DEA  410B  jsr @r1
  0C1D7DEC  0009  nop
  0C1D7DEE  A002  bra 0C1D7DF6
  0C1D7DF0  0009  nop
  ...       (não compilado nesta sessão)
  0C1D7DF6  60F2  mov.l @r15,r0
  0C1D7DF8  0302  stc SR,r3
  0C1D7DFA  9268  mov.w @([0C1D7ECE]),r2
  0C1D7DFC  C90F  and ##15,R0
  0C1D7DFE  4008  shll2 r0
  0C1D7E00  2329  and r2,r3
  0C1D7E02  4008  shll2 r0
  0C1D7E04  203B  or r3,r0
  0C1D7E06  400E  ldc r0,SR
  0C1D7E08  2CC8  tst r12,r12
  0C1D7E0A  8901  bt 0C1D7E10
  0C1D7E0C  A001  bra 0C1D7E12
  0C1D7E0E  60D3  mov r13,r0
  ...       (não compilado nesta sessão)
  0C1D7E12  7F04  add ##4,r15
  0C1D7E14  4F26  lds.l @r15+,PR
  0C1D7E16  6AF6  mov.l @r15+,r10
  0C1D7E18  6BF6  mov.l @r15+,r11
  0C1D7E1A  6CF6  mov.l @r15+,r12
  0C1D7E1C  6DF6  mov.l @r15+,r13
  0C1D7E1E  000B  rts
  0C1D7E20  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
```

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```


## Shenmue II (Europe) (En,Fr,De,Es) (Disc 1)

Dump: `/mnt/1TB/dcbat_off/20261007-192124_Shenmue_II__Europe___En_Fr_De_Es___Disc_/jit-12387.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```


## Skies of Arcadia (USA) (Disc 1)

Dump: `/mnt/1TB/dcbat/20261007-162126_Skies_of_Arcadia__USA___Disc_1__/jit-98449.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```

```
; 8C25104C-8C251134
  8C25104C  9247  mov.w @([8C2510DE]),r2
  8C25104E  6532  mov.l @r3,r5
  8C251050  EC00  mov ##0x00,r12
  8C251052  6EC3  mov r12,r14
  8C251054  045E  mov.l @(R0,r5),r4
  8C251056  0002  stc SR,r0
  8C251058  4009  shlr2 r0
  8C25105A  4009  shlr2 r0
  8C25105C  C90F  and ##15,R0
  8C25105E  2F02  mov.l r0,@r15
  8C251060  0002  stc SR,r0
  8C251062  2029  and r2,r0
  8C251064  CBF0  or ##240,R0
  8C251066  400E  ldc r0,SR
  8C251068  2558  tst r5,r5
  8C25106A  8B02  bf 8C251072
  ...       (não compilado nesta sessão)
  8C251072  D51D  mov.l @([8C2510E8]),r5
  8C251074  E740  mov ##0x40,r7
  8C251076  66C3  mov r12,r6
  8C251078  6242  mov.l @r4,r2
  8C25107A  3250  cmp/eq r5,r2
  8C25107C  8B06  bf 8C25108C
  8C25107E  7601  add ##1,r6
  8C251080  3673  cmp/ge r7,r6
  8C251082  8FF9  bf.s 8C251078
  8C251084  7440  add ##64,r4
  ...       (não compilado nesta sessão)
  8C25108C  D318  mov.l @([8C2510F0]),r3
  8C25108E  2452  mov.l r5,@r4
  8C251090  2D42  mov.l r4,@r13
  8C251092  430B  jsr @r3
  8C251094  E500  mov ##0x00,r5
  8C251096  E700  mov ##0x00,r7
  8C251098  D216  mov.l @([8C2510F4]),r2
  8C25109A  6573  mov r7,r5
  8C25109C  2FC6  mov.l r12,@-r15
  8C25109E  6673  mov r7,r6
  8C2510A0  420B  jsr @r2
  8C2510A2  64D2  mov.l @r13,r4
  8C2510A4  7F04  add ##4,r15
  8C2510A6  D20E  mov.l @([8C2510E0]),r2
  8C2510A8  E054  mov ##0x54,r0
  8C2510AA  6322  mov.l @r2,r3
  8C2510AC  013E  mov.l @(R0,r3),r1
  8C2510AE  7101  add ##1,r1
  8C2510B0  0316  mov.l r1,@(R0,r3)
  8C2510B2  60F2  mov.l @r15,r0
  8C2510B4  0302  stc SR,r3
  8C2510B6  9212  mov.w @([8C2510DE]),r2
  8C2510B8  C90F  and ##15,R0
  8C2510BA  4008  shll2 r0
  8C2510BC  2329  and r2,r3
  8C2510BE  4008  shll2 r0
  8C2510C0  203B  or r3,r0
  8C2510C2  400E  ldc r0,SR
  8C2510C4  2EE8  tst r14,r14
  8C2510C6  8903  bt 8C2510D0
  ...       (não compilado nesta sessão)
  8C2510D0  60E3  mov r14,r0
  8C2510D2  7F04  add ##4,r15
  8C2510D4  4F26  lds.l @r15+,PR
  8C2510D6  6CF6  mov.l @r15+,r12
  8C2510D8  6DF6  mov.l @r15+,r13
  8C2510DA  000B  rts
  8C2510DC  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C251100  2FE6  mov.l r14,@-r15
  8C251102  2FD6  mov.l r13,@-r15
  8C251104  2FC6  mov.l r12,@-r15
  8C251106  2FB6  mov.l r11,@-r15
  8C251108  2FA6  mov.l r10,@-r15
  8C25110A  4F22  sts.l PR,@-r15
  8C25110C  7FF8  add ##-8,r15
  8C25110E  0002  stc SR,r0
  8C251110  9357  mov.w @([8C2511C2]),r3
  8C251112  6A43  mov r4,r10
  8C251114  6D43  mov r4,r13
  8C251116  4009  shlr2 r0
  8C251118  4009  shlr2 r0
  8C25111A  C90F  and ##15,R0
  8C25111C  1F01  mov.l r0,@(4,r15)
  8C25111E  0002  stc SR,r0
  8C251120  2039  and r3,r0
  8C251122  CBF0  or ##240,R0
  8C251124  400E  ldc r0,SR
  8C251126  D228  mov.l @([8C2511C8]),r2
  8C251128  420B  jsr @r2
  8C25112A  64A3  mov r10,r4
  8C25112C  6E03  mov r0,r14
  8C25112E  2EE8  tst r14,r14
  8C251130  8B2F  bf 8C251192
  8C251132  DC26  mov.l @([8C2511CC]),r12
```

```
; 8C25A512-8C25A5FA
  8C25A512  FE77  fmov.s fr7,@(R0,r14)
  8C25A514  E038  mov ##0x38,r0
  8C25A516  FE67  fmov.s fr6,@(R0,r14)
  8C25A518  E03C  mov ##0x3C,r0
  8C25A51A  FE47  fmov.s fr4,@(R0,r14)
  8C25A51C  0002  stc SR,r0
  8C25A51E  4009  shlr2 r0
  8C25A520  4009  shlr2 r0
  8C25A522  C90F  and ##15,R0
  8C25A524  2F02  mov.l r0,@r15
  8C25A526  0002  stc SR,r0
  8C25A528  2039  and r3,r0
  8C25A52A  CBF0  or ##240,R0
  8C25A52C  400E  ldc r0,SR
  8C25A52E  B9E9  bsr 8C259904
  8C25A530  0009  nop
  8C25A532  D343  mov.l @([8C25A640]),r3
  8C25A534  D441  mov.l @([8C25A63C]),r4
  8C25A536  430B  jsr @r3
  8C25A538  0009  nop
  8C25A53A  60F2  mov.l @r15,r0
  8C25A53C  0202  stc SR,r2
  8C25A53E  937A  mov.w @([8C25A636]),r3
  8C25A540  C90F  and ##15,R0
  8C25A542  4008  shll2 r0
  8C25A544  2239  and r3,r2
  8C25A546  4008  shll2 r0
  8C25A548  202B  or r2,r0
  8C25A54A  400E  ldc r0,SR
  8C25A54C  D33D  mov.l @([8C25A644]),r3
  8C25A54E  430B  jsr @r3
  8C25A550  E400  mov ##0x00,r4
  8C25A552  D23D  mov.l @([8C25A648]),r2
  8C25A554  2202  mov.l r0,@r2
  8C25A556  7F08  add ##8,r15
  8C25A558  4F26  lds.l @r15+,PR
  8C25A55A  FFF9  fmov.s @r15+,fr15
  8C25A55C  68F6  mov.l @r15+,r8
  8C25A55E  69F6  mov.l @r15+,r9
  8C25A560  6AF6  mov.l @r15+,r10
  8C25A562  6BF6  mov.l @r15+,r11
  8C25A564  6CF6  mov.l @r15+,r12
  8C25A566  6DF6  mov.l @r15+,r13
  8C25A568  000B  rts
  8C25A56A  6EF6  mov.l @r15+,r14
  8C25A56C  4F22  sts.l PR,@-r15
  8C25A56E  7FFC  add ##-4,r15
  8C25A570  6343  mov r4,r3
  8C25A572  4300  shll r3
  8C25A574  6243  mov r4,r2
  8C25A576  332C  add r2,r3
  8C25A578  4308  shll2 r3
  8C25A57A  D134  mov.l @([8C25A64C]),r1
  8C25A57C  4308  shll2 r3
  8C25A57E  D234  mov.l @([8C25A650]),r2
  8C25A580  4308  shll2 r3
  8C25A582  2F42  mov.l r4,@r15
  8C25A584  331C  add r1,r3
  8C25A586  2232  mov.l r3,@r2
  8C25A588  D232  mov.l @([8C25A654]),r2
  8C25A58A  420B  jsr @r2
  8C25A58C  6433  mov r3,r4
  8C25A58E  61F2  mov.l @r15,r1
  8C25A590  D331  mov.l @([8C25A658]),r3
  8C25A592  D232  mov.l @([8C25A65C]),r2
  8C25A594  4108  shll2 r1
  8C25A596  313C  add r3,r1
  8C25A598  2212  mov.l r1,@r2
  8C25A59A  7F04  add ##4,r15
  8C25A59C  4F26  lds.l @r15+,PR
  8C25A59E  000B  rts
  8C25A5A0  0009  nop
  8C25A5A2  E500  mov ##0x00,r5
  8C25A5A4  D32E  mov.l @([8C25A660]),r3
  8C25A5A6  E040  mov ##0x40,r0
  8C25A5A8  2432  mov.l r3,@r4
  8C25A5AA  E208  mov ##0x08,r2
  8C25A5AC  E302  mov ##0x02,r3
  8C25A5AE  1451  mov.l r5,@(4,r4)
  8C25A5B0  E706  mov ##0x06,r7
  8C25A5B2  1452  mov.l r5,@(8,r4)
  8C25A5B4  E601  mov ##0x01,r6
  8C25A5B6  1463  mov.l r6,@(12,r4)
  8C25A5B8  1454  mov.l r5,@(16,r4)
  8C25A5BA  1475  mov.l r7,@(20,r4)
  8C25A5BC  1466  mov.l r6,@(24,r4)
  8C25A5BE  1457  mov.l r5,@(28,r4)
  8C25A5C0  1468  mov.l r6,@(32,r4)
  8C25A5C2  1459  mov.l r5,@(36,r4)
  8C25A5C4  145A  mov.l r5,@(40,r4)
  8C25A5C6  142B  mov.l r2,@(44,r4)
  8C25A5C8  147C  mov.l r7,@(48,r4)
  8C25A5CA  145D  mov.l r5,@(52,r4)
  8C25A5CC  145E  mov.l r5,@(56,r4)
  8C25A5CE  143F  mov.l r3,@(60,r4)
  8C25A5D0  0456  mov.l r5,@(R0,r4)
  8C25A5D2  E044  mov ##0x44,r0
  8C25A5D4  0456  mov.l r5,@(R0,r4)
  8C25A5D6  E048  mov ##0x48,r0
  8C25A5D8  0456  mov.l r5,@(R0,r4)
  8C25A5DA  E050  mov ##0x50,r0
  8C25A5DC  0456  mov.l r5,@(R0,r4)
  8C25A5DE  E04C  mov ##0x4C,r0
  8C25A5E0  0456  mov.l r5,@(R0,r4)
  8C25A5E2  E054  mov ##0x54,r0
  8C25A5E4  F49D  fldi1 fr4
  8C25A5E6  0456  mov.l r5,@(R0,r4)
  8C25A5E8  E058  mov ##0x58,r0
  8C25A5EA  0456  mov.l r5,@(R0,r4)
  8C25A5EC  E05C  mov ##0x5C,r0
  8C25A5EE  E304  mov ##0x04,r3
  8C25A5F0  0436  mov.l r3,@(R0,r4)
  8C25A5F2  E060  mov ##0x60,r0
  8C25A5F4  E203  mov ##0x03,r2
  8C25A5F6  0426  mov.l r2,@(R0,r4)
  8C25A5F8  E06C  mov ##0x6C,r0
```


## Sonic Adventure 2 (USA) (EnJaFrDeEs)

Dump: `/mnt/1TB/dcbat/20261007-163253_Sonic_Adventure_2__USA___EnJaFrDeEs__/jit-102386.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```

```
; 8C0BF31C-8C0BF404
  8C0BF31C  7FE8  add ##-24,r15
  8C0BF31E  1F45  mov.l r4,@(20,r15)
  8C0BF320  1F54  mov.l r5,@(16,r15)
  8C0BF322  1F63  mov.l r6,@(12,r15)
  8C0BF324  1F72  mov.l r7,@(8,r15)
  8C0BF326  0002  stc SR,r0
  8C0BF328  4009  shlr2 r0
  8C0BF32A  4009  shlr2 r0
  8C0BF32C  C90F  and ##15,R0
  8C0BF32E  2F02  mov.l r0,@r15
  8C0BF330  0002  stc SR,r0
  8C0BF332  9331  mov.w @([8C0BF398]),r3
  8C0BF334  2039  and r3,r0
  8C0BF336  CBF0  or ##240,R0
  8C0BF338  400E  ldc r0,SR
  8C0BF33A  52F4  mov.l @(16,r15),r2
  8C0BF33C  D117  mov.l @([8C0BF39C]),r1
  8C0BF33E  2122  mov.l r2,@r1
  8C0BF340  53F3  mov.l @(12,r15),r3
  8C0BF342  D217  mov.l @([8C0BF3A0]),r2
  8C0BF344  2232  mov.l r3,@r2
  8C0BF346  E300  mov ##0x00,r3
  8C0BF348  D016  mov.l @([8C0BF3A4]),r0
  8C0BF34A  2032  mov.l r3,@r0
  8C0BF34C  E100  mov ##0x00,r1
  8C0BF34E  D316  mov.l @([8C0BF3A8]),r3
  8C0BF350  2312  mov.l r1,@r3
  8C0BF352  E200  mov ##0x00,r2
  8C0BF354  D115  mov.l @([8C0BF3AC]),r1
  8C0BF356  2122  mov.l r2,@r1
  8C0BF358  E000  mov ##0x00,r0
  8C0BF35A  D215  mov.l @([8C0BF3B0]),r2
  8C0BF35C  2202  mov.l r0,@r2
  8C0BF35E  54F4  mov.l @(16,r15),r4
  8C0BF360  D314  mov.l @([8C0BF3B4]),r3
  8C0BF362  430B  jsr @r3
  8C0BF364  0009  nop
  8C0BF366  56F3  mov.l @(12,r15),r6
  8C0BF368  55F4  mov.l @(16,r15),r5
  8C0BF36A  54F5  mov.l @(20,r15),r4
  8C0BF36C  D312  mov.l @([8C0BF3B8]),r3
  8C0BF36E  430B  jsr @r3
  8C0BF370  0009  nop
  8C0BF372  1F01  mov.l r0,@(4,r15)
  8C0BF374  D511  mov.l @([8C0BF3BC]),r5
  8C0BF376  D312  mov.l @([8C0BF3C0]),r3
  8C0BF378  6432  mov.l @r3,r4
  8C0BF37A  D212  mov.l @([8C0BF3C4]),r2
  8C0BF37C  420B  jsr @r2
  8C0BF37E  0009  nop
  8C0BF380  51F1  mov.l @(4,r15),r1
  8C0BF382  2118  tst r1,r1
  8C0BF384  8B05  bf 8C0BF392
  8C0BF386  52F2  mov.l @(8,r15),r2
  8C0BF388  2228  tst r2,r2
  8C0BF38A  8902  bt 8C0BF392
  8C0BF38C  52F2  mov.l @(8,r15),r2
  8C0BF38E  420B  jsr @r2
  8C0BF390  0009  nop
  8C0BF392  E301  mov ##0x01,r3
  8C0BF394  A018  bra 8C0BF3C8
  8C0BF396  0009  nop
  ...       (não compilado nesta sessão)
  8C0BF3C8  D119  mov.l @([8C0BF430]),r1
  8C0BF3CA  2132  mov.l r3,@r1
  8C0BF3CC  60F2  mov.l @r15,r0
  8C0BF3CE  C90F  and ##15,R0
  8C0BF3D0  4008  shll2 r0
  8C0BF3D2  4008  shll2 r0
  8C0BF3D4  0202  stc SR,r2
  8C0BF3D6  932A  mov.w @([8C0BF42E]),r3
  8C0BF3D8  2239  and r3,r2
  8C0BF3DA  202B  or r2,r0
  8C0BF3DC  400E  ldc r0,SR
  8C0BF3DE  50F1  mov.l @(4,r15),r0
  8C0BF3E0  7F18  add ##24,r15
  8C0BF3E2  4F26  lds.l @r15+,PR
  8C0BF3E4  000B  rts
  8C0BF3E6  0009  nop
  ...       (não compilado nesta sessão)
```

```
; 8C11265A-8C112742
  8C11265A  FE77  fmov.s fr7,@(R0,r14)
  8C11265C  E038  mov ##0x38,r0
  8C11265E  FE67  fmov.s fr6,@(R0,r14)
  8C112660  E03C  mov ##0x3C,r0
  8C112662  FE47  fmov.s fr4,@(R0,r14)
  8C112664  0002  stc SR,r0
  8C112666  4009  shlr2 r0
  8C112668  4009  shlr2 r0
  8C11266A  C90F  and ##15,R0
  8C11266C  2F02  mov.l r0,@r15
  8C11266E  0002  stc SR,r0
  8C112670  2039  and r3,r0
  8C112672  CBF0  or ##240,R0
  8C112674  400E  ldc r0,SR
  8C112676  B9E9  bsr 8C111A4C
  8C112678  0009  nop
  8C11267A  D343  mov.l @([8C112788]),r3
  8C11267C  D441  mov.l @([8C112784]),r4
  8C11267E  430B  jsr @r3
  8C112680  0009  nop
  8C112682  60F2  mov.l @r15,r0
  8C112684  0202  stc SR,r2
  8C112686  937A  mov.w @([8C11277E]),r3
  8C112688  C90F  and ##15,R0
  8C11268A  4008  shll2 r0
  8C11268C  2239  and r3,r2
  8C11268E  4008  shll2 r0
  8C112690  202B  or r2,r0
  8C112692  400E  ldc r0,SR
  8C112694  D33D  mov.l @([8C11278C]),r3
  8C112696  430B  jsr @r3
  8C112698  E400  mov ##0x00,r4
  8C11269A  D23D  mov.l @([8C112790]),r2
  8C11269C  2202  mov.l r0,@r2
  8C11269E  7F08  add ##8,r15
  8C1126A0  4F26  lds.l @r15+,PR
  8C1126A2  FFF9  fmov.s @r15+,fr15
  8C1126A4  68F6  mov.l @r15+,r8
  8C1126A6  69F6  mov.l @r15+,r9
  8C1126A8  6AF6  mov.l @r15+,r10
  8C1126AA  6BF6  mov.l @r15+,r11
  8C1126AC  6CF6  mov.l @r15+,r12
  8C1126AE  6DF6  mov.l @r15+,r13
  8C1126B0  000B  rts
  8C1126B2  6EF6  mov.l @r15+,r14
  8C1126B4  4F22  sts.l PR,@-r15
  8C1126B6  7FFC  add ##-4,r15
  8C1126B8  6343  mov r4,r3
  8C1126BA  4300  shll r3
  8C1126BC  6243  mov r4,r2
  8C1126BE  332C  add r2,r3
  8C1126C0  4308  shll2 r3
  8C1126C2  D134  mov.l @([8C112794]),r1
  8C1126C4  4308  shll2 r3
  8C1126C6  D234  mov.l @([8C112798]),r2
  8C1126C8  4308  shll2 r3
  8C1126CA  2F42  mov.l r4,@r15
  8C1126CC  331C  add r1,r3
  8C1126CE  2232  mov.l r3,@r2
  8C1126D0  D232  mov.l @([8C11279C]),r2
  8C1126D2  420B  jsr @r2
  8C1126D4  6433  mov r3,r4
  8C1126D6  61F2  mov.l @r15,r1
  8C1126D8  D331  mov.l @([8C1127A0]),r3
  8C1126DA  D232  mov.l @([8C1127A4]),r2
  8C1126DC  4108  shll2 r1
  8C1126DE  313C  add r3,r1
  8C1126E0  2212  mov.l r1,@r2
  8C1126E2  7F04  add ##4,r15
  8C1126E4  4F26  lds.l @r15+,PR
  8C1126E6  000B  rts
  8C1126E8  0009  nop
  8C1126EA  E500  mov ##0x00,r5
  8C1126EC  D32E  mov.l @([8C1127A8]),r3
  8C1126EE  E040  mov ##0x40,r0
  8C1126F0  2432  mov.l r3,@r4
  8C1126F2  E208  mov ##0x08,r2
  8C1126F4  E302  mov ##0x02,r3
  8C1126F6  1451  mov.l r5,@(4,r4)
  8C1126F8  E706  mov ##0x06,r7
  8C1126FA  1452  mov.l r5,@(8,r4)
  8C1126FC  E601  mov ##0x01,r6
  8C1126FE  1463  mov.l r6,@(12,r4)
  8C112700  1454  mov.l r5,@(16,r4)
  8C112702  1475  mov.l r7,@(20,r4)
  8C112704  1466  mov.l r6,@(24,r4)
  8C112706  1457  mov.l r5,@(28,r4)
  8C112708  1468  mov.l r6,@(32,r4)
  8C11270A  1459  mov.l r5,@(36,r4)
  8C11270C  145A  mov.l r5,@(40,r4)
  8C11270E  142B  mov.l r2,@(44,r4)
  8C112710  147C  mov.l r7,@(48,r4)
  8C112712  145D  mov.l r5,@(52,r4)
  8C112714  145E  mov.l r5,@(56,r4)
  8C112716  143F  mov.l r3,@(60,r4)
  8C112718  0456  mov.l r5,@(R0,r4)
  8C11271A  E044  mov ##0x44,r0
  8C11271C  0456  mov.l r5,@(R0,r4)
  8C11271E  E048  mov ##0x48,r0
  8C112720  0456  mov.l r5,@(R0,r4)
  8C112722  E050  mov ##0x50,r0
  8C112724  0456  mov.l r5,@(R0,r4)
  8C112726  E04C  mov ##0x4C,r0
  8C112728  0456  mov.l r5,@(R0,r4)
  8C11272A  E054  mov ##0x54,r0
  8C11272C  F49D  fldi1 fr4
  8C11272E  0456  mov.l r5,@(R0,r4)
  8C112730  E058  mov ##0x58,r0
  8C112732  0456  mov.l r5,@(R0,r4)
  8C112734  E05C  mov ##0x5C,r0
  8C112736  E304  mov ##0x04,r3
  8C112738  0436  mov.l r3,@(R0,r4)
  8C11273A  E060  mov ##0x60,r0
  8C11273C  E203  mov ##0x03,r2
  8C11273E  0426  mov.l r2,@(R0,r4)
  8C112740  E06C  mov ##0x6C,r0
```

```
; 8C12E5F0-8C12E6D8
  8C12E5F0  9247  mov.w @([8C12E682]),r2
  8C12E5F2  6532  mov.l @r3,r5
  8C12E5F4  EC00  mov ##0x00,r12
  8C12E5F6  6EC3  mov r12,r14
  8C12E5F8  045E  mov.l @(R0,r5),r4
  8C12E5FA  0002  stc SR,r0
  8C12E5FC  4009  shlr2 r0
  8C12E5FE  4009  shlr2 r0
  8C12E600  C90F  and ##15,R0
  8C12E602  2F02  mov.l r0,@r15
  8C12E604  0002  stc SR,r0
  8C12E606  2029  and r2,r0
  8C12E608  CBF0  or ##240,R0
  8C12E60A  400E  ldc r0,SR
  8C12E60C  2558  tst r5,r5
  8C12E60E  8B02  bf 8C12E616
  ...       (não compilado nesta sessão)
  8C12E616  D51D  mov.l @([8C12E68C]),r5
  8C12E618  E740  mov ##0x40,r7
  8C12E61A  66C3  mov r12,r6
  8C12E61C  6242  mov.l @r4,r2
  8C12E61E  3250  cmp/eq r5,r2
  8C12E620  8B06  bf 8C12E630
  8C12E622  7601  add ##1,r6
  8C12E624  3673  cmp/ge r7,r6
  8C12E626  8FF9  bf.s 8C12E61C
  8C12E628  7440  add ##64,r4
  ...       (não compilado nesta sessão)
  8C12E630  D318  mov.l @([8C12E694]),r3
  8C12E632  2452  mov.l r5,@r4
  8C12E634  2D42  mov.l r4,@r13
  8C12E636  430B  jsr @r3
  8C12E638  E500  mov ##0x00,r5
  8C12E63A  E700  mov ##0x00,r7
  8C12E63C  D216  mov.l @([8C12E698]),r2
  8C12E63E  6573  mov r7,r5
  8C12E640  2FC6  mov.l r12,@-r15
  8C12E642  6673  mov r7,r6
  8C12E644  420B  jsr @r2
  8C12E646  64D2  mov.l @r13,r4
  8C12E648  7F04  add ##4,r15
  8C12E64A  D20E  mov.l @([8C12E684]),r2
  8C12E64C  E054  mov ##0x54,r0
  8C12E64E  6322  mov.l @r2,r3
  8C12E650  013E  mov.l @(R0,r3),r1
  8C12E652  7101  add ##1,r1
  8C12E654  0316  mov.l r1,@(R0,r3)
  8C12E656  60F2  mov.l @r15,r0
  8C12E658  0302  stc SR,r3
  8C12E65A  9212  mov.w @([8C12E682]),r2
  8C12E65C  C90F  and ##15,R0
  8C12E65E  4008  shll2 r0
  8C12E660  2329  and r2,r3
  8C12E662  4008  shll2 r0
  8C12E664  203B  or r3,r0
  8C12E666  400E  ldc r0,SR
  8C12E668  2EE8  tst r14,r14
  8C12E66A  8903  bt 8C12E674
  ...       (não compilado nesta sessão)
  8C12E674  60E3  mov r14,r0
  8C12E676  7F04  add ##4,r15
  8C12E678  4F26  lds.l @r15+,PR
  8C12E67A  6CF6  mov.l @r15+,r12
  8C12E67C  6DF6  mov.l @r15+,r13
  8C12E67E  000B  rts
  8C12E680  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C12E6A4  2FE6  mov.l r14,@-r15
  8C12E6A6  2FD6  mov.l r13,@-r15
  8C12E6A8  2FC6  mov.l r12,@-r15
  8C12E6AA  2FB6  mov.l r11,@-r15
  8C12E6AC  2FA6  mov.l r10,@-r15
  8C12E6AE  2F96  mov.l r9,@-r15
  8C12E6B0  2F86  mov.l r8,@-r15
  8C12E6B2  4F22  sts.l PR,@-r15
  8C12E6B4  7FF4  add ##-12,r15
  8C12E6B6  0002  stc SR,r0
  8C12E6B8  9363  mov.w @([8C12E782]),r3
  8C12E6BA  6943  mov r4,r9
  8C12E6BC  6D43  mov r4,r13
  8C12E6BE  4009  shlr2 r0
  8C12E6C0  4009  shlr2 r0
  8C12E6C2  C90F  and ##15,R0
  8C12E6C4  1F02  mov.l r0,@(8,r15)
  8C12E6C6  0002  stc SR,r0
  8C12E6C8  2039  and r3,r0
  8C12E6CA  CBF0  or ##240,R0
  8C12E6CC  400E  ldc r0,SR
  8C12E6CE  D22E  mov.l @([8C12E788]),r2
  8C12E6D0  420B  jsr @r2
  8C12E6D2  6493  mov r9,r4
  8C12E6D4  6E03  mov r0,r14
  8C12E6D6  2EE8  tst r14,r14
```


## Soulcalibur (USA)

Dump: `/mnt/1TB/dcbat/20261007-163634_Soulcalibur__USA__/jit-108030.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```


## Tomb Raider Chronicles (USA)

Dump: `/mnt/1TB/dcbat_off/20261007-192238_Tomb_Raider_Chronicles__USA__/jit-13420.txt`

```
; 8C0083F8-8C0084E0
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
```

```
; 8C008456-8C00853E
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
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
  ...       (não compilado nesta sessão)
  8C00853C  4F22  sts.l PR,@-r15
```


## cvs2

Dump: `/mnt/1TB/dcbat/20261007-150106_cvs2_/jit-1905.txt`

```
; 0C153D5E-0C153E46
  ...       (não compilado nesta sessão)
  0C153D60  4F22  sts.l PR,@-r15
  0C153D62  7FF0  add ##-16,r15
  0C153D64  1F43  mov.l r4,@(12,r15)
  0C153D66  1F52  mov.l r5,@(8,r15)
  0C153D68  0002  stc SR,r0
  0C153D6A  4009  shlr2 r0
  0C153D6C  4009  shlr2 r0
  0C153D6E  C90F  and ##15,R0
  0C153D70  2F02  mov.l r0,@r15
  0C153D72  0002  stc SR,r0
  0C153D74  9324  mov.w @([0C153DC0]),r3
  0C153D76  2039  and r3,r0
  0C153D78  CBF0  or ##240,R0
  0C153D7A  400E  ldc r0,SR
  0C153D7C  D311  mov.l @([0C153DC4]),r3
  0C153D7E  D212  mov.l @([0C153DC8]),r2
  0C153D80  6132  mov.l @r3,r1
  0C153D82  212B  or r2,r1
  0C153D84  2312  mov.l r1,@r3
  0C153D86  BF23  bsr 0C153BD0
  0C153D88  0009  nop
  0C153D8A  D710  mov.l @([0C153DCC]),r7
  0C153D8C  56F3  mov.l @(12,r15),r6
  0C153D8E  D50F  mov.l @([0C153DCC]),r5
  0C153D90  54F2  mov.l @(8,r15),r4
  0C153D92  D30F  mov.l @([0C153DD0]),r3
  0C153D94  430B  jsr @r3
  0C153D96  0009  nop
  0C153D98  1F01  mov.l r0,@(4,r15)
  0C153D9A  BEC1  bsr 0C153B20
  0C153D9C  0009  nop
  0C153D9E  E201  mov ##0x01,r2
  0C153DA0  D30C  mov.l @([0C153DD4]),r3
  0C153DA2  2322  mov.l r2,@r3
  0C153DA4  60F2  mov.l @r15,r0
  0C153DA6  C90F  and ##15,R0
  0C153DA8  4008  shll2 r0
  0C153DAA  4008  shll2 r0
  0C153DAC  0102  stc SR,r1
  0C153DAE  9207  mov.w @([0C153DC0]),r2
  0C153DB0  2129  and r2,r1
  0C153DB2  201B  or r1,r0
  0C153DB4  400E  ldc r0,SR
  0C153DB6  50F1  mov.l @(4,r15),r0
  0C153DB8  7F10  add ##16,r15
  0C153DBA  4F26  lds.l @r15+,PR
  0C153DBC  000B  rts
  0C153DBE  0009  nop
  ...       (não compilado nesta sessão)
```

```
; 0C1542E4-0C1543CC
  0C1542E4  2FA6  mov.l r10,@-r15
  0C1542E6  2F96  mov.l r9,@-r15
  0C1542E8  2F86  mov.l r8,@-r15
  0C1542EA  4F22  sts.l PR,@-r15
  0C1542EC  7FE8  add ##-24,r15
  0C1542EE  0002  stc SR,r0
  0C1542F0  4009  shlr2 r0
  0C1542F2  4009  shlr2 r0
  0C1542F4  C90F  and ##15,R0
  0C1542F6  2F02  mov.l r0,@r15
  0C1542F8  0002  stc SR,r0
  0C1542FA  9321  mov.w @([0C154340]),r3
  0C1542FC  2039  and r3,r0
  0C1542FE  CBF0  or ##240,R0
  0C154300  400E  ldc r0,SR
  0C154302  D210  mov.l @([0C154344]),r2
  0C154304  420B  jsr @r2
  0C154306  0009  nop
  0C154308  1F04  mov.l r0,@(16,r15)
  0C15430A  88FE  cmp/eq ##0xFE,R0
  0C15430C  8B28  bf 0C154360
  ...       (não compilado nesta sessão)
  0C154360  53F4  mov.l @(16,r15),r3
  0C154362  4311  cmp/pz r3
  0C154364  8901  bt 0C15436A
  ...       (não compilado nesta sessão)
  0C15436A  D11E  mov.l @([0C1543E4]),r1
  0C15436C  410B  jsr @r1
  0C15436E  0009  nop
  0C154370  D11D  mov.l @([0C1543E8]),r1
  0C154372  6212  mov.l @r1,r2
  0C154374  4200  shll r2
  0C154376  9333  mov.w @([0C1543E0]),r3
  0C154378  D11C  mov.l @([0C1543EC]),r1
  0C15437A  6012  mov.l @r1,r0
  0C15437C  2039  and r3,r0
  0C15437E  6A03  mov r0,r10
  0C154380  D01B  mov.l @([0C1543F0]),r0
  0C154382  D319  mov.l @([0C1543E8]),r3
  0C154384  6132  mov.l @r3,r1
  0C154386  4100  shll r1
  0C154388  011D  mov.w @(R0,r1),r1
  0C15438A  9329  mov.w @([0C1543E0]),r3
  0C15438C  D919  mov.l @([0C1543F4]),r9
  0C15438E  6892  mov.l @r9,r8
  0C154390  2839  and r3,r8
  0C154392  218E  mulu.w r8,r1
  0C154394  011A  sts MACL,r1
  0C154396  3A1C  add r1,r10
  0C154398  D017  mov.l @([0C1543F8]),r0
  0C15439A  02A5  mov.w r10,@(R0,r2)
  0C15439C  BBC0  bsr 0C153B20
  0C15439E  0009  nop
  0C1543A0  D316  mov.l @([0C1543FC]),r3
  0C1543A2  430B  jsr @r3
  0C1543A4  0009  nop
  0C1543A6  D216  mov.l @([0C154400]),r2
  0C1543A8  420B  jsr @r2
  0C1543AA  0009  nop
  0C1543AC  2008  tst r0,r0
  0C1543AE  8959  bt 0C154464
  ...       (não compilado nesta sessão)
```
