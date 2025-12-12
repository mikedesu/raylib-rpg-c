#pragma once

#include "gamestate.h"

bool libdraw_draw_dungeon_floor(shared_ptr<gamestate> g);

void libdraw_draw_dungeon_floor_entitytype(shared_ptr<gamestate> g,
                                           entitytype_t entitytype_0,
                                           function<bool(shared_ptr<gamestate>, entityid)> additional_check);
