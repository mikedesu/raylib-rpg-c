#include "test_component_table.h"
#include "test_dungeon_tile.h"
#include "test_gamestate.h"

int main(void) {
    test_component_table();
    test_gamestate();
    run_test_tile_creation();
    unit_test_summary();
    return 0;
}
