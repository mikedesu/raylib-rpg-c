#include "dungeon_floor_type.h"
#include "liblogic_init_dungeon.h"

void init_dungeon(shared_ptr<gamestate> g) {
    massert(g, "gamestate is null");

    g->dungeon = d_create();

    massert(g->dungeon, "failed to init dungeon");
    msuccess("dungeon initialized successfully");
    minfo("adding floors...");

    const int df_count = 1;
    // max size of 128x128 for now to maintain 60fps
    // dungeon floors, tiles etc will require re-write/re-design for optimization
    const int w = 24;
    const int h = 24;
    const dungeon_floor_type_t type = DUNGEON_FLOOR_TYPE_STONE;
    //dungeon_floor_type_t type = DUNGEON_FLOOR_TYPE_GRASS;
    for (int i = 0; i < df_count; i++) {
        d_add_floor(g->dungeon, type, w, h);
        // when ready to add doors, we can do so here
        // we have to create and place doors AFTER dungeon_floor creation
        //shared_ptr<dungeon_floor_t> df = d_add_floor(g->dungeon, type, w, h);
        //d_add_floor(g->dungeon, default_dungeon_floor_width, default_dungeon_floor_height);
    }
    msuccess("added %d floors to dungeon", df_count);
}
