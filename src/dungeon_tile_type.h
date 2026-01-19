#pragma once


#include <stdbool.h>
#include <string>


typedef enum {
    TILE_NONE,
    TILE_UPSTAIRS,
    TILE_DOWNSTAIRS,
    TILE_STONE_WALL_00,
    TILE_STONE_WALL_01,
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
    TILE_FLOOR_STONE_DIRT_UL_00,
    TILE_FLOOR_STONE_DIRT_U_00,
    TILE_FLOOR_STONE_DIRT_UR_00,
    TILE_FLOOR_STONE_DIRT_L_00,
    TILE_FLOOR_STONE_DIRT_C_00,
    TILE_FLOOR_STONE_DIRT_R_00,
    TILE_FLOOR_STONE_DIRT_DL_00,
    TILE_FLOOR_STONE_DIRT_D_00,
    TILE_FLOOR_STONE_DIRT_DR_00,
    TILE_FLOOR_STONE_DIRT_UL_01,
    TILE_FLOOR_STONE_DIRT_U_01,
    TILE_FLOOR_STONE_DIRT_UR_01,
    TILE_FLOOR_STONE_DIRT_L_01,
    TILE_FLOOR_STONE_DIRT_C_01,
    TILE_FLOOR_STONE_DIRT_R_01,
    TILE_FLOOR_STONE_DIRT_DL_01,
    TILE_FLOOR_STONE_DIRT_D_01,
    TILE_FLOOR_STONE_DIRT_DR_01,
    TILE_FLOOR_STONE_DIRT_UL_02,
    TILE_FLOOR_STONE_DIRT_U_02,
    TILE_FLOOR_STONE_DIRT_UR_02,
    TILE_FLOOR_STONE_DIRT_L_02,
    TILE_FLOOR_STONE_DIRT_C_02,
    TILE_FLOOR_STONE_DIRT_R_02,
    TILE_FLOOR_STONE_DIRT_DL_02,
    TILE_FLOOR_STONE_DIRT_D_02,
    TILE_FLOOR_STONE_DIRT_DR_02,
    TILE_FLOOR_STONE_DIRT_UL_03,
    TILE_FLOOR_STONE_DIRT_U_03,
    TILE_FLOOR_STONE_DIRT_UR_03,
    TILE_FLOOR_STONE_DIRT_DL_03,
    TILE_FLOOR_STONE_DIRT_D_03,
    TILE_FLOOR_STONE_DIRT_DR_03,
    TILE_FLOOR_STONE_DIRT_UL_04,
    TILE_FLOOR_STONE_DIRT_U_04,
    TILE_FLOOR_STONE_DIRT_UR_04,
    TILE_FLOOR_STONE_DIRT_DL_04,
    TILE_FLOOR_STONE_DIRT_D_04,
    TILE_FLOOR_STONE_DIRT_DR_04,
    TILE_FLOOR_STONE_DIRT_UL_05,
    TILE_FLOOR_STONE_DIRT_U_05,
    TILE_FLOOR_STONE_DIRT_UR_05,
    TILE_FLOOR_STONE_DIRT_DL_05,
    TILE_FLOOR_STONE_DIRT_D_05,
    TILE_FLOOR_STONE_DIRT_DR_05,
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
    TILE_UNKNOWN,
    TILE_COUNT
} tiletype_t;


static inline std::string tiletype2str(const tiletype_t type) {
    if (type == TILE_NONE)
        return "TileNone";
    else if (type == TILE_UNKNOWN)
        return "TileUnknown";
    else if (type == TILE_UPSTAIRS)
        return "TileUpstairs";
    else if (type == TILE_DOWNSTAIRS)
        return "TileDownstairs";
    else if (type >= TILE_FLOOR_STONE_00 && type <= TILE_FLOOR_STONE_11)
        return "TileStone";
    return "TileError";
}


constexpr static inline bool tiletype_is_none(const tiletype_t type) {
    return type == TILE_NONE;
}


constexpr static inline bool tile_is_not_none(const tiletype_t type) {
    return type != TILE_NONE;
}


constexpr static inline bool tiletype_is_wall(const tiletype_t type) {
    switch (type) {
    case TILE_STONE_WALL_00: return true;
    case TILE_STONE_WALL_01: return true;
    default: break;
    }
    return false;
}


constexpr static inline bool tile_is_walkable(const tiletype_t type) {
    return !tiletype_is_wall(type) && type != TILE_UNKNOWN && type != TILE_NONE && type != TILE_COUNT;
}


constexpr static inline bool tile_is_possible_upstairs(const tiletype_t type) {
    return tile_is_walkable(type) && type != TILE_DOWNSTAIRS && type != TILE_COUNT;
}


constexpr static inline bool tile_is_possible_downstairs(const tiletype_t type) {
    return tile_is_walkable(type) && type != TILE_UPSTAIRS && type != TILE_COUNT;
}
