#include "component_table_test.h"
#include "gamestate_test.h"
//#include "unit_test.h"

int main(void) {
    test_component_table();
    test_gamestate();
    unit_test_summary();
    return 0;
}
