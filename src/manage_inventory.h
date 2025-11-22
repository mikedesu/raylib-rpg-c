#pragma once

#include "gamestate.h"

bool add_to_inventory(shared_ptr<gamestate> g, entityid actor_id, entityid item_id);
