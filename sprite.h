#pragma once

#include <raylib.h>


typedef struct sprite {
    Texture2D* texture;
    Vector2 position;

    int width;
    int height;

    Rectangle src;
    Rectangle dest;

    int numcontexts;
    int numframes;

    int currentframe;
    int currentcontext;

} sprite;


sprite* sprite_create(Texture2D* t, int numcontexts, int numframes);
sprite* sprite_destroy(sprite* s);
void sprite_updatesrc(sprite* s);
void sprite_incrframe(sprite* s);
void sprite_incrcontext(sprite* s);
