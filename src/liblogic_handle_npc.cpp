#include "liblogic_handle_npc.h"
#include "liblogic_is_entity_adjacent.h"
#include "try_entity_attack.h"
#include "try_entity_move.h"

void handle_npc(shared_ptr<gamestate> g, entityid id) {
    massert(g, "Game state is NULL!");
    massert(id != ENTITYID_INVALID, "Entity is NULL!");

    minfo2("handle_npc: %d", id);

    if (id == 0) {
        return;
    }

    if (id == g->hero_id) {
        merror("handle_npc: id is equal to hero_id");
        return;
    }

    auto maybe_type = g->ct.get<entitytype>(id);
    if (!maybe_type.has_value()) {
        merror("entity lacks type!: id = %d", id);
        return;
    }

    entitytype_t type = maybe_type.value();
    if (type != ENTITY_NPC) {
        return;
    }

    auto maybe_dead = g->ct.get<dead>(id);
    if (!maybe_dead.has_value()) {
        return;
    }

    const bool is_dead = maybe_dead.value();
    if (is_dead) {
        return;
    }

    // this is a heuristic for handling entity actions
    // originally, we were just moving randomly
    // this example shows how, if the player is not adjacent to an NPC,
    // they will just move randomly. otherwise, they attack the player
    const entityid target_id = g->ct.get<target>(id).value_or(g->hero_id);
    if (is_entity_adjacent(g, id, target_id)) {
        // if id is adjacent to its target or the hero
        vec3 loc = g->ct.get<location>(target_id).value();
        try_entity_attack(g, id, loc.x, loc.y);
    } else {
        // else, randomly move
        try_entity_move(g, id, (vec3){rand() % 3 - 1, rand() % 3 - 1, 0});
    }
}
