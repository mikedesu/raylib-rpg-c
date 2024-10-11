#pragma once

#include "sprite.h"

typedef struct spritegroup_t {
    int size;
    int capacity;
    int current; // the current animation sprite in the spritegroup
    int move_x;
    int move_y;
    int off_x;
    int off_y;
    Rectangle dest;

    sprite** sprites;
} spritegroup_t;


spritegroup_t* spritegroup_create(int capacity);
sprite* spritegroup_get(spritegroup_t* sg, int index);
void spritegroup_add(spritegroup_t* sg, sprite* s);
void spritegroup_incr(spritegroup_t* sg);
void spritegroup_set(spritegroup_t* sg, int index, sprite* s);
void spritegroup_setcontexts(spritegroup_t* sg, int context);
void spritegroup_set_current(spritegroup_t* sg, int index);
void spritegroup_destroy(spritegroup_t* sg);
