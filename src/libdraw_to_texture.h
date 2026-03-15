/** @file libdraw_to_texture.h
 *  @brief Helpers that render scenes and overlays into off-screen textures.
 */

#pragma once

#include "draw_character_creation_screen.h"
#include "draw_frame_2d.h"
#include "draw_handle_debug_panel.h"
#include "draw_hud.h"
#include "draw_title_screen.h"
#include "libdraw_context.h"

/** @brief Render the HUD overlay into its dedicated off-screen texture. */
static inline void draw_hud_to_texture(gamestate& g) {
    BeginTextureMode(libdraw_ctx.hud_target_texture);
    draw_hud(g);
    EndTextureMode();
}

/** @brief Render the character-creation scene into its dedicated off-screen texture. */
static inline void draw_char_creation_to_texture(gamestate& g) {
    minfo3("draw char creation to texture");
    BeginTextureMode(libdraw_ctx.char_creation_target_texture);
    draw_character_creation_screen(g);
    handle_debug_panel(g);
    EndTextureMode();
    msuccess3("draw char creation to texture");
}

/** @brief Render the title or main-menu scene into its dedicated off-screen texture. */
static inline void draw_title_screen_to_texture(gamestate& g, bool show_menu) {
    BeginTextureMode(libdraw_ctx.title_target_texture);
    draw_title_screen(g, show_menu);
    handle_debug_panel(g);
    EndTextureMode();
}

/** @brief Render the gameplay world scene into its dedicated off-screen texture. */
static inline void libdraw_drawframe_2d_to_texture(gamestate& g, int vision_dist, int light_rad) {
    BeginTextureMode(libdraw_ctx.main_game_target_texture);

    libdraw_drawframe_2d(g, vision_dist, light_rad);

    EndTextureMode();
}
