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
            shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x, y, z});
            if (!tile) {
                continue;
            }
            if (tile_is_wall(tile->type)) {
                continue;
            }
            if (!tile->visible) {
                continue; // Do not draw entities on invisible tiles
            }
            if (tile->is_empty) {
                continue;
            }
            // Get hero's vision distance and location
            //int vision_distance = g_get_vision_distance(g, g->hero_id);
            //int light_dist = g_get_light_radius(g, g->hero_id) + g_get_entity_total_light_radius_bonus(g, g->hero_id);
            //int light_dist = 3;
            //vec3 hero_loc = g_get_location(g, g->hero_id);
            //int dist_to_check = MAX(vision_distance, light_dist);
            // Calculate Manhattan distance from hero to this tile (diamond pattern)
            //int distance = abs(loc.x - hero_loc.x) + abs(loc.y - hero_loc.y);
            // Only draw entities within vision distance
            //if (distance <= vision_distance) {
            //if (distance <= dist_to_check) {
            for (size_t i = 0; i < tile->entities->size(); i++) {
                entityid id = tile_get_entity(tile, i);
                draw_sprite_and_shadow(g, id);
            }
        }
    }
    return true;
}
