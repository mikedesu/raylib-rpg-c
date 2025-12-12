#pragma once

#include "gamestate.h"
#include "gamestate_inventory.h"

static inline bool drop_item_from_hero_inventory(shared_ptr<gamestate> g) {
    if (!g->ct.has<inventory>(g->hero_id))
        return false;

    size_t index = g->inventory_cursor.y * 7 + g->inventory_cursor.x;
    auto maybe_inventory = g->ct.get<inventory>(g->hero_id);
    if (!maybe_inventory.has_value())
        return false;

    auto inventory = maybe_inventory.value();
    if (index < 0 || index >= inventory->size())
        return false;

    entityid item_id = inventory->at(index);
    inventory->erase(inventory->begin() + index);

    if (item_id == g->ct.get<equipped_weapon>(g->hero_id).value_or(ENTITYID_INVALID))
        g->ct.set<equipped_weapon>(g->hero_id, ENTITYID_INVALID);

    // add it to the tile at the player's current location
    // get the player's location
    vec3 loc = g->ct.get<location>(g->hero_id).value();
    // get the tile at the player's location
    //auto tile = df_tile_at(g->dungeon->floors->at(loc.z), loc);
    //if (!tile)
    //    return false;
    auto df = d_get_current_floor(g->dungeon);

    if (!df_add_at(df, item_id, loc.x, loc.y)) {
        merror("Failed to add to %d, %d, %d", loc.x, loc.y, loc.z);
        return false;
    }

    // update the entity's location
    g->ct.set<location>(item_id, loc);
    // add to tile

    return true;
}
