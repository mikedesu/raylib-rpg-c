#pragma once
#include "inputstate.h"
#include <raylib.h>

void libdraw_init();
void libdraw_update_input(inputstate* const is);
void libdraw_drawframe();
void libdraw_close();
bool libdraw_windowshouldclose();
