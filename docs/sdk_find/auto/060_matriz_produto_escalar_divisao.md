# 060_matriz_produto_escalar_divisao

> Gerado por `tools/sdk_find.py`. Jogos: 2 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 0.63% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C152AEC` | 115 | 0.63% |
| Skies of Arcadia (USA) (Disc 1) | `8C2CE794` | 115 | 0.00% |

## Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) `8C152AEC`

Dump: `/mnt/1TB/dcbat_off/20261007-190751_Napple_Tale_-_Arsia_in_Daydream__Japan__/jit-6592.txt`

```
  8C152AEC  FFFB  fmov.s fr15,@-r15
  8C152AEE  FFEB  fmov.s fr14,@-r15
  8C152AF0  FFDB  fmov.s fr13,@-r15
  8C152AF2  FFCB  fmov.s fr12,@-r15
  8C152AF4  2F86  mov.l r8,@-r15
  8C152AF6  6346  mov.l @r4+,r3
  8C152AF8  6546  mov.l @r4+,r5
  8C152AFA  6646  mov.l @r4+,r6
  8C152AFC  6746  mov.l @r4+,r7
  8C152AFE  E800  mov ##0x00,r8
  8C152B00  FC59  fmov.s @r5+,fr12
  8C152B02  FD59  fmov.s @r5+,fr13
  8C152B04  FE59  fmov.s @r5+,fr14
  8C152B06  FF9D  fldi1 fr15
  8C152B08  FDFD  ftrv xmtrx,fv12
  8C152B0A  F859  fmov.s @r5+,fr8
  8C152B0C  F959  fmov.s @r5+,fr9
  8C152B0E  FA59  fmov.s @r5+,fr10
  8C152B10  FB8D  fldi0 fr11
  8C152B12  F9FD  ftrv xmtrx,fv8
  8C152B14  D107  mov.l @([8C152B34]),r1
  8C152B16  F18D  fldi0 fr1
  8C152B18  6212  mov.l @r1,r2
  8C152B1A  F28D  fldi0 fr2
  8C152B1C  7110  add ##16,r1
  8C152B1E  F38D  fldi0 fr3
  8C152B20  4205  rotr r2
  8C152B22  0183  pref @r1
  8C152B24  8D08  bt.s 8C152B38
  8C152B26  4205  rotr r2
  8C152B28  4205  rotr r2
  8C152B2A  7120  add ##32,r1
  8C152B2C  8946  bt 8C152BBC
  8C152B2E  AFF8  bra 8C152B22
  8C152B30  4205  rotr r2
  ...
  8C152B38  F419  fmov.s @r1+,fr4
  8C152B3A  8D17  bt.s 8C152B6C
  8C152B3C  F519  fmov.s @r1+,fr5
  8C152B3E  F619  fmov.s @r1+,fr6
  8C152B40  FB8D  fldi0 fr11
  8C152B42  F9ED  fipr fv12,fv8
  8C152B44  F78D  fldi0 fr7
  8C152B46  F7B5  fcmp/gt fr11,fr7
  8C152B48  7114  add ##20,r1
  8C152B4A  8D03  bt.s 8C152B54
  8C152B4C  4205  rotr r2
  8C152B4E  8935  bt 8C152BBC
  8C152B50  AFE7  bra 8C152B22
  8C152B52  4205  rotr r2
  8C152B54  71F4  add ##-12,r1
  8C152B56  F0BC  fmov fr11,fr0
  8C152B58  F419  fmov.s @r1+,fr4
  8C152B5A  F04D  fneg fr0 
  8C152B5C  F519  fmov.s @r1+,fr5
  8C152B5E  F14E  fmac fr0,fr4,fr1
  8C152B60  F619  fmov.s @r1+,fr6
  8C152B62  F25E  fmac fr0,fr5,fr2
  8C152B64  8D2A  bt.s 8C152BBC
  8C152B66  F36E  fmac fr0,fr6,fr3
  8C152B68  AFDB  bra 8C152B22
  8C152B6A  4205  rotr r2
  ...
  8C152BBC  F09D  fldi1 fr0
  8C152BBE  7720  add ##32,r7
  8C152BC0  FB9D  fldi1 fr11
  8C152BC2  F0E5  fcmp/gt fr14,fr0
  8C152BC4  F0E3  fdiv fr14,fr0
  8C152BC6  F449  fmov.s @r4+,fr4
  8C152BC8  F549  fmov.s @r4+,fr5
  8C152BCA  7518  add ##24,r5
  8C152BCC  FF49  fmov.s @r4+,fr15
  8C152BCE  0029  movt r0
  8C152BD0  F749  fmov.s @r4+,fr7
  8C152BD2  0583  pref @r5
  8C152BD4  74F0  add ##-16,r4
  8C152BD6  F73B  fmov.s fr3,@-r7
  8C152BD8  F5D2  fmul fr13,fr5
  8C152BDA  F72B  fmov.s fr2,@-r7
  8C152BDC  F4C2  fmul fr12,fr4
  8C152BDE  F71B  fmov.s fr1,@-r7
  8C152BE0  F7BB  fmov.s fr11,@-r7
  8C152BE2  F75E  fmac fr0,fr5,fr7
  8C152BE4  F70B  fmov.s fr0,@-r7
  8C152BE6  FF4E  fmac fr0,fr4,fr15
  8C152BE8  F77B  fmov.s fr7,@-r7
  8C152BEA  75E8  add ##-24,r5
  8C152BEC  F3FD  fschg
  8C152BEE  4310  dt r3
  8C152BF0  F7EB  fmov.s fr14,@-r7
  8C152BF2  0693  ocbi @r6
  8C152BF4  380C  add r0,r8
  8C152BF6  F3B1  fsub fr11,fr3
  8C152BF8  0783  pref @r7
  8C152BFA  F2B1  fsub fr11,fr2
  8C152BFC  7740  add ##64,r7
  8C152BFE  FB4D  fneg fr11 
  8C152C00  F72B  fmov.s fr2,@-r7
  8C152C02  FB10  fadd fr1,fr11
  8C152C04  F7AB  fmov.s fr10,@-r7
  8C152C06  7620  add ##32,r6
  8C152C08  F78B  fmov.s fr8,@-r7
  8C152C0A  F7CB  fmov.s fr12,@-r7
  8C152C0C  0693  ocbi @r6
  8C152C0E  7620  add ##32,r6
  8C152C10  0783  pref @r7
  8C152C12  F3FD  fschg
  8C152C14  8901  bt 8C152C1A
  8C152C16  AF73  bra 8C152B00
  8C152C18  7720  add ##32,r7
  8C152C1A  6083  mov r8,r0
  8C152C1C  68F6  mov.l @r15+,r8
  8C152C1E  FCF9  fmov.s @r15+,fr12
  8C152C20  FDF9  fmov.s @r15+,fr13
  8C152C22  FEF9  fmov.s @r15+,fr14
  8C152C24  000B  rts
  8C152C26  FFF9  fmov.s @r15+,fr15
```
