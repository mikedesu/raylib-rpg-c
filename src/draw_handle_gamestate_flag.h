/** @file draw_handle_gamestate_flag.h
 *  @brief Renderer-side helper for advancing coarse gameplay update phases.
 */

#pragma once

#include "check_default_animations.h"
#include "gamestate.h"

/** @brief Advance the high-level gameplay phase after animation/render work completes. */
void libdraw_handle_gamestate_flag(gamestate& g);
