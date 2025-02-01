#include "sprite.h"
#include "mprint.h"

#include <stdlib.h>

sprite* sprite_create(Texture2D* t, const int numcontexts, const int numframes) {
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
    s->dest = (Rectangle){0, 0, 0, 0};
    return s;
}




void sprite_incrframe(sprite* const s) {
    if (!s) return;
    s->currentframe = (s->currentframe + 1) % s->numframes;
    s->src.x = s->width * s->currentframe;
    if (s->currentframe == 0) s->num_loops++;
    //s->num_loops = s->num_loops + (s->currentframe == 0 ? 1 : 0);
}




void sprite_setcontext(sprite* const s, const int context) {
    if (!s) return;
    if (context < 0 || context >= s->numcontexts) return;
    // also set currentframe to 0 to reset animation frame
    s->currentcontext = context % s->numcontexts;
    s->src.y = s->height * s->currentcontext;
    s->currentframe = 0;
    s->src.x = 0;
}




void sprite_incrcontext(sprite* const s) {
    if (!s) return;
    s->currentcontext = (s->currentcontext + 1) % s->numcontexts;
    s->src.y = s->height * s->currentcontext;
}




void sprite_updatesrc(sprite* const s) {
    if (!s) return;
    s->src.x = s->width * s->currentframe;
    s->src.y = s->height * s->currentcontext;
}




void sprite_destroy(sprite* s) {
    if (!s) return;
    s->texture = NULL;
    free(s);
    s = NULL;
    msuccess("sprite_destroy success");
}



const int sprite_get_context(const sprite* const s) {
    return !s ? -1 : s->currentcontext;
}
