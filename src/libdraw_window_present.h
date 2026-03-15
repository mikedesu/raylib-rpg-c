/** @file libdraw_window_present.h
 *  @brief Dirty-scene refresh and final window-presentation helpers.
 */

#pragma once

#include "libdraw_context.h"
#include "libdraw_scene_render.h"

/**
 * @brief Re-render the active scene only when the cached frame is dirty.
 *
 * @param g Active game state controlling frame dirtiness.
 */
static inline void libdraw_refresh_dirty_scene(gamestate& g) {
    if (!g.frame_dirty) {
        return;
    }

    minfo3("frame is dirty");
    libdraw_render_current_scene_to_scene_texture(g);
    g.frame_dirty = false;
    g.frame_updates++;
    msuccess3("frame is no longer dirty");
}

/**
 * @brief Compose the current scene texture into the shared window-sized target texture.
 *
 * @param g Active game state.
 */
static inline void libdraw_compose_scene_to_window_target(gamestate& g) {
    BeginTextureMode(libdraw_ctx.target);
    ClearBackground(BLUE);
    libdraw_draw_current_scene_from_scene_texture(g);
    EndTextureMode();
}

/**
 * @brief Present the shared target texture to the OS window.
 *
 * @param g Active game state.
 */
static inline void libdraw_present_window_target(gamestate& g) {
    (void)g;
    libdraw_ctx.win_dest.width = GetScreenWidth();
    libdraw_ctx.win_dest.height = GetScreenHeight();
    DrawTexturePro(libdraw_ctx.target.texture, libdraw_ctx.target_src, libdraw_ctx.win_dest, Vector2{0, 0}, 0.0f, WHITE);
    DrawFPS(10, 10);
}
