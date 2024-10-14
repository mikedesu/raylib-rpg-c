#pragma once

#include <raylib.h>

typedef struct {
    int num_frames;
    int contexts;
    //int width;
    //int height;
    Texture2D texture;
    char asset_path[128];
} textureinfo;
