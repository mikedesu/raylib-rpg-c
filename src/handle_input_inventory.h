#pragma once

#include "drop_item.h"
#include "entitytype.h"
#include "gamestate.h"
#include "inputstate.h"
#include "play_sound.h"
#include "roll.h"
#include "sfx.h"
#include "use_potion.h"


static inline void handle_potion_use(gamestate& g, entityid id) {
    const entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
    if (type != ENTITY_ITEM)
        return;
    const itemtype_t i_type = g.ct.get<itemtype>(id).value_or(ITEM_NONE);
    if (i_type == ITEM_NONE || i_type != ITEM_POTION)
        return;
    if (use_potion(g, g.hero_id, id)) {
        g.flag = GAMESTATE_FLAG_PLAYER_ANIM;
        g.controlmode = CONTROLMODE_PLAYER;
        g.display_inventory_menu = false;
    }
}


static inline void handle_item_use(gamestate& g) {
    const size_t index = g.inventory_cursor.y * 7 + g.inventory_cursor.x;
    if (index < 0)
        return;
    auto maybe_inventory = g.ct.get<inventory>(g.hero_id);
    if (!maybe_inventory || !maybe_inventory.has_value())
        return;
    auto inventory = maybe_inventory.value();
    if (index >= inventory->size())
        return;
    entityid item_id = inventory->at(index);
    entitytype_t type = g.ct.get<entitytype>(item_id).value_or(ENTITY_NONE);
    if (type != ENTITY_ITEM)
        return;
    itemtype_t i_type = g.ct.get<itemtype>(item_id).value_or(ITEM_NONE);
    if (i_type == ITEM_NONE)
        return;
    if (i_type == ITEM_POTION) {
        handle_potion_use(g, item_id);
    }
}


static inline void handle_inventory_equip_shield(gamestate& g, entityid item_id) {
    // Check if this is the currently equipped weapon
    const entityid current_shield = g.ct.get<equipped_shield>(g.hero_id).value_or(ENTITYID_INVALID);
    // Unequip if it's already equipped
    if (current_shield == item_id)
        g.ct.set<equipped_shield>(g.hero_id, ENTITYID_INVALID);
    // Equip the new shield
    else
        g.ct.set<equipped_shield>(g.hero_id, item_id);
    g.flag = GAMESTATE_FLAG_PLAYER_ANIM;
    g.controlmode = CONTROLMODE_PLAYER;
    g.display_inventory_menu = false;
}


static inline void handle_inventory_equip_weapon(gamestate& g, entityid item_id) {
    // Check if this is the currently equipped weapon
    const entityid current_weapon = g.ct.get<equipped_weapon>(g.hero_id).value_or(ENTITYID_INVALID);
    // Unequip if it's already equipped
    if (current_weapon == item_id)
        g.ct.set<equipped_weapon>(g.hero_id, ENTITYID_INVALID);
    // Equip the new weapon
    else
        g.ct.set<equipped_weapon>(g.hero_id, item_id);
    g.flag = GAMESTATE_FLAG_PLAYER_ANIM;
    g.controlmode = CONTROLMODE_PLAYER;
    g.display_inventory_menu = false;
}


static inline void handle_inventory_equip_item(gamestate& g, entityid item_id) {
    itemtype_t item_type = g.ct.get<itemtype>(item_id).value_or(ITEM_NONE);
    switch (item_type) {
    case ITEM_NONE: break;
    case ITEM_WEAPON: handle_inventory_equip_weapon(g, item_id); break;
    case ITEM_SHIELD: handle_inventory_equip_shield(g, item_id); break;
    default: break;
    }
}


static inline void handle_inventory_equip(gamestate& g) {
    PlaySound(g.sfx.at(SFX_EQUIP_01));
    // equip item
    // get the item id of the current selection
    const size_t index = g.inventory_cursor.y * 7 + g.inventory_cursor.x;
    auto my_inventory = g.ct.get<inventory>(g.hero_id);
    if (!my_inventory)
        return;
    if (!my_inventory.has_value())
        return;
    auto unpacked_inventory = my_inventory.value();
    if (index < 0 || index >= unpacked_inventory->size())
        return;
    entityid item_id = unpacked_inventory->at(index);
    entitytype_t type = g.ct.get<entitytype>(item_id).value_or(ENTITY_NONE);
    if (type == ENTITY_ITEM)
        handle_inventory_equip_item(g, item_id);
}


static inline void handle_input_inventory(gamestate& g, inputstate& is) {
    if (g.controlmode != CONTROLMODE_INVENTORY)
        return;
    if (!g.display_inventory_menu)
        return;
    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        g.do_quit = true;
        return;
    }
    if (inputstate_is_pressed(is, KEY_I)) {
        g.controlmode = CONTROLMODE_PLAYER;
        g.display_inventory_menu = false;
        play_sound(SFX_BAG_CLOSE);
        return;
    }
    if (inputstate_is_pressed(is, KEY_LEFT) || inputstate_is_pressed(is, KEY_A)) {
        play_sound(SFX_CONFIRM_01);
        if (g.inventory_cursor.x > 0)
            g.inventory_cursor.x--;
    }
    if (inputstate_is_pressed(is, KEY_RIGHT) || inputstate_is_pressed(is, KEY_D)) {
        play_sound(SFX_CONFIRM_01);
        g.inventory_cursor.x++;
    }
    if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_W)) {
        play_sound(SFX_CONFIRM_01);
        if (g.inventory_cursor.y > 0)
            g.inventory_cursor.y--;
    }
    if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_X)) {
        play_sound(SFX_CONFIRM_01);
        g.inventory_cursor.y++;
    }
    if (inputstate_is_pressed(is, KEY_E)) {
        handle_inventory_equip(g);
    }
    if (inputstate_is_pressed(is, KEY_Q)) {
        // drop item
        play_sound(SFX_DISCARD_ITEM);
        drop_item_from_hero_inventory(g);
    }
    if (inputstate_is_pressed(is, KEY_ENTER)) {
        handle_item_use(g);
        play_sound(SFX_CONFIRM_01);
    }
}
