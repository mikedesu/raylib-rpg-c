#pragma once

#include "../gamestate.h"
#include "../sfx.h"
#include <cxxtest/TestSuite.h>

class InventoryTestSuite : public CxxTest::TestSuite {
private:
    vec3 add_initialized_floor(gamestate& g, int width = 8, int height = 8) {
        auto df = g.d.create_floor(BIOME_STONE, width, height);
        df->df_set_all_tiles(TILE_FLOOR_STONE_00);
        g.d.add_floor(df);
        g.d.is_initialized = true;
        return vec3{1, 1, 0};
    }

    entityid create_hero(gamestate& g, vec3 loc) {
        return g.create_player_at_with(loc, "hero", [](CT&, const entityid) {});
    }

    void press_key(inputstate& is, int key) {
        inputstate_reset(is);
        const int idx = key / BITS_PER_LONG;
        const int bit = key % BITS_PER_LONG;
        is.pressed[idx] |= (1ULL << bit);
    }

public:
    void testTryEntityPickupTakesTopItemFromMultiItemTile() {
        gamestate g;
        g.sfx.resize(71);
        const vec3 loc = add_initialized_floor(g);
        const entityid hero = create_hero(g, loc);

        const entityid dagger = g.create_weapon_at_with(g.ct, loc, g.dagger_init());
        const entityid potion = g.create_potion_at_with(loc, g.potion_init(POTION_HP_SMALL));

        TS_ASSERT_DIFFERS(dagger, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(potion, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.d.get_floor(0)->tile_at(loc).get_cached_item(), potion);

        TS_ASSERT(g.try_entity_pickup(hero));

        auto inv = g.ct.get<inventory>(hero);
        TS_ASSERT(inv.has_value());
        TS_ASSERT_EQUALS(inv.value()->size(), 1U);
        TS_ASSERT_EQUALS(inv.value()->at(0), potion);
        TS_ASSERT(g.is_in_inventory(hero, potion));
        TS_ASSERT(!g.is_in_inventory(hero, dagger));
        TS_ASSERT_EQUALS(g.d.get_floor(0)->tile_at(loc).get_cached_item(), dagger);
    }

    void testAddAndRemoveInventoryBookkeeping() {
        gamestate g;
        const vec3 loc = add_initialized_floor(g);
        const entityid hero = create_hero(g, loc);
        const entityid dagger = g.create_weapon_at_with(g.ct, loc, g.dagger_init());

        TS_ASSERT(g.add_to_inventory(hero, dagger));
        TS_ASSERT(g.is_in_inventory(hero, dagger));

        auto inv = g.ct.get<inventory>(hero);
        TS_ASSERT(inv.has_value());
        TS_ASSERT_EQUALS(inv.value()->size(), 1U);

        TS_ASSERT(g.remove_from_inventory(hero, dagger));
        TS_ASSERT(!g.is_in_inventory(hero, dagger));
        TS_ASSERT_EQUALS(g.ct.get<inventory>(hero).value()->size(), 0U);
        TS_ASSERT(!g.remove_from_inventory(hero, dagger));
    }

    void testEquipAndUnequipWeaponFlow() {
        gamestate g;
        const vec3 loc = add_initialized_floor(g);
        const entityid hero = create_hero(g, loc);
        const entityid dagger = g.create_weapon_at_with(g.ct, loc, g.dagger_init());

        TS_ASSERT(g.add_to_inventory(hero, dagger));
        g.display_inventory_menu = true;
        g.handle_hero_inventory_equip_weapon(dagger);

        TS_ASSERT_EQUALS(g.ct.get<equipped_weapon>(hero).value_or(ENTITYID_INVALID), dagger);
        TS_ASSERT_EQUALS(g.flag, GAMESTATE_FLAG_PLAYER_ANIM);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_PLAYER);
        TS_ASSERT(!g.display_inventory_menu);

        g.display_inventory_menu = true;
        g.handle_hero_inventory_equip_weapon(dagger);
        TS_ASSERT_EQUALS(g.ct.get<equipped_weapon>(hero).value_or(ENTITYID_INVALID), ENTITYID_INVALID);
        TS_ASSERT(!g.display_inventory_menu);
    }

    void testEquipAndUnequipShieldFlow() {
        gamestate g;
        const vec3 loc = add_initialized_floor(g);
        const entityid hero = create_hero(g, loc);
        const entityid shield = g.create_shield_at_with(g.ct, loc, g.shield_init());

        TS_ASSERT(g.add_to_inventory(hero, shield));
        g.display_inventory_menu = true;
        g.handle_hero_inventory_equip_shield(shield);

        TS_ASSERT_EQUALS(g.ct.get<equipped_shield>(hero).value_or(ENTITYID_INVALID), shield);
        TS_ASSERT_EQUALS(g.flag, GAMESTATE_FLAG_PLAYER_ANIM);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_PLAYER);
        TS_ASSERT(!g.display_inventory_menu);

        g.display_inventory_menu = true;
        g.handle_hero_inventory_equip_shield(shield);
        TS_ASSERT_EQUALS(g.ct.get<equipped_shield>(hero).value_or(ENTITYID_INVALID), ENTITYID_INVALID);
        TS_ASSERT(!g.display_inventory_menu);
    }

    void testDropItemFromHeroInventoryClearsEquipmentAndPlacesItemOnTile() {
        gamestate g;
        const vec3 loc = add_initialized_floor(g);
        const entityid hero = create_hero(g, loc);
        const entityid dagger = g.create_weapon_at_with(g.ct, loc, g.dagger_init());

        TS_ASSERT(g.add_to_inventory(hero, dagger));
        g.ct.set<equipped_weapon>(hero, dagger);
        g.inventory_cursor = Vector2{0, 0};

        TS_ASSERT(g.drop_item_from_hero_inventory());

        TS_ASSERT_EQUALS(g.ct.get<inventory>(hero).value()->size(), 0U);
        TS_ASSERT_EQUALS(g.ct.get<equipped_weapon>(hero).value_or(ENTITYID_INVALID), ENTITYID_INVALID);
        TS_ASSERT(vec3_equal(g.ct.get<location>(dagger).value_or(vec3{-1, -1, -1}), loc));
        TS_ASSERT_EQUALS(g.d.get_floor(0)->tile_at(loc).get_cached_item(), dagger);
    }

    void testUsePotionConsumesPotionAndUpdatesHp() {
        gamestate g;
        const vec3 loc = add_initialized_floor(g);
        const entityid hero = create_hero(g, loc);
        const entityid potion = g.create_potion_at_with(loc, g.potion_init(POTION_HP_SMALL));

        TS_ASSERT(g.add_to_inventory(hero, potion));
        g.ct.set<hp>(hero, vec2{2, 10});

        TS_ASSERT(g.use_potion(hero, potion));

        TS_ASSERT(!g.is_in_inventory(hero, potion));
        TS_ASSERT_EQUALS(g.ct.get<inventory>(hero).value()->size(), 0U);
        const vec2 hp_value = g.ct.get<hp>(hero).value_or(vec2{0, 0});
        TS_ASSERT(hp_value.x >= 3);
        TS_ASSERT(hp_value.x <= 8);
        TS_ASSERT_EQUALS(hp_value.y, 10);
        TS_ASSERT(g.msg_history.size() >= 2U);
    }

    void testChestTransferMovesItemsBetweenHeroAndChestInventories() {
        gamestate g;
        g.sfx.resize(71);
        const vec3 loc = add_initialized_floor(g);
        const entityid hero = create_hero(g, loc);
        const entityid chest = g.create_chest_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});
        const entityid dagger = g.create_weapon_at_with(g.ct, loc, g.dagger_init());

        TS_ASSERT_DIFFERS(chest, ENTITYID_INVALID);
        TS_ASSERT(g.add_to_inventory(hero, dagger));
        TS_ASSERT(g.transfer_inventory_item(hero, chest, dagger));
        TS_ASSERT(!g.is_in_inventory(hero, dagger));
        TS_ASSERT(g.is_in_inventory(chest, dagger));

        TS_ASSERT(g.open_chest_menu(chest));
        TS_ASSERT(g.display_chest_menu);
        g.toggle_chest_menu_mode();
        TS_ASSERT(g.chest_deposit_mode);

        g.toggle_chest_menu_mode();
        TS_ASSERT(!g.chest_deposit_mode);
        g.handle_chest_menu_confirm();

        TS_ASSERT(g.is_in_inventory(hero, dagger));
        TS_ASSERT(!g.is_in_inventory(chest, dagger));
        TS_ASSERT(g.display_chest_menu);

        g.close_chest_menu();
        TS_ASSERT(!g.display_chest_menu);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_PLAYER);
    }

    void testChestMenuClosesOnKeyDAndNotKeyO() {
        gamestate g;
        g.sfx.resize(71);
        const vec3 loc = add_initialized_floor(g);
        const entityid hero = create_hero(g, loc);
        const entityid chest = g.create_chest_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});

        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(chest, ENTITYID_INVALID);
        TS_ASSERT(g.open_chest_menu(chest));

        inputstate is = {};
        press_key(is, KEY_D);
        g.handle_input_chest(is);
        TS_ASSERT(!g.display_chest_menu);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_PLAYER);

        TS_ASSERT(g.open_chest_menu(chest));
        press_key(is, KEY_O);
        g.handle_input_chest(is);
        TS_ASSERT(g.display_chest_menu);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_CHEST);
    }

    void testMiniInventorySelectionScrollsPastVisibleWindow() {
        gamestate g;
        g.sfx.resize(71);
        g.prefer_mini_inventory_menu = true;
        g.mini_inventory_visible_count = 3;
        const vec3 loc = add_initialized_floor(g);
        const entityid hero = create_hero(g, loc);
        g.hero_id = hero;

        for (int i = 0; i < 5; i++) {
            const entityid dagger = g.create_weapon_at_with(g.ct, loc, g.dagger_init());
            TS_ASSERT_DIFFERS(dagger, ENTITYID_INVALID);
            TS_ASSERT(g.add_to_inventory(hero, dagger));
        }

        g.display_inventory_menu = true;
        g.controlmode = CONTROLMODE_INVENTORY;
        inputstate is = {};
        for (int i = 0; i < 4; i++) {
            press_key(is, KEY_DOWN);
            g.handle_input_inventory(is);
        }

        TS_ASSERT_EQUALS(g.get_inventory_selection_index(), 4U);
        TS_ASSERT_EQUALS(g.mini_inventory_scroll_offset, 2U);
        TS_ASSERT_EQUALS(static_cast<int>(g.inventory_cursor.y), 2);
    }
};
