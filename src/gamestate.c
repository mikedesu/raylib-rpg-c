#include "controlmode.h"
#include "entityid.h"
#include "gamestate.h"
#include "gamestate_flag.h"
#include "massert.h"
#include "mprint.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GAMESTATE_DEBUGPANEL_DEFAULT_X 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_Y 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE 10
#define GAMESTATE_INIT_ENTITYIDS_MAX 1000000

// have to update this function when we introduce new fields to Gamestate
gamestate* gamestateinitptr() {
    gamestate* g = (gamestate*)malloc(sizeof(gamestate));
    massert(g, "g is NULL");
    g->framecount = 0;
    g->turn_count = 0;
    g->debugpanel.x = GAMESTATE_DEBUGPANEL_DEFAULT_X;
    g->debugpanel.y = GAMESTATE_DEBUGPANEL_DEFAULT_Y;
    g->debugpanel.w = 200;
    g->debugpanel.h = 200;
    g->debugpanel.fg_color = RAYWHITE;
    g->debugpanel.bg_color = Fade(RED, 0.8f);
    g->debugpanel.pad_top = 0;
    g->debugpanel.pad_left = 0;
    g->debugpanel.pad_right = 0;
    g->debugpanel.pad_bottom = 0;
    g->debugpanel.font_size = GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE;
    g->lock = 0;
    g->targetwidth = -1;
    g->targetheight = -1;
    g->windowwidth = -1;
    g->windowheight = -1;
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
    g->processing_actions = false;
    g->cam_lockon = true;
    g->cam2d.target = (Vector2){0, 0};
    g->cam2d.offset = (Vector2){0, 0};
    g->cam2d.zoom = 10.0f;
    g->cam2d.rotation = 0.0;
    g->cam3d = (Camera3D){0};
    g->cam3d.position = (Vector3){0.0f, 20.0f, 20.0f};
    g->cam3d.target = (Vector3){0.0f, 0.0f, 0.0f};
    g->cam3d.up = (Vector3){0.0f, 1.0f, 0.0f};
    g->cam3d.fovy = 45.0f;
    g->cam3d.projection = CAMERA_PERSPECTIVE;
    g->camera_mode = CAMERA_FREE;
    g->is3d = false;
    g->fadealpha = 0.0f;
    g->controlmode = CONTROLMODE_PLAYER;
    g->fadestate = FADESTATENONE;
    g->d = NULL;
    g->entitymap = NULL;
    g->entityids = NULL;
    g->index_entityids = 0;
    g->max_entityids = -1;
    g->hero_id = -1;
    // current displayed dungeon floor
    g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
    g->entity_turn = -1;
    g->gameover = false;
    g->test_guard = false;
    g->font_size = 20;
    g->pad = 20;
    g->line_spacing = 1.0f;
    g->inventory_menu_selection = 0;

    g->components = ct_create();

    g->name_list_count = g->type_list_count = g->race_list_count = g->direction_list_count = g->loc_list_count = g->sprite_move_list_count =
        g->dead_list_count = g->update_list_count = g->attacking_list_count = g->blocking_list_count = g->block_success_list_count = g->damaged_list_count =
            g->inventory_list_count = g->equipped_weapon_list_count = g->equipped_shield_list_count = g->target_list_count = g->target_path_list_count = 0;

    g->name_list_capacity = g->type_list_capacity = g->race_list_capacity = g->direction_list_capacity = g->loc_list_capacity = g->sprite_move_list_capacity =
        g->dead_list_capacity = g->update_list_capacity = g->attacking_list_capacity = g->blocking_list_capacity = g->block_success_list_capacity =
            g->damaged_list_capacity = g->inventory_list_capacity = g->equipped_weapon_list_capacity = g->equipped_shield_list_capacity =
                g->target_list_capacity = g->target_path_list_capacity = LIST_INIT_CAPACITY;

    g->name_list = (name_component*)malloc(sizeof(name_component) * g->name_list_capacity);
    massert(g->name_list, "g->name_list is NULL");
    g->type_list = (type_component*)malloc(sizeof(type_component) * g->type_list_capacity);
    massert(g->type_list, "g->type_list is NULL");
    g->race_list = (race_component*)malloc(sizeof(race_component) * g->race_list_capacity);
    massert(g->race_list, "g->race_list is NULL");
    g->direction_list = (direction_component*)malloc(sizeof(direction_component) * g->direction_list_capacity);
    massert(g->direction_list, "g->direction_list is NULL");
    g->loc_list = (loc_component*)malloc(sizeof(loc_component) * g->loc_list_capacity);
    massert(g->loc_list, "g->loc_list is NULL");
    g->sprite_move_list = (sprite_move_component*)malloc(sizeof(sprite_move_component) * g->sprite_move_list_capacity);
    massert(g->sprite_move_list, "g->sprite_move_list is NULL");
    g->dead_list = (dead_component*)malloc(sizeof(dead_component) * g->dead_list_capacity);
    massert(g->dead_list, "g->dead_list is NULL");
    g->update_list = (update_component*)malloc(sizeof(update_component) * g->update_list_capacity);
    massert(g->update_list, "g->update_list is NULL");
    g->attacking_list = (attacking_component*)malloc(sizeof(attacking_component) * g->attacking_list_capacity);
    massert(g->attacking_list, "g->attacking_list is NULL");
    g->blocking_list = (blocking_component*)malloc(sizeof(blocking_component) * g->blocking_list_capacity);
    massert(g->blocking_list, "g->blocking_list is NULL");
    g->block_success_list = (block_success_component*)malloc(sizeof(block_success_component) * g->block_success_list_capacity);
    massert(g->block_success_list, "g->block_success_list is NULL");
    g->damaged_list = (damaged_component*)malloc(sizeof(damaged_component) * g->damaged_list_capacity);
    massert(g->damaged_list, "g->damaged_list is NULL");
    g->inventory_list = (inventory_component*)malloc(sizeof(inventory_component) * g->inventory_list_capacity);
    massert(g->inventory_list, "g->inventory_list is NULL");
    g->equipped_weapon_list = (equipped_weapon_component*)malloc(sizeof(equipped_weapon_component) * g->equipped_weapon_list_capacity);
    massert(g->equipped_weapon_list, "g->equipped_weapon_list is NULL");
    g->equipped_shield_list = (equipped_shield_component*)malloc(sizeof(equipped_shield_component) * g->equipped_shield_list_capacity);
    massert(g->equipped_shield_list, "g->equipped_shield_list is NULL");
    g->target_list = (target_component*)malloc(sizeof(target_component) * g->target_list_capacity);
    massert(g->target_list, "g->target_list is NULL");
    g->target_path_list = (target_path_component*)malloc(sizeof(target_path_component) * g->target_path_list_capacity);
    massert(g->target_path_list, "g->target_path_list is NULL");

    gamestate_init_msg_history(g);
    return g;
}

bool gamestate_init_msg_history(gamestate* const g) {
    massert(g, "g is NULL");
    g->msg_history.count = 0;
    g->msg_history.max_count = DEFAULT_MAX_HISTORY_SIZE;
    g->msg_history.messages = (char**)malloc(sizeof(char*) * g->msg_history.max_count);
    if (g->msg_history.messages == NULL) {
        merror("g->msg_history.messages is NULL");
        free(g);
        return false;
    }
    for (int i = 0; i < g->msg_history.max_count; i++) {
        g->msg_history.messages[i] = (char*)malloc(sizeof(char) * MAX_MSG_LENGTH);
        if (g->msg_history.messages[i] == NULL) {
            merror("g->msg_history.messages[%d] is NULL", i);
            for (int j = 0; j < i; j++) { free(g->msg_history.messages[j]); }
            free(g->msg_history.messages);
            //free(g);
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
    free(g->entityids);
    //minfo("Freeing dungeon");
    //dungeon_destroy(g->dungeon);
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
    free(g->equipped_weapon_list);
    free(g->equipped_shield_list);
    free(g->target_list);
    free(g->target_path_list);
    free(g);
    msuccess("Freed gamestate");
}

bool gs_add_entityid(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != -1, "id is -1");
    massert(g->index_entityids < g->max_entityids, "index_entityids >= max_entityids");
    massert(g->entityids, "g->entityids is NULL");
    g->entityids[g->index_entityids] = id;
    g->index_entityids++;
    return true;
}

void gamestate_set_hero_id(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    g->hero_id = id;
}

entityid gamestate_get_hero_id(const gamestate* const g) {
    massert(g, "g is NULL");
    return g->hero_id;
}

em_t* gamestate_get_entitymap(gamestate* const g) {
    massert(g, "g is NULL");
    return g->entitymap;
}

void gamestate_init_entityids(gamestate* const g) {
    massert(g, "g is NULL");
    g->entityids = (entityid*)malloc(sizeof(entityid) * GAMESTATE_INIT_ENTITYIDS_MAX);
    if (g->entityids == NULL) {
        merror("g->entityids is NULL");
        return;
    }
    for (int i = 0; i < g->max_entityids; i++) { g->entityids[i] = -1; }
    g->index_entityids = 0;
    g->max_entityids = GAMESTATE_INIT_ENTITYIDS_MAX;
}

entityid gamestate_get_entityid_unsafe(const gamestate* const g, int index) { return g->entityids[index]; }

//void gamestate_dungeon_destroy(gamestate* const g) {
//    if (!g) {
//        merror("g is NULL");
//        return;
//    }
//    dungeon_destroy(g->dungeon);
//    g->dungeon = NULL;
//}

int gamestate_get_entityid_index(const gamestate* const g, entityid id) {
    if (!g) {
        merror("g is NULL");
        return -1;
    }
    for (int i = 0; i < g->max_entityids; i++) {
        if (g->entityids[i] == id) { return i; }
    }
    return -1;
}

int gamestate_get_next_npc_entityid_from_index(const gamestate* const g, int index) {
    if (!g) {
        merror("g is NULL");
        return -1;
    }
    for (int i = index + 1; i < g->max_entityids; i++) {
        if (g->entityids[i] != -1) { return g->entityids[i]; }
    }
    return -1;
}

void gamestate_incr_entity_turn(gamestate* const g) {
    if (!g) {
        merror("g is NULL");
        return;
    }
    if (g->entity_turn == -1) {
        if (g->hero_id == -1) {
            merror("both g->entity_turn and g->hero_id are -1");
            return;
        }
        g->entity_turn = g->hero_id;
    } else {
        // given that entity_turn is an entityid, we need to find the next entity in our
        // entityids array that belongs to an NPC
        const int index = gamestate_get_entityid_index(g, g->entity_turn);
        if (index == -1) {
            merror("index is -1");
            return;
        }
        g->entity_turn = gamestate_get_next_npc_entityid_from_index(g, index);
    }
}

void gamestate_load_keybindings(gamestate* const g) {
    if (!g) {
        merror("g is NULL");
        return;
    }
    const char* filename = "keybindings.ini";
    load_keybindings(filename, &g->keybinding_list);
}

bool gamestate_add_msg_history(gamestate* const g, const char* msg) {
    massert(g, "g is NULL");
    massert(msg, "msg is NULL");
    if (g->msg_history.count >= g->msg_history.max_count) {
        merror("msg history is full");
        return false;
    }
    strncpy(g->msg_history.messages[g->msg_history.count], msg, MAX_MSG_LENGTH);
    g->msg_history.count++;
    return true;
}

void gamestate_set_debug_panel_pos_bottom_left(gamestate* const g) {
    massert(g, "g is NULL");
    if (g->windowwidth == -1 || g->windowheight == -1) {
        merror("windowwidth or windowheight is -1");
        return;
    }
    g->debugpanel.x = 0;
    g->debugpanel.y = g->windowheight - g->debugpanel.h;
}

void gamestate_set_debug_panel_pos_top_right(gamestate* const g) {
    massert(g, "g is NULL");
    if (g->windowwidth == -1 || g->windowheight == -1) {
        merror("windowwidth or windowheight is -1");
        return;
    }
    g->debugpanel.x = g->windowwidth - g->debugpanel.w;
    g->debugpanel.y = g->debugpanel.pad_right;
}

bool g_register_comp(gamestate* const g, entityid id, component comp) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(comp != C_COUNT, "comp is invalid");
    if (g->components == NULL) {
        merror("g->components is NULL");
        return false;
    }
    if (!ct_has_entity(g->components, id)) {
        if (!ct_add_entity(g->components, id)) {
            merror("ct_add_entity failed");
            return false;
        }
    }
    if (!ct_add_component(g->components, id, comp)) {
        merror("ct_add_component failed");
        return false;
    }
    return true;
}

bool g_register_comps(gamestate* const g, entityid id, ...) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->components == NULL) {
        merror("g->components is NULL");
        return false;
    }
    if (!ct_has_entity(g->components, id)) {
        if (!ct_add_entity(g->components, id)) {
            merror("ct_add_entity failed");
            return false;
        }
    }
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

bool g_add_name(gamestate* const g, entityid id, const char* name) {
    massert(g, "g is NULL");
    massert(name, "name is NULL");
    massert(g_has_component(g, id, C_NAME), "id %d does not have a name component", id);
    if (g->name_list_count >= g->name_list_capacity) {
        g->name_list_capacity *= 2;
        g->name_list = realloc(g->name_list, sizeof(name_component) * g->name_list_capacity);
        if (g->name_list == NULL) {
            merror("g->name_list is NULL");
            return false;
        }
    }
    init_name_component(&g->name_list[g->name_list_count], id, name);
    g->name_list_count++;
    return true;
}

bool g_has_component(const gamestate* const g, entityid id, component comp) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(comp != C_COUNT, "comp is invalid");
    if (g->components == NULL) {
        merror("g->components is NULL");
        return false;
    }
    //if (!ct_has_entity(g->components, id)) { return false; }
    return ct_has_component(g->components, id, comp);
}

const char* g_get_name(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g_has_component(g, id, C_NAME), "id %d does not have a type component", id);
    //if (!g_has_component(g, id, C_NAME)) {
    //    merror("id %d does not have a name component", id);
    //    return NULL;
    //}
    for (int i = 0; i < g->name_list_count; i++) {
        if (g->name_list[i].id == id) { return g->name_list[i].name; }
    }
    return NULL;
}

bool g_add_type(gamestate* const g, entityid id, entitytype_t type) {
    massert(g, "g is NULL");
    massert(type > ENTITY_NONE && type < ENTITY_TYPE_COUNT, "type is invalid");
    massert(g_has_component(g, id, C_TYPE), "id %d does not have a type component", id);
    if (g->type_list_count >= g->type_list_capacity) {
        g->type_list_capacity *= 2;
        g->type_list = realloc(g->type_list, sizeof(type_component) * g->type_list_capacity);
        if (g->type_list == NULL) {
            merror("g->type_list is NULL");
            return false;
        }
    }
    init_type_component(&g->type_list[g->type_list_count], id, type);
    g->type_list_count++;
    return true;
}

entitytype_t g_get_type(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    //massert(id != ENTITYID_INVALID, "id is invalid");
    if (id != ENTITYID_INVALID) {
        for (int i = 0; i < g->type_list_count; i++) {
            if (g->type_list[i].id == id) { return g->type_list[i].type; }
        }
    }
    return ENTITY_NONE;
}

bool g_has_type(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_TYPE);
}

bool g_is_type(const gamestate* const g, entityid id, entitytype_t type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(type > ENTITY_NONE && type < ENTITY_TYPE_COUNT, "type is invalid");
    massert(g->type_list, "g->type_list is NULL");
    if (g->type_list == NULL) {
        merror("g->type_list is NULL");
        return false;
    }
    return type == g_get_type(g, id);
}

bool g_add_race(gamestate* const g, entityid id, race_t race) {
    massert(g, "g is NULL");
    //massert(type > ENTITY_NONE && type < ENTITY_TYPE_COUNT, "type is invalid");
    // make sure the entity has the component
    massert(g_has_component(g, id, C_RACE), "id %d does not have a race component", id);
    if (g_has_component(g, id, C_RACE)) {
        if (g->race_list_count >= g->race_list_capacity) {
            g->race_list_capacity *= 2;
            g->race_list = realloc(g->race_list, sizeof(race_component) * g->race_list_capacity);
            if (g->race_list == NULL) {
                merror("g->race_list is NULL");
                return false;
            }
        }
        init_race_component(&g->race_list[g->race_list_count], id, race);
        g->race_list_count++;
        return true;
    }
    return false;
}

race_t g_get_race(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g_has_race(g, id)) {
        for (int i = 0; i < g->race_list_count; i++) {
            if (g->race_list[i].id == id) { return g->race_list[i].race; }
        }
    }
    return RACE_NONE;
}

bool g_is_race(gamestate* const g, entityid id, race_t race) {
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
    if (!g_has_component(g, id, C_DIRECTION)) {
        merror("id %d does not have a direction component", id);
        return DIR_NONE;
    }
    for (int i = 0; i < g->direction_list_count; i++) {
        if (g->direction_list[i].id == id) { return g->direction_list[i].dir; }
    }
    return DIR_NONE;
}

bool g_has_direction(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_DIRECTION);
}

bool g_add_direction(gamestate* const g, entityid id, direction_t dir) {
    massert(g, "g is NULL");
    massert(dir >= 0 && dir < DIR_COUNT, "dir is invalid");
    // make sure the entity has the component
    massert(g_has_component(g, id, C_DIRECTION), "id %d does not have a direction component", id);
    if (g->direction_list_count >= g->direction_list_capacity) {
        g->direction_list_capacity *= 2;
        g->direction_list = realloc(g->direction_list, sizeof(direction_component) * g->direction_list_capacity);
        if (g->direction_list == NULL) {
            merror("g->direction_list is NULL");
            return false;
        }
    }
    init_direction_component(&g->direction_list[g->direction_list_count], id, dir);
    g->direction_list_count++;
    return true;
}

bool g_is_direction(gamestate* const g, entityid id, direction_t dir) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_get_direction(g, id) == dir;
}

bool g_update_direction(gamestate* const g, entityid id, direction_t dir) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->direction_list == NULL) {
        merror("g->direction_list is NULL");
        return false;
    }
    for (int i = 0; i < g->direction_list_count; i++) {
        if (g->direction_list[i].id == id) {
            g->direction_list[i].dir = dir;
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

bool g_add_location(gamestate* const g, entityid id, loc_t loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the location component
    massert(g_has_component(g, id, C_LOCATION), "id %d does not have a location component", id);
    if (g->loc_list_count >= g->loc_list_capacity) {
        g->loc_list_capacity *= 2;
        g->loc_list = realloc(g->loc_list, sizeof(loc_component) * g->loc_list_capacity);
        if (g->loc_list == NULL) {
            merror("g->loc_list is NULL");
            return false;
        }
    }
    init_loc_component(&g->loc_list[g->loc_list_count], id, loc);
    g->loc_list_count++;
    return true;
}

bool g_update_location(gamestate* const g, entityid id, loc_t loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->loc_list == NULL) {
        merror("g->loc_list is NULL");
        return false;
    }
    for (int i = 0; i < g->loc_list_count; i++) {
        if (g->loc_list[i].id == id) {
            g->loc_list[i].loc = loc;
            return true;
        }
    }
    return false;
}

loc_t g_get_location(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->loc_list == NULL) {
        merror("g->loc_list is NULL");
        return (loc_t){-1, -1};
    }
    for (int i = 0; i < g->loc_list_count; i++) {
        if (g->loc_list[i].id == id) { return g->loc_list[i].loc; }
    }
    merror("id %d not found in loc_list", id);
    return (loc_t){-1, -1};
}

bool g_is_location(const gamestate* const g, entityid id, loc_t loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->loc_list == NULL) {
        merror("g->loc_list is NULL");
        return false;
    }
    loc_t loc2 = g_get_location(g, id);
    return (loc.x == loc2.x && loc.y == loc2.y);
}

bool g_has_sprite_move(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_SPRITE_MOVE);
}

bool g_add_sprite_move(gamestate* const g, entityid id, loc_t loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the sprite move component
    massert(g_has_component(g, id, C_SPRITE_MOVE), "id %d does not have a sprite move component", id);
    if (g->sprite_move_list_count >= g->sprite_move_list_capacity) {
        g->sprite_move_list_capacity *= 2;
        g->sprite_move_list = realloc(g->sprite_move_list, sizeof(sprite_move_component) * g->sprite_move_list_capacity);
        if (g->sprite_move_list == NULL) {
            merror("g->sprite_move_list is NULL");
            return false;
        }
    }
    init_sprite_move_component(&g->sprite_move_list[g->sprite_move_list_count], id, loc);
    g->sprite_move_list_count++;
    return true;
}

bool g_update_sprite_move(gamestate* const g, entityid id, loc_t loc) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g_has_component(g, id, C_SPRITE_MOVE), "id %d does not have a sprite move component", id);
    if (g_has_sprite_move(g, id)) {
        if (g->sprite_move_list == NULL) {
            merror("g->sprite_move_list is NULL");
            return false;
        }
        for (int i = 0; i < g->sprite_move_list_count; i++) {
            if (g->sprite_move_list[i].id == id) {
                g->sprite_move_list[i].loc = loc;
                return true;
            }
        }
    }
    return false;
}

loc_t g_get_sprite_move(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g_has_sprite_move(g, id), "id %d does not have a sprite move component", id);
    if (g_has_sprite_move(g, id)) {
        for (int i = 0; i < g->sprite_move_list_count; i++) {
            if (g->sprite_move_list[i].id == id) { return g->sprite_move_list[i].loc; }
        }
    }
    merror("id %d not found in sprite_move_list", id);
    return (loc_t){-1, -1};
}

bool g_has_dead(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_DEAD);
}

bool g_add_dead(gamestate* const g, entityid id, bool dead) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the dead component
    massert(g_has_component(g, id, C_DEAD), "id %d does not have a dead component", id);
    if (g->dead_list_count >= g->dead_list_capacity) {
        g->dead_list_capacity *= 2;
        g->dead_list = realloc(g->dead_list, sizeof(dead_component) * g->dead_list_capacity);
        if (g->dead_list == NULL) {
            merror("g->dead_list is NULL");
            return false;
        }
    }
    init_dead_component(&g->dead_list[g->dead_list_count], id, dead);
    g->dead_list_count++;
    return true;
}

bool g_update_dead(gamestate* const g, entityid id, bool dead) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(g_has_component(g, id, C_DEAD), "id %d does not have a dead component", id);
    if (g->dead_list == NULL) {
        merror("g->dead_list is NULL");
        return false;
    }
    for (int i = 0; i < g->dead_list_count; i++) {
        if (g->dead_list[i].id == id) {
            g->dead_list[i].dead = dead;
            return true;
        }
    }
    return false;
}

bool g_is_dead(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    //massert(id != ENTITYID_INVALID, "id is invalid");
    if (id != ENTITYID_INVALID) {
        if (g->dead_list == NULL) {
            merror("g->dead_list is NULL");
            return false;
        }
        for (int i = 0; i < g->dead_list_count; i++) {
            if (g->dead_list[i].id == id) { return g->dead_list[i].dead; }
        }
    }
    return false;
}

bool g_has_update(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_UPDATE);
}

bool g_add_update(gamestate* const g, entityid id, bool update) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the update component
    massert(g_has_component(g, id, C_UPDATE), "id %d does not have an update component", id);
    if (g->update_list_count >= g->update_list_capacity) {
        g->update_list_capacity *= 2;
        g->update_list = realloc(g->update_list, sizeof(update_component) * g->update_list_capacity);
        if (g->update_list == NULL) {
            merror("g->update_list is NULL");
            return false;
        }
    }
    init_update_component(&g->update_list[g->update_list_count], id, update);
    g->update_list_count++;
    return true;
}

bool g_get_update(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->update_list == NULL) {
        merror("g->update_list is NULL");
        return false;
    }
    for (int i = 0; i < g->update_list_count; i++) {
        if (g->update_list[i].id == id) { return g->update_list[i].update; }
    }
    merror("id %d not found in update_list", id);
    return false;
}

bool g_set_update(gamestate* const g, entityid id, bool update) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->update_list == NULL) {
        merror("g->update_list is NULL");
        return false;
    }
    for (int i = 0; i < g->update_list_count; i++) {
        if (g->update_list[i].id == id) {
            g->update_list[i].update = update;
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

bool g_add_attacking(gamestate* const g, entityid id, bool attacking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the attacking component
    massert(g_has_component(g, id, C_ATTACKING), "id %d does not have an attacking component", id);
    if (g->attacking_list_count >= g->attacking_list_capacity) {
        g->attacking_list_capacity *= 2;
        g->attacking_list = realloc(g->attacking_list, sizeof(attacking_component) * g->attacking_list_capacity);
        if (g->attacking_list == NULL) {
            merror("g->attacking_list is NULL");
            return false;
        }
    }
    init_attacking_component(&g->attacking_list[g->attacking_list_count], id, attacking);
    g->attacking_list_count++;
    return true;
}

bool g_set_attacking(gamestate* const g, entityid id, bool attacking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->attacking_list == NULL) {
        merror("g->attacking_list is NULL");
        return false;
    }
    for (int i = 0; i < g->attacking_list_count; i++) {
        if (g->attacking_list[i].id == id) {
            g->attacking_list[i].attacking = attacking;
            return true;
        }
    }
    return false;
}

bool g_get_attacking(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->attacking_list == NULL) {
        merror("g->attacking_list is NULL");
        return false;
    }
    for (int i = 0; i < g->attacking_list_count; i++) {
        if (g->attacking_list[i].id == id) { return g->attacking_list[i].attacking; }
    }
    merror("id %d not found in attacking_list", id);
    return false;
}

bool g_has_blocking(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_BLOCKING);
}

bool g_add_blocking(gamestate* const g, entityid id, bool blocking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the blocking component
    massert(g_has_component(g, id, C_BLOCKING), "id %d does not have a blocking component", id);
    if (g->blocking_list_count >= g->blocking_list_capacity) {
        g->blocking_list_capacity *= 2;
        g->blocking_list = realloc(g->blocking_list, sizeof(blocking_component) * g->blocking_list_capacity);
        if (g->blocking_list == NULL) {
            merror("g->blocking_list is NULL");
            return false;
        }
    }
    init_blocking_component(&g->blocking_list[g->blocking_list_count], id, blocking);
    g->blocking_list_count++;
    return true;
}

bool g_set_blocking(gamestate* const g, entityid id, bool blocking) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->blocking_list == NULL) {
        merror("g->blocking_list is NULL");
        return false;
    }
    for (int i = 0; i < g->blocking_list_count; i++) {
        if (g->blocking_list[i].id == id) {
            g->blocking_list[i].blocking = blocking;
            return true;
        }
    }
    return false;
}

bool g_get_blocking(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->blocking_list == NULL) {
        merror("g->blocking_list is NULL");
        return false;
    }
    for (int i = 0; i < g->blocking_list_count; i++) {
        if (g->blocking_list[i].id == id) { return g->blocking_list[i].blocking; }
    }
    merror("id %d not found in blocking_list", id);
    return false;
}

bool g_has_block_success(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_BLOCK_SUCCESS);
}

bool g_add_block_success(gamestate* const g, entityid id, bool block_success) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the block success component
    massert(g_has_component(g, id, C_BLOCK_SUCCESS), "id %d does not have a block success component", id);
    if (g->block_success_list_count >= g->block_success_list_capacity) {
        g->block_success_list_capacity *= 2;
        g->block_success_list = realloc(g->block_success_list, sizeof(block_success_component) * g->block_success_list_capacity);
        if (g->block_success_list == NULL) {
            merror("g->block_success_list is NULL");
            return false;
        }
    }
    init_block_success_component(&g->block_success_list[g->block_success_list_count], id, block_success);
    g->block_success_list_count++;
    return true;
}

bool g_set_block_success(gamestate* const g, entityid id, bool block_success) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->block_success_list == NULL) {
        merror("g->block_success_list is NULL");
        return false;
    }
    for (int i = 0; i < g->block_success_list_count; i++) {
        if (g->block_success_list[i].id == id) {
            g->block_success_list[i].block_success = block_success;
            return true;
        }
    }
    return false;
}

bool g_get_block_success(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->block_success_list == NULL) {
        merror("g->block_success_list is NULL");
        return false;
    }
    for (int i = 0; i < g->block_success_list_count; i++) {
        if (g->block_success_list[i].id == id) { return g->block_success_list[i].block_success; }
    }
    merror("id %d not found in block_success_list", id);
    return false;
}

bool g_has_damaged(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    return g_has_component(g, id, C_DAMAGED);
}

bool g_add_damaged(gamestate* const g, entityid id, bool damaged) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    // make sure the entity has the damaged component
    massert(g_has_component(g, id, C_DAMAGED), "id %d does not have a damaged component", id);
    if (g->damaged_list_count >= g->damaged_list_capacity) {
        g->damaged_list_capacity *= 2;
        g->damaged_list = realloc(g->damaged_list, sizeof(damaged_component) * g->damaged_list_capacity);
        if (g->damaged_list == NULL) {
            merror("g->damaged_list is NULL");
            return false;
        }
    }
    init_damaged_component(&g->damaged_list[g->damaged_list_count], id, damaged);
    g->damaged_list_count++;
    return true;
}

bool g_set_damaged(gamestate* const g, entityid id, bool damaged) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->damaged_list == NULL) {
        merror("g->damaged_list is NULL");
        return false;
    }
    for (int i = 0; i < g->damaged_list_count; i++) {
        if (g->damaged_list[i].id == id) {
            g->damaged_list[i].damaged = damaged;
            return true;
        }
    }
    return false;
}

bool g_get_damaged(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (g->damaged_list == NULL) {
        merror("g->damaged_list is NULL");
        return false;
    }
    for (int i = 0; i < g->damaged_list_count; i++) {
        if (g->damaged_list[i].id == id) { return g->damaged_list[i].damaged; }
    }
    //merror("id %d not found in damaged_list", id);
    return false;
}
