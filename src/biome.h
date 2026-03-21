/** @file biome.h
 *  @brief Biome identifiers used when creating dungeon floors.
 */

#pragma once

/// @brief Available biome styles for generated floors.
typedef enum
{
    BIOME_NONE = 0,
    BIOME_STONE,
    BIOME_GRASS,
    //BIOME_SAND,
    //BIOME_WATER,
    BIOME_COUNT,
} biome_t;
