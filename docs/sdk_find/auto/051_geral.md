# 051_geral

> Gerado por `tools/sdk_find.py`. Jogos: 6 · variantes (sequências normalizadas distintas): 4 · tempo perf somado: 0.83% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Grandia II (USA) | `8C06F23C` | 110 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C33C8BC` | 110 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C060624` | 38 | 0.00% |
| Macross M3 | `8C1FC988` | 79 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C161EC4` | 114 | 0.83% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C36F96C` | 110 | 0.00% |

## Grandia II (USA) `8C06F23C`

Dump: `/mnt/1TB/dcbat/20261002-192949_Grandia_II__USA__/jit-534412.txt`

```
  8C06F23C  2FE6  mov.l r14,@-r15
  8C06F23E  2FD6  mov.l r13,@-r15
  8C06F240  2FC6  mov.l r12,@-r15
  8C06F242  2FB6  mov.l r11,@-r15
  8C06F244  2FA6  mov.l r10,@-r15
  8C06F246  2F96  mov.l r9,@-r15
  8C06F248  2F86  mov.l r8,@-r15
  8C06F24A  4F22  sts.l PR,@-r15
  8C06F24C  7FF0  add ##-16,r15
  8C06F24E  6D43  mov r4,r13
  8C06F250  9424  mov.w @([8C06F29C]),r4
  8C06F252  9322  mov.w @([8C06F29A]),r3
  8C06F254  6B53  mov r5,r11
  8C06F256  34DC  add r13,r4
  8C06F258  9C21  mov.w @([8C06F29E]),r12
  8C06F25A  343C  add r3,r4
  8C06F25C  6843  mov r4,r8
  8C06F25E  941F  mov.w @([8C06F2A0]),r4
  8C06F260  3CDC  add r13,r12
  8C06F262  34CC  add r12,r4
  8C06F264  65C3  mov r12,r5
  8C06F266  6242  mov.l @r4,r2
  8C06F268  7808  add ##8,r8
  8C06F26A  2228  tst r2,r2
  8C06F26C  8F02  bf.s 8C06F274
  8C06F26E  7568  add ##104,r5
  8C06F270  A006  bra 8C06F280
  8C06F272  EE00  mov ##0x00,r14
  8C06F274  9015  mov.w @([8C06F2A2]),r0
  8C06F276  5E58  mov.l @(32,r5),r14
  8C06F278  5248  mov.l @(32,r4),r2
  8C06F27A  01CE  mov.l @(R0,r12),r1
  8C06F27C  3E28  sub r2,r14
  8C06F27E  3E1C  add r1,r14
  8C06F280  59C5  mov.l @(20,r12),r9
  8C06F282  2998  tst r9,r9
  8C06F284  8D12  bt.s 8C06F2AC
  8C06F286  5A59  mov.l @(36,r5),r10
  ...
  8C06F2AC  60B3  mov r11,r0
  8C06F2AE  8801  cmp/eq ##0x01,R0
  8C06F2B0  8B03  bf 8C06F2BA
  8C06F2B2  D342  mov.l @([8C06F3BC]),r3
  8C06F2B4  65E3  mov r14,r5
  8C06F2B6  430B  jsr @r3
  8C06F2B8  64C3  mov r12,r4
  8C06F2BA  60B3  mov r11,r0
  8C06F2BC  8801  cmp/eq ##0x01,R0
  8C06F2BE  8902  bt 8C06F2C6
  8C06F2C0  60B3  mov r11,r0
  8C06F2C2  8802  cmp/eq ##0x02,R0
  8C06F2C4  8B04  bf 8C06F2D0
  8C06F2C6  D23E  mov.l @([8C06F3C0]),r2
  8C06F2C8  65E3  mov r14,r5
  8C06F2CA  420B  jsr @r2
  8C06F2CC  64C3  mov r12,r4
  8C06F2CE  6E03  mov r0,r14
  8C06F2D0  D33C  mov.l @([8C06F3C4]),r3
  8C06F2D2  430B  jsr @r3
  8C06F2D4  64D3  mov r13,r4
  8C06F2D6  926A  mov.w @([8C06F3AE]),r2
  8C06F2D8  3027  cmp/gt r2,r0
  8C06F2DA  8904  bt 8C06F2E6
  8C06F2DC  9068  mov.w @([8C06F3B0]),r0
  8C06F2DE  538F  mov.l @(60,r8),r3
  8C06F2E0  01DE  mov.l @(R0,r13),r1
  8C06F2E2  3313  cmp/ge r1,r3
  8C06F2E4  891E  bt 8C06F324
  8C06F2E6  D338  mov.l @([8C06F3C8]),r3
  8C06F2E8  66F3  mov r15,r6
  8C06F2EA  65F3  mov r15,r5
  8C06F2EC  760C  add ##12,r6
  8C06F2EE  430B  jsr @r3
  8C06F2F0  64D3  mov r13,r4
  8C06F2F2  62F2  mov.l @r15,r2
  8C06F2F4  4211  cmp/pz r2
  8C06F2F6  8B15  bf 8C06F324
  8C06F2F8  66F3  mov r15,r6
  8C06F2FA  67F3  mov r15,r7
  8C06F2FC  65B3  mov r11,r5
  8C06F2FE  7604  add ##4,r6
  8C06F300  7708  add ##8,r7
  8C06F302  B01F  bsr 8C06F344
  8C06F304  64D3  mov r13,r4
  8C06F306  51F1  mov.l @(4,r15),r1
  8C06F308  D330  mov.l @([8C06F3CC]),r3
  8C06F30A  0A17  mul.l r1,r10
  8C06F30C  011A  sts MACL,r1
  8C06F30E  430B  jsr @r3
  8C06F310  50F2  mov.l @(8,r15),r0
  8C06F312  55F3  mov.l @(12,r15),r5
  8C06F314  3E08  sub r0,r14
  8C06F316  D32E  mov.l @([8C06F3D0]),r3
  8C06F318  67A3  mov r10,r7
  8C06F31A  66E3  mov r14,r6
  8C06F31C  430B  jsr @r3
  8C06F31E  64F2  mov.l @r15,r4
  8C06F320  2008  tst r0,r0
  8C06F322  8901  bt 8C06F328
  8C06F324  A004  bra 8C06F330
  8C06F326  E000  mov ##0x00,r0
  ...
  8C06F330  7F10  add ##16,r15
  8C06F332  4F26  lds.l @r15+,PR
  8C06F334  68F6  mov.l @r15+,r8
  8C06F336  69F6  mov.l @r15+,r9
  8C06F338  6AF6  mov.l @r15+,r10
  8C06F33A  6BF6  mov.l @r15+,r11
  8C06F33C  6CF6  mov.l @r15+,r12
  8C06F33E  6DF6  mov.l @r15+,r13
  8C06F340  000B  rts
  8C06F342  6EF6  mov.l @r15+,r14
```

## Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) `8C060624`

Dump: `/mnt/1TB/dcbat/20261007-163759_Le_Mans_24_Hours__Europe___En_Fr_De_Es_I/jit-109461.txt`

```
  8C060624  2FE6  mov.l r14,@-r15
  8C060626  2FD6  mov.l r13,@-r15
  8C060628  2FC6  mov.l r12,@-r15
  8C06062A  2FB6  mov.l r11,@-r15
  8C06062C  2FA6  mov.l r10,@-r15
  8C06062E  2F96  mov.l r9,@-r15
  8C060630  2F86  mov.l r8,@-r15
  8C060632  4F22  sts.l PR,@-r15
  8C060634  7FF0  add ##-16,r15
  8C060636  6D43  mov r4,r13
  8C060638  9424  mov.w @([8C060684]),r4
  8C06063A  9322  mov.w @([8C060682]),r3
  8C06063C  6B53  mov r5,r11
  8C06063E  34DC  add r13,r4
  8C060640  9C21  mov.w @([8C060686]),r12
  8C060642  343C  add r3,r4
  8C060644  6843  mov r4,r8
  8C060646  941F  mov.w @([8C060688]),r4
  8C060648  3CDC  add r13,r12
  8C06064A  34CC  add r12,r4
  8C06064C  65C3  mov r12,r5
  8C06064E  6242  mov.l @r4,r2
  8C060650  7808  add ##8,r8
  8C060652  2228  tst r2,r2
  8C060654  8F02  bf.s 8C06065C
  8C060656  7568  add ##104,r5
  8C060658  A006  bra 8C060668
  8C06065A  EE00  mov ##0x00,r14
  8C06065C  9015  mov.w @([8C06068A]),r0
  8C06065E  5E58  mov.l @(32,r5),r14
  8C060660  5248  mov.l @(32,r4),r2
  8C060662  01CE  mov.l @(R0,r12),r1
  8C060664  3E28  sub r2,r14
  8C060666  3E1C  add r1,r14
  8C060668  59C5  mov.l @(20,r12),r9
  8C06066A  2998  tst r9,r9
  8C06066C  8D12  bt.s 8C060694
  8C06066E  5A59  mov.l @(36,r5),r10
```

## Macross M3 `8C1FC988`

Dump: `/mnt/1TB/dcbat/20261006-081557_Macross_M3_/jit-5965.txt`

```
  8C1FC988  2FE6  mov.l r14,@-r15
  8C1FC98A  2FD6  mov.l r13,@-r15
  8C1FC98C  2FC6  mov.l r12,@-r15
  8C1FC98E  2FB6  mov.l r11,@-r15
  8C1FC990  2FA6  mov.l r10,@-r15
  8C1FC992  2F96  mov.l r9,@-r15
  8C1FC994  2F86  mov.l r8,@-r15
  8C1FC996  4F22  sts.l PR,@-r15
  8C1FC998  7FF0  add ##-16,r15
  8C1FC99A  6D43  mov r4,r13
  8C1FC99C  9424  mov.w @([8C1FC9E8]),r4
  8C1FC99E  9322  mov.w @([8C1FC9E6]),r3
  8C1FC9A0  6B53  mov r5,r11
  8C1FC9A2  34DC  add r13,r4
  8C1FC9A4  9C21  mov.w @([8C1FC9EA]),r12
  8C1FC9A6  343C  add r3,r4
  8C1FC9A8  6843  mov r4,r8
  8C1FC9AA  941F  mov.w @([8C1FC9EC]),r4
  8C1FC9AC  3CDC  add r13,r12
  8C1FC9AE  34CC  add r12,r4
  8C1FC9B0  65C3  mov r12,r5
  8C1FC9B2  6242  mov.l @r4,r2
  8C1FC9B4  7808  add ##8,r8
  8C1FC9B6  2228  tst r2,r2
  8C1FC9B8  8F02  bf.s 8C1FC9C0
  8C1FC9BA  7568  add ##104,r5
  8C1FC9BC  A006  bra 8C1FC9CC
  8C1FC9BE  EE00  mov ##0x00,r14
  ...
  8C1FC9CC  59C5  mov.l @(20,r12),r9
  8C1FC9CE  2998  tst r9,r9
  8C1FC9D0  8D12  bt.s 8C1FC9F8
  8C1FC9D2  5A59  mov.l @(36,r5),r10
  ...
  8C1FC9F8  60B3  mov r11,r0
  8C1FC9FA  8801  cmp/eq ##0x01,R0
  8C1FC9FC  8B03  bf 8C1FCA06
  8C1FC9FE  D342  mov.l @([8C1FCB08]),r3
  8C1FCA00  65E3  mov r14,r5
  8C1FCA02  430B  jsr @r3
  8C1FCA04  64C3  mov r12,r4
  8C1FCA06  60B3  mov r11,r0
  8C1FCA08  8801  cmp/eq ##0x01,R0
  8C1FCA0A  8902  bt 8C1FCA12
  ...
  8C1FCA12  D23E  mov.l @([8C1FCB0C]),r2
  8C1FCA14  65E3  mov r14,r5
  8C1FCA16  420B  jsr @r2
  8C1FCA18  64C3  mov r12,r4
  8C1FCA1A  6E03  mov r0,r14
  8C1FCA1C  D33C  mov.l @([8C1FCB10]),r3
  8C1FCA1E  430B  jsr @r3
  8C1FCA20  64D3  mov r13,r4
  8C1FCA22  926A  mov.w @([8C1FCAFA]),r2
  8C1FCA24  3027  cmp/gt r2,r0
  8C1FCA26  8904  bt 8C1FCA32
  8C1FCA28  9068  mov.w @([8C1FCAFC]),r0
  8C1FCA2A  538F  mov.l @(60,r8),r3
  8C1FCA2C  01DE  mov.l @(R0,r13),r1
  8C1FCA2E  3313  cmp/ge r1,r3
  8C1FCA30  891E  bt 8C1FCA70
  8C1FCA32  D338  mov.l @([8C1FCB14]),r3
  8C1FCA34  66F3  mov r15,r6
  8C1FCA36  65F3  mov r15,r5
  8C1FCA38  760C  add ##12,r6
  8C1FCA3A  430B  jsr @r3
  8C1FCA3C  64D3  mov r13,r4
  8C1FCA3E  62F2  mov.l @r15,r2
  8C1FCA40  4211  cmp/pz r2
  8C1FCA42  8B15  bf 8C1FCA70
  ...
  8C1FCA70  A004  bra 8C1FCA7C
  8C1FCA72  E000  mov ##0x00,r0
  ...
  8C1FCA7C  7F10  add ##16,r15
  8C1FCA7E  4F26  lds.l @r15+,PR
  8C1FCA80  68F6  mov.l @r15+,r8
  8C1FCA82  69F6  mov.l @r15+,r9
  8C1FCA84  6AF6  mov.l @r15+,r10
  8C1FCA86  6BF6  mov.l @r15+,r11
  8C1FCA88  6CF6  mov.l @r15+,r12
  8C1FCA8A  6DF6  mov.l @r15+,r13
  8C1FCA8C  000B  rts
  8C1FCA8E  6EF6  mov.l @r15+,r14
```

## Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) `8C161EC4`

Dump: `/mnt/1TB/dcbat_off/20261007-190751_Napple_Tale_-_Arsia_in_Daydream__Japan__/jit-6592.txt`

```
  8C161EC4  2FE6  mov.l r14,@-r15
  8C161EC6  2FD6  mov.l r13,@-r15
  8C161EC8  2FC6  mov.l r12,@-r15
  8C161ECA  2FB6  mov.l r11,@-r15
  8C161ECC  2FA6  mov.l r10,@-r15
  8C161ECE  2F96  mov.l r9,@-r15
  8C161ED0  2F86  mov.l r8,@-r15
  8C161ED2  4F22  sts.l PR,@-r15
  8C161ED4  7FF0  add ##-16,r15
  8C161ED6  6D43  mov r4,r13
  8C161ED8  9424  mov.w @([8C161F24]),r4
  8C161EDA  9322  mov.w @([8C161F22]),r3
  8C161EDC  6B53  mov r5,r11
  8C161EDE  34DC  add r13,r4
  8C161EE0  9C21  mov.w @([8C161F26]),r12
  8C161EE2  343C  add r3,r4
  8C161EE4  6843  mov r4,r8
  8C161EE6  941F  mov.w @([8C161F28]),r4
  8C161EE8  3CDC  add r13,r12
  8C161EEA  34CC  add r12,r4
  8C161EEC  65C3  mov r12,r5
  8C161EEE  6242  mov.l @r4,r2
  8C161EF0  7808  add ##8,r8
  8C161EF2  2228  tst r2,r2
  8C161EF4  8F02  bf.s 8C161EFC
  8C161EF6  7568  add ##104,r5
  8C161EF8  A006  bra 8C161F08
  8C161EFA  EE00  mov ##0x00,r14
  8C161EFC  9015  mov.w @([8C161F2A]),r0
  8C161EFE  5E58  mov.l @(32,r5),r14
  8C161F00  5248  mov.l @(32,r4),r2
  8C161F02  01CE  mov.l @(R0,r12),r1
  8C161F04  3E28  sub r2,r14
  8C161F06  3E1C  add r1,r14
  8C161F08  59C5  mov.l @(20,r12),r9
  8C161F0A  2998  tst r9,r9
  8C161F0C  8D12  bt.s 8C161F34
  8C161F0E  5A59  mov.l @(36,r5),r10
  ...
  8C161F34  60B3  mov r11,r0
  8C161F36  8801  cmp/eq ##0x01,R0
  8C161F38  8B03  bf 8C161F42
  8C161F3A  D342  mov.l @([8C162044]),r3
  8C161F3C  65E3  mov r14,r5
  8C161F3E  430B  jsr @r3
  8C161F40  64C3  mov r12,r4
  8C161F42  60B3  mov r11,r0
  8C161F44  8801  cmp/eq ##0x01,R0
  8C161F46  8902  bt 8C161F4E
  8C161F48  60B3  mov r11,r0
  8C161F4A  8802  cmp/eq ##0x02,R0
  8C161F4C  8B04  bf 8C161F58
  8C161F4E  D23E  mov.l @([8C162048]),r2
  8C161F50  65E3  mov r14,r5
  8C161F52  420B  jsr @r2
  8C161F54  64C3  mov r12,r4
  8C161F56  6E03  mov r0,r14
  8C161F58  D33C  mov.l @([8C16204C]),r3
  8C161F5A  430B  jsr @r3
  8C161F5C  64D3  mov r13,r4
  8C161F5E  926A  mov.w @([8C162036]),r2
  8C161F60  3027  cmp/gt r2,r0
  8C161F62  8904  bt 8C161F6E
  8C161F64  9068  mov.w @([8C162038]),r0
  8C161F66  538F  mov.l @(60,r8),r3
  8C161F68  01DE  mov.l @(R0,r13),r1
  8C161F6A  3313  cmp/ge r1,r3
  8C161F6C  891E  bt 8C161FAC
  8C161F6E  D338  mov.l @([8C162050]),r3
  8C161F70  66F3  mov r15,r6
  8C161F72  65F3  mov r15,r5
  8C161F74  760C  add ##12,r6
  8C161F76  430B  jsr @r3
  8C161F78  64D3  mov r13,r4
  8C161F7A  62F2  mov.l @r15,r2
  8C161F7C  4211  cmp/pz r2
  8C161F7E  8B15  bf 8C161FAC
  8C161F80  66F3  mov r15,r6
  8C161F82  67F3  mov r15,r7
  8C161F84  65B3  mov r11,r5
  8C161F86  7604  add ##4,r6
  8C161F88  7708  add ##8,r7
  8C161F8A  B01F  bsr 8C161FCC
  8C161F8C  64D3  mov r13,r4
  8C161F8E  51F1  mov.l @(4,r15),r1
  8C161F90  D330  mov.l @([8C162054]),r3
  8C161F92  0A17  mul.l r1,r10
  8C161F94  011A  sts MACL,r1
  8C161F96  430B  jsr @r3
  8C161F98  50F2  mov.l @(8,r15),r0
  8C161F9A  55F3  mov.l @(12,r15),r5
  8C161F9C  3E08  sub r0,r14
  8C161F9E  D32E  mov.l @([8C162058]),r3
  8C161FA0  67A3  mov r10,r7
  8C161FA2  66E3  mov r14,r6
  8C161FA4  430B  jsr @r3
  8C161FA6  64F2  mov.l @r15,r4
  8C161FA8  2008  tst r0,r0
  8C161FAA  8901  bt 8C161FB0
  8C161FAC  A004  bra 8C161FB8
  8C161FAE  E000  mov ##0x00,r0
  8C161FB0  538F  mov.l @(60,r8),r3
  8C161FB2  E001  mov ##0x01,r0
  8C161FB4  7301  add ##1,r3
  8C161FB6  183F  mov.l r3,@(60,r8)
  8C161FB8  7F10  add ##16,r15
  8C161FBA  4F26  lds.l @r15+,PR
  8C161FBC  68F6  mov.l @r15+,r8
  8C161FBE  69F6  mov.l @r15+,r9
  8C161FC0  6AF6  mov.l @r15+,r10
  8C161FC2  6BF6  mov.l @r15+,r11
  8C161FC4  6CF6  mov.l @r15+,r12
  8C161FC6  6DF6  mov.l @r15+,r13
  8C161FC8  000B  rts
  8C161FCA  6EF6  mov.l @r15+,r14
```
