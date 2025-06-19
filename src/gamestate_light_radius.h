#pragma once

#include "gamestate.h"

bool g_add_light_radius(gamestate* const g, entityid id, int radius);
bool g_has_light_radius(const gamestate* const g, entityid id);
bool g_set_light_radius(gamestate* const g, entityid id, int radius);
int g_get_light_radius(const gamestate* const g, entityid id);

bool g_add_light_radius_bonus(gamestate* const g, entityid id, int radius);
bool g_has_light_radius_bonus(const gamestate* const g, entityid id);
bool g_set_light_radius_bonus(gamestate* const g, entityid id, int radius);
int g_get_light_radius_bonus(const gamestate* const g, entityid id);
int g_get_entity_total_light_radius_bonus(const gamestate* const g, entityid id);
