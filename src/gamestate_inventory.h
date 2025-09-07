#pragma once

#include "gamestate.h"

bool g_add_inventory(shared_ptr<gamestate> g, entityid id);
bool g_has_inventory(shared_ptr<gamestate> g, entityid id);
shared_ptr<vector<entityid>> g_get_inventory(shared_ptr<gamestate> g, entityid id);

bool g_exists_in_inventory(shared_ptr<gamestate> g, entityid id, entityid item_id);
bool g_add_to_inventory(shared_ptr<gamestate> g, entityid id, entityid item_id);
bool g_remove_from_inventory(shared_ptr<gamestate> g, entityid id, entityid item_id);
