#include "ComponentTraits.h"
#include "entitytype.h"
#include "gamestate.h"

entityid tile_has_box(shared_ptr<gamestate> g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    massert(z >= 0, "floor is out of bounds");
    massert((size_t)z < g->dungeon->floors->size(), "floor is out of bounds");
    shared_ptr<dungeon_floor_t> df = d_get_floor(g->dungeon, z);
    shared_ptr<tile_t> t = df_tile_at(df, (vec3){x, y, z});
    for (int i = 0; (size_t)i < t->entities->size(); i++) {
        entityid id = tile_get_entity(t, i);
        entitytype_t type = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (id != ENTITYID_INVALID && type == ENTITY_WOODEN_BOX) {
            return id;
        }
    }
    return ENTITYID_INVALID;
}
