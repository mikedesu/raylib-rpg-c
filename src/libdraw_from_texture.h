#pragma once

#include "libdraw_context.h"

static inline void draw_title_screen_from_texture(gamestate& g) {
    DrawTexturePro(title_target_texture.texture, target_src, target_dest, Vector2{0, 0}, 0.0f, WHITE);
}

static inline void draw_hud_from_texture(gamestate& g) {
    DrawTexturePro(hud_target_texture.texture, target_src, target_dest, Vector2{0, 0}, 0.0f, WHITE);
}

static inline void draw_char_creation_from_texture(gamestate& g) {
    DrawTexturePro(char_creation_target_texture.texture, target_src, target_dest, Vector2{0, 0}, 0.0f, WHITE);
}

static inline void libdraw_drawframe_2d_from_texture(gamestate& g) {
    DrawTexturePro(main_game_target_texture.texture, target_src, target_dest, Vector2{0, 0}, 0.0f, WHITE);
}
