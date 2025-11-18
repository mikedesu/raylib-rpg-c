#include "libdraw_player_target_box.h"
#include "libgame_defines.h"

bool libdraw_draw_player_target_box(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    entityid id = g->hero_id;
    if (id == -1) return false;
    direction_t dir = g->ct.get<direction>(id).value();

    //vec3 loc = g_get_loc(g, id);
    vec3 loc = g->ct.get<location>(id).value();

    float x = loc.x + get_x_from_dir(dir);
    float y = loc.y + get_y_from_dir(dir);
    float w = DEFAULT_TILE_SIZE;
    float h = DEFAULT_TILE_SIZE;
    //Color base_c = GREEN;
    float a = 0.5f;
    if (g->player_changing_dir) {
        a = 0.75f;
    }
    DrawRectangleLinesEx((Rectangle){x * w, y * h, w, h}, 1, Fade(GREEN, a));
    return true;
}
