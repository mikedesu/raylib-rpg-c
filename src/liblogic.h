#pragma once

#include "gamestate.h"
#include "handle_input.h"
#include "handle_npcs.h"
#include "inputstate.h"
#include "update_debug_panel_buffer.h"
#include "update_player_state.h"
#include "update_player_tiles_explored.h"
#include <memory>

#define TILE_COUNT_ERROR -999

using std::shared_ptr;

static inline void liblogic_close(gamestate& g) {
    d_destroy(g.dungeon);
}

static inline void update_npcs_state(gamestate& g) {
    for (entityid id = 0; id < g.next_entityid; id++) {
        if (id == g.hero_id || g.ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_NPC)
            continue;
        unsigned char a = g.ct.get<txalpha>(id).value_or(255);
        if (a < 255)
            a++;
        g.ct.set<txalpha>(id, a);
        g.ct.set<damaged>(id, false);
    }
}


static inline void update_spells_state(gamestate& g) {
    for (entityid id = 0; id < g.next_entityid; id++) {
        if (id == g.hero_id || g.ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_SPELL)
            continue;
        unsigned char a = g.ct.get<txalpha>(id).value_or(255);
        if (a < 255)
            a++;
        g.ct.set<txalpha>(id, a);
        const bool is_complete = g.ct.get<spell_complete>(id).value_or(false);
        const bool is_destroyed = g.ct.get<destroyed>(id).value_or(false);
        if (is_complete && is_destroyed) {
            // remove it from the tile
            auto df = d_get_current_floor(g.dungeon);
            auto loc = g.ct.get<location>(id).value_or((vec3){-1, -1, -1});
            df_remove_at(df, id, loc.x, loc.y);
        }
    }
}


static inline void liblogic_tick(gamestate& g, inputstate& is) {
    // Spawn NPCs periodically
    //try_spawn_npc(g);
    update_player_tiles_explored(g);
    update_player_state(g);
    update_npcs_state(g);
    update_spells_state(g);
    handle_input(g, is);
    handle_npcs(g);
    update_debug_panel_buffer(g, is);
    g.currenttime = time(NULL);
    g.currenttimetm = localtime(&g.currenttime);
    strftime(g.currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g.currenttimetm);
}
