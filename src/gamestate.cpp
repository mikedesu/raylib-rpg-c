#include "controlmode.h"
#include "entityid.h"
#include "gamestate.h"
#include "gamestate_flag.h"
#include "libgame_defines.h"
#include "massert.h"
#include "mprint.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <memory>
#include <raylib.h>
#include <string>

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

#define GAMESTATE_DEBUGPANEL_DEFAULT_X 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_Y 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE 20
#define GAMESTATE_INIT_ENTITYIDS_MAX 1000000


// have to update this function when we introduce new fields to Gamestate
shared_ptr<gamestate> gamestateinitptr() {
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
    g->cam2d.zoom = 7.0f;
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
    g->fadestate = FADESTATENONE;
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
    g->chara_creation = make_shared<character_creation>();

    g->chara_creation->name = "hero";
    g->chara_creation->strength = 10;
    g->chara_creation->dexterity = 10;
    g->chara_creation->constitution = 10;
    g->chara_creation->hitdie = 6; // 1d6 hit die
    g->chara_creation->race = RACE_HUMAN;

    // why is the above line crashing?
    // the above line is also crashing
    msuccess("Gamestate character creation name set to empty string");

    g->current_scene = SCENE_TITLE;

    g->music_volume = DEFAULT_MUSIC_VOLUME;

    //g->component_table = make_shared<unordered_map<entityid, long>>();

    //g->equipped_weapon_list = make_shared<unordered_map<entityid, entityid>>();

    g->last_click_screen_pos = (Vector2){-1, -1};

    gamestate_init_music_paths(g);

    msuccess("Gamestate initialized successfully");
    return g;
}


void gamestate_init_music_paths(shared_ptr<gamestate> g) {
    massert(g, "g is NULL");
    minfo("Initializing music paths...");
    if (!g->music_file_paths) {
        g->music_file_paths = make_shared<vector<string>>();
    }
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

        g->music_file_paths->push_back(fullpath);
    }
    fclose(file);
    msuccess("Music paths initialized!");
}


void gamestate_free(shared_ptr<gamestate> g) {
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


entityid g_add_entity(shared_ptr<gamestate> g) {
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


bool g_set_hero_id(shared_ptr<gamestate> g, entityid id) {
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


entityid gamestate_get_hero_id(const gamestate* const g) {
    massert(g, "g is NULL");
    return g->hero_id;
}


bool gamestate_add_msg_history(shared_ptr<gamestate> g, string msg) {
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


void gamestate_set_debug_panel_pos_bottom_left(gamestate* const g) {
    massert(g, "g is NULL");
    if (g->windowwidth == -1 || g->windowheight == -1) return;
    g->debugpanel.x = 0;
    g->debugpanel.y = g->windowheight - g->debugpanel.h;
}


void gamestate_set_debug_panel_pos_top_right(gamestate* const g) {
    massert(g, "g is NULL");
    if (g->windowwidth == -1 || g->windowheight == -1) return;
    g->debugpanel.x = g->windowwidth - g->debugpanel.w;
    g->debugpanel.y = g->debugpanel.pad_right;
}
