#pragma once

#include "entityid.h"
#include "libgame_defines.h"
//#include "specifier.h"
#include "sprite.h"
#include "vec3.h"

#define SPRITEGROUP_ANIM_QUEUE_MAX 32

typedef struct spritegroup_t {
    int size;
    int capacity;
    int current; // the current animation sprite in the spritegroup
    int off_x;
    int off_y;
    int default_anim; // the default animation to return to after the timer expires
    int alpha;
    //specifier_t specifier;
    entityid id; // the id of the entity that this spritegroup belongs to
    // note this allows for one entity to have multiple spritegroups
    Rectangle dest;
    Rectangle move;
    sprite** sprites;

    float move_rate;
} spritegroup_t;

spritegroup_t* spritegroup_create(int capacity);

sprite* spritegroup_get(spritegroup_t* const sg, int index);
sprite* sg_get_current(spritegroup_t* const sg);
sprite* sg_get_current_plus_one(spritegroup_t* const sg);

int spritegroup_get_first_context(spritegroup_t* const sg);

//specifier_t spritegroup_get_specifier(spritegroup_t* const sg);

bool spritegroup_set_current(spritegroup_t* const sg, int index);
bool spritegroup_is_animating(spritegroup_t* const sg);

void spritegroup_add(spritegroup_t* const sg, sprite* s);
void spritegroup_set(spritegroup_t* const sg, int index, sprite* s);
void spritegroup_setcontexts(spritegroup_t* const sg, int context);
void spritegroup_destroy(spritegroup_t* sg);
//void spritegroup_set_specifier(spritegroup_t* const sg, specifier_t spec);
void spritegroup_set_stop_on_last_frame(spritegroup_t* const sg, bool do_stop);
//void spritegroup_update_dest(spritegroup_t* const sg);
bool spritegroup_update_dest(spritegroup_t* const sg);

//void spritegroup_snap_dest(spritegroup_t* const sg, vec3 loc);
bool spritegroup_snap_dest(spritegroup_t* const sg, int x, int y);

void sg_set_default_anim(spritegroup_t* const sg, int anim);
//void spritegroup_snap_dest(spritegroup_t* const sg, int x, int y);
