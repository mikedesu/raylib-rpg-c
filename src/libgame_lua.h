#pragma once

#include "mylua.h"
#include <stdbool.h>

const bool libgame_lua_set_entity_int(lua_State* L, const int id, const char* key, const int value);
const bool libgame_lua_set_entity_str(lua_State* L, const int id, const char* key, const char* value);
const bool libgame_lua_entity_move(lua_State* L, const int id, const int x, const int y);
const bool libgame_lua_entity_move_random_dir(lua_State* L, const int id);
const bool libgame_lua_create_dungeonfloor(lua_State* L, const int width, const int height, const int type);
const bool libgame_lua_tile_is_occupied_by_npc(lua_State* L, const int x, const int y);
const bool libgame_lua_set_tiletype(lua_State* L, const int type, const int x, const int y);
const bool libgame_lua_create_action(lua_State* L, const int entity_id, const int type, const int x, const int y);

const int libgame_lua_get_int(lua_State* L, const char* key);
const int libgame_lua_create_entity(lua_State* L, const char* name, const int type, const int x, const int y);
const int libgame_lua_get_entity_int(lua_State* L, const int id, const char* key);
const int libgame_lua_get_dungeonfloor_row_count(lua_State* L);
const int libgame_lua_get_dungeonfloor_col_count(lua_State* L);
const int libgame_lua_get_tiletype(lua_State* L, const int x, const int y);
const int libgame_lua_get_num_entities_at(lua_State* L, const int x, const int y);
const int libgame_lua_get_nth_entity_at(lua_State* L, const int n, const int x, const int y);
const int libgame_lua_get_action_count(lua_State* L);
const int libgame_lua_process_action(lua_State* L, const int index);
const int libgame_lua_get_num_entities(lua_State* L);
const int libgame_lua_get_nth_entity(lua_State* L, const int n);
const int libgame_lua_get_gamestate_int(lua_State* L, const char* key);
const int libgame_lua_get_nth_action_type(lua_State* L, const int n);
const int libgame_lua_get_nth_action_id(lua_State* L, const int n);

const char* libgame_lua_get_gamestate_str(lua_State* L, const char* key);
const char* libgame_lua_get_entity_str(lua_State* L, const int id, const char* key);
const char* libgame_lua_get_str(lua_State* L, const char* key);

void libgame_lua_set_int(lua_State* L, const char* key, const int value);
void libgame_lua_clear_actions(lua_State* L);
void libgame_lua_randomize_all_dungeon_tiles(lua_State* L);
void libgame_lua_randomize_dungeon_tiles(lua_State* L, const int x, const int y, const int w, const int h);
void libgame_lua_set_gamestate_int(lua_State* L, const char* key, const int value);
void libgame_lua_mytest(lua_State* L);
void libgame_lua_clear_was_damaged(lua_State* L);
void libgame_lua_print_entity_info(lua_State* L);
const int libgame_lua_get_nth_action_x(lua_State* L, const int n);
const int libgame_lua_get_nth_action_y(lua_State* L, const int n);


const int libgame_lua_get_inventory_count(lua_State* L, const int id);

//void libgame_lua_print_entities(lua_State* L);
//void libgame_lua_serialize_entities_and_print(lua_State* L);
//void libgame_lua_reserialization_test(lua_State* L);
//void libgame_lua_process_actions(lua_State* L);
//const bool libgame_lua_tile_is_occupied_by_player(lua_State* L, const int x, const int y);
//const bool libgame_lua_actions_exist(lua_State* L);
