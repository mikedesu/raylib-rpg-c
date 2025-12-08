#pragma once

#include "gamestate.h"

//entityid create_weapon_with(shared_ptr<gamestate> g, function<void(entityid)> weaponInitFunction);
entityid create_weapon_with(shared_ptr<gamestate> g, function<void(shared_ptr<gamestate>, entityid)> weaponInitFunction);
//entityid create_weapon_at_with(shared_ptr<gamestate> g, vec3 loc, function<void(entityid)> weaponInitFunction);
entityid create_weapon_at_with(shared_ptr<gamestate> g, vec3 loc, function<void(shared_ptr<gamestate>, entityid)> weaponInitFunction);
//function<void(entityid)> weaponInitFunction);
