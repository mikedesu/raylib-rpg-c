#pragma once

#include <memory>
#include <raylib.h>

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

shared_ptr<sprite> sprite_create2(Texture2D* t, int numcontexts, int numframes);

void sprite_incrframe2(shared_ptr<sprite> s);
void sprite_setcontext2(shared_ptr<sprite> s, int context);
void sprite_set_is_animating2(shared_ptr<sprite> s, bool is_animating);
int sprite_get_context2(shared_ptr<sprite> s);
bool sprite_is_animating2(shared_ptr<sprite> s);
