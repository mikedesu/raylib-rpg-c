#pragma once

#include "entityid.h"
#include "specifier.h"
#include "sprite.h"

#define SPRITEGROUP_ANIM_QUEUE_MAX 32

typedef struct spritegroup_t {
    int size;
    int capacity;
    int current; // the current animation sprite in the spritegroup
    int off_x;
    int off_y;
    int default_anim; // the default animation to return to after the timer expires
    specifier_t specifier;
    entityid id; // the id of the entity that this spritegroup belongs to
    // note this allows for one entity to have multiple spritegroups
    Rectangle dest;
    Rectangle move;
    sprite** sprites;
} spritegroup_t;

spritegroup_t* spritegroup_create(int capacity);

sprite* spritegroup_get(spritegroup_t* const sg, int index);

int spritegroup_get_first_context(spritegroup_t* const sg);

specifier_t spritegroup_get_specifier(spritegroup_t* const sg);

bool spritegroup_set_current(spritegroup_t* const sg, int index);
bool spritegroup_is_animating(spritegroup_t* const sg);

void spritegroup_add(spritegroup_t* const sg, sprite* s);
void spritegroup_set(spritegroup_t* const sg, int index, sprite* s);
void spritegroup_setcontexts(spritegroup_t* const sg, int context);
void spritegroup_destroy(spritegroup_t* sg);
void spritegroup_set_specifier(spritegroup_t* const sg, specifier_t spec);

void spritegroup_set_stop_on_last_frame(spritegroup_t* const sg, bool do_stop);
void spritegroup_update_dest(spritegroup_t* const sg);
void spritegroup_snap_dest(spritegroup_t* const sg, int x, int y);
