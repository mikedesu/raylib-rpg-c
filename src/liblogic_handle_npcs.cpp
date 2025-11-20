
#include "liblogic_handle_npc.h"
#include "liblogic_handle_npcs.h"
void handle_npcs(shared_ptr<gamestate> g) {
    if (g->flag == GAMESTATE_FLAG_NPC_TURN) {
        for (entityid id = 0; id < g->next_entityid; id++) {
            handle_npc(g, id);
        }
        // After processing all NPCs, set the flag to animate all movements together
        g->flag = GAMESTATE_FLAG_NPC_ANIM;
    }
}
