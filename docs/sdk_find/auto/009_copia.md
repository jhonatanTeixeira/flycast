# 009_copia

> Gerado por `tools/sdk_find.py`. Jogos: 10 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 5.78% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C063CEC` | 91 | 5.42% |
| Evolution - The World of Sacred Device (USA) | `8C1663F8` | 91 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C17247E` | 91 | 0.00% |
| Grandia II (USA) | `8C03B8D6` | 91 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C31E056` | 91 | 0.00% |
| Macross M3 | `8C1C5CEE` | 91 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C108E22` | 91 | 0.36% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C38D456` | 91 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C17B172` | 91 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C25A686` | 91 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C063CEC`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C063CEC  2FE6  mov.l r14,@-r15
  8C063CEE  2FD6  mov.l r13,@-r15
  8C063CF0  2FC6  mov.l r12,@-r15
  8C063CF2  2FB6  mov.l r11,@-r15
  8C063CF4  EB00  mov ##0x00,r11
  8C063CF6  2FA6  mov.l r10,@-r15
  8C063CF8  2F96  mov.l r9,@-r15
  8C063CFA  4F22  sts.l PR,@-r15
  8C063CFC  DC47  mov.l @([8C063E1C]),r12
  8C063CFE  DD48  mov.l @([8C063E20]),r13
  8C063D00  4D0B  jsr @r13
  8C063D02  6EB3  mov r11,r14
  8C063D04  D247  mov.l @([8C063E24]),r2
  8C063D06  2C02  mov.l r0,@r12
  8C063D08  6322  mov.l @r2,r3
  8C063D0A  2338  tst r3,r3
  8C063D0C  8917  bt 8C063D3E
  8C063D0E  DA47  mov.l @([8C063E2C]),r10
  8C063D10  D945  mov.l @([8C063E28]),r9
  8C063D12  A008  bra 8C063D26
  8C063D14  0009  nop
  8C063D16  4D0B  jsr @r13
  8C063D18  0009  nop
  8C063D1A  6503  mov r0,r5
  8C063D1C  4A0B  jsr @r10
  8C063D1E  64C2  mov.l @r12,r4
  8C063D20  490B  jsr @r9
  8C063D22  6403  mov r0,r4
  8C063D24  6E03  mov r0,r14
  8C063D26  D342  mov.l @([8C063E30]),r3
  8C063D28  6232  mov.l @r3,r2
  8C063D2A  2228  tst r2,r2
  8C063D2C  8902  bt 8C063D34
  8C063D2E  D241  mov.l @([8C063E34]),r2
  8C063D30  3E22  cmp/hs r2,r14
  8C063D32  8BF0  bf 8C063D16
  8C063D34  D33F  mov.l @([8C063E34]),r3
  8C063D36  3E32  cmp/hs r3,r14
  8C063D38  8901  bt 8C063D3E
  8C063D3A  BFCC  bsr 8C063CD6
  8C063D3C  0009  nop
  8C063D3E  D33E  mov.l @([8C063E38]),r3
  8C063D40  E201  mov ##0x01,r2
  8C063D42  D13B  mov.l @([8C063E30]),r1
  8C063D44  430B  jsr @r3
  8C063D46  2122  mov.l r2,@r1
  8C063D48  D43F  mov.l @([8C063E48]),r4
  8C063D4A  D23E  mov.l @([8C063E44]),r2
  8C063D4C  D03B  mov.l @([8C063E3C]),r0
  8C063D4E  D33C  mov.l @([8C063E40]),r3
  8C063D50  20B2  mov.l r11,@r0
  8C063D52  23B2  mov.l r11,@r3
  8C063D54  22B2  mov.l r11,@r2
  8C063D56  6142  mov.l @r4,r1
  8C063D58  7101  add ##1,r1
  8C063D5A  2412  mov.l r1,@r4
  8C063D5C  D13B  mov.l @([8C063E4C]),r1
  8C063D5E  410B  jsr @r1
  8C063D60  0009  nop
  8C063D62  D33B  mov.l @([8C063E50]),r3
  8C063D64  430B  jsr @r3
  8C063D66  0009  nop
  8C063D68  4D0B  jsr @r13
  8C063D6A  0009  nop
  8C063D6C  D33A  mov.l @([8C063E58]),r3
  8C063D6E  D239  mov.l @([8C063E54]),r2
  8C063D70  D13A  mov.l @([8C063E5C]),r1
  8C063D72  2202  mov.l r0,@r2
  8C063D74  6432  mov.l @r3,r4
  8C063D76  6242  mov.l @r4,r2
  8C063D78  2122  mov.l r2,@r1
  8C063D7A  D239  mov.l @([8C063E60]),r2
  8C063D7C  5041  mov.l @(4,r4),r0
  8C063D7E  2202  mov.l r0,@r2
  8C063D80  5342  mov.l @(8,r4),r3
  8C063D82  D038  mov.l @([8C063E64]),r0
  8C063D84  2032  mov.l r3,@r0
  8C063D86  D338  mov.l @([8C063E68]),r3
  8C063D88  5143  mov.l @(12,r4),r1
  8C063D8A  2312  mov.l r1,@r3
  8C063D8C  5244  mov.l @(16,r4),r2
  8C063D8E  D137  mov.l @([8C063E6C]),r1
  8C063D90  2122  mov.l r2,@r1
  8C063D92  4F26  lds.l @r15+,PR
  8C063D94  69F6  mov.l @r15+,r9
  8C063D96  6AF6  mov.l @r15+,r10
  8C063D98  6BF6  mov.l @r15+,r11
  8C063D9A  6CF6  mov.l @r15+,r12
  8C063D9C  6DF6  mov.l @r15+,r13
  8C063D9E  000B  rts
  8C063DA0  6EF6  mov.l @r15+,r14
```
