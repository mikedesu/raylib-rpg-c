#include "libdraw_hud.h"

void draw_hud(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    int turn = g->turn_count;
    int hp = 0;
    int maxhp = 0;
    int level = 0;
    int xp = 0;
    int next_level_xp = 0;
    int attack_bonus = 0;
    int ac = 0;
    int floor = g->dungeon->current_floor;
    int font_size = 10;
    char buffer0[1024] = {0};
    char buffer1[1024] = {0};
    const char* format_str_0 = "%s Lvl %d HP %d/%d Atk: %d AC: %d XP %d/%d";
    const char* format_str_1 = "Floor %d Turn %d";

    string n = g->ct.get<name>(g->hero_id).value_or("no-name");
    snprintf(buffer0, sizeof(buffer0), format_str_0, n.c_str(), level, hp, maxhp, attack_bonus, ac, xp, next_level_xp);
    snprintf(buffer1, sizeof(buffer1), format_str_1, floor, turn);
    int text_size0 = MeasureText(buffer0, font_size);
    float box_w = text_size0 + g->pad * 2;
    float box_h = font_size + g->pad;
    // instead, lets position the HUD at the top right corner of the screen
    float box_x = 0;
    float box_y = 0;
    Color bg = (Color){0x33, 0x33, 0x33, 0xFF}, fg = WHITE;
    DrawRectangleRec((Rectangle){box_x, box_y, box_w, box_h}, bg);
    int line_thickness = 1;
    DrawRectangleLinesEx((Rectangle){box_x, box_y, box_w, box_h}, line_thickness, fg);
    // Calculate text position to center it within the box
    int text_x = box_x + 10;
    int text_y = box_y + (box_h - font_size) / 2;
    DrawText(buffer0, text_x, text_y, font_size, fg);
}
