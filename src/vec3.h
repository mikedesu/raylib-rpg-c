/** @file vec3.h
 *  @brief Minimal integer 3D vector helpers for dungeon-space coordinates.
 */

#pragma once

/// @brief Integer 3D vector used for dungeon tile coordinates `(x, y, floor)`.
typedef struct vec3_t {
    int x;
    int y;
    int z;
} vec3;

/** @brief Return whether two `vec3` values are identical. */
constexpr static inline bool vec3_equal(vec3 a, vec3 b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

/** @brief Return whether a vector uses the project's invalid sentinel `{-1, -1, -1}`. */
constexpr static inline bool vec3_invalid(vec3 a) {
    return vec3_equal(a, vec3{-1, -1, -1});
}

/** @brief Return whether a vector does not use the invalid sentinel. */
constexpr static inline bool vec3_valid(vec3 a) {
    return !vec3_invalid(a);
}

/** @brief Return the component-wise sum of two vectors. */
constexpr static inline vec3 vec3_add(vec3 a, vec3 b) {
    return vec3{a.x + b.x, a.y + b.y, a.z + b.z};
}

/** @brief Return the component-wise difference of two vectors. */
constexpr static inline vec3 vec3_sub(vec3 a, vec3 b) {
    return vec3{a.x - b.x, a.y - b.y, a.z - b.z};
}

/** @brief Provide strict weak ordering so `vec3` can be used in ordered containers. */
constexpr static inline bool operator<(const vec3& a, const vec3& b) {
    if (a.x != b.x) {
        return a.x < b.x;
    }

    if (a.y != b.y) {
        return a.y < b.y;
    }

    return a.z < b.z;
}
