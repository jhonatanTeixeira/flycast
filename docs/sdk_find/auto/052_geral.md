# 052_geral

> Gerado por `tools/sdk_find.py`. Jogos: 5 · variantes (sequências normalizadas distintas): 1 · tempo perf somado: 0.82% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Grandia II (USA) | `8C0C20CC` | 78 | 0.00% |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C36A050` | 78 | 0.00% |
| Macross M3 | `8C1F9B80` | 78 | 0.00% |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C17F6DC` | 78 | 0.82% |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C36CB64` | 78 | 0.00% |

## Grandia II (USA) `8C0C20CC`

Dump: `/mnt/1TB/dcbat/20261002-192949_Grandia_II__USA__/jit-534412.txt`

```
  8C0C20CC  2FE6  mov.l r14,@-r15
  8C0C20CE  2FD6  mov.l r13,@-r15
  8C0C20D0  4F22  sts.l PR,@-r15
  8C0C20D2  7FEC  add ##-20,r15
  8C0C20D4  9369  mov.w @([8C0C21AA]),r3
  8C0C20D6  6E63  mov r6,r14
  8C0C20D8  9666  mov.w @([8C0C21A8]),r6
  8C0C20DA  0537  mul.l r3,r5
  8C0C20DC  364C  add r4,r6
  8C0C20DE  6263  mov r6,r2
  8C0C20E0  051A  sts MACL,r5
  8C0C20E2  352C  add r2,r5
  8C0C20E4  5151  mov.l @(4,r5),r1
  8C0C20E6  2118  tst r1,r1
  8C0C20E8  8B05  bf 8C0C20F6
  ...
  8C0C20F6  63F3  mov r15,r3
  8C0C20F8  7310  add ##16,r3
  8C0C20FA  2F36  mov.l r3,@-r15
  8C0C20FC  6D53  mov r5,r13
  8C0C20FE  62F3  mov r15,r2
  8C0C2100  7210  add ##16,r2
  8C0C2102  2F26  mov.l r2,@-r15
  8C0C2104  7D10  add ##16,r13
  8C0C2106  65F3  mov r15,r5
  8C0C2108  66F3  mov r15,r6
  8C0C210A  67F3  mov r15,r7
  8C0C210C  7508  add ##8,r5
  8C0C210E  7610  add ##16,r6
  8C0C2110  770C  add ##12,r7
  8C0C2112  B087  bsr 8C0C2224
  8C0C2114  64D3  mov r13,r4
  8C0C2116  7F08  add ##8,r15
  8C0C2118  52F4  mov.l @(16,r15),r2
  8C0C211A  4215  cmp/pl r2
  8C0C211C  8D04  bt.s 8C0C2128
  8C0C211E  E400  mov ##0x00,r4
  8C0C2120  61F2  mov.l @r15,r1
  8C0C2122  2E12  mov.l r1,@r14
  8C0C2124  A00A  bra 8C0C213C
  8C0C2126  1E41  mov.l r4,@(4,r14)
  8C0C2128  62F2  mov.l @r15,r2
  8C0C212A  53F1  mov.l @(4,r15),r3
  8C0C212C  3232  cmp/hs r3,r2
  8C0C212E  8909  bt 8C0C2144
  8C0C2130  63F2  mov.l @r15,r3
  8C0C2132  2E32  mov.l r3,@r14
  8C0C2134  52F1  mov.l @(4,r15),r2
  8C0C2136  63F2  mov.l @r15,r3
  8C0C2138  3238  sub r3,r2
  8C0C213A  1E21  mov.l r2,@(4,r14)
  8C0C213C  63F2  mov.l @r15,r3
  8C0C213E  1E32  mov.l r3,@(8,r14)
  8C0C2140  A00C  bra 8C0C215C
  8C0C2142  1E43  mov.l r4,@(12,r14)
  8C0C2144  62F2  mov.l @r15,r2
  8C0C2146  2E22  mov.l r2,@r14
  8C0C2148  63F2  mov.l @r15,r3
  8C0C214A  51D2  mov.l @(8,r13),r1
  8C0C214C  3138  sub r3,r1
  8C0C214E  1E11  mov.l r1,@(4,r14)
  8C0C2150  63D2  mov.l @r13,r3
  8C0C2152  1E32  mov.l r3,@(8,r14)
  8C0C2154  52F1  mov.l @(4,r15),r2
  8C0C2156  63D2  mov.l @r13,r3
  8C0C2158  3238  sub r3,r2
  8C0C215A  1E23  mov.l r2,@(12,r14)
  8C0C215C  53DD  mov.l @(52,r13),r3
  8C0C215E  E000  mov ##0x00,r0
  8C0C2160  1E34  mov.l r3,@(16,r14)
  8C0C2162  52F3  mov.l @(12,r15),r2
  8C0C2164  1E25  mov.l r2,@(20,r14)
  8C0C2166  53F2  mov.l @(8,r15),r3
  8C0C2168  1E36  mov.l r3,@(24,r14)
  8C0C216A  7F14  add ##20,r15
  8C0C216C  4F26  lds.l @r15+,PR
  8C0C216E  6DF6  mov.l @r15+,r13
  8C0C2170  000B  rts
  8C0C2172  6EF6  mov.l @r15+,r14
```
