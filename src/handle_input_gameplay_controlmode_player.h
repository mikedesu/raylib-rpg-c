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


static inline bool handle_quit_pressed(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        g->do_quit = true;
        return true;
    }
    return false;
}

static inline bool handle_cycle_messages(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (g->msg_system_is_active && inputstate_is_pressed(is, KEY_ENTER)) {
        play_sound(SFX_CONFIRM_01);
        cycle_messages(g);
        return true;
    }
    return false;
}


static inline bool handle_change_dir_intent(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (inputstate_is_pressed(is, KEY_S)) {
        g->player_changing_dir = true;
        return true;
    }
    return false;
}


static inline bool handle_display_inventory(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (inputstate_is_pressed(is, KEY_I)) {
        g->display_inventory_menu = true;
        g->controlmode = CONTROLMODE_INVENTORY;
        g->frame_dirty = true;
        play_sound(SFX_BAG_OPEN);
        return true;
    }
    return false;
}


static inline bool handle_camera_zoom(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (inputstate_is_pressed(is, KEY_LEFT_BRACKET)) {
        g->cam2d.zoom += DEFAULT_ZOOM_INCR;
        g->frame_dirty = true;
        return true;
    } else if (inputstate_is_pressed(is, KEY_RIGHT_BRACKET)) {
        g->cam2d.zoom -= (g->cam2d.zoom > 1.0) ? DEFAULT_ZOOM_INCR : 0.0;
        g->frame_dirty = true;
        return true;
    }
    return false;
}


static inline bool handle_change_dir(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (g->player_changing_dir) {
        auto maybe_player_is_dead = g->ct.get<dead>(g->hero_id);
        if (!maybe_player_is_dead.has_value())
            return true;
        const bool is_dead = maybe_player_is_dead.value();


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
            if (is_dead) {
                add_message(g, "You cannot attack while dead");
                return true;
            }

            g->ct.set<attacking>(g->hero_id, true);
            g->ct.set<update>(g->hero_id, true);
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            g->player_changing_dir = false;
        }
        return true;
    }
    return false;
}


static inline bool handle_move_up(shared_ptr<gamestate> g, shared_ptr<inputstate> is, bool is_dead) {
    if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_W)) {
        if (is_dead) {
            add_message(g, "You cannot move while dead");
            return true;
        }

        try_entity_move(g, g->hero_id, (vec3){0, -1, 0});
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}


static inline bool handle_move_down(shared_ptr<gamestate> g, shared_ptr<inputstate> is, bool is_dead) {
    if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_X)) {
        if (is_dead) {
            add_message(g, "You cannot move while dead");
            return true;
        }
        try_entity_move(g, g->hero_id, (vec3){0, 1, 0});
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}

static inline bool handle_move_left(shared_ptr<gamestate> g, shared_ptr<inputstate> is, bool is_dead) {
    if (inputstate_is_pressed(is, KEY_LEFT) || inputstate_is_pressed(is, KEY_A)) {
        if (is_dead) {
            add_message(g, "You cannot move while dead");
            return true;
        }
        try_entity_move(g, g->hero_id, (vec3){-1, 0, 0});
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}


static inline bool handle_move_right(shared_ptr<gamestate> g, shared_ptr<inputstate> is, bool is_dead) {
    if (inputstate_is_pressed(is, KEY_RIGHT) || inputstate_is_pressed(is, KEY_D)) {
        if (is_dead) {
            add_message(g, "You cannot move while dead");
            return true;
        }
        try_entity_move(g, g->hero_id, (vec3){1, 0, 0});
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}


static inline bool handle_move_up_left(shared_ptr<gamestate> g, shared_ptr<inputstate> is, bool is_dead) {
    if (inputstate_is_pressed(is, KEY_Q)) {
        if (is_dead) {
            add_message(g, "You cannot move while dead");
            return true;
        }
        try_entity_move(g, g->hero_id, (vec3){-1, -1, 0});
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}


static inline bool handle_move_up_right(shared_ptr<gamestate> g, shared_ptr<inputstate> is, bool is_dead) {
    if (inputstate_is_pressed(is, KEY_E)) {
        if (is_dead) {
            add_message(g, "You cannot move while dead");
            return true;
        }
        try_entity_move(g, g->hero_id, (vec3){1, -1, 0});
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}


static inline bool handle_move_down_left(shared_ptr<gamestate> g, shared_ptr<inputstate> is, bool is_dead) {
    if (inputstate_is_pressed(is, KEY_Z)) {
        if (is_dead) {
            add_message(g, "You cannot move while dead");
            return true;
        }
        try_entity_move(g, g->hero_id, (vec3){-1, 1, 0});
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}


static inline bool handle_move_down_right(shared_ptr<gamestate> g, shared_ptr<inputstate> is, bool is_dead) {
    if (inputstate_is_pressed(is, KEY_C)) {
        if (is_dead) {
            add_message(g, "You cannot move while dead");
            return true;
        }
        try_entity_move(g, g->hero_id, (vec3){1, 1, 0});
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}


static inline bool handle_attack(shared_ptr<gamestate> g, shared_ptr<inputstate> is, bool is_dead) {
    if (inputstate_is_pressed(is, KEY_APOSTROPHE)) {
        if (is_dead) {
            add_message(g, "You cannot attack while dead");
            return true;
        }
        if (g->ct.get<location>(g->hero_id).has_value() && g->ct.get<direction>(g->hero_id).has_value()) {
            const vec3 loc = get_loc_facing_player(g);
            try_entity_attack(g, g->hero_id, loc.x, loc.y);
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
    }
    return false;
}

static inline bool handle_pickup_item(shared_ptr<gamestate> g, shared_ptr<inputstate> is, bool is_dead) {
    if (inputstate_is_pressed(is, KEY_SLASH)) {
        if (is_dead) {
            add_message(g, "You cannot pick up items while dead");
            return true;
        }
        try_entity_pickup(g, g->hero_id);
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}

//static inline bool handle_attack(shared_ptr<gamestate> g, shared_ptr<inputstate> is, bool is_dead) {
//}

static inline bool handle_traverse_stairs(shared_ptr<gamestate> g, shared_ptr<inputstate> is, bool is_dead) {
    if (inputstate_is_pressed(is, KEY_PERIOD)) {
        if (is_dead) {
            add_message(g, "You cannot traverse stairs while dead");
            return true;
        }
        try_entity_stairs(g, g->hero_id);
        return true;
    }
    return false;
}


static inline bool handle_restart(shared_ptr<gamestate> g, shared_ptr<inputstate> is, bool is_dead) {
    if (inputstate_is_pressed(is, KEY_R) && is_dead) {
        g->do_restart = true;
        minfo("setting do_restart to true...");
        return true;
    }
    return false;
}


static inline bool handle_open_door(shared_ptr<gamestate> g, shared_ptr<inputstate> is, bool is_dead) {
    if (inputstate_is_pressed(is, KEY_O)) {
        if (is_dead) {
            add_message(g, "You cannot open doors while dead");
            return true;
        }
        const vec3 loc = get_loc_facing_player(g);
        try_entity_open_door(g, g->hero_id, loc);
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}


static inline void handle_input_gameplay_controlmode_player(shared_ptr<gamestate> g, shared_ptr<inputstate> is) {
    if (g->flag != GAMESTATE_FLAG_PLAYER_INPUT)
        return;
    if (handle_quit_pressed(g, is))
        return;
    if (handle_cycle_messages(g, is))
        return;
    // make sure player isnt dead
    auto maybe_player_is_dead = g->ct.get<dead>(g->hero_id);
    if (!maybe_player_is_dead.has_value())
        return;
    const bool is_dead = maybe_player_is_dead.value();
    if (handle_camera_zoom(g, is))
        return;
    if (handle_change_dir(g, is))
        return;
    if (handle_change_dir_intent(g, is))
        return;
    if (handle_display_inventory(g, is))
        return;
    if (handle_move_up(g, is, is_dead))
        return;
    if (handle_move_down(g, is, is_dead))
        return;
    if (handle_move_left(g, is, is_dead))
        return;
    if (handle_move_right(g, is, is_dead))
        return;
    if (handle_move_up_left(g, is, is_dead))
        return;
    if (handle_move_up_right(g, is, is_dead))
        return;
    if (handle_move_down_left(g, is, is_dead))
        return;
    if (handle_move_down_right(g, is, is_dead))
        return;
    if (handle_attack(g, is, is_dead))
        return;
    if (handle_pickup_item(g, is, is_dead))
        return;
    if (handle_traverse_stairs(g, is, is_dead))
        return;
    if (handle_open_door(g, is, is_dead))
        return;
    if (handle_restart(g, is, is_dead))
        return;
}
