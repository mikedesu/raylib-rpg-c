#pragma once

#include "gamestate.h"

extern RenderTexture2D title_target_texture;
extern Rectangle target_src;
extern Rectangle target_dest;
extern RenderTexture2D char_creation_target_texture;
extern RenderTexture2D hud_target_texture;
extern RenderTexture2D main_game_target_texture;

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
