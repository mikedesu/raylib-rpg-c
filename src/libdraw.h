#pragma once

/** @file libdraw.h
 *  @brief Top-level composition include for the renderer subsystem.
 */

#include "libdraw_context.h"

class gamestate;

void libdraw_render_current_scene_to_scene_texture(gamestate& g);
void libdraw_draw_current_scene_from_scene_texture(gamestate& g);
void libdraw_refresh_dirty_scene(gamestate& g);
void libdraw_compose_scene_to_window_target(gamestate& g);
void libdraw_present_window_target(gamestate& g);
void drawframe(gamestate& g);
void libdraw_init_render_targets(gamestate& g);
void libdraw_init_resources(gamestate& g);
void libdraw_init_rest(gamestate& g);
void libdraw_init(gamestate& g);
bool libdraw_windowshouldclose(gamestate& g);
void libdraw_close_partial();
void libdraw_close();
