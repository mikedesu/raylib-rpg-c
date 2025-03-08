#pragma once
#include "gamestate.h"
#include "inputstate.h"
#include <raylib.h>

//void libdraw_init();
//void libdraw_init(gamestate* const g);
void libdraw_init(const gamestate* const g);
void libdraw_update_input(inputstate* const is);
void libdraw_drawframe(const gamestate* const g);
void libdraw_close();
bool libdraw_windowshouldclose();
