# 039_copia

> Gerado por `tools/sdk_find.py`. Jogos: 9 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 1.05% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C08F1DE` | 47 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C1D932A` | 47 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1D88B6` | 47 | 0.00% |
| Grandia II (USA) | `8C0C37CE` | 47 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C36B752` | 47 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C06390A` | 47 | 0.00% |
| Macross M3 | `8C1FFC6E` | 47 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C180DDE` | 47 | 1.05% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C372C52` | 47 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C08F1DE`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C08F1DE  2FE6  mov.l r14,@-r15
  8C08F1E0  E700  mov ##0x00,r7
  8C08F1E2  2FD6  mov.l r13,@-r15
  8C08F1E4  6D73  mov r7,r13
  8C08F1E6  2FC6  mov.l r12,@-r15
  8C08F1E8  6C43  mov r4,r12
  8C08F1EA  2FB6  mov.l r11,@-r15
  8C08F1EC  EB09  mov ##0x09,r11
  8C08F1EE  2FA6  mov.l r10,@-r15
  8C08F1F0  6473  mov r7,r4
  8C08F1F2  9673  mov.w @([8C08F2DC]),r6
  8C08F1F4  2F96  mov.l r9,@-r15
  8C08F1F6  6953  mov r5,r9
  8C08F1F8  4F22  sts.l PR,@-r15
  8C08F1FA  36CC  add r12,r6
  8C08F1FC  6E63  mov r6,r14
  8C08F1FE  A012  bra 8C08F226
  8C08F200  4908  shll2 r9
  8C08F202  9073  mov.w @([8C08F2EC]),r0
  8C08F204  05EE  mov.l @(R0,r14),r5
  8C08F206  2558  tst r5,r5
  8C08F208  890A  bt 8C08F220
  8C08F20A  6093  mov r9,r0
  8C08F20C  6A53  mov r5,r10
  8C08F20E  E700  mov ##0x00,r7
  8C08F210  03AE  mov.l @(R0,r10),r3
  8C08F212  6573  mov r7,r5
  8C08F214  6673  mov r7,r6
  8C08F216  430B  jsr @r3
  8C08F218  64C3  mov r12,r4
  8C08F21A  6403  mov r0,r4
  8C08F21C  2448  tst r4,r4
  8C08F21E  8B04  bf 8C08F22A
  8C08F220  925D  mov.w @([8C08F2DE]),r2
  8C08F222  7D01  add ##1,r13
  8C08F224  3E2C  add r2,r14
  8C08F226  3DB3  cmp/ge r11,r13
  8C08F228  8BEB  bf 8C08F202
  8C08F22A  4F26  lds.l @r15+,PR
  8C08F22C  6043  mov r4,r0
  8C08F22E  69F6  mov.l @r15+,r9
  8C08F230  6AF6  mov.l @r15+,r10
  8C08F232  6BF6  mov.l @r15+,r11
  8C08F234  6CF6  mov.l @r15+,r12
  8C08F236  6DF6  mov.l @r15+,r13
  8C08F238  000B  rts
  8C08F23A  6EF6  mov.l @r15+,r14
```
