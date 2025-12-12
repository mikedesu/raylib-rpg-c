#pragma once

#include "add_message.h"
#include "change_player_dir.h"
#include "cycle_messages.h"
#include "gamestate.h"
#include "get_loc_facing_player.h"
#include "handle_input_help_menu.h"
#include "sfx.h"
#include "try_entity_attack.h"
#include "try_entity_move.h"
#include "try_entity_open_door.h"
#include "try_entity_pickup.h"
#include "try_entity_stairs.h"


static inline void handle_input_gameplay_controlmode_player(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        g->do_quit = true;
        return;
    }

    if (g->display_help_menu) {
        handle_input_help_menu(g, is);
        return;
    }

    if (g->msg_system_is_active) {
        // press enter to cycle thru message
        if (inputstate_is_pressed(is, KEY_ENTER)) {
            PlaySound(g->sfx->at(SFX_CONFIRM_01));
            cycle_messages(g);
        }
        return;
    }

    if (g->flag == GAMESTATE_FLAG_PLAYER_INPUT) {
        if (inputstate_is_pressed(is, KEY_LEFT_BRACKET)) {
            g->cam2d.zoom += DEFAULT_ZOOM_INCR;
            g->frame_dirty = true;
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
                // this is incorrect
                // this should be a regular attack
                g->ct.set<attacking>(g->hero_id, true);
                g->ct.set<update>(g->hero_id, true);
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
                g->player_changing_dir = false;
            }
            return;
        }

        if (inputstate_is_pressed(is, KEY_S)) {
            g->player_changing_dir = true;
            return;
        }

        if (inputstate_is_pressed(is, KEY_I)) {
            g->display_inventory_menu = true;
            g->controlmode = CONTROLMODE_INVENTORY;
            g->frame_dirty = true;
            //PlaySound(g->sfx->at(SFX_CONFIRM_01));
            PlaySound(g->sfx->at(SFX_BAG_OPEN));
            return;
        }

        if (g->hero_id == ENTITYID_INVALID)
            return;

        // make sure player isnt dead
        auto maybe_player_is_dead = g->ct.get<dead>(g->hero_id);
        if (!maybe_player_is_dead.has_value())
            return;

        const bool player_is_dead = maybe_player_is_dead.value();
        if (player_is_dead) {
            add_message(g, "You cant act while dead!");
            return;
        }


        // Handling movement
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
        }
        // Handling attack
        else if (inputstate_is_pressed(is, KEY_APOSTROPHE)) {
            if (g->ct.get<location>(g->hero_id).has_value() && g->ct.get<direction>(g->hero_id).has_value()) {
                const vec3 loc = get_loc_facing_player(g);
                try_entity_attack(g, g->hero_id, loc.x, loc.y);
                g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            }
        }
        // handling pickup item
        else if (inputstate_is_pressed(is, KEY_SLASH)) {
            try_entity_pickup(g, g->hero_id);
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        }
        // handling go up/down stairs
        // what happens when we go up or down stairs?
        else if (inputstate_is_pressed(is, KEY_PERIOD))
            try_entity_stairs(g, g->hero_id);
        // handling open door
        else if (inputstate_is_pressed(is, KEY_O)) {
            const vec3 loc = get_loc_facing_player(g);
            try_entity_open_door(g, g->hero_id, loc);
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        }
    }
}
