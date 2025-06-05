//#include "test_btree_int.h"
#include "test_component_table.h"
//#include "test_dungeon.h"
//#include "test_dungeon_floor.h"
#include "test_dungeon_tile.h"
#include "test_gamestate.h"

int main(void) {
    test_component_table();
    test_gamestate();
    test_dungeon_tiles();
    //test_dungeon_floors();
    //test_dungeon();
    //test_btree_int_tests();
    unit_test_summary();
    return 0;
}
