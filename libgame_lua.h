#pragma once

#include "mylua.h"
#include <stdbool.h>


const char* libgame_lua_get_str(lua_State* L, const char* key);

const int libgame_lua_get_int(lua_State* L, const char* key);

const int libgame_lua_create_entity(lua_State* L, const char* name, const int type, const int x, const int y);

const bool libgame_lua_set_entity_int(lua_State* L, const int id, const char* key, const int value);

const bool libgame_lua_set_entity_str(lua_State* L, const int id, const char* key, const char* value);

const int libgame_lua_get_entity_int(lua_State* L, const int id, const char* key);

const char* libgame_lua_get_entity_str(lua_State* L, const int id, const char* key);



const bool libgame_lua_create_dungeonfloor(lua_State* L, const int width, const int height, const int type);


const int libgame_lua_get_tiletype(lua_State* L, const int x, const int y);