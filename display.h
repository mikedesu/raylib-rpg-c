#pragma once

#include <raylib.h>

typedef struct display {
    int width;
    int height;
    Font font;
    RenderTexture2D target;

    Vector2 origin;

} display;

#define DISPLAYSIZE (sizeof(display))
