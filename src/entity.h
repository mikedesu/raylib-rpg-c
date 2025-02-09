#pragma once


#include "entityid.h"
#include "entitytype.h"
#include "race.h"

#define ENTITY_NAME_LEN_MAX 64

// forward declaration
typedef struct entity_t entity_t;

typedef struct entity_t {
    entityid id;
    entitytype_t type;
    race_t race;
    char name[ENTITY_NAME_LEN_MAX];

    int x;
    int y;

    entity_t* next;
} entity;



entity_t* entity_new(const entityid id, const entitytype_t type);
entity_t* entity_new_at(const entityid id, const entitytype_t type, const int x, const int y);

void entity_set_name(entity_t* const e, const char* name);
void entity_set_race(entity_t* const e, const race_t race);


void entity_free(entity_t* e);
