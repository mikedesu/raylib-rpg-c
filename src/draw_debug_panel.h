#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

static inline void draw_debug_panel(gamestate& g) {
    minfo3("draw debug panel");
    constexpr Color bg = {0, 0, 255, 255};
    constexpr Color fg = WHITE;

    constexpr int fontsize = 20;

    constexpr float yp = 10;
    constexpr float wp = 20;
    constexpr float hp = 20;
    constexpr float xp = 5;
    constexpr float rotation = 0;
    constexpr int thickness = 1;
    constexpr float h = fontsize * 30;
    constexpr Vector2 origin = {0, 0};

    const float w = MeasureText(g.debugpanel.buffer, fontsize);
    const float x = g.targetwidth - (w + wp);
    const float y = g.targetheight - (h + hp);
    const Rectangle r = {x, y - yp, w + wp, h + hp};

    DrawRectanglePro(r, origin, rotation, bg);
    DrawRectangleLinesEx(r, thickness, fg);
    DrawText(g.debugpanel.buffer, x + xp, y, fontsize, fg);
    msuccess3("draw debug panel");
}
