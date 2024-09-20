#pragma once

#include "sprite.h"

typedef struct spritegroup {
    int size;
    int capacity;
    int current;
    sprite** sprites;

    Rectangle dest;
} spritegroup;


spritegroup* spritegroup_create(int capacity);
void spritegroup_destroy(spritegroup* sg);

void spritegroup_add(spritegroup* sg, sprite* s);
void spritegroup_set(spritegroup* sg, int index, sprite* s);

void spritegroup_setcontexts(spritegroup* sg, int context);
