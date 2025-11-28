#pragma once

#include "gamestate.h"
#include "spritegroup.h"

void update_shield_for_entity(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
