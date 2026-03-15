/** @file draw_frame_2d.h
 *  @brief Main gameplay-scene 2D composition helper.
 */

#pragma once
#include "gamestate.h"

/** @brief Draw the gameplay world plus all active gameplay UI overlays. */
void libdraw_drawframe_2d(gamestate& g, int vision_dist, int light_rad);
