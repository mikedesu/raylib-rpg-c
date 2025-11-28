#include "dungeon.h"
#include "dungeon_floor.h"
//#include "dungeon_type.h"
#include "dungeon_floor_type.h"
#include "massert.h"
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>


using std::make_shared;
using std::shared_ptr;
using std::vector;


#define INITIAL_DUNGEON_CAPACITY 4


shared_ptr<dungeon_t> d_create() {
    shared_ptr<dungeon_t> dungeon = make_shared<dungeon_t>();
    if (!dungeon) return nullptr;
    dungeon->floors = make_shared<vector<shared_ptr<dungeon_floor_t>>>();
    if (!dungeon->floors) return nullptr;
    dungeon->current_floor = 0;
    dungeon->is_locked = false;
    return dungeon;
}


void d_destroy(shared_ptr<dungeon_t> d) {
    if (d && d->floors) d->floors->clear();
}


void d_free(shared_ptr<dungeon_t> dungeon) {
    //massert(dungeon, "dungeon is NULL");
    d_destroy(dungeon);
}


shared_ptr<dungeon_floor_t> d_add_floor(shared_ptr<dungeon_t> dungeon, dungeon_floor_type_t type, int width, int height) {
    if (!dungeon || width <= 0 || height <= 0 || dungeon->is_locked) {
        return nullptr;
    }

    const int current_floor = dungeon->floors->size();

    shared_ptr<dungeon_floor_t> new_floor = df_create(current_floor, type, width, height);

    if (!new_floor) {
        return nullptr;
    }

    df_assign_upstairs_in_area(new_floor, 0, 0, width, height);
    df_assign_downstairs_in_area(new_floor, 0, 0, width, height);

    //dungeon->floors->push_back(shared_ptr<dungeon_floor_t>(new_floor));
    dungeon->floors->push_back(new_floor);

    return new_floor;
}
