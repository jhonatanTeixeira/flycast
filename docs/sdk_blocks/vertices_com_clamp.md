# Laço de vértices com clamp de cor (tipo DOA2)

> Gerado por `tools/sdk_blocks_doc.py` a partir dos dumps do JIT em `/mnt/1TB` (2026-10-10). Índice: `README.md`. Contexto: `docs/native_sdk_code.md`.

Por vértice: carrega 3 pares, `ftrv` com a matriz, `fipr` (luz), `fdiv` (1/w), clamp de cor por tabela (`fcmp/gt` + `ftrc`), 4 pares + cabeçalho de 32 bytes na Store Queue e `pref`. ~45% do JIT do DOA2. **Já nativo no DOA2** (`8C101BC4`). Também em MvC2 e Shenmue II.

Assinatura (opcodes): `F6E9 6763 F28D F270 F79D 7640`

Referência da comparação: **Dead or Alive 2 (USA)**.

| Jogo | Endereço(s) | Iguais à referência |
|---|---|---|
| Dead or Alive 2 (USA) | `8C101BBC`, `8C102592` | referência |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C12AA7C`, `8C12B452` | 81 de 81 opcodes |
| Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) | `8C1D8D1C` | 65 de 65 opcodes |

"Iguais" conta só os deslocamentos compilados nas duas sessões (o dump guarda o que o jogo executou); o literal pool (constantes) não entra.

## Dead or Alive 2 (USA)

Dump: `/mnt/1TB/dcbat_off/20261007-192359_Dead_or_Alive_2__USA__/jit-14513.txt`

```
; 8C101BBC-8C101C64
  8C101BBC  5EE1  mov.l @(4,r14),r14
  8C101BBE  74E8  add ##-24,r4
  8C101BC0  3E4C  add r4,r14
  8C101BC2  F4E9  fmov.s @r14+,fr4
  8C101BC4  F6E9  fmov.s @r14+,fr6
  8C101BC6  6763  mov r6,r7
  8C101BC8  F28D  fldi0 fr2
  8C101BCA  F270  fadd fr7,fr2
  8C101BCC  F79D  fldi1 fr7
  8C101BCE  7640  add ##64,r6
  8C101BD0  F38D  fldi0 fr3
  8C101BD2  7520  add ##32,r5
  8C101BD4  F0E9  fmov.s @r14+,fr0
  8C101BD6  F5FD  ftrv xmtrx,fv4
  8C101BD8  FB8D  fldi0 fr11
  8C101BDA  4310  dt r3
  8C101BDC  6046  mov.l @r4+,r0
  8C101BDE  8D3D  bt.s 8C101C5C
  8C101BE0  61E3  mov r14,r1
  8C101BE2  F3ED  fipr fv12,fv0
  8C101BE4  C801  tst ##1,R0
  8C101BE6  6E46  mov.l @r4+,r14
  8C101BE8  8F10  bf.s 8C101C0C
  8C101BEA  F79D  fldi1 fr7
  8C101BEC  F743  fdiv fr4,fr7
  8C101BEE  0483  pref @r4
  8C101BF0  F3B5  fcmp/gt fr11,fr3
  8C101BF2  3E4C  add r4,r14
  8C101BF4  FF1D  flds fr15,FPUL
  8C101BF6  F8ED  fipr fv12,fv8
  8C101BF8  0E83  pref @r14
  8C101BFA  8F1D  bf.s 8C101C38
  8C101BFC  F20D  fsts FPUL,fr2
  8C101BFE  F230  fadd fr3,fr2
  8C101C00  F38D  fldi0 fr3
  8C101C02  A010  bra 8C101C26
  8C101C04  FB3D  ftrc fr11, FPUL
  ...       (não compilado nesta sessão)
  8C101C08  74E4  add ##-28,r4
  8C101C0A  F79D  fldi1 fr7
  8C101C0C  F743  fdiv fr4,fr7
  8C101C0E  7418  add ##24,r4
  8C101C10  F3B5  fcmp/gt fr11,fr3
  8C101C12  FF1D  flds fr15,FPUL
  8C101C14  F8ED  fipr fv12,fv8
  8C101C16  6E43  mov r4,r14
  8C101C18  7EE0  add ##-32,r14
  8C101C1A  0483  pref @r4
  8C101C1C  8F0C  bf.s 8C101C38
  8C101C1E  F20D  fsts FPUL,fr2
  8C101C20  F230  fadd fr3,fr2
  8C101C22  FB3D  ftrc fr11, FPUL
  8C101C24  F38D  fldi0 fr3
  8C101C26  005A  sts FPUL,r0
  8C101C28  3027  cmp/gt r2,r0
  8C101C2A  4008  shll2 r0
  8C101C2C  8B05  bf 8C101C3A
  8C101C2E  F3FD  fschg
  8C101C30  F386  fmov.s @(R0,r8),fr3
  8C101C32  A002  bra 8C101C3A
  8C101C34  F3FD  fschg
  ...       (não compilado nesta sessão)
  8C101C38  F38D  fldi0 fr3
  8C101C3A  F018  fmov.s @r1,fr0
  8C101C3C  F672  fmul fr7,fr6
  8C101C3E  F62B  fmov.s fr2,@-r6
  8C101C40  F572  fmul fr7,fr5
  8C101C42  F60B  fmov.s fr0,@-r6
  8C101C44  2338  tst r3,r3
  8C101C46  F66B  fmov.s fr6,@-r6
  8C101C48  F64B  fmov.s fr4,@-r6
  8C101C4A  8D02  bt.s 8C101C52
  8C101C4C  2672  mov.l r7,@r6
  8C101C4E  F4E9  fmov.s @r14+,fr4
  8C101C50  AFB8  bra 8C101BC4
  8C101C52  0683  pref @r6
  8C101C54  7620  add ##32,r6
  8C101C56  000B  rts
  8C101C58  F3FD  fschg
  ...       (não compilado nesta sessão)
  8C101C5C  6763  mov r6,r7
  8C101C5E  4721  shar r7
  8C101C60  4015  cmp/pl r0
  8C101C62  8FD1  bf.s 8C101C08
```

```
; 8C102592-8C10263A
  8C102592  5EE1  mov.l @(4,r14),r14
  8C102594  74E8  add ##-24,r4
  8C102596  3E4C  add r4,r14
  8C102598  F4E9  fmov.s @r14+,fr4
  8C10259A  F6E9  fmov.s @r14+,fr6
  8C10259C  6763  mov r6,r7
  8C10259E  F28D  fldi0 fr2
  8C1025A0  F270  fadd fr7,fr2
  8C1025A2  F79D  fldi1 fr7
  8C1025A4  7640  add ##64,r6
  8C1025A6  F38D  fldi0 fr3
  8C1025A8  7520  add ##32,r5
  8C1025AA  F0E9  fmov.s @r14+,fr0
  8C1025AC  F5FD  ftrv xmtrx,fv4
  8C1025AE  FB8D  fldi0 fr11
  8C1025B0  4310  dt r3
  8C1025B2  6046  mov.l @r4+,r0
  8C1025B4  8D3C  bt.s 8C102630
  8C1025B6  61E3  mov r14,r1
  8C1025B8  F3ED  fipr fv12,fv0
  8C1025BA  C801  tst ##1,R0
  8C1025BC  6E46  mov.l @r4+,r14
  8C1025BE  8F0F  bf.s 8C1025E0
  8C1025C0  F79D  fldi1 fr7
  8C1025C2  F743  fdiv fr4,fr7
  8C1025C4  0483  pref @r4
  8C1025C6  F3B5  fcmp/gt fr11,fr3
  8C1025C8  3E4C  add r4,r14
  8C1025CA  FF1D  flds fr15,FPUL
  8C1025CC  F8ED  fipr fv12,fv8
  8C1025CE  0E83  pref @r14
  8C1025D0  8F1C  bf.s 8C10260C
  8C1025D2  F20D  fsts FPUL,fr2
  8C1025D4  F230  fadd fr3,fr2
  8C1025D6  F38D  fldi0 fr3
  8C1025D8  A00F  bra 8C1025FA
  8C1025DA  FB3D  ftrc fr11, FPUL
  8C1025DC  74E4  add ##-28,r4
  8C1025DE  F79D  fldi1 fr7
  8C1025E0  F743  fdiv fr4,fr7
  8C1025E2  7418  add ##24,r4
  8C1025E4  F3B5  fcmp/gt fr11,fr3
  8C1025E6  FF1D  flds fr15,FPUL
  8C1025E8  F8ED  fipr fv12,fv8
  8C1025EA  6E43  mov r4,r14
  8C1025EC  7EE0  add ##-32,r14
  8C1025EE  0483  pref @r4
  8C1025F0  8F0C  bf.s 8C10260C
  8C1025F2  F20D  fsts FPUL,fr2
  8C1025F4  F230  fadd fr3,fr2
  8C1025F6  FB3D  ftrc fr11, FPUL
  8C1025F8  F38D  fldi0 fr3
  8C1025FA  005A  sts FPUL,r0
  8C1025FC  3027  cmp/gt r2,r0
  8C1025FE  4008  shll2 r0
  8C102600  8B05  bf 8C10260E
  8C102602  F3FD  fschg
  8C102604  F386  fmov.s @(R0,r8),fr3
  8C102606  A002  bra 8C10260E
  8C102608  F3FD  fschg
  ...       (não compilado nesta sessão)
  8C10260C  F38D  fldi0 fr3
  8C10260E  F018  fmov.s @r1,fr0
  8C102610  F672  fmul fr7,fr6
  8C102612  F62B  fmov.s fr2,@-r6
  8C102614  F572  fmul fr7,fr5
  8C102616  F60B  fmov.s fr0,@-r6
  8C102618  2338  tst r3,r3
  8C10261A  F66B  fmov.s fr6,@-r6
  8C10261C  F64B  fmov.s fr4,@-r6
  8C10261E  8D02  bt.s 8C102626
  8C102620  2672  mov.l r7,@r6
  8C102622  F4E9  fmov.s @r14+,fr4
  8C102624  AFB9  bra 8C10259A
  8C102626  0683  pref @r6
  8C102628  7620  add ##32,r6
  8C10262A  000B  rts
  8C10262C  F3FD  fschg
  ...       (não compilado nesta sessão)
  8C102630  6763  mov r6,r7
  8C102632  4721  shar r7
  8C102634  4910  dt r9
  8C102636  8DD1  bt.s 8C1025DC
  8C102638  F3ED  fipr fv12,fv0
```


## Marvel vs. Capcom 2 - New Age of Heroes (Europe)

Dump: `/mnt/1TB/dcbat_off/20261007-191632_Marvel_vs__Capcom_2_-_New_Age_of_Heroes_/jit-9882.txt`

```
; 8C12AA7C-8C12AB24
  8C12AA7C  5EE1  mov.l @(4,r14),r14
  8C12AA7E  74E8  add ##-24,r4
  8C12AA80  3E4C  add r4,r14
  8C12AA82  F4E9  fmov.s @r14+,fr4
  8C12AA84  F6E9  fmov.s @r14+,fr6
  8C12AA86  6763  mov r6,r7
  8C12AA88  F28D  fldi0 fr2
  8C12AA8A  F270  fadd fr7,fr2
  8C12AA8C  F79D  fldi1 fr7
  8C12AA8E  7640  add ##64,r6
  8C12AA90  F38D  fldi0 fr3
  8C12AA92  7520  add ##32,r5
  8C12AA94  F0E9  fmov.s @r14+,fr0
  8C12AA96  F5FD  ftrv xmtrx,fv4
  8C12AA98  FB8D  fldi0 fr11
  8C12AA9A  4310  dt r3
  8C12AA9C  6046  mov.l @r4+,r0
  8C12AA9E  8D3D  bt.s 8C12AB1C
  8C12AAA0  61E3  mov r14,r1
  8C12AAA2  F3ED  fipr fv12,fv0
  8C12AAA4  C801  tst ##1,R0
  8C12AAA6  6E46  mov.l @r4+,r14
  8C12AAA8  8F10  bf.s 8C12AACC
  8C12AAAA  F79D  fldi1 fr7
  8C12AAAC  F743  fdiv fr4,fr7
  8C12AAAE  0483  pref @r4
  8C12AAB0  F3B5  fcmp/gt fr11,fr3
  8C12AAB2  3E4C  add r4,r14
  8C12AAB4  FF1D  flds fr15,FPUL
  8C12AAB6  F8ED  fipr fv12,fv8
  8C12AAB8  0E83  pref @r14
  8C12AABA  8F1D  bf.s 8C12AAF8
  8C12AABC  F20D  fsts FPUL,fr2
  8C12AABE  F230  fadd fr3,fr2
  8C12AAC0  F38D  fldi0 fr3
  8C12AAC2  A010  bra 8C12AAE6
  8C12AAC4  FB3D  ftrc fr11, FPUL
  ...       (não compilado nesta sessão)
  8C12AAC8  74E4  add ##-28,r4
  8C12AACA  F79D  fldi1 fr7
  8C12AACC  F743  fdiv fr4,fr7
  8C12AACE  7418  add ##24,r4
  8C12AAD0  F3B5  fcmp/gt fr11,fr3
  8C12AAD2  FF1D  flds fr15,FPUL
  8C12AAD4  F8ED  fipr fv12,fv8
  8C12AAD6  6E43  mov r4,r14
  8C12AAD8  7EE0  add ##-32,r14
  8C12AADA  0483  pref @r4
  8C12AADC  8F0C  bf.s 8C12AAF8
  8C12AADE  F20D  fsts FPUL,fr2
  8C12AAE0  F230  fadd fr3,fr2
  8C12AAE2  FB3D  ftrc fr11, FPUL
  8C12AAE4  F38D  fldi0 fr3
  8C12AAE6  005A  sts FPUL,r0
  8C12AAE8  3027  cmp/gt r2,r0
  8C12AAEA  4008  shll2 r0
  8C12AAEC  8B05  bf 8C12AAFA
  8C12AAEE  F3FD  fschg
  8C12AAF0  F386  fmov.s @(R0,r8),fr3
  8C12AAF2  A002  bra 8C12AAFA
  8C12AAF4  F3FD  fschg
  ...       (não compilado nesta sessão)
  8C12AAF8  F38D  fldi0 fr3
  8C12AAFA  F018  fmov.s @r1,fr0
  8C12AAFC  F672  fmul fr7,fr6
  8C12AAFE  F62B  fmov.s fr2,@-r6
  8C12AB00  F572  fmul fr7,fr5
  8C12AB02  F60B  fmov.s fr0,@-r6
  8C12AB04  2338  tst r3,r3
  8C12AB06  F66B  fmov.s fr6,@-r6
  8C12AB08  F64B  fmov.s fr4,@-r6
  8C12AB0A  8D02  bt.s 8C12AB12
  8C12AB0C  2672  mov.l r7,@r6
  8C12AB0E  F4E9  fmov.s @r14+,fr4
  8C12AB10  AFB8  bra 8C12AA84
  8C12AB12  0683  pref @r6
  8C12AB14  7620  add ##32,r6
  8C12AB16  000B  rts
  8C12AB18  F3FD  fschg
  ...       (não compilado nesta sessão)
  8C12AB1C  6763  mov r6,r7
  8C12AB1E  4721  shar r7
  8C12AB20  4015  cmp/pl r0
  8C12AB22  8FD1  bf.s 8C12AAC8
```

```
; 8C12B452-8C12B4FA
  8C12B452  5EE1  mov.l @(4,r14),r14
  8C12B454  74E8  add ##-24,r4
  8C12B456  3E4C  add r4,r14
  8C12B458  F4E9  fmov.s @r14+,fr4
  8C12B45A  F6E9  fmov.s @r14+,fr6
  8C12B45C  6763  mov r6,r7
  8C12B45E  F28D  fldi0 fr2
  8C12B460  F270  fadd fr7,fr2
  8C12B462  F79D  fldi1 fr7
  8C12B464  7640  add ##64,r6
  8C12B466  F38D  fldi0 fr3
  8C12B468  7520  add ##32,r5
  8C12B46A  F0E9  fmov.s @r14+,fr0
  8C12B46C  F5FD  ftrv xmtrx,fv4
  8C12B46E  FB8D  fldi0 fr11
  8C12B470  4310  dt r3
  8C12B472  6046  mov.l @r4+,r0
  8C12B474  8D3C  bt.s 8C12B4F0
  8C12B476  61E3  mov r14,r1
  8C12B478  F3ED  fipr fv12,fv0
  8C12B47A  C801  tst ##1,R0
  8C12B47C  6E46  mov.l @r4+,r14
  8C12B47E  8F0F  bf.s 8C12B4A0
  8C12B480  F79D  fldi1 fr7
  8C12B482  F743  fdiv fr4,fr7
  8C12B484  0483  pref @r4
  8C12B486  F3B5  fcmp/gt fr11,fr3
  8C12B488  3E4C  add r4,r14
  8C12B48A  FF1D  flds fr15,FPUL
  8C12B48C  F8ED  fipr fv12,fv8
  8C12B48E  0E83  pref @r14
  8C12B490  8F1C  bf.s 8C12B4CC
  8C12B492  F20D  fsts FPUL,fr2
  8C12B494  F230  fadd fr3,fr2
  8C12B496  F38D  fldi0 fr3
  8C12B498  A00F  bra 8C12B4BA
  8C12B49A  FB3D  ftrc fr11, FPUL
  8C12B49C  74E4  add ##-28,r4
  8C12B49E  F79D  fldi1 fr7
  8C12B4A0  F743  fdiv fr4,fr7
  8C12B4A2  7418  add ##24,r4
  8C12B4A4  F3B5  fcmp/gt fr11,fr3
  8C12B4A6  FF1D  flds fr15,FPUL
  8C12B4A8  F8ED  fipr fv12,fv8
  8C12B4AA  6E43  mov r4,r14
  8C12B4AC  7EE0  add ##-32,r14
  8C12B4AE  0483  pref @r4
  8C12B4B0  8F0C  bf.s 8C12B4CC
  8C12B4B2  F20D  fsts FPUL,fr2
  8C12B4B4  F230  fadd fr3,fr2
  8C12B4B6  FB3D  ftrc fr11, FPUL
  8C12B4B8  F38D  fldi0 fr3
  8C12B4BA  005A  sts FPUL,r0
  8C12B4BC  3027  cmp/gt r2,r0
  8C12B4BE  4008  shll2 r0
  8C12B4C0  8B05  bf 8C12B4CE
  8C12B4C2  F3FD  fschg
  8C12B4C4  F386  fmov.s @(R0,r8),fr3
  8C12B4C6  A002  bra 8C12B4CE
  8C12B4C8  F3FD  fschg
  ...       (não compilado nesta sessão)
  8C12B4CE  F018  fmov.s @r1,fr0
  8C12B4D0  F672  fmul fr7,fr6
  8C12B4D2  F62B  fmov.s fr2,@-r6
  8C12B4D4  F572  fmul fr7,fr5
  8C12B4D6  F60B  fmov.s fr0,@-r6
  8C12B4D8  2338  tst r3,r3
  8C12B4DA  F66B  fmov.s fr6,@-r6
  8C12B4DC  F64B  fmov.s fr4,@-r6
  8C12B4DE  8D02  bt.s 8C12B4E6
  8C12B4E0  2672  mov.l r7,@r6
  8C12B4E2  F4E9  fmov.s @r14+,fr4
  8C12B4E4  AFB9  bra 8C12B45A
  8C12B4E6  0683  pref @r6
  8C12B4E8  7620  add ##32,r6
  8C12B4EA  000B  rts
  8C12B4EC  F3FD  fschg
  ...       (não compilado nesta sessão)
  8C12B4F0  6763  mov r6,r7
  8C12B4F2  4721  shar r7
  8C12B4F4  4910  dt r9
  8C12B4F6  8DD1  bt.s 8C12B49C
  8C12B4F8  F3ED  fipr fv12,fv0
```


## Shenmue II (Europe) (En,Fr,De,Es) (Disc 1)

Dump: `/mnt/1TB/dcbat_off/20261007-192124_Shenmue_II__Europe___En_Fr_De_Es___Disc_/jit-12387.txt`

```
; 8C1D8D1C-8C1D8DC4
  ...       (não compilado nesta sessão)
  8C1D8D22  F4E9  fmov.s @r14+,fr4
  8C1D8D24  F6E9  fmov.s @r14+,fr6
  8C1D8D26  6763  mov r6,r7
  8C1D8D28  F28D  fldi0 fr2
  8C1D8D2A  F270  fadd fr7,fr2
  8C1D8D2C  F79D  fldi1 fr7
  8C1D8D2E  7640  add ##64,r6
  8C1D8D30  F38D  fldi0 fr3
  8C1D8D32  7520  add ##32,r5
  8C1D8D34  F0E9  fmov.s @r14+,fr0
  8C1D8D36  F5FD  ftrv xmtrx,fv4
  8C1D8D38  FB8D  fldi0 fr11
  8C1D8D3A  4310  dt r3
  8C1D8D3C  6046  mov.l @r4+,r0
  8C1D8D3E  8D3D  bt.s 8C1D8DBC
  8C1D8D40  61E3  mov r14,r1
  8C1D8D42  F3ED  fipr fv12,fv0
  8C1D8D44  C801  tst ##1,R0
  8C1D8D46  6E46  mov.l @r4+,r14
  8C1D8D48  8F10  bf.s 8C1D8D6C
  8C1D8D4A  F79D  fldi1 fr7
  ...       (não compilado nesta sessão)
  8C1D8D68  74E4  add ##-28,r4
  8C1D8D6A  F79D  fldi1 fr7
  8C1D8D6C  F743  fdiv fr4,fr7
  8C1D8D6E  7418  add ##24,r4
  8C1D8D70  F3B5  fcmp/gt fr11,fr3
  8C1D8D72  FF1D  flds fr15,FPUL
  8C1D8D74  F8ED  fipr fv12,fv8
  8C1D8D76  6E43  mov r4,r14
  8C1D8D78  7EE0  add ##-32,r14
  8C1D8D7A  0483  pref @r4
  8C1D8D7C  8F0C  bf.s 8C1D8D98
  8C1D8D7E  F20D  fsts FPUL,fr2
  8C1D8D80  F230  fadd fr3,fr2
  8C1D8D82  FB3D  ftrc fr11, FPUL
  8C1D8D84  F38D  fldi0 fr3
  8C1D8D86  005A  sts FPUL,r0
  8C1D8D88  3027  cmp/gt r2,r0
  8C1D8D8A  4008  shll2 r0
  8C1D8D8C  8B05  bf 8C1D8D9A
  8C1D8D8E  F3FD  fschg
  8C1D8D90  F386  fmov.s @(R0,r8),fr3
  8C1D8D92  A002  bra 8C1D8D9A
  8C1D8D94  F3FD  fschg
  ...       (não compilado nesta sessão)
  8C1D8D98  F38D  fldi0 fr3
  8C1D8D9A  F018  fmov.s @r1,fr0
  8C1D8D9C  F672  fmul fr7,fr6
  8C1D8D9E  F62B  fmov.s fr2,@-r6
  8C1D8DA0  F572  fmul fr7,fr5
  8C1D8DA2  F60B  fmov.s fr0,@-r6
  8C1D8DA4  2338  tst r3,r3
  8C1D8DA6  F66B  fmov.s fr6,@-r6
  8C1D8DA8  F64B  fmov.s fr4,@-r6
  8C1D8DAA  8D02  bt.s 8C1D8DB2
  8C1D8DAC  2672  mov.l r7,@r6
  8C1D8DAE  F4E9  fmov.s @r14+,fr4
  8C1D8DB0  AFB8  bra 8C1D8D24
  8C1D8DB2  0683  pref @r6
  8C1D8DB4  7620  add ##32,r6
  8C1D8DB6  000B  rts
  8C1D8DB8  F3FD  fschg
  ...       (não compilado nesta sessão)
  8C1D8DBC  6763  mov r6,r7
  8C1D8DBE  4721  shar r7
  8C1D8DC0  4015  cmp/pl r0
  8C1D8DC2  8FD1  bf.s 8C1D8D68
```
