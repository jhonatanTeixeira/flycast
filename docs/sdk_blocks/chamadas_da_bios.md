# Chamadas da BIOS (syscalls por vetor)

> Gerado por `tools/sdk_blocks_doc.py` a partir dos dumps do JIT em `/mnt/1TB` (2026-10-10). Índice: `README.md`. Contexto: `docs/native_sdk_code.md`.

Stub que carrega o número da função em `r7`, o vetor da BIOS em `r0` (`0x8C0000BC` GD-ROM, `0x8C0000B0` sysinfo, `0x8C0000B4` fonte, `0x8C0000B8` flashrom, `0x8C0000E0` misc), lê o endereço e salta. Os valores ficam no literal pool (endereço entre colchetes), que o dump do JIT não guarda; as linhas `O` do dump mostram o endereço lido.

Assinatura (opcodes): `D702 D003 6002 402B`

Referência da comparação: **Dead or Alive 2 (USA)**.

| Jogo | Endereço(s) | Iguais à referência |
|---|---|---|
| Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan) | `8C20D294`, `8C20D2BC`, `8C222764`, `8C222778` | 5 de 5 opcodes |
| Dead or Alive 2 (USA) | `8C129D0C`, `8C1369F8`, `8C136A0C` | referência |
| Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] | `8C02EF88`, `8C02EFB0`, `8C030514`, `8C030528`, `8C03053C` | 5 de 5 opcodes |
| Evolution - The World of Sacred Device (USA) | `8C1B741C`, `8C1B7444`, `8C1DCB9C`, `8C1DCBB0` | 5 de 5 opcodes |
| Evolution 2 - Far Off Promise (USA) | `8C184DC8`, `8C184DF0`, `8C1B0844`, `8C1B0858` | 5 de 5 opcodes |
| Grandia II (USA) | `8C083AEC`, `8C083B14`, `8C0C90F0`, `8C0C9104` | 5 de 5 opcodes |
| King of Fighters The - Evolution (USA) (EnJaEsPt) | `8C3487D4`, `8C3487FC`, `8C36E590`, `8C36E5A4` | 5 de 5 opcodes |
| Le Mans 24 Hours (Europe) (En,Fr,De,Es,It) | `8C0196F8`, `8C019720`, `8C045FF0`, `8C046018`, `8C219154`, `8C21917C`, `8C21E5A4`, `8C21E5B8` | 5 de 5 opcodes |
| Macross M3 | `8C1CDE48`, `8C1CDE70`, `8C1D4984`, `8C1D4998`, `8C1D49AC` | 5 de 5 opcodes |
| Marvel vs. Capcom 2 - New Age of Heroes (Europe) | `8C18B894`, `8C18B8BC`, `8C1991A4`, `8C1991B8` | 5 de 5 opcodes |
| Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) | `8C115924`, `8C11594C`, `8C16BA9C`, `8C16BAB0`, `AC00EF08`, `AC00EF1C`, `AC00EF30`, `AC00EF44` | 5 de 5 opcodes |
| Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs) | `8C01CFB8`, `8C01CFE0`, `8C385654`, `8C38567C`, `8C385690`, `8C3856A4`, `8C3856B8` | 5 de 5 opcodes |
| Power Stone (USA) | `0C0EB6CC`, `0C0EB6F4`, `0C0ED9AC`, `0C0ED9C0` | 5 de 5 opcodes |
| Project Justice (USA) | `0C2D582C`, `0C2D5854`, `0C2EB224`, `0C2EB238` | 5 de 5 opcodes |
| Resident Evil - Code - Veronica (USA) (Disc 1) | `8C1B5D7C`, `8C1B5DA4`, `8C1D7364`, `8C1D7378`, `8C1D738C` | 5 de 5 opcodes |
| Shenmue (USA) (Disc 1) | `0C1E4B24`, `0C1E4B4C`, `0C1E8584`, `0C1E8598`, `0C1E85AC` | 5 de 5 opcodes |
| Skies of Arcadia (USA) (Disc 1) | `8C2A3A18`, `8C2E92A4`, `8C2E92B8`, `8C2E92CC` | 5 de 5 opcodes |
| Sonic Adventure 2 (USA) (EnJaFrDeEs) | `8C13738C`, `8C1373B4`, `8C15CEF4`, `8C15CF08` | 5 de 5 opcodes |
| Soulcalibur (USA) | `8C0FF2B8`, `8C0FF2E0`, `8C1014C8`, `8C1014DC` | 5 de 5 opcodes |
| Tomb Raider Chronicles (USA) | `01DF2BF8`, `01DF2D64`, `01DF2D78` | 5 de 5 opcodes |

"Iguais" conta só os deslocamentos compilados nas duas sessões (o dump guarda o que o jogo executou); o literal pool (constantes) não entra.

## Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan)

Dump: `/mnt/1TB/dcbat/20261002-160455_Capcom_vs__SNK_2_-_Millionaire_Fighting_/jit-462755.txt`

```
; 8C20D294-8C20D29E
  8C20D294  D702  mov.l @([8C20D2A0]),r7
  8C20D296  D003  mov.l @([8C20D2A4]),r0
  8C20D298  6002  mov.l @r0,r0
  8C20D29A  402B  jmp @r0
  8C20D29C  0009  nop
```

```
; 8C20D2BC-8C20D2C6
  8C20D2BC  D702  mov.l @([8C20D2C8]),r7
  8C20D2BE  D003  mov.l @([8C20D2CC]),r0
  8C20D2C0  6002  mov.l @r0,r0
  8C20D2C2  402B  jmp @r0
  8C20D2C4  0009  nop
```

```
; 8C222764-8C22276E
  8C222764  D702  mov.l @([8C222770]),r7
  8C222766  D003  mov.l @([8C222774]),r0
  8C222768  6002  mov.l @r0,r0
  8C22276A  402B  jmp @r0
  8C22276C  0009  nop
```

```
; 8C222778-8C222782
  8C222778  D702  mov.l @([8C222784]),r7
  8C22277A  D003  mov.l @([8C222788]),r0
  8C22277C  6002  mov.l @r0,r0
  8C22277E  402B  jmp @r0
  8C222780  0009  nop
```


## Dead or Alive 2 (USA)

Dump: `/mnt/1TB/dcbat_off/20261007-192359_Dead_or_Alive_2__USA__/jit-14513.txt`

```
; 8C129D0C-8C129D16
  8C129D0C  D702  mov.l @([8C129D18]),r7
  8C129D0E  D003  mov.l @([8C129D1C]),r0
  8C129D10  6002  mov.l @r0,r0
  8C129D12  402B  jmp @r0
  8C129D14  0009  nop
```

```
; 8C1369F8-8C136A02
  8C1369F8  D702  mov.l @([8C136A04]),r7
  8C1369FA  D003  mov.l @([8C136A08]),r0
  8C1369FC  6002  mov.l @r0,r0
  8C1369FE  402B  jmp @r0
  8C136A00  0009  nop
```

```
; 8C136A0C-8C136A16
  8C136A0C  D702  mov.l @([8C136A18]),r7
  8C136A0E  D003  mov.l @([8C136A1C]),r0
  8C136A10  6002  mov.l @r0,r0
  8C136A12  402B  jmp @r0
  8C136A14  0009  nop
```


## Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!]

Dump: `/mnt/1TB/dcbat_off/20261007-185447_Elemental_Gimmick_Gear_v1_001__1999__Vat/jit-2004.txt`

```
; 8C02EF88-8C02EF92
  8C02EF88  D702  mov.l @([8C02EF94]),r7
  8C02EF8A  D003  mov.l @([8C02EF98]),r0
  8C02EF8C  6002  mov.l @r0,r0
  8C02EF8E  402B  jmp @r0
  8C02EF90  0009  nop
```

```
; 8C02EFB0-8C02EFBA
  8C02EFB0  D702  mov.l @([8C02EFBC]),r7
  8C02EFB2  D003  mov.l @([8C02EFC0]),r0
  8C02EFB4  6002  mov.l @r0,r0
  8C02EFB6  402B  jmp @r0
  8C02EFB8  0009  nop
```

```
; 8C030514-8C03051E
  8C030514  D702  mov.l @([8C030520]),r7
  8C030516  D003  mov.l @([8C030524]),r0
  8C030518  6002  mov.l @r0,r0
  8C03051A  402B  jmp @r0
  8C03051C  0009  nop
```

```
; 8C030528-8C030532
  8C030528  D702  mov.l @([8C030534]),r7
  8C03052A  D003  mov.l @([8C030538]),r0
  8C03052C  6002  mov.l @r0,r0
  8C03052E  402B  jmp @r0
  8C030530  0009  nop
```

```
; 8C03053C-8C030546
  8C03053C  D702  mov.l @([8C030548]),r7
  8C03053E  D003  mov.l @([8C03054C]),r0
  8C030540  6002  mov.l @r0,r0
  8C030542  402B  jmp @r0
  8C030544  0009  nop
```


## Evolution - The World of Sacred Device (USA)

Dump: `/mnt/1TB/dcbat/20261007-151806_Evolution_-_The_World_of_Sacred_Device__/jit-24035.txt`

```
; 8C1B741C-8C1B7426
  8C1B741C  D702  mov.l @([8C1B7428]),r7
  8C1B741E  D003  mov.l @([8C1B742C]),r0
  8C1B7420  6002  mov.l @r0,r0
  8C1B7422  402B  jmp @r0
  8C1B7424  0009  nop
```

```
; 8C1B7444-8C1B744E
  8C1B7444  D702  mov.l @([8C1B7450]),r7
  8C1B7446  D003  mov.l @([8C1B7454]),r0
  8C1B7448  6002  mov.l @r0,r0
  8C1B744A  402B  jmp @r0
  8C1B744C  0009  nop
```

```
; 8C1DCB9C-8C1DCBA6
  8C1DCB9C  D702  mov.l @([8C1DCBA8]),r7
  8C1DCB9E  D003  mov.l @([8C1DCBAC]),r0
  8C1DCBA0  6002  mov.l @r0,r0
  8C1DCBA2  402B  jmp @r0
  8C1DCBA4  0009  nop
```

```
; 8C1DCBB0-8C1DCBBA
  8C1DCBB0  D702  mov.l @([8C1DCBBC]),r7
  8C1DCBB2  D003  mov.l @([8C1DCBC0]),r0
  8C1DCBB4  6002  mov.l @r0,r0
  8C1DCBB6  402B  jmp @r0
  8C1DCBB8  0009  nop
```


## Evolution 2 - Far Off Promise (USA)

Dump: `/mnt/1TB/dcbat/20261007-151251_Evolution_2_-_Far_Off_Promise__USA__/jit-16237.txt`

```
; 8C184DC8-8C184DD2
  8C184DC8  D702  mov.l @([8C184DD4]),r7
  8C184DCA  D003  mov.l @([8C184DD8]),r0
  8C184DCC  6002  mov.l @r0,r0
  8C184DCE  402B  jmp @r0
  8C184DD0  0009  nop
```

```
; 8C184DF0-8C184DFA
  8C184DF0  D702  mov.l @([8C184DFC]),r7
  8C184DF2  D003  mov.l @([8C184E00]),r0
  8C184DF4  6002  mov.l @r0,r0
  8C184DF6  402B  jmp @r0
  8C184DF8  0009  nop
```

```
; 8C1B0844-8C1B084E
  8C1B0844  D702  mov.l @([8C1B0850]),r7
  8C1B0846  D003  mov.l @([8C1B0854]),r0
  8C1B0848  6002  mov.l @r0,r0
  8C1B084A  402B  jmp @r0
  8C1B084C  0009  nop
```

```
; 8C1B0858-8C1B0862
  8C1B0858  D702  mov.l @([8C1B0864]),r7
  8C1B085A  D003  mov.l @([8C1B0868]),r0
  8C1B085C  6002  mov.l @r0,r0
  8C1B085E  402B  jmp @r0
  8C1B0860  0009  nop
```


## Grandia II (USA)

Dump: `/mnt/1TB/dcbat/20261007-152308_Grandia_II__USA__/jit-32936.txt`

```
; 8C083AEC-8C083AF6
  8C083AEC  D702  mov.l @([8C083AF8]),r7
  8C083AEE  D003  mov.l @([8C083AFC]),r0
  8C083AF0  6002  mov.l @r0,r0
  8C083AF2  402B  jmp @r0
  8C083AF4  0009  nop
```

```
; 8C083B14-8C083B1E
  8C083B14  D702  mov.l @([8C083B20]),r7
  8C083B16  D003  mov.l @([8C083B24]),r0
  8C083B18  6002  mov.l @r0,r0
  8C083B1A  402B  jmp @r0
  8C083B1C  0009  nop
```

```
; 8C0C90F0-8C0C90FA
  8C0C90F0  D702  mov.l @([8C0C90FC]),r7
  8C0C90F2  D003  mov.l @([8C0C9100]),r0
  8C0C90F4  6002  mov.l @r0,r0
  8C0C90F6  402B  jmp @r0
  8C0C90F8  0009  nop
```

```
; 8C0C9104-8C0C910E
  8C0C9104  D702  mov.l @([8C0C9110]),r7
  8C0C9106  D003  mov.l @([8C0C9114]),r0
  8C0C9108  6002  mov.l @r0,r0
  8C0C910A  402B  jmp @r0
  8C0C910C  0009  nop
```


## King of Fighters The - Evolution (USA) (EnJaEsPt)

Dump: `/mnt/1TB/dcbat/20261007-164133_King_of_Fighters_The_-_Evolution__USA___/jit-113430.txt`

```
; 8C3487D4-8C3487DE
  8C3487D4  D702  mov.l @([8C3487E0]),r7
  8C3487D6  D003  mov.l @([8C3487E4]),r0
  8C3487D8  6002  mov.l @r0,r0
  8C3487DA  402B  jmp @r0
  8C3487DC  0009  nop
```

```
; 8C3487FC-8C348806
  8C3487FC  D702  mov.l @([8C348808]),r7
  8C3487FE  D003  mov.l @([8C34880C]),r0
  8C348800  6002  mov.l @r0,r0
  8C348802  402B  jmp @r0
  8C348804  0009  nop
```

```
; 8C36E590-8C36E59A
  8C36E590  D702  mov.l @([8C36E59C]),r7
  8C36E592  D003  mov.l @([8C36E5A0]),r0
  8C36E594  6002  mov.l @r0,r0
  8C36E596  402B  jmp @r0
  8C36E598  0009  nop
```

```
; 8C36E5A4-8C36E5AE
  8C36E5A4  D702  mov.l @([8C36E5B0]),r7
  8C36E5A6  D003  mov.l @([8C36E5B4]),r0
  8C36E5A8  6002  mov.l @r0,r0
  8C36E5AA  402B  jmp @r0
  8C36E5AC  0009  nop
```


## Le Mans 24 Hours (Europe) (En,Fr,De,Es,It)

Dump: `/mnt/1TB/dcbat/20261007-163759_Le_Mans_24_Hours__Europe___En_Fr_De_Es_I/jit-109461.txt`

```
; 8C0196F8-8C019702
  8C0196F8  D702  mov.l @([8C019704]),r7
  8C0196FA  D003  mov.l @([8C019708]),r0
  8C0196FC  6002  mov.l @r0,r0
  8C0196FE  402B  jmp @r0
  8C019700  0009  nop
```

```
; 8C019720-8C01972A
  8C019720  D702  mov.l @([8C01972C]),r7
  8C019722  D003  mov.l @([8C019730]),r0
  8C019724  6002  mov.l @r0,r0
  8C019726  402B  jmp @r0
  8C019728  0009  nop
```

```
; 8C045FF0-8C045FFA
  8C045FF0  D702  mov.l @([8C045FFC]),r7
  8C045FF2  D003  mov.l @([8C046000]),r0
  8C045FF4  6002  mov.l @r0,r0
  8C045FF6  402B  jmp @r0
  8C045FF8  0009  nop
```

```
; 8C046018-8C046022
  8C046018  D702  mov.l @([8C046024]),r7
  8C04601A  D003  mov.l @([8C046028]),r0
  8C04601C  6002  mov.l @r0,r0
  8C04601E  402B  jmp @r0
  8C046020  0009  nop
```

```
; 8C219154-8C21915E
  8C219154  D702  mov.l @([8C219160]),r7
  8C219156  D003  mov.l @([8C219164]),r0
  8C219158  6002  mov.l @r0,r0
  8C21915A  402B  jmp @r0
  8C21915C  0009  nop
```

```
; 8C21917C-8C219186
  8C21917C  D702  mov.l @([8C219188]),r7
  8C21917E  D003  mov.l @([8C21918C]),r0
  8C219180  6002  mov.l @r0,r0
  8C219182  402B  jmp @r0
  8C219184  0009  nop
```

```
; 8C21E5A4-8C21E5AE
  8C21E5A4  D702  mov.l @([8C21E5B0]),r7
  8C21E5A6  D003  mov.l @([8C21E5B4]),r0
  8C21E5A8  6002  mov.l @r0,r0
  8C21E5AA  402B  jmp @r0
  8C21E5AC  0009  nop
```

```
; 8C21E5B8-8C21E5C2
  8C21E5B8  D702  mov.l @([8C21E5C4]),r7
  8C21E5BA  D003  mov.l @([8C21E5C8]),r0
  8C21E5BC  6002  mov.l @r0,r0
  8C21E5BE  402B  jmp @r0
  8C21E5C0  0009  nop
```


## Macross M3

Dump: `/mnt/1TB/dcbat/20261007-153242_Macross_M3_/jit-35982.txt`

```
; 8C1CDE48-8C1CDE52
  8C1CDE48  D702  mov.l @([8C1CDE54]),r7
  8C1CDE4A  D003  mov.l @([8C1CDE58]),r0
  8C1CDE4C  6002  mov.l @r0,r0
  8C1CDE4E  402B  jmp @r0
  8C1CDE50  0009  nop
```

```
; 8C1CDE70-8C1CDE7A
  8C1CDE70  D702  mov.l @([8C1CDE7C]),r7
  8C1CDE72  D003  mov.l @([8C1CDE80]),r0
  8C1CDE74  6002  mov.l @r0,r0
  8C1CDE76  402B  jmp @r0
  8C1CDE78  0009  nop
```

```
; 8C1D4984-8C1D498E
  8C1D4984  D702  mov.l @([8C1D4990]),r7
  8C1D4986  D003  mov.l @([8C1D4994]),r0
  8C1D4988  6002  mov.l @r0,r0
  8C1D498A  402B  jmp @r0
  8C1D498C  0009  nop
```

```
; 8C1D4998-8C1D49A2
  8C1D4998  D702  mov.l @([8C1D49A4]),r7
  8C1D499A  D003  mov.l @([8C1D49A8]),r0
  8C1D499C  6002  mov.l @r0,r0
  8C1D499E  402B  jmp @r0
  8C1D49A0  0009  nop
```

```
; 8C1D49AC-8C1D49B6
  8C1D49AC  D702  mov.l @([8C1D49B8]),r7
  8C1D49AE  D003  mov.l @([8C1D49BC]),r0
  8C1D49B0  6002  mov.l @r0,r0
  8C1D49B2  402B  jmp @r0
  8C1D49B4  0009  nop
```


## Marvel vs. Capcom 2 - New Age of Heroes (Europe)

Dump: `/mnt/1TB/dcbat_off/20261007-191632_Marvel_vs__Capcom_2_-_New_Age_of_Heroes_/jit-9882.txt`

```
; 8C18B894-8C18B89E
  8C18B894  D702  mov.l @([8C18B8A0]),r7
  8C18B896  D003  mov.l @([8C18B8A4]),r0
  8C18B898  6002  mov.l @r0,r0
  8C18B89A  402B  jmp @r0
  8C18B89C  0009  nop
```

```
; 8C18B8BC-8C18B8C6
  8C18B8BC  D702  mov.l @([8C18B8C8]),r7
  8C18B8BE  D003  mov.l @([8C18B8CC]),r0
  8C18B8C0  6002  mov.l @r0,r0
  8C18B8C2  402B  jmp @r0
  8C18B8C4  0009  nop
```

```
; 8C1991A4-8C1991AE
  8C1991A4  D702  mov.l @([8C1991B0]),r7
  8C1991A6  D003  mov.l @([8C1991B4]),r0
  8C1991A8  6002  mov.l @r0,r0
  8C1991AA  402B  jmp @r0
  8C1991AC  0009  nop
```

```
; 8C1991B8-8C1991C2
  8C1991B8  D702  mov.l @([8C1991C4]),r7
  8C1991BA  D003  mov.l @([8C1991C8]),r0
  8C1991BC  6002  mov.l @r0,r0
  8C1991BE  402B  jmp @r0
  8C1991C0  0009  nop
```


## Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0)

Dump: `/mnt/1TB/dcbat_off/20261007-190751_Napple_Tale_-_Arsia_in_Daydream__Japan__/jit-6592.txt`

```
; 8C115924-8C11592E
  8C115924  D702  mov.l @([8C115930]),r7
  8C115926  D003  mov.l @([8C115934]),r0
  8C115928  6002  mov.l @r0,r0
  8C11592A  402B  jmp @r0
  8C11592C  0009  nop
```

```
; 8C11594C-8C115956
  8C11594C  D702  mov.l @([8C115958]),r7
  8C11594E  D003  mov.l @([8C11595C]),r0
  8C115950  6002  mov.l @r0,r0
  8C115952  402B  jmp @r0
  8C115954  0009  nop
```

```
; 8C16BA9C-8C16BAA6
  8C16BA9C  D702  mov.l @([8C16BAA8]),r7
  8C16BA9E  D003  mov.l @([8C16BAAC]),r0
  8C16BAA0  6002  mov.l @r0,r0
  8C16BAA2  402B  jmp @r0
  8C16BAA4  0009  nop
```

```
; 8C16BAB0-8C16BABA
  8C16BAB0  D702  mov.l @([8C16BABC]),r7
  8C16BAB2  D003  mov.l @([8C16BAC0]),r0
  8C16BAB4  6002  mov.l @r0,r0
  8C16BAB6  402B  jmp @r0
  8C16BAB8  0009  nop
```

```
; AC00EF08-AC00EF12
  AC00EF08  D702  mov.l @([AC00EF14]),r7
  AC00EF0A  D003  mov.l @([AC00EF18]),r0
  AC00EF0C  6002  mov.l @r0,r0
  AC00EF0E  402B  jmp @r0
  AC00EF10  0009  nop
```

```
; AC00EF1C-AC00EF26
  AC00EF1C  D702  mov.l @([AC00EF28]),r7
  AC00EF1E  D003  mov.l @([AC00EF2C]),r0
  AC00EF20  6002  mov.l @r0,r0
  AC00EF22  402B  jmp @r0
  AC00EF24  0009  nop
```

```
; AC00EF30-AC00EF3A
  AC00EF30  D702  mov.l @([AC00EF3C]),r7
  AC00EF32  D003  mov.l @([AC00EF40]),r0
  AC00EF34  6002  mov.l @r0,r0
  AC00EF36  402B  jmp @r0
  AC00EF38  0009  nop
```

```
; AC00EF44-AC00EF4E
  AC00EF44  D702  mov.l @([AC00EF50]),r7
  AC00EF46  D003  mov.l @([AC00EF54]),r0
  AC00EF48  6002  mov.l @r0,r0
  AC00EF4A  402B  jmp @r0
  AC00EF4C  0009  nop
```


## Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs)

Dump: `/mnt/1TB/dcbat/20261007-155901_Phantasy_Star_Online_Ver__2__USA___EnJaF/jit-73352.txt`

```
; 8C01CFB8-8C01CFC2
  8C01CFB8  D702  mov.l @([8C01CFC4]),r7
  8C01CFBA  D003  mov.l @([8C01CFC8]),r0
  8C01CFBC  6002  mov.l @r0,r0
  8C01CFBE  402B  jmp @r0
  8C01CFC0  0009  nop
```

```
; 8C01CFE0-8C01CFEA
  8C01CFE0  D702  mov.l @([8C01CFEC]),r7
  8C01CFE2  D003  mov.l @([8C01CFF0]),r0
  8C01CFE4  6002  mov.l @r0,r0
  8C01CFE6  402B  jmp @r0
  8C01CFE8  0009  nop
```

```
; 8C385654-8C38565E
  8C385654  D702  mov.l @([8C385660]),r7
  8C385656  D003  mov.l @([8C385664]),r0
  8C385658  6002  mov.l @r0,r0
  8C38565A  402B  jmp @r0
  8C38565C  0009  nop
```

```
; 8C38567C-8C385686
  8C38567C  D702  mov.l @([8C385688]),r7
  8C38567E  D003  mov.l @([8C38568C]),r0
  8C385680  6002  mov.l @r0,r0
  8C385682  402B  jmp @r0
  8C385684  0009  nop
```

```
; 8C385690-8C38569A
  8C385690  D702  mov.l @([8C38569C]),r7
  8C385692  D003  mov.l @([8C3856A0]),r0
  8C385694  6002  mov.l @r0,r0
  8C385696  402B  jmp @r0
  8C385698  0009  nop
```

```
; 8C3856A4-8C3856AE
  8C3856A4  D702  mov.l @([8C3856B0]),r7
  8C3856A6  D003  mov.l @([8C3856B4]),r0
  8C3856A8  6002  mov.l @r0,r0
  8C3856AA  402B  jmp @r0
  8C3856AC  0009  nop
```

```
; 8C3856B8-8C3856C2
  8C3856B8  D702  mov.l @([8C3856C4]),r7
  8C3856BA  D003  mov.l @([8C3856C8]),r0
  8C3856BC  6002  mov.l @r0,r0
  8C3856BE  402B  jmp @r0
  8C3856C0  0009  nop
```


## Power Stone (USA)

Dump: `/mnt/1TB/dcbat/20261007-160119_Power_Stone__USA__/jit-75898.txt`

```
; 0C0EB6CC-0C0EB6D6
  0C0EB6CC  D702  mov.l @([0C0EB6D8]),r7
  0C0EB6CE  D003  mov.l @([0C0EB6DC]),r0
  0C0EB6D0  6002  mov.l @r0,r0
  0C0EB6D2  402B  jmp @r0
  0C0EB6D4  0009  nop
```

```
; 0C0EB6F4-0C0EB6FE
  0C0EB6F4  D702  mov.l @([0C0EB700]),r7
  0C0EB6F6  D003  mov.l @([0C0EB704]),r0
  0C0EB6F8  6002  mov.l @r0,r0
  0C0EB6FA  402B  jmp @r0
  0C0EB6FC  0009  nop
```

```
; 0C0ED9AC-0C0ED9B6
  0C0ED9AC  D702  mov.l @([0C0ED9B8]),r7
  0C0ED9AE  D003  mov.l @([0C0ED9BC]),r0
  0C0ED9B0  6002  mov.l @r0,r0
  0C0ED9B2  402B  jmp @r0
  0C0ED9B4  0009  nop
```

```
; 0C0ED9C0-0C0ED9CA
  0C0ED9C0  D702  mov.l @([0C0ED9CC]),r7
  0C0ED9C2  D003  mov.l @([0C0ED9D0]),r0
  0C0ED9C4  6002  mov.l @r0,r0
  0C0ED9C6  402B  jmp @r0
  0C0ED9C8  0009  nop
```


## Project Justice (USA)

Dump: `/mnt/1TB/dcbat/20261007-160314_Project_Justice__USA__/jit-78773.txt`

```
; 0C2D582C-0C2D5836
  0C2D582C  D702  mov.l @([0C2D5838]),r7
  0C2D582E  D003  mov.l @([0C2D583C]),r0
  0C2D5830  6002  mov.l @r0,r0
  0C2D5832  402B  jmp @r0
  0C2D5834  0009  nop
```

```
; 0C2D5854-0C2D585E
  0C2D5854  D702  mov.l @([0C2D5860]),r7
  0C2D5856  D003  mov.l @([0C2D5864]),r0
  0C2D5858  6002  mov.l @r0,r0
  0C2D585A  402B  jmp @r0
  0C2D585C  0009  nop
```

```
; 0C2EB224-0C2EB22E
  0C2EB224  D702  mov.l @([0C2EB230]),r7
  0C2EB226  D003  mov.l @([0C2EB234]),r0
  0C2EB228  6002  mov.l @r0,r0
  0C2EB22A  402B  jmp @r0
  0C2EB22C  0009  nop
```

```
; 0C2EB238-0C2EB242
  0C2EB238  D702  mov.l @([0C2EB244]),r7
  0C2EB23A  D003  mov.l @([0C2EB248]),r0
  0C2EB23C  6002  mov.l @r0,r0
  0C2EB23E  402B  jmp @r0
  0C2EB240  0009  nop
```


## Resident Evil - Code - Veronica (USA) (Disc 1)

Dump: `/mnt/1TB/dcbat/20261007-160515_Resident_Evil_-_Code_-_Veronica__USA___D/jit-82021.txt`

```
; 8C1B5D7C-8C1B5D86
  8C1B5D7C  D702  mov.l @([8C1B5D88]),r7
  8C1B5D7E  D003  mov.l @([8C1B5D8C]),r0
  8C1B5D80  6002  mov.l @r0,r0
  8C1B5D82  402B  jmp @r0
  8C1B5D84  0009  nop
```

```
; 8C1B5DA4-8C1B5DAE
  8C1B5DA4  D702  mov.l @([8C1B5DB0]),r7
  8C1B5DA6  D003  mov.l @([8C1B5DB4]),r0
  8C1B5DA8  6002  mov.l @r0,r0
  8C1B5DAA  402B  jmp @r0
  8C1B5DAC  0009  nop
```

```
; 8C1D7364-8C1D736E
  8C1D7364  D702  mov.l @([8C1D7370]),r7
  8C1D7366  D003  mov.l @([8C1D7374]),r0
  8C1D7368  6002  mov.l @r0,r0
  8C1D736A  402B  jmp @r0
  8C1D736C  0009  nop
```

```
; 8C1D7378-8C1D7382
  8C1D7378  D702  mov.l @([8C1D7384]),r7
  8C1D737A  D003  mov.l @([8C1D7388]),r0
  8C1D737C  6002  mov.l @r0,r0
  8C1D737E  402B  jmp @r0
  8C1D7380  0009  nop
```

```
; 8C1D738C-8C1D7396
  8C1D738C  D702  mov.l @([8C1D7398]),r7
  8C1D738E  D003  mov.l @([8C1D739C]),r0
  8C1D7390  6002  mov.l @r0,r0
  8C1D7392  402B  jmp @r0
  8C1D7394  0009  nop
```


## Shenmue (USA) (Disc 1)

Dump: `/mnt/1TB/dcbat_off/20261007-191903_Shenmue__USA___Disc_1__/jit-11043.txt`

```
; 0C1E4B24-0C1E4B2E
  0C1E4B24  D702  mov.l @([0C1E4B30]),r7
  0C1E4B26  D003  mov.l @([0C1E4B34]),r0
  0C1E4B28  6002  mov.l @r0,r0
  0C1E4B2A  402B  jmp @r0
  0C1E4B2C  0009  nop
```

```
; 0C1E4B4C-0C1E4B56
  0C1E4B4C  D702  mov.l @([0C1E4B58]),r7
  0C1E4B4E  D003  mov.l @([0C1E4B5C]),r0
  0C1E4B50  6002  mov.l @r0,r0
  0C1E4B52  402B  jmp @r0
  0C1E4B54  0009  nop
```

```
; 0C1E8584-0C1E858E
  0C1E8584  D702  mov.l @([0C1E8590]),r7
  0C1E8586  D003  mov.l @([0C1E8594]),r0
  0C1E8588  6002  mov.l @r0,r0
  0C1E858A  402B  jmp @r0
  0C1E858C  0009  nop
```

```
; 0C1E8598-0C1E85A2
  0C1E8598  D702  mov.l @([0C1E85A4]),r7
  0C1E859A  D003  mov.l @([0C1E85A8]),r0
  0C1E859C  6002  mov.l @r0,r0
  0C1E859E  402B  jmp @r0
  0C1E85A0  0009  nop
```

```
; 0C1E85AC-0C1E85B6
  0C1E85AC  D702  mov.l @([0C1E85B8]),r7
  0C1E85AE  D003  mov.l @([0C1E85BC]),r0
  0C1E85B0  6002  mov.l @r0,r0
  0C1E85B2  402B  jmp @r0
  0C1E85B4  0009  nop
```


## Skies of Arcadia (USA) (Disc 1)

Dump: `/mnt/1TB/dcbat/20261007-162126_Skies_of_Arcadia__USA___Disc_1__/jit-98449.txt`

```
; 8C2A3A18-8C2A3A22
  8C2A3A18  D702  mov.l @([8C2A3A24]),r7
  8C2A3A1A  D003  mov.l @([8C2A3A28]),r0
  8C2A3A1C  6002  mov.l @r0,r0
  8C2A3A1E  402B  jmp @r0
  8C2A3A20  0009  nop
```

```
; 8C2E92A4-8C2E92AE
  8C2E92A4  D702  mov.l @([8C2E92B0]),r7
  8C2E92A6  D003  mov.l @([8C2E92B4]),r0
  8C2E92A8  6002  mov.l @r0,r0
  8C2E92AA  402B  jmp @r0
  8C2E92AC  0009  nop
```

```
; 8C2E92B8-8C2E92C2
  8C2E92B8  D702  mov.l @([8C2E92C4]),r7
  8C2E92BA  D003  mov.l @([8C2E92C8]),r0
  8C2E92BC  6002  mov.l @r0,r0
  8C2E92BE  402B  jmp @r0
  8C2E92C0  0009  nop
```

```
; 8C2E92CC-8C2E92D6
  8C2E92CC  D702  mov.l @([8C2E92D8]),r7
  8C2E92CE  D003  mov.l @([8C2E92DC]),r0
  8C2E92D0  6002  mov.l @r0,r0
  8C2E92D2  402B  jmp @r0
  8C2E92D4  0009  nop
```


## Sonic Adventure 2 (USA) (EnJaFrDeEs)

Dump: `/mnt/1TB/dcbat/20261007-163253_Sonic_Adventure_2__USA___EnJaFrDeEs__/jit-102386.txt`

```
; 8C13738C-8C137396
  8C13738C  D702  mov.l @([8C137398]),r7
  8C13738E  D003  mov.l @([8C13739C]),r0
  8C137390  6002  mov.l @r0,r0
  8C137392  402B  jmp @r0
  8C137394  0009  nop
```

```
; 8C1373B4-8C1373BE
  8C1373B4  D702  mov.l @([8C1373C0]),r7
  8C1373B6  D003  mov.l @([8C1373C4]),r0
  8C1373B8  6002  mov.l @r0,r0
  8C1373BA  402B  jmp @r0
  8C1373BC  0009  nop
```

```
; 8C15CEF4-8C15CEFE
  8C15CEF4  D702  mov.l @([8C15CF00]),r7
  8C15CEF6  D003  mov.l @([8C15CF04]),r0
  8C15CEF8  6002  mov.l @r0,r0
  8C15CEFA  402B  jmp @r0
  8C15CEFC  0009  nop
```

```
; 8C15CF08-8C15CF12
  8C15CF08  D702  mov.l @([8C15CF14]),r7
  8C15CF0A  D003  mov.l @([8C15CF18]),r0
  8C15CF0C  6002  mov.l @r0,r0
  8C15CF0E  402B  jmp @r0
  8C15CF10  0009  nop
```


## Soulcalibur (USA)

Dump: `/mnt/1TB/dcbat/20261007-163634_Soulcalibur__USA__/jit-108030.txt`

```
; 8C0FF2B8-8C0FF2C2
  8C0FF2B8  D702  mov.l @([8C0FF2C4]),r7
  8C0FF2BA  D003  mov.l @([8C0FF2C8]),r0
  8C0FF2BC  6002  mov.l @r0,r0
  8C0FF2BE  402B  jmp @r0
  8C0FF2C0  0009  nop
```

```
; 8C0FF2E0-8C0FF2EA
  8C0FF2E0  D702  mov.l @([8C0FF2EC]),r7
  8C0FF2E2  D003  mov.l @([8C0FF2F0]),r0
  8C0FF2E4  6002  mov.l @r0,r0
  8C0FF2E6  402B  jmp @r0
  8C0FF2E8  0009  nop
```

```
; 8C1014C8-8C1014D2
  8C1014C8  D702  mov.l @([8C1014D4]),r7
  8C1014CA  D003  mov.l @([8C1014D8]),r0
  8C1014CC  6002  mov.l @r0,r0
  8C1014CE  402B  jmp @r0
  8C1014D0  0009  nop
```

```
; 8C1014DC-8C1014E6
  8C1014DC  D702  mov.l @([8C1014E8]),r7
  8C1014DE  D003  mov.l @([8C1014EC]),r0
  8C1014E0  6002  mov.l @r0,r0
  8C1014E2  402B  jmp @r0
  8C1014E4  0009  nop
```


## Tomb Raider Chronicles (USA)

Dump: `/mnt/1TB/dcbat_off/20261007-192238_Tomb_Raider_Chronicles__USA__/jit-13420.txt`

```
; 01DF2BF8-01DF2C02
  01DF2BF8  D702  mov.l @([01DF2C04]),r7
  01DF2BFA  D003  mov.l @([01DF2C08]),r0
  01DF2BFC  6002  mov.l @r0,r0
  01DF2BFE  402B  jmp @r0
  01DF2C00  0009  nop
```

```
; 01DF2D64-01DF2D6E
  01DF2D64  D702  mov.l @([01DF2D70]),r7
  01DF2D66  D003  mov.l @([01DF2D74]),r0
  01DF2D68  6002  mov.l @r0,r0
  01DF2D6A  402B  jmp @r0
  01DF2D6C  0009  nop
```

```
; 01DF2D78-01DF2D82
  01DF2D78  D702  mov.l @([01DF2D84]),r7
  01DF2D7A  D003  mov.l @([01DF2D88]),r0
  01DF2D7C  6002  mov.l @r0,r0
  01DF2D7E  402B  jmp @r0
  01DF2D80  0009  nop
```
