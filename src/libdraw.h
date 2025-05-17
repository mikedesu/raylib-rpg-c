#pragma once
#include "gamestate.h"
#include "inputstate.h"
#include "specifier.h"
#include "spritegroup.h"
#include <raylib.h>
#include <stdbool.h>

extern Shader shader_grayscale; // Global shader
extern Shader shader_tile_glow; // Global shader

void libdraw_init(gamestate* const g);
void libdraw_update_input(inputstate* const is);
void libdraw_drawframe(gamestate* const g);
void libdraw_close();
//bool libdraw_windowshouldclose();
bool libdraw_windowshouldclose(const gamestate* const g);
void libdraw_update_sprites(gamestate* const g);

//void libdraw_draw_message_history_placeholder(gamestate* const g);
//void libdraw_update_sprite_context(gamestate* const g, entityid id, direction_t dir);
