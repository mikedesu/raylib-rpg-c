#include "libgame_defines.h"
//#include "location.h"
#include "massert.h"
#include "mprint.h"
#include "spritegroup.h"
#include <cstdlib>

using std::make_shared;

spritegroup_t* spritegroup_create(int capacity) {
    minfo("Creating spritegroup with capacity %d", capacity);

    massert(capacity > 0, "capacity must be greater than 0, got %d", capacity);

    if (capacity <= 0) return NULL;

    minfo("Allocating memory for spritegroup...");
    spritegroup_t* sg = (spritegroup_t*)malloc(sizeof(spritegroup_t));
    massert(sg, "spritegroup is NULL");
    if (!sg) {
        merror("Failed to allocate memory for spritegroup");
        return NULL;
    }

    sg->current = sg->size = sg->off_x = sg->off_y = sg->default_anim = sg->id = 0;
    sg->alpha = 255;
    sg->capacity = capacity;

    //sg->sprites = (sprite**)malloc(sizeof(sprite*) * capacity);
    //massert(sg->sprites, "spritegroup sprites is NULL");
    //if (!sg->sprites) {
    //    free(sg);
    //    return NULL;
    //}

    minfo("Initializing spritegroup sprites unordered_map...");
    sg->sprites2 = unordered_map<entityid, shared_ptr<sprite>>();
    msuccess("Initialized spritegroup sprites unordered_map");
    //shared_ptr<unordered_map<int, shared_ptr<sprite>>> sprites2;
    //massert(sg->sprites2, "spritegroup sprites2 is NULL");

    sg->dest = sg->move = (Rectangle){0, 0, 0, 0};
    //sg->specifier = SPECIFIER_NONE;
    //sg->move_rate = 1.0 / DEFAULT_ANIM_SPEED;
    sg->move_rate = 1.0;
    msuccess("spritegroup_create success");
    return sg;
}

void spritegroup_destroy(spritegroup_t* sg) {
    if (!sg) {
        return;
    }
    free(sg);
}

void spritegroup_add(spritegroup_t* const sg, shared_ptr<sprite> s) {
    if (!sg) {
        return;
    }
    if (!s) {
        return;
    }
    if (sg->size < sg->capacity) {
        //sg->sprites2[sg->size] = s;
        sg->sprites2.insert({sg->size, s});
        sg->size++;
    }
}


void spritegroup_set(spritegroup_t* const sg, int index, shared_ptr<sprite> s) {
    massert(sg, "spritegroup is NULL");
    massert(s, "sprite is NULL");
    massert(index >= 0, "index is negative");
    massert(index < sg->capacity, "index is out of bounds");

    if (sg->sprites2.find(index) != sg->sprites2.end()) {
        sg->sprites2.insert({index, s});
    }
}

// each sprite has a 'context' that corresponds to different directions
void spritegroup_setcontexts(spritegroup_t* const sg, int context) {
    massert(sg, "spritegroup is NULL");
    for (int i = 0; i < sg->size; i++) {
        if (sg->sprites2.find(i) == sg->sprites2.end()) {
            continue;
        }

        //if (!sg->sprites[i]) continue;
        if (sg->sprites2.at(i)->numcontexts <= 0) continue;
        if (context < 0) continue;
        if (context >= sg->sprites2.at(i)->numcontexts) continue;
        sprite_setcontext2(sg->sprites2.at(i), context);
    }
}

shared_ptr<sprite> spritegroup_get(spritegroup_t* const sg, int index) {
    massert(sg, "spritegroup is NULL");
    massert(index >= 0, "index is negative");
    massert(index < sg->size, "index is out of bounds");

    //return sg->sprites2[index];
    return sg->sprites2.at(index);
}

shared_ptr<sprite> sg_get_current(spritegroup_t* const sg) {
    massert(sg, "spritegroup is NULL");
    massert(sg->current >= 0, "current is negative");
    massert(sg->current < sg->size, "current is out of bounds");
    //return sg->sprites2[sg->current];
    return sg->sprites2.at(sg->current);
}

shared_ptr<sprite> sg_get_current_plus_one(spritegroup_t* const sg) {
    massert(sg, "spritegroup is NULL");
    massert(sg->current >= 0, "current is negative");
    if (sg->current + 1 >= sg->size) return NULL;
    //return sg->sprites2[sg->current + 1];
    return sg->sprites2.at(sg->current + 1);
}

bool spritegroup_set_current(spritegroup_t* const sg, int index) {
    massert(sg, "spritegroup is NULL");
    massert(index >= 0, "index is negative: %d, %d", index, sg->size);
    massert(index < sg->size, "index is out of bounds for id %d: %d, %d", sg->id, index, sg->size);
    // Debug log for death animation tracking
    //if (sg->current != index) {
    //    minfo("Animation change: %d -> %d (EntityID: %d)", sg->current, index, sg->id);
    //}
    sg->current = index;
    return true;
}

//specifier_t spritegroup_get_specifier(spritegroup_t* const sg) { return sg ? sg->specifier : SPECIFIER_NONE; }

int spritegroup_get_first_context(spritegroup_t* const sg) {
    massert(sg, "spritegroup is NULL");
    massert(sg->size > 0, "spritegroup is empty");
    massert(sg->sprites2->at(0), "sprite is NULL");
    return sprite_get_context2(sg->sprites2->at(0));
}

void spritegroup_set_stop_on_last_frame(spritegroup_t* const sg, bool do_stop) {
    massert(sg, "spritegroup is NULL");
    // get the current sprite
    shared_ptr<sprite> s = spritegroup_get(sg, sg->current);
    massert(s, "sprite is NULL");
    // set the stop_on_last_frame flag
    s->stop_on_last_frame = do_stop;
    // lets also set stopframe on any possible shadow sprites
    // first make sure sg->current+1 does not exceed our bounds
    if (sg->current + 1 >= sg->size) {
        //merrorint("spritegroup_set_stop_on_last_frame: current sprite index is out of bounds", sg->current);
        return;
    }
    shared_ptr<sprite> shadow = spritegroup_get(sg, sg->current + 1);
    if (!shadow) {
        return;
    }
    shadow->stop_on_last_frame = do_stop;
}

bool spritegroup_is_animating(spritegroup_t* const sg) {
    if (!sg) {
        merror("spritegroup is NULL");
        return false;
    }
    //shared_ptr<sprite> s = sg->sprites[sg->current];
    //shared_ptr<sprite> s = sg->sprites2[sg->current];
    shared_ptr<sprite> s = sg->sprites2.at(sg->current);
    if (!s) {
        merror("sprite is NULL");
        return false;
    }
    return s->is_animating;
}

//void spritegroup_snap_dest(spritegroup_t* const sg, int x, int y) {
//    massert(sg, "spritegroup is NULL");
//    if (sg->move.x == 0 && sg->move.y == 0) {
//        sg->dest.x = x * DEFAULT_TILE_SIZE + sg->off_x;
//        sg->dest.y = y * DEFAULT_TILE_SIZE + sg->off_y;
//    }
//}

//void spritegroup_snap_dest(spritegroup_t* const sg, vec3 loc) {
bool spritegroup_snap_dest(spritegroup_t* const sg, int x, int y) {
    massert(sg, "spritegroup is NULL");
    bool retval = false;
    if (sg->move.x == 0 && sg->move.y == 0) {
        sg->dest.x = x * DEFAULT_TILE_SIZE + sg->off_x;
        sg->dest.y = y * DEFAULT_TILE_SIZE + sg->off_y;
        retval = true;
    }
    return retval;
}


bool spritegroup_update_dest(spritegroup_t* const sg) {
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

void sg_set_default_anim(spritegroup_t* const sg, int anim) {
    massert(sg, "spritegroup is NULL");
    sg->default_anim = anim;
}
