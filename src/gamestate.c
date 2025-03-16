#include "gamestate.h"
#include "controlmode.h"
#include "mprint.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define GAMESTATE_DEBUGPANEL_DEFAULT_X 5
#define GAMESTATE_DEBUGPANEL_DEFAULT_Y 5
#define GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE 20

// have to update this function when we introduce new fields to Gamestate
gamestate* gamestateinitptr() {
    gamestate* g = (gamestate*)malloc(sizeof(gamestate));
    if (g == NULL) {
#ifdef DEBUG
        merror("gamestateinitptr malloc failed");
#endif
        return NULL;
    }
    g->framecount = 0;

    g->debugpanel.x = GAMESTATE_DEBUGPANEL_DEFAULT_X;
    g->debugpanel.y = GAMESTATE_DEBUGPANEL_DEFAULT_Y;
    g->debugpanel.w = g->debugpanel.h = 200;
    g->debugpanel.fg_color = RAYWHITE;
    g->debugpanel.bg_color = Fade(RED, 0.8f);
    g->debugpanel.pad_top = g->debugpanel.pad_left = g->debugpanel.pad_right = g->debugpanel.pad_bottom = 10;
    g->debugpanel.font_size = GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE;
    g->lock = 0;
    g->targetwidth = g->targetheight = g->windowwidth = g->windowheight = -1;
    g->timebegan = g->currenttime = time(NULL);
    g->timebegantm = g->currenttimetm = localtime(&(g->currenttime));
    bzero(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF);
    bzero(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF);
    strftime(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF, "Start Time: %Y-%m-%d %H:%M:%S", g->timebegantm);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
    g->debugpanelon = g->player_input_received = g->is_locked = g->gridon = g->processing_actions = false;
    g->cam_lockon = true;
    g->cam2d.target = (Vector2){0, 0};
    g->cam2d.offset = (Vector2){100, 100};
    g->cam2d.zoom = 6.0f;
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

    return g;
}


void gamestatefree(gamestate* g) {
    if (!g) {
        merror("gamestatefree: g is NULL");
        return;
    }
    minfo("Freeing gamestate");
    minfo("Freeing em");
    em_free(g->entitymap);
    minfo("Freeing entityids");
    free(g->entityids);
    //free(g->timebegantm);
    //free(g->currenttimetm);
    minfo("Freeing dungeon");
    dungeon_destroy(g->dungeon);
    minfo("Freeing g...");
    free(g);
    msuccess("Freed gamestate");
}


void gamestate_add_entityid(gamestate* const g, const entityid id) {
    if (g == NULL) {
        merror("gamestate_add_entityid g is NULL");
        return;
    }
    if (g->entityids == NULL) {
        merror("gamestate_add_entityid g->entityids is NULL");
        return;
    }
    if (g->index_entityids >= g->max_entityids) {
        merror("gamestate_add_entityid g->index_entityids >= g->max_entityids");
        return;
    }
    g->entityids[g->index_entityids] = id;
    g->index_entityids++;
}


void gamestate_set_hero_id(gamestate* const g, const entityid id) {
    if (!g) {
        merror("gamestate_set_hero_id: g is NULL");
        return;
    }

    g->hero_id = id;
}


const entityid gamestate_get_hero_id(const gamestate* const g) {
    if (!g) {
        merror("gamestate_get_hero_id: g is NULL");
        return -1;
    }

    return g->hero_id;
}


em_t* gamestate_get_entitymap(gamestate* const g) {
    if (!g) {
        merror("gamestate_get_entitymap: g is NULL");
        return NULL;
    }

    return g->entitymap;
}

#define GAMESTATE_INIT_ENTITYIDS_MAX 1000

void gamestate_init_entityids(gamestate* const g) {

    if (!g) {
        merror("gamestate_init_entityids: g is NULL");
        return;
    }

    //if (g->max_entityids == -1) {
    //    merror("gamestate_init_entityids: g->max_entityids is -1");
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
