#pragma once
#include "gamestate.h"
#include "inputstate.h"

#define TILE_COUNT_ERROR -999


entityid liblogic_npc_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name);

entityid liblogic_player_create(gamestate* const g, race_t rt, int x, int y, int fl, const char* name);


const bool liblogic_player_on_tile(const gamestate* const g, const int x, const int y, const int floor);

const int liblogic_tile_npc_count(const gamestate* const g, const int x, const int y, const int floor);
//const int liblogic_tile_npc_living_count(const gamestate* const g, const int x, const int y, const int floor);
int liblogic_tile_npc_living_count(const gamestate* const g, int x, int y, int fl);

void liblogic_init(gamestate* const g);
void liblogic_close(gamestate* const g);
void liblogic_tick(const inputstate* const is, gamestate* const g);
void liblogic_add_entityid(gamestate* const g, entityid id); // New
void liblogic_handle_input(const inputstate* const is, gamestate* const g);
void liblogic_handle_input_player(const inputstate* const is, gamestate* const g);
void liblogic_handle_input_camera(const inputstate* const is, gamestate* const g);
void liblogic_update_debug_panel_buffer(gamestate* const g);
void liblogic_try_entity_move(gamestate* const g, entity* const e, int x, int y);
void liblogic_try_entity_attack(gamestate* const g, entityid attacker_id, int target_x, int target_y);
void liblogic_handle_npc(gamestate* const g);
void liblogic_handle_npcs(gamestate* const g);
void liblogic_update_player_state(gamestate* const g);
void liblogic_try_flip_switch(gamestate* const g, int x, int y, int fl);
void liblogic_update_npc_state(gamestate* const g, entityid id);
void liblogic_update_npcs_state(gamestate* const g);
void liblogic_execute_action(gamestate* const g, entity* const e, entity_action_t action);

//void liblogic_camera_lock_on(gamestate* const g);
//int liblogic_tile_npc_dead_count(const gamestate* const g, int x, int y, int floor);
