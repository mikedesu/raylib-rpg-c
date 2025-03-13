#pragma once
#include "gamestate.h"
#include "inputstate.h"

void liblogic_init(gamestate* const g);
void liblogic_tick(const inputstate* const is, gamestate* const g);
void liblogic_close(gamestate* const g);

entityid liblogic_entity_create(gamestate* const g, entitytype_t type, int x, int y, const char* name);
void liblogic_add_entityid(gamestate* const g, entityid id); // New
void liblogic_handle_input(const inputstate* const is, gamestate* const g);
void liblogic_handle_input_player(const inputstate* const is, gamestate* const g);
void liblogic_handle_input_camera(const inputstate* const is, gamestate* const g);
void liblogic_update_debug_panel_buffer(gamestate* const g);
void liblogic_try_entity_move_left(gamestate* const g, entity* const e);
void liblogic_try_entity_move_right(gamestate* const g, entity* const e);
void liblogic_try_entity_move_up(gamestate* const g, entity* const e);
void liblogic_try_entity_move_down(gamestate* const g, entity* const e);
