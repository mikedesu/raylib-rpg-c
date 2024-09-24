#pragma once

#include "entityid.h"
//#include <raylib.h>

#define ENTITY_NAME_MAX 128

typedef struct entity_t {
    char name[ENTITY_NAME_MAX];
    entityid id;
    int x;
    int y;
} entity_t;

entity_t* entity_create(const char* name);

void entity_destroy(entity_t* entity);
