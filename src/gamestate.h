#pragma once

#include "ComponentTable.h"
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
#include "monster_def.h"
#include "ringtype.h"
#include "scene.h"
#include <ctime>
#include <memory>
#include <raylib.h>
#include <set>
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


using std::set;
using std::shared_ptr;
using std::unordered_map;
using std::vector;

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

    shared_ptr<unordered_map<entityid, long>> component_table;


    //shared_ptr<unordered_map<entityid, bool>> pushable_list;
    //shared_ptr<unordered_map<entityid, int>> tx_alpha_list;
    shared_ptr<unordered_map<entityid, itemtype>> item_type_list;
    shared_ptr<unordered_map<entityid, potiontype>> potion_type_list;
    shared_ptr<unordered_map<entityid, weapontype>> weapon_type_list;
    shared_ptr<unordered_map<entityid, shared_ptr<unordered_map<int, int>>>> stats_list;

    shared_ptr<unordered_map<entityid, entityid>> equipped_weapon_list;

    shared_ptr<unordered_map<entityid, shared_ptr<vector<entityid>>>> inventory_list;

    ComponentTable ct;


    debugpanel_t debugpanel;

    Camera2D cam2d;
    Camera3D cam3d;

    fadestate_t fadestate;

    Font font;

    float line_spacing;

    shared_ptr<dungeon_t> dungeon;

    double last_frame_time;

    char frame_time_str[32];

    gamestate_flag_t flag;

    scene_t current_scene;

    int title_screen_selection;
    int max_title_screen_selections;

    entityid entity_turn;

    shared_ptr<vector<string>> msg_system;
    shared_ptr<vector<string>> msg_history;
    shared_ptr<character_creation> chara_creation;

    int msg_history_max_len_msg;
    int msg_history_max_len_msg_measure;

    bool msg_system_is_active;


    int current_music_index;
    int total_music_paths;

    char music_file_paths[MAX_MUSIC_PATHS][MAX_MUSIC_PATH_LENGTH];

    string version;

    int restart_count;

    bool do_restart;

    // Array of monster definitions
    float music_volume;
    // last click position
    Vector2 last_click_screen_pos;
    Color message_history_bgcolor;


    Vector2 inventory_cursor = {0, 0};


} gamestate;


shared_ptr<gamestate> gamestateinitptr();

void gamestate_init_music_paths(shared_ptr<gamestate> g);


shared_ptr<dungeon_t> g_get_dungeon(shared_ptr<gamestate> g);


entityid gamestate_get_hero_id(shared_ptr<gamestate> g);


monster_def* g_get_monster_def(shared_ptr<gamestate> g, race_t r);


bool gs_add_entityid(shared_ptr<gamestate> g, entityid id);


entityid g_add_entity(shared_ptr<gamestate> g);


bool gamestate_add_msg_history(shared_ptr<gamestate> g, string msg);
bool gamestate_init_msg_history(shared_ptr<gamestate> g);


bool g_set_hero_id(shared_ptr<gamestate> g, entityid id);


void gamestate_free(shared_ptr<gamestate> g);
void gamestate_set_debug_panel_pos_bottom_left(shared_ptr<gamestate> g);
void gamestate_set_debug_panel_pos_top_right(shared_ptr<gamestate> g);
void gamestate_load_keybindings(shared_ptr<gamestate> g);


bool g_has_comp(shared_ptr<gamestate> g, entityid id, component comp);
bool g_register_comp(shared_ptr<gamestate> g, entityid id, component comp);
bool g_add_comp(shared_ptr<gamestate> g, entityid id, component comp);


//int g_get_tx_alpha(shared_ptr<gamestate> g, entityid id);
//bool g_has_tx_alpha(shared_ptr<gamestate> g, entityid id);
//bool g_set_tx_alpha(shared_ptr<gamestate> g, entityid id, int alpha);
//bool g_add_tx_alpha(shared_ptr<gamestate> g, entityid id, int alpha);
//bool g_incr_tx_alpha(shared_ptr<gamestate> g, entityid id, int alpha);
//bool g_decr_tx_alpha(shared_ptr<gamestate> g, entityid id, int alpha);


bool g_add_item_type(shared_ptr<gamestate> g, entityid id, itemtype type);
bool g_has_item_type(shared_ptr<gamestate> g, entityid id);
itemtype g_get_item_type(shared_ptr<gamestate> g, entityid id);
bool g_set_item_type(shared_ptr<gamestate> g, entityid id, itemtype type);


// set up declarations for potion type just like item type
bool g_add_potion_type(shared_ptr<gamestate> g, entityid id, potiontype type);
bool g_has_potion_type(shared_ptr<gamestate> g, entityid id);
potiontype g_get_potion_type(shared_ptr<gamestate> g, entityid id);
bool g_set_potion_type(shared_ptr<gamestate> g, entityid id, potiontype type);


bool g_add_weapon_type(shared_ptr<gamestate> g, entityid id, weapontype type);
bool g_has_weapon_type(shared_ptr<gamestate> g, entityid id);
weapontype g_get_weapon_type(shared_ptr<gamestate> g, entityid id);
bool g_set_weapon_type(shared_ptr<gamestate> g, entityid id, weapontype type);


bool g_add_stats(shared_ptr<gamestate> g, entityid id);
bool g_has_stats(shared_ptr<gamestate> g, entityid id);
bool g_set_stat(shared_ptr<gamestate> g, entityid id, stats_slot slot, int value);
int g_get_stat(shared_ptr<gamestate> g, entityid id, stats_slot slot);
