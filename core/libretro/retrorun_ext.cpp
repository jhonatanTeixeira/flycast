/*
    Extensao privada que o frontend RetroRun usa para aplicar perfis por jogo
    ANTES do launch. Nao faz parte da API libretro; o RetroRun faz dlsym destes
    simbolos (ver retrorun/src/core/core_loader.cpp e
    retrorun/src/config/flycast_game_catalog.cpp). So os cores "RetroRun-aware"
    exportam isto.

    flycast is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
 */

#include "types.h"
#include "imgread/common.h"

#include <algorithm>
#include <cstring>
#include <cstddef>

// Identifica a variante deste core para o RetroRun. Ele usa isto para escolher
// o prefixo das core options ao aplicar o perfil por jogo: o nosso core usa
// `flycast2026_` (CORE_OPTION_NAME), nao o `flycast_` de um Flycast "stock".
extern "C" const char* flycast_retrorun_core_variant_v1(void)
{
    return "flycast2026";
}

// O RetroRun chama isto com o caminho do conteudo ANTES de retro_load_game.
// Devolve 1 e preenche `out` com o Product number do IP.BIN (ex.: "MK-51117"),
// sem bootar o jogo. Reusa o mesmo caminho de leitura do boot (imgread): abre a
// imagem, le o setor do IP.BIN na area de baixa densidade e fecha.
extern "C" int flycast_retrorun_get_product_number_v1(const char* filename,
                                                      char* out,
                                                      size_t out_size)
{
    if (out == nullptr || out_size == 0)
        return 0;
    out[0] = '\0';
    if (filename == nullptr || filename[0] == '\0')
        return 0;

    Disc* opened = OpenDisc(filename);
    if (opened == nullptr)
        return 0;

    // GetDriveSessionInfo() (usado abaixo) le do `disc` global; empresta o
    // nosso temporariamente e devolve o original no fim.
    Disc* const saved = disc;
    disc = opened;

    // FAD da area de baixa densidade (onde fica o IP.BIN), igual ao reios:
    // GD-ROM fixo em 45150; nos demais discos vem da informacao de sessao.
    u32 base_fad = 45150;
    if (opened->type != GdRom)
    {
        // Session 0 -> aponta a ultima sessao; a sessao valida tem o StartFAD
        // nos 3 ultimos bytes, como o reios faz.
        u8 ses[6] = {};
        libGDR_GetSessionInfo(ses, 0);
        libGDR_GetSessionInfo(ses, ses[2]);
        base_fad = (ses[3] << 16) | (ses[4] << 8) | ses[5];
    }

    u8 buf[2048] = {};
    opened->ReadSectors(base_fad, 1, buf, sizeof(buf));

    disc = saved;
    delete opened;

    // O IP.BIN comeca em buf[0]: hardware_id[16], maker_id[16], ks[5],
    // disk_type[6], disk_num[5], area_symbols[8], ctrl[4], dev/vga/wince/_unk1,
    // product_number[10] -- ou seja, product_number no offset 0x40 (ver
    // reios.h ip_meta_t). Copia, corta espaco/NUL a direita e devolve.
    char product[11] = {};
    memcpy(product, buf + 0x40, 10);
    int len = 10;
    while (len > 0 && (product[len - 1] == ' ' || product[len - 1] == '\0'))
        --len;
    if (len <= 0)
        return 0;

    const size_t copy = std::min<size_t>(static_cast<size_t>(len), out_size - 1);
    memcpy(out, product, copy);
    out[copy] = '\0';
    return 1;
}
