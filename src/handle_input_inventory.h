#pragma once

#include "entitytype.h"
#include "gamestate.h"
#include "inputstate.h"
#include "play_sound.h"
#include "roll.h"
#include "sfx.h"


static inline void handle_hero_potion_use(gamestate& g, entityid id) {
    const entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
    if (type != ENTITY_ITEM)
        return;
    const itemtype_t i_type = g.ct.get<itemtype>(id).value_or(ITEM_NONE);
    if (i_type == ITEM_NONE || i_type != ITEM_POTION)
        return;
    if (g.use_potion(g.hero_id, id)) {
        g.flag = GAMESTATE_FLAG_PLAYER_ANIM;
        g.controlmode = CONTROLMODE_PLAYER;
        g.display_inventory_menu = false;
    }
}


static inline void handle_hero_item_use(gamestate& g) {
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
        handle_hero_potion_use(g, item_id);
    }
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
        PlaySound(g.sfx[SFX_BAG_CLOSE]);
        return;
    }
    if (inputstate_is_pressed(is, KEY_LEFT) || inputstate_is_pressed(is, KEY_A)) {
        play_sound(SFX_CONFIRM_01);
        if (g.inventory_cursor.x > 0)
            g.inventory_cursor.x--;
    }
    if (inputstate_is_pressed(is, KEY_RIGHT) || inputstate_is_pressed(is, KEY_D)) {
        PlaySound(g.sfx[SFX_CONFIRM_01]);
        g.inventory_cursor.x++;
    }
    if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_W)) {
        PlaySound(g.sfx[SFX_CONFIRM_01]);
        if (g.inventory_cursor.y > 0)
            g.inventory_cursor.y--;
    }
    if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_X)) {
        PlaySound(g.sfx[SFX_CONFIRM_01]);
        g.inventory_cursor.y++;
    }
    if (inputstate_is_pressed(is, KEY_E)) {
        g.handle_hero_inventory_equip();
    }
    if (inputstate_is_pressed(is, KEY_Q)) {
        // drop item
        PlaySound(g.sfx[SFX_DISCARD_ITEM]);
        g.drop_item_from_hero_inventory();
    }
    if (inputstate_is_pressed(is, KEY_ENTER)) {
        handle_hero_item_use(g);
        PlaySound(g.sfx[SFX_CONFIRM_01]);
    }
}
