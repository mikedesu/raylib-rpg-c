/** @file libdraw_frame.h
 *  @brief Top-level per-frame renderer entry point.
 */

#pragma once

#include "gamestate.h"

/** @brief Execute one full renderer frame. */
void drawframe(gamestate& g);
