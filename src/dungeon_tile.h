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

    //entityid entities[DUNGEON_TILE_MAX_ENTITIES];
    entityid* entities;

    size_t entity_count;
    size_t entity_max;

} dungeon_tile_t;


dungeon_tile_t* dungeon_tile_create(const dungeon_tile_type_t type);
//void dungeon_tile_free(dungeon_tile_t* tile);

void dungeon_tile_init(dungeon_tile_t* tile, const dungeon_tile_type_t type);

const entityid dungeon_tile_add(dungeon_tile_t* tile, const entityid id);

const entityid dungeon_tile_remove(dungeon_tile_t* tile, const entityid id);
