#include "libgame_defines.h"
#include "massert.h"
#include "mprint.h"
#include "spritegroup.h"
#include <stdlib.h>

spritegroup_t* spritegroup_create(int capacity) {
    if (capacity <= 0) return NULL;
    spritegroup_t* sg = malloc(sizeof(spritegroup_t));
    if (!sg) {
        merror("spritegroup_create: failed to allocate memory for spritegroup_t");
        return NULL;
    }
    sg->current = 0;
    sg->size = 0;
    sg->off_x = 0;
    sg->off_y = 0;
    sg->default_anim = 0;
    sg->id = 0;
    sg->capacity = capacity;
    sg->sprites = malloc(sizeof(sprite*) * capacity);
    if (!sg->sprites) {
        merror("spritegroup_create: failed to allocate memory for "
               "spritegroup_t->sprites");
        free(sg);
        return NULL;
    }
    sg->dest = sg->move = (Rectangle){0, 0, 0, 0};
    sg->specifier = SPECIFIER_NONE;
    //sg->move_rate = 1.0 / DEFAULT_ANIM_SPEED;
    sg->move_rate = 1.0;
    return sg;
}

void spritegroup_set_specifier(spritegroup_t* const sg, specifier_t spec) {
    if (!sg) {
        merror("spritegroup_set_specifier: spritegroup is NULL");
        return;
    }
    sg->specifier = (spec >= SPECIFIER_NONE && spec < SPECIFIER_COUNT) ? spec : SPECIFIER_NONE;
}

void spritegroup_destroy(spritegroup_t* sg) {
    //minfo("destroying spritegroup_t");
    if (!sg) {
        merror("spritegroup_destroy: spritegroup is NULL");
        return;
    }
    if (!sg->sprites) {
        merror("spritegroup_destroy: spritegroup->sprites is NULL");
        return;
    }
    for (int i = 0; i < sg->size; i++) sprite_destroy(sg->sprites[i]);
    free(sg->sprites);
    free(sg);
}

void spritegroup_add(spritegroup_t* const sg, sprite* s) {
    if (!sg) {
        merror("spritegroup_add: spritegroup is NULL");
        return;
    }
    if (!s) {
        merror("spritegroup_add: sprite is NULL");
        return;
    }
    if (sg->size < sg->capacity) {
        sg->sprites[sg->size++] = s;
    } else {
        mwarning("spritegroup_add: spritegroup is full");
    }
}

void spritegroup_set(spritegroup_t* const sg, int index, sprite* s) {
    if (!sg) {
        merror("spritegroup_set: spritegroup is NULL");
        return;
    }
    if (!s) {
        merror("spritegroup_set: sprite is NULL");
        return;
    }
    if (index < 0) {
        merror("spritegroup_set: index is negative");
        return;
    }
    if (index >= sg->size) {
        merror("spritegroup_set: index is out of bounds");
        return;
    }
    // only set if the sprite is not NULL and sg->sprites[index] is NULL
    if (sg->sprites[index] == NULL) { sg->sprites[index] = s; }
}

// each sprite has a 'context' that corresponds to different directions
void spritegroup_setcontexts(spritegroup_t* const sg, int context) {
    massert(sg, "spritegroup_setcontexts: spritegroup is NULL");
    //if (!sg) {
    //    merror("spritegroup_setcontexts: spritegroup is NULL");
    //    return;
    //}
    //minfo("spritegroup_setcontexts: entering loop of sg->size %d", sg->size);
    for (int i = 0; i < sg->size; i++) {
        //minfo("spritegroup_setcontexts: setting sprite %d", i);
        if (!sg->sprites[i]) {
            //merror("spritegroup_setcontexts: sprite is NULL");
            continue;
        }
        if (sg->sprites[i]->numcontexts <= 0) {
            //merror("spritegroup_setcontexts: sprite has no contexts");
            continue;
        }
        if (context < 0) {
            //merror("spritegroup_setcontexts: context is negative");
            continue;
        }
        if (context >= sg->sprites[i]->numcontexts) {
            //merror("spritegroup_setcontexts: context is out of bounds");
            continue;
        }
        sprite_setcontext(sg->sprites[i], context);
    }
}

sprite* spritegroup_get(spritegroup_t* const sg, int index) {
    massert(sg, "spritegroup_get: spritegroup is NULL");
    massert(index >= 0, "spritegroup_get: index is negative");
    //massert(index < sg->size, "spritegroup_get: index is out of bounds");
    //if (!sg) {
    //    merror("spritegroup_get: spritegroup is NULL");
    //    return NULL;
    //}
    if (index >= sg->size) {
        //merror("spritegroup_get: index is out of bounds: %d >= %d", index, sg->size);
        return NULL;
    }
    return sg->sprites[index];
}

sprite* sg_get_current(spritegroup_t* const sg) {
    massert(sg, "sg_get_current: spritegroup is NULL");
    massert(sg->current >= 0, "sg_get_current: current is negative");
    massert(sg->current < sg->size, "sg_get_current: current is out of bounds");

    return sg->sprites[sg->current];
}

sprite* sg_get_current_plus_one(spritegroup_t* const sg) {
    massert(sg, "sg_get_current_plus_one: spritegroup is NULL");
    massert(sg->current >= 0, "sg_get_current_plus_one: current is negative");
    //massert(sg->current + 1 < sg->size, "sg_get_current_plus_one: current + 1 is out of bounds");

    if (sg->current + 1 >= sg->size) {
        //merror("sg_get_current_plus_one: current + 1 is out of bounds");
        return NULL;
    }

    return sg->sprites[sg->current + 1];
}

bool spritegroup_set_current(spritegroup_t* const sg, int index) {
    massert(sg, "spritegroup_set_current: spritegroup is NULL");
    massert(index >= 0, "spritegroup_set_current: index is negative");
    massert(index < sg->size, "spritegroup_set_current: index is out of bounds");
    //if (!sg) {
    //    merror("spritegroup_set_current: spritegroup is NULL");
    //    return false;
    //} else if (index >= sg->size) {
    //    //merrorint("spritegroup_set_current: index is out of bounds", index);
    //    return false;
    //} else if (index < 0) {
    //    merror("spritegroup_set_current: index is negative");
    //    return false;
    //}
    sg->current = index;
    return true;
}

specifier_t spritegroup_get_specifier(spritegroup_t* const sg) { return sg ? sg->specifier : SPECIFIER_NONE; }

int spritegroup_get_first_context(spritegroup_t* const sg) {
    massert(sg, "spritegroup_get_first_context: spritegroup is NULL");
    massert(sg->size > 0, "spritegroup_get_first_context: spritegroup is empty");
    //if (!sg) {
    //    merror("spritegroup_get_first_context: spritegroup is NULL");
    //    return -1;
    //}
    //if (sg->size <= 0) {
    //    merror("spritegroup_get_first_context: spritegroup is empty");
    //    return -1;
    //}
    return sprite_get_context(sg->sprites[0]);
}

void spritegroup_set_stop_on_last_frame(spritegroup_t* const sg, bool do_stop) {
    massert(sg, "spritegroup is NULL");
    // get the current sprite
    sprite* s = spritegroup_get(sg, sg->current);
    massert(s, "sprite is NULL");
    //if (!s) {
    //    merror("spritegroup_set_stop_on_last_frame: sprite is NULL");
    //    return;
    //}

    // set the stop_on_last_frame flag
    s->stop_on_last_frame = do_stop;

    // lets also set stopframe on any possible shadow sprites
    // first make sure sg->current+1 does not exceed our bounds
    if (sg->current + 1 >= sg->size) {
        //merrorint("spritegroup_set_stop_on_last_frame: current sprite index is out of bounds", sg->current);
        return;
    }

    //sprite* shadow = sg->sprites[sg->current + 1];
    sprite* shadow = spritegroup_get(sg, sg->current + 1);
    if (!shadow) {
        //merrorint("spritegroup_set_stop_on_last_frame: shadow sprite is NULL", sg->current + 1);
        return;
    }
    shadow->stop_on_last_frame = do_stop;
}

bool spritegroup_is_animating(spritegroup_t* const sg) {
    if (!sg) {
        merror("spritegroup_is_animating: spritegroup is NULL");
        return false;
    }
    sprite* s = sg->sprites[sg->current];
    if (!s) {
        merror("spritegroup_is_animating: sprite is NULL");
        return false;
    }
    return s->is_animating;
}

void spritegroup_snap_dest(spritegroup_t* const sg, int x, int y) {
    if (!sg) {
        merror("spritegroup_snap_dest: spritegroup is NULL");
        return;
    }
    if (sg->move.x == 0 && sg->move.y == 0) {
        sg->dest.x = x * DEFAULT_TILE_SIZE + sg->off_x;
        sg->dest.y = y * DEFAULT_TILE_SIZE + sg->off_y;
    }
}

void spritegroup_update_dest(spritegroup_t* const sg) {
    if (!sg) {
        //merror("spritegroup_update_dest: spritegroup is NULL");
        return;
    }
    if (sg->move.x > 0) {
        //sg->dest.x++;
        //sg->move.x--;
        sg->dest.x += sg->move_rate;
        sg->move.x -= sg->move_rate;
    } else if (sg->move.x < 0) {
        //sg->dest.x--;
        //sg->move.x++;
        sg->dest.x -= sg->move_rate;
        sg->move.x += sg->move_rate;
    }
    if (sg->move.y > 0) {
        //sg->dest.y++;
        //sg->move.y--;
        sg->dest.y += sg->move_rate;
        sg->move.y -= sg->move_rate;
    } else if (sg->move.y < 0) {
        //sg->dest.y--;
        //sg->move.y++;
        sg->dest.y -= sg->move_rate;
        sg->move.y += sg->move_rate;
    }
}
