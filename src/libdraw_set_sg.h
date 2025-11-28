
#pragma once

#include "gamestate.h"
#include "spritegroup.h"

void libdraw_set_sg_is_damaged(shared_ptr<gamestate> g, entityid id, spritegroup_t* const sg);
void libdraw_set_sg_is_dead(shared_ptr<gamestate> g, entityid id, spritegroup_t* const sg);
void libdraw_set_sg_block_success(shared_ptr<gamestate> g, entityid id, spritegroup_t* const sg);
