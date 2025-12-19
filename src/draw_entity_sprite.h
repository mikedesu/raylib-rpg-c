#pragma once

#include "gamestate.h"
#include "spritegroup.h"

static inline void draw_entity_sprite(const shared_ptr<gamestate> g, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(sg, "spritegroup is NULL");
    Rectangle dest = {sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};
    if (dest.width == 0 || dest.height == 0) {
        merror("ERROR: DEST WIDTH OR HEIGHT IS ZERO");
        return;
    }
    unsigned char a = g->ct.get<txalpha>(sg->id).value_or(255);
    const Color c = {255, 255, 255, a};
    auto s = sg_get_current(sg);
    massert(s, "sprite is NULL");
    if (sg->visible)
        DrawTexturePro(*s->texture, s->src, dest, (Vector2){0, 0}, 0, c);
    // draw a box around the sprite
    //DrawRectangleLinesEx(dest, 1, (Color){255, 0, 0, 255});
}
