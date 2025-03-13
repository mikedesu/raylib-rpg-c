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
    e->direction = DIRECTION_RIGHT;
    e->x = e->y = -1;
    e->sprite_move_x = e->sprite_move_y = 0;
    strncpy(e->name, "NONAME", ENTITY_NAME_LEN_MAX);
    e->next = NULL;
    e->floor = -1;
    return e;
}


entity_t* entity_new_at(const entityid id, const entitytype_t type, const int x, const int y, const int floor) {
    entity_t* e = entity_new(id, type);
    if (e == NULL) {
        return NULL;
    }
    e->x = x;
    e->y = y;
    e->floor = floor;
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


void entity_set_x(entity_t* const e, const int x) {
    if (e == NULL) {
        merror("e is NULL");
        return;
    }

    e->x = x;
}

void entity_set_y(entity_t* const e, const int y) {
    if (e == NULL) {
        merror("e is NULL");
        return;
    }

    e->y = y;
}


void entity_set_xy(entity_t* const e, const int x, const int y) {
    entity_set_x(e, x);
    entity_set_y(e, y);
}


void entity_incr_x(entity_t* const e) {
    if (e == NULL) {
        merror("e is NULL");
        return;
    }

    e->x++;
}


void entity_incr_y(entity_t* const e) {
    if (e == NULL) {
        merror("e is NULL");
        return;
    }

    e->y++;
}


void entity_decr_x(entity_t* const e) {
    if (e == NULL) {
        merror("e is NULL");
        return;
    }

    e->x--;
}

void entity_decr_y(entity_t* const e) {
    if (e == NULL) {
        merror("e is NULL");
        return;
    }

    e->y--;
}
