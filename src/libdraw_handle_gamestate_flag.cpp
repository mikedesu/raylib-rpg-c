
#include "libdraw_check_default_animations.h"
#include "libdraw_handle_gamestate_flag.h"

void libdraw_handle_gamestate_flag(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    //if (g->flag == GAMESTATE_FLAG_PLAYER_ANIM) {
    //    if (check_default_animations(g)) {
    //        g->flag = GAMESTATE_FLAG_NPC_TURN;
    //    }
    //    //g->test_guard = false;
    //} else if (g->flag == GAMESTATE_FLAG_NPC_ANIM) {
    //    if (check_default_animations(g)) {
    //        g->entity_turn = g->hero_id; // Reset directly to hero
    //        g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
    //        g->turn_count++;
    //    }
    //}

    if (check_default_animations(g)) {
        if (g->flag == GAMESTATE_FLAG_PLAYER_ANIM) {
            g->flag = GAMESTATE_FLAG_NPC_TURN;
        } else if (g->flag == GAMESTATE_FLAG_NPC_ANIM) {
            g->entity_turn = g->hero_id; // Reset directly to hero
            g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
            g->turn_count++;
        }
    }

    //g->test_guard = false;
}
