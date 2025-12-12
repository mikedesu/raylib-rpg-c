#pragma once

#include "check_default_animations.h"
#include "gamestate.h"

static inline void libdraw_handle_gamestate_flag(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    if (!check_default_animations(g))
        return;

    if (g->flag == GAMESTATE_FLAG_PLAYER_ANIM) {
#ifndef NPCS_ALL_AT_ONCE
        g->entity_turn++;
        if (g->entity_turn >= g->next_entityid)
            g->entity_turn = 0;
#endif

        g->flag = GAMESTATE_FLAG_NPC_TURN;
    } else if (g->flag == GAMESTATE_FLAG_NPC_ANIM) {
#ifndef NPCS_ALL_AT_ONCE
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
#else
        g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
        g->turn_count++;
#endif
    }
}
