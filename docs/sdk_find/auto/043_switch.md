# 043_switch

> Gerado por `tools/sdk_find.py`. Jogos: 10 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 0.98% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C064264` | 36 | 0.98% |
| Evolution - The World of Sacred Device (USA) | `8C166970` | 36 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1729E4` | 36 | 0.00% |
| Grandia II (USA) | `8C03BE78` | 36 | 0.00% |
| Macross M3 | `8C1C6290` | 36 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C1093C4` | 36 | 0.00% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C38D9F8` | 36 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C17B714` | 36 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C25AC28` | 36 | 0.00% |
| Sonic Adventure 2 (USA) (EnJaFrDeEs) | `8C112D70` | 36 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C064264`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C064264  2FE6  mov.l r14,@-r15
  8C064266  6053  mov r5,r0
  8C064268  4F22  sts.l PR,@-r15
  8C06426A  E10C  mov ##0x0C,r1
  8C06426C  3012  cmp/hs r1,r0
  8C06426E  8928  bt 8C0642C2
  8C064270  4000  shll r0
  8C064272  6103  mov r0,r1
  8C064274  C701  mova @([8C06427C]),R0
  8C064276  001D  mov.w @(R0,r1),r0
  8C064278  0023  braf r0
  8C06427A  0009  nop
  ...
  8C0642C2  DE2A  mov.l @([8C06436C]),r14
  8C0642C4  2448  tst r4,r4
  8C0642C6  8F05  bf.s 8C0642D4
  8C0642C8  6043  mov r4,r0
  8C0642CA  62E2  mov.l @r14,r2
  8C0642CC  E340  mov ##0x40,r3
  8C0642CE  2232  mov.l r3,@r2
  8C0642D0  62E2  mov.l @r14,r2
  8C0642D2  125B  mov.l r5,@(44,r2)
  8C0642D4  8801  cmp/eq ##0x01,R0
  8C0642D6  8B04  bf 8C0642E2
  8C0642D8  9345  mov.w @([8C064366]),r3
  8C0642DA  62E2  mov.l @r14,r2
  8C0642DC  2232  mov.l r3,@r2
  8C0642DE  62E2  mov.l @r14,r2
  8C0642E0  125C  mov.l r5,@(48,r2)
  8C0642E2  D323  mov.l @([8C064370]),r3
  8C0642E4  430B  jsr @r3
  8C0642E6  64E2  mov.l @r14,r4
  8C0642E8  4F26  lds.l @r15+,PR
  8C0642EA  D222  mov.l @([8C064374]),r2
  8C0642EC  64E2  mov.l @r14,r4
  8C0642EE  422B  jmp @r2
  8C0642F0  6EF6  mov.l @r15+,r14
```
