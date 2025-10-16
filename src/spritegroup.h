#pragma once

#include "entityid.h"
#include "libgame_defines.h"
#include "sprite.h"
#include "vec3.h"
#include <memory>
#include <unordered_map>
#include <vector>

#define SPRITEGROUP_ANIM_QUEUE_MAX 32

using std::shared_ptr;
using std::unordered_map;
using std::vector;

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

    vector<shared_ptr<sprite>>* sprites2;

    float move_rate;
} spritegroup_t;

spritegroup_t* spritegroup_create(int capacity);

shared_ptr<sprite> spritegroup_get(spritegroup_t* sg, int index);
shared_ptr<sprite> sg_get_current(spritegroup_t* sg);
shared_ptr<sprite> sg_get_current_plus_one(spritegroup_t* sg);

int spritegroup_get_first_context(spritegroup_t* sg);

void spritegroup_add(spritegroup_t* sg, shared_ptr<sprite> s);
//void spritegroup_set(spritegroup_t* const sg, int index, shared_ptr<sprite> s);
void spritegroup_setcontexts(spritegroup_t* sg, int context);
void spritegroup_destroy(spritegroup_t* sg);
void spritegroup_set_stop_on_last_frame(spritegroup_t* sg, bool do_stop);

bool spritegroup_set_current(spritegroup_t* sg, int index);
bool spritegroup_is_animating(spritegroup_t* sg);
bool spritegroup_update_dest(spritegroup_t* sg);
bool spritegroup_snap_dest(spritegroup_t* sg, int x, int y);

void sg_set_default_anim(spritegroup_t* sg, int anim);

//void sg_incr_frame(spritegroup_t* sg, int index);
