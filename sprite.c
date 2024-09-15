#include "sprite.h"

#include <stdlib.h>

sprite* sprite_create(Texture2D* t, int numcontexts, int numframes) {
    sprite* s = malloc(sizeof(sprite));
    if (!s)
        return NULL;

    s->numframes = numframes;
    s->numcontexts = numcontexts;

    s->currentframe = 0;
    s->currentcontext = 0;

    s->texture = t;

    return s;
}


sprite* sprite_destroy(sprite* s) {
    if (s) {
        s->texture = NULL;
        free(s);
    }
    return NULL;
}
