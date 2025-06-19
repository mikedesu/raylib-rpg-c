#pragma once

#include "gamestate.h"

bool g_add_explored_list(gamestate* const g, entityid id);
bool g_add_explored(gamestate* const g, entityid id, vec3 loc);
bool g_has_explored(const gamestate* const g, entityid id);
bool g_set_explored(gamestate* const g, entityid id, vec3 loc);
bool g_is_explored(const gamestate* const g, entityid id, vec3 loc);
bool g_get_explored_for_entity(const gamestate* const g, entityid id, vec3_list_component** explored);
