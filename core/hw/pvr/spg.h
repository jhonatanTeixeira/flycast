#pragma once
#include "types.h"

bool spg_Init();
void spg_Term();
void spg_Reset(bool Manual);

void CalculateSync();
void read_lightgun_position(int x, int y);

// Contador de vblank em tempo EMULADO (monotonico, +1 por vblank do PVR).
// Usado so para medir/reportar o periodo NATURAL do jogo ao frontend
// (o retrorun deriva o "fps alvo" dai); nao altera a emulacao.
u32 spg_vblank_count(void);
struct TA_context;
void SetREP(TA_context* cntx);
