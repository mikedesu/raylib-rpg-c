/** @file libdraw_lifecycle_impl.h
 *  @brief Implementation for renderer/window initialization and shutdown helpers.
 */

#pragma once

#include "libdraw_context.h"
#include "libdraw_lifecycle.h"
#include "libdraw_to_texture.h"
#include "load_music.h"
#include "load_sfx.h"
#include "load_textures.h"
#include "shaders.h"
#include "unload_textures.h"
#include <cstdio>

inline void libdraw_init_render_targets(gamestate& g) {
    g.windowwidth = DEFAULT_WIN_WIDTH;
    g.windowheight = DEFAULT_WIN_HEIGHT;
    g.targetwidth = DEFAULT_TARGET_WIDTH;
    g.targetheight = DEFAULT_TARGET_HEIGHT;

    const TextureFilter filter = TEXTURE_FILTER_POINT;
    libdraw_ctx.target = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    libdraw_ctx.title_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    libdraw_ctx.char_creation_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    libdraw_ctx.main_game_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    libdraw_ctx.hud_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);

    SetTextureFilter(libdraw_ctx.target.texture, filter);
    SetTextureFilter(libdraw_ctx.title_target_texture.texture, filter);
    SetTextureFilter(libdraw_ctx.char_creation_target_texture.texture, filter);
    SetTextureFilter(libdraw_ctx.main_game_target_texture.texture, filter);
    SetTextureFilter(libdraw_ctx.hud_target_texture.texture, filter);

    libdraw_ctx.target_src = Rectangle{0, 0, DEFAULT_TARGET_WIDTH * 1.0f, -DEFAULT_TARGET_HEIGHT * 1.0f};
    libdraw_ctx.target_dest = Rectangle{0, 0, DEFAULT_TARGET_WIDTH * 1.0f, DEFAULT_TARGET_HEIGHT * 1.0f};
}

inline void libdraw_init_resources(gamestate& g) {
    load_textures(libdraw_ctx.txinfo);
    load_shaders();

    const float x = DEFAULT_TARGET_WIDTH / 4.0f;
    const float y = DEFAULT_TARGET_HEIGHT / 4.0f;
    g.cam2d.offset = Vector2{x, y};

    draw_title_screen_to_texture(g, false);
    draw_char_creation_to_texture(g);

    InitAudioDevice();
    while (!IsAudioDeviceReady()) {
    }
    libdraw_load_music(g);
    libdraw_load_sfx(g);
}

inline void libdraw_init_rest(gamestate& g) {
    SetExitKey(KEY_ESCAPE);
    SetTargetFPS(60);
    libdraw_init_render_targets(g);
    libdraw_init_resources(g);
}

inline void libdraw_init(gamestate& g) {
    const int w = DEFAULT_WIN_WIDTH;
    const int h = DEFAULT_WIN_HEIGHT;
    const char* title = WINDOW_TITLE;
    char full_title[1024] = {0};
    snprintf(full_title, sizeof(full_title), "%s - %s", title, g.version.c_str());
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(w, h, full_title);
    g.windowwidth = w;
    g.windowheight = h;
    SetWindowMinSize(320, 240);
    libdraw_init_rest(g);
}

inline bool libdraw_windowshouldclose(gamestate& g) {
    return g.do_quit;
}

inline void libdraw_close_partial() {
    UnloadMusicStream(libdraw_ctx.audio.music);
    CloseAudioDevice();
    libdraw_unload_textures(libdraw_ctx.txinfo);
    unload_shaders();
    UnloadRenderTexture(libdraw_ctx.title_target_texture);
    UnloadRenderTexture(libdraw_ctx.char_creation_target_texture);
    UnloadRenderTexture(libdraw_ctx.main_game_target_texture);
    UnloadRenderTexture(libdraw_ctx.hud_target_texture);
    UnloadRenderTexture(libdraw_ctx.target);
}

inline void libdraw_close() {
    libdraw_close_partial();
    CloseWindow();
}
