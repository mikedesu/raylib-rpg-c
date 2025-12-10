#pragma once

#include "gamestate.h"
#include "handle_input_char_creation_scene.h"
#include "handle_input_gameplay_scene.h"
#include "handle_input_main_menu_scene.h"
#include "handle_input_title_scene.h"
#include "inputstate.h"
#include "scene.h"

static inline void handle_input(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    massert(is, "inputstate is NULL");
    massert(g, "gamestate is NULL");

    // no matter which mode we are in, we can toggle the debug panel

    if (inputstate_is_pressed(is, KEY_P)) {
        g->debugpanelon = !g->debugpanelon;
        minfo("Toggling debug panel: %s", g->debugpanelon ? "ON" : "OFF");
        return;
    }

    //switch (g->current_scene) {
    //case SCENE_TITLE:
    //case SCENE_MAIN_MENU: handle_input_title_scene(g, is); break;
    //case SCENE_CHARACTER_CREATION: handle_input_character_creation_scene(g, is); break;
    //case SCENE_GAMEPLAY: handle_input_title_scene(g, is); break;
    //default: break;
    //}

    if (g->current_scene == SCENE_TITLE) {
        handle_input_title_scene(g, is);
    } else if (g->current_scene == SCENE_MAIN_MENU) {
        handle_input_main_menu_scene(g, is);
    } else if (g->current_scene == SCENE_CHARACTER_CREATION) {
        handle_input_character_creation_scene(g, is);
    } else if (g->current_scene == SCENE_GAMEPLAY) {
        handle_input_gameplay_scene(g, is);
    }
}
