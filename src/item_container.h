#pragma once

#include "entityid.h"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;


class item_container {
public:
    item_container(int m, int n);

    bool set(entityid id, int x, int y);
    bool set(entityid id, int i);
    entityid get(int x, int y);
    entityid remove(int x, int y);
    bool add(entityid id);

private:
    shared_ptr<vector<entityid>> grid;
    int rows;
    int cols;

    int next_index;
};
