#include "entityid.h"
#include "item_container.h"
#include "massert.h"
#include "mprint.h"
#include <cstdlib>

using std::make_shared;


item_container::item_container(int m, int n) {
    minfo("Begin create new item container");
    if (m <= 0 || n <= 0) {
        merror("Both m and n must be > 0!");
        exit(-1);
    }
    grid = make_shared<vector<entityid>>();
    for (int i = 0; i < m * n; i++) {
        grid->push_back(ENTITYID_INVALID);
    }
    rows = m;
    cols = n;
    next_index = 0;
    msuccess("End create new item container");
}


bool item_container::set(entityid id, int x, int y) {
    // find the first i,j with ENTITYID_INVALID and replace with id, return true
    // otherwise, return false
    massert(x >= 0 && x < cols && y >= 0 && y < rows, "x or y out-of-bounds");
    (*grid)[y * cols + x] = id;
    return true;
}


bool item_container::set(entityid id, int i) {
    massert(i >= 0 && i < cols * rows, "i out-of-bounds");
    (*grid)[i] = id;
    return true;
}


entityid item_container::get(int x, int y) {
    massert(x >= 0 && x < cols && y >= 0 && y < rows, "x or y out-of-bounds");
    return (*grid)[y * cols + x];
}


entityid item_container::remove(int x, int y) {
    // if x or y out of bounds, return ENTITYID_INVALID
    if (x < 0 || x >= cols || y < 0 || y >= rows) {
        return ENTITYID_INVALID;
    }
    entityid item_id = get(x, y);
    set(ENTITYID_INVALID, x, y);
    return item_id;
}


bool item_container::add(entityid id) {
    massert(next_index >= 0, "next index is less-than-zero");
    if (next_index >= grid->size()) {
        merror("next index is greater than the vector size");
        return false;
    }
    if (id == ENTITYID_INVALID) {
        merror("entityid is invalid");
        return false;
    }
    set(id, next_index++);
    return true;
}
