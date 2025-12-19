#pragma once

#include "create_door.h"
#include "gamestate.h"

static inline void place_doors(shared_ptr<gamestate> g) {
    massert(g, "g is null");
    for (int z = 0; z < (int)g->dungeon.floors.size(); z++) {
        auto df = d_get_floor(g->dungeon, z);
        for (int x = 0; x < df.width; x++) {
            for (int y = 0; y < df.height; y++) {
                const vec3 loc = {x, y, z};
                auto tile = df_tile_at(df, loc);
                if (!tile || !tile->can_have_door)
                    continue;
                create_door_at_with(g, loc, [](shared_ptr<gamestate> g, entityid id) {});
            }
        }
    }
}
