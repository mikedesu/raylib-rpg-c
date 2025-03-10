#include "gamestate.h"
#include "controlmode.h"
#include "dungeon_floor.h"
#include "libgame_defines.h"
#include "mprint.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
    g->debugpanel.x = 0;
    g->debugpanel.y = 0;
    g->lock = 0;
    g->targetwidth = -1;
    g->targetheight = -1;
    g->windowwidth = -1;
    g->windowheight = -1;
    g->timebegan = time(NULL);
    g->currenttime = time(NULL);
    g->timebegantm = localtime(&(g->currenttime));
    g->currenttimetm = localtime(&(g->currenttime));
    bzero(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF);
    bzero(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF);
    strftime(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF, "Start Time:   %Y-%m-%d %H:%M:%S", g->timebegantm);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
    g->debugpanelon = false;
    g->cam_lockon = true;
    g->player_input_received = false;
    g->is_locked = false;
    g->gridon = false;
    g->processing_actions = false;
    g->cam2d.target = (Vector2){0, 0};
    g->cam2d.offset = (Vector2){0, 0};
    g->cam2d.zoom = 1.0f;
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

    //g->controlmode = CONTROLMODE_PLAYER;
    g->controlmode = CONTROLMODE_CAMERA;

    g->fadestate = FADESTATENONE;
    //g->spritegroups = NULL;
    g->hero_id = -1;
    g->dungeon_floor = NULL;
    g->entitymap = NULL;

    g->entityids = NULL;
    g->index_entityids = 0;
    g->max_entityids = -1;

    return g;
}


// have to update this function when we introduce new fields to Gamestate
void gamestatefree(gamestate* g) {
    if (g) {
        minfo("Freeing gamestate");
        //minfo("Freeing hashtable");
        //hashtable_entityid_spritegroup_destroy(g->spritegroups);
        minfo("Freeing dungeon");
        dungeon_floor_free(g->dungeon_floor);
        minfo("Freeing em");
        em_free(g->entitymap);
        minfo("Freeing entityids");
        free(g->entityids);
        //free(g->timebegantm);
        //free(g->currenttimetm);
        minfo("Freeing g...");
        free(g);
        msuccess("Freed gamestate");
    }
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
