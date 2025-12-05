#pragma once


#include "gamestate.h"
//entityid create_shield_at(shared_ptr<gamestate> g, vec3 loc);

//entityid create_shield(shared_ptr<gamestate> g, shieldtype_t type);


entityid create_shield_with(shared_ptr<gamestate> g, function<void(entityid)> shieldInitFunction);


entityid create_shield_at_with(shared_ptr<gamestate> g, vec3 loc, function<void(entityid)> shieldInitFunction);
