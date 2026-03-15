/** @file draw_handle_debug_panel.h
 *  @brief Debug-panel update-and-draw orchestration helper.
 */

#pragma once

#include "draw_debug_panel.h"
#include "draw_update_debug_panel.h"
#include "gamestate.h"

/** @brief Refresh and draw the debug panel when it is enabled. */
void handle_debug_panel(gamestate& g);
