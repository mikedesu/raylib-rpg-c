/** @file draw_debug_panel.h
 *  @brief Debug overlay panel drawing helper.
 */

#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

/** @brief Draw the on-screen debug panel using the current buffer contents. */
void draw_debug_panel(gamestate& g);
