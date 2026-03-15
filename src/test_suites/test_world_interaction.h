#pragma once

#include "../gamestate.h"
#include <cxxtest/TestSuite.h>

class WorldInteractionTestSuite : public CxxTest::TestSuite {
private:
    void add_floor(gamestate& g, int width = 6, int height = 6) {
        auto df = g.d.create_floor(BIOME_STONE, width, height);
        df->df_set_all_tiles(TILE_FLOOR_STONE_00);
        g.d.add_floor(df);
        g.d.is_initialized = true;
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
    void testTryEntityMoveBlockedByWall() {
        gamestate g;
        add_floor(g);
        const vec3 hero_loc{1, 1, 0};
        const entityid hero = create_hero(g, hero_loc);
        g.d.get_floor(0)->df_set_tile(TILE_STONE_WALL_00, 2, 1);

        TS_ASSERT(!g.try_entity_move(hero, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), hero_loc));
    }

    void testTryEntityMoveBlockedBySolidPropAndClosedDoor() {
        gamestate g;
        add_floor(g);
        const vec3 hero_loc{1, 1, 0};
        const entityid hero = create_hero(g, hero_loc);

        const entityid prop_id = g.create_prop_at_with(PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_EMPTY, vec3{2, 1, 0}, dungeon_prop_init(PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_EMPTY));
        TS_ASSERT_DIFFERS(prop_id, ENTITYID_INVALID);
        TS_ASSERT(!g.try_entity_move(hero, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), hero_loc));

        g.d.get_floor(0)->tile_at(vec3{2, 1, 0}).tile_remove(prop_id);
        const entityid door_id = g.create_door_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(door_id, ENTITYID_INVALID);
        TS_ASSERT(!g.try_entity_move(hero, vec3{1, 0, 0}));

        g.ct.set<door_open>(door_id, true);
        TS_ASSERT(g.try_entity_move(hero, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{2, 1, 0}));
    }

    void testCreateDoorAtWithRejectsOccupiedTile() {
        gamestate g;
        add_floor(g);

        const entityid prop_id = g.create_prop_at_with(PROP_DUNGEON_STATUE_00, vec3{2, 2, 0}, dungeon_prop_init(PROP_DUNGEON_STATUE_00));
        TS_ASSERT_DIFFERS(prop_id, ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.create_door_at_with(vec3{2, 2, 0}, [](CT&, const entityid) {}), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(g.d.get_floor(0)->tile_at(vec3{2, 2, 0}).get_cached_door(), ENTITYID_INVALID);
    }

    void testTryEntityMovePushesBox() {
        gamestate g;
        add_floor(g);
        const entityid hero = create_hero(g, vec3{2, 2, 0});
        const entityid box = g.create_box_at_with(vec3{3, 2, 0});

        TS_ASSERT_DIFFERS(box, ENTITYID_INVALID);
        TS_ASSERT(g.try_entity_move(hero, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{2, 2, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(box).value_or(vec3{-1, -1, -1}), vec3{4, 2, 0}));
        TS_ASSERT_EQUALS(g.d.get_floor(0)->tile_at(vec3{4, 2, 0}).get_cached_box(), box);
    }

    void testTryEntityPullMovesHeroAndBoxBackward() {
        gamestate g;
        add_floor(g);
        const entityid hero = create_hero(g, vec3{2, 2, 0});
        g.ct.set<direction>(hero, DIR_RIGHT);
        const entityid box = g.create_box_at_with(vec3{3, 2, 0});

        TS_ASSERT_DIFFERS(box, ENTITYID_INVALID);
        TS_ASSERT(g.try_entity_pull(hero));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{1, 2, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(box).value_or(vec3{-1, -1, -1}), vec3{2, 2, 0}));
        TS_ASSERT_EQUALS(g.d.get_floor(0)->tile_at(vec3{2, 2, 0}).get_cached_box(), box);
    }

    void testTryEntityPullDeadNpcKeepsDeadNpcCachePath() {
        gamestate g;
        add_floor(g);
        const entityid hero = create_hero(g, vec3{2, 2, 0});
        g.ct.set<direction>(hero, DIR_RIGHT);

        const vec3 corpse_loc{3, 2, 0};
        const entityid corpse = g.create_orc_at_with(corpse_loc, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(corpse, ENTITYID_INVALID);

        tile_t& corpse_tile = g.d.get_floor(0)->tile_at(corpse_loc);
        TS_ASSERT_EQUALS(corpse_tile.tile_remove(corpse), corpse);
        corpse_tile.add_dead_npc(corpse);
        g.ct.set<dead>(corpse, true);
        g.ct.set<pullable>(corpse, true);

        TS_ASSERT(g.try_entity_pull(hero));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{1, 2, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(corpse).value_or(vec3{-1, -1, -1}), vec3{2, 2, 0}));

        tile_t& new_tile = g.d.get_floor(0)->tile_at(vec3{2, 2, 0});
        TS_ASSERT_EQUALS(new_tile.get_cached_dead_npc(), corpse);
        TS_ASSERT_EQUALS(new_tile.get_cached_live_npc(), ENTITYID_INVALID);
    }

    void testTryEntityPullMovesPullableCandleProp() {
        gamestate g;
        add_floor(g);
        const entityid hero = create_hero(g, vec3{2, 2, 0});
        g.ct.set<direction>(hero, DIR_RIGHT);

        const entityid candle = g.create_prop_at_with(
            PROP_DUNGEON_CANDLE_00,
            vec3{3, 2, 0},
            dungeon_prop_init(PROP_DUNGEON_CANDLE_00));

        TS_ASSERT_DIFFERS(candle, ENTITYID_INVALID);
        TS_ASSERT(g.ct.get<pullable>(candle).value_or(false));

        TS_ASSERT(g.try_entity_pull(hero));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{1, 2, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(candle).value_or(vec3{-1, -1, -1}), vec3{2, 2, 0}));
        TS_ASSERT_EQUALS(g.d.get_floor(0)->tile_at(vec3{2, 2, 0}).get_cached_prop(), candle);
    }

    void testTryEntityMovePushesPushableTableProp() {
        gamestate g;
        add_floor(g);
        const entityid hero = create_hero(g, vec3{2, 2, 0});
        const entityid table = g.create_prop_at_with(
            PROP_DUNGEON_WOODEN_TABLE_00,
            vec3{3, 2, 0},
            dungeon_prop_init(PROP_DUNGEON_WOODEN_TABLE_00));

        TS_ASSERT_DIFFERS(table, ENTITYID_INVALID);
        TS_ASSERT(g.ct.get<pushable>(table).value_or(false));

        TS_ASSERT(g.try_entity_move(hero, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{2, 2, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(table).value_or(vec3{-1, -1, -1}), vec3{4, 2, 0}));
        TS_ASSERT_EQUALS(g.d.get_floor(0)->tile_at(vec3{4, 2, 0}).get_cached_prop(), table);
    }

    void testRunMoveActionUsesQueuedMovementIntent() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);

        TS_ASSERT(g.run_move_action(hero, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{2, 1, 0}));
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testRunPushActionUsesQueuedPushIntent() {
        gamestate g;
        add_floor(g);

        const entityid box = g.create_box_at_with(vec3{2, 2, 0});
        TS_ASSERT_DIFFERS(box, ENTITYID_INVALID);

        TS_ASSERT(g.run_push_action(box, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(box).value_or(vec3{-1, -1, -1}), vec3{3, 2, 0}));
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testRunMoveActionQueuesPushIntentForPushableTarget() {
        gamestate g;
        add_floor(g);
        const entityid hero = create_hero(g, vec3{2, 2, 0});
        const entityid table = g.create_prop_at_with(
            PROP_DUNGEON_WOODEN_TABLE_00,
            vec3{3, 2, 0},
            dungeon_prop_init(PROP_DUNGEON_WOODEN_TABLE_00));

        TS_ASSERT_DIFFERS(table, ENTITYID_INVALID);
        TS_ASSERT(g.ct.get<pushable>(table).value_or(false));

        TS_ASSERT(g.run_move_action(hero, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{2, 2, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(table).value_or(vec3{-1, -1, -1}), vec3{4, 2, 0}));
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testTryEntityPullMovesPullableTableProp() {
        gamestate g;
        add_floor(g);
        const entityid hero = create_hero(g, vec3{2, 2, 0});
        g.ct.set<direction>(hero, DIR_RIGHT);
        const entityid table = g.create_prop_at_with(
            PROP_DUNGEON_WOODEN_TABLE_01,
            vec3{3, 2, 0},
            dungeon_prop_init(PROP_DUNGEON_WOODEN_TABLE_01));

        TS_ASSERT_DIFFERS(table, ENTITYID_INVALID);
        TS_ASSERT(g.ct.get<pullable>(table).value_or(false));

        TS_ASSERT(g.try_entity_pull(hero));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{1, 2, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(table).value_or(vec3{-1, -1, -1}), vec3{2, 2, 0}));
        TS_ASSERT_EQUALS(g.d.get_floor(0)->tile_at(vec3{2, 2, 0}).get_cached_prop(), table);
    }

    void testRunPullActionUsesQueuedPullIntent() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{2, 2, 0});
        g.ct.set<direction>(hero, DIR_RIGHT);
        const entityid box = g.create_box_at_with(vec3{3, 2, 0});

        TS_ASSERT_DIFFERS(box, ENTITYID_INVALID);
        TS_ASSERT(g.run_pull_action(hero));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{1, 2, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(box).value_or(vec3{-1, -1, -1}), vec3{2, 2, 0}));
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testTryEntityMoveRespectsDoorOnEntityFloorNotCurrentFloor() {
        gamestate g;
        add_floor(g);
        add_floor(g);
        g.d.current_floor = 0;

        const entityid npc = g.create_orc_at_with(vec3{1, 1, 1}, [](CT&, const entityid) {});
        const entityid door = g.create_door_at_with(vec3{2, 1, 1}, [](CT&, const entityid) {});

        TS_ASSERT_DIFFERS(npc, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);
        TS_ASSERT(!g.ct.get<door_open>(door).value_or(true));

        TS_ASSERT(!g.try_entity_move(npc, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(npc).value_or(vec3{-1, -1, -1}), vec3{1, 1, 1}));
    }

    void testTryEntityOpenDoorUsesDoorLocationFloor() {
        gamestate g;
        add_floor(g);
        add_floor(g);
        g.d.current_floor = 0;

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid door = g.create_door_at_with(vec3{2, 1, 1}, [](CT&, const entityid) {});

        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);
        TS_ASSERT(!g.ct.get<door_open>(door).value_or(true));

        TS_ASSERT(g.try_entity_open_door(hero, vec3{2, 1, 1}));
        TS_ASSERT(g.ct.get<door_open>(door).value_or(false));
    }

    void testPressurePlateOpensAndClosesLinkedDoorWhenPlayerMovesOnAndOff() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid door = g.create_door_at_with(vec3{4, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);
        TS_ASSERT(g.create_floor_pressure_plate(vec3{2, 1, 0}, door));

        floor_pressure_plate_t* plate = g.get_floor_pressure_plate(vec3{2, 1, 0});
        TS_ASSERT(plate != nullptr);
        TS_ASSERT(!plate->active);
        TS_ASSERT_EQUALS(plate->txkey_up, TX_SWITCHES_PRESSURE_PLATE_UP_00);
        TS_ASSERT_EQUALS(plate->txkey_down, TX_SWITCHES_PRESSURE_PLATE_DOWN_00);
        TS_ASSERT(g.try_entity_move(hero, vec3{1, 0, 0}));
        TS_ASSERT(g.ct.get<door_open>(door).value_or(false));
        TS_ASSERT(plate->active);

        TS_ASSERT(g.try_entity_move(hero, vec3{1, 0, 0}));
        TS_ASSERT(!g.ct.get<door_open>(door).value_or(true));
        TS_ASSERT(!plate->active);
    }

    void testPressurePlateOpensAndClosesLinkedDoorForPushableObject() {
        gamestate g;
        add_floor(g);

        const entityid door = g.create_door_at_with(vec3{4, 1, 0}, [](CT&, const entityid) {});
        const entityid box = g.create_box_at_with(vec3{1, 1, 0});
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(box, ENTITYID_INVALID);
        TS_ASSERT(g.create_floor_pressure_plate(vec3{2, 1, 0}, door));
        floor_pressure_plate_t* plate = g.get_floor_pressure_plate(vec3{2, 1, 0});
        TS_ASSERT(plate != nullptr);

        TS_ASSERT(g.try_entity_move(box, vec3{1, 0, 0}));
        TS_ASSERT(g.ct.get<door_open>(door).value_or(false));
        TS_ASSERT(plate->active);

        TS_ASSERT(g.try_entity_move(box, vec3{1, 0, 0}));
        TS_ASSERT(!g.ct.get<door_open>(door).value_or(true));
        TS_ASSERT(!plate->active);
    }

    void testRunMoveActionQueuesPressurePlateRefreshFollowUp() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid door = g.create_door_at_with(vec3{4, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);
        TS_ASSERT(g.create_floor_pressure_plate(vec3{2, 1, 0}, door));

        floor_pressure_plate_t* plate = g.get_floor_pressure_plate(vec3{2, 1, 0});
        TS_ASSERT(plate != nullptr);
        TS_ASSERT(!plate->active);

        TS_ASSERT(g.run_move_action(hero, vec3{1, 0, 0}));
        TS_ASSERT(g.ct.get<door_open>(door).value_or(false));
        TS_ASSERT(plate->active);
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testQueuedPressurePlateRefreshSchedulesDoorStateFollowup() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid door = g.create_door_at_with(vec3{4, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);
        TS_ASSERT(g.create_floor_pressure_plate(vec3{2, 1, 0}, door));

        floor_pressure_plate_t* plate = g.get_floor_pressure_plate(vec3{2, 1, 0});
        TS_ASSERT(plate != nullptr);
        const entityid box = g.create_box_at_with(vec3{2, 1, 0});
        TS_ASSERT_DIFFERS(box, ENTITYID_INVALID);
        plate->active = false;
        g.ct.set<door_open>(door, false);

        TS_ASSERT(g.queue_pressure_plate_refresh_event(0));
        const gameplay_event_result_t result = g.process_gameplay_events();

        TS_ASSERT_EQUALS(result.type, EVENT_REFRESH_PRESSURE_PLATES);
        TS_ASSERT(result.succeeded);
        TS_ASSERT(plate->active);
        TS_ASSERT(g.ct.get<door_open>(door).value_or(false));
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testRunPullActionQueuesPressurePlateRefreshFollowUp() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{2, 1, 0});
        g.ct.set<direction>(hero, DIR_RIGHT);
        const entityid box = g.create_box_at_with(vec3{3, 1, 0});
        const entityid door = g.create_door_at_with(vec3{5, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(box, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);
        TS_ASSERT(g.create_floor_pressure_plate(vec3{1, 1, 0}, door));

        floor_pressure_plate_t* plate = g.get_floor_pressure_plate(vec3{1, 1, 0});
        TS_ASSERT(plate != nullptr);
        TS_ASSERT(!plate->active);

        TS_ASSERT(g.run_pull_action(hero));
        TS_ASSERT(g.ct.get<door_open>(door).value_or(false));
        TS_ASSERT(plate->active);
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{1, 1, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(box).value_or(vec3{-1, -1, -1}), vec3{2, 1, 0}));
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testPressurePlateControlledDoorCannotBeOpenedManually() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid door = g.create_door_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);
        TS_ASSERT(g.create_floor_pressure_plate(vec3{3, 1, 0}, door));

        TS_ASSERT(g.try_entity_open_door(hero, vec3{2, 1, 0}));
        TS_ASSERT(!g.ct.get<door_open>(door).value_or(true));
        TS_ASSERT(g.msg_system_is_active);
        TS_ASSERT(!g.msg_system.empty());
        TS_ASSERT_EQUALS(g.msg_system.front(), "You cannot open this door with your hands");
    }

    void testRunOpenDoorActionUsesQueuedDoorIntent() {
        gamestate g;
        g.sfx.resize(71);
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid door = g.create_door_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);
        TS_ASSERT(!g.ct.get<door_open>(door).value_or(true));

        TS_ASSERT(g.run_open_door_action(hero, vec3{2, 1, 0}));
        TS_ASSERT(g.ct.get<door_open>(door).value_or(false));
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testRunOpenDoorActionRejectsPressurePlateControlledDoor() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid door = g.create_door_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);
        TS_ASSERT(g.create_floor_pressure_plate(vec3{3, 1, 0}, door));

        TS_ASSERT(g.run_open_door_action(hero, vec3{2, 1, 0}));
        TS_ASSERT(!g.ct.get<door_open>(door).value_or(true));
        TS_ASSERT(g.msg_system_is_active);
        TS_ASSERT(!g.msg_system.empty());
        TS_ASSERT_EQUALS(g.msg_system.front(), "You cannot open this door with your hands");
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testDestroyPressurePlateSeversLinkAndClosesDoor() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid door = g.create_door_at_with(vec3{4, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);
        TS_ASSERT(g.create_floor_pressure_plate(vec3{2, 1, 0}, door));
        TS_ASSERT(g.try_entity_move(hero, vec3{1, 0, 0}));
        TS_ASSERT(g.ct.get<door_open>(door).value_or(false));

        TS_ASSERT(g.destroy_floor_pressure_plate(vec3{2, 1, 0}));
        floor_pressure_plate_t* plate = g.get_floor_pressure_plate(vec3{2, 1, 0});
        TS_ASSERT(plate != nullptr);
        TS_ASSERT(plate->destroyed);
        TS_ASSERT_EQUALS(plate->linked_door_id, ENTITYID_INVALID);
        TS_ASSERT(!g.ct.get<door_open>(door).value_or(true));
        TS_ASSERT(!g.door_is_pressure_plate_controlled(door));
    }

    void testTryEntityOpenChestUsesChestLocationFloorAndOpensMenu() {
        gamestate g;
        g.sfx.resize(71);
        add_floor(g);
        add_floor(g);
        g.d.current_floor = 0;

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid chest = g.create_chest_at_with(vec3{2, 1, 1}, [](CT&, const entityid) {});

        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(chest, ENTITYID_INVALID);
        TS_ASSERT(!g.ct.get<door_open>(chest).value_or(true));

        TS_ASSERT(g.try_entity_open_chest(hero, vec3{2, 1, 1}));
        TS_ASSERT(g.ct.get<door_open>(chest).value_or(false));
        TS_ASSERT_EQUALS(g.active_chest_id, chest);
        TS_ASSERT(g.display_chest_menu);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_CHEST);
    }

    void testRunOpenChestActionUsesQueuedChestIntent() {
        gamestate g;
        g.sfx.resize(71);
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid chest = g.create_chest_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});

        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(chest, ENTITYID_INVALID);
        TS_ASSERT(!g.ct.get<door_open>(chest).value_or(true));

        TS_ASSERT(g.run_open_chest_action(hero, vec3{2, 1, 0}));
        TS_ASSERT(g.ct.get<door_open>(chest).value_or(false));
        TS_ASSERT_EQUALS(g.active_chest_id, chest);
        TS_ASSERT(g.display_chest_menu);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_CHEST);
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testRunOpenChestActionClosesExistingChestMenu() {
        gamestate g;
        g.sfx.resize(71);
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid chest = g.create_chest_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});

        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(chest, ENTITYID_INVALID);
        TS_ASSERT(g.run_open_chest_action(hero, vec3{2, 1, 0}));
        TS_ASSERT(g.display_chest_menu);
        TS_ASSERT_EQUALS(g.active_chest_id, chest);

        TS_ASSERT(g.run_open_chest_action(hero, vec3{2, 1, 0}));
        TS_ASSERT(!g.ct.get<door_open>(chest).value_or(true));
        TS_ASSERT_EQUALS(g.active_chest_id, ENTITYID_INVALID);
        TS_ASSERT(!g.display_chest_menu);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_PLAYER);
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testRunTraverseStairsActionUsesQueuedStairsIntent() {
        gamestate g;
        g.sfx.resize(71);
        add_floor(g);
        add_floor(g);

        auto floor0 = g.d.get_floor(0);
        auto floor1 = g.d.get_floor(1);
        TS_ASSERT(floor0->df_set_downstairs_loc(vec3{1, 1, 0}));
        TS_ASSERT(floor1->df_set_upstairs_loc(vec3{2, 2, 1}));

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        g.hero_id = hero;
        g.d.current_floor = 0;

        TS_ASSERT(g.run_traverse_stairs_action(hero));
        TS_ASSERT_EQUALS(g.d.current_floor, 1);
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{2, 2, 1}));
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testRunTraverseStairsActionReportsTopFloorMessage() {
        gamestate g;
        add_floor(g);

        auto floor0 = g.d.get_floor(0);
        TS_ASSERT(floor0->df_set_upstairs_loc(vec3{1, 1, 0}));

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        g.hero_id = hero;
        g.d.current_floor = 0;

        TS_ASSERT(!g.run_traverse_stairs_action(hero));
        TS_ASSERT_EQUALS(g.d.current_floor, 0);
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{1, 1, 0}));
        TS_ASSERT(g.msg_system_is_active);
        TS_ASSERT(!g.msg_system.empty());
        TS_ASSERT_EQUALS(g.msg_system.front(), "You are already on the top floor!");
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testHandleOpenDoorUsesKeyDInsteadOfKeyO() {
        gamestate g;
        g.sfx.resize(71);
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid door = g.create_door_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);

        g.hero_id = hero;
        g.ct.set<direction>(hero, DIR_RIGHT);

        inputstate is = {};
        press_key(is, KEY_D);
        TS_ASSERT(g.handle_open_door(is, false));
        TS_ASSERT(g.ct.get<door_open>(door).value_or(false));

        g.ct.set<door_open>(door, false);
        inputstate_reset(is);
        press_key(is, KEY_O);
        TS_ASSERT(!g.handle_open_door(is, false));
        TS_ASSERT(!g.ct.get<door_open>(door).value_or(true));
    }

    void testTryEntityInteractOpensNpcDialogueModal() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid npc = g.create_npc_at_with(RACE_DWARF, vec3{2, 1, 0}, [](CT& ct, const entityid id) {
            ct.set<name>(id, "Borin");
            ct.set<dialogue_text>(id, "Keep your torch high.");
        });

        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(npc, ENTITYID_INVALID);
        TS_ASSERT(g.try_entity_interact(hero, vec3{2, 1, 0}));
        TS_ASSERT(g.display_interaction_modal);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_INTERACTION);
        TS_ASSERT_EQUALS(g.active_interaction_entity_id, npc);
        TS_ASSERT_EQUALS(g.interaction_title, "Borin");
        TS_ASSERT_EQUALS(g.interaction_body, "Keep your torch high.");
    }

    void testRunInteractActionUsesQueuedInteractIntent() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid npc = g.create_npc_at_with(RACE_DWARF, vec3{2, 1, 0}, [](CT& ct, const entityid id) {
            ct.set<name>(id, "Borin");
            ct.set<dialogue_text>(id, "Keep your torch high.");
        });

        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(npc, ENTITYID_INVALID);
        TS_ASSERT(g.run_interact_action(hero, vec3{2, 1, 0}));
        TS_ASSERT(g.display_interaction_modal);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_INTERACTION);
        TS_ASSERT_EQUALS(g.active_interaction_entity_id, npc);
        TS_ASSERT_EQUALS(g.interaction_title, "Borin");
        TS_ASSERT_EQUALS(g.interaction_body, "Keep your torch high.");
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testRunInteractActionReturnsFalseWhenNothingIsThere() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);

        TS_ASSERT(!g.run_interact_action(hero, vec3{2, 1, 0}));
        TS_ASSERT(!g.display_interaction_modal);
        TS_ASSERT(g.gameplay_events.empty());
    }

    void testTryEntityInteractOpensPropDescriptionModal() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid prop = g.create_prop_at_with(PROP_DUNGEON_JAR_00, vec3{2, 1, 0}, [](CT& ct, const entityid id) {
            ct.set<name>(id, "jar");
            ct.set<description>(id, "A cracked jar with dry clay inside.");
        });

        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(prop, ENTITYID_INVALID);
        TS_ASSERT(g.try_entity_interact(hero, vec3{2, 1, 0}));
        TS_ASSERT(g.display_interaction_modal);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_INTERACTION);
        TS_ASSERT_EQUALS(g.active_interaction_entity_id, prop);
        TS_ASSERT_EQUALS(g.interaction_title, "jar");
        TS_ASSERT_EQUALS(g.interaction_body, "A cracked jar with dry clay inside.");
    }

    void testTryEntityInteractOpensBoxDescriptionModal() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid box = g.create_box_at_with(vec3{2, 1, 0});

        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(box, ENTITYID_INVALID);
        TS_ASSERT(g.try_entity_interact(hero, vec3{2, 1, 0}));
        TS_ASSERT(g.display_interaction_modal);
        TS_ASSERT_EQUALS(g.active_interaction_entity_id, box);
        TS_ASSERT_EQUALS(g.interaction_title, "box");
        TS_ASSERT_EQUALS(g.interaction_body, "A plain wooden box with rough handles cut into the sides for hauling.");
    }

    void testTryEntityInteractOpensChestDescriptionModal() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid chest = g.create_chest_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});

        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(chest, ENTITYID_INVALID);
        TS_ASSERT(g.try_entity_interact(hero, vec3{2, 1, 0}));
        TS_ASSERT(g.display_interaction_modal);
        TS_ASSERT_EQUALS(g.active_interaction_entity_id, chest);
        TS_ASSERT_EQUALS(g.interaction_title, "treasure chest");
        TS_ASSERT_EQUALS(g.interaction_body, "A stout treasure chest reinforced with iron bands and built to survive rough handling. The lid stands closed.");

        g.close_interaction_modal();
        g.ct.set<door_open>(chest, true);
        TS_ASSERT(g.try_entity_interact(hero, vec3{2, 1, 0}));
        TS_ASSERT_EQUALS(g.interaction_body, "A stout treasure chest reinforced with iron bands and built to survive rough handling. The lid stands open.");
    }

    void testTryEntityInteractOpensDoorDescriptionModal() {
        gamestate g;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid door = g.create_door_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});

        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);
        TS_ASSERT(g.try_entity_interact(hero, vec3{2, 1, 0}));
        TS_ASSERT(g.display_interaction_modal);
        TS_ASSERT_EQUALS(g.active_interaction_entity_id, door);
        TS_ASSERT_EQUALS(g.interaction_title, "door");
        TS_ASSERT_EQUALS(g.interaction_body, "A heavy wooden door bound with iron straps and swollen from the dungeon damp. The door stands closed.");

        g.close_interaction_modal();
        g.ct.set<door_open>(door, true);
        TS_ASSERT(g.try_entity_interact(hero, vec3{2, 1, 0}));
        TS_ASSERT_EQUALS(g.interaction_body, "A heavy wooden door bound with iron straps and swollen from the dungeon damp. The door stands open.");
    }

    void testHandleInputInteractionClosesModalAndRestoresPlayerControl() {
        gamestate g;
        add_floor(g);
        g.test = true;
        g.controlmode = CONTROLMODE_PLAYER;
        g.open_interaction_modal(42, "Speaker", "Line");

        inputstate is{};
        press_key(is, KEY_ENTER);
        g.handle_input_interaction(is);

        TS_ASSERT(!g.display_interaction_modal);
        TS_ASSERT_EQUALS(g.controlmode, CONTROLMODE_PLAYER);
        TS_ASSERT_EQUALS(g.active_interaction_entity_id, ENTITYID_INVALID);
        TS_ASSERT(g.interaction_title.empty());
        TS_ASSERT(g.interaction_body.empty());
    }

    void testUpdatePlayerTilesExploredRevealsClosedDoorTileButNotBeyond() {
        gamestate g;
        add_floor(g);
        g.current_scene = SCENE_GAMEPLAY;

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        g.hero_id = hero;
        g.ct.set<light_radius>(hero, 4);

        const entityid door = g.create_door_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);
        TS_ASSERT(!g.ct.get<door_open>(door).value_or(true));

        TS_ASSERT(g.update_player_tiles_explored());

        tile_t& door_tile = g.d.get_floor(0)->tile_at(vec3{2, 1, 0});
        tile_t& beyond_tile = g.d.get_floor(0)->tile_at(vec3{3, 1, 0});
        TS_ASSERT(door_tile.get_visible());
        TS_ASSERT(door_tile.get_explored());
        TS_ASSERT(!beyond_tile.get_visible());
        TS_ASSERT(!beyond_tile.get_explored());
    }

    void testUpdatePlayerTilesExploredClearsVisibilityBeyondClosedDoorAfterClosing() {
        gamestate g;
        add_floor(g);
        g.current_scene = SCENE_GAMEPLAY;

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        g.hero_id = hero;
        g.ct.set<light_radius>(hero, 4);

        const entityid door = g.create_door_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(door, ENTITYID_INVALID);

        g.ct.set<door_open>(door, true);
        TS_ASSERT(g.update_player_tiles_explored());

        tile_t& beyond_tile = g.d.get_floor(0)->tile_at(vec3{3, 1, 0});
        TS_ASSERT(beyond_tile.get_visible());
        TS_ASSERT(beyond_tile.get_explored());

        g.ct.set<door_open>(door, false);
        TS_ASSERT(g.update_player_tiles_explored());

        TS_ASSERT(!beyond_tile.get_visible());
        TS_ASSERT(beyond_tile.get_explored());
    }

    void testCanMoveOntoTileContainingFreshlyKilledDeadNpc() {
        gamestate g;
        g.test = true;
        add_floor(g);

        const entityid hero = create_hero(g, vec3{1, 1, 0});
        const entityid orc = g.create_orc_at_with(vec3{2, 1, 0}, [](CT&, const entityid) {});
        TS_ASSERT_DIFFERS(hero, ENTITYID_INVALID);
        TS_ASSERT_DIFFERS(orc, ENTITYID_INVALID);

        g.ct.set<hp>(orc, vec2{1, 1});
        tile_t& target_tile = g.d.get_floor(0)->tile_at(vec3{2, 1, 0});
        g.process_attack_results(target_tile, hero, orc, true);

        TS_ASSERT(g.ct.get<dead>(orc).value_or(false));
        TS_ASSERT_EQUALS(target_tile.get_cached_live_npc(), ENTITYID_INVALID);
        TS_ASSERT_EQUALS(target_tile.get_cached_dead_npc(), orc);

        TS_ASSERT(g.try_entity_move(hero, vec3{1, 0, 0}));
        TS_ASSERT(vec3_equal(g.ct.get<location>(hero).value_or(vec3{-1, -1, -1}), vec3{2, 1, 0}));
        TS_ASSERT_EQUALS(target_tile.get_cached_live_npc(), hero);
        TS_ASSERT_EQUALS(target_tile.get_cached_dead_npc(), orc);
    }
};
