#include "entity.h"
#include "massert.h"
//#include "mprint.h"
#include <stdlib.h>
#include <string.h>

entity_t* e_new(entityid id, entitytype_t type) {
    entity_t* e = malloc(sizeof(entity_t));
    massert(e, "Failed to allocate memory for entity");
    massert(id >= 0, "id is less than 0");
    massert(type >= 0, "type is less than 0");
    massert(type < ENTITY_TYPE_COUNT, "type is greater than ENTITY_TYPE_COUNT");
    e->id = id;
    e->inventory_count = 0;
    e->weapon = -1;
    e->shield = -1;
    e->door_is_open = false;
    e->potion_type = POTION_NONE;
    e->next = NULL;
    e->target_path = NULL;
    e->target_path_length = 0;
    e->target = (loc_t){-1, -1, -1};
    return e;
}

// one caveat to the way entities are handled:
// entities may contain a "next" for use with the entitymap
// you MUST acquire the next pointer externally before calling this function
// otherwise things will break badly!
void e_free(entity_t* e) {
    massert(e, "e is NULL");
    massert(e->next == NULL, "e->next is not NULL. You MUST acquire the next pointer externally and set it to NULL before calling this function");
    if (e->target_path) {
        free(e->target_path);
        e->target_path = NULL;
        e->target_path_length = 0;
    }
    free(e);
}

void e_free_target_path(entity_t* const e) {
    massert(e, "e is NULL");
    if (e->target_path) {
        free(e->target_path);
        e->target_path = NULL;
        e->target_path_length = 0;
    }
}

entity_t* e_new_at(entityid id, entitytype_t type, int x, int y, int z) {
    massert(x >= 0, "x is less than 0");
    massert(y >= 0, "y is less than 0");
    massert(z >= 0, "floor is less than 0");
    entity_t* e = e_new(id, type);
    massert(e, "Failed to create entity");
    return e;
}
