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
    int numcontexts;
    int numframes;
    int currentframe;
    int currentcontext;
    int num_loops;


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


    int get_numcontexts() {
        return numcontexts;
    }
    int get_numframes() {
        return numframes;
    }
    int get_currentframe() {
        return currentcontext;
    }
    int get_currentcontext() {
        return currentframe;
    }
    int get_num_loops() {
        return num_loops;
    }

    void set_currentcontext(int n) {
        currentcontext = n;
    }

    void set_currentframe(int n) {
        currentframe = n;
    }

    void set_num_loops(int n) {
        num_loops = n;
    }


    Texture2D* texture;
    Rectangle src;
    Rectangle dest;

    bool stop_on_last_frame;
    bool is_animating;
};


static inline void sprite_setcontext2(shared_ptr<sprite> s, int ctx) {
    massert(s, "sprite_setcontext2: sprite is NULL");
    massert(ctx >= 0, "sprite_setcontext2: context is less than 0: %d", ctx);
    const int nc = s->get_numcontexts();
    massert(ctx < nc, "sprite_setcontext2: context is greater than numcontexts: %d < %d", ctx, nc);
    if (ctx < 0) {
        merror("sprite_setcontext2: context is less than 0");
        return;
    }
    if (ctx >= s->get_numcontexts()) {
        merror("sprite_setcontext2: context is greater than numcontexts: %d < %d", ctx, s->get_numcontexts());
        return;
    }
    s->set_currentcontext(ctx % s->get_numcontexts());
    s->src.y = s->get_height() * s->get_currentcontext();
    s->set_currentframe(0);
    s->src.x = 0;
}


static inline void sprite_set_is_animating2(shared_ptr<sprite> s, bool is_animating) {
    massert(s, "sprite_set_is_animating: sprite is NULL");
    s->is_animating = is_animating;
}


static inline int sprite_get_context2(shared_ptr<sprite> s) {
    massert(s, "sprite_get_context: sprite is NULL");
    return s->get_currentcontext();
}


static inline bool sprite_is_animating2(shared_ptr<sprite> s) {
    massert(s, "sprite_is_animating: sprite is NULL");
    return s->is_animating;
}
