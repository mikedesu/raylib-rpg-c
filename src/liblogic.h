#pragma once

#include "gamestate.h"
#include "inputstate.h"
#include <memory>

#define TILE_COUNT_ERROR -999

void liblogic_init(std::shared_ptr<gamestate> g);
void liblogic_close(std::shared_ptr<gamestate> g);
void liblogic_tick(std::shared_ptr<inputstate> is, std::shared_ptr<gamestate> g);
void liblogic_restart(std::shared_ptr<gamestate> g);
