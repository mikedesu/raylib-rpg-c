#pragma once

#include "gamestate.h"
#include "handle_npc.h"

static inline void handle_npcs(gamestate& g) {
    if (g.flag == GAMESTATE_FLAG_NPC_TURN) {
#ifndef NPCS_ALL_AT_ONCE
        if (g.entity_turn >= 0 && g.entity_turn < g.next_entityid) {
            handle_npc(g, g.entity_turn);
            g.flag = GAMESTATE_FLAG_NPC_ANIM;
        }
#else
        for (entityid id = 0; id < g.next_entityid; id++) {
            handle_npc(g, id);
        }
        g.flag = GAMESTATE_FLAG_NPC_ANIM;
#endif
    }
}
