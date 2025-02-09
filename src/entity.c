#include "entity.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

entity_t* entity_new(const entityid id, const entitytype_t type) {
    entity_t* e = (entity_t*)malloc(sizeof(entity_t));
    if (e == NULL) {
        merror("Failed to allocate memory for entity");
        return NULL;
    }
    e->id = id;
    e->type = type;
    e->race = RACE_NONE;
    e->x = -1;
    e->y = -1;
    strncpy(e->name, "NONAME", ENTITY_NAME_LEN_MAX);
    e->next = NULL;
    return e;
}




entity_t* entity_new_at(const entityid id, const entitytype_t type, const int x, const int y) {
    entity_t* e = entity_new(id, type);
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




void entity_set_name(entity_t* const e, const char* name) {
    if (e == NULL) {
        merror("Attempted to set name of NULL entity");
        return;
    }

    if (name == NULL) {
        merror("Attempted to set name of entity to NULL");
        return;
    }

    if (strlen(name) >= ENTITY_NAME_LEN_MAX) {
        merror("Attempted to set name of entity to string longer than ENTITY_NAME_LEN_MAX");
        return;
    }


    strncpy(e->name, name, ENTITY_NAME_LEN_MAX);
}



void entity_set_race(entity_t* const e, const race_t race) {
    if (e == NULL) {
        merror("e is NULL");
        return;
    }

    if (race < 0 || race >= RACE_COUNT) {
        merror("race is out of bounds");
        return;
    }

    e->race = race;
}
