#pragma once

#include "direction.h"
#include "entity_actions.h" // Include the new enum
#include "entityid.h"
#include "entitytype.h"
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

    //entityid* inventory;
    //int inventory_max_size;

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
    bool is_damaged;
    bool is_dead;

    entity_t* next;
} entity;

entity_t* entity_new(entityid id, entitytype_t type);
entity_t* entity_new_at(entityid id, entitytype_t type, int x, int y, int floor);
entity_t* entity_new_npc_at(entityid id, race_t r, int x, int y, int floor, const char* name);
entity_t* entity_new_weapon_at(entityid id, int x, int y, int floor, const char* name);

void entity_set_floor(entity_t* const e, int floor);
void entity_set_name(entity_t* const e, const char* name);
void entity_set_race(entity_t* const e, race_t race);
void entity_free(entity_t* e);
void entity_set_x(entity_t* const e, int x);
void entity_set_y(entity_t* const e, int y);
void entity_set_xy(entity_t* const e, int x, int y);
void entity_incr_x(entity_t* const e);
void entity_incr_y(entity_t* const e);
void entity_decr_x(entity_t* const e);
void entity_decr_y(entity_t* const e);
void entity_set_dir(entity_t* const e, direction_t dir);
void entity_set_type(entity_t* const e, entitytype_t type);
void entity_set_hp(entity_t* const e, int hp);
void entity_set_maxhp(entity_t* const e, int maxhp);
void entity_set_hpmaxhp(entity_t* const e, int hp, int maxhp);
void entity_incr_hp(entity_t* const e, int hp);
void entity_decr_hp(entity_t* const e, int hp);
void entity_set_default_action(entity_t* const e, entity_action_t action);

direction_t entity_get_dir(entity_t* const e);

entitytype_t entity_get_type(entity_t* const e);

entity_action_t entity_get_default_action(entity_t* const e);

int entity_get_hp(entity_t* const e);
int entity_get_maxhp(entity_t* const e);
int entity_get_inventory_count(entity_t* const e);

bool entity_item_is_already_in_inventory(entity_t* const e, entityid item_id);
bool entity_add_item_to_inventory(entity_t* const e, entityid item_id);
bool entity_remove_item_from_inventory(entity_t* const e, entityid item_id);
