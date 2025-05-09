#pragma once

#include "direction.h"
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

void init_name_component(name_component* comp, entityid id, const char* name);
void init_type_component(type_component* comp, entityid id, entitytype_t type);
void init_race_component(race_component* comp, entityid id, race_t race);
void init_direction_component(direction_component* comp, entityid id, direction_t dir);
void init_loc_component(loc_component* comp, entityid id, loc_t loc);
void init_sprite_move_component(sprite_move_component* comp, entityid id, loc_t loc);
void init_dead_component(dead_component* comp, entityid id, bool dead);
void init_update_component(update_component* comp, entityid id, bool update);

//typedef struct hp_component_t {
//    entityid id;
//    int hp;
//    int maxhp;
//} hp_component;

//typedef struct loc_component_t {
//    entityid id;
//    loc_t loc;
//} loc_component;
