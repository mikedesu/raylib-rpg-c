#pragma once

#include "gamestate.h"
bool g_add_equipped_weapon(shared_ptr<gamestate> g, entityid id, entityid wpn_id);
bool g_has_equipped_weapon(shared_ptr<gamestate> g, entityid id);
bool g_set_equipped_weapon(shared_ptr<gamestate> g, entityid id, entityid wpn_id);
entityid g_get_equipped_weapon(shared_ptr<gamestate> g, entityid id);
