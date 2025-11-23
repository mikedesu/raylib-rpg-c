#include "ComponentTraits.h"
#include "libdraw_dungeon_floor.h"
#include "libdraw_dungeon_tiles_2d.h"
#include "libdraw_sprite.h"

bool libdraw_draw_dungeon_floor(const shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    //dungeon_floor_t* const df = d_get_current_floor(g->d);
    shared_ptr<dungeon_floor_t> df = d_get_current_floor(g->dungeon);
    massert(df, "dungeon_floor is NULL");
    int z = g->dungeon->current_floor;
    draw_dungeon_tiles_2d(g, z, df);
    // dead
    //for (int y = 0; y < df->height; y++) {
    //    for (int x = 0; x < df->width; x++) {
    //        draw_entities_2d_at(g, df, true, (vec3){x, y, z});
    //    }
    //}
    // living
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            //draw_entities_2d_at(g, df, false, (vec3){x, y, z});
            vec3 loc = {x, y, z};
            shared_ptr<tile_t> tile = df_tile_at(df, loc);
            if (!tile) continue;
            if (tile_is_wall(tile->type)) continue;
            // Do not draw entities on invisible tiles
            if (!tile->visible) continue;
            if (tile->is_empty) continue;

            // Get hero's vision distance and location
            int vision_dist = g->ct.get<vision_distance>(g->hero_id).value_or(0);
            int light_rad = g->ct.get<light_radius>(g->hero_id).value_or(0);
            int light_dist = light_rad + 1;
            optional<vec3> maybe_hero_loc = g->ct.get<location>(g->hero_id);
            if (maybe_hero_loc.has_value()) {
                vec3 hero_loc = maybe_hero_loc.value();
                int dist_to_check = std::max(vision_dist, light_dist);
                // Calculate Manhattan distance from hero to this tile (diamond pattern)
                int distance = abs(loc.x - hero_loc.x) + abs(loc.y - hero_loc.y);
                // Only draw entities within vision distance
                if (distance <= vision_dist) {
                    if (distance <= dist_to_check) {
                        for (size_t i = 0; i < tile->entities->size(); i++) {
                            entityid id = tile_get_entity(tile, i);
                            draw_sprite_and_shadow(g, id);
                        }
                    }
                }
            }
        }
    }
    return true;
}
