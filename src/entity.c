#include "entity.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

entity_t* entity_new(const entityid id, const entitytype_t type, const char* name) {
    entity_t* e = (entity_t*)malloc(sizeof(entity_t));
    if (e == NULL) {
        merror("Failed to allocate memory for entity");
        return NULL;
    }
    e->id = id;
    e->type = type;
    e->x = -1;
    e->y = -1;
    strncpy(e->name, name, ENTITY_NAME_LEN_MAX);
    e->next = NULL;
    return e;
}




entity_t* entity_new_at(const entityid id, const entitytype_t type, const int x, const int y, const char* name) {
    entity_t* e = entity_new(id, type, name);
    if (e == NULL) {
        return NULL;
    }
    e->x = x;
    e->y = y;
    return e;
}




// this frees the entity so you lose the next pointer!
// make sure you wire the map correctly before calling this!
void entity_free(entity_t* e) {
    if (e == NULL) {
        merror("Attempted to destroy NULL entity");
        return;
    }
    free(e);
}
