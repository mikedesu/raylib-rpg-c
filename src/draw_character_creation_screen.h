#pragma once


#include "gamestate.h"
#include "libgame_defines.h"
#include "texture_ids.h"
#include "textureinfo.h"


extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];


static inline void draw_character_creation_screen(gamestate& g) {
    minfo3("draw character creation scene");

    const char* title_text = "Character Creation";
    const char* remaining_text[] = {"Press SPACE to re-roll stats", "Press LEFT/RIGHT to change race", "Press ENTER to confirm"};

    constexpr int font_size_0 = 40;
    constexpr int font_size_1 = 20;
    constexpr int w = DEFAULT_TARGET_WIDTH;
    constexpr int h = DEFAULT_TARGET_HEIGHT;
    constexpr int cx = w / 2;
    constexpr int sy = h / 4;
    constexpr int x = cx;
    constexpr int y0 = sy;
    constexpr int y1 = y0 + font_size_0 + 10;
    constexpr int offset = 10;
    constexpr int offset1 = font_size_1 + offset;
    constexpr int y2 = y1 + offset1;
    constexpr int y3 = y2 + offset1;
    constexpr int y4 = y3 + offset1;
    constexpr int y5 = y4 + offset1;
    constexpr int y6 = y5 + offset1;
    constexpr int y7 = y6 + offset1;
    constexpr int y8 = y7 + offset1;
    constexpr int y9 = y8 + offset1;
    constexpr int y10 = y9 + offset1;
    constexpr int y11 = y10 + font_size_1 + 8;
    constexpr int y12 = y11 + font_size_1 + 8;
    constexpr int y13 = y12 + font_size_1 + 8;

    constexpr float pad = -40;
    constexpr float dst2_y = sy + pad;
    constexpr float dst2_wh = 400;
    constexpr float dst2_x = cx - dst2_wh;

    constexpr Rectangle src = {0, 0, 32, 32};

    constexpr Rectangle dst2 = Rectangle{dst2_x, dst2_y, dst2_wh, dst2_wh};

    constexpr Vector2 zero_vec = Vector2{0, 0};

    ClearBackground(BLACK);
    DrawText(title_text, x, y0, font_size_0, WHITE);

    // Draw character stats
    DrawText(TextFormat("Name: %s", g.chara_creation.name.c_str()), x, y1, font_size_1, WHITE);
    DrawText(TextFormat("< Race: %s >", race2str(g.chara_creation.race).c_str()), x, y2, font_size_1, WHITE);
    DrawText(TextFormat("Hitdie: %d", g.chara_creation.hitdie), x, y3, font_size_1, WHITE);
    DrawText(TextFormat("Strength: %d", g.chara_creation.strength), x, y4, font_size_1, WHITE);
    DrawText(TextFormat("Dexterity: %d", g.chara_creation.dexterity), x, y5, font_size_1, WHITE);
    DrawText(TextFormat("Intelligence: %d", g.chara_creation.intelligence), x, y6, font_size_1, WHITE);
    DrawText(TextFormat("Wisdom: %d", g.chara_creation.wisdom), x, y7, font_size_1, WHITE);
    DrawText(TextFormat("Constitution: %d", g.chara_creation.constitution), x, y8, font_size_1, WHITE);
    DrawText(TextFormat("Charisma: %d", g.chara_creation.charisma), x, y9, font_size_1, WHITE);

    // Draw sprite placeholder

    // Draw a frame of the human idle texture
    switch (g.chara_creation.race) {
    case RACE_HUMAN: {
        DrawTexturePro(txinfo[TX_CHAR_HUMAN_IDLE].texture, src, dst2, zero_vec, 0.0f, WHITE);
    } break;
    case RACE_ORC: {
        DrawTexturePro(txinfo[TX_CHAR_ORC_IDLE].texture, src, dst2, zero_vec, 0.0f, WHITE);
    } break;
    case RACE_ELF: {
        DrawTexturePro(txinfo[TX_CHAR_ELF_IDLE].texture, src, dst2, zero_vec, 0.0f, WHITE);
    } break;
    case RACE_DWARF: {
        DrawTexturePro(txinfo[TX_CHAR_DWARF_IDLE].texture, src, dst2, zero_vec, 0.0f, WHITE);
    } break;
    case RACE_HALFLING: {
        DrawTexturePro(txinfo[TX_CHAR_HALFLING_IDLE].texture, src, dst2, zero_vec, 0.0f, WHITE);
    } break;
    case RACE_GOBLIN: {
        DrawTexturePro(txinfo[TX_CHAR_GOBLIN_IDLE].texture, src, dst2, zero_vec, 0.0f, WHITE);
    } break;
    case RACE_GREEN_SLIME: {
        DrawTexturePro(txinfo[TX_MONSTER_GREEN_SLIME_IDLE].texture, src, dst2, zero_vec, 0.0f, WHITE);
    } break;
    case RACE_BAT: {
        DrawTexturePro(txinfo[TX_MONSTER_BAT_IDLE].texture, src, dst2, zero_vec, 0.0f, WHITE);
    } break;
    case RACE_WOLF: {
        DrawTexturePro(txinfo[TX_MONSTER_WOLF_IDLE].texture, src, dst2, zero_vec, 0.0f, WHITE);
    } break;
    case RACE_WARG: {
        DrawTexturePro(txinfo[TX_MONSTER_WARG_IDLE].texture, src, dst2, zero_vec, 0.0f, WHITE);
    } break;
    default: break;
    }

    // Draw instructions
    DrawText(remaining_text[0], x, y11, font_size_1, WHITE);
    DrawText(remaining_text[1], x, y12, font_size_1, WHITE);
    DrawText(remaining_text[2], x, y13, font_size_1, WHITE);

    msuccess3("draw character creation scene");
}
