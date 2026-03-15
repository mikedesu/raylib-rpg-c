/** @file draw_shield_sprite.h
 *  @brief Equipped-shield draw helpers for front/back render layers.
 */

#pragma once

#include "gamestate.h"
#include "spritegroup.h"

/** @brief Draw the equipped shield sprite behind the entity when applicable. */
void draw_shield_sprite_back(gamestate& g, entityid id, spritegroup* sg);

/** @brief Draw the equipped shield sprite in front of the entity when applicable. */
void draw_shield_sprite_front(gamestate& g, entityid id, spritegroup* sg);
