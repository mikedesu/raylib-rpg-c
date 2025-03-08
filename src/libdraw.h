#pragma once
#include "gamestate.h"
#include "inputstate.h"
#include "specifier.h"
#include <raylib.h>
#include <stdbool.h>

void libdraw_init(gamestate* const g);
void libdraw_update_input(inputstate* const is);
void libdraw_drawframe(gamestate* const g);
void libdraw_close();
bool libdraw_windowshouldclose();
void libdraw_unload_texture(const int txkey);
void libdraw_load_texture(
    const int txkey, const int contexts, const int frames, const bool do_dither, const char* path);
void libdraw_load_textures();
void libdraw_unload_textures();

void libdraw_create_spritegroup(
    gamestate* const g, entityid id, int* keys, int num_keys, int offset_x, int offset_y, specifier_t spec);
