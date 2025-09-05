#pragma once

#include "gamestate.h"

bool g_add_inventory(shared_ptr<gamestate> g, entityid id);
bool g_has_inventory(shared_ptr<gamestate> g, entityid id);
shared_ptr<set<entityid>> g_get_inventory(shared_ptr<gamestate> g, entityid id);
