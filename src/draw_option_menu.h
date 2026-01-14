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
    //const string text = "Option Menu";

    float max_w = 0;

    for (size_t i = 0; i < g.options_menu.get_option_count(); i++)
    {
        const char* spaced_str = TextFormat("  %s", g.options_menu.get_option_str(g.options_menu.get_option(i)).c_str());
        const float w = MeasureText(spaced_str, fontsize);
        if (w > max_w)
        {
            max_w = w;
        }
    }

    const float h = fontsize * g.options_menu.get_option_count();
    const Rectangle r = {x, y, max_w + p, h + p};
    DrawRectanglePro(r, origin, rotation, bg);
    DrawRectangleLinesEx(r, thickness, fg);
    for (size_t i = 0; i < g.options_menu.get_option_count(); i++)
    {
        if (g.options_menu.get_selection() == i)
        {
            DrawText(TextFormat("+ %s", g.options_menu.get_option_str(g.options_menu.get_option(i)).c_str()), x + pa, y + pa + (fontsize * i), fontsize, fg);
        }
        else
        {
            DrawText(TextFormat("  %s", g.options_menu.get_option_str(g.options_menu.get_option(i)).c_str()), x + pa, y + pa + (fontsize * i), fontsize, fg);
        }
    }
}
