/** @file libdraw_from_texture.h
 *  @brief Helpers that draw scene-specific render textures into the active target.
 */

#pragma once

#include "gamestate.h"

/** @brief Draw the cached title-screen texture into the current render target. */
void draw_title_screen_from_texture(gamestate& g);

/** @brief Draw the cached HUD texture into the current render target. */
void draw_hud_from_texture(gamestate& g);

/** @brief Draw the cached character-creation texture into the current render target. */
void draw_char_creation_from_texture(gamestate& g);

/** @brief Draw the cached gameplay-world texture into the current render target. */
void libdraw_drawframe_2d_from_texture(gamestate& g);
