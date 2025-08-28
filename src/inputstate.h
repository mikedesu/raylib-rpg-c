#pragma once
//#include "mprint.h"
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
static inline void inputstate_reset(std::shared_ptr<inputstate> is) {
    for (int i = 0; i < NUM_LONGS; i++) {
        is->pressed[i] = 0;
        is->held[i] = 0;
    }
    for (int i = 0; i < 3; i++) {
        is->mouse_pressed[i] = false;
        is->mouse_held[i] = false;
        is->mouse_released[i] = false;
    }
}

// Update all key states from Raylib
static inline void inputstate_update(std::shared_ptr<inputstate> is) {
    inputstate_reset(is);

    // Update keyboard state
    for (int k = 0; k < MAX_KEYS; k++) {
        if (IsKeyPressed(k)) {
            if (k == KEY_ENTER) {
                printf("KEY_ENTER pressed\n");
            }
            int idx = k / BITS_PER_LONG;
            int bit = k % BITS_PER_LONG;
            is->pressed[idx] |= (1ULL << bit);
        }
        if (IsKeyDown(k)) {
            int idx = k / BITS_PER_LONG;
            int bit = k % BITS_PER_LONG;
            is->held[idx] |= (1ULL << bit);
        }
    }

    // Update mouse state
    is->mouse_position = GetMousePosition();
    for (int button = 0; button < 3; button++) {
        is->mouse_pressed[button] = IsMouseButtonPressed(button);
        is->mouse_held[button] = IsMouseButtonDown(button);
        is->mouse_released[button] = IsMouseButtonReleased(button);
    }
}

// Check if a key was pressed this frame
static inline bool inputstate_is_pressed(std::shared_ptr<inputstate> is, int key) {
    if (key < 0 || key >= MAX_KEYS) return false;
    int idx = key / BITS_PER_LONG;
    int bit = key % BITS_PER_LONG;
    return (is->pressed[idx] & (1ULL << bit)) != 0;
}

// Check if ANY key was pressed this frame
//static inline bool inputstate_any_pressed(const inputstate* is) {
static inline bool inputstate_any_pressed(const std::shared_ptr<inputstate> is) {
    if (!is) return false; // Invalid inputstate
    for (int idx = 0; idx < NUM_LONGS; idx++) {
        if (is->pressed[idx] != 0) return true; // At least one key pressed
    }
    return false;
}

// Check if a key is held down
static inline bool inputstate_is_held(std::shared_ptr<inputstate> is, int key) {
    if (key < 0 || key >= MAX_KEYS) return false;
    int idx = key / BITS_PER_LONG;
    int bit = key % BITS_PER_LONG;
    return (is->held[idx] & (1ULL << bit)) != 0;
}

// Get the first key pressed this frame
static inline int inputstate_get_pressed_key(std::shared_ptr<inputstate> is) {
    if (!is) {
        printf("inputstate is NULL\n");
        return -1;
    } // Invalid inputstate
    for (int idx = 0; idx < NUM_LONGS; idx++) {
        uint64_t bits = is->pressed[idx];
        if (bits != 0) { // At least one key pressed in this block
            for (int bit = 0; bit < BITS_PER_LONG; bit++) {
                if (bits & (1ULL << bit)) {
                    int key = idx * BITS_PER_LONG + bit;
                    if (key < MAX_KEYS) {
                        return key;
                    } // Return first pressed key
                    break; // Invalid key, stop searching this block
                }
            }
        }
    }
    return -1; // No key pressed
}

//static inline bool inputstate_is_left_shift_held(const inputstate* is) { return inputstate_is_held(is, KEY_LEFT_SHIFT); }
static inline bool inputstate_is_left_shift_held(std::shared_ptr<inputstate> is) { return inputstate_is_held(is, KEY_LEFT_SHIFT); }

static inline bool inputstate_is_right_shift_held(std::shared_ptr<inputstate> is) { return inputstate_is_held(is, KEY_RIGHT_SHIFT); }

static inline bool inputstate_is_shift_held(std::shared_ptr<inputstate> is) { return inputstate_is_left_shift_held(is) || inputstate_is_right_shift_held(is); }

// Mouse button checks
static inline bool inputstate_is_mouse_pressed(std::shared_ptr<inputstate> is, int button) { return button >= 0 && button < 3 && is->mouse_pressed[button]; }

static inline bool inputstate_is_mouse_held(std::shared_ptr<inputstate> is, int button) { return button >= 0 && button < 3 && is->mouse_held[button]; }

static inline bool inputstate_is_mouse_released(std::shared_ptr<inputstate> is, int button) { return button >= 0 && button < 3 && is->mouse_released[button]; }

static inline Vector2 inputstate_get_mouse_position(std::shared_ptr<inputstate> is) { return is->mouse_position; }

static inline bool inputstate_is_left_mouse_pressed(std::shared_ptr<inputstate> is) { return inputstate_is_mouse_pressed(is, MOUSE_BUTTON_LEFT); }

static inline bool inputstate_is_right_mouse_pressed(std::shared_ptr<inputstate> is) { return inputstate_is_mouse_pressed(is, MOUSE_BUTTON_RIGHT); }
