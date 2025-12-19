#pragma once

#include "gamestate.h"
#include "inputstate.h"


static inline void handle_camera_move(gamestate& g, inputstate& is) {
    const float move = g.cam2d.zoom;
    //const char* action = get_action_key(is, g);
    if (inputstate_is_held(is, KEY_RIGHT)) {
        g.cam2d.offset.x += move;
    } else if (inputstate_is_held(is, KEY_LEFT)) {
        g.cam2d.offset.x -= move;
    } else if (inputstate_is_held(is, KEY_UP)) {
        g.cam2d.offset.y -= move;
    } else if (inputstate_is_held(is, KEY_DOWN)) {
        g.cam2d.offset.y += move;
    }
}
