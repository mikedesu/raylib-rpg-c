#pragma once

#include "dungeon_tile_type.h"
#include <stdbool.h>

typedef struct {

    dungeon_tile_type_t type;
    bool visible;
    bool explored;

} dungeon_tile_t;
