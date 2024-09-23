#pragma once

#include "sprite.h"

typedef struct spritegroup_t {
    int size;
    int capacity;
    int current;
    Rectangle dest;
    Vector2 move;

    sprite** sprites;

} spritegroup_t;


spritegroup_t* spritegroup_create(int capacity);
void spritegroup_destroy(spritegroup_t* sg);

void spritegroup_add(spritegroup_t* sg, sprite* s);
void spritegroup_set(spritegroup_t* sg, int index, sprite* s);

void spritegroup_setcontexts(spritegroup_t* sg, int context);
