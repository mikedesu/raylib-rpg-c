#pragma once

#include "massert.h"
#include "mprint.h"
#include <memory>
#include <raylib.h>


using std::make_shared;
using std::shared_ptr;


class sprite {
private:
    int width;
    int height;

public:
    sprite() {
    }


    sprite(Texture* t, int nc, int nf)
        : texture(t)
        , numcontexts(nc)
        , numframes(nf) {
        massert(t, "texture t was null");
        currentframe = 0;
        currentcontext = 0;
        num_loops = 0;
        width = t->width / numframes;
        height = t->height / numcontexts;
        stop_on_last_frame = false;
        is_animating = true;
        // setting the source of the texture is about which frame and context we are on
        // context is the y access aka which row of sprites
        // frame is the x access aka which column of sprites
        // to calculate the source rectangle we need to know the width and height of the sprite
        // given the current frame and current context,
        // the width is the current frame times the width of the sprite
        float x = currentframe * width;
        float y = currentcontext * height;
        float w = width;
        float h = height;

        src = Rectangle{x, y, w, h};
        dest = Rectangle{0, 0, 0, 0};
    }

    ~sprite() {
    }


    void incr_frame() {
        if (!is_animating)
            return;
        currentframe++;
        if (stop_on_last_frame && currentframe == numframes - 1) {
            is_animating = false;
            src.x = width * currentframe;
            return;
        }
        if (currentframe >= numframes) {
            currentframe = 0;
            num_loops++;
        }
        src.x = width * currentframe;
    }


    int get_width() {
        return width;
    }
    int get_height() {
        return height;
    }
    void set_width(int w) {
        width = w;
    }
    void set_height(int h) {
        height = h;
    }


    Texture2D* texture;
    Rectangle src;
    Rectangle dest;
    int numcontexts;
    int numframes;
    int currentframe;
    int currentcontext;
    int num_loops;
    bool stop_on_last_frame;
    bool is_animating;
};


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
    s->src.y = s->get_height() * s->currentcontext;
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
