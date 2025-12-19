#pragma once

#include <cstdio>
#include <memory>
#include <raylib.h>
#include <stdint.h>

#define MAX_KEYS 400
#define BITS_PER_LONG 64
#define NUM_LONGS ((MAX_KEYS + BITS_PER_LONG - 1) / BITS_PER_LONG) // 13

typedef struct inputstate {
    uint64_t pressed[NUM_LONGS]; // Bits for keys pressed this frame
    uint64_t held[NUM_LONGS]; // Bits for keys held down
    bool mouse_pressed[3]; // Left, Right, Middle buttons pressed this frame
    bool mouse_held[3]; // Left, Right, Middle buttons held down
    bool mouse_released[3]; // Left, Right, Middle buttons released this frame
    Vector2 mouse_position; // Current mouse position
} inputstate;

// Reset all bits to 0
static inline void inputstate_reset(inputstate& is) {
    for (int i = 0; i < NUM_LONGS; i++)
        is.pressed[i] = is.held[i] = 0;
    for (int i = 0; i < 3; i++)
        is.mouse_pressed[i] = is.mouse_held[i] = is.mouse_released[i] = false;
}

// Update all key states from Raylib
static inline void inputstate_update(inputstate& is) {
    inputstate_reset(is);
    // Update keyboard state
    for (int k = 0; k < MAX_KEYS; k++) {
        if (IsKeyPressed(k)) {
            //if (k == KEY_ENTER)
            //    printf("KEY_ENTER pressed\n");
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

// Check if a key was pressed this frame
static inline bool inputstate_is_pressed(inputstate& is, int key) {
    if (key < 0 || key >= MAX_KEYS)
        return false;
    const int idx = key / BITS_PER_LONG, bit = key % BITS_PER_LONG;
    return (is.pressed[idx] & (1ULL << bit)) != 0;
}

// Check if ANY key was pressed this frame
static inline bool inputstate_any_pressed(inputstate& is) {
    for (int idx = 0; idx < NUM_LONGS; idx++) {
        if (is.pressed[idx] != 0)
            return true; // At least one key pressed
    }
    return false;
}

// Check if a key is held down
static inline bool inputstate_is_held(inputstate& is, int key) {
    if (key < 0 || key >= MAX_KEYS)
        return false;
    const int idx = key / BITS_PER_LONG, bit = key % BITS_PER_LONG;
    return (is.held[idx] & (1ULL << bit)) != 0;
}

// Get the first key pressed this frame
static inline int inputstate_get_pressed_key(inputstate& is) {
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

static inline bool inputstate_is_left_shift_held(inputstate& is) {
    return inputstate_is_held(is, KEY_LEFT_SHIFT);
}

static inline bool inputstate_is_right_shift_held(inputstate& is) {
    return inputstate_is_held(is, KEY_RIGHT_SHIFT);
}

static inline bool inputstate_is_shift_held(inputstate& is) {
    return inputstate_is_left_shift_held(is) || inputstate_is_right_shift_held(is);
}
