#pragma once

#include "gamestate.h"
#include "inputstate.h"
#include <memory>

#define TILE_COUNT_ERROR -999

using std::shared_ptr;

void liblogic_init(shared_ptr<gamestate> g);
void liblogic_close(shared_ptr<gamestate> g);
void liblogic_tick(shared_ptr<inputstate> is, shared_ptr<gamestate> g);
void liblogic_restart(shared_ptr<gamestate> g);
