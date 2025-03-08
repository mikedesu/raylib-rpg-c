#include "liblogic.h"
#include "em.h"
#include "entity.h"
#include "mprint.h"

void liblogic_init(gamestate* const g) {

    g->entitymap = em_new();
    // have to create a new entity for adding to entitymap

    entity* e = entity_new_at(0, ENTITY_PLAYER, 0, 0);
    if (e) {
        em_add(g->entitymap, e);
        g->hero_id = e->id;
        msuccessint("Logic Init! Hero ID: ", g->hero_id);
    } else {
        merror("Failed to create entity");
    }
}

void liblogic_tick(const inputstate* const is, gamestate* const g) {
    if (inputstate_is_held(is, KEY_RIGHT)) {
        g->hero_id += 1; // Dummy movement
        msuccess("Hero moved right!");
    }
    //msuccess("Logic Tick! v1");
}


void liblogic_close(gamestate* const g) {
    em_free(g->entitymap);
    msuccess("Logic Close!");
}
