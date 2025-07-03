#include "component.h"
#include "controlmode.h"
#include "direction.h"
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
#include <unordered_map>

//#include "inventory_sort.h"
//#include "item.h"
//#include "potion.h"
//#include "ringtype.h"
//#include "shield.h"
//#include "weapon.h"
//#include <stdbool.h>
//#include <vector>

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;

#define GAMESTATE_DEBUGPANEL_DEFAULT_X 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_Y 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE 20
#define GAMESTATE_INIT_ENTITYIDS_MAX 1000000

static void gamestate_init_music_paths(shared_ptr<gamestate> g);
static void gamestate_load_monster_defs(shared_ptr<gamestate> g);

// have to update this function when we introduce new fields to Gamestate
shared_ptr<gamestate> gamestateinitptr() {
    minfo("Initializing gamestate");
    const size_t n = LIST_INIT_CAPACITY;

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
    g->cam3d.projection = CAMERA_PERSPECTIVE, g->camera_mode = CAMERA_FREE;
    g->controlmode = CONTROLMODE_PLAYER;
    g->fadestate = FADESTATENONE;
    // current displayed dungeon floor
    g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
    g->font_size = GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE;
    g->pad = 20;
    g->line_spacing = 1.0f;

    g->next_entityid = 0;
    g->current_music_index = 0;
    g->total_music_paths = 0;
    g->restart_count = 0;
    g->do_restart = 0;

    g->title_screen_selection = 0;
    g->max_title_screen_selections = 2;

    g->monster_def_count = 0;
    g->monster_def_capacity = 0;
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

    g->monster_defs = NULL;

    gamestate_init_music_paths(g);

    g->msg_history = new vector<string>();
    g->msg_system = new vector<string>();

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
    gamestate_load_monster_defs(g);

    g->music_volume = DEFAULT_MUSIC_VOLUME;

    g->component_table = new unordered_map<entityid, long>();

    g->name_list = new unordered_map<entityid, string>();
    g->type_list = new unordered_map<entityid, entitytype_t>();
    g->race_list = new unordered_map<entityid, race_t>();
    g->loc_list = new unordered_map<entityid, vec3>();
    g->sprite_move_list = new unordered_map<entityid, Rectangle>();
    g->dir_list = new unordered_map<entityid, direction_t>();
    g->dead_list = new unordered_map<entityid, bool>();
    g->update_list = new unordered_map<entityid, bool>();
    g->attacking_list = new unordered_map<entityid, bool>();
    g->blocking_list = new unordered_map<entityid, bool>();
    g->block_success_list = new unordered_map<entityid, bool>();
    g->damaged_list = new unordered_map<entityid, bool>();


    g->tx_alpha_list = new unordered_map<entityid, int>();


    gamestate_load_keybindings(g);
    msuccess("Gamestate initialized successfully");
    return g;
}

static void gamestate_load_monster_defs(shared_ptr<gamestate> g) {
    massert(g, "g is NULL");
    const char* monster_defs_file = "monsters.csv";
    FILE* file = fopen(monster_defs_file, "r");
    massert(file, "Could not open monster definitions file: %s", monster_defs_file);
    char buffer[1024] = {0};
    // read the file line by line
    // only skip lines beginning with a #
    // even tho it is a .csv, the entries are separated by whitespace
    int count = 0;
    // first we need to run thru and count the num of entries we need space for
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
        if (buffer[0] == '#') continue;
        count++;
    }
    // rewind the file to the beginning
    rewind(file);
    // now we can allocate space for the monster definitions
    g->monster_defs = (monster_def*)malloc(sizeof(monster_def) * count);
    massert(g->monster_defs, "g->monster_defs is NULL");
    g->monster_def_count = 0, g->monster_def_capacity = count;
    // now we can read the file again and fill the monster definitions
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Remove newline character if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
        // if it begins with a #, skip for now
        if (buffer[0] == '#') continue;
        // parse the line into a monster_def
        monster_def* def = &g->monster_defs[g->monster_def_count];
        char race_name_buffer[1024] = {0};
        fscanf(file, "%s", race_name_buffer);
        // the below line is outdated
        // there are some new stats, so we need to read them all
        // hd_num hd_sides hd_mod ac st dx cn nt ws ch
        int hd_num, hd_sides, hd_mod, ac, st, dx, cn, nt, ws, ch;
        const char* format = "%d %d %d %d %d %d %d %d %d %d";
        fscanf(file, format, &hd_num, &hd_sides, &hd_mod, &ac, &st, &dx, &cn, &nt, &ws, &ch);
        def->hitdie = (vec3){hd_num, hd_sides, hd_mod};
        def->stats[STATS_AC] = ac;
        def->stats[STATS_STR] = st;
        def->stats[STATS_DEX] = dx;
        def->stats[STATS_CON] = cn;
        // we have not implemented these yet...
        //def->stats[STATS_INT] = nt;
        //def->stats[STATS_WIS] = ws;
        //def->stats[STATS_CHA] = ch;
        race_t race = str2race(race_name_buffer);
        def->race = race;
        g->monster_def_count++;

        g->message_history_bgcolor = DEFAULT_MSG_HIST_BGCOLOR;
        g->display_gameplay_settings_menu = false;
    }
    fclose(file);
    // at this point, we have loaded all the monster definitions and can use them
}

monster_def* g_get_monster_def(gamestate* const g, race_t r) {
    for (int i = 0; i < g->monster_def_count; i++) {
        if (g->monster_defs[i].race == r) {
            return &g->monster_defs[i];
        }
    }
    return NULL; // Not found
}

static void gamestate_init_music_paths(shared_ptr<gamestate> g) {
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

bool gamestate_init_msg_history(shared_ptr<gamestate> g) {
    massert(g, "g is NULL");
    g->msg_history = new vector<string>();
    if (!g->msg_history) {
        merror("Failed to allocate memory for message history");
        return false;
    }
    // no need to initialize the vector, it starts empty
    // no need to set max_count, we will use the vector's size
    return true;
}


void gamestate_free(shared_ptr<gamestate> g) {
    massert(g, "g is NULL");
    minfo("Freeing gamestate");

    //if (g->chara_creation) {
    //    g->chara_creation->name.clear(); // Clear the name string
    //    g->chara_creation.reset(); // Reset the shared pointer
    //    g->chara_creation = nullptr; // Set to nullptr
    //}

    if (g->msg_system) delete g->msg_system;
    if (g->msg_history) delete g->msg_history;
    if (g->component_table) delete g->component_table;
    if (g->name_list) delete g->name_list;
    if (g->type_list) delete g->type_list;
    if (g->race_list) delete g->race_list;
    if (g->loc_list) delete g->loc_list;
    if (g->sprite_move_list) delete g->sprite_move_list;
    if (g->dir_list) delete g->dir_list;
    if (g->dead_list) delete g->dead_list;
    if (g->update_list) delete g->update_list;
    if (g->attacking_list) delete g->attacking_list;
    if (g->blocking_list) delete g->blocking_list;
    if (g->block_success_list) delete g->block_success_list;
    if (g->damaged_list) delete g->damaged_list;
    if (g->tx_alpha_list) delete g->tx_alpha_list;

    if (g->monster_defs) {
        free(g->monster_defs);
    }
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


void gamestate_load_keybindings(shared_ptr<gamestate> g) {
    massert(g, "g is NULL");
    minfo("Loading keybindings from file");
    string filename = "keybindings.ini";
    // keybinding_list needs to be initialized
    if (!g->keybinding_list) {
        g->keybinding_list = make_shared<keybinding_list_t>();
        massert(g->keybinding_list, "g->keybinding_list is NULL");
    }
    load_keybindings(filename, g->keybinding_list);
    print_keybindings(g->keybinding_list);
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

    minfo("g_register_comp: Registering component %s for entity %d", component2str(comp), id);
    if (!g_has_comp(g, id, comp)) {
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
        minfo("Component %s registered for entity %d", component2str(comp), id);
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


bool g_has_name(shared_ptr<gamestate> g, entityid id) { return g_has_comp(g, id, C_NAME); }


bool g_add_name(shared_ptr<gamestate> g, entityid id, string name) {
    // Ensure entity exists in component table
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_NAME)) {
        merror("g_add_component failed for id %d", id);
        return false;
    }
    if (!g->name_list) {
        merror("g->name_list is NULL");
        return false;
    }
    (*g->name_list)[id] = name; // Insert or update the name
    return true;
}


string g_get_name(shared_ptr<gamestate> g, entityid id) {
    if (!g_has_name(g, id)) {
        merror("g_get_name: id %d does not have a name component", id);
        return "no-name-comp";
    }
    if (g->name_list) {
        // we can assume that the id is in the name_list
        massert(g->name_list->find(id) != g->name_list->end(), "g_get_name: id %d not found in name_list", id);
        return g->name_list->at(id);
    }
    return "no-name"; // Return an empty string if the id is not found
}


bool g_add_type(shared_ptr<gamestate> g, entityid id, entitytype_t type) {
    if (!g_add_comp(g, id, C_TYPE)) {
        merror("g_add_type: Failed to add component C_TYPE for id %d", id);
        return false;
    }
    if (!g->type_list) {
        merror("g->type_list is NULL");
        return false;
    }
    // Check if the type already exists for the entity
    (*g->type_list)[id] = type; // Insert or update the type
    return true;
}


entitytype_t g_get_type(shared_ptr<gamestate> g, entityid id) {
    //if (!g_has_comp(g, id, C_TYPE)) {
    if (!g_has_type(g, id)) {
        merror("g_get_type: id %d does not have a type component", id);
        return ENTITY_NONE; // Return ENTITY_NONE if the type component is not present
    }
    if (g->type_list) {
        massert(g->type_list->find(id) != g->type_list->end(), "g_get_type: id %d not found in type_list", id);
        return g->type_list->at(id);
    }
    return ENTITY_NONE; // Return ENTITY_NONE if the id is not found
}


bool g_has_type(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_comp(g, id, C_TYPE);
}


bool g_has_race(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_comp(g, id, C_RACE);
}


race_t g_get_race(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g_has_race(g, id)) {
        if (g->race_list) {
            massert(g->race_list->find(id) != g->race_list->end(), "g_get_race: id %d not found in race list", id);
            return g->race_list->at(id);
        }
    }
    merror("race component not found for id %d", id);
    return RACE_NONE;
}


bool g_add_race(shared_ptr<gamestate> g, entityid id, race_t race) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_RACE)) {
        merror("g_add_comp failed for id %d", id);
        return false;
    }
    if (!g->race_list) {
        merror("g->race_list is NULL");
        return false;
    }
    // Check if the type already exists for the entity
    (*g->race_list)[id] = race; // Insert or update the type
    return true;
}


bool g_update_race(shared_ptr<gamestate> g, entityid id, race_t race) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g_has_race(g, id)) {
        merror("g_update_race: id %d does not have a race component", id);
        return false;
    }
    if (!g->race_list) {
        merror("g->race_list is NULL");
        return false;
    }
    // Update
    (*g->race_list)[id] = race; // Insert or update the type
    return true;
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
        if (g->loc_list) {
            massert(g->loc_list->find(id) != g->loc_list->end(), "g_get_loc: id %d not found in loc list", id);
            return g->loc_list->at(id);
        }
    }
    merror("Location component not found for id %d", id);
    return (vec3){-1, -1, -1}; // Return an invalid location if not found
}


bool g_add_loc(shared_ptr<gamestate> g, entityid id, vec3 loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_LOCATION)) {
        merror("g_add_comp failed for id %d", id);
        return false;
    }
    if (!g->loc_list) {
        merror("g->loc_list is NULL");
        return false;
    }
    // Check if the location already exists for the entity
    (*g->loc_list)[id] = loc; // Insert or update the location
    return true;
}


bool g_update_loc(shared_ptr<gamestate> g, entityid id, vec3 loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->loc_list) {
        merror("g->loc_list is NULL");
        return false;
    }
    // Check if the entity has a location component
    if (g_has_loc(g, id)) {
        // Update the location for the entity
        (*g->loc_list)[id] = loc; // Update the location
        return true;
    }
    merror("g_update_loc: id %d does not have a location component", id);
    return false;
}


bool g_has_sprite_move(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_comp(g, id, C_SPRITE_MOVE);
}

Rectangle g_get_sprite_move(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g_has_sprite_move(g, id)) {
        if (g->sprite_move_list) {
            massert(g->sprite_move_list->find(id) != g->sprite_move_list->end(), "g_get_sprite_move: id %d not found in sprite move list", id);
            return g->sprite_move_list->at(id);
        }
    }
    merror("Sprite move component not found for id %d", id);
    return (Rectangle){0, 0, 0, 0}; // Return an empty rectangle if not found
}


bool g_add_sprite_move(shared_ptr<gamestate> g, entityid id, Rectangle loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_SPRITE_MOVE)) {
        merror("g_add_comp failed for id %d", id);
        return false;
    }
    if (!g->sprite_move_list) {
        merror("g->sprite_move_list is NULL");
        return false;
    }
    // Check if the sprite move already exists for the entity
    (*g->sprite_move_list)[id] = loc; // Insert or update the sprite move
    return true;
}


bool g_update_sprite_move(shared_ptr<gamestate> g, entityid id, Rectangle loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->sprite_move_list) {
        merror("g->sprite_move_list is NULL");
        return false;
    }
    // Check if the entity has a sprite move component
    if (g_has_sprite_move(g, id)) {
        // Update the sprite move for the entity
        (*g->sprite_move_list)[id] = loc; // Update the sprite move
        return true;
    }
    merror("g_update_sprite_move: id %d does not have a sprite move component", id);
    return false;
}


bool g_has_dir(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_comp(g, id, C_DIRECTION);
}


direction_t g_get_dir(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g_has_dir(g, id)) {
        if (g->dir_list) {
            massert(g->dir_list->find(id) != g->dir_list->end(), "g_get_dir: id %d not found in direction list", id);
            return g->dir_list->at(id);
        }
    }
    merror("Direction component not found for id %d", id);
    return DIR_NONE; // Return DIR_NONE if not found
}


bool g_add_dir(shared_ptr<gamestate> g, entityid id, direction_t dir) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_DIRECTION)) {
        merror("g_add_comp failed for id %d", id);
        return false;
    }
    if (!g->dir_list) {
        merror("g->dir_list is NULL");
        return false;
    }
    // Check if the direction already exists for the entity
    (*g->dir_list)[id] = dir; // Insert or update the direction
    return true;
}


bool g_update_dir(shared_ptr<gamestate> g, entityid id, direction_t dir) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->dir_list) {
        merror("g->dir_list is NULL");
        return false;
    }
    // Check if the entity has a direction component
    if (g_has_dir(g, id)) {
        // Update the direction for the entity
        (*g->dir_list)[id] = dir; // Update the direction
        return true;
    }
    merror("g_update_dir: id %d does not have a direction component", id);
    return false;
}


bool g_has_dead(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_comp(g, id, C_DEAD);
}


bool g_add_dead(shared_ptr<gamestate> g, entityid id, bool dead) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_DEAD)) {
        merror("g_add_dead: Failed to add component C_DEAD for id %d", id);
        return false;
    }
    if (!g->dead_list) {
        merror("g->dead_list is NULL");
        return false;
    }
    // Check if the dead status already exists for the entity
    (*g->dead_list)[id] = dead; // Insert or update the dead status
    return true;
}


bool g_update_dead(shared_ptr<gamestate> g, entityid id, bool dead) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->dead_list) {
        merror("g->dead_list is NULL");
        return false;
    }
    // Check if the entity has a dead component
    if (g_has_dead(g, id)) {
        // Update the dead status for the entity
        (*g->dead_list)[id] = dead; // Update the dead status
        return true;
    }
    merror("g_update_dead: id %d does not have a dead component", id);
    return false;
}


bool g_is_dead(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    if (id != ENTITYID_INVALID) {
        if (g->dead_list) {
            massert(g->dead_list->find(id) != g->dead_list->end(), "g_is_dead: id %d not found in dead list", id);
            return g->dead_list->at(id);
        }
    }
    merror("g_is_dead: id %d does not have a dead component", id);
    return false;
}


bool g_has_update(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_comp(g, id, C_UPDATE);
}


bool g_add_update(shared_ptr<gamestate> g, entityid id, bool update) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_UPDATE)) {
        merror("g_add_update: Failed to add component C_UPDATE for id %d", id);
        return false;
    }
    if (!g->update_list) {
        merror("g->update_list is NULL");
        return false;
    }
    // Check if the update status already exists for the entity
    (*g->update_list)[id] = update; // Insert or update the update status
    return true;
}


bool g_get_update(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->update_list) {
        massert(g->update_list->find(id) != g->update_list->end(), "g_get_update: id %d not found in update list", id);
        return g->update_list->at(id);
    }
    merror("g_get_update: id %d does not have an update component", id);
    return false; // Return false if the id is not found
}


bool g_set_update(shared_ptr<gamestate> g, entityid id, bool update) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->update_list) {
        merror("g->update_list is NULL");
        return false;
    }
    // Check if the entity has an update component
    if (g_has_update(g, id)) {
        // Update the update status for the entity
        (*g->update_list)[id] = update; // Update the update status
        return true;
    }
    merror("g_set_update: id %d does not have an update component", id);
    return false;
}


bool g_has_attacking(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_comp(g, id, C_ATTACKING);
}


bool g_add_attacking(shared_ptr<gamestate> g, entityid id, int attacking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_ATTACKING)) {
        merror("g_add_attacking: Failed to add component C_ATTACKING for id %d", id);
        return false;
    }
    if (!g->attacking_list) {
        merror("g->attacking_list is NULL");
        return false;
    }
    // Check if the attacking status already exists for the entity
    (*g->attacking_list)[id] = attacking; // Insert or update the attacking status
    return true;
}


bool g_set_attacking(shared_ptr<gamestate> g, entityid id, int attacking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->attacking_list) {
        merror("g->attacking_list is NULL");
        return false;
    }
    // Check if the entity has an attacking component
    if (g_has_attacking(g, id)) {
        // Update the attacking status for the entity
        (*g->attacking_list)[id] = attacking; // Update the attacking status
        return true;
    }
    merror("g_set_attacking: id %d does not have an attacking component", id);
    return false;
}


bool g_get_attacking(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->attacking_list) {
        massert(g->attacking_list->find(id) != g->attacking_list->end(), "g_get_attacking: id %d not found in attacking list", id);
        return g->attacking_list->at(id);
    }
    merror("g_get_attacking: id %d does not have an attacking component", id);
    return false; // Return false if the id is not found
}


bool g_has_blocking(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_comp(g, id, C_BLOCKING);
}


bool g_add_blocking(shared_ptr<gamestate> g, entityid id, int blocking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_BLOCKING)) {
        merror("g_add_blocking: Failed to add component C_BLOCKING for id %d", id);
        return false;
    }
    if (!g->blocking_list) {
        merror("g->blocking_list is NULL");
        return false;
    }
    // Check if the blocking status already exists for the entity
    (*g->blocking_list)[id] = blocking; // Insert or update the blocking status
    return true;
}


bool g_set_blocking(shared_ptr<gamestate> g, entityid id, int blocking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->blocking_list) {
        merror("g->blocking_list is NULL");
        return false;
    }
    // Check if the entity has a blocking component
    if (g_has_blocking(g, id)) {
        // Update the blocking status for the entity
        (*g->blocking_list)[id] = blocking; // Update the blocking status
        return true;
    }
    merror("g_set_blocking: id %d does not have a blocking component", id);
    return false;
}


bool g_get_blocking(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->blocking_list) {
        massert(g->blocking_list->find(id) != g->blocking_list->end(), "g_get_blocking: id %d not found in blocking list", id);
        return g->blocking_list->at(id);
    }
    merror("g_get_blocking: id %d does not have a blocking component", id);
    return false; // Return false if the id is not found
}


bool g_has_block_success(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_comp(g, id, C_BLOCK_SUCCESS);
}


bool g_add_block_success(shared_ptr<gamestate> g, entityid id, int block_success) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_BLOCK_SUCCESS)) {
        merror("g_add_block_success: Failed to add component C_BLOCK_SUCCESS for id %d", id);
        return false;
    }
    if (!g->block_success_list) {
        merror("g->block_success_list is NULL");
        return false;
    }
    // Check if the block success status already exists for the entity
    (*g->block_success_list)[id] = block_success; // Insert or update the block success status
    return true;
}


bool g_set_block_success(shared_ptr<gamestate> g, entityid id, int block_success) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->block_success_list) {
        merror("g->block_success_list is NULL");
        return false;
    }
    // Check if the entity has a block success component
    if (g_has_block_success(g, id)) {
        // Update the block success status for the entity
        (*g->block_success_list)[id] = block_success; // Update the block success status
        return true;
    }
    merror("g_set_block_success: id %d does not have a block success component", id);
    return false;
}


bool g_get_block_success(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->block_success_list) {
        massert(g->block_success_list->find(id) != g->block_success_list->end(), "g_get_block_success: id %d not found in block success list", id);
        return g->block_success_list->at(id);
    }
    merror("g_get_block_success: id %d does not have a block success component", id);
    return false; // Return false if the id is not found
}


bool g_has_damaged(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_comp(g, id, C_DAMAGED);
}


bool g_add_damaged(shared_ptr<gamestate> g, entityid id, int damaged) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_DAMAGED)) {
        merror("g_add_damaged: Failed to add component C_DAMAGED for id %d", id);
        return false;
    }
    if (!g->damaged_list) {
        merror("g->damaged_list is NULL");
        return false;
    }
    // Check if the damaged status already exists for the entity
    (*g->damaged_list)[id] = damaged; // Insert or update the damaged status
    return true;
}


bool g_set_damaged(shared_ptr<gamestate> g, entityid id, int damaged) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->damaged_list) {
        merror("g->damaged_list is NULL");
        return false;
    }
    // Check if the entity has a damaged component
    if (g_has_damaged(g, id)) {
        // Update the damaged status for the entity
        (*g->damaged_list)[id] = damaged; // Update the damaged status
        return true;
    }
    merror("g_set_damaged: id %d does not have a damaged component", id);
    return false;
}


bool g_get_damaged(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->damaged_list) {
        massert(g->damaged_list->find(id) != g->damaged_list->end(), "g_get_damaged: id %d not found in damaged list", id);
        return g->damaged_list->at(id);
    }
    merror("g_get_damaged: id %d does not have a damaged component", id);
    return false; // Return false if the id is not found
}


int g_get_tx_alpha(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->tx_alpha_list) {
        if (g_has_tx_alpha(g, id)) {
            return g->tx_alpha_list->at(id);
        }
        merror("g_get_tx_alpha: id %d does not have a tx alpha component", id);
        return 255;
    }
    merror("g_get_tx_alpha: g->tx_alpha_list is NULL");
    return 255; // Return -1 if the id is not found
}


bool g_has_tx_alpha(std::shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->tx_alpha_list) {
        merror("g->tx_alpha_list is NULL");
        return false;
    }
    // Check if the entity has a tx alpha component
    return g->tx_alpha_list->find(id) != g->tx_alpha_list->end();
}


bool g_set_tx_alpha(shared_ptr<gamestate> g, entityid id, int alpha) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->tx_alpha_list) {
        merror("g->tx_alpha_list is NULL");
        return false;
    }
    // Check if the entity has a tx alpha component
    if (g_has_tx_alpha(g, id)) {
        // Update the tx alpha for the entity
        (*g->tx_alpha_list)[id] = alpha; // Update the tx alpha
        return true;
    }
    merror("g_set_tx_alpha: id %d does not have a tx alpha component", id);
    return false;
}


bool g_add_tx_alpha(shared_ptr<gamestate> g, entityid id, int alpha) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_TX_ALPHA)) {
        merror("g_add_tx_alpha: Failed to add component C_TX_ALPHA for id %d", id);
        return false;
    }
    if (!g->tx_alpha_list) {
        merror("g->tx_alpha_list is NULL");
        return false;
    }
    // Check if the tx alpha already exists for the entity
    (*g->tx_alpha_list)[id] = alpha; // Insert or update the tx alpha
    return true;
}


bool g_incr_tx_alpha(shared_ptr<gamestate> g, entityid id, int alpha) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->tx_alpha_list) {
        merror("g->tx_alpha_list is NULL");
        return false;
    }
    // Check if the entity has a tx alpha component
    if (g->tx_alpha_list->find(id) != g->tx_alpha_list->end()) {
        // Increment the tx alpha for the entity
        //(*g->tx_alpha_list)[id] += alpha; // Increment the tx alpha

        // if the alpha is < 255, increment
        if ((*g->tx_alpha_list)[id] < 255) {
            (*g->tx_alpha_list)[id] += alpha; // Increment the tx alpha
            if ((*g->tx_alpha_list)[id] > 255) {
                (*g->tx_alpha_list)[id] = 255; // Cap at 255
            }
        }
        //else {
        //    merror("g_incr_tx_alpha: id %d already has max tx alpha", id);
        //}
        return true;
    }
    merror("g_incr_tx_alpha: id %d does not have a tx alpha component", id);
    return false;
}


bool g_decr_tx_alpha(shared_ptr<gamestate> g, entityid id, int alpha) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->tx_alpha_list) {
        merror("g->tx_alpha_list is NULL");
        return false;
    }
    // Check if the entity has a tx alpha component
    if (g->tx_alpha_list->find(id) != g->tx_alpha_list->end()) {
        // Decrement the tx alpha for the entity
        (*g->tx_alpha_list)[id] -= alpha; // Decrement the tx alpha
        return true;
    }
    merror("g_decr_tx_alpha: id %d does not have a tx alpha component", id);
    return false;
}


//bool g_has_default_action(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_DEFAULT_ACTION);
//}

//bool g_add_default_action(gamestate* const g, entityid id, int action) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    // make sure the entity has the default action component
//    return g_add_component(
//        g, id, C_DEFAULT_ACTION, (void*)&action, sizeof(int_component), (void**)&g->default_action_list, &g->default_action_list_count, &g->default_action_list_capacity);
//}

//bool g_set_default_action(gamestate* const g, entityid id, int action) {
//    massert(g, "g is NULL");
//    if (id == ENTITYID_INVALID) return false;
//    if (g->default_action_list == NULL) return false;
//    for (int i = 0; i < g->default_action_list_count; i++) {
//        if (g->default_action_list[i].id == id) {
//            g->default_action_list[i].data = action;
//            return true;
//        }
//    }
//    return false;
//}

//entity_action_t g_get_default_action(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->default_action_list == NULL) return ENTITY_ACTION_NONE;
//    for (int i = 0; i < g->default_action_list_count; i++)
//        if (g->default_action_list[i].id == id) return (entity_action_t)g->default_action_list[i].data;
//    return ENTITY_ACTION_NONE;
//}

//bool g_has_inventory(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_INVENTORY);
//}

//bool g_add_inventory(gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    // make sure the entity has the inventory component
//    return g_add_component(g, id, C_INVENTORY, (void*)NULL, sizeof(inventory_component), (void**)&g->inventory_list, &g->inventory_list_count, &g->inventory_list_capacity);
//    return true;
//}

// Comparison function for sorting by name
//static int compare_by_name(const void* a, const void* b) {
//    entityid id_a = *(entityid*)a;
//    entityid id_b = *(entityid*)b;
//    string name_a = g_get_name(g_sort_context, id_a);
//    string name_b = g_get_name(g_sort_context, id_b);
//    return name_a == name_b;
//}
// Comparison function for sorting by type
//static int compare_by_type(const void* a, const void* b) {
//    entityid id_a = *(entityid*)a;
//    entityid id_b = *(entityid*)b;
//    itemtype type_a = g_get_itemtype(g_sort_context, id_a);
//    itemtype type_b = g_get_itemtype(g_sort_context, id_b);
//    return type_a - type_b;
//}

//entityid* g_sort_inventory(gamestate* const g, entityid* inventory, int inv_count, inventory_sort sort_type) {
//entityid* g_sort_inventory(gamestate* const g, entityid* inventory, size_t inv_count, inventory_sort sort_type) {
//    massert(g, "gamestate is NULL");
//    massert(inventory, "inventory is NULL");
//    massert(inv_count >= 0, "invalid inventory count");
//    minfo("Sorting inventory...");
//minfo("Item count: %d", inv_count);
// Create a copy of the inventory array
//    entityid* sorted_inv = (entityid*)malloc(inv_count * sizeof(entityid));
//    massert(sorted_inv, "failed to allocate memory for sorted inventory");
//    memcpy(sorted_inv, inventory, inv_count * sizeof(entityid));
//    if (inv_count == 0 || inv_count == 1) {
//        minfo("No sorting needed, returning original inventory");
//        return sorted_inv; // No need to sort if there's 0 or 1 item
//    }
//    // Sort based on the specified type
//    g_sort_context = g; // Set global context
//    switch (sort_type) {
//    case INV_SORT_NAME: qsort(sorted_inv, inv_count, sizeof(entityid), compare_by_name); break;
//    case INV_SORT_TYPE: qsort(sorted_inv, inv_count, sizeof(entityid), compare_by_type); break;
//    default:
//        // No sorting needed, return copy as-is
//        break;
//    }
//    g_sort_context = NULL; // Clear global context
//    return sorted_inv;
//}

//bool g_add_to_inventory(gamestate* const g, entityid id, entityid itemid) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
//    massert(g->inventory_list, "g->inventory_list is NULL");
//    if (g->inventory_list == NULL) return false;
//    for (int i = 0; i < g->inventory_list_count; i++) {
//        if (g->inventory_list[i].id == id) {
//            if (g->inventory_list[i].count >= MAX_INVENTORY_SIZE) return false;
//            g->inventory_list[i].inventory[g->inventory_list[i].count++] = itemid;
//            msuccess("added item %d to inventory %d", itemid, id);
//            return true;
//        }
//    }
//    return false;
//}

//bool g_remove_from_inventory(gamestate* const g, entityid id, entityid itemid) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
//    massert(g->inventory_list, "g->inventory_list is NULL");
//    if (g->inventory_list == NULL) return false;
//    for (int i = 0; i < g->inventory_list_count; i++) {
//        if (g->inventory_list[i].id == id) {
//            for (size_t j = 0; j < g->inventory_list[i].count; j++) {
//                if (g->inventory_list[i].inventory[j] == itemid) {
//                    g->inventory_list[i].inventory[j] = g->inventory_list[i].inventory[g->inventory_list[i].count - 1];
//                    g->inventory_list[i].count--;
//                    msuccess("removed item %d from inventory %d", itemid, id);
//                    return true;
//                }
//            }
//        }
//    }
//    return false;
//}

//entityid* g_get_inventory(const gamestate* const g, entityid id, size_t* count) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    massert(count, "count is NULL");
//    massert(g->inventory_list, "g->inventory_list is NULL");
//    if (g->inventory_list == NULL) return NULL;
//    for (int i = 0; i < g->inventory_list_count; i++) {
//        if (g->inventory_list[i].id == id) {
//            *count = g->inventory_list[i].count;
//            return g->inventory_list[i].inventory;
//        }
//    }
//    *count = 0;
//    return NULL;
//}

//size_t g_get_inventory_count(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    massert(g->inventory_list, "g->inventory_list is NULL");
//    if (g->inventory_list == NULL) return 0;
//    for (int i = 0; i < g->inventory_list_count; i++)
//        if (g->inventory_list[i].id == id) return g->inventory_list[i].count;
//    return 0;
//}

//bool g_has_item_in_inventory(const gamestate* const g, entityid id, entityid itemid) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
//    massert(g->inventory_list, "g->inventory_list is NULL");
//    if (g->inventory_list == NULL) return false;
//    for (int i = 0; i < g->inventory_list_count; i++) {
//        if (g->inventory_list[i].id == id) {
//            for (size_t j = 0; j < g->inventory_list[i].count; j++)
//                if (g->inventory_list[i].inventory[j] == itemid) return true;
//        }
//    }
//    return false; //
//}

// updated inventory should have the same num of items as the original
//bool g_update_inventory(gamestate* const g, entityid id, entityid* new_inventory, int new_inventory_count) {
//bool g_update_inventory(gamestate* const g, entityid id, entityid* new_inventory, size_t new_inventory_count) {
// this assumes id has an inventory but lets verify
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    massert(new_inventory, "new_inventory is NULL");
//    massert(g->inventory_list, "g->inventory_list is NULL");
//    if (g->inventory_list == NULL) return false;
//    massert(g_has_inventory(g, id), "id %d does not have an inventory component", id);
//
//    for (int i = 0; i < g->inventory_list_count; i++) {
//        if (g->inventory_list[i].id == id) {
//            // clear the old inventory
//            g->inventory_list[i].count = 0;
//            // copy the new inventory
//            //for (int j = 0; j < MAX_INVENTORY_SIZE && new_inventory[j] != ENTITYID_INVALID; j++) {
//            for (size_t j = 0; j < new_inventory_count && j < MAX_INVENTORY_SIZE; j++) {
//                g->inventory_list[i].inventory[j] = new_inventory[j];
//                g->inventory_list[i].count++;
//            }
//            massert(
//                g->inventory_list[i].count == new_inventory_count, "new inventory count %ld does not match expected count %zu", g->inventory_list[i].count, new_inventory_count);
//            if (g->inventory_list[i].count == new_inventory_count) {
//                msuccess("updated inventory for id %d with %ld items", id, new_inventory_count);
//                return true;
//            }
//            merror("updated inventory for id %d but count mismatch: expected %ld, got %ld", id, new_inventory_count, g->inventory_list[i].count);
//            return false;
//        }
//    }
//    merror("id %d not found in inventory_list", id);
//    return false;
//}

//bool g_has_target(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_TARGET);
//}

//bool g_add_target(gamestate* const g, entityid id, vec3 target) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    // make sure the entity has the target component
//    return g_add_component(g, id, C_TARGET, (void*)&target, sizeof(vec3_component), (void**)&g->target_list, &g->target_list_count, &g->target_list_capacity);
//}

//bool g_set_target(gamestate* const g, entityid id, vec3 target) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->target_list == NULL) return false;
//    for (int i = 0; i < g->target_list_count; i++) {
//        if (g->target_list[i].id == id) {
//            g->target_list[i].data = target;
//            return true;
//        }
//    }
//    return false;
//}

//bool g_get_target(const gamestate* const g, entityid id, vec3* target) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    massert(target, "target is NULL");
//    if (g->target_list == NULL) return false;
//    for (int i = 0; i < g->target_list_count; i++) {
//        if (g->target_list[i].id == id) {
//            *target = g->target_list[i].data;
//            return true;
//        }
//    }
//    return false;
//}

//bool g_has_target_path(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_TARGET_PATH);
//}

//bool g_add_target_path(gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    // make sure the entity has the target path component
//    return g_add_component(
//        g, id, C_TARGET_PATH, (void*)NULL, sizeof(target_path_component), (void**)&g->target_path_list, &g->target_path_list_count, &g->target_path_list_capacity);
//    return true;
//}

//bool g_set_target_path(gamestate* const g, entityid id, vec3* target_path, int target_path_length) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->target_path_list == NULL) return false;
//    for (int i = 0; i < g->target_path_list_count; i++) {
//        if (g->target_path_list[i].id == id) {
//            g->target_path_list[i].target_path = target_path;
//            g->target_path_list[i].target_path_length = target_path_length;
//            return true;
//        }
//    }
//    return false;
//}

//bool g_get_target_path(const gamestate* const g, entityid id, vec3** target_path, int* target_path_length) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    massert(target_path, "target_path is NULL");
//    massert(target_path_length, "target_path_length is NULL");
//    if (g->target_path_list == NULL) return false;
//    for (int i = 0; i < g->target_path_list_count; i++) {
//        if (g->target_path_list[i].id == id) {
//            *target_path = g->target_path_list[i].target_path;
//            *target_path_length = g->target_path_list[i].target_path_length;
//            return true;
//        }
//    }
//    return false;
//}

//bool g_get_target_path_length(const gamestate* const g, entityid id, int* target_path_length) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    massert(target_path_length, "target_path_length is NULL");
//    if (g->target_path_list == NULL) return false;
//    for (int i = 0; i < g->target_path_list_count; i++) {
//        if (g->target_path_list[i].id == id) {
//            *target_path_length = g->target_path_list[i].target_path_length;
//            return true;
//        }
//    }
//    return false;
//}

//bool g_add_equipment(gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    // make sure the entity has the equipment component
//    return g_add_component(g, id, C_EQUIPMENT, (void*)NULL, sizeof(equipment_component), (void**)&g->equipment_list, &g->equipment_list_count, &g->equipment_list_capacity);
//}

//bool g_has_equipment(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_EQUIPMENT);
//}

//bool g_set_equipment(gamestate* const g, entityid id, equipment_slot slot, entityid itemid) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
//    if (g->equipment_list != NULL) {
//        for (int i = 0; i < g->equipment_list_count; i++) {
//            if (g->equipment_list[i].id == id) {
//                g->equipment_list[i].equipment[slot] = itemid;
//                return true;
//            }
//        }
//    }
//    return false;
//}

//bool g_unset_equipment(gamestate* const g, entityid id, equipment_slot slot) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->equipment_list != NULL) {
//        for (int i = 0; i < g->equipment_list_count; i++) {
//            if (g->equipment_list[i].id == id) {
//                g->equipment_list[i].equipment[slot] = ENTITYID_INVALID;
//                return true;
//            }
//        }
//    }
//    return false;
//}

//entityid g_get_equipment(const gamestate* const g, entityid id, equipment_slot slot) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->equipment_list != NULL)
//        for (int i = 0; i < g->equipment_list_count; i++)
//            if (g->equipment_list[i].id == id) return g->equipment_list[i].equipment[slot];
//    return ENTITYID_INVALID;
//}

//bool g_is_equipped(const gamestate* const g, entityid id, entityid itemid) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
//    if (g->equipment_list != NULL) {
//        for (int i = 0; i < g->equipment_list_count; i++)
//            if (g->equipment_list[i].id == id)
//                for (int j = 0; j < EQUIPMENT_SLOT_COUNT; j++)
//                    if (g->equipment_list[i].equipment[j] == itemid) return true;
//    }
//    return false;
//}

//dungeon_t* g_get_dungeon(gamestate* const g) {
//shared_ptr<dungeon_t> g_get_dungeon(gamestate* const g) {
//    massert(g, "g is NULL");
//    return g->dungeon;
//}

//bool g_add_stats(gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_add_component(g, id, C_STATS, (void*)NULL, sizeof(stats_component), (void**)&g->stats_list, &g->stats_list_count, &g->stats_list_capacity);
//}

//bool g_has_stats(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_STATS);
//}

//bool g_set_stat(gamestate* const g, entityid id, stats_slot stats_slot, int value) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    massert(g->stats_list, "g->stats_list is NULL");
//    if (g->stats_list == NULL) return false;
//    for (int i = 0; i < g->stats_list_count; i++) {
//        if (g->stats_list[i].id == id) {
//            g->stats_list[i].stats[stats_slot] = value;
//            return true;
//        }
//    }
//    return false;
//}

//int* g_get_stats(const gamestate* const g, entityid id, int* count) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    massert(count, "count is NULL");
//    massert(g->stats_list, "g->stats_list is NULL");
//    if (g->stats_list == NULL) return NULL;
//    for (int i = 0; i < g->stats_list_count; i++) {
//        if (g->stats_list[i].id == id) {
//            *count = STATS_COUNT;
//            return g->stats_list[i].stats;
//        }
//    }
//    *count = 0;
//    return NULL;
//}

//int g_get_stat(const gamestate* const g, entityid id, stats_slot stats_slot) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    massert(g->stats_list, "g->stats_list is NULL");
//    if (g->stats_list == NULL) return 0;
//    for (int i = 0; i < g->stats_list_count; i++)
//        if (g->stats_list[i].id == id) return g->stats_list[i].stats[stats_slot];
//    return 0;
//}

//bool g_add_itemtype(gamestate* const g, entityid id, int type) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_add_component(g, id, C_ITEMTYPE, (void*)&type, sizeof(int_component), (void**)&g->itemtype_list, &g->itemtype_list_count, &g->itemtype_list_capacity);
//}

//bool g_has_itemtype(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_ITEMTYPE);
//}

//bool g_set_itemtype(gamestate* const g, entityid id, int type) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->itemtype_list == NULL) return false;
//    for (int i = 0; i < g->itemtype_list_count; i++) {
//        if (g->itemtype_list[i].id == id) {
//            g->itemtype_list[i].data = type;
//            return true;
//        }
//    }
//    return false;
//}

//itemtype g_get_itemtype(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->itemtype_list == NULL) return ITEM_NONE;
//    for (int i = 0; i < g->itemtype_list_count; i++)
//        if (g->itemtype_list[i].id == id) return (itemtype)g->itemtype_list[i].data;
//    return ITEM_NONE;
//}

//bool g_add_weapontype(gamestate* const g, entityid id, int type) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    // make sure the entity has the weapontype component
//    return g_add_component(g, id, C_WEAPONTYPE, (void*)&type, sizeof(int_component), (void**)&g->weapontype_list, &g->weapontype_list_count, &g->weapontype_list_capacity);
//}

//bool g_has_weapontype(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_WEAPONTYPE);
//}

//bool g_set_weapontype(gamestate* const g, entityid id, int type) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->weapontype_list == NULL) return false;
//    for (int i = 0; i < g->weapontype_list_count; i++) {
//        if (g->weapontype_list[i].id == id) {
//            g->weapontype_list[i].data = type;
//            return true;
//        }
//    }
//    return false;
//}

//weapontype g_get_weapontype(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    if (id == ENTITYID_INVALID) return WEAPON_NONE;
//    if (g->weapontype_list == NULL) return WEAPON_NONE;
//    for (int i = 0; i < g->weapontype_list_count; i++)
//        if (g->weapontype_list[i].id == id) return (weapontype)g->weapontype_list[i].data;
//    return WEAPON_NONE;
//}

//bool g_is_weapontype(const gamestate* const g, entityid id, int type) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->weapontype_list == NULL) return false;
//    for (int i = 0; i < g->weapontype_list_count; i++)
//        if (g->weapontype_list[i].id == id) return g->weapontype_list[i].data == type;
//    return false;
//}

//bool g_add_shieldtype(gamestate* const g, entityid id, int type) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    // make sure the entity has the shieldtype component
//    return g_add_component(g, id, C_SHIELDTYPE, (void*)&type, sizeof(int_component), (void**)&g->shieldtype_list, &g->shieldtype_list_count, &g->shieldtype_list_capacity);
//}

//bool g_has_shieldtype(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_SHIELDTYPE);
//}

//bool g_set_shieldtype(gamestate* const g, entityid id, int type) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->shieldtype_list == NULL) return false;
//    for (int i = 0; i < g->shieldtype_list_count; i++) {
//        if (g->shieldtype_list[i].id == id) {
//            g->shieldtype_list[i].data = type;
//            return true;
//        }
//    }
//    return false;
//}

//shieldtype g_get_shieldtype(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->shieldtype_list == NULL) return SHIELD_NONE;
//    for (int i = 0; i < g->shieldtype_list_count; i++)
//        if (g->shieldtype_list[i].id == id) return (shieldtype)g->shieldtype_list[i].data;
//    return SHIELD_NONE;
//}

//bool g_is_shieldtype(const gamestate* const g, entityid id, int type) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->shieldtype_list == NULL) return false;
//    for (int i = 0; i < g->shieldtype_list_count; i++)
//        if (g->shieldtype_list[i].id == id) return g->shieldtype_list[i].data == type;
//    return false;
//}

//bool g_add_potiontype(gamestate* const g, entityid id, int type) {
//    minfo("g_add_potiontype");
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    // make sure the entity has the potiontype component
//    return g_add_component(g, id, C_POTIONTYPE, (void*)&type, sizeof(int_component), (void**)&g->potion_type_list, &g->potion_type_list_count, &g->potion_type_list_capacity);
//}

//bool g_has_potiontype(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_POTIONTYPE);
//}

//bool g_set_potiontype(gamestate* const g, entityid id, int type) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->potion_type_list == NULL) return false;
//    for (int i = 0; i < g->potion_type_list_count; i++) {
//        if (g->potion_type_list[i].id == id) {
//            g->potion_type_list[i].data = type;
//            return true;
//        }
//    }
//    return false;
//}

//bool g_is_potion(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->potion_type_list == NULL) return false;
//    return g_has_potiontype(g, id);
//}

//potiontype g_get_potiontype(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->potion_type_list == NULL) {
//        merror("g->potion_type_list is NULL");
//        return POTION_NONE;
//    }
//    for (int i = 0; i < g->potion_type_list_count; i++) {
//        if (g->potion_type_list[i].id == id) {
//            return (potiontype)g->potion_type_list[i].data;
//        }
//    }
//    return POTION_NONE;
//}

//bool g_add_damage(gamestate* const g, entityid id, vec3 r) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_add_component(g, id, C_DAMAGE, (void*)&r, sizeof(vec3_component), (void**)&g->damage_list, &g->damage_list_count, &g->damage_list_capacity);
//}

//bool g_has_damage(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_DAMAGE);
//}

//bool g_set_damage(gamestate* const g, entityid id, vec3 r) {
//    massert(g, "g is NULL");
//    if (id == ENTITYID_INVALID) return false;
//    if (g->damage_list == NULL) return false;
//    for (int i = 0; i < g->damage_list_count; i++) {
//        if (g->damage_list[i].id == id) {
//            g->damage_list[i].data = r;
//            return true;
//        }
//    }
//    return false;
//}

//vec3 g_get_damage(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->damage_list == NULL) return (vec3){0, 0, 0};
//    for (int i = 0; i < g->damage_list_count; i++)
//        if (g->damage_list[i].id == id) return g->damage_list[i].data;
//    return (vec3){0, 0, 0};
//}

//bool g_add_ac(gamestate* const g, entityid id, int ac) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    // make sure the entity has the ac component
//    return g_add_component(g, id, C_AC, (void*)&ac, sizeof(int_component), (void**)&g->ac_list, &g->ac_list_count, &g->ac_list_capacity);
//}

//bool g_has_ac(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_AC);
//}

//bool g_set_ac(gamestate* const g, entityid id, int ac) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->ac_list == NULL) {
//        merror("g->ac_list is NULL");
//        return false;
//    }
//    for (int i = 0; i < g->ac_list_count; i++) {
//        if (g->ac_list[i].id == id) {
//            g->ac_list[i].data = ac;
//            return true;
//        }
//    }
//    return false;
//}

//int g_get_ac(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->ac_list == NULL) {
//        merror("g->ac_list is NULL");
//        return 0;
//    }
//    for (int i = 0; i < g->ac_list_count; i++) {
//        if (g->ac_list[i].id == id) {
//            return g->ac_list[i].data;
//        }
//    }
//    return 0;
//}


//bool g_has_zapping(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_ZAPPING);
//}

//bool g_add_zapping(gamestate* const g, entityid id, int zapping) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    // make sure the entity has the zapping component
//    return g_add_component(g, id, C_ZAPPING, (void*)&zapping, sizeof(zapping_component), (void**)&g->zapping_list, &g->zapping_list_count, &g->zapping_list_capacity);
//}

//bool g_set_zapping(gamestate* const g, entityid id, int zapping) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->zapping_list == NULL) return false;
//    for (int i = 0; i < g->zapping_list_count; i++) {
//        if (g->zapping_list[i].id == id) {
//            g->zapping_list[i].data = zapping;
//            return true;
//        }
//    }
//    return false;
//}

//bool g_get_zapping(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->zapping_list == NULL) return false;
//    for (int i = 0; i < g->zapping_list_count; i++)
//        if (g->zapping_list[i].id == id) return g->zapping_list[i].data;
//    return false; // not zapping
//}

//bool g_add_base_attack_damage(gamestate* const g, entityid id, vec3 damage) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    // make sure the entity has the base attack damage component
//    return g_add_component(g,
//                           id,
//                           C_BASE_ATTACK_DAMAGE,
//                           (void*)&damage,
//                           sizeof(vec3_component),
//                           (void**)&g->base_attack_damage_list,
//                           &g->base_attack_damage_list_count,
//                           &g->base_attack_damage_list_capacity);
//}

//bool g_has_base_attack_damage(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_BASE_ATTACK_DAMAGE);
//}

//bool g_set_base_attack_damage(gamestate* const g, entityid id, vec3 damage) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->base_attack_damage_list == NULL) return false;
//    for (int i = 0; i < g->base_attack_damage_list_count; i++) {
//        if (g->base_attack_damage_list[i].id == id) {
//            g->base_attack_damage_list[i].data = damage;
//            return true;
//        }
//    }
//    return false;
//}

//vec3 g_get_base_attack_damage(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->base_attack_damage_list == NULL) return (vec3){0, 0, 0};
//    for (int i = 0; i < g->base_attack_damage_list_count; i++) {
//        if (g->base_attack_damage_list[i].id == id) return g->base_attack_damage_list[i].data;
//    }
//    return (vec3){0, 0, 0};
//}

//bool g_add_vision_distance(gamestate* const g, entityid id, int distance) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
// make sure the entity has the vision distance component
//    return g_add_component(
//        g, id, C_VISION_DISTANCE, (void*)&distance, sizeof(int_component), (void**)&g->vision_distance_list, &g->vision_distance_list_count, &g->vision_distance_list_capacity);
//}

//bool g_has_vision_distance(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_VISION_DISTANCE);
//}

//bool g_set_vision_distance(gamestate* const g, entityid id, int distance) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->vision_distance_list == NULL) return false;
//    for (int i = 0; i < g->vision_distance_list_count; i++) {
//        if (g->vision_distance_list[i].id == id) {
//            g->vision_distance_list[i].data = distance;
//            return true;
//        }
//    }
//    return false;
//}

//int g_get_vision_distance(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->vision_distance_list == NULL) return 0;
//    for (int i = 0; i < g->vision_distance_list_count; i++) {
//        if (g->vision_distance_list[i].id == id) return g->vision_distance_list[i].data;
//    }
//    return 0;
//}

/*
bool g_add_light_radius(gamestate* const g, entityid id, int radius) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the light radius component
    return g_add_component(
        g, id, C_LIGHT_RADIUS, (void*)&radius, sizeof(int_component), (void**)&g->light_radius_list, &g->light_radius_list_count, &g->light_radius_list_capacity);
}

bool g_has_light_radius(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_LIGHT_RADIUS);
}

bool g_set_light_radius(gamestate* const g, entityid id, int radius) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->light_radius_list == NULL) return false;
    for (int i = 0; i < g->light_radius_list_count; i++) {
        if (g->light_radius_list[i].id == id) {
            g->light_radius_list[i].data = radius;
            return true;
        }
    }
    return false;
}

int g_get_light_radius(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->light_radius_list == NULL) return 0;
    for (int i = 0; i < g->light_radius_list_count; i++) {
        if (g->light_radius_list[i].id == id) return g->light_radius_list[i].data;
    }
    return 0;
}
*/

//bool g_add_ringtype(gamestate* const g, entityid id, int type) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    // make sure the entity has the ringtype component
//    if (type < RING_NONE || type >= RING_TYPE_COUNT) {
//        merror("Invalid ring type %d", type);
//        return false;
//    }
//    g_add_component(g, id, C_RINGTYPE, (void*)&type, sizeof(int_component), (void**)&g->ringtype_list, &g->ringtype_list_count, &g->ringtype_list_capacity);
//    return true;
//}

//bool g_has_ringtype(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_component(g, id, C_RINGTYPE);
//}

//bool g_set_ringtype(gamestate* const g, entityid id, int type) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->ringtype_list == NULL) return false;
//    for (int i = 0; i < g->ringtype_list_count; i++) {
//        if (g->ringtype_list[i].id == id) {
//            g->ringtype_list[i].data = type;
//            return true;
//        }
//    }
//    return false;
//}

//ringtype g_get_ringtype(const gamestate* const g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g->ringtype_list == NULL) return RING_NONE;
//    for (int i = 0; i < g->ringtype_list_count; i++) {
//        if (g->ringtype_list[i].id == id) return (ringtype)g->ringtype_list[i].data;
//    }
//    return RING_NONE;
//}

/*
bool g_add_light_radius_bonus(gamestate* const g, entityid id, int radius) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the light radius bonus component
    return g_add_component(g,
                           id,
                           C_LIGHT_RADIUS_BONUS,
                           (void*)&radius,
                           sizeof(int_component),
                           (void**)&g->light_radius_bonus_list,
                           &g->light_radius_bonus_list_count,
                           &g->light_radius_bonus_list_capacity);
}

bool g_has_light_radius_bonus(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_LIGHT_RADIUS_BONUS);
}

bool g_set_light_radius_bonus(gamestate* const g, entityid id, int radius) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->light_radius_bonus_list == NULL) return false;
    for (int i = 0; i < g->light_radius_bonus_list_count; i++) {
        if (g->light_radius_bonus_list[i].id == id) {
            g->light_radius_bonus_list[i].data = radius;
            return true;
        }
    }
    return false;
}

int g_get_light_radius_bonus(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->light_radius_bonus_list == NULL) return 0;
    for (int i = 0; i < g->light_radius_bonus_list_count; i++) {
        if (g->light_radius_bonus_list[i].id == id) {
            return g->light_radius_bonus_list[i].data;
        }
    }
    return 0;
}

int g_get_entity_total_light_radius_bonus(const gamestate* const g, entityid id) {
    int total_light_radius_bonus = 0;
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
    // check each equipment slot
    for (int i = 0; i < EQUIPMENT_SLOT_COUNT; i++) {
        entityid equip_id = g_get_equipment(g, id, i);
        if (equip_id == ENTITYID_INVALID) continue;
        if (!g_has_light_radius_bonus(g, equip_id)) continue;
        total_light_radius_bonus += g_get_light_radius_bonus(g, equip_id);
    }
    // only return the total light radius bonus
    // it is fine if it is negative that might be fun for cursed items
    return total_light_radius_bonus;
}
*/
