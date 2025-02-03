#pragma once


#include "entityid.h"
#include "entitytype.h"

#define ENTITY_NAME_LEN_MAX 64

// forward declaration
typedef struct entity_t entity_t;

typedef struct entity_t {
    entityid id;
    entitytype_t type;
    char name[ENTITY_NAME_LEN_MAX];

    int x;
    int y;

    entity_t* next;
} entity;



entity_t* entity_new(const entityid id, const entitytype_t type, const char* name);
entity_t* entity_new_at(const entityid id, const entitytype_t type, const int x, const int y, const char* name);


void entity_free(entity_t* e);
