#pragma once

#include "direction.h"
#include "entitytype.h"
#include "gamestate.h"
#include "mylua.h"
// Asset packing
#include <raylib.h>


//void libgame_drawtorchgroup(gamestate* g);
//const entityid libgame_create_buckler_lua(gamestate* g, const char* name, const int x, const int y);
//const bool libgame_entity_move_lua(gamestate* g, entityid id, int x, int y);
//void libgame_load_textures_from_data(gamestate* g);
//void libgame_draw_entities_at(gamestate* g, const entitytype_t type, const int x, const int y);
//void libgame_loadtargettexture(gamestate* g);
//void libgame_loadfont(gamestate* const g);
//void libgame_init_datastructures(gamestate* g);
//void libgame_handleplayerinput_move(gamestate* const g, const int xdir, const int ydir);

bool libgame_windowshouldclose();
gamestate* libgame_getgamestate();
void libgame_closesavegamestate();
void libgame_closewindow();
void libgame_init();
const bool libgame_external_check_reload();

void libgame_update_gamestate(gamestate* g);
void libgame_drawframe(gamestate* g);
void libgame_close(gamestate* g);

void libgame_handle_player_wait_key(gamestate* const g);
void libgame_initsharedsetup(gamestate* const g);
void libgame_create_spritegroup_by_id(gamestate* const g, const entityid id);
void libgame_create_spritegroup(gamestate* const g, const entityid id, int* keys, const int num_keys, const int offset_x, const int offset_y, const specifier_t spec);
const entityid libgame_create_buckler_lua(gamestate* const g, const char* name, const int x, const int y);
void libgame_load_textures(gamestate* const g);
void libgame_unloadtextures(gamestate* const g);
void libgame_set_default_anim_for_id(gamestate* const g, const entityid id, const int anim);
void libgame_reset_entities_anim(gamestate* const g);
const entityid libgame_create_orc_lua(gamestate* const g, const char* name, const int x, const int y);
void libgame_unloadtexture(gamestate* const g, const int index);
const entityid libgame_create_hero_lua(gamestate* const g, const char* name, const int x, const int y);
void libgame_do_camera_lock_on(gamestate* const g);
void libgame_draw_entity(gamestate* const g, const entityid id);
void libgame_draw_entities_at_lua(gamestate* const g, const entitytype_t type, const int x, const int y);
//void libgame_entity_anim_incr(gamestate* const g, const entityid id);

const bool libgame_entity_anim_set(gamestate* const g, const entityid id, const int index);
void libgame_process_turn_action(gamestate* const g, const int i);

void libgame_handle_input_player(gamestate* const g);
void libgame_handle_caminput(gamestate* const g);
void libgame_handleinput(gamestate* const g);
void libgame_handle_debugpanel_switch(gamestate* const g);
void libgame_handle_modeswitch(gamestate* const g);
void libgame_handle_fade(gamestate* const g);
void libgame_handle_player_input_movement_key(gamestate* const g, const direction_t dir);
void libgame_handle_grid_switch(gamestate* const g);
void libgame_handle_npc_turn_lua(gamestate* const g, const entityid id);
void libgame_handle_npcs_turn_lua(gamestate* const g);
void libgame_reset_entity_anim(gamestate* const g, entityid id);
void libgame_initwindow(gamestate* const g);
void libgame_initwithstate(gamestate* const state);
void libgame_load_texture_from_disk(gamestate* const g, const int index, const int contexts, const int frames, const bool dodither, const char* path);
//void libgame_process_turn(gamestate* const g);
//void libgame_process_turn_actions(gamestate* const g);
void libgame_test_enemy_placement(gamestate* const g);
void libgame_update_smoothmove(gamestate* const g, const entityid id);
void libgame_update_spritegroup_move(gamestate* const g, const entityid id, const int x, const int y);
void libgame_update_debug_panel_buffer(gamestate* const g);
void libgame_update_smoothmoves_for_entitytype(gamestate* const g, const entitytype_t type);
void libgame_update_spritegroup(gamestate* const g, const entityid id, const specifier_t spec, const direction_t dir);
void libgame_update_spritegroup_by_lastmove(gamestate* const g, const entityid entity_id);
const int libgame_get_x_from_dir(const direction_t dir);
const int libgame_get_y_from_dir(const direction_t dir);
const direction_t libgame_get_dir_from_xy(const int xdir, const int ydir);
void libgame_update_entities_damaged_anim(gamestate* const g);
void libgame_update_entity_damaged_anim(gamestate* const g, const int i);
const char* libgame_get_str_from_dir(const direction_t dir);
void libgame_update_spritegroup_current(gamestate* const g, entityid id, int index);
void libgame_handle_player_input_attack_key(gamestate* const g);
void libgame_handle_player_input_pickup_key(gamestate* const g);
void libgame_handle_player_input_block_key(gamestate* const g);
void libgame_draw_entity_shadow(gamestate* const g, const entityid id);
void libgame_draw_entity_shield_back(gamestate* const g, const entityid id);
void libgame_draw_entity_shield_front(gamestate* const g, const entityid id);
void libgame_draw_entity_incr_frame(gamestate* const g, const entityid id);
void libgame_draw_gameplayscene_entities(gamestate* const g);
void libgame_load_textures_from_disk(gamestate* const g);
void libgame_calc_debugpanel_size(gamestate* const g);
void libgame_closeshared(gamestate* const g);
void libgame_draw_debug_panel(gamestate* const g);
void libgame_draw_title_scene(gamestate* const g);
void libgame_draw_gameplayscene(gamestate* const g);
void libgame_draw_fade(const gamestate* const g);
void libgame_drawgrid(gamestate* const g);
void libgame_draw_dungeonfloor(gamestate* const g);
void libgame_drawframeend(gamestate* const g);
void libgame_incr_current_action(gamestate* const g);
//void libgame_incr_current_action_key(gamestate* const g);
