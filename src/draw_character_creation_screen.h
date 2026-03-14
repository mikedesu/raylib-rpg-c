/** @file draw_character_creation_screen.h
 *  @brief Character-creation scene drawing helper.
 */

#pragma once

#include "gamestate.h"
#include "libgame_defines.h"
#include "texture_ids.h"
#include "textureinfo.h"
#include <array>
#include <string>


extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

/** @brief Draw the character-creation UI, stats, instructions, and race preview. */
static inline void draw_character_creation_screen(gamestate& g) {
    minfo3("draw character creation scene");

    const char* title_text = "Character Creation";
    const std::array<std::string, 4> instructions = {
        "Type to change your name (no spaces)",
        "Press LEFT/RIGHT to change race",
        "Press UP/DOWN to change alignment",
        "Press SPACE to re-roll, ENTER to confirm",
    };

    constexpr int font_size_0 = 40;
    constexpr int font_size_1 = 20;
    constexpr int w = DEFAULT_TARGET_WIDTH;
    constexpr int h = DEFAULT_TARGET_HEIGHT;
    constexpr int cx = w / 2;
    constexpr int sy = h / 4;
    constexpr int x = cx;
    constexpr int y0 = sy;
    constexpr int line_gap = 10;
    constexpr int line_step = font_size_1 + line_gap;
    constexpr int stats_start_y = y0 + font_size_0 + 10;
    constexpr int instructions_start_y = stats_start_y + line_step * 10 + 8;

    constexpr float pad = -40;
    constexpr float dst2_y = sy + pad;
    constexpr float dst2_wh = 400;
    constexpr float dst2_x = cx - dst2_wh;

    constexpr Rectangle src = {0, 0, 32, 32};

    constexpr Rectangle dst2 = Rectangle{dst2_x, dst2_y, dst2_wh, dst2_wh};

    constexpr Vector2 zero_vec = Vector2{0, 0};

    ClearBackground(BLACK);
    DrawText(title_text, x, y0, font_size_0, WHITE);

    const std::array<std::string, 10> stat_lines = {
        TextFormat("Name: %s_", g.chara_creation.name.c_str()),
        TextFormat("< Race: %s >", race2str(g.chara_creation.race).c_str()),
        TextFormat("< Alignment: %s >", alignment_to_str(g.chara_creation.alignment).c_str()),
        TextFormat("Hitdie: %d", g.chara_creation.hitdie),
        TextFormat("Strength: %d", g.chara_creation.strength),
        TextFormat("Dexterity: %d", g.chara_creation.dexterity),
        TextFormat("Intelligence: %d", g.chara_creation.intelligence),
        TextFormat("Wisdom: %d", g.chara_creation.wisdom),
        TextFormat("Constitution: %d", g.chara_creation.constitution),
        TextFormat("Charisma: %d", g.chara_creation.charisma),
    };

    int text_y = stats_start_y;
    for (const std::string& line : stat_lines) {
        DrawText(line.c_str(), x, text_y, font_size_1, WHITE);
        text_y += line_step;
    }

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

    case RACE_ZOMBIE: {
        DrawTexturePro(txinfo[TX_MONSTER_ZOMBIE_IDLE].texture, src, dst2, zero_vec, 0.0f, WHITE);
    } break;


    case RACE_SKELETON: {
        DrawTexturePro(txinfo[TX_MONSTER_SKELETON_IDLE].texture, src, dst2, zero_vec, 0.0f, WHITE);
    } break;


    case RACE_RAT: {
        DrawTexturePro(txinfo[TX_MONSTER_RAT_IDLE].texture, src, dst2, zero_vec, 0.0f, WHITE);
    } break;




    default: break;
    }

    text_y = instructions_start_y;
    for (const std::string& line : instructions) {
        DrawText(line.c_str(), x, text_y, font_size_1, WHITE);
        text_y += font_size_1 + 8;
    }

    msuccess3("draw character creation scene");
}
