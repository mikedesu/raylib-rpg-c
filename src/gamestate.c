#include "controlmode.h"
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
    g->dungeon = NULL;
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

    g->name_list_capacity = NAME_LIST_INIT_CAPACITY;
    g->name_list_count = 0;
    g->name_list = (name_component*)malloc(sizeof(name_component) * g->name_list_capacity);
    massert(g->name_list, "g->name_list is NULL");
    if (g->name_list == NULL) {
        merror("g->name_list is NULL");
        free(g);
        return NULL;
    }

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
    minfo("Freeing gamestate");
    //minfo("Freeing em");
    //em_free(g->entitymap);
    minfo("Freeing entityids");
    free(g->entityids);
    //minfo("Freeing dungeon");
    //dungeon_destroy(g->dungeon);
    // free message history
    gamestate_free_msg_history(g);

    ct_destroy(g->components);
    free(g->name_list);
    free(g->type_list);

    minfo("Freeing g...");
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

bool gs_register_comp(gamestate* const g, entityid id, component comp) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(comp != COMP_COUNT, "comp is invalid");
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

bool gs_register_comps(gamestate* const g, entityid id, ...) {
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
    //while ((comp = va_arg(args, component)) != COMP_COUNT) {
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

bool gs_add_name(gamestate* const g, entityid id, const char* name) {
    massert(g, "g is NULL");
    massert(name, "name is NULL");
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

bool gs_has_component(gamestate* const g, entityid id, component comp) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(comp != COMP_COUNT, "comp is invalid");
    if (g->components == NULL) {
        merror("g->components is NULL");
        return false;
    }
    if (!ct_has_entity(g->components, id)) { return false; }
    return ct_has_component(g->components, id, comp);
}

const char* gs_get_name(gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    for (int i = 0; i < g->name_list_count; i++) {
        if (g->name_list[i].id == id) { return g->name_list[i].name; }
    }
    return NULL;
}

bool gs_add_type(gamestate* const g, entityid id, entitytype_t type) {
    massert(g, "g is NULL");
    massert(type > ENTITY_NONE && type < ENTITY_TYPE_COUNT, "type is invalid");
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

entitytype_t gs_get_type(const gamestate* const g, entityid id) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    for (int i = 0; i < g->type_list_count; i++) {
        if (g->type_list[i].id == id) { return g->type_list[i].type; }
    }
    return ENTITY_NONE;
}

bool gs_is_type(const gamestate* const g, entityid id, entitytype_t type) {
    massert(g, "g is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(type > ENTITY_NONE && type < ENTITY_TYPE_COUNT, "type is invalid");
    massert(g->type_list, "g->type_list is NULL");
    if (g->type_list == NULL) {
        merror("g->type_list is NULL");
        return false;
    }
    return type == gs_get_type(g, id);
}
