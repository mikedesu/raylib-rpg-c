/** @file inputstate.h
 *  @brief Compact per-frame keyboard and mouse input snapshot helpers.
 */

#pragma once

#include <cstdio>
#include <memory>
#include <raylib.h>
#include <stdint.h>

/// @brief Maximum number of Raylib key codes tracked by the bitset input cache.
#define MAX_KEYS 400
/// @brief Number of bits packed into each keyboard-state storage slot.
#define BITS_PER_LONG 64
/// @brief Number of 64-bit slots needed to store `MAX_KEYS` bits.
#define NUM_LONGS ((MAX_KEYS + BITS_PER_LONG - 1) / BITS_PER_LONG) // 13

/**
 * @brief Frame-local snapshot of keyboard and mouse state.
 *
 * `pressed` stores edge-triggered key presses for the current frame while
 * `held` stores the continuously held keys sampled from Raylib.
 */
typedef struct inputstate {
    uint64_t pressed[NUM_LONGS]; // Bits for keys pressed this frame
    uint64_t held[NUM_LONGS]; // Bits for keys held down
    bool mouse_pressed[3]; // Left, Right, Middle buttons pressed this frame
    bool mouse_held[3]; // Left, Right, Middle buttons held down
    bool mouse_released[3]; // Left, Right, Middle buttons released this frame
    Vector2 mouse_position; // Current mouse position
} inputstate;

/** @brief Clear all cached key and mouse button state for a fresh frame. */
static inline void inputstate_reset(inputstate& is) {
    for (int i = 0; i < NUM_LONGS; i++)
        is.pressed[i] = is.held[i] = 0;
    for (int i = 0; i < 3; i++)
        is.mouse_pressed[i] = is.mouse_held[i] = is.mouse_released[i] = false;
}

/** @brief Sample keyboard and mouse state from Raylib into the snapshot. */
static inline void inputstate_update(inputstate& is) {
    inputstate_reset(is);
    // Update keyboard state
    for (int k = 0; k < MAX_KEYS; k++) {
        if (IsKeyPressed(k)) {
            const int idx = k / BITS_PER_LONG, bit = k % BITS_PER_LONG;
            is.pressed[idx] |= (1ULL << bit);
        }
        if (IsKeyDown(k)) {
            const int idx = k / BITS_PER_LONG, bit = k % BITS_PER_LONG;
            is.held[idx] |= (1ULL << bit);
        }
    }
    // Update mouse state
    is.mouse_position = GetMousePosition();
    for (int button = 0; button < 3; button++) {
        is.mouse_pressed[button] = IsMouseButtonPressed(button);
        is.mouse_held[button] = IsMouseButtonDown(button);
        is.mouse_released[button] = IsMouseButtonReleased(button);
    }
}

/** @brief Return whether a key transitioned to pressed during the current frame. */
static inline const bool inputstate_is_pressed(const inputstate& is, int key) {
    if (key < 0 || key >= MAX_KEYS) {
        return false;
    }
    const int idx = key / BITS_PER_LONG, bit = key % BITS_PER_LONG;
    return (is.pressed[idx] & (1ULL << bit)) != 0;
}

/** @brief Return whether any tracked key was pressed during the current frame. */
static inline bool inputstate_any_pressed(const inputstate& is) {
    for (int idx = 0; idx < NUM_LONGS; idx++) {
        if (is.pressed[idx] != 0) {
            return true; // At least one key pressed
        }
    }
    return false;
}

/** @brief Return whether a key is currently being held down. */
static inline const bool inputstate_is_held(const inputstate& is, int key) {
    if (key < 0 || key >= MAX_KEYS) {
        return false;
    }
    const int idx = key / BITS_PER_LONG, bit = key % BITS_PER_LONG;
    return (is.held[idx] & (1ULL << bit)) != 0;
}

/** @brief Return the first pressed key code found in the current frame, or `-1`. */
static inline int inputstate_get_pressed_key(const inputstate& is) {
    for (int idx = 0; idx < NUM_LONGS; idx++) {
        const uint64_t bits = is.pressed[idx];
        if (bits != 0) { // At least one key pressed in this block
            for (int bit = 0; bit < BITS_PER_LONG; bit++) {
                if (bits & (1ULL << bit)) {
                    const int key = idx * BITS_PER_LONG + bit;
                    // Return first pressed key
                    if (key < MAX_KEYS)
                        return key;
                    // Invalid key, stop searching this block
                    break;
                }
            }
        }
    }
    return -1; // No key pressed
}

/** @brief Return whether the left shift key is currently held. */
static inline bool inputstate_is_left_shift_held(const inputstate& is) {
    return inputstate_is_held(is, KEY_LEFT_SHIFT);
}

/** @brief Return whether the right shift key is currently held. */
static inline bool inputstate_is_right_shift_held(const inputstate& is) {
    return inputstate_is_held(is, KEY_RIGHT_SHIFT);
}

/** @brief Return whether either shift key is currently held. */
static inline bool inputstate_is_shift_held(const inputstate& is) {
    return inputstate_is_left_shift_held(is) || inputstate_is_right_shift_held(is);
}
