#pragma once


#include "gamestate.h"
entityid create_shield_at(shared_ptr<gamestate> g, vec3 loc, shieldtype_t type);
entityid create_shield(shared_ptr<gamestate> g, shieldtype_t type);
