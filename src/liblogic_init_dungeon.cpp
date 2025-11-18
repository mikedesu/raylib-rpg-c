#include "liblogic_init_dungeon.h"

void init_dungeon(shared_ptr<gamestate> g) {
    massert(g, "gamestate is null");
    g->dungeon = d_create();
    massert(g->dungeon, "failed to init dungeon");
    msuccess("dungeon initialized successfully");
    minfo("adding floors...");
    int df_count = 1;
    // max size of 128x128 for now to maintain 60fps
    // dungeon floors, tiles etc will require re-write/re-design for optimization
    int w = 4;
    int h = 4;
    for (int i = 0; i < df_count; i++) {
        d_add_floor(g->dungeon, w, h);
        //d_add_floor(g->dungeon, default_dungeon_floor_width, default_dungeon_floor_height);
    }
    msuccess("added %d floors to dungeon", df_count);
}
