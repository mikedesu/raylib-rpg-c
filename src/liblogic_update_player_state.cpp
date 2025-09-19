#include "gamestate.h"

void update_player_state(shared_ptr<gamestate> g) {
    massert(g, "Game state is NULL!");
    if (!g->gameover) {
        g_incr_tx_alpha(g, g->hero_id, 2);

        //if (g_is_dead(g, g->hero_id)) {
        if (g->ct.get<Dead>(g->hero_id).value_or(true)) {
            //add_message_history(g, "You died!");
            g->gameover = true;
        }
        //check_and_handle_level_up(g, g->hero_id);
    }
    //if (g_is_dead(g, g->hero_id)) {
    if (g->ct.get<Dead>(g->hero_id).value_or(true)) {
        return;
    }
}
