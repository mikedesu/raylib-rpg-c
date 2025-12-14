#pragma once

#include "ComponentTraits.h"
#include "gamestate.h"
using std::max;

static inline void draw_hud(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    const int turn = g->turn_count;

    const int myhp = g->ct.get<hp>(g->hero_id).value_or(-1);
    const int mymaxhp = g->ct.get<maxhp>(g->hero_id).value_or(-1);

    const int mylevel = g->ct.get<level>(g->hero_id).value_or(0);

    const int myxp = g->ct.get<xp>(g->hero_id).value_or(0);
    const int attack_bonus = 0;
    const int ac = 0;
    const int floor = g->dungeon->current_floor;
    const int font_size = 10;
    const int line_thickness = 1;

    char name_buffer[1024] = {0};
    char stats_buffer[1024] = {0};
    char stats2_buffer[1024] = {0};
    char floor_buffer[1024] = {0};

    const Color bg = Fade((Color){0, 0, 0xff, 0xFF}, 0.5f);
    const Color fg = WHITE;

    const string n = g->ct.get<name>(g->hero_id).value_or("no-name");
    const vec3 loc = g->ct.get<location>(g->hero_id).value_or((vec3){-1, -1, -1});

    const int str = g->ct.get<strength>(g->hero_id).value_or(-1);
    const int dex = g->ct.get<dexterity>(g->hero_id).value_or(-1);
    const int con = g->ct.get<constitution>(g->hero_id).value_or(-1);
    const int int_ = g->ct.get<intelligence>(g->hero_id).value_or(-1);
    const int wis = g->ct.get<wisdom>(g->hero_id).value_or(-1);
    const int cha = g->ct.get<charisma>(g->hero_id).value_or(-1);

    // Format each line separately
    snprintf(name_buffer, sizeof(name_buffer), "%s", n.c_str());
    snprintf(stats_buffer, sizeof(stats_buffer), "Lvl %d HP %d/%d  Atk: %d  AC: %d", mylevel, myhp, mymaxhp, attack_bonus, ac);
    snprintf(stats2_buffer, sizeof(stats_buffer), "Str %d Dex %d Con %d Int %d Wis %d Cha %d", str, dex, con, int_, wis, cha);
    snprintf(floor_buffer, sizeof(floor_buffer), "Location: (%d, %d, %d) Turn %d  XP %d", loc.x, loc.y, floor, turn, myxp);

    // Calculate max width of all lines
    const int name_width = MeasureText(name_buffer, font_size);
    const int stats_width = MeasureText(stats_buffer, font_size);
    const int stats2_width = MeasureText(stats2_buffer, font_size);
    const int floor_width = MeasureText(floor_buffer, font_size);
    const int max_width = max({name_width, stats_width, floor_width, stats2_width});

    // Calculate box dimensions based on widest line
    const float box_w = max_width + g->pad;
    const float box_h = (font_size + g->pad * 3); // 3 lines

    // Position box at bottom center
    const float box_x = g->targetwidth / 2.0f - (box_w / 2.0f);
    const float box_y = g->targetheight - box_h - g->pad * 1.0f;

    // Draw background box
    DrawRectangleRec((Rectangle){box_x, box_y, box_w, box_h}, bg);
    DrawRectangleLinesEx((Rectangle){box_x, box_y, box_w, box_h}, line_thickness, fg);

    // Draw each line with vertical spacing
    const int text_x = box_x + g->pad / 2.0f;
    int text_y = box_y + g->pad / 2.0f;

    DrawText(name_buffer, text_x, text_y, font_size, fg);
    text_y += font_size + 2;

    DrawText(stats_buffer, text_x, text_y, font_size, fg);
    text_y += font_size + 2;

    DrawText(stats2_buffer, text_x, text_y, font_size, fg);
    text_y += font_size + 2;

    DrawText(floor_buffer, text_x, text_y, font_size, fg);
    text_y += font_size + 2;
}
