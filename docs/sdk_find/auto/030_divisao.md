# 030_divisao

> Gerado por `tools/sdk_find.py`. Jogos: 14 · variantes (sequências normalizadas distintas): 4 · tempo perf somado: 1.27% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C2438B8` | 234 | 0.00% |
| Dead or Alive 2 (USA) | `8C13F1F8` | 234 | 0.32% |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C0A2A44` | 234 | 0.00% |
| Evolution - The World of Sacred Device (USA) | `8C1FDEF8` | 236 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C20E840` | 236 | 0.00% |
| Grandia II (USA) | `8C10D758` | 234 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C3B6B24` | 234 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C04F9F8` | 80 | 0.00% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C1B8318` | 234 | 0.58% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C1A6FEC` | 234 | 0.37% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C3590B4` | 234 | 0.00% |
| Power Stone (USA) | `0C115FB8` | 236 | 0.00% |
| Project Justice (USA) | `0C2EB4D8` | 234 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C1F40F0` | 236 | 0.00% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C2438B8`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C2438B8  2FE6  mov.l r14,@-r15
  8C2438BA  2FC6  mov.l r12,@-r15
  8C2438BC  2FB6  mov.l r11,@-r15
  8C2438BE  2FA6  mov.l r10,@-r15
  8C2438C0  2F96  mov.l r9,@-r15
  8C2438C2  2F86  mov.l r8,@-r15
  8C2438C4  7FE8  add ##-24,r15
  8C2438C6  6371  mov.w @r7,r3
  8C2438C8  C746  mova @([8C2439E4]),R0
  8C2438CA  5BFD  mov.l @(52,r15),r11
  8C2438CC  6A43  mov r4,r10
  8C2438CE  4328  shll16 r3
  8C2438D0  5CFC  mov.l @(48,r15),r12
  8C2438D2  435A  lds r3,FPUL
  8C2438D4  6373  mov r7,r3
  8C2438D6  7302  add ##2,r3
  8C2438D8  1F34  mov.l r3,@(16,r15)
  8C2438DA  F32D  float FPUL,fr3
  8C2438DC  6231  mov.w @r3,r2
  8C2438DE  4228  shll16 r2
  8C2438E0  425A  lds r2,FPUL
  8C2438E2  62B1  mov.w @r11,r2
  8C2438E4  F93C  fmov fr3,fr9
  8C2438E6  F32D  float FPUL,fr3
  8C2438E8  4228  shll16 r2
  8C2438EA  425A  lds r2,FPUL
  8C2438EC  62B3  mov r11,r2
  8C2438EE  7202  add ##2,r2
  8C2438F0  1F25  mov.l r2,@(20,r15)
  8C2438F2  6121  mov.w @r2,r1
  8C2438F4  F23C  fmov fr3,fr2
  8C2438F6  F32D  float FPUL,fr3
  8C2438F8  4128  shll16 r1
  8C2438FA  F408  fmov.s @r0,fr4
  8C2438FC  415A  lds r1,FPUL
  8C2438FE  E038  mov ##0x38,r0
  8C243900  FA3C  fmov fr3,fr10
  8C243902  F32D  float FPUL,fr3
  8C243904  FB3C  fmov fr3,fr11
  8C243906  01FD  mov.w @(R0,r15),r1
  8C243908  E03C  mov ##0x3C,r0
  8C24390A  E800  mov ##0x00,r8
  8C24390C  4108  shll2 r1
  8C24390E  4108  shll2 r1
  8C243910  415A  lds r1,FPUL
  8C243912  E920  mov ##0x20,r9
  8C243914  01FD  mov.w @(R0,r15),r1
  8C243916  6083  mov r8,r0
  8C243918  2F82  mov.l r8,@r15
  8C24391A  F32D  float FPUL,fr3
  8C24391C  4108  shll2 r1
  8C24391E  4108  shll2 r1
  8C243920  415A  lds r1,FPUL
  8C243922  E101  mov ##0x01,r1
  8C243924  1F12  mov.l r1,@(8,r15)
  8C243926  6153  mov r5,r1
  8C243928  3017  cmp/gt r1,r0
  8C24392A  1F81  mov.l r8,@(4,r15)
  8C24392C  F73C  fmov fr3,fr7
  8C24392E  F32D  float FPUL,fr3
  8C243930  F743  fdiv fr4,fr7
  8C243932  310E  addc r0,r1
  8C243934  4121  shar r1
  8C243936  F83C  fmov fr3,fr8
  8C243938  F843  fdiv fr4,fr8
  8C24393A  A093  bra 8C243A64
  8C24393C  1F13  mov.l r1,@(12,r15)
  8C24393E  64A1  mov.w @r10,r4
  8C243940  E3F8  mov ##0xF8,r3
  8C243942  6243  mov r4,r2
  8C243944  423C  shad r3,r2
  8C243946  6143  mov r4,r1
  8C243948  934A  mov.w @([8C2439E0]),r3
  8C24394A  4118  shll8 r1
  8C24394C  622C  extu.b r2,r2
  8C24394E  6423  mov r2,r4
  8C243950  2139  and r3,r1
  8C243952  241B  or r1,r4
  8C243954  D124  mov.l @([8C2439E8]),r1
  8C243956  624F  exts.w r4,r2
  8C243958  2218  tst r1,r2
  8C24395A  8901  bt 8C243960
  ...
  8C243960  933F  mov.w @([8C2439E2]),r3
  8C243962  644F  exts.w r4,r4
  8C243964  6183  mov r8,r1
  8C243966  7A02  add ##2,r10
  8C243968  2439  and r3,r4
  8C24396A  7401  add ##1,r4
  8C24396C  445A  lds r4,FPUL
  8C24396E  F32D  float FPUL,fr3
  8C243970  F63C  fmov fr3,fr6
  8C243972  F38C  fmov fr8,fr3
  8C243974  F322  fmul fr2,fr3
  8C243976  64A4  mov.b @r10+,r4
  8C243978  F07C  fmov fr7,fr0
  8C24397A  D01C  mov.l @([8C2439EC]),r0
  8C24397C  644C  extu.b r4,r4
  8C24397E  F39E  fmac fr0,fr9,fr3
  8C243980  4408  shll2 r4
  8C243982  F446  fmov.s @(R0,r4),fr4
  8C243984  6E43  mov r4,r14
  8C243986  F06C  fmov fr6,fr0
  8C243988  F59C  fmov fr9,fr5
  8C24398A  F34E  fmac fr0,fr4,fr3
  8C24398C  F07C  fmov fr7,fr0
  8C24398E  D018  mov.l @([8C2439F0]),r0
  8C243990  F13C  fmov fr3,fr1
  8C243992  F13D  ftrc fr1, FPUL
  8C243994  F38C  fmov fr8,fr3
  8C243996  F352  fmul fr5,fr3
  8C243998  F21C  fmov fr1,fr2
  8C24399A  045A  sts FPUL,r4
  8C24399C  F31E  fmac fr0,fr1,fr3
  8C24399E  F06C  fmov fr6,fr0
  8C2439A0  4429  shlr16 r4
  8C2439A2  2641  mov.w r4,@r6
  8C2439A4  7602  add ##2,r6
  8C2439A6  F4E6  fmov.s @(R0,r14),fr4
  8C2439A8  F34E  fmac fr0,fr4,fr3
  8C2439AA  F43C  fmov fr3,fr4
  8C2439AC  F43D  ftrc fr4, FPUL
  8C2439AE  F93C  fmov fr3,fr9
  8C2439B0  045A  sts FPUL,r4
  8C2439B2  4429  shlr16 r4
  8C2439B4  7102  add ##2,r1
  8C2439B6  644F  exts.w r4,r4
  8C2439B8  3193  cmp/ge r9,r1
  8C2439BA  2641  mov.w r4,@r6
  8C2439BC  8FD9  bf.s 8C243972
  8C2439BE  7602  add ##2,r6
  8C2439C0  64A1  mov.w @r10,r4
  8C2439C2  E3F8  mov ##0xF8,r3
  8C2439C4  6E43  mov r4,r14
  8C2439C6  4E3C  shad r3,r14
  8C2439C8  6243  mov r4,r2
  8C2439CA  9309  mov.w @([8C2439E0]),r3
  8C2439CC  4218  shll8 r2
  8C2439CE  2239  and r3,r2
  8C2439D0  6EEC  extu.b r14,r14
  8C2439D2  2E2B  or r2,r14
  8C2439D4  D204  mov.l @([8C2439E8]),r2
  8C2439D6  64EF  exts.w r14,r4
  8C2439D8  2248  tst r4,r2
  8C2439DA  890B  bt 8C2439F4
  ...
  8C2439F4  9358  mov.w @([8C243AA8]),r3
  8C2439F6  6183  mov r8,r1
  8C2439F8  7A02  add ##2,r10
  8C2439FA  2439  and r3,r4
  8C2439FC  7401  add ##1,r4
  8C2439FE  445A  lds r4,FPUL
  8C243A00  F32D  float FPUL,fr3
  8C243A02  F63C  fmov fr3,fr6
  8C243A04  F38C  fmov fr8,fr3
  8C243A06  F3B2  fmul fr11,fr3
  8C243A08  64A4  mov.b @r10+,r4
  8C243A0A  F07C  fmov fr7,fr0
  8C243A0C  D027  mov.l @([8C243AAC]),r0
  8C243A0E  644C  extu.b r4,r4
  8C243A10  F3AE  fmac fr0,fr10,fr3
  8C243A12  4408  shll2 r4
  8C243A14  F446  fmov.s @(R0,r4),fr4
  8C243A16  6E43  mov r4,r14
  8C243A18  F06C  fmov fr6,fr0
  8C243A1A  F5AC  fmov fr10,fr5
  8C243A1C  F34E  fmac fr0,fr4,fr3
  8C243A1E  F07C  fmov fr7,fr0
  8C243A20  D023  mov.l @([8C243AB0]),r0
  8C243A22  F13C  fmov fr3,fr1
  8C243A24  F13D  ftrc fr1, FPUL
  8C243A26  F38C  fmov fr8,fr3
  8C243A28  F352  fmul fr5,fr3
  8C243A2A  FB1C  fmov fr1,fr11
  8C243A2C  045A  sts FPUL,r4
  8C243A2E  F31E  fmac fr0,fr1,fr3
  8C243A30  F06C  fmov fr6,fr0
  8C243A32  4429  shlr16 r4
  8C243A34  2C41  mov.w r4,@r12
  8C243A36  7C02  add ##2,r12
  8C243A38  F4E6  fmov.s @(R0,r14),fr4
  8C243A3A  F34E  fmac fr0,fr4,fr3
  8C243A3C  F43C  fmov fr3,fr4
  8C243A3E  F43D  ftrc fr4, FPUL
  8C243A40  FA3C  fmov fr3,fr10
  8C243A42  045A  sts FPUL,r4
  8C243A44  4429  shlr16 r4
  8C243A46  7102  add ##2,r1
  8C243A48  644F  exts.w r4,r4
  8C243A4A  3193  cmp/ge r9,r1
  8C243A4C  2C41  mov.w r4,@r12
  8C243A4E  8FD9  bf.s 8C243A04
  8C243A50  7C02  add ##2,r12
  8C243A52  62F2  mov.l @r15,r2
  8C243A54  7201  add ##1,r2
  8C243A56  2F22  mov.l r2,@r15
  8C243A58  53F2  mov.l @(8,r15),r3
  8C243A5A  7302  add ##2,r3
  8C243A5C  1F32  mov.l r3,@(8,r15)
  8C243A5E  51F1  mov.l @(4,r15),r1
  8C243A60  7102  add ##2,r1
  8C243A62  1F11  mov.l r1,@(4,r15)
  8C243A64  62F2  mov.l @r15,r2
  8C243A66  53F3  mov.l @(12,r15),r3
  8C243A68  3233  cmp/ge r3,r2
  8C243A6A  8901  bt 8C243A70
  8C243A6C  AF67  bra 8C24393E
  8C243A6E  0009  nop
  8C243A70  F93D  ftrc fr9, FPUL
  8C243A72  6053  mov r5,r0
  8C243A74  035A  sts FPUL,r3
  8C243A76  F23D  ftrc fr2, FPUL
  8C243A78  4329  shlr16 r3
  8C243A7A  2731  mov.w r3,@r7
  8C243A7C  035A  sts FPUL,r3
  8C243A7E  FA3D  ftrc fr10, FPUL
  8C243A80  51F4  mov.l @(16,r15),r1
  8C243A82  4329  shlr16 r3
  8C243A84  2131  mov.w r3,@r1
  8C243A86  015A  sts FPUL,r1
  8C243A88  FB3D  ftrc fr11, FPUL
  8C243A8A  4129  shlr16 r1
  8C243A8C  035A  sts FPUL,r3
  8C243A8E  611F  exts.w r1,r1
  8C243A90  2B11  mov.w r1,@r11
  8C243A92  52F5  mov.l @(20,r15),r2
  8C243A94  4329  shlr16 r3
  8C243A96  2231  mov.w r3,@r2
  8C243A98  7F18  add ##24,r15
  8C243A9A  68F6  mov.l @r15+,r8
  8C243A9C  69F6  mov.l @r15+,r9
  8C243A9E  6AF6  mov.l @r15+,r10
  8C243AA0  6BF6  mov.l @r15+,r11
  8C243AA2  6CF6  mov.l @r15+,r12
  8C243AA4  000B  rts
  8C243AA6  6EF6  mov.l @r15+,r14
```

## Evolution - The World of Sacred Device (USA) `8C1FDEF8`

Dump: `/mnt/1TB/dcbat/20261007-151806_Evolution_-_The_World_of_Sacred_Device__/jit-24035.txt`

```
  8C1FDEF8  2FE6  mov.l r14,@-r15
  8C1FDEFA  2FC6  mov.l r12,@-r15
  8C1FDEFC  2FB6  mov.l r11,@-r15
  8C1FDEFE  2FA6  mov.l r10,@-r15
  8C1FDF00  2F96  mov.l r9,@-r15
  8C1FDF02  2F86  mov.l r8,@-r15
  8C1FDF04  7FE8  add ##-24,r15
  8C1FDF06  6371  mov.w @r7,r3
  8C1FDF08  C746  mova @([8C1FE024]),R0
  8C1FDF0A  5BFD  mov.l @(52,r15),r11
  8C1FDF0C  6A43  mov r4,r10
  8C1FDF0E  4328  shll16 r3
  8C1FDF10  5CFC  mov.l @(48,r15),r12
  8C1FDF12  435A  lds r3,FPUL
  8C1FDF14  6373  mov r7,r3
  8C1FDF16  7302  add ##2,r3
  8C1FDF18  1F34  mov.l r3,@(16,r15)
  8C1FDF1A  F32D  float FPUL,fr3
  8C1FDF1C  6231  mov.w @r3,r2
  8C1FDF1E  4228  shll16 r2
  8C1FDF20  425A  lds r2,FPUL
  8C1FDF22  62B1  mov.w @r11,r2
  8C1FDF24  F93C  fmov fr3,fr9
  8C1FDF26  F32D  float FPUL,fr3
  8C1FDF28  4228  shll16 r2
  8C1FDF2A  425A  lds r2,FPUL
  8C1FDF2C  62B3  mov r11,r2
  8C1FDF2E  7202  add ##2,r2
  8C1FDF30  1F25  mov.l r2,@(20,r15)
  8C1FDF32  6121  mov.w @r2,r1
  8C1FDF34  F23C  fmov fr3,fr2
  8C1FDF36  F32D  float FPUL,fr3
  8C1FDF38  4128  shll16 r1
  8C1FDF3A  F408  fmov.s @r0,fr4
  8C1FDF3C  415A  lds r1,FPUL
  8C1FDF3E  E038  mov ##0x38,r0
  8C1FDF40  FA3C  fmov fr3,fr10
  8C1FDF42  F32D  float FPUL,fr3
  8C1FDF44  FB3C  fmov fr3,fr11
  8C1FDF46  01FD  mov.w @(R0,r15),r1
  8C1FDF48  E03C  mov ##0x3C,r0
  8C1FDF4A  E800  mov ##0x00,r8
  8C1FDF4C  4108  shll2 r1
  8C1FDF4E  4108  shll2 r1
  8C1FDF50  415A  lds r1,FPUL
  8C1FDF52  E920  mov ##0x20,r9
  8C1FDF54  01FD  mov.w @(R0,r15),r1
  8C1FDF56  6083  mov r8,r0
  8C1FDF58  2F82  mov.l r8,@r15
  8C1FDF5A  F32D  float FPUL,fr3
  8C1FDF5C  4108  shll2 r1
  8C1FDF5E  4108  shll2 r1
  8C1FDF60  415A  lds r1,FPUL
  8C1FDF62  E101  mov ##0x01,r1
  8C1FDF64  1F12  mov.l r1,@(8,r15)
  8C1FDF66  6153  mov r5,r1
  8C1FDF68  3017  cmp/gt r1,r0
  8C1FDF6A  1F81  mov.l r8,@(4,r15)
  8C1FDF6C  F73C  fmov fr3,fr7
  8C1FDF6E  F32D  float FPUL,fr3
  8C1FDF70  F743  fdiv fr4,fr7
  8C1FDF72  310E  addc r0,r1
  8C1FDF74  4121  shar r1
  8C1FDF76  F83C  fmov fr3,fr8
  8C1FDF78  F843  fdiv fr4,fr8
  8C1FDF7A  A093  bra 8C1FE0A4
  8C1FDF7C  1F13  mov.l r1,@(12,r15)
  8C1FDF7E  64A1  mov.w @r10,r4
  8C1FDF80  E3F8  mov ##0xF8,r3
  8C1FDF82  6243  mov r4,r2
  8C1FDF84  423C  shad r3,r2
  8C1FDF86  6143  mov r4,r1
  8C1FDF88  934A  mov.w @([8C1FE020]),r3
  8C1FDF8A  4118  shll8 r1
  8C1FDF8C  622C  extu.b r2,r2
  8C1FDF8E  6423  mov r2,r4
  8C1FDF90  2139  and r3,r1
  8C1FDF92  241B  or r1,r4
  8C1FDF94  D124  mov.l @([8C1FE028]),r1
  8C1FDF96  624F  exts.w r4,r2
  8C1FDF98  2218  tst r1,r2
  8C1FDF9A  8901  bt 8C1FDFA0
  8C1FDF9C  A09C  bra 8C1FE0D8
  8C1FDF9E  50F1  mov.l @(4,r15),r0
  8C1FDFA0  933F  mov.w @([8C1FE022]),r3
  8C1FDFA2  644F  exts.w r4,r4
  8C1FDFA4  6183  mov r8,r1
  8C1FDFA6  7A02  add ##2,r10
  8C1FDFA8  2439  and r3,r4
  8C1FDFAA  7401  add ##1,r4
  8C1FDFAC  445A  lds r4,FPUL
  8C1FDFAE  F32D  float FPUL,fr3
  8C1FDFB0  F63C  fmov fr3,fr6
  8C1FDFB2  F38C  fmov fr8,fr3
  8C1FDFB4  F322  fmul fr2,fr3
  8C1FDFB6  64A4  mov.b @r10+,r4
  8C1FDFB8  F07C  fmov fr7,fr0
  8C1FDFBA  D01C  mov.l @([8C1FE02C]),r0
  8C1FDFBC  644C  extu.b r4,r4
  8C1FDFBE  F39E  fmac fr0,fr9,fr3
  8C1FDFC0  4408  shll2 r4
  8C1FDFC2  F446  fmov.s @(R0,r4),fr4
  8C1FDFC4  6E43  mov r4,r14
  8C1FDFC6  F06C  fmov fr6,fr0
  8C1FDFC8  F59C  fmov fr9,fr5
  8C1FDFCA  F34E  fmac fr0,fr4,fr3
  8C1FDFCC  F07C  fmov fr7,fr0
  8C1FDFCE  D018  mov.l @([8C1FE030]),r0
  8C1FDFD0  F13C  fmov fr3,fr1
  8C1FDFD2  F13D  ftrc fr1, FPUL
  8C1FDFD4  F38C  fmov fr8,fr3
  8C1FDFD6  F352  fmul fr5,fr3
  8C1FDFD8  F21C  fmov fr1,fr2
  8C1FDFDA  045A  sts FPUL,r4
  8C1FDFDC  F31E  fmac fr0,fr1,fr3
  8C1FDFDE  F06C  fmov fr6,fr0
  8C1FDFE0  4429  shlr16 r4
  8C1FDFE2  2641  mov.w r4,@r6
  8C1FDFE4  7602  add ##2,r6
  8C1FDFE6  F4E6  fmov.s @(R0,r14),fr4
  8C1FDFE8  F34E  fmac fr0,fr4,fr3
  8C1FDFEA  F43C  fmov fr3,fr4
  8C1FDFEC  F43D  ftrc fr4, FPUL
  8C1FDFEE  F93C  fmov fr3,fr9
  8C1FDFF0  045A  sts FPUL,r4
  8C1FDFF2  4429  shlr16 r4
  8C1FDFF4  7102  add ##2,r1
  8C1FDFF6  644F  exts.w r4,r4
  8C1FDFF8  3193  cmp/ge r9,r1
  8C1FDFFA  2641  mov.w r4,@r6
  8C1FDFFC  8FD9  bf.s 8C1FDFB2
  8C1FDFFE  7602  add ##2,r6
  8C1FE000  64A1  mov.w @r10,r4
  8C1FE002  E3F8  mov ##0xF8,r3
  8C1FE004  6E43  mov r4,r14
  8C1FE006  4E3C  shad r3,r14
  8C1FE008  6243  mov r4,r2
  8C1FE00A  9309  mov.w @([8C1FE020]),r3
  8C1FE00C  4218  shll8 r2
  8C1FE00E  2239  and r3,r2
  8C1FE010  6EEC  extu.b r14,r14
  8C1FE012  2E2B  or r2,r14
  8C1FE014  D204  mov.l @([8C1FE028]),r2
  8C1FE016  64EF  exts.w r14,r4
  8C1FE018  2248  tst r4,r2
  8C1FE01A  890B  bt 8C1FE034
  ...
  8C1FE034  9358  mov.w @([8C1FE0E8]),r3
  8C1FE036  6183  mov r8,r1
  8C1FE038  7A02  add ##2,r10
  8C1FE03A  2439  and r3,r4
  8C1FE03C  7401  add ##1,r4
  8C1FE03E  445A  lds r4,FPUL
  8C1FE040  F32D  float FPUL,fr3
  8C1FE042  F63C  fmov fr3,fr6
  8C1FE044  F38C  fmov fr8,fr3
  8C1FE046  F3B2  fmul fr11,fr3
  8C1FE048  64A4  mov.b @r10+,r4
  8C1FE04A  F07C  fmov fr7,fr0
  8C1FE04C  D027  mov.l @([8C1FE0EC]),r0
  8C1FE04E  644C  extu.b r4,r4
  8C1FE050  F3AE  fmac fr0,fr10,fr3
  8C1FE052  4408  shll2 r4
  8C1FE054  F446  fmov.s @(R0,r4),fr4
  8C1FE056  6E43  mov r4,r14
  8C1FE058  F06C  fmov fr6,fr0
  8C1FE05A  F5AC  fmov fr10,fr5
  8C1FE05C  F34E  fmac fr0,fr4,fr3
  8C1FE05E  F07C  fmov fr7,fr0
  8C1FE060  D023  mov.l @([8C1FE0F0]),r0
  8C1FE062  F13C  fmov fr3,fr1
  8C1FE064  F13D  ftrc fr1, FPUL
  8C1FE066  F38C  fmov fr8,fr3
  8C1FE068  F352  fmul fr5,fr3
  8C1FE06A  FB1C  fmov fr1,fr11
  8C1FE06C  045A  sts FPUL,r4
  8C1FE06E  F31E  fmac fr0,fr1,fr3
  8C1FE070  F06C  fmov fr6,fr0
  8C1FE072  4429  shlr16 r4
  8C1FE074  2C41  mov.w r4,@r12
  8C1FE076  7C02  add ##2,r12
  8C1FE078  F4E6  fmov.s @(R0,r14),fr4
  8C1FE07A  F34E  fmac fr0,fr4,fr3
  8C1FE07C  F43C  fmov fr3,fr4
  8C1FE07E  F43D  ftrc fr4, FPUL
  8C1FE080  FA3C  fmov fr3,fr10
  8C1FE082  045A  sts FPUL,r4
  8C1FE084  4429  shlr16 r4
  8C1FE086  7102  add ##2,r1
  8C1FE088  644F  exts.w r4,r4
  8C1FE08A  3193  cmp/ge r9,r1
  8C1FE08C  2C41  mov.w r4,@r12
  8C1FE08E  8FD9  bf.s 8C1FE044
  8C1FE090  7C02  add ##2,r12
  8C1FE092  62F2  mov.l @r15,r2
  8C1FE094  7201  add ##1,r2
  8C1FE096  2F22  mov.l r2,@r15
  8C1FE098  53F2  mov.l @(8,r15),r3
  8C1FE09A  7302  add ##2,r3
  8C1FE09C  1F32  mov.l r3,@(8,r15)
  8C1FE09E  51F1  mov.l @(4,r15),r1
  8C1FE0A0  7102  add ##2,r1
  8C1FE0A2  1F11  mov.l r1,@(4,r15)
  8C1FE0A4  62F2  mov.l @r15,r2
  8C1FE0A6  53F3  mov.l @(12,r15),r3
  8C1FE0A8  3233  cmp/ge r3,r2
  8C1FE0AA  8901  bt 8C1FE0B0
  8C1FE0AC  AF67  bra 8C1FDF7E
  8C1FE0AE  0009  nop
  8C1FE0B0  F93D  ftrc fr9, FPUL
  8C1FE0B2  6053  mov r5,r0
  8C1FE0B4  035A  sts FPUL,r3
  8C1FE0B6  F23D  ftrc fr2, FPUL
  8C1FE0B8  4329  shlr16 r3
  8C1FE0BA  2731  mov.w r3,@r7
  8C1FE0BC  035A  sts FPUL,r3
  8C1FE0BE  FA3D  ftrc fr10, FPUL
  8C1FE0C0  51F4  mov.l @(16,r15),r1
  8C1FE0C2  4329  shlr16 r3
  8C1FE0C4  2131  mov.w r3,@r1
  8C1FE0C6  015A  sts FPUL,r1
  8C1FE0C8  FB3D  ftrc fr11, FPUL
  8C1FE0CA  4129  shlr16 r1
  8C1FE0CC  035A  sts FPUL,r3
  8C1FE0CE  611F  exts.w r1,r1
  8C1FE0D0  2B11  mov.w r1,@r11
  8C1FE0D2  52F5  mov.l @(20,r15),r2
  8C1FE0D4  4329  shlr16 r3
  8C1FE0D6  2231  mov.w r3,@r2
  8C1FE0D8  7F18  add ##24,r15
  8C1FE0DA  68F6  mov.l @r15+,r8
  8C1FE0DC  69F6  mov.l @r15+,r9
  8C1FE0DE  6AF6  mov.l @r15+,r10
  8C1FE0E0  6BF6  mov.l @r15+,r11
  8C1FE0E2  6CF6  mov.l @r15+,r12
  8C1FE0E4  000B  rts
  8C1FE0E6  6EF6  mov.l @r15+,r14
```

## Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) `8C04F9F8`

Dump: `/mnt/1TB/dcbat/20261007-163759_Le_Mans_24_Hours__Europe___En_Fr_De_Es_I/jit-109461.txt`

```
  8C04F9F8  933F  mov.w @([8C04FA7A]),r3
  8C04F9FA  644F  exts.w r4,r4
  8C04F9FC  6183  mov r8,r1
  8C04F9FE  7A02  add ##2,r10
  8C04FA00  2439  and r3,r4
  8C04FA02  7401  add ##1,r4
  8C04FA04  445A  lds r4,FPUL
  8C04FA06  F32D  float FPUL,fr3
  8C04FA08  F63C  fmov fr3,fr6
  8C04FA0A  F38C  fmov fr8,fr3
  8C04FA0C  F322  fmul fr2,fr3
  8C04FA0E  64A4  mov.b @r10+,r4
  8C04FA10  F07C  fmov fr7,fr0
  8C04FA12  D01C  mov.l @([8C04FA84]),r0
  8C04FA14  644C  extu.b r4,r4
  8C04FA16  F39E  fmac fr0,fr9,fr3
  8C04FA18  4408  shll2 r4
  8C04FA1A  F446  fmov.s @(R0,r4),fr4
  8C04FA1C  6E43  mov r4,r14
  8C04FA1E  F06C  fmov fr6,fr0
  8C04FA20  F59C  fmov fr9,fr5
  8C04FA22  F34E  fmac fr0,fr4,fr3
  8C04FA24  F07C  fmov fr7,fr0
  8C04FA26  D018  mov.l @([8C04FA88]),r0
  8C04FA28  F13C  fmov fr3,fr1
  8C04FA2A  F13D  ftrc fr1, FPUL
  8C04FA2C  F38C  fmov fr8,fr3
  8C04FA2E  F352  fmul fr5,fr3
  8C04FA30  F21C  fmov fr1,fr2
  8C04FA32  045A  sts FPUL,r4
  8C04FA34  F31E  fmac fr0,fr1,fr3
  8C04FA36  F06C  fmov fr6,fr0
  8C04FA38  4429  shlr16 r4
  8C04FA3A  2641  mov.w r4,@r6
  8C04FA3C  7602  add ##2,r6
  8C04FA3E  F4E6  fmov.s @(R0,r14),fr4
  8C04FA40  F34E  fmac fr0,fr4,fr3
  8C04FA42  F43C  fmov fr3,fr4
  8C04FA44  F43D  ftrc fr4, FPUL
  8C04FA46  F93C  fmov fr3,fr9
  8C04FA48  045A  sts FPUL,r4
  8C04FA4A  4429  shlr16 r4
  8C04FA4C  7102  add ##2,r1
  8C04FA4E  644F  exts.w r4,r4
  8C04FA50  3193  cmp/ge r9,r1
  8C04FA52  2641  mov.w r4,@r6
  8C04FA54  8FD9  bf.s 8C04FA0A
  8C04FA56  7602  add ##2,r6
  8C04FA58  64A1  mov.w @r10,r4
  8C04FA5A  E3F8  mov ##0xF8,r3
  8C04FA5C  6E43  mov r4,r14
  8C04FA5E  4E3C  shad r3,r14
  8C04FA60  6243  mov r4,r2
  8C04FA62  9309  mov.w @([8C04FA78]),r3
  8C04FA64  4218  shll8 r2
  8C04FA66  2239  and r3,r2
  8C04FA68  6EEC  extu.b r14,r14
  8C04FA6A  2E2B  or r2,r14
  8C04FA6C  D204  mov.l @([8C04FA80]),r2
  8C04FA6E  64EF  exts.w r14,r4
  8C04FA70  2248  tst r4,r2
  8C04FA72  890B  bt 8C04FA8C
  ...
  8C04FA8C  9358  mov.w @([8C04FB40]),r3
  8C04FA8E  6183  mov r8,r1
  8C04FA90  7A02  add ##2,r10
  8C04FA92  2439  and r3,r4
  8C04FA94  7401  add ##1,r4
  8C04FA96  445A  lds r4,FPUL
  8C04FA98  F32D  float FPUL,fr3
  8C04FA9A  F63C  fmov fr3,fr6
  8C04FA9C  F38C  fmov fr8,fr3
  8C04FA9E  F3B2  fmul fr11,fr3
  8C04FAA0  64A4  mov.b @r10+,r4
  8C04FAA2  F07C  fmov fr7,fr0
  8C04FAA4  D027  mov.l @([8C04FB44]),r0
  8C04FAA6  644C  extu.b r4,r4
  8C04FAA8  F3AE  fmac fr0,fr10,fr3
  8C04FAAA  4408  shll2 r4
  8C04FAAC  F446  fmov.s @(R0,r4),fr4
  8C04FAAE  6E43  mov r4,r14
```

## Power Stone (USA) `0C115FB8`

Dump: `/mnt/1TB/dcbat/20261007-160119_Power_Stone__USA__/jit-75898.txt`

```
  0C115FB8  2FE6  mov.l r14,@-r15
  0C115FBA  2FC6  mov.l r12,@-r15
  0C115FBC  2FB6  mov.l r11,@-r15
  0C115FBE  2FA6  mov.l r10,@-r15
  0C115FC0  2F96  mov.l r9,@-r15
  0C115FC2  2F86  mov.l r8,@-r15
  0C115FC4  7FE8  add ##-24,r15
  0C115FC6  6371  mov.w @r7,r3
  0C115FC8  C747  mova @([0C1160E8]),R0
  0C115FCA  5BFD  mov.l @(52,r15),r11
  0C115FCC  6A43  mov r4,r10
  0C115FCE  4328  shll16 r3
  0C115FD0  5CFC  mov.l @(48,r15),r12
  0C115FD2  435A  lds r3,FPUL
  0C115FD4  6373  mov r7,r3
  0C115FD6  7302  add ##2,r3
  0C115FD8  1F34  mov.l r3,@(16,r15)
  0C115FDA  F32D  float FPUL,fr3
  0C115FDC  6231  mov.w @r3,r2
  0C115FDE  4228  shll16 r2
  0C115FE0  425A  lds r2,FPUL
  0C115FE2  62B1  mov.w @r11,r2
  0C115FE4  F93C  fmov fr3,fr9
  0C115FE6  F32D  float FPUL,fr3
  0C115FE8  4228  shll16 r2
  0C115FEA  425A  lds r2,FPUL
  0C115FEC  62B3  mov r11,r2
  0C115FEE  7202  add ##2,r2
  0C115FF0  1F25  mov.l r2,@(20,r15)
  0C115FF2  6121  mov.w @r2,r1
  0C115FF4  F23C  fmov fr3,fr2
  0C115FF6  F32D  float FPUL,fr3
  0C115FF8  4128  shll16 r1
  0C115FFA  F408  fmov.s @r0,fr4
  0C115FFC  415A  lds r1,FPUL
  0C115FFE  E038  mov ##0x38,r0
  0C116000  FA3C  fmov fr3,fr10
  0C116002  F32D  float FPUL,fr3
  0C116004  FB3C  fmov fr3,fr11
  0C116006  01FD  mov.w @(R0,r15),r1
  0C116008  E03C  mov ##0x3C,r0
  0C11600A  E800  mov ##0x00,r8
  0C11600C  4108  shll2 r1
  0C11600E  4108  shll2 r1
  0C116010  415A  lds r1,FPUL
  0C116012  E920  mov ##0x20,r9
  0C116014  01FD  mov.w @(R0,r15),r1
  0C116016  2F82  mov.l r8,@r15
  0C116018  F32D  float FPUL,fr3
  0C11601A  4108  shll2 r1
  0C11601C  4108  shll2 r1
  0C11601E  415A  lds r1,FPUL
  0C116020  E101  mov ##0x01,r1
  0C116022  1F12  mov.l r1,@(8,r15)
  0C116024  6153  mov r5,r1
  0C116026  1F81  mov.l r8,@(4,r15)
  0C116028  F73C  fmov fr3,fr7
  0C11602A  F32D  float FPUL,fr3
  0C11602C  F743  fdiv fr4,fr7
  0C11602E  F83C  fmov fr3,fr8
  0C116030  F843  fdiv fr4,fr8
  0C116032  6083  mov r8,r0
  0C116034  0009  nop
  0C116036  3017  cmp/gt r1,r0
  0C116038  310E  addc r0,r1
  0C11603A  4121  shar r1
  0C11603C  A094  bra 0C116168
  0C11603E  1F13  mov.l r1,@(12,r15)
  0C116040  64A1  mov.w @r10,r4
  0C116042  E3F8  mov ##0xF8,r3
  0C116044  6243  mov r4,r2
  0C116046  423C  shad r3,r2
  0C116048  6143  mov r4,r1
  0C11604A  934A  mov.w @([0C1160E2]),r3
  0C11604C  4118  shll8 r1
  0C11604E  622C  extu.b r2,r2
  0C116050  6423  mov r2,r4
  0C116052  2139  and r3,r1
  0C116054  241B  or r1,r4
  0C116056  D125  mov.l @([0C1160EC]),r1
  0C116058  624F  exts.w r4,r2
  0C11605A  2218  tst r1,r2
  0C11605C  8901  bt 0C116062
  ...
  0C116062  933F  mov.w @([0C1160E4]),r3
  0C116064  644F  exts.w r4,r4
  0C116066  6183  mov r8,r1
  0C116068  7A02  add ##2,r10
  0C11606A  2439  and r3,r4
  0C11606C  7401  add ##1,r4
  0C11606E  445A  lds r4,FPUL
  0C116070  F32D  float FPUL,fr3
  0C116072  F63C  fmov fr3,fr6
  0C116074  F38C  fmov fr8,fr3
  0C116076  F322  fmul fr2,fr3
  0C116078  64A4  mov.b @r10+,r4
  0C11607A  F07C  fmov fr7,fr0
  0C11607C  D01C  mov.l @([0C1160F0]),r0
  0C11607E  644C  extu.b r4,r4
  0C116080  F39E  fmac fr0,fr9,fr3
  0C116082  4408  shll2 r4
  0C116084  F446  fmov.s @(R0,r4),fr4
  0C116086  6E43  mov r4,r14
  0C116088  F06C  fmov fr6,fr0
  0C11608A  F59C  fmov fr9,fr5
  0C11608C  F34E  fmac fr0,fr4,fr3
  0C11608E  F07C  fmov fr7,fr0
  0C116090  D018  mov.l @([0C1160F4]),r0
  0C116092  F13C  fmov fr3,fr1
  0C116094  F13D  ftrc fr1, FPUL
  0C116096  F38C  fmov fr8,fr3
  0C116098  F352  fmul fr5,fr3
  0C11609A  F21C  fmov fr1,fr2
  0C11609C  045A  sts FPUL,r4
  0C11609E  F31E  fmac fr0,fr1,fr3
  0C1160A0  F06C  fmov fr6,fr0
  0C1160A2  4429  shlr16 r4
  0C1160A4  2641  mov.w r4,@r6
  0C1160A6  7602  add ##2,r6
  0C1160A8  F4E6  fmov.s @(R0,r14),fr4
  0C1160AA  F34E  fmac fr0,fr4,fr3
  0C1160AC  F43C  fmov fr3,fr4
  0C1160AE  F43D  ftrc fr4, FPUL
  0C1160B0  F93C  fmov fr3,fr9
  0C1160B2  045A  sts FPUL,r4
  0C1160B4  4429  shlr16 r4
  0C1160B6  7102  add ##2,r1
  0C1160B8  644F  exts.w r4,r4
  0C1160BA  3193  cmp/ge r9,r1
  0C1160BC  2641  mov.w r4,@r6
  0C1160BE  8FD9  bf.s 0C116074
  0C1160C0  7602  add ##2,r6
  0C1160C2  64A1  mov.w @r10,r4
  0C1160C4  E3F8  mov ##0xF8,r3
  0C1160C6  6E43  mov r4,r14
  0C1160C8  4E3C  shad r3,r14
  0C1160CA  6243  mov r4,r2
  0C1160CC  9309  mov.w @([0C1160E2]),r3
  0C1160CE  4218  shll8 r2
  0C1160D0  2239  and r3,r2
  0C1160D2  6EEC  extu.b r14,r14
  0C1160D4  2E2B  or r2,r14
  0C1160D6  D205  mov.l @([0C1160EC]),r2
  0C1160D8  64EF  exts.w r14,r4
  0C1160DA  2248  tst r4,r2
  0C1160DC  890C  bt 0C1160F8
  ...
  0C1160F8  9359  mov.w @([0C1161AE]),r3
  0C1160FA  6183  mov r8,r1
  0C1160FC  7A02  add ##2,r10
  0C1160FE  2439  and r3,r4
  0C116100  7401  add ##1,r4
  0C116102  445A  lds r4,FPUL
  0C116104  F32D  float FPUL,fr3
  0C116106  F63C  fmov fr3,fr6
  0C116108  F38C  fmov fr8,fr3
  0C11610A  F3B2  fmul fr11,fr3
  0C11610C  64A4  mov.b @r10+,r4
  0C11610E  F07C  fmov fr7,fr0
  0C116110  D027  mov.l @([0C1161B0]),r0
  0C116112  644C  extu.b r4,r4
  0C116114  F3AE  fmac fr0,fr10,fr3
  0C116116  4408  shll2 r4
  0C116118  F446  fmov.s @(R0,r4),fr4
  0C11611A  6E43  mov r4,r14
  0C11611C  F06C  fmov fr6,fr0
  0C11611E  F5AC  fmov fr10,fr5
  0C116120  F34E  fmac fr0,fr4,fr3
  0C116122  F07C  fmov fr7,fr0
  0C116124  D023  mov.l @([0C1161B4]),r0
  0C116126  F13C  fmov fr3,fr1
  0C116128  F13D  ftrc fr1, FPUL
  0C11612A  F38C  fmov fr8,fr3
  0C11612C  F352  fmul fr5,fr3
  0C11612E  FB1C  fmov fr1,fr11
  0C116130  045A  sts FPUL,r4
  0C116132  F31E  fmac fr0,fr1,fr3
  0C116134  F06C  fmov fr6,fr0
  0C116136  4429  shlr16 r4
  0C116138  2C41  mov.w r4,@r12
  0C11613A  7C02  add ##2,r12
  0C11613C  F4E6  fmov.s @(R0,r14),fr4
  0C11613E  F34E  fmac fr0,fr4,fr3
  0C116140  F43C  fmov fr3,fr4
  0C116142  F43D  ftrc fr4, FPUL
  0C116144  FA3C  fmov fr3,fr10
  0C116146  045A  sts FPUL,r4
  0C116148  4429  shlr16 r4
  0C11614A  7102  add ##2,r1
  0C11614C  644F  exts.w r4,r4
  0C11614E  3193  cmp/ge r9,r1
  0C116150  2C41  mov.w r4,@r12
  0C116152  8FD9  bf.s 0C116108
  0C116154  7C02  add ##2,r12
  0C116156  63F2  mov.l @r15,r3
  0C116158  7301  add ##1,r3
  0C11615A  2F32  mov.l r3,@r15
  0C11615C  52F2  mov.l @(8,r15),r2
  0C11615E  7202  add ##2,r2
  0C116160  1F22  mov.l r2,@(8,r15)
  0C116162  51F1  mov.l @(4,r15),r1
  0C116164  7102  add ##2,r1
  0C116166  1F11  mov.l r1,@(4,r15)
  0C116168  62F2  mov.l @r15,r2
  0C11616A  53F3  mov.l @(12,r15),r3
  0C11616C  3233  cmp/ge r3,r2
  0C11616E  8901  bt 0C116174
  0C116170  AF66  bra 0C116040
  0C116172  0009  nop
  0C116174  F93D  ftrc fr9, FPUL
  0C116176  035A  sts FPUL,r3
  0C116178  F23D  ftrc fr2, FPUL
  0C11617A  4329  shlr16 r3
  0C11617C  2731  mov.w r3,@r7
  0C11617E  035A  sts FPUL,r3
  0C116180  FA3D  ftrc fr10, FPUL
  0C116182  52F4  mov.l @(16,r15),r2
  0C116184  4329  shlr16 r3
  0C116186  633F  exts.w r3,r3
  0C116188  2231  mov.w r3,@r2
  0C11618A  025A  sts FPUL,r2
  0C11618C  FB3D  ftrc fr11, FPUL
  0C11618E  4229  shlr16 r2
  0C116190  2B21  mov.w r2,@r11
  0C116192  025A  sts FPUL,r2
  0C116194  53F5  mov.l @(20,r15),r3
  0C116196  4229  shlr16 r2
  0C116198  2321  mov.w r2,@r3
  0C11619A  6053  mov r5,r0
  0C11619C  0009  nop
  0C11619E  7F18  add ##24,r15
  0C1161A0  68F6  mov.l @r15+,r8
  0C1161A2  69F6  mov.l @r15+,r9
  0C1161A4  6AF6  mov.l @r15+,r10
  0C1161A6  6BF6  mov.l @r15+,r11
  0C1161A8  6CF6  mov.l @r15+,r12
  0C1161AA  000B  rts
  0C1161AC  6EF6  mov.l @r15+,r14
```
