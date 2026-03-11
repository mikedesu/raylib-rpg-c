#pragma once

#include "libdraw_context.h"
#include "libdraw_scene_render.h"

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

static inline void libdraw_compose_scene_to_window_target(gamestate& g) {
    BeginTextureMode(target);
    ClearBackground(BLUE);
    libdraw_draw_current_scene_from_scene_texture(g);
    EndTextureMode();
}

static inline void libdraw_present_window_target(gamestate& g) {
    (void)g;
    win_dest.width = GetScreenWidth();
    win_dest.height = GetScreenHeight();
    DrawTexturePro(target.texture, target_src, win_dest, Vector2{0, 0}, 0.0f, WHITE);
    DrawFPS(10, 10);
}
