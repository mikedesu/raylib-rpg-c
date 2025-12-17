#pragma once

#include <raylib.h>
#include <string>

using std::string;


static inline string get_random_orc_name() {
    const int r = GetRandomValue(0, 4);
    switch (r) {
    case 0: return "Kratog";
    case 1: return "Baluk";
    case 2: return "Gorag";
    case 3: return "Valuz";
    case 4: return "Zikool";
    default: break;
    }
    return "Zugzug";
}
