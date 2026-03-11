#pragma once

#include "libdraw_context.h"
#include "load_music.h"
#include "load_sfx.h"
#include "load_textures.h"
#include "shaders.h"
#include "unload_textures.h"
#include <cstdio>

static inline void libdraw_init_render_targets(gamestate& g) {
    g.windowwidth = DEFAULT_WIN_WIDTH;
    g.windowheight = DEFAULT_WIN_HEIGHT;
    g.targetwidth = DEFAULT_TARGET_WIDTH;
    g.targetheight = DEFAULT_TARGET_HEIGHT;

    const TextureFilter filter = TEXTURE_FILTER_POINT;
    target = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    title_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    char_creation_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    main_game_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    hud_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);

    SetTextureFilter(target.texture, filter);
    SetTextureFilter(title_target_texture.texture, filter);
    SetTextureFilter(char_creation_target_texture.texture, filter);
    SetTextureFilter(main_game_target_texture.texture, filter);
    SetTextureFilter(hud_target_texture.texture, filter);

    target_src = Rectangle{0, 0, DEFAULT_TARGET_WIDTH * 1.0f, -DEFAULT_TARGET_HEIGHT * 1.0f};
    target_dest = Rectangle{0, 0, DEFAULT_TARGET_WIDTH * 1.0f, DEFAULT_TARGET_HEIGHT * 1.0f};
}

static inline void libdraw_init_resources(gamestate& g) {
    load_textures(txinfo);
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

static inline void libdraw_init_rest(gamestate& g) {
    SetExitKey(KEY_ESCAPE);
    SetTargetFPS(60);
    libdraw_init_render_targets(g);
    libdraw_init_resources(g);
}

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

static inline bool libdraw_windowshouldclose(gamestate& g) {
    return g.do_quit;
}

static inline void libdraw_close_partial() {
    UnloadMusicStream(music);
    CloseAudioDevice();
    libdraw_unload_textures(txinfo);
    unload_shaders();
    UnloadRenderTexture(title_target_texture);
    UnloadRenderTexture(char_creation_target_texture);
    UnloadRenderTexture(main_game_target_texture);
    UnloadRenderTexture(hud_target_texture);
    UnloadRenderTexture(target);
}

static inline void libdraw_close() {
    libdraw_close_partial();
    CloseWindow();
}
