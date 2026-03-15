/** @file libdraw_create_spritegroup.h
 *  @brief Helpers for creating renderer spritegroups from texture-key arrays.
 */

#pragma once

#include "gamestate.h"

/**
 * @brief Create and register a spritegroup for an entity.
 *
 * The spritegroup is initialized from the provided texture keys and snapped to
 * the entity location when a location component exists.
 */
bool create_spritegroup(gamestate& g, entityid id, int* keys, int num_keys, int offset_x, int offset_y);

/** @brief Create a spritegroup using the project's default sprite offsets. */
bool create_sg(gamestate& g, entityid id, int* keys, int num_keys);
