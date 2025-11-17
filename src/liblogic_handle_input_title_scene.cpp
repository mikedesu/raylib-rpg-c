#include "liblogic_handle_input_title_scene.h"

void handle_input_title_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
        minfo("Title screen input detected, switching to main menu");
        g->current_scene = SCENE_MAIN_MENU;
        g->frame_dirty = true;
    }
}
