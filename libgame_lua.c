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




const bool libgame_lua_entity_move_random_dir(lua_State* L, const int id) {
    bool retval = false;
    if (L) {
        lua_getglobal(L, "EntityMoveRandomDir");
        lua_pushnumber(L, id);
        if (lua_pcall(L, 1, 1, 0) == 0) {
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



const bool libgame_lua_tile_is_occupied_by_player(lua_State* L, const int x, const int y) {
    bool retval = false;
    if (L) {
        lua_getglobal(L, "TileIsOccupiedByPlayer");
        lua_pushnumber(L, x);
        lua_pushnumber(L, y);
        if (lua_pcall(L, 2, 1, 0) == 0) {
            retval = lua_toboolean(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}



const bool libgame_lua_tile_is_occupied_by_npc(lua_State* L, const int x, const int y) {
    bool retval = false;
    if (L) {
        lua_getglobal(L, "TileIsOccupiedByNPC");
        lua_pushnumber(L, x);
        lua_pushnumber(L, y);
        if (lua_pcall(L, 2, 1, 0) == 0) {
            retval = lua_toboolean(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}



const int libgame_lua_get_dungeonfloor_row_count(lua_State* L) {
    int retval = -1;
    if (L) {
        lua_getglobal(L, "GetDungeonFloorRowCount");
        if (lua_pcall(L, 0, 1, 0) == 0) {
            retval = lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}




const int libgame_lua_get_dungeonfloor_col_count(lua_State* L) {
    int retval = -1;
    if (L) {
        lua_getglobal(L, "GetDungeonFloorColumnCount");
        if (lua_pcall(L, 0, 1, 0) == 0) {
            retval = lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}




const bool libgame_lua_set_tiletype(lua_State* L, const int type, const int x, const int y) {
    bool retval = false;
    if (L) {
        lua_getglobal(L, "SetTileType");
        lua_pushnumber(L, type);
        lua_pushnumber(L, x);
        lua_pushnumber(L, y);
        if (lua_pcall(L, 3, 1, 0) == 0) {
            retval = lua_toboolean(L, -1);
        }
        lua_pop(L, 1);
    }
    return retval;
}




void libgame_lua_randomize_all_dungeon_tiles(lua_State* L) {
    if (L) {
        lua_getglobal(L, "RandomizeAllDungeonTiles");
        lua_pcall(L, 0, 0, 0);
    }
}



void libgame_lua_randomize_dungeon_tiles(lua_State* L, const int x, const int y, const int w, const int h) {
    if (L) {
        lua_getglobal(L, "RandomizeDungeonTiles");
        lua_pushnumber(L, x);
        lua_pushnumber(L, y);
        lua_pushnumber(L, w);
        lua_pushnumber(L, h);
        lua_pcall(L, 4, 0, 0);
    }
}



void libgame_lua_set_int(lua_State* L, const char* key, const int value) {
    if (L) {
        lua_pushnumber(L, value);
        lua_setglobal(L, key);
    }
}
