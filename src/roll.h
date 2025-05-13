#pragma once

#include <stdlib.h>

typedef struct roll_t {
    int n;
    int sides;
    int modifier;
} roll;

static inline int do_roll(roll r) {
    int total = 0;
    for (int i = 0; i < r.n; i++) total += (rand() % r.sides) + 1;
    return total + r.modifier;
}
