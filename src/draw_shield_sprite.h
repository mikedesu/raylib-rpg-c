/** @file draw_shield_sprite.h
 *  @brief Equipped-shield draw helpers for front/back render layers.
 */

#pragma once

#include "gamestate.h"
#include "get_shield_sprite.h"
#include "spritegroup.h"

/** @brief Draw the equipped shield sprite behind the entity when applicable. */
static inline void draw_shield_sprite_back(gamestate& g, entityid id, spritegroup* sg) {
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    auto shield_back_s = get_shield_back_sprite(g, id, sg);
    if (shield_back_s) {
        DrawTexturePro(*shield_back_s->get_texture(), shield_back_s->get_src(), sg->dest, Vector2{0, 0}, 0, WHITE);
    }
    else {
        merror3("no shield back sprite");
    }
}

/** @brief Draw the equipped shield sprite in front of the entity when applicable. */
static inline void draw_shield_sprite_front(gamestate& g, entityid id, spritegroup* sg) {
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    auto shield_front_s = get_shield_front_sprite(g, id, sg);
    if (shield_front_s) {
        DrawTexturePro(*shield_front_s->get_texture(), shield_front_s->get_src(), sg->dest, Vector2{0, 0}, 0, WHITE);
    }
    else {
        merror3("no shield front sprite");
    }
}
