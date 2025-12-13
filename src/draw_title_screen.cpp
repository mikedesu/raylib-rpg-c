#include "draw_title_screen.h"
#include "libgame_defines.h"
#include "libgame_version.h"
#include "textureinfo.h"
#include <raylib.h>


extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];


void draw_title_screen(shared_ptr<gamestate> g, bool show_menu) {
    massert(g, "gamestate is NULL");

    const Vector2 origin = {0, 0};
    // Space between title texts
    const Color active_color = WHITE;
    const Color disabled_color = {0x99, 0x99, 0x99, 0xFF};
    //Color selection_color;
    // this will go above the title text
    const char* evildojo_presents_text = "@evildojo666 presents";
    const char* menu_text[2] = {"New Game", "Continue (coming soon)"};
    const char* title_text_0 = "project.";
    const char* title_text_1 = "rpg";
    const char* version_text = g->version.c_str();
    const char* start_text = "Press enter or space to begin";

    // new text
    const char* programming_by = "programming by darkmage";
    const char* art_by = "art by Krishna Palacio";
    const char* sound_effects_by = "sound effects by Leoh Paz";
    const char* music_by = "music by darkmage + suno.ai";

    const int spacing = 2;
    //const int spacing2 = 4;

    char buffer[1024] = {0};
    Color title_text_0_color = {0x66, 0x66, 0x66, 0xFF}, title_text_1_color = {0xFF, 0xFF, 0xFF, 0xFF};
    const int sm_font_size = 10;
    const int font_size = 40;


    const Vector2 m = MeasureTextEx(GetFontDefault(), title_text_0, font_size, spacing);
    const Vector2 m2 = MeasureTextEx(GetFontDefault(), title_text_1, font_size, spacing);

    const Vector2 start_measure = MeasureTextEx(GetFontDefault(), start_text, sm_font_size, spacing);

    const Vector2 version_measure = MeasureTextEx(GetFontDefault(), version_text, sm_font_size, spacing);

    const Rectangle frame = {g->targetwidth / 4.0f, g->targetheight / 4.0f, g->targetwidth / 2.0f, g->targetheight / 2.0f};

    //DrawRectangleLinesEx(frame, 2, RED);

    //const Vector2 dividing_line_0 = {g->targetwidth / 2.0f, 0};
    //const Vector2 dividing_line_1 = {g->targetwidth / 2.0f, g->targetheight * 1.0f};
    //DrawLineEx(dividing_line_0, dividing_line_1, 2.0f, RED);

    //const Vector2 dividing_line_2 = {0, g->targetheight / 2.0f};
    //const Vector2 dividing_line_3 = {g->targetwidth * 1.0f, g->targetheight / 2.0f};
    //DrawLineEx(dividing_line_2, dividing_line_3, 2.0f, RED);


    const float pad = 10;
    //const float w = DEFAULT_TARGET_WIDTH;
    //const float h = DEFAULT_TARGET_HEIGHT;
    //const float x = g->targetwidth / 2.0f - m.x / 2.0f;
    const float x = frame.x + frame.width / 2.0f - (m.x + m2.x) / 2.0f;
    //const float y = g->targetheight / 2.0f - font_size * 4;
    const float y = frame.y;
    const float title_text_1_x = x + m.x + pad;
    //const float version_x = (w - version_measure) / 2.0f;
    //float version_y = y + font_size + 10;
    // Add placeholder text between version and start text
    Vector2 pos = {x, y};

    const char* date_text = GAME_VERSION_DATE;
    const Vector2 date_measure = MeasureTextEx(GetFontDefault(), date_text, sm_font_size, spacing);
    //const float placeholder_x = g->targetwidth / 2.0f - date_measure.x / 2.0f;
    const float placeholder_y = y + font_size + sm_font_size + 10;
    //const float start_x = g->targetwidth / 2.0f - start_measure.x / 4.0f;

    ClearBackground(BLACK);
    //int evidojo_x = (w - evidojo_presents_measure) / 2.0f;
    //int evidojo_y = y - sm_font_size - 10; // Position it above the title text
    //DrawText(evidojo_presents_text, evidojo_x, evidojo_y, sm_font_size, WHITE);

    //DrawText(title_text_0, x, y, font_size, title_text_0_color);

    // Draw the "evildojo666 presents" text at the top
    const Vector2 evildojo_presents_measure = MeasureTextEx(GetFontDefault(), evildojo_presents_text, sm_font_size, spacing);
    pos = {g->targetwidth / 2.0f - evildojo_presents_measure.x / 2.0f, y - sm_font_size - pad};
    DrawTextPro(GetFontDefault(), evildojo_presents_text, pos, origin, 0, sm_font_size, spacing, WHITE);

    pos = {x, y};
    DrawTextPro(GetFontDefault(), title_text_0, pos, origin, 0, font_size, spacing, title_text_0_color);

    pos = {title_text_1_x, y};
    //DrawText(title_text_1, title_text_1_x, y, font_size, title_text_1_color);
    DrawTextPro(GetFontDefault(), title_text_1, pos, origin, 0, font_size, spacing, title_text_1_color);
    //DrawText(version_text, version_x, version_y, sm_font_size, WHITE);

    // Draw the date text
    pos = {g->targetwidth / 2.0f - date_measure.x / 2.0f, placeholder_y};
    //DrawText(date_text, pos.x, pos.y, sm_font_size, WHITE);
    DrawTextPro(GetFontDefault(), date_text, pos, origin, 0, sm_font_size, spacing, WHITE);

    // Draw the version text
    pos = {g->targetwidth / 2.0f - version_measure.x / 2.0f, pos.y + sm_font_size + pad};
    DrawTextPro(GetFontDefault(), version_text, pos, origin, 0, sm_font_size, spacing, WHITE);

    // Draw the programming by text
    const Vector2 prog_m = MeasureTextEx(GetFontDefault(), programming_by, sm_font_size, spacing);
    const Vector2 art_m = MeasureTextEx(GetFontDefault(), art_by, sm_font_size, spacing);
    const Vector2 music_m = MeasureTextEx(GetFontDefault(), music_by, sm_font_size, spacing);
    const Vector2 sfx_m = MeasureTextEx(GetFontDefault(), sound_effects_by, sm_font_size, spacing);

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
    const float sw = 128;
    const float rx = 0, ry = 0;
    const float mx = frame.x, my = frame.y + sw / 2;


    const Texture t1 = txinfo[TX_ACTION_SLASH_SWORD_B].texture;
    const Rectangle src = {rx, ry, 32, 32};
    const Rectangle dst = {mx, my, sw, sw};
    DrawTexturePro(t1, src, dst, origin, 0.0f, WHITE);
    const Texture t2 = txinfo[TX_HUMAN_ATTACK].texture;
    DrawTexturePro(t2, src, dst, origin, 0.0f, WHITE);
    const Texture t3 = txinfo[TX_ACTION_SLASH_SWORD_F].texture;
    DrawTexturePro(t3, src, dst, origin, 0.0f, WHITE);

    const float mx2 = frame.x + frame.width - sw, my2 = frame.y + sw / 2;
    //const float rx2 = 0;
    //ry2 = 0;
    const Rectangle src2 = {rx, ry, -32, 32}, dst2 = {mx2, my2, sw, sw};

    const Texture t4 = txinfo[TX_ACTION_SLASH_AXE_B].texture;
    DrawTexturePro(t4, src2, dst2, origin, 0.0f, WHITE);
    const Texture t5 = txinfo[TX_ORC_ATTACK].texture;
    DrawTexturePro(t5, src2, dst2, origin, 0.0f, WHITE);
    const Texture t6 = txinfo[TX_ACTION_SLASH_AXE_F].texture;
    DrawTexturePro(t6, src2, dst2, origin, 0.0f, WHITE);


    if (!show_menu) {
        //pos = {g->targetwidth / 2.0f - start_measure.x / 2.0f, g->targetheight / 2.0f - sm_font_size / 2.0f};
        //DrawTextPro(GetFontDefault(), start_text, pos, origin, 0, sm_font_size, spacing, WHITE);

        // Draw the music text
        pos = {g->targetwidth / 2.0f - start_measure.x / 2.0f, pos.y + sm_font_size + pad * 2};
        DrawText(start_text, pos.x, pos.y, sm_font_size, WHITE);


        return;
    }
    //const float start_y = frame.y + frame.height - sm_font_size - 10;
    const float start_y = pos.y + sm_font_size + pad * 4;
    // If show_menu is true, draw the new game, continue, options selection text
    const int menu_count = sizeof(menu_text) / sizeof(menu_text[0]);
    const int menu_spacing = 10;
    const int current_selection_index = g->title_screen_selection;
    for (int i = 0; i < menu_count; i++) {
        bzero(buffer, sizeof(buffer));
        float menu_x = (g->targetwidth - MeasureText(menu_text[i], sm_font_size)) / 2.0f;
        float menu_y = start_y + (i * (sm_font_size + menu_spacing));
        if (i == current_selection_index)
            snprintf(buffer, sizeof(buffer), "> %s", menu_text[i]);
        else
            snprintf(buffer, sizeof(buffer), "  %s", menu_text[i]);
        const Color selection_color = i == 0 ? active_color : disabled_color; // First item is always active
        DrawText(buffer, menu_x, menu_y, sm_font_size, selection_color);
    }
}
