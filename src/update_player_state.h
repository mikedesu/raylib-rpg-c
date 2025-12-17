#pragma once

#include "gamestate.h"

static inline void update_player_state(shared_ptr<gamestate> g) {
    massert(g->hero_id != ENTITYID_INVALID, "heroid is invalid");
    unsigned char a = g->ct.get<txalpha>(g->hero_id).value_or(255);
    if (a < 255)
        a++;
    g->ct.set<txalpha>(g->hero_id, a);
    if (g->ct.get<dead>(g->hero_id).value_or(true)) {
        g->gameover = true;
        return;
    }
    g->ct.set<blocking>(g->hero_id, false);
    g->ct.set<block_success>(g->hero_id, false);
    g->ct.set<damaged>(g->hero_id, false);
}
