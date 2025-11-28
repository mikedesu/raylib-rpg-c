#include "libdraw_entity_sprite.h"
#include "libdraw_shield_sprite.h"
#include "libdraw_sprite.h"
#include "libdraw_weapon_sprite.h"
#include "spritegroup.h"

extern unordered_map<entityid, spritegroup_t*> spritegroups;

void draw_sprite_and_shadow(const shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (spritegroups.find(id) == spritegroups.end()) {
        return;
    }
    spritegroup_t* sg = spritegroups[id];
    // old
    //entitytype_t type = g_get_type(g, id);
    //massert(sg, "spritegroup is NULL: id %d type: %s", id, entitytype_to_string(type));
    // Draw components in correct order
    //draw_shadow_for_entity(g, sg, id);
    draw_shield_sprite_back(g, id, sg);
    draw_weapon_sprite_back(g, id, sg);
    draw_entity_sprite(g, sg);
    draw_shield_sprite_front(g, id, sg);
    draw_weapon_sprite_front(g, id, sg);
}
