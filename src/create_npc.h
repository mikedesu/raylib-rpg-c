#pragma once

#include "gamestate.h"

entityid create_npc(shared_ptr<gamestate> g, race_t rt, vec3 loc, const string n);
