# 042_copia

> Gerado por `tools/sdk_find.py`. Jogos: 8 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 1.02% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C0739C4` | 30 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C1AC058` | 30 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1AC20C` | 30 | 0.00% |
| Grandia II (USA) | `8C070068` | 30 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C33D6E8` | 30 | 0.00% |
| Macross M3 | `8C1FD888` | 30 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C162CF0` | 30 | 1.02% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C37086C` | 30 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C0739C4`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C0739C4  2FE6  mov.l r14,@-r15
  8C0739C6  EE00  mov ##0x00,r14
  8C0739C8  D52E  mov.l @([8C073A84]),r5
  8C0739CA  904E  mov.w @([8C073A6A]),r0
  8C0739CC  2FD6  mov.l r13,@-r15
  8C0739CE  ED00  mov ##0x00,r13
  8C0739D0  2FC6  mov.l r12,@-r15
  8C0739D2  2FB6  mov.l r11,@-r15
  8C0739D4  4F22  sts.l PR,@-r15
  8C0739D6  0C5E  mov.l @(R0,r5),r12
  8C0739D8  7004  add ##4,r0
  8C0739DA  9B47  mov.w @([8C073A6C]),r11
  8C0739DC  045E  mov.l @(R0,r5),r4
  8C0739DE  4C15  cmp/pl r12
  8C0739E0  8F06  bf.s 8C0739F0
  8C0739E2  3E4C  add r4,r14
  8C0739E4  B00C  bsr 8C073A00
  8C0739E6  64E3  mov r14,r4
  8C0739E8  7D01  add ##1,r13
  8C0739EA  3DC3  cmp/ge r12,r13
  8C0739EC  8FFA  bf.s 8C0739E4
  8C0739EE  3EBC  add r11,r14
  8C0739F0  B20E  bsr 8C073E10
  8C0739F2  0009  nop
  8C0739F4  4F26  lds.l @r15+,PR
  8C0739F6  6BF6  mov.l @r15+,r11
  8C0739F8  6CF6  mov.l @r15+,r12
  8C0739FA  6DF6  mov.l @r15+,r13
  8C0739FC  000B  rts
  8C0739FE  6EF6  mov.l @r15+,r14
```
