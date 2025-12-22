#pragma once

#include "add_message.h"
#include "check_hearing.h"
#include "gamestate.h"
#include "manage_inventory.h"
#include "sfx.h"

static inline void handle_weapon_durability_loss(gamestate& g, entityid atk_id, entityid tgt_id) {
    const entityid equipped_wpn = g.ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);
    auto maybe_dura = g.ct.get<durability>(equipped_wpn);
    if (maybe_dura.has_value()) {
        const int dura = maybe_dura.value();
        g.ct.set<durability>(equipped_wpn, dura - 1 < 0 ? 0 : dura - 1);
        if (dura == 0) {
            // unequip item
            g.ct.set<equipped_weapon>(atk_id, ENTITYID_INVALID);
            // remove item from attacker's inventory
            remove_from_inventory(g, atk_id, equipped_wpn);
            // item destroyed
            g.ct.set<destroyed>(equipped_wpn, true);
            const bool event_heard = check_hearing(g, g.hero_id, g.ct.get<location>(tgt_id).value_or((vec3){-1, -1, -1}));
            if (event_heard)
                PlaySound(g.sfx[SFX_05_ALCHEMY_GLASS_BREAK]);
            add_message_history(g, "%s broke!", g.ct.get<name>(equipped_wpn).value_or("no-name").c_str());
        }
    }
}
