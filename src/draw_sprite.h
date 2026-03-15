
/** @file draw_sprite.h
 *  @brief Composite entity/equipment sprite drawing helper.
 */

#pragma once

#include "draw_entity_sprite.h"
#include "draw_shield_sprite.h"
#include "draw_weapon_sprite.h"
#include "gamestate.h"
#include "libdraw_context.h"
#include "spritegroup.h"

/** @brief Draw an entity sprite together with any equipped shield and weapon layers. */
static inline void draw_sprite_and_shadow(gamestate& g, entityid id) {
    //massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(libdraw_ctx.spritegroups.find(id) != libdraw_ctx.spritegroups.end(), "NO SPRITE GROUP FOR ID %d", id);
    //if (libdraw_ctx.spritegroups.find(id) == libdraw_ctx.spritegroups.end()) {
    //    merror("NO SPRITE GROUP FOR ID %d", id);
    //    return;
    //}
    spritegroup* sg = libdraw_ctx.spritegroups[id];
    massert(sg, "sg is NULL");
    // Draw components in correct order
    //draw_shadow_for_entity(g, sg, id);
    draw_shield_sprite_back(g, id, sg);
    draw_weapon_sprite_back(g, id, sg);
    draw_entity_sprite(g, sg);
    draw_shield_sprite_front(g, id, sg);
    draw_weapon_sprite_front(g, id, sg);
}
