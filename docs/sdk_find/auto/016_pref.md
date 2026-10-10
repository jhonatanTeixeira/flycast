# 016_pref

> Gerado por `tools/sdk_find.py`. Jogos: 5 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 2.37% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C0692B4` | 148 | 2.37% |
| Evolution - The World of Sacred Device (USA) | `8C19AB54` | 148 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C174D2C` | 148 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C198BC0` | 148 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C27F934` | 148 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C0692B4`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C0692B4  2FE6  mov.l r14,@-r15
  8C0692B6  7501  add ##1,r5
  8C0692B8  2FD6  mov.l r13,@-r15
  8C0692BA  4515  cmp/pl r5
  8C0692BC  2FC6  mov.l r12,@-r15
  8C0692BE  E700  mov ##0x00,r7
  8C0692C0  DE48  mov.l @([8C0693E4]),r14
  8C0692C2  C74B  mova @([8C0693F0]),R0
  8C0692C4  D646  mov.l @([8C0693E0]),r6
  8C0692C6  DC48  mov.l @([8C0693E8]),r12
  8C0692C8  8F43  bf.s 8C069352
  8C0692CA  F408  fmov.s @r0,fr4
  8C0692CC  6262  mov.l @r6,r2
  8C0692CE  6D73  mov r7,r13
  8C0692D0  4D08  shll2 r13
  8C0692D2  721C  add ##28,r2
  8C0692D4  6323  mov r2,r3
  8C0692D6  73FC  add ##-4,r3
  8C0692D8  2632  mov.l r3,@r6
  8C0692DA  5241  mov.l @(4,r4),r2
  8C0692DC  32DC  add r13,r2
  8C0692DE  6122  mov.l @r2,r1
  8C0692E0  2312  mov.l r1,@r3
  8C0692E2  6362  mov.l @r6,r3
  8C0692E4  73FC  add ##-4,r3
  8C0692E6  2632  mov.l r3,@r6
  8C0692E8  5242  mov.l @(8,r4),r2
  8C0692EA  32DC  add r13,r2
  8C0692EC  8521  mov.w @(2,r2),R0
  8C0692EE  6203  mov r0,r2
  8C0692F0  425A  lds r2,FPUL
  8C0692F2  F32D  float FPUL,fr3
  8C0692F4  F342  fmul fr4,fr3
  8C0692F6  F33A  fmov.s fr3,@r3
  8C0692F8  6362  mov.l @r6,r3
  8C0692FA  73FC  add ##-4,r3
  8C0692FC  2632  mov.l r3,@r6
  8C0692FE  5242  mov.l @(8,r4),r2
  8C069300  3D2C  add r2,r13
  8C069302  62D1  mov.w @r13,r2
  8C069304  425A  lds r2,FPUL
  8C069306  F32D  float FPUL,fr3
  8C069308  F342  fmul fr4,fr3
  8C06930A  F33A  fmov.s fr3,@r3
  8C06930C  6362  mov.l @r6,r3
  8C06930E  6D73  mov r7,r13
  8C069310  4D08  shll2 r13
  8C069312  73FC  add ##-4,r3
  8C069314  2632  mov.l r3,@r6
  8C069316  4D00  shll r13
  8C069318  F3E8  fmov.s @r14,fr3
  8C06931A  E004  mov ##0x04,r0
  8C06931C  F33A  fmov.s fr3,@r3
  8C06931E  6362  mov.l @r6,r3
  8C069320  73FC  add ##-4,r3
  8C069322  2632  mov.l r3,@r6
  8C069324  6242  mov.l @r4,r2
  8C069326  32DC  add r13,r2
  8C069328  F326  fmov.s @(R0,r2),fr3
  8C06932A  F33A  fmov.s fr3,@r3
  8C06932C  6362  mov.l @r6,r3
  8C06932E  73FC  add ##-4,r3
  8C069330  2632  mov.l r3,@r6
  8C069332  6242  mov.l @r4,r2
  8C069334  3D2C  add r2,r13
  8C069336  F3D8  fmov.s @r13,fr3
  8C069338  F33A  fmov.s fr3,@r3
  8C06933A  6362  mov.l @r6,r3
  8C06933C  73FC  add ##-4,r3
  8C06933E  2632  mov.l r3,@r6
  8C069340  23C2  mov.l r12,@r3
  8C069342  6262  mov.l @r6,r2
  8C069344  0283  pref @r2
  8C069346  7701  add ##1,r7
  8C069348  6323  mov r2,r3
  8C06934A  3753  cmp/ge r5,r7
  8C06934C  7320  add ##32,r3
  8C06934E  8FBD  bf.s 8C0692CC
  8C069350  2632  mov.l r3,@r6
  8C069352  6362  mov.l @r6,r3
  8C069354  6573  mov r7,r5
  8C069356  4508  shll2 r5
  8C069358  731C  add ##28,r3
  8C06935A  6233  mov r3,r2
  8C06935C  72FC  add ##-4,r2
  8C06935E  2622  mov.l r2,@r6
  8C069360  5341  mov.l @(4,r4),r3
  8C069362  335C  add r5,r3
  8C069364  6132  mov.l @r3,r1
  8C069366  2212  mov.l r1,@r2
  8C069368  6362  mov.l @r6,r3
  8C06936A  73FC  add ##-4,r3
  8C06936C  2632  mov.l r3,@r6
  8C06936E  5242  mov.l @(8,r4),r2
  8C069370  325C  add r5,r2
  8C069372  8521  mov.w @(2,r2),R0
  8C069374  6203  mov r0,r2
  8C069376  425A  lds r2,FPUL
  8C069378  F32D  float FPUL,fr3
  8C06937A  F342  fmul fr4,fr3
  8C06937C  F33A  fmov.s fr3,@r3
  8C06937E  6362  mov.l @r6,r3
  8C069380  73FC  add ##-4,r3
  8C069382  2632  mov.l r3,@r6
  8C069384  5242  mov.l @(8,r4),r2
  8C069386  352C  add r2,r5
  8C069388  6251  mov.w @r5,r2
  8C06938A  425A  lds r2,FPUL
  8C06938C  F32D  float FPUL,fr3
  8C06938E  F342  fmul fr4,fr3
  8C069390  F33A  fmov.s fr3,@r3
  8C069392  6362  mov.l @r6,r3
  8C069394  6573  mov r7,r5
  8C069396  4508  shll2 r5
  8C069398  73FC  add ##-4,r3
  8C06939A  4500  shll r5
  8C06939C  2632  mov.l r3,@r6
  8C06939E  F3E8  fmov.s @r14,fr3
  8C0693A0  E004  mov ##0x04,r0
  8C0693A2  F33A  fmov.s fr3,@r3
  8C0693A4  6362  mov.l @r6,r3
  8C0693A6  73FC  add ##-4,r3
  8C0693A8  2632  mov.l r3,@r6
  8C0693AA  6242  mov.l @r4,r2
  8C0693AC  325C  add r5,r2
  8C0693AE  F326  fmov.s @(R0,r2),fr3
  8C0693B0  F33A  fmov.s fr3,@r3
  8C0693B2  6362  mov.l @r6,r3
  8C0693B4  73FC  add ##-4,r3
  8C0693B6  2632  mov.l r3,@r6
  8C0693B8  6242  mov.l @r4,r2
  8C0693BA  352C  add r2,r5
  8C0693BC  D20B  mov.l @([8C0693EC]),r2
  8C0693BE  F358  fmov.s @r5,fr3
  8C0693C0  F33A  fmov.s fr3,@r3
  8C0693C2  6362  mov.l @r6,r3
  8C0693C4  73FC  add ##-4,r3
  8C0693C6  2632  mov.l r3,@r6
  8C0693C8  2322  mov.l r2,@r3
  8C0693CA  6362  mov.l @r6,r3
  8C0693CC  0383  pref @r3
  8C0693CE  6233  mov r3,r2
  8C0693D0  7220  add ##32,r2
  8C0693D2  2622  mov.l r2,@r6
  8C0693D4  6CF6  mov.l @r15+,r12
  8C0693D6  6DF6  mov.l @r15+,r13
  8C0693D8  000B  rts
  8C0693DA  6EF6  mov.l @r15+,r14
```
