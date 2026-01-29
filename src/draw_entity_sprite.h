#pragma once

#include "gamestate.h"
#include "spritegroup.h"

static inline void draw_entity_sprite(gamestate& g, spritegroup* sg) {
    massert(sg, "spritegroup is NULL");

    Rectangle dest = {sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};
    massert(dest.width > 0, "dest.width is 0");
    massert(dest.height > 0, "dest.height is 0");
    //if (dest.width == 0 || dest.height == 0) {
    //    merror("ERROR: DEST WIDTH OR HEIGHT IS ZERO");
    //    return;
    //}

    unsigned char a = g.ct.get<txalpha>(sg->id).value_or(255);
    const Color c = {255, 255, 255, a};
    shared_ptr<sprite> s = sg->get_current();
    massert(s, "sprite is NULL");
    //if (sg->visible)
    DrawTexturePro(*s->get_texture(), s->get_src(), dest, Vector2{0, 0}, 0, c);
#ifdef ENTITY_BORDER
    // draw a box around the sprite
    DrawRectangleLinesEx(dest, 1, Color{255, 0, 0, 128});
#endif
}
