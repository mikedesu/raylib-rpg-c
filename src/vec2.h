/** @file vec2.h
 *  @brief Minimal integer 2D vector helpers used by gameplay state.
 */

#pragma once

/// @brief Integer 2D vector used for hit points and other paired values.
typedef struct vec2_t {
    int x;
    int y;
} vec2;

/** @brief Return whether two `vec2` values are identical. */
constexpr static inline bool vec2_equal(vec2 a, vec2 b) {
    return a.x == b.x && a.y == b.y;
}

/** @brief Return whether a vector uses the project's invalid sentinel `{-1, -1}`. */
constexpr static inline bool vec2_invalid(vec2 a) {
    return vec2_equal(a, vec2{-1, -1});
}

/** @brief Return whether a vector does not use the invalid sentinel. */
constexpr static inline bool vec2_valid(vec2 a) {
    return !vec2_invalid(a);
}

/** @brief Return the component-wise sum of two vectors. */
constexpr static inline vec2 vec2_add(vec2 a, vec2 b) {
    return vec2{a.x + b.x, a.y + b.y};
}

/** @brief Return the component-wise difference of two vectors. */
constexpr static inline vec2 vec2_sub(vec2 a, vec2 b) {
    return vec2{a.x - b.x, a.y - b.y};
}
