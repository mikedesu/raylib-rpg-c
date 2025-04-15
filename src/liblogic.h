#pragma once
#include "gamestate.h"
#include "inputstate.h"

#define TILE_COUNT_ERROR -999

//entityid liblogic_npc_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name);
//entityid liblogic_player_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name);

//int liblogic_tile_npc_count(const gamestate* const g, int x, int y, int floor);
//int liblogic_tile_npc_living_count(const gamestate* const g, int x, int y, int fl);

void liblogic_init(gamestate* const g);
void liblogic_close(gamestate* const g);
void liblogic_tick(const inputstate* const is, gamestate* const g);
//void liblogic_add_entityid(gamestate* const g, entityid id); // New
//void liblogic_update_debug_panel_buffer(gamestate* const g);
//void liblogic_handle_npc(gamestate* const g);
//void liblogic_handle_npcs(gamestate* const g);
//void liblogic_update_npc_state(gamestate* const g, entityid id);
//void liblogic_update_npcs_state(gamestate* const g);

//entity_t* const liblogic_npc_create_ptr(gamestate* const g, race_t rt, int x, int y, int fl, const char* name);
//entityid liblogic_weapon_create(gamestate* const g, int x, int y, int fl, const char* name);
//entityid liblogic_shield_create(gamestate* const g, int x, int y, int fl, const char* name);
//bool liblogic_entity_has_weapon(gamestate* const g, entityid id);
//bool liblogic_entity_has_shield(gamestate* const g, entityid id);

//void liblogic_handle_input_player(const inputstate* const is, gamestate* const g);
//void liblogic_update_player_state(gamestate* const g);
//void liblogic_change_player_dir(gamestate* const g, direction_t dir);
//void liblogic_try_flip_switch(gamestate* const g, entity* const e, int x, int y, int fl);
//void liblogic_try_entity_wait(gamestate* const g, entity* const e);
//void liblogic_try_entity_block(gamestate* const g, entity* const e);
//const char* liblogic_get_action_key(const inputstate* const is, gamestate* const g);
//bool liblogic_player_on_tile(const gamestate* const g, int x, int y, int floor);
//bool liblogic_try_entity_pickup(gamestate* const g, entity* const e);
//bool liblogic_entities_adjacent(gamestate* const g, entityid eid0, entityid eid1);
//void liblogic_handle_input(const inputstate* const is, gamestate* const g);
//void liblogic_handle_input_camera(const inputstate* const is, gamestate* const g);
//void liblogic_execute_action(gamestate* const g, entity* const e, entity_action_t action);
//void liblogic_handle_camera_zoom(gamestate* const g, const inputstate* const is);
//void liblogic_handle_camera_move(gamestate* const g, const inputstate* const is);
//void liblogic_init_orcs_test(gamestate* const g);
//void liblogic_init_player(gamestate* const g);
//void liblogic_init_em(gamestate* const g);
//void liblogic_init_dungeon(gamestate* const g);
//void liblogic_init_weapon_test(gamestate* const g);
//void liblogic_init_orcs_test_naive_loop(gamestate* const g);
//void liblogic_init_orcs_test_intermediate(gamestate* const g);
//void liblogic_try_entity_move(gamestate* const g, entity* const e, int x, int y);
//void liblogic_try_entity_move_random(gamestate* const g, entity* const e);
//void liblogic_try_entity_move_player(gamestate* const g, entity* const e);
//void liblogic_try_entity_attack_player(gamestate* const g, entity* const e);
//void liblogic_try_entity_move_attack_player(gamestate* const g, entity* const e);
//void liblogic_try_entity_attack_random(gamestate* const g, entity* const e);
//void liblogic_try_entity_attack(gamestate* const g, entityid attacker_id, int target_x, int target_y);
//void liblogic_add_message(gamestate* g, const char* text);
//void liblogic_add_message(gamestate* g, const char* fmt, ...);
//void liblogic_add_message_history(gamestate* const g, const char* msg);
