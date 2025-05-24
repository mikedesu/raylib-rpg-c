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

static inline int do_roll_best_of_3(roll r) {
    massert(r.n > 0, "n is negative");
    massert(r.sides > 0, "sides is negative");
    int a, b, c;
    a = do_roll(r);
    b = do_roll(r);
    c = do_roll(r);
    int best = a > b ? (a > c ? a : c) : (b > c ? b : c);
    return best;
}
