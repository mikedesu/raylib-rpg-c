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
    e->direction = DIRECTION_LEFT;
    e->x = -1;
    e->y = -1;
    e->floor = -1;
    e->do_update = false;
    e->sprite_move_x = 0;
    e->sprite_move_y = 0;
    e->is_attacking = false;
    e->is_damaged = false;
    strncpy(e->name, "NONAME", ENTITY_NAME_LEN_MAX);
    e->next = NULL;
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


entity_t* entity_new_npc_at(const entityid id, race_t r, const int x, const int y, const int floor, const char* name) {
    entity_t* e = entity_new_at(id, ENTITY_NPC, x, y, floor);
    if (e == NULL) {
        return NULL;
    }
    entity_set_race(e, r);
    entity_set_name(e, name);
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


void entity_set_dir(entity_t* const e, const direction_t dir) {
    if (e == NULL) {
        merror("e is NULL");
        return;
    }
    // check that dir is valid
    if (dir < 0 || dir >= DIRECTION_COUNT) {
        merror("dir is out of bounds");
        return;
    }

    e->direction = dir;
}


const direction_t entity_get_dir(entity_t* const e) {
    if (e == NULL) {
        merror("e is NULL");
        return DIRECTION_NONE;
    }

    return e->direction;
}


void entity_set_type(entity_t* const e, const entitytype_t type) {

    if (!e) {
        merror("entity_set_type: e is NULL");
        return;
    }

    if (type < 0 || type >= ENTITY_COUNT) {
        merror("entity_set_type: type is out of bounds");
        return;
    }

    e->type = type;
}


const entitytype_t entity_get_type(entity_t* const e) {
    if (!e) {
        merror("entity_get_type: e is NULL");
        return ENTITY_NONE;
    }

    return e->type;
}
