//#include "libgame.h"

#include "gamestate.h"
#include "mprint.h"

void updategamestate(gamestate* state);

void updategamestate(gamestate* state) {
    // for right now, we will just update the frame count

    if(state) {
        //if(state->framecount % 60 == 0) {
        //    mprint("test");
        //}

        state->framecount++;
    }
}
