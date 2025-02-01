#include "dungeon_tile.h"
#include "mprint.h"
#include <string.h>

void dungeon_tile_init(dungeon_tile_t* tile, const dungeon_tile_type_t type) {
    if (tile == NULL) {
        merror("dungeon_tile_init: tile is NULL");
        return;
    }

    tile->type = type;
    tile->visible = false;
    tile->explored = false;

    memset(tile->entities, -1, sizeof(tile->entities));
}




void dungeon_tile_add(dungeon_tile_t* tile, const entityid id) {

    int index = -1;

    for (int i = 0; i < DUNGEON_TILE_MAX_ENTITIES; i++) {
        if (tile->entities[i] == -1) {
            //tile->entities[i] = id;
            index = i;
            break;
        }
    }

    if (index == -1) {
        merror("dungeon_tile_add: no room for entity");
        return;
    }

    tile->entities[index] = id;

    tile->entity_count++;

    char debug[256];
    sprintf(debug, "dungeon_tile_add: added entity %d to index %d", id, index);
    msuccess(debug);
}




void dungeon_tile_remove(dungeon_tile_t* tile, const entityid id) {
    for (int i = 0; i < DUNGEON_TILE_MAX_ENTITIES; i++) {
        if (tile->entities[i] == id) {
            tile->entities[i] = -1;
            return;
        }
    }
    merror("dungeon_tile_remove: entity not found");
}
