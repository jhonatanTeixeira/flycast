# 037_contexto

> Gerado por `tools/sdk_find.py`. Jogos: 18 · variantes (sequências normalizadas distintas): 6 · tempo perf somado: 1.11% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C220000` | 193 | 0.00% |
| Dead or Alive 2 (USA) | `8C13420C` | 193 | 0.17% |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C092FB8` | 185 | 0.47% |
| Evolution - The World of Sacred Device (USA) | `8C1DA274` | 193 | 0.00% |
| Evolution 2 - Far Off Promise (USA) | `8C1AE030` | 193 | 0.00% |
| Grandia II (USA) | `8C0C6850` | 193 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C36BBD4` | 193 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C04C4A0` | 24 | 0.00% |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C21FE50` | 168 | 0.00% |
| Macross M3 | `8C1D6398` | 193 | 0.00% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C1969B8` | 193 | 0.36% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C169274` | 193 | 0.11% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C017F28` | 168 | 0.00% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C3799D8` | 193 | 0.00% |
| Power Stone (USA) | `0C1110B8` | 185 | 0.00% |
| Project Justice (USA) | `0C2E046C` | 193 | 0.00% |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C1D4B50` | 193 | 0.00% |
| Skies of Arcadia (USA) (Disc 1) | `8C2E68A4` | 168 | 0.00% |
| Sonic Adventure 2 (USA) (EnJaFrDeEs) | `8C15AD08` | 169 | 0.00% |
| Soulcalibur (USA) | `8C241420` | 160 | 0.00% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C220000`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C220000  6103  mov r0,r1
  8C220002  C76B  mova @([8C2201B0]),R0
  8C220004  2016  mov.l r1,@-r0
  8C220006  70FC  add ##-4,r0
  8C220008  6102  mov.l @r0,r1
  8C22000A  7101  add ##1,r1
  8C22000C  2012  mov.l r1,@r0
  8C22000E  7004  add ##4,r0
  8C220010  4110  dt r1
  8C220012  6002  mov.l @r0,r0
  8C220014  8F07  bf.s 8C220026
  8C220016  61F6  mov.l @r15+,r1
  8C220018  2F06  mov.l r0,@-r15
  8C22001A  C760  mova @([8C22019C]),R0
  8C22001C  7F04  add ##4,r15
  8C22001E  20F2  mov.l r15,@r0
  8C220020  7FFC  add ##-4,r15
  8C220022  60F6  mov.l @r15+,r0
  8C220024  DF5E  mov.l @([8C2201A0]),r15
  8C220026  4F33  stc.l SSR,@-r15
  8C220028  4F43  stc.l SPC,@-r15
  8C22002A  4F22  sts.l PR,@-r15
  8C22002C  4F13  stc.l GBR,@-r15
  8C22002E  4F23  stc.l VBR,@-r15
  8C220030  4F02  sts.l MACH,@-r15
  8C220032  4F12  sts.l MACL,@-r15
  8C220034  2FE6  mov.l r14,@-r15
  8C220036  2FD6  mov.l r13,@-r15
  8C220038  2FC6  mov.l r12,@-r15
  8C22003A  2FB6  mov.l r11,@-r15
  8C22003C  2FA6  mov.l r10,@-r15
  8C22003E  2F96  mov.l r9,@-r15
  8C220040  2F86  mov.l r8,@-r15
  8C220042  2F76  mov.l r7,@-r15
  8C220044  2F66  mov.l r6,@-r15
  8C220046  2F56  mov.l r5,@-r15
  8C220048  2F46  mov.l r4,@-r15
  8C22004A  2F36  mov.l r3,@-r15
  8C22004C  2F26  mov.l r2,@-r15
  8C22004E  2F16  mov.l r1,@-r15
  8C220050  2F06  mov.l r0,@-r15
  8C220052  4F03  stc.l SR,@-r15
  8C220054  4F52  sts.l FPUL,@-r15
  8C220056  4F62  sts.l FPSCR,@-r15
  8C220058  E004  mov ##0x04,r0
  8C22005A  4028  shll16 r0
  8C22005C  406A  lds r0,FPSCR
  8C22005E  200A  xor r0,r0
  8C220060  405A  lds r0,FPUL
  8C220062  FFFB  fmov.s fr15,@-r15
  8C220064  FFEB  fmov.s fr14,@-r15
  8C220066  FFDB  fmov.s fr13,@-r15
  8C220068  FFCB  fmov.s fr12,@-r15
  8C22006A  FFBB  fmov.s fr11,@-r15
  8C22006C  FFAB  fmov.s fr10,@-r15
  8C22006E  FF9B  fmov.s fr9,@-r15
  8C220070  FF8B  fmov.s fr8,@-r15
  8C220072  FF7B  fmov.s fr7,@-r15
  8C220074  FF6B  fmov.s fr6,@-r15
  8C220076  FF5B  fmov.s fr5,@-r15
  8C220078  FF4B  fmov.s fr4,@-r15
  8C22007A  FF3B  fmov.s fr3,@-r15
  8C22007C  FF2B  fmov.s fr2,@-r15
  8C22007E  FF1B  fmov.s fr1,@-r15
  8C220080  FF0B  fmov.s fr0,@-r15
  8C220082  FBFD  frchg
  8C220084  FFFB  fmov.s fr15,@-r15
  8C220086  FFEB  fmov.s fr14,@-r15
  8C220088  FFDB  fmov.s fr13,@-r15
  8C22008A  FFCB  fmov.s fr12,@-r15
  8C22008C  FFBB  fmov.s fr11,@-r15
  8C22008E  FFAB  fmov.s fr10,@-r15
  8C220090  FF9B  fmov.s fr9,@-r15
  8C220092  FF8B  fmov.s fr8,@-r15
  8C220094  FF7B  fmov.s fr7,@-r15
  8C220096  FF6B  fmov.s fr6,@-r15
  8C220098  FF5B  fmov.s fr5,@-r15
  8C22009A  FF4B  fmov.s fr4,@-r15
  8C22009C  FF3B  fmov.s fr3,@-r15
  8C22009E  FF2B  fmov.s fr2,@-r15
  8C2200A0  FF1B  fmov.s fr1,@-r15
  8C2200A2  FF0B  fmov.s fr0,@-r15
  8C2200A4  FBFD  frchg
  8C2200A6  0002  stc SR,r0
  8C2200A8  D139  mov.l @([8C220190]),r1
  8C2200AA  2019  and r1,r0
  8C2200AC  CBF0  or ##240,R0
  8C2200AE  400E  ldc r0,SR
  8C2200B0  E3FE  mov ##0xFE,r3
  8C2200B2  4318  shll8 r3
  8C2200B4  0E22  stc VBR,r14
  8C2200B6  C73B  mova @([8C2201A4]),R0
  8C2200B8  6402  mov.l @r0,r4
  8C2200BA  6243  mov r4,r2
  8C2200BC  323C  add r3,r2
  8C2200BE  E002  mov ##0x02,r0
  8C2200C0  4018  shll8 r0
  8C2200C2  4221  shar r2
  8C2200C4  4221  shar r2
  8C2200C6  4221  shar r2
  8C2200C8  3E2C  add r2,r14
  8C2200CA  0EEE  mov.l @(R0,r14),r14
  8C2200CC  2EE8  tst r14,r14
  8C2200CE  8901  bt 8C2200D4
  8C2200D0  4E0B  jsr @r14
  8C2200D2  0009  nop
  8C2200D4  D02F  mov.l @([8C220194]),r0
  8C2200D6  0102  stc SR,r1
  8C2200D8  210B  or r0,r1
  8C2200DA  410E  ldc r1,SR
  8C2200DC  C732  mova @([8C2201A8]),R0
  8C2200DE  6102  mov.l @r0,r1
  8C2200E0  71FF  add ##-1,r1
  8C2200E2  2012  mov.l r1,@r0
  8C2200E4  2118  tst r1,r1
  8C2200E6  E004  mov ##0x04,r0
  8C2200E8  4028  shll16 r0
  8C2200EA  406A  lds r0,FPSCR
  8C2200EC  200A  xor r0,r0
  8C2200EE  405A  lds r0,FPUL
  8C2200F0  FBFD  frchg
  8C2200F2  F0F9  fmov.s @r15+,fr0
  8C2200F4  F1F9  fmov.s @r15+,fr1
  8C2200F6  F2F9  fmov.s @r15+,fr2
  8C2200F8  F3F9  fmov.s @r15+,fr3
  8C2200FA  F4F9  fmov.s @r15+,fr4
  8C2200FC  F5F9  fmov.s @r15+,fr5
  8C2200FE  F6F9  fmov.s @r15+,fr6
  8C220100  F7F9  fmov.s @r15+,fr7
  8C220102  F8F9  fmov.s @r15+,fr8
  8C220104  F9F9  fmov.s @r15+,fr9
  8C220106  FAF9  fmov.s @r15+,fr10
  8C220108  FBF9  fmov.s @r15+,fr11
  8C22010A  FCF9  fmov.s @r15+,fr12
  8C22010C  FDF9  fmov.s @r15+,fr13
  8C22010E  FEF9  fmov.s @r15+,fr14
  8C220110  FFF9  fmov.s @r15+,fr15
  8C220112  FBFD  frchg
  8C220114  F0F9  fmov.s @r15+,fr0
  8C220116  F1F9  fmov.s @r15+,fr1
  8C220118  F2F9  fmov.s @r15+,fr2
  8C22011A  F3F9  fmov.s @r15+,fr3
  8C22011C  F4F9  fmov.s @r15+,fr4
  8C22011E  F5F9  fmov.s @r15+,fr5
  8C220120  F6F9  fmov.s @r15+,fr6
  8C220122  F7F9  fmov.s @r15+,fr7
  8C220124  F8F9  fmov.s @r15+,fr8
  8C220126  F9F9  fmov.s @r15+,fr9
  8C220128  FAF9  fmov.s @r15+,fr10
  8C22012A  FBF9  fmov.s @r15+,fr11
  8C22012C  FCF9  fmov.s @r15+,fr12
  8C22012E  FDF9  fmov.s @r15+,fr13
  8C220130  FEF9  fmov.s @r15+,fr14
  8C220132  FFF9  fmov.s @r15+,fr15
  8C220134  4F66  lds.l @r15+,FPSCR
  8C220136  4F56  lds.l @r15+,FPUL
  8C220138  64F6  mov.l @r15+,r4
  8C22013A  60F6  mov.l @r15+,r0
  8C22013C  61F6  mov.l @r15+,r1
  8C22013E  62F6  mov.l @r15+,r2
  8C220140  63F6  mov.l @r15+,r3
  8C220142  64F6  mov.l @r15+,r4
  8C220144  65F6  mov.l @r15+,r5
  8C220146  66F6  mov.l @r15+,r6
  8C220148  67F6  mov.l @r15+,r7
  8C22014A  68F6  mov.l @r15+,r8
  8C22014C  69F6  mov.l @r15+,r9
  8C22014E  6AF6  mov.l @r15+,r10
  8C220150  6BF6  mov.l @r15+,r11
  8C220152  6CF6  mov.l @r15+,r12
  8C220154  6DF6  mov.l @r15+,r13
  8C220156  6EF6  mov.l @r15+,r14
  8C220158  4F16  lds.l @r15+,MAC
  8C22015A  4F06  lds.l @r15+,MACH
  8C22015C  4F27  ldc.l @r15+,VBR
  8C22015E  4F17  ldc.l @r15+,GBR
  8C220160  4F26  lds.l @r15+,PR
  8C220162  4F47  ldc.l @r15+,SPC
  8C220164  4F37  ldc.l @r15+,SSR
  8C220166  8B00  bf 8C22016A
  8C220168  DF0C  mov.l @([8C22019C]),r15
  8C22016A  2F06  mov.l r0,@-r15
  8C22016C  2F16  mov.l r1,@-r15
  8C22016E  D10A  mov.l @([8C220198]),r1
  8C220170  D004  mov.l @([8C220184]),r0
  8C220172  6112  mov.l @r1,r1
  8C220174  4018  shll8 r0
  8C220176  3103  cmp/ge r0,r1
  8C220178  8F06  bf.s 8C220188
  8C22017A  61F6  mov.l @r15+,r1
  8C22017C  0022  stc VBR,r0
  8C22017E  402B  jmp @r0
  8C220180  60F6  mov.l @r15+,r0
```

## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] `8C092FB8`

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
  8C092FB8  6103  mov r0,r1
  8C092FBA  C764  mova @([8C09314C]),R0
  8C092FBC  2016  mov.l r1,@-r0
  8C092FBE  70FC  add ##-4,r0
  8C092FC0  6102  mov.l @r0,r1
  8C092FC2  7101  add ##1,r1
  8C092FC4  2012  mov.l r1,@r0
  8C092FC6  7004  add ##4,r0
  8C092FC8  4110  dt r1
  8C092FCA  6002  mov.l @r0,r0
  8C092FCC  8F07  bf.s 8C092FDE
  8C092FCE  61F6  mov.l @r15+,r1
  8C092FD0  2F06  mov.l r0,@-r15
  8C092FD2  C759  mova @([8C093138]),R0
  8C092FD4  7F04  add ##4,r15
  8C092FD6  20F2  mov.l r15,@r0
  8C092FD8  7FFC  add ##-4,r15
  8C092FDA  60F6  mov.l @r15+,r0
  8C092FDC  DF57  mov.l @([8C09313C]),r15
  8C092FDE  4F33  stc.l SSR,@-r15
  8C092FE0  4F43  stc.l SPC,@-r15
  8C092FE2  4F22  sts.l PR,@-r15
  8C092FE4  4F13  stc.l GBR,@-r15
  8C092FE6  4F23  stc.l VBR,@-r15
  8C092FE8  4F02  sts.l MACH,@-r15
  8C092FEA  4F12  sts.l MACL,@-r15
  8C092FEC  2FE6  mov.l r14,@-r15
  8C092FEE  2FD6  mov.l r13,@-r15
  8C092FF0  2FC6  mov.l r12,@-r15
  8C092FF2  2FB6  mov.l r11,@-r15
  8C092FF4  2FA6  mov.l r10,@-r15
  8C092FF6  2F96  mov.l r9,@-r15
  8C092FF8  2F86  mov.l r8,@-r15
  8C092FFA  2F76  mov.l r7,@-r15
  8C092FFC  2F66  mov.l r6,@-r15
  8C092FFE  2F56  mov.l r5,@-r15
  8C093000  2F46  mov.l r4,@-r15
  8C093002  2F36  mov.l r3,@-r15
  8C093004  2F26  mov.l r2,@-r15
  8C093006  2F16  mov.l r1,@-r15
  8C093008  2F06  mov.l r0,@-r15
  8C09300A  4F03  stc.l SR,@-r15
  8C09300C  4F52  sts.l FPUL,@-r15
  8C09300E  4F62  sts.l FPSCR,@-r15
  8C093010  E004  mov ##0x04,r0
  8C093012  4028  shll16 r0
  8C093014  7001  add ##1,r0
  8C093016  406A  lds r0,FPSCR
  8C093018  200A  xor r0,r0
  8C09301A  405A  lds r0,FPUL
  8C09301C  FFFB  fmov.s fr15,@-r15
  8C09301E  FFEB  fmov.s fr14,@-r15
  8C093020  FFDB  fmov.s fr13,@-r15
  8C093022  FFCB  fmov.s fr12,@-r15
  8C093024  FFBB  fmov.s fr11,@-r15
  8C093026  FFAB  fmov.s fr10,@-r15
  8C093028  FF9B  fmov.s fr9,@-r15
  8C09302A  FF8B  fmov.s fr8,@-r15
  8C09302C  FF7B  fmov.s fr7,@-r15
  8C09302E  FF6B  fmov.s fr6,@-r15
  8C093030  FF5B  fmov.s fr5,@-r15
  8C093032  FF4B  fmov.s fr4,@-r15
  8C093034  FF3B  fmov.s fr3,@-r15
  8C093036  FF2B  fmov.s fr2,@-r15
  8C093038  FF1B  fmov.s fr1,@-r15
  8C09303A  FF0B  fmov.s fr0,@-r15
  8C09303C  FBFD  frchg
  8C09303E  FFFB  fmov.s fr15,@-r15
  8C093040  FFEB  fmov.s fr14,@-r15
  8C093042  FFDB  fmov.s fr13,@-r15
  8C093044  FFCB  fmov.s fr12,@-r15
  8C093046  FFBB  fmov.s fr11,@-r15
  8C093048  FFAB  fmov.s fr10,@-r15
  8C09304A  FF9B  fmov.s fr9,@-r15
  8C09304C  FF8B  fmov.s fr8,@-r15
  8C09304E  FF7B  fmov.s fr7,@-r15
  8C093050  FF6B  fmov.s fr6,@-r15
  8C093052  FF5B  fmov.s fr5,@-r15
  8C093054  FF4B  fmov.s fr4,@-r15
  8C093056  FF3B  fmov.s fr3,@-r15
  8C093058  FF2B  fmov.s fr2,@-r15
  8C09305A  FF1B  fmov.s fr1,@-r15
  8C09305C  FF0B  fmov.s fr0,@-r15
  8C09305E  FBFD  frchg
  8C093060  0002  stc SR,r0
  8C093062  D132  mov.l @([8C09312C]),r1
  8C093064  2019  and r1,r0
  8C093066  CBF0  or ##240,R0
  8C093068  400E  ldc r0,SR
  8C09306A  E3FE  mov ##0xFE,r3
  8C09306C  4318  shll8 r3
  8C09306E  0E22  stc VBR,r14
  8C093070  C733  mova @([8C093140]),R0
  8C093072  6402  mov.l @r0,r4
  8C093074  6243  mov r4,r2
  8C093076  323C  add r3,r2
  8C093078  E002  mov ##0x02,r0
  8C09307A  4018  shll8 r0
  8C09307C  4221  shar r2
  8C09307E  4221  shar r2
  8C093080  4221  shar r2
  8C093082  3E2C  add r2,r14
  8C093084  0EEE  mov.l @(R0,r14),r14
  8C093086  2EE8  tst r14,r14
  8C093088  8901  bt 8C09308E
  8C09308A  4E0B  jsr @r14
  8C09308C  0009  nop
  8C09308E  D028  mov.l @([8C093130]),r0
  8C093090  0102  stc SR,r1
  8C093092  210B  or r0,r1
  8C093094  410E  ldc r1,SR
  8C093096  C72B  mova @([8C093144]),R0
  8C093098  6102  mov.l @r0,r1
  8C09309A  71FF  add ##-1,r1
  8C09309C  2012  mov.l r1,@r0
  8C09309E  2118  tst r1,r1
  8C0930A0  E004  mov ##0x04,r0
  8C0930A2  4028  shll16 r0
  8C0930A4  7001  add ##1,r0
  8C0930A6  406A  lds r0,FPSCR
  8C0930A8  200A  xor r0,r0
  8C0930AA  405A  lds r0,FPUL
  8C0930AC  FBFD  frchg
  8C0930AE  F0F9  fmov.s @r15+,fr0
  8C0930B0  F1F9  fmov.s @r15+,fr1
  8C0930B2  F2F9  fmov.s @r15+,fr2
  8C0930B4  F3F9  fmov.s @r15+,fr3
  8C0930B6  F4F9  fmov.s @r15+,fr4
  8C0930B8  F5F9  fmov.s @r15+,fr5
  8C0930BA  F6F9  fmov.s @r15+,fr6
  8C0930BC  F7F9  fmov.s @r15+,fr7
  8C0930BE  F8F9  fmov.s @r15+,fr8
  8C0930C0  F9F9  fmov.s @r15+,fr9
  8C0930C2  FAF9  fmov.s @r15+,fr10
  8C0930C4  FBF9  fmov.s @r15+,fr11
  8C0930C6  FCF9  fmov.s @r15+,fr12
  8C0930C8  FDF9  fmov.s @r15+,fr13
  8C0930CA  FEF9  fmov.s @r15+,fr14
  8C0930CC  FFF9  fmov.s @r15+,fr15
  8C0930CE  FBFD  frchg
  8C0930D0  F0F9  fmov.s @r15+,fr0
  8C0930D2  F1F9  fmov.s @r15+,fr1
  8C0930D4  F2F9  fmov.s @r15+,fr2
  8C0930D6  F3F9  fmov.s @r15+,fr3
  8C0930D8  F4F9  fmov.s @r15+,fr4
  8C0930DA  F5F9  fmov.s @r15+,fr5
  8C0930DC  F6F9  fmov.s @r15+,fr6
  8C0930DE  F7F9  fmov.s @r15+,fr7
  8C0930E0  F8F9  fmov.s @r15+,fr8
  8C0930E2  F9F9  fmov.s @r15+,fr9
  8C0930E4  FAF9  fmov.s @r15+,fr10
  8C0930E6  FBF9  fmov.s @r15+,fr11
  8C0930E8  FCF9  fmov.s @r15+,fr12
  8C0930EA  FDF9  fmov.s @r15+,fr13
  8C0930EC  FEF9  fmov.s @r15+,fr14
  8C0930EE  FFF9  fmov.s @r15+,fr15
  8C0930F0  4F66  lds.l @r15+,FPSCR
  8C0930F2  4F56  lds.l @r15+,FPUL
  8C0930F4  64F6  mov.l @r15+,r4
  8C0930F6  60F6  mov.l @r15+,r0
  8C0930F8  61F6  mov.l @r15+,r1
  8C0930FA  62F6  mov.l @r15+,r2
  8C0930FC  63F6  mov.l @r15+,r3
  8C0930FE  64F6  mov.l @r15+,r4
  8C093100  65F6  mov.l @r15+,r5
  8C093102  66F6  mov.l @r15+,r6
  8C093104  67F6  mov.l @r15+,r7
  8C093106  68F6  mov.l @r15+,r8
  8C093108  69F6  mov.l @r15+,r9
  8C09310A  6AF6  mov.l @r15+,r10
  8C09310C  6BF6  mov.l @r15+,r11
  8C09310E  6CF6  mov.l @r15+,r12
  8C093110  6DF6  mov.l @r15+,r13
  8C093112  6EF6  mov.l @r15+,r14
  8C093114  4F16  lds.l @r15+,MAC
  8C093116  4F06  lds.l @r15+,MACH
  8C093118  4F27  ldc.l @r15+,VBR
  8C09311A  4F17  ldc.l @r15+,GBR
  8C09311C  4F26  lds.l @r15+,PR
  8C09311E  4F47  ldc.l @r15+,SPC
  8C093120  4F37  ldc.l @r15+,SSR
  8C093122  8B00  bf 8C093126
  8C093124  DF04  mov.l @([8C093138]),r15
  8C093126  002B  rte
  8C093128  0009  nop
```

## Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) `8C04C4A0`

Dump: `/mnt/1TB/dcbat/20261007-163759_Le_Mans_24_Hours__Europe___En_Fr_De_Es_I/jit-109461.txt`

```
  8C04C4A0  6103  mov r0,r1
  8C04C4A2  C76B  mova @([8C04C650]),R0
  8C04C4A4  2016  mov.l r1,@-r0
  8C04C4A6  70FC  add ##-4,r0
  8C04C4A8  6102  mov.l @r0,r1
  8C04C4AA  7101  add ##1,r1
  8C04C4AC  2012  mov.l r1,@r0
  8C04C4AE  7004  add ##4,r0
  8C04C4B0  4110  dt r1
  8C04C4B2  6002  mov.l @r0,r0
  8C04C4B4  8F07  bf.s 8C04C4C6
  8C04C4B6  61F6  mov.l @r15+,r1
  8C04C4B8  2F06  mov.l r0,@-r15
  8C04C4BA  C760  mova @([8C04C63C]),R0
  8C04C4BC  7F04  add ##4,r15
  8C04C4BE  20F2  mov.l r15,@r0
  8C04C4C0  7FFC  add ##-4,r15
  8C04C4C2  60F6  mov.l @r15+,r0
  8C04C4C4  DF5E  mov.l @([8C04C640]),r15
  8C04C4C6  4F33  stc.l SSR,@-r15
  8C04C4C8  4F43  stc.l SPC,@-r15
  8C04C4CA  4F22  sts.l PR,@-r15
  8C04C4CC  4F13  stc.l GBR,@-r15
  8C04C4CE  4F23  stc.l VBR,@-r15
```

## Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) `8C21FE50`

Dump: `/mnt/1TB/dcbat/20261007-163759_Le_Mans_24_Hours__Europe___En_Fr_De_Es_I/jit-109461.txt`

```
  8C21FE50  61F6  mov.l @r15+,r1
  8C21FE52  4F33  stc.l SSR,@-r15
  8C21FE54  4F43  stc.l SPC,@-r15
  8C21FE56  4F22  sts.l PR,@-r15
  8C21FE58  4F13  stc.l GBR,@-r15
  8C21FE5A  4F23  stc.l VBR,@-r15
  8C21FE5C  4F02  sts.l MACH,@-r15
  8C21FE5E  4F12  sts.l MACL,@-r15
  8C21FE60  2FE6  mov.l r14,@-r15
  8C21FE62  2FD6  mov.l r13,@-r15
  8C21FE64  2FC6  mov.l r12,@-r15
  8C21FE66  2FB6  mov.l r11,@-r15
  8C21FE68  2FA6  mov.l r10,@-r15
  8C21FE6A  2F96  mov.l r9,@-r15
  8C21FE6C  2F86  mov.l r8,@-r15
  8C21FE6E  2F76  mov.l r7,@-r15
  8C21FE70  2F66  mov.l r6,@-r15
  8C21FE72  2F56  mov.l r5,@-r15
  8C21FE74  2F46  mov.l r4,@-r15
  8C21FE76  2F36  mov.l r3,@-r15
  8C21FE78  2F26  mov.l r2,@-r15
  8C21FE7A  2F16  mov.l r1,@-r15
  8C21FE7C  2F06  mov.l r0,@-r15
  8C21FE7E  4F03  stc.l SR,@-r15
  8C21FE80  4F52  sts.l FPUL,@-r15
  8C21FE82  4F62  sts.l FPSCR,@-r15
  8C21FE84  E004  mov ##0x04,r0
  8C21FE86  4028  shll16 r0
  8C21FE88  406A  lds r0,FPSCR
  8C21FE8A  200A  xor r0,r0
  8C21FE8C  405A  lds r0,FPUL
  8C21FE8E  FFFB  fmov.s fr15,@-r15
  8C21FE90  FFEB  fmov.s fr14,@-r15
  8C21FE92  FFDB  fmov.s fr13,@-r15
  8C21FE94  FFCB  fmov.s fr12,@-r15
  8C21FE96  FFBB  fmov.s fr11,@-r15
  8C21FE98  FFAB  fmov.s fr10,@-r15
  8C21FE9A  FF9B  fmov.s fr9,@-r15
  8C21FE9C  FF8B  fmov.s fr8,@-r15
  8C21FE9E  FF7B  fmov.s fr7,@-r15
  8C21FEA0  FF6B  fmov.s fr6,@-r15
  8C21FEA2  FF5B  fmov.s fr5,@-r15
  8C21FEA4  FF4B  fmov.s fr4,@-r15
  8C21FEA6  FF3B  fmov.s fr3,@-r15
  8C21FEA8  FF2B  fmov.s fr2,@-r15
  8C21FEAA  FF1B  fmov.s fr1,@-r15
  8C21FEAC  FF0B  fmov.s fr0,@-r15
  8C21FEAE  FBFD  frchg
  8C21FEB0  FFFB  fmov.s fr15,@-r15
  8C21FEB2  FFEB  fmov.s fr14,@-r15
  8C21FEB4  FFDB  fmov.s fr13,@-r15
  8C21FEB6  FFCB  fmov.s fr12,@-r15
  8C21FEB8  FFBB  fmov.s fr11,@-r15
  8C21FEBA  FFAB  fmov.s fr10,@-r15
  8C21FEBC  FF9B  fmov.s fr9,@-r15
  8C21FEBE  FF8B  fmov.s fr8,@-r15
  8C21FEC0  FF7B  fmov.s fr7,@-r15
  8C21FEC2  FF6B  fmov.s fr6,@-r15
  8C21FEC4  FF5B  fmov.s fr5,@-r15
  8C21FEC6  FF4B  fmov.s fr4,@-r15
  8C21FEC8  FF3B  fmov.s fr3,@-r15
  8C21FECA  FF2B  fmov.s fr2,@-r15
  8C21FECC  FF1B  fmov.s fr1,@-r15
  8C21FECE  FF0B  fmov.s fr0,@-r15
  8C21FED0  FBFD  frchg
  8C21FED2  0002  stc SR,r0
  8C21FED4  D135  mov.l @([8C21FFAC]),r1
  8C21FED6  2019  and r1,r0
  8C21FED8  CBF0  or ##240,R0
  8C21FEDA  400E  ldc r0,SR
  8C21FEDC  E3FE  mov ##0xFE,r3
  8C21FEDE  4318  shll8 r3
  8C21FEE0  0E22  stc VBR,r14
  8C21FEE2  D434  mov.l @([8C21FFB4]),r4
  8C21FEE4  6442  mov.l @r4,r4
  8C21FEE6  6243  mov r4,r2
  8C21FEE8  323C  add r3,r2
  8C21FEEA  E002  mov ##0x02,r0
  8C21FEEC  4018  shll8 r0
  8C21FEEE  4221  shar r2
  8C21FEF0  4221  shar r2
  8C21FEF2  4221  shar r2
  8C21FEF4  3E2C  add r2,r14
  8C21FEF6  0EEE  mov.l @(R0,r14),r14
  8C21FEF8  2EE8  tst r14,r14
  8C21FEFA  8905  bt 8C21FF08
  8C21FEFC  4E0B  jsr @r14
  8C21FEFE  0009  nop
  8C21FF00  D02B  mov.l @([8C21FFB0]),r0
  8C21FF02  0102  stc SR,r1
  8C21FF04  210B  or r0,r1
  8C21FF06  410E  ldc r1,SR
  8C21FF08  E004  mov ##0x04,r0
  8C21FF0A  4028  shll16 r0
  8C21FF0C  406A  lds r0,FPSCR
  8C21FF0E  200A  xor r0,r0
  8C21FF10  405A  lds r0,FPUL
  8C21FF12  FBFD  frchg
  8C21FF14  F0F9  fmov.s @r15+,fr0
  8C21FF16  F1F9  fmov.s @r15+,fr1
  8C21FF18  F2F9  fmov.s @r15+,fr2
  8C21FF1A  F3F9  fmov.s @r15+,fr3
  8C21FF1C  F4F9  fmov.s @r15+,fr4
  8C21FF1E  F5F9  fmov.s @r15+,fr5
  8C21FF20  F6F9  fmov.s @r15+,fr6
  8C21FF22  F7F9  fmov.s @r15+,fr7
  8C21FF24  F8F9  fmov.s @r15+,fr8
  8C21FF26  F9F9  fmov.s @r15+,fr9
  8C21FF28  FAF9  fmov.s @r15+,fr10
  8C21FF2A  FBF9  fmov.s @r15+,fr11
  8C21FF2C  FCF9  fmov.s @r15+,fr12
  8C21FF2E  FDF9  fmov.s @r15+,fr13
  8C21FF30  FEF9  fmov.s @r15+,fr14
  8C21FF32  FFF9  fmov.s @r15+,fr15
  8C21FF34  FBFD  frchg
  8C21FF36  F0F9  fmov.s @r15+,fr0
  8C21FF38  F1F9  fmov.s @r15+,fr1
  8C21FF3A  F2F9  fmov.s @r15+,fr2
  8C21FF3C  F3F9  fmov.s @r15+,fr3
  8C21FF3E  F4F9  fmov.s @r15+,fr4
  8C21FF40  F5F9  fmov.s @r15+,fr5
  8C21FF42  F6F9  fmov.s @r15+,fr6
  8C21FF44  F7F9  fmov.s @r15+,fr7
  8C21FF46  F8F9  fmov.s @r15+,fr8
  8C21FF48  F9F9  fmov.s @r15+,fr9
  8C21FF4A  FAF9  fmov.s @r15+,fr10
  8C21FF4C  FBF9  fmov.s @r15+,fr11
  8C21FF4E  FCF9  fmov.s @r15+,fr12
  8C21FF50  FDF9  fmov.s @r15+,fr13
  8C21FF52  FEF9  fmov.s @r15+,fr14
  8C21FF54  FFF9  fmov.s @r15+,fr15
  8C21FF56  4F66  lds.l @r15+,FPSCR
  8C21FF58  4F56  lds.l @r15+,FPUL
  8C21FF5A  4F07  ldc.l @r15+,SR
  8C21FF5C  60F6  mov.l @r15+,r0
  8C21FF5E  61F6  mov.l @r15+,r1
  8C21FF60  62F6  mov.l @r15+,r2
  8C21FF62  63F6  mov.l @r15+,r3
  8C21FF64  64F6  mov.l @r15+,r4
  8C21FF66  65F6  mov.l @r15+,r5
  8C21FF68  66F6  mov.l @r15+,r6
  8C21FF6A  67F6  mov.l @r15+,r7
  8C21FF6C  68F6  mov.l @r15+,r8
  8C21FF6E  69F6  mov.l @r15+,r9
  8C21FF70  6AF6  mov.l @r15+,r10
  8C21FF72  6BF6  mov.l @r15+,r11
  8C21FF74  6CF6  mov.l @r15+,r12
  8C21FF76  6DF6  mov.l @r15+,r13
  8C21FF78  6EF6  mov.l @r15+,r14
  8C21FF7A  4F16  lds.l @r15+,MAC
  8C21FF7C  4F06  lds.l @r15+,MACH
  8C21FF7E  4F27  ldc.l @r15+,VBR
  8C21FF80  4F17  ldc.l @r15+,GBR
  8C21FF82  4F26  lds.l @r15+,PR
  8C21FF84  4F47  ldc.l @r15+,SPC
  8C21FF86  4F37  ldc.l @r15+,SSR
  8C21FF88  2F06  mov.l r0,@-r15
  8C21FF8A  2F16  mov.l r1,@-r15
  8C21FF8C  D109  mov.l @([8C21FFB4]),r1
  8C21FF8E  D004  mov.l @([8C21FFA0]),r0
  8C21FF90  6112  mov.l @r1,r1
  8C21FF92  4018  shll8 r0
  8C21FF94  3103  cmp/ge r0,r1
  8C21FF96  8F05  bf.s 8C21FFA4
  8C21FF98  61F6  mov.l @r15+,r1
  8C21FF9A  0022  stc VBR,r0
  8C21FF9C  402B  jmp @r0
  8C21FF9E  60F6  mov.l @r15+,r0
```

## Sonic Adventure 2 (USA) (EnJaFrDeEs) `8C15AD08`

Dump: `/mnt/1TB/dcbat/20261007-163225_Sonic_Adventure_2__USA___EnJaFrDeEs__/jit-101721.txt`

```
  8C15AD08  6103  mov r0,r1
  8C15AD0A  C75F  mova @([8C15AE88]),R0
  8C15AD0C  2016  mov.l r1,@-r0
  8C15AD0E  70FC  add ##-4,r0
  8C15AD10  6102  mov.l @r0,r1
  8C15AD12  7101  add ##1,r1
  8C15AD14  2012  mov.l r1,@r0
  8C15AD16  7004  add ##4,r0
  8C15AD18  4110  dt r1
  8C15AD1A  6002  mov.l @r0,r0
  8C15AD1C  8F07  bf.s 8C15AD2E
  8C15AD1E  61F6  mov.l @r15+,r1
  8C15AD20  2F06  mov.l r0,@-r15
  8C15AD22  C754  mova @([8C15AE74]),R0
  8C15AD24  7F04  add ##4,r15
  8C15AD26  20F2  mov.l r15,@r0
  8C15AD28  7FFC  add ##-4,r15
  8C15AD2A  60F6  mov.l @r15+,r0
  8C15AD2C  DF52  mov.l @([8C15AE78]),r15
  8C15AD2E  4F33  stc.l SSR,@-r15
  8C15AD30  4F43  stc.l SPC,@-r15
  8C15AD32  4F22  sts.l PR,@-r15
  8C15AD34  4F13  stc.l GBR,@-r15
  8C15AD36  4F23  stc.l VBR,@-r15
  8C15AD38  4F02  sts.l MACH,@-r15
  8C15AD3A  4F12  sts.l MACL,@-r15
  8C15AD3C  2FE6  mov.l r14,@-r15
  8C15AD3E  2FD6  mov.l r13,@-r15
  8C15AD40  2FC6  mov.l r12,@-r15
  8C15AD42  2FB6  mov.l r11,@-r15
  8C15AD44  2FA6  mov.l r10,@-r15
  8C15AD46  2F96  mov.l r9,@-r15
  8C15AD48  2F86  mov.l r8,@-r15
  8C15AD4A  2F76  mov.l r7,@-r15
  8C15AD4C  2F66  mov.l r6,@-r15
  8C15AD4E  2F56  mov.l r5,@-r15
  8C15AD50  2F46  mov.l r4,@-r15
  8C15AD52  2F36  mov.l r3,@-r15
  8C15AD54  2F26  mov.l r2,@-r15
  8C15AD56  2F16  mov.l r1,@-r15
  8C15AD58  2F06  mov.l r0,@-r15
  8C15AD5A  4F03  stc.l SR,@-r15
  8C15AD5C  4F52  sts.l FPUL,@-r15
  8C15AD5E  4F62  sts.l FPSCR,@-r15
  8C15AD60  E004  mov ##0x04,r0
  8C15AD62  4028  shll16 r0
  8C15AD64  406A  lds r0,FPSCR
  8C15AD66  200A  xor r0,r0
  8C15AD68  405A  lds r0,FPUL
  8C15AD6A  7FF8  add ##-8,r15
  8C15AD6C  60F3  mov r15,r0
  8C15AD6E  E1F8  mov ##0xF8,r1
  8C15AD70  2109  and r0,r1
  8C15AD72  7008  add ##8,r0
  8C15AD74  2102  mov.l r0,@r1
  8C15AD76  6F13  mov r1,r15
  8C15AD78  F3FD  fschg
  8C15AD7A  FFEB  fmov.s fr14,@-r15
  8C15AD7C  FFCB  fmov.s fr12,@-r15
  8C15AD7E  FFAB  fmov.s fr10,@-r15
  8C15AD80  FF8B  fmov.s fr8,@-r15
  8C15AD82  FF6B  fmov.s fr6,@-r15
  8C15AD84  FF4B  fmov.s fr4,@-r15
  8C15AD86  FF2B  fmov.s fr2,@-r15
  8C15AD88  FF0B  fmov.s fr0,@-r15
  8C15AD8A  FFFB  fmov.s fr15,@-r15
  8C15AD8C  FFDB  fmov.s fr13,@-r15
  8C15AD8E  FFBB  fmov.s fr11,@-r15
  8C15AD90  FF9B  fmov.s fr9,@-r15
  8C15AD92  FF7B  fmov.s fr7,@-r15
  8C15AD94  FF5B  fmov.s fr5,@-r15
  8C15AD96  FF3B  fmov.s fr3,@-r15
  8C15AD98  FF1B  fmov.s fr1,@-r15
  8C15AD9A  F3FD  fschg
  8C15AD9C  0002  stc SR,r0
  8C15AD9E  D132  mov.l @([8C15AE68]),r1
  8C15ADA0  2019  and r1,r0
  8C15ADA2  CBF0  or ##240,R0
  8C15ADA4  400E  ldc r0,SR
  8C15ADA6  E3FE  mov ##0xFE,r3
  8C15ADA8  4318  shll8 r3
  8C15ADAA  0E22  stc VBR,r14
  8C15ADAC  C733  mova @([8C15AE7C]),R0
  8C15ADAE  6402  mov.l @r0,r4
  8C15ADB0  6243  mov r4,r2
  8C15ADB2  323C  add r3,r2
  8C15ADB4  E002  mov ##0x02,r0
  8C15ADB6  4018  shll8 r0
  8C15ADB8  4221  shar r2
  8C15ADBA  4221  shar r2
  8C15ADBC  4221  shar r2
  8C15ADBE  3E2C  add r2,r14
  8C15ADC0  0EEE  mov.l @(R0,r14),r14
  8C15ADC2  2EE8  tst r14,r14
  8C15ADC4  8901  bt 8C15ADCA
  8C15ADC6  4E0B  jsr @r14
  8C15ADC8  0009  nop
  8C15ADCA  D028  mov.l @([8C15AE6C]),r0
  8C15ADCC  0102  stc SR,r1
  8C15ADCE  210B  or r0,r1
  8C15ADD0  410E  ldc r1,SR
  8C15ADD2  C72B  mova @([8C15AE80]),R0
  8C15ADD4  6102  mov.l @r0,r1
  8C15ADD6  71FF  add ##-1,r1
  8C15ADD8  2012  mov.l r1,@r0
  8C15ADDA  2118  tst r1,r1
  8C15ADDC  E004  mov ##0x04,r0
  8C15ADDE  4028  shll16 r0
  8C15ADE0  406A  lds r0,FPSCR
  8C15ADE2  200A  xor r0,r0
  8C15ADE4  405A  lds r0,FPUL
  8C15ADE6  F3FD  fschg
  8C15ADE8  F1F9  fmov.s @r15+,fr1
  8C15ADEA  F3F9  fmov.s @r15+,fr3
  8C15ADEC  F5F9  fmov.s @r15+,fr5
  8C15ADEE  F7F9  fmov.s @r15+,fr7
  8C15ADF0  F9F9  fmov.s @r15+,fr9
  8C15ADF2  FBF9  fmov.s @r15+,fr11
  8C15ADF4  FDF9  fmov.s @r15+,fr13
  8C15ADF6  FFF9  fmov.s @r15+,fr15
  8C15ADF8  F0F9  fmov.s @r15+,fr0
  8C15ADFA  F2F9  fmov.s @r15+,fr2
  8C15ADFC  F4F9  fmov.s @r15+,fr4
  8C15ADFE  F6F9  fmov.s @r15+,fr6
  8C15AE00  F8F9  fmov.s @r15+,fr8
  8C15AE02  FAF9  fmov.s @r15+,fr10
  8C15AE04  FCF9  fmov.s @r15+,fr12
  8C15AE06  FEF9  fmov.s @r15+,fr14
  8C15AE08  F3FD  fschg
  8C15AE0A  6FF6  mov.l @r15+,r15
  8C15AE0C  4F66  lds.l @r15+,FPSCR
  8C15AE0E  4F56  lds.l @r15+,FPUL
  8C15AE10  64F6  mov.l @r15+,r4
  8C15AE12  60F6  mov.l @r15+,r0
  8C15AE14  61F6  mov.l @r15+,r1
  8C15AE16  62F6  mov.l @r15+,r2
  8C15AE18  63F6  mov.l @r15+,r3
  8C15AE1A  64F6  mov.l @r15+,r4
  8C15AE1C  65F6  mov.l @r15+,r5
  8C15AE1E  66F6  mov.l @r15+,r6
  8C15AE20  67F6  mov.l @r15+,r7
  8C15AE22  68F6  mov.l @r15+,r8
  8C15AE24  69F6  mov.l @r15+,r9
  8C15AE26  6AF6  mov.l @r15+,r10
  8C15AE28  6BF6  mov.l @r15+,r11
  8C15AE2A  6CF6  mov.l @r15+,r12
  8C15AE2C  6DF6  mov.l @r15+,r13
  8C15AE2E  6EF6  mov.l @r15+,r14
  8C15AE30  4F16  lds.l @r15+,MAC
  8C15AE32  4F06  lds.l @r15+,MACH
  8C15AE34  4F27  ldc.l @r15+,VBR
  8C15AE36  4F17  ldc.l @r15+,GBR
  8C15AE38  4F26  lds.l @r15+,PR
  8C15AE3A  4F47  ldc.l @r15+,SPC
  8C15AE3C  4F37  ldc.l @r15+,SSR
  8C15AE3E  8B00  bf 8C15AE42
  8C15AE40  DF0C  mov.l @([8C15AE74]),r15
  8C15AE42  2F06  mov.l r0,@-r15
  8C15AE44  2F16  mov.l r1,@-r15
  8C15AE46  D10A  mov.l @([8C15AE70]),r1
  8C15AE48  D004  mov.l @([8C15AE5C]),r0
  8C15AE4A  6112  mov.l @r1,r1
  8C15AE4C  4018  shll8 r0
  8C15AE4E  3103  cmp/ge r0,r1
  8C15AE50  8F06  bf.s 8C15AE60
  8C15AE52  61F6  mov.l @r15+,r1
  8C15AE54  0022  stc VBR,r0
  8C15AE56  402B  jmp @r0
  8C15AE58  60F6  mov.l @r15+,r0
```

## Soulcalibur (USA) `8C241420`

Dump: `/mnt/1TB/dcbat/20261007-163634_Soulcalibur__USA__/jit-108030.txt`

```
  8C241420  61F6  mov.l @r15+,r1
  8C241422  4F33  stc.l SSR,@-r15
  8C241424  4F43  stc.l SPC,@-r15
  8C241426  4F22  sts.l PR,@-r15
  8C241428  4F13  stc.l GBR,@-r15
  8C24142A  4F23  stc.l VBR,@-r15
  8C24142C  4F02  sts.l MACH,@-r15
  8C24142E  4F12  sts.l MACL,@-r15
  8C241430  2FE6  mov.l r14,@-r15
  8C241432  2FD6  mov.l r13,@-r15
  8C241434  2FC6  mov.l r12,@-r15
  8C241436  2FB6  mov.l r11,@-r15
  8C241438  2FA6  mov.l r10,@-r15
  8C24143A  2F96  mov.l r9,@-r15
  8C24143C  2F86  mov.l r8,@-r15
  8C24143E  2F76  mov.l r7,@-r15
  8C241440  2F66  mov.l r6,@-r15
  8C241442  2F56  mov.l r5,@-r15
  8C241444  2F46  mov.l r4,@-r15
  8C241446  2F36  mov.l r3,@-r15
  8C241448  2F26  mov.l r2,@-r15
  8C24144A  2F16  mov.l r1,@-r15
  8C24144C  2F06  mov.l r0,@-r15
  8C24144E  4F03  stc.l SR,@-r15
  8C241450  4F52  sts.l FPUL,@-r15
  8C241452  4F62  sts.l FPSCR,@-r15
  8C241454  E004  mov ##0x04,r0
  8C241456  4028  shll16 r0
  8C241458  7001  add ##1,r0
  8C24145A  406A  lds r0,FPSCR
  8C24145C  200A  xor r0,r0
  8C24145E  405A  lds r0,FPUL
  8C241460  FFFB  fmov.s fr15,@-r15
  8C241462  FFEB  fmov.s fr14,@-r15
  8C241464  FFDB  fmov.s fr13,@-r15
  8C241466  FFCB  fmov.s fr12,@-r15
  8C241468  FFBB  fmov.s fr11,@-r15
  8C24146A  FFAB  fmov.s fr10,@-r15
  8C24146C  FF9B  fmov.s fr9,@-r15
  8C24146E  FF8B  fmov.s fr8,@-r15
  8C241470  FF7B  fmov.s fr7,@-r15
  8C241472  FF6B  fmov.s fr6,@-r15
  8C241474  FF5B  fmov.s fr5,@-r15
  8C241476  FF4B  fmov.s fr4,@-r15
  8C241478  FF3B  fmov.s fr3,@-r15
  8C24147A  FF2B  fmov.s fr2,@-r15
  8C24147C  FF1B  fmov.s fr1,@-r15
  8C24147E  FF0B  fmov.s fr0,@-r15
  8C241480  FBFD  frchg
  8C241482  FFFB  fmov.s fr15,@-r15
  8C241484  FFEB  fmov.s fr14,@-r15
  8C241486  FFDB  fmov.s fr13,@-r15
  8C241488  FFCB  fmov.s fr12,@-r15
  8C24148A  FFBB  fmov.s fr11,@-r15
  8C24148C  FFAB  fmov.s fr10,@-r15
  8C24148E  FF9B  fmov.s fr9,@-r15
  8C241490  FF8B  fmov.s fr8,@-r15
  8C241492  FF7B  fmov.s fr7,@-r15
  8C241494  FF6B  fmov.s fr6,@-r15
  8C241496  FF5B  fmov.s fr5,@-r15
  8C241498  FF4B  fmov.s fr4,@-r15
  8C24149A  FF3B  fmov.s fr3,@-r15
  8C24149C  FF2B  fmov.s fr2,@-r15
  8C24149E  FF1B  fmov.s fr1,@-r15
  8C2414A0  FF0B  fmov.s fr0,@-r15
  8C2414A2  FBFD  frchg
  8C2414A4  0002  stc SR,r0
  8C2414A6  D12E  mov.l @([8C241560]),r1
  8C2414A8  2019  and r1,r0
  8C2414AA  CBF0  or ##240,R0
  8C2414AC  400E  ldc r0,SR
  8C2414AE  E3FE  mov ##0xFE,r3
  8C2414B0  4318  shll8 r3
  8C2414B2  0E22  stc VBR,r14
  8C2414B4  D42C  mov.l @([8C241568]),r4
  8C2414B6  6442  mov.l @r4,r4
  8C2414B8  6243  mov r4,r2
  8C2414BA  323C  add r3,r2
  8C2414BC  E002  mov ##0x02,r0
  8C2414BE  4018  shll8 r0
  8C2414C0  4221  shar r2
  8C2414C2  4221  shar r2
  8C2414C4  4221  shar r2
  8C2414C6  3E2C  add r2,r14
  8C2414C8  0EEE  mov.l @(R0,r14),r14
  8C2414CA  2EE8  tst r14,r14
  8C2414CC  8905  bt 8C2414DA
  8C2414CE  4E0B  jsr @r14
  8C2414D0  0009  nop
  8C2414D2  D024  mov.l @([8C241564]),r0
  8C2414D4  0102  stc SR,r1
  8C2414D6  210B  or r0,r1
  8C2414D8  410E  ldc r1,SR
  8C2414DA  E004  mov ##0x04,r0
  8C2414DC  4028  shll16 r0
  8C2414DE  7001  add ##1,r0
  8C2414E0  406A  lds r0,FPSCR
  8C2414E2  200A  xor r0,r0
  8C2414E4  405A  lds r0,FPUL
  8C2414E6  FBFD  frchg
  8C2414E8  F0F9  fmov.s @r15+,fr0
  8C2414EA  F1F9  fmov.s @r15+,fr1
  8C2414EC  F2F9  fmov.s @r15+,fr2
  8C2414EE  F3F9  fmov.s @r15+,fr3
  8C2414F0  F4F9  fmov.s @r15+,fr4
  8C2414F2  F5F9  fmov.s @r15+,fr5
  8C2414F4  F6F9  fmov.s @r15+,fr6
  8C2414F6  F7F9  fmov.s @r15+,fr7
  8C2414F8  F8F9  fmov.s @r15+,fr8
  8C2414FA  F9F9  fmov.s @r15+,fr9
  8C2414FC  FAF9  fmov.s @r15+,fr10
  8C2414FE  FBF9  fmov.s @r15+,fr11
  8C241500  FCF9  fmov.s @r15+,fr12
  8C241502  FDF9  fmov.s @r15+,fr13
  8C241504  FEF9  fmov.s @r15+,fr14
  8C241506  FFF9  fmov.s @r15+,fr15
  8C241508  FBFD  frchg
  8C24150A  F0F9  fmov.s @r15+,fr0
  8C24150C  F1F9  fmov.s @r15+,fr1
  8C24150E  F2F9  fmov.s @r15+,fr2
  8C241510  F3F9  fmov.s @r15+,fr3
  8C241512  F4F9  fmov.s @r15+,fr4
  8C241514  F5F9  fmov.s @r15+,fr5
  8C241516  F6F9  fmov.s @r15+,fr6
  8C241518  F7F9  fmov.s @r15+,fr7
  8C24151A  F8F9  fmov.s @r15+,fr8
  8C24151C  F9F9  fmov.s @r15+,fr9
  8C24151E  FAF9  fmov.s @r15+,fr10
  8C241520  FBF9  fmov.s @r15+,fr11
  8C241522  FCF9  fmov.s @r15+,fr12
  8C241524  FDF9  fmov.s @r15+,fr13
  8C241526  FEF9  fmov.s @r15+,fr14
  8C241528  FFF9  fmov.s @r15+,fr15
  8C24152A  4F66  lds.l @r15+,FPSCR
  8C24152C  4F56  lds.l @r15+,FPUL
  8C24152E  4F07  ldc.l @r15+,SR
  8C241530  60F6  mov.l @r15+,r0
  8C241532  61F6  mov.l @r15+,r1
  8C241534  62F6  mov.l @r15+,r2
  8C241536  63F6  mov.l @r15+,r3
  8C241538  64F6  mov.l @r15+,r4
  8C24153A  65F6  mov.l @r15+,r5
  8C24153C  66F6  mov.l @r15+,r6
  8C24153E  67F6  mov.l @r15+,r7
  8C241540  68F6  mov.l @r15+,r8
  8C241542  69F6  mov.l @r15+,r9
  8C241544  6AF6  mov.l @r15+,r10
  8C241546  6BF6  mov.l @r15+,r11
  8C241548  6CF6  mov.l @r15+,r12
  8C24154A  6DF6  mov.l @r15+,r13
  8C24154C  6EF6  mov.l @r15+,r14
  8C24154E  4F16  lds.l @r15+,MAC
  8C241550  4F06  lds.l @r15+,MACH
  8C241552  4F27  ldc.l @r15+,VBR
  8C241554  4F17  ldc.l @r15+,GBR
  8C241556  4F26  lds.l @r15+,PR
  8C241558  4F47  ldc.l @r15+,SPC
  8C24155A  4F37  ldc.l @r15+,SSR
  8C24155C  002B  rte
  8C24155E  0009  nop
```
