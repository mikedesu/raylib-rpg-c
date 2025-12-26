#pragma once

#include "gamestate.h"
#include "libgame_defines.h"

static inline void draw_debug_panel(gamestate& g) {
    const Color bg = Fade((Color){0, 0, 255}, 0.75f);
    const Color fg = WHITE;
    const int fontsize = 10;
    const float w = MeasureText(g.debugpanel.buffer, fontsize);
    const float h = fontsize * 30; // Assuming single line text for now
    //const float x = DEFAULT_TARGET_WIDTH - w;
    const float x = 0;
    const float y = fontsize * 2;
    const Rectangle r = {x, y - 10, w + 20, h + 20};
    //DrawRectangle(x - 10 - 20, y - 10, w + 20, h + 20, bg);
    DrawRectanglePro(r, (Vector2){0, 0}, 0, bg);
    DrawRectangleLinesEx(r, 1, fg);
    DrawText(g.debugpanel.buffer, x + 5, y, fontsize, fg);
}
