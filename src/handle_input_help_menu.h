#pragma once

#include "gamestate.h"
#include "inputstate.h"

static inline void handle_input_help_menu(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (inputstate_any_pressed(is)) {
        g->display_help_menu = false;
        g->controlmode = CONTROLMODE_PLAYER;
        return;
    }
}
