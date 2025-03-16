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
void libdraw_load_texture(const int txkey,
                          const int contexts,
                          const int frames,
                          const bool do_dither,
                          const char* path);
void libdraw_load_textures();
void libdraw_unload_textures();

// may or may not deprecate specifier_t
void libdraw_create_spritegroup(gamestate* const g,
                                entityid id,
                                int* keys,
                                int num_keys,
                                int offset_x,
                                int offset_y,
                                specifier_t spec);

void libdraw_update_sprites(gamestate* const g);
void libdraw_update_sprite(gamestate* const g, entityid id);
void libdraw_draw_sprite(const gamestate* const g, const entityid id);
void libdraw_draw_sprite_and_shadow(const gamestate* const g, entityid id);
void libdraw_draw_dungeon_floor(const gamestate* const g);
//void libdraw_draw_dungeon_floor_tile(const gamestate* const g, const int x, const int y);
void libdraw_draw_dungeon_floor_tile(const gamestate* const g, dungeon_floor_t* const df, const int x, const int y);
void libdraw_draw_debug_panel(gamestate* const g);
void libdraw_calc_debugpanel_size(gamestate* const g);
void libdraw_update_sprite_context(gamestate* const g, entityid id, direction_t dir);
void libdraw_create_sg_byid(gamestate* const g, entityid id);
