#pragma once

#include "dungeon_tile_type.h"
#include "libgame_defines.h"
#include "massert.h"
#include "texture_ids.h"

static inline int get_txkey_for_tiletype(tiletype_t type) {
    massert(type >= TILE_NONE, "tile type is less than none somehow?");
    massert(type < TILE_COUNT, "tile type is greater than count somehow?");
    int k = -1;
    switch (type) {
    case TILE_UPSTAIRS: k = TX_TILES_UPSTAIRS_00; break;
    case TILE_DOWNSTAIRS: k = TX_TILES_DOWNSTAIRS_00; break;
    case TILE_STONE_WALL_00: k = TX_TILES_WALL_STONE_00; break;
    case TILE_STONE_WALL_01: k = TX_TILES_WALL_STONE_01; break;
    case TILE_FLOOR_STONE_DIRT_UL_00: k = TX_TILES_STONE_DIRT_UL_00; break;
    case TILE_FLOOR_STONE_DIRT_U_00: k = TX_TILES_STONE_DIRT_U_00; break;
    case TILE_FLOOR_STONE_DIRT_UR_00: k = TX_TILES_STONE_DIRT_UR_00; break;
    case TILE_FLOOR_STONE_DIRT_L_00: k = TX_TILES_STONE_DIRT_L_00; break;
    case TILE_FLOOR_STONE_DIRT_C_00: k = TX_TILES_STONE_DIRT_C_00; break;
    case TILE_FLOOR_STONE_DIRT_R_00: k = TX_TILES_STONE_DIRT_R_00; break;
    case TILE_FLOOR_STONE_DIRT_DL_00: k = TX_TILES_STONE_DIRT_DL_00; break;
    case TILE_FLOOR_STONE_DIRT_D_00: k = TX_TILES_STONE_DIRT_D_00; break;
    case TILE_FLOOR_STONE_DIRT_DR_00: k = TX_TILES_STONE_DIRT_DR_00; break;
    case TILE_FLOOR_STONE_DIRT_UL_01: k = TX_TILES_STONE_DIRT_UL_01; break;
    case TILE_FLOOR_STONE_DIRT_U_01: k = TX_TILES_STONE_DIRT_U_01; break;
    case TILE_FLOOR_STONE_DIRT_UR_01: k = TX_TILES_STONE_DIRT_UR_01; break;
    case TILE_FLOOR_STONE_DIRT_L_01: k = TX_TILES_STONE_DIRT_L_01; break;
    case TILE_FLOOR_STONE_DIRT_C_01: k = TX_TILES_STONE_DIRT_C_01; break;
    case TILE_FLOOR_STONE_DIRT_R_01: k = TX_TILES_STONE_DIRT_R_01; break;
    case TILE_FLOOR_STONE_DIRT_DL_01: k = TX_TILES_STONE_DIRT_DL_01; break;
    case TILE_FLOOR_STONE_DIRT_D_01: k = TX_TILES_STONE_DIRT_D_01; break;
    case TILE_FLOOR_STONE_DIRT_DR_01: k = TX_TILES_STONE_DIRT_DR_01; break;
    case TILE_FLOOR_STONE_DIRT_UL_02: k = TX_TILES_STONE_DIRT_UL_02; break;
    case TILE_FLOOR_STONE_DIRT_U_02: k = TX_TILES_STONE_DIRT_U_02; break;
    case TILE_FLOOR_STONE_DIRT_UR_02: k = TX_TILES_STONE_DIRT_UR_02; break;
    case TILE_FLOOR_STONE_DIRT_L_02: k = TX_TILES_STONE_DIRT_L_02; break;
    case TILE_FLOOR_STONE_DIRT_C_02: k = TX_TILES_STONE_DIRT_C_02; break;
    case TILE_FLOOR_STONE_DIRT_R_02: k = TX_TILES_STONE_DIRT_R_02; break;
    case TILE_FLOOR_STONE_DIRT_DL_02: k = TX_TILES_STONE_DIRT_DL_02; break;
    case TILE_FLOOR_STONE_DIRT_D_02: k = TX_TILES_STONE_DIRT_D_02; break;
    case TILE_FLOOR_STONE_DIRT_DR_02: k = TX_TILES_STONE_DIRT_DR_02; break;
    case TILE_FLOOR_STONE_DIRT_UL_03: k = TX_TILES_STONE_DIRT_UL_03; break;
    case TILE_FLOOR_STONE_DIRT_U_03: k = TX_TILES_STONE_DIRT_U_03; break;
    case TILE_FLOOR_STONE_DIRT_UR_03: k = TX_TILES_STONE_DIRT_UR_03; break;
    case TILE_FLOOR_STONE_DIRT_DL_03: k = TX_TILES_STONE_DIRT_DL_03; break;
    case TILE_FLOOR_STONE_DIRT_D_03: k = TX_TILES_STONE_DIRT_D_03; break;
    case TILE_FLOOR_STONE_DIRT_DR_03: k = TX_TILES_STONE_DIRT_DR_03; break;
    case TILE_FLOOR_STONE_DIRT_UL_04: k = TX_TILES_STONE_DIRT_UL_04; break;
    case TILE_FLOOR_STONE_DIRT_U_04: k = TX_TILES_STONE_DIRT_U_04; break;
    case TILE_FLOOR_STONE_DIRT_UR_04: k = TX_TILES_STONE_DIRT_UR_04; break;
    case TILE_FLOOR_STONE_DIRT_DL_04: k = TX_TILES_STONE_DIRT_DL_04; break;
    case TILE_FLOOR_STONE_DIRT_D_04: k = TX_TILES_STONE_DIRT_D_04; break;
    case TILE_FLOOR_STONE_DIRT_DR_04: k = TX_TILES_STONE_DIRT_DR_04; break;
    case TILE_FLOOR_STONE_DIRT_UL_05: k = TX_TILES_STONE_DIRT_UL_05; break;
    case TILE_FLOOR_STONE_DIRT_U_05: k = TX_TILES_STONE_DIRT_U_05; break;
    case TILE_FLOOR_STONE_DIRT_UR_05: k = TX_TILES_STONE_DIRT_UR_05; break;
    case TILE_FLOOR_STONE_DIRT_DL_05: k = TX_TILES_STONE_DIRT_DL_05; break;
    case TILE_FLOOR_STONE_DIRT_D_05: k = TX_TILES_STONE_DIRT_D_05; break;
    case TILE_FLOOR_STONE_DIRT_DR_05: k = TX_TILES_STONE_DIRT_DR_05; break;
    case TILE_FLOOR_STONE_00: k = TX_TILES_STONE_00; break;
    case TILE_FLOOR_STONE_01: k = TX_TILES_STONE_01; break;
    case TILE_FLOOR_STONE_02: k = TX_TILES_STONE_02; break;
    case TILE_FLOOR_STONE_03: k = TX_TILES_STONE_03; break;
    case TILE_FLOOR_STONE_04: k = TX_TILES_STONE_04; break;
    case TILE_FLOOR_STONE_05: k = TX_TILES_STONE_05; break;
    case TILE_FLOOR_STONE_06: k = TX_TILES_STONE_06; break;
    case TILE_FLOOR_STONE_07: k = TX_TILES_STONE_07; break;
    case TILE_FLOOR_STONE_08: k = TX_TILES_STONE_08; break;
    case TILE_FLOOR_STONE_09: k = TX_TILES_STONE_09; break;
    case TILE_FLOOR_STONE_10: k = TX_TILES_STONE_10; break;
    case TILE_FLOOR_STONE_11: k = TX_TILES_STONE_11; break;
    case TILE_FLOOR_GRASS_00: k = TX_TILES_GRASS_00; break;
    case TILE_FLOOR_GRASS_01: k = TX_TILES_GRASS_01; break;
    case TILE_FLOOR_GRASS_02: k = TX_TILES_GRASS_02; break;
    case TILE_FLOOR_GRASS_03: k = TX_TILES_GRASS_03; break;
    case TILE_FLOOR_GRASS_04: k = TX_TILES_GRASS_04; break;
    case TILE_FLOOR_GRASS_05: k = TX_TILES_GRASS_05; break;
    case TILE_FLOOR_GRASS_06: k = TX_TILES_GRASS_06; break;
    case TILE_FLOOR_GRASS_07: k = TX_TILES_GRASS_07; break;
    case TILE_FLOOR_GRASS_08: k = TX_TILES_GRASS_08; break;
    case TILE_FLOOR_GRASS_09: k = TX_TILES_GRASS_09; break;
    case TILE_FLOOR_GRASS_10: k = TX_TILES_GRASS_10; break;
    case TILE_FLOOR_GRASS_11: k = TX_TILES_GRASS_11; break;
    case TILE_FLOOR_GRASS_12: k = TX_TILES_GRASS_12; break;
    case TILE_FLOOR_GRASS_13: k = TX_TILES_GRASS_13; break;
    case TILE_FLOOR_GRASS_14: k = TX_TILES_GRASS_14; break;
    case TILE_FLOOR_GRASS_15: k = TX_TILES_GRASS_15; break;
    case TILE_FLOOR_GRASS_16: k = TX_TILES_GRASS_16; break;
    case TILE_FLOOR_GRASS_17: k = TX_TILES_GRASS_17; break;
    case TILE_FLOOR_GRASS_18: k = TX_TILES_GRASS_18; break;
    case TILE_FLOOR_GRASS_19: k = TX_TILES_GRASS_19; break;
    default: break;
    }
    return k;
}
