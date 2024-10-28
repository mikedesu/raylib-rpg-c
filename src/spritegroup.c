#include "mprint.h"
#include "spritegroup.h"
#include <stdlib.h>

spritegroup_t* spritegroup_create(int capacity) {
    spritegroup_t* sg = malloc(sizeof(spritegroup_t));
    if (!sg) {
        return NULL;
    }
    sg->current = 0;
    sg->capacity = capacity;
    sg->size = 0;
    sg->sprites = malloc(sizeof(sprite*) * capacity);
    if (!sg->sprites) {
        free(sg);
        return NULL;
    }
    sg->dest = (Rectangle){0, 0, 0, 0};
    sg->move_x = 0;
    sg->move_y = 0;
    sg->off_x = 0;
    sg->off_y = 0;
    sg->default_anim = 0;
    sg->prev_anim = 0;
    sg->specifier = SPECIFIER_NONE;
    sg->id = 0;
    return sg;
}




void spritegroup_set_specifier(spritegroup_t* sg, specifier_t spec) {
    if (sg) {
        // make sure it is a valid specifier
        if (spec >= SPECIFIER_NONE && spec < SPECIFIER_COUNT) {
            sg->specifier = spec;
        }
    }
}




void spritegroup_set_prev_anim(spritegroup_t* sg) {
    if (sg) {
        //if (sg->current < 0) {
        //    sg->current = 0;
        //    sg->prev_anim = 0;
        //} else if (sg->current >= sg->size) {
        //    sg->current = sg->size - 1;
        //    sg->prev_anim = sg->size - 1;
        //} else {
        sg->prev_anim = sg->current;
        //}
    }
}



void spritegroup_destroy(spritegroup_t* sg) {
    minfo("destroying spritegroup_t");
    if (sg) {
        if (sg->sprites) {
            for (int i = 0; i < sg->size; i++) {
                sprite_destroy(sg->sprites[i]);
            }
            free(sg->sprites);
        }
        free(sg);
    }
    //msuccess("spritegroup_t destroyed");
}


void spritegroup_add(spritegroup_t* sg, sprite* s) {
    //minfo("adding sprite to spritegroup_t");
    if (sg && s) {
        if (sg->size < sg->capacity) {
            sg->sprites[sg->size] = s;
            sg->size++;
            //msuccess("sprite added to spritegroup_t");
        } else {
            merror("spritegroup_t is full");
        }
    } else {
        if (!sg) {
            merror("spritegroup_t is NULL");
        }

        if (!s) {
            merror("sprite is NULL");
        }
    }
}


void spritegroup_set(spritegroup_t* sg, int index, sprite* s) {
    minfo("setting sprite in spritegroup_t");
    if (sg && s) {
        if (index < sg->size) {
            sg->sprites[index] = s;
            //msuccess("sprite set in spritegroup_t");
        }
    } else {
        if (!sg) {
            merror("spritegroup_t is NULL");
        }
        if (!s) {
            merror("sprite is NULL");
        }
    }
}


// each sprite has a 'context' that corresponds to different directions
void spritegroup_setcontexts(spritegroup_t* sg, int context) {
    if (sg) {
        for (int i = 0; i < sg->size; i++) {
            sprite_setcontext(sg->sprites[i], context);
        }
    } else {
        merror("spritegroup_t is NULL");
    }
}


sprite* spritegroup_get(spritegroup_t* sg, int index) {
    sprite* s = NULL;
    if (sg) {
        if (index < sg->size) {
            s = sg->sprites[index];
        }
    } else {
        merror("spritegroup_t is NULL");
    }

    return s;
}




void spritegroup_incr(spritegroup_t* sg) {
    if (sg) {
        sg->current += 2;
        if (sg->current >= sg->size) {
            sg->current = 0;
        }
    }
}



void spritegroup_set_current(spritegroup_t* sg, const int index) {
    if (sg) {
        if (index >= sg->size) {

            sg->current = sg->size - 1;
        } else if (index < 0) {
            sg->current = 0;
        } else {
            //spritegroup_set_prev_anim(sg);
            sg->prev_anim = sg->current;
            sg->current = index;

            // update the num_loop count on the previous sprite to 0
            // this is so that the animation will start from the beginning
            // when the sprite is switched
            // also reset the current frame

            //sprite* s = sg->sprites[sg->prev_anim];
            //s->currentframe = 0;
            //s->num_loops = 0;
        }
    }
}



specifier_t spritegroup_get_specifier(spritegroup_t* sg) {
    if (sg) {
        return sg->specifier;
    }
    return SPECIFIER_NONE;
}




const int spritegroup_get_first_context(spritegroup_t* sg) {

    if (sg) {
        if (sg->size > 0) {
            return sprite_get_context(sg->sprites[0]);
        }
    }
    return 0;
}
