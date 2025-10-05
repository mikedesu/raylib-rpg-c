#pragma once

#include "gamestate.h"
#include "textureinfo.h"

bool create_spritegroup(shared_ptr<gamestate> g, entityid id, int* keys, int num_keys, int offset_x, int offset_y);
