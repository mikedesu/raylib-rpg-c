#include "entity.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

entity_t* entity_new(entityid id, entitytype_t type) {
    entity_t* e = malloc(sizeof(entity_t));
    if (!e) {
        merror("Failed to allocate memory for entity");
        return NULL;
    }
    e->id = id;
    e->type = type;
    e->race = RACE_NONE;
    e->direction = DIR_RIGHT;
    e->x = e->y = e->floor = -1;
    e->hp = e->maxhp = 1;
    e->do_update = false;
    e->sprite_move_x = e->sprite_move_y = 0;
    e->is_attacking = e->is_damaged = e->is_dead = false;
    strncpy(e->name, "NONAME", ENTITY_NAME_LEN_MAX);
    e->next = NULL;
    e->default_action = ENTITY_ACTION_MOVE_RANDOM; // Default for NPCs, can be overridden
    return e;
}


entity_t* entity_new_at(entityid id, entitytype_t type, int x, int y, int floor) {
    entity_t* e = entity_new(id, type);
    if (!e) {
        merror("Failed to create entity");
        return NULL;
    }
    e->x = x;
    e->y = y;
    e->floor = floor;
    return e;
}


entity_t* entity_new_npc_at(entityid id, race_t r, int x, int y, int floor, const char* name) {
    entity_t* e = entity_new_at(id, ENTITY_NPC, x, y, floor);
    if (!e) {
        merror("Failed to create entity");
        return NULL;
    }
    entity_set_race(e, r);
    entity_set_name(e, name);
    return e;
}


// this frees the entity so you lose the next pointer!
// make sure you wire the map correctly before calling this!
void entity_free(entity_t* e) {
    if (!e) return;
    free(e);
}


void entity_set_name(entity_t* const e, const char* name) {
    if (!e || !name || strlen(name) >= ENTITY_NAME_LEN_MAX) return;
    strncpy(e->name, name, ENTITY_NAME_LEN_MAX);
}


void entity_set_race(entity_t* const e, race_t race) {
    if (!e || race < 0 || race >= RACE_COUNT) return;
    e->race = race;
}


void entity_set_x(entity_t* const e, int x) {
    if (!e || x < 0) return;
    e->x = x;
}

void entity_set_y(entity_t* const e, int y) {
    if (!e || y < 0) return;
    e->y = y;
}


void entity_set_xy(entity_t* const e, int x, int y) {
    entity_set_x(e, x);
    entity_set_y(e, y);
}


void entity_incr_x(entity_t* const e) {
    entity_set_x(e, e->x + 1);
}


void entity_incr_y(entity_t* const e) {
    entity_set_y(e, e->y + 1);
}


void entity_decr_x(entity_t* const e) {
    entity_set_x(e, e->x - 1);
}

void entity_decr_y(entity_t* const e) {
    entity_set_y(e, e->y - 1);
}


void entity_set_dir(entity_t* const e, direction_t dir) {
    if (!e || dir < 0 || dir >= DIR_COUNT) return;
    e->direction = dir;
}


direction_t entity_get_dir(entity_t* const e) {
    if (e == NULL) {
        merror("e is NULL");
        return DIR_NONE;
    }
    return e->direction;
}


void entity_set_type(entity_t* const e, entitytype_t type) {
    if (!e || type < 0 || type >= ENTITY_COUNT) return;
    e->type = type;
}


entitytype_t entity_get_type(entity_t* const e) {
    if (!e) {
        merror("entity_get_type: e is NULL");
        return ENTITY_NONE;
    }
    return e->type;
}


void entity_set_default_action(entity_t* const e, entity_action_t action) {
    if (!e || action < 0 || action >= ENTITY_ACTION_COUNT) return;
    e->default_action = action;
}


entity_action_t entity_get_default_action(entity_t* const e) {
    if (!e) {
        merror("entity_get_default_action: e is NULL");
        return ENTITY_ACTION_NONE;
    }
    return e->default_action;
}


void entity_set_hp(entity_t* const e, int hp) {
    if (!e) return;
    e->hp = hp;
}


int entity_get_hp(entity_t* const e) {
    if (!e) {
        merror("entity_get_hp: e is NULL");
        return -1;
    }

    return e->hp;
}


void entity_set_maxhp(entity_t* const e, int maxhp) {
    if (!e) return;
    e->maxhp = maxhp;
}


int entity_get_maxhp(entity_t* const e) {
    if (!e) {
        merror("entity_get_maxhp: e is NULL");
        return -1;
    }

    return e->maxhp;
}


void entity_set_hpmaxhp(entity_t* const e, int hp, int maxhp) {
    if (!e) return;
    e->hp = hp;
    e->maxhp = maxhp;
}
