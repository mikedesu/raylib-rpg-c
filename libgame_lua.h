#pragma once

#include "mylua.h"


const char* libgame_lua_get_str(lua_State* L, const char* key);
const int libgame_lua_get_int(lua_State* L, const char* key);
