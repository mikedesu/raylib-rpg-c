#pragma once

#include "gamestate.h"

using std::max;
using std::min;

static inline void update_tile(shared_ptr<tile_t> tile) {
    if (!tile)
        return;
    tile->explored = tile->visible = true;
}

static inline void update_player_tiles_explored(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    if (g->hero_id == ENTITYID_INVALID)
        return;
    auto df = g->dungeon->floors->at(g->dungeon->current_floor);
    auto maybe_loc = g->ct.get<location>(g->hero_id);
    if (!maybe_loc.has_value())
        return;
    const vec3 loc = maybe_loc.value();
    const int light_radius0 = g->ct.get<light_radius>(g->hero_id).value_or(1);
    // Precompute bounds for the loops
    const int min_x = max(0, loc.x - light_radius0), max_x = min(df->width - 1, loc.x + light_radius0);
    const int min_y = max(0, loc.y - light_radius0), max_y = min(df->height - 1, loc.y + light_radius0);
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            // Calculate Manhattan distance for diamond shape
            if (abs(x - loc.x) + abs(y - loc.y) > light_radius0)
                continue;
            update_tile(df_tile_at(df, (vec3){x, y, loc.z}));
        }
    }
}
