
/** @file draw_sprite.h
 *  @brief Composite entity/equipment sprite drawing helper.
 */

#pragma once

#include "gamestate.h"

/** @brief Draw an entity sprite together with any equipped shield and weapon layers. */
void draw_sprite_and_shadow(gamestate& g, entityid id);
