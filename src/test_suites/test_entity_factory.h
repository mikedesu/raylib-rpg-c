#pragma once

#include "../gamestate.h"
#include <cxxtest/TestSuite.h>

class EntityFactoryTestSuite : public CxxTest::TestSuite {
private:
    vec3 add_initialized_floor(gamestate& g, int width = 8, int height = 8) {
        auto df = g.d.create_floor(BIOME_STONE, width, height);
        df->df_set_all_tiles(TILE_FLOOR_STONE_00);
        g.d.add_floor(df);
        g.d.is_initialized = true;
        return vec3{1, 1, 0};
    }

public:
    void testAlignmentToStrReturnsExpectedNames() {
        TS_ASSERT_EQUALS(alignment_to_str(ALIGNMENT_GOOD_LAWFUL), "lawful good");
        TS_ASSERT_EQUALS(alignment_to_str(ALIGNMENT_NEUTRAL_NEUTRAL), "true neutral");
        TS_ASSERT_EQUALS(alignment_to_str(ALIGNMENT_EVIL_CHAOTIC), "chaotic evil");
    }

    void testCreateWeaponAtWithSetsExpectedWeaponComponents() {
        gamestate g;
        const vec3 loc = add_initialized_floor(g);

        const entityid id = g.create_weapon_at_with(g.ct, loc, g.dagger_init());

        TS_ASSERT_DIFFERS(id, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.ct.get<entitytype>(id).value_or(ENTITY_NONE), ENTITY_ITEM);
        TS_ASSERT_EQUALS(g.ct.get<itemtype>(id).value_or(ITEM_NONE), ITEM_WEAPON);
        TS_ASSERT_EQUALS(g.ct.get<weapontype>(id).value_or(WEAPON_NONE), WEAPON_DAGGER);
        TS_ASSERT_EQUALS(g.ct.get<name>(id).value_or(""), "dagger");
        TS_ASSERT_EQUALS(g.ct.get<description>(id).value_or(""), "Stabby stabby.");
        TS_ASSERT(g.ct.get<update>(id).value_or(false));
        TS_ASSERT(g.ct.get<location>(id).has_value());
        TS_ASSERT(vec3_equal(g.ct.get<location>(id).value_or(vec3{-1, -1, -1}), loc));

        auto damage_value = g.ct.get<damage>(id);
        TS_ASSERT(damage_value.has_value());
        TS_ASSERT_EQUALS(damage_value.value().x, 1);
        TS_ASSERT_EQUALS(damage_value.value().y, 4);
        TS_ASSERT_EQUALS(damage_value.value().z, 0);

        tile_t& tile = g.d.get_floor(0)->tile_at(loc);
        TS_ASSERT_EQUALS(tile.get_cached_item(), id);
    }

    void testCreateShieldAtWithSetsExpectedShieldComponents() {
        gamestate g;
        const vec3 loc = add_initialized_floor(g);

        const entityid id = g.create_shield_at_with(g.ct, loc, g.shield_init());

        TS_ASSERT_DIFFERS(id, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.ct.get<entitytype>(id).value_or(ENTITY_NONE), ENTITY_ITEM);
        TS_ASSERT_EQUALS(g.ct.get<itemtype>(id).value_or(ITEM_NONE), ITEM_SHIELD);
        TS_ASSERT_EQUALS(g.ct.get<shieldtype>(id).value_or(SHIELD_NONE), SHIELD_KITE);
        TS_ASSERT_EQUALS(g.ct.get<name>(id).value_or(""), "kite shield");
        TS_ASSERT_EQUALS(g.ct.get<description>(id).value_or(""), "Standard knight's shield");
        TS_ASSERT_EQUALS(g.ct.get<block_chance>(id).value_or(0), 90);
        TS_ASSERT(!g.ct.get<update>(id).value_or(true));
        TS_ASSERT(vec3_equal(g.ct.get<location>(id).value_or(vec3{-1, -1, -1}), loc));

        tile_t& tile = g.d.get_floor(0)->tile_at(loc);
        TS_ASSERT_EQUALS(tile.get_cached_item(), id);
    }

    void testCreatePotionAtWithSetsExpectedPotionComponents() {
        gamestate g;
        const vec3 loc = add_initialized_floor(g);

        const entityid id = g.create_potion_at_with(loc, g.potion_init(POTION_HP_SMALL));

        TS_ASSERT_DIFFERS(id, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.ct.get<entitytype>(id).value_or(ENTITY_NONE), ENTITY_ITEM);
        TS_ASSERT_EQUALS(g.ct.get<itemtype>(id).value_or(ITEM_NONE), ITEM_POTION);
        TS_ASSERT_EQUALS(g.ct.get<potiontype>(id).value_or(POTION_NONE), POTION_HP_SMALL);
        TS_ASSERT_EQUALS(g.ct.get<name>(id).value_or(""), "small healing potion");
        TS_ASSERT_EQUALS(g.ct.get<description>(id).value_or(""), "a small healing potion");
        TS_ASSERT(g.ct.get<update>(id).value_or(false));
        TS_ASSERT(vec3_equal(g.ct.get<location>(id).value_or(vec3{-1, -1, -1}), loc));

        auto healing_value = g.ct.get<healing>(id);
        TS_ASSERT(healing_value.has_value());
        TS_ASSERT_EQUALS(healing_value.value().x, 1);
        TS_ASSERT_EQUALS(healing_value.value().y, 6);
        TS_ASSERT_EQUALS(healing_value.value().z, 0);

        tile_t& tile = g.d.get_floor(0)->tile_at(loc);
        TS_ASSERT_EQUALS(tile.get_cached_item(), id);
    }

    void testCreatePropAtWithSetsExpectedPropComponents() {
        gamestate g;
        const vec3 loc = add_initialized_floor(g);

        const entityid id = g.create_prop_at_with(PROP_DUNGEON_STATUE_00, loc, dungeon_prop_init(PROP_DUNGEON_STATUE_00));

        TS_ASSERT_DIFFERS(id, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.ct.get<entitytype>(id).value_or(ENTITY_NONE), ENTITY_PROP);
        TS_ASSERT_EQUALS(g.ct.get<proptype>(id).value_or(PROP_NONE), PROP_DUNGEON_STATUE_00);
        TS_ASSERT_EQUALS(g.ct.get<name>(id).value_or(""), "statue");
        TS_ASSERT_EQUALS(g.ct.get<description>(id).value_or(""), "A heavy carved statue.");
        TS_ASSERT(g.ct.get<solid>(id).value_or(false));
        TS_ASSERT(g.ct.get<pushable>(id).value_or(false));
        TS_ASSERT(g.ct.get<update>(id).value_or(false));
        TS_ASSERT(vec3_equal(g.ct.get<location>(id).value_or(vec3{-1, -1, -1}), loc));

        tile_t& tile = g.d.get_floor(0)->tile_at(loc);
        TS_ASSERT_EQUALS(tile.get_cached_prop(), id);
    }

    void testCreateOrcAtWithSetsExpectedNpcComponents() {
        gamestate g;
        const vec3 loc = add_initialized_floor(g);

        const entityid id = g.create_orc_at_with(loc, [](CT&, const entityid) {});

        TS_ASSERT_DIFFERS(id, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.ct.get<entitytype>(id).value_or(ENTITY_NONE), ENTITY_NPC);
        TS_ASSERT_EQUALS(g.ct.get<race>(id).value_or(RACE_NONE), RACE_ORC);
        TS_ASSERT_EQUALS(g.ct.get<alignment>(id).value_or(ALIGNMENT_NONE), ALIGNMENT_EVIL_CHAOTIC);
        TS_ASSERT(g.ct.get<name>(id).has_value());
        TS_ASSERT(!g.ct.get<name>(id).value_or("").empty());
        TS_ASSERT(!g.ct.get<dead>(id).value_or(true));
        TS_ASSERT(g.ct.get<update>(id).value_or(false));
        TS_ASSERT(g.ct.get<inventory>(id).has_value());
        TS_ASSERT_EQUALS(g.ct.get<inventory>(id).value()->size(), 0U);
        const vec2 hp_value = g.ct.get<hp>(id).value_or(vec2{0, 0});
        TS_ASSERT(hp_value.x >= 1);
        TS_ASSERT(hp_value.y >= 1);
        TS_ASSERT(vec3_equal(g.ct.get<location>(id).value_or(vec3{-1, -1, -1}), loc));

        tile_t& tile = g.d.get_floor(0)->tile_at(loc);
        TS_ASSERT_EQUALS(tile.get_cached_live_npc(), id);
    }

    void testCreatePlayerAtWithUsesCharacterCreationAlignment() {
        gamestate g;
        const vec3 loc = add_initialized_floor(g);
        g.chara_creation.race = RACE_DWARF;
        g.chara_creation.alignment = ALIGNMENT_GOOD_LAWFUL;

        const entityid id = g.create_player_at_with(loc, "hero", g.player_init(12));

        TS_ASSERT_DIFFERS(id, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.ct.get<entitytype>(id).value_or(ENTITY_NONE), ENTITY_PLAYER);
        TS_ASSERT_EQUALS(g.ct.get<alignment>(id).value_or(ALIGNMENT_NONE), ALIGNMENT_GOOD_LAWFUL);
    }
};
