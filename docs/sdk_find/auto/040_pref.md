# 040_pref

> Gerado por `tools/sdk_find.py`. Jogos: 7 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 1.03% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C0B0BB4` | 117 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C214704` | 117 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C221DE4` | 117 | 0.00% |
| Grandia II (USA) | `8C141608` | 117 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C3E5468` | 117 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C1C1E28` | 117 | 1.03% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C4121A4` | 117 | 0.00% |

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C0B0BB4`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C0B0BB4  5050  mov.l @(0,r5),r0
  8C0B0BB6  6763  mov r6,r7
  8C0B0BB8  5651  mov.l @(4,r5),r6
  8C0B0BBA  5552  mov.l @(8,r5),r5
  8C0B0BBC  2FE6  mov.l r14,@-r15
  8C0B0BBE  F3FD  fschg
  8C0B0BC0  2FD6  mov.l r13,@-r15
  8C0B0BC2  EE40  mov ##0x40,r14
  8C0B0BC4  2FC6  mov.l r12,@-r15
  8C0B0BC6  4E00  shll r14
  8C0B0BC8  2FB6  mov.l r11,@-r15
  8C0B0BCA  EC06  mov ##0x06,r12
  8C0B0BCC  2FA6  mov.l r10,@-r15
  8C0B0BCE  4700  shll r7
  8C0B0BD0  8900  bt 8C0B0BD4
  8C0B0BD2  36EC  add r14,r6
  8C0B0BD4  8B40  bf 8C0B0C58
  8C0B0BD6  6353  mov r5,r3
  8C0B0BD8  6154  mov.b @r5+,r1
  8C0B0BDA  ED08  mov ##0x08,r13
  8C0B0BDC  6265  mov.w @r6+,r2
  8C0B0BDE  611C  extu.b r1,r1
  8C0B0BE0  6A54  mov.b @r5+,r10
  8C0B0BE2  312C  add r2,r1
  8C0B0BE4  6B65  mov.w @r6+,r11
  8C0B0BE6  011C  mov.b @(R0,r1),r1
  8C0B0BE8  6AAC  extu.b r10,r10
  8C0B0BEA  6254  mov.b @r5+,r2
  8C0B0BEC  3ABC  add r11,r10
  8C0B0BEE  0AAC  mov.b @(R0,r10),r10
  8C0B0BF0  2310  mov.b r1,@r3
  8C0B0BF2  7301  add ##1,r3
  8C0B0BF4  23A0  mov.b r10,@r3
  8C0B0BF6  7301  add ##1,r3
  8C0B0BF8  6165  mov.w @r6+,r1
  8C0B0BFA  6B54  mov.b @r5+,r11
  8C0B0BFC  622C  extu.b r2,r2
  8C0B0BFE  6A65  mov.w @r6+,r10
  8C0B0C00  321C  add r1,r2
  8C0B0C02  022C  mov.b @(R0,r2),r2
  8C0B0C04  6BBC  extu.b r11,r11
  8C0B0C06  6154  mov.b @r5+,r1
  8C0B0C08  3BAC  add r10,r11
  8C0B0C0A  0BBC  mov.b @(R0,r11),r11
  8C0B0C0C  2320  mov.b r2,@r3
  8C0B0C0E  7301  add ##1,r3
  8C0B0C10  23B0  mov.b r11,@r3
  8C0B0C12  7301  add ##1,r3
  8C0B0C14  6265  mov.w @r6+,r2
  8C0B0C16  6A54  mov.b @r5+,r10
  8C0B0C18  611C  extu.b r1,r1
  8C0B0C1A  6B65  mov.w @r6+,r11
  8C0B0C1C  312C  add r2,r1
  8C0B0C1E  011C  mov.b @(R0,r1),r1
  8C0B0C20  6AAC  extu.b r10,r10
  8C0B0C22  6254  mov.b @r5+,r2
  8C0B0C24  3ABC  add r11,r10
  8C0B0C26  0AAC  mov.b @(R0,r10),r10
  8C0B0C28  2310  mov.b r1,@r3
  8C0B0C2A  7301  add ##1,r3
  8C0B0C2C  23A0  mov.b r10,@r3
  8C0B0C2E  7301  add ##1,r3
  8C0B0C30  6165  mov.w @r6+,r1
  8C0B0C32  6B54  mov.b @r5+,r11
  8C0B0C34  622C  extu.b r2,r2
  8C0B0C36  6A65  mov.w @r6+,r10
  8C0B0C38  321C  add r1,r2
  8C0B0C3A  022C  mov.b @(R0,r2),r2
  8C0B0C3C  6BBC  extu.b r11,r11
  8C0B0C3E  6154  mov.b @r5+,r1
  8C0B0C40  3BAC  add r10,r11
  8C0B0C42  0BBC  mov.b @(R0,r11),r11
  8C0B0C44  4D10  dt r13
  8C0B0C46  2320  mov.b r2,@r3
  8C0B0C48  7301  add ##1,r3
  8C0B0C4A  23B0  mov.b r11,@r3
  8C0B0C4C  7301  add ##1,r3
  8C0B0C4E  6265  mov.w @r6+,r2
  8C0B0C50  8FC6  bf.s 8C0B0BE0
  8C0B0C52  611C  extu.b r1,r1
  8C0B0C54  75BF  add ##-65,r5
  8C0B0C56  76FE  add ##-2,r6
  8C0B0C58  F059  fmov.s @r5+,fr0
  8C0B0C5A  F259  fmov.s @r5+,fr2
  8C0B0C5C  F459  fmov.s @r5+,fr4
  8C0B0C5E  F659  fmov.s @r5+,fr6
  8C0B0C60  F40A  fmov.s fr0,@r4
  8C0B0C62  7408  add ##8,r4
  8C0B0C64  F42A  fmov.s fr2,@r4
  8C0B0C66  7408  add ##8,r4
  8C0B0C68  F44A  fmov.s fr4,@r4
  8C0B0C6A  7408  add ##8,r4
  8C0B0C6C  F46A  fmov.s fr6,@r4
  8C0B0C6E  0483  pref @r4
  8C0B0C70  7408  add ##8,r4
  8C0B0C72  F059  fmov.s @r5+,fr0
  8C0B0C74  F259  fmov.s @r5+,fr2
  8C0B0C76  F459  fmov.s @r5+,fr4
  8C0B0C78  F659  fmov.s @r5+,fr6
  8C0B0C7A  F40A  fmov.s fr0,@r4
  8C0B0C7C  7408  add ##8,r4
  8C0B0C7E  F42A  fmov.s fr2,@r4
  8C0B0C80  7408  add ##8,r4
  8C0B0C82  F44A  fmov.s fr4,@r4
  8C0B0C84  7408  add ##8,r4
  8C0B0C86  F46A  fmov.s fr6,@r4
  8C0B0C88  4C10  dt r12
  8C0B0C8A  0483  pref @r4
  8C0B0C8C  8F9F  bf.s 8C0B0BCE
  8C0B0C8E  7408  add ##8,r4
  8C0B0C90  6AF6  mov.l @r15+,r10
  8C0B0C92  6BF6  mov.l @r15+,r11
  8C0B0C94  F3FD  fschg
  8C0B0C96  6CF6  mov.l @r15+,r12
  8C0B0C98  6DF6  mov.l @r15+,r13
  8C0B0C9A  000B  rts
  8C0B0C9C  6EF6  mov.l @r15+,r14
```
