#pragma once

#include <raylib.h>


typedef struct sprite {
    Texture2D* texture;
    Vector2 position;

    int numcontexts;
    int numframes;

    int currentframe;
    int currentcontext;

} sprite;


sprite* sprite_create(Texture2D* t, int numcontexts, int numframes);
//sprite* sprite_create(const char* filename, int numcontexts, int numframes);
sprite* sprite_destroy(sprite* s);
