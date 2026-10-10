# 003_nativo_lightxf

> Gerado por `tools/sdk_find.py`. Jogos: 5 · variantes (sequências normalizadas distintas): 4 · tempo perf somado: 0.00% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Evolution - The World of Sacred Device (USA) | `8C1C9B6C` | 138 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1BCF40` | 93 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C14DDC0` | 69 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C1C00A0` | 142 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C2CA500` | 142 | 0.00% |

## Evolution - The World of Sacred Device (USA) `8C1C9B6C`

Dump: `/mnt/1TB/dcbat/20261007-151806_Evolution_-_The_World_of_Sacred_Device__/jit-24035.txt`

```
  8C1C9B6C  FFFB  fmov.s fr15,@-r15
  8C1C9B6E  FFEB  fmov.s fr14,@-r15
  8C1C9B70  FFDB  fmov.s fr13,@-r15
  8C1C9B72  FFCB  fmov.s fr12,@-r15
  8C1C9B74  2F86  mov.l r8,@-r15
  8C1C9B76  6346  mov.l @r4+,r3
  8C1C9B78  6546  mov.l @r4+,r5
  8C1C9B7A  6646  mov.l @r4+,r6
  8C1C9B7C  6746  mov.l @r4+,r7
  8C1C9B7E  E800  mov ##0x00,r8
  8C1C9B80  E000  mov ##0x00,r0
  8C1C9B82  FC59  fmov.s @r5+,fr12
  8C1C9B84  FD59  fmov.s @r5+,fr13
  8C1C9B86  FE59  fmov.s @r5+,fr14
  8C1C9B88  FF9D  fldi1 fr15
  8C1C9B8A  FDFD  ftrv xmtrx,fv12
  8C1C9B8C  F859  fmov.s @r5+,fr8
  8C1C9B8E  F959  fmov.s @r5+,fr9
  8C1C9B90  FA59  fmov.s @r5+,fr10
  8C1C9B92  FB8D  fldi0 fr11
  8C1C9B94  F9FD  ftrv xmtrx,fv8
  8C1C9B96  D107  mov.l @([8C1C9BB4]),r1
  8C1C9B98  F18D  fldi0 fr1
  8C1C9B9A  6212  mov.l @r1,r2
  8C1C9B9C  F28D  fldi0 fr2
  8C1C9B9E  7110  add ##16,r1
  8C1C9BA0  F38D  fldi0 fr3
  8C1C9BA2  4205  rotr r2
  8C1C9BA4  0183  pref @r1
  8C1C9BA6  8D07  bt.s 8C1C9BB8
  8C1C9BA8  4205  rotr r2
  8C1C9BAA  4205  rotr r2
  8C1C9BAC  7120  add ##32,r1
  8C1C9BAE  8945  bt 8C1C9C3C
  8C1C9BB0  AFF8  bra 8C1C9BA4
  8C1C9BB2  4205  rotr r2
  ...
  8C1C9BB8  F419  fmov.s @r1+,fr4
  8C1C9BBA  8D17  bt.s 8C1C9BEC
  8C1C9BBC  F519  fmov.s @r1+,fr5
  8C1C9BBE  F619  fmov.s @r1+,fr6
  8C1C9BC0  FB8D  fldi0 fr11
  8C1C9BC2  F9ED  fipr fv12,fv8
  8C1C9BC4  F78D  fldi0 fr7
  8C1C9BC6  F7B5  fcmp/gt fr11,fr7
  8C1C9BC8  7114  add ##20,r1
  8C1C9BCA  8D03  bt.s 8C1C9BD4
  8C1C9BCC  4205  rotr r2
  8C1C9BCE  8935  bt 8C1C9C3C
  8C1C9BD0  AFE8  bra 8C1C9BA4
  8C1C9BD2  4205  rotr r2
  8C1C9BD4  71F4  add ##-12,r1
  8C1C9BD6  F0BC  fmov fr11,fr0
  8C1C9BD8  F419  fmov.s @r1+,fr4
  8C1C9BDA  F04D  fneg fr0 
  8C1C9BDC  F519  fmov.s @r1+,fr5
  8C1C9BDE  F14E  fmac fr0,fr4,fr1
  8C1C9BE0  F619  fmov.s @r1+,fr6
  8C1C9BE2  F25E  fmac fr0,fr5,fr2
  8C1C9BE4  8D2A  bt.s 8C1C9C3C
  8C1C9BE6  F36E  fmac fr0,fr6,fr3
  8C1C9BE8  AFDC  bra 8C1C9BA4
  8C1C9BEA  4205  rotr r2
  8C1C9BEC  F4C1  fsub fr12,fr4
  8C1C9BEE  F619  fmov.s @r1+,fr6
  8C1C9BF0  F5D1  fsub fr13,fr5
  8C1C9BF2  F78D  fldi0 fr7
  8C1C9BF4  F6E1  fsub fr14,fr6
  8C1C9BF6  FB8D  fldi0 fr11
  8C1C9BF8  F08D  fldi0 fr0
  8C1C9BFA  F9ED  fipr fv12,fv8
  8C1C9BFC  FF19  fmov.s @r1+,fr15
  8C1C9BFE  F5ED  fipr fv12,fv4
  8C1C9C00  E000  mov ##0x00,r0
  8C1C9C02  FB05  fcmp/gt fr0,fr11
  8C1C9C04  F47C  fmov fr7,fr4
  8C1C9C06  4024  rotcl r0
  8C1C9C08  FF75  fcmp/gt fr7,fr15
  8C1C9C0A  F47D  FSRRA fr4
  8C1C9C0C  4024  rotcl r0
  8C1C9C0E  8803  cmp/eq ##0x03,R0
  8C1C9C10  8D05  bt.s 8C1C9C1E
  8C1C9C12  FF19  fmov.s @r1+,fr15
  8C1C9C14  4205  rotr r2
  8C1C9C16  710C  add ##12,r1
  8C1C9C18  8910  bt 8C1C9C3C
  ...
  8C1C9C1E  FB42  fmul fr4,fr11
  8C1C9C20  F7F5  fcmp/gt fr15,fr7
  8C1C9C22  F442  fmul fr4,fr4
  8C1C9C24  8F02  bf.s 8C1C9C2C
  8C1C9C26  F519  fmov.s @r1+,fr5
  ...
  8C1C9C2C  F0BC  fmov fr11,fr0
  8C1C9C2E  4205  rotr r2
  8C1C9C30  F419  fmov.s @r1+,fr4
  8C1C9C32  F15E  fmac fr0,fr5,fr1
  8C1C9C34  F619  fmov.s @r1+,fr6
  8C1C9C36  F24E  fmac fr0,fr4,fr2
  8C1C9C38  8FB3  bf.s 8C1C9BA2
  8C1C9C3A  F36E  fmac fr0,fr6,fr3
  8C1C9C3C  F09D  fldi1 fr0
  8C1C9C3E  7720  add ##32,r7
  8C1C9C40  F0E5  fcmp/gt fr14,fr0
  8C1C9C42  F0E3  fdiv fr14,fr0
  8C1C9C44  F849  fmov.s @r4+,fr8
  8C1C9C46  E000  mov ##0x00,r0
  8C1C9C48  F949  fmov.s @r4+,fr9
  8C1C9C4A  7518  add ##24,r5
  8C1C9C4C  FA49  fmov.s @r4+,fr10
  8C1C9C4E  380E  addc r0,r8
  8C1C9C50  FB49  fmov.s @r4+,fr11
  8C1C9C52  0583  pref @r5
  8C1C9C54  FF9D  fldi1 fr15
  8C1C9C56  F73B  fmov.s fr3,@-r7
  8C1C9C58  74F0  add ##-16,r4
  8C1C9C5A  F72B  fmov.s fr2,@-r7
  8C1C9C5C  F8C2  fmul fr12,fr8
  8C1C9C5E  F71B  fmov.s fr1,@-r7
  8C1C9C60  F9D2  fmul fr13,fr9
  8C1C9C62  F7FB  fmov.s fr15,@-r7
  8C1C9C64  FB9E  fmac fr0,fr9,fr11
  8C1C9C66  F70B  fmov.s fr0,@-r7
  8C1C9C68  FA8E  fmac fr0,fr8,fr10
  8C1C9C6A  F7BB  fmov.s fr11,@-r7
  8C1C9C6C  75E8  add ##-24,r5
  8C1C9C6E  F7AB  fmov.s fr10,@-r7
  8C1C9C70  4310  dt r3
  8C1C9C72  F7EB  fmov.s fr14,@-r7
  8C1C9C74  0693  ocbi @r6
  8C1C9C76  7620  add ##32,r6
  8C1C9C78  0783  pref @r7
  8C1C9C7A  8F82  bf.s 8C1C9B82
  8C1C9C7C  7720  add ##32,r7
  8C1C9C7E  6083  mov r8,r0
  8C1C9C80  68F6  mov.l @r15+,r8
  8C1C9C82  FCF9  fmov.s @r15+,fr12
  8C1C9C84  FDF9  fmov.s @r15+,fr13
  8C1C9C86  FEF9  fmov.s @r15+,fr14
  8C1C9C88  000B  rts
  8C1C9C8A  FFF9  fmov.s @r15+,fr15
```

## Evolution 2 - Far Off Promise (USA) `8C1BCF40`

Dump: `/mnt/1TB/dcbat/20261007-151251_Evolution_2_-_Far_Off_Promise__USA__/jit-16237.txt`

```
  8C1BCF40  FFFB  fmov.s fr15,@-r15
  8C1BCF42  FFEB  fmov.s fr14,@-r15
  8C1BCF44  FFDB  fmov.s fr13,@-r15
  8C1BCF46  FFCB  fmov.s fr12,@-r15
  8C1BCF48  2F86  mov.l r8,@-r15
  8C1BCF4A  6346  mov.l @r4+,r3
  8C1BCF4C  6546  mov.l @r4+,r5
  8C1BCF4E  6646  mov.l @r4+,r6
  8C1BCF50  6746  mov.l @r4+,r7
  8C1BCF52  E800  mov ##0x00,r8
  8C1BCF54  E000  mov ##0x00,r0
  8C1BCF56  FC59  fmov.s @r5+,fr12
  8C1BCF58  FD59  fmov.s @r5+,fr13
  8C1BCF5A  FE59  fmov.s @r5+,fr14
  8C1BCF5C  FF9D  fldi1 fr15
  8C1BCF5E  FDFD  ftrv xmtrx,fv12
  8C1BCF60  F859  fmov.s @r5+,fr8
  8C1BCF62  F959  fmov.s @r5+,fr9
  8C1BCF64  FA59  fmov.s @r5+,fr10
  8C1BCF66  FB8D  fldi0 fr11
  8C1BCF68  F9FD  ftrv xmtrx,fv8
  8C1BCF6A  D107  mov.l @([8C1BCF88]),r1
  8C1BCF6C  F18D  fldi0 fr1
  8C1BCF6E  6212  mov.l @r1,r2
  8C1BCF70  F28D  fldi0 fr2
  8C1BCF72  7110  add ##16,r1
  8C1BCF74  F38D  fldi0 fr3
  8C1BCF76  4205  rotr r2
  8C1BCF78  0183  pref @r1
  8C1BCF7A  8D07  bt.s 8C1BCF8C
  8C1BCF7C  4205  rotr r2
  ...
  8C1BCF8C  F419  fmov.s @r1+,fr4
  8C1BCF8E  8D17  bt.s 8C1BCFC0
  8C1BCF90  F519  fmov.s @r1+,fr5
  8C1BCF92  F619  fmov.s @r1+,fr6
  8C1BCF94  FB8D  fldi0 fr11
  8C1BCF96  F9ED  fipr fv12,fv8
  8C1BCF98  F78D  fldi0 fr7
  8C1BCF9A  F7B5  fcmp/gt fr11,fr7
  8C1BCF9C  7114  add ##20,r1
  8C1BCF9E  8D03  bt.s 8C1BCFA8
  8C1BCFA0  4205  rotr r2
  8C1BCFA2  8935  bt 8C1BD010
  ...
  8C1BCFA8  71F4  add ##-12,r1
  8C1BCFAA  F0BC  fmov fr11,fr0
  8C1BCFAC  F419  fmov.s @r1+,fr4
  8C1BCFAE  F04D  fneg fr0 
  8C1BCFB0  F519  fmov.s @r1+,fr5
  8C1BCFB2  F14E  fmac fr0,fr4,fr1
  8C1BCFB4  F619  fmov.s @r1+,fr6
  8C1BCFB6  F25E  fmac fr0,fr5,fr2
  8C1BCFB8  8D2A  bt.s 8C1BD010
  8C1BCFBA  F36E  fmac fr0,fr6,fr3
  ...
  8C1BD010  F09D  fldi1 fr0
  8C1BD012  7720  add ##32,r7
  8C1BD014  F0E5  fcmp/gt fr14,fr0
  8C1BD016  F0E3  fdiv fr14,fr0
  8C1BD018  F849  fmov.s @r4+,fr8
  8C1BD01A  E000  mov ##0x00,r0
  8C1BD01C  F949  fmov.s @r4+,fr9
  8C1BD01E  7518  add ##24,r5
  8C1BD020  FA49  fmov.s @r4+,fr10
  8C1BD022  380E  addc r0,r8
  8C1BD024  FB49  fmov.s @r4+,fr11
  8C1BD026  0583  pref @r5
  8C1BD028  FF9D  fldi1 fr15
  8C1BD02A  F73B  fmov.s fr3,@-r7
  8C1BD02C  74F0  add ##-16,r4
  8C1BD02E  F72B  fmov.s fr2,@-r7
  8C1BD030  F8C2  fmul fr12,fr8
  8C1BD032  F71B  fmov.s fr1,@-r7
  8C1BD034  F9D2  fmul fr13,fr9
  8C1BD036  F7FB  fmov.s fr15,@-r7
  8C1BD038  FB9E  fmac fr0,fr9,fr11
  8C1BD03A  F70B  fmov.s fr0,@-r7
  8C1BD03C  FA8E  fmac fr0,fr8,fr10
  8C1BD03E  F7BB  fmov.s fr11,@-r7
  8C1BD040  75E8  add ##-24,r5
  8C1BD042  F7AB  fmov.s fr10,@-r7
  8C1BD044  4310  dt r3
  8C1BD046  F7EB  fmov.s fr14,@-r7
  8C1BD048  0693  ocbi @r6
  8C1BD04A  7620  add ##32,r6
  8C1BD04C  0783  pref @r7
  8C1BD04E  8F82  bf.s 8C1BCF56
  8C1BD050  7720  add ##32,r7
  8C1BD052  6083  mov r8,r0
  8C1BD054  68F6  mov.l @r15+,r8
  8C1BD056  FCF9  fmov.s @r15+,fr12
  8C1BD058  FDF9  fmov.s @r15+,fr13
  8C1BD05A  FEF9  fmov.s @r15+,fr14
  8C1BD05C  000B  rts
  8C1BD05E  FFF9  fmov.s @r15+,fr15
```

## Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) `8C14DDC0`

Dump: `/mnt/1TB/dcbat_off/20261007-190751_Napple_Tale_-_Arsia_in_Daydream__Japan__/jit-6592.txt`

```
  8C14DDC0  FFFB  fmov.s fr15,@-r15
  8C14DDC2  FFEB  fmov.s fr14,@-r15
  8C14DDC4  FFDB  fmov.s fr13,@-r15
  8C14DDC6  FFCB  fmov.s fr12,@-r15
  8C14DDC8  2F86  mov.l r8,@-r15
  8C14DDCA  6346  mov.l @r4+,r3
  8C14DDCC  6546  mov.l @r4+,r5
  8C14DDCE  6646  mov.l @r4+,r6
  8C14DDD0  6746  mov.l @r4+,r7
  8C14DDD2  E800  mov ##0x00,r8
  8C14DDD4  E000  mov ##0x00,r0
  8C14DDD6  FC59  fmov.s @r5+,fr12
  8C14DDD8  FD59  fmov.s @r5+,fr13
  8C14DDDA  FE59  fmov.s @r5+,fr14
  8C14DDDC  FF9D  fldi1 fr15
  8C14DDDE  FDFD  ftrv xmtrx,fv12
  8C14DDE0  F859  fmov.s @r5+,fr8
  8C14DDE2  F959  fmov.s @r5+,fr9
  8C14DDE4  FA59  fmov.s @r5+,fr10
  8C14DDE6  FB8D  fldi0 fr11
  8C14DDE8  F9FD  ftrv xmtrx,fv8
  8C14DDEA  D107  mov.l @([8C14DE08]),r1
  8C14DDEC  F18D  fldi0 fr1
  8C14DDEE  6212  mov.l @r1,r2
  8C14DDF0  F28D  fldi0 fr2
  8C14DDF2  7110  add ##16,r1
  8C14DDF4  F38D  fldi0 fr3
  8C14DDF6  4205  rotr r2
  8C14DDF8  0183  pref @r1
  8C14DDFA  8D07  bt.s 8C14DE0C
  8C14DDFC  4205  rotr r2
  8C14DDFE  4205  rotr r2
  8C14DE00  7120  add ##32,r1
  8C14DE02  8945  bt 8C14DE90
  8C14DE04  AFF8  bra 8C14DDF8
  8C14DE06  4205  rotr r2
  ...
  8C14DE90  F09D  fldi1 fr0
  8C14DE92  7720  add ##32,r7
  8C14DE94  F0E5  fcmp/gt fr14,fr0
  8C14DE96  F0E3  fdiv fr14,fr0
  8C14DE98  F849  fmov.s @r4+,fr8
  8C14DE9A  E000  mov ##0x00,r0
  8C14DE9C  F949  fmov.s @r4+,fr9
  8C14DE9E  7518  add ##24,r5
  8C14DEA0  FA49  fmov.s @r4+,fr10
  8C14DEA2  380E  addc r0,r8
  8C14DEA4  FB49  fmov.s @r4+,fr11
  8C14DEA6  0583  pref @r5
  8C14DEA8  FF9D  fldi1 fr15
  8C14DEAA  F73B  fmov.s fr3,@-r7
  8C14DEAC  74F0  add ##-16,r4
  8C14DEAE  F72B  fmov.s fr2,@-r7
  8C14DEB0  F8C2  fmul fr12,fr8
  8C14DEB2  F71B  fmov.s fr1,@-r7
  8C14DEB4  F9D2  fmul fr13,fr9
  8C14DEB6  F7FB  fmov.s fr15,@-r7
  8C14DEB8  FB9E  fmac fr0,fr9,fr11
  8C14DEBA  F70B  fmov.s fr0,@-r7
  8C14DEBC  FA8E  fmac fr0,fr8,fr10
  8C14DEBE  F7BB  fmov.s fr11,@-r7
  8C14DEC0  75E8  add ##-24,r5
  8C14DEC2  F7AB  fmov.s fr10,@-r7
  8C14DEC4  4310  dt r3
  8C14DEC6  F7EB  fmov.s fr14,@-r7
  8C14DEC8  0693  ocbi @r6
  8C14DECA  7620  add ##32,r6
  8C14DECC  0783  pref @r7
  8C14DECE  8F82  bf.s 8C14DDD6
  8C14DED0  7720  add ##32,r7
```

## Resident Evil - Code - Veronica (USA) (Disc 1) `8C1C00A0`

Dump: `/mnt/1TB/dcbat/20261007-160515_Resident_Evil_-_Code_-_Veronica__USA___D/jit-82021.txt`

```
  8C1C00A0  FFFB  fmov.s fr15,@-r15
  8C1C00A2  FFEB  fmov.s fr14,@-r15
  8C1C00A4  FFDB  fmov.s fr13,@-r15
  8C1C00A6  FFCB  fmov.s fr12,@-r15
  8C1C00A8  2F86  mov.l r8,@-r15
  8C1C00AA  6346  mov.l @r4+,r3
  8C1C00AC  6546  mov.l @r4+,r5
  8C1C00AE  6646  mov.l @r4+,r6
  8C1C00B0  6746  mov.l @r4+,r7
  8C1C00B2  E800  mov ##0x00,r8
  8C1C00B4  E000  mov ##0x00,r0
  8C1C00B6  FC59  fmov.s @r5+,fr12
  8C1C00B8  FD59  fmov.s @r5+,fr13
  8C1C00BA  FE59  fmov.s @r5+,fr14
  8C1C00BC  FF9D  fldi1 fr15
  8C1C00BE  FDFD  ftrv xmtrx,fv12
  8C1C00C0  F859  fmov.s @r5+,fr8
  8C1C00C2  F959  fmov.s @r5+,fr9
  8C1C00C4  FA59  fmov.s @r5+,fr10
  8C1C00C6  FB8D  fldi0 fr11
  8C1C00C8  F9FD  ftrv xmtrx,fv8
  8C1C00CA  D107  mov.l @([8C1C00E8]),r1
  8C1C00CC  F18D  fldi0 fr1
  8C1C00CE  6212  mov.l @r1,r2
  8C1C00D0  F28D  fldi0 fr2
  8C1C00D2  7110  add ##16,r1
  8C1C00D4  F38D  fldi0 fr3
  8C1C00D6  4205  rotr r2
  8C1C00D8  0183  pref @r1
  8C1C00DA  8D07  bt.s 8C1C00EC
  8C1C00DC  4205  rotr r2
  8C1C00DE  4205  rotr r2
  8C1C00E0  7120  add ##32,r1
  8C1C00E2  8945  bt 8C1C0170
  8C1C00E4  AFF8  bra 8C1C00D8
  8C1C00E6  4205  rotr r2
  ...
  8C1C00EC  F419  fmov.s @r1+,fr4
  8C1C00EE  8D17  bt.s 8C1C0120
  8C1C00F0  F519  fmov.s @r1+,fr5
  8C1C00F2  F619  fmov.s @r1+,fr6
  8C1C00F4  FB8D  fldi0 fr11
  8C1C00F6  F9ED  fipr fv12,fv8
  8C1C00F8  F78D  fldi0 fr7
  8C1C00FA  F7B5  fcmp/gt fr11,fr7
  8C1C00FC  7114  add ##20,r1
  8C1C00FE  8D03  bt.s 8C1C0108
  8C1C0100  4205  rotr r2
  8C1C0102  8935  bt 8C1C0170
  8C1C0104  AFE8  bra 8C1C00D8
  8C1C0106  4205  rotr r2
  8C1C0108  71F4  add ##-12,r1
  8C1C010A  F0BC  fmov fr11,fr0
  8C1C010C  F419  fmov.s @r1+,fr4
  8C1C010E  F04D  fneg fr0 
  8C1C0110  F519  fmov.s @r1+,fr5
  8C1C0112  F14E  fmac fr0,fr4,fr1
  8C1C0114  F619  fmov.s @r1+,fr6
  8C1C0116  F25E  fmac fr0,fr5,fr2
  8C1C0118  8D2A  bt.s 8C1C0170
  8C1C011A  F36E  fmac fr0,fr6,fr3
  8C1C011C  AFDC  bra 8C1C00D8
  8C1C011E  4205  rotr r2
  8C1C0120  F4C1  fsub fr12,fr4
  8C1C0122  F619  fmov.s @r1+,fr6
  8C1C0124  F5D1  fsub fr13,fr5
  8C1C0126  F78D  fldi0 fr7
  8C1C0128  F6E1  fsub fr14,fr6
  8C1C012A  FB8D  fldi0 fr11
  8C1C012C  F08D  fldi0 fr0
  8C1C012E  F9ED  fipr fv12,fv8
  8C1C0130  FF19  fmov.s @r1+,fr15
  8C1C0132  F5ED  fipr fv12,fv4
  8C1C0134  E000  mov ##0x00,r0
  8C1C0136  FB05  fcmp/gt fr0,fr11
  8C1C0138  F47C  fmov fr7,fr4
  8C1C013A  4024  rotcl r0
  8C1C013C  FF75  fcmp/gt fr7,fr15
  8C1C013E  F47D  FSRRA fr4
  8C1C0140  4024  rotcl r0
  8C1C0142  8803  cmp/eq ##0x03,R0
  8C1C0144  8D05  bt.s 8C1C0152
  8C1C0146  FF19  fmov.s @r1+,fr15
  8C1C0148  4205  rotr r2
  8C1C014A  710C  add ##12,r1
  8C1C014C  8910  bt 8C1C0170
  8C1C014E  AFC3  bra 8C1C00D8
  8C1C0150  4205  rotr r2
  8C1C0152  FB42  fmul fr4,fr11
  8C1C0154  F7F5  fcmp/gt fr15,fr7
  8C1C0156  F442  fmul fr4,fr4
  8C1C0158  8F02  bf.s 8C1C0160
  8C1C015A  F519  fmov.s @r1+,fr5
  8C1C015C  FBF2  fmul fr15,fr11
  8C1C015E  FB42  fmul fr4,fr11
  8C1C0160  F0BC  fmov fr11,fr0
  8C1C0162  4205  rotr r2
  8C1C0164  F419  fmov.s @r1+,fr4
  8C1C0166  F15E  fmac fr0,fr5,fr1
  8C1C0168  F619  fmov.s @r1+,fr6
  8C1C016A  F24E  fmac fr0,fr4,fr2
  8C1C016C  8FB3  bf.s 8C1C00D6
  8C1C016E  F36E  fmac fr0,fr6,fr3
  8C1C0170  F09D  fldi1 fr0
  8C1C0172  7720  add ##32,r7
  8C1C0174  F0E5  fcmp/gt fr14,fr0
  8C1C0176  F0E3  fdiv fr14,fr0
  8C1C0178  F849  fmov.s @r4+,fr8
  8C1C017A  E000  mov ##0x00,r0
  8C1C017C  F949  fmov.s @r4+,fr9
  8C1C017E  7518  add ##24,r5
  8C1C0180  FA49  fmov.s @r4+,fr10
  8C1C0182  380E  addc r0,r8
  8C1C0184  FB49  fmov.s @r4+,fr11
  8C1C0186  0583  pref @r5
  8C1C0188  FF9D  fldi1 fr15
  8C1C018A  F73B  fmov.s fr3,@-r7
  8C1C018C  74F0  add ##-16,r4
  8C1C018E  F72B  fmov.s fr2,@-r7
  8C1C0190  F8C2  fmul fr12,fr8
  8C1C0192  F71B  fmov.s fr1,@-r7
  8C1C0194  F9D2  fmul fr13,fr9
  8C1C0196  F7FB  fmov.s fr15,@-r7
  8C1C0198  FB9E  fmac fr0,fr9,fr11
  8C1C019A  F70B  fmov.s fr0,@-r7
  8C1C019C  FA8E  fmac fr0,fr8,fr10
  8C1C019E  F7BB  fmov.s fr11,@-r7
  8C1C01A0  75E8  add ##-24,r5
  8C1C01A2  F7AB  fmov.s fr10,@-r7
  8C1C01A4  4310  dt r3
  8C1C01A6  F7EB  fmov.s fr14,@-r7
  8C1C01A8  0693  ocbi @r6
  8C1C01AA  7620  add ##32,r6
  8C1C01AC  0783  pref @r7
  8C1C01AE  8F82  bf.s 8C1C00B6
  8C1C01B0  7720  add ##32,r7
  8C1C01B2  6083  mov r8,r0
  8C1C01B4  68F6  mov.l @r15+,r8
  8C1C01B6  FCF9  fmov.s @r15+,fr12
  8C1C01B8  FDF9  fmov.s @r15+,fr13
  8C1C01BA  FEF9  fmov.s @r15+,fr14
  8C1C01BC  000B  rts
  8C1C01BE  FFF9  fmov.s @r15+,fr15
```
