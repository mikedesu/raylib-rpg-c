#pragma once
#include "gamestate.h"
#include "spritegroup.h"
void draw_weapon_sprite_back(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
void draw_weapon_sprite_front(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
