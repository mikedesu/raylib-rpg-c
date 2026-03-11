#pragma once

#include "libdraw_context.h"
#include "libdraw_scene_dispatch.h"
#include "libdraw_update_sprites.h"

static inline void libdraw_refresh_dirty_scene(gamestate& g) {
    if (!g.frame_dirty) {
        return;
    }

    minfo3("frame is dirty");
    libdraw_draw_current_scene_to_texture(g);
    g.frame_dirty = false;
    g.frame_updates++;
    msuccess3("frame is no longer dirty");
}

static inline void libdraw_compose_scene_to_window(gamestate& g) {
    minfo3("Begin Texture Mode target");
    BeginTextureMode(target);
    ClearBackground(BLUE);
    libdraw_draw_current_scene_from_texture(g);
    msuccess3("End Texture Mode target");
    EndTextureMode();

    win_dest.width = GetScreenWidth();
    win_dest.height = GetScreenHeight();
    DrawTexturePro(target.texture, target_src, win_dest, Vector2{0, 0}, 0.0f, WHITE);
    DrawFPS(10, 10);
}

static inline void drawframe(gamestate& g) {
    minfo3("drawframe");
#ifdef DEBUG
    const double start_time = GetTime();
#endif
    libdraw_update_sprites_pre(g);

    minfo3("begin drawing");
    BeginDrawing();
    ClearBackground(RED);
    libdraw_refresh_dirty_scene(g);
    libdraw_compose_scene_to_window(g);
    EndDrawing();
    msuccess3("end drawing");
#ifdef DEBUG
    g.last_frame_time = GetTime() - start_time;
    g.last_frame_times[g.last_frame_times_current] = g.last_frame_time;
    g.last_frame_times_current++;
    if (g.last_frame_times_current >= LAST_FRAME_TIMES_MAX) {
        g.last_frame_times_current = 0;
    }
    g.framecount++;

    if (g.last_frame_time > g.max_frame_time) {
        g.max_frame_time = g.last_frame_time;
    }
#endif

    libdraw_update_sprites_post(g);
    msuccess3("drawframe");
}
