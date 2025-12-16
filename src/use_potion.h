#pragma once


#include "entitytype.h"
#include "gamestate.h"
#include "manage_inventory.h"
#include "roll.h"

static inline bool use_potion(shared_ptr<gamestate> g, entityid actor_id, entityid item_id) {
    massert(actor_id != ENTITYID_INVALID, "actor_id is invalid");
    massert(item_id != ENTITYID_INVALID, "actor_id is invalid");

    const bool is_item = g->ct.get<entitytype>(item_id).value_or(ENTITY_NONE) == ENTITY_ITEM;
    const bool is_potion = g->ct.get<itemtype>(item_id).value_or(ITEM_NONE) == ITEM_POTION;
    const bool in_inventory = is_in_inventory(g, actor_id, item_id);

    if (is_item && is_potion && in_inventory) {
        // get the item's effects
        auto maybe_heal = g->ct.get<healing>(item_id);
        if (maybe_heal && maybe_heal.has_value()) {
            const auto heal = maybe_heal.value();
            const int amount = do_roll(heal);
            const int myhp = g->ct.get<hp>(actor_id).value_or(-1);
            const int mymaxhp = g->ct.get<maxhp>(actor_id).value_or(-1);
            g->ct.set<hp>(actor_id, mymaxhp ? mymaxhp : myhp + amount);
        }
        //====
        else {
            merror("Potion has no healing component");
            return false;
        }

        // consume the potion by removing it
        remove_from_inventory(g, actor_id, item_id);
        return true;
    }
    merror("id %d is not an item, potion, or isnt in the inventory", item_id);
    return false;
}
