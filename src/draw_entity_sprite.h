/** @file draw_entity_sprite.h
 *  @brief Helper for drawing an entity's primary spritegroup sprite.
 */

#pragma once

#include "gamestate.h"
#include "spritegroup.h"

/** @brief Draw the active sprite from a spritegroup at its destination rectangle. */
void draw_entity_sprite(gamestate& g, spritegroup* sg);
