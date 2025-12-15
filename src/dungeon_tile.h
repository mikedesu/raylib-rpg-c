#pragma once

#include "ComponentTraits.h"
#include "dungeon_tile_type.h"
#include "entityid.h"
#include "entitytype.h"
#include "massert.h"
#include "mprint.h"
#include "tile_id.h"
#include <algorithm>
#include <memory>
#include <vector>
//#include "gamestate.h"

#define DUNGEON_TILE_MAX_ENTITIES_DEFAULT 8
#define DUNGEON_TILE_MAX_ENTITIES_MAX 256


using std::find;
using std::make_shared;
using std::shared_ptr;
using std::vector;


// forward declaration for gamestate
//typedef struct gamestate gamestate;


typedef struct {
    tile_id id;
    tiletype_t type;
    bool visible;
    bool explored;
    bool cached_player_present;
    bool dirty_entities;
    bool dirty_visibility;
    bool is_empty;
    int cached_live_npcs;

    bool can_have_door;

    shared_ptr<vector<entityid>> entities;

} tile_t;


//shared_ptr<tile_t> tile_create(tiletype_t type);
//entityid tile_add(shared_ptr<tile_t> tile, entityid id);
//entityid tile_remove(shared_ptr<tile_t> tile, entityid id);
//size_t tile_live_npc_count_at(shared_ptr<gamestate> g, int x, int y, int z);
//void tile_init(shared_ptr<tile_t> t, tiletype_t type);
//void tile_free(shared_ptr<tile_t> t);
//void recompute_entity_cache(shared_ptr<gamestate> g, shared_ptr<tile_t> t);
//void recompute_entity_cache_at(shared_ptr<gamestate> g, int x, int y, int z);


static inline size_t tile_entity_count(const shared_ptr<tile_t> t) {
    return !t ? 0 : t->entities->size();
}


static inline entityid tile_get_entity(const shared_ptr<tile_t> t, size_t i) {
    return !t ? ENTITYID_INVALID : i < t->entities->size() ? t->entities->at(i) : ENTITYID_INVALID;
}


/*
static inline void recompute_entity_cache(shared_ptr<gamestate> g, shared_ptr<tile_t> t) {
    massert(g && t, "gamestate or tile is NULL");
    // Only recompute if cache is dirty
    if (!t->dirty_entities)
        return;
    // Reset counters
    t->cached_live_npcs = 0;
    t->cached_player_present = false;
    // Iterate through all entities on the tile
    for (size_t i = 0; i < t->entities->size(); i++) {
        entityid id = t->entities->at(i);
        // Skip dead entities
        if (g->ct.get<dead>(id).value_or(true)) {
            continue;
        }
        // Check entity type
        //entitytype_t type = g_get_type(g, id);
        entitytype_t type = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (type == ENTITY_NPC) {
            t->cached_live_npcs++;
        } else if (type == ENTITY_PLAYER) {
            t->cached_player_present = true;
        }
    }
    // Cache is now clean
    t->dirty_entities = false;
}
*/


/*
static inline void recompute_entity_cache_at(shared_ptr<gamestate> g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(x >= 0 && y >= 0 && z >= 0, "x, y, or z is out of bounds: %d, %d, %d", x, y, z);
    massert((size_t)z < g->dungeon->floors->size(), "z is out of bounds");
    shared_ptr<dungeon_floor_t> df = g->dungeon->floors->at(z);
    massert(df, "failed to get dungeon floor");
    //shared_ptr<tile_t> t = df_tile_at(df, (vec3){x, y, z});
    auto t = df_tile_at(df, (vec3){x, y, z});
    massert(t, "tile not found");
    recompute_entity_cache(g, t);
}
*/


/*
static inline bool tile_has_live_npcs(shared_ptr<gamestate> g, shared_ptr<tile_t> t) {
    if (!t) {
        return false;
    }
    recompute_entity_cache(g, t);
    return t->cached_live_npcs > 0;
}
*/


/*
static inline size_t tile_live_npc_count(shared_ptr<gamestate> g, shared_ptr<tile_t> t) {
    recompute_entity_cache(g, t);
    return t->cached_live_npcs;
}
*/


/*
static inline bool tile_has_player(shared_ptr<gamestate> g, shared_ptr<tile_t> t) {
    if (!t) {
        return false;
    }
    recompute_entity_cache(g, t);
    return t->cached_player_present;
}
*/


static inline bool tile_is_wall(shared_ptr<tile_t> t) {
    //massert(g, "gamestate is none");

    if (!t)
        return false;

    return tiletype_is_wall(t->type);
}


static inline void tile_init(shared_ptr<tile_t> t, tiletype_t type) {
    //massert(t, "tile is NULL");
    if (!t)
        return;

    t->type = type;
    t->visible = t->explored = t->cached_player_present = false;
    t->dirty_entities = t->dirty_visibility = true;
    t->cached_live_npcs = 0;
    t->entities = make_shared<vector<entityid>>();
    t->is_empty = true;
    t->can_have_door = false;
}


static inline entityid tile_add(shared_ptr<tile_t> t, entityid id) {
    massert(t && t->entities, "tile or tile entities is NULL");
    // Check if the entity already exists
    if (find(t->entities->begin(), t->entities->end(), id) != t->entities->end()) {
        merror("tile_add: entity already exists on tile");
        return ENTITYID_INVALID;
    }
    t->entities->push_back(id);
    t->dirty_entities = true;
    t->is_empty = false;
    return id;
}


static inline entityid tile_remove(shared_ptr<tile_t> tile, entityid id) {
    massert(tile && tile->entities, "tile or tile entities is NULL");
    massert(id != ENTITYID_INVALID, "tile_remove: id is invalid");
    auto it = find(tile->entities->begin(), tile->entities->end(), id);
    if (it != tile->entities->end()) {
        tile->entities->erase(it);
        tile->dirty_entities = true;
        tile->is_empty = tile->entities->size() == 0;
        return id;
    }
    merror("tile_remove: entity not found on tile");
    return ENTITYID_INVALID;
}


static inline shared_ptr<tile_t> tile_create(tiletype_t type) {
    massert(type >= TILE_NONE && type < TILE_COUNT, "tile_create: type is out-of-bounds");
    shared_ptr<tile_t> t = make_shared<tile_t>();
    if (!t)
        return nullptr;
    tile_init(t, type);
    return t;
}


static inline void tile_free(shared_ptr<tile_t> t) {
    if (t)
        t->entities->clear();
}

/*
static inline size_t tile_live_npc_count_at(shared_ptr<gamestate> g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(x >= 0 && y >= 0 && z >= 0, "x, y, or z is out of bounds: %d, %d, %d", x, y, z);
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
*/
