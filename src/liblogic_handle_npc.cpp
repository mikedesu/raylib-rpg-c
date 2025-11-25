#include "liblogic_handle_npc.h"
#include "liblogic_is_entity_adjacent.h"
#include "liblogic_try_entity_attack.h"
#include "liblogic_try_entity_move.h"

void handle_npc(shared_ptr<gamestate> g, entityid id) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity is NULL!");

    minfo("handle_npc: %d", id);

    if (id == g->hero_id) {
        merror("handle_npc: id is equal to hero_id");
        return;
    }

    if (g->ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_NPC) {
        //minfo("Handling NPC %d", id);

        if (g->ct.get<dead>(id).has_value()) {
            bool is_dead = g->ct.get<dead>(id).value();

            if (!is_dead) {
                entityid target_id = g->ct.get<target>(id).value_or(g->hero_id);
                if (is_entity_adjacent(g, id, target_id)) {
                    // if id is adjacent to its target or the hero
                    vec3 loc = g->ct.get<location>(target_id).value();
                    try_entity_attack(g, id, loc.x, loc.y);
                } else {
                    // else, randomly move
                    try_entity_move(g, id, (vec3){rand() % 3 - 1, rand() % 3 - 1, 0});
                }
            }
        }
        //execute_action(g, id, g_get_default_action(g, id));
    }
}
