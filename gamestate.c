#include "gamestate.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// have to update this function when we introduce new fields to Gamestate
gamestate* gamestate_init() {
    gamestate* g = malloc(sizeof(gamestate));
    if(g == NULL) {
        fprintf(stderr, "Failed to allocate memory for gamestate: %s\n", strerror(errno));
        return NULL;
    }
    g->framecount = 0;
    return g;
}

// have to update this function when we introduce new fields to Gamestate
void gamestate_destroy(gamestate* state) {
    if(state == NULL) {
        fprintf(stderr, "Gamestate_destroy: gamestate is NULL\n");
        return;
    }
    free(state);
}
