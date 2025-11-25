
#include "libdraw_check_default_animations.h"
#include "libdraw_handle_gamestate_flag.h"

void libdraw_handle_gamestate_flag(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");

    if (check_default_animations(g)) {
        if (g->flag == GAMESTATE_FLAG_PLAYER_ANIM) {
            g->entity_turn++;
            if (g->entity_turn >= g->next_entityid) {
                g->entity_turn = 0;
            }
            g->flag = GAMESTATE_FLAG_NPC_TURN;
        } else if (g->flag == GAMESTATE_FLAG_NPC_ANIM) {
            g->entity_turn++;
            if (g->entity_turn >= g->next_entityid) {
                g->entity_turn = 0;
            }
            if (g->entity_turn == g->hero_id) {
                g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
                g->turn_count++;
            } else {
                g->flag = GAMESTATE_FLAG_NPC_TURN;
            }
        }
    }

    //g->test_guard = false;
}
