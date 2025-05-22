#include "component_table_test.h"
#include "gamestate_test.h"
#include "dungeon_tile.h"

TEST(test_tile_creation) {
    tile_t* tile = tile_create(TILE_NONE);
    ASSERT(tile != NULL, "Failed to create tile");
    ASSERT(tile->type == TILE_NONE, "Tile type not initialized correctly");
    ASSERT(tile->entity_count == 0, "Entity count not initialized correctly");
    ASSERT(tile->entity_max == DUNGEON_TILE_MAX_ENTITIES_DEFAULT, "Entity max not initialized correctly");
    tile_free(tile);
}

int main(void) {
    test_component_table();
    test_gamestate();
    run_test_tile_creation();
    unit_test_summary();
    return 0;
}
