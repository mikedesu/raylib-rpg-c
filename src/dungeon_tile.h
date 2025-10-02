#pragma once

#include "dungeon_tile_type.h"
#include "entityid.h"
#include "entitytype.h"
#include "massert.h"
#include "tile_id.h"
#include <memory>
#include <vector>

#define DUNGEON_TILE_MAX_ENTITIES_DEFAULT 8
#define DUNGEON_TILE_MAX_ENTITIES_MAX 256


using std::shared_ptr;
using std::vector;

// forward declaration for gamestate
typedef struct gamestate gamestate;

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

    shared_ptr<vector<entityid>> entities;

} tile_t;

shared_ptr<tile_t> tile_create(tiletype_t type);

entityid tile_add(shared_ptr<tile_t> tile, entityid id);
entityid tile_remove(shared_ptr<tile_t> tile, entityid id);

size_t tile_live_npc_count_at(shared_ptr<gamestate> g, int x, int y, int z);

void tile_init(shared_ptr<tile_t> t, tiletype_t type);
void tile_free(shared_ptr<tile_t> t);
void recompute_entity_cache(shared_ptr<gamestate> g, shared_ptr<tile_t> t);
void recompute_entity_cache_at(shared_ptr<gamestate> g, int x, int y, int z);


static inline size_t tile_entity_count(const shared_ptr<tile_t> t) { return t->entities->size(); }


static inline entityid tile_get_entity(const shared_ptr<tile_t> t, size_t i) {
    return !t ? ENTITYID_INVALID : i < t->entities->size() ? t->entities->at(i) : ENTITYID_INVALID;
}


static inline bool tile_has_live_npcs(shared_ptr<gamestate> g, shared_ptr<tile_t> t) {
    if (!t) {
        return false;
    }
    recompute_entity_cache(g, t);
    return t->cached_live_npcs > 0;
}


static inline size_t tile_live_npc_count(shared_ptr<gamestate> g, shared_ptr<tile_t> t) {
    recompute_entity_cache(g, t);
    return t->cached_live_npcs;
}


static inline bool tile_has_player(shared_ptr<gamestate> g, shared_ptr<tile_t> t) {
    if (!t) {
        return false;
    }
    recompute_entity_cache(g, t);
    return t->cached_player_present;
}


//static inline bool tile_is_visible(const shared_ptr<tile_t> t) { return t->visible; }
//static inline void tile_set_explored(shared_ptr<tile_t> t, bool explored) { t->explored = explored; }
//static inline void tile_set_visible(shared_ptr<tile_t> t, bool visible) { t->visible = visible; }
//static inline bool tile_is_explored(const shared_ptr<tile_t> t) { return t->explored; }
