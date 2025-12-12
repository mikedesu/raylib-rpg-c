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
    // the current animation sprite in the spritegroup
    int current;
    int off_x;
    int off_y;
    // the default animation to return to after the timer expires
    int default_anim;
    int alpha;
    // the id of the entity that this spritegroup belongs to
    // note this allows for one entity to have multiple spritegroups
    entityid id;
    Rectangle dest;
    Rectangle move;
    vector<shared_ptr<sprite>>* sprites2;
    float move_rate;
} spritegroup_t;


static inline spritegroup_t* spritegroup_create(int capacity) {
    massert(capacity > 0, "capacity must be greater than 0, got %d", capacity);
    if (capacity <= 0)
        return NULL;
    spritegroup_t* sg = (spritegroup_t*)malloc(sizeof(spritegroup_t));
    massert(sg, "spritegroup is NULL");
    if (!sg)
        return NULL;
    sg->current = sg->size = sg->off_x = sg->off_y = sg->default_anim = sg->id = 0;
    sg->alpha = 255;
    sg->capacity = capacity;
    sg->sprites2 = new vector<shared_ptr<sprite>>();
    sg->dest = (Rectangle){0, 0, 0, 0};
    sg->move = (Rectangle){0, 0, 0, 0};
    sg->move_rate = 1.0;
    return sg;
}


static inline shared_ptr<sprite> spritegroup_get(spritegroup_t* sg, int index) {
    massert(sg, "spritegroup is NULL");
    massert(index >= 0, "index is negative");
    massert(index < sg->size, "index is out of bounds");
    return sg->sprites2->at(index);
}


static inline shared_ptr<sprite> sg_get_current(spritegroup_t* sg) {
    massert(sg, "spritegroup is NULL");
    massert(sg->current >= 0, "current is negative");
    massert(sg->current < sg->size, "current is out of bounds");
    return sg->sprites2->at(sg->current);
}


static inline shared_ptr<sprite> sg_get_current_plus_one(spritegroup_t* sg) {
    massert(sg, "spritegroup is NULL");
    massert(sg->current >= 0, "current is negative");
    if (sg->current + 1 >= sg->size)
        return NULL;
    return sg->sprites2->at(sg->current + 1);
}


static inline int spritegroup_get_first_context(spritegroup_t* sg) {
    massert(sg, "spritegroup is NULL");
    massert(sg->size > 0, "spritegroup is empty");
    massert(sg->sprites2->at(0), "sprite is NULL");
    return sprite_get_context2(sg->sprites2->at(0));
}


static inline void spritegroup_add(spritegroup_t* sg, shared_ptr<sprite> s) {
    if (!sg || !s || sg->size >= sg->capacity)
        return;
    sg->sprites2->push_back(s);
    sg->size++;
}


// each sprite has a 'context' that corresponds to different directions
static inline void spritegroup_setcontexts(spritegroup_t* sg, int context) {
    massert(sg, "spritegroup is NULL");
    for (int i = 0; i < sg->size; i++) {
        auto s = sg->sprites2->at(i);
        if (!s || s->numcontexts <= 0 || context < 0 || context >= s->numcontexts)
            continue;
        sprite_setcontext2(s, context);
    }
}


static inline void spritegroup_destroy(spritegroup_t* sg) {
    if (!sg)
        return;
    if (sg->sprites2) {
        sg->sprites2->clear();
        delete sg->sprites2;
        sg->sprites2 = NULL;
    }
    free(sg);
}


static inline void spritegroup_set_stop_on_last_frame(spritegroup_t* sg, bool do_stop) {
    massert(sg, "spritegroup is NULL");
    // get the current sprite
    auto s = spritegroup_get(sg, sg->current);
    massert(s, "sprite is NULL");
    // set the stop_on_last_frame flag
    s->stop_on_last_frame = do_stop;
    // lets also set stopframe on any possible shadow sprites
    // first make sure sg->current+1 does not exceed our bounds
    if (sg->current + 1 >= sg->size)
        return;
    auto shadow = spritegroup_get(sg, sg->current + 1);
    if (!shadow)
        return;
    shadow->stop_on_last_frame = do_stop;
}


static inline bool spritegroup_set_current(spritegroup_t* sg, int index) {
    minfo2("spritegroup set current");
    massert(sg, "spritegroup is NULL");
    massert(index >= 0, "index is negative: %d, %d", index, sg->size);
    massert(index < sg->size, "index is out of bounds for id %d: %d, %d", sg->id, index, sg->size);
    sg->current = index;
    // lets update the sprite's current frame to 0
    // since we prob want to start an animation at the beginning
    // if we are changing current
    sg->sprites2->at(sg->current)->currentframe = 0;
    return true;
}


static inline bool spritegroup_is_animating(spritegroup_t* sg) {
    if (!sg)
        return false;
    auto s = sg->sprites2->at(sg->current);
    if (!s)
        return false;
    return s->is_animating;
}


static inline bool spritegroup_update_dest(spritegroup_t* sg) {
    massert(sg, "spritegroup is NULL");
    bool retval = false;
    if (sg->move.x > 0) {
        sg->dest.x += sg->move_rate;
        sg->move.x -= sg->move_rate;
        retval = true;
    } else if (sg->move.x < 0) {
        sg->dest.x -= sg->move_rate;
        sg->move.x += sg->move_rate;
        retval = true;
    }
    if (sg->move.y > 0) {
        sg->dest.y += sg->move_rate;
        sg->move.y -= sg->move_rate;
        retval = true;
    } else if (sg->move.y < 0) {
        sg->dest.y -= sg->move_rate;
        sg->move.y += sg->move_rate;
        retval = true;
    }
    return retval;
}


static inline bool spritegroup_snap_dest(spritegroup_t* sg, int x, int y) {
    massert(sg, "spritegroup is NULL");
    bool retval = false;
    if (sg->move.x == 0 && sg->move.y == 0) {
        sg->dest.x = x * DEFAULT_TILE_SIZE + sg->off_x;
        sg->dest.y = y * DEFAULT_TILE_SIZE + sg->off_y;
        retval = true;
    }
    return retval;
}


static inline void sg_set_default_anim(spritegroup_t* sg, int anim) {
    massert(sg, "spritegroup is NULL");
    sg->default_anim = anim;
}
