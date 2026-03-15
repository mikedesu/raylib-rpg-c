/** @file draw_weapon_sprite.h
 *  @brief Equipped-weapon draw helpers for front/back render layers.
 */

#pragma once
#include "gamestate.h"
#include "spritegroup.h"

/** @brief Draw the equipped weapon sprite behind the entity when applicable. */
void draw_weapon_sprite_back(gamestate& g, entityid id, spritegroup* sg);

/** @brief Draw the equipped weapon sprite in front of the entity when applicable. */
void draw_weapon_sprite_front(gamestate& g, entityid id, spritegroup* sg);
