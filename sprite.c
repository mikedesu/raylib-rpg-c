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

    const int w = t->width / numframes;
    const int h = t->height / numcontexts;

    s->width = w;
    s->height = h;

    // setting the source of the texture is about which frame and context we are on
    // context is the y access aka which row of sprites
    // frame is the x access aka which column of sprites

    // to calculate the source rectangle we need to know the width and height of the sprite
    // given the current frame and current context,
    // the width is the current frame times the width of the sprite

    s->src = (Rectangle){s->currentframe * w, s->currentcontext * h, w, h};

    // this will force us to set the destination rectangle after instancing
    s->dest = (Rectangle){0, 0, 0, 0};

    return s;
}


void sprite_incrframe(sprite* s) {
    if (s) {
        s->currentframe = (s->currentframe + 1) % s->numframes;
        s->src.x = s->width * s->currentframe;
    }
}


void sprite_setcontext(sprite* s, int context) {
    if (s) {
        if (context < 0) {
            context = 0;
        } else if (context >= s->numcontexts) {
            context = s->numcontexts - 1;
        }

        s->currentcontext = context % s->numcontexts;
        s->src.y = s->height * s->currentcontext;
    }
}


void sprite_incrcontext(sprite* s) {
    if (s) {
        s->currentcontext = (s->currentcontext + 1) % s->numcontexts;
        s->src.y = s->height * s->currentcontext;
    }
}


void sprite_updatesrc(sprite* s) {
    if (s) {
        s->src.x = s->width * s->currentframe;
        s->src.y = s->height * s->currentcontext;
    }
}


sprite* sprite_destroy(sprite* s) {
    if (s) {
        // we do not unload the texture here because textures are stored and managed globally
        s->texture = NULL;
        free(s);
    }
    return NULL;
}
