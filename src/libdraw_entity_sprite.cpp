#include "libdraw_entity_sprite.h"

void draw_entity_sprite(const shared_ptr<gamestate> g, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(sg, "spritegroup is NULL");
    //sprite* s = sg_get_current(sg);
    shared_ptr<sprite> s = sg_get_current(sg);
    massert(s, "sprite is NULL");
    Rectangle dest = {sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};
    //unsigned char a = 255;
    //unsigned char a = (unsigned char)g_get_tx_alpha(g, sg->id);
    unsigned char a = g->ct.get<txalpha>(sg->id).value_or(255);
    DrawTexturePro(*s->texture, s->src, dest, (Vector2){0, 0}, 0, (Color){255, 255, 255, a});
    // draw a box around the sprite
    //DrawRectangleLinesEx(dest, 1, (Color){255, 0, 0, 255});
}
