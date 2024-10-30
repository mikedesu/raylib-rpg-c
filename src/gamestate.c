//#include "dungeonfloor.h"
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
    bzero(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF);
    strftime(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF, "Start Time:   %Y-%m-%d %H:%M:%S", g->timebegantm);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
    gamestate_update_current_time(g);
    //g->debugpanelon = false;
    g->debugpanelon = true;
    g->gridon = false;
    g->debugpanel.x = 0;
    g->debugpanel.y = 0;
    g->display.targetwidth = targetwidth;
    g->display.targetheight = targetheight;
    g->display.windowwidth = windowwidth;
    g->display.windowheight = windowheight;
    g->cam2d.target = (Vector2){0, 0};
    g->cam2d.offset = (Vector2){0, 0};
    g->cam2d.zoom = 4.0;
    g->cam2d.rotation = 0.0;
    g->cam_lockon = true;
    g->do_one_rotation = false;
    g->controlmode = CONTROLMODE_PLAYER;
    g->fadealpha = 0.0f;
    g->fadestate = FADESTATENONE;
    g->spritegroups = NULL;
    g->player_input_received = false;
    g->is_locked = false;
    g->lock_timer = 0;
    return g;
}




void gamestate_update_current_time(gamestate* const g) {
    if (!g)
        return;
    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&(g->currenttime));
    bzero(g->currenttimebuf, 64);
    strftime(g->currenttimebuf, 64, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
}




// have to update this function when we introduce new fields to Gamestate
void gamestatefree(gamestate* g) {
    if (!g)
        return;
    minfo("gamestatefree begin");
    minfo("gamestatefree freeing spritegroups");
    hashtable_entityid_spritegroup_destroy(g->spritegroups);
    minfo("gamestatefree freeing dungeonfloor");
    //if (g->dungeonfloor) {
    //    dungeonfloor_free(g->dungeonfloor);
    //}
    minfo("gamestatefree freeing gamestate");
    free(g);
    minfo("gamestatefree end");
}
