#pragma once
#include "gamestate.h"
#include "inputstate.h"

void liblogic_init(gamestate* const g);
void liblogic_tick(const inputstate* const is, gamestate* const g);
void liblogic_close(gamestate* const g);

entityid liblogic_entity_create(gamestate* const g, entitytype_t type, int x, int y, const char* name);
void liblogic_add_entityid(gamestate* const g, entityid id); // New
