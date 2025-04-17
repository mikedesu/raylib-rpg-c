#include "controlmode.h"
#include "gamestate.h"
#include "gamestate_flag.h"
#include "massert.h"
#include "mprint.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GAMESTATE_DEBUGPANEL_DEFAULT_X 5
#define GAMESTATE_DEBUGPANEL_DEFAULT_Y 5
#define GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE 20
#define GAMESTATE_INIT_ENTITYIDS_MAX 1000000

// have to update this function when we introduce new fields to Gamestate
gamestate* gamestateinitptr() {
    gamestate* g = (gamestate*)malloc(sizeof(gamestate));
    massert(g, "gamestateinitptr: g is NULL");
    g->framecount = 0;
    g->turn_count = 0;
    g->debugpanel.x = GAMESTATE_DEBUGPANEL_DEFAULT_X;
    g->debugpanel.y = GAMESTATE_DEBUGPANEL_DEFAULT_Y;
    g->debugpanel.w = 200;
    g->debugpanel.h = 200;
    g->debugpanel.fg_color = RAYWHITE;
    g->debugpanel.bg_color = Fade(RED, 0.8f);
    g->debugpanel.pad_top = 10;
    g->debugpanel.pad_left = 10;
    g->debugpanel.pad_right = 10;
    g->debugpanel.pad_bottom = 10;
    g->debugpanel.font_size = GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE;
    g->lock = 0;
    g->targetwidth = -1;
    g->targetheight = -1;
    g->windowwidth = -1;
    g->windowheight = -1;
    g->timebegan = g->currenttime = time(NULL);
    g->timebegantm = g->currenttimetm = localtime(&(g->currenttime));
    bzero(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF);
    bzero(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF);
    strftime(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF, "Start Time: %Y-%m-%d %H:%M:%S", g->timebegantm);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
    g->debugpanelon = false;
    g->player_input_received = false;
    g->is_locked = false;
    g->gridon = false;
    g->processing_actions = false;
    g->cam_lockon = true;
    g->cam2d.target = (Vector2){0, 0};
    g->cam2d.offset = (Vector2){100, 100};
    g->cam2d.zoom = 9.0f;
    g->cam2d.rotation = 0.0;
    g->cam3d = (Camera3D){0};
    //g->cam3d.position = (Vector3){0.0f, 30.0f, 0.0f};
    g->cam3d.position = (Vector3){0.0f, 20.0f, 20.0f};
    g->cam3d.target = (Vector3){0.0f, 0.0f, 0.0f};
    g->cam3d.up = (Vector3){0.0f, 1.0f, 0.0f};
    //g->cam3d.up = (Vector3){0.0f, 0.0f, -1.0f};
    g->cam3d.fovy = 45.0f;
    g->cam3d.projection = CAMERA_PERSPECTIVE;
    g->camera_mode = CAMERA_FREE;
    g->is3d = false;
    g->fadealpha = 0.0f;
    g->controlmode = CONTROLMODE_PLAYER;
    //g->controlmode = CONTROLMODE_CAMERA;
    g->fadestate = FADESTATENONE;
    //g->spritegroups = NULL;
    //g->dungeon_floor = NULL;
    g->dungeon = NULL;
    g->entitymap = NULL;
    g->entityids = NULL;
    g->index_entityids = 0;
    g->max_entityids = -1;
    g->hero_id = -1;
    // current displayed dungeon floor
    //g->current_dungeon_floor = 0;
    g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
    g->entity_turn = -1;
    g->gameover = false;
    g->test_guard = false;
    //g->msg_history.count = 0;
    //g->msg_history.max_count = DEFAULT_MAX_HISTORY_SIZE;
    //g->msg_history.messages = (char**)malloc(sizeof(char*) * g->msg_history.max_count);
    //if (g->msg_history.messages == NULL) {
    //    merror("gamestateinitptr: g->msg_history.messages is NULL");
    //    free(g);
    //    return NULL;
    //}
    //for (int i = 0; i < g->msg_history.max_count; i++) {
    //    g->msg_history.messages[i] = (char*)malloc(sizeof(char) * MAX_MSG_LENGTH);
    //    if (g->msg_history.messages[i] == NULL) {
    //        merror("gamestateinitptr: g->msg_history.messages[%d] is NULL", i);
    //        for (int j = 0; j < i; j++) { free(g->msg_history.messages[j]); }
    //        free(g->msg_history.messages);
    //        free(g);
    //        return NULL;
    //    }
    //}
    gamestate_init_msg_history(g);
    return g;
}

bool gamestate_init_msg_history(gamestate* const g) {
    massert(g, "gamestate_init_msg_history: g is NULL");
    g->msg_history.count = 0;
    g->msg_history.max_count = DEFAULT_MAX_HISTORY_SIZE;
    g->msg_history.messages = (char**)malloc(sizeof(char*) * g->msg_history.max_count);
    if (g->msg_history.messages == NULL) {
        merror("gamestateinitptr: g->msg_history.messages is NULL");
        free(g);
        return false;
    }
    for (int i = 0; i < g->msg_history.max_count; i++) {
        g->msg_history.messages[i] = (char*)malloc(sizeof(char) * MAX_MSG_LENGTH);
        if (g->msg_history.messages[i] == NULL) {
            merror("gamestateinitptr: g->msg_history.messages[%d] is NULL", i);
            for (int j = 0; j < i; j++) {
                free(g->msg_history.messages[j]);
            }
            free(g->msg_history.messages);
            //free(g);
            return false;
        }
    }
    return true;
}

bool gamestate_free_msg_history(gamestate* const g) {
    massert(g, "gamestate_free_msg_history: g is NULL");
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
    massert(g, "gamestatefree: g is NULL");
    //if (!g) {
    //    merror("gamestatefree: g is NULL");
    //    return;
    //}
    minfo("Freeing gamestate");
    minfo("Freeing em");
    em_free(g->entitymap);
    minfo("Freeing entityids");
    free(g->entityids);
    //free(g->timebegantm);
    //free(g->currenttimetm);
    minfo("Freeing dungeon");
    //if (g->dungeon) dungeon_destroy(g->dungeon);
    dungeon_destroy(g->dungeon);

    // free message history
    //for (int i = 0; i < g->msg_history.max_count; i++) {
    //    if (g->msg_history.messages[i]) {
    //        free(g->msg_history.messages[i]);
    //        g->msg_history.messages[i] = NULL;
    //    }
    //}
    //free(g->msg_history.messages);
    gamestate_free_msg_history(g);
    minfo("Freeing g...");
    free(g);
    msuccess("Freed gamestate");
}

bool gamestate_add_entityid(gamestate* const g, entityid id) {
    massert(g, "gamestate_add_entityid: g is NULL");
    massert(id != -1, "gamestate_add_entityid: id is -1");
    massert(g->index_entityids < g->max_entityids, "gamestate_add_entityid: index_entityids >= max_entityids");
    massert(g->entityids, "gamestate_add_entityid: g->entityids is NULL");
    //if (g == NULL) {
    //    merror("gamestate_add_entityid g is NULL");
    //    return false;
    //}
    //if (g->entityids == NULL) {
    //    merror("gamestate_add_entityid g->entityids is NULL");
    //    return false;
    //}
    //if (g->index_entityids >= g->max_entityids) {
    //    merror("gamestate_add_entityid g->index_entityids >= g->max_entityids");
    //    return false;
    //}
    g->entityids[g->index_entityids] = id;
    g->index_entityids++;
    return true;
}

void gamestate_set_hero_id(gamestate* const g, entityid id) {
    massert(g, "gamestate_set_hero_id: g is NULL");
    //if (!g) {
    //    merror("gamestate_set_hero_id: g is NULL");
    //    return;
    //}
    g->hero_id = id;
}

entityid gamestate_get_hero_id(const gamestate* const g) {
    massert(g, "gamestate_get_hero_id: g is NULL");
    //if (!g) {
    //    merror("gamestate_get_hero_id: g is NULL");
    //    return -1;
    //}
    return g->hero_id;
}

em_t* gamestate_get_entitymap(gamestate* const g) {
    massert(g, "gamestate_get_entitymap: g is NULL");
    //if (!g) {
    //    merror("gamestate_get_entitymap: g is NULL");
    //    return NULL;
    //}
    return g->entitymap;
}

void gamestate_init_entityids(gamestate* const g) {
    massert(g, "gamestate_init_entityids: g is NULL");
    //if (!g) {
    //    merror("gamestate_init_entityids: g is NULL");
    //    return;
    //}
    g->entityids = (entityid*)malloc(sizeof(entityid) * GAMESTATE_INIT_ENTITYIDS_MAX);
    if (g->entityids == NULL) {
        merror("gamestate_init_entityids: g->entityids is NULL");
        return;
    }
    for (int i = 0; i < g->max_entityids; i++) {
        g->entityids[i] = -1;
    }
    g->index_entityids = 0;
    g->max_entityids = GAMESTATE_INIT_ENTITYIDS_MAX;
}

entityid gamestate_get_entityid_unsafe(const gamestate* const g, int index) { return g->entityids[index]; }

void gamestate_dungeon_destroy(gamestate* const g) {
    if (!g) {
        merror("gamestate_dungeon_destroy: g is NULL");
        return;
    }
    dungeon_destroy(g->dungeon);
    g->dungeon = NULL;
}

int gamestate_get_entityid_index(const gamestate* const g, entityid id) {
    if (!g) {
        merror("gamestate_get_entityid_index: g is NULL");
        return -1;
    }
    for (int i = 0; i < g->max_entityids; i++) {
        if (g->entityids[i] == id) {
            return i;
        }
    }
    return -1;
}

int gamestate_get_next_npc_entityid_from_index(const gamestate* const g, int index) {
    if (!g) {
        merror("gamestate_get_next_npc_entityid_from_index: g is NULL");
        return -1;
    }
    for (int i = index + 1; i < g->max_entityids; i++) {
        if (g->entityids[i] != -1) {
            return g->entityids[i];
        }
    }
    return -1;
}

void gamestate_incr_entity_turn(gamestate* const g) {
    if (!g) {
        merror("gamestate_incr_entity_turn: g is NULL");
        return;
    }
    if (g->entity_turn == -1) {
        if (g->hero_id == -1) {
            merror("gamestate_incr_entity_turn: both g->entity_turn and g->hero_id are -1");
            return;
        }
        g->entity_turn = g->hero_id;
        //g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
    } else {
        // given that entity_turn is an entityid, we need to find the next entity in our
        // entityids array that belongs to an NPC
        const int index = gamestate_get_entityid_index(g, g->entity_turn);
        if (index == -1) {
            merror("gamestate_incr_entity_turn: index is -1");
            return;
        }
        g->entity_turn = gamestate_get_next_npc_entityid_from_index(g, index);
    }
}

void gamestate_load_keybindings(gamestate* const g) {
    if (!g) {
        merror("gamestate_load_keybindings: g is NULL");
        return;
    }
    const char* filename = "keybindings.ini";
    load_keybindings(filename, &g->keybinding_list);
}

bool gamestate_add_msg_history(gamestate* const g, const char* msg) {
    massert(g, "gamestate_add_msg_history: g is NULL");
    massert(msg, "gamestate_add_msg_history: msg is NULL");
    if (g->msg_history.count >= g->msg_history.max_count) {
        merror("gamestate_add_msg_history: msg history is full");
        return false;
    }
    strncpy(g->msg_history.messages[g->msg_history.count], msg, MAX_MSG_LENGTH);
    g->msg_history.count++;
    return true;
}

void gamestate_set_debug_panel_pos_bottom_left(gamestate* const g) {
    massert(g, "gamestate_set_debug_panel_pos_bottom_left: g is NULL");
    if (g->windowwidth == -1 || g->windowheight == -1) {
        merror("gamestate_set_debug_panel_pos_bottom_left: windowwidth or windowheight is -1");
        return;
    }
    g->debugpanel.x = 0;
    g->debugpanel.y = g->windowheight - g->debugpanel.h;
}

void gamestate_set_debug_panel_pos_top_right(gamestate* const g) {
    massert(g, "gamestate_set_debug_panel_pos_top_right: g is NULL");
    if (g->windowwidth == -1 || g->windowheight == -1) {
        merror("gamestate_set_debug_panel_pos_top_right: windowwidth or windowheight is -1");
        return;
    }
    g->debugpanel.x = g->windowwidth - g->debugpanel.w;
    g->debugpanel.y = g->debugpanel.pad_right;
}
