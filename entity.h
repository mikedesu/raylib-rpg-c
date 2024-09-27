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

void entity_set_name(entity_t* e, const char* n);
void entity_set_name_unsafe(entity_t* e, const char* n);
void entity_set_x(entity_t* e, const int x);
void entity_set_x_unsafe(entity_t* e, const int x);
void entity_set_y(entity_t* e, const int y);
void entity_set_y_unsafe(entity_t* e, const int y);
void entity_set_xy(entity_t* e, const int x, const int y);
void entity_set_xy_unsafe(entity_t* e, const int x, const int y);
void entity_destroy(entity_t* entity);
void entity_destroy_unsafe(entity_t* entity);
