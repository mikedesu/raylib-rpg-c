#include "dungeon_tile.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

void dungeon_tile_init(dungeon_tile_t* tile, const dungeon_tile_type_t type) {
    if (!tile) {
        merror("dungeon_tile_init: tile is NULL");
        return;
    }
    const size_t malloc_sz = sizeof(entityid) * DUNGEON_TILE_MAX_ENTITIES_DEFAULT;
    tile->type = type;
    tile->visible = tile->explored = false;
    tile->entities = (entityid*)malloc(malloc_sz);
    if (tile->entities == NULL) {
        merror("dungeon_tile_init: tile->entities malloc failed");
        return;
    }
    memset(tile->entities, -1, malloc_sz);
    tile->entity_count = 0;
    tile->entity_max = DUNGEON_TILE_MAX_ENTITIES_DEFAULT;
}


const bool dungeon_tile_resize(dungeon_tile_t* tile) {
    if (!tile) {
        merror("dungeon_tile_resize: tile is NULL");
        return false;
    }
    size_t new_max = tile->entity_max * 2;
    size_t malloc_sz = sizeof(entityid) * new_max;
    if (malloc_sz > DUNGEON_TILE_MAX_ENTITIES_MAX) {
        merror("dungeon_tile_resize: malloc_sz > DUNGEON_TILE_MAX_ENTITIES_MAX");
        return false;
    }
    // instead of reallocing, do a malloc instead, a memset, and then copy
    entityid* new_entities = (entityid*)malloc(malloc_sz);
    if (new_entities == NULL) {
        merror("dungeon_tile_resize: new_entities malloc failed");
        return false;
    }
    memset(new_entities, -1, malloc_sz);
    memcpy(new_entities, tile->entities, sizeof(entityid) * tile->entity_max);
    free(tile->entities);
    tile->entities = new_entities;
    tile->entity_max = new_max;
    return true;
}


const entityid dungeon_tile_add(dungeon_tile_t* tile, const entityid id) {
    minfoint("dungeon_tile_add: adding entity", id);
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
        // call add again
        return dungeon_tile_add(tile, id);
    }
    return index;
}


const entityid dungeon_tile_remove(dungeon_tile_t* tile, const entityid id) {
    bool did_remove = false;
    for (int i = 0; i < tile->entity_max; i++) {
        if (tile->entities[i] == id) {
            tile->entities[i] = -1;
            // shift all elements to the left with memcpy
            memcpy(&tile->entities[i], &tile->entities[i + 1], sizeof(entityid) * (tile->entity_max - i - 1));
            tile->entities[tile->entity_max - 1] = -1;
            tile->entity_count--;
            did_remove = true;
            break;
        }
    }
    if (!did_remove) {
        merror("dungeon_tile_remove: entity not found");
        return -1;
    }
    return id;
}


dungeon_tile_t* dungeon_tile_create(const dungeon_tile_type_t type) {
    dungeon_tile_t* tile = (dungeon_tile_t*)malloc(sizeof(dungeon_tile_t));
    if (!tile) {
        return NULL;
    }
    dungeon_tile_init(tile, type);
    return tile;
}


void dungeon_tile_free(dungeon_tile_t* tile) {
    if (!tile) {
        merror("dungeon_tile_free: tile is NULL");
        return;
    }
    free(tile->entities);
    tile->entities = NULL;
    free(tile);
}


const size_t dungeon_tile_entity_count(const dungeon_tile_t* tile) {
    if (!tile) {
        merror("dungeon_tile_entity_count: tile is NULL");
        return 0;
    }
    return tile->entity_count;
}


const entityid dungeon_tile_get_entity(const dungeon_tile_t* tile, const size_t index) {
    if (!tile) {
        merror("dungeon_tile_get_entity: tile is NULL");
        return -1;
    }
    if (index < tile->entity_max) {
        return tile->entities[index];
    }
    return -1;
}
