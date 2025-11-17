#include "libgame_defines.h"
#include "liblogic_change_player_dir.h"
#include "liblogic_cycle_messages.h"
#include "liblogic_handle_input_gameplay_controlmode_player.h"
#include "liblogic_handle_input_help_menu.h"
#include "liblogic_try_entity_attack.h"
#include "liblogic_try_entity_move.h"
#include "liblogic_try_entity_pickup.h"
#include "sfx.h"
#include <raylib.h>

void handle_input_gameplay_controlmode_player(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        g->do_quit = true;
        return;
    }

    if (g->display_help_menu) {
        handle_input_help_menu(g, is);
    }

    if (g->msg_system_is_active) {
        // press enter to cycle thru message
        if (inputstate_is_pressed(is, KEY_ENTER)) {
            cycle_messages(g);
        }
        return;
    }

    if (g->flag == GAMESTATE_FLAG_PLAYER_INPUT) {
        if (inputstate_is_pressed(is, KEY_LEFT_BRACKET)) {
            g->cam2d.zoom += DEFAULT_ZOOM_INCR;
            g->frame_dirty = true;
            //return;
        } else if (inputstate_is_pressed(is, KEY_RIGHT_BRACKET)) {
            g->cam2d.zoom -= (g->cam2d.zoom > 1.0) ? DEFAULT_ZOOM_INCR : 0.0;
            g->frame_dirty = true;
        }

        if (g->player_changing_dir) {
            // double 's' is wait one turn
            if (inputstate_is_pressed(is, KEY_S)) {
                g->player_changing_dir = false;
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_W)) {
                change_player_dir(g, DIR_UP);
            } else if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_X)) {
                change_player_dir(g, DIR_DOWN);
            } else if (inputstate_is_pressed(is, KEY_LEFT) || inputstate_is_pressed(is, KEY_A)) {
                change_player_dir(g, DIR_LEFT);
            } else if (inputstate_is_pressed(is, KEY_RIGHT) || inputstate_is_pressed(is, KEY_D)) {
                change_player_dir(g, DIR_RIGHT);
            } else if (inputstate_is_pressed(is, KEY_Q)) {
                change_player_dir(g, DIR_UP_LEFT);
            } else if (inputstate_is_pressed(is, KEY_E)) {
                change_player_dir(g, DIR_UP_RIGHT);
            } else if (inputstate_is_pressed(is, KEY_Z)) {
                change_player_dir(g, DIR_DOWN_LEFT);
            } else if (inputstate_is_pressed(is, KEY_C)) {
                change_player_dir(g, DIR_DOWN_RIGHT);
            } else if (inputstate_is_pressed(is, KEY_APOSTROPHE)) {
                //            g_set_attacking(g, g->hero_id, true);
                g->ct.set<attacking>(g->hero_id, true);
                //            g_set_update(g, g->hero_id, true);
                g->ct.set<update>(g->hero_id, true);
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
                g->player_changing_dir = false;

                //PlaySound(g->sfx->at(SFX_SLASH_ATTACK_SWORD_1));
            }
            return;
        }

        if (inputstate_is_pressed(is, KEY_S)) {
            g->player_changing_dir = true;
            return;
        }

        if (g->hero_id != ENTITYID_INVALID) {
            //vec3 loc = g_get_loc(g, g->hero_id);
            if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_W)) {
                try_entity_move(g, g->hero_id, (vec3){0, -1, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_X)) {
                try_entity_move(g, g->hero_id, (vec3){0, 1, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_LEFT) || inputstate_is_pressed(is, KEY_A)) {
                try_entity_move(g, g->hero_id, (vec3){-1, 0, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_RIGHT) || inputstate_is_pressed(is, KEY_D)) {
                try_entity_move(g, g->hero_id, (vec3){1, 0, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_Q)) {
                try_entity_move(g, g->hero_id, (vec3){-1, -1, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_E)) {
                try_entity_move(g, g->hero_id, (vec3){1, -1, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_Z)) {
                try_entity_move(g, g->hero_id, (vec3){-1, 1, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_C)) {
                try_entity_move(g, g->hero_id, (vec3){1, 1, 0});
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_APOSTROPHE)) {
                //PlaySound(g->sfx->at(SFX_SLASH_ATTACK_SWORD_1));

                if (g->ct.get<location>(g->hero_id).has_value() && g->ct.get<direction>(g->hero_id).has_value()) {
                    vec3 loc = g->ct.get<location>(g->hero_id).value();
                    direction_t dir = g->ct.get<direction>(g->hero_id).value();
                    if (dir == DIR_UP) {
                        loc.y -= 1;
                    } else if (dir == DIR_DOWN) {
                        loc.y += 1;
                    } else if (dir == DIR_LEFT) {
                        loc.x -= 1;
                    } else if (dir == DIR_RIGHT) {
                        loc.x += 1;
                    } else if (dir == DIR_UP_LEFT) {
                        loc.x -= 1;
                        loc.y -= 1;
                    } else if (dir == DIR_UP_RIGHT) {
                        loc.x += 1;
                        loc.y -= 1;
                    } else if (dir == DIR_DOWN_LEFT) {
                        loc.x -= 1;
                        loc.y += 1;
                    } else if (dir == DIR_DOWN_RIGHT) {
                        loc.x += 1;
                        loc.y += 1;
                    }

                    try_entity_attack(g, g->hero_id, loc.x, loc.y);

                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
                }

                //} else if (inputstate_is_pressed(is, KEY_SEMICOLON)) {
            } else if (inputstate_is_pressed(is, KEY_SLASH)) {
                //add_message(g, "pickup item (unimplemented)");
                try_entity_pickup(g, g->hero_id);
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            }
            //else if (inputstate_is_pressed(is, KEY_SLASH) && inputstate_is_shift_held(is)) {
            //    // open inventory
            //    //g->inventory_is_open = !g->inventory_is_open;
            //    g->display_help_menu = !g->display_help_menu;
            //    g->frame_dirty = true;
            //}
            else if (inputstate_is_pressed(is, KEY_I)) {
                g->display_inventory_menu = true;
                g->controlmode = CONTROLMODE_INVENTORY;
                g->frame_dirty = true;
            }
            return;
        }
    }
}
