#pragma once

#include "entityid.h"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;


class item_container {
public:
    item_container(int m, int n);

    bool add(entityid id);
    entityid remove(entityid id);
    entityid remove(int x, int y);

private:
    shared_ptr<vector<shared_ptr<vector<entityid>>>> grid;
    int rows;
    int cols;
};
