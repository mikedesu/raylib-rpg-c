/** @file option_menu.h
 *  @brief Simple ordered options-menu model for the gameplay settings UI.
 */

#pragma once

#include <cstddef>
#include <string>
#include <vector>

/// @brief High-level option sections that can be opened from the options menu.
typedef enum {
    OPTION_NONE,
    OPTION_SOUND,
    OPTION_WINDOW_BOXES,
    OPTION_CONTROLS,
    OPTION_INVENTORY_MENU,
} option_type;

using std::string;
using std::vector;

/**
 * @brief Lightweight selection model for the gameplay options menu.
 *
 * Stores the ordered option list and the currently highlighted entry.
 */
class option_menu {
private:
    vector<option_type> options;
    size_t selection;


public:
    /** @brief Construct the options menu with its default option ordering. */
    option_menu() {
        selection = 0;
        options.push_back(OPTION_SOUND);
        options.push_back(OPTION_WINDOW_BOXES);
        options.push_back(OPTION_CONTROLS);
        options.push_back(OPTION_INVENTORY_MENU);
    }


    /** @brief Return the currently highlighted option index. */
    size_t get_selection() {
        return selection;
    }

    /** @brief Move the highlighted selection down by one entry when possible. */
    void incr_selection() {
        if (selection < options.size() - 1) {
            selection++;
        }
    }

    /** @brief Move the highlighted selection up by one entry when possible. */
    void decr_selection() {
        if (selection > 0) {
            selection--;
        }
    }

    /** @brief Return mutable access to the ordered option list. */
    vector<option_type>& get_options() {
        return options;
    }

    /** @brief Return the number of menu options currently present. */
    size_t get_option_count() {
        return options.size();
    }

    /** @brief Return the option entry at the provided index. */
    option_type get_option(size_t i) {
        return options[i];
    }

    /** @brief Return a display string for the provided option type. */
    string get_option_str(option_type t) {
        switch (t) {
        case OPTION_NONE: return "none";
        case OPTION_CONTROLS: return "controls";
        case OPTION_SOUND: return "sound";
        case OPTION_WINDOW_BOXES: return "window boxes";
        case OPTION_INVENTORY_MENU: return "inventory menu";
        default: break;
        }
        return "unknown option";
    }
};
