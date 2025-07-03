#include "dungeon_tile.h"
#include "entityid.h"
#include "gamestate.h"
#include "mprint.h"
#include <algorithm>
#include <vector>

using std::find;
using std::make_shared;
using std::shared_ptr;
using std::vector;

void tile_init(shared_ptr<tile_t> t, tiletype_t type) {
    minfo("tile_init: Initializing tile of type %d", type);
    massert(t, "tile is NULL");
    t->type = type;
    t->visible = false;
    t->explored = false;
    t->dirty_entities = true;
    t->dirty_visibility = true;
    t->cached_live_npcs = 0;
    t->cached_player_present = false;
    t->entities = make_shared<vector<entityid>>();
    //t->has_pressure_plate = false;
    //t->has_wall_switch = false;
    //t->wall_switch_on = false;
    //t->entities = new vector<entityid>();
    //t->entities->reserve(DUNGEON_TILE_MAX_ENTITIES_DEFAULT);
    //t->pressure_plate_up_tx_key = -1;
    //t->pressure_plate_down_tx_key = -1;
    //t->pressure_plate_event = -1;
    //t->wall_switch_up_tx_key = -1;
    //t->wall_switch_down_tx_key = -1;
    //t->wall_switch_event = -1;
}


entityid tile_add(std::shared_ptr<tile_t> t, entityid id) {
    massert(t, "tile is NULL");
    massert(t->entities, "tile entities is NULL");
    // Check if the entity already exists
    if (find(t->entities->begin(), t->entities->end(), id) != t->entities->end()) {
        merror("tile_add: entity already exists on tile");
        return ENTITYID_INVALID;
    }
    t->entities->push_back(id);
    t->dirty_entities = true;
    return id;
}


entityid tile_remove(std::shared_ptr<tile_t> tile, entityid id) {
    massert(tile, "tile is NULL");
    massert(tile->entities, "tile entities is NULL");
    massert(id != ENTITYID_INVALID, "tile_remove: id is invalid");
    auto it = find(tile->entities->begin(), tile->entities->end(), id);
    if (it != tile->entities->end()) {
        tile->entities->erase(it);
        tile->dirty_entities = true;
        return id;
    }
    merror("dungeon_tile_remove: entity not found");
    return ENTITYID_INVALID;
}


shared_ptr<tile_t> tile_create(tiletype_t type) {
    massert(type >= TILE_NONE, "tile_create: type is invalid");
    massert(type < TILE_COUNT, "tile_create: type is out of bounds");
    shared_ptr<tile_t> t = make_shared<tile_t>();
    if (!t) {
        merror("tile_create: failed to allocate memory for tile");
        return nullptr;
    }
    tile_init(t, type);
    return t;
}


void tile_free(shared_ptr<tile_t> t) {
    if (t) {
        t->entities->clear();
    }
}


void recompute_entity_cache(shared_ptr<gamestate> g, shared_ptr<tile_t> t) {
    massert(g, "gamestate is NULL");
    massert(t, "tile is NULL");
    // Only recompute if cache is dirty
    if (!t->dirty_entities) {
        return;
    }
    // Reset counters
    t->cached_live_npcs = 0;
    t->cached_player_present = false;
    // Iterate through all entities on the tile
    for (int i = 0; i < t->entities->size(); i++) {
        entityid id = t->entities->at(i);
        // Skip dead entities
        if (g_is_dead(g, id)) {
            merror("entity %d is dead", id);
            continue;
        }
        // Check entity type
        entitytype_t type = g_get_type(g, id);
        if (type == ENTITY_NPC) {
            t->cached_live_npcs++;
        } else if (type == ENTITY_PLAYER) {
            t->cached_player_present = true;
        }
    }
    // Cache is now clean
    t->dirty_entities = false;
}


void recompute_entity_cache_at(shared_ptr<gamestate> g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(x >= 0, "x is out of bounds");
    massert(y >= 0, "y is out of bounds");
    massert(z >= 0, "z is out of bounds");
    massert(z < g->dungeon->floors->size(), "z is out of bounds");
    shared_ptr<dungeon_floor_t> df = g->dungeon->floors->at(z);
    massert(df, "failed to get dungeon floor");
    shared_ptr<tile_t> t = df_tile_at(df, (vec3){x, y, z});
    massert(t, "tile not found");
    recompute_entity_cache(g, t);
}


size_t tile_live_npc_count_at(shared_ptr<gamestate> g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(x >= 0, "x is out of bounds");
    massert(y >= 0, "y is out of bounds");
    massert(z >= 0, "z is out of bounds");
    recompute_entity_cache_at(g, x, y, z);
    shared_ptr<dungeon_t> d = g->dungeon;
    massert(d, "failed to get dungeon");
    shared_ptr<dungeon_floor_t> df = d_get_floor(d, z);
    massert(df, "failed to get dungeon floor");
    massert(x < df->width, "x is out of bounds");
    massert(y < df->height, "y is out of bounds");
    shared_ptr<tile_t> t = df_tile_at(df, (vec3){x, y, z});
    massert(t, "failed to get tile");
    return tile_live_npc_count(g, t);
}
