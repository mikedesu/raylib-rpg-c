#include "libgame_defines.h"
//#include "location.h"
#include "massert.h"
#include "mprint.h"
#include "spritegroup.h"
#include <stdlib.h>

spritegroup_t* spritegroup_create(int capacity) {
    if (capacity <= 0) return NULL;
    spritegroup_t* sg = malloc(sizeof(spritegroup_t));
    if (!sg) {
        merror("failed to allocate memory for spritegroup_t");
        return NULL;
    }
    sg->current = sg->size = sg->off_x = sg->off_y = sg->default_anim = sg->id = 0;
    sg->alpha = 255;
    sg->capacity = capacity;
    sg->sprites = malloc(sizeof(sprite*) * capacity);
    if (!sg->sprites) {
        merror("failed to allocate memory for spritegroup_t->sprites");
        free(sg);
        return NULL;
    }
    sg->dest = sg->move = (Rectangle){0, 0, 0, 0};
    sg->specifier = SPECIFIER_NONE;
    //sg->move_rate = 1.0 / DEFAULT_ANIM_SPEED;
    sg->move_rate = 1.0;
    //sg->move_rate = DEFAULT_TILE_SIZE;
    return sg;
}

void spritegroup_set_specifier(spritegroup_t* const sg, specifier_t spec) {
    if (!sg) {
        merror("spritegroup is NULL");
        return;
    }
    sg->specifier = (spec >= SPECIFIER_NONE && spec < SPECIFIER_COUNT) ? spec : SPECIFIER_NONE;
}

void spritegroup_destroy(spritegroup_t* sg) {
    //minfo("destroying spritegroup_t");
    if (!sg) {
        merror("spritegroup is NULL");
        return;
    }
    if (!sg->sprites) {
        merror("spritegroup->sprites is NULL");
        return;
    }
    for (int i = 0; i < sg->size; i++) sprite_destroy(sg->sprites[i]);
    free(sg->sprites);
    free(sg);
}

void spritegroup_add(spritegroup_t* const sg, sprite* s) {
    if (!sg) {
        merror("spritegroup is NULL");
        return;
    }
    if (!s) {
        merror("sprite is NULL");
        return;
    }
    if (sg->size < sg->capacity) {
        sg->sprites[sg->size++] = s;
    } else {
        mwarning("spritegroup is full");
    }
}

void spritegroup_set(spritegroup_t* const sg, int index, sprite* s) {
    massert(sg, "spritegroup is NULL");
    massert(s, "sprite is NULL");
    massert(index >= 0, "index is negative");
    massert(index < sg->capacity, "index is out of bounds");
    // only set if the sprite is not NULL and sg->sprites[index] is NULL
    if (sg->sprites[index] == NULL) sg->sprites[index] = s;
}

// each sprite has a 'context' that corresponds to different directions
void spritegroup_setcontexts(spritegroup_t* const sg, int context) {
    massert(sg, "spritegroup is NULL");
    for (int i = 0; i < sg->size; i++) {
        if (!sg->sprites[i]) {
            continue;
        }
        if (sg->sprites[i]->numcontexts <= 0) {
            continue;
        }
        if (context < 0) {
            continue;
        }
        if (context >= sg->sprites[i]->numcontexts) {
            continue;
        }
        sprite_setcontext(sg->sprites[i], context);
    }
}

sprite* spritegroup_get(spritegroup_t* const sg, int index) {
    massert(sg, "spritegroup is NULL");
    massert(index >= 0, "index is negative");
    massert(index < sg->size, "index is out of bounds");
    return sg->sprites[index];
}

sprite* sg_get_current(spritegroup_t* const sg) {
    massert(sg, "spritegroup is NULL");
    massert(sg->current >= 0, "current is negative");
    massert(sg->current < sg->size, "current is out of bounds");
    return sg->sprites[sg->current];
}

sprite* sg_get_current_plus_one(spritegroup_t* const sg) {
    massert(sg, "spritegroup is NULL");
    massert(sg->current >= 0, "current is negative");
    if (sg->current + 1 >= sg->size) {
        return NULL;
    }
    return sg->sprites[sg->current + 1];
}

bool spritegroup_set_current(spritegroup_t* const sg, int index) {
    massert(sg, "spritegroup is NULL");
    massert(index >= 0, "index is negative: %d, %d", index, sg->size);
    massert(index < sg->size, "index is out of bounds for id %d: %d, %d", sg->id, index, sg->size);

    // Debug log for death animation tracking
    if (sg->current != index) minfo("Animation change: %d -> %d (EntityID: %d)", sg->current, index, sg->id);

    sg->current = index;
    return true;
}

specifier_t spritegroup_get_specifier(spritegroup_t* const sg) { return sg ? sg->specifier : SPECIFIER_NONE; }

int spritegroup_get_first_context(spritegroup_t* const sg) {
    massert(sg, "spritegroup is NULL");
    massert(sg->size > 0, "spritegroup is empty");
    massert(sg->sprites[0], "sprite is NULL");
    return sprite_get_context(sg->sprites[0]);
}

void spritegroup_set_stop_on_last_frame(spritegroup_t* const sg, bool do_stop) {
    massert(sg, "spritegroup is NULL");
    // get the current sprite
    sprite* s = spritegroup_get(sg, sg->current);
    massert(s, "sprite is NULL");
    // set the stop_on_last_frame flag
    s->stop_on_last_frame = do_stop;
    // lets also set stopframe on any possible shadow sprites
    // first make sure sg->current+1 does not exceed our bounds
    if (sg->current + 1 >= sg->size) {
        //merrorint("spritegroup_set_stop_on_last_frame: current sprite index is out of bounds", sg->current);
        return;
    }
    sprite* shadow = spritegroup_get(sg, sg->current + 1);
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
    sprite* s = sg->sprites[sg->current];
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

void spritegroup_snap_dest(spritegroup_t* const sg, loc_t loc) {
    massert(sg, "spritegroup is NULL");
    if (sg->move.x == 0 && sg->move.y == 0) {
        sg->dest.x = loc.x * DEFAULT_TILE_SIZE + sg->off_x;
        sg->dest.y = loc.y * DEFAULT_TILE_SIZE + sg->off_y;
    }
}

void spritegroup_update_dest(spritegroup_t* const sg) {
    massert(sg, "spritegroup is NULL");
    if (sg->move.x > 0) {
        sg->dest.x += sg->move_rate;
        sg->move.x -= sg->move_rate;
    } else if (sg->move.x < 0) {
        sg->dest.x -= sg->move_rate;
        sg->move.x += sg->move_rate;
    }
    if (sg->move.y > 0) {
        sg->dest.y += sg->move_rate;
        sg->move.y -= sg->move_rate;
    } else if (sg->move.y < 0) {
        sg->dest.y -= sg->move_rate;
        sg->move.y += sg->move_rate;
    }
}

void sg_set_default_anim(spritegroup_t* const sg, int anim) {
    massert(sg, "spritegroup is NULL");
    sg->default_anim = anim;
}
