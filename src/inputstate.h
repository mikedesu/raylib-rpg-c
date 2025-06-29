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
} inputstate;

// Reset all bits to 0
//static inline void inputstate_reset(inputstate* is) {
static inline void inputstate_reset(std::shared_ptr<inputstate> is) {
    for (int i = 0; i < NUM_LONGS; i++) {
        is->pressed[i] = 0;
        is->held[i] = 0;
    }
}

// Update all key states from Raylib
//static inline void inputstate_update(inputstate* is) {
static inline void inputstate_update(std::shared_ptr<inputstate> is) {
    inputstate_reset(is);
    for (int k = 0; k < MAX_KEYS; k++) {
        if (IsKeyPressed(k)) {
            // debugging to see if KEY_ENTER is pressed
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
}

// Check if a key was pressed this frame
//static inline bool inputstate_is_pressed(const inputstate* is, int key) {
static inline bool inputstate_is_pressed(std::shared_ptr<inputstate> is, int key) {
    if (key < 0 || key >= MAX_KEYS) return false;
    int idx = key / BITS_PER_LONG;
    int bit = key % BITS_PER_LONG;
    return (is->pressed[idx] & (1ULL << bit)) != 0;
}

// Check if ANY key was pressed this frame
static inline bool inputstate_any_pressed(const inputstate* is) {
    if (!is) return false; // Invalid inputstate
    for (int idx = 0; idx < NUM_LONGS; idx++) {
        if (is->pressed[idx] != 0) return true; // At least one key pressed
    }
    return false;
}

// Check if a key is held down
//static inline bool inputstate_is_held(const inputstate* is, int key) {
static inline bool inputstate_is_held(std::shared_ptr<inputstate> is, int key) {
    if (key < 0 || key >= MAX_KEYS) return false;
    int idx = key / BITS_PER_LONG;
    int bit = key % BITS_PER_LONG;
    return (is->held[idx] & (1ULL << bit)) != 0;
}

// Get the first key pressed this frame
//static inline int inputstate_get_pressed_key(const inputstate* is) {
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
    //printf("No keys pressed in inputstate\n");
    return -1; // No key pressed
}

//static inline bool inputstate_is_left_shift_held(const inputstate* is) { return inputstate_is_held(is, KEY_LEFT_SHIFT); }
static inline bool inputstate_is_left_shift_held(std::shared_ptr<inputstate> is) { return inputstate_is_held(is, KEY_LEFT_SHIFT); }

static inline bool inputstate_is_right_shift_held(std::shared_ptr<inputstate> is) { return inputstate_is_held(is, KEY_RIGHT_SHIFT); }

static inline bool inputstate_is_shift_held(std::shared_ptr<inputstate> is) { return inputstate_is_left_shift_held(is) || inputstate_is_right_shift_held(is); }
