#include "liblogic.h"
#include "em.h"
#include "entity.h"
#include "mprint.h"
#include <stdlib.h>
#include <string.h>

static entityid next_entityid = 0; // Start at 0, increment for each new entity


void liblogic_init(gamestate* const g) {
    if (!g) {
        merror("liblogic_init: gamestate is NULL");
        return;
    }
    g->entitymap = em_new();
    entityid hero_id = liblogic_entity_create(g, ENTITY_PLAYER, 0, 0, "hero");
    if (hero_id != -1) {
        g->hero_id = hero_id;
        msuccessint("Logic Init! Hero ID: ", g->hero_id);
    } else {
        merror("Logic Init: failed to init hero");
    }
}


void liblogic_tick(const inputstate* const is, gamestate* const g) {
    if (inputstate_is_held(is, KEY_RIGHT)) {
        entity* hero = em_get(g->entitymap, g->hero_id);
        if (hero) {
            entity_incr_x(hero);
        } else {
            merror("Hero not found!");
        }
    }
}


void liblogic_close(gamestate* const g) {
    if (!g) {
        merror("liblogic_close: gamestate is NULL");
        return;
    }
    if (g->entitymap) {
        em_free(g->entitymap);
        g->entitymap = NULL;
    }
    msuccess("Logic Close!");
}


void liblogic_add_entityid(gamestate* const g, entityid id) {
    if (!g) {
        merror("liblogic_add_entityid: gamestate is NULL");
        return;
    }
    if (g->index_entityids >= g->max_entityids) {
        int new_max = g->max_entityids * 2;
        entityid* new_ids = realloc(g->entityids, new_max * sizeof(entityid));
        if (!new_ids) {
            merror("liblogic_add_entityid: realloc failed");
            return;
        }
        g->entityids = new_ids;
        g->max_entityids = new_max;
        msuccessint("Expanded entityids to ", new_max);
    }
    g->entityids[g->index_entityids++] = id;
    msuccessint("Added entity ID: ", id);
}


entityid liblogic_entity_create(gamestate* const g, entitytype_t type, int x, int y, const char* name) {
    if (!g || !g->entitymap) {
        merror("liblogic_entity_create: gamestate or entitymap is NULL");
        return -1;
    }
    if (!name || !name[0]) {
        merror("liblogic_entity_create: name is NULL or empty");
        return -1;
    }
    entity* e = entity_new_at(next_entityid++, type, x, y); // Assuming entity_new_at takes name next
    if (!e) {
        merror("liblogic_entity_create: failed to create entity");
        return -1;
    }
    // Assuming entity struct has a name field—copy it
    strncpy(e->name, name, ENTITY_NAME_LEN_MAX - 1); // Adjust ENTITY_NAME_MAXLEN if defined elsewhere
    e->name[ENTITY_NAME_LEN_MAX - 1] = '\0'; // Ensure null-terminated
    em_add(g->entitymap, e);
    liblogic_add_entityid(g, e->id);
    msuccessint2("Created entity at", x, y);
    return e->id;
}
