/** @file draw_update_debug_panel.h
 *  @brief Helper for rebuilding the debug panel text buffer.
 */

#pragma once

#include "gamestate.h"
#include "libdraw_context.h"
#include "spritegroup.h"

/** @brief Append the current debug text payload into the debug panel buffer. */
void update_debug_panel(gamestate& g);
