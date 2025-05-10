#pragma once

#include "component.h"
#include "component_table.h"
#include "controlmode.h"
#include "debugpanel.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "em.h"
#include "entityid.h"
#include "fadestate.h"
#include "gamestate_flag.h"
#include "keybinding.h"
#include <raylib.h>
#include <stdbool.h>
#include <time.h>

#define GAMESTATE_SIZEOFTIMEBUF 64
#define GAMESTATE_SIZEOFDEBUGPANELBUF 1024
#define GAMESTATE_SIZEOFTEXINFOARRAY 1024
#define MAX_MESSAGES 64
#define MAX_MSG_LENGTH 256

#define LIST_INIT_CAPACITY 16

#define DEFAULT_MAX_HISTORY_SIZE 1024

typedef struct {
    char messages[MAX_MESSAGES][MAX_MSG_LENGTH];
    int count; // Total messages in queue
    int index; // Current message being shown
    bool is_active; // Blocks input when true
} message_system;

typedef struct {
    char** messages;
    int count;
    int max_count;
} message_history;

typedef struct gamestate {
    int framecount, fadealpha, camera_mode, targetwidth, targetheight, windowwidth, windowheight, lock, turn_count;

    controlmode_t controlmode;
    entityid hero_id;
    time_t timebegan, currenttime;

    struct tm* timebegantm;
    struct tm* currenttimetm;

    char timebeganbuf[GAMESTATE_SIZEOFTIMEBUF], currenttimebuf[GAMESTATE_SIZEOFTIMEBUF];

    bool debugpanelon, gridon, cam_lockon, player_input_received, is_locked, processing_actions, is3d, gameover, player_changing_direction, test_guard,
        display_inventory_menu;

    int inventory_menu_selection;

    debugpanel_t debugpanel;

    Camera2D cam2d;
    Camera3D cam3d;

    fadestate_t fadestate;

    Font font;
    int font_size, pad;
    float line_spacing;

    dungeon_t* d;

    em_t* entitymap;

    entityid* entityids;
    int index_entityids, max_entityids;

    double last_frame_time;
    char frame_time_str[32];

    gamestate_flag_t flag;

    entityid entity_turn;

    message_system msg_system;
    message_history msg_history;

    keybinding_list_t keybinding_list;

    ct* components;

    name_component* name_list;
    type_component* type_list;
    race_component* race_list;
    direction_component* direction_list;
    loc_component* loc_list;
    sprite_move_component* sprite_move_list;
    dead_component* dead_list;
    update_component* update_list;
    attacking_component* attacking_list;
    blocking_component* blocking_list;
    block_success_component* block_success_list;
    damaged_component* damaged_list;
    inventory_component* inventory_list;
    equipped_weapon_component* equipped_weapon_list;
    equipped_shield_component* equipped_shield_list;
    target_component* target_list;
    target_path_component* target_path_list;

    int name_list_count, name_list_capacity, type_list_count, type_list_capacity, race_list_count, race_list_capacity, direction_list_count,
        direction_list_capacity, loc_list_count, loc_list_capacity, sprite_move_list_count, sprite_move_list_capacity, dead_list_count, dead_list_capacity,
        update_list_count, update_list_capacity, attacking_list_count, attacking_list_capacity, blocking_list_count, blocking_list_capacity,
        block_success_list_count, block_success_list_capacity, damaged_list_count, damaged_list_capacity, inventory_list_count, inventory_list_capacity,
        equipped_weapon_list_count, equipped_weapon_list_capacity, equipped_shield_list_count, equipped_shield_list_capacity, target_list_count,
        target_list_capacity, target_path_list_count, target_path_list_capacity;

} gamestate;

gamestate* gamestateinitptr();

entityid gamestate_get_entityid_unsafe(const gamestate* const g, int index);
entityid gamestate_get_hero_id(const gamestate* const g);
int gamestate_get_entityid_index(const gamestate* const g, entityid id);
int gamestate_get_next_npc_entityid_from_index(const gamestate* const g, int index);

em_t* gamestate_get_entitymap(gamestate* const g);

bool gs_add_entityid(gamestate* const g, entityid id);
bool gamestate_init_msg_history(gamestate* const g);
bool gamestate_free_msg_history(gamestate* const g);

bool gamestate_add_msg_history(gamestate* const g, const char* msg);

void gamestatefree(gamestate* g);
//void gamestate_dungeon_destroy(gamestate* const g);
void gamestate_init_entityids(gamestate* const g);
void gamestate_set_hero_id(gamestate* const g, entityid id);
void gamestate_set_debug_panel_pos_bottom_left(gamestate* const g);
void gamestate_set_debug_panel_pos_top_right(gamestate* const g);
void gamestate_incr_entity_turn(gamestate* const g);
void gamestate_load_keybindings(gamestate* const g);

bool g_has_component(const gamestate* const g, entityid id, component comp);
bool g_register_comp(gamestate* const g, entityid id, component comp);
bool g_register_comps(gamestate* const g, entityid id, ...);

bool g_add_name(gamestate* const g, entityid id, const char* name);
const char* g_get_name(gamestate* const g, entityid id);

bool g_add_type(gamestate* const g, entityid id, entitytype_t type);
entitytype_t g_get_type(const gamestate* const g, entityid id);
bool g_is_type(const gamestate* const g, entityid id, entitytype_t type);
bool g_has_type(const gamestate* const g, entityid id);

race_t g_get_race(gamestate* const g, entityid id);
bool g_add_race(gamestate* const g, entityid id, race_t race);
bool g_is_race(gamestate* const g, entityid id, race_t race);
bool g_has_race(const gamestate* const g, entityid id);

direction_t g_get_direction(const gamestate* const g, entityid id);
bool g_add_direction(gamestate* const g, entityid id, direction_t dir);
bool g_is_direction(gamestate* const g, entityid id, direction_t dir);
bool g_update_direction(gamestate* const g, entityid id, direction_t dir);
bool g_has_direction(const gamestate* const g, entityid id);

bool g_has_location(const gamestate* const g, entityid id);
bool g_add_location(gamestate* const g, entityid id, loc_t loc);
bool g_update_location(gamestate* const g, entityid id, loc_t loc);
loc_t g_get_location(const gamestate* const g, entityid id);
bool g_is_location(const gamestate* const g, entityid id, loc_t loc);

bool g_has_sprite_move(const gamestate* const g, entityid id);
bool g_add_sprite_move(gamestate* const g, entityid id, loc_t loc);
bool g_update_sprite_move(gamestate* const g, entityid id, loc_t loc);
loc_t g_get_sprite_move(const gamestate* const g, entityid id);

bool g_has_dead(const gamestate* const g, entityid id);
bool g_add_dead(gamestate* const g, entityid id, bool dead);
bool g_update_dead(gamestate* const g, entityid id, bool dead);
bool g_is_dead(const gamestate* const g, entityid id);

bool g_has_update(const gamestate* const g, entityid id);
bool g_add_update(gamestate* const g, entityid id, bool update);
bool g_get_update(gamestate* const g, entityid id);
bool g_set_update(gamestate* const g, entityid id, bool update);

bool g_has_attacking(const gamestate* const g, entityid id);
bool g_add_attacking(gamestate* const g, entityid id, bool attacking);
bool g_set_attacking(gamestate* const g, entityid id, bool attacking);
bool g_get_attacking(const gamestate* const g, entityid id);

bool g_has_blocking(const gamestate* const g, entityid id);
bool g_add_blocking(gamestate* const g, entityid id, bool blocking);
bool g_set_blocking(gamestate* const g, entityid id, bool blocking);
bool g_get_blocking(const gamestate* const g, entityid id);

bool g_has_block_success(const gamestate* const g, entityid id);
bool g_add_block_success(gamestate* const g, entityid id, bool block_success);
bool g_set_block_success(gamestate* const g, entityid id, bool block_success);
bool g_get_block_success(const gamestate* const g, entityid id);

bool g_has_damaged(const gamestate* const g, entityid id);
bool g_add_damaged(gamestate* const g, entityid id, bool damaged);
bool g_set_damaged(gamestate* const g, entityid id, bool damaged);
bool g_get_damaged(const gamestate* const g, entityid id);

//bool g_has_inventory(const gamestate* const g, entityid id);
//bool g_add_inventory(gamestate* const g, entityid id, entityid item_id);
//bool g_remove_inventory(gamestate* const g, entityid id, entityid item_id);
