#include "dungeonfloor.h"
#include "gamestate.h"
#include "mprint.h"
#include <errno.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// have to update this function when we introduce new fields to Gamestate
gamestate* gamestateinitptr(const int windowwidth, const int windowheight, const int targetwidth, const int targetheight) {
    //mprint("gamestateinitptr begin\n");
    gamestate* g = (gamestate*)malloc(sizeof(gamestate));
    if (g == NULL) {
        fprintf(stderr, "Failed to allocate memory for gamestate: %s\n", strerror(errno));
        return NULL;
    }
    g->framecount = 0;
    g->timebegan = time(NULL);
    g->timebegantm = localtime(&(g->timebegan));
    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&(g->currenttime));
    bzero(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF);
    strftime(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF, "Start Time:   %Y-%m-%d %H:%M:%S", g->timebegantm);
    bzero(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
    gamestate_update_current_time(g);
    g->debugpanelon = false;
    g->gridon = false;
    //g->debugpanelon = true;
    g->debugpanel.x = 0;
    g->debugpanel.y = 0;
    g->display.targetwidth = targetwidth;
    g->display.targetheight = targetheight;
    g->display.windowwidth = windowwidth;
    g->display.windowheight = windowheight;
    g->cam2d.target = (Vector2){0, 0};
    g->cam2d.offset = (Vector2){0, 0};
    //g->cam2d.offset = (Vector2){targetwidth / 2.0f, targetheight / 2.0f};
    //g->cam2d.offset = (Vector2){targetwidth / 8.0f, targetheight / 4.0f};
    //g->cam2d.offset = (Vector2){targetwidth / 2.0f, targetheight / 2.0f};
    //g->cam2d.offset = (Vector2){3 * targetwidth / 4.0f, 3 * targetheight / 4.0f};
    g->cam2d.zoom = 4.0;
    g->cam2d.rotation = 0.0;
    g->cam_lockon = true;
    g->do_one_rotation = false;
    //g->controlmode = CONTROLMODE_CAMERA;
    g->controlmode = CONTROLMODE_PLAYER;
    g->fadealpha = 0.0f;
    g->fadestate = FADESTATENONE;
    g->entityids = vectorentityid_new();
    //g->entities = hashtable_entityid_entity_create(1000);
    g->spritegroups = NULL;
    //g->hero_id = -1;
    //g->torch_id = -1;
    g->dungeonfloor = NULL;
    g->player_input_received = false;
    //g->is_updating_smooth_move = false;
    //g->smooth_move_index = 0;
    //mprint("gamestateinitptr end\n");
    g->is_locked = false;
    g->lock_timer = 0;
    return g;
}


void gamestate_update_current_time(gamestate* g) {
    if (g) {
        g->currenttime = time(NULL);
        g->currenttimetm = localtime(&(g->currenttime));
        bzero(g->currenttimebuf, 64);
        strftime(g->currenttimebuf, 64, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
    }
}


// have to update this function when we introduce new fields to Gamestate
void gamestatefree(gamestate* g) {
    minfo("gamestatefree begin");
    if (g != NULL) {
        //companyscenefree(s->cs);
        //free(s->timebegantm);
        //minfo("gamestatefree freeing entities");
        //hashtable_entityid_entity_destroy(g->entities);
        minfo("gamestatefree freeing spritegroups");
        hashtable_entityid_spritegroup_destroy(g->spritegroups);
        minfo("gamestatefree freeing entityids");
        vectorentityid_destroy(&g->entityids);
        minfo("gamestatefree freeing dungeonfloor");
        dungeonfloor_free(g->dungeonfloor);
        minfo("gamestatefree freeing gamestate");
        free(g);
    }
    minfo("gamestatefree end");
}
