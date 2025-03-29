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

    int cached_live_npcs;
    bool cached_player_present;
    bool dirty_entities;
    bool dirty_visibility;

} dungeon_tile_t;

dungeon_tile_t* tile_create(tiletype_t type);

entityid tile_add(dungeon_tile_t* const tile, entityid id);
entityid tile_remove(dungeon_tile_t* const tile, entityid id);
entityid tile_get_entity(const dungeon_tile_t* const t, size_t i);

size_t tile_entity_count(const dungeon_tile_t* const t);

bool tile_resize(dungeon_tile_t* t);
bool tile_has_pressure_plate(const dungeon_tile_t* const t);
bool tile_is_wall_switch_on(const dungeon_tile_t* const t);
bool tile_has_wall_switch(const dungeon_tile_t* const t);

int tile_get_pressure_plate_event(const dungeon_tile_t* const t);
int tile_get_wall_switch_event(const dungeon_tile_t* t);
int tile_get_wall_switch_down_tx_key(const dungeon_tile_t* t);
int tile_get_wall_switch_up_tx_key(const dungeon_tile_t* t);

void tile_init(dungeon_tile_t* const t, tiletype_t type);
void tile_free(dungeon_tile_t* t);
void tile_set_pressure_plate_down_tx_key(dungeon_tile_t* const t, int k);
void tile_set_pressure_plate_event(dungeon_tile_t* const t, int e);
void tile_set_pressure_plate(dungeon_tile_t* const t, bool b);
void tile_set_pressure_plate_up_tx_key(dungeon_tile_t* const t, int k);
void tile_set_wall_switch_on(dungeon_tile_t* const t, bool on);
void tile_set_wall_switch_event(dungeon_tile_t* const t, int e);
void tile_set_wall_switch_down_tx_key(dungeon_tile_t* const t, int k);
void tile_set_wall_switch_up_tx_key(dungeon_tile_t* const t, int k);
void tile_set_wall_switch(dungeon_tile_t* const t, bool b);

bool tile_has_live_npcs(dungeon_tile_t* t, em_t* em);

bool tile_has_player(dungeon_tile_t* t, em_t* em);
