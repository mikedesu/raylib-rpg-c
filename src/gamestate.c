//#include "dungeonfloor.h"
//#include <errno.h>
//#include "mprint.h"
#include "gamestate.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// have to update this function when we introduce new fields to Gamestate
//gamestate* gamestateinitptr(const int windowwidth, const int windowheight, const int targetwidth, const int targetheight) {
gamestate* gamestateinitptr() {
    //mprint("gamestateinitptr begin\n");
    gamestate* g = (gamestate*)malloc(sizeof(gamestate));
    if (g == NULL) return NULL;
    g->framecount = g->debugpanel.x = g->debugpanel.y = g->lock = 0;
    g->targetwidth = g->targetheight = g->windowwidth = g->windowheight = -1;
    g->timebegan = g->currenttime = time(NULL);
    g->timebegantm = g->currenttimetm = localtime(&(g->currenttime));
    bzero(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF);
    bzero(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF);
    strftime(g->timebeganbuf, GAMESTATE_SIZEOFTIMEBUF, "Start Time:   %Y-%m-%d %H:%M:%S", g->timebegantm);
    strftime(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
    //gamestate_update_current_time(g);

    //bzero(g->currenttimebuf, 64);
    //strftime(g->currenttimebuf, 64, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);

    g->debugpanelon = g->cam_lockon = true;
    g->player_input_received = g->is_locked = g->gridon = g->processing_actions = false;

    //printf("\033[31;1mtargetwidth: %d\033[0m\n", targetwidth);
    //printf("\033[31;1mtargetheight: %d\033[0m\n", targetheight);
    //printf("\033[31;1mwindowwidth: %d\033[0m\n", windowwidth);
    //printf("\033[31;1mwindowheight: %d\033[0m\n", windowheight);
    //g->cam2d.target = (Vector2){0, 0}, g->cam2d.offset = (Vector2){0, 0}, g->cam2d.zoom = 4.0, g->cam2d.rotation = 0.0, g->lock_timer = 0, g->fadealpha = 0.0f;
    g->cam2d.target = (Vector2){0, 0}, g->cam2d.offset = (Vector2){0, 0}, g->cam2d.zoom = 7.0, g->cam2d.rotation = 0.0, g->fadealpha = 0.0f;
    //g->cam_lockon = true, g->do_one_rotation = false, g->controlmode = CONTROLMODE_PLAYER, g->fadestate = FADESTATENONE, g->spritegroups = NULL, g->player_input_received = false, g->is_locked = false;
    g->controlmode = CONTROLMODE_PLAYER, g->fadestate = FADESTATENONE, g->spritegroups = NULL;
    return g;
}




//inline void gamestate_update_current_time(gamestate* const g) {
//    if (!g)
//        return;
//    g->currenttime = time(NULL);
//    g->currenttimetm = localtime(&(g->currenttime));
//    bzero(g->currenttimebuf, 64);
//    strftime(g->currenttimebuf, 64, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
//}




// have to update this function when we introduce new fields to Gamestate
void gamestatefree(gamestate* g) {
    if (!g) return;
    //minfo("gamestatefree begin");
    //minfo("gamestatefree freeing spritegroups");
    hashtable_entityid_spritegroup_destroy(g->spritegroups);
    //minfo("gamestatefree freeing dungeonfloor");
    //if (g->dungeonfloor) {
    //    dungeonfloor_free(g->dungeonfloor);
    //}
    //minfo("gamestatefree freeing gamestate");
    free(g);
    //minfo("gamestatefree end");
}
