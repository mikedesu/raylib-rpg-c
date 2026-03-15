/** @file libdraw_to_texture.h
 *  @brief Helpers that render scenes and overlays into off-screen textures.
 */

#pragma once

#include "gamestate.h"

/** @brief Render the HUD overlay into its dedicated off-screen texture. */
void draw_hud_to_texture(gamestate& g);

/** @brief Render the character-creation scene into its dedicated off-screen texture. */
void draw_char_creation_to_texture(gamestate& g);

/** @brief Render the title or main-menu scene into its dedicated off-screen texture. */
void draw_title_screen_to_texture(gamestate& g, bool show_menu);

/** @brief Render the gameplay world scene into its dedicated off-screen texture. */
void libdraw_drawframe_2d_to_texture(gamestate& g, int vision_dist, int light_rad);
