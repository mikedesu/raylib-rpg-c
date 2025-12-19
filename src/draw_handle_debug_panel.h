#pragma once

#include "draw_debug_panel.h"
#include "draw_update_debug_panel.h"
#include "gamestate.h"

static inline void handle_debug_panel(gamestate& g) {
    if (g.debugpanelon) {
        update_debug_panel(g);
        draw_debug_panel(g);
    }
}
