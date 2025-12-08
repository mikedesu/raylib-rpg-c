#pragma once

#include "gamestate.h"

entityid create_shield_with(shared_ptr<gamestate> g, function<void(shared_ptr<gamestate>, entityid)> shieldInitFunction);
entityid create_shield_at_with(shared_ptr<gamestate> g, vec3 loc, function<void(shared_ptr<gamestate>, entityid)> shieldInitFunction);
