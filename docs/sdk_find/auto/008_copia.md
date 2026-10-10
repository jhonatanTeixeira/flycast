# 008_copia

> Gerado por `tools/sdk_find.py`. Jogos: 8 · variantes (sequências normalizadas distintas): 5 · tempo perf somado: 5.87% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C05D81A` | 104 | 0.01% |
| Evolution - The World of Sacred Device (USA) | `8C15E84E` | 107 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C17BD82` | 104 | 0.00% |
| Grandia II (USA) | `8C028204` | 104 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C316F7C` | 107 | 0.00% |
| Macross M3 | `8C1F70A0` | 104 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C13503C` | 107 | 5.86% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C1727C2` | 95 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C05D81A`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C05D81A  2FE6  mov.l r14,@-r15
  8C05D81C  2FD6  mov.l r13,@-r15
  8C05D81E  2FC6  mov.l r12,@-r15
  8C05D820  2FB6  mov.l r11,@-r15
  8C05D822  2FA6  mov.l r10,@-r15
  8C05D824  2F96  mov.l r9,@-r15
  8C05D826  2F86  mov.l r8,@-r15
  8C05D828  4F22  sts.l PR,@-r15
  8C05D82A  DD1F  mov.l @([8C05D8A8]),r13
  8C05D82C  4D0B  jsr @r13
  8C05D82E  0009  nop
  8C05D830  D31E  mov.l @([8C05D8AC]),r3
  8C05D832  4D0B  jsr @r13
  8C05D834  2302  mov.l r0,@r3
  8C05D836  D21E  mov.l @([8C05D8B0]),r2
  8C05D838  6E03  mov r0,r14
  8C05D83A  6022  mov.l @r2,r0
  8C05D83C  8801  cmp/eq ##0x01,R0
  8C05D83E  8B02  bf 8C05D846
  8C05D840  D11C  mov.l @([8C05D8B4]),r1
  8C05D842  410B  jsr @r1
  8C05D844  0009  nop
  8C05D846  4D0B  jsr @r13
  8C05D848  0009  nop
  8C05D84A  D31B  mov.l @([8C05D8B8]),r3
  8C05D84C  6503  mov r0,r5
  8C05D84E  6C03  mov r0,r12
  8C05D850  430B  jsr @r3
  8C05D852  64E3  mov r14,r4
  8C05D854  D219  mov.l @([8C05D8BC]),r2
  8C05D856  420B  jsr @r2
  8C05D858  6403  mov r0,r4
  8C05D85A  931D  mov.w @([8C05D898]),r3
  8C05D85C  6403  mov r0,r4
  8C05D85E  3433  cmp/ge r3,r4
  8C05D860  8B07  bf 8C05D872
  8C05D862  D519  mov.l @([8C05D8C8]),r5
  8C05D864  D217  mov.l @([8C05D8C4]),r2
  8C05D866  D116  mov.l @([8C05D8C0]),r1
  8C05D868  21E2  mov.l r14,@r1
  8C05D86A  22C2  mov.l r12,@r2
  8C05D86C  6352  mov.l @r5,r3
  8C05D86E  334C  add r4,r3
  8C05D870  2532  mov.l r3,@r5
  8C05D872  4D0B  jsr @r13
  8C05D874  0009  nop
  8C05D876  9B11  mov.w @([8C05D89C]),r11
  8C05D878  EC00  mov ##0x00,r12
  8C05D87A  9A0E  mov.w @([8C05D89A]),r10
  8C05D87C  6EC3  mov r12,r14
  8C05D87E  6803  mov r0,r8
  8C05D880  E901  mov ##0x01,r9
  8C05D882  D412  mov.l @([8C05D8CC]),r4
  8C05D884  34EC  add r14,r4
  8C05D886  5241  mov.l @(4,r4),r2
  8C05D888  2228  tst r2,r2
  8C05D88A  8924  bt 8C05D8D6
  8C05D88C  5342  mov.l @(8,r4),r3
  8C05D88E  2338  tst r3,r3
  8C05D890  8B1E  bf 8C05D8D0
  ...
  8C05D8D0  E050  mov ##0x50,r0
  8C05D8D2  BF71  bsr 8C05D7B8
  8C05D8D4  0496  mov.l r9,@(R0,r4)
  8C05D8D6  3EAC  add r10,r14
  8C05D8D8  3EB2  cmp/hs r11,r14
  8C05D8DA  8BD2  bf 8C05D882
  8C05D8DC  4D0B  jsr @r13
  8C05D8DE  0009  nop
  8C05D8E0  D359  mov.l @([8C05DA48]),r3
  8C05D8E2  6503  mov r0,r5
  8C05D8E4  6E03  mov r0,r14
  8C05D8E6  430B  jsr @r3
  8C05D8E8  6483  mov r8,r4
  8C05D8EA  D258  mov.l @([8C05DA4C]),r2
  8C05D8EC  420B  jsr @r2
  8C05D8EE  6403  mov r0,r4
  8C05D8F0  E332  mov ##0x32,r3
  8C05D8F2  6403  mov r0,r4
  8C05D8F4  3433  cmp/ge r3,r4
  8C05D8F6  8B07  bf 8C05D908
  8C05D8F8  D557  mov.l @([8C05DA58]),r5
  8C05D8FA  D256  mov.l @([8C05DA54]),r2
  8C05D8FC  D154  mov.l @([8C05DA50]),r1
  8C05D8FE  2182  mov.l r8,@r1
  8C05D900  22E2  mov.l r14,@r2
  8C05D902  6352  mov.l @r5,r3
  8C05D904  334C  add r4,r3
  8C05D906  2532  mov.l r3,@r5
  8C05D908  D154  mov.l @([8C05DA5C]),r1
  8C05D90A  D055  mov.l @([8C05DA60]),r0
  8C05D90C  6212  mov.l @r1,r2
  8C05D90E  4D0B  jsr @r13
  8C05D910  2022  mov.l r2,@r0
  8C05D912  D354  mov.l @([8C05DA64]),r3
  8C05D914  2302  mov.l r0,@r3
  8C05D916  4F26  lds.l @r15+,PR
  8C05D918  68F6  mov.l @r15+,r8
  8C05D91A  69F6  mov.l @r15+,r9
  8C05D91C  6AF6  mov.l @r15+,r10
  8C05D91E  6BF6  mov.l @r15+,r11
  8C05D920  6CF6  mov.l @r15+,r12
  8C05D922  6DF6  mov.l @r15+,r13
  8C05D924  000B  rts
  8C05D926  6EF6  mov.l @r15+,r14
```

## Evolution - The World of Sacred Device (USA) `8C15E84E`

Dump: `/mnt/1TB/dcbat/20261007-151806_Evolution_-_The_World_of_Sacred_Device__/jit-24035.txt`

```
  8C15E84E  2FE6  mov.l r14,@-r15
  8C15E850  2FD6  mov.l r13,@-r15
  8C15E852  2FC6  mov.l r12,@-r15
  8C15E854  2FB6  mov.l r11,@-r15
  8C15E856  2FA6  mov.l r10,@-r15
  8C15E858  2F96  mov.l r9,@-r15
  8C15E85A  2F86  mov.l r8,@-r15
  8C15E85C  4F22  sts.l PR,@-r15
  8C15E85E  DD1F  mov.l @([8C15E8DC]),r13
  8C15E860  4D0B  jsr @r13
  8C15E862  0009  nop
  8C15E864  D31E  mov.l @([8C15E8E0]),r3
  8C15E866  4D0B  jsr @r13
  8C15E868  2302  mov.l r0,@r3
  8C15E86A  D21E  mov.l @([8C15E8E4]),r2
  8C15E86C  6E03  mov r0,r14
  8C15E86E  6022  mov.l @r2,r0
  8C15E870  8801  cmp/eq ##0x01,R0
  8C15E872  8B02  bf 8C15E87A
  8C15E874  D11C  mov.l @([8C15E8E8]),r1
  8C15E876  410B  jsr @r1
  8C15E878  0009  nop
  8C15E87A  4D0B  jsr @r13
  8C15E87C  0009  nop
  8C15E87E  D31B  mov.l @([8C15E8EC]),r3
  8C15E880  6503  mov r0,r5
  8C15E882  6C03  mov r0,r12
  8C15E884  430B  jsr @r3
  8C15E886  64E3  mov r14,r4
  8C15E888  D219  mov.l @([8C15E8F0]),r2
  8C15E88A  420B  jsr @r2
  8C15E88C  6403  mov r0,r4
  8C15E88E  931D  mov.w @([8C15E8CC]),r3
  8C15E890  6403  mov r0,r4
  8C15E892  3433  cmp/ge r3,r4
  8C15E894  8B07  bf 8C15E8A6
  8C15E896  D519  mov.l @([8C15E8FC]),r5
  8C15E898  D217  mov.l @([8C15E8F8]),r2
  8C15E89A  D116  mov.l @([8C15E8F4]),r1
  8C15E89C  21E2  mov.l r14,@r1
  8C15E89E  22C2  mov.l r12,@r2
  8C15E8A0  6352  mov.l @r5,r3
  8C15E8A2  334C  add r4,r3
  8C15E8A4  2532  mov.l r3,@r5
  8C15E8A6  4D0B  jsr @r13
  8C15E8A8  0009  nop
  8C15E8AA  9B11  mov.w @([8C15E8D0]),r11
  8C15E8AC  EC00  mov ##0x00,r12
  8C15E8AE  9A0E  mov.w @([8C15E8CE]),r10
  8C15E8B0  6EC3  mov r12,r14
  8C15E8B2  6803  mov r0,r8
  8C15E8B4  E901  mov ##0x01,r9
  8C15E8B6  D412  mov.l @([8C15E900]),r4
  8C15E8B8  34EC  add r14,r4
  8C15E8BA  5241  mov.l @(4,r4),r2
  8C15E8BC  2228  tst r2,r2
  8C15E8BE  8924  bt 8C15E90A
  8C15E8C0  5342  mov.l @(8,r4),r3
  8C15E8C2  2338  tst r3,r3
  8C15E8C4  8B1E  bf 8C15E904
  8C15E8C6  E050  mov ##0x50,r0
  8C15E8C8  A01F  bra 8C15E90A
  8C15E8CA  04C6  mov.l r12,@(R0,r4)
  ...
  8C15E904  E050  mov ##0x50,r0
  8C15E906  BF71  bsr 8C15E7EC
  8C15E908  0496  mov.l r9,@(R0,r4)
  8C15E90A  3EAC  add r10,r14
  8C15E90C  3EB2  cmp/hs r11,r14
  8C15E90E  8BD2  bf 8C15E8B6
  8C15E910  4D0B  jsr @r13
  8C15E912  0009  nop
  8C15E914  D359  mov.l @([8C15EA7C]),r3
  8C15E916  6503  mov r0,r5
  8C15E918  6E03  mov r0,r14
  8C15E91A  430B  jsr @r3
  8C15E91C  6483  mov r8,r4
  8C15E91E  D258  mov.l @([8C15EA80]),r2
  8C15E920  420B  jsr @r2
  8C15E922  6403  mov r0,r4
  8C15E924  E332  mov ##0x32,r3
  8C15E926  6403  mov r0,r4
  8C15E928  3433  cmp/ge r3,r4
  8C15E92A  8B07  bf 8C15E93C
  8C15E92C  D557  mov.l @([8C15EA8C]),r5
  8C15E92E  D256  mov.l @([8C15EA88]),r2
  8C15E930  D154  mov.l @([8C15EA84]),r1
  8C15E932  2182  mov.l r8,@r1
  8C15E934  22E2  mov.l r14,@r2
  8C15E936  6352  mov.l @r5,r3
  8C15E938  334C  add r4,r3
  8C15E93A  2532  mov.l r3,@r5
  8C15E93C  D154  mov.l @([8C15EA90]),r1
  8C15E93E  D055  mov.l @([8C15EA94]),r0
  8C15E940  6212  mov.l @r1,r2
  8C15E942  4D0B  jsr @r13
  8C15E944  2022  mov.l r2,@r0
  8C15E946  D354  mov.l @([8C15EA98]),r3
  8C15E948  2302  mov.l r0,@r3
  8C15E94A  4F26  lds.l @r15+,PR
  8C15E94C  68F6  mov.l @r15+,r8
  8C15E94E  69F6  mov.l @r15+,r9
  8C15E950  6AF6  mov.l @r15+,r10
  8C15E952  6BF6  mov.l @r15+,r11
  8C15E954  6CF6  mov.l @r15+,r12
  8C15E956  6DF6  mov.l @r15+,r13
  8C15E958  000B  rts
  8C15E95A  6EF6  mov.l @r15+,r14
```

## Grandia II (USA) `8C028204`

Dump: `/mnt/1TB/dcbat/20261002-192949_Grandia_II__USA__/jit-534412.txt`

```
  8C028204  2FE6  mov.l r14,@-r15
  8C028206  2FD6  mov.l r13,@-r15
  8C028208  2FC6  mov.l r12,@-r15
  8C02820A  2FB6  mov.l r11,@-r15
  8C02820C  2FA6  mov.l r10,@-r15
  8C02820E  2F96  mov.l r9,@-r15
  8C028210  2F86  mov.l r8,@-r15
  8C028212  4F22  sts.l PR,@-r15
  8C028214  DD32  mov.l @([8C0282E0]),r13
  8C028216  4D0B  jsr @r13
  8C028218  0009  nop
  8C02821A  D332  mov.l @([8C0282E4]),r3
  8C02821C  4D0B  jsr @r13
  8C02821E  2302  mov.l r0,@r3
  8C028220  D231  mov.l @([8C0282E8]),r2
  8C028222  6E03  mov r0,r14
  8C028224  6022  mov.l @r2,r0
  8C028226  8801  cmp/eq ##0x01,R0
  8C028228  8B02  bf 8C028230
  8C02822A  D130  mov.l @([8C0282EC]),r1
  8C02822C  410B  jsr @r1
  8C02822E  0009  nop
  8C028230  4D0B  jsr @r13
  8C028232  0009  nop
  8C028234  D32E  mov.l @([8C0282F0]),r3
  8C028236  6503  mov r0,r5
  8C028238  6C03  mov r0,r12
  8C02823A  430B  jsr @r3
  8C02823C  64E3  mov r14,r4
  8C02823E  D22D  mov.l @([8C0282F4]),r2
  8C028240  420B  jsr @r2
  8C028242  6403  mov r0,r4
  8C028244  9349  mov.w @([8C0282DA]),r3
  8C028246  6403  mov r0,r4
  8C028248  3433  cmp/ge r3,r4
  8C02824A  8B07  bf 8C02825C
  8C02824C  D52C  mov.l @([8C028300]),r5
  8C02824E  D22B  mov.l @([8C0282FC]),r2
  8C028250  D129  mov.l @([8C0282F8]),r1
  8C028252  21E2  mov.l r14,@r1
  8C028254  22C2  mov.l r12,@r2
  8C028256  6352  mov.l @r5,r3
  8C028258  334C  add r4,r3
  8C02825A  2532  mov.l r3,@r5
  8C02825C  4D0B  jsr @r13
  8C02825E  0009  nop
  8C028260  9B3D  mov.w @([8C0282DE]),r11
  8C028262  EC00  mov ##0x00,r12
  8C028264  9A3A  mov.w @([8C0282DC]),r10
  8C028266  6EC3  mov r12,r14
  8C028268  6803  mov r0,r8
  8C02826A  E901  mov ##0x01,r9
  8C02826C  D425  mov.l @([8C028304]),r4
  8C02826E  34EC  add r14,r4
  8C028270  5241  mov.l @(4,r4),r2
  8C028272  2228  tst r2,r2
  8C028274  8908  bt 8C028288
  8C028276  5342  mov.l @(8,r4),r3
  8C028278  2338  tst r3,r3
  8C02827A  8B02  bf 8C028282
  ...
  8C028282  E058  mov ##0x58,r0
  8C028284  BF76  bsr 8C028174
  8C028286  0496  mov.l r9,@(R0,r4)
  8C028288  3EAC  add r10,r14
  8C02828A  3EB2  cmp/hs r11,r14
  8C02828C  8BEE  bf 8C02826C
  8C02828E  4D0B  jsr @r13
  8C028290  0009  nop
  8C028292  D317  mov.l @([8C0282F0]),r3
  8C028294  6503  mov r0,r5
  8C028296  6E03  mov r0,r14
  8C028298  430B  jsr @r3
  8C02829A  6483  mov r8,r4
  8C02829C  D215  mov.l @([8C0282F4]),r2
  8C02829E  420B  jsr @r2
  8C0282A0  6403  mov r0,r4
  8C0282A2  E332  mov ##0x32,r3
  8C0282A4  6403  mov r0,r4
  8C0282A6  3433  cmp/ge r3,r4
  8C0282A8  8B07  bf 8C0282BA
  8C0282AA  D519  mov.l @([8C028310]),r5
  8C0282AC  D217  mov.l @([8C02830C]),r2
  8C0282AE  D116  mov.l @([8C028308]),r1
  8C0282B0  2182  mov.l r8,@r1
  8C0282B2  22E2  mov.l r14,@r2
  8C0282B4  6352  mov.l @r5,r3
  8C0282B6  334C  add r4,r3
  8C0282B8  2532  mov.l r3,@r5
  8C0282BA  D10A  mov.l @([8C0282E4]),r1
  8C0282BC  D015  mov.l @([8C028314]),r0
  8C0282BE  6212  mov.l @r1,r2
  8C0282C0  4D0B  jsr @r13
  8C0282C2  2022  mov.l r2,@r0
  8C0282C4  D314  mov.l @([8C028318]),r3
  8C0282C6  2302  mov.l r0,@r3
  8C0282C8  4F26  lds.l @r15+,PR
  8C0282CA  68F6  mov.l @r15+,r8
  8C0282CC  69F6  mov.l @r15+,r9
  8C0282CE  6AF6  mov.l @r15+,r10
  8C0282D0  6BF6  mov.l @r15+,r11
  8C0282D2  6CF6  mov.l @r15+,r12
  8C0282D4  6DF6  mov.l @r15+,r13
  8C0282D6  000B  rts
  8C0282D8  6EF6  mov.l @r15+,r14
```

## King of Fighters The - Evolution (USA) (EnJaEsPt) `8C316F7C`

Dump: `/mnt/1TB/dcbat/20261007-164133_King_of_Fighters_The_-_Evolution__USA___/jit-113430.txt`

```
  8C316F7C  2FE6  mov.l r14,@-r15
  8C316F7E  2FD6  mov.l r13,@-r15
  8C316F80  2FC6  mov.l r12,@-r15
  8C316F82  2FB6  mov.l r11,@-r15
  8C316F84  2FA6  mov.l r10,@-r15
  8C316F86  2F96  mov.l r9,@-r15
  8C316F88  2F86  mov.l r8,@-r15
  8C316F8A  4F22  sts.l PR,@-r15
  8C316F8C  DD32  mov.l @([8C317058]),r13
  8C316F8E  4D0B  jsr @r13
  8C316F90  0009  nop
  8C316F92  D332  mov.l @([8C31705C]),r3
  8C316F94  4D0B  jsr @r13
  8C316F96  2302  mov.l r0,@r3
  8C316F98  D231  mov.l @([8C317060]),r2
  8C316F9A  6E03  mov r0,r14
  8C316F9C  6022  mov.l @r2,r0
  8C316F9E  8801  cmp/eq ##0x01,R0
  8C316FA0  8B02  bf 8C316FA8
  8C316FA2  D130  mov.l @([8C317064]),r1
  8C316FA4  410B  jsr @r1
  8C316FA6  0009  nop
  8C316FA8  4D0B  jsr @r13
  8C316FAA  0009  nop
  8C316FAC  D32E  mov.l @([8C317068]),r3
  8C316FAE  6503  mov r0,r5
  8C316FB0  6C03  mov r0,r12
  8C316FB2  430B  jsr @r3
  8C316FB4  64E3  mov r14,r4
  8C316FB6  D22D  mov.l @([8C31706C]),r2
  8C316FB8  420B  jsr @r2
  8C316FBA  6403  mov r0,r4
  8C316FBC  9349  mov.w @([8C317052]),r3
  8C316FBE  6403  mov r0,r4
  8C316FC0  3433  cmp/ge r3,r4
  8C316FC2  8B07  bf 8C316FD4
  8C316FC4  D52C  mov.l @([8C317078]),r5
  8C316FC6  D22B  mov.l @([8C317074]),r2
  8C316FC8  D129  mov.l @([8C317070]),r1
  8C316FCA  21E2  mov.l r14,@r1
  8C316FCC  22C2  mov.l r12,@r2
  8C316FCE  6352  mov.l @r5,r3
  8C316FD0  334C  add r4,r3
  8C316FD2  2532  mov.l r3,@r5
  8C316FD4  4D0B  jsr @r13
  8C316FD6  0009  nop
  8C316FD8  9B3D  mov.w @([8C317056]),r11
  8C316FDA  EC00  mov ##0x00,r12
  8C316FDC  9A3A  mov.w @([8C317054]),r10
  8C316FDE  6EC3  mov r12,r14
  8C316FE0  6803  mov r0,r8
  8C316FE2  E901  mov ##0x01,r9
  8C316FE4  D425  mov.l @([8C31707C]),r4
  8C316FE6  34EC  add r14,r4
  8C316FE8  5241  mov.l @(4,r4),r2
  8C316FEA  2228  tst r2,r2
  8C316FEC  8908  bt 8C317000
  8C316FEE  5342  mov.l @(8,r4),r3
  8C316FF0  2338  tst r3,r3
  8C316FF2  8B02  bf 8C316FFA
  8C316FF4  E058  mov ##0x58,r0
  8C316FF6  A003  bra 8C317000
  8C316FF8  04C6  mov.l r12,@(R0,r4)
  8C316FFA  E058  mov ##0x58,r0
  8C316FFC  BF76  bsr 8C316EEC
  8C316FFE  0496  mov.l r9,@(R0,r4)
  8C317000  3EAC  add r10,r14
  8C317002  3EB2  cmp/hs r11,r14
  8C317004  8BEE  bf 8C316FE4
  8C317006  4D0B  jsr @r13
  8C317008  0009  nop
  8C31700A  D317  mov.l @([8C317068]),r3
  8C31700C  6503  mov r0,r5
  8C31700E  6E03  mov r0,r14
  8C317010  430B  jsr @r3
  8C317012  6483  mov r8,r4
  8C317014  D215  mov.l @([8C31706C]),r2
  8C317016  420B  jsr @r2
  8C317018  6403  mov r0,r4
  8C31701A  E332  mov ##0x32,r3
  8C31701C  6403  mov r0,r4
  8C31701E  3433  cmp/ge r3,r4
  8C317020  8B07  bf 8C317032
  8C317022  D519  mov.l @([8C317088]),r5
  8C317024  D217  mov.l @([8C317084]),r2
  8C317026  D116  mov.l @([8C317080]),r1
  8C317028  2182  mov.l r8,@r1
  8C31702A  22E2  mov.l r14,@r2
  8C31702C  6352  mov.l @r5,r3
  8C31702E  334C  add r4,r3
  8C317030  2532  mov.l r3,@r5
  8C317032  D10A  mov.l @([8C31705C]),r1
  8C317034  D015  mov.l @([8C31708C]),r0
  8C317036  6212  mov.l @r1,r2
  8C317038  4D0B  jsr @r13
  8C31703A  2022  mov.l r2,@r0
  8C31703C  D314  mov.l @([8C317090]),r3
  8C31703E  2302  mov.l r0,@r3
  8C317040  4F26  lds.l @r15+,PR
  8C317042  68F6  mov.l @r15+,r8
  8C317044  69F6  mov.l @r15+,r9
  8C317046  6AF6  mov.l @r15+,r10
  8C317048  6BF6  mov.l @r15+,r11
  8C31704A  6CF6  mov.l @r15+,r12
  8C31704C  6DF6  mov.l @r15+,r13
  8C31704E  000B  rts
  8C317050  6EF6  mov.l @r15+,r14
```

## Resident Evil - Code - Veronica (USA) (Disc 1) `8C1727C2`

Dump: `/mnt/1TB/dcbat/20261007-160515_Resident_Evil_-_Code_-_Veronica__USA___D/jit-82021.txt`

```
  8C1727C2  2FE6  mov.l r14,@-r15
  8C1727C4  2FD6  mov.l r13,@-r15
  8C1727C6  2FC6  mov.l r12,@-r15
  8C1727C8  2FB6  mov.l r11,@-r15
  8C1727CA  2FA6  mov.l r10,@-r15
  8C1727CC  2F96  mov.l r9,@-r15
  8C1727CE  2F86  mov.l r8,@-r15
  8C1727D0  4F22  sts.l PR,@-r15
  8C1727D2  DD1F  mov.l @([8C172850]),r13
  8C1727D4  4D0B  jsr @r13
  8C1727D6  0009  nop
  8C1727D8  D31E  mov.l @([8C172854]),r3
  8C1727DA  4D0B  jsr @r13
  8C1727DC  2302  mov.l r0,@r3
  8C1727DE  D21E  mov.l @([8C172858]),r2
  8C1727E0  6E03  mov r0,r14
  8C1727E2  6022  mov.l @r2,r0
  8C1727E4  8801  cmp/eq ##0x01,R0
  8C1727E6  8B02  bf 8C1727EE
  ...
  8C1727EE  4D0B  jsr @r13
  8C1727F0  0009  nop
  8C1727F2  D31B  mov.l @([8C172860]),r3
  8C1727F4  6503  mov r0,r5
  8C1727F6  6C03  mov r0,r12
  8C1727F8  430B  jsr @r3
  8C1727FA  64E3  mov r14,r4
  8C1727FC  D219  mov.l @([8C172864]),r2
  8C1727FE  420B  jsr @r2
  8C172800  6403  mov r0,r4
  8C172802  931D  mov.w @([8C172840]),r3
  8C172804  6403  mov r0,r4
  8C172806  3433  cmp/ge r3,r4
  8C172808  8B07  bf 8C17281A
  8C17280A  D519  mov.l @([8C172870]),r5
  8C17280C  D217  mov.l @([8C17286C]),r2
  8C17280E  D116  mov.l @([8C172868]),r1
  8C172810  21E2  mov.l r14,@r1
  8C172812  22C2  mov.l r12,@r2
  8C172814  6352  mov.l @r5,r3
  8C172816  334C  add r4,r3
  8C172818  2532  mov.l r3,@r5
  8C17281A  4D0B  jsr @r13
  8C17281C  0009  nop
  8C17281E  9B11  mov.w @([8C172844]),r11
  8C172820  EC00  mov ##0x00,r12
  8C172822  9A0E  mov.w @([8C172842]),r10
  8C172824  6EC3  mov r12,r14
  8C172826  6803  mov r0,r8
  8C172828  E901  mov ##0x01,r9
  8C17282A  D412  mov.l @([8C172874]),r4
  8C17282C  34EC  add r14,r4
  8C17282E  5241  mov.l @(4,r4),r2
  8C172830  2228  tst r2,r2
  8C172832  8924  bt 8C17287E
  ...
  8C17287E  3EAC  add r10,r14
  8C172880  3EB2  cmp/hs r11,r14
  8C172882  8BD2  bf 8C17282A
  8C172884  4D0B  jsr @r13
  8C172886  0009  nop
  8C172888  D359  mov.l @([8C1729F0]),r3
  8C17288A  6503  mov r0,r5
  8C17288C  6E03  mov r0,r14
  8C17288E  430B  jsr @r3
  8C172890  6483  mov r8,r4
  8C172892  D258  mov.l @([8C1729F4]),r2
  8C172894  420B  jsr @r2
  8C172896  6403  mov r0,r4
  8C172898  E332  mov ##0x32,r3
  8C17289A  6403  mov r0,r4
  8C17289C  3433  cmp/ge r3,r4
  8C17289E  8B07  bf 8C1728B0
  8C1728A0  D557  mov.l @([8C172A00]),r5
  8C1728A2  D256  mov.l @([8C1729FC]),r2
  8C1728A4  D154  mov.l @([8C1729F8]),r1
  8C1728A6  2182  mov.l r8,@r1
  8C1728A8  22E2  mov.l r14,@r2
  8C1728AA  6352  mov.l @r5,r3
  8C1728AC  334C  add r4,r3
  8C1728AE  2532  mov.l r3,@r5
  8C1728B0  D154  mov.l @([8C172A04]),r1
  8C1728B2  D055  mov.l @([8C172A08]),r0
  8C1728B4  6212  mov.l @r1,r2
  8C1728B6  4D0B  jsr @r13
  8C1728B8  2022  mov.l r2,@r0
  8C1728BA  D354  mov.l @([8C172A0C]),r3
  8C1728BC  2302  mov.l r0,@r3
  8C1728BE  4F26  lds.l @r15+,PR
  8C1728C0  68F6  mov.l @r15+,r8
  8C1728C2  69F6  mov.l @r15+,r9
  8C1728C4  6AF6  mov.l @r15+,r10
  8C1728C6  6BF6  mov.l @r15+,r11
  8C1728C8  6CF6  mov.l @r15+,r12
  8C1728CA  6DF6  mov.l @r15+,r13
  8C1728CC  000B  rts
  8C1728CE  6EF6  mov.l @r15+,r14
```
