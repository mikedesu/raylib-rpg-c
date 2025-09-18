
#include "ComponentTraits.h"
#include "gamestate_equipped_weapon.h"
#include "liblogic_drop_item.h"

bool drop_item_from_hero_inventory(shared_ptr<gamestate> g) {
    size_t index = g->inventory_cursor.y * 7 + g->inventory_cursor.x;
    auto inventory = g_get_inventory(g, g->hero_id);
    if (index >= 0 && index < inventory->size()) {
        entityid item_id = inventory->at(index);
        // remove it from the player's inventory
        if (g_remove_from_inventory(g, g->hero_id, item_id)) {
            // if it was an equipped weapon, unequip it
            if (g_get_equipped_weapon(g, g->hero_id) == item_id) {
                g_unequip_weapon(g, g->hero_id, item_id);
            }


            // add it to the tile at the player's current location

            // get the player's location
            //vec3 loc = g_get_loc(g, g->hero_id);
            vec3 loc = g->ct.get<Location>(g->hero_id).value();

            // get the tile at the player's location
            auto tile = df_tile_at(g->dungeon->floors->at(loc.z), loc);
            if (tile) {
                // update the entity's location
                //g_update_loc(g, item_id, loc);
                g->ct.set<Location>(item_id, loc);
                // add to tile
                tile_add(tile, item_id);
                return true;
            }
        }
    }
    return false;
}
