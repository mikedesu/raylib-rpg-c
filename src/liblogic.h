#pragma once

#include "gamestate.h"
#include "inputstate.h"

#define TILE_COUNT_ERROR -999

void liblogic_init(gamestate* const g);
void liblogic_close(gamestate* const g);
void liblogic_tick(const inputstate* const is, gamestate* const g);
