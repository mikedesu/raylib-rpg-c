#pragma once


#include "direction.h"
#include "entityid.h"
#include "entitytype.h"
#include "race.h"
#include <stdbool.h>

#include "entity_actions.h" // Include the new enum

#define ENTITY_NAME_LEN_MAX 64

// forward declaration
typedef struct entity_t entity_t;

typedef struct entity_t {
    entityid id;
    entitytype_t type;
    race_t race;
    char name[ENTITY_NAME_LEN_MAX];

    direction_t direction;

    int x;
    int y;
    int floor;
    int sprite_move_x;
    int sprite_move_y;

    bool do_update;
    bool is_attacking;
    bool is_damaged;

    entity_action_t default_action; // New field

    entity_t* next;

} entity;


entity_t* entity_new(const entityid id, const entitytype_t type);
entity_t* entity_new_at(const entityid id, const entitytype_t type, const int x, const int y, const int floor);
entity_t* entity_new_npc_at(const entityid id, race_t r, const int x, const int y, const int floor, const char* name);

void entity_set_name(entity_t* const e, const char* name);
void entity_set_race(entity_t* const e, const race_t race);

void entity_free(entity_t* e);

void entity_set_x(entity_t* const e, const int x);
void entity_set_y(entity_t* const e, const int y);
void entity_set_xy(entity_t* const e, const int x, const int y);
void entity_incr_x(entity_t* const e);
void entity_incr_y(entity_t* const e);
void entity_decr_x(entity_t* const e);
void entity_decr_y(entity_t* const e);

void entity_set_dir(entity_t* const e, const direction_t dir);
const direction_t entity_get_dir(entity_t* const e);

void entity_set_type(entity_t* const e, const entitytype_t type);
const entitytype_t entity_get_type(entity_t* const e);


void entity_set_default_action(entity_t* const e, const entity_action_t action);
const entity_action_t entity_get_default_action(entity_t* const e);
