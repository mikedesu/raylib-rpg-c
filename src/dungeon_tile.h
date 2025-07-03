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
    int cached_live_npcs;

    std::shared_ptr<std::vector<entityid>> entities;

} tile_t;

std::shared_ptr<tile_t> tile_create(tiletype_t type);

//entityid tile_add(std::shared_ptr<gamestate> g, std::shared_ptr<tile_t> tile, entityid id);
entityid tile_add(std::shared_ptr<tile_t> tile, entityid id);
//entityid tile_remove(std::shared_ptr<gamestate> g, std::shared_ptr<tile_t> tile, entityid id);
entityid tile_remove(std::shared_ptr<tile_t> tile, entityid id);

size_t tile_live_npc_count_at(std::shared_ptr<gamestate> g, int x, int y, int z);

void tile_init(std::shared_ptr<tile_t> t, tiletype_t type);
void tile_free(std::shared_ptr<tile_t> t);
void recompute_entity_cache(std::shared_ptr<gamestate> g, std::shared_ptr<tile_t> t);
void recompute_entity_cache_at(std::shared_ptr<gamestate> g, int x, int y, int z);

static inline size_t tile_entity_count(const std::shared_ptr<tile_t> t) {
    massert(t, "tile is NULL");
    return t->entities->size();
}

static inline entityid tile_get_entity(const std::shared_ptr<tile_t> t, size_t i) {
    massert(t, "tile is NULL");
    entityid retval = ENTITYID_INVALID;
    if (i < t->entities->size()) {
        retval = t->entities->at(i);
    }
    return retval;
}

static inline bool tile_has_live_npcs(std::shared_ptr<gamestate> g, std::shared_ptr<tile_t> t) {
    if (!t) {
        return false;
    }
    recompute_entity_cache(g, t);
    return t->cached_live_npcs > 0;
}

static inline size_t tile_live_npc_count(std::shared_ptr<gamestate> g, std::shared_ptr<tile_t> t) {
    massert(t, "tile is NULL");
    recompute_entity_cache(g, t);
    return t->cached_live_npcs;
}

static inline bool tile_has_player(std::shared_ptr<gamestate> g, std::shared_ptr<tile_t> t) {
    if (!t) {
        return false;
    }
    recompute_entity_cache(g, t);
    return t->cached_player_present;
}

static inline bool tile_is_visible(const std::shared_ptr<tile_t> t) {
    massert(t, "tile is NULL");
    return t->visible;
}

static inline void tile_set_visible(std::shared_ptr<tile_t> t, bool visible) {
    massert(t, "tile is NULL");
    t->visible = visible;
}

static inline bool tile_is_explored(const std::shared_ptr<tile_t> t) {
    massert(t, "tile is NULL");
    return t->explored;
}

static inline void tile_set_explored(std::shared_ptr<tile_t> t, bool explored) {
    massert(t, "tile is NULL");
    t->explored = explored;
}
