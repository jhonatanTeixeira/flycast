# 031_laco

> Gerado por `tools/sdk_find.py`. Jogos: 11 · variantes (sequências normalizadas distintas): 3 · tempo perf somado: 1.27% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C080A36` | 24 | 1.12% |
| Evolution - The World of Sacred Device (USA) | `8C1BAC32` | 24 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C188954` | 22 | 0.00% |
| Grandia II (USA) | `8C08628A` | 24 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C34CA12` | 24 | 0.00% |
| Macross M3 | `8C1C73BE` | 24 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C11905A` | 21 | 0.15% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C38EB26` | 24 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C1B8780` | 24 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C2A61C2` | 24 | 0.00% |
| Sonic Adventure 2 (USA) (EnJaFrDeEs) | `8C116702` | 22 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C080A36`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C080A36  2448  tst r4,r4
  8C080A38  8912  bt 8C080A60
  8C080A3A  E700  mov ##0x00,r7
  8C080A3C  6373  mov r7,r3
  8C080A3E  3352  cmp/hs r5,r3
  8C080A40  890E  bt 8C080A60
  8C080A42  6173  mov r7,r1
  8C080A44  4108  shll2 r1
  8C080A46  4108  shll2 r1
  8C080A48  6373  mov r7,r3
  8C080A4A  313C  add r3,r1
  8C080A4C  4108  shll2 r1
  8C080A4E  314C  add r4,r1
  8C080A50  6212  mov.l @r1,r2
  8C080A52  3260  cmp/eq r6,r2
  8C080A54  8B01  bf 8C080A5A
  8C080A56  000B  rts
  8C080A58  6073  mov r7,r0
  8C080A5A  7701  add ##1,r7
  8C080A5C  3752  cmp/hs r5,r7
  8C080A5E  8BF0  bf 8C080A42
  8C080A60  E0FF  mov ##0xFF,r0
  8C080A62  000B  rts
  8C080A64  0009  nop
```

## Evolution 2 - Far Off Promise (USA) `8C188954`

Dump: `/mnt/1TB/dcbat/20261007-151251_Evolution_2_-_Far_Off_Promise__USA__/jit-16237.txt`

```
  8C188954  2448  tst r4,r4
  8C188956  8912  bt 8C18897E
  8C188958  E700  mov ##0x00,r7
  8C18895A  6373  mov r7,r3
  8C18895C  3352  cmp/hs r5,r3
  8C18895E  890E  bt 8C18897E
  8C188960  6173  mov r7,r1
  8C188962  4108  shll2 r1
  8C188964  4108  shll2 r1
  8C188966  6373  mov r7,r3
  8C188968  313C  add r3,r1
  8C18896A  4108  shll2 r1
  8C18896C  314C  add r4,r1
  8C18896E  6212  mov.l @r1,r2
  8C188970  3260  cmp/eq r6,r2
  8C188972  8B01  bf 8C188978
  ...
  8C188978  7701  add ##1,r7
  8C18897A  3752  cmp/hs r5,r7
  8C18897C  8BF0  bf 8C188960
  8C18897E  E0FF  mov ##0xFF,r0
  8C188980  000B  rts
  8C188982  0009  nop
```

## Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) `8C11905A`

Dump: `/mnt/1TB/dcbat_off/20261007-190751_Napple_Tale_-_Arsia_in_Daydream__Japan__/jit-6592.txt`

```
  8C11905A  2448  tst r4,r4
  8C11905C  8912  bt 8C119084
  8C11905E  E700  mov ##0x00,r7
  8C119060  6373  mov r7,r3
  8C119062  3352  cmp/hs r5,r3
  8C119064  890E  bt 8C119084
  8C119066  6173  mov r7,r1
  8C119068  4108  shll2 r1
  8C11906A  4108  shll2 r1
  8C11906C  6373  mov r7,r3
  8C11906E  313C  add r3,r1
  8C119070  4108  shll2 r1
  8C119072  314C  add r4,r1
  8C119074  6212  mov.l @r1,r2
  8C119076  3260  cmp/eq r6,r2
  8C119078  8B01  bf 8C11907E
  8C11907A  000B  rts
  8C11907C  6073  mov r7,r0
  8C11907E  7701  add ##1,r7
  8C119080  3752  cmp/hs r5,r7
  8C119082  8BF0  bf 8C119066
```
