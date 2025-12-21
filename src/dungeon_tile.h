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

#define DUNGEON_TILE_MAX_ENTITIES_DEFAULT 8
#define DUNGEON_TILE_MAX_ENTITIES_MAX 256


using std::find;
using std::make_shared;
using std::shared_ptr;
using std::vector;


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
    entityid cached_npc;
    bool can_have_door;
    shared_ptr<vector<entityid>> entities;
} tile_t;


static inline size_t tile_entity_count(tile_t& t) {
    return t.entities->size();
}


static inline entityid tile_get_entity(tile_t& t, size_t i) {
    return i >= 0 && i < t.entities->size() ? t.entities->at(i) : ENTITYID_INVALID;
}


static inline bool tile_is_wall(tile_t& t) {
    return tiletype_is_wall(t.type);
}


static inline void tile_init(tile_t& t, tiletype_t type) {
    t.type = type;
    t.visible = t.explored = t.cached_player_present = t.can_have_door = false;
    t.dirty_entities = t.dirty_visibility = t.is_empty = true;
    t.cached_live_npcs = 0;
    t.cached_npc = ENTITYID_INVALID;
    t.entities = make_shared<vector<entityid>>();
}


static inline entityid tile_add(tile_t& t, entityid id) {
    massert(t.entities, "tile or tile entities is NULL");
    // Check if the entity already exists
    if (find(t.entities->begin(), t.entities->end(), id) != t.entities->end()) {
        merror("tile_add: entity already exists on tile");
        return ENTITYID_INVALID;
    }
    t.entities->push_back(id);
    t.dirty_entities = true;
    t.is_empty = false;
    return id;
}


static inline entityid tile_remove(tile_t& tile, entityid id) {
    massert(tile.entities, "tile or tile entities is NULL");
    massert(id != ENTITYID_INVALID, "tile_remove: id is invalid");
    auto it = find(tile.entities->begin(), tile.entities->end(), id);
    if (it == tile.entities->end()) {
        merror("tile_remove: entity not found on tile");
        return ENTITYID_INVALID;
    }
    tile.entities->erase(it);
    tile.dirty_entities = true;
    tile.is_empty = tile.entities->size() == 0;
    return id;
}


static inline void tile_create(tile_t& t, tiletype_t type) {
    massert(type >= TILE_NONE && type < TILE_COUNT, "tile_create: type is out-of-bounds");
    tile_init(t, type);
}


static inline void tile_free(tile_t& t) {
    t.entities->clear();
}
