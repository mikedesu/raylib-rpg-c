
#include "libdraw_handle_gamestate_flag.h"

void libdraw_handle_gamestate_flag(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    if (g->flag == GAMESTATE_FLAG_PLAYER_ANIM) {
        g->flag = GAMESTATE_FLAG_NPC_TURN;
        //g->test_guard = false;
    } else if (g->flag == GAMESTATE_FLAG_NPC_ANIM) {
        g->entity_turn = g->hero_id; // Reset directly to hero
        g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
        g->turn_count++;
    }
}
