#include "libdraw_hud.h"

void draw_hud(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    const int turn = g->turn_count;
    const int hp = 0;
    const int maxhp = 0;
    const int level = 0;
    const int xp = 0;
    const int next_level_xp = 0;
    const int attack_bonus = 0;
    const int ac = 0;
    const int floor = g->dungeon->current_floor;
    const int font_size = 10;
    const int line_thickness = 1;
    char buffer0[1024] = {0};
    char buffer1[1024] = {0};
    const Color bg = (Color){0x33, 0x33, 0x33, 0xFF};
    const Color fg = WHITE;
    const char* format_str_0 = "%s Lvl %d HP %d/%d Atk: %d AC: %d XP %d/%d";
    const char* format_str_1 = "Floor %d Turn %d";
    const string n = g->ct.get<name>(g->hero_id).value_or("no-name");


    snprintf(buffer0, sizeof(buffer0), format_str_0, n.c_str(), level, hp, maxhp, attack_bonus, ac, xp, next_level_xp);
    snprintf(buffer1, sizeof(buffer1), format_str_1, floor, turn);

    const int text_size0 = MeasureText(buffer0, font_size);
    const float box_w = text_size0 + g->pad * 2;
    const float box_h = font_size + g->pad;

    const float box_x = g->targetwidth / 2.0f - (box_w / 2.0f);
    const float box_y = g->targetheight - (box_h);

    const int text_x = box_x + 10;
    const int text_y = box_y + (box_h - font_size) / 2;

    DrawRectangleRec((Rectangle){box_x, box_y, box_w, box_h}, bg);
    DrawRectangleLinesEx((Rectangle){box_x, box_y, box_w, box_h}, line_thickness, fg);
    DrawText(buffer0, text_x, text_y, font_size, fg);
}
