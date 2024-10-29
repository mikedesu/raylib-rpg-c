#pragma once

#include "direction.h"
#include "entity.h"
#include "entitytype.h"
#include "gamestate.h"
#include "mylua.h"
// Asset packing
//#include "img_data_pack.h"
#include <raylib.h>

bool libgame_windowshouldclose();

gamestate* libgame_getgamestate();

void libgame_calc_debugpanel_size(gamestate* g);

const entityid libgame_create_hero_lua(gamestate* g, const char* name, const int x, const int y);
//void libgame_create_shield(gamestate* g, const char* name, const int x, const int y);
const entityid libgame_create_buckler_lua(gamestate* g, const char* name, const int x, const int y);
void libgame_create_spritegroup(gamestate* g, entityid id, int* keys, int num_keys, int offset_x, int offset_y, specifier_t spec);
void libgame_create_spritegroup_by_id(gamestate* g, entityid id);
void libgame_closeshared(gamestate* g);
void libgame_closesavegamestate();
void libgame_closewindow();
void libgame_close(gamestate* g);
void libgame_draw_debug_panel(gamestate* g);
void libgame_draw_company_scene(gamestate* g);
void libgame_draw_title_scene(gamestate* g);
void libgame_draw_gameplayscene(gamestate* g);
void libgame_draw_fade(gamestate* g);
void libgame_drawgrid(gamestate* g);
void libgame_draw_dungeonfloor(gamestate* g);
void libgame_drawframeend(gamestate* g);
void libgame_drawtorchgroup(gamestate* g);
void libgame_do_camera_lock_on(gamestate* g);
void libgame_draw_entity(gamestate* g, const entityid id);
void libgame_drawframe(gamestate* g);
void libgame_draw_entities_at(gamestate* g, const entitytype_t type, const int x, const int y);
void libgame_draw_entities_at_lua(gamestate* g, const entitytype_t type, const int x, const int y);
void libgame_entity_anim_incr(gamestate* g, const entityid id);
void libgame_entity_anim_set(gamestate* g, const entityid id, const int index);
void libgame_handle_input_player(gamestate* g);
void libgame_handle_caminput(gamestate* g);
void libgame_handleinput(gamestate* g);
void libgame_handle_debugpanel_switch(gamestate* g);
void libgame_handle_modeswitch(gamestate* g);
void libgame_handle_fade(gamestate* g);
void libgame_handleplayerinput_move(gamestate* g, const int xdir, const int ydir);
void libgame_handle_player_input_movement_key(gamestate* g, const direction_t dir);
void libgame_handle_grid_switch(gamestate* g);
void libgame_handle_npc_turn_lua(gamestate* g, const entityid id);
void libgame_handle_npcs_turn_lua(gamestate* g);
void libgame_initwindow(gamestate* g);
void libgame_initwithstate(gamestate* state);
void libgame_initsharedsetup(gamestate* g);
void libgame_init_datastructures(gamestate* g);
void libgame_init();
//void libgame_load_textures_from_data(gamestate* g);
void libgame_load_texture_from_disk(gamestate* g, int index, int contexts, int frames, bool dodither, const char* path);
//void libgame_load_texture_from_data_pack(gamestate* g, img_data_pack_t* pack);
void libgame_loadtargettexture(gamestate* g);
void libgame_loadfont(gamestate* g);
void libgame_load_textures(gamestate* g);
void libgame_process_turn(gamestate* g);
void libgame_process_turn_actions(gamestate* g);
void libgame_test_enemy_placement(gamestate* g);
void libgame_unloadtexture(gamestate* g, int index);
void libgame_unloadtextures(gamestate* g);
void libgame_update_smoothmove(gamestate* g, const entityid id);
void libgame_update_spritegroup_move(gamestate* g, const entityid id, const int x, const int y);

void libgame_update_debug_panel_buffer(gamestate* g);
void libgame_update_gamestate(gamestate* g);
void libgame_update_smoothmoves_for_entitytype(gamestate* g, const entitytype_t type);

//void libgame_update_spritegroup(gamestate* g, entityid id, direction_t dir);
void libgame_update_spritegroup(gamestate* g, const entityid id, const specifier_t spec, const direction_t dir);

void libgame_update_spritegroup_by_lastmove(gamestate* g, const entityid entity_id);
const int libgame_get_x_from_dir(const direction_t dir);
const int libgame_get_y_from_dir(const direction_t dir);
const direction_t libgame_get_dir_from_xy(const int xdir, const int ydir);
void libgame_update_entities_damaged_anim(gamestate* g);
void libgame_update_entity_damaged_anim(gamestate* g, const int i);
//void libgame_update_anim_indices(gamestate* g);
void libgame_set_default_anim_for_id(gamestate* g, const entityid id, const int anim);
//void libgame_set_default_anim_for_id(gamestate* g, entityid id, int anim);
const char* libgame_get_str_from_dir(const direction_t dir);
//void libgame_update_spritegroup_current(gamestate* g, entityid id);
void libgame_update_spritegroup_current(gamestate* g, entityid id, int index);
void libgame_reset_entities_anim(gamestate* g);
//void libgame_reset_spritegroup_on_loop(gamestate* g, const entityid id);
void libgame_load_textures_from_data(gamestate* g);
void libgame_handle_player_input_attack_key(gamestate* g);
void libgame_handle_player_input_pickup_key(gamestate* g);
void libgame_handle_player_input_block_key(gamestate* g);
void libgame_draw_entity_shadow(gamestate* g, const entityid id);
void libgame_draw_entity_shield_back(gamestate* g, const entityid id);
void libgame_draw_entity_shield_front(gamestate* g, const entityid id);
void libgame_init_sound();
//void libgame_draw_entity_incr_frame(gamestate* g, const entityid id);


//#ifdef MOBILE
//void libgame_handle_input_player_mobile(gamestate* g);
//#endif

const bool libgame_entity_move_lua(gamestate* g, entityid id, int x, int y);
const bool libgame_entity_move_check(gamestate* g, entity_t* e, int x, int y);
const bool libgame_entity_inventory_contains_type(gamestate* g, entityid id, itemtype_t type);
const bool libgame_external_check_reload();

const entityid libgame_create_orc_lua(gamestate* g, const char* name, const int x, const int y);
const entityid libgame_create_buckler_lua(gamestate* g, const char* name, const int x, const int y);



//void libgame_draw_items(gamestate* g, const itemtype_t type, const int x, const int y);
//void libgame_draw_items_that_are_not(gamestate* g, const itemtype_t type, const int x, const int y);
//const bool libgame_itemtype_is_at(gamestate* g, const itemtype_t type, const int x, const int y);
//const bool libgame_entitytype_is_at(gamestate* g, const entitytype_t type, const int x, const int y);
//lua_State* libgame_getlua();
//const bool libgame_is_tile_occupied_with_entitytype(gamestate* g, const entitytype_t type, const int x, const int y);
//void libgame_create_sword_spritegroup(gamestate* g, entityid id, const float off_x, const float off_y);
//void libgame_create_sword(gamestate* g, const char* name, const int x, const int y);
//const entityid libgame_create_entity(gamestate* g, const char* name, const entitytype_t type, const int x, const int y);
//void libgame_handle_player_attack(gamestate* g);
//const bool libgame_entity_try_attack(gamestate* g, entityid id, const int x, const int y);
//const char* libgame_lua_get_str(const char* key);
//const int libgame_lua_get_int(const char* key);
//const bool libgame_entity_move(gamestate* g, entityid id, int x, int y);
//const entityid libgame_entity_pickup_item(gamestate* g, const entityid id);
//void libgame_create_orc(gamestate* g, const char* name, const int x, const int y);
//void libgame_create_hero(gamestate* g, const char* name, const int x, const int y);
//void libgame_createitembytype(gamestate* g, const itemtype_t type, const int x, const int y);
//void libgame_entity_look(gamestate* g, entityid id);
//void libgame_handle_npc_turn(gamestate* g, entityid id);
//void libgame_create_torch_spritegroup(gamestate* g, entityid id, const float off_x, const float off_y);
//void libgame_do_one_camera_rotation(gamestate* g);
//void libgame_updateherospritegroup_right(gamestate* g);
//void libgame_updateherospritegroup_left(gamestate* g);
//void libgame_update_spritegroup_right(gamestate* g, entityid id);
//void libgame_update_spritegroup_left(gamestate* g, entityid id);
//void libgame_update_spritegroup_up(gamestate* g, entityid id);
//void libgame_update_spritegroup_down(gamestate* g, entityid id);
//void libgame_updateherospritegroup_up(gamestate* g);
//void libgame_updateherospritegroup_down(gamestate* g);
//void libgame_create_herospritegroup(gamestate* g, entityid id);
//void libgame_create_orcspritegroup(gamestate* g, entityid id);
//void libgame_create_shield_spritegroup(gamestate* g, entityid id, const int off_x, const int off_y);
//void libgame_close_shared_minus_lua(gamestate* g);
//void libgame_loadtexture(gamestate* g, int index, int contexts, int frames, bool dodither, const char* path);
//void libgame_load_textures_from_file(gamestate* g, const char* path);
//void libgame_load_texture_from_data(
//    gamestate* g, unsigned char* d, const int w, const int h, const int format, const int idx, const int ctxs, const int frames);
//void libgame_load_texture_from_data_test(gamestate* g, int index, int contexts, int frames);
//void libgame_handleplayerinput_key_right(gamestate* g);
//void libgame_handleplayerinput_key_left(gamestate* g);
//void libgame_handleplayerinput_key_down(gamestate* g);
//void libgame_handleplayerinput_key_up(gamestate* g);
//void libgame_handleplayerinput_key_down_left(gamestate* g);
//void libgame_handleplayerinput_key_up_left(gamestate* g);
//void libgame_handleplayerinput_key_up_right(gamestate* g);
//void libgame_handleplayerinput_key_down_right(gamestate* g);
//void libgame_init_dungeonfloor_random_tiles_unsafe(gamestate* g);
//void libgame_init_dungeonfloor_random_tiles(gamestate* g);
//void libgame_init_dungeonfloor(gamestate* g);
