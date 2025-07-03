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
#include "entitytype.h"
#include "fadestate.h"
#include "gamestate_flag.h"
#include "inventory_sort.h"
#include "keybinding.h"
#include "monster_def.h"
#include "ringtype.h"
#include "scene.h"
#include <ctime>
#include <raylib.h>
#include <unordered_map>


#define DEFAULT_MUSIC_VOLUME 0.0f
#define DEFAULT_MSG_HIST_BGCOLOR ((Color){0x33, 0x33, 0x33, 255})
#define GAMESTATE_SIZEOFTIMEBUF 64
#define GAMESTATE_SIZEOFDEBUGPANELBUF 1024
#define GAMESTATE_SIZEOFTEXINFOARRAY 1024
#define MAX_MESSAGES 64
#define MAX_MSG_LENGTH 256
#define LIST_INIT_CAPACITY 16
#define DEFAULT_MAX_HISTORY_SIZE 1024
#define MAX_MUSIC_PATHS 1024
#define MAX_MUSIC_PATH_LENGTH 256


typedef struct gamestate {
    controlmode_t controlmode;
    entityid hero_id;
    entityid new_entityid_begin;
    entityid new_entityid_end;
    entityid next_entityid;
    time_t timebegan;
    time_t currenttime;
    struct tm* timebegantm;
    struct tm* currenttimetm;
    char timebeganbuf[GAMESTATE_SIZEOFTIMEBUF];
    char currenttimebuf[GAMESTATE_SIZEOFTIMEBUF];
    bool debugpanelon;
    bool gridon;
    bool cam_lockon;
    bool player_input_received;
    bool is_locked;
    bool processing_actions;
    bool is3d;
    bool gameover;
    bool player_changing_direction;
    bool test_guard;
    bool display_inventory_menu;
    bool display_quit_menu;
    bool do_quit;
    bool dirty_entities;
    bool display_help_menu;
    bool cam_changed;
    bool frame_dirty;
    bool display_gameplay_settings_menu;
    bool display_sort_inventory_menu;
    bool music_volume_changed;

    int framecount;
    int fadealpha;
    int camera_mode;
    int targetwidth;
    int targetheight;
    int windowwidth;
    int windowheight;
    int lock;
    int turn_count;
    int frame_updates;
    int font_size;
    int pad;
    int inventory_menu_selection;
    int gameplay_settings_menu_selection;

    std::unordered_map<entityid, long>* component_table;

    std::unordered_map<entityid, string>* name_list;
    std::unordered_map<entityid, entitytype_t>* type_list;
    std::unordered_map<entityid, race_t>* race_list;
    std::unordered_map<entityid, vec3>* loc_list;
    std::unordered_map<entityid, Rectangle>* sprite_move_list;
    std::unordered_map<entityid, direction_t>* dir_list;
    std::unordered_map<entityid, bool>* dead_list;
    std::unordered_map<entityid, bool>* update_list;

    std::unordered_map<entityid, bool>* attacking_list;
    std::unordered_map<entityid, bool>* blocking_list;
    std::unordered_map<entityid, bool>* block_success_list;
    std::unordered_map<entityid, bool>* damaged_list;

    std::unordered_map<entityid, int>* tx_alpha_list;

    //std::unordered_map<entityid, inventory_component>* inventory_list;

    //inventory_component* inventory_list;
    //int inventory_list_count;
    //int inventory_list_capacity;
    //vec3_component* target_list;
    //int target_list_count;
    //int target_list_capacity;
    //target_path_component* target_path_list;
    //int target_path_list_count;
    //int target_path_list_capacity;
    //int_component* default_action_list;
    //int default_action_list_count;
    //int default_action_list_capacity;
    //int equipment_list_count;
    //int equipment_list_capacity;
    //int stats_list_count;
    //int stats_list_capacity;
    //int_component* itemtype_list;
    //int_component* weapontype_list;
    //int weapontype_list_count;
    //int weapontype_list_capacity;
    //int_component* shieldtype_list;
    //int shieldtype_list_count;
    //int shieldtype_list_capacity;
    //int_component* potion_type_list;
    //int potion_type_list_count;
    //int potion_type_list_capacity;
    //vec3_component* damage_list;
    //int damage_list_count;
    //int damage_list_capacity;
    //int_component* ac_list;
    //int ac_list_count;
    //int ac_list_capacity;
    //int_component* zapping_list;
    //int zapping_list_count;
    //int zapping_list_capacity;
    //vec3_component* base_attack_damage_list;
    //int base_attack_damage_list_count;
    //int base_attack_damage_list_capacity;
    //int_component* vision_distance_list;
    //int vision_distance_list_count;
    //int vision_distance_list_capacity;
    //int_component* light_radius_list;
    //int light_radius_list_count;
    //int light_radius_list_capacity;
    //int_component* light_radius_bonus_list;
    //int light_radius_bonus_list_count;
    //int light_radius_bonus_list_capacity;
    //int_component* ringtype_list;
    //int ringtype_list_count;
    //int ringtype_list_capacity;
    //vec3_list_component* explored_list; // List of explored tiles
    //int explored_list_count;
    //int explored_list_capacity;
    //vec3_list_component* visible_list; // List of visible tiles
    //int visible_list_count;
    //int visible_list_capacity;
    //equipment_component* equipment_list;
    //stats_component* stats_list;

    debugpanel_t debugpanel;

    Camera2D cam2d;
    Camera3D cam3d;

    fadestate_t fadestate;

    Font font;
    float line_spacing;

    std::shared_ptr<dungeon_t> dungeon;

    double last_frame_time;
    char frame_time_str[32];

    gamestate_flag_t flag;

    scene_t current_scene;

    int title_screen_selection;
    int max_title_screen_selections;

    entityid entity_turn;

    //message_system msg_system;
    std::vector<string>* msg_system;
    std::vector<string>* msg_history;

    bool msg_system_is_active;

    std::shared_ptr<keybinding_list_t> keybinding_list;

    int current_music_index;
    int total_music_paths;
    char music_file_paths[MAX_MUSIC_PATHS][MAX_MUSIC_PATH_LENGTH];

    string version;

    int restart_count;
    bool do_restart;

    //character_creation chara_creation;
    std::shared_ptr<character_creation> chara_creation;
    // Array of monster definitions
    monster_def* monster_defs;
    // Number of monster definitions
    int monster_def_count;
    int monster_def_capacity;

    float music_volume;

    Color message_history_bgcolor;

} gamestate;


std::shared_ptr<gamestate> gamestateinitptr();


std::shared_ptr<dungeon_t> g_get_dungeon(std::shared_ptr<gamestate> g);


entityid gamestate_get_hero_id(std::shared_ptr<gamestate> g);


monster_def* g_get_monster_def(std::shared_ptr<gamestate> g, race_t r);


bool gs_add_entityid(std::shared_ptr<gamestate> g, entityid id);


entityid g_add_entity(std::shared_ptr<gamestate> g);


bool gamestate_add_msg_history(std::shared_ptr<gamestate> g, string msg);
bool gamestate_init_msg_history(std::shared_ptr<gamestate> g);


bool g_set_hero_id(std::shared_ptr<gamestate> g, entityid id);


void gamestate_free(std::shared_ptr<gamestate> g);
void gamestate_set_debug_panel_pos_bottom_left(std::shared_ptr<gamestate> g);
void gamestate_set_debug_panel_pos_top_right(std::shared_ptr<gamestate> g);
void gamestate_load_keybindings(std::shared_ptr<gamestate> g);


bool g_has_comp(std::shared_ptr<gamestate> g, entityid id, component comp);
bool g_register_comp(std::shared_ptr<gamestate> g, entityid id, component comp);
bool g_add_comp(std::shared_ptr<gamestate> g, entityid id, component comp);


bool g_add_name(std::shared_ptr<gamestate> g, entityid id, string name);
bool g_has_name(std::shared_ptr<gamestate> g, entityid id);
string g_get_name(std::shared_ptr<gamestate> g, entityid id);


bool g_add_type(std::shared_ptr<gamestate> g, entityid id, entitytype_t type);
entitytype_t g_get_type(std::shared_ptr<gamestate> g, entityid id);
bool g_has_type(std::shared_ptr<gamestate> g, entityid id);


bool g_has_race(std::shared_ptr<gamestate> g, entityid id);
race_t g_get_race(std::shared_ptr<gamestate> g, entityid id);
bool g_add_race(std::shared_ptr<gamestate> g, entityid id, race_t race);
bool g_update_race(std::shared_ptr<gamestate> g, entityid id, race_t race);


bool g_has_loc(std::shared_ptr<gamestate> g, entityid id);
bool g_add_loc(std::shared_ptr<gamestate> g, entityid id, vec3 loc);
bool g_update_loc(std::shared_ptr<gamestate> g, entityid id, vec3 loc);
vec3 g_get_loc(std::shared_ptr<gamestate> g, entityid id);


direction_t g_get_dir(std::shared_ptr<gamestate> g, entityid id);
bool g_add_dir(std::shared_ptr<gamestate> g, entityid id, direction_t dir);
bool g_update_dir(std::shared_ptr<gamestate> g, entityid id, direction_t dir);
bool g_has_dir(std::shared_ptr<gamestate> g, entityid id);

bool g_has_sprite_move(std::shared_ptr<gamestate> g, entityid id);
bool g_add_sprite_move(std::shared_ptr<gamestate> g, entityid id, Rectangle loc);
bool g_update_sprite_move(std::shared_ptr<gamestate> g, entityid id, Rectangle loc);
Rectangle g_get_sprite_move(std::shared_ptr<gamestate> g, entityid id);

bool g_has_dead(std::shared_ptr<gamestate> g, entityid id);
bool g_add_dead(std::shared_ptr<gamestate> g, entityid id, bool dead);
bool g_update_dead(std::shared_ptr<gamestate> g, entityid id, bool dead);
bool g_is_dead(std::shared_ptr<gamestate> g, entityid id);

bool g_has_update(std::shared_ptr<gamestate> g, entityid id);
bool g_add_update(std::shared_ptr<gamestate> g, entityid id, bool update);
bool g_get_update(std::shared_ptr<gamestate> g, entityid id);
bool g_set_update(std::shared_ptr<gamestate> g, entityid id, bool update);

bool g_has_attacking(std::shared_ptr<gamestate> g, entityid id);
bool g_add_attacking(std::shared_ptr<gamestate> g, entityid id, int attacking);
bool g_set_attacking(std::shared_ptr<gamestate> g, entityid id, int attacking);
bool g_get_attacking(std::shared_ptr<gamestate> g, entityid id);

bool g_has_blocking(std::shared_ptr<gamestate> g, entityid id);
bool g_add_blocking(std::shared_ptr<gamestate> g, entityid id, int blocking);
bool g_set_blocking(std::shared_ptr<gamestate> g, entityid id, int blocking);
bool g_get_blocking(std::shared_ptr<gamestate> g, entityid id);

bool g_has_block_success(std::shared_ptr<gamestate> g, entityid id);
bool g_add_block_success(std::shared_ptr<gamestate> g, entityid id, int block_success);
bool g_set_block_success(std::shared_ptr<gamestate> g, entityid id, int block_success);
bool g_get_block_success(std::shared_ptr<gamestate> g, entityid id);

bool g_has_damaged(std::shared_ptr<gamestate> g, entityid id);
bool g_add_damaged(std::shared_ptr<gamestate> g, entityid id, int damaged);
bool g_set_damaged(std::shared_ptr<gamestate> g, entityid id, int damaged);
bool g_get_damaged(std::shared_ptr<gamestate> g, entityid id);


int g_get_tx_alpha(std::shared_ptr<gamestate> g, entityid id);
bool g_has_tx_alpha(std::shared_ptr<gamestate> g, entityid id);
bool g_set_tx_alpha(std::shared_ptr<gamestate> g, entityid id, int alpha);
bool g_add_tx_alpha(std::shared_ptr<gamestate> g, entityid id, int alpha);
bool g_incr_tx_alpha(std::shared_ptr<gamestate> g, entityid id, int alpha);
bool g_decr_tx_alpha(std::shared_ptr<gamestate> g, entityid id, int alpha);


//bool g_has_default_action(std::shared_ptr<gamestate> g, entityid id);
//bool g_add_default_action(std::shared_ptr<gamestate> g, entityid id, int action);
//bool g_set_default_action(std::shared_ptr<gamestate> g, entityid id, int action);
//entity_action_t g_get_default_action(std::shared_ptr<gamestate> g, entityid id);

//entityid* g_sort_inventory(std::shared_ptr<gamestate> g, entityid* inventory, size_t inv_count, inventory_sort sort_type);


//bool g_has_inventory(std::shared_ptr<gamestate> g, entityid id);
//bool g_add_inventory(std::shared_ptr<gamestate> g, entityid id);
//bool g_add_to_inventory(std::shared_ptr<gamestate> g, entityid id, entityid itemid);
//bool g_remove_from_inventory(std::shared_ptr<gamestate> g, entityid id, entityid itemid);
//entityid* g_get_inventory(const std::shared_ptr<gamestate> g, entityid id, size_t* count);
//size_t g_get_inventory_count(const std::shared_ptr<gamestate> g, entityid id);
//bool g_has_item_in_inventory(std::shared_ptr<gamestate> g, entityid id, entityid itemid);
//bool g_update_inventory(std::shared_ptr<gamestate> g, entityid id, entityid* new_inventory, size_t new_inventory_count);

//bool g_has_target(std::shared_ptr<gamestate> g, entityid id);
//bool g_add_target(std::shared_ptr<gamestate> g, entityid id, vec3 target);
//bool g_set_target(std::shared_ptr<gamestate> g, entityid id, vec3 target);
//bool g_get_target(std::shared_ptr<gamestate> g, entityid id, vec3* target);

//bool g_has_target_path(std::shared_ptr<gamestate> g, entityid id);
//bool g_add_target_path(std::shared_ptr<gamestate> g, entityid id);
//bool g_set_target_path(std::shared_ptr<gamestate> g, entityid id, vec3* target_path, int target_path_length);
//bool g_get_target_path(std::shared_ptr<gamestate> g, entityid id, vec3** target_path, int* target_path_length);
//bool g_get_target_path_length(std::shared_ptr<gamestate> g, entityid id, int* target_path_length);

//bool g_add_equipment(std::shared_ptr<gamestate> g, entityid id);
//bool g_has_equipment(std::shared_ptr<gamestate> g, entityid id);
//bool g_set_equipment(std::shared_ptr<gamestate> g, entityid id, equipment_slot slot, entityid itemid);
//bool g_unset_equipment(std::shared_ptr<gamestate> g, entityid id, equipment_slot slot);
//entityid g_get_equipment(std::shared_ptr<gamestate> g, entityid id, equipment_slot slot);
//bool g_is_equipped(std::shared_ptr<gamestate> g, entityid id, entityid itemid);

//bool g_add_stats(std::shared_ptr<gamestate> g, entityid id);
//bool g_has_stats(std::shared_ptr<gamestate> g, entityid id);
//bool g_set_stat(std::shared_ptr<gamestate> g, entityid id, stats_slot stats_slot, int value);
//int* g_get_stats(std::shared_ptr<gamestate> g, entityid id, int* count);
//int g_get_stat(std::shared_ptr<gamestate> g, entityid id, stats_slot stats_slot);

//bool g_add_itemtype(std::shared_ptr<gamestate> g, entityid id, int type);
//bool g_has_itemtype(std::shared_ptr<gamestate> g, entityid id);
//bool g_set_itemtype(std::shared_ptr<gamestate> g, entityid id, int type);
//itemtype g_get_itemtype(std::shared_ptr<gamestate> g, entityid id);

//bool g_add_weapontype(std::shared_ptr<gamestate> g, entityid id, int type);
//bool g_has_weapontype(std::shared_ptr<gamestate> g, entityid id);
//bool g_set_weapontype(std::shared_ptr<gamestate> g, entityid id, int type);
//weapontype g_get_weapontype(std::shared_ptr<gamestate> g, entityid id);
//bool g_is_weapontype(std::shared_ptr<gamestate> g, entityid id, int type);

//bool g_add_shieldtype(std::shared_ptr<gamestate> g, entityid id, int type);
//bool g_has_shieldtype(std::shared_ptr<gamestate> g, entityid id);
//bool g_set_shieldtype(std::shared_ptr<gamestate> g, entityid id, int type);
//shieldtype g_get_shieldtype(std::shared_ptr<gamestate> g, entityid id);
//bool g_is_shieldtype(std::shared_ptr<gamestate> g, entityid id, int type);

//bool g_add_potiontype(std::shared_ptr<gamestate> g, entityid id, int type);
//bool g_has_potiontype(std::shared_ptr<gamestate> g, entityid id);
//bool g_set_potiontype(std::shared_ptr<gamestate> g, entityid id, int type);
//bool g_is_potion(std::shared_ptr<gamestate> g, entityid id);
//potiontype g_get_potiontype(std::shared_ptr<gamestate> g, entityid id);

//bool g_add_damage(std::shared_ptr<gamestate> g, entityid id, vec3 r);
//bool g_has_damage(std::shared_ptr<gamestate> g, entityid id);
//bool g_set_damage(std::shared_ptr<gamestate> g, entityid id, vec3 r);
//vec3 g_get_damage(std::shared_ptr<gamestate> g, entityid id);

//bool g_add_ac(std::shared_ptr<gamestate> g, entityid id, int ac);
//bool g_has_ac(std::shared_ptr<gamestate> g, entityid id);
//bool g_set_ac(std::shared_ptr<gamestate> g, entityid id, int ac);
//int g_get_ac(std::shared_ptr<gamestate> g, entityid id);

//bool g_has_attacking(std::shared_ptr<gamestate> g, entityid id);
//bool g_add_attacking(std::shared_ptr<gamestate> g, entityid id, int attacking);
//bool g_set_attacking(std::shared_ptr<gamestate> g, entityid id, int attacking);
//bool g_get_attacking(std::shared_ptr<gamestate> g, entityid id);

//bool g_has_zapping(std::shared_ptr<gamestate> g, entityid id);
//bool g_add_zapping(std::shared_ptr<gamestate> g, entityid id, int zapping);
//bool g_set_zapping(std::shared_ptr<gamestate> g, entityid id, int zapping);
//bool g_get_zapping(std::shared_ptr<gamestate> g, entityid id);

//bool g_add_base_attack_damage(std::shared_ptr<gamestate> g, entityid id, vec3 damage);
//bool g_has_base_attack_damage(std::shared_ptr<gamestate> g, entityid id);
//bool g_set_base_attack_damage(std::shared_ptr<gamestate> g, entityid id, vec3 damage);
//vec3 g_get_base_attack_damage(std::shared_ptr<gamestate> g, entityid id);

//bool g_add_vision_distance(std::shared_ptr<gamestate> g, entityid id, int distance);
//bool g_has_vision_distance(std::shared_ptr<gamestate> g, entityid id);
//bool g_set_vision_distance(std::shared_ptr<gamestate> g, entityid id, int distance);
//int g_get_vision_distance(std::shared_ptr<gamestate> g, entityid id);

//bool g_add_ringtype(std::shared_ptr<gamestate> g, entityid id, ringtype type);
//bool g_has_ringtype(std::shared_ptr<gamestate> g, entityid id);
//bool g_set_ringtype(std::shared_ptr<gamestate> g, entityid id, ringtype type);
//ringtype g_get_ringtype(std::shared_ptr<gamestate> g, entityid id);
