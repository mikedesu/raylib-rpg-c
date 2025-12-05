#pragma once

#include "gamestate.h"

entityid create_npc(shared_ptr<gamestate> g, race_t rt, vec3 loc, const string n);
void set_npc_defaults(shared_ptr<gamestate> g, entityid id);
void set_npc_starting_stats(shared_ptr<gamestate> g, entityid id);
