#pragma once


#include "gamestate.h"
#include "libgame_defines.h"
#include "texture_ids.h"
#include "textureinfo.h"


extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];


static inline void draw_character_creation_screen(gamestate& g) {
    const char* title_text = "Character Creation";
    const char* remaining_text[] = {"Press SPACE to re-roll stats", "Press LEFT/RIGHT to change race", "Press ENTER to confirm"};
    int font_size = 20;
    const int w = DEFAULT_TARGET_WIDTH;
    const int h = DEFAULT_TARGET_HEIGHT;
    const int cx = w / 2;
    const int sy = h / 4;
    const int x = cx;
    int y = sy;
    ClearBackground(BLACK);
    DrawText(title_text, x, y, font_size, WHITE);
    y += font_size + 10;
    font_size = 10;
    // Draw character stats
    const int offset = 10;
    DrawText(TextFormat("Name: %s", g.chara_creation.name.c_str()), x, y, font_size, WHITE);
    y += font_size + offset;
    DrawText(TextFormat("< Race: %s >", race2str(g.chara_creation.race).c_str()), x, y, font_size, WHITE);
    y += font_size + offset;
    DrawText(TextFormat("Hitdie: %d", g.chara_creation.hitdie), x, y, font_size, WHITE);
    y += font_size + offset;
    DrawText(TextFormat("Strength: %d", g.chara_creation.strength), x, y, font_size, WHITE);
    y += font_size + offset;
    DrawText(TextFormat("Dexterity: %d", g.chara_creation.dexterity), x, y, font_size, WHITE);
    y += font_size + offset;
    DrawText(TextFormat("Intelligence: %d", g.chara_creation.intelligence), x, y, font_size, WHITE);
    y += font_size + offset;
    DrawText(TextFormat("Wisdom: %d", g.chara_creation.wisdom), x, y, font_size, WHITE);
    y += font_size + offset;
    DrawText(TextFormat("Constitution: %d", g.chara_creation.constitution), x, y, font_size, WHITE);
    y += font_size + offset;
    DrawText(TextFormat("Charisma: %d", g.chara_creation.charisma), x, y, font_size, WHITE);
    y += font_size + offset;
    // Draw sprite placeholder
    float pad = -40;
    Rectangle dst2 = (Rectangle){(float)cx - 210 + pad, (float)sy + pad, (float)200 - pad * 2, (float)200 - pad * 2};
    // Draw a frame of the human idle texture
    Rectangle src = {0, 0, 32, 32};
    int tx_key = TX_CHAR_HUMAN_IDLE;
    switch (g.chara_creation.race) {
    case RACE_HUMAN: tx_key = TX_CHAR_HUMAN_IDLE; break;
    case RACE_ORC: tx_key = TX_CHAR_ORC_IDLE; break;
    case RACE_ELF: tx_key = TX_CHAR_ELF_IDLE; break;
    case RACE_DWARF: tx_key = TX_CHAR_DWARF_IDLE; break;
    case RACE_HALFLING: tx_key = TX_CHAR_HALFLING_IDLE; break;
    case RACE_GOBLIN: tx_key = TX_CHAR_GOBLIN_IDLE; break;
    case RACE_GREEN_SLIME: tx_key = TX_MONSTER_GREEN_SLIME_IDLE; break;
    case RACE_BAT: tx_key = TX_MONSTER_BAT_IDLE; break;
    case RACE_WOLF: tx_key = TX_MONSTER_WOLF_IDLE; break;
    case RACE_WARG: tx_key = TX_MONSTER_WARG_IDLE; break;
    default: break;
    }
    DrawTexturePro(txinfo[tx_key].texture, src, dst2, (Vector2){0, 0}, 0.0f, WHITE);
    // Draw instructions
    y += font_size + 8;
    for (size_t i = 0; i < sizeof(remaining_text) / sizeof(remaining_text[0]); i++) {
        DrawText(remaining_text[i], x, y, font_size, WHITE);
        y += font_size + 4;
    }
}
