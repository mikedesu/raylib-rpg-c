#pragma once

#include "ComponentTraits.h"
#include "add_message.h"
#include "check_hearing.h"
#include "gamestate.h"
#include "get_entity_location.h"
#include "get_entity_name.h"
#include "sfx.h"

static inline void handle_weapon_durability_loss(gamestate& g, const entityid atk_id, const entityid tgt_id) {
    const entityid equipped_wpn = g.ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);
    auto maybe_dura = g.ct.get<durability>(equipped_wpn);
    if (!maybe_dura.has_value()) {
        return;
    }
    const int dura = maybe_dura.value();
    g.ct.set<durability>(equipped_wpn, dura - 1 < 0 ? 0 : dura - 1);
    if (dura > 0)
        return;
    // unequip item
    g.ct.set<equipped_weapon>(atk_id, ENTITYID_INVALID);
    // remove item from attacker's inventory
    g.remove_from_inventory(atk_id, equipped_wpn);
    // item destroyed
    g.ct.set<destroyed>(equipped_wpn, true);
    const bool event_heard = check_hearing(g, g.hero_id, get_entity_location(g, tgt_id));
    if (event_heard)
        PlaySound(g.sfx[SFX_05_ALCHEMY_GLASS_BREAK]);
    add_message_history(g, "%s broke!", get_entity_name(g, equipped_wpn).c_str());
}


static inline void handle_shield_durability_loss(gamestate& g, const entityid defender, const entityid attacker) {
    const entityid shield = g.ct.get<equipped_shield>(defender).value_or(ENTITYID_INVALID);
    auto maybe_dura = g.ct.get<durability>(shield);
    if (!maybe_dura.has_value())
        return;

    const int dura = maybe_dura.value();
    g.ct.set<durability>(shield, dura - 1 < 0 ? 0 : dura - 1);
    if (dura > 0)
        return;
    // unequip item
    g.ct.set<equipped_shield>(defender, ENTITYID_INVALID);
    // remove item from attacker's inventory
    g.remove_from_inventory(defender, shield);
    // item destroyed
    g.ct.set<destroyed>(shield, true);
    const bool event_heard = check_hearing(g, g.hero_id, get_entity_location(g, defender));
    if (event_heard)
        PlaySound(g.sfx[SFX_05_ALCHEMY_GLASS_BREAK]);

    add_message_history(g, "%s broke!", get_entity_name(g, shield).c_str());
}
