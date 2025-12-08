#pragma once

#include "gamestate.h"

void set_npc_defaults(shared_ptr<gamestate> g, entityid id);
void set_npc_starting_stats(shared_ptr<gamestate> g, entityid id);

entityid create_npc_with(shared_ptr<gamestate> g, race_t rt, function<void(shared_ptr<gamestate>, entityid)> npcInitFunction);
entityid create_npc_at_with(shared_ptr<gamestate> g, race_t rt, vec3 loc, function<void(shared_ptr<gamestate>, entityid)> npcInitFunction);

// Original function still available but implemented using the new lambda-based version
entityid create_npc(shared_ptr<gamestate> g, race_t rt, vec3 loc, const string n);
