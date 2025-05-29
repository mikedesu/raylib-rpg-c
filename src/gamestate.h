#pragma once

#include "ac_component.h"
#include "base_attack_damage_component.h"
#include "component.h"
#include "component_table.h"
#include "controlmode.h"
#include "damage_component.h"
#include "debugpanel.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "entityid.h"
#include "fadestate.h"
#include "gamestate_flag.h"
#include "keybinding.h"
#include <raylib.h>
#include <stdbool.h>
#include <time.h>

#define GAME_VERSION "v0.0.3.2"

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
    controlmode_t controlmode;
    entityid hero_id, new_entityid_begin, new_entityid_end, next_entityid;
    time_t timebegan, currenttime;
    struct tm *timebegantm, *currenttimetm;
    char timebeganbuf[GAMESTATE_SIZEOFTIMEBUF], currenttimebuf[GAMESTATE_SIZEOFTIMEBUF];
    bool debugpanelon, gridon, cam_lockon, player_input_received, is_locked, processing_actions, is3d, gameover, player_changing_direction, test_guard,
        display_inventory_menu, display_quit_menu, do_quit, dirty_entities; // true if new entities created this turn

    bool display_help_menu;
    //char help_menu_text[2048];

    int framecount, fadealpha, camera_mode, targetwidth, targetheight, windowwidth, windowheight, lock, turn_count;
    int font_size, pad, inventory_menu_selection;
    int name_list_count, name_list_capacity;
    int type_list_count, type_list_capacity;
    int race_list_count, race_list_capacity;
    int direction_list_count, direction_list_capacity;
    int loc_list_count, loc_list_capacity;
    int sprite_move_list_count, sprite_move_list_capacity;
    int dead_list_count, dead_list_capacity;
    int update_list_count, update_list_capacity;
    int attacking_list_count, attacking_list_capacity;
    int blocking_list_count, blocking_list_capacity;
    int block_success_list_count, block_success_list_capacity;
    int damaged_list_count, damaged_list_capacity;
    int inventory_list_count, inventory_list_capacity;
    int target_list_count, target_list_capacity;
    int target_path_list_count, target_path_list_capacity;
    int default_action_list_count, default_action_list_capacity;
    int equipment_list_count, equipment_list_capacity;
    int stats_list_count, stats_list_capacity;
    int itemtype_list_count, itemtype_list_capacity;
    int weapontype_list_count, weapontype_list_capacity;
    int shieldtype_list_count, shieldtype_list_capacity;
    int potion_type_list_count, potion_type_list_capacity;
    int damage_list_count, damage_list_capacity;
    int ac_list_count, ac_list_capacity;
    int zapping_list_count, zapping_list_capacity;
    int spell_effect_list_count, spell_effect_list_capacity;
    int base_attack_damage_list_count, base_attack_damage_list_capacity;

    debugpanel_t debugpanel;

    Camera2D cam2d;
    Camera3D cam3d;

    fadestate_t fadestate;

    Font font;
    float line_spacing;

    dungeon_t* d;

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
    zapping_component* zapping_list;
    blocking_component* blocking_list;
    block_success_component* block_success_list;
    damaged_component* damaged_list;
    inventory_component* inventory_list;
    target_component* target_list;
    target_path_component* target_path_list;
    default_action_component* default_action_list;
    equipment_component* equipment_list;
    stats_component* stats_list;
    itemtype_component* itemtype_list;
    weapontype_component* weapontype_list;
    shieldtype_component* shieldtype_list;
    potiontype_component* potion_type_list;
    damage_component* damage_list;
    ac_component* ac_list;
    spell_effect_component* spell_effect_list;
    base_attack_damage_component* base_attack_damage_list;

    int current_music_index;
    int total_music_paths;
    char music_file_paths[1024][128];

    char* version;

} gamestate;

gamestate* gamestateinitptr();

dungeon_t* g_get_dungeon(gamestate* const g);

entityid gamestate_get_hero_id(const gamestate* const g);

bool gs_add_entityid(gamestate* const g, entityid id);
bool gamestate_init_msg_history(gamestate* const g);
bool gamestate_free_msg_history(gamestate* const g);

bool gamestate_add_msg_history(gamestate* const g, const char* msg);

void gamestatefree(gamestate* g);
void gamestate_set_hero_id(gamestate* const g, entityid id);
void gamestate_set_debug_panel_pos_bottom_left(gamestate* const g);
void gamestate_set_debug_panel_pos_top_right(gamestate* const g);
void gamestate_load_keybindings(gamestate* const g);

bool g_has_component(const gamestate* const g, entityid id, component comp);
bool g_register_comp(gamestate* const g, entityid id, component comp);
bool g_register_comps(gamestate* const g, entityid id, ...);

bool g_add_component(gamestate* const g,
                     entityid id,
                     component comp,
                     void* data,
                     size_t component_size,
                     void** component_list,
                     int* component_count,
                     int* component_capacity);

bool g_add_name(gamestate* const g, entityid id, const char* name);
bool g_has_name(const gamestate* const g, entityid id);
const char* g_get_name(gamestate* const g, entityid id);

bool g_add_type(gamestate* const g, entityid id, entitytype_t type);
bool g_set_type(gamestate* const g, entityid id, entitytype_t type);
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

bool g_has_default_action(const gamestate* const g, entityid id);
bool g_add_default_action(gamestate* const g, entityid id, entity_action_t action);
bool g_set_default_action(gamestate* const g, entityid id, entity_action_t action);
entity_action_t g_get_default_action(const gamestate* const g, entityid id);

bool g_has_inventory(const gamestate* const g, entityid id);
bool g_add_inventory(gamestate* const g, entityid id);
bool g_add_to_inventory(gamestate* const g, entityid id, entityid itemid);
bool g_remove_from_inventory(gamestate* const g, entityid id, entityid itemid);
entityid* g_get_inventory(const gamestate* const g, entityid id, int* count);
size_t g_get_inventory_count(const gamestate* const g, entityid id);
bool g_has_item_in_inventory(const gamestate* const g, entityid id, entityid itemid);

bool g_has_target(const gamestate* const g, entityid id);
bool g_add_target(gamestate* const g, entityid id, loc_t target);
bool g_set_target(gamestate* const g, entityid id, loc_t target);
bool g_get_target(const gamestate* const g, entityid id, loc_t* target);

bool g_has_target_path(const gamestate* const g, entityid id);
bool g_add_target_path(gamestate* const g, entityid id);
bool g_set_target_path(gamestate* const g, entityid id, loc_t* target_path, int target_path_length);
bool g_get_target_path(const gamestate* const g, entityid id, loc_t** target_path, int* target_path_length);
bool g_get_target_path_length(const gamestate* const g, entityid id, int* target_path_length);

bool g_add_equipment(gamestate* const g, entityid id);
bool g_has_equipment(const gamestate* const g, entityid id);
bool g_set_equipment(gamestate* const g, entityid id, equipment_slot slot, entityid itemid);
bool g_unset_equipment(gamestate* const g, entityid id, equipment_slot slot);
entityid g_get_equipment(const gamestate* const g, entityid id, equipment_slot slot);
bool g_is_equipped(const gamestate* const g, entityid id, entityid itemid);

bool g_add_stats(gamestate* const g, entityid id);
bool g_has_stats(const gamestate* const g, entityid id);
bool g_set_stat(gamestate* const g, entityid id, stats_slot stats_slot, int value);
int* g_get_stats(const gamestate* const g, entityid id, int* count);
int g_get_stat(const gamestate* const g, entityid id, stats_slot stats_slot);

bool g_add_itemtype(gamestate* const g, entityid id, itemtype type);
bool g_has_itemtype(const gamestate* const g, entityid id);
bool g_set_itemtype(gamestate* const g, entityid id, itemtype type);
itemtype g_get_itemtype(const gamestate* const g, entityid id);

bool g_add_weapontype(gamestate* const g, entityid id, weapontype type);
bool g_has_weapontype(const gamestate* const g, entityid id);
bool g_set_weapontype(gamestate* const g, entityid id, weapontype type);
weapontype g_get_weapontype(const gamestate* const g, entityid id);
bool g_is_weapontype(const gamestate* const g, entityid id, weapontype type);

bool g_add_shieldtype(gamestate* const g, entityid id, shieldtype type);
bool g_has_shieldtype(const gamestate* const g, entityid id);
bool g_set_shieldtype(gamestate* const g, entityid id, shieldtype type);
shieldtype g_get_shieldtype(const gamestate* const g, entityid id);
bool g_is_shieldtype(const gamestate* const g, entityid id, shieldtype type);

bool g_add_potiontype(gamestate* const g, entityid id, potiontype type);
bool g_has_potiontype(const gamestate* const g, entityid id);
bool g_set_potiontype(gamestate* const g, entityid id, potiontype type);
bool g_is_potion(const gamestate* const g, entityid id);
potiontype g_get_potiontype(const gamestate* const g, entityid id);

bool g_add_damage(gamestate* const g, entityid id, roll r);
bool g_has_damage(const gamestate* const g, entityid id);
bool g_set_damage(gamestate* const g, entityid id, roll r);
roll g_get_damage(const gamestate* const g, entityid id);

bool g_add_ac(gamestate* const g, entityid id, int ac);
bool g_has_ac(const gamestate* const g, entityid id);
bool g_set_ac(gamestate* const g, entityid id, int ac);
int g_get_ac(const gamestate* const g, entityid id);

entityid g_add_entity(gamestate* const g);

bool g_has_attacking(const gamestate* const g, entityid id);
bool g_add_attacking(gamestate* const g, entityid id, bool attacking);
bool g_set_attacking(gamestate* const g, entityid id, bool attacking);
bool g_get_attacking(const gamestate* const g, entityid id);

bool g_has_zapping(const gamestate* const g, entityid id);
bool g_add_zapping(gamestate* const g, entityid id, bool zapping);
bool g_set_zapping(gamestate* const g, entityid id, bool zapping);
bool g_get_zapping(const gamestate* const g, entityid id);

bool g_add_spell_effect(gamestate* const g, entityid id, spell_effect effect);
bool g_has_spell_effect(const gamestate* const g, entityid id);
bool g_set_spell_effect(gamestate* const g, entityid id, spell_effect effect);
spell_effect g_get_spell_effect(const gamestate* const g, entityid id);

bool g_add_base_attack_damage(gamestate* const g, entityid id, roll damage);
bool g_has_base_attack_damage(const gamestate* const g, entityid id);
bool g_set_base_attack_damage(gamestate* const g, entityid id, roll damage);
roll g_get_base_attack_damage(const gamestate* const g, entityid id);
