#include "component.h"
#include "controlmode.h"
#include "direction.h"
#include "entityid.h"
#include "gamestate.h"
#include "gamestate_flag.h"
#include "libgame_defines.h"
#include "massert.h"
#include "mprint.h"
#include "stats_slot.h"
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
    //g->name_list = make_shared<unordered_map<entityid, string>>();
    //g->type_list = make_shared<unordered_map<entityid, entitytype_t>>();
    //g->race_list = make_shared<unordered_map<entityid, race_t>>();
    g->loc_list = make_shared<unordered_map<entityid, vec3>>();
    g->sprite_move_list = make_shared<unordered_map<entityid, Rectangle>>();
    g->dir_list = make_shared<unordered_map<entityid, direction_t>>();
    g->dead_list = make_shared<unordered_map<entityid, bool>>();
    g->update_list = make_shared<unordered_map<entityid, bool>>();
    g->attacking_list = make_shared<unordered_map<entityid, bool>>();
    g->blocking_list = make_shared<unordered_map<entityid, bool>>();
    g->block_success_list = make_shared<unordered_map<entityid, bool>>();
    g->damaged_list = make_shared<unordered_map<entityid, bool>>();
    g->pushable_list = make_shared<unordered_map<entityid, bool>>();
    g->tx_alpha_list = make_shared<unordered_map<entityid, int>>();
    g->item_type_list = make_shared<unordered_map<entityid, itemtype>>();
    g->potion_type_list = make_shared<unordered_map<entityid, potiontype>>();
    g->weapon_type_list = make_shared<unordered_map<entityid, weapontype>>();
    g->stats_list = make_shared<unordered_map<entityid, shared_ptr<unordered_map<int, int>>>>();
    g->equipped_weapon_list = make_shared<unordered_map<entityid, entityid>>();
    g->inventory_list = make_shared<unordered_map<entityid, shared_ptr<vector<entityid>>>>();

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

//bool gamestate_init_msg_history(shared_ptr<gamestate> g) {
//    massert(g, "g is NULL");
//g->msg_history = new vector<string>();
//if (!g->msg_history) {
//    merror("Failed to allocate memory for message history");
//    return false;
//}
// no need to initialize the vector, it starts empty
// no need to set max_count, we will use the vector's size
//    return true;
//}


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


//bool g_has_name(shared_ptr<gamestate> g, entityid id) { return g_has_comp(g, id, C_NAME); }


//bool g_add_name(shared_ptr<gamestate> g, entityid id, string name) {
// Ensure entity exists in component table
// Automatically register component if not already registered
//if (!g_add_comp(g, id, C_NAME)) {
//    merror("g_add_component failed for id %d", id);
//    return false;
//}
//if (!g->name_list) {
//    merror("g->name_list is NULL");
//    return false;
//}
//(*g->name_list)[id] = name; // Insert or update the name
//return true;
//}


//string g_get_name(shared_ptr<gamestate> g, entityid id) {
//    if (!g_has_name(g, id)) {
//        merror("g_get_name: id %d does not have a name component", id);
//        return "no-name-comp";
//    }
//    if (g->name_list) {
//        // we can assume that the id is in the name_list
//        massert(g->name_list->find(id) != g->name_list->end(), "g_get_name: id %d not found in name_list", id);
//        return g->name_list->at(id);
//    }
//    return "no-name"; // Return an empty string if the id is not found
//}


//bool g_add_type(shared_ptr<gamestate> g, entityid id, entitytype_t type) {
//    if (!g_add_comp(g, id, C_TYPE)) {
//        merror("g_add_type: Failed to add component C_TYPE for id %d", id);
//        return false;
//    }
//    if (!g->type_list) {
//        merror("g->type_list is NULL");
//        return false;
//    }
//    // Check if the type already exists for the entity
//    (*g->type_list)[id] = type; // Insert or update the type
//    //msuccess("Entity ID %d type set to %d", id, type);
//    return true;
//}


//entitytype_t g_get_type(shared_ptr<gamestate> g, entityid id) {
//    //if (!g_has_comp(g, id, C_TYPE)) {
//    if (!g_has_type(g, id)) {
//        merror("g_get_type: id %d does not have a type component", id);
//        return ENTITY_NONE; // Return ENTITY_NONE if the type component is not present
//    }
//    if (g->type_list) {
//        massert(g->type_list->find(id) != g->type_list->end(), "g_get_type: id %d not found in type_list", id);
//        return g->type_list->at(id);
//    }
//    return ENTITY_NONE; // Return ENTITY_NONE if the id is not found
//}


//bool g_has_type(shared_ptr<gamestate> g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_comp(g, id, C_TYPE);
//}


//bool g_has_race(shared_ptr<gamestate> g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    return g_has_comp(g, id, C_RACE);
//}


//race_t g_get_race(shared_ptr<gamestate> g, entityid id) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (g_has_race(g, id)) {
//        if (g->race_list) {
//            massert(g->race_list->find(id) != g->race_list->end(), "g_get_race: id %d not found in race list", id);
//            return g->race_list->at(id);
//        }
//    }
//    merror("race component not found for id %d", id);
//    return RACE_NONE;
//}
//
//
//bool g_add_race(shared_ptr<gamestate> g, entityid id, race_t race) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    // Automatically register component if not already registered
//    if (!g_add_comp(g, id, C_RACE)) {
//        merror("g_add_comp failed for id %d", id);
//        return false;
//    }
//    if (!g->race_list) {
//        merror("g->race_list is NULL");
//        return false;
//    }
//    // Check if the type already exists for the entity
//    (*g->race_list)[id] = race; // Insert or update the type
//    return true;
//}


//bool g_update_race(shared_ptr<gamestate> g, entityid id, race_t race) {
//    massert(g, "g is NULL");
//    massert(id != ENTITYID_INVALID, "id is invalid");
//    if (!g_has_race(g, id)) {
//        merror("g_update_race: id %d does not have a race component", id);
//        return false;
//    }
//    if (!g->race_list) {
//        merror("g->race_list is NULL");
//        return false;
//    }
//    // Update
//    (*g->race_list)[id] = race; // Insert or update the type
//    return true;
//}


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
    //merror("Direction component not found for id %d", id);
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
        //massert(g->attacking_list->find(id) != g->attacking_list->end(), "g_get_attacking: id %d not found in attacking list", id);
        if (g->attacking_list->find(id) == g->attacking_list->end()) {
            //merror("g_get_attacking: id %d not found in attacking list", id);
            return false; // Return false if the id is not found
        }
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
        //merror("g_get_tx_alpha: id %d does not have a tx alpha component", id);
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
        msuccess("Entity id %d alpha set to %d", id, alpha);
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


bool g_add_pushable(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_PUSHABLE)) {
        merror("g_add_pushable: Failed to add component C_PUSHABLE for id %d", id);
        return false;
    }
    // verify that the entity has a pushable component
    return g_has_comp(g, id, C_PUSHABLE);
}


bool g_is_pushable(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_comp(g, id, C_PUSHABLE);
}


bool g_add_item_type(shared_ptr<gamestate> g, entityid id, itemtype type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_ITEMTYPE)) {
        merror("g_add_item_type: Failed to add component C_ITEM_TYPE for id %d", id);
        return false;
    }
    if (!g->item_type_list) {
        merror("g->item_type_list is NULL");
        return false;
    }
    // Check if the item type already exists for the entity
    (*g->item_type_list)[id] = type; // Insert or update the item type

    return true;
}


bool g_has_item_type(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->item_type_list) {
        merror("g->item_type_list is NULL");
        return false;
    }
    // Check if the entity has an item type component
    return g->item_type_list->find(id) != g->item_type_list->end();
}


itemtype g_get_item_type(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->item_type_list) {
        if (g_has_item_type(g, id)) {
            massert(g->item_type_list->find(id) != g->item_type_list->end(), "g_get_item_type: id %d not found in item type list", id);
            return g->item_type_list->at(id);
        }
    }
    merror("g_get_item_type: id %d does not have an item type component", id);
    return ITEM_NONE; // Return ITEMTYPE_NONE if not found
}


bool g_set_item_type(shared_ptr<gamestate> g, entityid id, itemtype type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->item_type_list) {
        merror("g->item_type_list is NULL");
        return false;
    }
    // Check if the entity has an item type component
    if (g_has_item_type(g, id)) {
        // Update the item type for the entity
        (*g->item_type_list)[id] = type; // Update the item type
        return true;
    }
    merror("g_set_item_type: id %d does not have an item type component", id);
    return false;
}


bool g_add_potion_type(shared_ptr<gamestate> g, entityid id, potiontype type) {
    //TODO
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_POTIONTYPE)) {
        merror("g_add_potion_type: Failed to add component C_POTION_TYPE for id %d", id);
        return false;
    }
    if (!g->potion_type_list) {
        merror("g->potion_type_list is NULL");
        return false;
    }
    // Check if the potion type already exists for the entity
    (*g->potion_type_list)[id] = type; // Insert or update the potion type
    return true;
}


bool g_has_potion_type(shared_ptr<gamestate> g, entityid id) {
    //TODO
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->potion_type_list) {
        merror("g->potion_type_list is NULL");
        return false;
    }
    // Check if the entity has a potion type component
    return g->potion_type_list->find(id) != g->potion_type_list->end();
}


potiontype g_get_potion_type(shared_ptr<gamestate> g, entityid id) {
    //TODO
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->potion_type_list) {
        if (g_has_potion_type(g, id)) {
            massert(g->potion_type_list->find(id) != g->potion_type_list->end(), "g_get_potion_type: id %d not found in potion type list", id);
            return g->potion_type_list->at(id);
        }
    }
    merror("g_get_potion_type: id %d does not have a potion type component", id);
    return POTION_NONE; // Return POTION_NONE if not found
}


bool g_set_potion_type(shared_ptr<gamestate> g, entityid id, potiontype type) {
    //TODO
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->potion_type_list) {
        merror("g->potion_type_list is NULL");
        return false;
    }
    // Check if the entity has a potion type component
    if (g_has_potion_type(g, id)) {
        // Update the potion type for the entity
        (*g->potion_type_list)[id] = type; // Update the potion type
        return true;
    }
    merror("g_set_potion_type: id %d does not have a potion type component", id);
    return false;
}


bool g_add_weapon_type(std::shared_ptr<gamestate> g, entityid id, weapontype type) {
    //TODO
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Automatically register component if not already registered
    if (!g_add_comp(g, id, C_WEAPONTYPE)) {
        merror("g_add_weapon_type: Failed to add component C_WEAPON_TYPE for id %d", id);
        return false;
    }
    if (!g->weapon_type_list) {
        merror("g->weapon_type_list is NULL");
        return false;
    }
    (*g->weapon_type_list)[id] = type; // Insert or update the weapon type
    return true;
}


bool g_has_weapon_type(std::shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->weapon_type_list) {
        merror("g->weapon_type_list is NULL");
        return false;
    }
    return g->weapon_type_list->find(id) != g->weapon_type_list->end();
}


weapontype g_get_weapon_type(std::shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->weapon_type_list) {
        if (g_has_weapon_type(g, id)) {
            massert(g->weapon_type_list->find(id) != g->weapon_type_list->end(), "g_get_weapon_type: id %d not found in weapon type list", id);
            return g->weapon_type_list->at(id);
        }
    }
    return WEAPON_NONE; // Return WEAPON_NONE if not found
}


bool g_set_weapon_type(std::shared_ptr<gamestate> g, entityid id, weapontype type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->weapon_type_list) {
        merror("g->weapon_type_list is NULL");
        return false;
    }
    if (g_has_weapon_type(g, id)) {
        (*g->weapon_type_list)[id] = type; // Update the weapon type
        return true;
    }
    return false;
}


bool g_add_stats(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g_add_comp(g, id, C_STATS)) {
        merror("g_add_stats: Failed to add component C_STATS for id %d", id);
        return false;
    }
    if (!g->stats_list) {
        merror("g->stats_list is NULL");
        return false;
    }

    if (g->stats_list->find(id) != g->stats_list->end()) {
        merror("g_add_stats: id %d already has a stats component", id);
        return false;
    }

    (*g->stats_list)[id] = make_shared<unordered_map<int, int>>();

    for (int i = 0; i < STATS_COUNT; i++) {
        (*(*g->stats_list)[id])[i] = 0;
    }

    return true;
}


bool g_has_stats(shared_ptr<gamestate> g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->stats_list) {
        merror("g->stats_list is NULL");
        return false;
    }
    return g->stats_list->find(id) != g->stats_list->end();
}


bool g_set_stat(shared_ptr<gamestate> g, entityid id, stats_slot slot, int value) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->stats_list) {
        merror("g->stats_list is NULL");
        return false;
    }
    if (g_has_stats(g, id)) {
        (*(*g->stats_list)[id])[slot] = value;
        //msuccess("Entity id %d stat slot %d set to %d", id, slot, value);
        return true;
    }
    merror("g_set_stat: id %d does not have a stats component", id);
    return false;
}


int g_get_stat(shared_ptr<gamestate> g, entityid id, stats_slot slot) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g->stats_list) {
        merror("g->stats_list is NULL");
        return 0;
    }
    // Check if the entity has a stats component
    if (g_has_stats(g, id)) {
        massert(g->stats_list->find(id) != g->stats_list->end(), "g_get_stat: id %d not found in stats list", id);
        return (*(*g->stats_list)[id])[slot]; // Return the stat value
    }
    merror("g_get_stat: id %d does not have a stats component", id);
    return 0;
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
