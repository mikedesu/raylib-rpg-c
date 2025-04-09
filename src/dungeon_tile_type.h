#pragma once

#include <stdbool.h>

typedef enum {
    TILE_NONE,
    TILE_UPSTAIRS,
    TILE_DOWNSTAIRS,
    TILE_FLOOR_DIRT,
    TILE_FLOOR_STONE_00,
    TILE_FLOOR_STONE_01,
    TILE_FLOOR_STONE_02,
    TILE_FLOOR_STONE_03,
    TILE_FLOOR_STONE_04,
    TILE_FLOOR_STONE_05,
    TILE_FLOOR_STONE_06,
    TILE_FLOOR_STONE_07,
    TILE_FLOOR_STONE_08,
    TILE_FLOOR_STONE_09,
    TILE_FLOOR_STONE_10,
    TILE_FLOOR_STONE_11,
    TILE_STONE_WALL_00,
    TILE_STONE_WALL_01,
    TILE_STONE_WALL_02,
    TILE_FLOOR_GRASS_00,
    TILE_FLOOR_GRASS_01,
    TILE_FLOOR_GRASS_02,
    TILE_FLOOR_GRASS_03,
    TILE_FLOOR_GRASS_04,
    TILE_FLOOR_GRASS_05,
    TILE_FLOOR_GRASS_06,
    TILE_FLOOR_GRASS_07,
    TILE_FLOOR_GRASS_08,
    TILE_FLOOR_GRASS_09,
    TILE_FLOOR_GRASS_10,
    TILE_FLOOR_GRASS_11,
    TILE_FLOOR_GRASS_12,
    TILE_FLOOR_GRASS_13,
    TILE_FLOOR_GRASS_14,
    TILE_FLOOR_GRASS_15,
    TILE_FLOOR_GRASS_16,
    TILE_FLOOR_GRASS_17,
    TILE_FLOOR_GRASS_18,
    TILE_FLOOR_GRASS_19,
    TILE_FLOOR_DIRT_GRASS_00,
    TILE_FLOOR_DIRT_GRASS_01,
    TILE_FLOOR_DIRT_GRASS_02,
    TILE_FLOOR_DIRT_GRASS_03,
    TILE_FLOOR_DIRT_GRASS_04,
    TILE_FLOOR_DIRT_GRASS_05,
    TILE_FLOOR_DIRT_GRASS_06,
    TILE_FLOOR_DIRT_GRASS_07,
    TILE_FLOOR_DIRT_GRASS_08,
    TILE_FLOOR_DIRT_GRASS_09,
    TILE_FLOOR_DIRT_GRASS_10,
    TILE_FLOOR_DIRT_GRASS_11,
    TILE_FLOOR_DIRT_GRASS_12,
    TILE_FLOOR_DIRT_GRASS_13,
    TILE_FLOOR_DIRT_GRASS_14,
    TILE_WATER_00,
    TILE_WATER_01,
    TILE_FLOOR_DIRT_00,
    TILE_FLOOR_DIRT_01,
    TILE_FLOOR_DIRT_02,
    TILE_FLOOR_DIRT_03,
    TILE_FLOOR_DIRT_04,
    TILE_FLOOR_DIRT_05,
    TILE_FLOOR_DIRT_06,
    TILE_FLOOR_DIRT_07,
    TILE_FLOOR_DIRT_08,
    TILE_FLOOR_STONE_TRAP_OFF_00,
    TILE_FLOOR_STONE_TRAP_ON_00,
    TILE_UNKNOWN,
    TILE_COUNT
} tiletype_t;

static inline bool dungeon_tile_is_wall(const tiletype_t type) {
    return type >= TILE_STONE_WALL_00 && type <= TILE_STONE_WALL_02;
}

static inline bool dungeon_tile_is_walkable(const tiletype_t type) {
    return !dungeon_tile_is_wall(type) && type != TILE_UNKNOWN && type != TILE_NONE && type != TILE_COUNT;
    //return type != TILE_STONE_WALL_00 && type != TILE_STONE_WALL_01 && type != TILE_STONE_WALL_02 &&
    //       type != TILE_UNKNOWN && type != TILE_NONE;
}

static inline bool dungeon_tile_is_trap(const tiletype_t type) {
    return type == TILE_FLOOR_STONE_TRAP_OFF_00 || type == TILE_FLOOR_STONE_TRAP_ON_00;
}

static inline bool dungeon_tile_is_possible_upstairs(const tiletype_t type) {
    return dungeon_tile_is_walkable(type) && !dungeon_tile_is_trap(type) && type != TILE_DOWNSTAIRS &&
           type != TILE_COUNT;
}

static inline bool dungeon_tile_is_possible_downstairs(const tiletype_t type) {
    return dungeon_tile_is_walkable(type) && !dungeon_tile_is_trap(type) && type != TILE_UPSTAIRS && type != TILE_COUNT;
}
