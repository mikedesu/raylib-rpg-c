#include "libdraw_create_sg_byid.h"
#include "libdraw_handle_dirty_entities.h"

void libdraw_handle_dirty_entities(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    minfo2("Begin handle dirty entities");
    if (g->dirty_entities) {
        for (entityid i = g->new_entityid_begin; i < g->new_entityid_end; i++) {
            create_sg_byid(g, i);
        }
        g->dirty_entities = false;
        g->new_entityid_begin = ENTITYID_INVALID;
        g->new_entityid_end = ENTITYID_INVALID;
        g->frame_dirty = true;
    }
    minfo2("End handle dirty entities");
}
