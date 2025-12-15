#pragma once

#include "gamestate.h"

#include "draw_character_creation_screen.h"
#include "draw_hud.h"
#include "draw_message_history.h"
#include "draw_title_screen.h"
#include "libdraw_create_spritegroup.h"
#include "load_textures.h"
#include "unload_textures.h"

bool libdraw_windowshouldclose(shared_ptr<gamestate> g);

void libdraw_init(shared_ptr<gamestate> g);
void libdraw_init_rest(shared_ptr<gamestate> g);
void libdraw_drawframe(shared_ptr<gamestate> g);
void libdraw_close();
void libdraw_close_partial();
void libdraw_update_sprites(shared_ptr<gamestate> g);
//void libdraw_update_sprites_post(shared_ptr<gamestate> g);
