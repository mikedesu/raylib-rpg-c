#pragma once


#include "gamestate.h"
#include "libgame_defines.h"


static inline void draw_option_menu(gamestate& g)
{
    constexpr Color bg = Color{0, 0, 255, 128};
    constexpr Color fg = WHITE;
    constexpr int fontsize = 10;
    constexpr float x = 10;
    constexpr float y = 10;
    constexpr float rotation = 0;
    constexpr int thickness = 1;
    constexpr Vector2 origin = {0, 0};

    constexpr float p = 20;
    constexpr float pa = 10;

    const string text = "Option Menu";


    const float w = MeasureText(text.c_str(), fontsize);
    const float h = fontsize;

    const Rectangle r = {x, y, w + p, h + p};
    DrawRectanglePro(r, origin, rotation, bg);
    DrawRectangleLinesEx(r, thickness, fg);

    DrawText(TextFormat("%s", text.c_str()), x + pa, y + pa, fontsize, fg);
}
