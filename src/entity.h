#pragma once

#include "direction.h"
#include "entity_actions.h" // Include the new enum
#include "entityid.h"
#include "entitytype.h"
#include "massert.h"
#include "potiontype.h"
#include "race.h"
#include <stdbool.h>
#include <string.h>

#define ENTITY_NAME_LEN_MAX 64

#define ENTITY_INVENTORY_MAX_SIZE 32

// forward declaration
typedef struct entity_t entity_t;

typedef struct {
    int level;
    int hp;
    int maxhp;
    int mp;
    int maxmp;
} entity_stats_t;

typedef struct entity_t {
    entityid id;
    entitytype_t type;
    entity_action_t default_action; // New field
    direction_t direction;
    race_t race;
    char name[ENTITY_NAME_LEN_MAX];
    entityid inventory[ENTITY_INVENTORY_MAX_SIZE];
    int inventory_count;
    int x;
    int y;
    int floor;
    int sprite_move_x;
    int sprite_move_y;

    entity_stats_t stats;

    // if the entity is a potion...
    potiontype_t potion_type;

    bool do_update;
    bool is_attacking;
    bool is_blocking;
    bool block_success;
    bool is_damaged;
    bool dead;
    entityid weapon;
    entityid shield;
    entity_t* next;
} entity;

entity_t* e_new(entityid id, entitytype_t type);
entity_t* e_new_at(entityid id, entitytype_t type, int x, int y, int floor);

static inline void e_set_x(entity_t* const e, int x) {
    massert(e, "entity is null");
    massert(x >= 0, "x is less than 0");
    e->x = x;
}

static inline void e_set_y(entity_t* const e, int y) {
    massert(e, "entity is null");
    massert(y >= 0, "y is less than 0");
    e->y = y;
}

static inline void e_set_xy(entity_t* const e, int x, int y) {
    e_set_x(e, x);
    e_set_y(e, y);
}
static inline void e_set_name(entity_t* const e, const char* name) {
    massert(e, "e is NULL");
    massert(name, "name is NULL");
    massert(strlen(name) < ENTITY_NAME_LEN_MAX, "name is too long");
    strncpy(e->name, name, ENTITY_NAME_LEN_MAX);
}

static inline void e_set_hp(entity_t* const e, int hp) {
    massert(e, "e is NULL");
    e->stats.hp = hp < 0 ? 0 : hp > e->stats.maxhp ? e->stats.maxhp : hp;
}

static inline void e_set_maxhp(entity_t* const e, int maxhp) {
    massert(e, "e is NULL");
    massert(maxhp > 0, "maxhp is less than 1");
    e->stats.maxhp = maxhp;
}

static inline void e_set_is_dead(entity_t* const e, bool dead) {
    massert(e, "e is NULL");
    e->dead = dead;
}

static inline void e_set_do_update(entity_t* const e, bool do_update) {
    massert(e, "e is NULL");
    e->do_update = do_update;
}

static inline int e_get_inventory_count(entity_t* const e) {
    massert(e, "e is NULL");
    return e->inventory_count;
}

static inline void e_set_floor(entity_t* const e, int floor) {
    massert(e, "e is NULL");
    massert(floor >= 0, "floor is less than 0");
    e->floor = floor;
}

static inline void e_set_race(entity_t* const e, race_t race) {
    massert(e, "entity is null");
    massert(race >= 0, "race is less than 0");
    massert(race < RACE_COUNT, "race is greater than RACE_COUNT");
    e->race = race;
}

static inline entity_t* e_new_npc_at(entityid id, race_t r, int x, int y, int floor, const char* name) {
    entity_t* e = e_new_at(id, ENTITY_NPC, x, y, floor);
    massert(e, "Failed to create entity");
    e_set_race(e, r);
    e_set_name(e, name);
    return e;
}

static inline entity_t* e_new_weapon(entityid id, const char* name) {
    entity_t* e = e_new(id, ENTITY_WEAPON);
    massert(e, "Failed to create weapon entity");
    e_set_name(e, name);
    return e;
}

static inline entity_t* e_new_potion(entityid id, potiontype_t potion_type, const char* name) {
    massert(id >= 0, "id is less than 0");
    massert(potion_type >= 0, "potion_type is less than 0");
    massert(potion_type < POTION_COUNT, "potion_type is greater than POTION_COUNT");
    massert(name, "name is NULL");
    massert(strlen(name) > 0, "name is empty");
    massert(strlen(name) < ENTITY_NAME_LEN_MAX, "name is too long");
    entity_t* e = e_new(id, ENTITY_POTION);
    massert(e, "Failed to create potion entity");
    e_set_name(e, name);
    e->potion_type = potion_type;
    return e;
}

static inline entity_t* e_new_potion_at(entityid id, potiontype_t potion_type, const char* name, int x, int y, int floor) {
    entity_t* e = e_new_potion(id, potion_type, name);
    massert(e, "Failed to create potion entity");
    e_set_xy(e, x, y);
    e_set_floor(e, floor);
    return e;
}

static inline entity_t* e_new_weapon_at(entityid id, int x, int y, int floor, const char* name) {
    entity_t* e = e_new_weapon(id, name);
    massert(e, "Failed to create weapon entity");
    e_set_xy(e, x, y);
    e_set_floor(e, floor);
    return e;
}

static inline entity_t* e_new_shield(entityid id, const char* name) {
    entity_t* e = e_new(id, ENTITY_SHIELD);
    massert(e, "Failed to create shield entity");
    e_set_name(e, name);
    return e;
}

static inline entity_t* e_new_shield_at(entityid id, int x, int y, int floor, const char* name) {
    entity_t* e = e_new_shield(id, name);
    massert(e, "Failed to create shield entity");
    e_set_xy(e, x, y);
    e_set_floor(e, floor);
    return e;
}

static inline void e_incr_x(entity_t* const e) {
    massert(e, "e is NULL");
    e_set_x(e, e->x + 1);
}

static inline void e_incr_y(entity_t* const e) {
    massert(e, "e is NULL");
    e_set_y(e, e->y + 1);
}

static inline void e_decr_x(entity_t* const e) {
    massert(e, "e is NULL");
    e_set_x(e, e->x - 1);
}

static inline void e_decr_y(entity_t* const e) {
    massert(e, "e is NULL");
    e_set_y(e, e->y - 1);
}

static inline void e_set_dir(entity_t* const e, direction_t dir) {
    massert(e, "e is NULL");
    massert(dir >= 0, "dir is less than 0");
    massert(dir < DIR_COUNT, "dir is greater than DIR_COUNT");
    e->direction = dir;
}

static inline direction_t e_get_dir(entity_t* const e) {
    massert(e, "e is NULL");
    return e->direction;
}

static inline void e_set_type(entity_t* const e, entitytype_t type) {
    massert(e, "e is NULL");
    massert(type >= 0, "type is less than 0");
    massert(type < ENTITY_TYPE_COUNT, "type is greater than ENTITY_COUNT");
    e->type = type;
}

static inline void e_set_default_action(entity_t* const e, entity_action_t action) {
    massert(e, "e is NULL");
    massert(action >= 0, "action is less than 0");
    massert(action < ENTITY_ACTION_COUNT, "action is greater than ENTITY_ACTION_COUNT");
    e->default_action = action;
}

static inline entity_action_t e_get_default_action(entity_t* const e) {
    massert(e, "e is NULL");
    return e->default_action;
}

static inline int e_get_hp(entity_t* const e) {
    massert(e, "e is NULL");
    return e->stats.hp;
}

static inline int e_get_maxhp(entity_t* const e) {
    massert(e, "e is NULL");
    return e->stats.maxhp;
}

static inline void e_set_hpmaxhp(entity_t* const e, int hp, int maxhp) {
    massert(e, "e is NULL");
    e_set_hp(e, hp);
    e_set_maxhp(e, maxhp);
}

static inline void e_incr_hp(entity_t* const e, int hp) {
    massert(e, "e is NULL");
    e_set_hp(e, e->stats.hp + hp);
}

static inline void e_decr_hp(entity_t* const e, int hp) {
    massert(e, "e is NULL");
    e_set_hp(e, e->stats.hp - hp);
}

static inline bool e_is_alive(entity_t* const e) {
    massert(e, "e is NULL");
    return !e->dead;
}

static inline bool e_is_dead(entity_t* const e) {
    massert(e, "e_is_dead: e is NULL");
    return e->dead;
}

static inline entitytype_t e_get_type(entity_t* const e) {
    massert(e, "e_get_type: e is NULL");
    return e->type;
}

static inline void e_equip_weapon(entity* e, entityid weapon_id) {
    massert(e, "entity is NULL");
    e->weapon = weapon_id;
}

static inline bool e_item_is_in_inventory(entity_t* const e, entityid item_id) {
    massert(e, "e is NULL");
    massert(item_id >= 0, "item_id is less than 0");
    for (int i = 0; i < e->inventory_count; i++) {
        if (e->inventory[i] == item_id) return true;
    }
    return false;
}

bool e_add_item_to_inventory(entity_t* const e, entityid item_id);
bool e_remove_item_from_inventory(entity_t* const e, entityid item_id);
