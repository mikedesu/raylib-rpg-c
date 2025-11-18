#include "liblogic_drop_item.h"
#include "liblogic_handle_input_inventory.h"
#include "sfx.h"

void handle_input_inventory(shared_ptr<inputstate> is, shared_ptr<gamestate> g) {
    massert(is, "Input state is NULL!");
    massert(g, "Game state is NULL!");
    if (g->controlmode != CONTROLMODE_INVENTORY) {
        //merror("wrong mode");
        return;
    }
    if (!g->display_inventory_menu) {
        return;
    }
    //if (inputstate_is_pressed(is, KEY_ESCAPE) || inputstate_is_pressed(is, KEY_I)) {
    if (inputstate_is_pressed(is, KEY_I)) {
        g->controlmode = CONTROLMODE_PLAYER;
        g->display_inventory_menu = false;
        //g->display_gameplay_settings_menu = false;
        //g->display_sort_inventory_menu = false;
        PlaySound(g->sfx->at(SFX_BAG_CLOSE));
        return;
    }
    if (inputstate_is_pressed(is, KEY_LEFT)) {
        //PlaySound(g->sfx->at(SFX_CONFIRM_01));
        if (g->inventory_cursor.x > 0) {
            g->inventory_cursor.x--;
        }
    }
    if (inputstate_is_pressed(is, KEY_RIGHT)) {
        //PlaySound(g->sfx->at(SFX_CONFIRM_01));
        g->inventory_cursor.x++;
    }
    if (inputstate_is_pressed(is, KEY_UP)) {
        //PlaySound(g->sfx->at(SFX_CONFIRM_01));
        if (g->inventory_cursor.y > 0) {
            g->inventory_cursor.y--;
        }
    }
    if (inputstate_is_pressed(is, KEY_DOWN)) {
        //PlaySound(g->sfx->at(SFX_CONFIRM_01));
        g->inventory_cursor.y++;
    }
    if (inputstate_is_pressed(is, KEY_E)) {
        PlaySound(g->sfx->at(SFX_EQUIP_01));
        // equip item
        // get the item id of the current selection
        size_t index = g->inventory_cursor.y * 7 + g->inventory_cursor.x;

        optional<shared_ptr<vector<entityid>>> my_inventory = g->ct.get<inventory>(g->hero_id);
        if (my_inventory) {
            if (my_inventory.has_value()) {
                shared_ptr<vector<entityid>> unpacked_inventory = my_inventory.value();
                if (index >= 0 && index < unpacked_inventory->size()) {
                    entityid item_id = unpacked_inventory->at(index);
                    entitytype_t type = g->ct.get<entitytype>(item_id).value_or(ENTITY_NONE);
                    if (type == ENTITY_ITEM) {
                        itemtype_t item_type = g->ct.get<itemtype>(item_id).value_or(ITEM_NONE);
                        if (item_type != ITEM_NONE) {
                            if (item_type == ITEM_WEAPON) {
                                // try to equip it

                                g->ct.set<equipped_weapon>(g->hero_id, item_id);
                            }
                        }
                    }
                }
            }
        }
        //                minfo("equipped item %d", item_id);
        //            } else {
        //                minfo("failed to equip item %d", item_id);
        //            }
        //        }
        //    }
    }
    //}
    //if (inputstate_is_pressed(is, KEY_U)) {
    // unequip weapon/item
    // we will assume it is a weapon for now
    //entityid weapon_id = g_get_equipped_weapon(g, g->hero_id);
    //if (weapon_id != ENTITYID_INVALID) {
    //    if (g_unequip_weapon(g, g->hero_id, weapon_id)) {
    //        minfo("unequipped weapon %d", weapon_id);
    //    } else {
    //        minfo("failed to unequip weapon %d", weapon_id);
    //    }
    //}
    //}
    if (inputstate_is_pressed(is, KEY_D)) {
        // drop item
        drop_item_from_hero_inventory(g);
    }
    // cycle menus
    //if (inputstate_is_pressed(is, KEY_LEFT) ||
    //    inputstate_is_pressed(is, KEY_RIGHT)) {
    //    g->controlmode = CONTROLMODE_GAMEPLAY_SETTINGS;
    //    g->display_gameplay_settings_menu = true;
    //    g->display_inventory_menu = false;
    //    return;
    //}
    //if (inputstate_is_pressed(is, KEY_S)) {
    //    minfo("toggling sort inventory menu");
    //    g->display_sort_inventory_menu = !g->display_sort_inventory_menu;
    //    return;
    //}
    //size_t count = 0;
    //entityid* inventory = g_get_inventory(g, g->hero_id, &count);
    //if (g->display_inventory_menu && g->display_sort_inventory_menu) {
    //    handle_input_sort_inventory(is, g);
    //    return;
    //}
    //
    //    if (count == 0) return;
    // make sure we separate control of inv menu from its sort inv menu
    //    if (g->display_inventory_menu && !g->display_sort_inventory_menu) {
    //        if (inputstate_is_pressed(is, KEY_DOWN) ||
    //            inputstate_is_pressed(is, KEY_X)) {
    //            g->inventory_menu_selection =
    //                (size_t)g->inventory_menu_selection + 1 >= count
    //                    ? 0
    //                    : g->inventory_menu_selection + 1;
    //        } else if (inputstate_is_pressed(is, KEY_UP) ||
    //                   inputstate_is_pressed(is, KEY_W)) {
    //            g->inventory_menu_selection = g->inventory_menu_selection - 1 < 0
    //                                              ? count - 1
    //                                              : g->inventory_menu_selection - 1;
    //            // drop item
    //        } else if (inputstate_is_pressed(is, KEY_RIGHT_BRACKET)) {
    //            // we need to grab the entityid of the selected item
    //            entityid item_id = inventory[g->inventory_menu_selection];
    //            g_remove_from_inventory(g, g->hero_id, item_id);
    //            //handle_sort_inventory(g);
    //            // add the item to the tile where the player is located at
    //            vec3 loc = g_get_location(g, g->hero_id);
    //            dungeon_floor_t* const df = d_get_floor(g->d, loc.z);
    //            massert(df, "Dungeon floor is NULL!");
    //            vec3 loc_cast = {loc.x, loc.y, loc.z};
    //            //tile_t* const tile = df_tile_at(df, loc_cast);
    //            shared_ptr<tile_t> tile = df_tile_at(df, loc_cast);
    //            massert(tile, "Tile is NULL!");
    //            if (!tile_add(tile, item_id)) return;
    //            // we also have to update the location of the item
    //            g_update_location(g, item_id, loc);
    //            g->controlmode = CONTROLMODE_PLAYER;
    //            g->display_inventory_menu = false;
    //        } else if (inputstate_is_pressed(is, KEY_ENTER) ||
    //                   inputstate_is_pressed(is, KEY_APOSTROPHE)) {
    //            entityid item_id = inventory[g->inventory_menu_selection];
    //            // we will eventually adjust this to check which slot it needs to go into based on its various types
    //            entitytype_t type = g_get_type(g, item_id);
    //            if (type == ENTITY_ITEM) {
    //                itemtype item_type = g_get_itemtype(g, item_id);
    //                if (item_type == ITEM_POTION) {
    //                    potiontype potion_type = g_get_potiontype(g, item_id);
    //                    if (potion_type == POTION_HEALTH_SMALL) {
    //                        int hp = g_get_stat(g, g->hero_id, STATS_HP);
    //                        int maxhp = g_get_stat(g, g->hero_id, STATS_MAXHP);
    //                        if (hp < maxhp) {
    //                            int small_hp_health_roll = rand() % 4 + 1;
    //                            hp += small_hp_health_roll;
    //                            if (hp > maxhp) hp = maxhp;
    //                            g_set_stat(g, g->hero_id, STATS_HP, hp);
    //                            add_message_history(
    //                                g,
    //                                "%s drank a %s and recovered %d HP",
    //                                g_get_name(g, g->hero_id).c_str(),
    //                                g_get_name(g, item_id).c_str(),
    //                                small_hp_health_roll);
    //                            // remove the potion from the inventory
    //                            g_remove_from_inventory(g, g->hero_id, item_id);
    //                            g->controlmode = CONTROLMODE_PLAYER;
    //                            g->display_inventory_menu = false;
    //                            g->controlmode = CONTROLMODE_PLAYER;
    //                            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //                        }
    //                    }
    //                } else if (item_type == ITEM_WEAPON) {
    //                    // check if the item is already equipped
    //                    //entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_WEAPON);
    //                    //if (equipped_item != ENTITYID_INVALID) {
    //                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_WEAPON, item_id);
    //                    add_message_history(g,
    //                                        "%s equipped %s",
    //                                        g_get_name(g, g->hero_id).c_str(),
    //                                        g_get_name(g, item_id).c_str());
    //                    //}
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->display_inventory_menu = false;
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //                } else if (item_type == ITEM_SHIELD) {
    //                    //entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_SHIELD);
    //                    //if (equipped_item != ENTITYID_INVALID) {
    //                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_SHIELD, item_id);
    //                    add_message_history(g,
    //                                        "%s equipped %s",
    //                                        g_get_name(g, g->hero_id).c_str(),
    //                                        g_get_name(g, item_id).c_str());
    //                    //}
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->display_inventory_menu = false;
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //                } else if (item_type == ITEM_WAND) {
    //                    //entityid equipped_item = g_get_equipment(g, g->hero_id, EQUIP_SLOT_WAND);
    //                    //if (equipped_item != ENTITYID_INVALID) {
    //                    // unequip the currently equipped item
    //                    //    g_unset_equipment(g, g->hero_id, EQUIP_SLOT_WAND);
    //                    //    add_message_history(g, "%s unequipped %s", g_get_name(g, g->hero_id), g_get_name(g, item_id));
    //                    //} else {
    //                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_WAND, item_id);
    //                    add_message_history(g,
    //                                        "%s equipped %s",
    //                                        g_get_name(g, g->hero_id).c_str(),
    //                                        g_get_name(g, item_id).c_str());
    //                    //}
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->display_inventory_menu = false;
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //                } else if (item_type == ITEM_RING) {
    //                    g_set_equipment(g, g->hero_id, EQUIP_SLOT_RING, item_id);
    //                    add_message_history(g,
    //                                        "%s equipped %s",
    //                                        g_get_name(g, g->hero_id).c_str(),
    //                                        g_get_name(g, item_id).c_str());
    //
    //                    // update player tiles
    //                    update_player_tiles_explored(g);
    //
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->display_inventory_menu = false;
    //                    g->controlmode = CONTROLMODE_PLAYER;
    //                    g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
    //                }
    //            }
    //        }
    //    }
}
