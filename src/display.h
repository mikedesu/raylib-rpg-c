#pragma once

#include <raylib.h>

typedef struct display {
    int targetwidth;
    int targetheight;
    int windowwidth;
    int windowheight;

    //Font font;
    //RenderTexture2D target;
    //Vector2 origin;
} display_t;

//#define DISPLAYSIZE (sizeof(int) * 2 + sizeof(Font) + sizeof(RenderTexture2D) + sizeof(Vector2))
