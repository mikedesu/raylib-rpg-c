/** @file test_helpers.h
 *  @brief Shared test utility helpers used across CxxTest suites.
 */

#pragma once

#include "../gamestate.h"
#include "../inputstate.h"

/// @brief Number of SFX slots required by tests that trigger sound-producing actions.
constexpr int SFX_COUNT = 71;

/// @brief Resize the gamestate SFX vector to avoid out-of-bounds access in tests.
inline void init_test_sfx(gamestate& g) {
    g.sfx.resize(SFX_COUNT);
}

/**
 * @brief Create a stone floor, mark the dungeon initialized, and return a spawn location.
 *
 * Use this when your test needs a single ready-to-use floor.
 */
inline vec3 add_initialized_floor(gamestate& g, int width = 8, int height = 8) {
    auto df = g.d.create_floor(BIOME_STONE, width, height);
    df->df_set_all_tiles(TILE_FLOOR_STONE_00);
    g.d.add_floor(df);
    g.d.is_initialized = true;
    return vec3{1, 1, 0};
}

/**
 * @brief Create a stone floor without marking the dungeon initialized.
 *
 * Use this when building multi-floor setups where you manage is_initialized yourself.
 */
inline void add_simple_floor(gamestate& g, int width = 8, int height = 8) {
    auto df = g.d.create_floor(BIOME_STONE, width, height);
    df->df_set_all_tiles(TILE_FLOOR_STONE_00);
    g.d.add_floor(df);
}

/// @brief Reset the inputstate and set a single key as pressed.
inline void press_key(inputstate& is, int key) {
    inputstate_reset(is);
    const int idx = key / BITS_PER_LONG;
    const int bit = key % BITS_PER_LONG;
    is.pressed[idx] |= (1ULL << bit);
}

/// @brief Set a single key as held (does NOT reset inputstate first).
inline void hold_key(inputstate& is, int key) {
    const int idx = key / BITS_PER_LONG;
    const int bit = key % BITS_PER_LONG;
    is.held[idx] |= (1ULL << bit);
}

/// @brief Count all entities of the given type across the entire gamestate.
inline size_t count_entities_of_type(gamestate& g, entitytype_t type) {
    size_t count = 0;
    for (entityid id = 1; id < g.next_entityid; id++) {
        if (g.ct.get<entitytype>(id).value_or(ENTITY_NONE) == type) {
            count++;
        }
    }
    return count;
}

/**
 * @brief Return the first live NPC on the given floor.
 * @param race_value Optional race filter; pass RACE_NONE to skip filtering.
 */
inline entityid find_live_npc_on_floor(gamestate& g, int floor, race_t race_value = RACE_NONE) {
    for (entityid id = 1; id < g.next_entityid; id++) {
        if (g.ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_NPC) {
            continue;
        }
        if (g.ct.get<dead>(id).value_or(true)) {
            continue;
        }
        if (race_value != RACE_NONE && g.ct.get<race>(id).value_or(RACE_NONE) != race_value) {
            continue;
        }
        const vec3 loc = g.ct.get<location>(id).value_or(vec3{-1, -1, -1});
        if (loc.z == floor) {
            return id;
        }
    }
    return ENTITYID_INVALID;
}

/// @brief Create a minimal hero at the given location with no custom init.
inline entityid create_hero(gamestate& g, vec3 loc) {
    return g.create_player_at_with(loc, "hero", [](CT&, const entityid) { });
}
