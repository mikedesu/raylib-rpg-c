#pragma once

#include "gamestate.h"

static inline void update_debug_panel(gamestate& g) {
    // concat a string onto the end of the debug panel message
    char tmp[1024] = {0};
    strncat(g.debugpanel.buffer, tmp, sizeof(g.debugpanel.buffer) - strlen(g.debugpanel.buffer) - 1);
}
