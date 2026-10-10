# 059_geral

> Gerado por `tools/sdk_find.py`. Jogos: 6 · variantes (sequências normalizadas distintas): 4 · tempo perf somado: 0.67% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Grandia II (USA) | `8C06EA0C` | 103 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C33C08C` | 113 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C05FEDC` | 9 | 0.00% |
| Macross M3 | `8C1FC158` | 103 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C161694` | 123 | 0.67% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C36F13C` | 113 | 0.00% |

## Grandia II (USA) `8C06EA0C`

Dump: `/mnt/1TB/dcbat/20261002-192949_Grandia_II__USA__/jit-534412.txt`

```
  8C06EA0C  2FE6  mov.l r14,@-r15
  8C06EA0E  2FD6  mov.l r13,@-r15
  8C06EA10  2FC6  mov.l r12,@-r15
  8C06EA12  2FB6  mov.l r11,@-r15
  8C06EA14  2FA6  mov.l r10,@-r15
  8C06EA16  2F96  mov.l r9,@-r15
  8C06EA18  4F22  sts.l PR,@-r15
  8C06EA1A  7FF4  add ##-12,r15
  8C06EA1C  6E43  mov r4,r14
  8C06EA1E  E400  mov ##0x00,r4
  8C06EA20  6C73  mov r7,r12
  8C06EA22  6D43  mov r4,r13
  8C06EA24  6A63  mov r6,r10
  8C06EA26  2F52  mov.l r5,@r15
  8C06EA28  2C42  mov.l r4,@r12
  8C06EA2A  65A3  mov r10,r5
  8C06EA2C  53FA  mov.l @(40,r15),r3
  8C06EA2E  944E  mov.w @([8C06EACE]),r4
  8C06EA30  23D2  mov.l r13,@r3
  8C06EA32  934B  mov.w @([8C06EACC]),r3
  8C06EA34  34EC  add r14,r4
  8C06EA36  62F2  mov.l @r15,r2
  8C06EA38  343C  add r3,r4
  8C06EA3A  6943  mov r4,r9
  8C06EA3C  7908  add ##8,r9
  8C06EA3E  1F21  mov.l r2,@(4,r15)
  8C06EA40  1FA2  mov.l r10,@(8,r15)
  8C06EA42  569C  mov.l @(48,r9),r6
  8C06EA44  B07D  bsr 8C06EB42
  8C06EA46  64F2  mov.l @r15,r4
  8C06EA48  9343  mov.w @([8C06EAD2]),r3
  8C06EA4A  6B03  mov r0,r11
  8C06EA4C  23B8  tst r11,r3
  8C06EA4E  8903  bt 8C06EA58
  8C06EA50  D322  mov.l @([8C06EADC]),r3
  8C06EA52  E501  mov ##0x01,r5
  8C06EA54  430B  jsr @r3
  8C06EA56  64E3  mov r14,r4
  8C06EA58  923C  mov.w @([8C06EAD4]),r2
  8C06EA5A  3B20  cmp/eq r2,r11
  8C06EA5C  8B0B  bf 8C06EA76
  ...
  8C06EA76  65A3  mov r10,r5
  8C06EA78  66B3  mov r11,r6
  8C06EA7A  B10E  bsr 8C06EC9A
  8C06EA7C  64E3  mov r14,r4
  8C06EA7E  2008  tst r0,r0
  8C06EA80  8904  bt 8C06EA8C
  ...
  8C06EA8C  E304  mov ##0x04,r3
  8C06EA8E  3A37  cmp/gt r3,r10
  8C06EA90  8B4D  bf 8C06EB2E
  8C06EA92  E24C  mov ##0x4C,r2
  8C06EA94  22B8  tst r11,r2
  8C06EA96  890A  bt 8C06EAAE
  8C06EA98  65F3  mov r15,r5
  8C06EA9A  7504  add ##4,r5
  8C06EA9C  66C3  mov r12,r6
  8C06EA9E  B11A  bsr 8C06ECD6
  8C06EAA0  64E3  mov r14,r4
  8C06EAA2  6D03  mov r0,r13
  8C06EAA4  2DD8  tst r13,r13
  8C06EAA6  8B42  bf 8C06EB2E
  8C06EAA8  9215  mov.w @([8C06EAD6]),r2
  8C06EAAA  A03D  bra 8C06EB28
  8C06EAAC  192C  mov.l r2,@(48,r9)
  8C06EAAE  E102  mov ##0x02,r1
  8C06EAB0  21B8  tst r11,r1
  8C06EAB2  8932  bt 8C06EB1A
  8C06EAB4  D10C  mov.l @([8C06EAE8]),r1
  8C06EAB6  E52F  mov ##0x2F,r5
  8C06EAB8  410B  jsr @r1
  8C06EABA  64E3  mov r14,r4
  8C06EABC  8801  cmp/eq ##0x01,R0
  8C06EABE  8903  bt 8C06EAC8
  8C06EAC0  B2A1  bsr 8C06F006
  8C06EAC2  64E3  mov r14,r4
  8C06EAC4  2008  tst r0,r0
  8C06EAC6  8B11  bf 8C06EAEC
  ...
  8C06EAEC  65F3  mov r15,r5
  8C06EAEE  7504  add ##4,r5
  8C06EAF0  B290  bsr 8C06F014
  8C06EAF2  64E3  mov r14,r4
  8C06EAF4  2008  tst r0,r0
  8C06EAF6  8909  bt 8C06EB0C
  ...
  8C06EB0C  65F3  mov r15,r5
  8C06EB0E  7504  add ##4,r5
  8C06EB10  66C3  mov r12,r6
  8C06EB12  B47C  bsr 8C06F40E
  8C06EB14  64E3  mov r14,r4
  8C06EB16  A00A  bra 8C06EB2E
  8C06EB18  6D03  mov r0,r13
  ...
  8C06EB28  53FA  mov.l @(40,r15),r3
  8C06EB2A  E201  mov ##0x01,r2
  8C06EB2C  2322  mov.l r2,@r3
  8C06EB2E  60D3  mov r13,r0
  8C06EB30  7F0C  add ##12,r15
  8C06EB32  4F26  lds.l @r15+,PR
  8C06EB34  69F6  mov.l @r15+,r9
  8C06EB36  6AF6  mov.l @r15+,r10
  8C06EB38  6BF6  mov.l @r15+,r11
  8C06EB3A  6CF6  mov.l @r15+,r12
  8C06EB3C  6DF6  mov.l @r15+,r13
  8C06EB3E  000B  rts
  8C06EB40  6EF6  mov.l @r15+,r14
```

## King of Fighters The - Evolution (USA) (EnJaEsPt) `8C33C08C`

Dump: `/mnt/1TB/dcbat/20261007-164133_King_of_Fighters_The_-_Evolution__USA___/jit-113430.txt`

```
  8C33C08C  2FE6  mov.l r14,@-r15
  8C33C08E  2FD6  mov.l r13,@-r15
  8C33C090  2FC6  mov.l r12,@-r15
  8C33C092  2FB6  mov.l r11,@-r15
  8C33C094  2FA6  mov.l r10,@-r15
  8C33C096  2F96  mov.l r9,@-r15
  8C33C098  4F22  sts.l PR,@-r15
  8C33C09A  7FF4  add ##-12,r15
  8C33C09C  6E43  mov r4,r14
  8C33C09E  E400  mov ##0x00,r4
  8C33C0A0  6C73  mov r7,r12
  8C33C0A2  6D43  mov r4,r13
  8C33C0A4  6A63  mov r6,r10
  8C33C0A6  2F52  mov.l r5,@r15
  8C33C0A8  2C42  mov.l r4,@r12
  8C33C0AA  65A3  mov r10,r5
  8C33C0AC  53FA  mov.l @(40,r15),r3
  8C33C0AE  944E  mov.w @([8C33C14E]),r4
  8C33C0B0  23D2  mov.l r13,@r3
  8C33C0B2  934B  mov.w @([8C33C14C]),r3
  8C33C0B4  34EC  add r14,r4
  8C33C0B6  62F2  mov.l @r15,r2
  8C33C0B8  343C  add r3,r4
  8C33C0BA  6943  mov r4,r9
  8C33C0BC  7908  add ##8,r9
  8C33C0BE  1F21  mov.l r2,@(4,r15)
  8C33C0C0  1FA2  mov.l r10,@(8,r15)
  8C33C0C2  569C  mov.l @(48,r9),r6
  8C33C0C4  B07D  bsr 8C33C1C2
  8C33C0C6  64F2  mov.l @r15,r4
  8C33C0C8  9343  mov.w @([8C33C152]),r3
  8C33C0CA  6B03  mov r0,r11
  8C33C0CC  23B8  tst r11,r3
  8C33C0CE  8903  bt 8C33C0D8
  8C33C0D0  D322  mov.l @([8C33C15C]),r3
  8C33C0D2  E501  mov ##0x01,r5
  8C33C0D4  430B  jsr @r3
  8C33C0D6  64E3  mov r14,r4
  8C33C0D8  923C  mov.w @([8C33C154]),r2
  8C33C0DA  3B20  cmp/eq r2,r11
  8C33C0DC  8B0B  bf 8C33C0F6
  ...
  8C33C0F6  65A3  mov r10,r5
  8C33C0F8  66B3  mov r11,r6
  8C33C0FA  B10E  bsr 8C33C31A
  8C33C0FC  64E3  mov r14,r4
  8C33C0FE  2008  tst r0,r0
  8C33C100  8904  bt 8C33C10C
  ...
  8C33C10C  E304  mov ##0x04,r3
  8C33C10E  3A37  cmp/gt r3,r10
  8C33C110  8B4D  bf 8C33C1AE
  8C33C112  E24C  mov ##0x4C,r2
  8C33C114  22B8  tst r11,r2
  8C33C116  890A  bt 8C33C12E
  8C33C118  65F3  mov r15,r5
  8C33C11A  7504  add ##4,r5
  8C33C11C  66C3  mov r12,r6
  8C33C11E  B11A  bsr 8C33C356
  8C33C120  64E3  mov r14,r4
  8C33C122  6D03  mov r0,r13
  8C33C124  2DD8  tst r13,r13
  8C33C126  8B42  bf 8C33C1AE
  8C33C128  9215  mov.w @([8C33C156]),r2
  8C33C12A  A03D  bra 8C33C1A8
  8C33C12C  192C  mov.l r2,@(48,r9)
  8C33C12E  E102  mov ##0x02,r1
  8C33C130  21B8  tst r11,r1
  8C33C132  8932  bt 8C33C19A
  8C33C134  D10C  mov.l @([8C33C168]),r1
  8C33C136  E52F  mov ##0x2F,r5
  8C33C138  410B  jsr @r1
  8C33C13A  64E3  mov r14,r4
  8C33C13C  8801  cmp/eq ##0x01,R0
  8C33C13E  8903  bt 8C33C148
  8C33C140  B2A1  bsr 8C33C686
  8C33C142  64E3  mov r14,r4
  8C33C144  2008  tst r0,r0
  8C33C146  8B11  bf 8C33C16C
  ...
  8C33C16C  65F3  mov r15,r5
  8C33C16E  7504  add ##4,r5
  8C33C170  B290  bsr 8C33C694
  8C33C172  64E3  mov r14,r4
  8C33C174  2008  tst r0,r0
  8C33C176  8909  bt 8C33C18C
  8C33C178  65F3  mov r15,r5
  8C33C17A  7504  add ##4,r5
  8C33C17C  66C3  mov r12,r6
  8C33C17E  B42A  bsr 8C33C9D6
  8C33C180  64E3  mov r14,r4
  8C33C182  6D03  mov r0,r13
  8C33C184  2DD8  tst r13,r13
  8C33C186  8B12  bf 8C33C1AE
  8C33C188  A00E  bra 8C33C1A8
  8C33C18A  0009  nop
  8C33C18C  65F3  mov r15,r5
  8C33C18E  7504  add ##4,r5
  8C33C190  66C3  mov r12,r6
  8C33C192  B47C  bsr 8C33CA8E
  8C33C194  64E3  mov r14,r4
  8C33C196  A00A  bra 8C33C1AE
  8C33C198  6D03  mov r0,r13
  ...
  8C33C1A8  53FA  mov.l @(40,r15),r3
  8C33C1AA  E201  mov ##0x01,r2
  8C33C1AC  2322  mov.l r2,@r3
  8C33C1AE  60D3  mov r13,r0
  8C33C1B0  7F0C  add ##12,r15
  8C33C1B2  4F26  lds.l @r15+,PR
  8C33C1B4  69F6  mov.l @r15+,r9
  8C33C1B6  6AF6  mov.l @r15+,r10
  8C33C1B8  6BF6  mov.l @r15+,r11
  8C33C1BA  6CF6  mov.l @r15+,r12
  8C33C1BC  6DF6  mov.l @r15+,r13
  8C33C1BE  000B  rts
  8C33C1C0  6EF6  mov.l @r15+,r14
```

## Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) `8C05FEDC`

Dump: `/mnt/1TB/dcbat/20261007-163759_Le_Mans_24_Hours__Europe___En_Fr_De_Es_I/jit-109461.txt`

```
  8C05FEDC  2008  tst r0,r0
  8C05FEDE  8909  bt 8C05FEF4
  ...
  8C05FEF4  65F3  mov r15,r5
  8C05FEF6  7504  add ##4,r5
  8C05FEF8  66C3  mov r12,r6
  8C05FEFA  B47C  bsr 8C0607F6
  8C05FEFC  64E3  mov r14,r4
  8C05FEFE  A00A  bra 8C05FF16
  8C05FF00  6D03  mov r0,r13
```

## Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) `8C161694`

Dump: `/mnt/1TB/dcbat_off/20261007-190751_Napple_Tale_-_Arsia_in_Daydream__Japan__/jit-6592.txt`

```
  8C161694  2FE6  mov.l r14,@-r15
  8C161696  2FD6  mov.l r13,@-r15
  8C161698  2FC6  mov.l r12,@-r15
  8C16169A  2FB6  mov.l r11,@-r15
  8C16169C  2FA6  mov.l r10,@-r15
  8C16169E  2F96  mov.l r9,@-r15
  8C1616A0  4F22  sts.l PR,@-r15
  8C1616A2  7FF4  add ##-12,r15
  8C1616A4  6E43  mov r4,r14
  8C1616A6  E400  mov ##0x00,r4
  8C1616A8  6C73  mov r7,r12
  8C1616AA  6D43  mov r4,r13
  8C1616AC  6A63  mov r6,r10
  8C1616AE  2F52  mov.l r5,@r15
  8C1616B0  2C42  mov.l r4,@r12
  8C1616B2  65A3  mov r10,r5
  8C1616B4  53FA  mov.l @(40,r15),r3
  8C1616B6  944E  mov.w @([8C161756]),r4
  8C1616B8  23D2  mov.l r13,@r3
  8C1616BA  934B  mov.w @([8C161754]),r3
  8C1616BC  34EC  add r14,r4
  8C1616BE  62F2  mov.l @r15,r2
  8C1616C0  343C  add r3,r4
  8C1616C2  6943  mov r4,r9
  8C1616C4  7908  add ##8,r9
  8C1616C6  1F21  mov.l r2,@(4,r15)
  8C1616C8  1FA2  mov.l r10,@(8,r15)
  8C1616CA  569C  mov.l @(48,r9),r6
  8C1616CC  B07D  bsr 8C1617CA
  8C1616CE  64F2  mov.l @r15,r4
  8C1616D0  9343  mov.w @([8C16175A]),r3
  8C1616D2  6B03  mov r0,r11
  8C1616D4  23B8  tst r11,r3
  8C1616D6  8903  bt 8C1616E0
  8C1616D8  D322  mov.l @([8C161764]),r3
  8C1616DA  E501  mov ##0x01,r5
  8C1616DC  430B  jsr @r3
  8C1616DE  64E3  mov r14,r4
  8C1616E0  923C  mov.w @([8C16175C]),r2
  8C1616E2  3B20  cmp/eq r2,r11
  8C1616E4  8B0B  bf 8C1616FE
  8C1616E6  D120  mov.l @([8C161768]),r1
  8C1616E8  410B  jsr @r1
  8C1616EA  64E3  mov r14,r4
  8C1616EC  2008  tst r0,r0
  8C1616EE  8906  bt 8C1616FE
  ...
  8C1616FE  65A3  mov r10,r5
  8C161700  66B3  mov r11,r6
  8C161702  B10E  bsr 8C161922
  8C161704  64E3  mov r14,r4
  8C161706  2008  tst r0,r0
  8C161708  8904  bt 8C161714
  8C16170A  D318  mov.l @([8C16176C]),r3
  8C16170C  430B  jsr @r3
  8C16170E  64E3  mov r14,r4
  8C161710  A051  bra 8C1617B6
  8C161712  0009  nop
  8C161714  E304  mov ##0x04,r3
  8C161716  3A37  cmp/gt r3,r10
  8C161718  8B4D  bf 8C1617B6
  8C16171A  E24C  mov ##0x4C,r2
  8C16171C  22B8  tst r11,r2
  8C16171E  890A  bt 8C161736
  8C161720  65F3  mov r15,r5
  8C161722  7504  add ##4,r5
  8C161724  66C3  mov r12,r6
  8C161726  B11A  bsr 8C16195E
  8C161728  64E3  mov r14,r4
  8C16172A  6D03  mov r0,r13
  8C16172C  2DD8  tst r13,r13
  8C16172E  8B42  bf 8C1617B6
  8C161730  9215  mov.w @([8C16175E]),r2
  8C161732  A03D  bra 8C1617B0
  8C161734  192C  mov.l r2,@(48,r9)
  8C161736  E102  mov ##0x02,r1
  8C161738  21B8  tst r11,r1
  8C16173A  8932  bt 8C1617A2
  8C16173C  D10C  mov.l @([8C161770]),r1
  8C16173E  E52F  mov ##0x2F,r5
  8C161740  410B  jsr @r1
  8C161742  64E3  mov r14,r4
  8C161744  8801  cmp/eq ##0x01,R0
  8C161746  8903  bt 8C161750
  8C161748  B2A1  bsr 8C161C8E
  8C16174A  64E3  mov r14,r4
  8C16174C  2008  tst r0,r0
  8C16174E  8B11  bf 8C161774
  ...
  8C161774  65F3  mov r15,r5
  8C161776  7504  add ##4,r5
  8C161778  B290  bsr 8C161C9C
  8C16177A  64E3  mov r14,r4
  8C16177C  2008  tst r0,r0
  8C16177E  8909  bt 8C161794
  8C161780  65F3  mov r15,r5
  8C161782  7504  add ##4,r5
  8C161784  66C3  mov r12,r6
  8C161786  B42A  bsr 8C161FDE
  8C161788  64E3  mov r14,r4
  8C16178A  6D03  mov r0,r13
  8C16178C  2DD8  tst r13,r13
  8C16178E  8B12  bf 8C1617B6
  8C161790  A00E  bra 8C1617B0
  8C161792  0009  nop
  8C161794  65F3  mov r15,r5
  8C161796  7504  add ##4,r5
  8C161798  66C3  mov r12,r6
  8C16179A  B47C  bsr 8C162096
  8C16179C  64E3  mov r14,r4
  8C16179E  A00A  bra 8C1617B6
  8C1617A0  6D03  mov r0,r13
  ...
  8C1617B0  53FA  mov.l @(40,r15),r3
  8C1617B2  E201  mov ##0x01,r2
  8C1617B4  2322  mov.l r2,@r3
  8C1617B6  60D3  mov r13,r0
  8C1617B8  7F0C  add ##12,r15
  8C1617BA  4F26  lds.l @r15+,PR
  8C1617BC  69F6  mov.l @r15+,r9
  8C1617BE  6AF6  mov.l @r15+,r10
  8C1617C0  6BF6  mov.l @r15+,r11
  8C1617C2  6CF6  mov.l @r15+,r12
  8C1617C4  6DF6  mov.l @r15+,r13
  8C1617C6  000B  rts
  8C1617C8  6EF6  mov.l @r15+,r14
```
