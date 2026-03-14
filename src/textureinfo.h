/** @file textureinfo.h
 *  @brief Loaded texture metadata paired with asset path and spritesheet layout.
 */

#pragma once

#include <raylib.h>

/// @brief Maximum stored asset-path length for generated texture metadata.
#define TEXTUREINFO_ASSET_PATH_MAX_LENGTH 512

/// @brief Runtime texture record including spritesheet layout and original asset path.
typedef struct {
    int num_frames;
    int contexts;
    Texture2D texture;
    char asset_path[TEXTUREINFO_ASSET_PATH_MAX_LENGTH];
} textureinfo;
