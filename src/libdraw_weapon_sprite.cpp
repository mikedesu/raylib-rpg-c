#include "libdraw_get_weapon_sprite.h"
#include "libdraw_weapon_sprite.h"

void draw_weapon_sprite_back(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    shared_ptr<sprite> weapon_back_s = get_weapon_back_sprite(g, id, sg);
    if (weapon_back_s) {
        DrawTexturePro(*weapon_back_s->texture, weapon_back_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
    }
}


void draw_weapon_sprite_front(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    shared_ptr<sprite> weapon_front_s = get_weapon_front_sprite(g, id, sg);
    if (weapon_front_s) {
        DrawTexturePro(*weapon_front_s->texture, weapon_front_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
    }
}
