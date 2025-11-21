#include "liblogic_handle_npc.h"
#include "liblogic_try_entity_attack.h"
#include "liblogic_try_entity_move.h"

void handle_npc(shared_ptr<gamestate> g, entityid id) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity is NULL!");
    if (id == g->hero_id) {
        //merror("Tried to handle hero id %d as NPC! This should not happen.", id);
        return;
    }
    if (g->ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_NPC) {
        //minfo("Handling NPC %d", id);
        if (g->ct.get<dead>(id).has_value()) {
            bool is_dead = g->ct.get<dead>(id).value();
            if (!is_dead) {
                // test first by attacking to the left
                //vec3 loc = g->ct.get<location>(id).value();
                //try_entity_attack(g, id, loc.x - 1, loc.y);

                // else, randomly move
                try_entity_move(g, id, (vec3){rand() % 3 - 1, rand() % 3 - 1, 0});
            }
        }
        //execute_action(g, id, g_get_default_action(g, id));
    }
}
