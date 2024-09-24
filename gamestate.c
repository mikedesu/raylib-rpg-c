#include "gamestate.h"
#include "mprint.h"

#include <errno.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// have to update this function when we introduce new fields to Gamestate
gamestate* gamestateinitptr(const int windowwidth,
                            const int windowheight,
                            const int targetwidth,
                            const int targetheight) {
    mprint("gamestateinit\n");
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
    strftime(g->timebeganbuf,
             GAMESTATE_SIZEOFTIMEBUF,
             "Start Time:   %Y-%m-%d %H:%M:%S",
             g->timebegantm);
    bzero(g->currenttimebuf, GAMESTATE_SIZEOFTIMEBUF);
    strftime(g->currenttimebuf,
             GAMESTATE_SIZEOFTIMEBUF,
             "Current Time: %Y-%m-%d %H:%M:%S",
             g->currenttimetm);

    gamestateupdatecurrenttime(g);

    g->debugpanelon = true;
    g->debugpanel.x = 0;
    g->debugpanel.y = 0;

    g->display.targetwidth = targetwidth;
    g->display.targetheight = targetheight;
    g->display.windowwidth = windowwidth;
    g->display.windowheight = windowheight;

    g->cam2d.target = (Vector2){0, 0};
    g->cam2d.offset = (Vector2){targetwidth / 2.0f, targetheight / 2.0f};

    g->cam2d.zoom = 4.0;

    g->cam2d.rotation = 0.0;

    //g->controlmode = CONTROLMODE_CAMERA;
    g->controlmode = CONTROLMODE_PLAYER;

    g->fadealpha = 0.0f;
    g->fadestate = FADESTATENONE;

    g->entities = hashtable_entityid_entity_create(1000);

    return g;
}


void gamestateupdatecurrenttime(gamestate* g) {
    if (g == NULL) {
        fprintf(stderr, "gamestateupdatecurrenttime: gamestate is NULL\n");
        return;
    }

    g->currenttime = time(NULL);
    g->currenttimetm = localtime(&(g->currenttime));
    bzero(g->currenttimebuf, 64);
    strftime(g->currenttimebuf, 64, "Current Time: %Y-%m-%d %H:%M:%S", g->currenttimetm);
}


// have to update this function when we introduce new fields to Gamestate
void gamestatefree(gamestate* s) {
    if (s != NULL) {
        //companyscenefree(s->cs);
        //free(s->timebegantm);
        hashtable_entityid_entity_destroy(s->entities);
        free(s);
    }
}
