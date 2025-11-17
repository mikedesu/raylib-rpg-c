#include "liblogic_change_player_dir.h"

void change_player_dir(shared_ptr<gamestate> g, direction_t dir) {
    massert(g, "Game state is NULL!");
    if (g->ct.get<dead>(g->hero_id).value_or(true)) {
        return;
    }
    g->ct.set<direction>(g->hero_id, dir);
    g->ct.set<update>(g->hero_id, true);
    g->player_changing_dir = false;
    g->frame_dirty = true;
}
