#include "gamestate.h"
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

    //g->cam2d.zoom = DEFAULT_CAMERA_ZOOM;
    g->cam2d.zoom = 20.0f;

    g->cam2d.rotation = 0.0;

    g->fadealpha = 0.0f;
    g->controlmode = CONTROLMODE_PLAYER;
    g->fadestate = FADESTATENONE;
    g->spritegroups = NULL;
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
    if (!g) {
#ifdef DEBUG
        merror("gamestatefree g is NULL");
#endif
        return;
    }
    hashtable_entityid_spritegroup_destroy(g->spritegroups);
    dungeon_floor_free(g->dungeon_floor);
    em_free(g->entitymap);
    free(g->entityids);
    //free(g->timebegantm);
    //free(g->currenttimetm);
    free(g);
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
