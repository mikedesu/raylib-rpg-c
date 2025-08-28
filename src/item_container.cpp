#include "entityid.h"
#include "item_container.h"
//#include "massert.h"
#include "mprint.h"
#include <cstdlib>

using std::make_shared;


item_container::item_container(int m, int n) {
    if (m <= 0 || n <= 0) {
        merror("Both m and n must be > 0!");
        exit(-1);
    }
    grid.clear();
    grid.reserve(m);
    for (int i = 0; i < m; i++) {
        shared_ptr<vector<entityid>> row = make_shared<vector<entityid>>();
        row->reserve(n);
        for (int j = 0; j < n; j++) {
            (*row)[j] = ENTITYID_INVALID;
        }
        //grid.push_back(row);
        grid[i] = row;
    }
    rows = m;
    cols = n;
}


bool item_container::add(entityid id) {
    // find the first i,j with ENTITYID_INVALID and replace with id, return true
    // otherwise, return false
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if ((*(grid[i]))[j] == ENTITYID_INVALID) {
                (*(grid[i]))[j] = id;
                return true;
            }
        }
    }
    return false;
}


entityid item_container::remove(entityid id) {
    // find the first i,j with id and replace with ENTITYID_INVALID, return true
    // otherwise, return false
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if ((*(grid[i]))[j] == id) {
                (*(grid[i]))[j] = ENTITYID_INVALID;
                return id;
            }
        }
    }
    return ENTITYID_INVALID;
}


entityid item_container::remove(int x, int y) {
    // if x or y out of bounds, return ENTITYID_INVALID
    if (x < 0 || x >= cols || y < 0 || y >= rows) {
        return ENTITYID_INVALID;
    }
    entityid id = (*(grid[y]))[x];
    (*(grid[y]))[x] = ENTITYID_INVALID;
    return id;
}
