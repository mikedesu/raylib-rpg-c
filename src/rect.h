#pragma once

typedef struct rect_t {
    int x;
    int y;
    int w;
    int h;
} rect;

static inline int rect_equal(const rect a, const rect b) { return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h; }
