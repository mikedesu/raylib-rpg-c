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

} sprite;


sprite* sprite_create(Texture2D* t, const int numcontexts, const int numframes);
void sprite_destroy(sprite* s);
void sprite_updatesrc(sprite* s);
void sprite_incrframe(sprite* s);
void sprite_incrcontext(sprite* s);
void sprite_setcontext(sprite* s, const int context);
const int sprite_get_context(sprite* s);
