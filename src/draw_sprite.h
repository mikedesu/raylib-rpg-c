
#pragma once

#include "draw_entity_sprite.h"
#include "draw_shield_sprite.h"
#include "draw_weapon_sprite.h"
#include "gamestate.h"
#include "spritegroup.h"

extern unordered_map<entityid, spritegroup*> spritegroups;

static inline void draw_sprite_and_shadow(gamestate& g, entityid id) {
    //massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(spritegroups.find(id) != spritegroups.end(), "NO SPRITE GROUP FOR ID %d", id);
    //if (spritegroups.find(id) == spritegroups.end()) {
    //    merror("NO SPRITE GROUP FOR ID %d", id);
    //    return;
    //}
    spritegroup* sg = spritegroups[id];
    massert(sg, "sg is NULL");
    // Draw components in correct order
    //draw_shadow_for_entity(g, sg, id);
    draw_shield_sprite_back(g, id, sg);
    draw_weapon_sprite_back(g, id, sg);
    draw_entity_sprite(g, sg);
    draw_shield_sprite_front(g, id, sg);
    draw_weapon_sprite_front(g, id, sg);
}
