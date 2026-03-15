/** @file update_sprite.h
 *  @brief Per-entity spritegroup update helpers for context, movement, and animation state.
 */

#pragma once
#include "gamestate.h"
#include "spritegroup.h"

void libdraw_update_sprite_context_ptr(gamestate& g, spritegroup* group, direction_t dir);
void libdraw_update_sprite_position(gamestate& g, entityid id, spritegroup* sg);
void libdraw_update_sprite_ptr(gamestate& g, entityid id, spritegroup* sg);
void libdraw_update_sprite_pre(gamestate& g, entityid id);
