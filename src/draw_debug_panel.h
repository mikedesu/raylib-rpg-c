#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

static inline void draw_debug_panel(gamestate& g) {
    //massert(g, "gamestate is NULL");
    Color bg = Fade((Color){0, 0, 255}, 0.75f);
    Color fg = WHITE;
    int fontsize = 10;
    int w = MeasureText(g.debugpanel.buffer, fontsize);
    int h = fontsize * 30; // Assuming single line text for now
    int x = DEFAULT_TARGET_WIDTH - w;
    int y = fontsize * 2;
    DrawRectangle(x - 10 - 20, y - 10, w + 20, h + 20, bg);
    DrawRectangleLines(x - 10 - 20, y - 10, w + 20, h + 20, fg);
    DrawText(g.debugpanel.buffer, x - 20, y, fontsize, fg);
}
