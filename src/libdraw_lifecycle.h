/** @file libdraw_lifecycle.h
 *  @brief Renderer/window initialization and shutdown helpers.
 */

#pragma once

#include "gamestate.h"

void libdraw_init_render_targets(gamestate& g);
void libdraw_init_resources(gamestate& g);
void libdraw_init_rest(gamestate& g);
void libdraw_init(gamestate& g);
bool libdraw_windowshouldclose(gamestate& g);
void libdraw_close_partial();
void libdraw_close();
