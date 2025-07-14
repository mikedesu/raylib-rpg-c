#pragma once

#include "character_creation.h"
#include "component.h"
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
#include <memory>
#include <raylib.h>
#include <unordered_map>
#include <vector>


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
    bool player_changing_dir;
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
    std::unordered_map<entityid, bool>* pushable_list;
    std::unordered_map<entityid, int>* tx_alpha_list;
    std::unordered_map<entityid, itemtype>* item_type_list;
    std::unordered_map<entityid, potiontype>* potion_type_list;
    std::unordered_map<entityid, weapontype>* weapon_type_list;

    std::shared_ptr<std::unordered_map<entityid, std::shared_ptr<std::unordered_map<int, int>>>> stats_list;

    std::shared_ptr<std::unordered_map<entityid, entityid>> equipped_weapon_list;


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

    std::shared_ptr<character_creation> chara_creation;

    // Array of monster definitions
    //monster_def* monster_defs;

    float music_volume;


    // last click position

    Vector2 last_click_screen_pos;


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


bool g_add_pushable(std::shared_ptr<gamestate> g, entityid id);
bool g_is_pushable(std::shared_ptr<gamestate> g, entityid id);


bool g_add_item_type(std::shared_ptr<gamestate> g, entityid id, itemtype type);
bool g_has_item_type(std::shared_ptr<gamestate> g, entityid id);
itemtype g_get_item_type(std::shared_ptr<gamestate> g, entityid id);
bool g_set_item_type(std::shared_ptr<gamestate> g, entityid id, itemtype type);


// set up declarations for potion type just like item type
bool g_add_potion_type(std::shared_ptr<gamestate> g, entityid id, potiontype type);
bool g_has_potion_type(std::shared_ptr<gamestate> g, entityid id);
potiontype g_get_potion_type(std::shared_ptr<gamestate> g, entityid id);
bool g_set_potion_type(std::shared_ptr<gamestate> g, entityid id, potiontype type);


bool g_add_weapon_type(std::shared_ptr<gamestate> g, entityid id, weapontype type);
bool g_has_weapon_type(std::shared_ptr<gamestate> g, entityid id);
weapontype g_get_weapon_type(std::shared_ptr<gamestate> g, entityid id);
bool g_set_weapon_type(std::shared_ptr<gamestate> g, entityid id, weapontype type);


bool g_add_stats(std::shared_ptr<gamestate> g, entityid id);
bool g_has_stats(std::shared_ptr<gamestate> g, entityid id);
bool g_set_stat(std::shared_ptr<gamestate> g, entityid id, stats_slot slot, int value);
int g_get_stat(std::shared_ptr<gamestate> g, entityid id, stats_slot slot);


bool g_add_equipped_weapon(std::shared_ptr<gamestate> g, entityid id, entityid wpn_id);
bool g_has_equipped_weapon(std::shared_ptr<gamestate> g, entityid id);
bool g_set_equipped_weapon(std::shared_ptr<gamestate> g, entityid id, entityid wpn_id);
entityid g_get_equipped_weapon(std::shared_ptr<gamestate> g, entityid id);
