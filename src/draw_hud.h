#pragma once


#include "ComponentTraits.h"
#include "gamestate.h"
#include "stat_bonus.h"


using std::max;


static inline void draw_hud(gamestate& g) {
    const int turn = g.turn_count;
    const int myhp = g.ct.get<hp>(g.hero_id).value_or(-1);
    const int mymaxhp = g.ct.get<maxhp>(g.hero_id).value_or(-1);
    const int mylevel = g.ct.get<level>(g.hero_id).value_or(0);
    const int myxp = g.ct.get<xp>(g.hero_id).value_or(0);
    const int attack_bonus = get_stat_bonus(g.ct.get<strength>(g.hero_id).value_or(10));
    const int ac = g.compute_armor_class(g.hero_id);
    const int floor = g.d.current_floor;
    const int font_size = 10;
    const int line_thickness = 1;
    const int str = g.ct.get<strength>(g.hero_id).value_or(-1);
    const int dex = g.ct.get<dexterity>(g.hero_id).value_or(-1);
    const int con = g.ct.get<constitution>(g.hero_id).value_or(-1);
    const int int_ = g.ct.get<intelligence>(g.hero_id).value_or(-1);
    const int wis = g.ct.get<wisdom>(g.hero_id).value_or(-1);
    const int cha = g.ct.get<charisma>(g.hero_id).value_or(-1);
    const string n = g.ct.get<name>(g.hero_id).value_or("no-name");
    const vec3 loc = g.ct.get<location>(g.hero_id).value_or((vec3){-1, -1, -1});
    char bf[4][1024] = {
        {0},
        {0},
        {0},
        {0},
    };
    const Color bg = Fade((Color){0, 0, 0xff, 0xFF}, 0.5f), fg = WHITE;
    // Format each line separately
    snprintf(bf[0], sizeof(bf[0]), "%s", n.c_str());
    snprintf(bf[1], sizeof(bf[1]), "Lvl %d HP %d/%d  Atk: %d  AC: %d", mylevel, myhp, mymaxhp, attack_bonus, ac);
    snprintf(bf[2], sizeof(bf[2]), "Str %d Dex %d Con %d Int %d Wis %d Cha %d", str, dex, con, int_, wis, cha);
    snprintf(bf[3], sizeof(bf[3]), "Location: (%d, %d, %d) Turn %d  XP %d", loc.x, loc.y, floor, turn, myxp);
    // Calculate max width of all lines
    const int b0_w = MeasureText(bf[0], font_size);
    const int b1_w = MeasureText(bf[1], font_size);
    const int b2_w = MeasureText(bf[2], font_size);
    const int b3_w = MeasureText(bf[3], font_size);
    const int max_w = max({b0_w, b1_w, b2_w, b3_w});
    // Calculate box dimensions based on widest line
    const float box_w = max_w + g.pad;
    const float box_h = (font_size + g.pad * 3); // 3 lines
    // Position box at bottom center
    const float box_x = g.targetwidth / 2.0f - (box_w / 2.0f);
    const float box_y = g.targetheight - box_h - g.pad * 1.0f;
    // Draw background box
    DrawRectangleRec((Rectangle){box_x, box_y, box_w, box_h}, bg);
    DrawRectangleLinesEx((Rectangle){box_x, box_y, box_w, box_h}, line_thickness, fg);
    // Draw each line with vertical spacing
    const int text_x = box_x + g.pad / 2.0f;
    int text_y = box_y + g.pad / 2.0f;
    for (int i = 0; i < 4; i++) {
        DrawText(bf[i], text_x, text_y, font_size, fg);
        text_y += font_size + 2;
    }
}
