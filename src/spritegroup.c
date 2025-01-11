#include "spritegroup.h"
#include "mprint.h"
#include <stdlib.h>




spritegroup_t* spritegroup_create(const int capacity) {
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
    //sg->move_x = sg->move_y = sg->off_x = sg->off_y =
    sg->default_anim = 0;
    //sg->prev_anim = 0;
    sg->id = 0;
    sg->capacity = capacity;
    sg->sprites = malloc(sizeof(sprite*) * capacity);
    if (!sg->sprites) {
        merror("spritegroup_create: failed to allocate memory for "
               "spritegroup_t->sprites");
        free(sg);
        return NULL;
    }
    sg->dest = (Rectangle){0, 0, 0, 0};
    sg->move = (Rectangle){0, 0, 0, 0};
    sg->specifier = SPECIFIER_NONE;
    // zero out the animation queue
    //spritegroup_clear_anim_queue(sg);
    return sg;
}




void spritegroup_set_specifier(spritegroup_t* const sg, const specifier_t spec) {
    if (!sg) {
        merror("spritegroup_set_specifier: spritegroup is NULL");
        return;
    }
    // make sure it is a valid specifier
    //sg->specifier = spec >= SPECIFIER_NONE && spec < SPECIFIER_COUNT ? spec : SPECIFIER_NONE;
    sg->specifier = spec >= SPECIFIER_NONE && spec < SPECIFIER_COUNT ? spec : SPECIFIER_NONE;
}




//void spritegroup_set_prev_anim(spritegroup_t* const sg) {
//    if (!sg) {
//        merror("spritegroup_set_prev_anim: spritegroup is NULL");
//        return;
//    }
//    sg->prev_anim = sg->current;
//}



void spritegroup_destroy(spritegroup_t* sg) {
    //minfo("destroying spritegroup_t");
    if (!sg) {
        merror("spritegroup_destroy: spritegroup is NULL");
        return;
    }
    if (sg->sprites) {
        for (int i = 0; i < sg->size; i++) {
            sprite_destroy(sg->sprites[i]);
        }
        free(sg->sprites);
    }
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
        sg->sprites[sg->size] = s;
        sg->size++;
    } else {
        mwarning("spritegroup_add: spritegroup is full");
    }
}


void spritegroup_set(spritegroup_t* const sg, const int index, sprite* s) {
    if (!sg) {
        merror("spritegroup_set: spritegroup is NULL");
        return;
    }
    if (!s) {
        merror("spritegroup_set: sprite is NULL");
        return;
    }
    if (index >= 0 && index < sg->size) {
        sg->sprites[index] = s;
    } else {
        merror("spritegroup_set: index is out of bounds");
    }
}




// each sprite has a 'context' that corresponds to different directions
void spritegroup_setcontexts(spritegroup_t* const sg, const int context) {
    if (!sg) {
        merror("spritegroup_setcontexts: spritegroup is NULL");
        return;
    }
    for (int i = 0; i < sg->size; i++) {
        sprite_setcontext(sg->sprites[i], context);
    }
}




sprite* spritegroup_get(spritegroup_t* const sg, const int index) {
    if (!sg) {
        merror("spritegroup_get: spritegroup is NULL");
        return NULL;
    }
    return index < sg->size ? sg->sprites[index] : NULL;
}




//void spritegroup_incr(spritegroup_t* sg) {
//    if (!sg) return;
//    sg->current += 2;
//    sg->current = sg->current >= sg->size ? 0 : sg->current;
//}



const bool spritegroup_set_current(spritegroup_t* const sg, const int index) {
    if (!sg) {
        merror("spritegroup_set_current: spritegroup is NULL");
        return false;
    }
    if (index >= sg->size) {
        merror("spritegroup_set_current: index is out of bounds");
        return false;
        //sg->current = sg->size - 1;
    } else if (index < 0) {
        merror("spritegroup_set_current: index is negative");
        return false;
        //sg->current = 0;
    }
    //sg->prev_anim = sg->current;
    sg->current = index;
    return true;
}



const specifier_t spritegroup_get_specifier(spritegroup_t* const sg) {
    return sg ? sg->specifier : SPECIFIER_NONE;
}




const int spritegroup_get_first_context(spritegroup_t* const sg) {
    if (!sg) {
        merror("spritegroup_get_first_context: spritegroup is NULL");
        return -1;
    }
    if (sg->size <= 0) {
        merror("spritegroup_get_first_context: spritegroup is empty");
        return -1;
    }
    return sprite_get_context(sg->sprites[0]);
}



//void spritegroup_enqueue_anim(spritegroup_t* const sg, const int anim) {
//    if (!sg) {
//        merror("spritegroup_enqueue_anim: spritegroup is NULL");
//        return;
//    }
//    if (sg->anim_queue_count < SPRITEGROUP_ANIM_QUEUE_MAX) {
//        sg->anim_queue[sg->anim_queue_count] = anim;
//        sg->anim_queue_count++;
//    } else {
//        mwarning("spritegroup_enqueue_anim: animation queue is full");
//    }
//}

//const int spritegroup_get_anim_in_queue(spritegroup_t* const sg,
//                                        const int index) {
//    if (!sg) {
//        merror("spritegroup_get_anim_in_queue: spritegroup is NULL");
//        return -1;
//    }
//
//    if (index < 0 || index >= SPRITEGROUP_ANIM_QUEUE_MAX) {
//        merror("spritegroup_get_anim_in_queue: index is out of bounds");
//        return -1;
//    }
//
//    return sg->anim_queue[index];
//}




//void spritegroup_clear_anim_queue(spritegroup_t* const sg) {
//    if (!sg) {
//        merror("spritegroup_clear_anim_queue: spritegroup is NULL");
//        return;
//    }
//    for (int i = 0; i < SPRITEGROUP_ANIM_QUEUE_MAX; i++) sg->anim_queue[i] = -1;
//    sg->anim_queue_count = 0;
//}
