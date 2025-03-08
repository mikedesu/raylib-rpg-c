#include "liblogic.h"
#include "mprint.h"

void liblogic_init(gamestate* const g) {
    msuccess("Logic Init!");
}

void liblogic_tick(const inputstate* const is, gamestate* const g) {
    if (inputstate_is_held(is, KEY_RIGHT)) {
        g->hero_id += 1; // Dummy movement
        msuccess("Hero moved right!");
    }
    msuccess("Logic Tick! v1");
}
