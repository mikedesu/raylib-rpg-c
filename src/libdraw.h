#pragma once

#include "gamestate.h"
#include "inputstate.h"
#include "spritegroup.h"
#include <memory>
#include <raylib.h>

extern Shader shader_grayscale; // Global shader
extern Shader shader_tile_glow; // Global shader

using std::shared_ptr;

bool libdraw_windowshouldclose(shared_ptr<gamestate> g);

void libdraw_init(shared_ptr<gamestate> g);
void libdraw_init_rest(shared_ptr<gamestate> g);
void libdraw_drawframe(shared_ptr<gamestate> g);
void libdraw_close();
void libdraw_close_partial();
void libdraw_update_sprites(shared_ptr<gamestate> g);
void libdraw_update_sprites_post(shared_ptr<gamestate> g);
void libdraw_update_sprites_pre(shared_ptr<gamestate> g);
