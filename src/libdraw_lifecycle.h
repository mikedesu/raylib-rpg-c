/** @file libdraw_lifecycle.h
 *  @brief Renderer/window initialization and shutdown helpers.
 */

#pragma once

#include "libdraw_context.h"
#include "load_music.h"
#include "load_sfx.h"
#include "load_textures.h"
#include "shaders.h"
#include "unload_textures.h"
#include <cstdio>

/** @brief Create and configure the renderer's off-screen render targets. */
static inline void libdraw_init_render_targets(gamestate& g) {
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

/** @brief Load textures, shaders, audio, and initial scene textures for rendering. */
static inline void libdraw_init_resources(gamestate& g) {
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

/** @brief Apply remaining renderer defaults after the OS window has been created. */
static inline void libdraw_init_rest(gamestate& g) {
    SetExitKey(KEY_ESCAPE);
    SetTargetFPS(60);
    libdraw_init_render_targets(g);
    libdraw_init_resources(g);
}

/**
 * @brief Initialize the render window, render targets, textures, shaders, and audio state.
 *
 * @param g Active game state that owns runtime renderer configuration.
 */
static inline void libdraw_init(gamestate& g) {
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

/**
 * @brief Return whether the application should leave the main render loop.
 *
 * @param g Active game state.
 * @return `true` when the game has requested shutdown.
 */
static inline bool libdraw_windowshouldclose(gamestate& g) {
    return g.do_quit;
}

/** @brief Release renderer-owned resources without closing the OS window. */
static inline void libdraw_close_partial() {
    UnloadMusicStream(music);
    CloseAudioDevice();
    libdraw_unload_textures(libdraw_ctx.txinfo);
    unload_shaders();
    UnloadRenderTexture(libdraw_ctx.title_target_texture);
    UnloadRenderTexture(libdraw_ctx.char_creation_target_texture);
    UnloadRenderTexture(libdraw_ctx.main_game_target_texture);
    UnloadRenderTexture(libdraw_ctx.hud_target_texture);
    UnloadRenderTexture(libdraw_ctx.target);
}

/**
 * @brief Shutdown renderer-owned resources and close the game window.
 */
static inline void libdraw_close() {
    libdraw_close_partial();
    CloseWindow();
}
