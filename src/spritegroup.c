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
    return sg;
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
            sg->current = index;
        }
    }
}
