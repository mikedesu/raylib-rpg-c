#pragma once

#include "character_creation.h"
#include "component.h"
#include "component_table.h"
#include "controlmode.h"
#include "debugpanel.h"
#include "direction.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "entity_actions.h"
#include "entityid.h"
#include "fadestate.h"
#include "gamestate_flag.h"
#include "inventory_sort.h"
#include "keybinding.h"
#include "monster_def.h"
#include "ringtype.h"
#include "scene.h"
#include <raylib.h>
#include <stdbool.h>
#include <time.h>

#define GAME_VERSION "v0.0.3.16-20250619"
#define DEFAULT_MUSIC_VOLUME 0.0f
#define DEFAULT_MSG_HIST_BGCOLOR ((Color){0x33, 0x33, 0x33, 255})
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
    bool debugpanelon, gridon, cam_lockon, player_input_received, is_locked, processing_actions, is3d, gameover, player_changing_direction, test_guard, display_inventory_menu,
        display_quit_menu, do_quit, dirty_entities, display_help_menu, cam_changed, frame_dirty;
    bool display_gameplay_settings_menu, display_sort_inventory_menu;
    bool music_volume_changed;

    //char help_menu_text[2048];

    int framecount, fadealpha, camera_mode, targetwidth, targetheight, windowwidth, windowheight, lock, turn_count, frame_updates;

    int sort_inventory_menu_selection;
    int sort_inventory_menu_selection_max;

    entityid* hero_inventory_sorted_by_name;
    entityid* hero_inventory_sorted_by_type;

    int font_size, pad, inventory_menu_selection;
    int gameplay_settings_menu_selection;
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
    int base_attack_damage_list_count, base_attack_damage_list_capacity;
    int vision_distance_list_count, vision_distance_list_capacity;
    int light_radius_list_count, light_radius_list_capacity;
    int light_radius_bonus_list_count, light_radius_bonus_list_capacity;
    int ringtype_list_count, ringtype_list_capacity;

    //int spell_effect_list_count, spell_effect_list_capacity;

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

    scene_t current_scene;

    int title_screen_selection;
    int max_title_screen_selections;

    entityid entity_turn;

    message_system msg_system;
    message_history msg_history;

    keybinding_list_t keybinding_list;

    ct* components;

    name_component* name_list;

    int_component* type_list;
    int_component* race_list;
    int_component* direction_list;
    int_component* default_action_list;
    int_component* itemtype_list;
    int_component* weapontype_list;
    int_component* shieldtype_list;
    int_component* potion_type_list;
    int_component* ac_list;
    int_component* dead_list;
    int_component* update_list;
    int_component* attacking_list;
    int_component* zapping_list;
    int_component* blocking_list;
    int_component* block_success_list;
    int_component* damaged_list;
    int_component* vision_distance_list;
    int_component* light_radius_list;
    int_component* light_radius_bonus_list;
    int_component* ringtype_list;

    rect_component* sprite_move_list;

    vec3_component* loc_list;
    vec3_component* target_list;
    vec3_component* damage_list;
    vec3_component* base_attack_damage_list;

    inventory_component* inventory_list;
    target_path_component* target_path_list;
    equipment_component* equipment_list;
    stats_component* stats_list;

    int current_music_index;
    int total_music_paths;
    char music_file_paths[1024][128];

    char* version;

    int restart_count;
    bool do_restart;

    character_creation chara_creation;
    // Array of monster definitions
    monster_def* monster_defs;
    // Number of monster definitions
    int monster_def_count;
    int monster_def_capacity;

    float music_volume;

    Color message_history_bgcolor;

} gamestate;

gamestate* gamestateinitptr();

dungeon_t* g_get_dungeon(gamestate* const g);

entityid gamestate_get_hero_id(const gamestate* const g);

monster_def* g_get_monster_def(gamestate* const g, race_t r);

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

bool g_add_component(gamestate* const g, entityid id, component comp, void* data, size_t component_size, void** component_list, int* component_count, int* component_capacity);

bool g_add_name(gamestate* const g, entityid id, const char* name);
bool g_has_name(const gamestate* const g, entityid id);
const char* g_get_name(gamestate* const g, entityid id);

bool g_add_type(gamestate* const g, entityid id, int type);
bool g_set_type(gamestate* const g, entityid id, int type);
entitytype_t g_get_type(const gamestate* const g, entityid id);
bool g_is_type(const gamestate* const g, entityid id, int type);
bool g_has_type(const gamestate* const g, entityid id);

race_t g_get_race(gamestate* const g, entityid id);
bool g_add_race(gamestate* const g, entityid id, int race);
bool g_is_race(gamestate* const g, entityid id, int race);
bool g_has_race(const gamestate* const g, entityid id);

direction_t g_get_direction(const gamestate* const g, entityid id);
bool g_add_direction(gamestate* const g, entityid id, int dir);
bool g_is_direction(gamestate* const g, entityid id, int dir);
bool g_update_direction(gamestate* const g, entityid id, int dir);
bool g_has_direction(const gamestate* const g, entityid id);

bool g_has_location(const gamestate* const g, entityid id);
bool g_add_location(gamestate* const g, entityid id, vec3 loc);
bool g_update_location(gamestate* const g, entityid id, vec3 loc);
vec3 g_get_location(const gamestate* const g, entityid id);
bool g_is_location(const gamestate* const g, entityid id, vec3 loc);

bool g_has_sprite_move(const gamestate* const g, entityid id);
bool g_add_sprite_move(gamestate* const g, entityid id, Rectangle loc);
bool g_update_sprite_move(gamestate* const g, entityid id, Rectangle loc);
Rectangle g_get_sprite_move(const gamestate* const g, entityid id);

bool g_has_dead(const gamestate* const g, entityid id);
bool g_add_dead(gamestate* const g, entityid id, int dead);
bool g_update_dead(gamestate* const g, entityid id, int dead);
bool g_is_dead(const gamestate* const g, entityid id);

bool g_has_update(const gamestate* const g, entityid id);
bool g_add_update(gamestate* const g, entityid id, int update);
bool g_get_update(gamestate* const g, entityid id);
bool g_set_update(gamestate* const g, entityid id, int update);

bool g_has_attacking(const gamestate* const g, entityid id);
bool g_add_attacking(gamestate* const g, entityid id, int attacking);
bool g_set_attacking(gamestate* const g, entityid id, int attacking);
bool g_get_attacking(const gamestate* const g, entityid id);

bool g_has_blocking(const gamestate* const g, entityid id);
bool g_add_blocking(gamestate* const g, entityid id, int blocking);
bool g_set_blocking(gamestate* const g, entityid id, int blocking);
bool g_get_blocking(const gamestate* const g, entityid id);

bool g_has_block_success(const gamestate* const g, entityid id);
bool g_add_block_success(gamestate* const g, entityid id, int block_success);
bool g_set_block_success(gamestate* const g, entityid id, int block_success);
bool g_get_block_success(const gamestate* const g, entityid id);

bool g_has_damaged(const gamestate* const g, entityid id);
bool g_add_damaged(gamestate* const g, entityid id, int damaged);
bool g_set_damaged(gamestate* const g, entityid id, int damaged);
bool g_get_damaged(const gamestate* const g, entityid id);

bool g_has_default_action(const gamestate* const g, entityid id);
bool g_add_default_action(gamestate* const g, entityid id, int action);
bool g_set_default_action(gamestate* const g, entityid id, int action);
entity_action_t g_get_default_action(const gamestate* const g, entityid id);

//entityid* g_sort_inventory(gamestate* const g, entityid* inventory, int inv_count, inventory_sort sort_type);
entityid* g_sort_inventory(gamestate* const g, entityid* inventory, size_t inv_count, inventory_sort sort_type);
//entityid* g_sort_inventory(gamestate* const g, entityid* inventory, int inv_count, inventory_sort sort_type);
bool g_has_inventory(const gamestate* const g, entityid id);
bool g_add_inventory(gamestate* const g, entityid id);
bool g_add_to_inventory(gamestate* const g, entityid id, entityid itemid);
bool g_remove_from_inventory(gamestate* const g, entityid id, entityid itemid);
//entityid* g_get_inventory(const gamestate* const g, entityid id, int* count);
entityid* g_get_inventory(const gamestate* const g, entityid id, size_t* count);
size_t g_get_inventory_count(const gamestate* const g, entityid id);
bool g_has_item_in_inventory(const gamestate* const g, entityid id, entityid itemid);
//bool g_update_inventory(gamestate* const g, entityid id, entityid* new_inventory);
//bool g_update_inventory(gamestate* const g, entityid id, entityid* new_inventory, int new_inventory_count);
bool g_update_inventory(gamestate* const g, entityid id, entityid* new_inventory, size_t new_inventory_count);

bool g_has_target(const gamestate* const g, entityid id);
bool g_add_target(gamestate* const g, entityid id, vec3 target);
bool g_set_target(gamestate* const g, entityid id, vec3 target);
bool g_get_target(const gamestate* const g, entityid id, vec3* target);

bool g_has_target_path(const gamestate* const g, entityid id);
bool g_add_target_path(gamestate* const g, entityid id);
bool g_set_target_path(gamestate* const g, entityid id, vec3* target_path, int target_path_length);
bool g_get_target_path(const gamestate* const g, entityid id, vec3** target_path, int* target_path_length);
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

bool g_add_itemtype(gamestate* const g, entityid id, int type);
bool g_has_itemtype(const gamestate* const g, entityid id);
bool g_set_itemtype(gamestate* const g, entityid id, int type);
itemtype g_get_itemtype(const gamestate* const g, entityid id);

bool g_add_weapontype(gamestate* const g, entityid id, int type);
bool g_has_weapontype(const gamestate* const g, entityid id);
bool g_set_weapontype(gamestate* const g, entityid id, int type);
weapontype g_get_weapontype(const gamestate* const g, entityid id);
bool g_is_weapontype(const gamestate* const g, entityid id, int type);

bool g_add_shieldtype(gamestate* const g, entityid id, int type);
bool g_has_shieldtype(const gamestate* const g, entityid id);
bool g_set_shieldtype(gamestate* const g, entityid id, int type);
shieldtype g_get_shieldtype(const gamestate* const g, entityid id);
bool g_is_shieldtype(const gamestate* const g, entityid id, int type);

bool g_add_potiontype(gamestate* const g, entityid id, int type);
bool g_has_potiontype(const gamestate* const g, entityid id);
bool g_set_potiontype(gamestate* const g, entityid id, int type);
bool g_is_potion(const gamestate* const g, entityid id);
potiontype g_get_potiontype(const gamestate* const g, entityid id);

bool g_add_damage(gamestate* const g, entityid id, vec3 r);
bool g_has_damage(const gamestate* const g, entityid id);
bool g_set_damage(gamestate* const g, entityid id, vec3 r);
vec3 g_get_damage(const gamestate* const g, entityid id);

bool g_add_ac(gamestate* const g, entityid id, int ac);
bool g_has_ac(const gamestate* const g, entityid id);
bool g_set_ac(gamestate* const g, entityid id, int ac);
int g_get_ac(const gamestate* const g, entityid id);

entityid g_add_entity(gamestate* const g);

bool g_has_attacking(const gamestate* const g, entityid id);
bool g_add_attacking(gamestate* const g, entityid id, int attacking);
bool g_set_attacking(gamestate* const g, entityid id, int attacking);
bool g_get_attacking(const gamestate* const g, entityid id);

bool g_has_zapping(const gamestate* const g, entityid id);
bool g_add_zapping(gamestate* const g, entityid id, int zapping);
bool g_set_zapping(gamestate* const g, entityid id, int zapping);
bool g_get_zapping(const gamestate* const g, entityid id);

//bool g_add_spell_effect(gamestate* const g, entityid id, spell_effect effect);
//bool g_has_spell_effect(const gamestate* const g, entityid id);
//bool g_set_spell_effect(gamestate* const g, entityid id, spell_effect effect);
//spell_effect g_get_spell_effect(const gamestate* const g, entityid id);

bool g_add_base_attack_damage(gamestate* const g, entityid id, vec3 damage);
bool g_has_base_attack_damage(const gamestate* const g, entityid id);
bool g_set_base_attack_damage(gamestate* const g, entityid id, vec3 damage);
vec3 g_get_base_attack_damage(const gamestate* const g, entityid id);

bool g_add_vision_distance(gamestate* const g, entityid id, int distance);
bool g_has_vision_distance(const gamestate* const g, entityid id);
bool g_set_vision_distance(gamestate* const g, entityid id, int distance);
int g_get_vision_distance(const gamestate* const g, entityid id);

bool g_add_light_radius(gamestate* const g, entityid id, int radius);
bool g_has_light_radius(const gamestate* const g, entityid id);
bool g_set_light_radius(gamestate* const g, entityid id, int radius);
int g_get_light_radius(const gamestate* const g, entityid id);

bool g_add_light_radius_bonus(gamestate* const g, entityid id, int radius);
bool g_has_light_radius_bonus(const gamestate* const g, entityid id);
bool g_set_light_radius_bonus(gamestate* const g, entityid id, int radius);
int g_get_light_radius_bonus(const gamestate* const g, entityid id);

bool g_add_ringtype(gamestate* const g, entityid id, int type);
bool g_has_ringtype(const gamestate* const g, entityid id);
bool g_set_ringtype(gamestate* const g, entityid id, int type);
ringtype g_get_ringtype(const gamestate* const g, entityid id);

int g_get_entity_total_light_radius_bonus(const gamestate* const g, entityid id);
