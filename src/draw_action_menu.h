#pragma once


#include "gamestate.h"
#include "libgame_defines.h"


static inline void draw_action_menu(gamestate& g) {
    constexpr Color bg = Color{0, 0, 255, 128};
    constexpr Color fg = WHITE;
    constexpr int fontsize = 10;
    constexpr int hp = 20;
    constexpr float x = 10;
    constexpr float y = 10;
    constexpr float rotation = 0;
    constexpr int thickness = 1;
    constexpr float xp = 10;
    constexpr float yp = 10;
    constexpr float wp = 20;
    constexpr Vector2 origin = {0, 0};
    const vector<string> actions = {"Attack", "Open", "Inventory"};
    const float h = fontsize * actions.size();
    const string s = "> Inventory";
    const float w = MeasureText(s.c_str(), fontsize);
    const Rectangle r = {x, y, w + wp, h + hp};
    DrawRectanglePro(r, origin, rotation, bg);
    DrawRectangleLinesEx(r, thickness, fg);
    for (size_t i = 0; i < actions.size(); i++) {
        if (g.action_selection == i) {
            DrawText(TextFormat("> %s", actions[i].c_str()), x + xp, y + yp + (fontsize * i), fontsize, fg);
        } else {
            DrawText(TextFormat("  %s", actions[i].c_str()), x + xp, y + yp + (fontsize * i), fontsize, fg);
        }
    }
}
