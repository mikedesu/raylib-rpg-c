#pragma once

#include "entityid.h"
#include "entitytype.h"
#include "location.h"
#include <stdio.h>

typedef enum component_t { COMP_NONE, COMP_ID, COMP_NAME, COMP_TYPE, COMP_COUNT } component;

typedef struct name_component_t {
    entityid id;
    char name[32];
} name_component;

typedef struct type_component_t {
    entityid id;
    entitytype_t type;
} type_component;

void init_name_component(name_component* comp, entityid id, const char* name);
void init_type_component(type_component* comp, entityid id, entitytype_t type);

//typedef struct hp_component_t {
//    entityid id;
//    int hp;
//    int maxhp;
//} hp_component;

//typedef struct loc_component_t {
//    entityid id;
//    loc_t loc;
//} loc_component;
