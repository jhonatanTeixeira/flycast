# Descoberta automática de funções repetidas (Dreamcast)

> Gerado por `tools/sdk_find.py` a partir dos dumps do JIT em `/mnt/1TB` (sem jogar de novo). Método e limites no cabeçalho do script e em `docs/native_sdk_code.md`.

Jogos (um dump cada; * = tem amostras do perf, entra no tempo):

- Capcom vs. SNK 2 - Millionaire Fighting 2001 (Japan)
- Dead or Alive 2 (USA) *
- Elemental Gimmick Gear v1.001 (1999)(Vatical)(US)[!] *
- Evolution - The World of Sacred Device (USA)
- Evolution 2 - Far Off Promise (USA)
- Grandia II (USA)
- King of Fighters The - Evolution (USA) (EnJaEsPt)
- Le Mans 24 Hours (Europe) (En,Fr,De,Es,It)
- Macross M3
- Marvel vs. Capcom 2 - New Age of Heroes (Europe) *
- Napple Tale - Arsia in Daydream (Japan) (T-En by Cargodin v1.0) *
- Phantasy Star Online Ver. 2 (USA) (EnJaFrDeEs)
- Power Stone (USA)
- Project Justice (USA)
- Resident Evil - Code - Veronica (USA) (Disc 1)
- Shenmue (USA) (Disc 1) *
- Shenmue II (Europe) (En,Fr,De,Es) (Disc 1) *
- Skies of Arcadia (USA) (Disc 1)
- Sonic Adventure 2 (USA) (EnJaFrDeEs)
- Soulcalibur (USA)
- Tomb Raider Chronicles (USA) *

Grupos com a mesma função (ou muito parecida) em 2+ jogos: **2798**. Os 60 primeiros têm listagem própria em `auto/`. Os grupos que já têm versão nativa vêm primeiro (nome `nativo_*`): o tempo deles roda em C++, fora dos blocos do JIT, e por isso aparece ~0. O tempo só existe para os jogos com `*`; nos outros a coluna conta 0.

| # | Nome | Jogos | Instr. | Variantes | Tempo perf (% emu, soma) | Exemplo |
|---|---|---|---|---|---|---|
| 1 | [001_nativo_doa2](auto/001_nativo_doa2.md) | 5 | 232 | 7 | 18.78 | Marvel vs. Capcom 2 - New Ag `8C12B1E0` |
| 2 | [002_nativo_stripemit](auto/002_nativo_stripemit.md) | 5 | 178 | 2 | 0.00 | Evolution - The World of Sac `8C1C91EC` |
| 3 | [003_nativo_lightxf](auto/003_nativo_lightxf.md) | 5 | 142 | 4 | 0.00 | Resident Evil - Code - Veron `8C1C00A0` |
| 4 | [004_matriz_produto_escalar_divisao](auto/004_matriz_produto_escalar_divisao.md) | 6 | 283 | 10 | 9.77 | Capcom vs. SNK 2 - Millionai `8C17FDF0` |
| 5 | [005_matriz_produto_escalar_divisao](auto/005_matriz_produto_escalar_divisao.md) | 5 | 204 | 5 | 9.53 | Shenmue II (Europe) (En,Fr,D `8C1D9B80` |
| 6 | [006_geral](auto/006_geral.md) | 21 | 433 | 17 | 8.78 | Le Mans 24 Hours (Europe) (E `8C015ED0` |
| 7 | [007_matriz_divisao_pref](auto/007_matriz_divisao_pref.md) | 3 | 93 | 2 | 7.69 | Marvel vs. Capcom 2 - New Ag `8C1304E0` |
| 8 | [008_copia](auto/008_copia.md) | 8 | 107 | 5 | 5.87 | Napple Tale - Arsia in Daydr `8C13503C` |
| 9 | [009_copia](auto/009_copia.md) | 10 | 91 | 1 | 5.78 | Elemental Gimmick Gear v1.00 `8C063CEC` |
| 10 | [010_matriz](auto/010_matriz.md) | 7 | 369 | 1 | 5.44 | Napple Tale - Arsia in Daydr `8C1C1658` |
| 11 | [011_matriz_divisao_pref](auto/011_matriz_divisao_pref.md) | 6 | 446 | 7 | 4.97 | Shenmue II (Europe) (En,Fr,D `8C1D8680` |
| 12 | [012_switch](auto/012_switch.md) | 13 | 512 | 10 | 4.33 | Napple Tale - Arsia in Daydr `8C12C720` |
| 13 | [013_geral](auto/013_geral.md) | 15 | 26 | 1 | 4.03 | Marvel vs. Capcom 2 - New Ag `8C178FBA` |
| 14 | [014_geral](auto/014_geral.md) | 13 | 184 | 4 | 2.99 | Evolution 2 - Far Off Promis `8C19EDA0` |
| 15 | [015_float](auto/015_float.md) | 19 | 71 | 9 | 2.38 | Elemental Gimmick Gear v1.00 `8C066AF8` |
| 16 | [016_pref](auto/016_pref.md) | 5 | 148 | 1 | 2.37 | Elemental Gimmick Gear v1.00 `8C0692B4` |
| 17 | [017_copia](auto/017_copia.md) | 10 | 24 | 1 | 2.24 | Napple Tale - Arsia in Daydr `8C1368A4` |
| 18 | [018_geral](auto/018_geral.md) | 8 | 268 | 2 | 2.16 | Napple Tale - Arsia in Daydr `8C1C193C` |
| 19 | [019_copia](auto/019_copia.md) | 5 | 119 | 1 | 2.15 | Elemental Gimmick Gear v1.00 `8C069590` |
| 20 | [020_copia](auto/020_copia.md) | 5 | 34 | 1 | 2.03 | Marvel vs. Capcom 2 - New Ag `8C1A91AC` |
| 21 | [021_geral](auto/021_geral.md) | 6 | 573 | 3 | 2.00 | Grandia II (USA) `8C0BF0F4` |
| 22 | [022_copia](auto/022_copia.md) | 9 | 78 | 5 | 1.93 | Napple Tale - Arsia in Daydr `8C163170` |
| 23 | [023_geral](auto/023_geral.md) | 8 | 61 | 2 | 1.79 | Napple Tale - Arsia in Daydr `8C162D2C` |
| 24 | [024_copia](auto/024_copia.md) | 20 | 25 | 5 | 1.75 | Marvel vs. Capcom 2 - New Ag `8C17AF00` |
| 25 | [025_geral](auto/025_geral.md) | 10 | 13 | 1 | 1.74 | Elemental Gimmick Gear v1.00 `8C06192E` |
| 26 | [026_matriz](auto/026_matriz.md) | 6 | 143 | 8 | 1.73 | Shenmue (USA) (Disc 1) `0C0942EC` |
| 27 | [027_geral](auto/027_geral.md) | 3 | 58 | 1 | 1.67 | Elemental Gimmick Gear v1.00 `8C06667C` |
| 28 | [028_copia](auto/028_copia.md) | 2 | 37 | 1 | 1.58 | Marvel vs. Capcom 2 - New Ag `8C031CB6` |
| 29 | [029_geral](auto/029_geral.md) | 7 | 31 | 2 | 1.45 | Shenmue II (Europe) (En,Fr,D `8C1DFCF0` |
| 30 | [030_divisao](auto/030_divisao.md) | 14 | 236 | 4 | 1.27 | Evolution - The World of Sac `8C1FDEF8` |
| 31 | [031_laco](auto/031_laco.md) | 11 | 24 | 3 | 1.27 | Elemental Gimmick Gear v1.00 `8C080A36` |
| 32 | [032_geral](auto/032_geral.md) | 8 | 50 | 1 | 1.27 | Elemental Gimmick Gear v1.00 `8C06985C` |
| 33 | [033_matriz_pref_switch](auto/033_matriz_pref_switch.md) | 6 | 204 | 10 | 1.27 | Project Justice (USA) `0C154E20` |
| 34 | [034_matriz_produto_escalar_divisao](auto/034_matriz_produto_escalar_divisao.md) | 5 | 73 | 3 | 1.22 | Dead or Alive 2 (USA) `8C101F00` |
| 35 | [035_divisao](auto/035_divisao.md) | 8 | 38 | 7 | 1.15 | King of Fighters The - Evolu `8C324D9C` |
| 36 | [036_divisao](auto/036_divisao.md) | 6 | 70 | 2 | 1.13 | Marvel vs. Capcom 2 - New Ag `8C11EA10` |
| 37 | [037_contexto](auto/037_contexto.md) | 18 | 193 | 6 | 1.11 | Marvel vs. Capcom 2 - New Ag `8C1969B8` |
| 38 | [038_geral](auto/038_geral.md) | 2 | 14 | 1 | 1.10 | Shenmue (USA) (Disc 1) `0C08CC44` |
| 39 | [039_copia](auto/039_copia.md) | 9 | 47 | 1 | 1.05 | Napple Tale - Arsia in Daydr `8C180DDE` |
| 40 | [040_pref](auto/040_pref.md) | 7 | 117 | 1 | 1.03 | Napple Tale - Arsia in Daydr `8C1C1E28` |
| 41 | [041_cache](auto/041_cache.md) | 4 | 15 | 2 | 1.03 | Marvel vs. Capcom 2 - New Ag `8C02DEA8` |
| 42 | [042_copia](auto/042_copia.md) | 8 | 30 | 1 | 1.02 | Napple Tale - Arsia in Daydr `8C162CF0` |
| 43 | [043_switch](auto/043_switch.md) | 10 | 36 | 1 | 0.98 | Elemental Gimmick Gear v1.00 `8C064264` |
| 44 | [044_pref](auto/044_pref.md) | 7 | 233 | 1 | 0.97 | Napple Tale - Arsia in Daydr `8C1C217C` |
| 45 | [045_geral](auto/045_geral.md) | 7 | 41 | 1 | 0.95 | Napple Tale - Arsia in Daydr `8C1C2440` |
| 46 | [046_copia](auto/046_copia.md) | 21 | 127 | 3 | 0.93 | Evolution - The World of Sac `8C0083F8` |
| 47 | [047_geral](auto/047_geral.md) | 11 | 41 | 1 | 0.90 | Elemental Gimmick Gear v1.00 `8C07D578` |
| 48 | [048_geral](auto/048_geral.md) | 9 | 147 | 11 | 0.89 | Le Mans 24 Hours (Europe) (E `8C05C342` |
| 49 | [049_geral](auto/049_geral.md) | 18 | 12 | 1 | 0.88 | Napple Tale - Arsia in Daydr `8C16024C` |
| 50 | [050_geral](auto/050_geral.md) | 18 | 11 | 1 | 0.87 | Napple Tale - Arsia in Daydr `8C160264` |
| 51 | [051_geral](auto/051_geral.md) | 6 | 114 | 4 | 0.83 | Napple Tale - Arsia in Daydr `8C161EC4` |
| 52 | [052_geral](auto/052_geral.md) | 5 | 78 | 1 | 0.82 | Napple Tale - Arsia in Daydr `8C17F6DC` |
| 53 | [053_divisao_pref](auto/053_divisao_pref.md) | 4 | 404 | 4 | 0.82 | Project Justice (USA) `0C14BAF0` |
| 54 | [054_copia](auto/054_copia.md) | 15 | 95 | 11 | 0.80 | Capcom vs. SNK 2 - Millionai `8C17B3F4` |
| 55 | [055_geral](auto/055_geral.md) | 6 | 153 | 2 | 0.74 | Napple Tale - Arsia in Daydr `8C17C098` |
| 56 | [056_matriz_divisao_pref](auto/056_matriz_divisao_pref.md) | 2 | 133 | 3 | 0.73 | Shenmue II (Europe) (En,Fr,D `8C1D5820` |
| 57 | [057_geral](auto/057_geral.md) | 11 | 38 | 2 | 0.72 | Napple Tale - Arsia in Daydr `8C116A40` |
| 58 | [058_geral](auto/058_geral.md) | 8 | 32 | 2 | 0.72 | Napple Tale - Arsia in Daydr `8C17F834` |
| 59 | [059_geral](auto/059_geral.md) | 6 | 123 | 4 | 0.67 | Napple Tale - Arsia in Daydr `8C161694` |
| 60 | [060_matriz_produto_escalar_divisao](auto/060_matriz_produto_escalar_divisao.md) | 2 | 115 | 1 | 0.63 | Napple Tale - Arsia in Daydr `8C152AEC` |
| 61 | 061_geral | 11 | 41 | 1 | 0.61 | Elemental Gimmick Gear v1.00 `8C07D5D6` |
| 62 | 062_matriz_divisao | 4 | 46 | 1 | 0.60 | Dead or Alive 2 (USA) `8C10A160` |
| 63 | 063_geral | 6 | 81 | 3 | 0.59 | Le Mans 24 Hours (Europe) (E `8C05D6CC` |
| 64 | 064_matriz | 6 | 12 | 1 | 0.57 | Shenmue (USA) (Disc 1) `0C1D1130` |
| 65 | 065_geral | 7 | 218 | 1 | 0.56 | Napple Tale - Arsia in Daydr `8C1C19DC` |
| 66 | 066_geral | 5 | 65 | 1 | 0.56 | Napple Tale - Arsia in Daydr `8C17F784` |
| 67 | 067_geral | 5 | 106 | 2 | 0.55 | Napple Tale - Arsia in Daydr `8C1604C6` |
| 68 | 068_pref | 4 | 28 | 2 | 0.54 | Shenmue II (Europe) (En,Fr,D `8C1DA240` |
| 69 | 069_matriz_produto_escalar_divisao | 3 | 197 | 3 | 0.54 | Shenmue II (Europe) (En,Fr,D `8C1DA5E0` |
| 70 | 070_matriz | 2 | 38 | 5 | 0.53 | Shenmue (USA) (Disc 1) `0C09405C` |
| 71 | 071_geral | 8 | 111 | 2 | 0.52 | Napple Tale - Arsia in Daydr `8C161388` |
| 72 | 072_copia | 8 | 90 | 2 | 0.51 | Napple Tale - Arsia in Daydr `8C17BF34` |
| 73 | 073_matriz | 2 | 26 | 2 | 0.51 | Shenmue (USA) (Disc 1) `0C1D1DC0` |
| 74 | 074_geral | 18 | 127 | 2 | 0.50 | Elemental Gimmick Gear v1.00 `8C09241C` |
| 75 | 075_geral | 16 | 95 | 5 | 0.50 | Napple Tale - Arsia in Daydr `8C1395FE` |
| 76 | 076_matriz | 7 | 30 | 1 | 0.50 | Marvel vs. Capcom 2 - New Ag `8C120C70` |
| 77 | 077_geral | 7 | 197 | 1 | 0.49 | Napple Tale - Arsia in Daydr `8C1C11B8` |
| 78 | 078_geral | 15 | 82 | 5 | 0.48 | Napple Tale - Arsia in Daydr `8C1397A4` |
| 79 | 079_geral | 6 | 58 | 2 | 0.48 | Napple Tale - Arsia in Daydr `8C161C9C` |
| 80 | 080_divisao | 2 | 255 | 1 | 0.48 | Marvel vs. Capcom 2 - New Ag `8C1220E0` |
| 81 | 081_geral | 5 | 59 | 1 | 0.47 | Napple Tale - Arsia in Daydr `8C17BFF6` |
| 82 | 082_copia | 5 | 52 | 2 | 0.46 | Napple Tale - Arsia in Daydr `8C16292C` |
| 83 | 083_copia | 4 | 292 | 4 | 0.46 | Phantasy Star Online Ver. 2  `8C368460` |
| 84 | 084_matriz | 5 | 12 | 1 | 0.46 | Shenmue (USA) (Disc 1) `0C1D10D0` |
| 85 | 085_matriz_pref | 2 | 57 | 2 | 0.46 | Shenmue (USA) (Disc 1) `0C1D3980` |
| 86 | 086_float | 15 | 49 | 6 | 0.45 | Tomb Raider Chronicles (USA) `8C012824` |
| 87 | 087_copia | 13 | 210 | 10 | 0.45 | Skies of Arcadia (USA) (Disc `8C325D90` |
| 88 | 088_matriz | 6 | 26 | 2 | 0.44 | Dead or Alive 2 (USA) `8C0FDB50` |
| 89 | 089_geral | 5 | 58 | 2 | 0.42 | Napple Tale - Arsia in Daydr `8C1605F6` |
| 90 | 090_geral | 8 | 56 | 3 | 0.42 | Napple Tale - Arsia in Daydr `8C1A86DC` |
| 91 | 091_copia | 8 | 444 | 6 | 0.41 | Napple Tale - Arsia in Daydr `8C1C8914` |
| 92 | 092_laco | 2 | 44 | 1 | 0.40 | Shenmue (USA) (Disc 1) `0C08CA10` |
| 93 | 093_geral | 5 | 24 | 3 | 0.39 | Marvel vs. Capcom 2 - New Ag `8C1243C0` |
| 94 | 094_float | 4 | 71 | 2 | 0.39 | Dead or Alive 2 (USA) `8C100A30` |
| 95 | 095_geral | 19 | 106 | 3 | 0.38 | Elemental Gimmick Gear v1.00 `8C092664` |
| 96 | 096_geral | 8 | 43 | 2 | 0.38 | Napple Tale - Arsia in Daydr `8C1630D4` |
| 97 | 097_raiz_divisao | 5 | 91 | 4 | 0.38 | Dead or Alive 2 (USA) `8C0FAA50` |
| 98 | 098_geral | 8 | 33 | 1 | 0.37 | Napple Tale - Arsia in Daydr `8C17F8C4` |
| 99 | 099_copia | 5 | 44 | 2 | 0.37 | Napple Tale - Arsia in Daydr `8C162994` |
| 100 | 100_copia | 2 | 17 | 1 | 0.37 | Marvel vs. Capcom 2 - New Ag `8C04557C` |
| 101 | 101_copia | 18 | 230 | 4 | 0.36 | Elemental Gimmick Gear v1.00 `8C091E18` |
| 102 | 102_geral | 8 | 85 | 2 | 0.36 | Napple Tale - Arsia in Daydr `8C15F424` |
| 103 | 103_pref | 4 | 122 | 7 | 0.36 | Marvel vs. Capcom 2 - New Ag `8C1302A0` |
| 104 | 104_copia | 17 | 71 | 1 | 0.35 | Marvel vs. Capcom 2 - New Ag `8C1AB61C` |
| 105 | 105_geral | 9 | 33 | 3 | 0.35 | Napple Tale - Arsia in Daydr `8C16480C` |
| 106 | 106_matriz | 6 | 26 | 2 | 0.35 | Marvel vs. Capcom 2 - New Ag `8C120CD0` |
| 107 | 107_geral | 5 | 32 | 3 | 0.35 | Napple Tale - Arsia in Daydr `8C134FAC` |
| 108 | 108_geral | 18 | 35 | 1 | 0.34 | Elemental Gimmick Gear v1.00 `8C092562` |
| 109 | 109_geral | 9 | 64 | 4 | 0.34 | Napple Tale - Arsia in Daydr `8C15F39C` |
| 110 | 110_copia | 19 | 200 | 11 | 0.33 | Dead or Alive 2 (USA) `8C12EAE0` |
| 111 | 111_copia | 8 | 40 | 1 | 0.33 | Napple Tale - Arsia in Daydr `8C161338` |
| 112 | 112_geral | 6 | 49 | 1 | 0.33 | Napple Tale - Arsia in Daydr `8C15EE00` |
| 113 | 113_geral | 6 | 29 | 3 | 0.33 | Napple Tale - Arsia in Daydr `8C15FBF2` |
| 114 | 114_copia | 5 | 86 | 1 | 0.33 | Marvel vs. Capcom 2 - New Ag `8C18EC60` |
| 115 | 115_copia | 17 | 26 | 4 | 0.32 | Elemental Gimmick Gear v1.00 `8C030494` |
| 116 | 116_geral | 8 | 58 | 3 | 0.32 | Napple Tale - Arsia in Daydr `8C1611C8` |
| 117 | 117_geral | 8 | 8 | 1 | 0.31 | Napple Tale - Arsia in Daydr `8C17FDB2` |
| 118 | 118_geral | 5 | 512 | 2 | 0.31 | Marvel vs. Capcom 2 - New Ag `8C17C7A0` |
| 119 | 119_geral | 8 | 40 | 1 | 0.30 | Napple Tale - Arsia in Daydr `8C17F874` |
| 120 | 120_matriz_divisao_pref | 3 | 445 | 3 | 0.30 | Capcom vs. SNK 2 - Millionai `8C17F9F0` |
| 121 | 121_geral | 18 | 20 | 1 | 0.30 | Marvel vs. Capcom 2 - New Ag `8C129C90` |
| 122 | 122_float | 6 | 22 | 2 | 0.30 | Shenmue (USA) (Disc 1) `0C1D28B0` |
| 123 | 123_geral | 6 | 37 | 1 | 0.29 | Napple Tale - Arsia in Daydr `8C15ED44` |
| 124 | 124_divisao | 8 | 895 | 3 | 0.29 | Le Mans 24 Hours (Europe) (E `8C089994` |
| 125 | 125_copia | 4 | 94 | 1 | 0.29 | Elemental Gimmick Gear v1.00 `8C06B3C0` |
| 126 | 126_copia | 13 | 19 | 1 | 0.28 | Elemental Gimmick Gear v1.00 `8C097B0C` |
| 127 | 127_geral | 8 | 22 | 1 | 0.28 | Napple Tale - Arsia in Daydr `8C15F586` |
| 128 | 128_copia | 7 | 82 | 1 | 0.28 | Napple Tale - Arsia in Daydr `8C1C2494` |
| 129 | 129_divisao_pref | 3 | 142 | 1 | 0.28 | Marvel vs. Capcom 2 - New Ag `8C128290` |
| 130 | 130_copia | 2 | 33 | 1 | 0.28 | Marvel vs. Capcom 2 - New Ag `8C127150` |
| 131 | 131_copia | 19 | 33 | 1 | 0.28 | Elemental Gimmick Gear v1.00 `8C0761DA` |
| 132 | 132_geral | 18 | 461 | 3 | 0.27 | Dead or Alive 2 (USA) `8C0DF8FC` |
| 133 | 133_geral | 5 | 43 | 1 | 0.27 | Napple Tale - Arsia in Daydr `8C160700` |
| 134 | 134_matriz | 2 | 83 | 1 | 0.27 | Shenmue (USA) (Disc 1) `0C091868` |
| 135 | 135_copia | 17 | 130 | 7 | 0.26 | Phantasy Star Online Ver. 2  `8C0192E8` |
| 136 | 136_copia | 8 | 47 | 1 | 0.26 | Napple Tale - Arsia in Daydr `8C1603C8` |
| 137 | 137_matriz_produto_escalar | 6 | 146 | 4 | 0.26 | Dead or Alive 2 (USA) `8C111F80` |
| 138 | 138_geral | 6 | 59 | 5 | 0.26 | Napple Tale - Arsia in Daydr `8C161D60` |
| 139 | 139_divisao | 4 | 168 | 2 | 0.26 | Shenmue (USA) (Disc 1) `0C1D1560` |
| 140 | 140_pref | 4 | 27 | 1 | 0.26 | Dead or Alive 2 (USA) `8C10A220` |
| 141 | 141_geral | 20 | 55 | 1 | 0.25 | Elemental Gimmick Gear v1.00 `8C031EA4` |
| 142 | 142_geral | 9 | 29 | 1 | 0.25 | Napple Tale - Arsia in Daydr `8C180E3C` |
| 143 | 143_geral | 8 | 56 | 2 | 0.25 | Napple Tale - Arsia in Daydr `8C136460` |
| 144 | 144_matriz | 7 | 20 | 2 | 0.25 | Shenmue (USA) (Disc 1) `0C1D25C0` |
| 145 | 145_geral | 19 | 10 | 1 | 0.24 | Shenmue (USA) (Disc 1) `0C1D8058` |
| 146 | 146_geral | 11 | 9 | 1 | 0.24 | Elemental Gimmick Gear v1.00 `8C07E174` |
| 147 | 147_geral | 19 | 11 | 1 | 0.24 | Shenmue (USA) (Disc 1) `0C1DE318` |
| 148 | 148_geral | 10 | 18 | 3 | 0.24 | Napple Tale - Arsia in Daydr `8C1368D4` |
| 149 | 149_pref | 9 | 97 | 2 | 0.24 | Napple Tale - Arsia in Daydr `8C1C1D64` |
| 150 | 150_geral | 8 | 103 | 3 | 0.24 | Napple Tale - Arsia in Daydr `8C16067E` |
| 151 | 151_geral | 8 | 35 | 1 | 0.24 | Napple Tale - Arsia in Daydr `8C160A08` |
| 152 | 152_float | 7 | 18 | 2 | 0.24 | Dead or Alive 2 (USA) `8C0FE670` |
| 153 | 153_geral | 5 | 46 | 1 | 0.24 | Napple Tale - Arsia in Daydr `8C15F6B6` |
| 154 | 154_geral | 21 | 1220 | 61 | 0.23 | Skies of Arcadia (USA) (Disc `8C1A66FC` |
| 155 | 155_pref | 14 | 183 | 2 | 0.23 | Grandia II (USA) `8C0B74C0` |
| 156 | 156_matriz | 9 | 232 | 16 | 0.23 | Skies of Arcadia (USA) (Disc `8C27D414` |
| 157 | 157_geral | 6 | 83 | 2 | 0.23 | Napple Tale - Arsia in Daydr `8C162096` |
| 158 | 158_geral | 5 | 45 | 3 | 0.23 | King of Fighters The - Evolu `8C33C862` |
| 159 | 159_contexto | 2 | 146 | 2 | 0.23 | Shenmue (USA) (Disc 1) `8C004C66` |
| 160 | 160_copia | 5 | 34 | 1 | 0.22 | Marvel vs. Capcom 2 - New Ag `8C1A9124` |
| 161 | 161_copia | 19 | 65 | 2 | 0.22 | Elemental Gimmick Gear v1.00 `8C09180A` |
| 162 | 162_geral | 8 | 21 | 1 | 0.22 | Napple Tale - Arsia in Daydr `8C1636A0` |
| 163 | 163_divisao_pref | 5 | 202 | 3 | 0.22 | Shenmue II (Europe) (En,Fr,D `8C1D6AC0` |
| 164 | 164_produto_escalar | 5 | 46 | 2 | 0.22 | Shenmue (USA) (Disc 1) `0C1D1240` |
| 165 | 165_geral | 6 | 10 | 1 | 0.21 | Marvel vs. Capcom 2 - New Ag `8C121810` |
| 166 | 166_geral | 9 | 59 | 3 | 0.21 | Grandia II (USA) `8C06E4EA` |
| 167 | 167_pref | 7 | 304 | 4 | 0.21 | Evolution 2 - Far Off Promis `8C1C0FE0` |
| 168 | 168_geral | 7 | 24 | 2 | 0.21 | Napple Tale - Arsia in Daydr `8C162FD0` |
| 169 | 169_copia | 4 | 108 | 4 | 0.21 | Marvel vs. Capcom 2 - New Ag `8C12A07E` |
| 170 | 170_geral | 21 | 33 | 2 | 0.19 | Le Mans 24 Hours (Europe) (E `8C01087C` |
| 171 | 171_geral | 8 | 30 | 1 | 0.19 | Napple Tale - Arsia in Daydr `8C160426` |
| 172 | 172_copia | 7 | 63 | 2 | 0.19 | Napple Tale - Arsia in Daydr `8C1C0D52` |
| 173 | 173_geral | 6 | 34 | 1 | 0.19 | Napple Tale - Arsia in Daydr `8C15EDA8` |
| 174 | 174_matriz | 6 | 8 | 1 | 0.19 | Marvel vs. Capcom 2 - New Ag `8C1217E0` |
| 175 | 175_geral | 5 | 82 | 2 | 0.19 | Napple Tale - Arsia in Daydr `8C162154` |
| 176 | 176_matriz_pref | 5 | 70 | 4 | 0.19 | Dead or Alive 2 (USA) `8C10A540` |
| 177 | 177_geral | 2 | 63 | 2 | 0.19 | Dead or Alive 2 (USA) `8C100890` |
| 178 | 178_copia | 8 | 196 | 2 | 0.18 | Le Mans 24 Hours (Europe) (E `8C082DF0` |
| 179 | 179_geral | 17 | 12 | 1 | 0.18 | Napple Tale - Arsia in Daydr `8C1394A4` |
| 180 | 180_geral | 16 | 11 | 1 | 0.18 | Napple Tale - Arsia in Daydr `8C1394BC` |
| 181 | 181_geral | 15 | 21 | 1 | 0.18 | Marvel vs. Capcom 2 - New Ag `8C00FA00` |
| 182 | 182_copia | 8 | 61 | 2 | 0.18 | Napple Tale - Arsia in Daydr `8C1C0E9E` |
| 183 | 183_geral | 8 | 26 | 1 | 0.18 | Napple Tale - Arsia in Daydr `8C17BCB0` |
| 184 | 184_geral | 7 | 34 | 2 | 0.18 | Napple Tale - Arsia in Daydr `8C163050` |
| 185 | 185_matriz | 6 | 22 | 2 | 0.18 | Shenmue (USA) (Disc 1) `0C1D2340` |
| 186 | 186_pref | 2 | 513 | 1 | 0.18 | Napple Tale - Arsia in Daydr `8C150278` |
| 187 | 187_copia | 18 | 655 | 3 | 0.17 | Le Mans 24 Hours (Europe) (E `8C047538` |
| 188 | 188_geral | 8 | 61 | 3 | 0.17 | Napple Tale - Arsia in Daydr `8C15F50C` |
| 189 | 189_geral | 8 | 10 | 1 | 0.17 | Napple Tale - Arsia in Daydr `8C17D24C` |
| 190 | 190_matriz_divisao_pref | 2 | 131 | 2 | 0.17 | Dead or Alive 2 (USA) `8C10AA36` |
| 191 | 191_geral | 9 | 20 | 1 | 0.17 | Napple Tale - Arsia in Daydr `8C17CFF6` |
| 192 | 192_geral | 8 | 120 | 2 | 0.17 | Napple Tale - Arsia in Daydr `8C16148A` |
| 193 | 193_geral | 8 | 19 | 2 | 0.17 | Napple Tale - Arsia in Daydr `8C161E26` |
| 194 | 194_produto_escalar_raiz | 6 | 23 | 4 | 0.17 | Dead or Alive 2 (USA) `8C0FEA00` |
| 195 | 195_pref | 13 | 367 | 4 | 0.16 | Grandia II (USA) `8C0B7A00` |
| 196 | 196_geral | 9 | 55 | 3 | 0.16 | Napple Tale - Arsia in Daydr `8C17D378` |
| 197 | 197_geral | 9 | 18 | 2 | 0.16 | Napple Tale - Arsia in Daydr `8C161922` |
| 198 | 198_geral | 9 | 16 | 2 | 0.16 | Napple Tale - Arsia in Daydr `8C161D14` |
| 199 | 199_geral | 9 | 8 | 1 | 0.16 | Napple Tale - Arsia in Daydr `8C180EAE` |
| 200 | 200_geral | 6 | 22 | 2 | 0.16 | Napple Tale - Arsia in Daydr `8C1617CA` |
| 201 | 201_divisao | 5 | 58 | 2 | 0.16 | Marvel vs. Capcom 2 - New Ag `8C11F250` |
| 202 | 202_float | 4 | 51 | 2 | 0.16 | Dead or Alive 2 (USA) `8C100AC0` |
| 203 | 203_geral | 3 | 38 | 1 | 0.16 | Marvel vs. Capcom 2 - New Ag `8C11BF40` |
| 204 | 204_geral | 18 | 85 | 1 | 0.15 | Elemental Gimmick Gear v1.00 `8C092356` |
| 205 | 205_copia | 8 | 107 | 2 | 0.15 | Napple Tale - Arsia in Daydr `8C1C25DC` |
| 206 | 206_geral | 4 | 52 | 1 | 0.15 | Napple Tale - Arsia in Daydr `8C1C1360` |
| 207 | 207_geral | 3 | 16 | 1 | 0.15 | Elemental Gimmick Gear v1.00 `8C030210` |
| 208 | 208_float | 2 | 95 | 1 | 0.15 | Dead or Alive 2 (USA) `8C103C70` |
| 209 | 209_produto_escalar | 2 | 14 | 1 | 0.15 | Dead or Alive 2 (USA) `8C0FEA80` |
| 210 | 210_copia | 19 | 24 | 2 | 0.15 | Marvel vs. Capcom 2 - New Ag `8C1864A6` |
| 211 | 211_copia | 17 | 46 | 2 | 0.15 | Elemental Gimmick Gear v1.00 `8C076092` |
| 212 | 212_copia | 8 | 109 | 2 | 0.15 | Napple Tale - Arsia in Daydr `8C17BCEC` |
| 213 | 213_geral | 8 | 39 | 3 | 0.15 | Le Mans 24 Hours (Europe) (E `8C054BAA` |
| 214 | 214_pref | 7 | 9 | 1 | 0.15 | Napple Tale - Arsia in Daydr `8C1C23B4` |
| 215 | 215_geral | 5 | 22 | 1 | 0.15 | Napple Tale - Arsia in Daydr `8C161DE2` |
| 216 | 216_geral | 20 | 51 | 2 | 0.14 | Elemental Gimmick Gear v1.00 `8C031F18` |
| 217 | 217_copia | 18 | 19 | 1 | 0.14 | Elemental Gimmick Gear v1.00 `8C077D1E` |
| 218 | 218_geral | 14 | 178 | 2 | 0.14 | Elemental Gimmick Gear v1.00 `8C095BC8` |
| 219 | 219_matriz_produto_escalar | 9 | 230 | 3 | 0.14 | Evolution - The World of Sac `8C197EF8` |
| 220 | 220_divisao | 5 | 80 | 2 | 0.14 | Dead or Alive 2 (USA) `8C0FD500` |
| 221 | 221_copia | 5 | 28 | 1 | 0.14 | Marvel vs. Capcom 2 - New Ag `8C1A87E0` |
| 222 | 222_geral | 5 | 23 | 1 | 0.14 | Napple Tale - Arsia in Daydr `8C136396` |
| 223 | 223_divisao | 2 | 171 | 2 | 0.14 | Power Stone (USA) `0C0E12E0` |
| 224 | 224_geral | 18 | 59 | 1 | 0.13 | Elemental Gimmick Gear v1.00 `8C0925C0` |
| 225 | 225_copia | 18 | 28 | 1 | 0.13 | Marvel vs. Capcom 2 - New Ag `8C129D50` |
| 226 | 226_geral | 8 | 31 | 2 | 0.13 | Napple Tale - Arsia in Daydr `8C160A60` |
| 227 | 227_geral | 8 | 11 | 1 | 0.13 | Napple Tale - Arsia in Daydr `8C16022A` |
| 228 | 228_geral | 8 | 8 | 1 | 0.13 | Napple Tale - Arsia in Daydr `8C17FD92` |
| 229 | 229_geral | 5 | 14 | 1 | 0.13 | Napple Tale - Arsia in Daydr `8C15F624` |
| 230 | 230_copia | 5 | 85 | 2 | 0.12 | Marvel vs. Capcom 2 - New Ag `8C1939A0` |
| 231 | 231_copia | 5 | 36 | 1 | 0.12 | Marvel vs. Capcom 2 - New Ag `8C17AE40` |
| 232 | 232_geral | 18 | 23 | 2 | 0.12 | Elemental Gimmick Gear v1.00 `8C061322` |
| 233 | 233_copia | 16 | 446 | 15 | 0.12 | Le Mans 24 Hours (Europe) (E `8C013248` |
| 234 | 234_float | 13 | 49 | 1 | 0.12 | Elemental Gimmick Gear v1.00 `8C099FB4` |
| 235 | 235_geral | 9 | 14 | 1 | 0.12 | Napple Tale - Arsia in Daydr `8C1637FE` |
| 236 | 236_geral | 8 | 14 | 1 | 0.12 | Napple Tale - Arsia in Daydr `8C15FB80` |
| 237 | 237_geral | 7 | 18 | 1 | 0.12 | Napple Tale - Arsia in Daydr `8C163074` |
| 238 | 238_geral | 5 | 24 | 2 | 0.12 | Napple Tale - Arsia in Daydr `8C1612B8` |
| 239 | 239_matriz_produto_escalar_divisao | 2 | 217 | 2 | 0.12 | Skies of Arcadia (USA) (Disc `8C2CA340` |
| 240 | 240_geral | 17 | 30 | 1 | 0.11 | Elemental Gimmick Gear v1.00 `8C0A069A` |
| 241 | 241_geral | 18 | 119 | 4 | 0.11 | Capcom vs. SNK 2 - Millionai `8C1F80CC` |
| 242 | 242_geral | 9 | 8 | 1 | 0.11 | Napple Tale - Arsia in Daydr `8C180E8E` |
| 243 | 243_raiz_divisao | 7 | 93 | 7 | 0.11 | Grandia II (USA) `8C0158DC` |
| 244 | 244_geral | 6 | 14 | 2 | 0.11 | Grandia II (USA) `8C0C2D1C` |
| 245 | 245_copia | 5 | 26 | 1 | 0.11 | Marvel vs. Capcom 2 - New Ag `8C18DB44` |
| 246 | 246_geral | 5 | 15 | 1 | 0.11 | Marvel vs. Capcom 2 - New Ag `8C123EB0` |
| 247 | 247_matriz_raiz_divisao | 3 | 223 | 2 | 0.11 | Napple Tale - Arsia in Daydr `8C119910` |
| 248 | 248_copia | 2 | 53 | 1 | 0.11 | Marvel vs. Capcom 2 - New Ag `8C03577E` |
| 249 | 249_copia | 21 | 18 | 1 | 0.10 | Tomb Raider Chronicles (USA) `8C008330` |
| 250 | 250_geral | 18 | 9 | 1 | 0.10 | Dead or Alive 2 (USA) `8C134F48` |
| 251 | 251_copia | 11 | 20 | 2 | 0.10 | Shenmue (USA) (Disc 1) `0C04273C` |
| 252 | 252_copia | 5 | 288 | 4 | 0.10 | Le Mans 24 Hours (Europe) (E `8C07B180` |
| 253 | 253_copia | 5 | 215 | 4 | 0.10 | Dead or Alive 2 (USA) `8C130A20` |
| 254 | 254_float | 4 | 26 | 2 | 0.10 | Dead or Alive 2 (USA) `8C0FE770` |
| 255 | 255_float | 4 | 22 | 1 | 0.10 | Marvel vs. Capcom 2 - New Ag `8C121720` |
| 256 | 256_copia | 3 | 107 | 1 | 0.10 | Elemental Gimmick Gear v1.00 `8C061356` |
| 257 | 257_geral | 3 | 20 | 1 | 0.10 | Elemental Gimmick Gear v1.00 `8C00FA00` |
| 258 | 258_copia | 2 | 136 | 1 | 0.10 | Dead or Alive 2 (USA) `8C104BF0` |
| 259 | 259_geral | 17 | 95 | 4 | 0.09 | Elemental Gimmick Gear v1.00 `8C076050` |
| 260 | 260_geral | 19 | 23 | 1 | 0.09 | Elemental Gimmick Gear v1.00 `8C05EB98` |
| 261 | 261_geral | 14 | 168 | 4 | 0.09 | Evolution 2 - Far Off Promis `8C1CEF20` |
| 262 | 262_contexto | 9 | 175 | 1 | 0.09 | Napple Tale - Arsia in Daydr `8C16BC18` |
| 263 | 263_geral | 9 | 79 | 4 | 0.09 | Le Mans 24 Hours (Europe) (E `8C05784A` |
| 264 | 264_geral | 9 | 12 | 1 | 0.09 | Napple Tale - Arsia in Daydr `8C1611B0` |
| 265 | 265_geral | 8 | 15 | 1 | 0.09 | Napple Tale - Arsia in Daydr `8C1602B8` |
| 266 | 266_geral | 8 | 10 | 1 | 0.09 | Napple Tale - Arsia in Daydr `8C15FBA4` |
| 267 | 267_geral | 7 | 26 | 1 | 0.09 | Napple Tale - Arsia in Daydr `8C12F72C` |
| 268 | 268_geral | 6 | 16 | 2 | 0.09 | Napple Tale - Arsia in Daydr `8C163EC2` |
| 269 | 269_divisao_pref | 5 | 513 | 2 | 0.09 | Evolution 2 - Far Off Promis `8C1FD0B0` |
| 270 | 270_copia | 5 | 32 | 1 | 0.09 | Marvel vs. Capcom 2 - New Ag `8C19158E` |
| 271 | 271_geral | 5 | 24 | 2 | 0.09 | Evolution 2 - Far Off Promis `8C1D8DB4` |
| 272 | 272_copia | 17 | 25 | 1 | 0.08 | Elemental Gimmick Gear v1.00 `8C0304D8` |
| 273 | 273_copia | 15 | 118 | 4 | 0.08 | Evolution - The World of Sac `8C1B1ADC` |
| 274 | 274_copia | 15 | 102 | 2 | 0.08 | Dead or Alive 2 (USA) `8C1174B6` |
| 275 | 275_float | 7 | 308 | 1 | 0.08 | Napple Tale - Arsia in Daydr `8C1C1F14` |
| 276 | 276_float | 6 | 24 | 2 | 0.08 | Dead or Alive 2 (USA) `8C0FE530` |
| 277 | 277_geral | 5 | 27 | 2 | 0.08 | Evolution 2 - Far Off Promis `8C1D8E58` |
| 278 | 278_geral | 5 | 17 | 1 | 0.08 | Dead or Alive 2 (USA) `8C12D2C0` |
| 279 | 279_geral | 5 | 11 | 1 | 0.08 | Napple Tale - Arsia in Daydr `8C15FB36` |
| 280 | 280_geral | 4 | 51 | 2 | 0.08 | Marvel vs. Capcom 2 - New Ag `8C129F60` |
| 281 | 281_geral | 4 | 22 | 1 | 0.08 | Elemental Gimmick Gear v1.00 `8C06BB28` |
| 282 | 282_copia | 3 | 113 | 2 | 0.08 | Elemental Gimmick Gear v1.00 `8C062284` |
| 283 | 283_copia | 2 | 113 | 2 | 0.08 | Power Stone (USA) `0C06297A` |
| 284 | 284_divisao | 2 | 51 | 2 | 0.08 | Resident Evil - Code - Veron `8C19FC70` |
| 285 | 285_produto_escalar | 2 | 14 | 1 | 0.08 | Shenmue (USA) (Disc 1) `0C0914C0` |
| 286 | 286_float | 2 | 12 | 1 | 0.08 | Shenmue II (Europe) (En,Fr,D `8C04F320` |
| 287 | 287_switch | 18 | 27 | 1 | 0.07 | Elemental Gimmick Gear v1.00 `8C0786FA` |
| 288 | 288_copia | 16 | 23 | 1 | 0.07 | Marvel vs. Capcom 2 - New Ag `8C17FB10` |
| 289 | 289_copia | 11 | 24 | 2 | 0.07 | Shenmue (USA) (Disc 1) `0C039A58` |
| 290 | 290_float | 8 | 38 | 2 | 0.07 | Napple Tale - Arsia in Daydr `8C1C160C` |
| 291 | 291_divisao | 7 | 129 | 2 | 0.07 | Dead or Alive 2 (USA) `8C13F0E0` |
| 292 | 292_float | 7 | 50 | 1 | 0.07 | Elemental Gimmick Gear v1.00 `8C06A664` |
| 293 | 293_geral | 7 | 9 | 1 | 0.07 | Napple Tale - Arsia in Daydr `8C161FCC` |
| 294 | 294_geral | 6 | 13 | 1 | 0.07 | Napple Tale - Arsia in Daydr `8C15ED8E` |
| 295 | 295_geral | 6 | 9 | 1 | 0.07 | Napple Tale - Arsia in Daydr `8C15E81C` |
| 296 | 296_geral | 6 | 9 | 1 | 0.07 | Napple Tale - Arsia in Daydr `8C162DD4` |
| 297 | 297_copia | 5 | 53 | 3 | 0.07 | Capcom vs. SNK 2 - Millionai `8C1F4238` |
| 298 | 298_pref | 2 | 291 | 1 | 0.07 | Napple Tale - Arsia in Daydr `8C152000` |
| 299 | 299_float | 2 | 16 | 2 | 0.07 | Shenmue (USA) (Disc 1) `0C1D2B60` |
| 300 | 300_pref | 8 | 106 | 1 | 0.07 | Elemental Gimmick Gear v1.00 `8C0693F4` |
| 301 | 301_float | 5 | 18 | 1 | 0.07 | Marvel vs. Capcom 2 - New Ag `8C1202A0` |
| 302 | 302_pref | 4 | 27 | 1 | 0.07 | Dead or Alive 2 (USA) `8C102040` |
| 303 | 303_copia | 7 | 40 | 1 | 0.06 | Napple Tale - Arsia in Daydr `8C1C23C8` |
| 304 | 304_geral | 6 | 70 | 3 | 0.06 | King of Fighters The - Evolu `8C363DE8` |
| 305 | 305_geral | 5 | 26 | 1 | 0.06 | Marvel vs. Capcom 2 - New Ag `8C122EB0` |
| 306 | 306_copia | 4 | 49 | 1 | 0.06 | Marvel vs. Capcom 2 - New Ag `8C17D8E0` |
| 307 | 307_geral | 19 | 12 | 1 | 0.06 | Elemental Gimmick Gear v1.00 `8C0929B8` |
| 308 | 308_geral | 18 | 26 | 1 | 0.06 | Elemental Gimmick Gear v1.00 `8C09251A` |
| 309 | 309_copia | 18 | 18 | 1 | 0.06 | Elemental Gimmick Gear v1.00 `8C077CFA` |
| 310 | 310_geral | 17 | 32 | 1 | 0.06 | Elemental Gimmick Gear v1.00 `8C0937C0` |
| 311 | 311_geral | 16 | 97 | 6 | 0.06 | Dead or Alive 2 (USA) `8C120C8C` |
| 312 | 312_copia | 14 | 86 | 2 | 0.06 | Power Stone (USA) `0C10DABE` |
| 313 | 313_copia | 13 | 199 | 11 | 0.06 | Le Mans 24 Hours (Europe) (E `8C013988` |
| 314 | 314_copia | 13 | 60 | 1 | 0.06 | Elemental Gimmick Gear v1.00 `8C0835A0` |
| 315 | 315_geral | 12 | 26 | 1 | 0.06 | Elemental Gimmick Gear v1.00 `8C0899E4` |
| 316 | 316_copia | 9 | 124 | 2 | 0.06 | Elemental Gimmick Gear v1.00 `8C02F2F8` |
| 317 | 317_float | 8 | 56 | 2 | 0.06 | Phantasy Star Online Ver. 2  `8C397D24` |
| 318 | 318_geral | 8 | 29 | 1 | 0.06 | Napple Tale - Arsia in Daydr `8C1C15A6` |
| 319 | 319_matriz_produto_escalar_divisao | 7 | 79 | 1 | 0.06 | Napple Tale - Arsia in Daydr `8C177750` |
| 320 | 320_matriz | 7 | 18 | 2 | 0.06 | Marvel vs. Capcom 2 - New Ag `8C121430` |
| 321 | 321_divisao | 6 | 96 | 4 | 0.06 | Evolution - The World of Sac `8C19F998` |
| 322 | 322_copia | 5 | 67 | 2 | 0.06 | Napple Tale - Arsia in Daydr `8C147F52` |
| 323 | 323_copia | 5 | 54 | 2 | 0.06 | Marvel vs. Capcom 2 - New Ag `8C191E00` |
| 324 | 324_geral | 4 | 118 | 2 | 0.06 | Dead or Alive 2 (USA) `8C11FC44` |
| 325 | 325_divisao_pref | 4 | 84 | 1 | 0.06 | Dead or Alive 2 (USA) `8C10A320` |
| 326 | 326_geral | 4 | 37 | 1 | 0.06 | Marvel vs. Capcom 2 - New Ag `8C120C20` |
| 327 | 327_produto_escalar | 3 | 30 | 2 | 0.06 | Dead or Alive 2 (USA) `8C0FDF80` |
| 328 | 328_float | 2 | 13 | 2 | 0.06 | Dead or Alive 2 (USA) `8C110FA0` |
| 329 | 329_preenche | 21 | 9 | 1 | 0.05 | Tomb Raider Chronicles (USA) `8C00836C` |
| 330 | 330_copia | 18 | 81 | 2 | 0.05 | Le Mans 24 Hours (Europe) (E `8C21F20E` |
| 331 | 331_copia | 18 | 29 | 1 | 0.05 | Elemental Gimmick Gear v1.00 `8C079020` |
| 332 | 332_geral | 18 | 16 | 1 | 0.05 | Elemental Gimmick Gear v1.00 `8C05F530` |
| 333 | 333_geral | 18 | 9 | 1 | 0.05 | Dead or Alive 2 (USA) `8C124634` |
| 334 | 334_divisao | 17 | 1249 | 3 | 0.05 | Le Mans 24 Hours (Europe) (E `8C01D2E2` |
| 335 | 335_copia | 16 | 36 | 2 | 0.05 | Marvel vs. Capcom 2 - New Ag `8C182410` |
| 336 | 336_geral | 15 | 41 | 1 | 0.05 | Elemental Gimmick Gear v1.00 `8C02E77C` |
| 337 | 337_copia | 13 | 123 | 2 | 0.05 | Grandia II (USA) `8C0F5520` |
| 338 | 338_copia | 13 | 67 | 2 | 0.05 | Grandia II (USA) `8C0BB960` |
| 339 | 339_copia | 12 | 69 | 3 | 0.05 | Napple Tale - Arsia in Daydr `8C137ED2` |
| 340 | 340_pref | 9 | 336 | 2 | 0.05 | Le Mans 24 Hours (Europe) (E `8C05B67C` |
| 341 | 341_copia | 8 | 38 | 1 | 0.05 | Shenmue II (Europe) (En,Fr,D `8C041FAC` |
| 342 | 342_geral | 8 | 12 | 1 | 0.05 | Napple Tale - Arsia in Daydr `8C1602EE` |
| 343 | 343_geral | 8 | 8 | 1 | 0.05 | Napple Tale - Arsia in Daydr `8C17FD82` |
| 344 | 344_geral | 7 | 12 | 1 | 0.05 | Napple Tale - Arsia in Daydr `8C1C15F4` |
| 345 | 345_produto_escalar_raiz | 6 | 10 | 1 | 0.05 | Dead or Alive 2 (USA) `8C0FE850` |
| 346 | 346_raiz_divisao | 6 | 8 | 1 | 0.05 | Marvel vs. Capcom 2 - New Ag `8C11EC40` |
| 347 | 347_geral | 5 | 139 | 5 | 0.05 | Marvel vs. Capcom 2 - New Ag `8C1A5220` |
| 348 | 348_geral | 5 | 99 | 2 | 0.05 | Marvel vs. Capcom 2 - New Ag `8C18E2A0` |
| 349 | 349_geral | 5 | 57 | 1 | 0.05 | Marvel vs. Capcom 2 - New Ag `8C1925A6` |
| 350 | 350_geral | 5 | 46 | 1 | 0.05 | Marvel vs. Capcom 2 - New Ag `8C19377A` |
| 351 | 351_laco | 5 | 32 | 2 | 0.05 | Dead or Alive 2 (USA) `8C12FFEE` |
| 352 | 352_laco | 5 | 15 | 1 | 0.05 | Marvel vs. Capcom 2 - New Ag `8C17AF54` |
| 353 | 353_pref | 4 | 101 | 1 | 0.05 | Napple Tale - Arsia in Daydr `8C16F488` |
| 354 | 354_geral | 4 | 12 | 1 | 0.05 | Marvel vs. Capcom 2 - New Ag `8C124440` |
| 355 | 355_copia | 3 | 182 | 2 | 0.05 | Dead or Alive 2 (USA) `8C120A3C` |
| 356 | 356_copia | 3 | 98 | 2 | 0.05 | Elemental Gimmick Gear v1.00 `8C076680` |
| 357 | 357_geral | 3 | 17 | 1 | 0.05 | Marvel vs. Capcom 2 - New Ag `8C123D10` |
| 358 | 358_float | 3 | 11 | 1 | 0.05 | Shenmue II (Europe) (En,Fr,D `8C04D1C8` |
| 359 | 359_copia | 2 | 44 | 1 | 0.05 | Dead or Alive 2 (USA) `8C0FCB10` |
| 360 | 360_cache | 2 | 20 | 2 | 0.05 | Shenmue (USA) (Disc 1) `0C0967B0` |
| 361 | 361_matriz | 2 | 18 | 2 | 0.05 | Shenmue (USA) (Disc 1) `0C1D25F0` |
| 362 | 362_copia | 19 | 12 | 1 | 0.04 | Shenmue (USA) (Disc 1) `0C1D7468` |
| 363 | 363_copia | 17 | 42 | 3 | 0.04 | Marvel vs. Capcom 2 - New Ag `8C174B18` |
| 364 | 364_geral | 17 | 17 | 3 | 0.04 | Elemental Gimmick Gear v1.00 `8C0707B6` |
| 365 | 365_copia | 16 | 278 | 11 | 0.04 | Napple Tale - Arsia in Daydr `8C131E7E` |
| 366 | 366_copia | 14 | 1725 | 8 | 0.04 | Le Mans 24 Hours (Europe) (E `8C04DD6C` |
| 367 | 367_copia | 14 | 118 | 2 | 0.04 | Phantasy Star Online Ver. 2  `8C37F598` |
| 368 | 368_copia | 14 | 83 | 2 | 0.04 | Evolution - The World of Sac `8C1F0B90` |
| 369 | 369_copia | 13 | 21 | 1 | 0.04 | Elemental Gimmick Gear v1.00 `8C07750E` |
| 370 | 370_copia | 13 | 19 | 1 | 0.04 | Elemental Gimmick Gear v1.00 `8C097AE0` |
| 371 | 371_geral | 12 | 60 | 1 | 0.04 | Marvel vs. Capcom 2 - New Ag `8C19558C` |
| 372 | 372_geral | 12 | 31 | 4 | 0.04 | Capcom vs. SNK 2 - Millionai `8C204122` |
| 373 | 373_float | 9 | 2095 | 12 | 0.04 | Le Mans 24 Hours (Europe) (E `8C0C6A24` |
| 374 | 374_geral | 9 | 32 | 1 | 0.04 | Napple Tale - Arsia in Daydr `8C1697A0` |
| 375 | 375_float | 8 | 43 | 1 | 0.04 | Napple Tale - Arsia in Daydr `8C1C1D0C` |
| 376 | 376_float | 7 | 74 | 1 | 0.04 | Elemental Gimmick Gear v1.00 `8C066A40` |
| 377 | 377_geral | 6 | 18 | 2 | 0.04 | Napple Tale - Arsia in Daydr `8C163C50` |
| 378 | 378_divisao | 5 | 241 | 1 | 0.04 | Dead or Alive 2 (USA) `8C0FECA0` |
| 379 | 379_copia | 5 | 228 | 3 | 0.04 | Le Mans 24 Hours (Europe) (E `8C06D9A8` |
| 380 | 380_copia | 5 | 179 | 3 | 0.04 | Marvel vs. Capcom 2 - New Ag `8C17BE80` |
| 381 | 381_pref | 5 | 173 | 4 | 0.04 | Dead or Alive 2 (USA) `8C130760` |
| 382 | 382_geral | 5 | 150 | 1 | 0.04 | Napple Tale - Arsia in Daydr `8C15CC14` |
| 383 | 383_copia | 5 | 110 | 3 | 0.04 | Le Mans 24 Hours (Europe) (E `8C239218` |
| 384 | 384_copia | 5 | 69 | 1 | 0.04 | Marvel vs. Capcom 2 - New Ag `8C19276E` |
| 385 | 385_geral | 5 | 48 | 3 | 0.04 | Le Mans 24 Hours (Europe) (E `8C06D3E2` |
| 386 | 386_geral | 5 | 40 | 1 | 0.04 | Marvel vs. Capcom 2 - New Ag `8C17CEE0` |
| 387 | 387_copia | 5 | 20 | 1 | 0.04 | Napple Tale - Arsia in Daydr `8C132672` |
| 388 | 388_float | 5 | 17 | 2 | 0.04 | Shenmue II (Europe) (En,Fr,D `8C1DA280` |
| 389 | 389_float | 4 | 17 | 2 | 0.04 | Dead or Alive 2 (USA) `8C100D00` |
| 390 | 390_float | 4 | 15 | 1 | 0.04 | Shenmue II (Europe) (En,Fr,D `8C1DBBC0` |
| 391 | 391_float | 4 | 8 | 1 | 0.04 | Dead or Alive 2 (USA) `8C0FD1F0` |
| 392 | 392_produto_escalar_pref | 3 | 59 | 3 | 0.04 | Dead or Alive 2 (USA) `8C112220` |
| 393 | 393_matriz | 3 | 12 | 1 | 0.04 | Elemental Gimmick Gear v1.00 `8C0669E4` |
| 394 | 394_raiz_divisao | 2 | 153 | 2 | 0.04 | Napple Tale - Arsia in Daydr `8C147300` |
| 395 | 395_divisao_pref | 2 | 104 | 1 | 0.04 | Shenmue (USA) (Disc 1) `0C0914F0` |
| 396 | 396_geral | 2 | 40 | 2 | 0.04 | Marvel vs. Capcom 2 - New Ag `8C02E368` |
| 397 | 397_laco | 2 | 27 | 1 | 0.04 | Shenmue (USA) (Disc 1) `0C092F14` |
| 398 | 398_float | 2 | 13 | 1 | 0.04 | Marvel vs. Capcom 2 - New Ag `8C0301A4` |
| 399 | 399_copia | 21 | 247 | 2 | 0.03 | Dead or Alive 2 (USA) `8C008D74` |
| 400 | 400_geral | 19 | 33 | 1 | 0.03 | Dead or Alive 2 (USA) `8C12494A` |
| 401 | 401_geral | 19 | 11 | 1 | 0.03 | Dead or Alive 2 (USA) `8C134D94` |
| 402 | 402_cache | 18 | 33 | 7 | 0.03 | Dead or Alive 2 (USA) `8C117804` |
| 403 | 403_geral | 18 | 32 | 1 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C1780EA` |
| 404 | 404_geral | 18 | 27 | 1 | 0.03 | Elemental Gimmick Gear v1.00 `8C0610DE` |
| 405 | 405_geral | 18 | 17 | 1 | 0.03 | Elemental Gimmick Gear v1.00 `8C02E384` |
| 406 | 406_geral | 18 | 17 | 1 | 0.03 | Elemental Gimmick Gear v1.00 `8C0300B0` |
| 407 | 407_geral | 18 | 15 | 1 | 0.03 | Elemental Gimmick Gear v1.00 `8C092956` |
| 408 | 408_geral | 18 | 13 | 1 | 0.03 | Elemental Gimmick Gear v1.00 `8C0629B0` |
| 409 | 409_geral | 18 | 13 | 1 | 0.03 | Elemental Gimmick Gear v1.00 `8C0760FA` |
| 410 | 410_geral | 17 | 19 | 1 | 0.03 | Shenmue II (Europe) (En,Fr,D `8C047E00` |
| 411 | 411_geral | 16 | 92 | 8 | 0.03 | Grandia II (USA) `8C07A608` |
| 412 | 412_geral | 16 | 44 | 2 | 0.03 | Le Mans 24 Hours (Europe) (E `8C013B92` |
| 413 | 413_geral | 15 | 12 | 1 | 0.03 | Dead or Alive 2 (USA) `8C12107A` |
| 414 | 414_geral | 14 | 156 | 3 | 0.03 | Shenmue (USA) (Disc 1) `0C04C90A` |
| 415 | 415_copia | 14 | 60 | 1 | 0.03 | Elemental Gimmick Gear v1.00 `8C096CA6` |
| 416 | 416_copia | 10 | 98 | 2 | 0.03 | Shenmue II (Europe) (En,Fr,D `8C03F938` |
| 417 | 417_laco | 10 | 40 | 3 | 0.03 | Elemental Gimmick Gear v1.00 `8C080B78` |
| 418 | 418_copia | 8 | 85 | 1 | 0.03 | Elemental Gimmick Gear v1.00 `8C0694C8` |
| 419 | 419_geral | 8 | 65 | 2 | 0.03 | Napple Tale - Arsia in Daydr `8C116E48` |
| 420 | 420_pref | 7 | 49 | 1 | 0.03 | Napple Tale - Arsia in Daydr `8C1C2350` |
| 421 | 421_geral | 7 | 28 | 1 | 0.03 | Napple Tale - Arsia in Daydr `8C1C1128` |
| 422 | 422_laco | 7 | 19 | 1 | 0.03 | Napple Tale - Arsia in Daydr `8C1C2418` |
| 423 | 423_geral | 7 | 10 | 1 | 0.03 | Elemental Gimmick Gear v1.00 `8C0305CC` |
| 424 | 424_switch | 6 | 76 | 6 | 0.03 | Project Justice (USA) `0C155800` |
| 425 | 425_laco | 6 | 33 | 3 | 0.03 | Dead or Alive 2 (USA) `8C101E2A` |
| 426 | 426_float | 6 | 16 | 1 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C120980` |
| 427 | 427_divisao | 5 | 395 | 5 | 0.03 | Le Mans 24 Hours (Europe) (E `8C06E6A0` |
| 428 | 428_geral | 5 | 230 | 6 | 0.03 | Dead or Alive 2 (USA) `8C132260` |
| 429 | 429_pref | 5 | 132 | 1 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C17BCE0` |
| 430 | 430_copia | 5 | 118 | 3 | 0.03 | Le Mans 24 Hours (Europe) (E `8C078396` |
| 431 | 431_geral | 5 | 58 | 1 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C18FBE0` |
| 432 | 432_geral | 5 | 56 | 2 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C1A8320` |
| 433 | 433_matriz_produto_escalar | 5 | 49 | 2 | 0.03 | Evolution - The World of Sac `8C198764` |
| 434 | 434_geral | 5 | 47 | 1 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C17BC40` |
| 435 | 435_geral | 5 | 47 | 1 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C17C0A0` |
| 436 | 436_float | 5 | 33 | 2 | 0.03 | Dead or Alive 2 (USA) `8C103A50` |
| 437 | 437_geral | 5 | 17 | 1 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C129F34` |
| 438 | 438_copia | 4 | 78 | 1 | 0.03 | Elemental Gimmick Gear v1.00 `8C05CC58` |
| 439 | 439_geral | 3 | 79 | 2 | 0.03 | Evolution - The World of Sac `8C15FDE8` |
| 440 | 440_geral | 3 | 43 | 1 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C11C7F0` |
| 441 | 441_preenche | 3 | 25 | 1 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C177C1C` |
| 442 | 442_copia | 3 | 20 | 1 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C11F980` |
| 443 | 443_float | 3 | 16 | 2 | 0.03 | Project Justice (USA) `0C148700` |
| 444 | 444_geral | 3 | 13 | 1 | 0.03 | Dead or Alive 2 (USA) `8C0F9E00` |
| 445 | 445_produto_escalar | 3 | 12 | 1 | 0.03 | Shenmue (USA) (Disc 1) `0C1D29F0` |
| 446 | 446_divisao_pref | 2 | 517 | 1 | 0.03 | Napple Tale - Arsia in Daydr `8C178D78` |
| 447 | 447_copia | 2 | 133 | 2 | 0.03 | Phantasy Star Online Ver. 2  `8C359DC8` |
| 448 | 448_matriz_divisao_pref | 2 | 121 | 1 | 0.03 | Shenmue II (Europe) (En,Fr,D `8C1D5940` |
| 449 | 449_copia | 2 | 115 | 2 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C125C20` |
| 450 | 450_matriz_produto_escalar_divisao | 2 | 103 | 1 | 0.03 | Shenmue II (Europe) (En,Fr,D `8C04D4B6` |
| 451 | 451_raiz | 2 | 81 | 2 | 0.03 | Shenmue (USA) (Disc 1) `0C091910` |
| 452 | 452_geral | 2 | 33 | 1 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C1262A0` |
| 453 | 453_float | 2 | 22 | 2 | 0.03 | Project Justice (USA) `0C147480` |
| 454 | 454_geral | 2 | 19 | 1 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C123CE0` |
| 455 | 455_geral | 2 | 16 | 1 | 0.03 | Elemental Gimmick Gear v1.00 `8C01A24C` |
| 456 | 456_produto_escalar_raiz | 2 | 15 | 1 | 0.03 | Shenmue (USA) (Disc 1) `0C091478` |
| 457 | 457_float | 2 | 13 | 1 | 0.03 | Dead or Alive 2 (USA) `8C109A80` |
| 458 | 458_copia | 2 | 12 | 1 | 0.03 | Shenmue (USA) (Disc 1) `0C1CCEC0` |
| 459 | 459_float | 2 | 11 | 1 | 0.03 | Marvel vs. Capcom 2 - New Ag `8C121E40` |
| 460 | 460_geral | 2 | 10 | 1 | 0.03 | Shenmue II (Europe) (En,Fr,D `8C00FA00` |
| 461 | 461_produto_escalar_raiz | 2 | 8 | 1 | 0.03 | Shenmue II (Europe) (En,Fr,D `8C04CDE8` |
| 462 | 462_geral | 21 | 29 | 1 | 0.02 | Shenmue II (Europe) (En,Fr,D `8C008AD0` |
| 463 | 463_geral | 19 | 10 | 1 | 0.02 | Elemental Gimmick Gear v1.00 `8C092D9C` |
| 464 | 464_copia | 18 | 215 | 2 | 0.02 | Le Mans 24 Hours (Europe) (E `8C0436AE` |
| 465 | 465_trava | 18 | 30 | 4 | 0.02 | Evolution - The World of Sac `8C1DAEF6` |
| 466 | 466_geral | 18 | 16 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C1867BE` |
| 467 | 467_laco | 18 | 8 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C129EF4` |
| 468 | 468_geral | 18 | 8 | 1 | 0.02 | Elemental Gimmick Gear v1.00 `8C078A90` |
| 469 | 469_geral | 16 | 20 | 1 | 0.02 | Shenmue (USA) (Disc 1) `0C041584` |
| 470 | 470_geral | 16 | 9 | 1 | 0.02 | Elemental Gimmick Gear v1.00 `8C060634` |
| 471 | 471_geral | 14 | 153 | 2 | 0.02 | Grandia II (USA) `8C0F8360` |
| 472 | 472_geral | 13 | 143 | 5 | 0.02 | Evolution 2 - Far Off Promis `8C1D37C4` |
| 473 | 473_geral | 12 | 26 | 1 | 0.02 | Napple Tale - Arsia in Daydr `8C137E7C` |
| 474 | 474_geral | 12 | 13 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C1AB28C` |
| 475 | 475_geral | 10 | 15 | 1 | 0.02 | Elemental Gimmick Gear v1.00 `8C063186` |
| 476 | 476_float | 9 | 83 | 4 | 0.02 | Evolution - The World of Sac `8C198610` |
| 477 | 477_copia | 9 | 74 | 2 | 0.02 | Napple Tale - Arsia in Daydr `8C17D3F2` |
| 478 | 478_copia | 9 | 37 | 1 | 0.02 | Napple Tale - Arsia in Daydr `8C163C76` |
| 479 | 479_geral | 9 | 8 | 1 | 0.02 | Napple Tale - Arsia in Daydr `8C1C87AC` |
| 480 | 480_geral | 8 | 52 | 2 | 0.02 | Napple Tale - Arsia in Daydr `8C12F860` |
| 481 | 481_pref | 8 | 49 | 1 | 0.02 | Shenmue (USA) (Disc 1) `0C03CCA4` |
| 482 | 482_pref | 8 | 48 | 1 | 0.02 | Shenmue (USA) (Disc 1) `0C03CC40` |
| 483 | 483_geral | 8 | 38 | 1 | 0.02 | Napple Tale - Arsia in Daydr `8C1C1160` |
| 484 | 484_copia | 8 | 33 | 2 | 0.02 | Napple Tale - Arsia in Daydr `8C1C1564` |
| 485 | 485_geral | 8 | 28 | 1 | 0.02 | Napple Tale - Arsia in Daydr `8C12FF84` |
| 486 | 486_geral | 7 | 35 | 1 | 0.02 | Napple Tale - Arsia in Daydr `8C1C10AC` |
| 487 | 487_float | 7 | 30 | 1 | 0.02 | Napple Tale - Arsia in Daydr `8C143ED4` |
| 488 | 488_float | 7 | 27 | 1 | 0.02 | Napple Tale - Arsia in Daydr `8C1198C4` |
| 489 | 489_matriz | 7 | 10 | 1 | 0.02 | Napple Tale - Arsia in Daydr `8C119ADC` |
| 490 | 490_geral | 6 | 108 | 1 | 0.02 | Napple Tale - Arsia in Daydr `8C15D96C` |
| 491 | 491_geral | 6 | 22 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C1838F8` |
| 492 | 492_laco | 6 | 17 | 3 | 0.02 | Dead or Alive 2 (USA) `8C101E16` |
| 493 | 493_contador | 6 | 16 | 1 | 0.02 | Shenmue II (Europe) (En,Fr,D `8C1D68CA` |
| 494 | 494_laco | 6 | 15 | 1 | 0.02 | Shenmue II (Europe) (En,Fr,D `8C1D8F7A` |
| 495 | 495_pref | 5 | 419 | 4 | 0.02 | Skies of Arcadia (USA) (Disc `8C290630` |
| 496 | 496_pref | 5 | 76 | 3 | 0.02 | Napple Tale - Arsia in Daydr `8C10B508` |
| 497 | 497_divisao | 5 | 73 | 1 | 0.02 | Dead or Alive 2 (USA) `8C0F9FA0` |
| 498 | 498_geral | 5 | 72 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C192676` |
| 499 | 499_float | 5 | 69 | 3 | 0.02 | Le Mans 24 Hours (Europe) (E `8C06991C` |
| 500 | 500_geral | 5 | 65 | 2 | 0.02 | Dead or Alive 2 (USA) `8C1206C0` |
| 501 | 501_copia | 5 | 64 | 2 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C190900` |
| 502 | 502_copia | 5 | 61 | 5 | 0.02 | Skies of Arcadia (USA) (Disc `8C28BA3C` |
| 503 | 503_geral | 5 | 56 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C192B40` |
| 504 | 504_geral | 5 | 51 | 2 | 0.02 | Napple Tale - Arsia in Daydr `8C1246B0` |
| 505 | 505_geral | 5 | 39 | 1 | 0.02 | Dead or Alive 2 (USA) `8C133E06` |
| 506 | 506_geral | 5 | 35 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C192A6C` |
| 507 | 507_geral | 5 | 34 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C1A7AA0` |
| 508 | 508_geral | 5 | 32 | 1 | 0.02 | Napple Tale - Arsia in Daydr `8C148336` |
| 509 | 509_geral | 5 | 29 | 1 | 0.02 | Dead or Alive 2 (USA) `8C0FBF80` |
| 510 | 510_geral | 5 | 29 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C190440` |
| 511 | 511_geral | 5 | 28 | 2 | 0.02 | Dead or Alive 2 (USA) `8C13CC66` |
| 512 | 512_copia | 5 | 24 | 1 | 0.02 | Napple Tale - Arsia in Daydr `8C10F818` |
| 513 | 513_geral | 5 | 22 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C19661C` |
| 514 | 514_geral | 5 | 8 | 1 | 0.02 | Dead or Alive 2 (USA) `8C1313A0` |
| 515 | 515_copia | 4 | 66 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C11BD70` |
| 516 | 516_geral | 4 | 52 | 1 | 0.02 | Elemental Gimmick Gear v1.00 `8C0892C4` |
| 517 | 517_geral | 4 | 38 | 1 | 0.02 | Dead or Alive 2 (USA) `8C12E8C0` |
| 518 | 518_pref | 4 | 37 | 1 | 0.02 | Dead or Alive 2 (USA) `8C10A2C0` |
| 519 | 519_produto_escalar_raiz | 4 | 16 | 1 | 0.02 | Dead or Alive 2 (USA) `8C0FE750` |
| 520 | 520_geral | 4 | 15 | 2 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C122F30` |
| 521 | 521_matriz_divisao | 3 | 67 | 1 | 0.02 | Napple Tale - Arsia in Daydr `8C152C2C` |
| 522 | 522_geral | 3 | 49 | 1 | 0.02 | Elemental Gimmick Gear v1.00 `8C06B634` |
| 523 | 523_copia | 3 | 43 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C177CA8` |
| 524 | 524_geral | 3 | 22 | 1 | 0.02 | Elemental Gimmick Gear v1.00 `8C05CE20` |
| 525 | 525_geral | 3 | 15 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C192540` |
| 526 | 526_geral | 3 | 10 | 1 | 0.02 | Dead or Alive 2 (USA) `8C1065E0` |
| 527 | 527_contexto | 2 | 99 | 2 | 0.02 | Shenmue (USA) (Disc 1) `AC004D3C` |
| 528 | 528_divisao | 2 | 65 | 2 | 0.02 | Shenmue II (Europe) (En,Fr,D `8C050958` |
| 529 | 529_divisao | 2 | 59 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C122C90` |
| 530 | 530_geral | 2 | 24 | 1 | 0.02 | Shenmue II (Europe) (En,Fr,D `8C1DF34C` |
| 531 | 531_geral | 2 | 15 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C10891A` |
| 532 | 532_raiz | 2 | 14 | 1 | 0.02 | Shenmue (USA) (Disc 1) `0C09144C` |
| 533 | 533_geral | 2 | 13 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C039C30` |
| 534 | 534_float | 2 | 12 | 1 | 0.02 | Shenmue (USA) (Disc 1) `0C094570` |
| 535 | 535_float | 2 | 11 | 1 | 0.02 | Marvel vs. Capcom 2 - New Ag `8C1088C6` |
| 536 | 536_divisao | 2 | 8 | 1 | 0.02 | Shenmue (USA) (Disc 1) `0C091438` |
| 537 | 537_copia | 21 | 125 | 2 | 0.01 | Tomb Raider Chronicles (USA) `8C00898C` |
| 538 | 538_geral | 21 | 74 | 1 | 0.01 | Tomb Raider Chronicles (USA) `8C009980` |
| 539 | 539_copia | 21 | 44 | 1 | 0.01 | Shenmue II (Europe) (En,Fr,D `8C00908C` |
| 540 | 540_geral | 19 | 106 | 7 | 0.01 | Dead or Alive 2 (USA) `8C1168D0` |
| 541 | 541_copia | 18 | 34 | 1 | 0.01 | Dead or Alive 2 (USA) `8C13536C` |
| 542 | 542_geral | 17 | 39 | 13 | 0.01 | Shenmue (USA) (Disc 1) `0C03B660` |
| 543 | 543_geral | 16 | 38 | 5 | 0.01 | Dead or Alive 2 (USA) `8C11FDF4` |
| 544 | 544_copia | 14 | 261 | 4 | 0.01 | Evolution 2 - Far Off Promis `8C1D1320` |
| 545 | 545_copia | 14 | 52 | 1 | 0.01 | Shenmue (USA) (Disc 1) `0C03F610` |
| 546 | 546_geral | 13 | 92 | 3 | 0.01 | Dead or Alive 2 (USA) `8C11EC34` |
| 547 | 547_copia | 13 | 82 | 3 | 0.01 | Dead or Alive 2 (USA) `8C11EB90` |
| 548 | 548_geral | 13 | 70 | 3 | 0.01 | Dead or Alive 2 (USA) `8C13E784` |
| 549 | 549_geral | 13 | 15 | 1 | 0.01 | Dead or Alive 2 (USA) `8C0FB090` |
| 550 | 550_geral | 13 | 12 | 1 | 0.01 | Shenmue (USA) (Disc 1) `0C04D760` |
| 551 | 551_geral | 9 | 72 | 2 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C1769EC` |
| 552 | 552_contexto | 9 | 22 | 1 | 0.01 | Napple Tale - Arsia in Daydr `8C16BB88` |
| 553 | 553_geral | 8 | 217 | 8 | 0.01 | Evolution - The World of Sac `8C1CAC54` |
| 554 | 554_copia | 8 | 57 | 3 | 0.01 | Napple Tale - Arsia in Daydr `8C1C0CE0` |
| 555 | 555_geral | 8 | 17 | 1 | 0.01 | Napple Tale - Arsia in Daydr `8C12F998` |
| 556 | 556_pref | 7 | 499 | 13 | 0.01 | Skies of Arcadia (USA) (Disc `8C2924EC` |
| 557 | 557_geral | 7 | 75 | 3 | 0.01 | Napple Tale - Arsia in Daydr `8C179408` |
| 558 | 558_copia | 7 | 74 | 2 | 0.01 | Napple Tale - Arsia in Daydr `8C17D538` |
| 559 | 559_geral | 7 | 64 | 3 | 0.01 | Napple Tale - Arsia in Daydr `8C169548` |
| 560 | 560_pref | 7 | 39 | 1 | 0.01 | Napple Tale - Arsia in Daydr `8C115F8C` |
| 561 | 561_geral | 7 | 37 | 2 | 0.01 | Napple Tale - Arsia in Daydr `8C1695D4` |
| 562 | 562_pref | 7 | 35 | 1 | 0.01 | Napple Tale - Arsia in Daydr `8C116038` |
| 563 | 563_geral | 7 | 27 | 1 | 0.01 | Napple Tale - Arsia in Daydr `8C1C10F2` |
| 564 | 564_matriz | 7 | 20 | 2 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C121280` |
| 565 | 565_produto_escalar | 7 | 13 | 1 | 0.01 | Napple Tale - Arsia in Daydr `8C119C08` |
| 566 | 566_copia | 6 | 186 | 3 | 0.01 | Capcom vs. SNK 2 - Millionai `8C203720` |
| 567 | 567_geral | 6 | 155 | 5 | 0.01 | Evolution 2 - Far Off Promis `8C1A570E` |
| 568 | 568_geral | 6 | 121 | 7 | 0.01 | Skies of Arcadia (USA) (Disc `8C290AAE` |
| 569 | 569_geral | 6 | 90 | 1 | 0.01 | Napple Tale - Arsia in Daydr `8C1813D0` |
| 570 | 570_float | 6 | 13 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C1217F0` |
| 571 | 571_contador | 6 | 12 | 1 | 0.01 | Dead or Alive 2 (USA) `8C101E1E` |
| 572 | 572_geral | 6 | 8 | 1 | 0.01 | Elemental Gimmick Gear v1.00 `8C0605DA` |
| 573 | 573_geral | 5 | 157 | 5 | 0.01 | Shenmue II (Europe) (En,Fr,D `8C03ECA4` |
| 574 | 574_copia | 5 | 129 | 1 | 0.01 | Dead or Alive 2 (USA) `8C12AE3C` |
| 575 | 575_geral | 5 | 102 | 1 | 0.01 | Napple Tale - Arsia in Daydr `8C1C0F50` |
| 576 | 576_geral | 5 | 82 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C1A84E0` |
| 577 | 577_geral | 5 | 79 | 2 | 0.01 | Resident Evil - Code - Veron `8C17CD1C` |
| 578 | 578_geral | 5 | 74 | 5 | 0.01 | Dead or Alive 2 (USA) `8C132920` |
| 579 | 579_copia | 5 | 69 | 2 | 0.01 | Napple Tale - Arsia in Daydr `8C148072` |
| 580 | 580_geral | 5 | 62 | 1 | 0.01 | Napple Tale - Arsia in Daydr `8C179C30` |
| 581 | 581_geral | 5 | 61 | 1 | 0.01 | Napple Tale - Arsia in Daydr `8C17F636` |
| 582 | 582_copia | 5 | 49 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C11C0B0` |
| 583 | 583_geral | 5 | 48 | 1 | 0.01 | Napple Tale - Arsia in Daydr `8C13554C` |
| 584 | 584_geral | 5 | 40 | 2 | 0.01 | Phantasy Star Online Ver. 2  `8C356508` |
| 585 | 585_geral | 5 | 34 | 1 | 0.01 | Shenmue II (Europe) (En,Fr,D `8C03D788` |
| 586 | 586_geral | 5 | 32 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C1A7800` |
| 587 | 587_geral | 5 | 28 | 1 | 0.01 | Dead or Alive 2 (USA) `8C12D760` |
| 588 | 588_geral | 5 | 26 | 1 | 0.01 | Dead or Alive 2 (USA) `8C13C880` |
| 589 | 589_geral | 5 | 26 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C1A877E` |
| 590 | 590_float | 5 | 22 | 1 | 0.01 | Napple Tale - Arsia in Daydr `8C119884` |
| 591 | 591_geral | 5 | 20 | 1 | 0.01 | Dead or Alive 2 (USA) `8C13EFE0` |
| 592 | 592_geral | 5 | 19 | 2 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C17B260` |
| 593 | 593_geral | 5 | 19 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C1A7858` |
| 594 | 594_geral | 5 | 17 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C19237E` |
| 595 | 595_geral | 5 | 17 | 1 | 0.01 | Shenmue II (Europe) (En,Fr,D `8C03ECDA` |
| 596 | 596_laco | 5 | 14 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C1921C0` |
| 597 | 597_geral | 5 | 11 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C192320` |
| 598 | 598_geral | 5 | 9 | 1 | 0.01 | Elemental Gimmick Gear v1.00 `8C063CD6` |
| 599 | 599_geral | 5 | 8 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C17ADC0` |
| 600 | 600_geral | 4 | 180 | 2 | 0.01 | Capcom vs. SNK 2 - Millionai `8C172B40` |
| 601 | 601_copia | 4 | 140 | 2 | 0.01 | Phantasy Star Online Ver. 2  `8C37B2C4` |
| 602 | 602_pref | 4 | 136 | 2 | 0.01 | Napple Tale - Arsia in Daydr `8C12F3CA` |
| 603 | 603_copia | 4 | 60 | 1 | 0.01 | Dead or Alive 2 (USA) `8C11CCC2` |
| 604 | 604_matriz_divisao_cache | 4 | 54 | 2 | 0.01 | Napple Tale - Arsia in Daydr `8C16F564` |
| 605 | 605_copia | 4 | 37 | 1 | 0.01 | Napple Tale - Arsia in Daydr `8C1354A4` |
| 606 | 606_geral | 4 | 31 | 1 | 0.01 | Elemental Gimmick Gear v1.00 `8C031F80` |
| 607 | 607_geral | 4 | 20 | 2 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C121DF0` |
| 608 | 608_copia | 4 | 19 | 2 | 0.01 | Shenmue (USA) (Disc 1) `0C1D11B0` |
| 609 | 609_geral | 4 | 16 | 1 | 0.01 | Dead or Alive 2 (USA) `8C13C0C0` |
| 610 | 610_geral | 4 | 14 | 1 | 0.01 | Shenmue II (Europe) (En,Fr,D `8C1D92D6` |
| 611 | 611_geral | 4 | 13 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C122F50` |
| 612 | 612_float | 4 | 11 | 1 | 0.01 | Dead or Alive 2 (USA) `8C0FB0B0` |
| 613 | 613_copia | 3 | 132 | 2 | 0.01 | Le Mans 24 Hours (Europe) (E `8C22C4C0` |
| 614 | 614_copia | 3 | 125 | 1 | 0.01 | Elemental Gimmick Gear v1.00 `8C0808B2` |
| 615 | 615_geral | 3 | 61 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C124270` |
| 616 | 616_geral | 3 | 10 | 1 | 0.01 | Dead or Alive 2 (USA) `8C0F9DA0` |
| 617 | 617_geral | 3 | 9 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C04344A` |
| 618 | 618_float | 3 | 8 | 1 | 0.01 | Dead or Alive 2 (USA) `8C0FE590` |
| 619 | 619_matriz_divisao_pref | 2 | 988 | 7 | 0.01 | Soulcalibur (USA) `8C049CC0` |
| 620 | 620_geral | 2 | 387 | 2 | 0.01 | Shenmue (USA) (Disc 1) `0C152126` |
| 621 | 621_divisao | 2 | 169 | 1 | 0.01 | Dead or Alive 2 (USA) `8C0FD7F0` |
| 622 | 622_matriz | 2 | 86 | 1 | 0.01 | Shenmue (USA) (Disc 1) `0C091A38` |
| 623 | 623_copia | 2 | 80 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C125980` |
| 624 | 624_copia | 2 | 70 | 1 | 0.01 | Shenmue II (Europe) (En,Fr,D `8C03DAE0` |
| 625 | 625_produto_escalar | 2 | 56 | 1 | 0.01 | Napple Tale - Arsia in Daydr `8C143F14` |
| 626 | 626_geral | 2 | 47 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C010458` |
| 627 | 627_copia | 2 | 40 | 1 | 0.01 | Shenmue II (Europe) (En,Fr,D `AC005990` |
| 628 | 628_geral | 2 | 37 | 1 | 0.01 | Dead or Alive 2 (USA) `8C0FBA90` |
| 629 | 629_geral | 2 | 32 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C0102C0` |
| 630 | 630_geral | 2 | 20 | 1 | 0.01 | Dead or Alive 2 (USA) `8C0FCBA0` |
| 631 | 631_geral | 2 | 18 | 1 | 0.01 | Dead or Alive 2 (USA) `8C105CF0` |
| 632 | 632_geral | 2 | 15 | 2 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C031C98` |
| 633 | 633_float | 2 | 15 | 1 | 0.01 | Shenmue (USA) (Disc 1) `0C0915E0` |
| 634 | 634_geral | 2 | 14 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C1088E8` |
| 635 | 635_geral | 2 | 12 | 1 | 0.01 | Shenmue (USA) (Disc 1) `0C1DC260` |
| 636 | 636_geral | 2 | 12 | 1 | 0.01 | Shenmue (USA) (Disc 1) `8C004058` |
| 637 | 637_divisao | 2 | 11 | 1 | 0.01 | Shenmue (USA) (Disc 1) `0C091610` |
| 638 | 638_geral | 2 | 8 | 1 | 0.01 | Marvel vs. Capcom 2 - New Ag `8C03E00C` |
| 639 | 639_geral | 21 | 190 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C009214` |
| 640 | 640_copia | 21 | 106 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C008652` |
| 641 | 641_copia | 21 | 105 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0090F8` |
| 642 | 642_geral | 21 | 71 | 2 | 0.00 | Napple Tale - Arsia in Daydr `AC00E020` |
| 643 | 643_preenche | 21 | 64 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0085CC` |
| 644 | 644_copia | 21 | 64 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C008B68` |
| 645 | 645_copia | 21 | 57 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C008F70` |
| 646 | 646_copia | 21 | 51 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0088B0` |
| 647 | 647_geral | 21 | 50 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C00853C` |
| 648 | 648_geral | 21 | 50 | 11 | 0.00 | Elemental Gimmick Gear v1.00 `8C030568` |
| 649 | 649_geral | 21 | 43 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C008844` |
| 650 | 650_copia | 21 | 43 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C00D940` |
| 651 | 651_laco | 21 | 36 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C008FFC` |
| 652 | 652_geral | 21 | 28 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0097B4` |
| 653 | 653_geral | 21 | 26 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0087FC` |
| 654 | 654_geral | 21 | 26 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C009E24` |
| 655 | 655_copia | 21 | 20 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0083C0` |
| 656 | 656_geral | 21 | 20 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C00D820` |
| 657 | 657_geral | 21 | 19 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C009DEC` |
| 658 | 658_geral | 21 | 19 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C00D900` |
| 659 | 659_geral | 21 | 18 | 4 | 0.00 | Skies of Arcadia (USA) (Disc `8C190E66` |
| 660 | 660_geral | 21 | 14 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C008AA0` |
| 661 | 661_geral | 21 | 14 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C008B4C` |
| 662 | 662_geral | 21 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C008380` |
| 663 | 663_geral | 21 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0083A8` |
| 664 | 664_geral | 21 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C009C74` |
| 665 | 665_geral | 21 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C00D86C` |
| 666 | 666_geral | 21 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C00D888` |
| 667 | 667_geral | 21 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `AC00E0DC` |
| 668 | 668_geral | 21 | 10 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C009074` |
| 669 | 669_laco | 21 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C009C60` |
| 670 | 670_geral | 21 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `AC008300` |
| 671 | 671_geral | 21 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C009C4C` |
| 672 | 672_copia | 20 | 42 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C00892C` |
| 673 | 673_geral | 20 | 40 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C00DB4C` |
| 674 | 674_geral | 20 | 24 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C00DBA2` |
| 675 | 675_geral | 20 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C00D8C6` |
| 676 | 676_geral | 20 | 10 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C00DAEC` |
| 677 | 677_geral | 19 | 525 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0451DE` |
| 678 | 678_divisao | 19 | 191 | 7 | 0.00 | Le Mans 24 Hours (Europe) (E `8C044D0E` |
| 679 | 679_geral | 19 | 159 | 3 | 0.00 | Capcom vs. SNK 2 - Millionai `8C218660` |
| 680 | 680_geral | 19 | 147 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C009488` |
| 681 | 681_copia | 19 | 106 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20D406` |
| 682 | 682_copia | 19 | 95 | 6 | 0.00 | Shenmue (USA) (Disc 1) `0C1D760A` |
| 683 | 683_copia | 19 | 94 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C078224` |
| 684 | 684_geral | 19 | 75 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C205F48` |
| 685 | 685_copia | 19 | 69 | 4 | 0.00 | King of Fighters The - Evolu `8C319A1A` |
| 686 | 686_copia | 19 | 50 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F71B4` |
| 687 | 687_geral | 19 | 48 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C022EBC` |
| 688 | 688_geral | 19 | 48 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C0910BC` |
| 689 | 689_geral | 19 | 46 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C017944` |
| 690 | 690_geral | 19 | 45 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C207F20` |
| 691 | 691_geral | 19 | 41 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20D670` |
| 692 | 692_geral | 19 | 39 | 4 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F7246` |
| 693 | 693_copia | 19 | 29 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C205EC2` |
| 694 | 694_copia | 19 | 28 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20D330` |
| 695 | 695_copia | 19 | 26 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2227B4` |
| 696 | 696_geral | 19 | 24 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20D8B8` |
| 697 | 697_geral | 19 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C009858` |
| 698 | 698_geral | 19 | 19 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F5E84` |
| 699 | 699_copia | 19 | 19 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C205F22` |
| 700 | 700_geral | 19 | 18 | 3 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21A010` |
| 701 | 701_geral | 19 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C00940A` |
| 702 | 702_laco | 19 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F5EAA` |
| 703 | 703_trava | 19 | 14 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C221DA0` |
| 704 | 704_geral | 19 | 14 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C233EC0` |
| 705 | 705_laco | 19 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20D2D0` |
| 706 | 706_preenche | 19 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2220B4` |
| 707 | 707_copia | 19 | 10 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20D308` |
| 708 | 708_preenche | 19 | 10 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20D31C` |
| 709 | 709_geral | 19 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C208898` |
| 710 | 710_geral | 19 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C205CA8` |
| 711 | 711_geral | 19 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C208252` |
| 712 | 712_geral | 19 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C220D22` |
| 713 | 713_geral | 19 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C220D32` |
| 714 | 714_copia | 19 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C222108` |
| 715 | 715_contexto_cache | 18 | 433 | 6 | 0.00 | Le Mans 24 Hours (Europe) (E `8C04C084` |
| 716 | 716_divisao | 18 | 387 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0461CA` |
| 717 | 717_copia | 18 | 252 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C091B60` |
| 718 | 718_copia | 18 | 190 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C01CA52` |
| 719 | 719_copia | 18 | 186 | 5 | 0.00 | Le Mans 24 Hours (Europe) (E `8C21EFCC` |
| 720 | 720_copia | 18 | 140 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C221A9A` |
| 721 | 721_copia | 18 | 132 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C01E548` |
| 722 | 722_copia | 18 | 111 | 3 | 0.00 | Soulcalibur (USA) `8C241A46` |
| 723 | 723_geral | 18 | 105 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20D728` |
| 724 | 724_geral | 18 | 95 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C01C8DA` |
| 725 | 725_geral | 18 | 93 | 7 | 0.00 | Grandia II (USA) `8C016114` |
| 726 | 726_geral | 18 | 67 | 5 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0457D4` |
| 727 | 727_geral | 18 | 62 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C075746` |
| 728 | 728_geral | 18 | 59 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C07C2D0` |
| 729 | 729_geral | 18 | 54 | 3 | 0.00 | Evolution - The World of Sac `8C1B212A` |
| 730 | 730_copia | 18 | 48 | 2 | 0.00 | Dead or Alive 2 (USA) `8C122AA2` |
| 731 | 731_geral | 18 | 47 | 3 | 0.00 | Evolution - The World of Sac `8C16434C` |
| 732 | 732_geral | 18 | 44 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F6928` |
| 733 | 733_copia | 18 | 40 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `AC1F8DC4` |
| 734 | 734_geral | 18 | 36 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C0787EA` |
| 735 | 735_laco | 18 | 29 | 4 | 0.00 | Capcom vs. SNK 2 - Millionai `8C17B5B8` |
| 736 | 736_geral | 18 | 29 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FB0A6` |
| 737 | 737_geral | 18 | 27 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F615C` |
| 738 | 738_geral | 18 | 23 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C221E24` |
| 739 | 739_geral | 18 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F92FC` |
| 740 | 740_geral | 18 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F98F8` |
| 741 | 741_geral | 18 | 20 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C207D2C` |
| 742 | 742_geral | 18 | 19 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C221DCC` |
| 743 | 743_geral | 18 | 19 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F9A10` |
| 744 | 744_geral | 18 | 19 | 2 | 0.00 | Resident Evil - Code - Veron `8C1B08EC` |
| 745 | 745_geral | 18 | 18 | 4 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F8D98` |
| 746 | 746_geral | 18 | 18 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20904C` |
| 747 | 747_trava | 18 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C221DF2` |
| 748 | 748_geral | 18 | 16 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C243740` |
| 749 | 749_geral | 18 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C205DEA` |
| 750 | 750_geral | 18 | 14 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F7ADC` |
| 751 | 751_copia | 18 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20D2EC` |
| 752 | 752_geral | 18 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C220D08` |
| 753 | 753_geral | 18 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C222138` |
| 754 | 754_geral | 18 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C220CA4` |
| 755 | 755_geral | 18 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20903C` |
| 756 | 756_geral | 17 | 195 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2221B0` |
| 757 | 757_geral | 17 | 123 | 3 | 0.00 | Capcom vs. SNK 2 - Millionai `8C208B54` |
| 758 | 758_copia | 17 | 118 | 3 | 0.00 | Dead or Alive 2 (USA) `8C134758` |
| 759 | 759_cache | 17 | 108 | 8 | 0.00 | Elemental Gimmick Gear v1.00 `AC07CCA0` |
| 760 | 760_copia | 17 | 90 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20919E` |
| 761 | 761_copia | 17 | 89 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C209EC8` |
| 762 | 762_copia | 17 | 85 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C23694A` |
| 763 | 763_copia | 17 | 79 | 3 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F7F10` |
| 764 | 764_geral | 17 | 68 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2368B4` |
| 765 | 765_geral | 17 | 66 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C208998` |
| 766 | 766_geral | 17 | 64 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2088C4` |
| 767 | 767_laco | 17 | 61 | 4 | 0.00 | Phantasy Star Online Ver. 2  `8C00892C` |
| 768 | 768_geral | 17 | 57 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20C578` |
| 769 | 769_copia | 17 | 53 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20B83A` |
| 770 | 770_copia | 17 | 53 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20BD30` |
| 771 | 771_copia | 17 | 48 | 3 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20C0DE` |
| 772 | 772_geral | 17 | 45 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C207638` |
| 773 | 773_geral | 17 | 43 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `AC1F85E8` |
| 774 | 774_copia | 17 | 42 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20C1EA` |
| 775 | 775_geral | 17 | 38 | 2 | 0.00 | Evolution - The World of Sac `8C1DCB30` |
| 776 | 776_geral | 17 | 26 | 2 | 0.00 | Evolution - The World of Sac `8C1B1E94` |
| 777 | 777_geral | 17 | 24 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C221E8C` |
| 778 | 778_geral | 17 | 24 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21A050` |
| 779 | 779_copia | 17 | 23 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C17B5F8` |
| 780 | 780_trava | 17 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C221E52` |
| 781 | 781_geral | 17 | 19 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C37FC78` |
| 782 | 782_geral | 17 | 16 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C209170` |
| 783 | 783_geral | 17 | 16 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20C548` |
| 784 | 784_geral | 17 | 16 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `AC010000` |
| 785 | 785_float | 17 | 14 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C191D64` |
| 786 | 786_preenche | 17 | 14 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C208FD8` |
| 787 | 787_geral | 17 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20C1BA` |
| 788 | 788_geral | 17 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C22214E` |
| 789 | 789_geral | 17 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F7B9A` |
| 790 | 790_geral | 16 | 271 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F61EC` |
| 791 | 791_laco | 16 | 83 | 8 | 0.00 | Shenmue (USA) (Disc 1) `0C1DC710` |
| 792 | 792_geral | 16 | 62 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C01CE04` |
| 793 | 793_copia | 16 | 58 | 2 | 0.00 | Dead or Alive 2 (USA) `8C13E642` |
| 794 | 794_copia | 16 | 52 | 4 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F96C0` |
| 795 | 795_geral | 16 | 49 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20C7FA` |
| 796 | 796_copia | 16 | 47 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C07071A` |
| 797 | 797_geral | 16 | 28 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FA2C2` |
| 798 | 798_geral | 16 | 26 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F6C14` |
| 799 | 799_laco | 16 | 20 | 2 | 0.00 | Dead or Alive 2 (USA) `8C120360` |
| 800 | 800_geral | 16 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20C73C` |
| 801 | 801_geral | 16 | 10 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20123C` |
| 802 | 802_geral | 16 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F7C22` |
| 803 | 803_geral | 16 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C18845E` |
| 804 | 804_geral | 15 | 166 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C06A2A4` |
| 805 | 805_copia | 15 | 120 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1818E2` |
| 806 | 806_copia | 15 | 104 | 7 | 0.00 | Elemental Gimmick Gear v1.00 `8C061C70` |
| 807 | 807_copia | 15 | 103 | 3 | 0.00 | Dead or Alive 2 (USA) `8C11E930` |
| 808 | 808_copia | 15 | 97 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C203084` |
| 809 | 809_raiz_divisao | 15 | 66 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C200B9C` |
| 810 | 810_copia | 15 | 66 | 2 | 0.00 | Dead or Alive 2 (USA) `8C132A44` |
| 811 | 811_geral | 15 | 57 | 7 | 0.00 | Elemental Gimmick Gear v1.00 `8C07B14C` |
| 812 | 812_copia | 15 | 49 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C0135E8` |
| 813 | 813_copia | 15 | 25 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F5BDE` |
| 814 | 814_geral | 15 | 24 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C206A34` |
| 815 | 815_geral | 15 | 23 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C17B4CC` |
| 816 | 816_geral | 15 | 20 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F5B76` |
| 817 | 817_geral | 15 | 20 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F9998` |
| 818 | 818_geral | 15 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2049DA` |
| 819 | 819_geral | 15 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C200F70` |
| 820 | 820_geral | 15 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2013DE` |
| 821 | 821_geral | 15 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C203F14` |
| 822 | 822_geral | 15 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C23643C` |
| 823 | 823_geral | 15 | 11 | 1 | 0.00 | Dead or Alive 2 (USA) `8C116678` |
| 824 | 824_geral | 15 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F738A` |
| 825 | 825_geral | 15 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F9922` |
| 826 | 826_geral | 14 | 287 | 6 | 0.00 | Power Stone (USA) `0C108B64` |
| 827 | 827_geral | 14 | 210 | 10 | 0.00 | Evolution - The World of Sac `8C1CE8E8` |
| 828 | 828_geral | 14 | 164 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C02C342` |
| 829 | 829_copia | 14 | 141 | 2 | 0.00 | Grandia II (USA) `8C0BCBE0` |
| 830 | 830_geral | 14 | 107 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0833B6` |
| 831 | 831_geral | 14 | 88 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09A5FC` |
| 832 | 832_geral | 14 | 88 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C096160` |
| 833 | 833_copia | 14 | 83 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C09A3E0` |
| 834 | 834_geral | 14 | 74 | 6 | 0.00 | Skies of Arcadia (USA) (Disc `8C256B92` |
| 835 | 835_float | 14 | 74 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C082A26` |
| 836 | 836_geral | 14 | 70 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FA01C` |
| 837 | 837_geral | 14 | 70 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C096340` |
| 838 | 838_copia | 14 | 56 | 2 | 0.00 | Evolution - The World of Sac `8C1D234E` |
| 839 | 839_geral | 14 | 53 | 4 | 0.00 | King of Fighters The - Evolu `8C333ACC` |
| 840 | 840_geral | 14 | 53 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C098DE4` |
| 841 | 841_geral | 14 | 52 | 5 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2036A6` |
| 842 | 842_geral | 14 | 48 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C093C2C` |
| 843 | 843_geral | 14 | 47 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C200D58` |
| 844 | 844_geral | 14 | 45 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C087E20` |
| 845 | 845_geral | 14 | 45 | 3 | 0.00 | Evolution - The World of Sac `8C1CE31A` |
| 846 | 846_geral | 14 | 42 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C081BE2` |
| 847 | 847_geral | 14 | 38 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09A540` |
| 848 | 848_geral | 14 | 36 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C096560` |
| 849 | 849_geral | 14 | 35 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09AE80` |
| 850 | 850_geral | 14 | 31 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21CEA6` |
| 851 | 851_geral | 14 | 30 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09A5C0` |
| 852 | 852_geral | 14 | 29 | 2 | 0.00 | Evolution - The World of Sac `8C1CE0CC` |
| 853 | 853_geral | 14 | 27 | 5 | 0.00 | Evolution - The World of Sac `8C1CEC66` |
| 854 | 854_geral | 14 | 24 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0820EC` |
| 855 | 855_geral | 14 | 22 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C376E96` |
| 856 | 856_geral | 14 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C201CEE` |
| 857 | 857_geral | 14 | 16 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C236894` |
| 858 | 858_geral | 14 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F9FC6` |
| 859 | 859_geral | 14 | 15 | 1 | 0.00 | Dead or Alive 2 (USA) `8C1259B6` |
| 860 | 860_geral | 14 | 14 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C097B56` |
| 861 | 861_preenche | 14 | 14 | 1 | 0.00 | Dead or Alive 2 (USA) `8C125A44` |
| 862 | 862_geral | 14 | 14 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C081C5E` |
| 863 | 863_geral | 14 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21D090` |
| 864 | 864_geral | 14 | 13 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C081F9E` |
| 865 | 865_geral | 14 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2363BA` |
| 866 | 866_geral | 14 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C23640C` |
| 867 | 867_geral | 14 | 11 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09A37C` |
| 868 | 868_geral | 14 | 10 | 1 | 0.00 | Dead or Alive 2 (USA) `8C12281C` |
| 869 | 869_geral | 14 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F40B6` |
| 870 | 870_geral | 14 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21CCB2` |
| 871 | 871_geral | 14 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C17B558` |
| 872 | 872_geral | 14 | 8 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C082A16` |
| 873 | 873_divisao | 13 | 475 | 5 | 0.00 | Phantasy Star Online Ver. 2  `8C3C9B80` |
| 874 | 874_geral | 13 | 245 | 7 | 0.00 | Grandia II (USA) `8C0BC938` |
| 875 | 875_geral | 13 | 229 | 11 | 0.00 | Napple Tale - Arsia in Daydr `8C132B3C` |
| 876 | 876_copia | 13 | 194 | 7 | 0.00 | Grandia II (USA) `8C0BC6C0` |
| 877 | 877_geral | 13 | 164 | 5 | 0.00 | Evolution - The World of Sac `8C1F5652` |
| 878 | 878_copia | 13 | 152 | 2 | 0.00 | Soulcalibur (USA) `8C23C0C0` |
| 879 | 879_copia | 13 | 132 | 9 | 0.00 | Evolution - The World of Sac `8C1EF10A` |
| 880 | 880_geral | 13 | 130 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C093D20` |
| 881 | 881_geral | 13 | 125 | 7 | 0.00 | Grandia II (USA) `8C0BC516` |
| 882 | 882_copia | 13 | 109 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C097372` |
| 883 | 883_copia | 13 | 101 | 6 | 0.00 | Grandia II (USA) `8C0BC3D0` |
| 884 | 884_geral | 13 | 90 | 4 | 0.00 | Dead or Alive 2 (USA) `8C1208D2` |
| 885 | 885_geral | 13 | 87 | 4 | 0.00 | Phantasy Star Online Ver. 2  `8C35A5EE` |
| 886 | 886_geral | 13 | 87 | 9 | 0.00 | Le Mans 24 Hours (Europe) (E `8C218706` |
| 887 | 887_geral | 13 | 76 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C088984` |
| 888 | 888_copia | 13 | 74 | 3 | 0.00 | Evolution - The World of Sac `8C1EDC60` |
| 889 | 889_geral | 13 | 46 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0964C0` |
| 890 | 890_copia | 13 | 42 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C098A20` |
| 891 | 891_geral | 13 | 41 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F4788` |
| 892 | 892_preenche | 13 | 40 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C098F80` |
| 893 | 893_geral | 13 | 38 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F53A8` |
| 894 | 894_geral | 13 | 37 | 3 | 0.00 | Napple Tale - Arsia in Daydr `8C110692` |
| 895 | 895_geral | 13 | 34 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F535C` |
| 896 | 896_geral | 13 | 25 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21D030` |
| 897 | 897_geral | 13 | 22 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C098AA0` |
| 898 | 898_copia | 13 | 20 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09A34E` |
| 899 | 899_laco | 13 | 19 | 3 | 0.00 | Evolution - The World of Sac `8C15D642` |
| 900 | 900_geral | 13 | 18 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C082AE0` |
| 901 | 901_geral | 13 | 15 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C114424` |
| 902 | 902_geral | 13 | 14 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C201D44` |
| 903 | 903_geral | 13 | 12 | 1 | 0.00 | Dead or Alive 2 (USA) `8C13C820` |
| 904 | 904_geral | 13 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2013C8` |
| 905 | 905_copia | 13 | 11 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C096000` |
| 906 | 906_geral | 13 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F8F0E` |
| 907 | 907_geral | 13 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C23642A` |
| 908 | 908_geral | 13 | 8 | 1 | 0.00 | Evolution - The World of Sac `8C1CBC4E` |
| 909 | 909_geral | 12 | 385 | 5 | 0.00 | Capcom vs. SNK 2 - Millionai `8C095B08` |
| 910 | 910_copia | 12 | 171 | 4 | 0.00 | Skies of Arcadia (USA) (Disc `8C2DA640` |
| 911 | 911_divisao | 12 | 166 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0637E0` |
| 912 | 912_geral | 12 | 139 | 8 | 0.00 | Resident Evil - Code - Veron `8C1AC46C` |
| 913 | 913_geral | 12 | 117 | 1 | 0.00 | Dead or Alive 2 (USA) `8C11E460` |
| 914 | 914_geral | 12 | 113 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C209474` |
| 915 | 915_geral | 12 | 113 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09C020` |
| 916 | 916_geral | 12 | 85 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C209722` |
| 917 | 917_copia | 12 | 84 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C0631F4` |
| 918 | 918_geral | 12 | 84 | 3 | 0.00 | Evolution - The World of Sac `8C1F1DC0` |
| 919 | 919_geral | 12 | 74 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C209628` |
| 920 | 920_geral | 12 | 68 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C063C42` |
| 921 | 921_geral | 12 | 57 | 4 | 0.00 | Grandia II (USA) `8C071160` |
| 922 | 922_geral | 12 | 55 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C024698` |
| 923 | 923_copia | 12 | 51 | 2 | 0.00 | Dead or Alive 2 (USA) `8C11FAA8` |
| 924 | 924_geral | 12 | 51 | 3 | 0.00 | Napple Tale - Arsia in Daydr `8C114300` |
| 925 | 925_copia | 12 | 45 | 4 | 0.00 | Evolution 2 - Far Off Promis `8C1A7CFC` |
| 926 | 926_copia | 12 | 44 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C095F78` |
| 927 | 927_geral | 12 | 40 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F8EC8` |
| 928 | 928_geral | 12 | 38 | 13 | 0.00 | Le Mans 24 Hours (Europe) (E `8C231E32` |
| 929 | 929_geral | 12 | 33 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09C328` |
| 930 | 930_geral | 12 | 31 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F3BEC` |
| 931 | 931_geral | 12 | 29 | 2 | 0.00 | Dead or Alive 2 (USA) `8C12014C` |
| 932 | 932_geral | 12 | 28 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FA2FE` |
| 933 | 933_geral | 12 | 28 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C085380` |
| 934 | 934_geral | 12 | 27 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C063C0C` |
| 935 | 935_geral | 12 | 27 | 2 | 0.00 | Grandia II (USA) `8C03BA48` |
| 936 | 936_geral | 12 | 26 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09687A` |
| 937 | 937_geral | 12 | 20 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2040E8` |
| 938 | 938_geral | 12 | 19 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20DB2C` |
| 939 | 939_geral | 12 | 18 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C222048` |
| 940 | 940_geral | 12 | 18 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06B9C0` |
| 941 | 941_geral | 12 | 18 | 2 | 0.00 | Evolution - The World of Sac `8C1CE748` |
| 942 | 942_geral | 12 | 16 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21D0AA` |
| 943 | 943_geral | 12 | 15 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21D4BC` |
| 944 | 944_geral | 12 | 14 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C081F4A` |
| 945 | 945_geral | 12 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FAC2C` |
| 946 | 946_geral | 12 | 12 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06484C` |
| 947 | 947_geral | 12 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C049D52` |
| 948 | 948_geral | 12 | 9 | 1 | 0.00 | Dead or Alive 2 (USA) `8C11F050` |
| 949 | 949_geral | 12 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F7B84` |
| 950 | 950_geral | 12 | 8 | 1 | 0.00 | Dead or Alive 2 (USA) `8C116ECA` |
| 951 | 951_copia | 11 | 428 | 7 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0531D4` |
| 952 | 952_geral | 11 | 351 | 6 | 0.00 | Grandia II (USA) `8C0F9AC0` |
| 953 | 953_divisao | 11 | 212 | 9 | 0.00 | Le Mans 24 Hours (Europe) (E `8C01445E` |
| 954 | 954_geral | 11 | 210 | 2 | 0.00 | Evolution - The World of Sac `8C1F2AC6` |
| 955 | 955_geral | 11 | 167 | 9 | 0.00 | Grandia II (USA) `8C025E50` |
| 956 | 956_geral | 11 | 136 | 9 | 0.00 | Skies of Arcadia (USA) (Disc `8C028910` |
| 957 | 957_copia | 11 | 135 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C09799C` |
| 958 | 958_geral | 11 | 96 | 9 | 0.00 | Resident Evil - Code - Veron `8C1AC640` |
| 959 | 959_divisao | 11 | 92 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C01471C` |
| 960 | 960_geral | 11 | 75 | 7 | 0.00 | Shenmue II (Europe) (En,Fr,D `8C03F388` |
| 961 | 961_copia | 11 | 66 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0147FE` |
| 962 | 962_geral | 11 | 56 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C063EA6` |
| 963 | 963_geral | 11 | 43 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F9DC0` |
| 964 | 964_laco | 11 | 38 | 2 | 0.00 | Evolution - The World of Sac `8C1CED14` |
| 965 | 965_geral | 11 | 37 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05B600` |
| 966 | 966_copia | 11 | 36 | 2 | 0.00 | Evolution - The World of Sac `8C1CECC0` |
| 967 | 967_geral | 11 | 30 | 3 | 0.00 | Grandia II (USA) `8C041CDA` |
| 968 | 968_copia | 11 | 30 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C097960` |
| 969 | 969_geral | 11 | 25 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C066592` |
| 970 | 970_geral | 11 | 24 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C066940` |
| 971 | 971_geral | 11 | 18 | 1 | 0.00 | Evolution - The World of Sac `8C1F2328` |
| 972 | 972_geral | 11 | 17 | 2 | 0.00 | Skies of Arcadia (USA) (Disc `8C25974C` |
| 973 | 973_float | 11 | 16 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C031100` |
| 974 | 974_geral | 11 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C072078` |
| 975 | 975_geral | 11 | 9 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C066794` |
| 976 | 976_copia | 10 | 198 | 4 | 0.00 | Evolution - The World of Sac `8C1EF92C` |
| 977 | 977_copia | 10 | 113 | 2 | 0.00 | Grandia II (USA) `8C037CA4` |
| 978 | 978_geral | 10 | 98 | 4 | 0.00 | Phantasy Star Online Ver. 2  `8C38B95E` |
| 979 | 979_copia | 10 | 86 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C097CC0` |
| 980 | 980_divisao | 10 | 71 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C014618` |
| 981 | 981_geral | 10 | 58 | 1 | 0.00 | Dead or Alive 2 (USA) `8C1181D0` |
| 982 | 982_copia | 10 | 44 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C06EE56` |
| 983 | 983_geral | 10 | 42 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05B4EC` |
| 984 | 984_geral | 10 | 37 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C02F594` |
| 985 | 985_geral | 10 | 35 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C010CF8` |
| 986 | 986_geral | 10 | 32 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C096D2E` |
| 987 | 987_geral | 10 | 30 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C096D6E` |
| 988 | 988_geral | 10 | 29 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C205244` |
| 989 | 989_geral | 10 | 29 | 1 | 0.00 | Dead or Alive 2 (USA) `8C1180DA` |
| 990 | 990_geral | 10 | 28 | 3 | 0.00 | Evolution 2 - Far Off Promis `8C17283C` |
| 991 | 991_geral | 10 | 23 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C06C12E` |
| 992 | 992_copia | 10 | 20 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09C8BE` |
| 993 | 993_geral | 10 | 19 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07790C` |
| 994 | 994_laco | 10 | 19 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09C898` |
| 995 | 995_copia | 10 | 18 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F3300` |
| 996 | 996_geral | 10 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20125A` |
| 997 | 997_geral | 10 | 17 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06A4C4` |
| 998 | 998_copia | 10 | 15 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C17B508` |
| 999 | 999_geral | 10 | 14 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C081F82` |
| 1000 | 1000_geral | 10 | 12 | 1 | 0.00 | Dead or Alive 2 (USA) `8C114206` |
| 1001 | 1001_geral | 10 | 11 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06657C` |
| 1002 | 1002_geral | 10 | 11 | 1 | 0.00 | Evolution - The World of Sac `8C19C03C` |
| 1003 | 1003_geral | 10 | 9 | 1 | 0.00 | Dead or Alive 2 (USA) `8C116F94` |
| 1004 | 1004_geral | 10 | 8 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C077830` |
| 1005 | 1005_geral | 10 | 8 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C082EA0` |
| 1006 | 1006_geral | 10 | 8 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0A27A6` |
| 1007 | 1007_float | 10 | 8 | 1 | 0.00 | Evolution - The World of Sac `8C1A2ACA` |
| 1008 | 1008_copia | 9 | 328 | 4 | 0.00 | Evolution - The World of Sac `8C1D7530` |
| 1009 | 1009_copia | 9 | 259 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C058250` |
| 1010 | 1010_copia | 9 | 254 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09D1FE` |
| 1011 | 1011_copia | 9 | 252 | 6 | 0.00 | Phantasy Star Online Ver. 2  `8C36162A` |
| 1012 | 1012_copia | 9 | 251 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09CDBA` |
| 1013 | 1013_copia | 9 | 229 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09CFF4` |
| 1014 | 1014_geral | 9 | 226 | 6 | 0.00 | Le Mans 24 Hours (Europe) (E `8C01E850` |
| 1015 | 1015_copia | 9 | 223 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C04CD8C` |
| 1016 | 1016_geral | 9 | 209 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C08B8C8` |
| 1017 | 1017_divisao | 9 | 173 | 5 | 0.00 | Grandia II (USA) `8C03B596` |
| 1018 | 1018_preenche | 9 | 159 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C09E0EA` |
| 1019 | 1019_copia | 9 | 155 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C09D55A` |
| 1020 | 1020_geral | 9 | 142 | 3 | 0.00 | Evolution 2 - Far Off Promis `8C19A19C` |
| 1021 | 1021_geral | 9 | 120 | 2 | 0.00 | Grandia II (USA) `8C0C6C30` |
| 1022 | 1022_divisao | 9 | 115 | 4 | 0.00 | Le Mans 24 Hours (Europe) (E `8C013EA0` |
| 1023 | 1023_divisao | 9 | 100 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C056212` |
| 1024 | 1024_geral | 9 | 99 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2202FC` |
| 1025 | 1025_geral | 9 | 99 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C05F582` |
| 1026 | 1026_geral | 9 | 93 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C063838` |
| 1027 | 1027_geral | 9 | 89 | 4 | 0.00 | Dead or Alive 2 (USA) `8C12110C` |
| 1028 | 1028_geral | 9 | 88 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C060C1E` |
| 1029 | 1029_preenche | 9 | 83 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09D4B4` |
| 1030 | 1030_geral | 9 | 80 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C09E3C0` |
| 1031 | 1031_copia | 9 | 76 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C097BCA` |
| 1032 | 1032_geral | 9 | 69 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C05D56C` |
| 1033 | 1033_copia | 9 | 67 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C07C0EC` |
| 1034 | 1034_copia | 9 | 59 | 3 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F3B60` |
| 1035 | 1035_geral | 9 | 57 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09CC44` |
| 1036 | 1036_geral | 9 | 57 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C027C60` |
| 1037 | 1037_geral | 9 | 56 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07304C` |
| 1038 | 1038_float | 9 | 56 | 5 | 0.00 | Resident Evil - Code - Veron `8C1971FC` |
| 1039 | 1039_geral | 9 | 53 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C072D2C` |
| 1040 | 1040_geral | 9 | 52 | 3 | 0.00 | Grandia II (USA) `8C0C0AD0` |
| 1041 | 1041_geral | 9 | 49 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09CA40` |
| 1042 | 1042_copia | 9 | 48 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09CB68` |
| 1043 | 1043_copia | 9 | 45 | 2 | 0.00 | Evolution - The World of Sac `8C1A77D0` |
| 1044 | 1044_copia | 9 | 44 | 3 | 0.00 | Dead or Alive 2 (USA) `8C113EA6` |
| 1045 | 1045_geral | 9 | 44 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C199AA0` |
| 1046 | 1046_geral | 9 | 43 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07793E` |
| 1047 | 1047_copia | 9 | 41 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08EEF2` |
| 1048 | 1048_geral | 9 | 41 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08F0C6` |
| 1049 | 1049_copia | 9 | 40 | 2 | 0.00 | Dead or Alive 2 (USA) `8C115D9C` |
| 1050 | 1050_copia | 9 | 40 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C38A474` |
| 1051 | 1051_geral | 9 | 40 | 2 | 0.00 | Evolution - The World of Sac `8C1B876A` |
| 1052 | 1052_geral | 9 | 40 | 1 | 0.00 | Grandia II (USA) `8C0C6D6C` |
| 1053 | 1053_preenche | 9 | 39 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09D444` |
| 1054 | 1054_geral | 9 | 38 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C075270` |
| 1055 | 1055_copia | 9 | 37 | 3 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20C018` |
| 1056 | 1056_geral | 9 | 37 | 3 | 0.00 | Phantasy Star Online Ver. 2  `8C34F77E` |
| 1057 | 1057_geral | 9 | 37 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C074BB0` |
| 1058 | 1058_geral | 9 | 36 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C073B2E` |
| 1059 | 1059_copia | 9 | 36 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08EF44` |
| 1060 | 1060_geral | 9 | 35 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08F06C` |
| 1061 | 1061_copia | 9 | 35 | 2 | 0.00 | Evolution - The World of Sac `8C1B7554` |
| 1062 | 1062_copia | 9 | 34 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FB008` |
| 1063 | 1063_geral | 9 | 34 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C073AD2` |
| 1064 | 1064_copia | 9 | 34 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C06CA62` |
| 1065 | 1065_geral | 9 | 34 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07799E` |
| 1066 | 1066_copia | 9 | 34 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C18878C` |
| 1067 | 1067_pref | 9 | 33 | 1 | 0.00 | Evolution - The World of Sac `8C1B7C2A` |
| 1068 | 1068_geral | 9 | 31 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09E460` |
| 1069 | 1069_copia | 9 | 31 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0665EA` |
| 1070 | 1070_geral | 9 | 30 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C09E49E` |
| 1071 | 1071_geral | 9 | 30 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C077852` |
| 1072 | 1072_geral | 9 | 29 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0778CC` |
| 1073 | 1073_float | 9 | 28 | 1 | 0.00 | Evolution - The World of Sac `8C1A2A84` |
| 1074 | 1074_geral | 9 | 28 | 1 | 0.00 | Grandia II (USA) `8C037FB8` |
| 1075 | 1075_preenche | 9 | 26 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09CC10` |
| 1076 | 1076_float | 9 | 26 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06A524` |
| 1077 | 1077_geral | 9 | 26 | 1 | 0.00 | Grandia II (USA) `8C037FF4` |
| 1078 | 1078_geral | 9 | 25 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1ACDB8` |
| 1079 | 1079_geral | 9 | 25 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C072638` |
| 1080 | 1080_geral | 9 | 25 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09CCBA` |
| 1081 | 1081_geral | 9 | 24 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08EFC8` |
| 1082 | 1082_geral | 9 | 24 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07446A` |
| 1083 | 1083_geral | 9 | 23 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08D1E4` |
| 1084 | 1084_geral | 9 | 23 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C074A56` |
| 1085 | 1085_geral | 9 | 23 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08D244` |
| 1086 | 1086_copia | 9 | 22 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0779EC` |
| 1087 | 1087_geral | 9 | 22 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071630` |
| 1088 | 1088_geral | 9 | 20 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C22040C` |
| 1089 | 1089_geral | 9 | 20 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C073EC2` |
| 1090 | 1090_geral | 9 | 20 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07E730` |
| 1091 | 1091_geral | 9 | 20 | 2 | 0.00 | King of Fighters The - Evolu `8C34CF22` |
| 1092 | 1092_preenche | 9 | 19 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08EFA2` |
| 1093 | 1093_float | 9 | 19 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06A620` |
| 1094 | 1094_geral | 9 | 19 | 1 | 0.00 | Grandia II (USA) `8C0BAFFC` |
| 1095 | 1095_geral | 9 | 18 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09A4C2` |
| 1096 | 1096_geral | 9 | 18 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09CBEC` |
| 1097 | 1097_geral | 9 | 18 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09E770` |
| 1098 | 1098_geral | 9 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C209298` |
| 1099 | 1099_geral | 9 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C205D94` |
| 1100 | 1100_geral | 9 | 17 | 1 | 0.00 | Evolution - The World of Sac `8C19BD94` |
| 1101 | 1101_geral | 9 | 16 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C073F0E` |
| 1102 | 1102_geral | 9 | 16 | 2 | 0.00 | Evolution - The World of Sac `8C1F7DB0` |
| 1103 | 1103_geral | 9 | 16 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C073958` |
| 1104 | 1104_geral | 9 | 16 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08C504` |
| 1105 | 1105_copia | 9 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20C62E` |
| 1106 | 1106_geral | 9 | 15 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08EEC8` |
| 1107 | 1107_geral | 9 | 14 | 2 | 0.00 | Evolution - The World of Sac `8C1D4928` |
| 1108 | 1108_laco | 9 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20BFEA` |
| 1109 | 1109_geral | 9 | 13 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08D21E` |
| 1110 | 1110_geral | 9 | 13 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09CCEE` |
| 1111 | 1111_float | 9 | 13 | 1 | 0.00 | Evolution - The World of Sac `8C16BE58` |
| 1112 | 1112_geral | 9 | 11 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08D2EE` |
| 1113 | 1113_laco | 9 | 11 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08E48E` |
| 1114 | 1114_geral | 9 | 10 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0A0A08` |
| 1115 | 1115_geral | 9 | 10 | 1 | 0.00 | Evolution - The World of Sac `8C1D49B6` |
| 1116 | 1116_laco | 9 | 10 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C171878` |
| 1117 | 1117_geral | 9 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F7C68` |
| 1118 | 1118_geral | 9 | 9 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0747DE` |
| 1119 | 1119_contexto | 9 | 8 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C077694` |
| 1120 | 1120_geral | 8 | 344 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C08DA7E` |
| 1121 | 1121_raiz_divisao | 8 | 315 | 2 | 0.00 | Grandia II (USA) `8C0655D8` |
| 1122 | 1122_geral | 8 | 271 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D0D20` |
| 1123 | 1123_copia | 8 | 220 | 3 | 0.00 | Evolution - The World of Sac `8C2173BC` |
| 1124 | 1124_geral | 8 | 219 | 8 | 0.00 | Grandia II (USA) `8C03D824` |
| 1125 | 1125_geral | 8 | 191 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05E358` |
| 1126 | 1126_copia | 8 | 189 | 5 | 0.00 | King of Fighters The - Evolu `8C31D44A` |
| 1127 | 1127_geral | 8 | 139 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C08E13C` |
| 1128 | 1128_copia | 8 | 134 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0B01E4` |
| 1129 | 1129_copia | 8 | 131 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C3820C2` |
| 1130 | 1130_copia | 8 | 112 | 5 | 0.00 | King of Fighters The - Evolu `8C330F00` |
| 1131 | 1131_geral | 8 | 103 | 2 | 0.00 | Evolution - The World of Sac `8C213690` |
| 1132 | 1132_matriz_produto_escalar_divisao | 8 | 98 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C061D2A` |
| 1133 | 1133_geral | 8 | 97 | 3 | 0.00 | Evolution 2 - Far Off Promis `8C1D2398` |
| 1134 | 1134_geral | 8 | 95 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C079B10` |
| 1135 | 1135_geral | 8 | 95 | 2 | 0.00 | King of Fighters The - Evolu `8C3320B4` |
| 1136 | 1136_geral | 8 | 94 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A3DC4` |
| 1137 | 1137_copia | 8 | 93 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C071A08` |
| 1138 | 1138_copia | 8 | 86 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1AB208` |
| 1139 | 1139_geral | 8 | 85 | 3 | 0.00 | Resident Evil - Code - Veron `8C1B23A4` |
| 1140 | 1140_copia | 8 | 83 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C08D318` |
| 1141 | 1141_geral | 8 | 75 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C0722E8` |
| 1142 | 1142_geral | 8 | 74 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20A9E4` |
| 1143 | 1143_geral | 8 | 74 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1A2AE4` |
| 1144 | 1144_geral | 8 | 71 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C05E580` |
| 1145 | 1145_geral | 8 | 66 | 2 | 0.00 | Grandia II (USA) `8C06DE2E` |
| 1146 | 1146_copia | 8 | 66 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D1F20` |
| 1147 | 1147_geral | 8 | 66 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D30F8` |
| 1148 | 1148_geral | 8 | 65 | 2 | 0.00 | Dead or Alive 2 (USA) `8C1148F4` |
| 1149 | 1149_preenche | 8 | 65 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09E05C` |
| 1150 | 1150_copia | 8 | 65 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1FF33C` |
| 1151 | 1151_copia | 8 | 62 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D1D68` |
| 1152 | 1152_geral | 8 | 61 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09E288` |
| 1153 | 1153_preenche | 8 | 60 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09DFE4` |
| 1154 | 1154_copia | 8 | 59 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20909C` |
| 1155 | 1155_copia | 8 | 56 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1FEE04` |
| 1156 | 1156_geral | 8 | 54 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1FED90` |
| 1157 | 1157_geral | 8 | 52 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C0715C8` |
| 1158 | 1158_geral | 8 | 51 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1A3F08` |
| 1159 | 1159_geral | 8 | 50 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05E1B6` |
| 1160 | 1160_geral | 8 | 50 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05E21A` |
| 1161 | 1161_geral | 8 | 50 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08E022` |
| 1162 | 1162_matriz_produto_escalar | 8 | 50 | 4 | 0.00 | Evolution - The World of Sac `8C197CDC` |
| 1163 | 1163_geral | 8 | 46 | 2 | 0.00 | Dead or Alive 2 (USA) `8C1146BA` |
| 1164 | 1164_geral | 8 | 46 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A2D30` |
| 1165 | 1165_copia | 8 | 46 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D2594` |
| 1166 | 1166_copia | 8 | 45 | 3 | 0.00 | King of Fighters The - Evolu `8C338A98` |
| 1167 | 1167_geral | 8 | 45 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08CCEC` |
| 1168 | 1168_geral | 8 | 44 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1FEE98` |
| 1169 | 1169_geral | 8 | 43 | 1 | 0.00 | Dead or Alive 2 (USA) `8C114758` |
| 1170 | 1170_geral | 8 | 41 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C074E6C` |
| 1171 | 1171_geral | 8 | 40 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08DF22` |
| 1172 | 1172_geral | 8 | 40 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C20DF4C` |
| 1173 | 1173_geral | 8 | 39 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05E11C` |
| 1174 | 1174_geral | 8 | 39 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C179648` |
| 1175 | 1175_geral | 8 | 38 | 2 | 0.00 | Dead or Alive 2 (USA) `8C120314` |
| 1176 | 1176_geral | 8 | 35 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C072338` |
| 1177 | 1177_preenche | 8 | 35 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08B438` |
| 1178 | 1178_geral | 8 | 34 | 1 | 0.00 | Dead or Alive 2 (USA) `8C114B88` |
| 1179 | 1179_geral | 8 | 34 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C072544` |
| 1180 | 1180_geral | 8 | 33 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D27A4` |
| 1181 | 1181_geral | 8 | 32 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089A20` |
| 1182 | 1182_geral | 8 | 30 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08F158` |
| 1183 | 1183_geral | 8 | 30 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09E53C` |
| 1184 | 1184_preenche | 8 | 29 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06E982` |
| 1185 | 1185_pref | 8 | 29 | 3 | 0.00 | Grandia II (USA) `8C06DCE0` |
| 1186 | 1186_geral | 8 | 29 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08DEA4` |
| 1187 | 1187_geral | 8 | 29 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C178AE8` |
| 1188 | 1188_copia | 8 | 27 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C05D992` |
| 1189 | 1189_geral | 8 | 27 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071BFA` |
| 1190 | 1190_geral | 8 | 25 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05E182` |
| 1191 | 1191_copia | 8 | 25 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08D8EE` |
| 1192 | 1192_geral | 8 | 25 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C074042` |
| 1193 | 1193_copia | 8 | 24 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08CE56` |
| 1194 | 1194_geral | 8 | 24 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07439C` |
| 1195 | 1195_geral | 8 | 23 | 1 | 0.00 | Evolution - The World of Sac `8C1A5E9C` |
| 1196 | 1196_geral | 8 | 22 | 1 | 0.00 | Dead or Alive 2 (USA) `8C114688` |
| 1197 | 1197_geral | 8 | 22 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08E08E` |
| 1198 | 1198_geral | 8 | 22 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C074332` |
| 1199 | 1199_geral | 8 | 22 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C084C80` |
| 1200 | 1200_geral | 8 | 22 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08C66C` |
| 1201 | 1201_geral | 8 | 22 | 3 | 0.00 | Grandia II (USA) `8C06DE16` |
| 1202 | 1202_geral | 8 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20D22C` |
| 1203 | 1203_geral | 8 | 21 | 1 | 0.00 | Dead or Alive 2 (USA) `8C11F5D8` |
| 1204 | 1204_geral | 8 | 21 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C07171A` |
| 1205 | 1205_geral | 8 | 21 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C073FF8` |
| 1206 | 1206_geral | 8 | 21 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C070FC4` |
| 1207 | 1207_geral | 8 | 21 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08CA94` |
| 1208 | 1208_geral | 8 | 21 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A2D00` |
| 1209 | 1209_geral | 8 | 21 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D2628` |
| 1210 | 1210_geral | 8 | 20 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C073B9C` |
| 1211 | 1211_geral | 8 | 20 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1ADBA8` |
| 1212 | 1212_geral | 8 | 19 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08CE0C` |
| 1213 | 1213_geral | 8 | 18 | 2 | 0.00 | Evolution - The World of Sac `8C1AC142` |
| 1214 | 1214_geral | 8 | 18 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08C74C` |
| 1215 | 1215_geral | 8 | 18 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08DFF0` |
| 1216 | 1216_geral | 8 | 18 | 1 | 0.00 | Evolution - The World of Sac `8C1F7734` |
| 1217 | 1217_preenche | 8 | 18 | 1 | 0.00 | Grandia II (USA) `8C023FC0` |
| 1218 | 1218_geral | 8 | 17 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06EAA0` |
| 1219 | 1219_geral | 8 | 17 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D2544` |
| 1220 | 1220_geral | 8 | 17 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D256C` |
| 1221 | 1221_geral | 8 | 17 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D2600` |
| 1222 | 1222_geral | 8 | 17 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D2658` |
| 1223 | 1223_geral | 8 | 16 | 1 | 0.00 | Dead or Alive 2 (USA) `8C11F276` |
| 1224 | 1224_float | 8 | 16 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09E660` |
| 1225 | 1225_float | 8 | 16 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09E680` |
| 1226 | 1226_geral | 8 | 16 | 1 | 0.00 | Evolution - The World of Sac `8C1B8B5A` |
| 1227 | 1227_geral | 8 | 16 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C20CCBC` |
| 1228 | 1228_geral | 8 | 15 | 1 | 0.00 | Dead or Alive 2 (USA) `8C11F5BA` |
| 1229 | 1229_geral | 8 | 15 | 1 | 0.00 | Dead or Alive 2 (USA) `8C11F606` |
| 1230 | 1230_geral | 8 | 14 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071D66` |
| 1231 | 1231_geral | 8 | 13 | 1 | 0.00 | Dead or Alive 2 (USA) `8C11F2A0` |
| 1232 | 1232_geral | 8 | 13 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071184` |
| 1233 | 1233_geral | 8 | 13 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08C7F4` |
| 1234 | 1234_geral | 8 | 13 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09E302` |
| 1235 | 1235_geral | 8 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20C5FC` |
| 1236 | 1236_geral | 8 | 11 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05CEAC` |
| 1237 | 1237_geral | 8 | 11 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08C71C` |
| 1238 | 1238_geral | 8 | 10 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08C7CC` |
| 1239 | 1239_geral | 8 | 9 | 1 | 0.00 | Dead or Alive 2 (USA) `8C116F50` |
| 1240 | 1240_geral | 8 | 9 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071B90` |
| 1241 | 1241_geral | 8 | 9 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C070FEE` |
| 1242 | 1242_geral | 8 | 9 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0739A0` |
| 1243 | 1243_copia | 8 | 9 | 1 | 0.00 | Evolution - The World of Sac `8C1F5BEA` |
| 1244 | 1244_geral | 8 | 8 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08CE46` |
| 1245 | 1245_geral | 8 | 8 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07188C` |
| 1246 | 1246_geral | 8 | 8 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08CA84` |
| 1247 | 1247_geral | 8 | 8 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08F27E` |
| 1248 | 1248_pref | 7 | 651 | 7 | 0.00 | Phantasy Star Online Ver. 2  `8C3B6C4A` |
| 1249 | 1249_copia | 7 | 391 | 2 | 0.00 | Shenmue II (Europe) (En,Fr,D `8C041028` |
| 1250 | 1250_pref | 7 | 331 | 7 | 0.00 | Phantasy Star Online Ver. 2  `8C3B5ACE` |
| 1251 | 1251_divisao_pref | 7 | 261 | 4 | 0.00 | King of Fighters The - Evolu `8C326124` |
| 1252 | 1252_copia | 7 | 188 | 2 | 0.00 | King of Fighters The - Evolu `8C363C20` |
| 1253 | 1253_copia | 7 | 164 | 4 | 0.00 | Grandia II (USA) `8C026286` |
| 1254 | 1254_divisao | 7 | 160 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C066F1C` |
| 1255 | 1255_geral | 7 | 139 | 6 | 0.00 | Power Stone (USA) `0C053434` |
| 1256 | 1256_copia | 7 | 135 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C085132` |
| 1257 | 1257_pref | 7 | 128 | 2 | 0.00 | Evolution - The World of Sac `8C1C833C` |
| 1258 | 1258_copia | 7 | 126 | 4 | 0.00 | Dead or Alive 2 (USA) `8C122B12` |
| 1259 | 1259_copia | 7 | 122 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C203442` |
| 1260 | 1260_copia | 7 | 114 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C3B73EA` |
| 1261 | 1261_copia | 7 | 107 | 4 | 0.00 | Resident Evil - Code - Veron `8C1B64D6` |
| 1262 | 1262_copia | 7 | 105 | 1 | 0.00 | King of Fighters The - Evolu `8C36366C` |
| 1263 | 1263_geral | 7 | 95 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C090D30` |
| 1264 | 1264_matriz_produto_escalar_divisao | 7 | 93 | 4 | 0.00 | Evolution 2 - Far Off Promis `8C197594` |
| 1265 | 1265_divisao | 7 | 91 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C191EC8` |
| 1266 | 1266_copia | 7 | 90 | 3 | 0.00 | Evolution - The World of Sac `8C1D482E` |
| 1267 | 1267_copia | 7 | 90 | 1 | 0.00 | King of Fighters The - Evolu `8C3637B0` |
| 1268 | 1268_copia | 7 | 89 | 2 | 0.00 | Evolution - The World of Sac `8C1A7E96` |
| 1269 | 1269_divisao | 7 | 88 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05D4C0` |
| 1270 | 1270_geral | 7 | 88 | 1 | 0.00 | Evolution - The World of Sac `8C1D6FDC` |
| 1271 | 1271_geral | 7 | 86 | 2 | 0.00 | King of Fighters The - Evolu `8C330834` |
| 1272 | 1272_copia | 7 | 84 | 6 | 0.00 | Evolution 2 - Far Off Promis `8C1777FE` |
| 1273 | 1273_switch | 7 | 80 | 1 | 0.00 | Evolution - The World of Sac `8C1A3064` |
| 1274 | 1274_geral | 7 | 74 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C05448A` |
| 1275 | 1275_matriz | 7 | 74 | 2 | 0.00 | Evolution - The World of Sac `8C1BD7F0` |
| 1276 | 1276_divisao | 7 | 73 | 1 | 0.00 | Evolution - The World of Sac `8C1AD5DC` |
| 1277 | 1277_geral | 7 | 73 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1A3950` |
| 1278 | 1278_geral | 7 | 71 | 1 | 0.00 | Grandia II (USA) `8C069900` |
| 1279 | 1279_copia | 7 | 71 | 1 | 0.00 | King of Fighters The - Evolu `8C310CAC` |
| 1280 | 1280_copia | 7 | 69 | 3 | 0.00 | Power Stone (USA) `0C0F1C72` |
| 1281 | 1281_geral | 7 | 65 | 4 | 0.00 | Phantasy Star Online Ver. 2  `8C38BA5C` |
| 1282 | 1282_geral | 7 | 64 | 1 | 0.00 | King of Fighters The - Evolu `8C310EE4` |
| 1283 | 1283_copia | 7 | 60 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1FF288` |
| 1284 | 1284_copia | 7 | 57 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C202E78` |
| 1285 | 1285_copia | 7 | 56 | 1 | 0.00 | Dead or Alive 2 (USA) `8C114DEE` |
| 1286 | 1286_geral | 7 | 56 | 1 | 0.00 | King of Fighters The - Evolu `8C312C14` |
| 1287 | 1287_copia | 7 | 55 | 1 | 0.00 | Dead or Alive 2 (USA) `8C114238` |
| 1288 | 1288_geral | 7 | 54 | 1 | 0.00 | Evolution - The World of Sac `8C1D4E3A` |
| 1289 | 1289_geral | 7 | 52 | 4 | 0.00 | Evolution - The World of Sac `8C1A34CE` |
| 1290 | 1290_geral | 7 | 52 | 1 | 0.00 | King of Fighters The - Evolu `8C330340` |
| 1291 | 1291_copia | 7 | 51 | 2 | 0.00 | Grandia II (USA) `8C0416C0` |
| 1292 | 1292_geral | 7 | 49 | 1 | 0.00 | King of Fighters The - Evolu `8C3302D0` |
| 1293 | 1293_geral | 7 | 48 | 2 | 0.00 | Grandia II (USA) `8C06F9BC` |
| 1294 | 1294_geral | 7 | 48 | 3 | 0.00 | Skies of Arcadia (USA) (Disc `8C2A65A4` |
| 1295 | 1295_pref | 7 | 47 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C186074` |
| 1296 | 1296_geral | 7 | 46 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05D570` |
| 1297 | 1297_geral | 7 | 46 | 2 | 0.00 | Resident Evil - Code - Veron `8C19D4B8` |
| 1298 | 1298_geral | 7 | 45 | 1 | 0.00 | Evolution - The World of Sac `8C15D7D8` |
| 1299 | 1299_geral | 7 | 43 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C073B7E` |
| 1300 | 1300_geral | 7 | 43 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C125204` |
| 1301 | 1301_geral | 7 | 40 | 1 | 0.00 | Evolution - The World of Sac `8C1D4EC0` |
| 1302 | 1302_copia | 7 | 39 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C090AF0` |
| 1303 | 1303_geral | 7 | 38 | 4 | 0.00 | Evolution - The World of Sac `8C1D49FA` |
| 1304 | 1304_geral | 7 | 37 | 2 | 0.00 | Evolution - The World of Sac `8C1AD69C` |
| 1305 | 1305_pref | 7 | 37 | 1 | 0.00 | Grandia II (USA) `8C03C704` |
| 1306 | 1306_geral | 7 | 36 | 2 | 0.00 | Grandia II (USA) `8C0845A0` |
| 1307 | 1307_geral | 7 | 32 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A3D50` |
| 1308 | 1308_geral | 7 | 28 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1FFE3C` |
| 1309 | 1309_geral | 7 | 28 | 4 | 0.00 | Grandia II (USA) `8C03DADA` |
| 1310 | 1310_geral | 7 | 28 | 1 | 0.00 | King of Fighters The - Evolu `8C330704` |
| 1311 | 1311_geral | 7 | 27 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C073C40` |
| 1312 | 1312_geral | 7 | 27 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08ED92` |
| 1313 | 1313_geral | 7 | 27 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08EDD2` |
| 1314 | 1314_geral | 7 | 27 | 1 | 0.00 | Evolution - The World of Sac `8C1A3146` |
| 1315 | 1315_geral | 7 | 27 | 1 | 0.00 | Evolution - The World of Sac `8C1D4944` |
| 1316 | 1316_geral | 7 | 26 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09107C` |
| 1317 | 1317_geral | 7 | 26 | 1 | 0.00 | Evolution - The World of Sac `8C1A344C` |
| 1318 | 1318_geral | 7 | 25 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071900` |
| 1319 | 1319_geral | 7 | 24 | 1 | 0.00 | Dead or Alive 2 (USA) `8C120850` |
| 1320 | 1320_geral | 7 | 22 | 1 | 0.00 | Evolution - The World of Sac `8C1F7700` |
| 1321 | 1321_geral | 7 | 22 | 1 | 0.00 | Grandia II (USA) `8C075EE0` |
| 1322 | 1322_geral | 7 | 21 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0743D8` |
| 1323 | 1323_geral | 7 | 17 | 1 | 0.00 | Evolution - The World of Sac `8C1A33CC` |
| 1324 | 1324_float | 7 | 16 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C066EFC` |
| 1325 | 1325_geral | 7 | 16 | 3 | 0.00 | Evolution - The World of Sac `8C180040` |
| 1326 | 1326_produto_escalar | 7 | 16 | 1 | 0.00 | Evolution - The World of Sac `8C198838` |
| 1327 | 1327_geral | 7 | 14 | 1 | 0.00 | Evolution - The World of Sac `8C1D4992` |
| 1328 | 1328_geral | 7 | 13 | 1 | 0.00 | Evolution - The World of Sac `8C1AD024` |
| 1329 | 1329_geral | 7 | 12 | 1 | 0.00 | Dead or Alive 2 (USA) `8C13E6EE` |
| 1330 | 1330_geral | 7 | 12 | 1 | 0.00 | Evolution - The World of Sac `8C1A3434` |
| 1331 | 1331_geral | 7 | 10 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05D4A4` |
| 1332 | 1332_geral | 7 | 10 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C11B638` |
| 1333 | 1333_geral | 7 | 9 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DCAA` |
| 1334 | 1334_geral | 7 | 9 | 1 | 0.00 | Evolution - The World of Sac `8C1F0046` |
| 1335 | 1335_geral | 7 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F9BB4` |
| 1336 | 1336_geral | 7 | 8 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C02492E` |
| 1337 | 1337_float | 6 | 649 | 8 | 0.00 | Dead or Alive 2 (USA) `8C02DA1C` |
| 1338 | 1338_divisao | 6 | 572 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C067198` |
| 1339 | 1339_pref | 6 | 491 | 10 | 0.00 | Phantasy Star Online Ver. 2  `8C3AF694` |
| 1340 | 1340_copia | 6 | 395 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C057B2C` |
| 1341 | 1341_geral | 6 | 291 | 3 | 0.00 | Project Justice (USA) `0C026FB6` |
| 1342 | 1342_copia | 6 | 274 | 1 | 0.00 | King of Fighters The - Evolu `8C3A1DE4` |
| 1343 | 1343_preenche | 6 | 253 | 2 | 0.00 | Grandia II (USA) `8C0FC4E0` |
| 1344 | 1344_copia | 6 | 198 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C207014` |
| 1345 | 1345_raiz | 6 | 194 | 2 | 0.00 | Skies of Arcadia (USA) (Disc `8C2960C0` |
| 1346 | 1346_preenche | 6 | 182 | 2 | 0.00 | Grandia II (USA) `8C0FC726` |
| 1347 | 1347_divisao | 6 | 177 | 4 | 0.00 | Phantasy Star Online Ver. 2  `8C364280` |
| 1348 | 1348_copia | 6 | 157 | 3 | 0.00 | Grandia II (USA) `8C0F6D6C` |
| 1349 | 1349_copia | 6 | 151 | 3 | 0.00 | Phantasy Star Online Ver. 2  `8C3C0E92` |
| 1350 | 1350_copia | 6 | 146 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FAEBC` |
| 1351 | 1351_copia | 6 | 139 | 3 | 0.00 | Grandia II (USA) `8C0860D4` |
| 1352 | 1352_float | 6 | 136 | 1 | 0.00 | Grandia II (USA) `8C078738` |
| 1353 | 1353_geral | 6 | 133 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C030238` |
| 1354 | 1354_divisao | 6 | 130 | 3 | 0.00 | Resident Evil - Code - Veron `8C19A488` |
| 1355 | 1355_copia | 6 | 126 | 2 | 0.00 | Grandia II (USA) `8C0F5E12` |
| 1356 | 1356_copia | 6 | 120 | 1 | 0.00 | Grandia II (USA) `8C070E0E` |
| 1357 | 1357_geral | 6 | 119 | 4 | 0.00 | Macross M3 `8C1F6D38` |
| 1358 | 1358_copia | 6 | 115 | 2 | 0.00 | King of Fighters The - Evolu `8C3633F8` |
| 1359 | 1359_geral | 6 | 109 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0602B4` |
| 1360 | 1360_geral | 6 | 106 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C05D48E` |
| 1361 | 1361_geral | 6 | 94 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0600BE` |
| 1362 | 1362_geral | 6 | 93 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C064294` |
| 1363 | 1363_copia | 6 | 89 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C06C2B2` |
| 1364 | 1364_geral | 6 | 88 | 3 | 0.00 | Grandia II (USA) `8C0C2BF8` |
| 1365 | 1365_geral | 6 | 86 | 2 | 0.00 | Grandia II (USA) `8C07062C` |
| 1366 | 1366_copia | 6 | 76 | 2 | 0.00 | King of Fighters The - Evolu `8C34C97A` |
| 1367 | 1367_geral | 6 | 75 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C05E43C` |
| 1368 | 1368_copia | 6 | 75 | 1 | 0.00 | King of Fighters The - Evolu `8C3B5C0C` |
| 1369 | 1369_geral | 6 | 74 | 5 | 0.00 | Skies of Arcadia (USA) (Disc `8C2DBEA0` |
| 1370 | 1370_copia | 6 | 74 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C20CD4C` |
| 1371 | 1371_geral | 6 | 72 | 1 | 0.00 | Grandia II (USA) `8C0C1C98` |
| 1372 | 1372_geral | 6 | 71 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C05CE40` |
| 1373 | 1373_geral | 6 | 71 | 2 | 0.00 | King of Fighters The - Evolu `8C3122F0` |
| 1374 | 1374_copia | 6 | 68 | 2 | 0.00 | Dead or Alive 2 (USA) `8C129452` |
| 1375 | 1375_pref | 6 | 67 | 3 | 0.00 | Phantasy Star Online Ver. 2  `8C397E62` |
| 1376 | 1376_geral | 6 | 63 | 4 | 0.00 | Shenmue II (Europe) (En,Fr,D `8C03F410` |
| 1377 | 1377_copia | 6 | 62 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20C062` |
| 1378 | 1378_copia | 6 | 62 | 2 | 0.00 | Evolution - The World of Sac `8C1A64D6` |
| 1379 | 1379_pref | 6 | 62 | 3 | 0.00 | Grandia II (USA) `8C055058` |
| 1380 | 1380_geral | 6 | 62 | 2 | 0.00 | Grandia II (USA) `8C06CABC` |
| 1381 | 1381_copia | 6 | 62 | 1 | 0.00 | King of Fighters The - Evolu `8C3B7010` |
| 1382 | 1382_copia | 6 | 60 | 2 | 0.00 | Grandia II (USA) `8C07AACA` |
| 1383 | 1383_geral | 6 | 60 | 2 | 0.00 | Grandia II (USA) `8C0FD088` |
| 1384 | 1384_geral | 6 | 60 | 1 | 0.00 | King of Fighters The - Evolu `8C36C098` |
| 1385 | 1385_copia | 6 | 59 | 3 | 0.00 | Napple Tale - Arsia in Daydr `8C138B74` |
| 1386 | 1386_geral | 6 | 57 | 1 | 0.00 | Grandia II (USA) `8C0FD85E` |
| 1387 | 1387_geral | 6 | 55 | 1 | 0.00 | Grandia II (USA) `8C06EDCE` |
| 1388 | 1388_geral | 6 | 54 | 1 | 0.00 | Grandia II (USA) `8C071038` |
| 1389 | 1389_pref | 6 | 51 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C185F78` |
| 1390 | 1390_copia | 6 | 49 | 1 | 0.00 | Grandia II (USA) `8C03453E` |
| 1391 | 1391_geral | 6 | 47 | 3 | 0.00 | Phantasy Star Online Ver. 2  `8C397DF4` |
| 1392 | 1392_copia | 6 | 45 | 1 | 0.00 | Dead or Alive 2 (USA) `8C1201E8` |
| 1393 | 1393_geral | 6 | 45 | 4 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0146B0` |
| 1394 | 1394_matriz_produto_escalar | 6 | 44 | 2 | 0.00 | Evolution - The World of Sac `8C197C80` |
| 1395 | 1395_geral | 6 | 44 | 1 | 0.00 | Macross M3 `8C1EC2D8` |
| 1396 | 1396_geral | 6 | 43 | 3 | 0.00 | King of Fighters The - Evolu `8C31664C` |
| 1397 | 1397_geral | 6 | 41 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C179280` |
| 1398 | 1398_geral | 6 | 41 | 1 | 0.00 | Grandia II (USA) `8C06ED7C` |
| 1399 | 1399_geral | 6 | 41 | 1 | 0.00 | Grandia II (USA) `8C0FCF60` |
| 1400 | 1400_geral | 6 | 40 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C202FDC` |
| 1401 | 1401_geral | 6 | 40 | 2 | 0.00 | Grandia II (USA) `8C025370` |
| 1402 | 1402_geral | 6 | 40 | 1 | 0.00 | Grandia II (USA) `8C06F7CC` |
| 1403 | 1403_preenche | 6 | 40 | 2 | 0.00 | Grandia II (USA) `8C07A94A` |
| 1404 | 1404_geral | 6 | 39 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C177CC4` |
| 1405 | 1405_geral | 6 | 39 | 3 | 0.00 | Evolution - The World of Sac `8C199568` |
| 1406 | 1406_copia | 6 | 39 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C05E490` |
| 1407 | 1407_geral | 6 | 37 | 2 | 0.00 | Grandia II (USA) `8C0BFCB6` |
| 1408 | 1408_geral | 6 | 35 | 1 | 0.00 | Grandia II (USA) `8C0FD420` |
| 1409 | 1409_geral | 6 | 34 | 1 | 0.00 | Dead or Alive 2 (USA) `8C11F190` |
| 1410 | 1410_geral | 6 | 34 | 2 | 0.00 | Grandia II (USA) `8C03DDA2` |
| 1411 | 1411_geral | 6 | 34 | 2 | 0.00 | Grandia II (USA) `8C077C8A` |
| 1412 | 1412_geral | 6 | 34 | 4 | 0.00 | Grandia II (USA) `8C03BD64` |
| 1413 | 1413_geral | 6 | 33 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C178FB8` |
| 1414 | 1414_copia | 6 | 33 | 2 | 0.00 | Grandia II (USA) `8C077EA6` |
| 1415 | 1415_geral | 6 | 33 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C05D36C` |
| 1416 | 1416_copia | 6 | 32 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C011374` |
| 1417 | 1417_geral | 6 | 32 | 1 | 0.00 | Grandia II (USA) `8C07147E` |
| 1418 | 1418_geral | 6 | 32 | 1 | 0.00 | Grandia II (USA) `8C0FBB74` |
| 1419 | 1419_geral | 6 | 32 | 1 | 0.00 | Grandia II (USA) `8C0FD8DA` |
| 1420 | 1420_geral | 6 | 31 | 2 | 0.00 | Grandia II (USA) `8C0707E0` |
| 1421 | 1421_geral | 6 | 31 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1884D8` |
| 1422 | 1422_divisao | 6 | 30 | 1 | 0.00 | Grandia II (USA) `8C078848` |
| 1423 | 1423_geral | 6 | 30 | 1 | 0.00 | Grandia II (USA) `8C0FD818` |
| 1424 | 1424_geral | 6 | 29 | 3 | 0.00 | Skies of Arcadia (USA) (Disc `8C011818` |
| 1425 | 1425_geral | 6 | 29 | 3 | 0.00 | Grandia II (USA) `8C06FEA4` |
| 1426 | 1426_geral | 6 | 29 | 1 | 0.00 | Grandia II (USA) `8C0FD100` |
| 1427 | 1427_geral | 6 | 29 | 1 | 0.00 | Grandia II (USA) `8C0FD6B4` |
| 1428 | 1428_geral | 6 | 28 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0741E4` |
| 1429 | 1429_geral | 6 | 28 | 1 | 0.00 | Grandia II (USA) `8C0777DA` |
| 1430 | 1430_copia | 6 | 28 | 1 | 0.00 | Grandia II (USA) `8C077CE8` |
| 1431 | 1431_geral | 6 | 28 | 1 | 0.00 | Grandia II (USA) `8C0FB7E2` |
| 1432 | 1432_geral | 6 | 26 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C203EB4` |
| 1433 | 1433_geral | 6 | 25 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07193A` |
| 1434 | 1434_laco | 6 | 25 | 1 | 0.00 | Grandia II (USA) `8C0FD1C8` |
| 1435 | 1435_geral | 6 | 25 | 1 | 0.00 | Grandia II (USA) `8C0FD270` |
| 1436 | 1436_geral | 6 | 25 | 1 | 0.00 | Grandia II (USA) `8C070CC4` |
| 1437 | 1437_geral | 6 | 25 | 1 | 0.00 | Grandia II (USA) `8C0BF976` |
| 1438 | 1438_copia | 6 | 24 | 1 | 0.00 | Evolution - The World of Sac `8C1F0058` |
| 1439 | 1439_copia | 6 | 24 | 1 | 0.00 | Grandia II (USA) `8C07A9B8` |
| 1440 | 1440_copia | 6 | 24 | 1 | 0.00 | Grandia II (USA) `8C08688C` |
| 1441 | 1441_geral | 6 | 23 | 1 | 0.00 | Evolution - The World of Sac `8C1995CC` |
| 1442 | 1442_geral | 6 | 23 | 1 | 0.00 | Grandia II (USA) `8C0C29A0` |
| 1443 | 1443_geral | 6 | 23 | 1 | 0.00 | Grandia II (USA) `8C0FD2B4` |
| 1444 | 1444_geral | 6 | 22 | 1 | 0.00 | Grandia II (USA) `8C070C2C` |
| 1445 | 1445_geral | 6 | 22 | 1 | 0.00 | Grandia II (USA) `8C0FD3EC` |
| 1446 | 1446_geral | 6 | 21 | 3 | 0.00 | Phantasy Star Online Ver. 2  `8C23E7FC` |
| 1447 | 1447_copia | 6 | 21 | 1 | 0.00 | Grandia II (USA) `8C0FCFFA` |
| 1448 | 1448_geral | 6 | 21 | 1 | 0.00 | Grandia II (USA) `8C078152` |
| 1449 | 1449_geral | 6 | 21 | 1 | 0.00 | Grandia II (USA) `8C0FCF3A` |
| 1450 | 1450_geral | 6 | 20 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C178C6C` |
| 1451 | 1451_geral | 6 | 20 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C20EAAC` |
| 1452 | 1452_geral | 6 | 20 | 1 | 0.00 | Grandia II (USA) `8C070D20` |
| 1453 | 1453_geral | 6 | 20 | 1 | 0.00 | Grandia II (USA) `8C0FD352` |
| 1454 | 1454_geral | 6 | 20 | 1 | 0.00 | Grandia II (USA) `8C0FD4C6` |
| 1455 | 1455_geral | 6 | 20 | 1 | 0.00 | Grandia II (USA) `8C0FD4F6` |
| 1456 | 1456_copia | 6 | 19 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07D480` |
| 1457 | 1457_copia | 6 | 19 | 1 | 0.00 | Evolution - The World of Sac `8C1EFFC8` |
| 1458 | 1458_geral | 6 | 19 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C20F060` |
| 1459 | 1459_geral | 6 | 19 | 1 | 0.00 | Grandia II (USA) `8C06FEE6` |
| 1460 | 1460_geral | 6 | 19 | 2 | 0.00 | Grandia II (USA) `8C0FD02E` |
| 1461 | 1461_geral | 6 | 19 | 1 | 0.00 | Grandia II (USA) `8C0716AC` |
| 1462 | 1462_geral | 6 | 19 | 1 | 0.00 | Grandia II (USA) `8C0BF944` |
| 1463 | 1463_geral | 6 | 19 | 1 | 0.00 | Grandia II (USA) `8C0FD324` |
| 1464 | 1464_geral | 6 | 19 | 1 | 0.00 | Grandia II (USA) `8C0FD382` |
| 1465 | 1465_geral | 6 | 19 | 1 | 0.00 | Grandia II (USA) `8C0FD572` |
| 1466 | 1466_geral | 6 | 18 | 1 | 0.00 | Grandia II (USA) `8C070EFE` |
| 1467 | 1467_geral | 6 | 18 | 1 | 0.00 | Grandia II (USA) `8C070DAC` |
| 1468 | 1468_geral | 6 | 17 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C090E50` |
| 1469 | 1469_geral | 6 | 16 | 1 | 0.00 | Dead or Alive 2 (USA) `8C11F558` |
| 1470 | 1470_float | 6 | 16 | 1 | 0.00 | Evolution - The World of Sac `8C199484` |
| 1471 | 1471_geral | 6 | 16 | 1 | 0.00 | Grandia II (USA) `8C0868DA` |
| 1472 | 1472_geral | 6 | 16 | 1 | 0.00 | Grandia II (USA) `8C0B6760` |
| 1473 | 1473_geral | 6 | 15 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0704A4` |
| 1474 | 1474_geral | 6 | 15 | 1 | 0.00 | Grandia II (USA) `8C06EE3C` |
| 1475 | 1475_geral | 6 | 14 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C177BF4` |
| 1476 | 1476_geral | 6 | 13 | 1 | 0.00 | Grandia II (USA) `8C0C4748` |
| 1477 | 1477_geral | 6 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C177AA0` |
| 1478 | 1478_geral | 6 | 12 | 1 | 0.00 | Evolution - The World of Sac `8C11110C` |
| 1479 | 1479_pref | 6 | 12 | 1 | 0.00 | Evolution - The World of Sac `8C1A47FA` |
| 1480 | 1480_geral | 6 | 12 | 1 | 0.00 | Grandia II (USA) `8C0FD05C` |
| 1481 | 1481_geral | 6 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C208E58` |
| 1482 | 1482_geral | 6 | 11 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071688` |
| 1483 | 1483_geral | 6 | 11 | 1 | 0.00 | Grandia II (USA) `8C06E2EE` |
| 1484 | 1484_float | 6 | 10 | 1 | 0.00 | Grandia II (USA) `8C0698E8` |
| 1485 | 1485_geral | 6 | 9 | 1 | 0.00 | Evolution - The World of Sac `8C1A5BA8` |
| 1486 | 1486_geral | 6 | 9 | 1 | 0.00 | Grandia II (USA) `8C070DDC` |
| 1487 | 1487_geral | 6 | 9 | 1 | 0.00 | Grandia II (USA) `8C0FC4CE` |
| 1488 | 1488_geral | 6 | 9 | 1 | 0.00 | Grandia II (USA) `8C0FCFBE` |
| 1489 | 1489_switch | 6 | 8 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C066E8C` |
| 1490 | 1490_geral | 6 | 8 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09E590` |
| 1491 | 1491_produto_escalar_raiz | 6 | 8 | 1 | 0.00 | Evolution - The World of Sac `8C1986BC` |
| 1492 | 1492_geral | 6 | 8 | 1 | 0.00 | Grandia II (USA) `8C035836` |
| 1493 | 1493_divisao | 5 | 982 | 3 | 0.00 | Skies of Arcadia (USA) (Disc `8C27C590` |
| 1494 | 1494_divisao_pref | 5 | 532 | 1 | 0.00 | Evolution - The World of Sac `8C1DEEC0` |
| 1495 | 1495_divisao | 5 | 382 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0535C4` |
| 1496 | 1496_geral | 5 | 354 | 2 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C190C00` |
| 1497 | 1497_copia | 5 | 326 | 3 | 0.00 | Dead or Alive 2 (USA) `8C12A46A` |
| 1498 | 1498_divisao | 5 | 322 | 3 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21C2A0` |
| 1499 | 1499_pref | 5 | 305 | 1 | 0.00 | Evolution - The World of Sac `8C1C8F6C` |
| 1500 | 1500_copia | 5 | 295 | 6 | 0.00 | Dead or Alive 2 (USA) `8C131600` |
| 1501 | 1501_geral | 5 | 239 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C218E20` |
| 1502 | 1502_copia | 5 | 222 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C078C80` |
| 1503 | 1503_copia | 5 | 207 | 2 | 0.00 | Grandia II (USA) `8C076F98` |
| 1504 | 1504_copia | 5 | 204 | 4 | 0.00 | Evolution 2 - Far Off Promis `8C179858` |
| 1505 | 1505_geral | 5 | 195 | 3 | 0.00 | Phantasy Star Online Ver. 2  `8C392BF4` |
| 1506 | 1506_copia | 5 | 187 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C219A00` |
| 1507 | 1507_copia | 5 | 186 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C226BC0` |
| 1508 | 1508_copia | 5 | 174 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C213AC0` |
| 1509 | 1509_float | 5 | 172 | 3 | 0.00 | Skies of Arcadia (USA) (Disc `8C2B9228` |
| 1510 | 1510_geral | 5 | 169 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C217D40` |
| 1511 | 1511_copia | 5 | 169 | 2 | 0.00 | Skies of Arcadia (USA) (Disc `8C252444` |
| 1512 | 1512_copia | 5 | 163 | 5 | 0.00 | Grandia II (USA) `8C0779BC` |
| 1513 | 1513_geral | 5 | 160 | 2 | 0.00 | Grandia II (USA) `8C0C1284` |
| 1514 | 1514_geral | 5 | 159 | 3 | 0.00 | Shenmue II (Europe) (En,Fr,D `8C04491E` |
| 1515 | 1515_copia | 5 | 157 | 2 | 0.00 | Dead or Alive 2 (USA) `8C12FB20` |
| 1516 | 1516_divisao | 5 | 151 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C05A7E0` |
| 1517 | 1517_geral | 5 | 142 | 8 | 0.00 | Le Mans 24 Hours (Europe) (E `8C07A2E0` |
| 1518 | 1518_geral | 5 | 138 | 2 | 0.00 | Grandia II (USA) `8C0283D2` |
| 1519 | 1519_copia | 5 | 133 | 3 | 0.00 | Soulcalibur (USA) `8C22F254` |
| 1520 | 1520_copia | 5 | 132 | 3 | 0.00 | Phantasy Star Online Ver. 2  `8C355150` |
| 1521 | 1521_geral | 5 | 131 | 1 | 0.00 | Grandia II (USA) `8C0C6B18` |
| 1522 | 1522_geral | 5 | 124 | 5 | 0.00 | Le Mans 24 Hours (Europe) (E `8C05FDC0` |
| 1523 | 1523_copia | 5 | 116 | 1 | 0.00 | Grandia II (USA) `8C06FB80` |
| 1524 | 1524_copia | 5 | 115 | 4 | 0.00 | Phantasy Star Online Ver. 2  `8C38A5FC` |
| 1525 | 1525_copia | 5 | 110 | 1 | 0.00 | Evolution - The World of Sac `8C1EE530` |
| 1526 | 1526_geral | 5 | 108 | 2 | 0.00 | King of Fighters The - Evolu `8C36A978` |
| 1527 | 1527_copia | 5 | 105 | 1 | 0.00 | Macross M3 `8C1EE9A4` |
| 1528 | 1528_copia | 5 | 101 | 1 | 0.00 | Macross M3 `8C1EECDC` |
| 1529 | 1529_copia | 5 | 101 | 2 | 0.00 | Macross M3 `8C1EF060` |
| 1530 | 1530_copia | 5 | 94 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C23B5CE` |
| 1531 | 1531_geral | 5 | 89 | 4 | 0.00 | Le Mans 24 Hours (Europe) (E `8C07222E` |
| 1532 | 1532_geral | 5 | 89 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C216C40` |
| 1533 | 1533_copia | 5 | 87 | 1 | 0.00 | King of Fighters The - Evolu `8C310D70` |
| 1534 | 1534_geral | 5 | 83 | 2 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C18E460` |
| 1535 | 1535_geral | 5 | 81 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FD0A0` |
| 1536 | 1536_copia | 5 | 80 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21CA6C` |
| 1537 | 1537_geral | 5 | 80 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07908C` |
| 1538 | 1538_geral | 5 | 79 | 1 | 0.00 | Grandia II (USA) `8C027682` |
| 1539 | 1539_geral | 5 | 78 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C10F1E6` |
| 1540 | 1540_geral | 5 | 77 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C216A60` |
| 1541 | 1541_geral | 5 | 77 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C218120` |
| 1542 | 1542_copia | 5 | 75 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2198C0` |
| 1543 | 1543_copia | 5 | 75 | 2 | 0.00 | King of Fighters The - Evolu `8C320E4A` |
| 1544 | 1544_copia | 5 | 72 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C215D76` |
| 1545 | 1545_geral | 5 | 72 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C178ED0` |
| 1546 | 1546_geral | 5 | 72 | 2 | 0.00 | Grandia II (USA) `8C0FDC58` |
| 1547 | 1547_geral | 5 | 71 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21C920` |
| 1548 | 1548_geral | 5 | 70 | 2 | 0.00 | Grandia II (USA) `8C0719A4` |
| 1549 | 1549_copia | 5 | 70 | 2 | 0.00 | King of Fighters The - Evolu `8C33619A` |
| 1550 | 1550_geral | 5 | 68 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C16806E` |
| 1551 | 1551_divisao | 5 | 67 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C173330` |
| 1552 | 1552_geral | 5 | 67 | 1 | 0.00 | Grandia II (USA) `8C0780CC` |
| 1553 | 1553_geral | 5 | 64 | 1 | 0.00 | Grandia II (USA) `8C06F5A4` |
| 1554 | 1554_geral | 5 | 64 | 1 | 0.00 | Grandia II (USA) `8C027754` |
| 1555 | 1555_geral | 5 | 63 | 2 | 0.00 | Grandia II (USA) `8C077BF4` |
| 1556 | 1556_geral | 5 | 62 | 3 | 0.00 | Phantasy Star Online Ver. 2  `8C35A4C0` |
| 1557 | 1557_copia | 5 | 62 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C217020` |
| 1558 | 1558_geral | 5 | 60 | 5 | 0.00 | Le Mans 24 Hours (Europe) (E `8C074B00` |
| 1559 | 1559_copia | 5 | 60 | 2 | 0.00 | Grandia II (USA) `8C077D66` |
| 1560 | 1560_copia | 5 | 59 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C219380` |
| 1561 | 1561_geral | 5 | 58 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C22F900` |
| 1562 | 1562_copia | 5 | 58 | 1 | 0.00 | Grandia II (USA) `8C06DF9C` |
| 1563 | 1563_geral | 5 | 57 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C233A20` |
| 1564 | 1564_geral | 5 | 56 | 1 | 0.00 | Evolution - The World of Sac `8C163406` |
| 1565 | 1565_geral | 5 | 56 | 3 | 0.00 | King of Fighters The - Evolu `8C335BC8` |
| 1566 | 1566_copia | 5 | 56 | 2 | 0.00 | King of Fighters The - Evolu `8C3345E4` |
| 1567 | 1567_geral | 5 | 55 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C217100` |
| 1568 | 1568_geral | 5 | 55 | 1 | 0.00 | Grandia II (USA) `8C1407FC` |
| 1569 | 1569_copia | 5 | 55 | 1 | 0.00 | King of Fighters The - Evolu `8C3A1808` |
| 1570 | 1570_float | 5 | 54 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C171EC0` |
| 1571 | 1571_geral | 5 | 54 | 4 | 0.00 | Napple Tale - Arsia in Daydr `8C142EBE` |
| 1572 | 1572_geral | 5 | 53 | 2 | 0.00 | Resident Evil - Code - Veron `8C17DAD0` |
| 1573 | 1573_geral | 5 | 52 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FBDE0` |
| 1574 | 1574_copia | 5 | 50 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C216BB2` |
| 1575 | 1575_geral | 5 | 49 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C16784E` |
| 1576 | 1576_copia | 5 | 49 | 1 | 0.00 | Grandia II (USA) `8C07A8B8` |
| 1577 | 1577_geral | 5 | 48 | 3 | 0.00 | Dead or Alive 2 (USA) `8C130CC0` |
| 1578 | 1578_geral | 5 | 48 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C10F2C2` |
| 1579 | 1579_copia | 5 | 47 | 1 | 0.00 | Grandia II (USA) `8C0C1DAE` |
| 1580 | 1580_divisao | 5 | 47 | 1 | 0.00 | King of Fighters The - Evolu `8C3B5B9C` |
| 1581 | 1581_geral | 5 | 46 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21C880` |
| 1582 | 1582_geral | 5 | 46 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A38A4` |
| 1583 | 1583_geral | 5 | 46 | 3 | 0.00 | Napple Tale - Arsia in Daydr `8C162554` |
| 1584 | 1584_copia | 5 | 45 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21F07E` |
| 1585 | 1585_geral | 5 | 45 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C215FC8` |
| 1586 | 1586_geral | 5 | 45 | 2 | 0.00 | Grandia II (USA) `8C06DAE2` |
| 1587 | 1587_geral | 5 | 44 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C213400` |
| 1588 | 1588_geral | 5 | 44 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C233140` |
| 1589 | 1589_geral | 5 | 44 | 2 | 0.00 | Grandia II (USA) `8C06E5BC` |
| 1590 | 1590_copia | 5 | 43 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C233CA2` |
| 1591 | 1591_geral | 5 | 43 | 1 | 0.00 | Grandia II (USA) `8C071074` |
| 1592 | 1592_geral | 5 | 43 | 1 | 0.00 | Grandia II (USA) `8C0C0524` |
| 1593 | 1593_copia | 5 | 42 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21ED8C` |
| 1594 | 1594_geral | 5 | 42 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FA9D8` |
| 1595 | 1595_geral | 5 | 42 | 1 | 0.00 | Grandia II (USA) `8C06DF16` |
| 1596 | 1596_geral | 5 | 42 | 1 | 0.00 | Grandia II (USA) `8C0772C8` |
| 1597 | 1597_pref | 5 | 42 | 1 | 0.00 | Grandia II (USA) `8C05549A` |
| 1598 | 1598_geral | 5 | 42 | 2 | 0.00 | Grandia II (USA) `8C0C27E6` |
| 1599 | 1599_geral | 5 | 41 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21DAEC` |
| 1600 | 1600_geral | 5 | 41 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21E0EE` |
| 1601 | 1601_geral | 5 | 41 | 2 | 0.00 | Soulcalibur (USA) `8C223FA2` |
| 1602 | 1602_geral | 5 | 41 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C35263C` |
| 1603 | 1603_copia | 5 | 40 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C1320E0` |
| 1604 | 1604_geral | 5 | 40 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C081A80` |
| 1605 | 1605_geral | 5 | 40 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A3B4C` |
| 1606 | 1606_geral | 5 | 40 | 1 | 0.00 | Grandia II (USA) `8C07175E` |
| 1607 | 1607_geral | 5 | 39 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1A52F6` |
| 1608 | 1608_geral | 5 | 39 | 3 | 0.00 | Evolution 2 - Far Off Promis `8C1A381C` |
| 1609 | 1609_geral | 5 | 38 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21CA20` |
| 1610 | 1610_geral | 5 | 38 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2330C0` |
| 1611 | 1611_geral | 5 | 38 | 1 | 0.00 | Grandia II (USA) `8C028804` |
| 1612 | 1612_geral | 5 | 37 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16FEF0` |
| 1613 | 1613_geral | 5 | 37 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C234B00` |
| 1614 | 1614_geral | 5 | 36 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C213380` |
| 1615 | 1615_geral | 5 | 36 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2169E0` |
| 1616 | 1616_geral | 5 | 36 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21E148` |
| 1617 | 1617_produto_escalar_raiz | 5 | 36 | 1 | 0.00 | Evolution - The World of Sac `8C1980CC` |
| 1618 | 1618_geral | 5 | 36 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A3C50` |
| 1619 | 1619_geral | 5 | 36 | 2 | 0.00 | Grandia II (USA) `8C0C510A` |
| 1620 | 1620_geral | 5 | 35 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C172A20` |
| 1621 | 1621_laco | 5 | 35 | 2 | 0.00 | Dead or Alive 2 (USA) `8C1326B0` |
| 1622 | 1622_switch | 5 | 35 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2337C0` |
| 1623 | 1623_geral | 5 | 35 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C234060` |
| 1624 | 1624_geral | 5 | 35 | 4 | 0.00 | Skies of Arcadia (USA) (Disc `8C251F20` |
| 1625 | 1625_geral | 5 | 35 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A3BD8` |
| 1626 | 1626_divisao | 5 | 35 | 1 | 0.00 | Grandia II (USA) `8C077794` |
| 1627 | 1627_copia | 5 | 35 | 1 | 0.00 | Grandia II (USA) `8C0C1ED2` |
| 1628 | 1628_geral | 5 | 34 | 4 | 0.00 | Le Mans 24 Hours (Europe) (E `8C231F4C` |
| 1629 | 1629_geral | 5 | 34 | 3 | 0.00 | Resident Evil - Code - Veron `8C1D810A` |
| 1630 | 1630_geral | 5 | 33 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C37EBF2` |
| 1631 | 1631_copia | 5 | 32 | 2 | 0.00 | Dead or Alive 2 (USA) `8C1332F4` |
| 1632 | 1632_geral | 5 | 32 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C233BFC` |
| 1633 | 1633_preenche | 5 | 31 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2197E0` |
| 1634 | 1634_geral | 5 | 31 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C35F984` |
| 1635 | 1635_geral | 5 | 31 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0676D8` |
| 1636 | 1636_geral | 5 | 31 | 1 | 0.00 | Grandia II (USA) `8C0C28BA` |
| 1637 | 1637_geral | 5 | 31 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C133874` |
| 1638 | 1638_geral | 5 | 30 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21DCBA` |
| 1639 | 1639_divisao | 5 | 30 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C17BD20` |
| 1640 | 1640_geral | 5 | 30 | 3 | 0.00 | Dead or Alive 2 (USA) `8C11B740` |
| 1641 | 1641_geral | 5 | 30 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A3654` |
| 1642 | 1642_geral | 5 | 30 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A3750` |
| 1643 | 1643_geral | 5 | 29 | 1 | 0.00 | Evolution - The World of Sac `8C15D794` |
| 1644 | 1644_geral | 5 | 29 | 1 | 0.00 | Grandia II (USA) `8C0FD6F8` |
| 1645 | 1645_geral | 5 | 29 | 1 | 0.00 | Grandia II (USA) `8C0FD746` |
| 1646 | 1646_copia | 5 | 28 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C219140` |
| 1647 | 1647_geral | 5 | 28 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F9C84` |
| 1648 | 1648_geral | 5 | 28 | 1 | 0.00 | Evolution - The World of Sac `8C1652FC` |
| 1649 | 1649_geral | 5 | 28 | 3 | 0.00 | Evolution - The World of Sac `8C1A33EE` |
| 1650 | 1650_geral | 5 | 28 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C3CD668` |
| 1651 | 1651_geral | 5 | 27 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21C804` |
| 1652 | 1652_float | 5 | 27 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C176060` |
| 1653 | 1653_geral | 5 | 27 | 1 | 0.00 | Grandia II (USA) `8C06CD34` |
| 1654 | 1654_geral | 5 | 27 | 1 | 0.00 | Grandia II (USA) `8C0717BE` |
| 1655 | 1655_geral | 5 | 27 | 1 | 0.00 | Grandia II (USA) `8C0FD794` |
| 1656 | 1656_geral | 5 | 26 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C218220` |
| 1657 | 1657_geral | 5 | 26 | 1 | 0.00 | Grandia II (USA) `8C06DB4C` |
| 1658 | 1658_geral | 5 | 26 | 2 | 0.00 | Grandia II (USA) `8C0C287C` |
| 1659 | 1659_geral | 5 | 26 | 1 | 0.00 | Grandia II (USA) `8C0C2AD2` |
| 1660 | 1660_geral | 5 | 26 | 1 | 0.00 | Grandia II (USA) `8C06FE58` |
| 1661 | 1661_geral | 5 | 25 | 2 | 0.00 | Grandia II (USA) `8C027986` |
| 1662 | 1662_geral | 5 | 25 | 2 | 0.00 | King of Fighters The - Evolu `8C33DF14` |
| 1663 | 1663_geral | 5 | 25 | 1 | 0.00 | King of Fighters The - Evolu `8C33466E` |
| 1664 | 1664_geral | 5 | 24 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F5084` |
| 1665 | 1665_geral | 5 | 24 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C215C80` |
| 1666 | 1666_preenche | 5 | 24 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C216D68` |
| 1667 | 1667_switch | 5 | 24 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C218280` |
| 1668 | 1668_geral | 5 | 24 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2336A0` |
| 1669 | 1669_geral | 5 | 24 | 4 | 0.00 | Resident Evil - Code - Veron `8C17D172` |
| 1670 | 1670_geral | 5 | 24 | 1 | 0.00 | King of Fighters The - Evolu `8C311374` |
| 1671 | 1671_laco | 5 | 23 | 2 | 0.00 | Dead or Alive 2 (USA) `8C12FAC4` |
| 1672 | 1672_geral | 5 | 23 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21DA96` |
| 1673 | 1673_geral | 5 | 23 | 3 | 0.00 | Grandia II (USA) `8C028B78` |
| 1674 | 1674_geral | 5 | 23 | 1 | 0.00 | Grandia II (USA) `8C06CC0C` |
| 1675 | 1675_geral | 5 | 23 | 1 | 0.00 | Grandia II (USA) `8C0C05BA` |
| 1676 | 1676_preenche | 5 | 22 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16B900` |
| 1677 | 1677_geral | 5 | 22 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C2298A0` |
| 1678 | 1678_geral | 5 | 22 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C218080` |
| 1679 | 1679_geral | 5 | 22 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21F31A` |
| 1680 | 1680_geral | 5 | 22 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C233E20` |
| 1681 | 1681_geral | 5 | 22 | 1 | 0.00 | King of Fighters The - Evolu `8C3A6854` |
| 1682 | 1682_geral | 5 | 22 | 1 | 0.00 | King of Fighters The - Evolu `8C3A6C44` |
| 1683 | 1683_geral | 5 | 21 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FEDF6` |
| 1684 | 1684_geral | 5 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C201D1A` |
| 1685 | 1685_geral | 5 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2136C0` |
| 1686 | 1686_laco | 5 | 21 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21F048` |
| 1687 | 1687_geral | 5 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21F12C` |
| 1688 | 1688_geral | 5 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C201D98` |
| 1689 | 1689_geral | 5 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C219078` |
| 1690 | 1690_geral | 5 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21F19E` |
| 1691 | 1691_geral | 5 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21F1D8` |
| 1692 | 1692_geral | 5 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21F212` |
| 1693 | 1693_geral | 5 | 21 | 2 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C1A7940` |
| 1694 | 1694_geral | 5 | 21 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C061694` |
| 1695 | 1695_geral | 5 | 21 | 1 | 0.00 | Grandia II (USA) `8C0710F2` |
| 1696 | 1696_geral | 5 | 20 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21D530` |
| 1697 | 1697_geral | 5 | 20 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21E7D6` |
| 1698 | 1698_geral | 5 | 20 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C234120` |
| 1699 | 1699_geral | 5 | 20 | 1 | 0.00 | Grandia II (USA) `8C0710CA` |
| 1700 | 1700_geral | 5 | 20 | 1 | 0.00 | Grandia II (USA) `8C027E2A` |
| 1701 | 1701_geral | 5 | 20 | 1 | 0.00 | Grandia II (USA) `8C06D2CE` |
| 1702 | 1702_geral | 5 | 20 | 1 | 0.00 | Grandia II (USA) `8C06E41A` |
| 1703 | 1703_geral | 5 | 20 | 1 | 0.00 | Grandia II (USA) `8C0FD466` |
| 1704 | 1704_geral | 5 | 20 | 1 | 0.00 | Grandia II (USA) `8C0FD496` |
| 1705 | 1705_laco | 5 | 19 | 2 | 0.00 | Dead or Alive 2 (USA) `8C133364` |
| 1706 | 1706_geral | 5 | 19 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16B8D0` |
| 1707 | 1707_geral | 5 | 19 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21E8CE` |
| 1708 | 1708_geral | 5 | 19 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C078B0C` |
| 1709 | 1709_copia | 5 | 19 | 1 | 0.00 | Evolution - The World of Sac `8C1F0020` |
| 1710 | 1710_geral | 5 | 19 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A37C8` |
| 1711 | 1711_geral | 5 | 19 | 1 | 0.00 | Grandia II (USA) `8C06CBAC` |
| 1712 | 1712_geral | 5 | 19 | 1 | 0.00 | Grandia II (USA) `8C0C2CD6` |
| 1713 | 1713_laco | 5 | 19 | 1 | 0.00 | Grandia II (USA) `8C0C49A4` |
| 1714 | 1714_geral | 5 | 19 | 1 | 0.00 | Grandia II (USA) `8C0FD5CE` |
| 1715 | 1715_geral | 5 | 19 | 3 | 0.00 | King of Fighters The - Evolu `8C31D11A` |
| 1716 | 1716_copia | 5 | 18 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16C120` |
| 1717 | 1717_geral | 5 | 18 | 2 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C18DB20` |
| 1718 | 1718_geral | 5 | 18 | 1 | 0.00 | Dead or Alive 2 (USA) `8C102166` |
| 1719 | 1719_geral | 5 | 18 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1813C6` |
| 1720 | 1720_geral | 5 | 18 | 1 | 0.00 | Grandia II (USA) `8C029410` |
| 1721 | 1721_geral | 5 | 18 | 2 | 0.00 | King of Fighters The - Evolu `8C33A444` |
| 1722 | 1722_geral | 5 | 18 | 1 | 0.00 | Grandia II (USA) `8C0FDBE2` |
| 1723 | 1723_geral | 5 | 18 | 1 | 0.00 | Grandia II (USA) `8C06D6C8` |
| 1724 | 1724_geral | 5 | 18 | 1 | 0.00 | Grandia II (USA) `8C07082A` |
| 1725 | 1725_geral | 5 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C219838` |
| 1726 | 1726_geral | 5 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FBB40` |
| 1727 | 1727_geral | 5 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C200720` |
| 1728 | 1728_geral | 5 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C215D00` |
| 1729 | 1729_geral | 5 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21DA74` |
| 1730 | 1730_geral | 5 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21F16C` |
| 1731 | 1731_geral | 5 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2341C0` |
| 1732 | 1732_geral | 5 | 17 | 1 | 0.00 | Dead or Alive 2 (USA) `8C113D04` |
| 1733 | 1733_float | 5 | 17 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06A564` |
| 1734 | 1734_geral | 5 | 17 | 1 | 0.00 | Grandia II (USA) `8C070704` |
| 1735 | 1735_geral | 5 | 17 | 1 | 0.00 | Grandia II (USA) `8C0C285A` |
| 1736 | 1736_geral | 5 | 16 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C172AC0` |
| 1737 | 1737_geral | 5 | 16 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C201DCC` |
| 1738 | 1738_geral | 5 | 16 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C234400` |
| 1739 | 1739_geral | 5 | 16 | 1 | 0.00 | Grandia II (USA) `8C06F624` |
| 1740 | 1740_geral | 5 | 16 | 1 | 0.00 | Grandia II (USA) `8C0FD68C` |
| 1741 | 1741_geral | 5 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21DB84` |
| 1742 | 1742_geral | 5 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FCC30` |
| 1743 | 1743_geral | 5 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C217FA0` |
| 1744 | 1744_geral | 5 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21E880` |
| 1745 | 1745_geral | 5 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C233540` |
| 1746 | 1746_geral | 5 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C233C3E` |
| 1747 | 1747_geral | 5 | 15 | 1 | 0.00 | Dead or Alive 2 (USA) `8C11E912` |
| 1748 | 1748_geral | 5 | 15 | 1 | 0.00 | Dead or Alive 2 (USA) `8C102172` |
| 1749 | 1749_geral | 5 | 15 | 1 | 0.00 | Grandia II (USA) `8C0C0AAE` |
| 1750 | 1750_geral | 5 | 15 | 1 | 0.00 | Grandia II (USA) `8C0286C8` |
| 1751 | 1751_geral | 5 | 15 | 1 | 0.00 | Macross M3 `8C1F0BA8` |
| 1752 | 1752_laco | 5 | 14 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21EAD6` |
| 1753 | 1753_geral | 5 | 14 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16BA80` |
| 1754 | 1754_laco | 5 | 14 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16F960` |
| 1755 | 1755_geral | 5 | 14 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21832E` |
| 1756 | 1756_geral | 5 | 14 | 1 | 0.00 | Grandia II (USA) `8C06E0C4` |
| 1757 | 1757_geral | 5 | 14 | 1 | 0.00 | Grandia II (USA) `8C0286F8` |
| 1758 | 1758_geral | 5 | 14 | 1 | 0.00 | King of Fighters The - Evolu `8C365386` |
| 1759 | 1759_geral | 5 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21DAC4` |
| 1760 | 1760_geral | 5 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16C7D0` |
| 1761 | 1761_geral | 5 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2180E0` |
| 1762 | 1762_geral | 5 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21981E` |
| 1763 | 1763_geral | 5 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C233F00` |
| 1764 | 1764_divisao | 5 | 13 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06A6D0` |
| 1765 | 1765_float | 5 | 13 | 1 | 0.00 | Evolution - The World of Sac `8C197C60` |
| 1766 | 1766_geral | 5 | 13 | 1 | 0.00 | Grandia II (USA) `8C0C2CB4` |
| 1767 | 1767_geral | 5 | 13 | 1 | 0.00 | King of Fighters The - Evolu `8C332000` |
| 1768 | 1768_laco | 5 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C233BD8` |
| 1769 | 1769_geral | 5 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16B9A0` |
| 1770 | 1770_matriz | 5 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16FA90` |
| 1771 | 1771_geral | 5 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C243700` |
| 1772 | 1772_geral | 5 | 12 | 1 | 0.00 | Dead or Alive 2 (USA) `8C121732` |
| 1773 | 1773_geral | 5 | 12 | 1 | 0.00 | Grandia II (USA) `8C06EF4C` |
| 1774 | 1774_geral | 5 | 12 | 1 | 0.00 | Grandia II (USA) `8C077206` |
| 1775 | 1775_geral | 5 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21E8B8` |
| 1776 | 1776_geral | 5 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C200780` |
| 1777 | 1777_laco | 5 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C215D60` |
| 1778 | 1778_geral | 5 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2185A6` |
| 1779 | 1779_geral | 5 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C219178` |
| 1780 | 1780_geral | 5 | 11 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C172468` |
| 1781 | 1781_geral | 5 | 11 | 1 | 0.00 | Grandia II (USA) `8C06FE16` |
| 1782 | 1782_geral | 5 | 11 | 1 | 0.00 | Grandia II (USA) `8C0C2920` |
| 1783 | 1783_geral | 5 | 11 | 1 | 0.00 | Grandia II (USA) `8C028F60` |
| 1784 | 1784_geral | 5 | 10 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C216D98` |
| 1785 | 1785_geral | 5 | 10 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2170A2` |
| 1786 | 1786_geral | 5 | 10 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21910C` |
| 1787 | 1787_geral | 5 | 10 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21CB4C` |
| 1788 | 1788_geral | 5 | 10 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C233DD4` |
| 1789 | 1789_geral | 5 | 10 | 1 | 0.00 | Grandia II (USA) `8C0C2846` |
| 1790 | 1790_geral | 5 | 10 | 1 | 0.00 | Grandia II (USA) `8C029802` |
| 1791 | 1791_geral | 5 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FB980` |
| 1792 | 1792_geral | 5 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FCC80` |
| 1793 | 1793_geral | 5 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FCE40` |
| 1794 | 1794_geral | 5 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FCE80` |
| 1795 | 1795_geral | 5 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2192B2` |
| 1796 | 1796_geral | 5 | 9 | 1 | 0.00 | Grandia II (USA) `8C029884` |
| 1797 | 1797_geral | 5 | 9 | 1 | 0.00 | Grandia II (USA) `8C06E182` |
| 1798 | 1798_geral | 5 | 9 | 1 | 0.00 | Grandia II (USA) `8C0C2ED8` |
| 1799 | 1799_geral | 5 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FCD00` |
| 1800 | 1800_geral | 5 | 8 | 1 | 0.00 | Grandia II (USA) `8C0C27CA` |
| 1801 | 1801_divisao_pref | 4 | 573 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1FC2BC` |
| 1802 | 1802_copia | 4 | 305 | 1 | 0.00 | Grandia II (USA) `8C02723C` |
| 1803 | 1803_copia | 4 | 212 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C080348` |
| 1804 | 1804_copia | 4 | 208 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C384CC4` |
| 1805 | 1805_copia | 4 | 179 | 2 | 0.00 | Dead or Alive 2 (USA) `8C12F43E` |
| 1806 | 1806_copia | 4 | 178 | 1 | 0.00 | Macross M3 `8C1E79E8` |
| 1807 | 1807_geral | 4 | 169 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16C500` |
| 1808 | 1808_copia | 4 | 164 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C380A26` |
| 1809 | 1809_geral | 4 | 152 | 2 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C124530` |
| 1810 | 1810_copia | 4 | 151 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05C540` |
| 1811 | 1811_copia | 4 | 144 | 2 | 0.00 | Evolution - The World of Sac `8C1F7B68` |
| 1812 | 1812_pref | 4 | 138 | 2 | 0.00 | Grandia II (USA) `8C066F58` |
| 1813 | 1813_divisao | 4 | 135 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16CC60` |
| 1814 | 1814_geral | 4 | 134 | 4 | 0.00 | Evolution - The World of Sac `8C1CB59C` |
| 1815 | 1815_raiz | 4 | 134 | 1 | 0.00 | Evolution - The World of Sac `8C1A26C4` |
| 1816 | 1816_preenche | 4 | 129 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C067B32` |
| 1817 | 1817_float | 4 | 122 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FF340` |
| 1818 | 1818_copia | 4 | 119 | 3 | 0.00 | Skies of Arcadia (USA) (Disc `8C259312` |
| 1819 | 1819_copia | 4 | 118 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C384750` |
| 1820 | 1820_copia | 4 | 117 | 2 | 0.00 | Resident Evil - Code - Veron `8C1B7D64` |
| 1821 | 1821_copia | 4 | 116 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C381EBE` |
| 1822 | 1822_geral | 4 | 110 | 4 | 0.00 | Napple Tale - Arsia in Daydr `8C12142C` |
| 1823 | 1823_copia | 4 | 106 | 1 | 0.00 | King of Fighters The - Evolu `8C312A9C` |
| 1824 | 1824_copia | 4 | 102 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C10C8DC` |
| 1825 | 1825_copia | 4 | 101 | 1 | 0.00 | Macross M3 `8C1EEE54` |
| 1826 | 1826_geral | 4 | 99 | 4 | 0.00 | Skies of Arcadia (USA) (Disc `8C28D446` |
| 1827 | 1827_geral | 4 | 96 | 2 | 0.00 | Evolution - The World of Sac `8C15EC20` |
| 1828 | 1828_geral | 4 | 96 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06C7FC` |
| 1829 | 1829_copia | 4 | 94 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05B66C` |
| 1830 | 1830_geral | 4 | 92 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C3B947A` |
| 1831 | 1831_copia | 4 | 92 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C166D26` |
| 1832 | 1832_divisao | 4 | 88 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C146DB8` |
| 1833 | 1833_copia | 4 | 84 | 2 | 0.00 | Evolution - The World of Sac `8C1FE540` |
| 1834 | 1834_copia | 4 | 82 | 1 | 0.00 | Macross M3 `8C1EEBD8` |
| 1835 | 1835_copia | 4 | 81 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C172730` |
| 1836 | 1836_copia | 4 | 80 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C383A5A` |
| 1837 | 1837_copia | 4 | 79 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C173D20` |
| 1838 | 1838_copia | 4 | 79 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089B20` |
| 1839 | 1839_geral | 4 | 79 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3822EA` |
| 1840 | 1840_geral | 4 | 78 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16D070` |
| 1841 | 1841_geral | 4 | 78 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06C110` |
| 1842 | 1842_geral | 4 | 78 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089488` |
| 1843 | 1843_copia | 4 | 78 | 3 | 0.00 | King of Fighters The - Evolu `8C111C00` |
| 1844 | 1844_geral | 4 | 77 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16C390` |
| 1845 | 1845_geral | 4 | 75 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16C430` |
| 1846 | 1846_divisao | 4 | 74 | 3 | 0.00 | Evolution 2 - Far Off Promis `8C1893D0` |
| 1847 | 1847_copia | 4 | 74 | 1 | 0.00 | Macross M3 `8C1EB498` |
| 1848 | 1848_float | 4 | 72 | 2 | 0.00 | Evolution - The World of Sac `8C1C2530` |
| 1849 | 1849_copia | 4 | 72 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C1629F8` |
| 1850 | 1850_copia | 4 | 72 | 4 | 0.00 | Napple Tale - Arsia in Daydr `8C166E94` |
| 1851 | 1851_copia | 4 | 71 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16B460` |
| 1852 | 1852_geral | 4 | 71 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06A814` |
| 1853 | 1853_geral | 4 | 71 | 1 | 0.00 | Grandia II (USA) `8C06F92E` |
| 1854 | 1854_copia | 4 | 71 | 1 | 0.00 | Macross M3 `8C1EEAF4` |
| 1855 | 1855_geral | 4 | 70 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DFE6` |
| 1856 | 1856_copia | 4 | 70 | 3 | 0.00 | Evolution - The World of Sac `8C1A757E` |
| 1857 | 1857_geral | 4 | 69 | 2 | 0.00 | Grandia II (USA) `8C024188` |
| 1858 | 1858_copia | 4 | 68 | 2 | 0.00 | Power Stone (USA) `0C0DFE20` |
| 1859 | 1859_copia | 4 | 68 | 4 | 0.00 | Evolution - The World of Sac `8C1AF5A0` |
| 1860 | 1860_geral | 4 | 67 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C206E28` |
| 1861 | 1861_copia | 4 | 67 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06C73C` |
| 1862 | 1862_pref | 4 | 67 | 1 | 0.00 | Evolution - The World of Sac `8C1B8368` |
| 1863 | 1863_geral | 4 | 67 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C384310` |
| 1864 | 1864_geral | 4 | 64 | 2 | 0.00 | Evolution - The World of Sac `8C1AEFE8` |
| 1865 | 1865_geral | 4 | 64 | 1 | 0.00 | Grandia II (USA) `8C07AB92` |
| 1866 | 1866_copia | 4 | 63 | 2 | 0.00 | Dead or Alive 2 (USA) `8C11F06A` |
| 1867 | 1867_pref | 4 | 63 | 1 | 0.00 | Evolution - The World of Sac `8C1B8420` |
| 1868 | 1868_geral | 4 | 62 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05C778` |
| 1869 | 1869_geral | 4 | 62 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C05DAE4` |
| 1870 | 1870_copia | 4 | 62 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1D405C` |
| 1871 | 1871_copia | 4 | 62 | 1 | 0.00 | Grandia II (USA) `8C077E18` |
| 1872 | 1872_copia | 4 | 62 | 1 | 0.00 | Grandia II (USA) `8C0788C0` |
| 1873 | 1873_geral | 4 | 61 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16CA30` |
| 1874 | 1874_copia | 4 | 61 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05C844` |
| 1875 | 1875_copia | 4 | 58 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089220` |
| 1876 | 1876_copia | 4 | 58 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08974C` |
| 1877 | 1877_copia | 4 | 58 | 1 | 0.00 | King of Fighters The - Evolu `8C31AF8E` |
| 1878 | 1878_copia | 4 | 57 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0896AC` |
| 1879 | 1879_copia | 4 | 56 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C174C80` |
| 1880 | 1880_geral | 4 | 56 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C177C18` |
| 1881 | 1881_copia | 4 | 56 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C095AA0` |
| 1882 | 1882_matriz_produto_escalar | 4 | 56 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C11B160` |
| 1883 | 1883_geral | 4 | 55 | 1 | 0.00 | King of Fighters The - Evolu `8C33E5B4` |
| 1884 | 1884_copia | 4 | 55 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C383DF0` |
| 1885 | 1885_geral | 4 | 53 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C174C10` |
| 1886 | 1886_copia | 4 | 53 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C06B340` |
| 1887 | 1887_geral | 4 | 51 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06C924` |
| 1888 | 1888_geral | 4 | 51 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C384640` |
| 1889 | 1889_copia | 4 | 50 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16B9D0` |
| 1890 | 1890_geral | 4 | 50 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21ED22` |
| 1891 | 1891_geral | 4 | 50 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FC9AC` |
| 1892 | 1892_geral | 4 | 48 | 1 | 0.00 | Grandia II (USA) `8C0C1C0C` |
| 1893 | 1893_geral | 4 | 48 | 1 | 0.00 | Macross M3 `8C1E7A20` |
| 1894 | 1894_copia | 4 | 48 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C38404E` |
| 1895 | 1895_geral | 4 | 47 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089404` |
| 1896 | 1896_geral | 4 | 46 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09C2A4` |
| 1897 | 1897_geral | 4 | 44 | 1 | 0.00 | Grandia II (USA) `8C040584` |
| 1898 | 1898_geral | 4 | 43 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21F356` |
| 1899 | 1899_geral | 4 | 43 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06C314` |
| 1900 | 1900_geral | 4 | 42 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06CCB4` |
| 1901 | 1901_geral | 4 | 42 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C383F04` |
| 1902 | 1902_geral | 4 | 40 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1728E0` |
| 1903 | 1903_copia | 4 | 40 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21EBFE` |
| 1904 | 1904_geral | 4 | 40 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C05C36C` |
| 1905 | 1905_geral | 4 | 40 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C17165A` |
| 1906 | 1906_copia | 4 | 40 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C383FA4` |
| 1907 | 1907_copia | 4 | 39 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16B500` |
| 1908 | 1908_laco | 4 | 39 | 2 | 0.00 | Dead or Alive 2 (USA) `8C132734` |
| 1909 | 1909_copia | 4 | 38 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C177ABE` |
| 1910 | 1910_geral | 4 | 38 | 2 | 0.00 | Soulcalibur (USA) `8C21B1B4` |
| 1911 | 1911_geral | 4 | 38 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0802E8` |
| 1912 | 1912_geral | 4 | 37 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F46B8` |
| 1913 | 1913_copia | 4 | 37 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05B2C0` |
| 1914 | 1914_copia | 4 | 37 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C066744` |
| 1915 | 1915_geral | 4 | 36 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16B930` |
| 1916 | 1916_geral | 4 | 36 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16D1F0` |
| 1917 | 1917_geral | 4 | 36 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C1677F0` |
| 1918 | 1918_float | 4 | 35 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16F670` |
| 1919 | 1919_copia | 4 | 35 | 2 | 0.00 | Grandia II (USA) `8C07CE5C` |
| 1920 | 1920_geral | 4 | 35 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05D94C` |
| 1921 | 1921_pref | 4 | 35 | 1 | 0.00 | Grandia II (USA) `8C03D20C` |
| 1922 | 1922_geral | 4 | 34 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FE5A0` |
| 1923 | 1923_copia | 4 | 34 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08932C` |
| 1924 | 1924_geral | 4 | 34 | 4 | 0.00 | King of Fighters The - Evolu `8C3207EE` |
| 1925 | 1925_geral | 4 | 33 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089390` |
| 1926 | 1926_copia | 4 | 32 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C174070` |
| 1927 | 1927_matriz_divisao | 4 | 32 | 1 | 0.00 | Evolution - The World of Sac `8C198158` |
| 1928 | 1928_geral | 4 | 32 | 1 | 0.00 | Grandia II (USA) `8C070388` |
| 1929 | 1929_geral | 4 | 31 | 1 | 0.00 | Macross M3 `8C1E4F2C` |
| 1930 | 1930_geral | 4 | 30 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05B340` |
| 1931 | 1931_geral | 4 | 30 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C21D988` |
| 1932 | 1932_geral | 4 | 29 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21EF2A` |
| 1933 | 1933_preenche | 4 | 29 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16BE20` |
| 1934 | 1934_geral | 4 | 28 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16FEB0` |
| 1935 | 1935_geral | 4 | 28 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21EEAE` |
| 1936 | 1936_float | 4 | 28 | 1 | 0.00 | Evolution - The World of Sac `8C198114` |
| 1937 | 1937_geral | 4 | 28 | 1 | 0.00 | Evolution - The World of Sac `8C1D4B54` |
| 1938 | 1938_geral | 4 | 28 | 1 | 0.00 | Grandia II (USA) `8C0773F0` |
| 1939 | 1939_geral | 4 | 28 | 1 | 0.00 | Grandia II (USA) `8C077430` |
| 1940 | 1940_copia | 4 | 27 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21EDF8` |
| 1941 | 1941_copia | 4 | 27 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C172E10` |
| 1942 | 1942_geral | 4 | 27 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0643B2` |
| 1943 | 1943_copia | 4 | 27 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06CAB0` |
| 1944 | 1944_geral | 4 | 27 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C384008` |
| 1945 | 1945_geral | 4 | 26 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06B560` |
| 1946 | 1946_geral | 4 | 25 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06BB60` |
| 1947 | 1947_geral | 4 | 25 | 2 | 0.00 | Evolution - The World of Sac `8C1B377E` |
| 1948 | 1948_preenche | 4 | 24 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16F4B0` |
| 1949 | 1949_geral | 4 | 24 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06E076` |
| 1950 | 1950_geral | 4 | 24 | 1 | 0.00 | Grandia II (USA) `8C041726` |
| 1951 | 1951_geral | 4 | 23 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C200600` |
| 1952 | 1952_geral | 4 | 23 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2019D0` |
| 1953 | 1953_geral | 4 | 23 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C064102` |
| 1954 | 1954_float | 4 | 23 | 1 | 0.00 | Evolution - The World of Sac `8C1A3D3C` |
| 1955 | 1955_geral | 4 | 23 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C37E852` |
| 1956 | 1956_geral | 4 | 22 | 2 | 0.00 | Grandia II (USA) `8C0287C0` |
| 1957 | 1957_geral | 4 | 22 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3840B2` |
| 1958 | 1958_geral | 4 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FBB68` |
| 1959 | 1959_copia | 4 | 20 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06A494` |
| 1960 | 1960_geral | 4 | 20 | 1 | 0.00 | Grandia II (USA) `8C028374` |
| 1961 | 1961_geral | 4 | 20 | 1 | 0.00 | Grandia II (USA) `8C0773C8` |
| 1962 | 1962_laco | 4 | 20 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C383D28` |
| 1963 | 1963_laco | 4 | 19 | 1 | 0.00 | Dead or Alive 2 (USA) `8C1228F0` |
| 1964 | 1964_geral | 4 | 19 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05BA20` |
| 1965 | 1965_geral | 4 | 18 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16BA50` |
| 1966 | 1966_geral | 4 | 18 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C206C44` |
| 1967 | 1967_geral | 4 | 18 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21ECC2` |
| 1968 | 1968_geral | 4 | 18 | 1 | 0.00 | Dead or Alive 2 (USA) `8C10216E` |
| 1969 | 1969_geral | 4 | 18 | 1 | 0.00 | Evolution - The World of Sac `8C1D49CA` |
| 1970 | 1970_geral | 4 | 18 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C17B292` |
| 1971 | 1971_geral | 4 | 18 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3842B4` |
| 1972 | 1972_geral | 4 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16EF30` |
| 1973 | 1973_geral | 4 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16CAC0` |
| 1974 | 1974_geral | 4 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16CB80` |
| 1975 | 1975_geral | 4 | 17 | 1 | 0.00 | Dead or Alive 2 (USA) `8C102162` |
| 1976 | 1976_geral | 4 | 17 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06C9F2` |
| 1977 | 1977_geral | 4 | 16 | 2 | 0.00 | Dead or Alive 2 (USA) `8C105FA0` |
| 1978 | 1978_geral | 4 | 16 | 2 | 0.00 | Dead or Alive 2 (USA) `8C1309CE` |
| 1979 | 1979_geral | 4 | 16 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0A07BC` |
| 1980 | 1980_geral | 4 | 16 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3803DE` |
| 1981 | 1981_float | 4 | 16 | 2 | 0.00 | Power Stone (USA) `0C0E21B0` |
| 1982 | 1982_geral | 4 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FCBE0` |
| 1983 | 1983_geral | 4 | 15 | 1 | 0.00 | Dead or Alive 2 (USA) `8C10216A` |
| 1984 | 1984_geral | 4 | 15 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0629CA` |
| 1985 | 1985_geral | 4 | 15 | 1 | 0.00 | Grandia II (USA) `8C03DD72` |
| 1986 | 1986_laco | 4 | 15 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C380C04` |
| 1987 | 1987_geral | 4 | 14 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C208F66` |
| 1988 | 1988_geral | 4 | 14 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C070414` |
| 1989 | 1989_pref | 4 | 14 | 1 | 0.00 | Grandia II (USA) `8C03D298` |
| 1990 | 1990_geral | 4 | 14 | 1 | 0.00 | King of Fighters The - Evolu `8C320834` |
| 1991 | 1991_geral | 4 | 14 | 1 | 0.00 | Macross M3 `8C013EFE` |
| 1992 | 1992_geral | 4 | 14 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C12E192` |
| 1993 | 1993_geral | 4 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1728A0` |
| 1994 | 1994_geral | 4 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C176934` |
| 1995 | 1995_geral | 4 | 13 | 1 | 0.00 | Grandia II (USA) `8C041764` |
| 1996 | 1996_float | 4 | 13 | 1 | 0.00 | King of Fighters The - Evolu `8C323520` |
| 1997 | 1997_geral | 4 | 13 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C380682` |
| 1998 | 1998_geral | 4 | 13 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3841B0` |
| 1999 | 1999_divisao | 4 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C174450` |
| 2000 | 2000_geral | 4 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16C070` |
| 2001 | 2001_geral | 4 | 12 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05CD58` |
| 2002 | 2002_geral | 4 | 12 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C170696` |
| 2003 | 2003_geral | 4 | 12 | 1 | 0.00 | King of Fighters The - Evolu `8C364F50` |
| 2004 | 2004_geral | 4 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16C0A0` |
| 2005 | 2005_copia | 4 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1768AE` |
| 2006 | 2006_geral | 4 | 11 | 1 | 0.00 | Grandia II (USA) `8C034FD2` |
| 2007 | 2007_geral | 4 | 11 | 1 | 0.00 | Macross M3 `8C1E7028` |
| 2008 | 2008_geral | 4 | 10 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16CC40` |
| 2009 | 2009_copia | 4 | 10 | 1 | 0.00 | Grandia II (USA) `8C0C3E72` |
| 2010 | 2010_preenche | 4 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C176978` |
| 2011 | 2011_geral | 4 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FCD40` |
| 2012 | 2012_geral | 4 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FCE00` |
| 2013 | 2013_geral | 4 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1768C4` |
| 2014 | 2014_geral | 4 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1FEF60` |
| 2015 | 2015_geral | 4 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21E688` |
| 2016 | 2016_preenche | 3 | 1092 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09D6CE` |
| 2017 | 2017_divisao_pref | 3 | 583 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C3B4CB0` |
| 2018 | 2018_geral | 3 | 534 | 2 | 0.00 | Evolution - The World of Sac `8C1D5D9C` |
| 2019 | 2019_geral | 3 | 396 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C065BBA` |
| 2020 | 2020_geral | 3 | 385 | 3 | 0.00 | Resident Evil - Code - Veron `8C17B8B6` |
| 2021 | 2021_divisao | 3 | 351 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0667F2` |
| 2022 | 2022_raiz | 3 | 343 | 3 | 0.00 | Evolution 2 - Far Off Promis `8C1D95A4` |
| 2023 | 2023_geral | 3 | 342 | 3 | 0.00 | Soulcalibur (USA) `8C23CC68` |
| 2024 | 2024_divisao_pref | 3 | 290 | 3 | 0.00 | Evolution 2 - Far Off Promis `8C18B9FC` |
| 2025 | 2025_copia | 3 | 270 | 3 | 0.00 | Phantasy Star Online Ver. 2  `8C12CB44` |
| 2026 | 2026_pref | 3 | 260 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C196668` |
| 2027 | 2027_geral | 3 | 236 | 4 | 0.00 | Evolution - The World of Sac `8C19C8E4` |
| 2028 | 2028_geral | 3 | 234 | 3 | 0.00 | Phantasy Star Online Ver. 2  `8C3946E8` |
| 2029 | 2029_geral | 3 | 231 | 33 | 0.00 | Le Mans 24 Hours (Europe) (E `8C032318` |
| 2030 | 2030_geral | 3 | 228 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C06E74A` |
| 2031 | 2031_copia | 3 | 224 | 3 | 0.00 | Phantasy Star Online Ver. 2  `8C12CFD8` |
| 2032 | 2032_geral | 3 | 222 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C09B340` |
| 2033 | 2033_geral | 3 | 215 | 2 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C122890` |
| 2034 | 2034_copia | 3 | 193 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C02F130` |
| 2035 | 2035_copia | 3 | 185 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05CF24` |
| 2036 | 2036_geral | 3 | 185 | 2 | 0.00 | Skies of Arcadia (USA) (Disc `8C2C0262` |
| 2037 | 2037_raiz_divisao_pref | 3 | 174 | 1 | 0.00 | King of Fighters The - Evolu `8C324554` |
| 2038 | 2038_copia | 3 | 171 | 1 | 0.00 | Evolution - The World of Sac `8C1A7D3C` |
| 2039 | 2039_geral | 3 | 169 | 3 | 0.00 | Evolution - The World of Sac `8C06F5D0` |
| 2040 | 2040_divisao | 3 | 169 | 2 | 0.00 | Evolution - The World of Sac `8C165D0C` |
| 2041 | 2041_geral | 3 | 157 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08D920` |
| 2042 | 2042_divisao | 3 | 156 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C06E31E` |
| 2043 | 2043_geral | 3 | 155 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08B5F2` |
| 2044 | 2044_geral | 3 | 152 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1AB302` |
| 2045 | 2045_raiz_divisao_pref | 3 | 151 | 2 | 0.00 | Evolution - The World of Sac `8C1C1AF8` |
| 2046 | 2046_divisao_pref | 3 | 145 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C177360` |
| 2047 | 2047_geral | 3 | 144 | 4 | 0.00 | Grandia II (USA) `8C076112` |
| 2048 | 2048_copia | 3 | 142 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07F58A` |
| 2049 | 2049_copia | 3 | 140 | 1 | 0.00 | Dead or Alive 2 (USA) `8C13338A` |
| 2050 | 2050_copia | 3 | 131 | 3 | 0.00 | King of Fighters The - Evolu `8C310FF0` |
| 2051 | 2051_copia | 3 | 122 | 2 | 0.00 | Evolution - The World of Sac `8C1C3B24` |
| 2052 | 2052_copia | 3 | 115 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C073490` |
| 2053 | 2053_copia | 3 | 115 | 1 | 0.00 | Evolution - The World of Sac `8C1C3D42` |
| 2054 | 2054_geral | 3 | 114 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08CB6C` |
| 2055 | 2055_geral | 3 | 112 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DCC6` |
| 2056 | 2056_geral | 3 | 106 | 3 | 0.00 | King of Fighters The - Evolu `8C33395C` |
| 2057 | 2057_geral | 3 | 105 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0AFCC0` |
| 2058 | 2058_copia | 3 | 105 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C20D9F4` |
| 2059 | 2059_copia | 3 | 104 | 3 | 0.00 | Grandia II (USA) `8C0C93D8` |
| 2060 | 2060_divisao | 3 | 104 | 2 | 0.00 | Resident Evil - Code - Veron `8C19C5B0` |
| 2061 | 2061_copia | 3 | 103 | 3 | 0.00 | Resident Evil - Code - Veron `8C173DF4` |
| 2062 | 2062_geral | 3 | 102 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C179AF8` |
| 2063 | 2063_divisao | 3 | 101 | 2 | 0.00 | Evolution - The World of Sac `8C1C2A8C` |
| 2064 | 2064_float | 3 | 101 | 3 | 0.00 | Skies of Arcadia (USA) (Disc `8C2BE6EC` |
| 2065 | 2065_geral | 3 | 98 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05D658` |
| 2066 | 2066_geral | 3 | 97 | 2 | 0.00 | Resident Evil - Code - Veron `8C1C583C` |
| 2067 | 2067_copia | 3 | 95 | 1 | 0.00 | Evolution - The World of Sac `8C1A7A70` |
| 2068 | 2068_geral | 3 | 95 | 10 | 0.00 | Evolution 2 - Far Off Promis `8C116E28` |
| 2069 | 2069_geral | 3 | 95 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1FF5AC` |
| 2070 | 2070_geral | 3 | 94 | 2 | 0.00 | Skies of Arcadia (USA) (Disc `8C29AEF4` |
| 2071 | 2071_geral | 3 | 93 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C072ED6` |
| 2072 | 2072_divisao | 3 | 92 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0665DA` |
| 2073 | 2073_copia | 3 | 92 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C20DC98` |
| 2074 | 2074_copia | 3 | 92 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C20DFD4` |
| 2075 | 2075_geral | 3 | 92 | 24 | 0.00 | Phantasy Star Online Ver. 2  `8C1CC1F0` |
| 2076 | 2076_divisao | 3 | 91 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C06E100` |
| 2077 | 2077_copia | 3 | 90 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DE16` |
| 2078 | 2078_geral | 3 | 89 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06E52A` |
| 2079 | 2079_copia | 3 | 88 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1ABF7C` |
| 2080 | 2080_geral | 3 | 88 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C07472E` |
| 2081 | 2081_geral | 3 | 87 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06EAC2` |
| 2082 | 2082_geral | 3 | 87 | 3 | 0.00 | Skies of Arcadia (USA) (Disc `8C012D9C` |
| 2083 | 2083_geral | 3 | 86 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1AA74C` |
| 2084 | 2084_float | 3 | 85 | 3 | 0.00 | Resident Evil - Code - Veron `8C1BCD58` |
| 2085 | 2085_copia | 3 | 84 | 3 | 0.00 | Skies of Arcadia (USA) (Disc `8C284FE0` |
| 2086 | 2086_copia | 3 | 83 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05DD48` |
| 2087 | 2087_geral | 3 | 83 | 2 | 0.00 | Evolution - The World of Sac `8C1A85A2` |
| 2088 | 2088_copia | 3 | 81 | 1 | 0.00 | Dead or Alive 2 (USA) `8C13363C` |
| 2089 | 2089_divisao | 3 | 81 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C146EB0` |
| 2090 | 2090_geral | 3 | 80 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08E4A4` |
| 2091 | 2091_copia | 3 | 79 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C072434` |
| 2092 | 2092_geral | 3 | 79 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08E3F0` |
| 2093 | 2093_geral | 3 | 79 | 1 | 0.00 | Evolution - The World of Sac `8C1D4D80` |
| 2094 | 2094_geral | 3 | 78 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0732EC` |
| 2095 | 2095_geral | 3 | 78 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08E66E` |
| 2096 | 2096_geral | 3 | 77 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071076` |
| 2097 | 2097_geral | 3 | 77 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C073592` |
| 2098 | 2098_geral | 3 | 77 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C20D808` |
| 2099 | 2099_copia | 3 | 77 | 1 | 0.00 | King of Fighters The - Evolu `8C324972` |
| 2100 | 2100_geral | 3 | 76 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C073F54` |
| 2101 | 2101_geral | 3 | 76 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08E7E4` |
| 2102 | 2102_geral | 3 | 75 | 3 | 0.00 | Grandia II (USA) `8C078E98` |
| 2103 | 2103_geral | 3 | 75 | 1 | 0.00 | Resident Evil - Code - Veron `8C1740D4` |
| 2104 | 2104_geral | 3 | 74 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C202748` |
| 2105 | 2105_divisao | 3 | 74 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C147264` |
| 2106 | 2106_copia | 3 | 74 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C0041C0` |
| 2107 | 2107_copia | 3 | 73 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C06E9C0` |
| 2108 | 2108_geral | 3 | 72 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C22C1E8` |
| 2109 | 2109_geral | 3 | 70 | 2 | 0.00 | Evolution - The World of Sac `8C1AD6E6` |
| 2110 | 2110_geral | 3 | 70 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05B3A8` |
| 2111 | 2111_geral | 3 | 69 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C203C1C` |
| 2112 | 2112_geral | 3 | 69 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06B6E8` |
| 2113 | 2113_geral | 3 | 69 | 1 | 0.00 | King of Fighters The - Evolu `8C316356` |
| 2114 | 2114_geral | 3 | 68 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05D1DE` |
| 2115 | 2115_geral | 3 | 68 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C062134` |
| 2116 | 2116_geral | 3 | 68 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071426` |
| 2117 | 2117_copia | 3 | 68 | 2 | 0.00 | Grandia II (USA) `8C05A5D0` |
| 2118 | 2118_copia | 3 | 67 | 2 | 0.00 | Evolution - The World of Sac `8C15C9C4` |
| 2119 | 2119_geral | 3 | 66 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C08B54A` |
| 2120 | 2120_copia | 3 | 65 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C2052AC` |
| 2121 | 2121_copia | 3 | 65 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0809B4` |
| 2122 | 2122_geral | 3 | 64 | 1 | 0.00 | King of Fighters The - Evolu `8C33054C` |
| 2123 | 2123_copia | 3 | 63 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05D9C8` |
| 2124 | 2124_geral | 3 | 63 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08E888` |
| 2125 | 2125_geral | 3 | 62 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C177470` |
| 2126 | 2126_geral | 3 | 62 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08E72A` |
| 2127 | 2127_geral | 3 | 61 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C206BBC` |
| 2128 | 2128_geral | 3 | 61 | 3 | 0.00 | Project Justice (USA) `0C2DF578` |
| 2129 | 2129_copia | 3 | 61 | 3 | 0.00 | Skies of Arcadia (USA) (Disc `8C251A88` |
| 2130 | 2130_copia | 3 | 60 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1ABE84` |
| 2131 | 2131_copia | 3 | 60 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071AC6` |
| 2132 | 2132_geral | 3 | 60 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C3944C4` |
| 2133 | 2133_geral | 3 | 59 | 2 | 0.00 | Resident Evil - Code - Veron `8C170286` |
| 2134 | 2134_geral | 3 | 58 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C066E94` |
| 2135 | 2135_geral | 3 | 58 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0AFD92` |
| 2136 | 2136_copia | 3 | 58 | 1 | 0.00 | Grandia II (USA) `8C0134D8` |
| 2137 | 2137_geral | 3 | 57 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C060D00` |
| 2138 | 2138_geral | 3 | 57 | 1 | 0.00 | King of Fighters The - Evolu `8C363A20` |
| 2139 | 2139_geral | 3 | 56 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C073420` |
| 2140 | 2140_geral | 3 | 56 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0B00DC` |
| 2141 | 2141_geral | 3 | 55 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071FF8` |
| 2142 | 2142_matriz_divisao | 3 | 55 | 1 | 0.00 | Evolution - The World of Sac `8C199AF4` |
| 2143 | 2143_divisao | 3 | 55 | 2 | 0.00 | Power Stone (USA) `0C0DF400` |
| 2144 | 2144_geral | 3 | 55 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C01D29E` |
| 2145 | 2145_copia | 3 | 53 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08C52A` |
| 2146 | 2146_pref | 3 | 53 | 1 | 0.00 | Evolution - The World of Sac `8C1C0DA4` |
| 2147 | 2147_geral | 3 | 53 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C141DF2` |
| 2148 | 2148_float | 3 | 53 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C1471F8` |
| 2149 | 2149_geral | 3 | 52 | 3 | 0.00 | Dead or Alive 2 (USA) `8C105310` |
| 2150 | 2150_geral | 3 | 52 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05D15C` |
| 2151 | 2151_copia | 3 | 52 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0662D8` |
| 2152 | 2152_geral | 3 | 52 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1ABA06` |
| 2153 | 2153_geral | 3 | 51 | 2 | 0.00 | Evolution - The World of Sac `8C1ACE94` |
| 2154 | 2154_matriz_divisao | 3 | 51 | 2 | 0.00 | Evolution - The World of Sac `8C1C0D28` |
| 2155 | 2155_geral | 3 | 51 | 1 | 0.00 | King of Fighters The - Evolu `8C324D18` |
| 2156 | 2156_geral | 3 | 51 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C38F070` |
| 2157 | 2157_copia | 3 | 50 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0736C4` |
| 2158 | 2158_geral | 3 | 49 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05D35C` |
| 2159 | 2159_geral | 3 | 49 | 2 | 0.00 | Evolution - The World of Sac `8C1A6DD2` |
| 2160 | 2160_float | 3 | 49 | 1 | 0.00 | Evolution - The World of Sac `8C1C2970` |
| 2161 | 2161_geral | 3 | 48 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F34B6` |
| 2162 | 2162_copia | 3 | 48 | 2 | 0.00 | Resident Evil - Code - Veron `8C17D1EC` |
| 2163 | 2163_geral | 3 | 47 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C201C02` |
| 2164 | 2164_copia | 3 | 47 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06E4CC` |
| 2165 | 2165_copia | 3 | 47 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08E558` |
| 2166 | 2166_geral | 3 | 47 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09C168` |
| 2167 | 2167_copia | 3 | 46 | 1 | 0.00 | Evolution - The World of Sac `8C1CB9C0` |
| 2168 | 2168_geral | 3 | 45 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06B5B0` |
| 2169 | 2169_geral | 3 | 45 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C3911AC` |
| 2170 | 2170_copia | 3 | 45 | 1 | 0.00 | King of Fighters The - Evolu `8C3200EC` |
| 2171 | 2171_geral | 3 | 45 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C394668` |
| 2172 | 2172_matriz_divisao | 3 | 45 | 2 | 0.00 | Resident Evil - Code - Veron `8C1A1E84` |
| 2173 | 2173_geral | 3 | 45 | 1 | 0.00 | Resident Evil - Code - Veron `8C1797BC` |
| 2174 | 2174_geral | 3 | 44 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C093A5C` |
| 2175 | 2175_geral | 3 | 43 | 1 | 0.00 | Dead or Alive 2 (USA) `8C12F040` |
| 2176 | 2176_copia | 3 | 42 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C060B92` |
| 2177 | 2177_geral | 3 | 42 | 3 | 0.00 | Elemental Gimmick Gear v1.00 `8C08A0F0` |
| 2178 | 2178_geral | 3 | 42 | 1 | 0.00 | Macross M3 `8C1F5DC8` |
| 2179 | 2179_geral | 3 | 41 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C20416C` |
| 2180 | 2180_geral | 3 | 41 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C072DCC` |
| 2181 | 2181_geral | 3 | 41 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07381E` |
| 2182 | 2182_geral | 3 | 41 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08DF78` |
| 2183 | 2183_geral | 3 | 41 | 2 | 0.00 | Evolution - The World of Sac `8C1ACF9C` |
| 2184 | 2184_matriz_divisao | 3 | 41 | 1 | 0.00 | Evolution - The World of Sac `8C199B84` |
| 2185 | 2185_geral | 3 | 41 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C3526F0` |
| 2186 | 2186_geral | 3 | 40 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C201B4A` |
| 2187 | 2187_geral | 3 | 40 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C201C74` |
| 2188 | 2188_geral | 3 | 40 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C05BACC` |
| 2189 | 2189_float | 3 | 40 | 1 | 0.00 | Evolution - The World of Sac `8C1C397C` |
| 2190 | 2190_geral | 3 | 39 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C072CD2` |
| 2191 | 2191_geral | 3 | 39 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D8DEC` |
| 2192 | 2192_geral | 3 | 38 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C081120` |
| 2193 | 2193_geral | 3 | 38 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C05D458` |
| 2194 | 2194_geral | 3 | 38 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C075066` |
| 2195 | 2195_copia | 3 | 38 | 2 | 0.00 | Evolution - The World of Sac `8C1DDC4C` |
| 2196 | 2196_geral | 3 | 38 | 1 | 0.00 | Resident Evil - Code - Veron `8C1ACEE8` |
| 2197 | 2197_geral | 3 | 38 | 1 | 0.00 | Resident Evil - Code - Veron `8C1ACF70` |
| 2198 | 2198_geral | 3 | 37 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C072760` |
| 2199 | 2199_geral | 3 | 37 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08A148` |
| 2200 | 2200_geral | 3 | 37 | 1 | 0.00 | King of Fighters The - Evolu `8C33C9D6` |
| 2201 | 2201_geral | 3 | 37 | 4 | 0.00 | Resident Evil - Code - Veron `8C1ACBC8` |
| 2202 | 2202_geral | 3 | 36 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DF2E` |
| 2203 | 2203_geral | 3 | 36 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09CD5C` |
| 2204 | 2204_geral | 3 | 36 | 1 | 0.00 | King of Fighters The - Evolu `8C368418` |
| 2205 | 2205_geral | 3 | 36 | 1 | 0.00 | Resident Evil - Code - Veron `8C1ACB48` |
| 2206 | 2206_geral | 3 | 35 | 1 | 0.00 | Evolution - The World of Sac `8C15DEDE` |
| 2207 | 2207_pref | 3 | 35 | 1 | 0.00 | Grandia II (USA) `8C03D252` |
| 2208 | 2208_pref | 3 | 35 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C1260BE` |
| 2209 | 2209_geral | 3 | 34 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0723D8` |
| 2210 | 2210_geral | 3 | 34 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0744F8` |
| 2211 | 2211_geral | 3 | 34 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08C77E` |
| 2212 | 2212_geral | 3 | 34 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A36CC` |
| 2213 | 2213_geral | 3 | 34 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C35241C` |
| 2214 | 2214_geral | 3 | 33 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C06E5BC` |
| 2215 | 2215_geral | 3 | 33 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16F450` |
| 2216 | 2216_geral | 3 | 33 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DDD4` |
| 2217 | 2217_geral | 3 | 33 | 1 | 0.00 | Resident Evil - Code - Veron `8C1ACFF8` |
| 2218 | 2218_geral | 3 | 32 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C17BD20` |
| 2219 | 2219_copia | 3 | 32 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C06CFD6` |
| 2220 | 2220_geral | 3 | 32 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08A1A4` |
| 2221 | 2221_float | 3 | 32 | 1 | 0.00 | Grandia II (USA) `8C030CBE` |
| 2222 | 2222_geral | 3 | 32 | 1 | 0.00 | Grandia II (USA) `8C0C557A` |
| 2223 | 2223_geral | 3 | 32 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C1482F6` |
| 2224 | 2224_geral | 3 | 30 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C071814` |
| 2225 | 2225_geral | 3 | 30 | 1 | 0.00 | King of Fighters The - Evolu `8C3311A8` |
| 2226 | 2226_geral | 3 | 30 | 2 | 0.00 | Macross M3 `8C1D2914` |
| 2227 | 2227_geral | 3 | 30 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3860FA` |
| 2228 | 2228_geral | 3 | 30 | 3 | 0.00 | Shenmue (USA) (Disc 1) `0C0F587C` |
| 2229 | 2229_pref | 3 | 29 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C18080E` |
| 2230 | 2230_geral | 3 | 29 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1720C0` |
| 2231 | 2231_geral | 3 | 29 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F365C` |
| 2232 | 2232_geral | 3 | 29 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0730DA` |
| 2233 | 2233_geral | 3 | 29 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08A1F6` |
| 2234 | 2234_copia | 3 | 29 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09C504` |
| 2235 | 2235_geral | 3 | 29 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C1A8852` |
| 2236 | 2236_geral | 3 | 29 | 1 | 0.00 | Grandia II (USA) `8C014E90` |
| 2237 | 2237_geral | 3 | 28 | 1 | 0.00 | Evolution - The World of Sac `8C15D122` |
| 2238 | 2238_geral | 3 | 28 | 1 | 0.00 | King of Fighters The - Evolu `8C3166BC` |
| 2239 | 2239_laco | 3 | 27 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F7D38` |
| 2240 | 2240_preenche | 3 | 27 | 1 | 0.00 | Dead or Alive 2 (USA) `8C12AC40` |
| 2241 | 2241_geral | 3 | 27 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071040` |
| 2242 | 2242_geral | 3 | 27 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0740F0` |
| 2243 | 2243_geral | 3 | 27 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C074B7A` |
| 2244 | 2244_geral | 3 | 27 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C074E2A` |
| 2245 | 2245_float | 3 | 27 | 1 | 0.00 | Evolution - The World of Sac `8C1C3854` |
| 2246 | 2246_geral | 3 | 27 | 2 | 0.00 | King of Fighters The - Evolu `8C31EBB0` |
| 2247 | 2247_copia | 3 | 26 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05DAB0` |
| 2248 | 2248_geral | 3 | 26 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0714FC` |
| 2249 | 2249_geral | 3 | 26 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071538` |
| 2250 | 2250_float | 3 | 26 | 1 | 0.00 | King of Fighters The - Evolu `8C322F68` |
| 2251 | 2251_geral | 3 | 26 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C10DCE4` |
| 2252 | 2252_geral | 3 | 25 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C17CA90` |
| 2253 | 2253_geral | 3 | 25 | 1 | 0.00 | Dead or Alive 2 (USA) `8C13BF7C` |
| 2254 | 2254_copia | 3 | 25 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06EA52` |
| 2255 | 2255_geral | 3 | 25 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0720BE` |
| 2256 | 2256_geral | 3 | 25 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0746C8` |
| 2257 | 2257_geral | 3 | 25 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C077A1A` |
| 2258 | 2258_geral | 3 | 25 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08C47A` |
| 2259 | 2259_geral | 3 | 25 | 1 | 0.00 | Evolution - The World of Sac `8C1A3484` |
| 2260 | 2260_geral | 3 | 25 | 1 | 0.00 | King of Fighters The - Evolu `8C33DE20` |
| 2261 | 2261_geral | 3 | 25 | 1 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C124230` |
| 2262 | 2262_geral | 3 | 24 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DEFE` |
| 2263 | 2263_geral | 3 | 24 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05C308` |
| 2264 | 2264_geral | 3 | 24 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089EF6` |
| 2265 | 2265_geral | 3 | 24 | 1 | 0.00 | Macross M3 `8C1406D6` |
| 2266 | 2266_geral | 3 | 24 | 1 | 0.00 | Macross M3 `8C1F5E34` |
| 2267 | 2267_geral | 3 | 23 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C17BCB2` |
| 2268 | 2268_geral | 3 | 23 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05E6B4` |
| 2269 | 2269_laco | 3 | 23 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D46C` |
| 2270 | 2270_geral | 3 | 23 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0748D8` |
| 2271 | 2271_geral | 3 | 23 | 1 | 0.00 | Grandia II (USA) `8C028116` |
| 2272 | 2272_geral | 3 | 23 | 1 | 0.00 | Grandia II (USA) `8C013190` |
| 2273 | 2273_geral | 3 | 23 | 1 | 0.00 | King of Fighters The - Evolu `8C31688C` |
| 2274 | 2274_geral | 3 | 23 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C113E56` |
| 2275 | 2275_geral | 3 | 22 | 2 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C123D80` |
| 2276 | 2276_geral | 3 | 22 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05DDF2` |
| 2277 | 2277_geral | 3 | 22 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05DB60` |
| 2278 | 2278_geral | 3 | 22 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DF76` |
| 2279 | 2279_geral | 3 | 22 | 1 | 0.00 | Macross M3 `8C1C61E0` |
| 2280 | 2280_geral | 3 | 21 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0737F4` |
| 2281 | 2281_geral | 3 | 21 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C081090` |
| 2282 | 2282_geral | 3 | 21 | 1 | 0.00 | Evolution - The World of Sac `8C1CEE80` |
| 2283 | 2283_geral | 3 | 21 | 1 | 0.00 | Evolution - The World of Sac `8C1D4F10` |
| 2284 | 2284_float | 3 | 20 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16F6C0` |
| 2285 | 2285_geral | 3 | 20 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05D5CE` |
| 2286 | 2286_geral | 3 | 20 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C074184` |
| 2287 | 2287_produto_escalar | 3 | 20 | 1 | 0.00 | Evolution - The World of Sac `8C1A4AB4` |
| 2288 | 2288_geral | 3 | 20 | 1 | 0.00 | King of Fighters The - Evolu `8C31EE3E` |
| 2289 | 2289_geral | 3 | 19 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C074B54` |
| 2290 | 2290_geral | 3 | 19 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06BBA0` |
| 2291 | 2291_geral | 3 | 19 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C074608` |
| 2292 | 2292_geral | 3 | 19 | 3 | 0.00 | Evolution 2 - Far Off Promis `8C1AD0C0` |
| 2293 | 2293_geral | 3 | 19 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08C448` |
| 2294 | 2294_laco | 3 | 19 | 1 | 0.00 | Evolution - The World of Sac `8C1D4488` |
| 2295 | 2295_copia | 3 | 19 | 1 | 0.00 | Grandia II (USA) `8C0388E2` |
| 2296 | 2296_geral | 3 | 18 | 1 | 0.00 | Dead or Alive 2 (USA) `8C114DB0` |
| 2297 | 2297_geral | 3 | 18 | 1 | 0.00 | Dead or Alive 2 (USA) `8C0F83A0` |
| 2298 | 2298_geral | 3 | 18 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05B880` |
| 2299 | 2299_geral | 3 | 18 | 1 | 0.00 | King of Fighters The - Evolu `8C3682C4` |
| 2300 | 2300_laco | 3 | 17 | 1 | 0.00 | Dead or Alive 2 (USA) `8C13BEC0` |
| 2301 | 2301_copia | 3 | 17 | 1 | 0.00 | Dead or Alive 2 (USA) `8C13CE80` |
| 2302 | 2302_geral | 3 | 17 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C060AEE` |
| 2303 | 2303_geral | 3 | 17 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C072E70` |
| 2304 | 2304_geral | 3 | 17 | 1 | 0.00 | King of Fighters The - Evolu `8C3197AC` |
| 2305 | 2305_geral | 3 | 17 | 1 | 0.00 | King of Fighters The - Evolu `8C31EDD4` |
| 2306 | 2306_geral | 3 | 16 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C074022` |
| 2307 | 2307_geral | 3 | 16 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D548` |
| 2308 | 2308_geral | 3 | 16 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D8B0` |
| 2309 | 2309_geral | 3 | 16 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07266A` |
| 2310 | 2310_geral | 3 | 16 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C074132` |
| 2311 | 2311_geral | 3 | 16 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0A03B4` |
| 2312 | 2312_float | 3 | 16 | 1 | 0.00 | Grandia II (USA) `8C016CB4` |
| 2313 | 2313_geral | 3 | 16 | 1 | 0.00 | King of Fighters The - Evolu `8C33CA64` |
| 2314 | 2314_geral | 3 | 16 | 1 | 0.00 | King of Fighters The - Evolu `8C31EDF6` |
| 2315 | 2315_laco | 3 | 16 | 1 | 0.00 | King of Fighters The - Evolu `8C31F08C` |
| 2316 | 2316_geral | 3 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C22BC74` |
| 2317 | 2317_cache | 3 | 15 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0685FA` |
| 2318 | 2318_geral | 3 | 15 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0719E4` |
| 2319 | 2319_geral | 3 | 15 | 1 | 0.00 | Evolution - The World of Sac `8C1F02A0` |
| 2320 | 2320_geral | 3 | 15 | 1 | 0.00 | Grandia II (USA) `8C0388C4` |
| 2321 | 2321_geral | 3 | 15 | 1 | 0.00 | King of Fighters The - Evolu `8C339282` |
| 2322 | 2322_copia | 3 | 14 | 1 | 0.00 | Dead or Alive 2 (USA) `8C106500` |
| 2323 | 2323_geral | 3 | 14 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071576` |
| 2324 | 2324_geral | 3 | 14 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C074998` |
| 2325 | 2325_geral | 3 | 14 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089AE4` |
| 2326 | 2326_geral | 3 | 14 | 1 | 0.00 | Evolution - The World of Sac `8C1A86A8` |
| 2327 | 2327_divisao | 3 | 14 | 1 | 0.00 | Evolution - The World of Sac `8C16BE72` |
| 2328 | 2328_geral | 3 | 14 | 1 | 0.00 | King of Fighters The - Evolu `8C3A7226` |
| 2329 | 2329_copia | 3 | 14 | 1 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C10FBE0` |
| 2330 | 2330_geral | 3 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16EF10` |
| 2331 | 2331_geral | 3 | 13 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C23684C` |
| 2332 | 2332_preenche | 3 | 13 | 1 | 0.00 | Dead or Alive 2 (USA) `8C106070` |
| 2333 | 2333_geral | 3 | 13 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07492C` |
| 2334 | 2334_geral | 3 | 13 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06E482` |
| 2335 | 2335_geral | 3 | 13 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06E49C` |
| 2336 | 2336_geral | 3 | 13 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09C77C` |
| 2337 | 2337_geral | 3 | 13 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09C7C0` |
| 2338 | 2338_geral | 3 | 13 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D8996` |
| 2339 | 2339_geral | 3 | 13 | 1 | 0.00 | Grandia II (USA) `8C028A6A` |
| 2340 | 2340_preenche | 3 | 13 | 1 | 0.00 | King of Fighters The - Evolu `8C31F0AC` |
| 2341 | 2341_geral | 3 | 13 | 1 | 0.00 | King of Fighters The - Evolu `8C33EBB8` |
| 2342 | 2342_geral | 3 | 13 | 1 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C179232` |
| 2343 | 2343_geral | 3 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C172050` |
| 2344 | 2344_geral | 3 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21E5CA` |
| 2345 | 2345_geral | 3 | 12 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05E0C2` |
| 2346 | 2346_geral | 3 | 12 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09C6BC` |
| 2347 | 2347_geral | 3 | 12 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09C71C` |
| 2348 | 2348_geral | 3 | 12 | 1 | 0.00 | Evolution - The World of Sac `8C1A34B6` |
| 2349 | 2349_geral | 3 | 12 | 1 | 0.00 | Evolution - The World of Sac `8C1ADF22` |
| 2350 | 2350_float | 3 | 12 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C176390` |
| 2351 | 2351_matriz | 3 | 12 | 1 | 0.00 | Grandia II (USA) `8C04DDA8` |
| 2352 | 2352_geral | 3 | 12 | 1 | 0.00 | Grandia II (USA) `8C07FE3C` |
| 2353 | 2353_geral | 3 | 12 | 1 | 0.00 | King of Fighters The - Evolu `8C31EE26` |
| 2354 | 2354_geral | 3 | 11 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071672` |
| 2355 | 2355_geral | 3 | 11 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C071DAC` |
| 2356 | 2356_geral | 3 | 11 | 1 | 0.00 | Evolution - The World of Sac `8C1DB094` |
| 2357 | 2357_geral | 3 | 11 | 1 | 0.00 | King of Fighters The - Evolu `8C3682F2` |
| 2358 | 2358_geral | 3 | 10 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16F650` |
| 2359 | 2359_geral | 3 | 10 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C22BF88` |
| 2360 | 2360_geral | 3 | 10 | 1 | 0.00 | Dead or Alive 2 (USA) `8C13C000` |
| 2361 | 2361_laco | 3 | 10 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0631D4` |
| 2362 | 2362_geral | 3 | 10 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D66C` |
| 2363 | 2363_geral | 3 | 10 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05E660` |
| 2364 | 2364_geral | 3 | 10 | 1 | 0.00 | Grandia II (USA) `8C03568C` |
| 2365 | 2365_geral | 3 | 10 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C113DD6` |
| 2366 | 2366_geral | 3 | 9 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21E256` |
| 2367 | 2367_geral | 3 | 9 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05E6E2` |
| 2368 | 2368_geral | 3 | 9 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C030F30` |
| 2369 | 2369_geral | 3 | 9 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C060264` |
| 2370 | 2370_geral | 3 | 9 | 1 | 0.00 | Grandia II (USA) `8C07F540` |
| 2371 | 2371_geral | 3 | 9 | 1 | 0.00 | King of Fighters The - Evolu `8C31A352` |
| 2372 | 2372_matriz | 3 | 9 | 1 | 0.00 | King of Fighters The - Evolu `8C3235B4` |
| 2373 | 2373_geral | 3 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C22C28A` |
| 2374 | 2374_geral | 3 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16D650` |
| 2375 | 2375_float | 3 | 8 | 1 | 0.00 | Dead or Alive 2 (USA) `8C0F9D70` |
| 2376 | 2376_geral | 3 | 8 | 1 | 0.00 | Grandia II (USA) `8C0358A8` |
| 2377 | 2377_divisao | 2 | 754 | 2 | 0.00 | Skies of Arcadia (USA) (Disc `8C282F50` |
| 2378 | 2378_divisao_pref | 2 | 736 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3A02D8` |
| 2379 | 2379_divisao_pref | 2 | 697 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3B19A4` |
| 2380 | 2380_divisao_pref | 2 | 554 | 1 | 0.00 | Evolution - The World of Sac `8C1C162C` |
| 2381 | 2381_divisao_pref | 2 | 531 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3A3424` |
| 2382 | 2382_divisao_pref | 2 | 396 | 2 | 0.00 | King of Fighters The - Evolu `8C3B22FC` |
| 2383 | 2383_raiz_divisao_pref | 2 | 392 | 1 | 0.00 | Evolution - The World of Sac `8C1C1C26` |
| 2384 | 2384_divisao | 2 | 378 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C04663C` |
| 2385 | 2385_divisao | 2 | 368 | 2 | 0.00 | Skies of Arcadia (USA) (Disc `8C28357C` |
| 2386 | 2386_pref | 2 | 352 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3B12EC` |
| 2387 | 2387_divisao | 2 | 344 | 1 | 0.00 | Evolution - The World of Sac `8C03E618` |
| 2388 | 2388_pref | 2 | 333 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3AEAD4` |
| 2389 | 2389_copia | 2 | 333 | 2 | 0.00 | Power Stone (USA) `0C0E5D98` |
| 2390 | 2390_divisao | 2 | 331 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C016BF6` |
| 2391 | 2391_divisao | 2 | 311 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C12049C` |
| 2392 | 2392_pref | 2 | 311 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3B1720` |
| 2393 | 2393_copia | 2 | 286 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1FFB4C` |
| 2394 | 2394_geral | 2 | 286 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3757CE` |
| 2395 | 2395_pref | 2 | 257 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3AE684` |
| 2396 | 2396_pref | 2 | 248 | 2 | 0.00 | King of Fighters The - Evolu `8C38F258` |
| 2397 | 2397_divisao | 2 | 246 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C146F58` |
| 2398 | 2398_copia | 2 | 210 | 2 | 0.00 | Evolution - The World of Sac `8C15C620` |
| 2399 | 2399_pref | 2 | 209 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C39EE2C` |
| 2400 | 2400_pref | 2 | 206 | 1 | 0.00 | Dead or Alive 2 (USA) `8C11B9C0` |
| 2401 | 2401_geral | 2 | 202 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09AEC6` |
| 2402 | 2402_copia | 2 | 201 | 2 | 0.00 | Dead or Alive 2 (USA) `8C105950` |
| 2403 | 2403_pref | 2 | 201 | 2 | 0.00 | King of Fighters The - Evolu `8C3923D8` |
| 2404 | 2404_copia | 2 | 201 | 7 | 0.00 | Phantasy Star Online Ver. 2  `8C097EF0` |
| 2405 | 2405_copia | 2 | 200 | 2 | 0.00 | Macross M3 `8C1EAC48` |
| 2406 | 2406_copia | 2 | 180 | 2 | 0.00 | Soulcalibur (USA) `8C221816` |
| 2407 | 2407_geral | 2 | 177 | 1 | 0.00 | Evolution - The World of Sac `8C009488` |
| 2408 | 2408_divisao_pref | 2 | 177 | 2 | 0.00 | Resident Evil - Code - Veron `8C1A1EF4` |
| 2409 | 2409_geral | 2 | 164 | 3 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0180C8` |
| 2410 | 2410_divisao | 2 | 163 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3BB744` |
| 2411 | 2411_geral | 2 | 161 | 1 | 0.00 | Grandia II (USA) `8C084628` |
| 2412 | 2412_copia | 2 | 159 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21D6B0` |
| 2413 | 2413_geral | 2 | 158 | 2 | 0.00 | Resident Evil - Code - Veron `8C1A3884` |
| 2414 | 2414_copia | 2 | 156 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C20D478` |
| 2415 | 2415_geral | 2 | 156 | 2 | 0.00 | Grandia II (USA) `8C07441C` |
| 2416 | 2416_copia | 2 | 156 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C359986` |
| 2417 | 2417_pref | 2 | 156 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C39DE84` |
| 2418 | 2418_pref | 2 | 156 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3B15D4` |
| 2419 | 2419_copia | 2 | 154 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C011008` |
| 2420 | 2420_geral | 2 | 151 | 2 | 0.00 | Skies of Arcadia (USA) (Disc `8C28BCF6` |
| 2421 | 2421_copia | 2 | 146 | 2 | 0.00 | Shenmue (USA) (Disc 1) `0C1D3C18` |
| 2422 | 2422_geral | 2 | 145 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1774F0` |
| 2423 | 2423_divisao | 2 | 145 | 2 | 0.00 | Evolution - The World of Sac `8C16611A` |
| 2424 | 2424_matriz_produto_escalar_divisao | 2 | 142 | 2 | 0.00 | Dead or Alive 2 (USA) `8C103334` |
| 2425 | 2425_copia | 2 | 138 | 1 | 0.00 | Shenmue (USA) (Disc 1) `AC004070` |
| 2426 | 2426_geral | 2 | 132 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C37AA28` |
| 2427 | 2427_divisao | 2 | 131 | 2 | 0.00 | Skies of Arcadia (USA) (Disc `8C01394C` |
| 2428 | 2428_copia | 2 | 127 | 1 | 0.00 | Shenmue (USA) (Disc 1) `AC004998` |
| 2429 | 2429_geral | 2 | 125 | 2 | 0.00 | Shenmue (USA) (Disc 1) `0C094C80` |
| 2430 | 2430_geral | 2 | 123 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C176730` |
| 2431 | 2431_copia | 2 | 116 | 1 | 0.00 | Evolution - The World of Sac `8C1996FC` |
| 2432 | 2432_copia | 2 | 115 | 1 | 0.00 | Dead or Alive 2 (USA) `8C1011B0` |
| 2433 | 2433_copia | 2 | 114 | 1 | 0.00 | Dead or Alive 2 (USA) `8C12074A` |
| 2434 | 2434_geral | 2 | 106 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C017F90` |
| 2435 | 2435_geral | 2 | 106 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C13293C` |
| 2436 | 2436_geral | 2 | 103 | 3 | 0.00 | Grandia II (USA) `8C3EB0F4` |
| 2437 | 2437_copia | 2 | 103 | 1 | 0.00 | Shenmue (USA) (Disc 1) `AC004B18` |
| 2438 | 2438_geral | 2 | 102 | 5 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C0F2230` |
| 2439 | 2439_geral | 2 | 100 | 1 | 0.00 | Dead or Alive 2 (USA) `8C100220` |
| 2440 | 2440_geral | 2 | 98 | 1 | 0.00 | Grandia II (USA) `8C02B3C0` |
| 2441 | 2441_copia | 2 | 96 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3943CC` |
| 2442 | 2442_geral | 2 | 93 | 2 | 0.00 | Dead or Alive 2 (USA) `8C0FA720` |
| 2443 | 2443_geral | 2 | 93 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05FE54` |
| 2444 | 2444_pref | 2 | 93 | 2 | 0.00 | Resident Evil - Code - Veron `8C1A1294` |
| 2445 | 2445_geral | 2 | 92 | 2 | 0.00 | King of Fighters The - Evolu `8C010080` |
| 2446 | 2446_geral | 2 | 92 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C386F54` |
| 2447 | 2447_copia | 2 | 92 | 2 | 0.00 | Sonic Adventure 2 (USA) (EnJ `8C157F4A` |
| 2448 | 2448_geral | 2 | 90 | 12 | 0.00 | Phantasy Star Online Ver. 2  `8C20B230` |
| 2449 | 2449_geral | 2 | 89 | 1 | 0.00 | Dead or Alive 2 (USA) `8C105E60` |
| 2450 | 2450_copia | 2 | 88 | 1 | 0.00 | Dead or Alive 2 (USA) `8C104A40` |
| 2451 | 2451_copia | 2 | 88 | 2 | 0.00 | Skies of Arcadia (USA) (Disc `8C0DF9AE` |
| 2452 | 2452_copia | 2 | 87 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21D9A8` |
| 2453 | 2453_pref | 2 | 87 | 1 | 0.00 | Macross M3 `8C1C284C` |
| 2454 | 2454_copia | 2 | 85 | 2 | 0.00 | Grandia II (USA) `8C0866E8` |
| 2455 | 2455_geral | 2 | 85 | 2 | 0.00 | Skies of Arcadia (USA) (Disc `8C29A5D4` |
| 2456 | 2456_copia | 2 | 84 | 1 | 0.00 | Macross M3 `8C1D29EC` |
| 2457 | 2457_float | 2 | 83 | 1 | 0.00 | Resident Evil - Code - Veron `8C197620` |
| 2458 | 2458_copia | 2 | 82 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C067868` |
| 2459 | 2459_geral | 2 | 79 | 1 | 0.00 | Evolution - The World of Sac `8C1CCE00` |
| 2460 | 2460_geral | 2 | 79 | 1 | 0.00 | Grandia II (USA) `8C0C53E8` |
| 2461 | 2461_geral | 2 | 78 | 8 | 0.00 | Le Mans 24 Hours (Europe) (E `8C07957E` |
| 2462 | 2462_geral | 2 | 78 | 1 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C180250` |
| 2463 | 2463_geral | 2 | 77 | 1 | 0.00 | Macross M3 `8C0100FC` |
| 2464 | 2464_geral | 2 | 75 | 1 | 0.00 | Evolution - The World of Sac `8C031FB6` |
| 2465 | 2465_geral | 2 | 75 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C359CC4` |
| 2466 | 2466_geral | 2 | 74 | 2 | 0.00 | Grandia II (USA) `8C059E90` |
| 2467 | 2467_copia | 2 | 74 | 1 | 0.00 | King of Fighters The - Evolu `8C324B14` |
| 2468 | 2468_geral | 2 | 73 | 1 | 0.00 | Evolution - The World of Sac `8C1C68FC` |
| 2469 | 2469_copia | 2 | 73 | 1 | 0.00 | Grandia II (USA) `8C0410F0` |
| 2470 | 2470_geral | 2 | 73 | 2 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C035DF2` |
| 2471 | 2471_geral | 2 | 72 | 1 | 0.00 | Grandia II (USA) `8C0864C4` |
| 2472 | 2472_matriz_divisao_cache | 2 | 72 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3A40F4` |
| 2473 | 2473_geral | 2 | 72 | 2 | 0.00 | Shenmue (USA) (Disc 1) `0C08F0A8` |
| 2474 | 2474_geral | 2 | 71 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D29B0` |
| 2475 | 2475_geral | 2 | 71 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C1671E4` |
| 2476 | 2476_geral | 2 | 71 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C10D558` |
| 2477 | 2477_copia | 2 | 70 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C21DBB8` |
| 2478 | 2478_copia | 2 | 69 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C01765C` |
| 2479 | 2479_copia | 2 | 69 | 1 | 0.00 | Shenmue II (Europe) (En,Fr,D `8C048912` |
| 2480 | 2480_copia | 2 | 68 | 2 | 0.00 | Dead or Alive 2 (USA) `8C105190` |
| 2481 | 2481_copia | 2 | 68 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C01B280` |
| 2482 | 2482_copia | 2 | 66 | 2 | 0.00 | Dead or Alive 2 (USA) `8C105D20` |
| 2483 | 2483_raiz_divisao | 2 | 66 | 2 | 0.00 | Shenmue (USA) (Disc 1) `0C0948A0` |
| 2484 | 2484_copia | 2 | 65 | 2 | 0.00 | Evolution - The World of Sac `8C167C50` |
| 2485 | 2485_geral | 2 | 64 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1770D0` |
| 2486 | 2486_geral | 2 | 64 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C38BB1C` |
| 2487 | 2487_copia | 2 | 63 | 1 | 0.00 | Dead or Alive 2 (USA) `8C105B90` |
| 2488 | 2488_geral | 2 | 63 | 1 | 0.00 | Evolution - The World of Sac `8C1C4128` |
| 2489 | 2489_copia | 2 | 63 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D2F80` |
| 2490 | 2490_copia | 2 | 62 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C07BAC6` |
| 2491 | 2491_copia | 2 | 62 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D2EA8` |
| 2492 | 2492_copia | 2 | 62 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C10DAF6` |
| 2493 | 2493_geral | 2 | 62 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3760CA` |
| 2494 | 2494_copia | 2 | 61 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C36DAEC` |
| 2495 | 2495_copia | 2 | 60 | 3 | 0.00 | Skies of Arcadia (USA) (Disc `8C28BACE` |
| 2496 | 2496_geral | 2 | 60 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C39EB20` |
| 2497 | 2497_geral | 2 | 59 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D18E` |
| 2498 | 2498_geral | 2 | 59 | 1 | 0.00 | Macross M3 `8C013E0A` |
| 2499 | 2499_geral | 2 | 58 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D3EE` |
| 2500 | 2500_geral | 2 | 58 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D6DC` |
| 2501 | 2501_geral | 2 | 57 | 1 | 0.00 | Evolution - The World of Sac `8C043856` |
| 2502 | 2502_pref | 2 | 56 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C397F6E` |
| 2503 | 2503_matriz_divisao_cache | 2 | 56 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3B5660` |
| 2504 | 2504_copia | 2 | 54 | 2 | 0.00 | Dead or Alive 2 (USA) `8C104B50` |
| 2505 | 2505_divisao | 2 | 54 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C10E240` |
| 2506 | 2506_copia | 2 | 52 | 3 | 0.00 | Capcom vs. SNK 2 - Millionai `8C069DA4` |
| 2507 | 2507_copia | 2 | 52 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C06E6D2` |
| 2508 | 2508_geral | 2 | 52 | 8 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0BDD5C` |
| 2509 | 2509_geral | 2 | 52 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C13228A` |
| 2510 | 2510_geral | 2 | 51 | 1 | 0.00 | Grandia II (USA) `8C0869E4` |
| 2511 | 2511_float | 2 | 51 | 2 | 0.00 | Napple Tale - Arsia in Daydr `8C14AA40` |
| 2512 | 2512_copia | 2 | 50 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C018D24` |
| 2513 | 2513_copia | 2 | 50 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C3598CC` |
| 2514 | 2514_copia | 2 | 49 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C01752E` |
| 2515 | 2515_geral | 2 | 49 | 1 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C189D82` |
| 2516 | 2516_geral | 2 | 48 | 1 | 0.00 | Dead or Alive 2 (USA) `8C11B020` |
| 2517 | 2517_geral | 2 | 48 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D2BA` |
| 2518 | 2518_geral | 2 | 48 | 1 | 0.00 | Evolution - The World of Sac `8C1BA324` |
| 2519 | 2519_geral | 2 | 48 | 1 | 0.00 | King of Fighters The - Evolu `8C338C40` |
| 2520 | 2520_copia | 2 | 47 | 1 | 0.00 | Macross M3 `8C013AFE` |
| 2521 | 2521_geral | 2 | 47 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C167C54` |
| 2522 | 2522_geral | 2 | 47 | 1 | 0.00 | Shenmue II (Europe) (En,Fr,D `8C03E7E2` |
| 2523 | 2523_copia | 2 | 46 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05B47C` |
| 2524 | 2524_geral | 2 | 46 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C35A2FE` |
| 2525 | 2525_geral | 2 | 45 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C066846` |
| 2526 | 2526_geral | 2 | 45 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C070A66` |
| 2527 | 2527_geral | 2 | 45 | 1 | 0.00 | Grandia II (USA) `8C077F10` |
| 2528 | 2528_geral | 2 | 45 | 2 | 0.00 | Grandia II (USA) `8C10E2C4` |
| 2529 | 2529_geral | 2 | 45 | 1 | 0.00 | King of Fighters The - Evolu `8C338B76` |
| 2530 | 2530_geral | 2 | 45 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C165C4C` |
| 2531 | 2531_geral | 2 | 45 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C35A824` |
| 2532 | 2532_geral | 2 | 44 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D128` |
| 2533 | 2533_geral | 2 | 44 | 1 | 0.00 | King of Fighters The - Evolu `8C311C0C` |
| 2534 | 2534_geral | 2 | 43 | 2 | 0.00 | Evolution 2 - Far Off Promis `8C09C83E` |
| 2535 | 2535_geral | 2 | 43 | 2 | 0.00 | Shenmue (USA) (Disc 1) `0C08CAF8` |
| 2536 | 2536_copia | 2 | 42 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D064` |
| 2537 | 2537_geral | 2 | 42 | 3 | 0.00 | Soulcalibur (USA) `8C21BA8C` |
| 2538 | 2538_float | 2 | 42 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C398F98` |
| 2539 | 2539_laco | 2 | 42 | 1 | 0.00 | Shenmue (USA) (Disc 1) `0C08CA84` |
| 2540 | 2540_geral | 2 | 41 | 1 | 0.00 | King of Fighters The - Evolu `8C3A1C44` |
| 2541 | 2541_copia | 2 | 41 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0175DC` |
| 2542 | 2542_geral | 2 | 41 | 5 | 0.00 | Phantasy Star Online Ver. 2  `8C193E54` |
| 2543 | 2543_geral | 2 | 41 | 3 | 0.00 | Soulcalibur (USA) `8C21B23C` |
| 2544 | 2544_laco | 2 | 40 | 1 | 0.00 | Dead or Alive 2 (USA) `8C104980` |
| 2545 | 2545_copia | 2 | 40 | 1 | 0.00 | Evolution - The World of Sac `8C0362DE` |
| 2546 | 2546_geral | 2 | 40 | 1 | 0.00 | Grandia II (USA) `8C079CBC` |
| 2547 | 2547_geral | 2 | 40 | 2 | 0.00 | King of Fighters The - Evolu `8C31CDA4` |
| 2548 | 2548_copia | 2 | 40 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C21C4E6` |
| 2549 | 2549_geral | 2 | 40 | 2 | 0.00 | Shenmue (USA) (Disc 1) `0C1D1D10` |
| 2550 | 2550_geral | 2 | 39 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D016` |
| 2551 | 2551_copia | 2 | 39 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C05CB0C` |
| 2552 | 2552_geral | 2 | 39 | 1 | 0.00 | Evolution - The World of Sac `8C1AF664` |
| 2553 | 2553_geral | 2 | 39 | 2 | 0.00 | Evolution - The World of Sac `8C1AF770` |
| 2554 | 2554_geral | 2 | 39 | 1 | 0.00 | Shenmue (USA) (Disc 1) `0C08CB58` |
| 2555 | 2555_copia | 2 | 39 | 2 | 0.00 | Shenmue II (Europe) (En,Fr,D `8C04F378` |
| 2556 | 2556_copia | 2 | 39 | 1 | 0.00 | Shenmue II (Europe) (En,Fr,D `8C048F4A` |
| 2557 | 2557_copia | 2 | 38 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C069500` |
| 2558 | 2558_preenche | 2 | 38 | 3 | 0.00 | Capcom vs. SNK 2 - Millionai `8C077720` |
| 2559 | 2559_geral | 2 | 38 | 1 | 0.00 | Dead or Alive 2 (USA) `8C11348A` |
| 2560 | 2560_geral | 2 | 38 | 1 | 0.00 | Dead or Alive 2 (USA) `8C130F80` |
| 2561 | 2561_geral | 2 | 38 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C06CE3A` |
| 2562 | 2562_geral | 2 | 38 | 2 | 0.00 | Evolution - The World of Sac `8C1AF6E8` |
| 2563 | 2563_geral | 2 | 38 | 1 | 0.00 | Evolution - The World of Sac `8C1AF880` |
| 2564 | 2564_geral | 2 | 38 | 2 | 0.00 | Evolution - The World of Sac `8C1AF990` |
| 2565 | 2565_copia | 2 | 37 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0174D4` |
| 2566 | 2566_matriz | 2 | 37 | 1 | 0.00 | Shenmue (USA) (Disc 1) `0C094010` |
| 2567 | 2567_copia | 2 | 36 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0778A4` |
| 2568 | 2568_copia | 2 | 36 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C01D040` |
| 2569 | 2569_geral | 2 | 36 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C10C750` |
| 2570 | 2570_geral | 2 | 35 | 1 | 0.00 | Evolution - The World of Sac `8C1AFAA0` |
| 2571 | 2571_geral | 2 | 35 | 1 | 0.00 | Evolution - The World of Sac `8C1AFB18` |
| 2572 | 2572_geral | 2 | 35 | 1 | 0.00 | Grandia II (USA) `8C026F54` |
| 2573 | 2573_copia | 2 | 35 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C220326` |
| 2574 | 2574_geral | 2 | 35 | 1 | 0.00 | Macross M3 `8C1D299C` |
| 2575 | 2575_float | 2 | 35 | 1 | 0.00 | Shenmue (USA) (Disc 1) `0C08F2A8` |
| 2576 | 2576_geral | 2 | 34 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D7EC` |
| 2577 | 2577_copia | 2 | 34 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C010F44` |
| 2578 | 2578_geral | 2 | 33 | 1 | 0.00 | Dead or Alive 2 (USA) `8C105E10` |
| 2579 | 2579_geral | 2 | 33 | 1 | 0.00 | Dead or Alive 2 (USA) `8C106450` |
| 2580 | 2580_geral | 2 | 33 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08A016` |
| 2581 | 2581_geral | 2 | 33 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DABC` |
| 2582 | 2582_geral | 2 | 33 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C0753C4` |
| 2583 | 2583_geral | 2 | 33 | 1 | 0.00 | Grandia II (USA) `8C02B344` |
| 2584 | 2584_geral | 2 | 33 | 2 | 0.00 | King of Fighters The - Evolu `8C320850` |
| 2585 | 2585_float | 2 | 33 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C391314` |
| 2586 | 2586_geral | 2 | 33 | 1 | 0.00 | Power Stone (USA) `0C0DFF60` |
| 2587 | 2587_copia | 2 | 33 | 1 | 0.00 | Shenmue II (Europe) (En,Fr,D `8C04AB70` |
| 2588 | 2588_copia | 2 | 32 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06CE8A` |
| 2589 | 2589_geral | 2 | 32 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D77A` |
| 2590 | 2590_geral | 2 | 32 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06CD92` |
| 2591 | 2591_geral | 2 | 32 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089DF8` |
| 2592 | 2592_geral | 2 | 32 | 1 | 0.00 | Evolution - The World of Sac `8C1A65FC` |
| 2593 | 2593_geral | 2 | 32 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C018DB0` |
| 2594 | 2594_geral | 2 | 32 | 2 | 0.00 | Resident Evil - Code - Veron `8C1C8742` |
| 2595 | 2595_copia | 2 | 31 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0694B4` |
| 2596 | 2596_geral | 2 | 31 | 2 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0779AA` |
| 2597 | 2597_copia | 2 | 31 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F3392` |
| 2598 | 2598_copia | 2 | 31 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C024558` |
| 2599 | 2599_copia | 2 | 31 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C080874` |
| 2600 | 2600_geral | 2 | 31 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09057E` |
| 2601 | 2601_divisao | 2 | 31 | 1 | 0.00 | Evolution - The World of Sac `8C06A6C4` |
| 2602 | 2602_geral | 2 | 31 | 1 | 0.00 | Evolution - The World of Sac `8C15FFC4` |
| 2603 | 2603_geral | 2 | 31 | 1 | 0.00 | Grandia II (USA) `8C10E04C` |
| 2604 | 2604_copia | 2 | 31 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C38617C` |
| 2605 | 2605_divisao | 2 | 30 | 1 | 0.00 | Grandia II (USA) `8C07749C` |
| 2606 | 2606_geral | 2 | 30 | 1 | 0.00 | Shenmue (USA) (Disc 1) `AC0044D4` |
| 2607 | 2607_geral | 2 | 29 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D0C6` |
| 2608 | 2608_geral | 2 | 29 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A5540` |
| 2609 | 2609_float | 2 | 29 | 1 | 0.00 | Grandia II (USA) `8C028940` |
| 2610 | 2610_geral | 2 | 29 | 1 | 0.00 | Grandia II (USA) `8C10E0C4` |
| 2611 | 2611_geral | 2 | 29 | 2 | 0.00 | King of Fighters The - Evolu `8C3433AE` |
| 2612 | 2612_geral | 2 | 29 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C10CD78` |
| 2613 | 2613_geral | 2 | 29 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C13165C` |
| 2614 | 2614_geral | 2 | 29 | 1 | 0.00 | Shenmue (USA) (Disc 1) `AC004530` |
| 2615 | 2615_geral | 2 | 28 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089D5E` |
| 2616 | 2616_geral | 2 | 28 | 1 | 0.00 | Skies of Arcadia (USA) (Disc `8C255558` |
| 2617 | 2617_geral | 2 | 27 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0779E8` |
| 2618 | 2618_geral | 2 | 27 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06CDD2` |
| 2619 | 2619_geral | 2 | 27 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089DA6` |
| 2620 | 2620_geral | 2 | 27 | 1 | 0.00 | King of Fighters The - Evolu `8C313746` |
| 2621 | 2621_copia | 2 | 27 | 1 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C18A74E` |
| 2622 | 2622_geral | 2 | 27 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C0AC65C` |
| 2623 | 2623_geral | 2 | 27 | 1 | 0.00 | Resident Evil - Code - Veron `8C17D37E` |
| 2624 | 2624_copia | 2 | 26 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C174E66` |
| 2625 | 2625_copia | 2 | 26 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C06E630` |
| 2626 | 2626_geral | 2 | 26 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08A090` |
| 2627 | 2627_geral | 2 | 26 | 1 | 0.00 | Evolution - The World of Sac `8C1A8494` |
| 2628 | 2628_geral | 2 | 26 | 1 | 0.00 | King of Fighters The - Evolu `8C313BF8` |
| 2629 | 2629_geral | 2 | 26 | 1 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C123D40` |
| 2630 | 2630_contexto | 2 | 26 | 1 | 0.00 | Shenmue (USA) (Disc 1) `AC004000` |
| 2631 | 2631_geral | 2 | 25 | 1 | 0.00 | Evolution - The World of Sac `8C1AC364` |
| 2632 | 2632_geral | 2 | 25 | 1 | 0.00 | Evolution - The World of Sac `8C009858` |
| 2633 | 2633_float | 2 | 25 | 1 | 0.00 | Evolution - The World of Sac `8C032344` |
| 2634 | 2634_geral | 2 | 25 | 1 | 0.00 | Evolution - The World of Sac `8C164436` |
| 2635 | 2635_geral | 2 | 25 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A591E` |
| 2636 | 2636_copia | 2 | 25 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C0F0EF0` |
| 2637 | 2637_geral | 2 | 25 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C109944` |
| 2638 | 2638_geral | 2 | 24 | 2 | 0.00 | Elemental Gimmick Gear v1.00 `8C089E6C` |
| 2639 | 2639_geral | 2 | 24 | 1 | 0.00 | Evolution - The World of Sac `8C15C8EE` |
| 2640 | 2640_float | 2 | 24 | 1 | 0.00 | Grandia II (USA) `8C04B9FC` |
| 2641 | 2641_geral | 2 | 23 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C07453A` |
| 2642 | 2642_geral | 2 | 23 | 1 | 0.00 | Dead or Alive 2 (USA) `8C104B00` |
| 2643 | 2643_geral | 2 | 23 | 1 | 0.00 | Dead or Alive 2 (USA) `8C1300A0` |
| 2644 | 2644_geral | 2 | 23 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DA30` |
| 2645 | 2645_produto_escalar | 2 | 23 | 1 | 0.00 | Evolution - The World of Sac `8C1A4A78` |
| 2646 | 2646_geral | 2 | 23 | 1 | 0.00 | Grandia II (USA) `8C03BE1C` |
| 2647 | 2647_geral | 2 | 23 | 1 | 0.00 | Grandia II (USA) `8C040104` |
| 2648 | 2648_geral | 2 | 23 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C21C5D4` |
| 2649 | 2649_geral | 2 | 23 | 8 | 0.00 | Le Mans 24 Hours (Europe) (E `8C061054` |
| 2650 | 2650_geral | 2 | 23 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C149214` |
| 2651 | 2651_geral | 2 | 23 | 1 | 0.00 | Resident Evil - Code - Veron `8C17D348` |
| 2652 | 2652_geral | 2 | 23 | 1 | 0.00 | Shenmue (USA) (Disc 1) `AC004268` |
| 2653 | 2653_geral | 2 | 22 | 2 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C0363CA` |
| 2654 | 2654_preenche | 2 | 22 | 2 | 0.00 | Project Justice (USA) `0C145F20` |
| 2655 | 2655_geral | 2 | 22 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089C8E` |
| 2656 | 2656_geral | 2 | 22 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089F36` |
| 2657 | 2657_float | 2 | 22 | 1 | 0.00 | Evolution - The World of Sac `8C032376` |
| 2658 | 2658_divisao | 2 | 22 | 1 | 0.00 | Evolution - The World of Sac `8C1996AC` |
| 2659 | 2659_geral | 2 | 22 | 2 | 0.00 | Skies of Arcadia (USA) (Disc `8C25F91C` |
| 2660 | 2660_geral | 2 | 22 | 1 | 0.00 | Grandia II (USA) `8C07739C` |
| 2661 | 2661_copia | 2 | 21 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C174E14` |
| 2662 | 2662_geral | 2 | 21 | 1 | 0.00 | Dead or Alive 2 (USA) `8C106350` |
| 2663 | 2663_geral | 2 | 21 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D57C` |
| 2664 | 2664_geral | 2 | 21 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D50C` |
| 2665 | 2665_divisao | 2 | 21 | 1 | 0.00 | Evolution - The World of Sac `8C031D00` |
| 2666 | 2666_geral | 2 | 21 | 1 | 0.00 | Evolution - The World of Sac `8C03C008` |
| 2667 | 2667_geral | 2 | 21 | 1 | 0.00 | Evolution - The World of Sac `8C1F5B3E` |
| 2668 | 2668_geral | 2 | 21 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C178A04` |
| 2669 | 2669_geral | 2 | 21 | 1 | 0.00 | King of Fighters The - Evolu `8C31E59C` |
| 2670 | 2670_float | 2 | 21 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C14AA16` |
| 2671 | 2671_geral | 2 | 21 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C14AB8C` |
| 2672 | 2672_geral | 2 | 20 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C16F4F0` |
| 2673 | 2673_geral | 2 | 20 | 1 | 0.00 | Dead or Alive 2 (USA) `8C105C50` |
| 2674 | 2674_geral | 2 | 20 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C090658` |
| 2675 | 2675_geral | 2 | 20 | 1 | 0.00 | Evolution - The World of Sac `8C03CB0C` |
| 2676 | 2676_geral | 2 | 20 | 1 | 0.00 | Grandia II (USA) `8C026FB6` |
| 2677 | 2677_geral | 2 | 20 | 2 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0A3B74` |
| 2678 | 2678_geral | 2 | 20 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C35A998` |
| 2679 | 2679_geral | 2 | 19 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C174DE6` |
| 2680 | 2680_geral | 2 | 19 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DA7E` |
| 2681 | 2681_geral | 2 | 19 | 1 | 0.00 | Evolution - The World of Sac `8C1A696A` |
| 2682 | 2682_geral | 2 | 19 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C220202` |
| 2683 | 2683_geral | 2 | 19 | 1 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C108794` |
| 2684 | 2684_geral | 2 | 19 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C011560` |
| 2685 | 2685_geral | 2 | 19 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C131636` |
| 2686 | 2686_geral | 2 | 19 | 1 | 0.00 | Shenmue (USA) (Disc 1) `AC00479A` |
| 2687 | 2687_pref | 2 | 18 | 1 | 0.00 | Dead or Alive 2 (USA) `8C10ADC0` |
| 2688 | 2688_divisao | 2 | 18 | 1 | 0.00 | Dead or Alive 2 (USA) `8C0FC960` |
| 2689 | 2689_copia | 2 | 18 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D680` |
| 2690 | 2690_geral | 2 | 18 | 1 | 0.00 | Evolution - The World of Sac `8C00940A` |
| 2691 | 2691_geral | 2 | 18 | 1 | 0.00 | Evolution - The World of Sac `8C15FEE0` |
| 2692 | 2692_geral | 2 | 18 | 1 | 0.00 | Evolution - The World of Sac `8C1A66D2` |
| 2693 | 2693_pref | 2 | 18 | 1 | 0.00 | Grandia II (USA) `8C03AA50` |
| 2694 | 2694_geral | 2 | 18 | 1 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C18031C` |
| 2695 | 2695_float | 2 | 18 | 1 | 0.00 | Shenmue (USA) (Disc 1) `0C08F170` |
| 2696 | 2696_copia | 2 | 18 | 1 | 0.00 | Shenmue (USA) (Disc 1) `0C08CBC8` |
| 2697 | 2697_float | 2 | 18 | 1 | 0.00 | Shenmue (USA) (Disc 1) `0C08D800` |
| 2698 | 2698_geral | 2 | 18 | 1 | 0.00 | Shenmue II (Europe) (En,Fr,D `8C04117E` |
| 2699 | 2699_geral | 2 | 17 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0734F8` |
| 2700 | 2700_geral | 2 | 17 | 1 | 0.00 | King of Fighters The - Evolu `8C348458` |
| 2701 | 2701_geral | 2 | 17 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C21C47E` |
| 2702 | 2702_geral | 2 | 17 | 2 | 0.00 | Phantasy Star Online Ver. 2  `8C18EC64` |
| 2703 | 2703_geral | 2 | 17 | 1 | 0.00 | Macross M3 `8C1C4F1E` |
| 2704 | 2704_geral | 2 | 17 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C167C04` |
| 2705 | 2705_geral | 2 | 17 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C02A820` |
| 2706 | 2706_geral | 2 | 16 | 3 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0BBAFC` |
| 2707 | 2707_geral | 2 | 16 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C177D2E` |
| 2708 | 2708_geral | 2 | 16 | 1 | 0.00 | Dead or Alive 2 (USA) `8C1013B0` |
| 2709 | 2709_geral | 2 | 16 | 1 | 0.00 | Grandia II (USA) `8C0FDB16` |
| 2710 | 2710_geral | 2 | 16 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C167CEC` |
| 2711 | 2711_geral | 2 | 16 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C109924` |
| 2712 | 2712_geral | 2 | 16 | 2 | 0.00 | Shenmue (USA) (Disc 1) `0C0913A0` |
| 2713 | 2713_geral | 2 | 15 | 1 | 0.00 | Dead or Alive 2 (USA) `8C100CE0` |
| 2714 | 2714_geral | 2 | 15 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1A2C50` |
| 2715 | 2715_geral | 2 | 15 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C044A92` |
| 2716 | 2716_geral | 2 | 14 | 1 | 0.00 | Dead or Alive 2 (USA) `8C0F9DE0` |
| 2717 | 2717_geral | 2 | 14 | 1 | 0.00 | Dead or Alive 2 (USA) `8C0FC920` |
| 2718 | 2718_geral | 2 | 14 | 1 | 0.00 | Dead or Alive 2 (USA) `8C0FC940` |
| 2719 | 2719_float | 2 | 14 | 1 | 0.00 | Dead or Alive 2 (USA) `8C0FE6A0` |
| 2720 | 2720_geral | 2 | 14 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06CE12` |
| 2721 | 2721_geral | 2 | 14 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C010568` |
| 2722 | 2722_geral | 2 | 14 | 1 | 0.00 | Grandia II (USA) `8C02943A` |
| 2723 | 2723_geral | 2 | 14 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C359D78` |
| 2724 | 2724_geral | 2 | 14 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C013C30` |
| 2725 | 2725_float | 2 | 14 | 1 | 0.00 | Project Justice (USA) `0C148850` |
| 2726 | 2726_geral | 2 | 13 | 1 | 0.00 | Dead or Alive 2 (USA) `8C0F2CCA` |
| 2727 | 2727_geral | 2 | 13 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06CED2` |
| 2728 | 2728_geral | 2 | 13 | 1 | 0.00 | Evolution - The World of Sac `8C1AA796` |
| 2729 | 2729_preenche | 2 | 13 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `AC01EB6E` |
| 2730 | 2730_geral | 2 | 13 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C03BC20` |
| 2731 | 2731_geral | 2 | 13 | 1 | 0.00 | Resident Evil - Code - Veron `8C1976D4` |
| 2732 | 2732_laco | 2 | 13 | 2 | 0.00 | Shenmue (USA) (Disc 1) `0C1BFD70` |
| 2733 | 2733_float | 2 | 13 | 1 | 0.00 | Shenmue (USA) (Disc 1) `0C093F40` |
| 2734 | 2734_copia | 2 | 13 | 1 | 0.00 | Shenmue (USA) (Disc 1) `AC0048F6` |
| 2735 | 2735_geral | 2 | 12 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C1F3D54` |
| 2736 | 2736_geral | 2 | 12 | 1 | 0.00 | Dead or Alive 2 (USA) `8C0FA6F0` |
| 2737 | 2737_geral | 2 | 12 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DBE6` |
| 2738 | 2738_geral | 2 | 12 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DC26` |
| 2739 | 2739_geral | 2 | 12 | 1 | 0.00 | Evolution - The World of Sac `8C1D4442` |
| 2740 | 2740_geral | 2 | 12 | 1 | 0.00 | Grandia II (USA) `8C035918` |
| 2741 | 2741_geral | 2 | 12 | 1 | 0.00 | King of Fighters The - Evolu `8C35C196` |
| 2742 | 2742_geral | 2 | 12 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C010FF0` |
| 2743 | 2743_laco | 2 | 12 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0112C4` |
| 2744 | 2744_geral | 2 | 12 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C12CAF8` |
| 2745 | 2745_copia | 2 | 12 | 1 | 0.00 | Shenmue (USA) (Disc 1) `AC010000` |
| 2746 | 2746_geral | 2 | 11 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C06C272` |
| 2747 | 2747_geral | 2 | 11 | 1 | 0.00 | Dead or Alive 2 (USA) `8C0FA6C0` |
| 2748 | 2748_geral | 2 | 11 | 1 | 0.00 | Dead or Alive 2 (USA) `8C125CE8` |
| 2749 | 2749_geral | 2 | 11 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06D36C` |
| 2750 | 2750_geral | 2 | 11 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DB50` |
| 2751 | 2751_preenche | 2 | 11 | 2 | 0.00 | King of Fighters The - Evolu `8C017912` |
| 2752 | 2752_geral | 2 | 11 | 1 | 0.00 | Macross M3 `8C1D2970` |
| 2753 | 2753_float | 2 | 11 | 1 | 0.00 | Napple Tale - Arsia in Daydr `8C1441D8` |
| 2754 | 2754_preenche | 2 | 11 | 1 | 0.00 | Shenmue (USA) (Disc 1) `AC004910` |
| 2755 | 2755_geral | 2 | 11 | 1 | 0.00 | Shenmue (USA) (Disc 1) `AC00493A` |
| 2756 | 2756_geral | 2 | 11 | 1 | 0.00 | Skies of Arcadia (USA) (Disc `8C1AF400` |
| 2757 | 2757_geral | 2 | 10 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0B7CA6` |
| 2758 | 2758_geral | 2 | 10 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089D42` |
| 2759 | 2759_geral | 2 | 10 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08A06E` |
| 2760 | 2760_geral | 2 | 10 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C08A0DC` |
| 2761 | 2761_geral | 2 | 10 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C089C50` |
| 2762 | 2762_geral | 2 | 10 | 1 | 0.00 | Grandia II (USA) `8C08603E` |
| 2763 | 2763_geral | 2 | 10 | 1 | 0.00 | King of Fighters The - Evolu `8C128CC2` |
| 2764 | 2764_geral | 2 | 10 | 1 | 0.00 | King of Fighters The - Evolu `8C338B2C` |
| 2765 | 2765_geral | 2 | 10 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0152FC` |
| 2766 | 2766_geral | 2 | 10 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C0ED854` |
| 2767 | 2767_geral | 2 | 10 | 1 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C04185A` |
| 2768 | 2768_geral | 2 | 10 | 1 | 0.00 | Marvel vs. Capcom 2 - New Ag `8C10889C` |
| 2769 | 2769_divisao | 2 | 10 | 1 | 0.00 | Shenmue (USA) (Disc 1) `0C091648` |
| 2770 | 2770_geral | 2 | 9 | 1 | 0.00 | Dead or Alive 2 (USA) `8C1013E0` |
| 2771 | 2771_geral | 2 | 9 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09C600` |
| 2772 | 2772_geral | 2 | 9 | 1 | 0.00 | Evolution - The World of Sac `8C03CAFA` |
| 2773 | 2773_geral | 2 | 9 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1ADA9E` |
| 2774 | 2774_geral | 2 | 9 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C06CBC0` |
| 2775 | 2775_geral | 2 | 9 | 1 | 0.00 | Macross M3 `8C1F5C1C` |
| 2776 | 2776_geral | 2 | 9 | 1 | 0.00 | Phantasy Star Online Ver. 2  `8C012D70` |
| 2777 | 2777_geral | 2 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C011220` |
| 2778 | 2778_geral | 2 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C070524` |
| 2779 | 2779_geral | 2 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C070534` |
| 2780 | 2780_geral | 2 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C086904` |
| 2781 | 2781_geral | 2 | 8 | 1 | 0.00 | Capcom vs. SNK 2 - Millionai `8C0AD01E` |
| 2782 | 2782_geral | 2 | 8 | 1 | 0.00 | Dead or Alive 2 (USA) `8C0FA6E0` |
| 2783 | 2783_float | 2 | 8 | 1 | 0.00 | Dead or Alive 2 (USA) `8C0E1D5C` |
| 2784 | 2784_geral | 2 | 8 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C06DBD2` |
| 2785 | 2785_geral | 2 | 8 | 1 | 0.00 | Elemental Gimmick Gear v1.00 `8C09C6FC` |
| 2786 | 2786_geral | 2 | 8 | 1 | 0.00 | Evolution - The World of Sac `8C04ECAC` |
| 2787 | 2787_geral | 2 | 8 | 1 | 0.00 | Evolution - The World of Sac `8C04ECBC` |
| 2788 | 2788_geral | 2 | 8 | 1 | 0.00 | Evolution - The World of Sac `8C1CFAAE` |
| 2789 | 2789_geral | 2 | 8 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D857C` |
| 2790 | 2790_geral | 2 | 8 | 1 | 0.00 | Evolution 2 - Far Off Promis `8C1D8976` |
| 2791 | 2791_geral | 2 | 8 | 1 | 0.00 | Grandia II (USA) `8C3E3C64` |
| 2792 | 2792_geral | 2 | 8 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C21F4B6` |
| 2793 | 2793_geral | 2 | 8 | 1 | 0.00 | Le Mans 24 Hours (Europe) (E `8C063C4E` |
| 2794 | 2794_geral | 2 | 8 | 1 | 0.00 | Macross M3 `8C1F8662` |
| 2795 | 2795_geral | 2 | 8 | 1 | 0.00 | Marvel vs. Capcom 2 - New Ag `0CE38E70` |
| 2796 | 2796_geral | 2 | 8 | 1 | 0.00 | Shenmue (USA) (Disc 1) `0C08C914` |
| 2797 | 2797_geral | 2 | 8 | 1 | 0.00 | Shenmue (USA) (Disc 1) `0C08C928` |
| 2798 | 2798_geral | 2 | 8 | 1 | 0.00 | Shenmue (USA) (Disc 1) `AC004988` |
