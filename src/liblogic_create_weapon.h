#pragma once

#include "gamestate.h"

entityid create_weapon_at(shared_ptr<gamestate> g, vec3 loc, weapontype_t type);
entityid create_weapon(shared_ptr<gamestate> g, weapontype_t type);
