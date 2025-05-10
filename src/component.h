#pragma once

#include "direction.h"
#include "entity_actions.h"
#include "entityid.h"
#include "entitytype.h"
#include "location.h"
#include "race.h"
#include <stdbool.h>
#include <stdio.h>

typedef enum component_t
{
    C_NONE,
    C_ID,
    C_NAME,
    C_TYPE,
    C_RACE,
    C_DIRECTION,
    C_LOCATION,
    C_SPRITE_MOVE,
    C_DEAD,
    C_UPDATE,
    // not yet implemented
    C_ATTACKING,
    C_BLOCKING,
    C_BLOCK_SUCCESS,
    C_DAMAGED,
    //C_INVENTORY,
    //C_EQUIPPED_WEAPON,
    //C_EQUIPPED_SHIELD,
    //C_TARGET,
    //C_TARGET_PATH,
    //C_DOOR_STATE,
    C_DEFAULT_ACTION,
    //C_ATTRIBUTES,
    //C_MODIFIERS,
    C_COUNT
} component;

typedef struct name_component_t {
    entityid id;
    char name[32];
} name_component;

typedef struct type_component_t {
    entityid id;
    entitytype_t type;
} type_component;

typedef struct race_component_t {
    entityid id;
    race_t race;
} race_component;

typedef struct direction_component_t {
    entityid id;
    direction_t dir;
} direction_component;

typedef struct loc_component_t {
    entityid id;
    loc_t loc;
} loc_component;

typedef struct sprite_move_component_t {
    entityid id;
    loc_t loc;
} sprite_move_component;

typedef struct dead_component_t {
    entityid id;
    bool dead;
} dead_component;

typedef struct update_component_t {
    entityid id;
    bool update;
} update_component;

typedef struct attacking_component_t {
    entityid id;
    bool attacking;
} attacking_component;

typedef struct blocking_component_t {
    entityid id;
    bool blocking;
} blocking_component;

typedef struct block_success_component_t {
    entityid id;
    bool block_success;
} block_success_component;

typedef struct damaged_component_t {
    entityid id;
    bool damaged;
} damaged_component;

typedef struct inventory_component_t {
    entityid id;
    entityid inventory[32]; // max 32 items
    int count;
} inventory_component;

typedef struct equipped_weapon_component_t {
    entityid id;
    entityid weapon;
} equipped_weapon_component;

typedef struct equipped_shield_component_t {
    entityid id;
    entityid shield;
} equipped_shield_component;

typedef struct target_component_t {
    entityid id;
    loc_t target;
} target_component;

typedef struct target_path_component_t {
    entityid id;
    loc_t* target_path;
    int target_path_length;
} target_path_component;

typedef struct default_action_component_t {
    entityid id;
    entity_action_t action;
} default_action_component;

void init_name_component(name_component* comp, entityid id, const char* name);
void init_type_component(type_component* comp, entityid id, entitytype_t type);
void init_race_component(race_component* comp, entityid id, race_t race);
void init_direction_component(direction_component* comp, entityid id, direction_t dir);
void init_loc_component(loc_component* comp, entityid id, loc_t loc);
void init_sprite_move_component(sprite_move_component* comp, entityid id, loc_t loc);
void init_dead_component(dead_component* comp, entityid id, bool dead);
void init_update_component(update_component* comp, entityid id, bool update);
void init_attacking_component(attacking_component* comp, entityid id, bool attacking);
void init_blocking_component(blocking_component* comp, entityid id, bool blocking);
void init_block_success_component(block_success_component* comp, entityid id, bool block_success);
void init_damaged_component(damaged_component* comp, entityid id, bool damaged);
void init_inventory_component(inventory_component* comp, entityid id);
void init_equipped_weapon_component(equipped_weapon_component* comp, entityid id, entityid weapon);
void init_equipped_shield_component(equipped_shield_component* comp, entityid id, entityid shield);
void init_target_component(target_component* comp, entityid id, loc_t target);
void init_target_path_component(target_path_component* comp, entityid id, loc_t* target_path, int target_path_length);
void init_default_action_component(default_action_component* comp, entityid id, entity_action_t default_action);
