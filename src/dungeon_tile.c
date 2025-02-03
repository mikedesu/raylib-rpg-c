#include "dungeon_tile.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

void dungeon_tile_init(dungeon_tile_t* tile, const dungeon_tile_type_t type) {
    if (tile == NULL) {
        merror("dungeon_tile_init: tile is NULL");
        return;
    }

    tile->type = type;
    tile->visible = false;
    tile->explored = false;

    size_t malloc_sz = sizeof(entityid) * DUNGEON_TILE_MAX_ENTITIES_DEFAULT;
    tile->entities = (entityid*)malloc(malloc_sz);
    if (tile->entities == NULL) {
        merror("dungeon_tile_init: tile->entities malloc failed");
        return;
    }
    memset(tile->entities, -1, malloc_sz);
    tile->entity_count = 0;
    tile->entity_max = DUNGEON_TILE_MAX_ENTITIES_DEFAULT;
}



void dungeon_tile_resize(dungeon_tile_t* tile) {
    size_t new_max = tile->entity_max * 2;
    size_t malloc_sz = sizeof(entityid) * new_max;
    if (malloc_sz > DUNGEON_TILE_MAX_ENTITIES_MAX) {
        merror("dungeon_tile_resize: malloc_sz > DUNGEON_TILE_MAX_ENTITIES_MAX");
        return;
    }
    if (tile) {
        // instead of reallocing, do a malloc instead, a memset, and then copy
        entityid* new_entities = (entityid*)malloc(malloc_sz);
        if (new_entities == NULL) {
            merror("dungeon_tile_resize: new_entities malloc failed");
            return;
        }
        memset(new_entities, -1, malloc_sz);
        memcpy(new_entities, tile->entities, sizeof(entityid) * tile->entity_max);
        free(tile->entities);
        tile->entities = new_entities;
        tile->entity_max = new_max;
    }
}



void dungeon_tile_add(dungeon_tile_t* tile, const entityid id) {
    int index = -1;
    for (int i = 0; i < DUNGEON_TILE_MAX_ENTITIES_DEFAULT; i++) {
        if (tile->entities[i] == -1) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        tile->entities[index] = id;
        tile->entity_count++;
    } else {
        merror("dungeon_tile_add: no space for entity");
        minfo("resizing...");
        dungeon_tile_resize(tile);
    }
}




void dungeon_tile_remove(dungeon_tile_t* tile, const entityid id) {
    for (int i = 0; i < tile->entity_max; i++) {
        if (tile->entities[i] == id) {
            tile->entities[i] = -1;
            // shift all elements to the left with memcpy
            memcpy(&tile->entities[i], &tile->entities[i + 1], sizeof(entityid) * (tile->entity_max - i - 1));

            //for (int j = i; j < tile->entity_max - 1; j++) {
            //    tile->entities[j] = tile->entities[j + 1];
            //}

            tile->entities[tile->entity_max - 1] = -1;
            tile->entity_count--;
        }
    }
}



dungeon_tile_t* dungeon_tile_create(const dungeon_tile_type_t type) {
    dungeon_tile_t* tile = (dungeon_tile_t*)malloc(sizeof(dungeon_tile_t));
    if (tile == NULL) {
        return NULL;
    }
    dungeon_tile_init(tile, type);
    return tile;
}



void dungeon_tile_free(dungeon_tile_t* tile) {
    if (tile) {
        free(tile->entities);
        tile->entities = NULL;
        free(tile);
    }
}
