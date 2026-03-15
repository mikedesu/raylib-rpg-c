/** @file libdraw_window_present.h
 *  @brief Dirty-scene refresh and final window-presentation helpers.
 */

#pragma once

#include "gamestate.h"

void libdraw_refresh_dirty_scene(gamestate& g);
void libdraw_compose_scene_to_window_target(gamestate& g);
void libdraw_present_window_target(gamestate& g);
