#pragma once

#include "gamestate.h"

static inline void init_dungeon(gamestate& g, int df_count) {
    massert(df_count > 0, "df_count is <= 0");
    d_create(g.dungeon);
    msuccess("dungeon initialized successfully");
    minfo("adding floors...");
    // max size of 128x128 for now to maintain 60fps
    // dungeon floors, tiles etc will require re-write/re-design for optimization
    const int w = 32;
    const int h = 32;
    const biome_t type = BIOME_STONE;
    //dungeon_floor_type_t type = DUNGEON_FLOOR_TYPE_GRASS;
    for (int i = 0; i < df_count; i++) {
        d_add_floor(g.dungeon, type, w, h);
    }
    msuccess("added %d floors to dungeon", df_count);
}
