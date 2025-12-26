#pragma once

#include "gamestate.h"
#include "handle_input_gameplay_scene.h"
#include "inputstate.h"

static inline void handle_input(gamestate& g, inputstate& is) {
    // no matter which mode we are in, we can toggle the debug panel
    if (inputstate_is_pressed(is, KEY_P)) {
        g.debugpanelon = !g.debugpanelon;
        minfo("Toggling debug panel: %s", g.debugpanelon ? "ON" : "OFF");
        return;
    }

    if (g.current_scene == SCENE_TITLE)
        g.handle_input_title_scene(is);
    else if (g.current_scene == SCENE_MAIN_MENU)
        g.handle_input_main_menu_scene(is);
    else if (g.current_scene == SCENE_CHARACTER_CREATION)
        g.handle_input_character_creation_scene(is);
    else if (g.current_scene == SCENE_GAMEPLAY)
        handle_input_gameplay_scene(g, is);
}
