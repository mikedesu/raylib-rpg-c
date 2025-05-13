#pragma once

#include "massert.h"
#include <stdlib.h>

typedef struct roll_t {
    int n;
    int sides;
    int modifier;
} roll;

static inline int do_roll(roll r) {
    massert(r.n > 0, "n is negative");
    massert(r.sides > 0, "sides is negative");
    int total = 0;
    // if sides is 1, then we just return n + modifier
    if (r.sides == 1) {
        return r.n + r.modifier;
    }
    for (int i = 0; i < r.n; i++) total += (rand() % r.sides) + 1;
    return total + r.modifier;
}
