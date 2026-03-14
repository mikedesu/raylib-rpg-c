/** @file environments.h
 *  @brief Environment identifiers for world or content tagging.
 */

#pragma once

/// @brief Broad environment categories available to world/content systems.
typedef enum environment_t
{
    E_NONE,
    E_DESERT,
    E_FOREST,
    E_DUNGEON,
    E_CAVE,
    E_TOWN,
    E_TUNDRA,
    E_WASTELAND,
    E_HELLSCAPE,
    E_COUNT
} environment;
