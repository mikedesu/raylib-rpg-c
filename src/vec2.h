#pragma once

typedef struct vec2_t {
    int x;
    int y;
} vec2;

constexpr static inline bool vec2_equal(vec2 a, vec2 b) {
    return a.x == b.x && a.y == b.y;
}

constexpr static inline bool vec2_invalid(vec2 a) {
    return vec2_equal(a, vec2{-1, -1});
}

constexpr static inline bool vec2_valid(vec2 a) {
    return !vec2_invalid(a);
}

constexpr static inline vec2 vec2_add(vec2 a, vec2 b) {
    return vec2{a.x + b.x, a.y + b.y};
}

constexpr static inline vec2 vec2_sub(vec2 a, vec2 b) {
    return vec2{a.x - b.x, a.y - b.y};
}
