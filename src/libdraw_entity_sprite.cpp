#include "libdraw_entity_sprite.h"

void draw_entity_sprite(const shared_ptr<gamestate> g, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(sg, "spritegroup is NULL");

    //auto maybe_loc = g->ct.get<location>(sg->id);
    //if (!maybe_loc.has_value()) {
    //    merror("entity id %d lacks location", sg->id);
    //    return;
    //}
    //auto loc = maybe_loc.value();

    //Rectangle dest = {sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};

    Rectangle dest = {sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};
    if (dest.width == 0 || dest.height == 0) {
        merror("ERROR: DEST WIDTH OR HEIGHT IS ZERO");
    }

    unsigned char a = g->ct.get<txalpha>(sg->id).value_or(255);

    shared_ptr<sprite> s = sg_get_current(sg);
    massert(s, "sprite is NULL");

    DrawTexturePro(*s->texture, s->src, dest, (Vector2){0, 0}, 0, (Color){255, 255, 255, a});
    // draw a box around the sprite
    //DrawRectangleLinesEx(dest, 1, (Color){255, 0, 0, 255});
}
