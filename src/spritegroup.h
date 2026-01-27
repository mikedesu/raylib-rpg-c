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
    int current; // the current animation sprite in the spritegroup
    int off_x;
    int off_y;
    int default_anim; // the default animation to return to after the timer expires
    int alpha;
    entityid id; // the id of the entity that this spritegroup belongs to
    Rectangle dest;
    Rectangle move;
    vector<shared_ptr<sprite>>* sprites2;
    float move_rate;
    bool visible;

    spritegroup(int cap) {
        massert(cap > 0, "cap must be greater than 0, got %d", cap);
        current = 0;
        capacity = cap;
        size = 0;
        off_x = 0;
        off_y = 0;
        default_anim = 0;
        alpha = 255;
        id = 0;
        sprites2 = new vector<shared_ptr<sprite>>(cap);
        dest = move = Rectangle{0, 0, 0, 0};
        move_rate = 1.0;
        visible = true;
    }

    ~spritegroup() {
        if (sprites2) {
            sprites2->clear();
            delete sprites2;
            sprites2 = NULL;
        }
    }

    shared_ptr<sprite> get(int index) {
        return sprites2->at(index);
    }

    shared_ptr<sprite> get_current() {
        return sprites2->at(current);
    }

    void add(shared_ptr<sprite> s) {
        massert(s, "s is null");
        massert(size < capacity, "size %d is >= capacity %d", size, capacity);

        //if (!s || size >= capacity) {
        //    return;
        //}
        sprites2->push_back(s);
        size++;
    }

    // each sprite has a 'context' that corresponds to different directions
    void setcontexts(int context) {
        for (int i = 0; i < size; i++) {
            auto s = sprites2->at(i);
            if (!s || s->get_numcontexts() <= 0 || context < 0 || context >= s->get_numcontexts())
                continue;
            s->set_context(context);
        }
    }

    void set_stop_on_last_frame(bool do_stop) {
        auto s = get_current();
        massert(s, "sprite is NULL");
        s->set_stop_on_last_frame(do_stop);
    }

    void set_default_anim(int anim) {
        default_anim = anim;
    }

    bool snap_dest(int x, int y) {
        if (move.x != 0 || move.y != 0)
            return false;
        dest.x = x * DEFAULT_TILE_SIZE + off_x;
        dest.y = y * DEFAULT_TILE_SIZE + off_y;
        return true;
    }

    bool set_current(int index) {
        minfo2("spritegroup set current");
        //massert(sg, "spritegroup is NULL");
        massert(index >= 0, "index is negative: %d, %d", index, size);
        massert(index < size, "index is out of bounds for id %d: %d, %d", id, index, size);
        current = index;
        // lets update the sprite's current frame to 0
        // since we prob want to start an animation at the beginning
        // if we are changing current
        sprites2->at(current)->set_currentframe(0);
        // we might prob wanna reset numloops as well
        sprites2->at(current)->set_num_loops(0);
        return true;
    }

    bool update_dest() {
        bool retval = false;
        if (move.x > 0) {
            dest.x += move_rate;
            move.x -= move_rate;
            retval = true;
        } else if (move.x < 0) {
            dest.x -= move_rate;
            move.x += move_rate;
            retval = true;
        }
        if (move.y > 0) {
            dest.y += move_rate;
            move.y -= move_rate;
            retval = true;
        } else if (move.y < 0) {
            dest.y -= move_rate;
            move.y += move_rate;
            retval = true;
        }
        return retval;
    }
};
