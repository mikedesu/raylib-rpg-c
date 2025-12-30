#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

static inline void draw_debug_panel(const gamestate& g) {
    const Color bg = Fade((Color){0, 0, 255}, 0.75f);
    const Color fg = WHITE;
    const int fontsize = 10;
    const float w = MeasureText(g.debugpanel.buffer, fontsize);
    const float h = fontsize * 30;
    const float x = g.targetwidth / 2.0 - w / 2.0;
    const float y = fontsize * 2;
    const float yp = 10;
    const float wp = 20;
    const float hp = 20;
    const Rectangle r = {x, y - yp, w + wp, h + hp};
    const Vector2 origin = {0, 0};
    const float rotation = 0;
    DrawRectanglePro(r, origin, rotation, bg);
    const int thickness = 1;
    DrawRectangleLinesEx(r, thickness, fg);
    const float xp = 5;
    DrawText(g.debugpanel.buffer, x + xp, y, fontsize, fg);
}
