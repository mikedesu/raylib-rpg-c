/** @file libdraw_from_texture.h
 *  @brief Helpers that draw scene-specific render textures into the active target.
 */

#pragma once

#include "libdraw_context.h"

/** @brief Draw the cached title-screen texture into the current render target. */
static inline void draw_title_screen_from_texture(gamestate& g) {
    DrawTexturePro(libdraw_ctx.title_target_texture.texture, libdraw_ctx.target_src, libdraw_ctx.target_dest, Vector2{0, 0}, 0.0f, WHITE);
}

/** @brief Draw the cached HUD texture into the current render target. */
static inline void draw_hud_from_texture(gamestate& g) {
    DrawTexturePro(libdraw_ctx.hud_target_texture.texture, libdraw_ctx.target_src, libdraw_ctx.target_dest, Vector2{0, 0}, 0.0f, WHITE);
}

/** @brief Draw the cached character-creation texture into the current render target. */
static inline void draw_char_creation_from_texture(gamestate& g) {
    DrawTexturePro(libdraw_ctx.char_creation_target_texture.texture, libdraw_ctx.target_src, libdraw_ctx.target_dest, Vector2{0, 0}, 0.0f, WHITE);
}

/** @brief Draw the cached gameplay-world texture into the current render target. */
static inline void libdraw_drawframe_2d_from_texture(gamestate& g) {
    DrawTexturePro(libdraw_ctx.main_game_target_texture.texture, libdraw_ctx.target_src, libdraw_ctx.target_dest, Vector2{0, 0}, 0.0f, WHITE);
}
