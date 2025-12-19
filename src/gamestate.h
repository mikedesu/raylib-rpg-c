#pragma once

#include "ComponentTable.h"
#include "character_creation.h"
#include "controlmode.h"
#include "debugpanel.h"
#include "direction.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "entity_actions.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate_flag.h"
#include "get_racial_hd.h"
#include "libgame_version.h"
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


using std::make_shared;
using std::set;
using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;


typedef struct gamestate {
    controlmode_t controlmode;
    debugpanel_t debugpanel;

    entityid hero_id;
    entityid new_entityid_begin;
    entityid new_entityid_end;
    entityid next_entityid;
    entityid entity_turn;

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
    bool do_restart;
    bool msg_system_is_active;

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
    int title_screen_selection;
    int max_title_screen_selections;
    int msg_history_max_len_msg;
    int msg_history_max_len_msg_measure;
    int current_music_index;
    int restart_count;

    float line_spacing;
    float music_volume;

    double last_frame_time;


    shared_ptr<dungeon_t> dungeon;

    char frame_time_str[32];

    gamestate_flag_t flag;

    scene_t current_scene;

    shared_ptr<vector<string>> msg_system;
    shared_ptr<vector<string>> msg_history;
    shared_ptr<vector<Sound>> sfx;

    vector<string> music_file_paths;
    //shared_ptr<character_creation> chara_creation;

    character_creation chara_creation;


    string version;

    Color message_history_bgcolor;
    Vector2 last_click_screen_pos;
    Vector2 inventory_cursor = {0, 0};
    Camera2D cam2d;
    Camera3D cam3d;
    Font font;

    ComponentTable ct;

} gamestate;


//shared_ptr<gamestate> gamestateinitptr();
//shared_ptr<dungeon_t> g_get_dungeon(shared_ptr<gamestate> g);
//entityid gamestate_get_hero_id(shared_ptr<gamestate> g);
//entityid g_add_entity(shared_ptr<gamestate> g);
//bool gs_add_entityid(shared_ptr<gamestate> g, entityid id);
//bool gamestate_add_msg_history(shared_ptr<gamestate> g, string msg);
//bool gamestate_init_msg_history(shared_ptr<gamestate> g);
//bool g_set_hero_id(shared_ptr<gamestate> g, entityid id);
//void gamestate_init_music_paths(shared_ptr<gamestate> g);
//void gamestate_free(shared_ptr<gamestate> g);
//void gamestate_set_debug_panel_pos_bottom_left(shared_ptr<gamestate> g);
//void gamestate_set_debug_panel_pos_top_right(shared_ptr<gamestate> g);
//void gamestate_load_keybindings(shared_ptr<gamestate> g);


#define GAMESTATE_DEBUGPANEL_DEFAULT_X 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_Y 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE 20
#define GAMESTATE_INIT_ENTITYIDS_MAX 1000000


static inline void gamestate_init_music_paths(shared_ptr<gamestate> g) {
    massert(g, "g is NULL");
    minfo("Initializing music paths...");
    //if (!g->music_file_paths) {
    //    g->music_file_paths = make_shared<vector<string>>();
    //}
    const char* music_path_file = "music.txt";
    FILE* file = fopen(music_path_file, "r");
    massert(file, "Could not open music path file: %s", music_path_file);
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Remove newline character if present
        minfo("Removing newline character...");
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        // if it begins with a #, skip for now
        minfo("Checking for comment...");
        if (buffer[0] == '#') {
            mwarning("Comment received, skipping...");
            continue;
        }
        // copy into g->music_file_paths
        minfo("Pushing back %s", buffer);
        //g->music_file_paths->push_back(string(buffer));

        // need to pre-pend the folder path
        string fullpath = "audio/music/" + string(buffer);
        g->music_file_paths.push_back(fullpath);
    }
    fclose(file);
    msuccess("Music paths initialized!");
}


// have to update this function when we introduce new fields to Gamestate
static inline shared_ptr<gamestate> gamestateinitptr() {
    minfo("Initializing gamestate");
    //const size_t n = LIST_INIT_CAPACITY;
    shared_ptr<gamestate> g = make_shared<gamestate>();
    massert(g, "g is NULL");
    g->msg_system_is_active = false;
    g->version = GAME_VERSION;
    g->cam_lockon = true;
    g->frame_dirty = true;
    g->debugpanel.x = GAMESTATE_DEBUGPANEL_DEFAULT_X;
    g->debugpanel.y = GAMESTATE_DEBUGPANEL_DEFAULT_Y;
    g->debugpanel.w = 200;
    g->debugpanel.h = 200;
    g->debugpanel.fg_color = RAYWHITE;
    g->debugpanel.bg_color = RED;
    g->debugpanel.font_size = GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE;
    g->targetwidth = -1;
    g->targetheight = -1;
    g->windowwidth = -1;
    g->windowheight = -1;
    g->hero_id = -1;
    g->entity_turn = -1;
    g->new_entityid_begin = -1;
    g->new_entityid_end = -1;
    g->timebegan = time(NULL);
    g->currenttime = time(NULL);
    g->timebegantm = localtime(&(g->timebegan));
    g->currenttimetm = localtime(&(g->currenttime));
    bzero(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF);
    bzero(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF);
    strftime(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF, "Start Time: %Y-%m-%d %H:%M:%S", g->timebegantm);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
    g->debugpanelon = false;
    g->player_input_received = false;
    g->is_locked = false;
    g->gridon = false;
    g->display_inventory_menu = false;
    g->display_quit_menu = false;
    g->display_help_menu = false;
    g->do_quit = false;
    g->processing_actions = false;
    g->cam_changed = false;
    g->is3d = false;
    g->gameover = false;
    g->test_guard = false;
    g->dirty_entities = false;
    g->display_sort_inventory_menu = false;
    g->music_volume_changed = false;
    g->gameplay_settings_menu_selection = 0;
    g->cam2d.target = (Vector2){0, 0};
    g->cam2d.offset = (Vector2){0, 0};
    g->cam2d.zoom = 4.0f;
    g->cam2d.rotation = 0.0;
    g->fadealpha = 0.0;
    g->cam3d = (Camera3D){0};
    g->cam3d.position = (Vector3){0.0f, 20.0f, 20.0f};
    g->cam3d.target = (Vector3){0.0f, 0.0f, 0.0f};
    g->cam3d.up = (Vector3){0.0f, 1.0f, 0.0f};
    g->cam3d.fovy = 45.0f;
    g->cam3d.projection = CAMERA_PERSPECTIVE;
    g->camera_mode = CAMERA_FREE;
    g->controlmode = CONTROLMODE_PLAYER;
    // current displayed dungeon floor
    g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
    g->font_size = GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE;
    g->pad = 20;
    g->line_spacing = 1.0f;

    g->player_changing_dir = false;

    // weird bug maybe when set to 0?
    g->next_entityid = 1;

    g->current_music_index = 0;
    g->restart_count = 0;
    g->do_restart = 0;

    g->title_screen_selection = 0;
    g->max_title_screen_selections = 2;

    g->lock = 0;
    g->frame_updates = 0;
    g->framecount = 0;
    g->turn_count = 0;
    g->debugpanel.pad_top = 0;
    g->debugpanel.pad_left = 0;
    g->debugpanel.pad_right = 0;
    g->debugpanel.pad_bottom = 0;
    g->inventory_menu_selection = 0;

    g->dungeon = nullptr;


    g->msg_history = make_shared<vector<string>>();
    g->msg_system = make_shared<vector<string>>();
    g->msg_history_max_len_msg = 0;
    g->msg_history_max_len_msg_measure = 0;

    // initialize character creation
    //g->chara_creation = make_shared<character_creation>();

    g->chara_creation.name = "hero";

    g->chara_creation.strength = 10;
    g->chara_creation.dexterity = 10;
    g->chara_creation.intelligence = 10;
    g->chara_creation.wisdom = 10;
    g->chara_creation.constitution = 10;
    g->chara_creation.charisma = 10;

    g->chara_creation.race = RACE_HUMAN;
    g->chara_creation.hitdie = get_racial_hd(RACE_HUMAN);

    // why is the above line crashing?
    // the above line is also crashing
    msuccess("Gamestate character creation name set to empty string");

    g->current_scene = SCENE_TITLE;

    g->music_volume = DEFAULT_MUSIC_VOLUME;

    //g->component_table = make_shared<unordered_map<entityid, long>>();

    //g->equipped_weapon_list = make_shared<unordered_map<entityid, entityid>>();

    g->last_click_screen_pos = (Vector2){-1, -1};

    gamestate_init_music_paths(g);

    g->sfx = make_shared<vector<Sound>>();

    msuccess("Gamestate initialized successfully");
    return g;
}


static inline void gamestate_free(shared_ptr<gamestate> g) {
    massert(g, "g is NULL");
    minfo("Freeing gamestate");
    //if (g->chara_creation) {
    //    g->chara_creation->name.clear(); // Clear the name string
    //    g->chara_creation.reset(); // Reset the shared pointer
    //    g->chara_creation = nullptr; // Set to nullptr
    //}
    //if (g->msg_system) delete g->msg_system;
    //if (g->msg_history) delete g->msg_history;
}


static inline entityid g_add_entity(shared_ptr<gamestate> g) {
    entityid id = g->next_entityid;
    if (!g->dirty_entities) {
        g->dirty_entities = true;
        g->new_entityid_begin = id;
        g->new_entityid_end = id + 1;
    } else {
        //g->dirty_entities = true;
        g->new_entityid_end = id + 1;
    }
    g->next_entityid++;
    return id;
}


static inline bool g_set_hero_id(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (id == ENTITYID_INVALID) {
        merror("g_set_hero_id: id is invalid");
        return false;
    }
    g->hero_id = id;
    msuccess("Hero ID set to %d", id);
    return true;
}


static inline entityid gamestate_get_hero_id(const gamestate* const g) {
    massert(g, "g is NULL");
    return g->hero_id;
}


static inline bool gamestate_add_msg_history(shared_ptr<gamestate> g, string msg) {
    massert(g, "g is NULL");
    massert(msg != "", "msg is empty string");
    if (!g->msg_history) {
        merror("Message history is not initialized");
        return false;
    }
    // Add the message to the history
    g->msg_history->push_back(msg);
    return true;
}


static inline void gamestate_set_debug_panel_pos_bottom_left(gamestate* const g) {
    massert(g, "g is NULL");
    if (g->windowwidth == -1 || g->windowheight == -1)
        return;
    g->debugpanel.x = 0;
    g->debugpanel.y = g->windowheight - g->debugpanel.h;
}


static inline void gamestate_set_debug_panel_pos_top_right(gamestate* const g) {
    massert(g, "g is NULL");
    if (g->windowwidth == -1 || g->windowheight == -1)
        return;
    g->debugpanel.x = g->windowwidth - g->debugpanel.w;
    g->debugpanel.y = g->debugpanel.pad_right;
}


//void set_comp(shared_ptr<gamestate> g
