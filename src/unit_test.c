#include "component_table_test.h"
#include "gamestate.h"
#include "unit_test.h"

TEST(test_gamestate_create_destroy) {
    gamestate* g = gamestateinitptr();
    ASSERT(g != NULL, "gamestate creation failed");
    gamestatefree(g);
}

TEST(test_gamestate_add_entity) {
    gamestate* g = gamestateinitptr();
    ASSERT(g != NULL, "gamestate creation failed");
    entityid id = g_add_entity(g);
    ASSERT(id != ENTITYID_INVALID, "entity creation failed");
    // Add and verify some components
    ASSERT(g_add_name(g, id, "Test Entity"), "add name failed");
    ASSERT(g_has_name(g, id), "name should exist");
    ASSERT(strcmp(g_get_name(g, id), "Test Entity") == 0, "name should match");
    ASSERT(g_add_type(g, id, ENTITY_PLAYER), "add type failed");
    ASSERT(g_has_type(g, id), "type should exist");
    ASSERT(g_get_type(g, id) == ENTITY_PLAYER, "type should be ENTITY_PLAYER");
    loc_t loc = {1, 2, 0};
    ASSERT(g_add_location(g, id, loc), "add location failed");
    ASSERT(g_has_location(g, id), "location should exist");
    loc_t actual_loc = g_get_location(g, id);
    ASSERT(actual_loc.x == loc.x && actual_loc.y == loc.y, "location should match");
    gamestatefree(g);
}

TEST(test_gamestate_multiple_entities) {
    gamestate* g = gamestateinitptr();
    ASSERT(g != NULL, "gamestate creation failed");
    // Create first entity
    entityid id1 = g_add_entity(g);
    ASSERT(id1 != ENTITYID_INVALID, "entity creation failed");
    ASSERT(g_add_name(g, id1, "Entity 1"), "add name failed");
    ASSERT(g_add_type(g, id1, ENTITY_NPC), "add type failed");
    ASSERT(g_add_location(g, id1, (loc_t){3, 4, 0}), "add location failed");
    // Create second entity
    entityid id2 = g_add_entity(g);
    ASSERT(id2 != ENTITYID_INVALID, "entity creation failed");
    ASSERT(g_add_name(g, id2, "Entity 2"), "add name failed");
    ASSERT(g_add_type(g, id2, ENTITY_ITEM), "add type failed");
    ASSERT(g_add_location(g, id2, (loc_t){5, 6, 0}), "add location failed");
    // Verify first entity
    ASSERT(g_has_name(g, id1), "entity 1 should have name");
    ASSERT(strcmp(g_get_name(g, id1), "Entity 1") == 0, "entity 1 name should match");
    ASSERT(g_get_type(g, id1) == ENTITY_NPC, "entity 1 type should be ENTITY_NPC");
    loc_t loc1 = g_get_location(g, id1);
    ASSERT(loc1.x == 3 && loc1.y == 4, "entity 1 location should match");
    // Verify second entity
    ASSERT(g_has_name(g, id2), "entity 2 should have name");
    ASSERT(strcmp(g_get_name(g, id2), "Entity 2") == 0, "entity 2 name should match");
    ASSERT(g_get_type(g, id2) == ENTITY_ITEM, "entity 2 type should be ENTITY_ITEM");
    loc_t loc2 = g_get_location(g, id2);
    ASSERT(loc2.x == 5 && loc2.y == 6, "entity 2 location should match");
    // Verify entity counts
    ASSERT(g->next_entityid == 2, "should have 2 entities");
    ASSERT(g->name_list_count == 2, "should have 2 name components");
    ASSERT(g->type_list_count == 2, "should have 2 type components");
    ASSERT(g->loc_list_count == 2, "should have 2 location components");
    gamestatefree(g);
}

TEST(test_gamestate_components) {
    gamestate* g = gamestateinitptr();
    ASSERT(g != NULL, "gamestate creation failed");
    
    entityid id = g_add_entity(g);
    ASSERT(id != ENTITYID_INVALID, "entity creation failed");

    // Test basic components
    ASSERT(g_add_name(g, id, "Test Entity"), "add name failed");
    ASSERT(g_has_name(g, id), "name should exist");
    ASSERT(strcmp(g_get_name(g, id), "Test Entity") == 0, "name should match");

    ASSERT(g_add_type(g, id, ENTITY_PLAYER), "add type failed");
    ASSERT(g_has_type(g, id), "type should exist");
    ASSERT(g_get_type(g, id) == ENTITY_PLAYER, "type should be ENTITY_PLAYER");

    loc_t loc = {10, 20, 0};
    ASSERT(g_add_location(g, id, loc), "add location failed");
    ASSERT(g_has_location(g, id), "location should exist");
    loc_t actual_loc = g_get_location(g, id);
    ASSERT(actual_loc.x == loc.x && actual_loc.y == loc.y, "location should match");

    // Test stats components
    ASSERT(g_add_stats(g, id), "add stats failed");
    ASSERT(g_has_stats(g, id), "stats should exist");
    ASSERT(g_set_stat(g, id, STATS_HP, 100), "set stat failed");
    ASSERT(g_get_stat(g, id, STATS_HP) == 100, "get stat failed");

    // Test equipment components
    ASSERT(g_add_equipment(g, id), "add equipment failed");
    ASSERT(g_has_equipment(g, id), "equipment should exist");
    entityid weapon_id = g_add_entity(g);
    ASSERT(g_set_equipment(g, id, EQUIP_SLOT_WEAPON, weapon_id), "set equipment failed");
    ASSERT(g_get_equipment(g, id, EQUIP_SLOT_WEAPON) == weapon_id, "get equipment failed");

    // Test inventory components
    ASSERT(g_add_inventory(g, id), "add inventory failed");
    ASSERT(g_has_inventory(g, id), "inventory should exist");
    entityid item_id = g_add_entity(g);
    ASSERT(g_add_to_inventory(g, id, item_id), "add to inventory failed");
    ASSERT(g_has_item_in_inventory(g, id, item_id), "item should be in inventory");

    // Test damage components
    roll damage_roll = {1, 6, 2};
    ASSERT(g_add_damage(g, id, damage_roll), "add damage failed");
    ASSERT(g_has_damage(g, id), "damage should exist");
    roll actual_damage = g_get_damage(g, id);
    ASSERT(actual_damage.n == damage_roll.n && 
           actual_damage.sides == damage_roll.sides && 
           actual_damage.modifier == damage_roll.modifier, "damage should match");

    // Test AC components
    ASSERT(g_add_ac(g, id, 15), "add AC failed");
    ASSERT(g_has_ac(g, id), "AC should exist");
    ASSERT(g_get_ac(g, id) == 15, "AC should match");

    gamestatefree(g);
}

static void test_gamestate(void) {
    run_test_gamestate_create_destroy();
    run_test_gamestate_add_entity();
    run_test_gamestate_multiple_entities();
    run_test_gamestate_components();
}

int main(void) {
    test_component_table();
    test_gamestate();
    unit_test_summary();
    return 0;
}
