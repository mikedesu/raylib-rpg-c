#include "mprint.h"
#include "spritegroup.h"
#include <stdlib.h>




spritegroup_t* spritegroup_create(const int capacity) {
    if (capacity <= 0) return NULL;
    spritegroup_t* sg = malloc(sizeof(spritegroup_t));
    if (!sg) return NULL;
    sg->current = sg->size = sg->move_x = sg->move_y = sg->off_x = sg->off_y = sg->default_anim = sg->prev_anim = sg->id = 0;
    sg->capacity = capacity;
    sg->sprites = malloc(sizeof(sprite*) * capacity);
    if (!sg->sprites) {
        free(sg);
        return NULL;
    }
    sg->dest = (Rectangle){0, 0, 0, 0};
    sg->specifier = SPECIFIER_NONE;
    return sg;
}




void spritegroup_set_specifier(spritegroup_t* const sg, const specifier_t spec) {
    if (!sg) return;
    // make sure it is a valid specifier
    sg->specifier = spec >= SPECIFIER_NONE && spec < SPECIFIER_COUNT ? spec : SPECIFIER_NONE;
}




void spritegroup_set_prev_anim(spritegroup_t* const sg) {
    if (!sg) return;
    sg->prev_anim = sg->current;
}



void spritegroup_destroy(spritegroup_t* sg) {
    minfo("destroying spritegroup_t");
    if (!sg) return;
    if (sg->sprites) {
        for (int i = 0; i < sg->size; i++) { sprite_destroy(sg->sprites[i]); }
        free(sg->sprites);
    }
    free(sg);
}




void spritegroup_add(spritegroup_t* const sg, sprite* s) {
    if (!sg || !s) return;
    if (sg->size < sg->capacity) {
        sg->sprites[sg->size] = s;
        sg->size++;
    }
}


void spritegroup_set(spritegroup_t* const sg, const int index, sprite* s) {
    if (!sg || !s) return;
    if (index >= 0 && index < sg->size) sg->sprites[index] = s;
}




// each sprite has a 'context' that corresponds to different directions
void spritegroup_setcontexts(spritegroup_t* const sg, const int context) {
    if (!sg) return;
    for (int i = 0; i < sg->size; i++) sprite_setcontext(sg->sprites[i], context);
}




sprite* spritegroup_get(spritegroup_t* const sg, const int index) {
    if (!sg) return NULL;
    return index < sg->size ? sg->sprites[index] : NULL;
}




//void spritegroup_incr(spritegroup_t* sg) {
//    if (!sg) return;
//    sg->current += 2;
//    sg->current = sg->current >= sg->size ? 0 : sg->current;
//}



void spritegroup_set_current(spritegroup_t* const sg, const int index) {
    if (!sg) return;
    if (index >= sg->size) {
        sg->current = sg->size - 1;
    } else if (index < 0) {
        sg->current = 0;
    } else {
        sg->prev_anim = sg->current;
        sg->current = index;
    }
}



const specifier_t spritegroup_get_specifier(spritegroup_t* const sg) { return sg ? sg->specifier : SPECIFIER_NONE; }




const int spritegroup_get_first_context(spritegroup_t* const sg) {
    if (!sg || sg->size <= 0) return 0;
    return sprite_get_context(sg->sprites[0]);
}
