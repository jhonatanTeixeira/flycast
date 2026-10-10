# 055_geral

> Gerado por `tools/sdk_find.py`. Jogos: 6 · variantes (sequências normalizadas distintas): 2 · tempo perf somado: 0.74% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Grandia II (USA) | `8C0BEA88` | 153 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C366A0C` | 153 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C055346` | 42 | 0.00% |
| Macross M3 | `8C1F14EC` | 153 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C17C098` | 153 | 0.74% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C366CCC` | 153 | 0.00% |

## Grandia II (USA) `8C0BEA88`

Dump: `/mnt/1TB/dcbat/20261002-192949_Grandia_II__USA__/jit-534412.txt`

```
  8C0BEA88  6753  mov r5,r7
  8C0BEA8A  E3FC  mov ##0xFC,r3
  8C0BEA8C  7704  add ##4,r7
  8C0BEA8E  2FE6  mov.l r14,@-r15
  8C0BEA90  2739  and r3,r7
  8C0BEA92  2FD6  mov.l r13,@-r15
  8C0BEA94  7504  add ##4,r5
  8C0BEA96  2FC6  mov.l r12,@-r15
  8C0BEA98  3578  sub r7,r5
  8C0BEA9A  2FB6  mov.l r11,@-r15
  8C0BEA9C  4508  shll2 r5
  8C0BEA9E  2FA6  mov.l r10,@-r15
  8C0BEAA0  4500  shll r5
  8C0BEAA2  2F96  mov.l r9,@-r15
  8C0BEAA4  6D74  mov.b @r7+,r13
  8C0BEAA6  EC20  mov ##0x20,r12
  8C0BEAA8  6274  mov.b @r7+,r2
  8C0BEAAA  4D18  shll8 r13
  8C0BEAAC  6174  mov.b @r7+,r1
  8C0BEAAE  622C  extu.b r2,r2
  8C0BEAB0  2D2B  or r2,r13
  8C0BEAB2  6274  mov.b @r7+,r2
  8C0BEAB4  611C  extu.b r1,r1
  8C0BEAB6  4D18  shll8 r13
  8C0BEAB8  6E74  mov.b @r7+,r14
  8C0BEABA  2D1B  or r1,r13
  8C0BEABC  4D18  shll8 r13
  8C0BEABE  6174  mov.b @r7+,r1
  8C0BEAC0  622C  extu.b r2,r2
  8C0BEAC2  4E18  shll8 r14
  8C0BEAC4  611C  extu.b r1,r1
  8C0BEAC6  2D2B  or r2,r13
  8C0BEAC8  6274  mov.b @r7+,r2
  8C0BEACA  2E1B  or r1,r14
  8C0BEACC  4E18  shll8 r14
  8C0BEACE  6174  mov.b @r7+,r1
  8C0BEAD0  622C  extu.b r2,r2
  8C0BEAD2  2E2B  or r2,r14
  8C0BEAD4  E21E  mov ##0x1E,r2
  8C0BEAD6  611C  extu.b r1,r1
  8C0BEAD8  3523  cmp/ge r2,r5
  8C0BEADA  4E18  shll8 r14
  8C0BEADC  2E1B  or r1,r14
  8C0BEADE  8F22  bf.s 8C0BEB26
  8C0BEAE0  4D5D  shld r5,r13
  ...
  8C0BEB26  60D3  mov r13,r0
  8C0BEB28  4004  rotl r0
  8C0BEB2A  4004  rotl r0
  8C0BEB2C  C903  and ##3,R0
  8C0BEB2E  4D08  shll2 r13
  8C0BEB30  6B03  mov r0,r11
  8C0BEB32  7502  add ##2,r5
  8C0BEB34  7502  add ##2,r5
  8C0BEB36  35C3  cmp/ge r12,r5
  8C0BEB38  8B10  bf 8C0BEB5C
  ...
  8C0BEB5C  4D08  shll2 r13
  8C0BEB5E  E21D  mov ##0x1D,r2
  8C0BEB60  3523  cmp/ge r2,r5
  8C0BEB62  8B21  bf 8C0BEBA8
  ...
  8C0BEBA8  6AD3  mov r13,r10
  8C0BEBAA  4D08  shll2 r13
  8C0BEBAC  622B  neg r2,r2
  8C0BEBAE  4D00  shll r13
  8C0BEBB0  7503  add ##3,r5
  8C0BEBB2  4A2D  shld r2,r10
  8C0BEBB4  7501  add ##1,r5
  8C0BEBB6  35C3  cmp/ge r12,r5
  8C0BEBB8  8B10  bf 8C0BEBDC
  ...
  8C0BEBDC  4D00  shll r13
  8C0BEBDE  E311  mov ##0x11,r3
  8C0BEBE0  3533  cmp/ge r3,r5
  8C0BEBE2  8B20  bf 8C0BEC26
  ...
  8C0BEC26  69D3  mov r13,r9
  8C0BEC28  E20F  mov ##0x0F,r2
  8C0BEC2A  4929  shlr16 r9
  8C0BEC2C  4D2D  shld r2,r13
  8C0BEC2E  750F  add ##15,r5
  8C0BEC30  4901  shlr r9
  8C0BEC32  7501  add ##1,r5
  8C0BEC34  35C3  cmp/ge r12,r5
  8C0BEC36  8B10  bf 8C0BEC5A
  ...
  8C0BEC5A  4D00  shll r13
  8C0BEC5C  E211  mov ##0x11,r2
  8C0BEC5E  3523  cmp/ge r2,r5
  8C0BEC60  8B20  bf 8C0BECA4
  8C0BEC62  3528  sub r2,r5
  8C0BEC64  2558  tst r5,r5
  8C0BEC66  890B  bt 8C0BEC80
  8C0BEC68  E30F  mov ##0x0F,r3
  8C0BEC6A  61E3  mov r14,r1
  8C0BEC6C  3358  sub r5,r3
  8C0BEC6E  633B  neg r3,r3
  8C0BEC70  413D  shld r3,r1
  8C0BEC72  2D1B  or r1,r13
  8C0BEC74  60D3  mov r13,r0
  8C0BEC76  4029  shlr16 r0
  8C0BEC78  6DE3  mov r14,r13
  8C0BEC7A  4001  shlr r0
  8C0BEC7C  A004  bra 8C0BEC88
  8C0BEC7E  4D5D  shld r5,r13
  ...
  8C0BEC88  6E74  mov.b @r7+,r14
  8C0BEC8A  6274  mov.b @r7+,r2
  8C0BEC8C  4E18  shll8 r14
  8C0BEC8E  6374  mov.b @r7+,r3
  8C0BEC90  622C  extu.b r2,r2
  8C0BEC92  2E2B  or r2,r14
  8C0BEC94  6274  mov.b @r7+,r2
  8C0BEC96  633C  extu.b r3,r3
  8C0BEC98  4E18  shll8 r14
  8C0BEC9A  2E3B  or r3,r14
  8C0BEC9C  622C  extu.b r2,r2
  8C0BEC9E  4E18  shll8 r14
  8C0BECA0  A006  bra 8C0BECB0
  8C0BECA2  2E2B  or r2,r14
  ...
  8C0BECB0  7501  add ##1,r5
  8C0BECB2  35C3  cmp/ge r12,r5
  8C0BECB4  8B10  bf 8C0BECD8
  ...
  8C0BECD8  4D00  shll r13
  8C0BECDA  7501  add ##1,r5
  8C0BECDC  35C3  cmp/ge r12,r5
  8C0BECDE  8B10  bf 8C0BED02
  ...
  8C0BED02  4D00  shll r13
  8C0BED04  E20A  mov ##0x0A,r2
  8C0BED06  3523  cmp/ge r2,r5
  8C0BED08  8B0B  bf 8C0BED22
  ...
  8C0BED22  67D3  mov r13,r7
  8C0BED24  4719  shlr8 r7
  8C0BED26  4709  shlr2 r7
  8C0BED28  7516  add ##22,r5
  8C0BED2A  E31C  mov ##0x1C,r3
  8C0BED2C  4A3C  shad r3,r10
  8C0BED2E  7501  add ##1,r5
  8C0BED30  35C3  cmp/ge r12,r5
  8C0BED32  2BB8  tst r11,r11
  8C0BED34  0229  movt r2
  8C0BED36  2422  mov.l r2,@r4
  8C0BED38  E20D  mov ##0x0D,r2
  8C0BED3A  4021  shar r0
  8C0BED3C  492C  shad r2,r9
  8C0BED3E  4021  shar r0
  8C0BED40  2A9B  or r9,r10
  8C0BED42  2A0B  or r0,r10
  8C0BED44  14A1  mov.l r10,@(4,r4)
  8C0BED46  E00C  mov ##0x0C,r0
  8C0BED48  1472  mov.l r7,@(8,r4)
  8C0BED4A  2602  mov.l r0,@r6
  8C0BED4C  69F6  mov.l @r15+,r9
  8C0BED4E  6AF6  mov.l @r15+,r10
  8C0BED50  6BF6  mov.l @r15+,r11
  8C0BED52  6CF6  mov.l @r15+,r12
  8C0BED54  6DF6  mov.l @r15+,r13
  8C0BED56  000B  rts
  8C0BED58  6EF6  mov.l @r15+,r14
```

## Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) `8C055346`

Dump: `/mnt/1TB/dcbat/20261007-163759_Le_Mans_24_Hours__Europe___En_Fr_De_Es_I/jit-109461.txt`

```
  8C055346  69D3  mov r13,r9
  8C055348  E20F  mov ##0x0F,r2
  8C05534A  4929  shlr16 r9
  8C05534C  4D2D  shld r2,r13
  8C05534E  750F  add ##15,r5
  8C055350  4901  shlr r9
  8C055352  7501  add ##1,r5
  8C055354  35C3  cmp/ge r12,r5
  8C055356  8B10  bf 8C05537A
  ...
  8C05537A  4D00  shll r13
  8C05537C  E211  mov ##0x11,r2
  8C05537E  3523  cmp/ge r2,r5
  8C055380  8B20  bf 8C0553C4
  8C055382  3528  sub r2,r5
  8C055384  2558  tst r5,r5
  8C055386  890B  bt 8C0553A0
  8C055388  E30F  mov ##0x0F,r3
  8C05538A  61E3  mov r14,r1
  8C05538C  3358  sub r5,r3
  8C05538E  633B  neg r3,r3
  8C055390  413D  shld r3,r1
  8C055392  2D1B  or r1,r13
  8C055394  60D3  mov r13,r0
  8C055396  4029  shlr16 r0
  8C055398  6DE3  mov r14,r13
  8C05539A  4001  shlr r0
  8C05539C  A004  bra 8C0553A8
  8C05539E  4D5D  shld r5,r13
  ...
  8C0553A8  6E74  mov.b @r7+,r14
  8C0553AA  6274  mov.b @r7+,r2
  8C0553AC  4E18  shll8 r14
  8C0553AE  6374  mov.b @r7+,r3
  8C0553B0  622C  extu.b r2,r2
  8C0553B2  2E2B  or r2,r14
  8C0553B4  6274  mov.b @r7+,r2
  8C0553B6  633C  extu.b r3,r3
  8C0553B8  4E18  shll8 r14
  8C0553BA  2E3B  or r3,r14
  8C0553BC  622C  extu.b r2,r2
  8C0553BE  4E18  shll8 r14
  8C0553C0  A006  bra 8C0553D0
  8C0553C2  2E2B  or r2,r14
```
