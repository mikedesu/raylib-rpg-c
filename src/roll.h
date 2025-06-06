#pragma once

#include "massert.h"
#include "vec3.h"
#include <stdlib.h>

static inline int do_roll(vec3 r) {
    massert(r.x > 0, "n is negative");
    massert(r.y > 0, "sides is negative");
    int total = 0;
    // if sides is 1, then we just return n + modifier
    if (r.y == 1) return r.x + r.z;
    for (int i = 0; i < r.x; i++) total += (rand() % r.y) + 1;
    return total + r.z;
}

static inline int do_roll_best_of_3(vec3 r) {
    massert(r.x > 0, "n is negative");
    massert(r.y > 0, "sides is negative");
    int a = do_roll(r), b = do_roll(r), c = do_roll(r);
    return a > b ? (a > c ? a : c) : (b > c ? b : c);
}
