#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

static inline void draw_option_menu(gamestate& g) {
    constexpr Color bg = Color{0, 0, 255, 128}, fg = WHITE;
    constexpr float x = 10, y = 10, p = 20, pa = 10, rotation = 0;
    constexpr int fsize = 10, thickness = 1;
    constexpr Vector2 origin = {0, 0};
    float max_w = 0;
    for (size_t i = 0; i < g.options_menu.get_option_count(); i++) {
        const char* spaced_str = TextFormat("  %s", g.options_menu.get_option_str(g.options_menu.get_option(i)).c_str());
        const float w = MeasureText(spaced_str, fsize);
        if (w > max_w) {
            max_w = w;
        }
    }
    const float h = fsize * g.options_menu.get_option_count();
    const float padded_w = max_w + p, padded_h = h + p;
    const Rectangle r = {x, y, padded_w, padded_h};
    DrawRectanglePro(r, origin, rotation, bg);
    DrawRectangleLinesEx(r, thickness, fg);
    for (size_t i = 0; i < g.options_menu.get_option_count(); i++) {
        constexpr int x0 = x + pa;
        const int y0 = y + pa + fsize * i;
        const option_type otype = g.options_menu.get_option(i);
        const string ostr = g.options_menu.get_option_str(otype);
        const char* cstr = ostr.c_str();
        if (g.options_menu.get_selection() == i) {
            DrawText(TextFormat("> %s", cstr), x0, y0, fsize, fg);
        } else {
            DrawText(TextFormat("  %s", cstr), x0, y0, fsize, fg);
        }
    }
}
