#include "component.h"
#include "controlmode.h"
#include "entityid.h"
#include "gamestate.h"
#include "gamestate_flag.h"
#include "inventory_sort.h"
#include "item.h"
#include "massert.h"
#include "mprint.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GAMESTATE_DEBUGPANEL_DEFAULT_X 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_Y 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE 20
#define GAMESTATE_INIT_ENTITYIDS_MAX 1000000
// Temporary global for qsort comparisons
static gamestate* g_sort_context = NULL;

static void gamestate_init_music_paths(gamestate* const g);
static void gamestate_load_monster_defs(gamestate* const g);

// have to update this function when we introduce new fields to Gamestate
gamestate* gamestateinitptr() {
    const size_t n = LIST_INIT_CAPACITY;
    gamestate* g = (gamestate*)malloc(sizeof(gamestate));
    massert(g, "g is NULL");
    g->version = GAME_VERSION;
    g->cam_lockon = g->frame_dirty = true;
    g->debugpanel.x = GAMESTATE_DEBUGPANEL_DEFAULT_X, g->debugpanel.y = GAMESTATE_DEBUGPANEL_DEFAULT_Y;
    g->debugpanel.w = g->debugpanel.h = 200;
    g->debugpanel.fg_color = RAYWHITE, g->debugpanel.bg_color = RED;
    g->debugpanel.font_size = GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE;
    g->targetwidth = g->targetheight = g->windowwidth = g->windowheight = g->hero_id = g->entity_turn = g->new_entityid_begin = g->new_entityid_end = -1;
    g->timebegan = g->currenttime = time(NULL);
    g->timebegantm = localtime(&(g->timebegan)), g->currenttimetm = localtime(&(g->currenttime));
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
    g->ringtype_list_count = 0;
    g->light_radius_bonus_list_count = 0;
    g->sort_inventory_menu_selection = 0;
    g->sort_inventory_menu_selection_max = 2;
    g->hero_inventory_sorted_by_name = NULL;
    g->hero_inventory_sorted_by_type = NULL;
    g->gameplay_settings_menu_selection = 0;
    g->cam2d.target = (Vector2){0, 0};
    g->cam2d.offset = (Vector2){0, 0};
    g->cam2d.zoom = 1.0f;
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
    g->components = ct_create();
    g->next_entityid = 0;
    g->current_music_index = 0;
    g->total_music_paths = 0;
    g->restart_count = 0;
    g->do_restart = 0;
    g->title_screen_selection = 0;
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
    g->name_list_count = 0;
    g->type_list_count = 0;
    g->race_list_count = 0;
    g->direction_list_count = 0;
    g->loc_list_count = 0;
    g->sprite_move_list_count = 0;
    g->dead_list_count = 0;
    g->update_list_count = 0;
    g->attacking_list_count = 0;
    g->blocking_list_count = 0;
    g->block_success_list_count = 0;
    g->damaged_list_count = 0;
    g->inventory_list_count = 0;
    g->target_list_count = 0;
    g->target_path_list_count = 0;
    g->default_action_list_count = 0;
    g->equipment_list_count = 0;
    g->stats_list_count = 0;
    g->itemtype_list_count = 0;
    g->weapontype_list_count = 0;
    g->shieldtype_list_count = 0;
    g->potion_type_list_count = 0;
    g->damage_list_count = 0;
    g->ac_list_count = 0;
    g->zapping_list_count = 0;
    g->base_attack_damage_list_count = 0;
    g->vision_distance_list_count = 0;
    g->light_radius_list_count = 0;
    g->name_list_capacity = n;
    g->type_list_capacity = n;
    g->race_list_capacity = n;
    g->direction_list_capacity = n;
    g->loc_list_capacity = n;
    g->sprite_move_list_capacity = n;
    g->dead_list_capacity = n;
    g->update_list_capacity = n;
    g->attacking_list_capacity = n;
    g->blocking_list_capacity = n;
    g->block_success_list_capacity = n;
    g->damaged_list_capacity = n;
    g->inventory_list_capacity = n;
    g->target_list_capacity = n;
    g->target_path_list_capacity = n;
    g->default_action_list_capacity = n;
    g->equipment_list_capacity = n;
    g->stats_list_capacity = n;
    g->itemtype_list_capacity = n;
    g->weapontype_list_capacity = n;
    g->shieldtype_list_capacity = n;
    g->potion_type_list_capacity = n;
    g->damage_list_capacity = n;
    g->ac_list_capacity = n;
    g->zapping_list_capacity = n;
    g->base_attack_damage_list_capacity = n;
    g->vision_distance_list_capacity = n;
    g->light_radius_list_capacity = n;
    g->ringtype_list_capacity = n;
    g->light_radius_bonus_list_capacity = n;
    g->name_list = (name_component*)malloc(sizeof(name_component) * n);
    g->type_list = (int_component*)malloc(sizeof(int_component) * n);
    g->race_list = (int_component*)malloc(sizeof(int_component) * n);
    g->direction_list = (int_component*)malloc(sizeof(int_component) * n);
    g->loc_list = (vec3_component*)malloc(sizeof(vec3_component) * n);
    g->sprite_move_list = (rect_component*)malloc(sizeof(rect_component) * n);
    g->target_list = (vec3_component*)malloc(sizeof(vec3_component) * n);
    g->dead_list = (int_component*)malloc(sizeof(int_component) * n);
    g->update_list = (int_component*)malloc(sizeof(int_component) * n);
    g->attacking_list = (int_component*)malloc(sizeof(int_component) * n);
    g->blocking_list = (int_component*)malloc(sizeof(int_component) * n);
    g->block_success_list = (int_component*)malloc(sizeof(int_component) * n);
    g->damaged_list = (int_component*)malloc(sizeof(int_component) * n);
    g->inventory_list = (inventory_component*)malloc(sizeof(inventory_component) * n);
    g->target_path_list = (target_path_component*)malloc(sizeof(target_path_component) * n);
    g->default_action_list = (int_component*)malloc(sizeof(int_component) * n);
    g->equipment_list = (equipment_component*)malloc(sizeof(equipment_component) * n);
    g->stats_list = (stats_component*)malloc(sizeof(stats_component) * n);
    g->itemtype_list = (int_component*)malloc(sizeof(int_component) * n);
    g->weapontype_list = (int_component*)malloc(sizeof(int_component) * n);
    g->shieldtype_list = (int_component*)malloc(sizeof(int_component) * n);
    g->potion_type_list = (int_component*)malloc(sizeof(int_component) * n);
    g->ac_list = (int_component*)malloc(sizeof(int_component) * n);
    g->zapping_list = (int_component*)malloc(sizeof(int_component) * n);
    g->damage_list = (vec3_component*)malloc(sizeof(vec3_component) * n);
    g->base_attack_damage_list = (vec3_component*)malloc(sizeof(vec3_component) * n);
    g->vision_distance_list = (int_component*)malloc(sizeof(int_component) * n);
    g->light_radius_list = (int_component*)malloc(sizeof(int_component) * n);
    g->ringtype_list = (int_component*)malloc(sizeof(int_component) * n);
    g->light_radius_bonus_list = (int_component*)malloc(sizeof(int_component) * n);

    massert(g->name_list, "g->name_list is NULL");
    massert(g->type_list, "g->type_list is NULL");
    massert(g->race_list, "g->race_list is NULL");
    massert(g->direction_list, "g->direction_list is NULL");
    massert(g->loc_list, "g->loc_list is NULL");
    massert(g->sprite_move_list, "g->sprite_move_list is NULL");
    massert(g->target_list, "g->target_list is NULL");
    massert(g->dead_list, "g->dead_list is NULL");
    massert(g->update_list, "g->update_list is NULL");
    massert(g->attacking_list, "g->attacking_list is NULL");
    massert(g->blocking_list, "g->blocking_list is NULL");
    massert(g->block_success_list, "g->block_success_list is NULL");
    massert(g->damaged_list, "g->damaged_list is NULL");
    massert(g->inventory_list, "g->inventory_list is NULL");
    massert(g->target_path_list, "g->target_path_list is NULL");
    massert(g->default_action_list, "g->default_action_list is NULL");
    massert(g->equipment_list, "g->equipment_list is NULL");
    massert(g->stats_list, "g->stats_list is NULL");
    massert(g->itemtype_list, "g->itemtype_list is NULL");
    massert(g->weapontype_list, "g->weapontype_list is NULL");
    massert(g->shieldtype_list, "g->shieldtype_list is NULL");
    massert(g->potion_type_list, "g->potion_list is NULL");
    massert(g->ac_list, "g->ac_list is NULL");
    massert(g->zapping_list, "g->zapping_list is NULL");
    massert(g->damage_list, "g->damage_list is NULL");
    massert(g->base_attack_damage_list, "g->base_attack_damage_list is NULL");
    massert(g->vision_distance_list, "g->vision_distance_list is NULL");
    massert(g->light_radius_list, "g->light_radius_list is NULL");
    massert(g->light_radius_bonus_list, "g->light_radius_bonus_list is NULL");
    massert(g->ringtype_list, "g->ringtype_list is NULL");

    g->d = NULL;
    g->monster_defs = NULL;

    gamestate_init_music_paths(g);
    gamestate_init_msg_history(g);
    g->max_title_screen_selections = 3;
    g->chara_creation = (character_creation){0};
    g->current_scene = SCENE_TITLE;
    gamestate_load_monster_defs(g);

    g->music_volume = DEFAULT_MUSIC_VOLUME;

    return g;
}

static void gamestate_load_monster_defs(gamestate* const g) {
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

static void gamestate_init_music_paths(gamestate* const g) {
    massert(g, "g is NULL");
    const char* music_path_file = "music.txt";
    FILE* file = fopen(music_path_file, "r");
    massert(file, "Could not open music path file: %s", music_path_file);
    int i = 0;
    char buffer[1024];
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

bool gamestate_init_msg_history(gamestate* const g) {
    massert(g, "g is NULL");
    g->msg_history.count = 0, g->msg_history.max_count = DEFAULT_MAX_HISTORY_SIZE;
    g->msg_history.messages = (char**)malloc(sizeof(char*) * g->msg_history.max_count);
    if (g->msg_history.messages == NULL) {
        merror("g->msg_history.messages is NULL");
        free(g);
        return false;
    }
    for (int i = 0; i < g->msg_history.max_count; i++) {
        g->msg_history.messages[i] = (char*)malloc(sizeof(char) * MAX_MSG_LENGTH);
        if (g->msg_history.messages[i] == NULL) {
            //merror("g->msg_history.messages[%d] is NULL", i);
            for (int j = 0; j < i; j++) free(g->msg_history.messages[j]);
            free(g->msg_history.messages);
            return false;
        }
    }
    return true;
}

bool gamestate_free_msg_history(gamestate* const g) {
    massert(g, "g is NULL");
    for (int i = 0; i < g->msg_history.max_count; i++) {
        if (g->msg_history.messages[i]) {
            free(g->msg_history.messages[i]);
            g->msg_history.messages[i] = NULL;
        }
    }
    free(g->msg_history.messages);
    g->msg_history.messages = NULL;
    return true;
}

void gamestatefree(gamestate* g) {
    massert(g, "g is NULL");
    // free message history
    gamestate_free_msg_history(g);
    ct_destroy(g->components);
    free(g->name_list);
    free(g->type_list);
    free(g->race_list);
    free(g->direction_list);
    free(g->loc_list);
    free(g->sprite_move_list);
    free(g->dead_list);
    free(g->update_list);
    free(g->attacking_list);
    free(g->blocking_list);
    free(g->block_success_list);
    free(g->damaged_list);
    free(g->inventory_list);
    free(g->target_list);
    free(g->target_path_list);
    free(g->default_action_list);
    free(g->equipment_list);
    free(g->stats_list);
    free(g->itemtype_list);
    free(g->weapontype_list);
    free(g->shieldtype_list);
    free(g->potion_type_list);
    free(g->damage_list);
    free(g->ac_list);
    free(g->zapping_list);
    free(g->base_attack_damage_list);
    free(g->vision_distance_list);
    free(g->light_radius_list);
    free(g->ringtype_list);
    free(g->light_radius_bonus_list);
    if (g->monster_defs) {
        free(g->monster_defs);
        g->monster_defs = NULL;
    }
    free(g);
}

void gamestate_set_hero_id(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    g->hero_id = id;
}

entityid gamestate_get_hero_id(const gamestate* const g) {
    massert(g, "g is NULL");
    return g->hero_id;
}

void gamestate_load_keybindings(gamestate* const g) {
    if (!g) return;
    const char* filename = "keybindings.ini";
    load_keybindings(filename, &g->keybinding_list);
}

bool gamestate_add_msg_history(gamestate* const g, const char* msg) {
    massert(g, "g is NULL");
    massert(msg, "msg is NULL");
    if (g->msg_history.count >= g->msg_history.max_count) return false;
    strncpy(g->msg_history.messages[g->msg_history.count], msg, MAX_MSG_LENGTH);
    g->msg_history.count++;
    return true;
}

void gamestate_set_debug_panel_pos_bottom_left(gamestate* const g) {
    massert(g, "g is NULL");
    if (g->windowwidth == -1 || g->windowheight == -1) return;
    g->debugpanel.x = 0, g->debugpanel.y = g->windowheight - g->debugpanel.h;
}

void gamestate_set_debug_panel_pos_top_right(gamestate* const g) {
    massert(g, "g is NULL");
    if (g->windowwidth == -1 || g->windowheight == -1) return;
    g->debugpanel.x = g->windowwidth - g->debugpanel.w, g->debugpanel.y = g->debugpanel.pad_right;
}

bool g_register_comp(gamestate* const g, entityid id, component comp) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(comp != C_COUNT, "comp is invalid");
    if (g->components == NULL) return false;
    ct_add_entity(g->components, id); // {
    if (!ct_add_component(g->components, id, comp)) return false;
    return true;
}

bool g_register_comps(gamestate* const g, entityid id, ...) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->components == NULL) return false;
    if (!ct_add_entity(g->components, id)) return false;
    va_list args;
    va_start(args, id);
    component comp;
    //while ((comp = va_arg(args, component)) != C_COUNT) {
    while ((comp = va_arg(args, component)) != 0) {
        if (!ct_add_component(g->components, id, comp)) {
            merror("ct_add_component failed: %d %d", id, comp);
            va_end(args);
            return false;
        }
    }
    va_end(args);
    return true;
}

bool g_add_component(gamestate* const g, entityid id, component comp, void* data, size_t c_size, void** c_list, int* c_count, int* c_capacity) {
    //minfo("g_add_component: id: %d, comp: %s", id, component2str(comp));
    massert(g, "g is NULL");
    if (!data) {
        mwarning("data is NULL");
    }
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(*c_list, "c_list is NULL");
    massert(*c_capacity > 0, "c_capacity is 0");
    massert(*c_count >= 0, "c_count is negative");
    massert(*c_count <= g->next_entityid, "c_count >= g->next_entityid: %d >= %d", *c_count, g->next_entityid);
    // Ensure entity exists in component table
    if (!ct_has_entity(g->components, id) && !ct_add_entity(g->components, id)) return false;
    // Automatically register component if not already registered
    if (!g_has_component(g, id, comp) && !g_register_comp(g, id, comp)) return false;
    if (*c_count >= *c_capacity) {
        *c_capacity *= 2;
        *c_list = realloc(*c_list, c_size * (*c_capacity));
        if (*c_list == NULL) return false;
    }
    // Initialize the component
    void* c_ptr = *c_list + (*c_count * c_size);
    switch (comp) {
    case C_NAME: init_name_component((name_component*)c_ptr, id, (const char*)data); break;
    case C_TYPE: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_RACE: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_DIRECTION: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_ITEMTYPE: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_POTIONTYPE: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_AC: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_WEAPONTYPE: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_SHIELDTYPE: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_DEFAULT_ACTION: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_DEAD: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_UPDATE: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_ATTACKING: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_ZAPPING: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_BLOCKING: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_BLOCK_SUCCESS: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_DAMAGED: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_LOCATION: init_vec3_component((vec3_component*)c_ptr, id, *(vec3*)data); break;
    case C_SPRITE_MOVE: init_vec3_component((vec3_component*)c_ptr, id, *(vec3*)data); break;
    case C_TARGET: init_vec3_component((vec3_component*)c_ptr, id, *(vec3*)data); break;
    case C_INVENTORY: init_inventory_component((inventory_component*)c_ptr, id); break;
    case C_TARGET_PATH: init_target_path_component((target_path_component*)c_ptr, id, NULL, 0);
    case C_EQUIPMENT: init_equipment_component((equipment_component*)c_ptr, id); break;
    case C_STATS: init_stats_component((stats_component*)c_ptr, id); break;
    case C_DAMAGE: init_vec3_component((vec3_component*)c_ptr, id, *(vec3*)data); break;
    case C_BASE_ATTACK_DAMAGE: init_vec3_component((vec3_component*)c_ptr, id, *(vec3*)data); break;
    case C_LIGHT_RADIUS: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_LIGHT_RADIUS_BONUS: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_VISION_DISTANCE: init_int_component((int_component*)c_ptr, id, *(int*)data); break;
    case C_RINGTYPE:
        init_int_component((int_component*)c_ptr, id, *(int*)data);
        break;
        //case C_SPELL_EFFECT: init_spell_effect_component((spell_effect_component*)c_ptr, id, *(spell_effect*)data); break;
    default: merror("Unsupported component type: %s", component2str(comp)); return false;
    }
    (*c_count)++;
    return true;
}

bool g_add_name(gamestate* const g, entityid id, const char* name) {
    massert(g, "g is NULL");
    massert(name, "name is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_add_component(g, id, C_NAME, (void*)name, sizeof(name_component), (void**)&g->name_list, &g->name_list_count, &g->name_list_capacity);
}

bool g_has_component(const gamestate* const g, entityid id, component comp) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(comp != C_COUNT, "comp is invalid");
    if (g->components == NULL) return false;
    return ct_has_component(g->components, id, comp);
}

bool g_has_name(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_NAME);
}

const char* g_get_name(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    //massert(g_has_name(g, id), "id %d does not have a name component, is type %s", id, entitytype_to_string(g_get_type(g, id)));

    // if something doesn't have a name, we return NULL
    if (!g_has_name(g, id)) return NULL;

    for (int i = 0; i < g->name_list_count; i++)
        if (g->name_list[i].id == id) return g->name_list[i].name;
    return NULL;
}

bool g_add_type(gamestate* const g, entityid id, int type) {
    massert(g, "g is NULL");
    massert(type > ENTITY_NONE && type < ENTITY_TYPE_COUNT, "type is invalid");
    return g_add_component(g, id, C_TYPE, (void*)&type, sizeof(int_component), (void**)&g->type_list, &g->type_list_count, &g->type_list_capacity);
}

entitytype_t g_get_type(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    if (id != ENTITYID_INVALID)
        for (int i = 0; i < g->type_list_count; i++)
            if (g->type_list[i].id == id) return g->type_list[i].data;
    return ENTITY_NONE;
}

bool g_set_type(gamestate* const g, entityid id, int type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(type > ENTITY_NONE && type < ENTITY_TYPE_COUNT, "type is invalid");
    massert(g->type_list, "g->type_list is NULL");
    if (g->type_list == NULL) return false;
    for (int i = 0; i < g->type_list_count; i++) {
        if (g->type_list[i].id == id) {
            g->type_list[i].data = type;
            return true;
        }
    }
    return false;
}

bool g_has_type(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_TYPE);
}

bool g_is_type(const gamestate* const g, entityid id, int type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(type > ENTITY_NONE && type < ENTITY_TYPE_COUNT, "type is invalid");
    massert(g->type_list, "g->type_list is NULL");
    if (g->type_list == NULL) return false;
    return type == g_get_type(g, id);
}

bool g_add_race(gamestate* const g, entityid id, int race) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_add_component(g, id, C_RACE, (void*)&race, sizeof(int_component), (void**)&g->race_list, &g->race_list_count, &g->race_list_capacity);
}

race_t g_get_race(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g_has_race(g, id))
        for (int i = 0; i < g->race_list_count; i++)
            if (g->race_list[i].id == id) return g->race_list[i].data;
    return RACE_NONE;
}

bool g_is_race(gamestate* const g, entityid id, int race) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_get_race(g, id) == race;
}

bool g_has_race(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_RACE);
}

direction_t g_get_direction(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (!g_has_component(g, id, C_DIRECTION)) return DIR_NONE;
    for (int i = 0; i < g->direction_list_count; i++)
        if (g->direction_list[i].id == id) return g->direction_list[i].data;
    return DIR_NONE;
}

bool g_has_direction(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_DIRECTION);
}

bool g_add_direction(gamestate* const g, entityid id, int dir) {
    massert(g, "g is NULL");
    massert(dir >= 0 && dir < DIR_COUNT, "dir is invalid");
    // make sure the entity has the component
    return g_add_component(g, id, C_DIRECTION, (void*)&dir, sizeof(int_component), (void**)&g->direction_list, &g->direction_list_count, &g->direction_list_capacity);
}

bool g_is_direction(gamestate* const g, entityid id, int dir) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_get_direction(g, id) == dir;
}

bool g_update_direction(gamestate* const g, entityid id, int dir) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->direction_list == NULL) return false;
    for (int i = 0; i < g->direction_list_count; i++) {
        if (g->direction_list[i].id == id) {
            g->direction_list[i].data = dir;
            return true;
        }
    }
    return false;
}

bool g_has_location(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_LOCATION);
}

//bool g_add_location(gamestate* const g, entityid id, loc_t loc) {
bool g_add_location(gamestate* const g, entityid id, vec3 loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the location component
    return g_add_component(g, id, C_LOCATION, (void*)&loc, sizeof(vec3_component), (void**)&g->loc_list, &g->loc_list_count, &g->loc_list_capacity);
}

bool g_update_location(gamestate* const g, entityid id, vec3 loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->loc_list == NULL) return false;
    for (int i = 0; i < g->loc_list_count; i++) {
        if (g->loc_list[i].id == id) {
            g->loc_list[i].data = loc;
            return true;
        }
    }
    return false;
}

vec3 g_get_location(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->loc_list == NULL) return (vec3){-1, -1, -1};
    for (int i = 0; i < g->loc_list_count; i++)
        if (g->loc_list[i].id == id) return g->loc_list[i].data;
    return (vec3){-1, -1, -1};
}

bool g_is_location(const gamestate* const g, entityid id, vec3 loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->loc_list == NULL) return false;
    vec3 loc2 = g_get_location(g, id);
    return (loc.x == loc2.x && loc.y == loc2.y && loc.z == loc2.z);
}

bool g_has_sprite_move(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_SPRITE_MOVE);
}

bool g_add_sprite_move(gamestate* const g, entityid id, Rectangle loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the sprite move component
    return g_add_component(g, id, C_SPRITE_MOVE, (void*)&loc, sizeof(rect_component), (void**)&g->sprite_move_list, &g->sprite_move_list_count, &g->sprite_move_list_capacity);
}

bool g_update_sprite_move(gamestate* const g, entityid id, Rectangle loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g_has_component(g, id, C_SPRITE_MOVE), "id %d does not have a sprite move component", id);
    if (g_has_sprite_move(g, id)) {
        if (g->sprite_move_list == NULL) return false;
        for (int i = 0; i < g->sprite_move_list_count; i++) {
            if (g->sprite_move_list[i].id == id) {
                g->sprite_move_list[i].data = loc;
                return true;
            }
        }
    }
    return false;
}

Rectangle g_get_sprite_move(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g_has_sprite_move(g, id), "id %d does not have a sprite move component", id);
    if (g_has_sprite_move(g, id)) {
        for (int i = 0; i < g->sprite_move_list_count; i++)
            if (g->sprite_move_list[i].id == id) return g->sprite_move_list[i].data;
    }
    //merror("id %d not found in sprite_move_list", id);
    return (Rectangle){0, 0, 0, 0}; // Return an empty rectangle if not found
}

bool g_has_dead(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_DEAD);
}

bool g_add_dead(gamestate* const g, entityid id, int dead) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // Add dead component
    bool result = g_add_component(g, id, C_DEAD, (void*)&dead, sizeof(int_component), (void**)&g->dead_list, &g->dead_list_count, &g->dead_list_capacity);
    // If entity died and has a location, mark its tile as dirty
    if (dead && g_has_location(g, id)) {
        vec3 loc = g_get_location(g, id);
        dungeon_floor_t* df = d_get_floor(g->d, loc.z);
        if (df) {
            vec3 loc_cast = {loc.x, loc.y, loc.z};
            tile_t* tile = df_tile_at(df, loc_cast);
            if (tile) tile->dirty_entities = true;
        }
    }
    return result;
}

bool g_update_dead(gamestate* const g, entityid id, int dead) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g_has_component(g, id, C_DEAD), "id %d does not have a dead component", id);
    if (g->dead_list == NULL) return false;
    for (int i = 0; i < g->dead_list_count; i++) {
        if (g->dead_list[i].id == id) {
            g->dead_list[i].data = dead;
            return true;
        }
    }
    return false;
}

bool g_is_dead(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    if (id != ENTITYID_INVALID) {
        if (g->dead_list == NULL) return false;
        for (int i = 0; i < g->dead_list_count; i++)
            if (g->dead_list[i].id == id) return g->dead_list[i].data;
    }
    return false;
}

bool g_has_update(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_UPDATE);
}

bool g_add_update(gamestate* const g, entityid id, int update) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the update component
    return g_add_component(g, id, C_UPDATE, (void*)&update, sizeof(update_component), (void**)&g->update_list, &g->update_list_count, &g->update_list_capacity);
}

bool g_get_update(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->update_list == NULL) return false;
    for (int i = 0; i < g->update_list_count; i++)
        if (g->update_list[i].id == id) return g->update_list[i].data;
    merror("id %d not found in update_list", id);
    return false;
}

bool g_set_update(gamestate* const g, entityid id, int update) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->update_list == NULL) return false;
    for (int i = 0; i < g->update_list_count; i++) {
        if (g->update_list[i].id == id) {
            g->update_list[i].data = update;
            return true;
        }
    }
    return false;
}

bool g_has_attacking(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_ATTACKING);
}

bool g_add_attacking(gamestate* const g, entityid id, int attacking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the attacking component
    return g_add_component(g, id, C_ATTACKING, (void*)&attacking, sizeof(attacking_component), (void**)&g->attacking_list, &g->attacking_list_count, &g->attacking_list_capacity);
}

bool g_set_attacking(gamestate* const g, entityid id, int attacking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->attacking_list == NULL) return false;
    for (int i = 0; i < g->attacking_list_count; i++) {
        if (g->attacking_list[i].id == id) {
            g->attacking_list[i].data = attacking;
            return true;
        }
    }
    return false;
}

bool g_get_attacking(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->attacking_list == NULL) return false;
    for (int i = 0; i < g->attacking_list_count; i++)
        if (g->attacking_list[i].id == id) return g->attacking_list[i].data;
    //merror("id %d not found in attacking_list", id);
    return false;
}

bool g_has_blocking(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_BLOCKING);
}

bool g_add_blocking(gamestate* const g, entityid id, int blocking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the blocking component
    massert(g_has_component(g, id, C_BLOCKING), "id %d does not have a blocking component", id);
    return g_add_component(g, id, C_BLOCKING, (void*)&blocking, sizeof(blocking_component), (void**)&g->blocking_list, &g->blocking_list_count, &g->blocking_list_capacity);
}

bool g_set_blocking(gamestate* const g, entityid id, int blocking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->blocking_list == NULL) return false;
    for (int i = 0; i < g->blocking_list_count; i++) {
        if (g->blocking_list[i].id == id) {
            g->blocking_list[i].data = blocking;
            return true;
        }
    }
    return false;
}

bool g_get_blocking(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->blocking_list == NULL) return false;
    for (int i = 0; i < g->blocking_list_count; i++)
        if (g->blocking_list[i].id == id) return g->blocking_list[i].data;
    //merror("id %d not found in blocking_list", id);
    return false;
}

bool g_has_block_success(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_BLOCK_SUCCESS);
}

bool g_add_block_success(gamestate* const g, entityid id, int block_success) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the block success component
    return g_add_component(g,
                           id,
                           C_BLOCK_SUCCESS,
                           (void*)&block_success,
                           sizeof(block_success_component),
                           (void**)&g->block_success_list,
                           &g->block_success_list_count,
                           &g->block_success_list_capacity);
}

bool g_set_block_success(gamestate* const g, entityid id, int block_success) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->block_success_list == NULL) return false;
    for (int i = 0; i < g->block_success_list_count; i++) {
        if (g->block_success_list[i].id == id) {
            g->block_success_list[i].data = block_success;
            return true;
        }
    }
    return false;
}

bool g_get_block_success(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->block_success_list == NULL) return false;
    for (int i = 0; i < g->block_success_list_count; i++)
        if (g->block_success_list[i].id == id) return g->block_success_list[i].data;
    return false;
}

bool g_has_damaged(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_DAMAGED);
}

bool g_add_damaged(gamestate* const g, entityid id, int damaged) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the damaged component
    return g_add_component(g, id, C_DAMAGED, (void*)&damaged, sizeof(damaged_component), (void**)&g->damaged_list, &g->damaged_list_count, &g->damaged_list_capacity);
}

bool g_set_damaged(gamestate* const g, entityid id, int damaged) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->damaged_list == NULL) return false;
    for (int i = 0; i < g->damaged_list_count; i++) {
        if (g->damaged_list[i].id == id) {
            g->damaged_list[i].data = damaged;
            return true;
        }
    }
    return false;
}

bool g_get_damaged(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->damaged_list == NULL) return false;
    for (int i = 0; i < g->damaged_list_count; i++)
        if (g->damaged_list[i].id == id) return g->damaged_list[i].data;
    return false;
}

bool g_has_default_action(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_DEFAULT_ACTION);
}

bool g_add_default_action(gamestate* const g, entityid id, int action) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the default action component
    return g_add_component(
        g, id, C_DEFAULT_ACTION, (void*)&action, sizeof(int_component), (void**)&g->default_action_list, &g->default_action_list_count, &g->default_action_list_capacity);
}

bool g_set_default_action(gamestate* const g, entityid id, int action) {
    massert(g, "g is NULL");
    if (id == ENTITYID_INVALID) return false;
    if (g->default_action_list == NULL) return false;
    for (int i = 0; i < g->default_action_list_count; i++) {
        if (g->default_action_list[i].id == id) {
            g->default_action_list[i].data = action;
            return true;
        }
    }
    return false;
}

entity_action_t g_get_default_action(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->default_action_list == NULL) return ENTITY_ACTION_NONE;
    for (int i = 0; i < g->default_action_list_count; i++)
        if (g->default_action_list[i].id == id) return g->default_action_list[i].data;
    return ENTITY_ACTION_NONE;
}

bool g_has_inventory(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_INVENTORY);
}

bool g_add_inventory(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the inventory component
    return g_add_component(g, id, C_INVENTORY, (void*)NULL, sizeof(inventory_component), (void**)&g->inventory_list, &g->inventory_list_count, &g->inventory_list_capacity);
    return true;
}

// Comparison function for sorting by name
static int compare_by_name(const void* a, const void* b) {
    entityid id_a = *(entityid*)a;
    entityid id_b = *(entityid*)b;
    const char* name_a = g_get_name(g_sort_context, id_a);
    const char* name_b = g_get_name(g_sort_context, id_b);
    return strcmp(name_a, name_b);
}
// Comparison function for sorting by type
static int compare_by_type(const void* a, const void* b) {
    entityid id_a = *(entityid*)a;
    entityid id_b = *(entityid*)b;
    itemtype type_a = g_get_itemtype(g_sort_context, id_a);
    itemtype type_b = g_get_itemtype(g_sort_context, id_b);
    return type_a - type_b;
}

//entityid* g_sort_inventory(gamestate* const g, entityid* inventory, int inv_count, inventory_sort sort_type) {
entityid* g_sort_inventory(gamestate* const g, entityid* inventory, size_t inv_count, inventory_sort sort_type) {
    massert(g, "gamestate is NULL");
    massert(inventory, "inventory is NULL");
    massert(inv_count >= 0, "invalid inventory count");

    minfo("Sorting inventory...");
    //minfo("Item count: %d", inv_count);

    // Create a copy of the inventory array
    entityid* sorted_inv = malloc(inv_count * sizeof(entityid));
    massert(sorted_inv, "failed to allocate memory for sorted inventory");
    memcpy(sorted_inv, inventory, inv_count * sizeof(entityid));

    if (inv_count == 0 || inv_count == 1) {
        minfo("No sorting needed, returning original inventory");
        return sorted_inv; // No need to sort if there's 0 or 1 item
    }

    // Sort based on the specified type
    g_sort_context = g; // Set global context
    switch (sort_type) {
    case INV_SORT_NAME: qsort(sorted_inv, inv_count, sizeof(entityid), compare_by_name); break;
    case INV_SORT_TYPE: qsort(sorted_inv, inv_count, sizeof(entityid), compare_by_type); break;
    default:
        // No sorting needed, return copy as-is
        break;
    }
    g_sort_context = NULL; // Clear global context
    return sorted_inv;
}

bool g_add_to_inventory(gamestate* const g, entityid id, entityid itemid) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
    massert(g->inventory_list, "g->inventory_list is NULL");
    if (g->inventory_list == NULL) return false;
    for (int i = 0; i < g->inventory_list_count; i++) {
        if (g->inventory_list[i].id == id) {
            if (g->inventory_list[i].count >= MAX_INVENTORY_SIZE) return false;
            g->inventory_list[i].inventory[g->inventory_list[i].count++] = itemid;
            msuccess("added item %d to inventory %d", itemid, id);
            return true;
        }
    }
    return false;
}

bool g_remove_from_inventory(gamestate* const g, entityid id, entityid itemid) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
    massert(g->inventory_list, "g->inventory_list is NULL");
    if (g->inventory_list == NULL) return false;
    for (int i = 0; i < g->inventory_list_count; i++) {
        if (g->inventory_list[i].id == id) {
            for (int j = 0; j < g->inventory_list[i].count; j++) {
                if (g->inventory_list[i].inventory[j] == itemid) {
                    g->inventory_list[i].inventory[j] = g->inventory_list[i].inventory[g->inventory_list[i].count - 1];
                    g->inventory_list[i].count--;
                    msuccess("removed item %d from inventory %d", itemid, id);
                    return true;
                }
            }
        }
    }
    return false;
}

//entityid* g_get_inventory(const gamestate* const g, entityid id, int* count) {
entityid* g_get_inventory(const gamestate* const g, entityid id, size_t* count) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(count, "count is NULL");
    massert(g->inventory_list, "g->inventory_list is NULL");
    if (g->inventory_list == NULL) return NULL;
    for (int i = 0; i < g->inventory_list_count; i++) {
        if (g->inventory_list[i].id == id) {
            *count = g->inventory_list[i].count;
            return g->inventory_list[i].inventory;
        }
    }
    *count = 0;
    return NULL;
}

size_t g_get_inventory_count(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g->inventory_list, "g->inventory_list is NULL");
    if (g->inventory_list == NULL) return 0;
    for (int i = 0; i < g->inventory_list_count; i++)
        if (g->inventory_list[i].id == id) return g->inventory_list[i].count;
    return 0;
}

bool g_has_item_in_inventory(const gamestate* const g, entityid id, entityid itemid) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
    massert(g->inventory_list, "g->inventory_list is NULL");
    if (g->inventory_list == NULL) return false;
    for (int i = 0; i < g->inventory_list_count; i++) {
        if (g->inventory_list[i].id == id) {
            for (int j = 0; j < g->inventory_list[i].count; j++)
                if (g->inventory_list[i].inventory[j] == itemid) return true;
        }
    }
    return false; //
}

// updated inventory should have the same num of items as the original
//bool g_update_inventory(gamestate* const g, entityid id, entityid* new_inventory, int new_inventory_count) {
bool g_update_inventory(gamestate* const g, entityid id, entityid* new_inventory, size_t new_inventory_count) {
    // this assumes id has an inventory but lets verify
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(new_inventory, "new_inventory is NULL");
    massert(g->inventory_list, "g->inventory_list is NULL");
    if (g->inventory_list == NULL) return false;
    massert(g_has_inventory(g, id), "id %d does not have an inventory component", id);

    for (int i = 0; i < g->inventory_list_count; i++) {
        if (g->inventory_list[i].id == id) {
            // clear the old inventory
            g->inventory_list[i].count = 0;
            // copy the new inventory
            //for (int j = 0; j < MAX_INVENTORY_SIZE && new_inventory[j] != ENTITYID_INVALID; j++) {
            for (int j = 0; j < new_inventory_count && j < MAX_INVENTORY_SIZE; j++) {
                g->inventory_list[i].inventory[j] = new_inventory[j];
                g->inventory_list[i].count++;
            }
            massert(g->inventory_list[i].count == new_inventory_count, "new inventory count %d does not match expected count %zu", g->inventory_list[i].count, new_inventory_count);
            if (g->inventory_list[i].count == new_inventory_count) {
                msuccess("updated inventory for id %d with %d items", id, new_inventory_count);
                return true;
            }
            merror("updated inventory for id %d but count mismatch: expected %d, got %d", id, new_inventory_count, g->inventory_list[i].count);
            return false;
        }
    }
    merror("id %d not found in inventory_list", id);
    return false;
}

bool g_has_target(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_TARGET);
}

bool g_add_target(gamestate* const g, entityid id, vec3 target) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the target component
    return g_add_component(g, id, C_TARGET, (void*)&target, sizeof(vec3_component), (void**)&g->target_list, &g->target_list_count, &g->target_list_capacity);
}

bool g_set_target(gamestate* const g, entityid id, vec3 target) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->target_list == NULL) return false;
    for (int i = 0; i < g->target_list_count; i++) {
        if (g->target_list[i].id == id) {
            g->target_list[i].data = target;
            return true;
        }
    }
    return false;
}

bool g_get_target(const gamestate* const g, entityid id, vec3* target) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(target, "target is NULL");
    if (g->target_list == NULL) return false;
    for (int i = 0; i < g->target_list_count; i++) {
        if (g->target_list[i].id == id) {
            *target = g->target_list[i].data;
            return true;
        }
    }
    return false;
}

bool g_has_target_path(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_TARGET_PATH);
}

bool g_add_target_path(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the target path component
    return g_add_component(
        g, id, C_TARGET_PATH, (void*)NULL, sizeof(target_path_component), (void**)&g->target_path_list, &g->target_path_list_count, &g->target_path_list_capacity);
    return true;
}

bool g_set_target_path(gamestate* const g, entityid id, vec3* target_path, int target_path_length) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->target_path_list == NULL) return false;
    for (int i = 0; i < g->target_path_list_count; i++) {
        if (g->target_path_list[i].id == id) {
            g->target_path_list[i].target_path = target_path;
            g->target_path_list[i].target_path_length = target_path_length;
            return true;
        }
    }
    return false;
}

bool g_get_target_path(const gamestate* const g, entityid id, vec3** target_path, int* target_path_length) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(target_path, "target_path is NULL");
    massert(target_path_length, "target_path_length is NULL");
    if (g->target_path_list == NULL) return false;
    for (int i = 0; i < g->target_path_list_count; i++) {
        if (g->target_path_list[i].id == id) {
            *target_path = g->target_path_list[i].target_path;
            *target_path_length = g->target_path_list[i].target_path_length;
            return true;
        }
    }
    return false;
}

bool g_get_target_path_length(const gamestate* const g, entityid id, int* target_path_length) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(target_path_length, "target_path_length is NULL");
    if (g->target_path_list == NULL) return false;
    for (int i = 0; i < g->target_path_list_count; i++) {
        if (g->target_path_list[i].id == id) {
            *target_path_length = g->target_path_list[i].target_path_length;
            return true;
        }
    }
    return false;
}

bool g_add_equipment(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the equipment component
    return g_add_component(g, id, C_EQUIPMENT, (void*)NULL, sizeof(equipment_component), (void**)&g->equipment_list, &g->equipment_list_count, &g->equipment_list_capacity);
}

bool g_has_equipment(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_EQUIPMENT);
}

bool g_set_equipment(gamestate* const g, entityid id, equipment_slot slot, entityid itemid) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
    if (g->equipment_list != NULL) {
        for (int i = 0; i < g->equipment_list_count; i++) {
            if (g->equipment_list[i].id == id) {
                g->equipment_list[i].equipment[slot] = itemid;
                return true;
            }
        }
    }
    return false;
}

bool g_unset_equipment(gamestate* const g, entityid id, equipment_slot slot) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->equipment_list != NULL) {
        for (int i = 0; i < g->equipment_list_count; i++) {
            if (g->equipment_list[i].id == id) {
                g->equipment_list[i].equipment[slot] = ENTITYID_INVALID;
                return true;
            }
        }
    }
    return false;
}

entityid g_get_equipment(const gamestate* const g, entityid id, equipment_slot slot) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->equipment_list != NULL)
        for (int i = 0; i < g->equipment_list_count; i++)
            if (g->equipment_list[i].id == id) return g->equipment_list[i].equipment[slot];
    return ENTITYID_INVALID;
}

bool g_is_equipped(const gamestate* const g, entityid id, entityid itemid) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(itemid != ENTITYID_INVALID, "itemid is invalid");
    if (g->equipment_list != NULL) {
        for (int i = 0; i < g->equipment_list_count; i++)
            if (g->equipment_list[i].id == id)
                for (int j = 0; j < EQUIPMENT_SLOT_COUNT; j++)
                    if (g->equipment_list[i].equipment[j] == itemid) return true;
    }
    return false;
}

dungeon_t* g_get_dungeon(gamestate* const g) {
    massert(g, "g is NULL");
    return g->d;
}

bool g_add_stats(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_add_component(g, id, C_STATS, (void*)NULL, sizeof(stats_component), (void**)&g->stats_list, &g->stats_list_count, &g->stats_list_capacity);
}

bool g_has_stats(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_STATS);
}

bool g_set_stat(gamestate* const g, entityid id, stats_slot stats_slot, int value) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g->stats_list, "g->stats_list is NULL");
    if (g->stats_list == NULL) return false;
    for (int i = 0; i < g->stats_list_count; i++) {
        if (g->stats_list[i].id == id) {
            g->stats_list[i].stats[stats_slot] = value;
            return true;
        }
    }
    return false;
}

int* g_get_stats(const gamestate* const g, entityid id, int* count) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(count, "count is NULL");
    massert(g->stats_list, "g->stats_list is NULL");
    if (g->stats_list == NULL) return NULL;
    for (int i = 0; i < g->stats_list_count; i++) {
        if (g->stats_list[i].id == id) {
            *count = STATS_COUNT;
            return g->stats_list[i].stats;
        }
    }
    *count = 0;
    return NULL;
}

int g_get_stat(const gamestate* const g, entityid id, stats_slot stats_slot) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g->stats_list, "g->stats_list is NULL");
    if (g->stats_list == NULL) return 0;
    for (int i = 0; i < g->stats_list_count; i++)
        if (g->stats_list[i].id == id) return g->stats_list[i].stats[stats_slot];
    return 0;
}

bool g_add_itemtype(gamestate* const g, entityid id, int type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_add_component(g, id, C_ITEMTYPE, (void*)&type, sizeof(int_component), (void**)&g->itemtype_list, &g->itemtype_list_count, &g->itemtype_list_capacity);
}

bool g_has_itemtype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_ITEMTYPE);
}

bool g_set_itemtype(gamestate* const g, entityid id, int type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->itemtype_list == NULL) return false;
    for (int i = 0; i < g->itemtype_list_count; i++) {
        if (g->itemtype_list[i].id == id) {
            g->itemtype_list[i].data = type;
            return true;
        }
    }
    return false;
}

itemtype g_get_itemtype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->itemtype_list == NULL) return ITEM_NONE;
    for (int i = 0; i < g->itemtype_list_count; i++)
        if (g->itemtype_list[i].id == id) return g->itemtype_list[i].data;
    return ITEM_NONE;
}

bool g_add_weapontype(gamestate* const g, entityid id, int type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the weapontype component
    return g_add_component(g, id, C_WEAPONTYPE, (void*)&type, sizeof(int_component), (void**)&g->weapontype_list, &g->weapontype_list_count, &g->weapontype_list_capacity);
}

bool g_has_weapontype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_WEAPONTYPE);
}

bool g_set_weapontype(gamestate* const g, entityid id, int type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->weapontype_list == NULL) return false;
    for (int i = 0; i < g->weapontype_list_count; i++) {
        if (g->weapontype_list[i].id == id) {
            g->weapontype_list[i].data = type;
            return true;
        }
    }
    return false;
}

weapontype g_get_weapontype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    if (id == ENTITYID_INVALID) return WEAPON_NONE;
    if (g->weapontype_list == NULL) return WEAPON_NONE;
    for (int i = 0; i < g->weapontype_list_count; i++)
        if (g->weapontype_list[i].id == id) return g->weapontype_list[i].data;
    return WEAPON_NONE;
}

bool g_is_weapontype(const gamestate* const g, entityid id, int type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->weapontype_list == NULL) return false;
    for (int i = 0; i < g->weapontype_list_count; i++)
        if (g->weapontype_list[i].id == id) return g->weapontype_list[i].data == type;
    return false;
}

bool g_add_shieldtype(gamestate* const g, entityid id, int type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the shieldtype component
    return g_add_component(g, id, C_SHIELDTYPE, (void*)&type, sizeof(int_component), (void**)&g->shieldtype_list, &g->shieldtype_list_count, &g->shieldtype_list_capacity);
}

bool g_has_shieldtype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_SHIELDTYPE);
}

bool g_set_shieldtype(gamestate* const g, entityid id, int type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->shieldtype_list == NULL) return false;
    for (int i = 0; i < g->shieldtype_list_count; i++) {
        if (g->shieldtype_list[i].id == id) {
            g->shieldtype_list[i].data = type;
            return true;
        }
    }
    return false;
}

shieldtype g_get_shieldtype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->shieldtype_list == NULL) return SHIELD_NONE;
    for (int i = 0; i < g->shieldtype_list_count; i++)
        if (g->shieldtype_list[i].id == id) return g->shieldtype_list[i].data;
    return SHIELD_NONE;
}

bool g_is_shieldtype(const gamestate* const g, entityid id, int type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->shieldtype_list == NULL) return false;
    for (int i = 0; i < g->shieldtype_list_count; i++)
        if (g->shieldtype_list[i].id == id) return g->shieldtype_list[i].data == type;
    return false;
}

bool g_add_potiontype(gamestate* const g, entityid id, int type) {
    minfo("g_add_potiontype");
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the potiontype component
    return g_add_component(g, id, C_POTIONTYPE, (void*)&type, sizeof(int_component), (void**)&g->potion_type_list, &g->potion_type_list_count, &g->potion_type_list_capacity);
}

bool g_has_potiontype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_POTIONTYPE);
}

bool g_set_potiontype(gamestate* const g, entityid id, int type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->potion_type_list == NULL) return false;
    for (int i = 0; i < g->potion_type_list_count; i++) {
        if (g->potion_type_list[i].id == id) {
            g->potion_type_list[i].data = type;
            return true;
        }
    }
    return false;
}

bool g_is_potion(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->potion_type_list == NULL) return false;
    return g_has_potiontype(g, id);
}

potiontype g_get_potiontype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->potion_type_list == NULL) {
        merror("g->potion_type_list is NULL");
        return POTION_NONE;
    }
    for (int i = 0; i < g->potion_type_list_count; i++) {
        if (g->potion_type_list[i].id == id) {
            return g->potion_type_list[i].data;
        }
    }
    return POTION_NONE;
}

bool g_add_damage(gamestate* const g, entityid id, vec3 r) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_add_component(g, id, C_DAMAGE, (void*)&r, sizeof(vec3_component), (void**)&g->damage_list, &g->damage_list_count, &g->damage_list_capacity);
}

bool g_has_damage(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_DAMAGE);
}

bool g_set_damage(gamestate* const g, entityid id, vec3 r) {
    massert(g, "g is NULL");
    if (id == ENTITYID_INVALID) return false;
    if (g->damage_list == NULL) return false;
    for (int i = 0; i < g->damage_list_count; i++) {
        if (g->damage_list[i].id == id) {
            g->damage_list[i].data = r;
            return true;
        }
    }
    return false;
}

vec3 g_get_damage(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->damage_list == NULL) return (vec3){0, 0, 0};
    for (int i = 0; i < g->damage_list_count; i++)
        if (g->damage_list[i].id == id) return g->damage_list[i].data;
    return (vec3){0, 0, 0};
}

bool g_add_ac(gamestate* const g, entityid id, int ac) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the ac component
    return g_add_component(g, id, C_AC, (void*)&ac, sizeof(int_component), (void**)&g->ac_list, &g->ac_list_count, &g->ac_list_capacity);
}

bool g_has_ac(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_AC);
}

bool g_set_ac(gamestate* const g, entityid id, int ac) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->ac_list == NULL) {
        merror("g->ac_list is NULL");
        return false;
    }
    for (int i = 0; i < g->ac_list_count; i++) {
        if (g->ac_list[i].id == id) {
            g->ac_list[i].data = ac;
            return true;
        }
    }
    return false;
}

int g_get_ac(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->ac_list == NULL) {
        merror("g->ac_list is NULL");
        return 0;
    }
    for (int i = 0; i < g->ac_list_count; i++) {
        if (g->ac_list[i].id == id) {
            return g->ac_list[i].data;
        }
    }
    return 0;
}

entityid g_add_entity(gamestate* const g) {
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

bool g_has_zapping(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_ZAPPING);
}

bool g_add_zapping(gamestate* const g, entityid id, int zapping) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the zapping component
    return g_add_component(g, id, C_ZAPPING, (void*)&zapping, sizeof(zapping_component), (void**)&g->zapping_list, &g->zapping_list_count, &g->zapping_list_capacity);
}

bool g_set_zapping(gamestate* const g, entityid id, int zapping) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->zapping_list == NULL) return false;
    for (int i = 0; i < g->zapping_list_count; i++) {
        if (g->zapping_list[i].id == id) {
            g->zapping_list[i].data = zapping;
            return true;
        }
    }
    return false;
}

bool g_get_zapping(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->zapping_list == NULL) return false;
    for (int i = 0; i < g->zapping_list_count; i++)
        if (g->zapping_list[i].id == id) return g->zapping_list[i].data;
    return false; // not zapping
}

bool g_add_base_attack_damage(gamestate* const g, entityid id, vec3 damage) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the base attack damage component
    return g_add_component(g,
                           id,
                           C_BASE_ATTACK_DAMAGE,
                           (void*)&damage,
                           sizeof(vec3_component),
                           (void**)&g->base_attack_damage_list,
                           &g->base_attack_damage_list_count,
                           &g->base_attack_damage_list_capacity);
}

bool g_has_base_attack_damage(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_BASE_ATTACK_DAMAGE);
}

bool g_set_base_attack_damage(gamestate* const g, entityid id, vec3 damage) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->base_attack_damage_list == NULL) return false;
    for (int i = 0; i < g->base_attack_damage_list_count; i++) {
        if (g->base_attack_damage_list[i].id == id) {
            g->base_attack_damage_list[i].data = damage;
            return true;
        }
    }
    return false;
}

vec3 g_get_base_attack_damage(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->base_attack_damage_list == NULL) return (vec3){0, 0, 0};
    for (int i = 0; i < g->base_attack_damage_list_count; i++) {
        if (g->base_attack_damage_list[i].id == id) return g->base_attack_damage_list[i].data;
    }
    return (vec3){0, 0, 0};
}

bool g_add_vision_distance(gamestate* const g, entityid id, int distance) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the vision distance component
    return g_add_component(
        g, id, C_VISION_DISTANCE, (void*)&distance, sizeof(int_component), (void**)&g->vision_distance_list, &g->vision_distance_list_count, &g->vision_distance_list_capacity);
}

bool g_has_vision_distance(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_VISION_DISTANCE);
}

bool g_set_vision_distance(gamestate* const g, entityid id, int distance) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->vision_distance_list == NULL) return false;
    for (int i = 0; i < g->vision_distance_list_count; i++) {
        if (g->vision_distance_list[i].id == id) {
            g->vision_distance_list[i].data = distance;
            return true;
        }
    }
    return false;
}

int g_get_vision_distance(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->vision_distance_list == NULL) return 0;
    for (int i = 0; i < g->vision_distance_list_count; i++) {
        if (g->vision_distance_list[i].id == id) return g->vision_distance_list[i].data;
    }
    return 0;
}

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

bool g_add_ringtype(gamestate* const g, entityid id, int type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the ringtype component
    if (type < RING_NONE || type >= RING_TYPE_COUNT) {
        merror("Invalid ring type %d", type);
        return false;
    }

    g_add_component(g, id, C_RINGTYPE, (void*)&type, sizeof(int_component), (void**)&g->ringtype_list, &g->ringtype_list_count, &g->ringtype_list_capacity);
    return true;
}

bool g_has_ringtype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_RINGTYPE);
}

bool g_set_ringtype(gamestate* const g, entityid id, int type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->ringtype_list == NULL) return false;
    for (int i = 0; i < g->ringtype_list_count; i++) {
        if (g->ringtype_list[i].id == id) {
            g->ringtype_list[i].data = type;
            return true;
        }
    }
    return false;
}

ringtype g_get_ringtype(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->ringtype_list == NULL) return RING_NONE;
    for (int i = 0; i < g->ringtype_list_count; i++) {
        if (g->ringtype_list[i].id == id) return g->ringtype_list[i].data;
    }
    return RING_NONE;
}

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
        if (g->light_radius_bonus_list[i].id == id) return g->light_radius_bonus_list[i].data;
    }
    return 0;
}

//int g_get_entity_total_light_radius_bonus(gamestate* const g, entityid id) {
int g_get_entity_total_light_radius_bonus(const gamestate* const g, entityid id) {
    int total_light_radius_bonus = 0;
    // get the light radius bonus from the equipment
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is invalid");
    // check each equipment slot
    for (int i = 0; i < EQUIPMENT_SLOT_COUNT; i++) {
        entityid equip_id = g_get_equipment(g, id, i);
        if (equip_id == ENTITYID_INVALID) continue;
        if (!g_has_light_radius_bonus(g, equip_id)) continue;
        int light_radius_bonus = g_get_light_radius_bonus(g, equip_id);
        total_light_radius_bonus += light_radius_bonus;
    }
    // only return the total light radius bonus
    // it is fine if it is negative that might be fun for cursed items
    return total_light_radius_bonus;
}
