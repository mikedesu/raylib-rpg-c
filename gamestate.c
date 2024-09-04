#include "gamestate.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// have to update this function when we introduce new fields to Gamestate
gamestate* gamestateinit() {
    gamestate* g = (gamestate*)malloc(GAMESTATESIZE);
    if(g == NULL) {
        fprintf(stderr, "Failed to allocate memory for gamestate: %s\n", strerror(errno));
        return NULL;
    }
    g->framecount = 0;
    g->debugx = 0;
    g->debugy = 0;
    g->fontsize = 20;
    bzero(g->debugtxtbfr, 256);
    return g;
}

// have to update this function when we introduce new fields to Gamestate
void gamestatefree(gamestate* state) {
    if(state == NULL) {
        fprintf(stderr, "Gamestate_destroy: gamestate is NULL\n");
        return;
    }
    free(state);
}
