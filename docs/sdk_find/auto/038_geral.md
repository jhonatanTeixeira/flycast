# 038_geral

> Gerado por `tools/sdk_find.py`. Jogos: 2 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 1.10% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Shenmue (USA) (Disc 1) | `0C08CC44` | 14 | 0.59% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C04F3CC` | 14 | 0.51% |

## Shenmue (USA) (Disc 1) `0C08CC44`

Dump: `/mnt/1TB/dcbat_off/20261007-191903_Shenmue__USA___Disc_1__/jit-11043.txt`

```
  0C08CC44  6043  mov r4,r0
  0C08CC46  2008  tst r0,r0
  0C08CC48  8908  bt 0C08CC5C
  0C08CC4A  5102  mov.l @(8,r0),r1
  0C08CC4C  3510  cmp/eq r1,r5
  0C08CC4E  8905  bt 0C08CC5C
  0C08CC50  3517  cmp/gt r1,r5
  0C08CC52  8B01  bf 0C08CC58
  0C08CC54  AFF7  bra 0C08CC46
  0C08CC56  6002  mov.l @r0,r0
  0C08CC58  AFF5  bra 0C08CC46
  0C08CC5A  5001  mov.l @(4,r0),r0
  0C08CC5C  000B  rts
  0C08CC5E  0009  nop
```
