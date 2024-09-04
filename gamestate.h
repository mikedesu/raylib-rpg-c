#pragma once

typedef struct gamestate {
    unsigned int framecount;
} gamestate;

gamestate* gamestate_init();
void gamestate_destroy(gamestate* gamestate);
