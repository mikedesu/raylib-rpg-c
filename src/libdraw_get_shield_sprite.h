#pragma once

#include "gamestate.h"
#include "sprite.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"


shared_ptr<sprite> get_shield_front_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
shared_ptr<sprite> get_shield_back_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
