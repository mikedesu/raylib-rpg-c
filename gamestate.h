#pragma once

typedef struct gamestate {
    unsigned int framecount;
    // const char *s = "evildojo";
} gamestate;

gamestate* gamestate_init();
// void Gamestate_update(Gamestate *gamestate);
void gamestate_destroy(gamestate* gamestate);
