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
#define GAMESTATE_DEBUGPANEL_DEFAULT_X 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_Y 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE 20
#define GAMESTATE_INIT_ENTITYIDS_MAX 1000000


using std::make_shared;
using std::string;
using std::unordered_map;
using std::vector;


//typedef struct gamestate {


class gamestate {
public:
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

    dungeon_t dungeon;

    char frame_time_str[32];

    gamestate_flag_t flag;

    scene_t current_scene;

    vector<string> msg_system;
    vector<string> msg_history;
    vector<Sound> sfx;
    vector<string> music_file_paths;

    character_creation chara_creation;

    string version;

    Color message_history_bgcolor;
    Vector2 last_click_screen_pos;
    Vector2 inventory_cursor = {0, 0};
    Camera2D cam2d;
    Font font;

    ComponentTable ct;


    gamestate() {
        minfo("Initializing gamestate");
        reset();
        msuccess("Gamestate initialized successfully");
    }


    void init_music_paths() {
        minfo("Initializing music paths...");
        const char* music_path_file = "music.txt";
        FILE* file = fopen(music_path_file, "r");
        massert(file, "Could not open music path file: %s", music_path_file);
        char buffer[128];
        music_file_paths.clear();
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
            // need to pre-pend the folder path
            string fullpath = "audio/music/" + string(buffer);
            music_file_paths.push_back(fullpath);
        }
        fclose(file);
        msuccess("Music paths initialized!");
    }


    void reset() {
        minfo("gamestate reset");

        msg_system_is_active = false;
        version = GAME_VERSION;
        cam_lockon = true;
        frame_dirty = true;
        debugpanel.x = GAMESTATE_DEBUGPANEL_DEFAULT_X;
        debugpanel.y = GAMESTATE_DEBUGPANEL_DEFAULT_Y;
        debugpanel.w = 200;
        debugpanel.h = 200;
        debugpanel.fg_color = RAYWHITE;
        debugpanel.bg_color = RED;
        debugpanel.font_size = GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE;

        targetwidth = targetheight = -1;
        windowwidth = windowheight = -1;
        hero_id = entity_turn = new_entityid_begin = new_entityid_end = -1;
        timebegan = time(NULL);
        currenttime = time(NULL);
        timebegantm = localtime(&timebegan);
        currenttimetm = localtime(&currenttime);
        bzero(timebeganbuf, GAMESTATE_SIZEOFTIMEBUF);
        bzero(currenttimebuf, GAMESTATE_SIZEOFTIMEBUF);
        strftime(timebeganbuf, GAMESTATE_SIZEOFTIMEBUF, "Start Time: %Y-%m-%d %H:%M:%S", timebegantm);
        strftime(currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", currenttimetm);

        debugpanelon = player_input_received = is_locked = gridon = display_inventory_menu = display_quit_menu = display_help_menu = do_quit =
            processing_actions = cam_changed = gameover = test_guard = dirty_entities = display_sort_inventory_menu = music_volume_changed = false;

        gameplay_settings_menu_selection = 0;
        cam2d.target = cam2d.offset = (Vector2){0, 0};
        cam2d.zoom = 4.0f;
        cam2d.rotation = 0.0;
        fadealpha = 0.0;
        controlmode = CONTROLMODE_PLAYER;
        // current displayed dungeon floor
        flag = GAMESTATE_FLAG_PLAYER_INPUT;
        font_size = GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE;
        pad = 20;
        line_spacing = 1.0f;
        player_changing_dir = false;
        // weird bug maybe when set to 0?
        next_entityid = 1;
        current_music_index = 0;
        restart_count = 0;
        do_restart = 0;
        title_screen_selection = 0;
        max_title_screen_selections = 2;
        lock = 0;
        frame_updates = 0;
        framecount = 0;
        turn_count = 0;
        inventory_menu_selection = 0;

        debugpanel.pad_top = debugpanel.pad_left = debugpanel.pad_right = debugpanel.pad_bottom = 0;
        msg_history_max_len_msg = msg_history_max_len_msg_measure = 0;
        // initialize character creation
        chara_creation.name = "hero";
        chara_creation.strength = 10;
        chara_creation.dexterity = 10;
        chara_creation.intelligence = 10;
        chara_creation.wisdom = 10;
        chara_creation.constitution = 10;
        chara_creation.charisma = 10;
        chara_creation.race = RACE_HUMAN;
        chara_creation.hitdie = get_racial_hd(RACE_HUMAN);
        // why is the above line crashing?
        // the above line is also crashing
        msuccess("Gamestate character creation name set to empty string");
        current_scene = SCENE_TITLE;
        music_volume = DEFAULT_MUSIC_VOLUME;
        last_click_screen_pos = (Vector2){-1, -1};

        init_music_paths();
    }


    bool set_hero_id(entityid id) {
        massert(id != ENTITYID_INVALID, "id is invalid");
        if (id == ENTITYID_INVALID) {
            merror("g_set_hero_id: id is invalid");
            return false;
        }
        hero_id = id;
        msuccess("Hero ID set to %d", id);
        return true;
    }


    entityid add_entity() {
        const entityid id = next_entityid;
        if (!dirty_entities) {
            dirty_entities = true;
            new_entityid_begin = id;
            new_entityid_end = id + 1;
        } else {
            new_entityid_end = id + 1;
        }
        next_entityid++;
        return id;
    }
};
