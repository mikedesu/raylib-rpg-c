#include "handle_npc.h"
#include "liblogic_handle_npcs.h"

void handle_npcs(shared_ptr<gamestate> g) {
    if (g->flag == GAMESTATE_FLAG_NPC_TURN) {
        if (g->entity_turn >= 0 && g->entity_turn < g->next_entityid) {
            handle_npc(g, g->entity_turn);
            g->flag = GAMESTATE_FLAG_NPC_ANIM;
        }
    }
}
