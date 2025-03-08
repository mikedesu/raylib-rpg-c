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
void libdraw_unload_texture(const int txkey);
void libdraw_load_texture(
    const int txkey, const int contexts, const int frames, const bool do_dither, const char* path);


void libdraw_load_textures();
void libdraw_unload_textures();
