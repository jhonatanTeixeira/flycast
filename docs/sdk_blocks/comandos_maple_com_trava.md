# Comandos Maple com trava (`tas.b`)

> Gerado por `tools/sdk_blocks_doc.py` a partir dos dumps do JIT em `/mnt/1TB` (2026-10-10). Índice: `README.md`. Contexto: `docs/native_sdk_code.md`.

Funções do SDK que mandam comandos ao barramento Maple (controle, VMU, vibração). Cada uma pega a trava com `tas.b` (se ocupada, sai sem esperar), chama o despachante com o código do comando em `r5` (`0x01` informação do dispositivo, `0x0A` informação da mídia, `0x0B` ler bloco, `0x0C` gravar bloco, `0x0E` ajustar condição) e solta a trava. Não é quente (`tas.b` 0,0% da emu).

Assinatura (opcodes): `4F22 6032 401B 8907`

Referência da comparação: **Dead or Alive 2 (USA)**.

| Jogo | Endereço(s) | Iguais à referência |
|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C221CA0` | 162 de 162 opcodes |
| Dead or Alive 2 (USA) | `8C135F34` | referência |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C09299C` | 126 de 126 opcodes |
| Evolution - The World of Sacred Device (USA) | `8C1DC0D8` | 126 de 126 opcodes |
| Evolution 2 - Far Off Promise (USA) | `8C1AFD80` | 162 de 162 opcodes |
| Grandia II (USA) | `8C0C862C` | 162 de 162 opcodes |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C36DACC` | 162 de 162 opcodes |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C21BFE8` | 162 de 162 opcodes |
| Macross M3 | `8C1D0D4C` | 126 de 126 opcodes |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C1986E0` | 162 de 162 opcodes |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C16AFD8` | 126 de 126 opcodes |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C37E604` | 162 de 162 opcodes |
| Power Stone (USA) | `0C112AF4` | 162 de 162 opcodes |
| Project Justice (USA) | `0C2E21F0` | 162 de 162 opcodes |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C1D68A0` | 162 de 162 opcodes |
| Shenmue (USA) (Disc 1) | `0C1DFC90` | 112 de 162 opcodes |
| Skies of Arcadia (USA) (Disc 1) | `8C2E87E0` | 162 de 162 opcodes |
| Sonic Adventure 2 (USA) (EnJaFrDeEs) | `8C15CA5C` | 85 de 85 opcodes |
| Soulcalibur (USA) | `8C240F58` | 130 de 130 opcodes |

"Iguais" conta só os deslocamentos compilados nas duas sessões (o dump guarda o que o jogo executou); o literal pool (constantes) não entra.

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan)

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
; 8C221CA0-8C22206E
  ...       (não compilado nesta sessão)
  8C221CBC  4F22  sts.l PR,@-r15
  8C221CBE  E300  mov ##0x00,r3
  8C221CC0  6633  mov r3,r6
  8C221CC2  6733  mov r3,r7
  8C221CC4  2F36  mov.l r3,@-r15
  8C221CC6  2F36  mov.l r3,@-r15
  8C221CC8  BA28  bsr 8C22111C
  8C221CCA  E501  mov ##0x01,r5
  8C221CCC  7F08  add ##8,r15
  8C221CCE  4F26  lds.l @r15+,PR
  8C221CD0  000B  rts
  8C221CD2  0009  nop
  ...       (não compilado nesta sessão)
  8C221DA0  D35E  mov.l @([8C221F1C]),r3
  8C221DA2  4F22  sts.l PR,@-r15
  8C221DA4  6032  mov.l @r3,r0
  8C221DA6  401B  tas.b @r0
  8C221DA8  8907  bt 8C221DBA
  ...       (não compilado nesta sessão)
  8C221DBA  B007  bsr 8C221DCC
  8C221DBC  0009  nop
  8C221DBE  D357  mov.l @([8C221F1C]),r3
  8C221DC0  E100  mov ##0x00,r1
  8C221DC2  6232  mov.l @r3,r2
  8C221DC4  2210  mov.b r1,@r2
  8C221DC6  4F26  lds.l @r15+,PR
  8C221DC8  000B  rts
  8C221DCA  0009  nop
  8C221DCC  2FE6  mov.l r14,@-r15
  8C221DCE  4F22  sts.l PR,@-r15
  8C221DD0  7FF8  add ##-8,r15
  8C221DD2  4628  shll16 r6
  8C221DD4  6EF3  mov r15,r14
  8C221DD6  4618  shll8 r6
  8C221DD8  2E52  mov.l r5,@r14
  8C221DDA  E300  mov ##0x00,r3
  8C221DDC  1E61  mov.l r6,@(4,r14)
  8C221DDE  2F36  mov.l r3,@-r15
  8C221DE0  E702  mov ##0x02,r7
  8C221DE2  66E3  mov r14,r6
  8C221DE4  2F36  mov.l r3,@-r15
  8C221DE6  B999  bsr 8C22111C
  8C221DE8  E50A  mov ##0x0A,r5
  8C221DEA  7F10  add ##16,r15
  8C221DEC  4F26  lds.l @r15+,PR
  8C221DEE  000B  rts
  8C221DF0  6EF6  mov.l @r15+,r14
  8C221DF2  D34A  mov.l @([8C221F1C]),r3
  8C221DF4  4F22  sts.l PR,@-r15
  8C221DF6  6032  mov.l @r3,r0
  8C221DF8  401B  tas.b @r0
  8C221DFA  8907  bt 8C221E0C
  ...       (não compilado nesta sessão)
  8C221E0C  53F1  mov.l @(4,r15),r3
  8C221E0E  2F36  mov.l r3,@-r15
  8C221E10  B008  bsr 8C221E24
  8C221E12  0009  nop
  8C221E14  7F04  add ##4,r15
  8C221E16  D341  mov.l @([8C221F1C]),r3
  8C221E18  E100  mov ##0x00,r1
  8C221E1A  6232  mov.l @r3,r2
  8C221E1C  2210  mov.b r1,@r2
  8C221E1E  4F26  lds.l @r15+,PR
  8C221E20  000B  rts
  8C221E22  0009  nop
  8C221E24  2FE6  mov.l r14,@-r15
  8C221E26  4F22  sts.l PR,@-r15
  8C221E28  7FF8  add ##-8,r15
  8C221E2A  4628  shll16 r6
  8C221E2C  6EF3  mov r15,r14
  8C221E2E  4728  shll16 r7
  8C221E30  2E52  mov.l r5,@r14
  8C221E32  53F4  mov.l @(16,r15),r3
  8C221E34  4618  shll8 r6
  8C221E36  267B  or r7,r6
  8C221E38  263B  or r3,r6
  8C221E3A  E300  mov ##0x00,r3
  8C221E3C  1E61  mov.l r6,@(4,r14)
  8C221E3E  2F36  mov.l r3,@-r15
  8C221E40  E702  mov ##0x02,r7
  8C221E42  66E3  mov r14,r6
  8C221E44  2F36  mov.l r3,@-r15
  8C221E46  B969  bsr 8C22111C
  8C221E48  E50B  mov ##0x0B,r5
  8C221E4A  7F10  add ##16,r15
  8C221E4C  4F26  lds.l @r15+,PR
  8C221E4E  000B  rts
  8C221E50  6EF6  mov.l @r15+,r14
  8C221E52  D332  mov.l @([8C221F1C]),r3
  8C221E54  4F22  sts.l PR,@-r15
  8C221E56  6032  mov.l @r3,r0
  8C221E58  401B  tas.b @r0
  8C221E5A  8907  bt 8C221E6C
  ...       (não compilado nesta sessão)
  8C221E6C  53F3  mov.l @(12,r15),r3
  8C221E6E  2F36  mov.l r3,@-r15
  8C221E70  52F3  mov.l @(12,r15),r2
  8C221E72  2F26  mov.l r2,@-r15
  8C221E74  53F3  mov.l @(12,r15),r3
  8C221E76  2F36  mov.l r3,@-r15
  8C221E78  B008  bsr 8C221E8C
  8C221E7A  0009  nop
  8C221E7C  7F0C  add ##12,r15
  8C221E7E  D327  mov.l @([8C221F1C]),r3
  8C221E80  E100  mov ##0x00,r1
  8C221E82  6232  mov.l @r3,r2
  8C221E84  2210  mov.b r1,@r2
  8C221E86  4F26  lds.l @r15+,PR
  8C221E88  000B  rts
  8C221E8A  0009  nop
  8C221E8C  2FE6  mov.l r14,@-r15
  8C221E8E  4F22  sts.l PR,@-r15
  8C221E90  7FF8  add ##-8,r15
  8C221E92  4628  shll16 r6
  8C221E94  6EF3  mov r15,r14
  8C221E96  4618  shll8 r6
  8C221E98  2E52  mov.l r5,@r14
  8C221E9A  4728  shll16 r7
  8C221E9C  53F4  mov.l @(16,r15),r3
  8C221E9E  267B  or r7,r6
  8C221EA0  263B  or r3,r6
  8C221EA2  1E61  mov.l r6,@(4,r14)
  8C221EA4  E702  mov ##0x02,r7
  8C221EA6  53F6  mov.l @(24,r15),r3
  8C221EA8  66E3  mov r14,r6
  8C221EAA  2F36  mov.l r3,@-r15
  8C221EAC  52F6  mov.l @(24,r15),r2
  8C221EAE  2F26  mov.l r2,@-r15
  8C221EB0  B934  bsr 8C22111C
  8C221EB2  E50C  mov ##0x0C,r5
  8C221EB4  7F10  add ##16,r15
  8C221EB6  4F26  lds.l @r15+,PR
  8C221EB8  000B  rts
  8C221EBA  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C22200C  D30B  mov.l @([8C22203C]),r3
  8C22200E  4F22  sts.l PR,@-r15
  8C222010  6032  mov.l @r3,r0
  8C222012  401B  tas.b @r0
  8C222014  8907  bt 8C222026
  ...       (não compilado nesta sessão)
  8C222026  B00F  bsr 8C222048
  8C222028  0009  nop
  8C22202A  D304  mov.l @([8C22203C]),r3
  8C22202C  E100  mov ##0x00,r1
  8C22202E  6232  mov.l @r3,r2
  8C222030  2210  mov.b r1,@r2
  8C222032  4F26  lds.l @r15+,PR
  8C222034  000B  rts
  8C222036  0009  nop
  ...       (não compilado nesta sessão)
  8C222048  4F22  sts.l PR,@-r15
  8C22204A  7FF4  add ##-12,r15
  8C22204C  1F61  mov.l r6,@(4,r15)
  8C22204E  1F72  mov.l r7,@(8,r15)
  8C222050  E701  mov ##0x01,r7
  8C222052  2F52  mov.l r5,@r15
  8C222054  53F2  mov.l @(8,r15),r3
  8C222056  2F36  mov.l r3,@-r15
  8C222058  52F2  mov.l @(8,r15),r2
  8C22205A  2F26  mov.l r2,@-r15
  8C22205C  66F3  mov r15,r6
  8C22205E  7608  add ##8,r6
  8C222060  B85C  bsr 8C22111C
  8C222062  E50E  mov ##0x0E,r5
  8C222064  7F14  add ##20,r15
  8C222066  4F26  lds.l @r15+,PR
  8C222068  000B  rts
  8C22206A  0009  nop
  ...       (não compilado nesta sessão)
```


## Dead or Alive 2 (USA)

Dump: `/mnt/1TB/dcbat_off/20261007-192359_Dead_or_Alive_2__USA__/jit-14513.txt`

```
; 8C135F34-8C136302
  ...       (não compilado nesta sessão)
  8C135F50  4F22  sts.l PR,@-r15
  8C135F52  E300  mov ##0x00,r3
  8C135F54  6633  mov r3,r6
  8C135F56  6733  mov r3,r7
  8C135F58  2F36  mov.l r3,@-r15
  8C135F5A  2F36  mov.l r3,@-r15
  8C135F5C  BA28  bsr 8C1353B0
  8C135F5E  E501  mov ##0x01,r5
  8C135F60  7F08  add ##8,r15
  8C135F62  4F26  lds.l @r15+,PR
  8C135F64  000B  rts
  8C135F66  0009  nop
  ...       (não compilado nesta sessão)
  8C136034  D35E  mov.l @([8C1361B0]),r3
  8C136036  4F22  sts.l PR,@-r15
  8C136038  6032  mov.l @r3,r0
  8C13603A  401B  tas.b @r0
  8C13603C  8907  bt 8C13604E
  ...       (não compilado nesta sessão)
  8C13604E  B007  bsr 8C136060
  8C136050  0009  nop
  8C136052  D357  mov.l @([8C1361B0]),r3
  8C136054  E100  mov ##0x00,r1
  8C136056  6232  mov.l @r3,r2
  8C136058  2210  mov.b r1,@r2
  8C13605A  4F26  lds.l @r15+,PR
  8C13605C  000B  rts
  8C13605E  0009  nop
  8C136060  2FE6  mov.l r14,@-r15
  8C136062  4F22  sts.l PR,@-r15
  8C136064  7FF8  add ##-8,r15
  8C136066  4628  shll16 r6
  8C136068  6EF3  mov r15,r14
  8C13606A  4618  shll8 r6
  8C13606C  2E52  mov.l r5,@r14
  8C13606E  E300  mov ##0x00,r3
  8C136070  1E61  mov.l r6,@(4,r14)
  8C136072  2F36  mov.l r3,@-r15
  8C136074  E702  mov ##0x02,r7
  8C136076  66E3  mov r14,r6
  8C136078  2F36  mov.l r3,@-r15
  8C13607A  B999  bsr 8C1353B0
  8C13607C  E50A  mov ##0x0A,r5
  8C13607E  7F10  add ##16,r15
  8C136080  4F26  lds.l @r15+,PR
  8C136082  000B  rts
  8C136084  6EF6  mov.l @r15+,r14
  8C136086  D34A  mov.l @([8C1361B0]),r3
  8C136088  4F22  sts.l PR,@-r15
  8C13608A  6032  mov.l @r3,r0
  8C13608C  401B  tas.b @r0
  8C13608E  8907  bt 8C1360A0
  ...       (não compilado nesta sessão)
  8C1360A0  53F1  mov.l @(4,r15),r3
  8C1360A2  2F36  mov.l r3,@-r15
  8C1360A4  B008  bsr 8C1360B8
  8C1360A6  0009  nop
  8C1360A8  7F04  add ##4,r15
  8C1360AA  D341  mov.l @([8C1361B0]),r3
  8C1360AC  E100  mov ##0x00,r1
  8C1360AE  6232  mov.l @r3,r2
  8C1360B0  2210  mov.b r1,@r2
  8C1360B2  4F26  lds.l @r15+,PR
  8C1360B4  000B  rts
  8C1360B6  0009  nop
  8C1360B8  2FE6  mov.l r14,@-r15
  8C1360BA  4F22  sts.l PR,@-r15
  8C1360BC  7FF8  add ##-8,r15
  8C1360BE  4628  shll16 r6
  8C1360C0  6EF3  mov r15,r14
  8C1360C2  4728  shll16 r7
  8C1360C4  2E52  mov.l r5,@r14
  8C1360C6  53F4  mov.l @(16,r15),r3
  8C1360C8  4618  shll8 r6
  8C1360CA  267B  or r7,r6
  8C1360CC  263B  or r3,r6
  8C1360CE  E300  mov ##0x00,r3
  8C1360D0  1E61  mov.l r6,@(4,r14)
  8C1360D2  2F36  mov.l r3,@-r15
  8C1360D4  E702  mov ##0x02,r7
  8C1360D6  66E3  mov r14,r6
  8C1360D8  2F36  mov.l r3,@-r15
  8C1360DA  B969  bsr 8C1353B0
  8C1360DC  E50B  mov ##0x0B,r5
  8C1360DE  7F10  add ##16,r15
  8C1360E0  4F26  lds.l @r15+,PR
  8C1360E2  000B  rts
  8C1360E4  6EF6  mov.l @r15+,r14
  8C1360E6  D332  mov.l @([8C1361B0]),r3
  8C1360E8  4F22  sts.l PR,@-r15
  8C1360EA  6032  mov.l @r3,r0
  8C1360EC  401B  tas.b @r0
  8C1360EE  8907  bt 8C136100
  ...       (não compilado nesta sessão)
  8C136100  53F3  mov.l @(12,r15),r3
  8C136102  2F36  mov.l r3,@-r15
  8C136104  52F3  mov.l @(12,r15),r2
  8C136106  2F26  mov.l r2,@-r15
  8C136108  53F3  mov.l @(12,r15),r3
  8C13610A  2F36  mov.l r3,@-r15
  8C13610C  B008  bsr 8C136120
  8C13610E  0009  nop
  8C136110  7F0C  add ##12,r15
  8C136112  D327  mov.l @([8C1361B0]),r3
  8C136114  E100  mov ##0x00,r1
  8C136116  6232  mov.l @r3,r2
  8C136118  2210  mov.b r1,@r2
  8C13611A  4F26  lds.l @r15+,PR
  8C13611C  000B  rts
  8C13611E  0009  nop
  8C136120  2FE6  mov.l r14,@-r15
  8C136122  4F22  sts.l PR,@-r15
  8C136124  7FF8  add ##-8,r15
  8C136126  4628  shll16 r6
  8C136128  6EF3  mov r15,r14
  8C13612A  4618  shll8 r6
  8C13612C  2E52  mov.l r5,@r14
  8C13612E  4728  shll16 r7
  8C136130  53F4  mov.l @(16,r15),r3
  8C136132  267B  or r7,r6
  8C136134  263B  or r3,r6
  8C136136  1E61  mov.l r6,@(4,r14)
  8C136138  E702  mov ##0x02,r7
  8C13613A  53F6  mov.l @(24,r15),r3
  8C13613C  66E3  mov r14,r6
  8C13613E  2F36  mov.l r3,@-r15
  8C136140  52F6  mov.l @(24,r15),r2
  8C136142  2F26  mov.l r2,@-r15
  8C136144  B934  bsr 8C1353B0
  8C136146  E50C  mov ##0x0C,r5
  8C136148  7F10  add ##16,r15
  8C13614A  4F26  lds.l @r15+,PR
  8C13614C  000B  rts
  8C13614E  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C1362A0  D30B  mov.l @([8C1362D0]),r3
  8C1362A2  4F22  sts.l PR,@-r15
  8C1362A4  6032  mov.l @r3,r0
  8C1362A6  401B  tas.b @r0
  8C1362A8  8907  bt 8C1362BA
  ...       (não compilado nesta sessão)
  8C1362BA  B00F  bsr 8C1362DC
  8C1362BC  0009  nop
  8C1362BE  D304  mov.l @([8C1362D0]),r3
  8C1362C0  E100  mov ##0x00,r1
  8C1362C2  6232  mov.l @r3,r2
  8C1362C4  2210  mov.b r1,@r2
  8C1362C6  4F26  lds.l @r15+,PR
  8C1362C8  000B  rts
  8C1362CA  0009  nop
  ...       (não compilado nesta sessão)
  8C1362DC  4F22  sts.l PR,@-r15
  8C1362DE  7FF4  add ##-12,r15
  8C1362E0  1F61  mov.l r6,@(4,r15)
  8C1362E2  1F72  mov.l r7,@(8,r15)
  8C1362E4  E701  mov ##0x01,r7
  8C1362E6  2F52  mov.l r5,@r15
  8C1362E8  53F2  mov.l @(8,r15),r3
  8C1362EA  2F36  mov.l r3,@-r15
  8C1362EC  52F2  mov.l @(8,r15),r2
  8C1362EE  2F26  mov.l r2,@-r15
  8C1362F0  66F3  mov r15,r6
  8C1362F2  7608  add ##8,r6
  8C1362F4  B85C  bsr 8C1353B0
  8C1362F6  E50E  mov ##0x0E,r5
  8C1362F8  7F14  add ##20,r15
  8C1362FA  4F26  lds.l @r15+,PR
  8C1362FC  000B  rts
  8C1362FE  0009  nop
  ...       (não compilado nesta sessão)
```


## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!]

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
; 8C09299C-8C092BB0
  ...       (não compilado nesta sessão)
  8C0929B8  4F22  sts.l PR,@-r15
  8C0929BA  E300  mov ##0x00,r3
  8C0929BC  6633  mov r3,r6
  8C0929BE  6733  mov r3,r7
  8C0929C0  2F36  mov.l r3,@-r15
  8C0929C2  2F36  mov.l r3,@-r15
  8C0929C4  BA28  bsr 8C091E18
  8C0929C6  E501  mov ##0x01,r5
  8C0929C8  7F08  add ##8,r15
  8C0929CA  4F26  lds.l @r15+,PR
  8C0929CC  000B  rts
  8C0929CE  0009  nop
  ...       (não compilado nesta sessão)
  8C092A9C  D35E  mov.l @([8C092C18]),r3
  8C092A9E  4F22  sts.l PR,@-r15
  8C092AA0  6032  mov.l @r3,r0
  8C092AA2  401B  tas.b @r0
  8C092AA4  8907  bt 8C092AB6
  ...       (não compilado nesta sessão)
  8C092AB6  B007  bsr 8C092AC8
  8C092AB8  0009  nop
  8C092ABA  D357  mov.l @([8C092C18]),r3
  8C092ABC  E100  mov ##0x00,r1
  8C092ABE  6232  mov.l @r3,r2
  8C092AC0  2210  mov.b r1,@r2
  8C092AC2  4F26  lds.l @r15+,PR
  8C092AC4  000B  rts
  8C092AC6  0009  nop
  8C092AC8  2FE6  mov.l r14,@-r15
  8C092ACA  4F22  sts.l PR,@-r15
  8C092ACC  7FF8  add ##-8,r15
  8C092ACE  4628  shll16 r6
  8C092AD0  6EF3  mov r15,r14
  8C092AD2  4618  shll8 r6
  8C092AD4  2E52  mov.l r5,@r14
  8C092AD6  E300  mov ##0x00,r3
  8C092AD8  1E61  mov.l r6,@(4,r14)
  8C092ADA  2F36  mov.l r3,@-r15
  8C092ADC  E702  mov ##0x02,r7
  8C092ADE  66E3  mov r14,r6
  8C092AE0  2F36  mov.l r3,@-r15
  8C092AE2  B999  bsr 8C091E18
  8C092AE4  E50A  mov ##0x0A,r5
  8C092AE6  7F10  add ##16,r15
  8C092AE8  4F26  lds.l @r15+,PR
  8C092AEA  000B  rts
  8C092AEC  6EF6  mov.l @r15+,r14
  8C092AEE  D34A  mov.l @([8C092C18]),r3
  8C092AF0  4F22  sts.l PR,@-r15
  8C092AF2  6032  mov.l @r3,r0
  8C092AF4  401B  tas.b @r0
  8C092AF6  8907  bt 8C092B08
  ...       (não compilado nesta sessão)
  8C092B08  53F1  mov.l @(4,r15),r3
  8C092B0A  2F36  mov.l r3,@-r15
  8C092B0C  B008  bsr 8C092B20
  8C092B0E  0009  nop
  8C092B10  7F04  add ##4,r15
  8C092B12  D341  mov.l @([8C092C18]),r3
  8C092B14  E100  mov ##0x00,r1
  8C092B16  6232  mov.l @r3,r2
  8C092B18  2210  mov.b r1,@r2
  8C092B1A  4F26  lds.l @r15+,PR
  8C092B1C  000B  rts
  8C092B1E  0009  nop
  8C092B20  2FE6  mov.l r14,@-r15
  8C092B22  4F22  sts.l PR,@-r15
  8C092B24  7FF8  add ##-8,r15
  8C092B26  4628  shll16 r6
  8C092B28  6EF3  mov r15,r14
  8C092B2A  4728  shll16 r7
  8C092B2C  2E52  mov.l r5,@r14
  8C092B2E  53F4  mov.l @(16,r15),r3
  8C092B30  4618  shll8 r6
  8C092B32  267B  or r7,r6
  8C092B34  263B  or r3,r6
  8C092B36  E300  mov ##0x00,r3
  8C092B38  1E61  mov.l r6,@(4,r14)
  8C092B3A  2F36  mov.l r3,@-r15
  8C092B3C  E702  mov ##0x02,r7
  8C092B3E  66E3  mov r14,r6
  8C092B40  2F36  mov.l r3,@-r15
  8C092B42  B969  bsr 8C091E18
  8C092B44  E50B  mov ##0x0B,r5
  8C092B46  7F10  add ##16,r15
  8C092B48  4F26  lds.l @r15+,PR
  8C092B4A  000B  rts
  8C092B4C  6EF6  mov.l @r15+,r14
  8C092B4E  D332  mov.l @([8C092C18]),r3
  8C092B50  4F22  sts.l PR,@-r15
  8C092B52  6032  mov.l @r3,r0
  8C092B54  401B  tas.b @r0
  8C092B56  8907  bt 8C092B68
  ...       (não compilado nesta sessão)
  8C092B68  53F3  mov.l @(12,r15),r3
  8C092B6A  2F36  mov.l r3,@-r15
  8C092B6C  52F3  mov.l @(12,r15),r2
  8C092B6E  2F26  mov.l r2,@-r15
  8C092B70  53F3  mov.l @(12,r15),r3
  8C092B72  2F36  mov.l r3,@-r15
  8C092B74  B008  bsr 8C092B88
  8C092B76  0009  nop
  8C092B78  7F0C  add ##12,r15
  8C092B7A  D327  mov.l @([8C092C18]),r3
  8C092B7C  E100  mov ##0x00,r1
  8C092B7E  6232  mov.l @r3,r2
  8C092B80  2210  mov.b r1,@r2
  8C092B82  4F26  lds.l @r15+,PR
  8C092B84  000B  rts
  8C092B86  0009  nop
  8C092B88  2FE6  mov.l r14,@-r15
  8C092B8A  4F22  sts.l PR,@-r15
  8C092B8C  7FF8  add ##-8,r15
  8C092B8E  4628  shll16 r6
  8C092B90  6EF3  mov r15,r14
  8C092B92  4618  shll8 r6
  8C092B94  2E52  mov.l r5,@r14
  8C092B96  4728  shll16 r7
  8C092B98  53F4  mov.l @(16,r15),r3
  8C092B9A  267B  or r7,r6
  8C092B9C  263B  or r3,r6
  8C092B9E  1E61  mov.l r6,@(4,r14)
  8C092BA0  E702  mov ##0x02,r7
  8C092BA2  53F6  mov.l @(24,r15),r3
  8C092BA4  66E3  mov r14,r6
  8C092BA6  2F36  mov.l r3,@-r15
  8C092BA8  52F6  mov.l @(24,r15),r2
  8C092BAA  2F26  mov.l r2,@-r15
  8C092BAC  B934  bsr 8C091E18
  8C092BAE  E50C  mov ##0x0C,r5
```


## Evolution - The World of Sacred Device (USA)

Dump: `/mnt/1TB/dcbat/20261007-151806_Evolution_-_The_World_of_Sacred_Device__/jit-24035.txt`

```
; 8C1DC0D8-8C1DC2EC
  ...       (não compilado nesta sessão)
  8C1DC0F4  4F22  sts.l PR,@-r15
  8C1DC0F6  E300  mov ##0x00,r3
  8C1DC0F8  6633  mov r3,r6
  8C1DC0FA  6733  mov r3,r7
  8C1DC0FC  2F36  mov.l r3,@-r15
  8C1DC0FE  2F36  mov.l r3,@-r15
  8C1DC100  BA28  bsr 8C1DB554
  8C1DC102  E501  mov ##0x01,r5
  8C1DC104  7F08  add ##8,r15
  8C1DC106  4F26  lds.l @r15+,PR
  8C1DC108  000B  rts
  8C1DC10A  0009  nop
  ...       (não compilado nesta sessão)
  8C1DC1D8  D35E  mov.l @([8C1DC354]),r3
  8C1DC1DA  4F22  sts.l PR,@-r15
  8C1DC1DC  6032  mov.l @r3,r0
  8C1DC1DE  401B  tas.b @r0
  8C1DC1E0  8907  bt 8C1DC1F2
  ...       (não compilado nesta sessão)
  8C1DC1F2  B007  bsr 8C1DC204
  8C1DC1F4  0009  nop
  8C1DC1F6  D357  mov.l @([8C1DC354]),r3
  8C1DC1F8  E100  mov ##0x00,r1
  8C1DC1FA  6232  mov.l @r3,r2
  8C1DC1FC  2210  mov.b r1,@r2
  8C1DC1FE  4F26  lds.l @r15+,PR
  8C1DC200  000B  rts
  8C1DC202  0009  nop
  8C1DC204  2FE6  mov.l r14,@-r15
  8C1DC206  4F22  sts.l PR,@-r15
  8C1DC208  7FF8  add ##-8,r15
  8C1DC20A  4628  shll16 r6
  8C1DC20C  6EF3  mov r15,r14
  8C1DC20E  4618  shll8 r6
  8C1DC210  2E52  mov.l r5,@r14
  8C1DC212  E300  mov ##0x00,r3
  8C1DC214  1E61  mov.l r6,@(4,r14)
  8C1DC216  2F36  mov.l r3,@-r15
  8C1DC218  E702  mov ##0x02,r7
  8C1DC21A  66E3  mov r14,r6
  8C1DC21C  2F36  mov.l r3,@-r15
  8C1DC21E  B999  bsr 8C1DB554
  8C1DC220  E50A  mov ##0x0A,r5
  8C1DC222  7F10  add ##16,r15
  8C1DC224  4F26  lds.l @r15+,PR
  8C1DC226  000B  rts
  8C1DC228  6EF6  mov.l @r15+,r14
  8C1DC22A  D34A  mov.l @([8C1DC354]),r3
  8C1DC22C  4F22  sts.l PR,@-r15
  8C1DC22E  6032  mov.l @r3,r0
  8C1DC230  401B  tas.b @r0
  8C1DC232  8907  bt 8C1DC244
  ...       (não compilado nesta sessão)
  8C1DC244  53F1  mov.l @(4,r15),r3
  8C1DC246  2F36  mov.l r3,@-r15
  8C1DC248  B008  bsr 8C1DC25C
  8C1DC24A  0009  nop
  8C1DC24C  7F04  add ##4,r15
  8C1DC24E  D341  mov.l @([8C1DC354]),r3
  8C1DC250  E100  mov ##0x00,r1
  8C1DC252  6232  mov.l @r3,r2
  8C1DC254  2210  mov.b r1,@r2
  8C1DC256  4F26  lds.l @r15+,PR
  8C1DC258  000B  rts
  8C1DC25A  0009  nop
  8C1DC25C  2FE6  mov.l r14,@-r15
  8C1DC25E  4F22  sts.l PR,@-r15
  8C1DC260  7FF8  add ##-8,r15
  8C1DC262  4628  shll16 r6
  8C1DC264  6EF3  mov r15,r14
  8C1DC266  4728  shll16 r7
  8C1DC268  2E52  mov.l r5,@r14
  8C1DC26A  53F4  mov.l @(16,r15),r3
  8C1DC26C  4618  shll8 r6
  8C1DC26E  267B  or r7,r6
  8C1DC270  263B  or r3,r6
  8C1DC272  E300  mov ##0x00,r3
  8C1DC274  1E61  mov.l r6,@(4,r14)
  8C1DC276  2F36  mov.l r3,@-r15
  8C1DC278  E702  mov ##0x02,r7
  8C1DC27A  66E3  mov r14,r6
  8C1DC27C  2F36  mov.l r3,@-r15
  8C1DC27E  B969  bsr 8C1DB554
  8C1DC280  E50B  mov ##0x0B,r5
  8C1DC282  7F10  add ##16,r15
  8C1DC284  4F26  lds.l @r15+,PR
  8C1DC286  000B  rts
  8C1DC288  6EF6  mov.l @r15+,r14
  8C1DC28A  D332  mov.l @([8C1DC354]),r3
  8C1DC28C  4F22  sts.l PR,@-r15
  8C1DC28E  6032  mov.l @r3,r0
  8C1DC290  401B  tas.b @r0
  8C1DC292  8907  bt 8C1DC2A4
  ...       (não compilado nesta sessão)
  8C1DC2A4  53F3  mov.l @(12,r15),r3
  8C1DC2A6  2F36  mov.l r3,@-r15
  8C1DC2A8  52F3  mov.l @(12,r15),r2
  8C1DC2AA  2F26  mov.l r2,@-r15
  8C1DC2AC  53F3  mov.l @(12,r15),r3
  8C1DC2AE  2F36  mov.l r3,@-r15
  8C1DC2B0  B008  bsr 8C1DC2C4
  8C1DC2B2  0009  nop
  8C1DC2B4  7F0C  add ##12,r15
  8C1DC2B6  D327  mov.l @([8C1DC354]),r3
  8C1DC2B8  E100  mov ##0x00,r1
  8C1DC2BA  6232  mov.l @r3,r2
  8C1DC2BC  2210  mov.b r1,@r2
  8C1DC2BE  4F26  lds.l @r15+,PR
  8C1DC2C0  000B  rts
  8C1DC2C2  0009  nop
  8C1DC2C4  2FE6  mov.l r14,@-r15
  8C1DC2C6  4F22  sts.l PR,@-r15
  8C1DC2C8  7FF8  add ##-8,r15
  8C1DC2CA  4628  shll16 r6
  8C1DC2CC  6EF3  mov r15,r14
  8C1DC2CE  4618  shll8 r6
  8C1DC2D0  2E52  mov.l r5,@r14
  8C1DC2D2  4728  shll16 r7
  8C1DC2D4  53F4  mov.l @(16,r15),r3
  8C1DC2D6  267B  or r7,r6
  8C1DC2D8  263B  or r3,r6
  8C1DC2DA  1E61  mov.l r6,@(4,r14)
  8C1DC2DC  E702  mov ##0x02,r7
  8C1DC2DE  53F6  mov.l @(24,r15),r3
  8C1DC2E0  66E3  mov r14,r6
  8C1DC2E2  2F36  mov.l r3,@-r15
  8C1DC2E4  52F6  mov.l @(24,r15),r2
  8C1DC2E6  2F26  mov.l r2,@-r15
  8C1DC2E8  B934  bsr 8C1DB554
  8C1DC2EA  E50C  mov ##0x0C,r5
```


## Evolution 2 - Far Off Promise (USA)

Dump: `/mnt/1TB/dcbat/20261007-151251_Evolution_2_-_Far_Off_Promise__USA__/jit-16237.txt`

```
; 8C1AFD80-8C1B014E
  ...       (não compilado nesta sessão)
  8C1AFD9C  4F22  sts.l PR,@-r15
  8C1AFD9E  E300  mov ##0x00,r3
  8C1AFDA0  6633  mov r3,r6
  8C1AFDA2  6733  mov r3,r7
  8C1AFDA4  2F36  mov.l r3,@-r15
  8C1AFDA6  2F36  mov.l r3,@-r15
  8C1AFDA8  BA28  bsr 8C1AF1FC
  8C1AFDAA  E501  mov ##0x01,r5
  8C1AFDAC  7F08  add ##8,r15
  8C1AFDAE  4F26  lds.l @r15+,PR
  8C1AFDB0  000B  rts
  8C1AFDB2  0009  nop
  ...       (não compilado nesta sessão)
  8C1AFE80  D35E  mov.l @([8C1AFFFC]),r3
  8C1AFE82  4F22  sts.l PR,@-r15
  8C1AFE84  6032  mov.l @r3,r0
  8C1AFE86  401B  tas.b @r0
  8C1AFE88  8907  bt 8C1AFE9A
  ...       (não compilado nesta sessão)
  8C1AFE9A  B007  bsr 8C1AFEAC
  8C1AFE9C  0009  nop
  8C1AFE9E  D357  mov.l @([8C1AFFFC]),r3
  8C1AFEA0  E100  mov ##0x00,r1
  8C1AFEA2  6232  mov.l @r3,r2
  8C1AFEA4  2210  mov.b r1,@r2
  8C1AFEA6  4F26  lds.l @r15+,PR
  8C1AFEA8  000B  rts
  8C1AFEAA  0009  nop
  8C1AFEAC  2FE6  mov.l r14,@-r15
  8C1AFEAE  4F22  sts.l PR,@-r15
  8C1AFEB0  7FF8  add ##-8,r15
  8C1AFEB2  4628  shll16 r6
  8C1AFEB4  6EF3  mov r15,r14
  8C1AFEB6  4618  shll8 r6
  8C1AFEB8  2E52  mov.l r5,@r14
  8C1AFEBA  E300  mov ##0x00,r3
  8C1AFEBC  1E61  mov.l r6,@(4,r14)
  8C1AFEBE  2F36  mov.l r3,@-r15
  8C1AFEC0  E702  mov ##0x02,r7
  8C1AFEC2  66E3  mov r14,r6
  8C1AFEC4  2F36  mov.l r3,@-r15
  8C1AFEC6  B999  bsr 8C1AF1FC
  8C1AFEC8  E50A  mov ##0x0A,r5
  8C1AFECA  7F10  add ##16,r15
  8C1AFECC  4F26  lds.l @r15+,PR
  8C1AFECE  000B  rts
  8C1AFED0  6EF6  mov.l @r15+,r14
  8C1AFED2  D34A  mov.l @([8C1AFFFC]),r3
  8C1AFED4  4F22  sts.l PR,@-r15
  8C1AFED6  6032  mov.l @r3,r0
  8C1AFED8  401B  tas.b @r0
  8C1AFEDA  8907  bt 8C1AFEEC
  ...       (não compilado nesta sessão)
  8C1AFEEC  53F1  mov.l @(4,r15),r3
  8C1AFEEE  2F36  mov.l r3,@-r15
  8C1AFEF0  B008  bsr 8C1AFF04
  8C1AFEF2  0009  nop
  8C1AFEF4  7F04  add ##4,r15
  8C1AFEF6  D341  mov.l @([8C1AFFFC]),r3
  8C1AFEF8  E100  mov ##0x00,r1
  8C1AFEFA  6232  mov.l @r3,r2
  8C1AFEFC  2210  mov.b r1,@r2
  8C1AFEFE  4F26  lds.l @r15+,PR
  8C1AFF00  000B  rts
  8C1AFF02  0009  nop
  8C1AFF04  2FE6  mov.l r14,@-r15
  8C1AFF06  4F22  sts.l PR,@-r15
  8C1AFF08  7FF8  add ##-8,r15
  8C1AFF0A  4628  shll16 r6
  8C1AFF0C  6EF3  mov r15,r14
  8C1AFF0E  4728  shll16 r7
  8C1AFF10  2E52  mov.l r5,@r14
  8C1AFF12  53F4  mov.l @(16,r15),r3
  8C1AFF14  4618  shll8 r6
  8C1AFF16  267B  or r7,r6
  8C1AFF18  263B  or r3,r6
  8C1AFF1A  E300  mov ##0x00,r3
  8C1AFF1C  1E61  mov.l r6,@(4,r14)
  8C1AFF1E  2F36  mov.l r3,@-r15
  8C1AFF20  E702  mov ##0x02,r7
  8C1AFF22  66E3  mov r14,r6
  8C1AFF24  2F36  mov.l r3,@-r15
  8C1AFF26  B969  bsr 8C1AF1FC
  8C1AFF28  E50B  mov ##0x0B,r5
  8C1AFF2A  7F10  add ##16,r15
  8C1AFF2C  4F26  lds.l @r15+,PR
  8C1AFF2E  000B  rts
  8C1AFF30  6EF6  mov.l @r15+,r14
  8C1AFF32  D332  mov.l @([8C1AFFFC]),r3
  8C1AFF34  4F22  sts.l PR,@-r15
  8C1AFF36  6032  mov.l @r3,r0
  8C1AFF38  401B  tas.b @r0
  8C1AFF3A  8907  bt 8C1AFF4C
  ...       (não compilado nesta sessão)
  8C1AFF4C  53F3  mov.l @(12,r15),r3
  8C1AFF4E  2F36  mov.l r3,@-r15
  8C1AFF50  52F3  mov.l @(12,r15),r2
  8C1AFF52  2F26  mov.l r2,@-r15
  8C1AFF54  53F3  mov.l @(12,r15),r3
  8C1AFF56  2F36  mov.l r3,@-r15
  8C1AFF58  B008  bsr 8C1AFF6C
  8C1AFF5A  0009  nop
  8C1AFF5C  7F0C  add ##12,r15
  8C1AFF5E  D327  mov.l @([8C1AFFFC]),r3
  8C1AFF60  E100  mov ##0x00,r1
  8C1AFF62  6232  mov.l @r3,r2
  8C1AFF64  2210  mov.b r1,@r2
  8C1AFF66  4F26  lds.l @r15+,PR
  8C1AFF68  000B  rts
  8C1AFF6A  0009  nop
  8C1AFF6C  2FE6  mov.l r14,@-r15
  8C1AFF6E  4F22  sts.l PR,@-r15
  8C1AFF70  7FF8  add ##-8,r15
  8C1AFF72  4628  shll16 r6
  8C1AFF74  6EF3  mov r15,r14
  8C1AFF76  4618  shll8 r6
  8C1AFF78  2E52  mov.l r5,@r14
  8C1AFF7A  4728  shll16 r7
  8C1AFF7C  53F4  mov.l @(16,r15),r3
  8C1AFF7E  267B  or r7,r6
  8C1AFF80  263B  or r3,r6
  8C1AFF82  1E61  mov.l r6,@(4,r14)
  8C1AFF84  E702  mov ##0x02,r7
  8C1AFF86  53F6  mov.l @(24,r15),r3
  8C1AFF88  66E3  mov r14,r6
  8C1AFF8A  2F36  mov.l r3,@-r15
  8C1AFF8C  52F6  mov.l @(24,r15),r2
  8C1AFF8E  2F26  mov.l r2,@-r15
  8C1AFF90  B934  bsr 8C1AF1FC
  8C1AFF92  E50C  mov ##0x0C,r5
  8C1AFF94  7F10  add ##16,r15
  8C1AFF96  4F26  lds.l @r15+,PR
  8C1AFF98  000B  rts
  8C1AFF9A  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C1B00EC  D30B  mov.l @([8C1B011C]),r3
  8C1B00EE  4F22  sts.l PR,@-r15
  8C1B00F0  6032  mov.l @r3,r0
  8C1B00F2  401B  tas.b @r0
  8C1B00F4  8907  bt 8C1B0106
  ...       (não compilado nesta sessão)
  8C1B0106  B00F  bsr 8C1B0128
  8C1B0108  0009  nop
  8C1B010A  D304  mov.l @([8C1B011C]),r3
  8C1B010C  E100  mov ##0x00,r1
  8C1B010E  6232  mov.l @r3,r2
  8C1B0110  2210  mov.b r1,@r2
  8C1B0112  4F26  lds.l @r15+,PR
  8C1B0114  000B  rts
  8C1B0116  0009  nop
  ...       (não compilado nesta sessão)
  8C1B0128  4F22  sts.l PR,@-r15
  8C1B012A  7FF4  add ##-12,r15
  8C1B012C  1F61  mov.l r6,@(4,r15)
  8C1B012E  1F72  mov.l r7,@(8,r15)
  8C1B0130  E701  mov ##0x01,r7
  8C1B0132  2F52  mov.l r5,@r15
  8C1B0134  53F2  mov.l @(8,r15),r3
  8C1B0136  2F36  mov.l r3,@-r15
  8C1B0138  52F2  mov.l @(8,r15),r2
  8C1B013A  2F26  mov.l r2,@-r15
  8C1B013C  66F3  mov r15,r6
  8C1B013E  7608  add ##8,r6
  8C1B0140  B85C  bsr 8C1AF1FC
  8C1B0142  E50E  mov ##0x0E,r5
  8C1B0144  7F14  add ##20,r15
  8C1B0146  4F26  lds.l @r15+,PR
  8C1B0148  000B  rts
  8C1B014A  0009  nop
  ...       (não compilado nesta sessão)
```


## Grandia II (USA)

Dump: `/mnt/1TB/dcbat/20261007-152308_Grandia_II__USA__/jit-32936.txt`

```
; 8C0C862C-8C0C89FA
  ...       (não compilado nesta sessão)
  8C0C8648  4F22  sts.l PR,@-r15
  8C0C864A  E300  mov ##0x00,r3
  8C0C864C  6633  mov r3,r6
  8C0C864E  6733  mov r3,r7
  8C0C8650  2F36  mov.l r3,@-r15
  8C0C8652  2F36  mov.l r3,@-r15
  8C0C8654  BA28  bsr 8C0C7AA8
  8C0C8656  E501  mov ##0x01,r5
  8C0C8658  7F08  add ##8,r15
  8C0C865A  4F26  lds.l @r15+,PR
  8C0C865C  000B  rts
  8C0C865E  0009  nop
  ...       (não compilado nesta sessão)
  8C0C872C  D35E  mov.l @([8C0C88A8]),r3
  8C0C872E  4F22  sts.l PR,@-r15
  8C0C8730  6032  mov.l @r3,r0
  8C0C8732  401B  tas.b @r0
  8C0C8734  8907  bt 8C0C8746
  ...       (não compilado nesta sessão)
  8C0C8746  B007  bsr 8C0C8758
  8C0C8748  0009  nop
  8C0C874A  D357  mov.l @([8C0C88A8]),r3
  8C0C874C  E100  mov ##0x00,r1
  8C0C874E  6232  mov.l @r3,r2
  8C0C8750  2210  mov.b r1,@r2
  8C0C8752  4F26  lds.l @r15+,PR
  8C0C8754  000B  rts
  8C0C8756  0009  nop
  8C0C8758  2FE6  mov.l r14,@-r15
  8C0C875A  4F22  sts.l PR,@-r15
  8C0C875C  7FF8  add ##-8,r15
  8C0C875E  4628  shll16 r6
  8C0C8760  6EF3  mov r15,r14
  8C0C8762  4618  shll8 r6
  8C0C8764  2E52  mov.l r5,@r14
  8C0C8766  E300  mov ##0x00,r3
  8C0C8768  1E61  mov.l r6,@(4,r14)
  8C0C876A  2F36  mov.l r3,@-r15
  8C0C876C  E702  mov ##0x02,r7
  8C0C876E  66E3  mov r14,r6
  8C0C8770  2F36  mov.l r3,@-r15
  8C0C8772  B999  bsr 8C0C7AA8
  8C0C8774  E50A  mov ##0x0A,r5
  8C0C8776  7F10  add ##16,r15
  8C0C8778  4F26  lds.l @r15+,PR
  8C0C877A  000B  rts
  8C0C877C  6EF6  mov.l @r15+,r14
  8C0C877E  D34A  mov.l @([8C0C88A8]),r3
  8C0C8780  4F22  sts.l PR,@-r15
  8C0C8782  6032  mov.l @r3,r0
  8C0C8784  401B  tas.b @r0
  8C0C8786  8907  bt 8C0C8798
  ...       (não compilado nesta sessão)
  8C0C8798  53F1  mov.l @(4,r15),r3
  8C0C879A  2F36  mov.l r3,@-r15
  8C0C879C  B008  bsr 8C0C87B0
  8C0C879E  0009  nop
  8C0C87A0  7F04  add ##4,r15
  8C0C87A2  D341  mov.l @([8C0C88A8]),r3
  8C0C87A4  E100  mov ##0x00,r1
  8C0C87A6  6232  mov.l @r3,r2
  8C0C87A8  2210  mov.b r1,@r2
  8C0C87AA  4F26  lds.l @r15+,PR
  8C0C87AC  000B  rts
  8C0C87AE  0009  nop
  8C0C87B0  2FE6  mov.l r14,@-r15
  8C0C87B2  4F22  sts.l PR,@-r15
  8C0C87B4  7FF8  add ##-8,r15
  8C0C87B6  4628  shll16 r6
  8C0C87B8  6EF3  mov r15,r14
  8C0C87BA  4728  shll16 r7
  8C0C87BC  2E52  mov.l r5,@r14
  8C0C87BE  53F4  mov.l @(16,r15),r3
  8C0C87C0  4618  shll8 r6
  8C0C87C2  267B  or r7,r6
  8C0C87C4  263B  or r3,r6
  8C0C87C6  E300  mov ##0x00,r3
  8C0C87C8  1E61  mov.l r6,@(4,r14)
  8C0C87CA  2F36  mov.l r3,@-r15
  8C0C87CC  E702  mov ##0x02,r7
  8C0C87CE  66E3  mov r14,r6
  8C0C87D0  2F36  mov.l r3,@-r15
  8C0C87D2  B969  bsr 8C0C7AA8
  8C0C87D4  E50B  mov ##0x0B,r5
  8C0C87D6  7F10  add ##16,r15
  8C0C87D8  4F26  lds.l @r15+,PR
  8C0C87DA  000B  rts
  8C0C87DC  6EF6  mov.l @r15+,r14
  8C0C87DE  D332  mov.l @([8C0C88A8]),r3
  8C0C87E0  4F22  sts.l PR,@-r15
  8C0C87E2  6032  mov.l @r3,r0
  8C0C87E4  401B  tas.b @r0
  8C0C87E6  8907  bt 8C0C87F8
  ...       (não compilado nesta sessão)
  8C0C87F8  53F3  mov.l @(12,r15),r3
  8C0C87FA  2F36  mov.l r3,@-r15
  8C0C87FC  52F3  mov.l @(12,r15),r2
  8C0C87FE  2F26  mov.l r2,@-r15
  8C0C8800  53F3  mov.l @(12,r15),r3
  8C0C8802  2F36  mov.l r3,@-r15
  8C0C8804  B008  bsr 8C0C8818
  8C0C8806  0009  nop
  8C0C8808  7F0C  add ##12,r15
  8C0C880A  D327  mov.l @([8C0C88A8]),r3
  8C0C880C  E100  mov ##0x00,r1
  8C0C880E  6232  mov.l @r3,r2
  8C0C8810  2210  mov.b r1,@r2
  8C0C8812  4F26  lds.l @r15+,PR
  8C0C8814  000B  rts
  8C0C8816  0009  nop
  8C0C8818  2FE6  mov.l r14,@-r15
  8C0C881A  4F22  sts.l PR,@-r15
  8C0C881C  7FF8  add ##-8,r15
  8C0C881E  4628  shll16 r6
  8C0C8820  6EF3  mov r15,r14
  8C0C8822  4618  shll8 r6
  8C0C8824  2E52  mov.l r5,@r14
  8C0C8826  4728  shll16 r7
  8C0C8828  53F4  mov.l @(16,r15),r3
  8C0C882A  267B  or r7,r6
  8C0C882C  263B  or r3,r6
  8C0C882E  1E61  mov.l r6,@(4,r14)
  8C0C8830  E702  mov ##0x02,r7
  8C0C8832  53F6  mov.l @(24,r15),r3
  8C0C8834  66E3  mov r14,r6
  8C0C8836  2F36  mov.l r3,@-r15
  8C0C8838  52F6  mov.l @(24,r15),r2
  8C0C883A  2F26  mov.l r2,@-r15
  8C0C883C  B934  bsr 8C0C7AA8
  8C0C883E  E50C  mov ##0x0C,r5
  8C0C8840  7F10  add ##16,r15
  8C0C8842  4F26  lds.l @r15+,PR
  8C0C8844  000B  rts
  8C0C8846  6EF6  mov.l @r15+,r14
  8C0C8848  D317  mov.l @([8C0C88A8]),r3
  8C0C884A  4F22  sts.l PR,@-r15
  8C0C884C  6032  mov.l @r3,r0
  8C0C884E  401B  tas.b @r0
  8C0C8850  8907  bt 8C0C8862
  ...       (não compilado nesta sessão)
  8C0C8862  53F1  mov.l @(4,r15),r3
  8C0C8864  2F36  mov.l r3,@-r15
  8C0C8866  B008  bsr 8C0C887A
  8C0C8868  0009  nop
  8C0C886A  7F04  add ##4,r15
  8C0C886C  D30E  mov.l @([8C0C88A8]),r3
  8C0C886E  E100  mov ##0x00,r1
  8C0C8870  6232  mov.l @r3,r2
  8C0C8872  2210  mov.b r1,@r2
  8C0C8874  4F26  lds.l @r15+,PR
  8C0C8876  000B  rts
  8C0C8878  0009  nop
  8C0C887A  2FE6  mov.l r14,@-r15
  8C0C887C  4F22  sts.l PR,@-r15
  8C0C887E  7FF8  add ##-8,r15
  8C0C8880  4628  shll16 r6
  8C0C8882  6EF3  mov r15,r14
  8C0C8884  4728  shll16 r7
  8C0C8886  2E52  mov.l r5,@r14
  8C0C8888  53F4  mov.l @(16,r15),r3
  8C0C888A  4618  shll8 r6
  8C0C888C  267B  or r7,r6
  8C0C888E  263B  or r3,r6
  8C0C8890  E300  mov ##0x00,r3
  8C0C8892  1E61  mov.l r6,@(4,r14)
  8C0C8894  2F36  mov.l r3,@-r15
  8C0C8896  E702  mov ##0x02,r7
  8C0C8898  66E3  mov r14,r6
  8C0C889A  2F36  mov.l r3,@-r15
  8C0C889C  B904  bsr 8C0C7AA8
  8C0C889E  E50D  mov ##0x0D,r5
  8C0C88A0  7F10  add ##16,r15
  8C0C88A2  4F26  lds.l @r15+,PR
  8C0C88A4  000B  rts
  8C0C88A6  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C0C8998  D30B  mov.l @([8C0C89C8]),r3
  8C0C899A  4F22  sts.l PR,@-r15
  8C0C899C  6032  mov.l @r3,r0
  8C0C899E  401B  tas.b @r0
  8C0C89A0  8907  bt 8C0C89B2
  ...       (não compilado nesta sessão)
  8C0C89B2  B00F  bsr 8C0C89D4
  8C0C89B4  0009  nop
  8C0C89B6  D304  mov.l @([8C0C89C8]),r3
  8C0C89B8  E100  mov ##0x00,r1
  8C0C89BA  6232  mov.l @r3,r2
  8C0C89BC  2210  mov.b r1,@r2
  8C0C89BE  4F26  lds.l @r15+,PR
  8C0C89C0  000B  rts
  8C0C89C2  0009  nop
  ...       (não compilado nesta sessão)
  8C0C89D4  4F22  sts.l PR,@-r15
  8C0C89D6  7FF4  add ##-12,r15
  8C0C89D8  1F61  mov.l r6,@(4,r15)
  8C0C89DA  1F72  mov.l r7,@(8,r15)
  8C0C89DC  E701  mov ##0x01,r7
  8C0C89DE  2F52  mov.l r5,@r15
  8C0C89E0  53F2  mov.l @(8,r15),r3
  8C0C89E2  2F36  mov.l r3,@-r15
  8C0C89E4  52F2  mov.l @(8,r15),r2
  8C0C89E6  2F26  mov.l r2,@-r15
  8C0C89E8  66F3  mov r15,r6
  8C0C89EA  7608  add ##8,r6
  8C0C89EC  B85C  bsr 8C0C7AA8
  8C0C89EE  E50E  mov ##0x0E,r5
  8C0C89F0  7F14  add ##20,r15
  8C0C89F2  4F26  lds.l @r15+,PR
  8C0C89F4  000B  rts
  8C0C89F6  0009  nop
  ...       (não compilado nesta sessão)
```


## King of Fighters The - Evolution (USA) (EnJaEsPt)

Dump: `/mnt/1TB/dcbat/20261007-164133_King_of_Fighters_The_-_Evolution__USA___/jit-113430.txt`

```
; 8C36DACC-8C36DE9A
  ...       (não compilado nesta sessão)
  8C36DAE8  4F22  sts.l PR,@-r15
  8C36DAEA  E300  mov ##0x00,r3
  8C36DAEC  6633  mov r3,r6
  8C36DAEE  6733  mov r3,r7
  8C36DAF0  2F36  mov.l r3,@-r15
  8C36DAF2  2F36  mov.l r3,@-r15
  8C36DAF4  BA28  bsr 8C36CF48
  8C36DAF6  E501  mov ##0x01,r5
  8C36DAF8  7F08  add ##8,r15
  8C36DAFA  4F26  lds.l @r15+,PR
  8C36DAFC  000B  rts
  8C36DAFE  0009  nop
  ...       (não compilado nesta sessão)
  8C36DBCC  D35E  mov.l @([8C36DD48]),r3
  8C36DBCE  4F22  sts.l PR,@-r15
  8C36DBD0  6032  mov.l @r3,r0
  8C36DBD2  401B  tas.b @r0
  8C36DBD4  8907  bt 8C36DBE6
  ...       (não compilado nesta sessão)
  8C36DBE6  B007  bsr 8C36DBF8
  8C36DBE8  0009  nop
  8C36DBEA  D357  mov.l @([8C36DD48]),r3
  8C36DBEC  E100  mov ##0x00,r1
  8C36DBEE  6232  mov.l @r3,r2
  8C36DBF0  2210  mov.b r1,@r2
  8C36DBF2  4F26  lds.l @r15+,PR
  8C36DBF4  000B  rts
  8C36DBF6  0009  nop
  8C36DBF8  2FE6  mov.l r14,@-r15
  8C36DBFA  4F22  sts.l PR,@-r15
  8C36DBFC  7FF8  add ##-8,r15
  8C36DBFE  4628  shll16 r6
  8C36DC00  6EF3  mov r15,r14
  8C36DC02  4618  shll8 r6
  8C36DC04  2E52  mov.l r5,@r14
  8C36DC06  E300  mov ##0x00,r3
  8C36DC08  1E61  mov.l r6,@(4,r14)
  8C36DC0A  2F36  mov.l r3,@-r15
  8C36DC0C  E702  mov ##0x02,r7
  8C36DC0E  66E3  mov r14,r6
  8C36DC10  2F36  mov.l r3,@-r15
  8C36DC12  B999  bsr 8C36CF48
  8C36DC14  E50A  mov ##0x0A,r5
  8C36DC16  7F10  add ##16,r15
  8C36DC18  4F26  lds.l @r15+,PR
  8C36DC1A  000B  rts
  8C36DC1C  6EF6  mov.l @r15+,r14
  8C36DC1E  D34A  mov.l @([8C36DD48]),r3
  8C36DC20  4F22  sts.l PR,@-r15
  8C36DC22  6032  mov.l @r3,r0
  8C36DC24  401B  tas.b @r0
  8C36DC26  8907  bt 8C36DC38
  ...       (não compilado nesta sessão)
  8C36DC38  53F1  mov.l @(4,r15),r3
  8C36DC3A  2F36  mov.l r3,@-r15
  8C36DC3C  B008  bsr 8C36DC50
  8C36DC3E  0009  nop
  8C36DC40  7F04  add ##4,r15
  8C36DC42  D341  mov.l @([8C36DD48]),r3
  8C36DC44  E100  mov ##0x00,r1
  8C36DC46  6232  mov.l @r3,r2
  8C36DC48  2210  mov.b r1,@r2
  8C36DC4A  4F26  lds.l @r15+,PR
  8C36DC4C  000B  rts
  8C36DC4E  0009  nop
  8C36DC50  2FE6  mov.l r14,@-r15
  8C36DC52  4F22  sts.l PR,@-r15
  8C36DC54  7FF8  add ##-8,r15
  8C36DC56  4628  shll16 r6
  8C36DC58  6EF3  mov r15,r14
  8C36DC5A  4728  shll16 r7
  8C36DC5C  2E52  mov.l r5,@r14
  8C36DC5E  53F4  mov.l @(16,r15),r3
  8C36DC60  4618  shll8 r6
  8C36DC62  267B  or r7,r6
  8C36DC64  263B  or r3,r6
  8C36DC66  E300  mov ##0x00,r3
  8C36DC68  1E61  mov.l r6,@(4,r14)
  8C36DC6A  2F36  mov.l r3,@-r15
  8C36DC6C  E702  mov ##0x02,r7
  8C36DC6E  66E3  mov r14,r6
  8C36DC70  2F36  mov.l r3,@-r15
  8C36DC72  B969  bsr 8C36CF48
  8C36DC74  E50B  mov ##0x0B,r5
  8C36DC76  7F10  add ##16,r15
  8C36DC78  4F26  lds.l @r15+,PR
  8C36DC7A  000B  rts
  8C36DC7C  6EF6  mov.l @r15+,r14
  8C36DC7E  D332  mov.l @([8C36DD48]),r3
  8C36DC80  4F22  sts.l PR,@-r15
  8C36DC82  6032  mov.l @r3,r0
  8C36DC84  401B  tas.b @r0
  8C36DC86  8907  bt 8C36DC98
  ...       (não compilado nesta sessão)
  8C36DC98  53F3  mov.l @(12,r15),r3
  8C36DC9A  2F36  mov.l r3,@-r15
  8C36DC9C  52F3  mov.l @(12,r15),r2
  8C36DC9E  2F26  mov.l r2,@-r15
  8C36DCA0  53F3  mov.l @(12,r15),r3
  8C36DCA2  2F36  mov.l r3,@-r15
  8C36DCA4  B008  bsr 8C36DCB8
  8C36DCA6  0009  nop
  8C36DCA8  7F0C  add ##12,r15
  8C36DCAA  D327  mov.l @([8C36DD48]),r3
  8C36DCAC  E100  mov ##0x00,r1
  8C36DCAE  6232  mov.l @r3,r2
  8C36DCB0  2210  mov.b r1,@r2
  8C36DCB2  4F26  lds.l @r15+,PR
  8C36DCB4  000B  rts
  8C36DCB6  0009  nop
  8C36DCB8  2FE6  mov.l r14,@-r15
  8C36DCBA  4F22  sts.l PR,@-r15
  8C36DCBC  7FF8  add ##-8,r15
  8C36DCBE  4628  shll16 r6
  8C36DCC0  6EF3  mov r15,r14
  8C36DCC2  4618  shll8 r6
  8C36DCC4  2E52  mov.l r5,@r14
  8C36DCC6  4728  shll16 r7
  8C36DCC8  53F4  mov.l @(16,r15),r3
  8C36DCCA  267B  or r7,r6
  8C36DCCC  263B  or r3,r6
  8C36DCCE  1E61  mov.l r6,@(4,r14)
  8C36DCD0  E702  mov ##0x02,r7
  8C36DCD2  53F6  mov.l @(24,r15),r3
  8C36DCD4  66E3  mov r14,r6
  8C36DCD6  2F36  mov.l r3,@-r15
  8C36DCD8  52F6  mov.l @(24,r15),r2
  8C36DCDA  2F26  mov.l r2,@-r15
  8C36DCDC  B934  bsr 8C36CF48
  8C36DCDE  E50C  mov ##0x0C,r5
  8C36DCE0  7F10  add ##16,r15
  8C36DCE2  4F26  lds.l @r15+,PR
  8C36DCE4  000B  rts
  8C36DCE6  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C36DE38  D30B  mov.l @([8C36DE68]),r3
  8C36DE3A  4F22  sts.l PR,@-r15
  8C36DE3C  6032  mov.l @r3,r0
  8C36DE3E  401B  tas.b @r0
  8C36DE40  8907  bt 8C36DE52
  ...       (não compilado nesta sessão)
  8C36DE52  B00F  bsr 8C36DE74
  8C36DE54  0009  nop
  8C36DE56  D304  mov.l @([8C36DE68]),r3
  8C36DE58  E100  mov ##0x00,r1
  8C36DE5A  6232  mov.l @r3,r2
  8C36DE5C  2210  mov.b r1,@r2
  8C36DE5E  4F26  lds.l @r15+,PR
  8C36DE60  000B  rts
  8C36DE62  0009  nop
  ...       (não compilado nesta sessão)
  8C36DE74  4F22  sts.l PR,@-r15
  8C36DE76  7FF4  add ##-12,r15
  8C36DE78  1F61  mov.l r6,@(4,r15)
  8C36DE7A  1F72  mov.l r7,@(8,r15)
  8C36DE7C  E701  mov ##0x01,r7
  8C36DE7E  2F52  mov.l r5,@r15
  8C36DE80  53F2  mov.l @(8,r15),r3
  8C36DE82  2F36  mov.l r3,@-r15
  8C36DE84  52F2  mov.l @(8,r15),r2
  8C36DE86  2F26  mov.l r2,@-r15
  8C36DE88  66F3  mov r15,r6
  8C36DE8A  7608  add ##8,r6
  8C36DE8C  B85C  bsr 8C36CF48
  8C36DE8E  E50E  mov ##0x0E,r5
  8C36DE90  7F14  add ##20,r15
  8C36DE92  4F26  lds.l @r15+,PR
  8C36DE94  000B  rts
  8C36DE96  0009  nop
  ...       (não compilado nesta sessão)
```


## Le Mans 24 Hours (Europe) (En,Fr,De,Es,It)

Dump: `/mnt/1TB/dcbat/20261007-163759_Le_Mans_24_Hours__Europe___En_Fr_De_Es_I/jit-109461.txt`

```
; 8C21BFE8-8C21C3B6
  ...       (não compilado nesta sessão)
  8C21C004  4F22  sts.l PR,@-r15
  8C21C006  E300  mov ##0x00,r3
  8C21C008  6633  mov r3,r6
  8C21C00A  6733  mov r3,r7
  8C21C00C  2F36  mov.l r3,@-r15
  8C21C00E  2F36  mov.l r3,@-r15
  8C21C010  BA28  bsr 8C21B464
  8C21C012  E501  mov ##0x01,r5
  8C21C014  7F08  add ##8,r15
  8C21C016  4F26  lds.l @r15+,PR
  8C21C018  000B  rts
  8C21C01A  0009  nop
  ...       (não compilado nesta sessão)
  8C21C0E8  D35E  mov.l @([8C21C264]),r3
  8C21C0EA  4F22  sts.l PR,@-r15
  8C21C0EC  6032  mov.l @r3,r0
  8C21C0EE  401B  tas.b @r0
  8C21C0F0  8907  bt 8C21C102
  ...       (não compilado nesta sessão)
  8C21C102  B007  bsr 8C21C114
  8C21C104  0009  nop
  8C21C106  D357  mov.l @([8C21C264]),r3
  8C21C108  E100  mov ##0x00,r1
  8C21C10A  6232  mov.l @r3,r2
  8C21C10C  2210  mov.b r1,@r2
  8C21C10E  4F26  lds.l @r15+,PR
  8C21C110  000B  rts
  8C21C112  0009  nop
  8C21C114  2FE6  mov.l r14,@-r15
  8C21C116  4F22  sts.l PR,@-r15
  8C21C118  7FF8  add ##-8,r15
  8C21C11A  4628  shll16 r6
  8C21C11C  6EF3  mov r15,r14
  8C21C11E  4618  shll8 r6
  8C21C120  2E52  mov.l r5,@r14
  8C21C122  E300  mov ##0x00,r3
  8C21C124  1E61  mov.l r6,@(4,r14)
  8C21C126  2F36  mov.l r3,@-r15
  8C21C128  E702  mov ##0x02,r7
  8C21C12A  66E3  mov r14,r6
  8C21C12C  2F36  mov.l r3,@-r15
  8C21C12E  B999  bsr 8C21B464
  8C21C130  E50A  mov ##0x0A,r5
  8C21C132  7F10  add ##16,r15
  8C21C134  4F26  lds.l @r15+,PR
  8C21C136  000B  rts
  8C21C138  6EF6  mov.l @r15+,r14
  8C21C13A  D34A  mov.l @([8C21C264]),r3
  8C21C13C  4F22  sts.l PR,@-r15
  8C21C13E  6032  mov.l @r3,r0
  8C21C140  401B  tas.b @r0
  8C21C142  8907  bt 8C21C154
  ...       (não compilado nesta sessão)
  8C21C154  53F1  mov.l @(4,r15),r3
  8C21C156  2F36  mov.l r3,@-r15
  8C21C158  B008  bsr 8C21C16C
  8C21C15A  0009  nop
  8C21C15C  7F04  add ##4,r15
  8C21C15E  D341  mov.l @([8C21C264]),r3
  8C21C160  E100  mov ##0x00,r1
  8C21C162  6232  mov.l @r3,r2
  8C21C164  2210  mov.b r1,@r2
  8C21C166  4F26  lds.l @r15+,PR
  8C21C168  000B  rts
  8C21C16A  0009  nop
  8C21C16C  2FE6  mov.l r14,@-r15
  8C21C16E  4F22  sts.l PR,@-r15
  8C21C170  7FF8  add ##-8,r15
  8C21C172  4628  shll16 r6
  8C21C174  6EF3  mov r15,r14
  8C21C176  4728  shll16 r7
  8C21C178  2E52  mov.l r5,@r14
  8C21C17A  53F4  mov.l @(16,r15),r3
  8C21C17C  4618  shll8 r6
  8C21C17E  267B  or r7,r6
  8C21C180  263B  or r3,r6
  8C21C182  E300  mov ##0x00,r3
  8C21C184  1E61  mov.l r6,@(4,r14)
  8C21C186  2F36  mov.l r3,@-r15
  8C21C188  E702  mov ##0x02,r7
  8C21C18A  66E3  mov r14,r6
  8C21C18C  2F36  mov.l r3,@-r15
  8C21C18E  B969  bsr 8C21B464
  8C21C190  E50B  mov ##0x0B,r5
  8C21C192  7F10  add ##16,r15
  8C21C194  4F26  lds.l @r15+,PR
  8C21C196  000B  rts
  8C21C198  6EF6  mov.l @r15+,r14
  8C21C19A  D332  mov.l @([8C21C264]),r3
  8C21C19C  4F22  sts.l PR,@-r15
  8C21C19E  6032  mov.l @r3,r0
  8C21C1A0  401B  tas.b @r0
  8C21C1A2  8907  bt 8C21C1B4
  ...       (não compilado nesta sessão)
  8C21C1B4  53F3  mov.l @(12,r15),r3
  8C21C1B6  2F36  mov.l r3,@-r15
  8C21C1B8  52F3  mov.l @(12,r15),r2
  8C21C1BA  2F26  mov.l r2,@-r15
  8C21C1BC  53F3  mov.l @(12,r15),r3
  8C21C1BE  2F36  mov.l r3,@-r15
  8C21C1C0  B008  bsr 8C21C1D4
  8C21C1C2  0009  nop
  8C21C1C4  7F0C  add ##12,r15
  8C21C1C6  D327  mov.l @([8C21C264]),r3
  8C21C1C8  E100  mov ##0x00,r1
  8C21C1CA  6232  mov.l @r3,r2
  8C21C1CC  2210  mov.b r1,@r2
  8C21C1CE  4F26  lds.l @r15+,PR
  8C21C1D0  000B  rts
  8C21C1D2  0009  nop
  8C21C1D4  2FE6  mov.l r14,@-r15
  8C21C1D6  4F22  sts.l PR,@-r15
  8C21C1D8  7FF8  add ##-8,r15
  8C21C1DA  4628  shll16 r6
  8C21C1DC  6EF3  mov r15,r14
  8C21C1DE  4618  shll8 r6
  8C21C1E0  2E52  mov.l r5,@r14
  8C21C1E2  4728  shll16 r7
  8C21C1E4  53F4  mov.l @(16,r15),r3
  8C21C1E6  267B  or r7,r6
  8C21C1E8  263B  or r3,r6
  8C21C1EA  1E61  mov.l r6,@(4,r14)
  8C21C1EC  E702  mov ##0x02,r7
  8C21C1EE  53F6  mov.l @(24,r15),r3
  8C21C1F0  66E3  mov r14,r6
  8C21C1F2  2F36  mov.l r3,@-r15
  8C21C1F4  52F6  mov.l @(24,r15),r2
  8C21C1F6  2F26  mov.l r2,@-r15
  8C21C1F8  B934  bsr 8C21B464
  8C21C1FA  E50C  mov ##0x0C,r5
  8C21C1FC  7F10  add ##16,r15
  8C21C1FE  4F26  lds.l @r15+,PR
  8C21C200  000B  rts
  8C21C202  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C21C354  D30B  mov.l @([8C21C384]),r3
  8C21C356  4F22  sts.l PR,@-r15
  8C21C358  6032  mov.l @r3,r0
  8C21C35A  401B  tas.b @r0
  8C21C35C  8907  bt 8C21C36E
  ...       (não compilado nesta sessão)
  8C21C36E  B00F  bsr 8C21C390
  8C21C370  0009  nop
  8C21C372  D304  mov.l @([8C21C384]),r3
  8C21C374  E100  mov ##0x00,r1
  8C21C376  6232  mov.l @r3,r2
  8C21C378  2210  mov.b r1,@r2
  8C21C37A  4F26  lds.l @r15+,PR
  8C21C37C  000B  rts
  8C21C37E  0009  nop
  ...       (não compilado nesta sessão)
  8C21C390  4F22  sts.l PR,@-r15
  8C21C392  7FF4  add ##-12,r15
  8C21C394  1F61  mov.l r6,@(4,r15)
  8C21C396  1F72  mov.l r7,@(8,r15)
  8C21C398  E701  mov ##0x01,r7
  8C21C39A  2F52  mov.l r5,@r15
  8C21C39C  53F2  mov.l @(8,r15),r3
  8C21C39E  2F36  mov.l r3,@-r15
  8C21C3A0  52F2  mov.l @(8,r15),r2
  8C21C3A2  2F26  mov.l r2,@-r15
  8C21C3A4  66F3  mov r15,r6
  8C21C3A6  7608  add ##8,r6
  8C21C3A8  B85C  bsr 8C21B464
  8C21C3AA  E50E  mov ##0x0E,r5
  8C21C3AC  7F14  add ##20,r15
  8C21C3AE  4F26  lds.l @r15+,PR
  8C21C3B0  000B  rts
  8C21C3B2  0009  nop
  ...       (não compilado nesta sessão)
```


## Macross M3

Dump: `/mnt/1TB/dcbat/20261007-153242_Macross_M3_/jit-35982.txt`

```
; 8C1D0D4C-8C1D0F60
  ...       (não compilado nesta sessão)
  8C1D0D68  4F22  sts.l PR,@-r15
  8C1D0D6A  E300  mov ##0x00,r3
  8C1D0D6C  6633  mov r3,r6
  8C1D0D6E  6733  mov r3,r7
  8C1D0D70  2F36  mov.l r3,@-r15
  8C1D0D72  2F36  mov.l r3,@-r15
  8C1D0D74  BA28  bsr 8C1D01C8
  8C1D0D76  E501  mov ##0x01,r5
  8C1D0D78  7F08  add ##8,r15
  8C1D0D7A  4F26  lds.l @r15+,PR
  8C1D0D7C  000B  rts
  8C1D0D7E  0009  nop
  ...       (não compilado nesta sessão)
  8C1D0E4C  D35E  mov.l @([8C1D0FC8]),r3
  8C1D0E4E  4F22  sts.l PR,@-r15
  8C1D0E50  6032  mov.l @r3,r0
  8C1D0E52  401B  tas.b @r0
  8C1D0E54  8907  bt 8C1D0E66
  ...       (não compilado nesta sessão)
  8C1D0E66  B007  bsr 8C1D0E78
  8C1D0E68  0009  nop
  8C1D0E6A  D357  mov.l @([8C1D0FC8]),r3
  8C1D0E6C  E100  mov ##0x00,r1
  8C1D0E6E  6232  mov.l @r3,r2
  8C1D0E70  2210  mov.b r1,@r2
  8C1D0E72  4F26  lds.l @r15+,PR
  8C1D0E74  000B  rts
  8C1D0E76  0009  nop
  8C1D0E78  2FE6  mov.l r14,@-r15
  8C1D0E7A  4F22  sts.l PR,@-r15
  8C1D0E7C  7FF8  add ##-8,r15
  8C1D0E7E  4628  shll16 r6
  8C1D0E80  6EF3  mov r15,r14
  8C1D0E82  4618  shll8 r6
  8C1D0E84  2E52  mov.l r5,@r14
  8C1D0E86  E300  mov ##0x00,r3
  8C1D0E88  1E61  mov.l r6,@(4,r14)
  8C1D0E8A  2F36  mov.l r3,@-r15
  8C1D0E8C  E702  mov ##0x02,r7
  8C1D0E8E  66E3  mov r14,r6
  8C1D0E90  2F36  mov.l r3,@-r15
  8C1D0E92  B999  bsr 8C1D01C8
  8C1D0E94  E50A  mov ##0x0A,r5
  8C1D0E96  7F10  add ##16,r15
  8C1D0E98  4F26  lds.l @r15+,PR
  8C1D0E9A  000B  rts
  8C1D0E9C  6EF6  mov.l @r15+,r14
  8C1D0E9E  D34A  mov.l @([8C1D0FC8]),r3
  8C1D0EA0  4F22  sts.l PR,@-r15
  8C1D0EA2  6032  mov.l @r3,r0
  8C1D0EA4  401B  tas.b @r0
  8C1D0EA6  8907  bt 8C1D0EB8
  ...       (não compilado nesta sessão)
  8C1D0EB8  53F1  mov.l @(4,r15),r3
  8C1D0EBA  2F36  mov.l r3,@-r15
  8C1D0EBC  B008  bsr 8C1D0ED0
  8C1D0EBE  0009  nop
  8C1D0EC0  7F04  add ##4,r15
  8C1D0EC2  D341  mov.l @([8C1D0FC8]),r3
  8C1D0EC4  E100  mov ##0x00,r1
  8C1D0EC6  6232  mov.l @r3,r2
  8C1D0EC8  2210  mov.b r1,@r2
  8C1D0ECA  4F26  lds.l @r15+,PR
  8C1D0ECC  000B  rts
  8C1D0ECE  0009  nop
  8C1D0ED0  2FE6  mov.l r14,@-r15
  8C1D0ED2  4F22  sts.l PR,@-r15
  8C1D0ED4  7FF8  add ##-8,r15
  8C1D0ED6  4628  shll16 r6
  8C1D0ED8  6EF3  mov r15,r14
  8C1D0EDA  4728  shll16 r7
  8C1D0EDC  2E52  mov.l r5,@r14
  8C1D0EDE  53F4  mov.l @(16,r15),r3
  8C1D0EE0  4618  shll8 r6
  8C1D0EE2  267B  or r7,r6
  8C1D0EE4  263B  or r3,r6
  8C1D0EE6  E300  mov ##0x00,r3
  8C1D0EE8  1E61  mov.l r6,@(4,r14)
  8C1D0EEA  2F36  mov.l r3,@-r15
  8C1D0EEC  E702  mov ##0x02,r7
  8C1D0EEE  66E3  mov r14,r6
  8C1D0EF0  2F36  mov.l r3,@-r15
  8C1D0EF2  B969  bsr 8C1D01C8
  8C1D0EF4  E50B  mov ##0x0B,r5
  8C1D0EF6  7F10  add ##16,r15
  8C1D0EF8  4F26  lds.l @r15+,PR
  8C1D0EFA  000B  rts
  8C1D0EFC  6EF6  mov.l @r15+,r14
  8C1D0EFE  D332  mov.l @([8C1D0FC8]),r3
  8C1D0F00  4F22  sts.l PR,@-r15
  8C1D0F02  6032  mov.l @r3,r0
  8C1D0F04  401B  tas.b @r0
  8C1D0F06  8907  bt 8C1D0F18
  ...       (não compilado nesta sessão)
  8C1D0F18  53F3  mov.l @(12,r15),r3
  8C1D0F1A  2F36  mov.l r3,@-r15
  8C1D0F1C  52F3  mov.l @(12,r15),r2
  8C1D0F1E  2F26  mov.l r2,@-r15
  8C1D0F20  53F3  mov.l @(12,r15),r3
  8C1D0F22  2F36  mov.l r3,@-r15
  8C1D0F24  B008  bsr 8C1D0F38
  8C1D0F26  0009  nop
  8C1D0F28  7F0C  add ##12,r15
  8C1D0F2A  D327  mov.l @([8C1D0FC8]),r3
  8C1D0F2C  E100  mov ##0x00,r1
  8C1D0F2E  6232  mov.l @r3,r2
  8C1D0F30  2210  mov.b r1,@r2
  8C1D0F32  4F26  lds.l @r15+,PR
  8C1D0F34  000B  rts
  8C1D0F36  0009  nop
  8C1D0F38  2FE6  mov.l r14,@-r15
  8C1D0F3A  4F22  sts.l PR,@-r15
  8C1D0F3C  7FF8  add ##-8,r15
  8C1D0F3E  4628  shll16 r6
  8C1D0F40  6EF3  mov r15,r14
  8C1D0F42  4618  shll8 r6
  8C1D0F44  2E52  mov.l r5,@r14
  8C1D0F46  4728  shll16 r7
  8C1D0F48  53F4  mov.l @(16,r15),r3
  8C1D0F4A  267B  or r7,r6
  8C1D0F4C  263B  or r3,r6
  8C1D0F4E  1E61  mov.l r6,@(4,r14)
  8C1D0F50  E702  mov ##0x02,r7
  8C1D0F52  53F6  mov.l @(24,r15),r3
  8C1D0F54  66E3  mov r14,r6
  8C1D0F56  2F36  mov.l r3,@-r15
  8C1D0F58  52F6  mov.l @(24,r15),r2
  8C1D0F5A  2F26  mov.l r2,@-r15
  8C1D0F5C  B934  bsr 8C1D01C8
  8C1D0F5E  E50C  mov ##0x0C,r5
```


## Marvel vs. Capcom 2 - New Age of Heroes (Europe)

Dump: `/mnt/1TB/dcbat_off/20261007-191632_Marvel_vs__Capcom_2_-_New_Age_of_Heroes_/jit-9882.txt`

```
; 8C1986E0-8C198AAE
  ...       (não compilado nesta sessão)
  8C1986FC  4F22  sts.l PR,@-r15
  8C1986FE  E300  mov ##0x00,r3
  8C198700  6633  mov r3,r6
  8C198702  6733  mov r3,r7
  8C198704  2F36  mov.l r3,@-r15
  8C198706  2F36  mov.l r3,@-r15
  8C198708  BA28  bsr 8C197B5C
  8C19870A  E501  mov ##0x01,r5
  8C19870C  7F08  add ##8,r15
  8C19870E  4F26  lds.l @r15+,PR
  8C198710  000B  rts
  8C198712  0009  nop
  ...       (não compilado nesta sessão)
  8C1987E0  D35E  mov.l @([8C19895C]),r3
  8C1987E2  4F22  sts.l PR,@-r15
  8C1987E4  6032  mov.l @r3,r0
  8C1987E6  401B  tas.b @r0
  8C1987E8  8907  bt 8C1987FA
  ...       (não compilado nesta sessão)
  8C1987FA  B007  bsr 8C19880C
  8C1987FC  0009  nop
  8C1987FE  D357  mov.l @([8C19895C]),r3
  8C198800  E100  mov ##0x00,r1
  8C198802  6232  mov.l @r3,r2
  8C198804  2210  mov.b r1,@r2
  8C198806  4F26  lds.l @r15+,PR
  8C198808  000B  rts
  8C19880A  0009  nop
  8C19880C  2FE6  mov.l r14,@-r15
  8C19880E  4F22  sts.l PR,@-r15
  8C198810  7FF8  add ##-8,r15
  8C198812  4628  shll16 r6
  8C198814  6EF3  mov r15,r14
  8C198816  4618  shll8 r6
  8C198818  2E52  mov.l r5,@r14
  8C19881A  E300  mov ##0x00,r3
  8C19881C  1E61  mov.l r6,@(4,r14)
  8C19881E  2F36  mov.l r3,@-r15
  8C198820  E702  mov ##0x02,r7
  8C198822  66E3  mov r14,r6
  8C198824  2F36  mov.l r3,@-r15
  8C198826  B999  bsr 8C197B5C
  8C198828  E50A  mov ##0x0A,r5
  8C19882A  7F10  add ##16,r15
  8C19882C  4F26  lds.l @r15+,PR
  8C19882E  000B  rts
  8C198830  6EF6  mov.l @r15+,r14
  8C198832  D34A  mov.l @([8C19895C]),r3
  8C198834  4F22  sts.l PR,@-r15
  8C198836  6032  mov.l @r3,r0
  8C198838  401B  tas.b @r0
  8C19883A  8907  bt 8C19884C
  ...       (não compilado nesta sessão)
  8C19884C  53F1  mov.l @(4,r15),r3
  8C19884E  2F36  mov.l r3,@-r15
  8C198850  B008  bsr 8C198864
  8C198852  0009  nop
  8C198854  7F04  add ##4,r15
  8C198856  D341  mov.l @([8C19895C]),r3
  8C198858  E100  mov ##0x00,r1
  8C19885A  6232  mov.l @r3,r2
  8C19885C  2210  mov.b r1,@r2
  8C19885E  4F26  lds.l @r15+,PR
  8C198860  000B  rts
  8C198862  0009  nop
  8C198864  2FE6  mov.l r14,@-r15
  8C198866  4F22  sts.l PR,@-r15
  8C198868  7FF8  add ##-8,r15
  8C19886A  4628  shll16 r6
  8C19886C  6EF3  mov r15,r14
  8C19886E  4728  shll16 r7
  8C198870  2E52  mov.l r5,@r14
  8C198872  53F4  mov.l @(16,r15),r3
  8C198874  4618  shll8 r6
  8C198876  267B  or r7,r6
  8C198878  263B  or r3,r6
  8C19887A  E300  mov ##0x00,r3
  8C19887C  1E61  mov.l r6,@(4,r14)
  8C19887E  2F36  mov.l r3,@-r15
  8C198880  E702  mov ##0x02,r7
  8C198882  66E3  mov r14,r6
  8C198884  2F36  mov.l r3,@-r15
  8C198886  B969  bsr 8C197B5C
  8C198888  E50B  mov ##0x0B,r5
  8C19888A  7F10  add ##16,r15
  8C19888C  4F26  lds.l @r15+,PR
  8C19888E  000B  rts
  8C198890  6EF6  mov.l @r15+,r14
  8C198892  D332  mov.l @([8C19895C]),r3
  8C198894  4F22  sts.l PR,@-r15
  8C198896  6032  mov.l @r3,r0
  8C198898  401B  tas.b @r0
  8C19889A  8907  bt 8C1988AC
  ...       (não compilado nesta sessão)
  8C1988AC  53F3  mov.l @(12,r15),r3
  8C1988AE  2F36  mov.l r3,@-r15
  8C1988B0  52F3  mov.l @(12,r15),r2
  8C1988B2  2F26  mov.l r2,@-r15
  8C1988B4  53F3  mov.l @(12,r15),r3
  8C1988B6  2F36  mov.l r3,@-r15
  8C1988B8  B008  bsr 8C1988CC
  8C1988BA  0009  nop
  8C1988BC  7F0C  add ##12,r15
  8C1988BE  D327  mov.l @([8C19895C]),r3
  8C1988C0  E100  mov ##0x00,r1
  8C1988C2  6232  mov.l @r3,r2
  8C1988C4  2210  mov.b r1,@r2
  8C1988C6  4F26  lds.l @r15+,PR
  8C1988C8  000B  rts
  8C1988CA  0009  nop
  8C1988CC  2FE6  mov.l r14,@-r15
  8C1988CE  4F22  sts.l PR,@-r15
  8C1988D0  7FF8  add ##-8,r15
  8C1988D2  4628  shll16 r6
  8C1988D4  6EF3  mov r15,r14
  8C1988D6  4618  shll8 r6
  8C1988D8  2E52  mov.l r5,@r14
  8C1988DA  4728  shll16 r7
  8C1988DC  53F4  mov.l @(16,r15),r3
  8C1988DE  267B  or r7,r6
  8C1988E0  263B  or r3,r6
  8C1988E2  1E61  mov.l r6,@(4,r14)
  8C1988E4  E702  mov ##0x02,r7
  8C1988E6  53F6  mov.l @(24,r15),r3
  8C1988E8  66E3  mov r14,r6
  8C1988EA  2F36  mov.l r3,@-r15
  8C1988EC  52F6  mov.l @(24,r15),r2
  8C1988EE  2F26  mov.l r2,@-r15
  8C1988F0  B934  bsr 8C197B5C
  8C1988F2  E50C  mov ##0x0C,r5
  8C1988F4  7F10  add ##16,r15
  8C1988F6  4F26  lds.l @r15+,PR
  8C1988F8  000B  rts
  8C1988FA  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C198A4C  D30B  mov.l @([8C198A7C]),r3
  8C198A4E  4F22  sts.l PR,@-r15
  8C198A50  6032  mov.l @r3,r0
  8C198A52  401B  tas.b @r0
  8C198A54  8907  bt 8C198A66
  ...       (não compilado nesta sessão)
  8C198A66  B00F  bsr 8C198A88
  8C198A68  0009  nop
  8C198A6A  D304  mov.l @([8C198A7C]),r3
  8C198A6C  E100  mov ##0x00,r1
  8C198A6E  6232  mov.l @r3,r2
  8C198A70  2210  mov.b r1,@r2
  8C198A72  4F26  lds.l @r15+,PR
  8C198A74  000B  rts
  8C198A76  0009  nop
  ...       (não compilado nesta sessão)
  8C198A88  4F22  sts.l PR,@-r15
  8C198A8A  7FF4  add ##-12,r15
  8C198A8C  1F61  mov.l r6,@(4,r15)
  8C198A8E  1F72  mov.l r7,@(8,r15)
  8C198A90  E701  mov ##0x01,r7
  8C198A92  2F52  mov.l r5,@r15
  8C198A94  53F2  mov.l @(8,r15),r3
  8C198A96  2F36  mov.l r3,@-r15
  8C198A98  52F2  mov.l @(8,r15),r2
  8C198A9A  2F26  mov.l r2,@-r15
  8C198A9C  66F3  mov r15,r6
  8C198A9E  7608  add ##8,r6
  8C198AA0  B85C  bsr 8C197B5C
  8C198AA2  E50E  mov ##0x0E,r5
  8C198AA4  7F14  add ##20,r15
  8C198AA6  4F26  lds.l @r15+,PR
  8C198AA8  000B  rts
  8C198AAA  0009  nop
  ...       (não compilado nesta sessão)
```


## Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0)

Dump: `/mnt/1TB/dcbat_off/20261007-190751_Napple_Tale_-_Arsia_in_Daydream__Japan__/jit-6592.txt`

```
; 8C16AFD8-8C16B1EC
  ...       (não compilado nesta sessão)
  8C16AFF4  4F22  sts.l PR,@-r15
  8C16AFF6  E300  mov ##0x00,r3
  8C16AFF8  6633  mov r3,r6
  8C16AFFA  6733  mov r3,r7
  8C16AFFC  2F36  mov.l r3,@-r15
  8C16AFFE  2F36  mov.l r3,@-r15
  8C16B000  BA28  bsr 8C16A454
  8C16B002  E501  mov ##0x01,r5
  8C16B004  7F08  add ##8,r15
  8C16B006  4F26  lds.l @r15+,PR
  8C16B008  000B  rts
  8C16B00A  0009  nop
  ...       (não compilado nesta sessão)
  8C16B0D8  D35E  mov.l @([8C16B254]),r3
  8C16B0DA  4F22  sts.l PR,@-r15
  8C16B0DC  6032  mov.l @r3,r0
  8C16B0DE  401B  tas.b @r0
  8C16B0E0  8907  bt 8C16B0F2
  ...       (não compilado nesta sessão)
  8C16B0F2  B007  bsr 8C16B104
  8C16B0F4  0009  nop
  8C16B0F6  D357  mov.l @([8C16B254]),r3
  8C16B0F8  E100  mov ##0x00,r1
  8C16B0FA  6232  mov.l @r3,r2
  8C16B0FC  2210  mov.b r1,@r2
  8C16B0FE  4F26  lds.l @r15+,PR
  8C16B100  000B  rts
  8C16B102  0009  nop
  8C16B104  2FE6  mov.l r14,@-r15
  8C16B106  4F22  sts.l PR,@-r15
  8C16B108  7FF8  add ##-8,r15
  8C16B10A  4628  shll16 r6
  8C16B10C  6EF3  mov r15,r14
  8C16B10E  4618  shll8 r6
  8C16B110  2E52  mov.l r5,@r14
  8C16B112  E300  mov ##0x00,r3
  8C16B114  1E61  mov.l r6,@(4,r14)
  8C16B116  2F36  mov.l r3,@-r15
  8C16B118  E702  mov ##0x02,r7
  8C16B11A  66E3  mov r14,r6
  8C16B11C  2F36  mov.l r3,@-r15
  8C16B11E  B999  bsr 8C16A454
  8C16B120  E50A  mov ##0x0A,r5
  8C16B122  7F10  add ##16,r15
  8C16B124  4F26  lds.l @r15+,PR
  8C16B126  000B  rts
  8C16B128  6EF6  mov.l @r15+,r14
  8C16B12A  D34A  mov.l @([8C16B254]),r3
  8C16B12C  4F22  sts.l PR,@-r15
  8C16B12E  6032  mov.l @r3,r0
  8C16B130  401B  tas.b @r0
  8C16B132  8907  bt 8C16B144
  ...       (não compilado nesta sessão)
  8C16B144  53F1  mov.l @(4,r15),r3
  8C16B146  2F36  mov.l r3,@-r15
  8C16B148  B008  bsr 8C16B15C
  8C16B14A  0009  nop
  8C16B14C  7F04  add ##4,r15
  8C16B14E  D341  mov.l @([8C16B254]),r3
  8C16B150  E100  mov ##0x00,r1
  8C16B152  6232  mov.l @r3,r2
  8C16B154  2210  mov.b r1,@r2
  8C16B156  4F26  lds.l @r15+,PR
  8C16B158  000B  rts
  8C16B15A  0009  nop
  8C16B15C  2FE6  mov.l r14,@-r15
  8C16B15E  4F22  sts.l PR,@-r15
  8C16B160  7FF8  add ##-8,r15
  8C16B162  4628  shll16 r6
  8C16B164  6EF3  mov r15,r14
  8C16B166  4728  shll16 r7
  8C16B168  2E52  mov.l r5,@r14
  8C16B16A  53F4  mov.l @(16,r15),r3
  8C16B16C  4618  shll8 r6
  8C16B16E  267B  or r7,r6
  8C16B170  263B  or r3,r6
  8C16B172  E300  mov ##0x00,r3
  8C16B174  1E61  mov.l r6,@(4,r14)
  8C16B176  2F36  mov.l r3,@-r15
  8C16B178  E702  mov ##0x02,r7
  8C16B17A  66E3  mov r14,r6
  8C16B17C  2F36  mov.l r3,@-r15
  8C16B17E  B969  bsr 8C16A454
  8C16B180  E50B  mov ##0x0B,r5
  8C16B182  7F10  add ##16,r15
  8C16B184  4F26  lds.l @r15+,PR
  8C16B186  000B  rts
  8C16B188  6EF6  mov.l @r15+,r14
  8C16B18A  D332  mov.l @([8C16B254]),r3
  8C16B18C  4F22  sts.l PR,@-r15
  8C16B18E  6032  mov.l @r3,r0
  8C16B190  401B  tas.b @r0
  8C16B192  8907  bt 8C16B1A4
  ...       (não compilado nesta sessão)
  8C16B1A4  53F3  mov.l @(12,r15),r3
  8C16B1A6  2F36  mov.l r3,@-r15
  8C16B1A8  52F3  mov.l @(12,r15),r2
  8C16B1AA  2F26  mov.l r2,@-r15
  8C16B1AC  53F3  mov.l @(12,r15),r3
  8C16B1AE  2F36  mov.l r3,@-r15
  8C16B1B0  B008  bsr 8C16B1C4
  8C16B1B2  0009  nop
  8C16B1B4  7F0C  add ##12,r15
  8C16B1B6  D327  mov.l @([8C16B254]),r3
  8C16B1B8  E100  mov ##0x00,r1
  8C16B1BA  6232  mov.l @r3,r2
  8C16B1BC  2210  mov.b r1,@r2
  8C16B1BE  4F26  lds.l @r15+,PR
  8C16B1C0  000B  rts
  8C16B1C2  0009  nop
  8C16B1C4  2FE6  mov.l r14,@-r15
  8C16B1C6  4F22  sts.l PR,@-r15
  8C16B1C8  7FF8  add ##-8,r15
  8C16B1CA  4628  shll16 r6
  8C16B1CC  6EF3  mov r15,r14
  8C16B1CE  4618  shll8 r6
  8C16B1D0  2E52  mov.l r5,@r14
  8C16B1D2  4728  shll16 r7
  8C16B1D4  53F4  mov.l @(16,r15),r3
  8C16B1D6  267B  or r7,r6
  8C16B1D8  263B  or r3,r6
  8C16B1DA  1E61  mov.l r6,@(4,r14)
  8C16B1DC  E702  mov ##0x02,r7
  8C16B1DE  53F6  mov.l @(24,r15),r3
  8C16B1E0  66E3  mov r14,r6
  8C16B1E2  2F36  mov.l r3,@-r15
  8C16B1E4  52F6  mov.l @(24,r15),r2
  8C16B1E6  2F26  mov.l r2,@-r15
  8C16B1E8  B934  bsr 8C16A454
  8C16B1EA  E50C  mov ##0x0C,r5
```


## Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs)

Dump: `/mnt/1TB/dcbat/20261007-155901_Phantasy_Star_Online_Ver__2__USA___EnJaF/jit-73352.txt`

```
; 8C37E604-8C37E9D2
  ...       (não compilado nesta sessão)
  8C37E620  4F22  sts.l PR,@-r15
  8C37E622  E300  mov ##0x00,r3
  8C37E624  6633  mov r3,r6
  8C37E626  6733  mov r3,r7
  8C37E628  2F36  mov.l r3,@-r15
  8C37E62A  2F36  mov.l r3,@-r15
  8C37E62C  BA28  bsr 8C37DA80
  8C37E62E  E501  mov ##0x01,r5
  8C37E630  7F08  add ##8,r15
  8C37E632  4F26  lds.l @r15+,PR
  8C37E634  000B  rts
  8C37E636  0009  nop
  ...       (não compilado nesta sessão)
  8C37E704  D35E  mov.l @([8C37E880]),r3
  8C37E706  4F22  sts.l PR,@-r15
  8C37E708  6032  mov.l @r3,r0
  8C37E70A  401B  tas.b @r0
  8C37E70C  8907  bt 8C37E71E
  ...       (não compilado nesta sessão)
  8C37E71E  B007  bsr 8C37E730
  8C37E720  0009  nop
  8C37E722  D357  mov.l @([8C37E880]),r3
  8C37E724  E100  mov ##0x00,r1
  8C37E726  6232  mov.l @r3,r2
  8C37E728  2210  mov.b r1,@r2
  8C37E72A  4F26  lds.l @r15+,PR
  8C37E72C  000B  rts
  8C37E72E  0009  nop
  8C37E730  2FE6  mov.l r14,@-r15
  8C37E732  4F22  sts.l PR,@-r15
  8C37E734  7FF8  add ##-8,r15
  8C37E736  4628  shll16 r6
  8C37E738  6EF3  mov r15,r14
  8C37E73A  4618  shll8 r6
  8C37E73C  2E52  mov.l r5,@r14
  8C37E73E  E300  mov ##0x00,r3
  8C37E740  1E61  mov.l r6,@(4,r14)
  8C37E742  2F36  mov.l r3,@-r15
  8C37E744  E702  mov ##0x02,r7
  8C37E746  66E3  mov r14,r6
  8C37E748  2F36  mov.l r3,@-r15
  8C37E74A  B999  bsr 8C37DA80
  8C37E74C  E50A  mov ##0x0A,r5
  8C37E74E  7F10  add ##16,r15
  8C37E750  4F26  lds.l @r15+,PR
  8C37E752  000B  rts
  8C37E754  6EF6  mov.l @r15+,r14
  8C37E756  D34A  mov.l @([8C37E880]),r3
  8C37E758  4F22  sts.l PR,@-r15
  8C37E75A  6032  mov.l @r3,r0
  8C37E75C  401B  tas.b @r0
  8C37E75E  8907  bt 8C37E770
  ...       (não compilado nesta sessão)
  8C37E770  53F1  mov.l @(4,r15),r3
  8C37E772  2F36  mov.l r3,@-r15
  8C37E774  B008  bsr 8C37E788
  8C37E776  0009  nop
  8C37E778  7F04  add ##4,r15
  8C37E77A  D341  mov.l @([8C37E880]),r3
  8C37E77C  E100  mov ##0x00,r1
  8C37E77E  6232  mov.l @r3,r2
  8C37E780  2210  mov.b r1,@r2
  8C37E782  4F26  lds.l @r15+,PR
  8C37E784  000B  rts
  8C37E786  0009  nop
  8C37E788  2FE6  mov.l r14,@-r15
  8C37E78A  4F22  sts.l PR,@-r15
  8C37E78C  7FF8  add ##-8,r15
  8C37E78E  4628  shll16 r6
  8C37E790  6EF3  mov r15,r14
  8C37E792  4728  shll16 r7
  8C37E794  2E52  mov.l r5,@r14
  8C37E796  53F4  mov.l @(16,r15),r3
  8C37E798  4618  shll8 r6
  8C37E79A  267B  or r7,r6
  8C37E79C  263B  or r3,r6
  8C37E79E  E300  mov ##0x00,r3
  8C37E7A0  1E61  mov.l r6,@(4,r14)
  8C37E7A2  2F36  mov.l r3,@-r15
  8C37E7A4  E702  mov ##0x02,r7
  8C37E7A6  66E3  mov r14,r6
  8C37E7A8  2F36  mov.l r3,@-r15
  8C37E7AA  B969  bsr 8C37DA80
  8C37E7AC  E50B  mov ##0x0B,r5
  8C37E7AE  7F10  add ##16,r15
  8C37E7B0  4F26  lds.l @r15+,PR
  8C37E7B2  000B  rts
  8C37E7B4  6EF6  mov.l @r15+,r14
  8C37E7B6  D332  mov.l @([8C37E880]),r3
  8C37E7B8  4F22  sts.l PR,@-r15
  8C37E7BA  6032  mov.l @r3,r0
  8C37E7BC  401B  tas.b @r0
  8C37E7BE  8907  bt 8C37E7D0
  ...       (não compilado nesta sessão)
  8C37E7D0  53F3  mov.l @(12,r15),r3
  8C37E7D2  2F36  mov.l r3,@-r15
  8C37E7D4  52F3  mov.l @(12,r15),r2
  8C37E7D6  2F26  mov.l r2,@-r15
  8C37E7D8  53F3  mov.l @(12,r15),r3
  8C37E7DA  2F36  mov.l r3,@-r15
  8C37E7DC  B008  bsr 8C37E7F0
  8C37E7DE  0009  nop
  8C37E7E0  7F0C  add ##12,r15
  8C37E7E2  D327  mov.l @([8C37E880]),r3
  8C37E7E4  E100  mov ##0x00,r1
  8C37E7E6  6232  mov.l @r3,r2
  8C37E7E8  2210  mov.b r1,@r2
  8C37E7EA  4F26  lds.l @r15+,PR
  8C37E7EC  000B  rts
  8C37E7EE  0009  nop
  8C37E7F0  2FE6  mov.l r14,@-r15
  8C37E7F2  4F22  sts.l PR,@-r15
  8C37E7F4  7FF8  add ##-8,r15
  8C37E7F6  4628  shll16 r6
  8C37E7F8  6EF3  mov r15,r14
  8C37E7FA  4618  shll8 r6
  8C37E7FC  2E52  mov.l r5,@r14
  8C37E7FE  4728  shll16 r7
  8C37E800  53F4  mov.l @(16,r15),r3
  8C37E802  267B  or r7,r6
  8C37E804  263B  or r3,r6
  8C37E806  1E61  mov.l r6,@(4,r14)
  8C37E808  E702  mov ##0x02,r7
  8C37E80A  53F6  mov.l @(24,r15),r3
  8C37E80C  66E3  mov r14,r6
  8C37E80E  2F36  mov.l r3,@-r15
  8C37E810  52F6  mov.l @(24,r15),r2
  8C37E812  2F26  mov.l r2,@-r15
  8C37E814  B934  bsr 8C37DA80
  8C37E816  E50C  mov ##0x0C,r5
  8C37E818  7F10  add ##16,r15
  8C37E81A  4F26  lds.l @r15+,PR
  8C37E81C  000B  rts
  8C37E81E  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C37E970  D30B  mov.l @([8C37E9A0]),r3
  8C37E972  4F22  sts.l PR,@-r15
  8C37E974  6032  mov.l @r3,r0
  8C37E976  401B  tas.b @r0
  8C37E978  8907  bt 8C37E98A
  ...       (não compilado nesta sessão)
  8C37E98A  B00F  bsr 8C37E9AC
  8C37E98C  0009  nop
  8C37E98E  D304  mov.l @([8C37E9A0]),r3
  8C37E990  E100  mov ##0x00,r1
  8C37E992  6232  mov.l @r3,r2
  8C37E994  2210  mov.b r1,@r2
  8C37E996  4F26  lds.l @r15+,PR
  8C37E998  000B  rts
  8C37E99A  0009  nop
  ...       (não compilado nesta sessão)
  8C37E9AC  4F22  sts.l PR,@-r15
  8C37E9AE  7FF4  add ##-12,r15
  8C37E9B0  1F61  mov.l r6,@(4,r15)
  8C37E9B2  1F72  mov.l r7,@(8,r15)
  8C37E9B4  E701  mov ##0x01,r7
  8C37E9B6  2F52  mov.l r5,@r15
  8C37E9B8  53F2  mov.l @(8,r15),r3
  8C37E9BA  2F36  mov.l r3,@-r15
  8C37E9BC  52F2  mov.l @(8,r15),r2
  8C37E9BE  2F26  mov.l r2,@-r15
  8C37E9C0  66F3  mov r15,r6
  8C37E9C2  7608  add ##8,r6
  8C37E9C4  B85C  bsr 8C37DA80
  8C37E9C6  E50E  mov ##0x0E,r5
  8C37E9C8  7F14  add ##20,r15
  8C37E9CA  4F26  lds.l @r15+,PR
  8C37E9CC  000B  rts
  8C37E9CE  0009  nop
  ...       (não compilado nesta sessão)
```


## Power Stone (USA)

Dump: `/mnt/1TB/dcbat/20261007-160119_Power_Stone__USA__/jit-75898.txt`

```
; 0C112AF4-0C112EC2
  ...       (não compilado nesta sessão)
  0C112B10  4F22  sts.l PR,@-r15
  0C112B12  E300  mov ##0x00,r3
  0C112B14  6633  mov r3,r6
  0C112B16  6733  mov r3,r7
  0C112B18  2F36  mov.l r3,@-r15
  0C112B1A  2F36  mov.l r3,@-r15
  0C112B1C  BA28  bsr 0C111F70
  0C112B1E  E501  mov ##0x01,r5
  0C112B20  7F08  add ##8,r15
  0C112B22  4F26  lds.l @r15+,PR
  0C112B24  000B  rts
  0C112B26  0009  nop
  ...       (não compilado nesta sessão)
  0C112BF4  D35E  mov.l @([0C112D70]),r3
  0C112BF6  4F22  sts.l PR,@-r15
  0C112BF8  6032  mov.l @r3,r0
  0C112BFA  401B  tas.b @r0
  0C112BFC  8907  bt 0C112C0E
  ...       (não compilado nesta sessão)
  0C112C0E  B007  bsr 0C112C20
  0C112C10  0009  nop
  0C112C12  D357  mov.l @([0C112D70]),r3
  0C112C14  E100  mov ##0x00,r1
  0C112C16  6232  mov.l @r3,r2
  0C112C18  2210  mov.b r1,@r2
  0C112C1A  4F26  lds.l @r15+,PR
  0C112C1C  000B  rts
  0C112C1E  0009  nop
  0C112C20  2FE6  mov.l r14,@-r15
  0C112C22  4F22  sts.l PR,@-r15
  0C112C24  7FF8  add ##-8,r15
  0C112C26  4628  shll16 r6
  0C112C28  6EF3  mov r15,r14
  0C112C2A  4618  shll8 r6
  0C112C2C  2E52  mov.l r5,@r14
  0C112C2E  E300  mov ##0x00,r3
  0C112C30  1E61  mov.l r6,@(4,r14)
  0C112C32  2F36  mov.l r3,@-r15
  0C112C34  E702  mov ##0x02,r7
  0C112C36  66E3  mov r14,r6
  0C112C38  2F36  mov.l r3,@-r15
  0C112C3A  B999  bsr 0C111F70
  0C112C3C  E50A  mov ##0x0A,r5
  0C112C3E  7F10  add ##16,r15
  0C112C40  4F26  lds.l @r15+,PR
  0C112C42  000B  rts
  0C112C44  6EF6  mov.l @r15+,r14
  0C112C46  D34A  mov.l @([0C112D70]),r3
  0C112C48  4F22  sts.l PR,@-r15
  0C112C4A  6032  mov.l @r3,r0
  0C112C4C  401B  tas.b @r0
  0C112C4E  8907  bt 0C112C60
  ...       (não compilado nesta sessão)
  0C112C60  53F1  mov.l @(4,r15),r3
  0C112C62  2F36  mov.l r3,@-r15
  0C112C64  B008  bsr 0C112C78
  0C112C66  0009  nop
  0C112C68  7F04  add ##4,r15
  0C112C6A  D341  mov.l @([0C112D70]),r3
  0C112C6C  E100  mov ##0x00,r1
  0C112C6E  6232  mov.l @r3,r2
  0C112C70  2210  mov.b r1,@r2
  0C112C72  4F26  lds.l @r15+,PR
  0C112C74  000B  rts
  0C112C76  0009  nop
  0C112C78  2FE6  mov.l r14,@-r15
  0C112C7A  4F22  sts.l PR,@-r15
  0C112C7C  7FF8  add ##-8,r15
  0C112C7E  4628  shll16 r6
  0C112C80  6EF3  mov r15,r14
  0C112C82  4728  shll16 r7
  0C112C84  2E52  mov.l r5,@r14
  0C112C86  53F4  mov.l @(16,r15),r3
  0C112C88  4618  shll8 r6
  0C112C8A  267B  or r7,r6
  0C112C8C  263B  or r3,r6
  0C112C8E  E300  mov ##0x00,r3
  0C112C90  1E61  mov.l r6,@(4,r14)
  0C112C92  2F36  mov.l r3,@-r15
  0C112C94  E702  mov ##0x02,r7
  0C112C96  66E3  mov r14,r6
  0C112C98  2F36  mov.l r3,@-r15
  0C112C9A  B969  bsr 0C111F70
  0C112C9C  E50B  mov ##0x0B,r5
  0C112C9E  7F10  add ##16,r15
  0C112CA0  4F26  lds.l @r15+,PR
  0C112CA2  000B  rts
  0C112CA4  6EF6  mov.l @r15+,r14
  0C112CA6  D332  mov.l @([0C112D70]),r3
  0C112CA8  4F22  sts.l PR,@-r15
  0C112CAA  6032  mov.l @r3,r0
  0C112CAC  401B  tas.b @r0
  0C112CAE  8907  bt 0C112CC0
  ...       (não compilado nesta sessão)
  0C112CC0  53F3  mov.l @(12,r15),r3
  0C112CC2  2F36  mov.l r3,@-r15
  0C112CC4  52F3  mov.l @(12,r15),r2
  0C112CC6  2F26  mov.l r2,@-r15
  0C112CC8  53F3  mov.l @(12,r15),r3
  0C112CCA  2F36  mov.l r3,@-r15
  0C112CCC  B008  bsr 0C112CE0
  0C112CCE  0009  nop
  0C112CD0  7F0C  add ##12,r15
  0C112CD2  D327  mov.l @([0C112D70]),r3
  0C112CD4  E100  mov ##0x00,r1
  0C112CD6  6232  mov.l @r3,r2
  0C112CD8  2210  mov.b r1,@r2
  0C112CDA  4F26  lds.l @r15+,PR
  0C112CDC  000B  rts
  0C112CDE  0009  nop
  0C112CE0  2FE6  mov.l r14,@-r15
  0C112CE2  4F22  sts.l PR,@-r15
  0C112CE4  7FF8  add ##-8,r15
  0C112CE6  4628  shll16 r6
  0C112CE8  6EF3  mov r15,r14
  0C112CEA  4618  shll8 r6
  0C112CEC  2E52  mov.l r5,@r14
  0C112CEE  4728  shll16 r7
  0C112CF0  53F4  mov.l @(16,r15),r3
  0C112CF2  267B  or r7,r6
  0C112CF4  263B  or r3,r6
  0C112CF6  1E61  mov.l r6,@(4,r14)
  0C112CF8  E702  mov ##0x02,r7
  0C112CFA  53F6  mov.l @(24,r15),r3
  0C112CFC  66E3  mov r14,r6
  0C112CFE  2F36  mov.l r3,@-r15
  0C112D00  52F6  mov.l @(24,r15),r2
  0C112D02  2F26  mov.l r2,@-r15
  0C112D04  B934  bsr 0C111F70
  0C112D06  E50C  mov ##0x0C,r5
  0C112D08  7F10  add ##16,r15
  0C112D0A  4F26  lds.l @r15+,PR
  0C112D0C  000B  rts
  0C112D0E  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  0C112E60  D30B  mov.l @([0C112E90]),r3
  0C112E62  4F22  sts.l PR,@-r15
  0C112E64  6032  mov.l @r3,r0
  0C112E66  401B  tas.b @r0
  0C112E68  8907  bt 0C112E7A
  ...       (não compilado nesta sessão)
  0C112E7A  B00F  bsr 0C112E9C
  0C112E7C  0009  nop
  0C112E7E  D304  mov.l @([0C112E90]),r3
  0C112E80  E100  mov ##0x00,r1
  0C112E82  6232  mov.l @r3,r2
  0C112E84  2210  mov.b r1,@r2
  0C112E86  4F26  lds.l @r15+,PR
  0C112E88  000B  rts
  0C112E8A  0009  nop
  ...       (não compilado nesta sessão)
  0C112E9C  4F22  sts.l PR,@-r15
  0C112E9E  7FF4  add ##-12,r15
  0C112EA0  1F61  mov.l r6,@(4,r15)
  0C112EA2  1F72  mov.l r7,@(8,r15)
  0C112EA4  E701  mov ##0x01,r7
  0C112EA6  2F52  mov.l r5,@r15
  0C112EA8  53F2  mov.l @(8,r15),r3
  0C112EAA  2F36  mov.l r3,@-r15
  0C112EAC  52F2  mov.l @(8,r15),r2
  0C112EAE  2F26  mov.l r2,@-r15
  0C112EB0  66F3  mov r15,r6
  0C112EB2  7608  add ##8,r6
  0C112EB4  B85C  bsr 0C111F70
  0C112EB6  E50E  mov ##0x0E,r5
  0C112EB8  7F14  add ##20,r15
  0C112EBA  4F26  lds.l @r15+,PR
  0C112EBC  000B  rts
  0C112EBE  0009  nop
  ...       (não compilado nesta sessão)
```


## Project Justice (USA)

Dump: `/mnt/1TB/dcbat/20261007-160314_Project_Justice__USA__/jit-78773.txt`

```
; 0C2E21F0-0C2E25BE
  ...       (não compilado nesta sessão)
  0C2E220C  4F22  sts.l PR,@-r15
  0C2E220E  E300  mov ##0x00,r3
  0C2E2210  6633  mov r3,r6
  0C2E2212  6733  mov r3,r7
  0C2E2214  2F36  mov.l r3,@-r15
  0C2E2216  2F36  mov.l r3,@-r15
  0C2E2218  BA28  bsr 0C2E166C
  0C2E221A  E501  mov ##0x01,r5
  0C2E221C  7F08  add ##8,r15
  0C2E221E  4F26  lds.l @r15+,PR
  0C2E2220  000B  rts
  0C2E2222  0009  nop
  ...       (não compilado nesta sessão)
  0C2E22F0  D35E  mov.l @([0C2E246C]),r3
  0C2E22F2  4F22  sts.l PR,@-r15
  0C2E22F4  6032  mov.l @r3,r0
  0C2E22F6  401B  tas.b @r0
  0C2E22F8  8907  bt 0C2E230A
  ...       (não compilado nesta sessão)
  0C2E230A  B007  bsr 0C2E231C
  0C2E230C  0009  nop
  0C2E230E  D357  mov.l @([0C2E246C]),r3
  0C2E2310  E100  mov ##0x00,r1
  0C2E2312  6232  mov.l @r3,r2
  0C2E2314  2210  mov.b r1,@r2
  0C2E2316  4F26  lds.l @r15+,PR
  0C2E2318  000B  rts
  0C2E231A  0009  nop
  0C2E231C  2FE6  mov.l r14,@-r15
  0C2E231E  4F22  sts.l PR,@-r15
  0C2E2320  7FF8  add ##-8,r15
  0C2E2322  4628  shll16 r6
  0C2E2324  6EF3  mov r15,r14
  0C2E2326  4618  shll8 r6
  0C2E2328  2E52  mov.l r5,@r14
  0C2E232A  E300  mov ##0x00,r3
  0C2E232C  1E61  mov.l r6,@(4,r14)
  0C2E232E  2F36  mov.l r3,@-r15
  0C2E2330  E702  mov ##0x02,r7
  0C2E2332  66E3  mov r14,r6
  0C2E2334  2F36  mov.l r3,@-r15
  0C2E2336  B999  bsr 0C2E166C
  0C2E2338  E50A  mov ##0x0A,r5
  0C2E233A  7F10  add ##16,r15
  0C2E233C  4F26  lds.l @r15+,PR
  0C2E233E  000B  rts
  0C2E2340  6EF6  mov.l @r15+,r14
  0C2E2342  D34A  mov.l @([0C2E246C]),r3
  0C2E2344  4F22  sts.l PR,@-r15
  0C2E2346  6032  mov.l @r3,r0
  0C2E2348  401B  tas.b @r0
  0C2E234A  8907  bt 0C2E235C
  ...       (não compilado nesta sessão)
  0C2E235C  53F1  mov.l @(4,r15),r3
  0C2E235E  2F36  mov.l r3,@-r15
  0C2E2360  B008  bsr 0C2E2374
  0C2E2362  0009  nop
  0C2E2364  7F04  add ##4,r15
  0C2E2366  D341  mov.l @([0C2E246C]),r3
  0C2E2368  E100  mov ##0x00,r1
  0C2E236A  6232  mov.l @r3,r2
  0C2E236C  2210  mov.b r1,@r2
  0C2E236E  4F26  lds.l @r15+,PR
  0C2E2370  000B  rts
  0C2E2372  0009  nop
  0C2E2374  2FE6  mov.l r14,@-r15
  0C2E2376  4F22  sts.l PR,@-r15
  0C2E2378  7FF8  add ##-8,r15
  0C2E237A  4628  shll16 r6
  0C2E237C  6EF3  mov r15,r14
  0C2E237E  4728  shll16 r7
  0C2E2380  2E52  mov.l r5,@r14
  0C2E2382  53F4  mov.l @(16,r15),r3
  0C2E2384  4618  shll8 r6
  0C2E2386  267B  or r7,r6
  0C2E2388  263B  or r3,r6
  0C2E238A  E300  mov ##0x00,r3
  0C2E238C  1E61  mov.l r6,@(4,r14)
  0C2E238E  2F36  mov.l r3,@-r15
  0C2E2390  E702  mov ##0x02,r7
  0C2E2392  66E3  mov r14,r6
  0C2E2394  2F36  mov.l r3,@-r15
  0C2E2396  B969  bsr 0C2E166C
  0C2E2398  E50B  mov ##0x0B,r5
  0C2E239A  7F10  add ##16,r15
  0C2E239C  4F26  lds.l @r15+,PR
  0C2E239E  000B  rts
  0C2E23A0  6EF6  mov.l @r15+,r14
  0C2E23A2  D332  mov.l @([0C2E246C]),r3
  0C2E23A4  4F22  sts.l PR,@-r15
  0C2E23A6  6032  mov.l @r3,r0
  0C2E23A8  401B  tas.b @r0
  0C2E23AA  8907  bt 0C2E23BC
  ...       (não compilado nesta sessão)
  0C2E23BC  53F3  mov.l @(12,r15),r3
  0C2E23BE  2F36  mov.l r3,@-r15
  0C2E23C0  52F3  mov.l @(12,r15),r2
  0C2E23C2  2F26  mov.l r2,@-r15
  0C2E23C4  53F3  mov.l @(12,r15),r3
  0C2E23C6  2F36  mov.l r3,@-r15
  0C2E23C8  B008  bsr 0C2E23DC
  0C2E23CA  0009  nop
  0C2E23CC  7F0C  add ##12,r15
  0C2E23CE  D327  mov.l @([0C2E246C]),r3
  0C2E23D0  E100  mov ##0x00,r1
  0C2E23D2  6232  mov.l @r3,r2
  0C2E23D4  2210  mov.b r1,@r2
  0C2E23D6  4F26  lds.l @r15+,PR
  0C2E23D8  000B  rts
  0C2E23DA  0009  nop
  0C2E23DC  2FE6  mov.l r14,@-r15
  0C2E23DE  4F22  sts.l PR,@-r15
  0C2E23E0  7FF8  add ##-8,r15
  0C2E23E2  4628  shll16 r6
  0C2E23E4  6EF3  mov r15,r14
  0C2E23E6  4618  shll8 r6
  0C2E23E8  2E52  mov.l r5,@r14
  0C2E23EA  4728  shll16 r7
  0C2E23EC  53F4  mov.l @(16,r15),r3
  0C2E23EE  267B  or r7,r6
  0C2E23F0  263B  or r3,r6
  0C2E23F2  1E61  mov.l r6,@(4,r14)
  0C2E23F4  E702  mov ##0x02,r7
  0C2E23F6  53F6  mov.l @(24,r15),r3
  0C2E23F8  66E3  mov r14,r6
  0C2E23FA  2F36  mov.l r3,@-r15
  0C2E23FC  52F6  mov.l @(24,r15),r2
  0C2E23FE  2F26  mov.l r2,@-r15
  0C2E2400  B934  bsr 0C2E166C
  0C2E2402  E50C  mov ##0x0C,r5
  0C2E2404  7F10  add ##16,r15
  0C2E2406  4F26  lds.l @r15+,PR
  0C2E2408  000B  rts
  0C2E240A  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  0C2E255C  D30B  mov.l @([0C2E258C]),r3
  0C2E255E  4F22  sts.l PR,@-r15
  0C2E2560  6032  mov.l @r3,r0
  0C2E2562  401B  tas.b @r0
  0C2E2564  8907  bt 0C2E2576
  ...       (não compilado nesta sessão)
  0C2E2576  B00F  bsr 0C2E2598
  0C2E2578  0009  nop
  0C2E257A  D304  mov.l @([0C2E258C]),r3
  0C2E257C  E100  mov ##0x00,r1
  0C2E257E  6232  mov.l @r3,r2
  0C2E2580  2210  mov.b r1,@r2
  0C2E2582  4F26  lds.l @r15+,PR
  0C2E2584  000B  rts
  0C2E2586  0009  nop
  ...       (não compilado nesta sessão)
  0C2E2598  4F22  sts.l PR,@-r15
  0C2E259A  7FF4  add ##-12,r15
  0C2E259C  1F61  mov.l r6,@(4,r15)
  0C2E259E  1F72  mov.l r7,@(8,r15)
  0C2E25A0  E701  mov ##0x01,r7
  0C2E25A2  2F52  mov.l r5,@r15
  0C2E25A4  53F2  mov.l @(8,r15),r3
  0C2E25A6  2F36  mov.l r3,@-r15
  0C2E25A8  52F2  mov.l @(8,r15),r2
  0C2E25AA  2F26  mov.l r2,@-r15
  0C2E25AC  66F3  mov r15,r6
  0C2E25AE  7608  add ##8,r6
  0C2E25B0  B85C  bsr 0C2E166C
  0C2E25B2  E50E  mov ##0x0E,r5
  0C2E25B4  7F14  add ##20,r15
  0C2E25B6  4F26  lds.l @r15+,PR
  0C2E25B8  000B  rts
  0C2E25BA  0009  nop
  ...       (não compilado nesta sessão)
```


## Resident Evil - Code - Veronica (USA) (Disc 1)

Dump: `/mnt/1TB/dcbat/20261007-160515_Resident_Evil_-_Code_-_Veronica__USA___D/jit-82021.txt`

```
; 8C1D68A0-8C1D6C6E
  ...       (não compilado nesta sessão)
  8C1D68BC  4F22  sts.l PR,@-r15
  8C1D68BE  E300  mov ##0x00,r3
  8C1D68C0  6633  mov r3,r6
  8C1D68C2  6733  mov r3,r7
  8C1D68C4  2F36  mov.l r3,@-r15
  8C1D68C6  2F36  mov.l r3,@-r15
  8C1D68C8  BA28  bsr 8C1D5D1C
  8C1D68CA  E501  mov ##0x01,r5
  8C1D68CC  7F08  add ##8,r15
  8C1D68CE  4F26  lds.l @r15+,PR
  8C1D68D0  000B  rts
  8C1D68D2  0009  nop
  ...       (não compilado nesta sessão)
  8C1D69A0  D35E  mov.l @([8C1D6B1C]),r3
  8C1D69A2  4F22  sts.l PR,@-r15
  8C1D69A4  6032  mov.l @r3,r0
  8C1D69A6  401B  tas.b @r0
  8C1D69A8  8907  bt 8C1D69BA
  ...       (não compilado nesta sessão)
  8C1D69BA  B007  bsr 8C1D69CC
  8C1D69BC  0009  nop
  8C1D69BE  D357  mov.l @([8C1D6B1C]),r3
  8C1D69C0  E100  mov ##0x00,r1
  8C1D69C2  6232  mov.l @r3,r2
  8C1D69C4  2210  mov.b r1,@r2
  8C1D69C6  4F26  lds.l @r15+,PR
  8C1D69C8  000B  rts
  8C1D69CA  0009  nop
  8C1D69CC  2FE6  mov.l r14,@-r15
  8C1D69CE  4F22  sts.l PR,@-r15
  8C1D69D0  7FF8  add ##-8,r15
  8C1D69D2  4628  shll16 r6
  8C1D69D4  6EF3  mov r15,r14
  8C1D69D6  4618  shll8 r6
  8C1D69D8  2E52  mov.l r5,@r14
  8C1D69DA  E300  mov ##0x00,r3
  8C1D69DC  1E61  mov.l r6,@(4,r14)
  8C1D69DE  2F36  mov.l r3,@-r15
  8C1D69E0  E702  mov ##0x02,r7
  8C1D69E2  66E3  mov r14,r6
  8C1D69E4  2F36  mov.l r3,@-r15
  8C1D69E6  B999  bsr 8C1D5D1C
  8C1D69E8  E50A  mov ##0x0A,r5
  8C1D69EA  7F10  add ##16,r15
  8C1D69EC  4F26  lds.l @r15+,PR
  8C1D69EE  000B  rts
  8C1D69F0  6EF6  mov.l @r15+,r14
  8C1D69F2  D34A  mov.l @([8C1D6B1C]),r3
  8C1D69F4  4F22  sts.l PR,@-r15
  8C1D69F6  6032  mov.l @r3,r0
  8C1D69F8  401B  tas.b @r0
  8C1D69FA  8907  bt 8C1D6A0C
  ...       (não compilado nesta sessão)
  8C1D6A0C  53F1  mov.l @(4,r15),r3
  8C1D6A0E  2F36  mov.l r3,@-r15
  8C1D6A10  B008  bsr 8C1D6A24
  8C1D6A12  0009  nop
  8C1D6A14  7F04  add ##4,r15
  8C1D6A16  D341  mov.l @([8C1D6B1C]),r3
  8C1D6A18  E100  mov ##0x00,r1
  8C1D6A1A  6232  mov.l @r3,r2
  8C1D6A1C  2210  mov.b r1,@r2
  8C1D6A1E  4F26  lds.l @r15+,PR
  8C1D6A20  000B  rts
  8C1D6A22  0009  nop
  8C1D6A24  2FE6  mov.l r14,@-r15
  8C1D6A26  4F22  sts.l PR,@-r15
  8C1D6A28  7FF8  add ##-8,r15
  8C1D6A2A  4628  shll16 r6
  8C1D6A2C  6EF3  mov r15,r14
  8C1D6A2E  4728  shll16 r7
  8C1D6A30  2E52  mov.l r5,@r14
  8C1D6A32  53F4  mov.l @(16,r15),r3
  8C1D6A34  4618  shll8 r6
  8C1D6A36  267B  or r7,r6
  8C1D6A38  263B  or r3,r6
  8C1D6A3A  E300  mov ##0x00,r3
  8C1D6A3C  1E61  mov.l r6,@(4,r14)
  8C1D6A3E  2F36  mov.l r3,@-r15
  8C1D6A40  E702  mov ##0x02,r7
  8C1D6A42  66E3  mov r14,r6
  8C1D6A44  2F36  mov.l r3,@-r15
  8C1D6A46  B969  bsr 8C1D5D1C
  8C1D6A48  E50B  mov ##0x0B,r5
  8C1D6A4A  7F10  add ##16,r15
  8C1D6A4C  4F26  lds.l @r15+,PR
  8C1D6A4E  000B  rts
  8C1D6A50  6EF6  mov.l @r15+,r14
  8C1D6A52  D332  mov.l @([8C1D6B1C]),r3
  8C1D6A54  4F22  sts.l PR,@-r15
  8C1D6A56  6032  mov.l @r3,r0
  8C1D6A58  401B  tas.b @r0
  8C1D6A5A  8907  bt 8C1D6A6C
  ...       (não compilado nesta sessão)
  8C1D6A6C  53F3  mov.l @(12,r15),r3
  8C1D6A6E  2F36  mov.l r3,@-r15
  8C1D6A70  52F3  mov.l @(12,r15),r2
  8C1D6A72  2F26  mov.l r2,@-r15
  8C1D6A74  53F3  mov.l @(12,r15),r3
  8C1D6A76  2F36  mov.l r3,@-r15
  8C1D6A78  B008  bsr 8C1D6A8C
  8C1D6A7A  0009  nop
  8C1D6A7C  7F0C  add ##12,r15
  8C1D6A7E  D327  mov.l @([8C1D6B1C]),r3
  8C1D6A80  E100  mov ##0x00,r1
  8C1D6A82  6232  mov.l @r3,r2
  8C1D6A84  2210  mov.b r1,@r2
  8C1D6A86  4F26  lds.l @r15+,PR
  8C1D6A88  000B  rts
  8C1D6A8A  0009  nop
  8C1D6A8C  2FE6  mov.l r14,@-r15
  8C1D6A8E  4F22  sts.l PR,@-r15
  8C1D6A90  7FF8  add ##-8,r15
  8C1D6A92  4628  shll16 r6
  8C1D6A94  6EF3  mov r15,r14
  8C1D6A96  4618  shll8 r6
  8C1D6A98  2E52  mov.l r5,@r14
  8C1D6A9A  4728  shll16 r7
  8C1D6A9C  53F4  mov.l @(16,r15),r3
  8C1D6A9E  267B  or r7,r6
  8C1D6AA0  263B  or r3,r6
  8C1D6AA2  1E61  mov.l r6,@(4,r14)
  8C1D6AA4  E702  mov ##0x02,r7
  8C1D6AA6  53F6  mov.l @(24,r15),r3
  8C1D6AA8  66E3  mov r14,r6
  8C1D6AAA  2F36  mov.l r3,@-r15
  8C1D6AAC  52F6  mov.l @(24,r15),r2
  8C1D6AAE  2F26  mov.l r2,@-r15
  8C1D6AB0  B934  bsr 8C1D5D1C
  8C1D6AB2  E50C  mov ##0x0C,r5
  8C1D6AB4  7F10  add ##16,r15
  8C1D6AB6  4F26  lds.l @r15+,PR
  8C1D6AB8  000B  rts
  8C1D6ABA  6EF6  mov.l @r15+,r14
  8C1D6ABC  D317  mov.l @([8C1D6B1C]),r3
  8C1D6ABE  4F22  sts.l PR,@-r15
  8C1D6AC0  6032  mov.l @r3,r0
  8C1D6AC2  401B  tas.b @r0
  8C1D6AC4  8907  bt 8C1D6AD6
  ...       (não compilado nesta sessão)
  8C1D6AD6  53F1  mov.l @(4,r15),r3
  8C1D6AD8  2F36  mov.l r3,@-r15
  8C1D6ADA  B008  bsr 8C1D6AEE
  8C1D6ADC  0009  nop
  8C1D6ADE  7F04  add ##4,r15
  8C1D6AE0  D30E  mov.l @([8C1D6B1C]),r3
  8C1D6AE2  E100  mov ##0x00,r1
  8C1D6AE4  6232  mov.l @r3,r2
  8C1D6AE6  2210  mov.b r1,@r2
  8C1D6AE8  4F26  lds.l @r15+,PR
  8C1D6AEA  000B  rts
  8C1D6AEC  0009  nop
  8C1D6AEE  2FE6  mov.l r14,@-r15
  8C1D6AF0  4F22  sts.l PR,@-r15
  8C1D6AF2  7FF8  add ##-8,r15
  8C1D6AF4  4628  shll16 r6
  8C1D6AF6  6EF3  mov r15,r14
  8C1D6AF8  4728  shll16 r7
  8C1D6AFA  2E52  mov.l r5,@r14
  8C1D6AFC  53F4  mov.l @(16,r15),r3
  8C1D6AFE  4618  shll8 r6
  8C1D6B00  267B  or r7,r6
  8C1D6B02  263B  or r3,r6
  8C1D6B04  E300  mov ##0x00,r3
  8C1D6B06  1E61  mov.l r6,@(4,r14)
  8C1D6B08  2F36  mov.l r3,@-r15
  8C1D6B0A  E702  mov ##0x02,r7
  8C1D6B0C  66E3  mov r14,r6
  8C1D6B0E  2F36  mov.l r3,@-r15
  8C1D6B10  B904  bsr 8C1D5D1C
  8C1D6B12  E50D  mov ##0x0D,r5
  8C1D6B14  7F10  add ##16,r15
  8C1D6B16  4F26  lds.l @r15+,PR
  8C1D6B18  000B  rts
  8C1D6B1A  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C1D6C0C  D30B  mov.l @([8C1D6C3C]),r3
  8C1D6C0E  4F22  sts.l PR,@-r15
  8C1D6C10  6032  mov.l @r3,r0
  8C1D6C12  401B  tas.b @r0
  8C1D6C14  8907  bt 8C1D6C26
  ...       (não compilado nesta sessão)
  8C1D6C26  B00F  bsr 8C1D6C48
  8C1D6C28  0009  nop
  8C1D6C2A  D304  mov.l @([8C1D6C3C]),r3
  8C1D6C2C  E100  mov ##0x00,r1
  8C1D6C2E  6232  mov.l @r3,r2
  8C1D6C30  2210  mov.b r1,@r2
  8C1D6C32  4F26  lds.l @r15+,PR
  8C1D6C34  000B  rts
  8C1D6C36  0009  nop
  ...       (não compilado nesta sessão)
  8C1D6C48  4F22  sts.l PR,@-r15
  8C1D6C4A  7FF4  add ##-12,r15
  8C1D6C4C  1F61  mov.l r6,@(4,r15)
  8C1D6C4E  1F72  mov.l r7,@(8,r15)
  8C1D6C50  E701  mov ##0x01,r7
  8C1D6C52  2F52  mov.l r5,@r15
  8C1D6C54  53F2  mov.l @(8,r15),r3
  8C1D6C56  2F36  mov.l r3,@-r15
  8C1D6C58  52F2  mov.l @(8,r15),r2
  8C1D6C5A  2F26  mov.l r2,@-r15
  8C1D6C5C  66F3  mov r15,r6
  8C1D6C5E  7608  add ##8,r6
  8C1D6C60  B85C  bsr 8C1D5D1C
  8C1D6C62  E50E  mov ##0x0E,r5
  8C1D6C64  7F14  add ##20,r15
  8C1D6C66  4F26  lds.l @r15+,PR
  8C1D6C68  000B  rts
  8C1D6C6A  0009  nop
  ...       (não compilado nesta sessão)
```


## Shenmue (USA) (Disc 1)

Dump: `/mnt/1TB/dcbat_off/20261007-191903_Shenmue__USA___Disc_1__/jit-11043.txt`

```
; 0C1DFC90-0C1E005E
  ...       (não compilado nesta sessão)
  0C1DFCAC  4F22  sts.l PR,@-r15
  0C1DFCAE  E300  mov ##0x00,r3
  0C1DFCB0  6633  mov r3,r6
  0C1DFCB2  6733  mov r3,r7
  0C1DFCB4  2F36  mov.l r3,@-r15
  0C1DFCB6  2F36  mov.l r3,@-r15
  0C1DFCB8  BA18  bsr 0C1DF0EC
  0C1DFCBA  E501  mov ##0x01,r5
  0C1DFCBC  7F08  add ##8,r15
  0C1DFCBE  4F26  lds.l @r15+,PR
  0C1DFCC0  000B  rts
  0C1DFCC2  0009  nop
  ...       (não compilado nesta sessão)
  0C1DFD90  D35E  mov.l @([0C1DFF0C]),r3
  0C1DFD92  4F22  sts.l PR,@-r15
  0C1DFD94  6032  mov.l @r3,r0
  0C1DFD96  401B  tas.b @r0
  0C1DFD98  8907  bt 0C1DFDAA
  ...       (não compilado nesta sessão)
  0C1DFDAA  B007  bsr 0C1DFDBC
  0C1DFDAC  0009  nop
  0C1DFDAE  D357  mov.l @([0C1DFF0C]),r3
  0C1DFDB0  E100  mov ##0x00,r1
  0C1DFDB2  6232  mov.l @r3,r2
  0C1DFDB4  2210  mov.b r1,@r2
  0C1DFDB6  4F26  lds.l @r15+,PR
  0C1DFDB8  000B  rts
  0C1DFDBA  0009  nop
  0C1DFDBC  2FE6  mov.l r14,@-r15
  0C1DFDBE  4628  shll16 r6
  0C1DFDC0  4F22  sts.l PR,@-r15
  0C1DFDC2  4618  shll8 r6
  0C1DFDC4  E300  mov ##0x00,r3
  0C1DFDC6  7FF8  add ##-8,r15
  0C1DFDC8  6EF3  mov r15,r14
  0C1DFDCA  E702  mov ##0x02,r7
  0C1DFDCC  2E52  mov.l r5,@r14
  0C1DFDCE  1E61  mov.l r6,@(4,r14)
  0C1DFDD0  66E3  mov r14,r6
  0C1DFDD2  2F36  mov.l r3,@-r15
  0C1DFDD4  2F36  mov.l r3,@-r15
  0C1DFDD6  B989  bsr 0C1DF0EC
  0C1DFDD8  E50A  mov ##0x0A,r5
  0C1DFDDA  7F10  add ##16,r15
  0C1DFDDC  4F26  lds.l @r15+,PR
  0C1DFDDE  000B  rts
  0C1DFDE0  6EF6  mov.l @r15+,r14
  0C1DFDE2  D34A  mov.l @([0C1DFF0C]),r3
  0C1DFDE4  4F22  sts.l PR,@-r15
  0C1DFDE6  6032  mov.l @r3,r0
  0C1DFDE8  401B  tas.b @r0
  0C1DFDEA  8907  bt 0C1DFDFC
  ...       (não compilado nesta sessão)
  0C1DFDFC  53F1  mov.l @(4,r15),r3
  0C1DFDFE  2F36  mov.l r3,@-r15
  0C1DFE00  B008  bsr 0C1DFE14
  0C1DFE02  0009  nop
  0C1DFE04  D341  mov.l @([0C1DFF0C]),r3
  0C1DFE06  E100  mov ##0x00,r1
  0C1DFE08  7F04  add ##4,r15
  0C1DFE0A  6232  mov.l @r3,r2
  0C1DFE0C  2210  mov.b r1,@r2
  0C1DFE0E  4F26  lds.l @r15+,PR
  0C1DFE10  000B  rts
  0C1DFE12  0009  nop
  0C1DFE14  2FE6  mov.l r14,@-r15
  0C1DFE16  4628  shll16 r6
  0C1DFE18  4F22  sts.l PR,@-r15
  0C1DFE1A  4618  shll8 r6
  0C1DFE1C  4728  shll16 r7
  0C1DFE1E  7FF8  add ##-8,r15
  0C1DFE20  267B  or r7,r6
  0C1DFE22  6EF3  mov r15,r14
  0C1DFE24  E702  mov ##0x02,r7
  0C1DFE26  2E52  mov.l r5,@r14
  0C1DFE28  53F4  mov.l @(16,r15),r3
  0C1DFE2A  263B  or r3,r6
  0C1DFE2C  E300  mov ##0x00,r3
  0C1DFE2E  1E61  mov.l r6,@(4,r14)
  0C1DFE30  2F36  mov.l r3,@-r15
  0C1DFE32  66E3  mov r14,r6
  0C1DFE34  2F36  mov.l r3,@-r15
  0C1DFE36  B959  bsr 0C1DF0EC
  0C1DFE38  E50B  mov ##0x0B,r5
  0C1DFE3A  7F10  add ##16,r15
  0C1DFE3C  4F26  lds.l @r15+,PR
  0C1DFE3E  000B  rts
  0C1DFE40  6EF6  mov.l @r15+,r14
  0C1DFE42  D332  mov.l @([0C1DFF0C]),r3
  0C1DFE44  4F22  sts.l PR,@-r15
  0C1DFE46  6032  mov.l @r3,r0
  0C1DFE48  401B  tas.b @r0
  0C1DFE4A  8907  bt 0C1DFE5C
  ...       (não compilado nesta sessão)
  0C1DFE5C  53F3  mov.l @(12,r15),r3
  0C1DFE5E  2F36  mov.l r3,@-r15
  0C1DFE60  52F3  mov.l @(12,r15),r2
  0C1DFE62  2F26  mov.l r2,@-r15
  0C1DFE64  53F3  mov.l @(12,r15),r3
  0C1DFE66  2F36  mov.l r3,@-r15
  0C1DFE68  B008  bsr 0C1DFE7C
  0C1DFE6A  0009  nop
  0C1DFE6C  D327  mov.l @([0C1DFF0C]),r3
  0C1DFE6E  E100  mov ##0x00,r1
  0C1DFE70  7F0C  add ##12,r15
  0C1DFE72  6232  mov.l @r3,r2
  0C1DFE74  2210  mov.b r1,@r2
  0C1DFE76  4F26  lds.l @r15+,PR
  0C1DFE78  000B  rts
  0C1DFE7A  0009  nop
  0C1DFE7C  2FE6  mov.l r14,@-r15
  0C1DFE7E  4628  shll16 r6
  0C1DFE80  4F22  sts.l PR,@-r15
  0C1DFE82  4618  shll8 r6
  0C1DFE84  4728  shll16 r7
  0C1DFE86  7FF8  add ##-8,r15
  0C1DFE88  267B  or r7,r6
  0C1DFE8A  6EF3  mov r15,r14
  0C1DFE8C  E702  mov ##0x02,r7
  0C1DFE8E  2E52  mov.l r5,@r14
  0C1DFE90  53F4  mov.l @(16,r15),r3
  0C1DFE92  263B  or r3,r6
  0C1DFE94  1E61  mov.l r6,@(4,r14)
  0C1DFE96  66E3  mov r14,r6
  0C1DFE98  53F6  mov.l @(24,r15),r3
  0C1DFE9A  2F36  mov.l r3,@-r15
  0C1DFE9C  52F6  mov.l @(24,r15),r2
  0C1DFE9E  2F26  mov.l r2,@-r15
  0C1DFEA0  B924  bsr 0C1DF0EC
  0C1DFEA2  E50C  mov ##0x0C,r5
  0C1DFEA4  7F10  add ##16,r15
  0C1DFEA6  4F26  lds.l @r15+,PR
  0C1DFEA8  000B  rts
  0C1DFEAA  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  0C1DFFFC  D30B  mov.l @([0C1E002C]),r3
  0C1DFFFE  4F22  sts.l PR,@-r15
  0C1E0000  6032  mov.l @r3,r0
  0C1E0002  401B  tas.b @r0
  0C1E0004  8907  bt 0C1E0016
  ...       (não compilado nesta sessão)
  0C1E0016  B00F  bsr 0C1E0038
  0C1E0018  0009  nop
  0C1E001A  D304  mov.l @([0C1E002C]),r3
  0C1E001C  E100  mov ##0x00,r1
  0C1E001E  6232  mov.l @r3,r2
  0C1E0020  2210  mov.b r1,@r2
  0C1E0022  4F26  lds.l @r15+,PR
  0C1E0024  000B  rts
  0C1E0026  0009  nop
  ...       (não compilado nesta sessão)
  0C1E0038  4F22  sts.l PR,@-r15
  0C1E003A  7FF4  add ##-12,r15
  0C1E003C  1F61  mov.l r6,@(4,r15)
  0C1E003E  1F72  mov.l r7,@(8,r15)
  0C1E0040  E701  mov ##0x01,r7
  0C1E0042  2F52  mov.l r5,@r15
  0C1E0044  53F2  mov.l @(8,r15),r3
  0C1E0046  2F36  mov.l r3,@-r15
  0C1E0048  52F2  mov.l @(8,r15),r2
  0C1E004A  2F26  mov.l r2,@-r15
  0C1E004C  66F3  mov r15,r6
  0C1E004E  7608  add ##8,r6
  0C1E0050  B84C  bsr 0C1DF0EC
  0C1E0052  E50E  mov ##0x0E,r5
  0C1E0054  7F14  add ##20,r15
  0C1E0056  4F26  lds.l @r15+,PR
  0C1E0058  000B  rts
  0C1E005A  0009  nop
  ...       (não compilado nesta sessão)
```


## Skies of Arcadia (USA) (Disc 1)

Dump: `/mnt/1TB/dcbat/20261007-162126_Skies_of_Arcadia__USA___Disc_1__/jit-98449.txt`

```
; 8C2E87E0-8C2E8BAE
  ...       (não compilado nesta sessão)
  8C2E87FC  4F22  sts.l PR,@-r15
  8C2E87FE  E300  mov ##0x00,r3
  8C2E8800  6633  mov r3,r6
  8C2E8802  6733  mov r3,r7
  8C2E8804  2F36  mov.l r3,@-r15
  8C2E8806  2F36  mov.l r3,@-r15
  8C2E8808  BA28  bsr 8C2E7C5C
  8C2E880A  E501  mov ##0x01,r5
  8C2E880C  7F08  add ##8,r15
  8C2E880E  4F26  lds.l @r15+,PR
  8C2E8810  000B  rts
  8C2E8812  0009  nop
  ...       (não compilado nesta sessão)
  8C2E88E0  D35E  mov.l @([8C2E8A5C]),r3
  8C2E88E2  4F22  sts.l PR,@-r15
  8C2E88E4  6032  mov.l @r3,r0
  8C2E88E6  401B  tas.b @r0
  8C2E88E8  8907  bt 8C2E88FA
  ...       (não compilado nesta sessão)
  8C2E88FA  B007  bsr 8C2E890C
  8C2E88FC  0009  nop
  8C2E88FE  D357  mov.l @([8C2E8A5C]),r3
  8C2E8900  E100  mov ##0x00,r1
  8C2E8902  6232  mov.l @r3,r2
  8C2E8904  2210  mov.b r1,@r2
  8C2E8906  4F26  lds.l @r15+,PR
  8C2E8908  000B  rts
  8C2E890A  0009  nop
  8C2E890C  2FE6  mov.l r14,@-r15
  8C2E890E  4F22  sts.l PR,@-r15
  8C2E8910  7FF8  add ##-8,r15
  8C2E8912  4628  shll16 r6
  8C2E8914  6EF3  mov r15,r14
  8C2E8916  4618  shll8 r6
  8C2E8918  2E52  mov.l r5,@r14
  8C2E891A  E300  mov ##0x00,r3
  8C2E891C  1E61  mov.l r6,@(4,r14)
  8C2E891E  2F36  mov.l r3,@-r15
  8C2E8920  E702  mov ##0x02,r7
  8C2E8922  66E3  mov r14,r6
  8C2E8924  2F36  mov.l r3,@-r15
  8C2E8926  B999  bsr 8C2E7C5C
  8C2E8928  E50A  mov ##0x0A,r5
  8C2E892A  7F10  add ##16,r15
  8C2E892C  4F26  lds.l @r15+,PR
  8C2E892E  000B  rts
  8C2E8930  6EF6  mov.l @r15+,r14
  8C2E8932  D34A  mov.l @([8C2E8A5C]),r3
  8C2E8934  4F22  sts.l PR,@-r15
  8C2E8936  6032  mov.l @r3,r0
  8C2E8938  401B  tas.b @r0
  8C2E893A  8907  bt 8C2E894C
  ...       (não compilado nesta sessão)
  8C2E894C  53F1  mov.l @(4,r15),r3
  8C2E894E  2F36  mov.l r3,@-r15
  8C2E8950  B008  bsr 8C2E8964
  8C2E8952  0009  nop
  8C2E8954  7F04  add ##4,r15
  8C2E8956  D341  mov.l @([8C2E8A5C]),r3
  8C2E8958  E100  mov ##0x00,r1
  8C2E895A  6232  mov.l @r3,r2
  8C2E895C  2210  mov.b r1,@r2
  8C2E895E  4F26  lds.l @r15+,PR
  8C2E8960  000B  rts
  8C2E8962  0009  nop
  8C2E8964  2FE6  mov.l r14,@-r15
  8C2E8966  4F22  sts.l PR,@-r15
  8C2E8968  7FF8  add ##-8,r15
  8C2E896A  4628  shll16 r6
  8C2E896C  6EF3  mov r15,r14
  8C2E896E  4728  shll16 r7
  8C2E8970  2E52  mov.l r5,@r14
  8C2E8972  53F4  mov.l @(16,r15),r3
  8C2E8974  4618  shll8 r6
  8C2E8976  267B  or r7,r6
  8C2E8978  263B  or r3,r6
  8C2E897A  E300  mov ##0x00,r3
  8C2E897C  1E61  mov.l r6,@(4,r14)
  8C2E897E  2F36  mov.l r3,@-r15
  8C2E8980  E702  mov ##0x02,r7
  8C2E8982  66E3  mov r14,r6
  8C2E8984  2F36  mov.l r3,@-r15
  8C2E8986  B969  bsr 8C2E7C5C
  8C2E8988  E50B  mov ##0x0B,r5
  8C2E898A  7F10  add ##16,r15
  8C2E898C  4F26  lds.l @r15+,PR
  8C2E898E  000B  rts
  8C2E8990  6EF6  mov.l @r15+,r14
  8C2E8992  D332  mov.l @([8C2E8A5C]),r3
  8C2E8994  4F22  sts.l PR,@-r15
  8C2E8996  6032  mov.l @r3,r0
  8C2E8998  401B  tas.b @r0
  8C2E899A  8907  bt 8C2E89AC
  ...       (não compilado nesta sessão)
  8C2E89AC  53F3  mov.l @(12,r15),r3
  8C2E89AE  2F36  mov.l r3,@-r15
  8C2E89B0  52F3  mov.l @(12,r15),r2
  8C2E89B2  2F26  mov.l r2,@-r15
  8C2E89B4  53F3  mov.l @(12,r15),r3
  8C2E89B6  2F36  mov.l r3,@-r15
  8C2E89B8  B008  bsr 8C2E89CC
  8C2E89BA  0009  nop
  8C2E89BC  7F0C  add ##12,r15
  8C2E89BE  D327  mov.l @([8C2E8A5C]),r3
  8C2E89C0  E100  mov ##0x00,r1
  8C2E89C2  6232  mov.l @r3,r2
  8C2E89C4  2210  mov.b r1,@r2
  8C2E89C6  4F26  lds.l @r15+,PR
  8C2E89C8  000B  rts
  8C2E89CA  0009  nop
  8C2E89CC  2FE6  mov.l r14,@-r15
  8C2E89CE  4F22  sts.l PR,@-r15
  8C2E89D0  7FF8  add ##-8,r15
  8C2E89D2  4628  shll16 r6
  8C2E89D4  6EF3  mov r15,r14
  8C2E89D6  4618  shll8 r6
  8C2E89D8  2E52  mov.l r5,@r14
  8C2E89DA  4728  shll16 r7
  8C2E89DC  53F4  mov.l @(16,r15),r3
  8C2E89DE  267B  or r7,r6
  8C2E89E0  263B  or r3,r6
  8C2E89E2  1E61  mov.l r6,@(4,r14)
  8C2E89E4  E702  mov ##0x02,r7
  8C2E89E6  53F6  mov.l @(24,r15),r3
  8C2E89E8  66E3  mov r14,r6
  8C2E89EA  2F36  mov.l r3,@-r15
  8C2E89EC  52F6  mov.l @(24,r15),r2
  8C2E89EE  2F26  mov.l r2,@-r15
  8C2E89F0  B934  bsr 8C2E7C5C
  8C2E89F2  E50C  mov ##0x0C,r5
  8C2E89F4  7F10  add ##16,r15
  8C2E89F6  4F26  lds.l @r15+,PR
  8C2E89F8  000B  rts
  8C2E89FA  6EF6  mov.l @r15+,r14
  8C2E89FC  D317  mov.l @([8C2E8A5C]),r3
  8C2E89FE  4F22  sts.l PR,@-r15
  8C2E8A00  6032  mov.l @r3,r0
  8C2E8A02  401B  tas.b @r0
  8C2E8A04  8907  bt 8C2E8A16
  ...       (não compilado nesta sessão)
  8C2E8A16  53F1  mov.l @(4,r15),r3
  8C2E8A18  2F36  mov.l r3,@-r15
  8C2E8A1A  B008  bsr 8C2E8A2E
  8C2E8A1C  0009  nop
  8C2E8A1E  7F04  add ##4,r15
  8C2E8A20  D30E  mov.l @([8C2E8A5C]),r3
  8C2E8A22  E100  mov ##0x00,r1
  8C2E8A24  6232  mov.l @r3,r2
  8C2E8A26  2210  mov.b r1,@r2
  8C2E8A28  4F26  lds.l @r15+,PR
  8C2E8A2A  000B  rts
  8C2E8A2C  0009  nop
  8C2E8A2E  2FE6  mov.l r14,@-r15
  8C2E8A30  4F22  sts.l PR,@-r15
  8C2E8A32  7FF8  add ##-8,r15
  8C2E8A34  4628  shll16 r6
  8C2E8A36  6EF3  mov r15,r14
  8C2E8A38  4728  shll16 r7
  8C2E8A3A  2E52  mov.l r5,@r14
  8C2E8A3C  53F4  mov.l @(16,r15),r3
  8C2E8A3E  4618  shll8 r6
  8C2E8A40  267B  or r7,r6
  8C2E8A42  263B  or r3,r6
  8C2E8A44  E300  mov ##0x00,r3
  8C2E8A46  1E61  mov.l r6,@(4,r14)
  8C2E8A48  2F36  mov.l r3,@-r15
  8C2E8A4A  E702  mov ##0x02,r7
  8C2E8A4C  66E3  mov r14,r6
  8C2E8A4E  2F36  mov.l r3,@-r15
  8C2E8A50  B904  bsr 8C2E7C5C
  8C2E8A52  E50D  mov ##0x0D,r5
  8C2E8A54  7F10  add ##16,r15
  8C2E8A56  4F26  lds.l @r15+,PR
  8C2E8A58  000B  rts
  8C2E8A5A  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
  8C2E8B4C  D30B  mov.l @([8C2E8B7C]),r3
  8C2E8B4E  4F22  sts.l PR,@-r15
  8C2E8B50  6032  mov.l @r3,r0
  8C2E8B52  401B  tas.b @r0
  8C2E8B54  8907  bt 8C2E8B66
  ...       (não compilado nesta sessão)
  8C2E8B66  B00F  bsr 8C2E8B88
  8C2E8B68  0009  nop
  8C2E8B6A  D304  mov.l @([8C2E8B7C]),r3
  8C2E8B6C  E100  mov ##0x00,r1
  8C2E8B6E  6232  mov.l @r3,r2
  8C2E8B70  2210  mov.b r1,@r2
  8C2E8B72  4F26  lds.l @r15+,PR
  8C2E8B74  000B  rts
  8C2E8B76  0009  nop
  ...       (não compilado nesta sessão)
  8C2E8B88  4F22  sts.l PR,@-r15
  8C2E8B8A  7FF4  add ##-12,r15
  8C2E8B8C  1F61  mov.l r6,@(4,r15)
  8C2E8B8E  1F72  mov.l r7,@(8,r15)
  8C2E8B90  E701  mov ##0x01,r7
  8C2E8B92  2F52  mov.l r5,@r15
  8C2E8B94  53F2  mov.l @(8,r15),r3
  8C2E8B96  2F36  mov.l r3,@-r15
  8C2E8B98  52F2  mov.l @(8,r15),r2
  8C2E8B9A  2F26  mov.l r2,@-r15
  8C2E8B9C  66F3  mov r15,r6
  8C2E8B9E  7608  add ##8,r6
  8C2E8BA0  B85C  bsr 8C2E7C5C
  8C2E8BA2  E50E  mov ##0x0E,r5
  8C2E8BA4  7F14  add ##20,r15
  8C2E8BA6  4F26  lds.l @r15+,PR
  8C2E8BA8  000B  rts
  8C2E8BAA  0009  nop
  ...       (não compilado nesta sessão)
```


## Sonic Adventure 2 (USA) (EnJaFrDeEs)

Dump: `/mnt/1TB/dcbat/20261007-163253_Sonic_Adventure_2__USA___EnJaFrDeEs__/jit-102386.txt`

```
; 8C15CA5C-8C15CC10
  ...       (não compilado nesta sessão)
  8C15CA78  4F22  sts.l PR,@-r15
  8C15CA7A  E300  mov ##0x00,r3
  8C15CA7C  6633  mov r3,r6
  8C15CA7E  6733  mov r3,r7
  8C15CA80  2F36  mov.l r3,@-r15
  8C15CA82  2F36  mov.l r3,@-r15
  8C15CA84  BA28  bsr 8C15BED8
  8C15CA86  E501  mov ##0x01,r5
  8C15CA88  7F08  add ##8,r15
  8C15CA8A  4F26  lds.l @r15+,PR
  8C15CA8C  000B  rts
  8C15CA8E  0009  nop
  ...       (não compilado nesta sessão)
  8C15CB5C  D35E  mov.l @([8C15CCD8]),r3
  8C15CB5E  4F22  sts.l PR,@-r15
  8C15CB60  6032  mov.l @r3,r0
  8C15CB62  401B  tas.b @r0
  8C15CB64  8907  bt 8C15CB76
  ...       (não compilado nesta sessão)
  8C15CB76  B007  bsr 8C15CB88
  8C15CB78  0009  nop
  8C15CB7A  D357  mov.l @([8C15CCD8]),r3
  8C15CB7C  E100  mov ##0x00,r1
  8C15CB7E  6232  mov.l @r3,r2
  8C15CB80  2210  mov.b r1,@r2
  8C15CB82  4F26  lds.l @r15+,PR
  8C15CB84  000B  rts
  8C15CB86  0009  nop
  8C15CB88  2FE6  mov.l r14,@-r15
  8C15CB8A  4F22  sts.l PR,@-r15
  8C15CB8C  7FF8  add ##-8,r15
  8C15CB8E  4628  shll16 r6
  8C15CB90  6EF3  mov r15,r14
  8C15CB92  4618  shll8 r6
  8C15CB94  2E52  mov.l r5,@r14
  8C15CB96  E300  mov ##0x00,r3
  8C15CB98  1E61  mov.l r6,@(4,r14)
  8C15CB9A  2F36  mov.l r3,@-r15
  8C15CB9C  E702  mov ##0x02,r7
  8C15CB9E  66E3  mov r14,r6
  8C15CBA0  2F36  mov.l r3,@-r15
  8C15CBA2  B999  bsr 8C15BED8
  8C15CBA4  E50A  mov ##0x0A,r5
  8C15CBA6  7F10  add ##16,r15
  8C15CBA8  4F26  lds.l @r15+,PR
  8C15CBAA  000B  rts
  8C15CBAC  6EF6  mov.l @r15+,r14
  8C15CBAE  D34A  mov.l @([8C15CCD8]),r3
  8C15CBB0  4F22  sts.l PR,@-r15
  8C15CBB2  6032  mov.l @r3,r0
  8C15CBB4  401B  tas.b @r0
  8C15CBB6  8907  bt 8C15CBC8
  ...       (não compilado nesta sessão)
  8C15CBC8  53F1  mov.l @(4,r15),r3
  8C15CBCA  2F36  mov.l r3,@-r15
  8C15CBCC  B008  bsr 8C15CBE0
  8C15CBCE  0009  nop
  8C15CBD0  7F04  add ##4,r15
  8C15CBD2  D341  mov.l @([8C15CCD8]),r3
  8C15CBD4  E100  mov ##0x00,r1
  8C15CBD6  6232  mov.l @r3,r2
  8C15CBD8  2210  mov.b r1,@r2
  8C15CBDA  4F26  lds.l @r15+,PR
  8C15CBDC  000B  rts
  8C15CBDE  0009  nop
  8C15CBE0  2FE6  mov.l r14,@-r15
  8C15CBE2  4F22  sts.l PR,@-r15
  8C15CBE4  7FF8  add ##-8,r15
  8C15CBE6  4628  shll16 r6
  8C15CBE8  6EF3  mov r15,r14
  8C15CBEA  4728  shll16 r7
  8C15CBEC  2E52  mov.l r5,@r14
  8C15CBEE  53F4  mov.l @(16,r15),r3
  8C15CBF0  4618  shll8 r6
  8C15CBF2  267B  or r7,r6
  8C15CBF4  263B  or r3,r6
  8C15CBF6  E300  mov ##0x00,r3
  8C15CBF8  1E61  mov.l r6,@(4,r14)
  8C15CBFA  2F36  mov.l r3,@-r15
  8C15CBFC  E702  mov ##0x02,r7
  8C15CBFE  66E3  mov r14,r6
  8C15CC00  2F36  mov.l r3,@-r15
  8C15CC02  B969  bsr 8C15BED8
  8C15CC04  E50B  mov ##0x0B,r5
  8C15CC06  7F10  add ##16,r15
  8C15CC08  4F26  lds.l @r15+,PR
  8C15CC0A  000B  rts
  8C15CC0C  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
```


## Soulcalibur (USA)

Dump: `/mnt/1TB/dcbat/20261007-163634_Soulcalibur__USA__/jit-108030.txt`

```
; 8C240F58-8C2411D6
  ...       (não compilado nesta sessão)
  8C240F74  4F22  sts.l PR,@-r15
  8C240F76  E300  mov ##0x00,r3
  8C240F78  6633  mov r3,r6
  8C240F7A  6733  mov r3,r7
  8C240F7C  2F36  mov.l r3,@-r15
  8C240F7E  2F36  mov.l r3,@-r15
  8C240F80  BA28  bsr 8C2403D4
  8C240F82  E501  mov ##0x01,r5
  8C240F84  7F08  add ##8,r15
  8C240F86  4F26  lds.l @r15+,PR
  8C240F88  000B  rts
  8C240F8A  0009  nop
  ...       (não compilado nesta sessão)
  8C241058  D35E  mov.l @([8C2411D4]),r3
  8C24105A  4F22  sts.l PR,@-r15
  8C24105C  6032  mov.l @r3,r0
  8C24105E  401B  tas.b @r0
  8C241060  8907  bt 8C241072
  ...       (não compilado nesta sessão)
  8C241072  B007  bsr 8C241084
  8C241074  0009  nop
  8C241076  D357  mov.l @([8C2411D4]),r3
  8C241078  E100  mov ##0x00,r1
  8C24107A  6232  mov.l @r3,r2
  8C24107C  2210  mov.b r1,@r2
  8C24107E  4F26  lds.l @r15+,PR
  8C241080  000B  rts
  8C241082  0009  nop
  8C241084  2FE6  mov.l r14,@-r15
  8C241086  4F22  sts.l PR,@-r15
  8C241088  7FF8  add ##-8,r15
  8C24108A  4628  shll16 r6
  8C24108C  6EF3  mov r15,r14
  8C24108E  4618  shll8 r6
  8C241090  2E52  mov.l r5,@r14
  8C241092  E300  mov ##0x00,r3
  8C241094  1E61  mov.l r6,@(4,r14)
  8C241096  2F36  mov.l r3,@-r15
  8C241098  E702  mov ##0x02,r7
  8C24109A  66E3  mov r14,r6
  8C24109C  2F36  mov.l r3,@-r15
  8C24109E  B999  bsr 8C2403D4
  8C2410A0  E50A  mov ##0x0A,r5
  8C2410A2  7F10  add ##16,r15
  8C2410A4  4F26  lds.l @r15+,PR
  8C2410A6  000B  rts
  8C2410A8  6EF6  mov.l @r15+,r14
  8C2410AA  D34A  mov.l @([8C2411D4]),r3
  8C2410AC  4F22  sts.l PR,@-r15
  8C2410AE  6032  mov.l @r3,r0
  8C2410B0  401B  tas.b @r0
  8C2410B2  8907  bt 8C2410C4
  ...       (não compilado nesta sessão)
  8C2410C4  53F1  mov.l @(4,r15),r3
  8C2410C6  2F36  mov.l r3,@-r15
  8C2410C8  B008  bsr 8C2410DC
  8C2410CA  0009  nop
  8C2410CC  7F04  add ##4,r15
  8C2410CE  D341  mov.l @([8C2411D4]),r3
  8C2410D0  E100  mov ##0x00,r1
  8C2410D2  6232  mov.l @r3,r2
  8C2410D4  2210  mov.b r1,@r2
  8C2410D6  4F26  lds.l @r15+,PR
  8C2410D8  000B  rts
  8C2410DA  0009  nop
  8C2410DC  2FE6  mov.l r14,@-r15
  8C2410DE  4F22  sts.l PR,@-r15
  8C2410E0  7FF8  add ##-8,r15
  8C2410E2  4628  shll16 r6
  8C2410E4  6EF3  mov r15,r14
  8C2410E6  4728  shll16 r7
  8C2410E8  2E52  mov.l r5,@r14
  8C2410EA  53F4  mov.l @(16,r15),r3
  8C2410EC  4618  shll8 r6
  8C2410EE  267B  or r7,r6
  8C2410F0  263B  or r3,r6
  8C2410F2  E300  mov ##0x00,r3
  8C2410F4  1E61  mov.l r6,@(4,r14)
  8C2410F6  2F36  mov.l r3,@-r15
  8C2410F8  E702  mov ##0x02,r7
  8C2410FA  66E3  mov r14,r6
  8C2410FC  2F36  mov.l r3,@-r15
  8C2410FE  B969  bsr 8C2403D4
  8C241100  E50B  mov ##0x0B,r5
  8C241102  7F10  add ##16,r15
  8C241104  4F26  lds.l @r15+,PR
  8C241106  000B  rts
  8C241108  6EF6  mov.l @r15+,r14
  8C24110A  D332  mov.l @([8C2411D4]),r3
  8C24110C  4F22  sts.l PR,@-r15
  8C24110E  6032  mov.l @r3,r0
  8C241110  401B  tas.b @r0
  8C241112  8907  bt 8C241124
  ...       (não compilado nesta sessão)
  8C241124  53F3  mov.l @(12,r15),r3
  8C241126  2F36  mov.l r3,@-r15
  8C241128  52F3  mov.l @(12,r15),r2
  8C24112A  2F26  mov.l r2,@-r15
  8C24112C  53F3  mov.l @(12,r15),r3
  8C24112E  2F36  mov.l r3,@-r15
  8C241130  B008  bsr 8C241144
  8C241132  0009  nop
  8C241134  7F0C  add ##12,r15
  8C241136  D327  mov.l @([8C2411D4]),r3
  8C241138  E100  mov ##0x00,r1
  8C24113A  6232  mov.l @r3,r2
  8C24113C  2210  mov.b r1,@r2
  8C24113E  4F26  lds.l @r15+,PR
  8C241140  000B  rts
  8C241142  0009  nop
  8C241144  2FE6  mov.l r14,@-r15
  8C241146  4F22  sts.l PR,@-r15
  8C241148  7FF8  add ##-8,r15
  8C24114A  4628  shll16 r6
  8C24114C  6EF3  mov r15,r14
  8C24114E  4618  shll8 r6
  8C241150  2E52  mov.l r5,@r14
  8C241152  4728  shll16 r7
  8C241154  53F4  mov.l @(16,r15),r3
  8C241156  267B  or r7,r6
  8C241158  263B  or r3,r6
  8C24115A  1E61  mov.l r6,@(4,r14)
  8C24115C  E702  mov ##0x02,r7
  8C24115E  53F6  mov.l @(24,r15),r3
  8C241160  66E3  mov r14,r6
  8C241162  2F36  mov.l r3,@-r15
  8C241164  52F6  mov.l @(24,r15),r2
  8C241166  2F26  mov.l r2,@-r15
  8C241168  B934  bsr 8C2403D4
  8C24116A  E50C  mov ##0x0C,r5
  8C24116C  7F10  add ##16,r15
  8C24116E  4F26  lds.l @r15+,PR
  8C241170  000B  rts
  8C241172  6EF6  mov.l @r15+,r14
  8C241174  D317  mov.l @([8C2411D4]),r3
  8C241176  4F22  sts.l PR,@-r15
  8C241178  6032  mov.l @r3,r0
  8C24117A  401B  tas.b @r0
  8C24117C  8907  bt 8C24118E
  ...       (não compilado nesta sessão)
  8C24118E  53F1  mov.l @(4,r15),r3
  8C241190  2F36  mov.l r3,@-r15
  8C241192  B008  bsr 8C2411A6
  8C241194  0009  nop
  8C241196  7F04  add ##4,r15
  8C241198  D30E  mov.l @([8C2411D4]),r3
  8C24119A  E100  mov ##0x00,r1
  8C24119C  6232  mov.l @r3,r2
  8C24119E  2210  mov.b r1,@r2
  8C2411A0  4F26  lds.l @r15+,PR
  8C2411A2  000B  rts
  8C2411A4  0009  nop
  8C2411A6  2FE6  mov.l r14,@-r15
  8C2411A8  4F22  sts.l PR,@-r15
  8C2411AA  7FF8  add ##-8,r15
  8C2411AC  4628  shll16 r6
  8C2411AE  6EF3  mov r15,r14
  8C2411B0  4728  shll16 r7
  8C2411B2  2E52  mov.l r5,@r14
  8C2411B4  53F4  mov.l @(16,r15),r3
  8C2411B6  4618  shll8 r6
  8C2411B8  267B  or r7,r6
  8C2411BA  263B  or r3,r6
  8C2411BC  E300  mov ##0x00,r3
  8C2411BE  1E61  mov.l r6,@(4,r14)
  8C2411C0  2F36  mov.l r3,@-r15
  8C2411C2  E702  mov ##0x02,r7
  8C2411C4  66E3  mov r14,r6
  8C2411C6  2F36  mov.l r3,@-r15
  8C2411C8  B904  bsr 8C2403D4
  8C2411CA  E50D  mov ##0x0D,r5
  8C2411CC  7F10  add ##16,r15
  8C2411CE  4F26  lds.l @r15+,PR
  8C2411D0  000B  rts
  8C2411D2  6EF6  mov.l @r15+,r14
  ...       (não compilado nesta sessão)
```
