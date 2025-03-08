#pragma once

#include <raylib.h>

#define TEXTUREINFO_ASSET_PATH_MAX_LENGTH 512

typedef struct {
    int num_frames;
    int contexts;
    Texture2D texture;
    char asset_path[TEXTUREINFO_ASSET_PATH_MAX_LENGTH];
} textureinfo;
