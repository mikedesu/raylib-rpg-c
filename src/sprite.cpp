#include "massert.h"
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
    s->stop_on_last_frame = false;
    s->is_animating = true;
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
    massert(s, "sprite_incrframe: sprite is NULL");
    //if (!s->is_animating) return;
    if (!s->is_animating) {
        //minfo("Frame increment blocked - animation stopped");
        return;
    }

    //if (s->stop_on_last_frame && s->currentframe == s->numframes - 1) {
    //    s->is_animating = false;
    //    return;
    //}

    if (s->stop_on_last_frame && s->currentframe == s->numframes - 1) {
        //minfo("Reached last frame of stopped animation (Frame %d)", s->currentframe);
        s->is_animating = false;
        return;
    }

    s->currentframe++;
    if (s->currentframe >= s->numframes) s->currentframe = 0;
    s->src.x = s->width * s->currentframe;
    if (s->currentframe == 0) s->num_loops++;
}

void sprite_setcontext(sprite* const s, int context) {
    massert(s, "sprite_setcontext: sprite is NULL");
    massert(context >= 0, "sprite_setcontext: context is less than 0: %d", context);
    massert(context < s->numcontexts, "sprite_setcontext: context is greater than numcontexts: %d < %d", context, s->numcontexts);

    if (context < 0) {
        merror("sprite_setcontext: context is less than 0");
        return;
    }

    if (context >= s->numcontexts) {
        merror("sprite_setcontext: context is greater than numcontexts: %d < %d", context, s->numcontexts);
        return;
    }

    s->currentcontext = context % s->numcontexts;
    s->src.y = s->height * s->currentcontext;
    s->currentframe = 0;
    s->src.x = 0;
}

//void sprite_incrcontext(sprite* const s) {
//    massert(s, "sprite_incrcontext: sprite is NULL");
//    s->currentcontext = (s->currentcontext + 1) % s->numcontexts;
//    s->src.y = s->height * s->currentcontext;
//}

//void sprite_updatesrc(sprite* const s) {
//    massert(s, "sprite_updatesrc: sprite is NULL");
//    s->src.x = s->width * s->currentframe;
//}

void sprite_destroy(sprite* s) {
    massert(s, "sprite_destroy: sprite is NULL");
    s->texture = NULL;
    free(s);
    s = NULL;
    msuccess("sprite_destroy success");
}

int sprite_get_context(const sprite* const s) {
    massert(s, "sprite_get_context: sprite is NULL");
    return s->currentcontext;
}

void sprite_set_is_animating(sprite* const s, bool is_animating) {
    massert(s, "sprite_set_is_animating: sprite is NULL");
    s->is_animating = is_animating;
}

bool sprite_is_animating(const sprite* const s) {
    massert(s, "sprite_is_animating: sprite is NULL");
    return s->is_animating;
}
