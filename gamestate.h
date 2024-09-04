#pragma once

typedef struct gamestate {
    unsigned int framecount;
    unsigned int fontsize;
    unsigned int debugx;
    unsigned int debugy;
    char debugtxtbfr[256];

} gamestate;

#define GAMESTATESIZE (sizeof(unsigned int) * 4 + sizeof(char) * 256)

gamestate* gamestateinit();
void gamestatefree(gamestate* gamestate);
