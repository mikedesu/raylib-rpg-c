#pragma once

#include <raylib.h>

typedef struct txinfo {
    Texture2D tx;
    int nframes;
    int ctxs;
    char asset_path[100];
    int width;
    int height;
} txinfo;
