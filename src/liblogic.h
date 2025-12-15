#pragma once

#include "gamestate.h"
#include "inputstate.h"
#include <memory>

#define TILE_COUNT_ERROR -999

using std::shared_ptr;

void liblogic_init(shared_ptr<gamestate> g);
void liblogic_tick(shared_ptr<inputstate> is, shared_ptr<gamestate> g);
void liblogic_restart(shared_ptr<gamestate> g);


static inline void liblogic_close(shared_ptr<gamestate> g) {
    massert(g, "liblogic_close: gamestate is NULL");
    d_free(g->dungeon);
}


static inline void update_npcs_state(shared_ptr<gamestate> g) {
    for (entityid id = 0; id < g->next_entityid; id++) {
        if (id == g->hero_id)
            continue;
        unsigned char a = g->ct.get<txalpha>(id).value_or(255);
        if (a < 255)
            a++;
        g->ct.set<txalpha>(id, a);
        g->ct.set<damaged>(id, false);
    }
}
