#pragma once

#include <raylib.h>

typedef struct display {
    int width;
    int height;
    Font font;
    RenderTexture2D target;

    Vector2 origin;

} display;

#define DISPLAYSIZE (sizeof(int) * 2 + sizeof(Font) + sizeof(RenderTexture2D) + sizeof(Vector2))
