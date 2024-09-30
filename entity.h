#pragma once

#include "entityid.h"
#include "entitytype.h"
#include <raylib.h>

#define ENTITY_NAME_MAX 128

typedef struct entity_t {
    char name[ENTITY_NAME_MAX];
    entityid id;
    entitytype_t type;
    Vector2 pos;
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


void entity_set_pos(entity_t* e, const Vector2 pos);
void entity_set_pos_unsafe(entity_t* e, const Vector2 pos);


void entity_move(entity_t* e, const Vector2 dir);
void entity_move_unsafe(entity_t* e, const Vector2 dir);


void entity_destroy(entity_t* entity);
void entity_destroy_unsafe(entity_t* entity);
