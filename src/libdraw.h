#pragma once

#include "gamestate.h"

#include "libdraw_create_spritegroup.h"
#include "libdraw_draw_character_creation_screen.h"
#include "libdraw_draw_inventory_menu.h"
#include "libdraw_help_menu.h"
#include "libdraw_hud.h"
#include "libdraw_load_textures.h"
#include "libdraw_message_history.h"
#include "libdraw_title_screen.h"
#include "libdraw_unload_textures.h"

bool libdraw_windowshouldclose(shared_ptr<gamestate> g);

void libdraw_init(shared_ptr<gamestate> g);
void libdraw_init_rest(shared_ptr<gamestate> g);
void libdraw_drawframe(shared_ptr<gamestate> g);
void libdraw_close();
void libdraw_close_partial();
void libdraw_update_sprites(shared_ptr<gamestate> g);
void libdraw_update_sprites_post(shared_ptr<gamestate> g);
void libdraw_update_sprites_pre(shared_ptr<gamestate> g);
