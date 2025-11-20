#include "liblogic_handle_input_title_scene.h"
#include "sfx.h"
#include <raylib.h>

void handle_input_title_scene(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        g->do_quit = true;
        return;
    }

    if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
        minfo("Title screen input detected, switching to main menu");
        g->current_scene = SCENE_MAIN_MENU;
        g->frame_dirty = true;

        PlaySound(g->sfx->at(SFX_CONFIRM_01));
    }
}
