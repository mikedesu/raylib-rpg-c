#pragma once
#include "gamestate.h"
#include "inputstate.h"

void liblogic_init(gamestate* const g);
void liblogic_tick(const inputstate* const is, gamestate* const g);
void liblogic_close(gamestate* const g);
