#pragma once


#include "gamestate.h"
#include "libgame_defines.h"



static inline void draw_action_menu(const gamestate& g) {
    const Color bg = Fade((Color){0, 0, 255}, 0.75f);
    constexpr Color fg = WHITE;
    constexpr int fontsize = 10;
    constexpr int hp = 20;

    const vector<string> actions = {"Attack", "Open", "Inventory"};

    const float h = fontsize * actions.size();
    constexpr float x = 10;
    constexpr float y = 10;
    constexpr float rotation = 0;
    constexpr int thickness = 1;
    constexpr float xp = 10;
    constexpr float yp = 10;
    constexpr float wp = 20;

    //const float w = MeasureText("Attack", fontsize);
    const string s = "> Inventory";
    const float w = MeasureText(s.c_str(), fontsize);

    const Rectangle r = {x, y, w + wp, h + hp};
    constexpr Vector2 origin = {0, 0};
    DrawRectanglePro(r, origin, rotation, bg);
    DrawRectangleLinesEx(r, thickness, fg);


    //const int num_texts = 1;



    for (size_t i = 0; i < actions.size(); i++) {
        if (g.action_selection == i) {
            DrawText(TextFormat("> %s", actions[i].c_str()), x + xp, y + yp + (fontsize * i), fontsize, fg);
        } else {
            DrawText(TextFormat("  %s", actions[i].c_str()), x + xp, y + yp + (fontsize * i), fontsize, fg);
            //DrawText(actions[i].c_str(), x + xp, y + yp + (fontsize * i), fontsize, fg);
        }
    }

    //DrawText("Attack", x + xp, y + yp, fontsize, fg);
}
