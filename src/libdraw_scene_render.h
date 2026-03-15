/** @file libdraw_scene_render.h
 *  @brief Scene-to-texture render dispatch helpers.
 */

#pragma once

#include "gamestate.h"

void libdraw_render_current_scene_to_scene_texture(gamestate& g);
void libdraw_draw_current_scene_from_scene_texture(gamestate& g);
