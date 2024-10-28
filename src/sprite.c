#include "mprint.h"
#include "sprite.h"

#include <stdlib.h>

sprite* sprite_create(Texture2D* t, int numcontexts, int numframes) {
    if (!t) {
        merror("sprite_create failed: texture is null");
        return NULL;
    }
    sprite* s = malloc(sizeof(sprite));
    if (!s) {
        merror("sprite_create failed: could not allocate memory for sprite");
        return NULL;
    }
    s->numframes = numframes;
    s->numcontexts = numcontexts;
    s->currentframe = 0;
    s->currentcontext = 0;
    s->num_loops = 0;
    s->texture = t;
    s->width = t->width / numframes;
    s->height = t->height / numcontexts;
    // setting the source of the texture is about which frame and context we are on
    // context is the y access aka which row of sprites
    // frame is the x access aka which column of sprites
    // to calculate the source rectangle we need to know the width and height of the sprite
    // given the current frame and current context,
    // the width is the current frame times the width of the sprite
    s->src = (Rectangle){s->currentframe * s->width, s->currentcontext * s->height, s->width, s->height};
    // this will force us to set the destination rectangle after instancing
    s->dest = (Rectangle){0, 0, 0, 0};
    return s;
}


void sprite_incrframe(sprite* s) {
    if (s) {
        s->currentframe = (s->currentframe + 1) % s->numframes;
        s->src.x = s->width * s->currentframe;
        if (s->currentframe == 0) {
            s->num_loops++;
        }
    }
}


void sprite_setcontext(sprite* s, int context) {
    if (s) {
        if (context < 0) {
            context = 0;
            // also set currentframe to 0 to reset animation frame
        } else if (context >= s->numcontexts) {
            context = s->numcontexts - 1;
        }
        s->currentcontext = context % s->numcontexts;
        s->src.y = s->height * s->currentcontext;
        s->currentframe = 0;
        s->src.x = 0;
        //msuccess("sprite_setcontext success");
    } else {
        merror("sprite_setcontext failed: sprite is null");
    }
}


void sprite_incrcontext(sprite* s) {
    if (s) {
        s->currentcontext = (s->currentcontext + 1) % s->numcontexts;
        s->src.y = s->height * s->currentcontext;
    } else {
        merror("sprite_incrcontext failed: sprite is null");
    }
}


void sprite_updatesrc(sprite* s) {
    if (s) {
        s->src.x = s->width * s->currentframe;
        s->src.y = s->height * s->currentcontext;
    } else {
        merror("sprite_updatesrc failed: sprite is null");
    }
}


sprite* sprite_destroy(sprite* s) {
    if (s) {
        // we do not unload the texture here because textures are stored and managed globally
        s->texture = NULL;
        free(s);
        s = NULL;
        msuccess("sprite_destroy success");
    } else {
        merror("sprite_destroy failed: sprite is null");
    }
    return NULL;
}



const int sprite_get_context(sprite* s) {

    if (s) {
        return s->currentcontext;
    } else {
        merror("sprite_get_context failed: sprite is null");
    }
    return -1;
}
