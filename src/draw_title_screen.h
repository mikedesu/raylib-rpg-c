#pragma once

#include "gamestate.h"
#include "texture_ids.h"
#include "textureinfo.h"
#include <raylib.h>

extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

static inline void draw_title_screen(gamestate& g, bool show_menu) {
    constexpr int sm_font_size = 20;
    constexpr int font_size = 80;

    char buffer[1024] = {0};
    const char* menu_text[2] = {"New Game", "Continue (coming soon)"};
    // Space between title texts
    constexpr Color active_color = WHITE;
    constexpr Color disabled_color = {0x99, 0x99, 0x99, 0xFF};
    constexpr Color title_text_0_color = {0x66, 0x66, 0x66, 0xFF};
    constexpr Color title_text_1_color = {0xFF, 0xFF, 0xFF, 0xFF};
    constexpr int spacing = 2;
    constexpr Vector2 origin = {0, 0};
    constexpr float sw = 128;
    constexpr float rx = 0;
    constexpr float ry = 0;
    constexpr float pad = 10;
    constexpr int menu_count = sizeof(menu_text) / sizeof(menu_text[0]);
    constexpr int menu_spacing = 10;

    // this will go above the title text
    const char* evildojo_presents_text = "@evildojo666 presents";
    const char* title_text_0 = "project.rpg";
    const char* title_text_1 = "          rpg";
    const char* start_text = "Press enter or space to begin";
    const char* programming_by = "programming by darkmage";
    const char* art_by = "art by Krishna Palacio";
    const char* sound_effects_by = "sound effects by Leoh Paz";
    const char* music_by = "music by darkmage + suno.ai";
    const char* version_text = g.version.c_str();
    const char* date_text = GAME_VERSION_DATE;

    const Rectangle frame = {g.targetwidth / 4.0f, g.targetheight / 4.0f, g.targetwidth / 2.0f, g.targetheight / 2.0f};
    // Add placeholder text between version and start text
    ClearBackground(BLACK);
    // Draw the "evildojo666 presents" text at the top
    const Vector2 m = MeasureTextEx(GetFontDefault(), title_text_0, font_size, spacing);
    //const Vector2 m2 = MeasureTextEx(GetFontDefault(), title_text_1, font_size, spacing);
    const Vector2 start_measure = MeasureTextEx(GetFontDefault(), start_text, sm_font_size, spacing);
    const Vector2 version_measure = MeasureTextEx(GetFontDefault(), version_text, sm_font_size, spacing);
    const Vector2 date_measure = MeasureTextEx(GetFontDefault(), date_text, sm_font_size, spacing);
    const Vector2 evildojo_presents_measure = MeasureTextEx(GetFontDefault(), evildojo_presents_text, sm_font_size, spacing);
    const Vector2 prog_m = MeasureTextEx(GetFontDefault(), programming_by, sm_font_size, spacing);
    const Vector2 art_m = MeasureTextEx(GetFontDefault(), art_by, sm_font_size, spacing);
    const Vector2 music_m = MeasureTextEx(GetFontDefault(), music_by, sm_font_size, spacing);
    const Vector2 sfx_m = MeasureTextEx(GetFontDefault(), sound_effects_by, sm_font_size, spacing);


    //const float x = frame.x + frame.width / 2.0f - (m.x + m2.x) / 2.0f;
    const float x = frame.x + frame.width / 2.0f - m.x / 2.0f;
    const float y = frame.y;

    //const float title_text_1_x = x + m.x + pad + pad;

    //Vector2 pos = {x, y};

    const float placeholder_y = y + font_size + sm_font_size + 10;

    Vector2 pos = {g.targetwidth / 2.0f - evildojo_presents_measure.x / 2.0f, y - sm_font_size - pad};

    DrawText(evildojo_presents_text, pos.x, pos.y, sm_font_size, WHITE);

    pos = {x, y};
    DrawText(title_text_0, pos.x, pos.y, font_size, title_text_0_color);
    //pos = {title_text_1_x, y};
    DrawText(title_text_1, pos.x, pos.y, font_size, title_text_1_color);




    // Draw the date text
    pos = {g.targetwidth / 2.0f - date_measure.x / 2.0f, placeholder_y};
    DrawText(date_text, pos.x, pos.y, sm_font_size, WHITE);
    // Draw the version text
    pos = {g.targetwidth / 2.0f - version_measure.x / 2.0f, pos.y + sm_font_size + pad};
    DrawText(version_text, pos.x, pos.y, sm_font_size, WHITE);
    // Draw the programming by text
    pos = {g.targetwidth / 2.0f - prog_m.x / 2.0f, pos.y + sm_font_size + pad};
    DrawText(programming_by, pos.x, pos.y, sm_font_size, WHITE);
    // Draw the art text
    pos = {g.targetwidth / 2.0f - art_m.x / 2.0f, pos.y + sm_font_size + pad};
    DrawText(art_by, pos.x, pos.y, sm_font_size, WHITE);
    // Draw the sfx text
    pos = {g.targetwidth / 2.0f - sfx_m.x / 2.0f, pos.y + sm_font_size + pad};
    DrawText(sound_effects_by, pos.x, pos.y, sm_font_size, WHITE);
    // Draw the music text
    pos = {g.targetwidth / 2.0f - music_m.x / 2.0f, pos.y + sm_font_size + pad};
    DrawText(music_by, pos.x, pos.y, sm_font_size, WHITE);
    // experiment
    // draw the first frame of one of our hero textures
    const float mx = frame.x;
    const float my = frame.y + sw / 2;
    const Texture t1 = txinfo[TX_ACTIONS_SLASH_SHORT_SWORD_B].texture;
    const Texture t2 = txinfo[TX_CHAR_HUMAN_ATTACK_SLASH].texture;
    const Texture t3 = txinfo[TX_ACTIONS_SLASH_SHORT_SWORD_F].texture;
    const Texture t4 = txinfo[TX_ACTIONS_SLASH_AXE_B].texture;
    const Texture t5 = txinfo[TX_CHAR_ORC_ATTACK].texture;
    const Texture t6 = txinfo[TX_ACTIONS_SLASH_AXE_F].texture;

    constexpr Rectangle src = {rx, ry, 32, 32};
    const Rectangle dst = {mx, my, sw, sw};
    const float mx2 = frame.x + frame.width - sw;
    const float my2 = frame.y + sw / 2;
    const Rectangle src2 = {rx, ry, -32, 32}, dst2 = {mx2, my2, sw, sw};

    DrawTexturePro(t1, src, dst, origin, 0.0f, WHITE);
    DrawTexturePro(t2, src, dst, origin, 0.0f, WHITE);
    DrawTexturePro(t3, src, dst, origin, 0.0f, WHITE);
    DrawTexturePro(t4, src2, dst2, origin, 0.0f, WHITE);
    DrawTexturePro(t5, src2, dst2, origin, 0.0f, WHITE);
    DrawTexturePro(t6, src2, dst2, origin, 0.0f, WHITE);
    if (!show_menu) {
        // Draw the start text
        pos = {g.targetwidth / 2.0f - start_measure.x / 2.0f, pos.y + sm_font_size + pad * 2};
        DrawText(start_text, pos.x, pos.y, sm_font_size, WHITE);
        return;
    }
    const float start_y = pos.y + sm_font_size + pad * 4;
    // If show_menu is true, draw the new game, continue, options selection text
    const int current_selection_index = g.title_screen_selection;
    for (int i = 0; i < menu_count; i++) {
        bzero(buffer, sizeof(buffer));
        const float menu_x = (g.targetwidth - MeasureText(menu_text[i], sm_font_size)) / 2.0f, menu_y = start_y + (i * (sm_font_size + menu_spacing));
        if (i == current_selection_index) {
            snprintf(buffer, sizeof(buffer), "> %s", menu_text[i]);
        }
        else {
            snprintf(buffer, sizeof(buffer), "  %s", menu_text[i]);
        }
        const Color selection_color = i == 0 ? active_color : disabled_color; // First item is always active
        DrawText(buffer, menu_x, menu_y, sm_font_size, selection_color);
    }
}
