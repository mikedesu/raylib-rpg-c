#include "libgame_lua.h"
#include "mprint.h"




const char* libgame_lua_get_str(lua_State* L, const char* key) {
    if (!L) {
        merror("L is NULL");
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
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, key);
    if (lua_isnumber(L, -1)) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_get_int: value is not a number");
        char buf[128];
        snprintf(buf, 128, "GetGamestateAttr(%s)", key);
        merror(buf);
        retval = -1;
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_create_entity(
    lua_State* L, const char* name, const int type, const int x, const int y) {
    if (!L) {
        merror("L is NULL");
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
    } else {
        merror("libgame_lua_create_entity: failed to create entity");
    }
    lua_pop(L, 1);
    return retval;
}




const bool libgame_lua_set_entity_int(lua_State* L,
                                      const int id,
                                      const char* key,
                                      const int value) {
    if (!L) {
        merror("L is NULL");
        return false;
    }
    bool retval = false;
    lua_getglobal(L, "SetEntityAttr");
    lua_pushnumber(L, id);
    lua_pushstring(L, key);
    lua_pushnumber(L, value);
    if (lua_pcall(L, 3, 1, 0) == 0) {
        retval = lua_toboolean(L, -1);
    } else {
        merror("libgame_lua_set_entity_int: failed to set entity int");
    }
    lua_pop(L, 1);
    return retval;
}




const bool libgame_lua_set_entity_str(lua_State* L,
                                      const int id,
                                      const char* key,
                                      const char* value) {
    if (!L) {
        merror("L is NULL");
        return false;
    }
    bool retval = false;
    lua_getglobal(L, "SetEntityAttr");
    lua_pushnumber(L, id);
    lua_pushstring(L, key);
    lua_pushstring(L, value);
    if (lua_pcall(L, 3, 1, 0) == 0) {
        retval = lua_toboolean(L, -1);
    } else {
        merror("libgame_lua_set_entity_str: failed to set entity string");
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_gamestate_int(lua_State* L, const char* key) {
    if (!L) {
        merror("L is NULL");
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
    } else {
        merror("libgame_lua_get_gamestate_int: failed to get gamestate int");
    }
    lua_pop(L, 1);
    return retval;
}




const char* libgame_lua_get_gamestate_str(lua_State* L, const char* key) {
    if (!L) {
        merror("L is NULL");
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
    } else {
        merror("libgame_lua_get_gamestate_str: failed to get gamestate string");
    }
    lua_pop(L, 1);
    return retval;
}




const int
libgame_lua_get_entity_int(lua_State* L, const int id, const char* key) {
    if (!L) {
        merror("L is NULL");
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
    } else {
        merror("libgame_lua_get_entity_int: failed to get entity int");
    }
    lua_pop(L, 1);
    return retval;
}




const char*
libgame_lua_get_entity_str(lua_State* L, const int id, const char* key) {
    if (!L) {
        merror("L is NULL");
        return NULL;
    }
    const char* retval = NULL;
    lua_getglobal(L, "GetEntityAttr");
    lua_pushnumber(L, id);
    lua_pushstring(L, key);
    if (lua_pcall(L, 2, 1, 0) == 0) {
        retval = lua_tostring(L, -1);
    } else {
        merror("libgame_lua_get_entity_str: failed to get entity string");
    }
    lua_pop(L, 1);
    return retval;
}




const bool libgame_lua_create_dungeonfloor(lua_State* L,
                                           const int width,
                                           const int height,
                                           const int type) {
    if (!L) {
        merror("L is NULL");
        return false;
    }
    bool retval = false;
    lua_getglobal(L, "CreateDungeonFloor");
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    lua_pushnumber(L, type);
    if (lua_pcall(L, 3, 0, 0) == 0) {
        retval = true;
    } else {
        merror(
            "libgame_lua_create_dungeonfloor: failed to create dungeon floor");
    }
    return retval;
}



const int libgame_lua_get_tiletype(lua_State* L, const int x, const int y) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetTileType");
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 2, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_get_tiletype: failed to get tile type");
    }
    lua_pop(L, 1);
    return retval;
}




const bool libgame_lua_entity_move(lua_State* L,
                                   const int id,
                                   const float x,
                                   const float y) {
    if (!L) {
        merror("L is NULL");
        return false;
    }
    bool retval = false;
    lua_getglobal(L, "EntityMove");
    lua_pushnumber(L, id);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 3, 0, 0) == 0) {
        retval = true;
    } else {
        merror("libgame_lua_entity_move: failed to move entity");
    }
    return retval;
}




const bool libgame_lua_entity_move_random_dir(lua_State* L, const int id) {
    if (!L) {
        merror("L is NULL");
        return false;
    }
    bool retval = false;
    lua_getglobal(L, "EntityMoveRandomDir");
    lua_pushnumber(L, id);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_toboolean(L, -1);
    } else {
        merror("libgame_lua_entity_move_random_dir: failed to move entity");
    }
    lua_pop(L, 1);
    return retval;
}




const int
libgame_lua_get_num_entities_at(lua_State* L, const int x, const int y) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetNumEntitiesAt");
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 2, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror(
            "libgame_lua_get_num_entities_at: failed to get num entities at");
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_nth_entity_at(lua_State* L,
                                        const int n,
                                        const int x,
                                        const int y) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetNthEntityAt");
    lua_pushnumber(L, n);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 3, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_get_nth_entity_at: failed to get nth entity at");
    }
    lua_pop(L, 1);
    return retval;
}



const bool
libgame_lua_tile_is_occupied_by_npc(lua_State* L, const int x, const int y) {
    if (!L) {
        merror("L is NULL");
        return false;
    }
    bool retval = false;
    lua_getglobal(L, "TileIsOccupiedByNPC");
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 2, 1, 0) == 0) {
        retval = lua_toboolean(L, -1);
    } else {
        merror("libgame_lua_tile_is_occupied_by_npc: failed to check tile");
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_dungeonfloor_row_count(lua_State* L) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetDungeonFloorRowCount");
    if (lua_pcall(L, 0, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror(
            "libgame_lua_get_dungeonfloor_row_count: failed to get row count");
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_dungeonfloor_col_count(lua_State* L) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    //if (L) {
    lua_getglobal(L, "GetDungeonFloorColumnCount");
    if (lua_pcall(L, 0, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_get_dungeonfloor_col_count: failed to get column "
               "count");
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
        merror("L is NULL");
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
    } else {
        merror("libgame_lua_set_tiletype: failed to set tile type");
    }
    lua_pop(L, 1);
    //}
    return retval;
}




void libgame_lua_randomize_all_dungeon_tiles(lua_State* L) {
    if (!L) {
        merror("L is NULL");
        return;
    }
    //if (L) {
    lua_getglobal(L, "RandomizeAllDungeonTiles");
    if (lua_pcall(L, 0, 0, 0) != 0) {
        merror("libgame_lua_randomize_all_dungeon_tiles: failed to randomize "
               "all dungeon tiles");
    }
    //}
}




void libgame_lua_randomize_dungeon_tiles(
    lua_State* L, const int x, const int y, const int w, const int h) {
    //if (!L) return;
    if (!L) {
        merror("L is NULL");
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
    if (lua_pcall(L, 4, 0, 0) != 0) {
        merror("libgame_lua_randomize_dungeon_tiles: failed to randomize "
               "dungeon tiles");
    }
    //}
}



void libgame_lua_set_int(lua_State* L, const char* key, const int value) {
    //if (!L) return;
    if (!L) {
        merror("L is NULL");
        return;
    }
    lua_pushnumber(L, value);
    lua_setglobal(L, key);
}




const bool libgame_lua_create_action(lua_State* L,
                                     const int entity_id,
                                     const int type,
                                     const int x,
                                     const int y) {
    if (!L) {
        merror("L is NULL");
        return false;
    }
    bool retval = false;
    lua_getglobal(L, "CreateAction");
    lua_pushnumber(L, entity_id);
    lua_pushnumber(L, type);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 4, 1, 0) == 0) {
        retval = lua_toboolean(L, -1);
    } else {
        merror("libgame_lua_create_action: failed to create action");
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_action_count(lua_State* L) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetActionCount");
    if (lua_pcall(L, 0, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_get_action_count: failed to get action count");
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_process_action(lua_State* L, const int index) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "ProcessAction");
    lua_pushnumber(L, index);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_process_action: failed to process action");
    }
    lua_pop(L, 1);
    return retval;
}




void libgame_lua_clear_actions(lua_State* L) {
    if (!L) {
        merror("L is NULL");
        return;
    } else {
        msuccess("libgame_lua_clear_actions");
    }
    lua_getglobal(L, "ClearActions");
    lua_pcall(L, 0, 0, 0);
}




const int libgame_lua_get_num_entities(lua_State* L) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetNumEntities");
    if (lua_pcall(L, 0, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_get_num_entities: failed to get num entities");
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_nth_entity(lua_State* L, const int n) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetNthEntity");
    lua_pushnumber(L, n);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_get_nth_entity: failed to get nth entity");
    }
    lua_pop(L, 1);
    return retval;
}




void libgame_lua_set_gamestate_int(lua_State* L,
                                   const char* key,
                                   const int value) {
    if (!L) {
        merror("L is NULL");
        return;
    }
    lua_getglobal(L, "SetGamestateAttr");
    lua_pushstring(L, key);
    lua_pushnumber(L, value);
    if (lua_pcall(L, 2, 0, 0) != 0) {
        merror("libgame_lua_set_gamestate_int: failed to set gamestate int");
    }
}




void libgame_lua_mytest(lua_State* L) {
    if (!L) {
        merror("L is NULL");
        return;
    }
    lua_getglobal(L, "MyTest");
    if (lua_pcall(L, 0, 0, 0) != 0) {
        merror("libgame_lua_mytest: failed to call MyTest");
    }
}




const int libgame_lua_get_nth_action_type(lua_State* L, const int n) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetNthActionType");
    lua_pushnumber(L, n);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror(
            "libgame_lua_get_nth_action_type: failed to get nth action type");
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_nth_action_x(lua_State* L, const int n) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetNthActionX");
    lua_pushnumber(L, n);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_get_nth_action_x: failed to get nth action x");
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_nth_action_y(lua_State* L, const int n) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetNthActionY");
    lua_pushnumber(L, n);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_get_nth_action_y: failed to get nth action y");
    }
    lua_pop(L, 1);
    return retval;
}




void libgame_lua_clear_was_damaged(lua_State* L) {
    if (!L) {
        merror("L is NULL");
        return;
    }
    lua_getglobal(L, "ClearWasDamaged");
    if (lua_pcall(L, 0, 0, 0) != 0) {
        merror("libgame_lua_clear_was_damaged: failed to clear was damaged");
    }
}



void libgame_lua_print_entity_info(lua_State* L) {
    if (!L) {
        merror("L is NULL");
        return;
    }
    lua_getglobal(L, "PrintEntityInfo");
    if (lua_pcall(L, 0, 0, 0) != 0) {
        merror("libgame_lua_print_entity_info: failed to print entity info");
    }
}




const int libgame_lua_get_nth_action_id(lua_State* L, const int n) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetNthActionId");
    lua_pushnumber(L, n);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_get_nth_action_id: failed to get nth action id");
    }
    lua_pop(L, 1);
    return retval;
}



const int libgame_lua_get_inventory_count(lua_State* L, const int id) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetInventoryCount");
    lua_pushnumber(L, id);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror(
            "libgame_lua_get_inventory_count: failed to get inventory count");
    }
    lua_pop(L, 1);
    return retval;
}



const entityid libgame_lua_get_entity_shield(lua_State* L, const int id) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    entityid retval = -1;
    lua_getglobal(L, "GetEntityShield");
    lua_pushnumber(L, id);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_get_entity_shield: failed to get entity shield");
    }
    lua_pop(L, 1);
    return retval;
}




void libgame_lua_incr_current_turn(lua_State* L) {
    if (!L) {
        merror("L is NULL");
        return;
    }
    lua_getglobal(L, "IncrementCurrentTurn");
    if (lua_pcall(L, 0, 0, 0) != 0) {
        merror(
            "libgame_lua_incr_current_turn: failed to increment current turn");
    }
}



const int libgame_lua_get_move_seq_begin(lua_State* L, const int begin) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetMoveSeqBegin");
    lua_pushnumber(L, begin);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_get_move_seq_begin: failed to get move seq begin");
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_move_seq_end(lua_State* L, const int begin) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetMoveSeqEnd");
    lua_pushnumber(L, begin);
    if (lua_pcall(L, 1, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_get_move_seq_end: failed to get move seq end");
    }
    lua_pop(L, 1);
    return retval;
}



const int libgame_lua_get_action_results_count(lua_State* L) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetActionResultsCount");
    if (lua_pcall(L, 0, 1, 0) == 0) {
        retval = lua_tonumber(L, -1);
    } else {
        merror("libgame_lua_get_action_results_count: failed to get action "
               "results "
               "count");
    }
    lua_pop(L, 1);
    return retval;
}


const int
libgame_lua_get_action_result(lua_State* L, const int index, const char* key) {
    if (!L) {
        merror("L is NULL");
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
    } else {
        merror("libgame_lua_get_action_result: failed to get action result");
    }
    lua_pop(L, 1);
    return retval;
}



void libgame_lua_process_actions(lua_State* L) {
    if (!L) {
        merror("L is NULL");
        return;
    }
    lua_getglobal(L, "ProcessActions");
    if (lua_pcall(L, 0, 0, 0) != 0) {
        merror("libgame_lua_process_actions: failed to process actions");
    }
}


void libgame_lua_clear_action_results(lua_State* L) {
    if (!L) {
        merror("L is NULL");
        return;
    }
    lua_getglobal(L, "ClearActionResults");
    if (lua_pcall(L, 0, 0, 0) != 0) {
        merror(
            "libgame_lua_clear_action_results: failed to clear action results");
    }
}



void libgame_lua_update_tile_position(lua_State* L,
                                      const int dungeon_x,
                                      const int dungeon_y,
                                      const int x,
                                      const int y) {
    if (!L) {
        merror("L is NULL");
        return;
    }
    lua_getglobal(L, "UpdateTilePosition");
    lua_pushnumber(L, dungeon_x);
    lua_pushnumber(L, dungeon_y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 4, 0, 0) != 0) {
        merror(
            "libgame_lua_update_tile_position: failed to update tile position");
    }
}



const int libgame_lua_get_tile_x(lua_State* L, const int x, const int y) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    int retval = -1;
    lua_getglobal(L, "GetTileX");
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 2, 1, 0) != 0) {
        merror("libgame_lua_get_tile_x: failed to get tile x");
    } else {
        retval = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    return retval;
}




const int libgame_lua_get_tile_y(lua_State* L, const int x, const int y) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    lua_getglobal(L, "GetTileY");
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 2, 1, 0) != 0) {
        merror("libgame_lua_get_tile_y: failed to get tile y");
        return -1;
    }
    const int retval = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return retval;
}




const float
libgame_lua_get_entity_float(lua_State* L, const int id, const char* key) {
    if (!L) {
        merror("L is NULL");
        return -1;
    }
    float retval = -1;
    lua_getglobal(L, "GetEntityAttr");
    lua_pushnumber(L, id);
    lua_pushstring(L, key);
    if (lua_pcall(L, 2, 1, 0) == 0) {
        if (lua_isnumber(L, -1)) {
            retval = lua_tonumber(L, -1);
        } else {
            merror("libgame_lua_get_entity_float: value is not a number");
            retval = -1;
        }
    } else {
        merror("libgame_lua_get_entity_float: failed to get entity float");
    }
    lua_pop(L, 1);
    return retval;
}
