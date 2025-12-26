#pragma once

#include "gamestate.h"
#include "handle_input.h"
#include "handle_npcs.h"
#include "inputstate.h"
#include "update_debug_panel_buffer.h"

#define TILE_COUNT_ERROR -999

static inline void liblogic_tick(gamestate& g, inputstate& is) {
    // Spawn NPCs periodically
    //try_spawn_npc(g);
    g.update_player_tiles_explored();

    g.update_player_state();

    g.update_npcs_state();

    g.update_spells_state();

    handle_input(g, is);
    handle_npcs(g);
    update_debug_panel_buffer(g, is);
    g.currenttime = time(NULL);
    g.currenttimetm = localtime(&g.currenttime);
    strftime(g.currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g.currenttimetm);
}
