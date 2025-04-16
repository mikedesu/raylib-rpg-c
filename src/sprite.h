#pragma once

#include <raylib.h>

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

sprite* sprite_create(Texture2D* t, int numcontexts, int numframes);

void sprite_destroy(sprite* s);
void sprite_incrframe(sprite* const s);
void sprite_setcontext(sprite* const s, int context);
void sprite_set_is_animating(sprite* const s, bool is_animating);

int sprite_get_context(const sprite* const s);

bool sprite_is_animating(const sprite* const s);
