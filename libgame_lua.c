#include "libgame_lua.h"
#include "mprint.h"




const char* libgame_lua_get_str(lua_State* L, const char* key) {
    const char* retval = NULL;
    if (L) {
        lua_getglobal(L, key);
        if (lua_isstring(L, -1)) {
            retval = lua_tostring(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}




const int libgame_lua_get_int(lua_State* L, const char* key) {
    int retval = -1;
    if (L) {
        lua_getglobal(L, key);
        if (lua_isnumber(L, -1)) {
            retval = lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}




const int libgame_lua_create_entity(lua_State* L, const char* name, const int type, const int x, const int y) {
    int retval = -1;
    if (L) {
        lua_getglobal(L, "CreateEntity");
        lua_pushstring(L, name);
        lua_pushnumber(L, type);
        lua_pushnumber(L, x);
        lua_pushnumber(L, y);
        if (lua_pcall(L, 4, 1, 0) == 0) {
            retval = lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}




const bool libgame_lua_set_entity_int(lua_State* L, const int id, const char* key, const int value) {
    bool retval = false;
    if (L) {
        lua_getglobal(L, "SetEntityAttr");
        lua_pushnumber(L, id);
        lua_pushstring(L, key);
        lua_pushnumber(L, value);
        if (lua_pcall(L, 3, 1, 0) == 0) {
            retval = lua_toboolean(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}




const bool libgame_lua_set_entity_str(lua_State* L, const int id, const char* key, const char* value) {
    bool retval = false;
    if (L) {
        lua_getglobal(L, "SetEntityAttr");
        lua_pushnumber(L, id);
        lua_pushstring(L, key);
        lua_pushstring(L, value);
        if (lua_pcall(L, 3, 1, 0) == 0) {
            retval = lua_toboolean(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}




const int libgame_lua_get_entity_int(lua_State* L, const int id, const char* key) {
    int retval = -1;
    if (L) {
        lua_getglobal(L, "GetEntityAttr");
        lua_pushnumber(L, id);
        lua_pushstring(L, key);
        if (lua_pcall(L, 2, 1, 0) == 0) {
            if (lua_isnumber(L, -1)) {
                retval = lua_tonumber(L, -1);
            } else {
                merror("libgame_lua_get_entity_int: value is not a number");
                retval = -1;
            }
        }
        lua_pop(L, 1);
    }
    return retval;
}




const char* libgame_lua_get_entity_str(lua_State* L, const int id, const char* key) {
    const char* retval = NULL;
    if (L) {
        lua_getglobal(L, "GetEntityAttr");
        lua_pushnumber(L, id);
        lua_pushstring(L, key);
        if (lua_pcall(L, 2, 1, 0) == 0) {
            retval = lua_tostring(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}




const bool libgame_lua_create_dungeonfloor(lua_State* L, const int width, const int height, const int type) {
    bool retval = false;
    if (L) {
        lua_getglobal(L, "CreateDungeonFloor");
        lua_pushnumber(L, width);
        lua_pushnumber(L, height);
        lua_pushnumber(L, type);
        if (lua_pcall(L, 3, 0, 0) == 0) {
            retval = true;
        }
    }
    return retval;
}



const int libgame_lua_get_tiletype(lua_State* L, const int x, const int y) {
    int retval = -1;
    if (L) {
        lua_getglobal(L, "GetTileType");
        lua_pushnumber(L, x);
        lua_pushnumber(L, y);
        if (lua_pcall(L, 2, 1, 0) == 0) {
            retval = lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}




const bool libgame_lua_entity_move(lua_State* L, const int id, const int x, const int y) {
    bool retval = false;
    if (L) {
        lua_getglobal(L, "EntityMove");
        lua_pushnumber(L, id);
        lua_pushnumber(L, x);
        lua_pushnumber(L, y);
        if (lua_pcall(L, 3, 1, 0) == 0) {
            retval = lua_toboolean(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}




const int libgame_lua_get_num_entities_at(lua_State* L, const int x, const int y) {
    int retval = -1;
    if (L) {
        lua_getglobal(L, "GetNumEntitiesAt");
        lua_pushnumber(L, x);
        lua_pushnumber(L, y);
        if (lua_pcall(L, 2, 1, 0) == 0) {
            retval = lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}




const int libgame_lua_get_nth_entity_at(lua_State* L, const int n, const int x, const int y) {
    int retval = -1;
    if (L) {
        lua_getglobal(L, "GetNthEntityAt");
        lua_pushnumber(L, n);
        lua_pushnumber(L, x);
        lua_pushnumber(L, y);
        if (lua_pcall(L, 3, 1, 0) == 0) {
            retval = lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}
