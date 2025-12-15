#pragma once

#include "gamestate.h"
#include "libgame_version.h"
#include "texture_ids.h"
#include "textureinfo.h"


extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];


static inline void draw_title_screen(shared_ptr<gamestate> g, bool show_menu) {
    massert(g, "gamestate is NULL");
    char buffer[1024] = {0};
    // Space between title texts
    const Color active_color = WHITE, disabled_color = {0x99, 0x99, 0x99, 0xFF}, title_text_0_color = {0x66, 0x66, 0x66, 0xFF},
                title_text_1_color = {0xFF, 0xFF, 0xFF, 0xFF};
    //Color selection_color;
    // this will go above the title text
    const char* menu_text[2] = {"New Game", "Continue (coming soon)"};
    const char *evildojo_presents_text = "@evildojo666 presents", *title_text_0 = "project.", *title_text_1 = "rpg",
               *start_text = "Press enter or space to begin", *programming_by = "programming by darkmage", *art_by = "art by Krishna Palacio",
               *sound_effects_by = "sound effects by Leoh Paz", *music_by = "music by darkmage + suno.ai", *version_text = g->version.c_str(),
               *date_text = GAME_VERSION_DATE;
    //const int spacing2 = 4;
    const int spacing = 2, sm_font_size = 10, font_size = 40;
    const Rectangle frame = {g->targetwidth / 4.0f, g->targetheight / 4.0f, g->targetwidth / 2.0f, g->targetheight / 2.0f};
    // Add placeholder text between version and start text
    ClearBackground(BLACK);
    // Draw the "evildojo666 presents" text at the top
    const Vector2 origin = {0, 0}, m = MeasureTextEx(GetFontDefault(), title_text_0, font_size, spacing),
                  m2 = MeasureTextEx(GetFontDefault(), title_text_1, font_size, spacing),
                  start_measure = MeasureTextEx(GetFontDefault(), start_text, sm_font_size, spacing),
                  version_measure = MeasureTextEx(GetFontDefault(), version_text, sm_font_size, spacing),
                  date_measure = MeasureTextEx(GetFontDefault(), date_text, sm_font_size, spacing),
                  evildojo_presents_measure = MeasureTextEx(GetFontDefault(), evildojo_presents_text, sm_font_size, spacing),
                  prog_m = MeasureTextEx(GetFontDefault(), programming_by, sm_font_size, spacing),
                  art_m = MeasureTextEx(GetFontDefault(), art_by, sm_font_size, spacing),
                  music_m = MeasureTextEx(GetFontDefault(), music_by, sm_font_size, spacing),
                  sfx_m = MeasureTextEx(GetFontDefault(), sound_effects_by, sm_font_size, spacing);
    const float pad = 10, x = frame.x + frame.width / 2.0f - (m.x + m2.x) / 2.0f, y = frame.y, title_text_1_x = x + m.x + pad;
    Vector2 pos = {x, y};
    const float placeholder_y = y + font_size + sm_font_size + 10;
    pos = {g->targetwidth / 2.0f - evildojo_presents_measure.x / 2.0f, y - sm_font_size - pad};
    DrawTextPro(GetFontDefault(), evildojo_presents_text, pos, origin, 0, sm_font_size, spacing, WHITE);
    pos = {x, y};
    DrawTextPro(GetFontDefault(), title_text_0, pos, origin, 0, font_size, spacing, title_text_0_color);
    pos = {title_text_1_x, y};
    DrawTextPro(GetFontDefault(), title_text_1, pos, origin, 0, font_size, spacing, title_text_1_color);
    // Draw the date text
    pos = {g->targetwidth / 2.0f - date_measure.x / 2.0f, placeholder_y};
    DrawTextPro(GetFontDefault(), date_text, pos, origin, 0, sm_font_size, spacing, WHITE);
    // Draw the version text
    pos = {g->targetwidth / 2.0f - version_measure.x / 2.0f, pos.y + sm_font_size + pad};
    DrawTextPro(GetFontDefault(), version_text, pos, origin, 0, sm_font_size, spacing, WHITE);
    // Draw the programming by text
    pos = {g->targetwidth / 2.0f - prog_m.x / 2.0f, pos.y + sm_font_size + pad};
    DrawText(programming_by, pos.x, pos.y, sm_font_size, WHITE);
    // Draw the art text
    pos = {g->targetwidth / 2.0f - art_m.x / 2.0f, pos.y + sm_font_size + pad};
    DrawText(art_by, pos.x, pos.y, sm_font_size, WHITE);
    // Draw the sfx text
    pos = {g->targetwidth / 2.0f - sfx_m.x / 2.0f, pos.y + sm_font_size + pad};
    DrawText(sound_effects_by, pos.x, pos.y, sm_font_size, WHITE);
    // Draw the music text
    pos = {g->targetwidth / 2.0f - music_m.x / 2.0f, pos.y + sm_font_size + pad};
    DrawText(music_by, pos.x, pos.y, sm_font_size, WHITE);
    // experiment
    // draw the first frame of one of our hero textures
    const float sw = 128, rx = 0, ry = 0, mx = frame.x, my = frame.y + sw / 2;
    const Texture t1 = txinfo[TX_ACTIONS_SLASH_SWORD_B].texture;
    const Rectangle src = {rx, ry, 32, 32}, dst = {mx, my, sw, sw};
    DrawTexturePro(t1, src, dst, origin, 0.0f, WHITE);
    const Texture t2 = txinfo[TX_CHAR_HUMAN_ATTACK].texture;
    DrawTexturePro(t2, src, dst, origin, 0.0f, WHITE);
    const Texture t3 = txinfo[TX_ACTIONS_SLASH_SWORD_F].texture;
    DrawTexturePro(t3, src, dst, origin, 0.0f, WHITE);
    const float mx2 = frame.x + frame.width - sw, my2 = frame.y + sw / 2;
    const Rectangle src2 = {rx, ry, -32, 32}, dst2 = {mx2, my2, sw, sw};
    const Texture t4 = txinfo[TX_ACTIONS_SLASH_AXE_B].texture;
    DrawTexturePro(t4, src2, dst2, origin, 0.0f, WHITE);
    const Texture t5 = txinfo[TX_CHAR_ORC_ATTACK].texture;
    DrawTexturePro(t5, src2, dst2, origin, 0.0f, WHITE);
    const Texture t6 = txinfo[TX_ACTIONS_SLASH_AXE_F].texture;
    DrawTexturePro(t6, src2, dst2, origin, 0.0f, WHITE);
    if (!show_menu) {
        // Draw the start text
        pos = {g->targetwidth / 2.0f - start_measure.x / 2.0f, pos.y + sm_font_size + pad * 2};
        DrawText(start_text, pos.x, pos.y, sm_font_size, WHITE);
        return;
    }
    const float start_y = pos.y + sm_font_size + pad * 4;
    // If show_menu is true, draw the new game, continue, options selection text
    const int menu_count = sizeof(menu_text) / sizeof(menu_text[0]), menu_spacing = 10, current_selection_index = g->title_screen_selection;
    for (int i = 0; i < menu_count; i++) {
        bzero(buffer, sizeof(buffer));
        const float menu_x = (g->targetwidth - MeasureText(menu_text[i], sm_font_size)) / 2.0f, menu_y = start_y + (i * (sm_font_size + menu_spacing));
        if (i == current_selection_index)
            snprintf(buffer, sizeof(buffer), "> %s", menu_text[i]);
        else
            snprintf(buffer, sizeof(buffer), "  %s", menu_text[i]);
        const Color selection_color = i == 0 ? active_color : disabled_color; // First item is always active
        DrawText(buffer, menu_x, menu_y, sm_font_size, selection_color);
    }
}
