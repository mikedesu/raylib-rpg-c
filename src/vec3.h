#pragma once

typedef struct vec3_t {
    int x;
    int y;
    int z;
} vec3;

constexpr static inline bool vec3_equal(const vec3 a, const vec3 b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

constexpr static inline bool vec3_invalid(const vec3 a) {
    return vec3_equal(a, (vec3){-1, -1, -1});
}
