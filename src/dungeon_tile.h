#pragma once

#include "dungeon_tile_type.h"
#include "entityid.h"
#include <stdbool.h>
#include <stddef.h>

#define DUNGEON_TILE_MAX_ENTITIES 32

typedef struct {

    dungeon_tile_type_t type;
    bool visible;
    bool explored;

    entityid entities[DUNGEON_TILE_MAX_ENTITIES];
    size_t entity_count;

} dungeon_tile_t;



void dungeon_tile_init(dungeon_tile_t* tile, const dungeon_tile_type_t type);


void dungeon_tile_add(dungeon_tile_t* tile, const entityid id);
void dungeon_tile_remove(dungeon_tile_t* tile, const entityid id);
