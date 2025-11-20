#pragma once

#include "gamestate.h"
#include "spritegroup.h"
void libdraw_update_sprite_position(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
void libdraw_update_sprite_context_ptr(shared_ptr<gamestate> g, spritegroup_t* group, direction_t dir);
void libdraw_update_sprite_ptr(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
