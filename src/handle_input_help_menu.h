/** @file handle_input_help_menu.h
 *  @brief Legacy helper for dismissing the help menu on any input.
 */

#pragma once

#include "gamestate.h"
#include "inputstate.h"

/** @brief Close the help menu when any input is pressed. */
static inline void handle_input_help_menu(shared_ptr<gamestate> g, inputstate& is) {
    //massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (inputstate_any_pressed(is)) {
        g->display_help_menu = false;
        g->controlmode = CONTROLMODE_PLAYER;
        return;
    }
}
