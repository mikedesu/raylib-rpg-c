#include "entity.h"

#include <stdlib.h>
#include <string.h>

static entityid next_entity_id = 0;

entity_t* entity_create(const char* name) {
    entity_t* entity = malloc(sizeof(entity_t));
    entity->id = next_entity_id++;
    entity->x = -1;
    entity->y = -1;

    strncpy(entity->name, name, ENTITY_NAME_MAX);

    return entity;
}

void entity_destroy(entity_t* entity) {
    free(entity);
}
