#pragma once
#include "gamestate.h"
#include "inputstate.h"

void liblogic_init(gamestate* const g);
void liblogic_tick(const inputstate* const is, gamestate* const g);
void liblogic_close(gamestate* const g);

//entityid liblogic_entity_create(gamestate* const g, entitytype_t type, int x, int y, const char* name);
//const entityid liblogic_entity_create(
//    gamestate* const g, const entitytype_t type, const int x, const int y, const int floor, const char* name);
const entityid liblogic_npc_create(gamestate* const g,
                                   const race_t race_type,
                                   const int x,
                                   const int y,
                                   const int floor,
                                   const char* name);

const entityid liblogic_player_create(gamestate* const g,
                                      const race_t race_type,
                                      const int x,
                                      const int y,
                                      const int floor,
                                      const char* name);


void liblogic_add_entityid(gamestate* const g, entityid id); // New
void liblogic_handle_input(const inputstate* const is, gamestate* const g);
void liblogic_handle_input_player(const inputstate* const is, gamestate* const g);
void liblogic_handle_input_camera(const inputstate* const is, gamestate* const g);
void liblogic_update_debug_panel_buffer(gamestate* const g);
//void liblogic_try_entity_move_left(gamestate* const g, entity* const e);
//void liblogic_try_entity_move_right(gamestate* const g, entity* const e);
//void liblogic_try_entity_move_up(gamestate* const g, entity* const e);
//void liblogic_try_entity_move_down(gamestate* const g, entity* const e);
void liblogic_try_entity_move(gamestate* const g, entity* const e, int x, int y);
const int liblogic_tile_npc_count(const gamestate* const g, const int x, const int y, const int floor);
void liblogic_try_entity_attack(gamestate* const g, entityid attacker_id, int target_x, int target_y);
void liblogic_handle_npc(gamestate* const g);
const bool liblogic_player_on_tile(const gamestate* const g, const int x, const int y, const int floor);
