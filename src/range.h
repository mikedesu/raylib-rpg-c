#pragma once

#include <stdlib.h>

typedef struct range_t {
    int min;
    int max;
} range;

static inline int get_random_in_range(range r) {
    if (r.min > r.max) {
        int temp = r.min;
        r.min = r.max;
        r.max = temp;
    }
    return rand() % (r.max - r.min + 1) + r.min;
}
