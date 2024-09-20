#include "mprint.h"
#include "spritegroup.h"
#include <stdlib.h>

spritegroup* spritegroup_create(int capacity) {
    mprint("creating spritegroup\n");
    spritegroup* sg = malloc(sizeof(spritegroup));
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

    return sg;
}


void spritegroup_destroy(spritegroup* sg) {
    mprint("destroying spritegroup\n");
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


void spritegroup_add(spritegroup* sg, sprite* s) {
    mprint("adding sprite to spritegroup\n");
    if (sg && s) {
        if (sg->size < sg->capacity) {
            sg->sprites[sg->size] = s;
            sg->size++;
        } else {
            merror("spritegroup is full\n");
        }
    } else {
        if (!sg) {
            merror("spritegroup is NULL\n");
        }

        if (!s) {
            merror("sprite is NULL\n");
        }
    }
}


void spritegroup_set(spritegroup* sg, int index, sprite* s) {
    mprint("setting sprite in spritegroup\n");
    if (sg && s) {
        if (index < sg->size) {
            sg->sprites[index] = s;
        }
    } else {
        if (!sg) {
            merror("spritegroup is NULL\n");
        }

        if (!s) {
            merror("sprite is NULL\n");
        }
    }
}


void spritegroup_setcontexts(spritegroup* sg, int context) {

    if (sg) {
        for (int i = 0; i < sg->size; i++) {
            sprite_setcontext(sg->sprites[i], context);
        }
    } else {
        merror("spritegroup is NULL\n");
    }
}
