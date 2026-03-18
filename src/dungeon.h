/** @file dungeon.h
 *  @brief Dungeon-level container for floor ownership and active floor selection.
 */

#pragma once

#include "dungeon_floor.h"
#include "dungeon_tile_type.h"
#include "room.h"
#include <functional>
#include <memory>
#include <optional>
#include <vector>

using std::function;
using std::make_shared;
using std::shared_ptr;
using std::vector;

/**
 * @brief Runtime container for the stack of dungeon floors in the active game.
 *
 * Owns the created floor objects, tracks the currently active floor index, and
 * exposes simple helpers for floor creation and retrieval.
 */
class dungeon {
public:
    /// @brief Owned dungeon floors in ascending floor-index order.
    vector<shared_ptr<dungeon_floor>> floors;
    /// @brief Index of the floor currently used for gameplay and rendering.
    int current_floor;
    /// @brief Guard flag used to prevent structural floor edits during locked phases.
    bool is_locked;
    /// @brief Return whether the dungeon runtime has completed initialization.
    bool is_initialized;

    /** @brief Construct an empty unlocked dungeon container. */
    dungeon() {
        current_floor = 0;
        is_locked = false;
        is_initialized = false;
    }

    /** @brief Release owned floor references. */
    ~dungeon() {
        floors.clear();
    }

    /** @brief Return the number of floors currently owned by the dungeon. */
    size_t get_floor_count() {
        return floors.size();
    }

    /**
     * @brief Return a floor by index.
     *
     * @param index Zero-based floor index.
     * @return Shared pointer to the requested floor.
     * @warning Asserts when `index` is out of bounds.
     */
    shared_ptr<dungeon_floor> get_floor(const size_t index) {
        massert(index >= 0 && index < floors.size(), "index is OOB: index is %ld", index);
        return floors.at(index);
    }

    /**
     * @brief Return the currently active floor.
     *
     * @return Shared pointer to the floor selected by `current_floor`.
     */
    shared_ptr<dungeon_floor> get_current_floor() {
        return get_floor(current_floor);
    }

    /**
     * @brief Create a new floor object initialized for this dungeon.
     *
     * The new floor is initialized with the next sequential floor index but is
     * not added to `floors` until `add_floor()` is called.
     *
     * @param type Biome to apply to the created floor.
     * @param width Floor width in tiles.
     * @param height Floor height in tiles.
     * @return Newly created initialized floor.
     * @warning Asserts if the dungeon is currently locked.
     * @see add_floor
     */
    shared_ptr<dungeon_floor> create_floor(biome_t type, int width = DEFAULT_DUNGEON_FLOOR_WIDTH, int height = DEFAULT_DUNGEON_FLOOR_HEIGHT) {
        massert(!is_locked, "dungeon is locked");
        shared_ptr<dungeon_floor> df = make_shared<dungeon_floor>(width, height);
        df->init(floors.size(), type);
        return df;
    }

    /**
     * @brief Add a previously created floor to the dungeon.
     *
     * @param df Floor to append to the owned floor list.
     */
    void add_floor(shared_ptr<dungeon_floor> df) {
        floors.push_back(df);
    }
};
