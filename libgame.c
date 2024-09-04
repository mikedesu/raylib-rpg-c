#include "gamestate.h"
#include "mprint.h"


void updategamestate(gamestate* state);


void updategamestate(gamestate* state) {
    // for right now, we will just update the frame count
    if(state) {
        // if(state->framecount % 60 == 0) {
        //     mprint("test");
        // }
        state->framecount++;

        // we can update these while the game is running to re-position the debug text
        state->debugx = 0;
        state->debugy = 0;
        state->fontsize = 20;
    }
}
