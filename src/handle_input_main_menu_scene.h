#pragma once

#include "gamestate.h"
#include "inputstate.h"
#include "play_sound.h"
#include "sfx.h"

//static inline void handle_input_main_menu_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
static inline void handle_input_main_menu_scene(gamestate& g, inputstate& is) {
    if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
        if (g.title_screen_selection == 0) {
            minfo("Switching to character creation scene");
            g.current_scene = SCENE_CHARACTER_CREATION;
            g.frame_dirty = true;
        }

        //PlaySound(g.sfx->at(SFX_CONFIRM_01));
        play_sound(SFX_CONFIRM_01);

    } else if (inputstate_is_pressed(is, KEY_DOWN)) {
        minfo("Title screen selection++");
        g.title_screen_selection++;
        if (g.title_screen_selection >= g.max_title_screen_selections) {
            g.title_screen_selection = 0;
        }
        //PlaySound(g.sfx->at(SFX_CONFIRM_01));
        play_sound(SFX_CONFIRM_01);
    } else if (inputstate_is_pressed(is, KEY_UP)) {
        minfo("Title screen selection--");
        g.title_screen_selection--;
        if (g.title_screen_selection < 0) {
            g.title_screen_selection = g.max_title_screen_selections - 1;
        }
        //PlaySound(g.sfx->at(SFX_CONFIRM_01));
        play_sound(SFX_CONFIRM_01);
    } else if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        g.do_quit = true;
        //PlaySound(g.sfx->at(SFX_CONFIRM_01));
        play_sound(SFX_CONFIRM_01);
    }

    g.frame_dirty = true;
}
