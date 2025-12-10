#pragma once

#include "gamestate.h"
#include "handle_camera_move.h"
#include "handle_input_gameplay_controlmode_player.h"
#include "handle_input_inventory.h"
#include "inputstate.h"

static inline void handle_input_gameplay_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    massert(g, "Game state is NULL!");
    massert(is, "Input state is NULL!");
    if (inputstate_is_pressed(is, KEY_B)) {
        if (g->controlmode == CONTROLMODE_PLAYER) {
            g->controlmode = CONTROLMODE_CAMERA;
        } else if (g->controlmode == CONTROLMODE_CAMERA) {
            g->controlmode = CONTROLMODE_PLAYER;
        }
        g->frame_dirty = true;
        //    //return;
    }
    if (g->controlmode == CONTROLMODE_CAMERA) {
        handle_camera_move(g, is);
        g->frame_dirty = true;
        //return;
    }
    if (g->controlmode == CONTROLMODE_PLAYER) {
        handle_input_gameplay_controlmode_player(g, is);
        return;
    }
    if (g->controlmode == CONTROLMODE_INVENTORY) {
        handle_input_inventory(is, g);
        return;
    }
}
