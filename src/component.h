#pragma once

#include "direction.h"
#include "entityid.h"
#include "entitytype.h"
#include "location.h"
#include "race.h"
#include <stdio.h>

typedef enum component_t
{
    COMP_NONE,
    COMP_ID,
    COMP_NAME,
    COMP_TYPE,
    COMP_RACE,
    COMP_DIRECTION,
    COMP_LOCATION,
    COMP_SPRITE_MOVE,
    COMP_COUNT
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

void init_name_component(name_component* comp, entityid id, const char* name);
void init_type_component(type_component* comp, entityid id, entitytype_t type);
void init_race_component(race_component* comp, entityid id, race_t race);
void init_direction_component(direction_component* comp, entityid id, direction_t dir);
void init_loc_component(loc_component* comp, entityid id, loc_t loc);
void init_sprite_move_component(sprite_move_component* comp, entityid id, loc_t loc);

//typedef struct hp_component_t {
//    entityid id;
//    int hp;
//    int maxhp;
//} hp_component;

//typedef struct loc_component_t {
//    entityid id;
//    loc_t loc;
//} loc_component;
