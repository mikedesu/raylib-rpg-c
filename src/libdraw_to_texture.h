#pragma once

#include "gamestate.h"
void draw_hud_to_texture(shared_ptr<gamestate> g);
void draw_character_creation_screen_to_texture(shared_ptr<gamestate> g);
void draw_title_screen_to_texture(shared_ptr<gamestate> g, bool show_menu);
void libdraw_drawframe_2d_to_texture(shared_ptr<gamestate> g);
