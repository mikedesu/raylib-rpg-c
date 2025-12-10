#pragma once

#include "massert.h"
#include "mprint.h"
#include <memory>
#include <raylib.h>

using std::make_shared;
using std::shared_ptr;

typedef struct sprite {
    Texture2D* texture;
    Rectangle src;
    Rectangle dest;
    int width;
    int height;
    int numcontexts;
    int numframes;
    int currentframe;
    int currentcontext;
    int num_loops;

    bool stop_on_last_frame;
    bool is_animating;
} sprite;

static inline shared_ptr<sprite> sprite_create2(Texture2D* t, int numcontexts, int numframes) {
    if (!t) {
        merror("sprite_create failed: texture is null");
        return NULL;
    }
    shared_ptr<sprite> s = make_shared<sprite>();
    s->numframes = numframes;
    s->numcontexts = numcontexts;
    s->currentframe = s->currentcontext = s->num_loops = 0;
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
    s->src = (Rectangle){(float)(s->currentframe * s->width), (float)(s->currentcontext * s->height), (float)s->width, (float)s->height};
    s->dest = (Rectangle){0, 0, 0, 0};
    return s;
}


static inline void sprite_incrframe2(shared_ptr<sprite> s) {
    massert(s, "sprite_incrframe: sprite is NULL");
    if (!s->is_animating) {
        return;
    }

    if (s->stop_on_last_frame && s->currentframe == s->numframes - 1) {
        s->is_animating = false;
        return;
    }

    s->currentframe++;
    if (s->currentframe >= s->numframes) {
        s->currentframe = 0;
    }

    s->src.x = s->width * s->currentframe;
    if (s->currentframe == 0) {
        s->num_loops++;
    }

    //if (s->num_loops >= 1) {
    //    sg->current = sg->default_anim;
    //    s->num_loops = 0;
    //}
}


static inline void sprite_setcontext2(shared_ptr<sprite> s, int context) {
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
    s->currentframe = s->src.x = 0;
}


static inline void sprite_set_is_animating2(shared_ptr<sprite> s, bool is_animating) {
    massert(s, "sprite_set_is_animating: sprite is NULL");
    s->is_animating = is_animating;
}


static inline int sprite_get_context2(shared_ptr<sprite> s) {
    massert(s, "sprite_get_context: sprite is NULL");
    return s->currentcontext;
}


static inline bool sprite_is_animating2(shared_ptr<sprite> s) {
    massert(s, "sprite_is_animating: sprite is NULL");
    return s->is_animating;
}
