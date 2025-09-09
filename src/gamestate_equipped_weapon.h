#pragma once

#include "gamestate.h"

bool g_add_equipped_weapon(shared_ptr<gamestate> g, entityid id, entityid wpn_id);
bool g_has_equipped_weapon(shared_ptr<gamestate> g, entityid id);

entityid g_get_equipped_weapon(shared_ptr<gamestate> g, entityid id);

bool g_equip_weapon(shared_ptr<gamestate> g, entityid id, entityid wpn_id);
bool g_unequip_weapon(shared_ptr<gamestate> g, entityid id, entityid wpn_id);
