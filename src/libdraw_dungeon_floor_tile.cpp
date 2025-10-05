#include "get_txkey_for_tiletype.h"
#include "libdraw_dungeon_floor_tile.h"

bool draw_dungeon_floor_tile(shared_ptr<gamestate> g, textureinfo* txinfo, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(txinfo, "txinfo is null");
    massert(x >= 0, "x is less than 0");
    massert(y >= 0, "y is less than 0");
    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, z);
    massert(df, "dungeon_floor is NULL");
    massert(x < df->width, "x is out of bounds");
    massert(y < df->height, "y is out of bounds");
    massert(df, "dungeon_floor is NULL");
    shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x, y, z});
    massert(tile, "tile is NULL");
    if (tile->type == TILE_NONE) {
        //merror("Tile at (%d, %d) is TILE_NONE", x, y);
        return true;
    }
    if (!tile->visible) {
        //merror("Tile at (%d, %d) is not visible", x, y);
        return true;
    }
    //minfo("Drawing tile at (%d, %d) with type %d", x, y, tile->type);
    // Get hero's vision distance and location
    //int vision_distance = g_get_vision_distance(g, g->hero_id);
    // its not actually the vision distance we need,
    // its the total light radius
    //int light_dist = g_get_light_radius(g, g->hero_id) +
    //                 g_get_entity_total_light_radius_bonus(g, g->hero_id);
    //vec3 hero_loc = g_get_location(g, g->hero_id);
    // Calculate Manhattan distance from hero to this tile (diamond pattern)
    //int distance = abs(x - hero_loc.x) + abs(y - hero_loc.y);
    // Get tile texture
    int txkey = get_txkey_for_tiletype(tile->type);
    if (txkey < 0) {
        return false;
    }
    Texture2D* texture = &txinfo[txkey].texture;
    if (texture->id <= 0) {
        return false;
    }
    // Calculate drawing position
    int offset_x = -12;
    int offset_y = -12;
    int px = x * DEFAULT_TILE_SIZE + offset_x;
    int py = y * DEFAULT_TILE_SIZE + offset_y;
    Rectangle src = {0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
    Rectangle dest = {(float)px, (float)py, (float)DEFAULT_TILE_SIZE_SCALED, (float)DEFAULT_TILE_SIZE_SCALED};
    // Draw tile with fade if beyond vision distance
    //Color draw_color = distance > vision_distance ? Fade(WHITE, 0.4f) : // Faded for out-of-range tiles
    Color draw_color = WHITE;
    DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, draw_color);
    //if (tile->has_pressure_plate) {
    //    int txkey2 = tile->pressure_plate_up_tx_key;
    //    if (txkey2 < 0) {
    //        return false;
    //    }
    //    Texture2D* texture = &txinfo[txkey2].texture;
    //    if (texture->id <= 0) {
    //        return false;
    //    }
    //    DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
    //}
    //if (tile->has_wall_switch) {
    //    int txkey = tile->wall_switch_on ? tile->wall_switch_down_tx_key : tile->wall_switch_up_tx_key;
    //    if (txkey < 0) {
    //        return false;
    //    }
    //    Texture2D* texture = &txinfo[txkey].texture;
    //    if (texture->id <= 0) {
    //        return false;
    //    }
    //    DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
    //}
    return true;
}
