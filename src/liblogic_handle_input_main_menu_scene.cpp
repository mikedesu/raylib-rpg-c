#include "liblogic_handle_input_main_menu_scene.h"

void handle_input_main_menu_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
        if (g->title_screen_selection == 0) {
            minfo("Switching to character creation scene");
            g->current_scene = SCENE_CHARACTER_CREATION;
            g->frame_dirty = true;
        }
    } else if (inputstate_is_pressed(is, KEY_DOWN)) {
        minfo("Title screen selection++");
        g->title_screen_selection++;
        if (g->title_screen_selection >= g->max_title_screen_selections) {
            g->title_screen_selection = 0;
        }
    } else if (inputstate_is_pressed(is, KEY_UP)) {
        minfo("Title screen selection--");
        g->title_screen_selection--;
        if (g->title_screen_selection < 0) {
            g->title_screen_selection = g->max_title_screen_selections - 1;
        }
    } else if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        g->do_quit = true;
    }

    g->frame_dirty = true;
}
