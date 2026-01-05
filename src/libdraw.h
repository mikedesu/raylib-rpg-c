#pragma once

#include "gamestate.h"

#include "draw_character_creation_screen.h"
#include "draw_hud.h"
#include "draw_message_history.h"
#include "draw_title_screen.h"
#include "libdraw_create_spritegroup.h"
#include "libdraw_from_texture.h"
#include "libdraw_to_texture.h"
#include "libdraw_update_sprites.h"
#include "libgame_defines.h"
#include "load_music.h"
#include "load_sfx.h"
#include "load_textures.h"
#include "scene.h"
#include "shaders.h"
#include "unload_textures.h"

unordered_map<entityid, spritegroup_t*> spritegroups;
textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];
unordered_map<int, Shader> shaders;
RenderTexture2D title_target_texture = {0};
RenderTexture2D char_creation_target_texture = {0};
RenderTexture2D main_game_target_texture = {0};
RenderTexture2D hud_target_texture = {0};
RenderTexture2D target = {0};
Rectangle target_src = {0, 0, DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT};
Rectangle target_dest = {0, 0, DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT};
Rectangle win_dest = {0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};
Music music;
int ANIM_SPEED = DEFAULT_ANIM_SPEED;
int libdraw_restart_count = 0;


static inline void libdraw_drawframe(gamestate& g)
{
    const double start_time = GetTime();
    libdraw_update_sprites_pre(g);
    BeginDrawing();
    ClearBackground(RED);
    if (g.frame_dirty)
    {
        switch (g.current_scene)
        {
        case SCENE_TITLE: draw_title_screen_to_texture(g, false); break;
        case SCENE_MAIN_MENU: draw_title_screen_to_texture(g, true); break;
        case SCENE_CHARACTER_CREATION: draw_character_creation_screen_to_texture(g); break;
        case SCENE_GAMEPLAY: libdraw_drawframe_2d_to_texture(g); break;
        default: break;
        }

        g.frame_dirty = false;
        g.frame_updates++;
    }
    // draw to the target texture
    BeginTextureMode(target);
    ClearBackground(BLUE);
    switch (g.current_scene)
    {
    case SCENE_TITLE: draw_title_screen_from_texture(g); break;
    case SCENE_MAIN_MENU: draw_title_screen_from_texture(g); break;
    case SCENE_CHARACTER_CREATION: draw_character_creation_screen_from_texture(g); break;
    case SCENE_GAMEPLAY: libdraw_drawframe_2d_from_texture(g); break;
    default: break;
    }
    EndTextureMode();
    // draw the target texture to the window
    win_dest.width = GetScreenWidth();
    win_dest.height = GetScreenHeight();
    DrawTexturePro(target.texture, target_src, win_dest, (Vector2){0, 0}, 0.0f, WHITE);
    EndDrawing();

#ifdef DEBUG
    g.last_frame_time = GetTime() - start_time;
    g.last_frame_times[g.last_frame_times_current] = g.last_frame_time;
    g.last_frame_times_current++;
    if (g.last_frame_times_current >= LAST_FRAME_TIMES_MAX)
    {
        g.last_frame_times_current = 0;
    }
    g.framecount++;
#endif

    libdraw_update_sprites_post(g);
}


static inline void libdraw_init_rest(gamestate& g)
{
    SetExitKey(KEY_ESCAPE);
    SetTargetFPS(60);
    g.windowwidth = DEFAULT_WIN_WIDTH, g.windowheight = DEFAULT_WIN_HEIGHT;
    g.targetwidth = DEFAULT_TARGET_WIDTH, g.targetheight = DEFAULT_TARGET_HEIGHT;
    TextureFilter filter = TEXTURE_FILTER_POINT; // Use trilinear filtering for better quality
    //TextureFilter filter = TEXTURE_FILTER_BILINEAR; // Use trilinear filtering for better quality
    //TextureFilter filter = TEXTURE_FILTER_TRILINEAR; // Use trilinear filtering for better quality
    //TextureFilter filter = TEXTURE_FILTER_ANISOTROPIC_16X;
    target = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    SetTextureFilter(target.texture, filter);
    //SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
    // Use anisotropic filtering for better quality
    title_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    char_creation_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    main_game_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    hud_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    SetTextureFilter(title_target_texture.texture, filter);
    SetTextureFilter(char_creation_target_texture.texture, filter);
    SetTextureFilter(main_game_target_texture.texture, filter);
    SetTextureFilter(hud_target_texture.texture, filter);
    target_src = (Rectangle){0, 0, DEFAULT_TARGET_WIDTH * 1.0f, -DEFAULT_TARGET_HEIGHT * 1.0f};
    target_dest = (Rectangle){0, 0, DEFAULT_TARGET_WIDTH * 1.0f, DEFAULT_TARGET_HEIGHT * 1.0f};
    load_textures(txinfo);
    load_shaders();
    //const float x = DEFAULT_TARGET_WIDTH / 2.0f, y = DEFAULT_TARGET_HEIGHT / 4.0f;
    const float x = DEFAULT_TARGET_WIDTH / 4.0f;
    const float y = DEFAULT_TARGET_HEIGHT / 4.0f;
    g.cam2d.offset = (Vector2){x, y};
    draw_title_screen_to_texture(g, false);
    draw_character_creation_screen_to_texture(g);
    InitAudioDevice();
    while (!IsAudioDeviceReady())
        ;
    libdraw_load_music(g);
    libdraw_load_sfx(g);
}


static inline void libdraw_init(gamestate& g)
{
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


static inline bool libdraw_windowshouldclose(gamestate& g)
{
    return g.do_quit;
}


static inline void libdraw_close_partial()
{
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


static inline void libdraw_close()
{
    libdraw_close_partial();
    CloseWindow();
}
