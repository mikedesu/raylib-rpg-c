#include "entityid.h"
#include "update_player_tiles_explored.h"

void update_player_tiles_explored(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    if (g->hero_id != ENTITYID_INVALID) {
        shared_ptr<dungeon_floor_t> df = g->dungeon->floors->at(g->dungeon->current_floor);
        optional<vec3> maybe_loc = g->ct.get<location>(g->hero_id);
        if (maybe_loc.has_value()) {
            vec3 loc = maybe_loc.value();
            int my_light_radius = g->ct.get<light_radius>(g->hero_id).value_or(1);
            for (int i = -my_light_radius; i <= my_light_radius; i++) {
                for (int j = -my_light_radius; j <= my_light_radius; j++) {
                    // Calculate Manhattan distance for diamond shape
                    int dist = abs(i) + abs(j);
                    // Only reveal tiles within the light radius
                    if (dist <= my_light_radius) {
                        vec3 loc2 = {loc.x + i, loc.y + j, loc.z};
                        // Skip if out of bounds
                        if (loc2.x < 0 || loc2.x >= df->width || loc2.y < 0 || loc2.y >= df->height) continue;
                        shared_ptr<tile_t> tile = df_tile_at(df, loc2);
                        massert(tile, "failed to get tile at hero location");
                        tile->explored = true;
                        tile->visible = true;
                    }
                }
            }
        }
    }
}
