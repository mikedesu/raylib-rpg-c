#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

static inline void draw_debug_panel(gamestate& g) {
    minfo3("draw debug panel");
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

    DrawRectanglePro(r, origin, rotation, g.get_debug_panel_bgcolor());
    DrawRectangleLinesEx(r, thickness, g.window_box_fgcolor);
    DrawText(g.debugpanel.buffer, x + xp, y, fontsize, g.window_box_fgcolor);
    msuccess3("draw debug panel");
}
