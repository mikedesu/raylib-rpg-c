#pragma once

#include "dungeon_tile_type.h"
#include "em.h"
#include "entityid.h"
#include "entitytype.h"
#include <stdbool.h>
#include <stddef.h>

#define DUNGEON_TILE_MAX_ENTITIES_DEFAULT 32
#define DUNGEON_TILE_MAX_ENTITIES_MAX 256

typedef struct {
    tiletype_t type;
    bool visible;
    bool explored;
    entityid* entities;
    size_t entity_count;
    size_t entity_max;

    bool has_pressure_plate;
    int pressure_plate_up_tx_key;
    int pressure_plate_down_tx_key;
    int pressure_plate_event;

    bool has_wall_switch;
    bool wall_switch_on;
    int wall_switch_up_tx_key;
    int wall_switch_down_tx_key;
    int wall_switch_event;

    //int cached_entities;
    int cached_live_npcs;
    bool cached_player_present;
    bool dirty_entities;
    bool dirty_visibility;

} tile_t;

tile_t* tile_create(tiletype_t type);

entityid tile_add(tile_t* const tile, entityid id);
entityid tile_remove(tile_t* const tile, entityid id);
//entityid tile_get_entity(const tile_t* const t, size_t i);
//size_t tile_entity_count(const tile_t* const t);
bool tile_resize(tile_t* t);
bool tile_has_pressure_plate(const tile_t* const t);
//bool tile_is_wall_switch_on(const tile_t* const t);
//bool tile_has_wall_switch(const tile_t* const t);
int tile_get_pressure_plate_event(const tile_t* const t);
//int tile_get_wall_switch_event(const tile_t* t);
//int tile_get_wall_switch_down_tx_key(const tile_t* t);
//int tile_get_wall_switch_up_tx_key(const tile_t* t);
void tile_init(tile_t* const t, tiletype_t type);
void tile_free(tile_t* t);
void tile_set_pressure_plate_down_tx_key(tile_t* const t, int k);
void tile_set_pressure_plate_event(tile_t* const t, int e);
void tile_set_pressure_plate(tile_t* const t, bool b);
void tile_set_pressure_plate_up_tx_key(tile_t* const t, int k);
//void tile_set_wall_switch_on(tile_t* const t, bool on);
//void tile_set_wall_switch_event(tile_t* const t, int e);
//void tile_set_wall_switch_down_tx_key(tile_t* const t, int k);
//void tile_set_wall_switch_up_tx_key(tile_t* const t, int k);
//void tile_set_wall_switch(tile_t* const t, bool b);
bool tile_has_live_npcs(tile_t* t, em_t* em);
bool tile_has_player(tile_t* t, em_t* em);

size_t tile_live_npc_count(tile_t* t, em_t* em);

static inline size_t tile_entity_count(const tile_t* const t) { return !t ? 0 : t->entity_count; }
static inline entityid tile_get_entity(const tile_t* const t, size_t i) { return !t ? -1 : i < t->entity_max ? t->entities[i] : -1; }
static inline bool tile_is_wall_switch_on(const tile_t* const t) { return !t ? false : t->wall_switch_on; }

static inline void tile_set_wall_switch_on(tile_t* const t, bool on) {
    if (!t) return;
    t->wall_switch_on = on;
}

static inline void tile_set_wall_switch(tile_t* const t, bool b) {
    if (!t) return;
    t->has_wall_switch = b;
}

static inline void tile_set_wall_switch_up_tx_key(tile_t* t, int key) {
    if (!t) return;
    t->wall_switch_up_tx_key = key;
}

static inline void tile_set_wall_switch_down_tx_key(tile_t* const t, int key) {
    if (!t) return;
    t->wall_switch_down_tx_key = key;
}

static inline void tile_set_wall_switch_event(tile_t* const t, int event) {
    if (!t) return;
    t->wall_switch_event = event;
}

static inline bool tile_has_wall_switch(const tile_t* const t) { return !t ? false : t->has_wall_switch; }
static inline int tile_get_wall_switch_up_tx_key(const tile_t* const t) { return !t ? -1 : t->wall_switch_up_tx_key; }
static inline int tile_get_wall_switch_down_tx_key(const tile_t* const t) { return !t ? -1 : t->wall_switch_down_tx_key; }
static inline int tile_get_wall_switch_event(const tile_t* const t) { return !t ? -1 : t->wall_switch_event; }
