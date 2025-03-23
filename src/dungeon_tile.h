#pragma once

#include "dungeon_tile_type.h"
#include "entityid.h"
#include <stdbool.h>
#include <stddef.h>

#define DUNGEON_TILE_MAX_ENTITIES_DEFAULT 32
#define DUNGEON_TILE_MAX_ENTITIES_MAX 256

typedef struct {
    dungeon_tile_type_t type;
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

} dungeon_tile_t;

dungeon_tile_t* dungeon_tile_create(const dungeon_tile_type_t type);

void dungeon_tile_free(dungeon_tile_t* tile);
void dungeon_tile_init(dungeon_tile_t* tile, const dungeon_tile_type_t type);

bool dungeon_tile_resize(dungeon_tile_t* t);
entityid dungeon_tile_add(dungeon_tile_t* tile, const entityid id);
const entityid dungeon_tile_remove(dungeon_tile_t* tile, const entityid id);
const entityid dungeon_tile_get_entity(const dungeon_tile_t* tile, const size_t index);

const size_t dungeon_tile_entity_count(const dungeon_tile_t* tile);

void dungeon_tile_set_pressure_plate(dungeon_tile_t* const t, bool b);
void dungeon_tile_set_pressure_plate_up_tx_key(dungeon_tile_t* const t, int k);
void dungeon_tile_set_pressure_plate_down_tx_key(dungeon_tile_t* const t, int k);
void dungeon_tile_set_pressure_plate_event(dungeon_tile_t* const t, int e);

const bool dungeon_tile_has_pressure_plate(const dungeon_tile_t* tile);
const bool dungeon_tile_is_wall_switch_on(const dungeon_tile_t* tile);
const bool dungeon_tile_has_wall_switch(const dungeon_tile_t* tile);

const int dungeon_tile_get_pressure_plate_event(const dungeon_tile_t* tile);
const int dungeon_tile_get_wall_switch_event(const dungeon_tile_t* tile);
const int dungeon_tile_get_wall_switch_down_tx_key(const dungeon_tile_t* tile);
const int dungeon_tile_get_wall_switch_up_tx_key(const dungeon_tile_t* tile);

void dungeon_tile_set_wall_switch_on(dungeon_tile_t* tile, const bool on);
void dungeon_tile_set_wall_switch_event(dungeon_tile_t* tile, const int event);
void dungeon_tile_set_wall_switch_down_tx_key(dungeon_tile_t* tile, const int key);
void dungeon_tile_set_wall_switch_up_tx_key(dungeon_tile_t* tile, const int key);
void dungeon_tile_set_wall_switch(dungeon_tile_t* tile, const bool has_wall_switch);
