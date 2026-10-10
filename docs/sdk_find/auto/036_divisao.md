# 036_divisao

> Gerado por `tools/sdk_find.py`. Jogos: 6 · variantes (sequências normalizadas distintas): 2 · tempo perf somado: 1.13% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C16E410` | 70 | 0.00% |
| Dead or Alive 2 (USA) | `8C0FAC40` | 70 | 0.17% |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C11EA10` | 70 | 0.85% |
| Power Stone (USA) | `0C0DF4D0` | 70 | 0.00% |
| Project Justice (USA) | `0C144E80` | 70 | 0.00% |
| Shenmue (USA) (Disc 1) | `0C1CDD90` | 70 | 0.11% |

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) `8C16E410`

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
  8C16E410  D32F  mov.l @([8C16E4D0]),r3
  8C16E412  2FE6  mov.l r14,@-r15
  8C16E414  6E4D  extu.w r4,r14
  8C16E416  3E37  cmp/gt r3,r14
  8C16E418  8B02  bf 8C16E420
  8C16E41A  D12E  mov.l @([8C16E4D4]),r1
  8C16E41C  31E8  sub r14,r1
  8C16E41E  6E13  mov r1,r14
  8C16E420  9454  mov.w @([8C16E4CC]),r4
  8C16E422  34E8  sub r14,r4
  8C16E424  A2B4  bra 8C16E990
  8C16E426  6EF6  mov.l @r15+,r14
  ...
  8C16E990  644D  extu.w r4,r4
  8C16E992  F79D  fldi1 fr7
  8C16E994  445A  lds r4,FPUL
  8C16E996  C71D  mova @([8C16EA0C]),R0
  8C16E998  F208  fmov.s @r0,fr2
  8C16E99A  C71D  mova @([8C16EA10]),R0
  8C16E99C  F108  fmov.s @r0,fr1
  8C16E99E  F770  fadd fr7,fr7
  8C16E9A0  F32D  float FPUL,fr3
  8C16E9A2  C71C  mova @([8C16EA14]),R0
  8C16E9A4  F508  fmov.s @r0,fr5
  8C16E9A6  C71C  mova @([8C16EA18]),R0
  8C16E9A8  F008  fmov.s @r0,fr0
  8C16E9AA  E40B  mov ##0x0B,r4
  8C16E9AC  E503  mov ##0x03,r5
  8C16E9AE  F322  fmul fr2,fr3
  8C16E9B0  F313  fdiv fr1,fr3
  8C16E9B2  F43C  fmov fr3,fr4
  8C16E9B4  F473  fdiv fr7,fr4
  8C16E9B6  F34C  fmov fr4,fr3
  8C16E9B8  F353  fdiv fr5,fr3
  8C16E9BA  F300  fadd fr0,fr3
  8C16E9BC  F33D  ftrc fr3, FPUL
  8C16E9BE  065A  sts FPUL,r6
  8C16E9C0  465A  lds r6,FPUL
  8C16E9C2  F32D  float FPUL,fr3
  8C16E9C4  F352  fmul fr5,fr3
  8C16E9C6  F58D  fldi0 fr5
  8C16E9C8  F431  fsub fr3,fr4
  8C16E9CA  F64C  fmov fr4,fr6
  8C16E9CC  F642  fmul fr4,fr6
  8C16E9CE  445A  lds r4,FPUL
  8C16E9D0  74FE  add ##-2,r4
  8C16E9D2  3453  cmp/ge r5,r4
  8C16E9D4  F22D  float FPUL,fr2
  8C16E9D6  F251  fsub fr5,fr2
  8C16E9D8  F56C  fmov fr6,fr5
  8C16E9DA  8DF8  bt.s 8C16E9CE
  8C16E9DC  F523  fdiv fr2,fr5
  8C16E9DE  F69D  fldi1 fr6
  8C16E9E0  E301  mov ##0x01,r3
  8C16E9E2  F36C  fmov fr6,fr3
  8C16E9E4  F351  fsub fr5,fr3
  8C16E9E6  2638  tst r3,r6
  8C16E9E8  F433  fdiv fr3,fr4
  8C16E9EA  F24C  fmov fr4,fr2
  8C16E9EC  F272  fmul fr7,fr2
  8C16E9EE  F04C  fmov fr4,fr0
  8C16E9F0  F64E  fmac fr0,fr4,fr6
  8C16E9F2  F42C  fmov fr2,fr4
  8C16E9F4  8F14  bf.s 8C16EA20
  8C16E9F6  F463  fdiv fr6,fr4
  8C16E9F8  000B  rts
  8C16E9FA  F04C  fmov fr4,fr0
  ...
  8C16EA20  F04C  fmov fr4,fr0
  8C16EA22  F04D  fneg fr0 
  8C16EA24  000B  rts
  8C16EA26  0009  nop
```

## Shenmue (USA) (Disc 1) `0C1CDD90`

Dump: `/mnt/1TB/dcbat_off/20261007-191903_Shenmue__USA___Disc_1__/jit-11043.txt`

```
  0C1CDD90  2FE6  mov.l r14,@-r15
  0C1CDD92  6E4D  extu.w r4,r14
  0C1CDD94  D31A  mov.l @([0C1CDE00]),r3
  0C1CDD96  3E37  cmp/gt r3,r14
  0C1CDD98  8B02  bf 0C1CDDA0
  0C1CDD9A  D11A  mov.l @([0C1CDE04]),r1
  0C1CDD9C  31E8  sub r14,r1
  0C1CDD9E  6E13  mov r1,r14
  0C1CDDA0  9429  mov.w @([0C1CDDF6]),r4
  0C1CDDA2  34E8  sub r14,r4
  0C1CDDA4  A2BC  bra 0C1CE320
  0C1CDDA6  6EF6  mov.l @r15+,r14
  ...
  0C1CE320  644D  extu.w r4,r4
  0C1CE322  445A  lds r4,FPUL
  0C1CE324  C71D  mova @([0C1CE39C]),R0
  0C1CE326  F708  fmov.s @r0,fr7
  0C1CE328  C71D  mova @([0C1CE3A0]),R0
  0C1CE32A  F208  fmov.s @r0,fr2
  0C1CE32C  C71D  mova @([0C1CE3A4]),R0
  0C1CE32E  F32D  float FPUL,fr3
  0C1CE330  F108  fmov.s @r0,fr1
  0C1CE332  C71D  mova @([0C1CE3A8]),R0
  0C1CE334  F508  fmov.s @r0,fr5
  0C1CE336  C71D  mova @([0C1CE3AC]),R0
  0C1CE338  F008  fmov.s @r0,fr0
  0C1CE33A  E503  mov ##0x03,r5
  0C1CE33C  E40B  mov ##0x0B,r4
  0C1CE33E  F322  fmul fr2,fr3
  0C1CE340  F313  fdiv fr1,fr3
  0C1CE342  F43C  fmov fr3,fr4
  0C1CE344  F473  fdiv fr7,fr4
  0C1CE346  F34C  fmov fr4,fr3
  0C1CE348  F353  fdiv fr5,fr3
  0C1CE34A  F300  fadd fr0,fr3
  0C1CE34C  F33D  ftrc fr3, FPUL
  0C1CE34E  065A  sts FPUL,r6
  0C1CE350  465A  lds r6,FPUL
  0C1CE352  F32D  float FPUL,fr3
  0C1CE354  F352  fmul fr5,fr3
  0C1CE356  F58D  fldi0 fr5
  0C1CE358  F431  fsub fr3,fr4
  0C1CE35A  F64C  fmov fr4,fr6
  0C1CE35C  F642  fmul fr4,fr6
  0C1CE35E  445A  lds r4,FPUL
  0C1CE360  74FE  add ##-2,r4
  0C1CE362  3453  cmp/ge r5,r4
  0C1CE364  F22D  float FPUL,fr2
  0C1CE366  F251  fsub fr5,fr2
  0C1CE368  F56C  fmov fr6,fr5
  0C1CE36A  8DF8  bt.s 0C1CE35E
  0C1CE36C  F523  fdiv fr2,fr5
  0C1CE36E  F69D  fldi1 fr6
  0C1CE370  E301  mov ##0x01,r3
  0C1CE372  F36C  fmov fr6,fr3
  0C1CE374  F351  fsub fr5,fr3
  0C1CE376  2638  tst r3,r6
  0C1CE378  F433  fdiv fr3,fr4
  0C1CE37A  F24C  fmov fr4,fr2
  0C1CE37C  F272  fmul fr7,fr2
  0C1CE37E  F04C  fmov fr4,fr0
  0C1CE380  F64E  fmac fr0,fr4,fr6
  0C1CE382  F42C  fmov fr2,fr4
  0C1CE384  8F14  bf.s 0C1CE3B0
  0C1CE386  F463  fdiv fr6,fr4
  0C1CE388  000B  rts
  0C1CE38A  F04C  fmov fr4,fr0
  ...
  0C1CE3B0  F04C  fmov fr4,fr0
  0C1CE3B2  F04D  fneg fr0 
  0C1CE3B4  000B  rts
  0C1CE3B6  0009  nop
```
