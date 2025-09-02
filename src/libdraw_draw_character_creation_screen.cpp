#include "gamestate.h"
#include "libgame_defines.h"
#include "massert.h"
#include "textureinfo.h"

#include <memory>

using std::shared_ptr;

extern textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

void draw_character_creation_screen(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    const char* title_text = "Character Creation";
    const char* remaining_text[] = {"Press SPACE to re-roll stats",
                                    "Press LEFT/RIGHT to change race (unavailable for now)",
                                    "Press UP/DOWN to change class (unavailable for now)",
                                    "Press ENTER to confirm"};
    int font_size = 20;
    int w = DEFAULT_TARGET_WIDTH;
    int h = DEFAULT_TARGET_HEIGHT;
    int cx = w / 2;
    int sy = h / 4;
    int x = cx;
    int y = sy;
    //char buffer[2048] = {0};
    ClearBackground(BLACK);
    DrawText(title_text, x, y, font_size, WHITE);
    y += font_size + 10;
    font_size = 10;
    // Draw character stats
    DrawText(TextFormat("Name: %s", g->chara_creation->name.c_str()), x, y, font_size, WHITE);
    y += font_size + 10;
    DrawText(TextFormat("Race: %s", race2str(g->chara_creation->race).c_str()), x, y, font_size, WHITE);
    y += font_size + 10;
    DrawText(TextFormat("Hitdie: %d", g->chara_creation->hitdie), x, y, font_size, WHITE);
    y += font_size + 10;
    DrawText(TextFormat("Strength: %d", g->chara_creation->strength), x, y, font_size, WHITE);
    y += font_size + 10;
    DrawText(TextFormat("Dexterity: %d", g->chara_creation->dexterity), x, y, font_size, WHITE);
    y += font_size + 10;
    DrawText(TextFormat("Constitution: %d", g->chara_creation->constitution), x, y, font_size, WHITE);
    y += font_size + 10;
    // Draw sprite placeholder
    Rectangle dst = (Rectangle){(float)cx - 210, (float)sy, (float)200, (float)200};
    float pad = 40;
    Rectangle dst2 = (Rectangle){(float)cx - 210 + pad, (float)sy + pad, (float)200 - pad * 2, (float)200 - pad * 2};
    DrawRectangleLinesEx(dst, 4, RED);
    DrawRectangleLinesEx(dst2, 4, BLUE);
    // Draw a frame of the human idle texture
    Rectangle src = {12, 12, 8, 8};
    //DrawTexturePro(txinfo[TX_HUMAN_IDLE].texture, src, dst, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTexturePro(txinfo[TX_HUMAN_IDLE].texture, src, dst2, (Vector2){0, 0}, 0.0f, WHITE);

    // Draw instructions
    y += font_size + 8;
    for (size_t i = 0; i < sizeof(remaining_text) / sizeof(remaining_text[0]); i++) {
        DrawText(remaining_text[i], x, y, font_size, WHITE);
        y += font_size + 4;
    }
}
