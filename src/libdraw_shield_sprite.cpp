#include "libdraw_get_shield_sprite.h"
#include "libdraw_shield_sprite.h"

void draw_shield_sprite_back(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    shared_ptr<sprite> shield_back_s = get_shield_back_sprite(g, id, sg);
    if (shield_back_s) DrawTexturePro(*shield_back_s->texture, shield_back_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
}


void draw_shield_sprite_front(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    shared_ptr<sprite> shield_front_s = get_shield_front_sprite(g, id, sg);
    if (shield_front_s) DrawTexturePro(*shield_front_s->texture, shield_front_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
}
