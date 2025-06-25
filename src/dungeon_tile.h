#pragma once

#include "dungeon_tile_type.h"
#include "entityid.h"
#include "entitytype.h"
#include "massert.h"
#include <vector>

#define DUNGEON_TILE_MAX_ENTITIES_DEFAULT 8
#define DUNGEON_TILE_MAX_ENTITIES_MAX 256

// forward declaration for gamestate
typedef struct gamestate gamestate;

typedef struct {
    tiletype_t type;
    bool visible;
    bool explored;

    std::vector<entityid>* entities;

    //bool has_pressure_plate;
    //int pressure_plate_up_tx_key;
    //int pressure_plate_down_tx_key;
    //int pressure_plate_event;
    //bool has_wall_switch;
    //bool wall_switch_on;
    //int wall_switch_up_tx_key;
    //int wall_switch_down_tx_key;
    //int wall_switch_event;

    //int cached_entities;
    int cached_live_npcs;
    bool cached_player_present;
    bool dirty_entities;
    bool dirty_visibility;

} tile_t;

tile_t* tile_create(tiletype_t type);

entityid tile_add(tile_t* const tile, entityid id);
entityid tile_remove(tile_t* const tile, entityid id);

bool tile_resize(tile_t* t);

void tile_init(tile_t* const t, tiletype_t type);
void tile_free(tile_t* t);
void recompute_entity_cache(gamestate* g, tile_t* t);
void recompute_entity_cache_at(gamestate* g, int x, int y, int z);

size_t tile_live_npc_count_at(gamestate* g, int x, int y, int z);

// Serialization functions
//size_t tile_serialized_size(const tile_t* t);
//size_t tile_serialize(const tile_t* t, char* buffer, size_t buffer_size);
//bool tile_deserialize(tile_t* t, const char* buffer, size_t buffer_size);

// Memory size calculation
size_t tile_memory_size(const tile_t* t);

//static inline void tile_set_pressure_plate(tile_t* const t, bool b) {
//    massert(t, "tile is NULL");
//    t->has_pressure_plate = b;
//}

//static inline void tile_set_pressure_plate_up_tx_key(tile_t* const t, int k) {
//    massert(t, "tile is NULL");
//    t->pressure_plate_up_tx_key = k;
//}

//static inline void tile_set_pressure_plate_down_tx_key(tile_t* const t, int k) {
//    massert(t, "tile is NULL");
//    t->pressure_plate_down_tx_key = k;
//}

//static inline void tile_set_pressure_plate_event(tile_t* const t, int e) {
//    massert(t, "tile is NULL");
//    t->pressure_plate_event = e;
//}

static inline size_t tile_entity_count(const tile_t* const t) {
    massert(t, "tile is NULL");
    return t->entities->size();
}

static inline entityid tile_get_entity(const tile_t* const t, size_t i) {
    massert(t, "tile is NULL");
    return i < t->entities->size() ? t->entities->at(i) : ENTITYID_INVALID;
}

//static inline bool tile_is_wall_switch_on(const tile_t* const t) {
//    massert(t, "tile is NULL");
//    return t->wall_switch_on;
//}

//static inline void tile_set_wall_switch_on(tile_t* const t, bool on) {
//    massert(t, "tile is NULL");
//    t->wall_switch_on = on;
//}

//static inline void tile_set_wall_switch(tile_t* const t, bool b) {
//    massert(t, "tile is NULL");
//    t->has_wall_switch = b;
//}

//static inline void tile_set_wall_switch_up_tx_key(tile_t* t, int key) {
//    massert(t, "tile is NULL");
//    t->wall_switch_up_tx_key = key;
//}

//static inline void tile_set_wall_switch_down_tx_key(tile_t* const t, int key) {
//    massert(t, "tile is NULL");
//    t->wall_switch_down_tx_key = key;
//}

//static inline void tile_set_wall_switch_event(tile_t* const t, int event) {
//    massert(t, "tile is NULL");
//    t->wall_switch_event = event;
//}

//static inline bool tile_has_wall_switch(const tile_t* const t) {
//    massert(t, "tile is NULL");
//    return t->has_wall_switch;
//}

//static inline int tile_get_wall_switch_up_tx_key(const tile_t* const t) {
//    massert(t, "tile is NULL");
//    return t->wall_switch_up_tx_key;
//}

//static inline int tile_get_wall_switch_down_tx_key(const tile_t* const t) {
//    massert(t, "tile is NULL");
//    return t->wall_switch_down_tx_key;
//}

//static inline int tile_get_wall_switch_event(const tile_t* const t) {
//    massert(t, "tile is NULL");
//    return t->wall_switch_event;
//}

//static inline bool tile_has_pressure_plate(const tile_t* const t) {
//    massert(t, "tile is NULL");
//    return t->has_pressure_plate;
//}

//static inline int tile_get_pressure_plate_event(const tile_t* const t) {
//    massert(t, "tile is NULL");
//    return t->pressure_plate_event;
//}

static inline bool tile_has_live_npcs(gamestate* g, tile_t* t) {
    if (!t) return false;
    recompute_entity_cache(g, t);
    return t->cached_live_npcs > 0;
}

static inline size_t tile_live_npc_count(gamestate* g, tile_t* t) {
    massert(t, "tile is NULL");
    recompute_entity_cache(g, t);
    return t->cached_live_npcs;
}

static inline bool tile_has_player(gamestate* g, tile_t* t) {
    if (!t) return false;
    recompute_entity_cache(g, t);
    return t->cached_player_present;
}

static inline bool tile_is_visible(const tile_t* const t) {
    massert(t, "tile is NULL");
    return t->visible;
}

static inline void tile_set_visible(tile_t* const t, bool visible) {
    massert(t, "tile is NULL");
    t->visible = visible;
}

static inline bool tile_is_explored(const tile_t* const t) {
    massert(t, "tile is NULL");
    return t->explored;
}

static inline void tile_set_explored(tile_t* const t, bool explored) {
    massert(t, "tile is NULL");
    t->explored = explored;
}
