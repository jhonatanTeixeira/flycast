# 023_geral

> Gerado por `tools/sdk_find.py`. Jogos: 8 · variantes (sequências normalizadas distintas): 2 · tempo perf somado: 1.79% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C073A00` | 61 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C1AC094` | 61 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1AC248` | 61 | 0.00% |
| Grandia II (USA) | `8C0700A4` | 61 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C33D724` | 61 | 0.00% |
| Macross M3 | `8C1FD8C4` | 49 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C162D2C` | 61 | 1.79% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C3708A8` | 61 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C073A00`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C073A00  2FE6  mov.l r14,@-r15
  8C073A02  E040  mov ##0x40,r0
  8C073A04  6E43  mov r4,r14
  8C073A06  4F22  sts.l PR,@-r15
  8C073A08  00EE  mov.l @(R0,r14),r0
  8C073A0A  8801  cmp/eq ##0x01,R0
  8C073A0C  8D09  bt.s 8C073A22
  8C073A0E  6403  mov r0,r4
  8C073A10  6043  mov r4,r0
  8C073A12  8802  cmp/eq ##0x02,R0
  8C073A14  890E  bt 8C073A34
  8C073A16  6043  mov r4,r0
  8C073A18  8803  cmp/eq ##0x03,R0
  8C073A1A  890B  bt 8C073A34
  8C073A1C  6043  mov r4,r0
  8C073A1E  8804  cmp/eq ##0x04,R0
  8C073A20  8B3F  bf 8C073AA2
  8C073A22  6043  mov r4,r0
  8C073A24  8802  cmp/eq ##0x02,R0
  8C073A26  8905  bt 8C073A34
  8C073A28  6043  mov r4,r0
  8C073A2A  8803  cmp/eq ##0x03,R0
  8C073A2C  8902  bt 8C073A34
  8C073A2E  6043  mov r4,r0
  8C073A30  8804  cmp/eq ##0x04,R0
  8C073A32  8B01  bf 8C073A38
  8C073A34  B038  bsr 8C073AA8
  8C073A36  64E3  mov r14,r4
  8C073A38  E040  mov ##0x40,r0
  8C073A3A  00EE  mov.l @(R0,r14),r0
  8C073A3C  8801  cmp/eq ##0x01,R0
  8C073A3E  8D0C  bt.s 8C073A5A
  8C073A40  6403  mov r0,r4
  8C073A42  8802  cmp/eq ##0x02,R0
  8C073A44  890D  bt 8C073A62
  8C073A46  8803  cmp/eq ##0x03,R0
  8C073A48  891E  bt 8C073A88
  8C073A4A  8804  cmp/eq ##0x04,R0
  8C073A4C  8920  bt 8C073A90
  ...
  8C073A5A  B028  bsr 8C073AAE
  8C073A5C  64E3  mov r14,r4
  8C073A5E  A01D  bra 8C073A9C
  8C073A60  0009  nop
  8C073A62  B036  bsr 8C073AD2
  8C073A64  64E3  mov r14,r4
  8C073A66  A019  bra 8C073A9C
  8C073A68  0009  nop
  ...
  8C073A88  B0DA  bsr 8C073C40
  8C073A8A  64E3  mov r14,r4
  8C073A8C  A006  bra 8C073A9C
  8C073A8E  0009  nop
  8C073A90  B0FE  bsr 8C073C90
  8C073A92  64E3  mov r14,r4
  8C073A94  A002  bra 8C073A9C
  8C073A96  0009  nop
  ...
  8C073A9C  6403  mov r0,r4
  8C073A9E  E040  mov ##0x40,r0
  8C073AA0  0E46  mov.l r4,@(R0,r14)
  8C073AA2  4F26  lds.l @r15+,PR
  8C073AA4  000B  rts
  8C073AA6  6EF6  mov.l @r15+,r14
```

## Macross M3 `8C1FD8C4`

Dump: `/mnt/1TB/dcbat/20261006-081557_Macross_M3_/jit-5965.txt`

```
  8C1FD8C4  2FE6  mov.l r14,@-r15
  8C1FD8C6  E040  mov ##0x40,r0
  8C1FD8C8  6E43  mov r4,r14
  8C1FD8CA  4F22  sts.l PR,@-r15
  8C1FD8CC  00EE  mov.l @(R0,r14),r0
  8C1FD8CE  8801  cmp/eq ##0x01,R0
  8C1FD8D0  8D09  bt.s 8C1FD8E6
  8C1FD8D2  6403  mov r0,r4
  8C1FD8D4  6043  mov r4,r0
  8C1FD8D6  8802  cmp/eq ##0x02,R0
  8C1FD8D8  890E  bt 8C1FD8F8
  8C1FD8DA  6043  mov r4,r0
  8C1FD8DC  8803  cmp/eq ##0x03,R0
  8C1FD8DE  890B  bt 8C1FD8F8
  8C1FD8E0  6043  mov r4,r0
  8C1FD8E2  8804  cmp/eq ##0x04,R0
  8C1FD8E4  8B3F  bf 8C1FD966
  8C1FD8E6  6043  mov r4,r0
  8C1FD8E8  8802  cmp/eq ##0x02,R0
  8C1FD8EA  8905  bt 8C1FD8F8
  8C1FD8EC  6043  mov r4,r0
  8C1FD8EE  8803  cmp/eq ##0x03,R0
  8C1FD8F0  8902  bt 8C1FD8F8
  8C1FD8F2  6043  mov r4,r0
  8C1FD8F4  8804  cmp/eq ##0x04,R0
  8C1FD8F6  8B01  bf 8C1FD8FC
  8C1FD8F8  B038  bsr 8C1FD96C
  8C1FD8FA  64E3  mov r14,r4
  8C1FD8FC  E040  mov ##0x40,r0
  8C1FD8FE  00EE  mov.l @(R0,r14),r0
  8C1FD900  8801  cmp/eq ##0x01,R0
  8C1FD902  8D0C  bt.s 8C1FD91E
  8C1FD904  6403  mov r0,r4
  8C1FD906  8802  cmp/eq ##0x02,R0
  8C1FD908  890D  bt 8C1FD926
  ...
  8C1FD91E  B031  bsr 8C1FD984
  8C1FD920  64E3  mov r14,r4
  8C1FD922  A01D  bra 8C1FD960
  8C1FD924  0009  nop
  8C1FD926  B03F  bsr 8C1FD9A8
  8C1FD928  64E3  mov r14,r4
  8C1FD92A  A019  bra 8C1FD960
  8C1FD92C  0009  nop
  ...
  8C1FD960  6403  mov r0,r4
  8C1FD962  E040  mov ##0x40,r0
  8C1FD964  0E46  mov.l r4,@(R0,r14)
  8C1FD966  4F26  lds.l @r15+,PR
  8C1FD968  000B  rts
  8C1FD96A  6EF6  mov.l @r15+,r14
```
