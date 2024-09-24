#include "mprint.h"
#include "spritegroup.h"
#include <stdlib.h>

spritegroup_t* spritegroup_create(int capacity) {
    minfo("creating spritegroup_t");
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

    sg->move = (Vector2){0, 0};

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
}


void spritegroup_add(spritegroup_t* sg, sprite* s) {
    minfo("adding sprite to spritegroup_t");
    if (sg && s) {
        if (sg->size < sg->capacity) {
            sg->sprites[sg->size] = s;
            sg->size++;
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
