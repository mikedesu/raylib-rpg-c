#include "libgame_lua.h"

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
