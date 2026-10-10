# 056_matriz_divisao_pref

> Gerado por `tools/sdk_find.py`. Jogos: 2 · variantes (sequências normalizadas distintas): 3 · tempo perf somado: 0.73% da emu.

| Jogo | Entrada | Instr. | Tempo perf |
|---|---|---|---|
| Dead or Alive 2 (USA) | `8C10A7E0` | 130 | 0.16% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C1D5820` | 133 | 0.33% |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C1D6400` | 112 | 0.24% |

## Dead or Alive 2 (USA) `8C10A7E0`

Dump: `/mnt/1TB/dcbat_off/20261007-192359_Dead_or_Alive_2__USA__/jit-14513.txt`

```
  8C10A7E0  6346  mov.l @r4+,r3
  8C10A7E2  7620  add ##32,r6
  8C10A7E4  F3FD  fschg
  8C10A7E6  6042  mov.l @r4,r0
  8C10A7E8  6E43  mov r4,r14
  8C10A7EA  C801  tst ##1,R0
  8C10A7EC  7420  add ##32,r4
  8C10A7EE  8B02  bf 8C10A7F6
  ...
  8C10A7F6  F4E9  fmov.s @r14+,fr4
  8C10A7F8  F6E9  fmov.s @r14+,fr6
  8C10A7FA  F79D  fldi1 fr7
  8C10A7FC  F2E9  fmov.s @r14+,fr2
  8C10A7FE  F0E8  fmov.s @r14,fr0
  8C10A800  F5FD  ftrv xmtrx,fv4
  8C10A802  F38D  fldi0 fr3
  8C10A804  F79D  fldi1 fr7
  8C10A806  F743  fdiv fr4,fr7
  8C10A808  F62B  fmov.s fr2,@-r6
  8C10A80A  4310  dt r3
  8C10A80C  6046  mov.l @r4+,r0
  8C10A80E  8D4F  bt.s 8C10A8B0
  8C10A810  6E46  mov.l @r4+,r14
  8C10A812  C801  tst ##1,R0
  8C10A814  6263  mov r6,r2
  8C10A816  8D03  bt.s 8C10A820
  8C10A818  3E4C  add r4,r14
  8C10A81A  6E43  mov r4,r14
  8C10A81C  7418  add ##24,r4
  8C10A81E  7EF8  add ##-8,r14
  8C10A820  7420  add ##32,r4
  8C10A822  F8E9  fmov.s @r14+,fr8
  8C10A824  0483  pref @r4
  8C10A826  FAE9  fmov.s @r14+,fr10
  8C10A828  FB9D  fldi1 fr11
  8C10A82A  F672  fmul fr7,fr6
  8C10A82C  F2E9  fmov.s @r14+,fr2
  8C10A82E  F572  fmul fr7,fr5
  8C10A830  F60B  fmov.s fr0,@-r6
  8C10A832  F9FD  ftrv xmtrx,fv8
  8C10A834  F66B  fmov.s fr6,@-r6
  8C10A836  74E0  add ##-32,r4
  8C10A838  F64B  fmov.s fr4,@-r6
  8C10A83A  2622  mov.l r2,@r6
  8C10A83C  F0E8  fmov.s @r14,fr0
  8C10A83E  4310  dt r3
  8C10A840  0683  pref @r6
  8C10A842  7640  add ##64,r6
  8C10A844  F38D  fldi0 fr3
  8C10A846  F62B  fmov.s fr2,@-r6
  8C10A848  6263  mov r6,r2
  8C10A84A  FB9D  fldi1 fr11
  8C10A84C  FB83  fdiv fr8,fr11
  8C10A84E  6046  mov.l @r4+,r0
  8C10A850  8D1A  bt.s 8C10A888
  8C10A852  6E46  mov.l @r4+,r14
  8C10A854  C801  tst ##1,R0
  8C10A856  8D03  bt.s 8C10A860
  8C10A858  3E4C  add r4,r14
  8C10A85A  6E43  mov r4,r14
  8C10A85C  7418  add ##24,r4
  8C10A85E  7EF8  add ##-8,r14
  8C10A860  7420  add ##32,r4
  8C10A862  F4E9  fmov.s @r14+,fr4
  8C10A864  0483  pref @r4
  8C10A866  F6E9  fmov.s @r14+,fr6
  8C10A868  F79D  fldi1 fr7
  8C10A86A  FAB2  fmul fr11,fr10
  8C10A86C  F2E9  fmov.s @r14+,fr2
  8C10A86E  F9B2  fmul fr11,fr9
  8C10A870  F60B  fmov.s fr0,@-r6
  8C10A872  F5FD  ftrv xmtrx,fv4
  8C10A874  F6AB  fmov.s fr10,@-r6
  8C10A876  74E0  add ##-32,r4
  8C10A878  F68B  fmov.s fr8,@-r6
  8C10A87A  7540  add ##64,r5
  8C10A87C  2622  mov.l r2,@r6
  8C10A87E  F0E8  fmov.s @r14,fr0
  8C10A880  0683  pref @r6
  8C10A882  AFBE  bra 8C10A802
  8C10A884  7640  add ##64,r6
  ...
  8C10A888  4015  cmp/pl r0
  8C10A88A  63E3  mov r14,r3
  8C10A88C  8B0C  bf 8C10A8A8
  8C10A88E  C880  tst ##128,R0
  8C10A890  890A  bt 8C10A8A8
  8C10A892  6046  mov.l @r4+,r0
  8C10A894  6263  mov r6,r2
  8C10A896  4221  shar r2
  8C10A898  6E46  mov.l @r4+,r14
  8C10A89A  C801  tst ##1,R0
  8C10A89C  3E4C  add r4,r14
  8C10A89E  89DF  bt 8C10A860
  8C10A8A0  6E43  mov r4,r14
  8C10A8A2  7418  add ##24,r4
  8C10A8A4  AFDC  bra 8C10A860
  8C10A8A6  7EF8  add ##-8,r14
  8C10A8A8  7520  add ##32,r5
  8C10A8AA  F48C  fmov fr8,fr4
  8C10A8AC  A010  bra 8C10A8D0
  8C10A8AE  F6AC  fmov fr10,fr6
  8C10A8B0  4015  cmp/pl r0
  8C10A8B2  63E3  mov r14,r3
  8C10A8B4  8B0C  bf 8C10A8D0
  8C10A8B6  C880  tst ##128,R0
  8C10A8B8  890A  bt 8C10A8D0
  8C10A8BA  6046  mov.l @r4+,r0
  8C10A8BC  6263  mov r6,r2
  8C10A8BE  4221  shar r2
  8C10A8C0  6E46  mov.l @r4+,r14
  8C10A8C2  C801  tst ##1,R0
  8C10A8C4  3E4C  add r4,r14
  8C10A8C6  89AB  bt 8C10A820
  8C10A8C8  6E43  mov r4,r14
  8C10A8CA  7418  add ##24,r4
  8C10A8CC  AFA8  bra 8C10A820
  8C10A8CE  7EF8  add ##-8,r14
  8C10A8D0  F672  fmul fr7,fr6
  8C10A8D2  6263  mov r6,r2
  8C10A8D4  4221  shar r2
  8C10A8D6  F572  fmul fr7,fr5
  8C10A8D8  F60B  fmov.s fr0,@-r6
  8C10A8DA  74F8  add ##-8,r4
  8C10A8DC  F66B  fmov.s fr6,@-r6
  8C10A8DE  7520  add ##32,r5
  8C10A8E0  F64B  fmov.s fr4,@-r6
  8C10A8E2  2622  mov.l r2,@r6
  8C10A8E4  F3FD  fschg
  8C10A8E6  0683  pref @r6
  8C10A8E8  000B  rts
  8C10A8EA  7620  add ##32,r6
```

## Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) `8C1D5820`

Dump: `/mnt/1TB/dcbat_off/20261007-192124_Shenmue_II__Europe___En_Fr_De_Es___Disc_/jit-12387.txt`

```
  8C1D5820  6346  mov.l @r4+,r3
  8C1D5822  7620  add ##32,r6
  8C1D5824  F3FD  fschg
  8C1D5826  6042  mov.l @r4,r0
  8C1D5828  6E43  mov r4,r14
  8C1D582A  C801  tst ##1,R0
  8C1D582C  7420  add ##32,r4
  8C1D582E  8B02  bf 8C1D5836
  8C1D5830  5EE1  mov.l @(4,r14),r14
  8C1D5832  74E8  add ##-24,r4
  8C1D5834  3E4C  add r4,r14
  8C1D5836  F4E9  fmov.s @r14+,fr4
  8C1D5838  F6E9  fmov.s @r14+,fr6
  8C1D583A  F79D  fldi1 fr7
  8C1D583C  F2E9  fmov.s @r14+,fr2
  8C1D583E  F0E8  fmov.s @r14,fr0
  8C1D5840  F5FD  ftrv xmtrx,fv4
  8C1D5842  F38D  fldi0 fr3
  8C1D5844  F79D  fldi1 fr7
  8C1D5846  F743  fdiv fr4,fr7
  8C1D5848  F62B  fmov.s fr2,@-r6
  8C1D584A  4310  dt r3
  8C1D584C  6046  mov.l @r4+,r0
  8C1D584E  8D4F  bt.s 8C1D58F0
  8C1D5850  6E46  mov.l @r4+,r14
  8C1D5852  C801  tst ##1,R0
  8C1D5854  6263  mov r6,r2
  8C1D5856  8D03  bt.s 8C1D5860
  8C1D5858  3E4C  add r4,r14
  8C1D585A  6E43  mov r4,r14
  8C1D585C  7418  add ##24,r4
  8C1D585E  7EF8  add ##-8,r14
  8C1D5860  7420  add ##32,r4
  8C1D5862  F8E9  fmov.s @r14+,fr8
  8C1D5864  0483  pref @r4
  8C1D5866  FAE9  fmov.s @r14+,fr10
  8C1D5868  FB9D  fldi1 fr11
  8C1D586A  F672  fmul fr7,fr6
  8C1D586C  F2E9  fmov.s @r14+,fr2
  8C1D586E  F572  fmul fr7,fr5
  8C1D5870  F60B  fmov.s fr0,@-r6
  8C1D5872  F9FD  ftrv xmtrx,fv8
  8C1D5874  F66B  fmov.s fr6,@-r6
  8C1D5876  74E0  add ##-32,r4
  8C1D5878  F64B  fmov.s fr4,@-r6
  8C1D587A  2622  mov.l r2,@r6
  8C1D587C  F0E8  fmov.s @r14,fr0
  8C1D587E  4310  dt r3
  8C1D5880  0683  pref @r6
  8C1D5882  7640  add ##64,r6
  8C1D5884  F38D  fldi0 fr3
  8C1D5886  F62B  fmov.s fr2,@-r6
  8C1D5888  6263  mov r6,r2
  8C1D588A  FB9D  fldi1 fr11
  8C1D588C  FB83  fdiv fr8,fr11
  8C1D588E  6046  mov.l @r4+,r0
  8C1D5890  8D1A  bt.s 8C1D58C8
  8C1D5892  6E46  mov.l @r4+,r14
  8C1D5894  C801  tst ##1,R0
  8C1D5896  8D03  bt.s 8C1D58A0
  8C1D5898  3E4C  add r4,r14
  8C1D589A  6E43  mov r4,r14
  8C1D589C  7418  add ##24,r4
  8C1D589E  7EF8  add ##-8,r14
  8C1D58A0  7420  add ##32,r4
  8C1D58A2  F4E9  fmov.s @r14+,fr4
  8C1D58A4  0483  pref @r4
  8C1D58A6  F6E9  fmov.s @r14+,fr6
  8C1D58A8  F79D  fldi1 fr7
  8C1D58AA  FAB2  fmul fr11,fr10
  8C1D58AC  F2E9  fmov.s @r14+,fr2
  8C1D58AE  F9B2  fmul fr11,fr9
  8C1D58B0  F60B  fmov.s fr0,@-r6
  8C1D58B2  F5FD  ftrv xmtrx,fv4
  8C1D58B4  F6AB  fmov.s fr10,@-r6
  8C1D58B6  74E0  add ##-32,r4
  8C1D58B8  F68B  fmov.s fr8,@-r6
  8C1D58BA  7540  add ##64,r5
  8C1D58BC  2622  mov.l r2,@r6
  8C1D58BE  F0E8  fmov.s @r14,fr0
  8C1D58C0  0683  pref @r6
  8C1D58C2  AFBE  bra 8C1D5842
  8C1D58C4  7640  add ##64,r6
  ...
  8C1D58C8  4015  cmp/pl r0
  8C1D58CA  63E3  mov r14,r3
  8C1D58CC  8B0C  bf 8C1D58E8
  8C1D58CE  C880  tst ##128,R0
  8C1D58D0  890A  bt 8C1D58E8
  8C1D58D2  6046  mov.l @r4+,r0
  8C1D58D4  6263  mov r6,r2
  8C1D58D6  4221  shar r2
  8C1D58D8  6E46  mov.l @r4+,r14
  8C1D58DA  C801  tst ##1,R0
  8C1D58DC  3E4C  add r4,r14
  8C1D58DE  89DF  bt 8C1D58A0
  8C1D58E0  6E43  mov r4,r14
  8C1D58E2  7418  add ##24,r4
  8C1D58E4  AFDC  bra 8C1D58A0
  8C1D58E6  7EF8  add ##-8,r14
  8C1D58E8  7520  add ##32,r5
  8C1D58EA  F48C  fmov fr8,fr4
  8C1D58EC  A010  bra 8C1D5910
  8C1D58EE  F6AC  fmov fr10,fr6
  8C1D58F0  4015  cmp/pl r0
  8C1D58F2  63E3  mov r14,r3
  8C1D58F4  8B0C  bf 8C1D5910
  8C1D58F6  C880  tst ##128,R0
  8C1D58F8  890A  bt 8C1D5910
  8C1D58FA  6046  mov.l @r4+,r0
  8C1D58FC  6263  mov r6,r2
  8C1D58FE  4221  shar r2
  8C1D5900  6E46  mov.l @r4+,r14
  8C1D5902  C801  tst ##1,R0
  8C1D5904  3E4C  add r4,r14
  8C1D5906  89AB  bt 8C1D5860
  8C1D5908  6E43  mov r4,r14
  8C1D590A  7418  add ##24,r4
  8C1D590C  AFA8  bra 8C1D5860
  8C1D590E  7EF8  add ##-8,r14
  8C1D5910  F672  fmul fr7,fr6
  8C1D5912  6263  mov r6,r2
  8C1D5914  4221  shar r2
  8C1D5916  F572  fmul fr7,fr5
  8C1D5918  F60B  fmov.s fr0,@-r6
  8C1D591A  74F8  add ##-8,r4
  8C1D591C  F66B  fmov.s fr6,@-r6
  8C1D591E  7520  add ##32,r5
  8C1D5920  F64B  fmov.s fr4,@-r6
  8C1D5922  2622  mov.l r2,@r6
  8C1D5924  F3FD  fschg
  8C1D5926  0683  pref @r6
  8C1D5928  000B  rts
  8C1D592A  7620  add ##32,r6
```

## Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) `8C1D6400`

Dump: `/mnt/1TB/dcbat_off/20261007-192124_Shenmue_II__Europe___En_Fr_De_Es___Disc_/jit-12387.txt`

```
  8C1D6400  6346  mov.l @r4+,r3
  8C1D6402  7620  add ##32,r6
  8C1D6404  F3FD  fschg
  8C1D6406  6042  mov.l @r4,r0
  8C1D6408  6E43  mov r4,r14
  8C1D640A  C801  tst ##1,R0
  8C1D640C  7420  add ##32,r4
  8C1D640E  8B02  bf 8C1D6416
  ...
  8C1D6416  F4E9  fmov.s @r14+,fr4
  8C1D6418  F6E9  fmov.s @r14+,fr6
  8C1D641A  F79D  fldi1 fr7
  8C1D641C  F2E9  fmov.s @r14+,fr2
  8C1D641E  F0E8  fmov.s @r14,fr0
  8C1D6420  F5FD  ftrv xmtrx,fv4
  8C1D6422  F79D  fldi1 fr7
  8C1D6424  F743  fdiv fr4,fr7
  8C1D6426  F62B  fmov.s fr2,@-r6
  8C1D6428  4310  dt r3
  8C1D642A  6046  mov.l @r4+,r0
  8C1D642C  8D4E  bt.s 8C1D64CC
  8C1D642E  6E46  mov.l @r4+,r14
  8C1D6430  C801  tst ##1,R0
  8C1D6432  6263  mov r6,r2
  8C1D6434  8D03  bt.s 8C1D643E
  8C1D6436  3E4C  add r4,r14
  8C1D6438  6E43  mov r4,r14
  8C1D643A  7418  add ##24,r4
  8C1D643C  7EF8  add ##-8,r14
  8C1D643E  7420  add ##32,r4
  8C1D6440  F8E9  fmov.s @r14+,fr8
  8C1D6442  0483  pref @r4
  8C1D6444  FAE9  fmov.s @r14+,fr10
  8C1D6446  FB9D  fldi1 fr11
  8C1D6448  F672  fmul fr7,fr6
  8C1D644A  F2E9  fmov.s @r14+,fr2
  8C1D644C  F572  fmul fr7,fr5
  8C1D644E  F60B  fmov.s fr0,@-r6
  8C1D6450  F9FD  ftrv xmtrx,fv8
  8C1D6452  F66B  fmov.s fr6,@-r6
  8C1D6454  74E0  add ##-32,r4
  8C1D6456  F64B  fmov.s fr4,@-r6
  8C1D6458  2622  mov.l r2,@r6
  8C1D645A  F0E8  fmov.s @r14,fr0
  8C1D645C  4310  dt r3
  8C1D645E  0683  pref @r6
  8C1D6460  7640  add ##64,r6
  8C1D6462  F62B  fmov.s fr2,@-r6
  8C1D6464  6263  mov r6,r2
  8C1D6466  FB9D  fldi1 fr11
  8C1D6468  FB83  fdiv fr8,fr11
  8C1D646A  6046  mov.l @r4+,r0
  8C1D646C  8D1A  bt.s 8C1D64A4
  8C1D646E  6E46  mov.l @r4+,r14
  8C1D6470  C801  tst ##1,R0
  8C1D6472  8D03  bt.s 8C1D647C
  8C1D6474  3E4C  add r4,r14
  8C1D6476  6E43  mov r4,r14
  8C1D6478  7418  add ##24,r4
  8C1D647A  7EF8  add ##-8,r14
  8C1D647C  7420  add ##32,r4
  8C1D647E  F4E9  fmov.s @r14+,fr4
  8C1D6480  0483  pref @r4
  8C1D6482  F6E9  fmov.s @r14+,fr6
  8C1D6484  F79D  fldi1 fr7
  8C1D6486  FAB2  fmul fr11,fr10
  8C1D6488  F2E9  fmov.s @r14+,fr2
  8C1D648A  F9B2  fmul fr11,fr9
  8C1D648C  F60B  fmov.s fr0,@-r6
  8C1D648E  F5FD  ftrv xmtrx,fv4
  8C1D6490  F6AB  fmov.s fr10,@-r6
  8C1D6492  74E0  add ##-32,r4
  8C1D6494  F68B  fmov.s fr8,@-r6
  8C1D6496  7540  add ##64,r5
  8C1D6498  2622  mov.l r2,@r6
  8C1D649A  F0E8  fmov.s @r14,fr0
  8C1D649C  0683  pref @r6
  8C1D649E  AFC0  bra 8C1D6422
  8C1D64A0  7640  add ##64,r6
  ...
  8C1D64A4  4015  cmp/pl r0
  8C1D64A6  63E3  mov r14,r3
  8C1D64A8  8B0C  bf 8C1D64C4
  8C1D64AA  C880  tst ##128,R0
  8C1D64AC  890A  bt 8C1D64C4
  8C1D64AE  6046  mov.l @r4+,r0
  8C1D64B0  6263  mov r6,r2
  8C1D64B2  4221  shar r2
  8C1D64B4  6E46  mov.l @r4+,r14
  8C1D64B6  C801  tst ##1,R0
  8C1D64B8  3E4C  add r4,r14
  8C1D64BA  89DF  bt 8C1D647C
  8C1D64BC  6E43  mov r4,r14
  8C1D64BE  7418  add ##24,r4
  8C1D64C0  AFDC  bra 8C1D647C
  8C1D64C2  7EF8  add ##-8,r14
  8C1D64C4  7520  add ##32,r5
  8C1D64C6  F48C  fmov fr8,fr4
  8C1D64C8  A010  bra 8C1D64EC
  8C1D64CA  F6AC  fmov fr10,fr6
  ...
  8C1D64EC  F672  fmul fr7,fr6
  8C1D64EE  6263  mov r6,r2
  8C1D64F0  4221  shar r2
  8C1D64F2  F572  fmul fr7,fr5
  8C1D64F4  F60B  fmov.s fr0,@-r6
  8C1D64F6  74F8  add ##-8,r4
  8C1D64F8  F66B  fmov.s fr6,@-r6
  8C1D64FA  7520  add ##32,r5
  8C1D64FC  F64B  fmov.s fr4,@-r6
  8C1D64FE  2622  mov.l r2,@r6
  8C1D6500  F3FD  fschg
  8C1D6502  0683  pref @r6
  8C1D6504  000B  rts
  8C1D6506  7620  add ##32,r6
```
