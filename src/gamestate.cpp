#include "component.h"
#include "controlmode.h"
//#include "direction.h"
#include "entityid.h"
#include "gamestate.h"
#include "gamestate_flag.h"
#include "libgame_defines.h"
#include "massert.h"
#include "mprint.h"
//#include "stats_slot.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <memory>
#include <raylib.h>
#include <string>
#include <unordered_map>

//#include "item.h"
//#include "ringtype.h"
//#include "shield.h"

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::unordered_map;
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

    g->next_entityid = 0;
    g->current_music_index = 0;
    g->total_music_paths = 0;
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

    gamestate_init_music_paths(g);

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

    g->component_table = make_shared<unordered_map<entityid, long>>();

    g->equipped_weapon_list = make_shared<unordered_map<entityid, entityid>>();

    g->last_click_screen_pos = (Vector2){-1, -1};

    msuccess("Gamestate initialized successfully");
    return g;
}


void gamestate_init_music_paths(shared_ptr<gamestate> g) {
    massert(g, "g is NULL");
    const char* music_path_file = "music.txt";
    FILE* file = fopen(music_path_file, "r");
    massert(file, "Could not open music path file: %s", music_path_file);
    int i = 0;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Remove newline character if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
        // if it begins with a #, skip for now
        if (buffer[0] == '#') continue;
        // copy into g->music_file_paths[i]
        // we have to add "audio/music/" to the beginning of the path
        if (i < 1024) {
            snprintf(g->music_file_paths[i], sizeof(g->music_file_paths[i]), "%s", buffer);
            g->music_file_paths[i][sizeof(g->music_file_paths[i]) - 1] = '\0'; // Ensure null termination
            i++;
        } else {
            merror("Too many music paths in %s", music_path_file);
            break;
        }
    }
    fclose(file);
    g->total_music_paths = i;
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


bool g_register_comp(shared_ptr<gamestate> g, entityid id, component comp) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(comp != C_COUNT, "comp is invalid");
    //minfo("g_register_comp: Registering component %s for entity %d", component2str(comp), id);
    if (!g_has_comp(g, id, comp)) {
        //minfo("g_register_comp: component unregistered. Registering...");
        // If the component is not already registered, add it
        if (g->component_table->find(id) == g->component_table->end()) {
            (*g->component_table)[id] = 0; // Initialize with 0 components
        }
        (*g->component_table)[id] |= (1L << comp); // Set the bit for the component
        // assert that the bit has been set correctly
        long components = g->component_table->at(id);
        long comp_bit = 1L << comp; // Shift 1 to the left by 'comp' positions
        massert((components & comp_bit) != 0, "g_register_comp: Component %s not set for entity %d", component2str(comp), id);
        //merror("g_register_comp: Failed to set component %s for entity %d", component2str(comp), id);
        //return false; // Failed to set the component
        //}
    }
    // Check if the component was successfully registered
    if (g_has_comp(g, id, comp)) {
        //minfo("Component %s already registered for entity %d", component2str(comp), id);
        return true;
    }
    merror("Failed to register component %s for entity %d", component2str(comp), id);
    return false;
}


bool g_add_comp(shared_ptr<gamestate> g, entityid id, component comp) { return g_register_comp(g, id, comp); }


bool g_has_comp(shared_ptr<gamestate> g, entityid id, component comp) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(comp != C_COUNT, "comp is invalid");
    massert(g->component_table, "g->component_table is NULL");
    // Check if the entity exists in the component table
    // component table is now an unordered_map
    //minfo("g_has_comp: Checking if entity %d has component %s", id, component2str(comp));
    if (g->component_table->find(id) == g->component_table->end()) {
        return false; // Entity does not exist
    }
    // Check if the component is registered for the entity
    long components = g->component_table->at(id);
    // Check if the component bit is set
    long comp_bit = 1L << comp; // Shift 1 to the left by 'comp' positions
    if ((components & comp_bit) != 0) {
        return true; // Component is registered
    }
    return false; // Component is not registered
}


bool g_has_loc(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_comp(g, id, C_LOCATION);
}


vec3 g_get_loc(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g_has_loc(g, id)) {
        //if (g->loc_list) {
        //    massert(g->loc_list->find(id) != g->loc_list->end(), "g_get_loc: id %d not found in loc list", id);
        //    return g->loc_list->at(id);
        //}
    }
    merror("Location component not found for id %d", id);
    return (vec3){-1, -1, -1}; // Return an invalid location if not found
}
