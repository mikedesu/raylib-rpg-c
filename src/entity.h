#pragma once

#include "direction.h"
#include "entity_actions.h" // Include the new enum
#include "entityid.h"
#include "entitytype.h"
#include "massert.h"
#include "race.h"
#include <stdbool.h>

#define ENTITY_NAME_LEN_MAX 64

#define ENTITY_INVENTORY_MAX_SIZE 32

// forward declaration
typedef struct entity_t entity_t;

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
    int level;
    int hp;
    int maxhp;
    int mp;
    int maxmp;
    bool do_update;
    bool is_attacking;
    bool is_blocking;
    bool block_success;
    bool is_damaged;
    bool is_dead;
    entityid weapon;
    entityid shield;
    entity_t* next;
} entity;

entity_t* e_new_npc_at(entityid id, race_t r, int x, int y, int floor, const char* name);

entity_t* e_new(entityid id, entitytype_t type);
entity_t* e_new_at(entityid id, entitytype_t type, int x, int y, int floor);
entity_t* e_new_weapon_at(entityid id, int x, int y, int floor, const char* name);
entity_t* e_new_shield_at(entityid id, int x, int y, int floor, const char* name);

void e_free(entity_t* e);
void e_set_floor(entity_t* const e, int floor);
void e_set_name(entity_t* const e, const char* name);
void e_set_race(entity_t* const e, race_t race);
void e_set_x(entity_t* const e, int x);
void e_set_y(entity_t* const e, int y);
void e_set_xy(entity_t* const e, int x, int y);
void e_set_dir(entity_t* const e, direction_t dir);
void e_set_type(entity_t* const e, entitytype_t type);
void e_set_hp(entity_t* const e, int hp);
void e_set_maxhp(entity_t* const e, int maxhp);
void e_set_hpmaxhp(entity_t* const e, int hp, int maxhp);
void e_set_default_action(entity_t* const e, entity_action_t action);
void e_set_is_dead(entity_t* const e, bool is_dead);
void e_set_do_update(entity_t* const e, bool do_update);
void e_incr_hp(entity_t* const e, int hp);
void e_incr_x(entity_t* const e);
void e_incr_y(entity_t* const e);
void e_decr_hp(entity_t* const e, int hp);
void e_decr_x(entity_t* const e);
void e_decr_y(entity_t* const e);

direction_t e_get_dir(entity_t* const e);

entity_action_t e_get_default_action(entity_t* const e);

int e_get_hp(entity_t* const e);
int e_get_maxhp(entity_t* const e);
int e_get_inventory_count(entity_t* const e);

bool e_item_is_already_in_inventory(entity_t* const e, entityid item_id);
bool e_add_item_to_inventory(entity_t* const e, entityid item_id);
bool e_remove_item_from_inventory(entity_t* const e, entityid item_id);
bool e_is_alive(entity_t* const e);

static inline bool e_is_dead(entity_t* const e) {
    massert(e, "e_is_dead: e is NULL");
    return e->is_dead;
}

static inline entitytype_t e_get_type(entity_t* const e) {
    massert(e, "e_get_type: e is NULL");
    return e->type;
}
