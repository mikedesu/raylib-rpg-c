#pragma once

#include "gamestate.h"

static inline void init_dungeon(shared_ptr<gamestate> g) {
    massert(g, "gamestate is null");

    g->dungeon = d_create();

    massert(g->dungeon, "failed to init dungeon");
    msuccess("dungeon initialized successfully");
    minfo("adding floors...");

    const int df_count = 20;
    // max size of 128x128 for now to maintain 60fps
    // dungeon floors, tiles etc will require re-write/re-design for optimization
    const int w = 32;
    const int h = 32;
    const biome_t type = BIOME_STONE;
    //dungeon_floor_type_t type = DUNGEON_FLOOR_TYPE_GRASS;

    for (int i = 0; i < df_count; i++)
        d_add_floor(g->dungeon, type, w, h);

    msuccess("added %d floors to dungeon", df_count);
}
