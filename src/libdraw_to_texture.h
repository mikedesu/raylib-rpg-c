#pragma once

#include "draw_character_creation_screen.h"
#include "draw_frame_2d.h"
#include "draw_handle_debug_panel.h"
#include "draw_hud.h"
#include "draw_title_screen.h"
#include "gamestate.h"

extern RenderTexture2D title_target_texture;
extern RenderTexture2D char_creation_target_texture;
extern RenderTexture2D hud_target_texture;
extern RenderTexture2D main_game_target_texture;

static inline void draw_hud_to_texture(gamestate& g) {
    BeginTextureMode(hud_target_texture);
    draw_hud(g);
    EndTextureMode();
}

static inline void draw_char_creation_to_texture(gamestate& g) {
    minfo3("draw char creation to texture");
    BeginTextureMode(char_creation_target_texture);
    draw_character_creation_screen(g);
    handle_debug_panel(g);
    EndTextureMode();
    msuccess3("draw char creation to texture");
}

static inline void draw_title_screen_to_texture(gamestate& g, bool show_menu) {
    BeginTextureMode(title_target_texture);
    draw_title_screen(g, show_menu);
    handle_debug_panel(g);
    EndTextureMode();
}

static inline void libdraw_drawframe_2d_to_texture(gamestate& g, int vision_dist, int light_rad) {
    BeginTextureMode(main_game_target_texture);

    libdraw_drawframe_2d(g, vision_dist, light_rad);

    EndTextureMode();
}
