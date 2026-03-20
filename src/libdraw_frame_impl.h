/** @file libdraw_frame_impl.h
 *  @brief Implementation for the top-level per-frame renderer entry point.
 */

#pragma once

#include "libdraw_context.h"
#include "libdraw_frame.h"
#include "libdraw_frame_stats.h"
#include "libdraw_update_sprites.h"
#include "libdraw_window_present.h"

/** @brief Execute one full renderer frame. */
inline void drawframe(gamestate& g) {
    minfo3("drawframe");
    const double start_time = libdraw_frame_begin_time();
    libdraw_update_sprites_pre(g);

    BeginDrawing();
    ClearBackground(RED);
    libdraw_refresh_dirty_scene(g);
    libdraw_compose_scene_to_window_target(g);
    libdraw_present_window_target(g);
    EndDrawing();
    libdraw_finish_frame_stats(g, start_time);

    libdraw_update_sprites_post(g);
    msuccess3("drawframe");
}
