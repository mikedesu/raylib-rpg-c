#include "gamestate.h"
#include "mprint.h"

#include <string.h>
#include <time.h>

void updategamestate(gamestate* state);


void updategamestate(gamestate* g) {
    // for right now, we will just update the frame count
    if(g) {
        g->framecount++;
        // we can update these while the game is running to re-position the debug text
        //memset(g->debugtxtbfr, 0, 256);
        //snprintf(g->debugtxtbfr, 256, "evildojo %d", g->framecount);

        memset(g->dp.bfr, 0, 256);
        snprintf(g->dp.bfr,
                 256,
                 "framecount %d\ntime: %ld\nfuck god\nhail satan\n",
                 //"hail satan\n666\npraise the devil");
                 g->framecount,
                 time(NULL));

        g->dp.x = 10;
        g->dp.y = 10;

        //g->debugx = 10;
        //g->debugy = 10;
        //g->fontsize = 20;
    }
}
