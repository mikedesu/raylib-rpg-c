#pragma once

#include "gamestate.h"
#include "inputstate.h"
#include "play_sound.h"
#include "sfx.h"

static inline void handle_input_title_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        g->do_quit = true;
        return;
    }

    if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
        minfo("Title screen input detected, switching to main menu");
        g->current_scene = SCENE_MAIN_MENU;
        g->frame_dirty = true;

        play_sound(SFX_CONFIRM_01);
    }
}
