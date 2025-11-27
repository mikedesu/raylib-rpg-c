#pragma once

#include "gamestate.h"
#include "textureinfo.h"

bool draw_dungeon_floor_tile(shared_ptr<gamestate> g, textureinfo* txinfo, int x, int y, int z);
bool draw_dungeon_floor_wall(shared_ptr<gamestate> g, textureinfo* txinfo, int x, int y, int z);
