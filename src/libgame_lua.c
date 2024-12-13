#include "libgame_lua.h"
#include "mprint.h"




const char* libgame_lua_get_str(lua_State* L, const char* key) {
    if (!L) {
        return NULL;
    }
    const char* retval = NULL;
    lua_getglobal(L, key);
    if (lua_isstring(L, -1)) {
        retval = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_int(lua_State* L, const char* key) {
    if (!L) {
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, key);
    if (lua_isnumber(L, -1)) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_create_entity(
    lua_State* L, const char* name, const int type, const int x, const int y) {
    if (!L) {
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "CreateEntity");
    lua_pushstring(L, name);
    lua_pushnumber(L, type);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 4, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}




const bool libgame_lua_set_entity_int(lua_State* L,
                                      const int id,
                                      const char* key,
                                      const int value) {
    if (!L) {
        return false;
    }
    bool retval = false;
    lua_getglobal(L, "SetEntityAttr");
    lua_pushnumber(L, id);
    lua_pushstring(L, key);
    lua_pushnumber(L, value);
    if (lua_pcall(L, 3, 1, 0) == 0) {
        retval = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}




const bool libgame_lua_set_entity_str(lua_State* L,
                                      const int id,
                                      const char* key,
                                      const char* value) {
    if (!L) {
        return false;
    }
    bool retval = false;
    lua_getglobal(L, "SetEntityAttr");
    lua_pushnumber(L, id);
    lua_pushstring(L, key);
    lua_pushstring(L, value);
    if (lua_pcall(L, 3, 1, 0) == 0) {
        retval = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_gamestate_int(lua_State* L, const char* key) {
    if (!L) {
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetGamestateAttr");
    lua_pushstring(L, key);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        if (lua_isnumber(L, -1)) {
            retval = lua_tonumber(L, -1);
        } else {
            merror("libgame_lua_get_gamestate_int: value is not a number");
            char buf[128];
            snprintf(buf, 128, "GetGamestateAttr(%s)", key);
            merror(buf);
            retval = -1;
        }
    }
    lua_pop(L, 1);
    return retval;
}




const char* libgame_lua_get_gamestate_str(lua_State* L, const char* key) {
    if (!L) {
        return NULL;
    }
    const char* retval = NULL;
    lua_getglobal(L, "GetGamestateAttr");
    lua_pushstring(L, key);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        if (lua_isstring(L, -1)) {
            retval = lua_tostring(L, -1);
        } else {
            merror("libgame_lua_get_gamestate_str: value is not a string");
            char buf[128];
            snprintf(buf, 128, "GetGamestateAttr(%s)", key);
            merror(buf);
            retval = NULL;
        }
    }
    lua_pop(L, 1);
    return retval;
}




const int
libgame_lua_get_entity_int(lua_State* L, const int id, const char* key) {
    if (!L) {
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetEntityAttr");
    lua_pushnumber(L, id);
    lua_pushstring(L, key);
    if (lua_pcall(L, 2, 1, 0) == 0) {
        if (lua_isnumber(L, -1)) {
            retval = lua_tonumber(L, -1);
        } else {
            //merror("libgame_lua_get_entity_int: value is not a number");
            //char buf[128];
            //snprintf(buf, 128, "GetEntityAttr(%d, %s)", id, key);
            //merror(buf);
            retval = -1;
        }
    }
    lua_pop(L, 1);
    return retval;
}




const char*
libgame_lua_get_entity_str(lua_State* L, const int id, const char* key) {
    if (!L) {
        return NULL;
    }
    const char* retval = NULL;
    lua_getglobal(L, "GetEntityAttr");
    lua_pushnumber(L, id);
    lua_pushstring(L, key);
    if (lua_pcall(L, 2, 1, 0) == 0) {
        retval = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}




const bool libgame_lua_create_dungeonfloor(lua_State* L,
                                           const int width,
                                           const int height,
                                           const int type) {
    if (!L) {
        return false;
    }
    bool retval = false;
    lua_getglobal(L, "CreateDungeonFloor");
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    lua_pushnumber(L, type);
    if (lua_pcall(L, 3, 0, 0) == 0) {
        retval = true;
    }
    return retval;
}



const int libgame_lua_get_tiletype(lua_State* L, const int x, const int y) {
    if (!L) {
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetTileType");
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 2, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}




const bool
libgame_lua_entity_move(lua_State* L, const int id, const int x, const int y) {
    if (!L) {
        return false;
    }
    bool retval = false;
    lua_getglobal(L, "EntityMove");
    lua_pushnumber(L, id);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 3, 1, 0) == 0) {
        retval = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}




const bool libgame_lua_entity_move_random_dir(lua_State* L, const int id) {
    if (!L) {
        return false;
    }
    bool retval = false;
    lua_getglobal(L, "EntityMoveRandomDir");
    lua_pushnumber(L, id);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}




const int
libgame_lua_get_num_entities_at(lua_State* L, const int x, const int y) {
    if (!L) {
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetNumEntitiesAt");
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 2, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_nth_entity_at(lua_State* L,
                                        const int n,
                                        const int x,
                                        const int y) {
    if (!L) {
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetNthEntityAt");
    lua_pushnumber(L, n);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 3, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}



const bool
libgame_lua_tile_is_occupied_by_npc(lua_State* L, const int x, const int y) {
    if (!L) {
        return false;
    }
    bool retval = false;
    lua_getglobal(L, "TileIsOccupiedByNPC");
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 2, 1, 0) == 0) {
        retval = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_dungeonfloor_row_count(lua_State* L) {
    if (!L) {
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetDungeonFloorRowCount");
    if (lua_pcall(L, 0, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_dungeonfloor_col_count(lua_State* L) {
    if (!L) {
        return -1;
    }
    int retval = -1;
    //if (L) {
    lua_getglobal(L, "GetDungeonFloorColumnCount");
    if (lua_pcall(L, 0, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    //}
    return retval;
}




const bool libgame_lua_set_tiletype(lua_State* L,
                                    const int type,
                                    const int x,
                                    const int y) {

    if (!L) {
        return false;
    }
    bool retval = false;
    //if (L) {
    lua_getglobal(L, "SetTileType");
    lua_pushnumber(L, type);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 3, 1, 0) == 0) {
        retval = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);
    //}
    return retval;
}




void libgame_lua_randomize_all_dungeon_tiles(lua_State* L) {
    if (!L) {
        return;
    }
    //if (L) {
    lua_getglobal(L, "RandomizeAllDungeonTiles");
    lua_pcall(L, 0, 0, 0);
    //}
}




void libgame_lua_randomize_dungeon_tiles(
    lua_State* L, const int x, const int y, const int w, const int h) {
    //if (!L) return;
    if (!L) {
        return;
    }
    //if (L) {
    //char buf[128];
    //snprintf(buf, 128, "RandomizeDungeonTiles(%d, %d, %d, %d)", x, y, w, h);
    //minfo(buf);

    lua_getglobal(L, "RandomizeDungeonTiles");
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    lua_pcall(L, 4, 0, 0);
    //}
}



void libgame_lua_set_int(lua_State* L, const char* key, const int value) {
    //if (!L) return;
    if (!L) {
        return;
    }
    //if (L) {
    lua_pushnumber(L, value);
    lua_setglobal(L, key);
    //}
}




const bool libgame_lua_create_action(lua_State* L,
                                     const int entity_id,
                                     const int type,
                                     const int x,
                                     const int y) {
    //if (!L) return false;
    if (!L) {
        return false;
    }
    bool retval = false;
    //if (L) {
    //char buf[256];
    //snprintf(buf, 256, "CreateAction(%d, %d, %d, %d)", entity_id, type, x, y);
    //minfo(buf);
    lua_getglobal(L, "CreateAction");
    lua_pushnumber(L, entity_id);
    lua_pushnumber(L, type);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 4, 1, 0) == 0) {
        retval = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);
    //}
    return retval;
}




const int libgame_lua_get_action_count(lua_State* L) {
    //if (!L) return -1;
    if (!L) {
        return -1;
    }
    int retval = -1;
    //if (L) {
    lua_getglobal(L, "GetActionCount");
    if (lua_pcall(L, 0, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    //}
    return retval;
}




const int libgame_lua_process_action(lua_State* L, const int index) {
    //if (!L) return -1;
    if (!L) {
        return -1;
    }
    int retval = -1;
    //if (L) {
    lua_getglobal(L, "ProcessAction");
    lua_pushnumber(L, index);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    //}
    return retval;
}




void libgame_lua_clear_actions(lua_State* L) {
    //if (!L) return;
    if (!L) {
        return;
    }
    //if (L) {
    lua_getglobal(L, "ClearActions");
    lua_pcall(L, 0, 0, 0);
    //}
}




const int libgame_lua_get_num_entities(lua_State* L) {
    //if (!L) return -1;
    if (!L) {
        return -1;
    }
    int retval = -1;
    //if (L) {
    lua_getglobal(L, "GetNumEntities");
    if (lua_pcall(L, 0, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    //}
    return retval;
}




const int libgame_lua_get_nth_entity(lua_State* L, const int n) {
    //char buf[128];
    //snprintf(buf, 128, "GetNthEntity(%d)", n);
    //minfo(buf);
    //if (!L) return -1;
    if (!L) {
        return -1;
    }
    int retval = -1;
    //if (L) {
    lua_getglobal(L, "GetNthEntity");
    lua_pushnumber(L, n);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    //}
    //snprintf(buf, 128, "GetNthEntity(%d) = %d", n, retval);
    //msuccess(buf);
    return retval;
}




//void libgame_lua_print_entities(lua_State* L) {
//    if (L) {
//        lua_getglobal(L, "PrintEntities");
//        lua_pcall(L, 0, 0, 0);
//    }
//}




//void libgame_lua_reserialization_test(lua_State* L) {
//    minfo("libgame_lua_reserialization_test");
//    if (L) {
//        lua_getglobal(L, "ReserializationTest");
//        lua_pcall(L, 0, 0, 0);
//    }
//}



void libgame_lua_set_gamestate_int(lua_State* L,
                                   const char* key,
                                   const int value) {
    //if (!L) return;
    if (!L) {
        return;
    }
    //if (L) {
    lua_getglobal(L, "SetGamestateAttr");
    lua_pushstring(L, key);
    lua_pushnumber(L, value);
    lua_pcall(L, 2, 0, 0);
    //}
}


//const bool libgame_lua_process_next_action(lua_State* L) {
//    if (L) {
//        lua_getglobal(L, "ProcessNextAction");
//        lua_pcall(L, 0, 0, 0);
//    }
//}



//const bool libgame_lua_actions_exist(lua_State* L) {
//    bool retval = false;
//    if (L) {
//        lua_getglobal(L, "ActionsExist");
//        if (lua_pcall(L, 0, 1, 0) == 0) {
//            retval = lua_toboolean(L, -1);
//        }
//        lua_pop(L, 1);
//    }
//    return retval;
//}


void libgame_lua_mytest(lua_State* L) {
    //if (!L) return;
    if (!L) {
        return;
    }
    //if (L) {
    lua_getglobal(L, "MyTest");
    lua_pcall(L, 0, 0, 0);
    //}
}




const int libgame_lua_get_nth_action_type(lua_State* L, const int n) {
    //if (!L) return -1;
    if (!L) {
        return -1;
    }
    int retval = -1;
    //if (L) {
    lua_getglobal(L, "GetNthActionType");
    lua_pushnumber(L, n);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    //}
    return retval;
}




const int libgame_lua_get_nth_action_x(lua_State* L, const int n) {
    //if (!L) return -1;
    if (!L) {
        return -1;
    }
    int retval = -1;
    //if (L) {
    lua_getglobal(L, "GetNthActionX");
    lua_pushnumber(L, n);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    //}
    return retval;
}




const int libgame_lua_get_nth_action_y(lua_State* L, const int n) {
    //if (!L) return -1;
    if (!L) {
        return -1;
    }
    int retval = -1;
    //if (L) {
    lua_getglobal(L, "GetNthActionY");
    lua_pushnumber(L, n);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    //}
    return retval;
}




void libgame_lua_clear_was_damaged(lua_State* L) {
    //if (!L) return;
    if (!L) {
        return;
    }
    //if (L) {
    lua_getglobal(L, "ClearWasDamaged");
    lua_pcall(L, 0, 0, 0);
    //}
}



void libgame_lua_print_entity_info(lua_State* L) {
    //if (!L) return;
    if (!L) {
        return;
    }
    //if (L) {
    lua_getglobal(L, "PrintEntityInfo");
    lua_pcall(L, 0, 0, 0);
    //}
}




const int libgame_lua_get_nth_action_id(lua_State* L, const int n) {
    //if (!L) return -1;
    if (!L) {
        return -1;
    }
    int retval = -1;
    //if (L) {
    lua_getglobal(L, "GetNthActionId");
    lua_pushnumber(L, n);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    //}
    return retval;
}



const int libgame_lua_get_inventory_count(lua_State* L, const int id) {
    //if (!L) return -1;
    if (!L) {
        return -1;
    }
    int retval = -1;
    //if (L) {
    lua_getglobal(L, "GetInventoryCount");
    lua_pushnumber(L, id);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    //}
    return retval;
}



const entityid libgame_lua_get_entity_shield(lua_State* L, const int id) {
    //if (!L) return -1;
    if (!L) {
        return -1;
    }
    entityid retval = -1;
    //if (L) {
    lua_getglobal(L, "GetEntityShield");
    lua_pushnumber(L, id);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    //}
    return retval;
}




void libgame_lua_incr_current_turn(lua_State* L) {
    //if (!L) return;
    if (!L) {
        return;
    }
    lua_getglobal(L, "IncrementCurrentTurn");
    lua_pcall(L, 0, 0, 0);
}



const int libgame_lua_get_move_seq_begin(lua_State* L, const int begin) {
    //if (!L) return -1;
    if (!L) {
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetMoveSeqBegin");
    lua_pushnumber(L, begin);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_move_seq_end(lua_State* L, const int begin) {
    //if (!L) return -1;
    if (!L) {
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetMoveSeqEnd");
    lua_pushnumber(L, begin);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}



const int libgame_lua_get_action_results_count(lua_State* L) {
    //if (!L) return -1;
    if (!L) {
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetActionResultsCount");
    if (lua_pcall(L, 0, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}


const int
libgame_lua_get_action_result(lua_State* L, const int index, const char* key) {

    //if (!L) return -1;
    if (!L) {
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetActionResult");
    lua_pushnumber(L, index);
    lua_pushstring(L, key);
    if (lua_pcall(L, 2, 1, 0) == 0) {
        if (lua_isnumber(L, -1)) {
            retval = lua_tonumber(L, -1);
        } else {
            merror("libgame_lua_get_action_result: value is not a number");
            char buf[128];
            snprintf(buf, 128, "GetActionResult(%d, %s)", index, key);
            merror(buf);
            retval = -1;
        }
    }
    lua_pop(L, 1);
    return retval;
}



void libgame_lua_process_actions(lua_State* L) {
    if (!L) {
        return;
    }
    lua_getglobal(L, "ProcessActions");
    lua_pcall(L, 0, 0, 0);
}


void libgame_lua_clear_action_results(lua_State* L) {
    if (!L) {
#ifdef DEBUG
        merror("libgame_lua_clear_action_results: L is NULL");
#endif
        return;
    }
    lua_getglobal(L, "ClearActionResults");
    lua_pcall(L, 0, 0, 0);
}
