#pragma once

#include "actionresults.h"
#include "actions.h"
#include "direction.h"
#include "entitytype.h"
#include "gamestate.h"
#include "mylua.h"
#include "race.h"
#include <raylib.h>

bool libgame_windowshouldclose();
const bool libgame_external_check_reload();
const bool libgame_entity_set_anim(gamestate* const g, const entityid id, const int index);
const bool libgame_create_entity_checks(gamestate* const g, const int x, const int y);

gamestate* libgame_getgamestate();
void libgame_closesavegamestate();
void libgame_closewindow();
void libgame_init();
void libgame_update_gamestate(gamestate* g);
void libgame_drawframe(gamestate* g);
void libgame_close(gamestate* g);
void libgame_handle_player_wait_key(gamestate* const g);
void libgame_initsharedsetup(gamestate* const g);

//const entityid libgame_create_buckler(gamestate* const g,
//                                      const char* name,
//                                      const int x,
//                                      const int y);
//const entityid libgame_create_orc(gamestate* const g,
//                                  const char* name,
//                                  const int x,
//                                  const int y);
const int libgame_get_x_from_dir(const direction_t dir);
const int libgame_get_y_from_dir(const direction_t dir);
const int
libgame_entity_update_context(gamestate* const g, const entityid id, const specifier_t spec, const direction_t dir);

const int libgame_entity_update_context_unsafe(gamestate* const g,
                                               const entityid id,
                                               const specifier_t spec,
                                               const direction_t dir);
const bool libgame_entity_set_anim_unsafe(gamestate* const g, const entityid id, const int index);
void libgame_handle_move_unsafe(gamestate* const g, const entityid id, const int x, const int y);
void libgame_handle_caminput_unsafe(gamestate* const g);


//const entityid libgame_create_entity(gamestate* const g, const char* name);
const direction_t libgame_get_dir_from_xy(const int xdir, const int ydir);

void libgame_create_spritegroup(gamestate* const g,
                                const entityid id,
                                int* keys,
                                const int num_keys,
                                const int offset_x,
                                const int offset_y,
                                const specifier_t spec);
void libgame_load_textures(gamestate* const g);
void libgame_unloadtextures(gamestate* const g);
void libgame_set_default_anim_for_id(gamestate* const g, const entityid id, const int anim);
void libgame_reset_entities_anim(gamestate* const g);
void libgame_unloadtexture(gamestate* const g, const int index);
void libgame_do_camera_lock_on(gamestate* const g);
void libgame_draw_entity(gamestate* const g, const entityid id);
void libgame_draw_entities_at_lua(gamestate* const g, const entitytype_t type, const int x, const int y);
void libgame_handle_input_player(gamestate* const g);
void libgame_handle_caminput(gamestate* const g);
void libgame_handle_input(gamestate* const g);
void libgame_handle_debugpanel_switch(gamestate* const g);
void libgame_handle_modeswitch(gamestate* const g);
void libgame_handle_fade(gamestate* const g);
void libgame_get_hero_xy(gamestate* const g, int* x, int* y);
void libgame_handle_grid_switch(gamestate* const g);
void libgame_handle_npc_turn_lua(gamestate* const g, const entityid id);
void libgame_handle_npcs_turn_lua(gamestate* const g);
void libgame_reset_entity_anim(gamestate* const g, entityid id);
void libgame_initwindow(gamestate* const g);
void libgame_initwithstate(gamestate* const state);
void libgame_load_texture_from_disk(
    gamestate* const g, const int index, const int contexts, const int frames, const bool dodither, const char* path);
void libgame_process_turn_begin_end(gamestate* const g, const int begin, const int end);
void libgame_test_enemy_placement(gamestate* const g);
void libgame_update_smoothmove(gamestate* const g, const entityid id);
void libgame_update_debug_panel_buffer(gamestate* const g);
void libgame_get_entity_xy(gamestate* const g, const entityid id, int* x, int* y);
const char* libgame_get_str_from_dir(const direction_t dir);
void libgame_draw_entity_shadow(gamestate* const g, const entityid id);
void libgame_draw_entity_incr_frame(gamestate* const g, const entityid id);
void libgame_draw_gameplayscene_entities(gamestate* const g);
void libgame_load_textures_from_disk(gamestate* const g);
void libgame_calc_debugpanel_size(gamestate* const g);
void libgame_closeshared(gamestate* const g);
void libgame_draw_debug_panel(gamestate* const g);
void libgame_draw_company_scene(gamestate* const g);
void libgame_draw_title_scene(gamestate* const g);
void libgame_draw_gameplayscene(gamestate* const g);
void libgame_draw_fade(const gamestate* const g);
//void libgame_drawgrid(gamestate* const g);
void libgame_draw_dungeon_floor(gamestate* const g);
void libgame_draw_dungeon_floor_3d(gamestate* const g);
void libgame_drawframeend(gamestate* const g);
void libgame_incr_current_action(gamestate* const g);
void libgame_update_hero_shield_spritegroup(gamestate* const g);
void libgame_handle_test_enemy_placement(gamestate* const g);
void libgame_test_enemy_placement(gamestate* const g);

void libgame_create_entity(
    gamestate* const g, const int x, const int y, entitytype_t type, race_t race, const char* name);
void libgame_create_entity_unsafe(
    gamestate* const g, const int x, const int y, entitytype_t type, race_t race, const char* name);
void libgame_create_npc_orc_unsafe(gamestate* const g, const int x, const int y);
void libgame_create_npc_orc_test_unsafe(gamestate* const g);


void libgame_init_datastructures(gamestate* const g);
void libgame_init_spritegroups(gamestate* const g);
void libgame_precompute_dungeonfloor_tile_positions(gamestate* const g);
//void libgame_update_spritegroup_dest(gamestate* const g, const entityid id);
void libgame_update_all_entity_sg_dests(gamestate* const g);
void libgame_init_entityids(gamestate* const g);
void libgame_handle_move(gamestate* const g, const entityid id, const int x, const int y);
void libgame_draw_dungeon_floor_entities(gamestate* const g);
void libgame_draw_dungeon_floor_entities_unsafe(gamestate* const g);
void libgame_draw_dungeon_floor_entities_3d_unsafe(gamestate* const g);
//void libgame_draw_dungeon_floor_tiles(gamestate* const g);
//void libgame_draw_dungeon_floor_tiles_unsafe(gamestate* const g);
void libgame_draw_dungeon_floor_tiles_unsafe_floors(gamestate* const g);
void libgame_draw_dungeon_floor_tiles_unsafe_walls(gamestate* const g);
void libgame_draw_dungeon_floor_tiles_3d_unsafe(gamestate* const g);
void libgame_set_race_specific_parameters(entity* e);
void libgame_create_npc_orc(gamestate* const g, const int x, const int y);
//void libgame_create_npc_orc_test(gamestate* const g);
void libgame_handle_caminput_zoom(gamestate* const g);
void libgame_set_tile_params(gamestate* const g, dungeon_tile_type_t type, int i, int j);
//void libgame_init_entity_list(gamestate* const state);
//bool libgame_assign_entity_position(gamestate* const g, const entityid id, const int x, const int y);
//const int libgame_get_entity_pos_x(gamestate const* g, const entityid id);
//const int libgame_get_entity_pos_y(gamestate const* g, const entityid id);
//const int libgame_create_spritegroup_by_id(gamestate* const g, const entityid id, const direction_t dir);
//void libgame_create_hero(gamestate* const g, const int x, const int y);
//void libgame_create_hero(gamestate* const g, const int x, const int y, race_t race, const char* name);
//
//
//const bool libgame_handle_sprite_update(gamestate* const g,
//                                        const entityid actor_id,
//                                        const actionresults_t actionresults_type,
//                                        const int xdir,
//                                        const int ydir);
