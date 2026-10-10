# 017_copia

> Gerado por `tools/sdk_find.py`. Jogos: 10 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 2.24% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C06CB04` | 24 | 0.00% |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C06CBB8` | 24 | 0.00% |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C06CC7A` | 24 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C1ADC98` | 24 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C1ADD4A` | 24 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C1ADE0C` | 24 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1A653C` | 24 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1A65EE` | 24 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1A66B0` | 24 | 0.00% |
| Grandia II (USA) | `8C075A84` | 24 | 0.00% |
| Grandia II (USA) | `8C075B36` | 24 | 0.00% |
| Grandia II (USA) | `8C075BF8` | 24 | 0.00% |
| Grandia II (USA) | `8C075CFC` | 24 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C335364` | 24 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C335416` | 24 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C3355DC` | 24 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C067538` | 24 | 0.00% |
| Macross M3 | `8C1E8180` | 24 | 0.00% |
| Macross M3 | `8C1E8232` | 24 | 0.00% |
| Macross M3 | `8C1E82F4` | 24 | 0.00% |
| Macross M3 | `8C1E83F8` | 24 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C13662C` | 24 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C1366DE` | 24 | 0.01% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C1367A0` | 24 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C1368A4` | 24 | 2.23% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C36273C` | 24 | 0.00% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C3627EE` | 24 | 0.00% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C3628B0` | 24 | 0.00% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C3629B4` | 24 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C1AAD36` | 24 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C06CB04`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C06CB04  2FE6  mov.l r14,@-r15
  8C06CB06  2FD6  mov.l r13,@-r15
  8C06CB08  2FC6  mov.l r12,@-r15
  8C06CB0A  D430  mov.l @([8C06CBCC]),r4
  8C06CB0C  4F22  sts.l PR,@-r15
  8C06CB0E  9C5C  mov.w @([8C06CBCA]),r12
  8C06CB10  6E43  mov r4,r14
  8C06CB12  A008  bra 8C06CB26
  8C06CB14  3C4C  add r4,r12
  8C06CB16  6DE3  mov r14,r13
  8C06CB18  62D2  mov.l @r13,r2
  8C06CB1A  2228  tst r2,r2
  8C06CB1C  8902  bt 8C06CB24
  8C06CB1E  62D2  mov.l @r13,r2
  8C06CB20  420B  jsr @r2
  8C06CB22  54D1  mov.l @(4,r13),r4
  8C06CB24  7E08  add ##8,r14
  8C06CB26  3EC2  cmp/hs r12,r14
  8C06CB28  8BF5  bf 8C06CB16
  8C06CB2A  4F26  lds.l @r15+,PR
  8C06CB2C  6CF6  mov.l @r15+,r12
  8C06CB2E  6DF6  mov.l @r15+,r13
  8C06CB30  000B  rts
  8C06CB32  6EF6  mov.l @r15+,r14
```
