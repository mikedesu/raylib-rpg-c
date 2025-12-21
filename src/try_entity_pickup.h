#pragma once

#include "add_message.h"
#include "entityid.h"
#include "gamestate.h"
#include "manage_inventory.h"
#include "play_sound.h"
#include "sfx.h"
#include "tile_get_item.h"

static inline bool try_entity_pickup(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "Entity is NULL!");
    g.ct.set<update>(id, true);

    // check if the player is on a tile with an item
    auto maybe_loc = g.ct.get<location>(id);
    if (!maybe_loc.has_value()) {
        merror("id %d has no location", id);
        return false;
    }

    const vec3 loc = maybe_loc.value();
    dungeon_floor_t& df = d_get_floor(g.dungeon, loc.z);
    tile_t& tile = df_tile_at(df, loc);

    bool item_picked_up = false;

    //auto item_id = ENTITYID_INVALID;
    //for (size_t i = 0; i < tile.entities->size(); i++) {
    //    entityid itemid = tile.entities->at(i);
    //    auto type = g.ct.get<entitytype>(itemid).value_or(ENTITY_NONE);
    //    if (type == ENTITY_ITEM) {
    //        if (add_to_inventory(g, id, itemid)) {
    //            tile_remove(tile, itemid);
    //            play_sound(SFX_CONFIRM_01);
    //            item_picked_up = true;
    //            item_id = itemid;
    //        }
    //        auto t = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
    //        if (t == ENTITY_PLAYER) {
    //            g.flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //        }
    //        break;
    //    }
    //}

    // lets try using our new cached_item via tile_get_item
    const entityid item_id = tile_get_item(g, tile);

    if (item_id != ENTITYID_INVALID && add_to_inventory(g, id, item_id)) {
        tile_remove(tile, item_id);
        play_sound(SFX_CONFIRM_01);
        item_picked_up = true;
        auto item_name = g.ct.get<name>(item_id).value_or("no-name-item");
        add_message_history(g, "You picked up %s", item_name.c_str());
    } else if (item_id == ENTITYID_INVALID) {
        merror("No item cached");
    }

    auto t = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
    if (t == ENTITY_PLAYER) {
        g.flag = GAMESTATE_FLAG_PLAYER_ANIM;
    }

    //if (item_picked_up) {
    //    auto item_name = g.ct.get<name>(item_id).value_or("no-name-item");
    //    add_message_history(g, "You picked up %s", item_name.c_str());
    //}

    return item_picked_up;
}
