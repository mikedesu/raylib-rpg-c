#pragma once
#include "gamestate.h"
#include "inputstate.h"
#include "specifier.h"
#include "spritegroup.h"
#include <memory>
#include <raylib.h>

extern Shader shader_grayscale; // Global shader
extern Shader shader_tile_glow; // Global shader

bool libdraw_windowshouldclose(std::shared_ptr<gamestate> g);

void libdraw_init(std::shared_ptr<gamestate> g);
void libdraw_init_rest(std::shared_ptr<gamestate> g);
//void libdraw_update_input(std::shared_ptr<inputstate> is);
void libdraw_drawframe(std::shared_ptr<gamestate> g);
void libdraw_close();
void libdraw_close_partial();
void libdraw_update_sprites(std::shared_ptr<gamestate> g);
void libdraw_update_sprites_post(std::shared_ptr<gamestate> g);
void libdraw_update_sprites_pre(std::shared_ptr<gamestate> g);
