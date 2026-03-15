/** @file libdraw_update_sprites.h
 *  @brief Pre/post-frame spritegroup and animation update helpers.
 */

#pragma once

#include "gamestate.h"

/** @brief Run pre-draw sprite and dirty-entity updates for the current frame. */
void libdraw_update_sprites_pre(gamestate& g);

/** @brief Run post-draw animation advancement and gamestate-flag handling. */
void libdraw_update_sprites_post(gamestate& g);
