#pragma once

typedef struct vec3_t {
    int x;
    int y;
    int z;
} vec3;


constexpr static inline bool vec3_equal(vec3 a, vec3 b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}


constexpr static inline bool vec3_invalid(vec3 a) {
    return vec3_equal(a, vec3{-1, -1, -1});
}


constexpr static inline bool vec3_valid(vec3 a) {
    return !vec3_invalid(a);
}


constexpr static inline vec3 vec3_add(vec3 a, vec3 b) {
    return vec3{a.x + b.x, a.y + b.y, a.z + b.z};
}


constexpr static inline vec3 vec3_sub(vec3 a, vec3 b) {
    return vec3{a.x - b.x, a.y - b.y, a.z - b.z};
}


constexpr static inline bool operator<(const vec3& a, const vec3& b) {
    if (a.x != b.x) {
        return a.x < b.x;
    }

    if (a.y != b.y) {
        return a.y < b.y;
    }

    return a.z < b.z;
}
