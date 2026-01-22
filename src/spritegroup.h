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

class spritegroup {
public:
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
    bool visible;



    spritegroup(int cap) {
        massert(cap > 0, "cap must be greater than 0, got %d", cap);
        current = size = off_x = off_y = default_anim = id = 0;
        alpha = 255;
        sprites2 = new vector<shared_ptr<sprite>>();
        dest = move = Rectangle{0, 0, 0, 0};
        move_rate = 1.0;
        visible = true;
    }

    shared_ptr<sprite> get(int index) {return sprites2->at(index);}
    shared_ptr<sprite> get_current() {return sprites2->at(current);}

    void add(shared_ptr<sprite> s) {
        if (!s || size >= capacity)
            return;
        sprites2->push_back(s);
        size++;
    }

    // each sprite has a 'context' that corresponds to different directions
    void setcontexts(int context) {
        for (int i = 0; i < size; i++) {
            auto s = sprites2->at(i);
            if (!s || s->get_numcontexts() <= 0 || context < 0 || context >= s->get_numcontexts()) continue;
            s->set_context(context);
        }
    }
};



static inline void spritegroup_destroy(spritegroup* sg) {
    if (!sg)
        return;
    if (sg->sprites2) {
        sg->sprites2->clear();
        delete sg->sprites2;
        sg->sprites2 = NULL;
    }
    free(sg);
}

static inline void spritegroup_set_stop_on_last_frame(spritegroup* sg, bool do_stop) {
    massert(sg, "spritegroup is NULL");
    // get the current sprite
    auto s = sg->get(sg->current);
    massert(s, "sprite is NULL");
    s->set_stop_on_last_frame(do_stop);
    // lets also set stopframe on any possible shadow sprites
    // first make sure sg->current+1 does not exceed our bounds
    if (sg->current + 1 >= sg->size) return;
    auto shadow = sg->get(sg->current + 1);
    if (!shadow) return;
    shadow->set_stop_on_last_frame(do_stop);
}

static inline bool spritegroup_set_current(spritegroup* sg, int index) {
    minfo2("spritegroup set current");
    massert(sg, "spritegroup is NULL");
    massert(index >= 0, "index is negative: %d, %d", index, sg->size);
    massert(index < sg->size, "index is out of bounds for id %d: %d, %d", sg->id, index, sg->size);
    sg->current = index;
    // lets update the sprite's current frame to 0
    // since we prob want to start an animation at the beginning
    // if we are changing current
    sg->sprites2->at(sg->current)->set_currentframe(0);
    // we might prob wanna reset numloops as well
    sg->sprites2->at(sg->current)->set_num_loops(0);
    return true;
}

static inline bool spritegroup_is_animating(spritegroup* sg) {
    if (!sg || !sg->sprites2->at(sg->current))
        return false;
    return sg->sprites2->at(sg->current)->get_is_animating();
}

static inline bool spritegroup_update_dest(spritegroup* sg) {
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

static inline bool spritegroup_snap_dest(spritegroup* sg, int x, int y) {
    massert(sg, "spritegroup is NULL");
    bool retval = false;
    if (sg->move.x == 0 && sg->move.y == 0) {
        sg->dest.x = x * DEFAULT_TILE_SIZE + sg->off_x;
        sg->dest.y = y * DEFAULT_TILE_SIZE + sg->off_y;
        retval = true;
    }
    return retval;
}

static inline void sg_set_default_anim(spritegroup* sg, int anim) {
    massert(sg, "spritegroup is NULL");
    sg->default_anim = anim;
}
