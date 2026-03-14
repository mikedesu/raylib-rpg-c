/** @file gameplay_keybindings.h
 *  @brief Keyboard profiles, gameplay input actions, and key-label helpers.
 */

#pragma once

#include <array>
#include <cctype>
#include <raylib.h>
#include <string>

using std::array;
using std::string;

/// @brief Built-in keyboard layouts offered by the controls UI.
typedef enum {
    KEYBOARD_PROFILE_FULL = 0,
    KEYBOARD_PROFILE_LAPTOP,
    KEYBOARD_PROFILE_COUNT
} keyboard_profile_t;

/// @brief Bindable gameplay actions exposed through the controls system.
typedef enum {
    INPUT_ACTION_MOVE_UP = 0,
    INPUT_ACTION_MOVE_DOWN,
    INPUT_ACTION_MOVE_LEFT,
    INPUT_ACTION_MOVE_RIGHT,
    INPUT_ACTION_MOVE_UP_LEFT,
    INPUT_ACTION_MOVE_UP_RIGHT,
    INPUT_ACTION_MOVE_DOWN_LEFT,
    INPUT_ACTION_MOVE_DOWN_RIGHT,
    INPUT_ACTION_DIRECTION_MODE,
    INPUT_ACTION_FACE_WAIT,
    INPUT_ACTION_FACE_UP,
    INPUT_ACTION_FACE_DOWN,
    INPUT_ACTION_FACE_LEFT,
    INPUT_ACTION_FACE_RIGHT,
    INPUT_ACTION_FACE_UP_LEFT,
    INPUT_ACTION_FACE_UP_RIGHT,
    INPUT_ACTION_FACE_DOWN_LEFT,
    INPUT_ACTION_FACE_DOWN_RIGHT,
    INPUT_ACTION_FACE_ATTACK,
    INPUT_ACTION_ATTACK,
    INPUT_ACTION_PICKUP,
    INPUT_ACTION_STAIRS,
    INPUT_ACTION_OPEN,
    INPUT_ACTION_INTERACT,
    INPUT_ACTION_INVENTORY,
    INPUT_ACTION_PULL,
    INPUT_ACTION_HELP,
    INPUT_ACTION_OPTIONS,
    INPUT_ACTION_CAMERA_TOGGLE,
    INPUT_ACTION_RESTART,
    INPUT_ACTION_TOGGLE_FULL_LIGHT,
    INPUT_ACTION_ZOOM_IN,
    INPUT_ACTION_ZOOM_OUT,
    INPUT_ACTION_COUNT
} gameplay_input_action_t;

/// @brief Number of key slots stored for each bindable gameplay action.
static constexpr int GAMEPLAY_KEYBINDING_SLOTS = 3;
/// @brief Fixed-size binding record storing primary and alternate keys for one action.
using gameplay_keybinding_t = array<int, GAMEPLAY_KEYBINDING_SLOTS>;

/** @brief Return the user-facing label for a keyboard profile. */
static inline const char* keyboard_profile_label(keyboard_profile_t profile) {
    switch (profile) {
    case KEYBOARD_PROFILE_FULL:
        return "Full Keyboard";
    case KEYBOARD_PROFILE_LAPTOP:
        return "Laptop Keyboard";
    case KEYBOARD_PROFILE_COUNT:
    default:
        break;
    }
    return "Unknown";
}

/** @brief Return the user-facing label for a bindable gameplay action. */
static inline const char* gameplay_input_action_label(gameplay_input_action_t action) {
    switch (action) {
    case INPUT_ACTION_MOVE_UP: return "Move Up";
    case INPUT_ACTION_MOVE_DOWN: return "Move Down";
    case INPUT_ACTION_MOVE_LEFT: return "Move Left";
    case INPUT_ACTION_MOVE_RIGHT: return "Move Right";
    case INPUT_ACTION_MOVE_UP_LEFT: return "Move Up-Left";
    case INPUT_ACTION_MOVE_UP_RIGHT: return "Move Up-Right";
    case INPUT_ACTION_MOVE_DOWN_LEFT: return "Move Down-Left";
    case INPUT_ACTION_MOVE_DOWN_RIGHT: return "Move Down-Right";
    case INPUT_ACTION_DIRECTION_MODE: return "Face Direction Mode";
    case INPUT_ACTION_FACE_WAIT: return "Face Mode Wait";
    case INPUT_ACTION_FACE_UP: return "Face Up";
    case INPUT_ACTION_FACE_DOWN: return "Face Down";
    case INPUT_ACTION_FACE_LEFT: return "Face Left";
    case INPUT_ACTION_FACE_RIGHT: return "Face Right";
    case INPUT_ACTION_FACE_UP_LEFT: return "Face Up-Left";
    case INPUT_ACTION_FACE_UP_RIGHT: return "Face Up-Right";
    case INPUT_ACTION_FACE_DOWN_LEFT: return "Face Down-Left";
    case INPUT_ACTION_FACE_DOWN_RIGHT: return "Face Down-Right";
    case INPUT_ACTION_FACE_ATTACK: return "Face Mode Attack";
    case INPUT_ACTION_ATTACK: return "Attack";
    case INPUT_ACTION_PICKUP: return "Pick Up Item";
    case INPUT_ACTION_STAIRS: return "Use Stairs";
    case INPUT_ACTION_OPEN: return "Open Door / Chest";
    case INPUT_ACTION_INTERACT: return "Interact / Examine";
    case INPUT_ACTION_INVENTORY: return "Inventory";
    case INPUT_ACTION_PULL: return "Pull";
    case INPUT_ACTION_HELP: return "Help";
    case INPUT_ACTION_OPTIONS: return "Options";
    case INPUT_ACTION_CAMERA_TOGGLE: return "Camera Mode";
    case INPUT_ACTION_RESTART: return "Restart";
    case INPUT_ACTION_TOGGLE_FULL_LIGHT: return "Toggle Full Light";
    case INPUT_ACTION_ZOOM_IN: return "Zoom In";
    case INPUT_ACTION_ZOOM_OUT: return "Zoom Out";
    case INPUT_ACTION_COUNT:
    default:
        break;
    }
    return "Unknown Action";
}

/** @brief Convert a Raylib key code into a compact UI display string. */
static inline string gameplay_key_name(int key) {
    switch (key) {
    case -1: return "Unbound";
    case KEY_UP: return "Up";
    case KEY_DOWN: return "Down";
    case KEY_LEFT: return "Left";
    case KEY_RIGHT: return "Right";
    case KEY_ENTER: return "Enter";
    case KEY_ESCAPE: return "Esc";
    case KEY_SPACE: return "Space";
    case KEY_TAB: return "Tab";
    case KEY_BACKSPACE: return "Backspace";
    case KEY_SLASH: return "/";
    case KEY_PERIOD: return ".";
    case KEY_COMMA: return ",";
    case KEY_MINUS: return "-";
    case KEY_EQUAL: return "=";
    case KEY_GRAVE: return "`";
    case KEY_APOSTROPHE: return "'";
    case KEY_SEMICOLON: return ";";
    case KEY_LEFT_SHIFT: return "Left Shift";
    case KEY_RIGHT_SHIFT: return "Right Shift";
    case KEY_KP_1: return "KP1";
    case KEY_KP_2: return "KP2";
    case KEY_KP_3: return "KP3";
    case KEY_KP_4: return "KP4";
    case KEY_KP_5: return "KP5";
    case KEY_KP_6: return "KP6";
    case KEY_KP_7: return "KP7";
    case KEY_KP_8: return "KP8";
    case KEY_KP_9: return "KP9";
    default:
        break;
    }

    if (key >= KEY_A && key <= KEY_Z) {
        const char c = static_cast<char>('A' + (key - KEY_A));
        return string(1, c);
    }
    if (key >= KEY_ZERO && key <= KEY_NINE) {
        const char c = static_cast<char>('0' + (key - KEY_ZERO));
        return string(1, c);
    }
    if (key >= 32 && key <= 126) {
        const char c = static_cast<char>(std::toupper(key));
        return string(1, c);
    }
    return TextFormat("Key %d", key);
}
