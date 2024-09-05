#pragma once

#include <raylib.h>
#include <string>

typedef struct {
    int num_frames;
    int contexts;
    int width;
    int height;
    Texture2D texture;
    char asset_path[128];
} textureinfo;
