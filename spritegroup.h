#pragma once

#include "sprite.h"

typedef struct spritegroup_t {
    int size;
    int capacity;

    // the current sprite row
    int current;
    Rectangle dest;
    Vector2 move;

    int off_x;
    int off_y;

    sprite** sprites;

} spritegroup_t;


spritegroup_t* spritegroup_create(int capacity);
void spritegroup_destroy(spritegroup_t* sg);

void spritegroup_add(spritegroup_t* sg, sprite* s);
void spritegroup_set(spritegroup_t* sg, int index, sprite* s);

sprite* spritegroup_get(spritegroup_t* sg, int index);

void spritegroup_setcontexts(spritegroup_t* sg, int context);


void spritegroup_incr(spritegroup_t* sg);
