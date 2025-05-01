#pragma once

#include "dungeon_tile_type.h"
#include "get_txkey_for_tiletype.h"
#include "libgame_defines.h"

static inline int get_txkey_for_tiletype(tiletype_t type) {
    int key = -1;
    switch (type) {
    case TILE_UPSTAIRS: key = TX_UPSTAIRS_00; break;
    case TILE_DOWNSTAIRS: key = TX_DOWNSTAIRS_00; break;
    //case TILE_FLOOR_DIRT: key = TX_DIRT_00; break;
    case TILE_STONE_WALL_00: key = TX_STONEWALL_00; break;
    case TILE_STONE_WALL_01: key = TX_STONEWALL_01; break;
    case TILE_STONE_WALL_02: key = TX_STONEWALL_02; break;
    case TILE_STONE_WALL_03: key = TX_STONEWALL_03; break;

    case TILE_FLOOR_STONE_DIRT_UL_00: key = TX_STONE_DIRT_UL_00; break;
    case TILE_FLOOR_STONE_DIRT_U_00: key = TX_STONE_DIRT_U_00; break;
    case TILE_FLOOR_STONE_DIRT_UR_00: key = TX_STONE_DIRT_UR_00; break;
    case TILE_FLOOR_STONE_DIRT_L_00: key = TX_STONE_DIRT_L_00; break;
    case TILE_FLOOR_STONE_DIRT_C_00: key = TX_STONE_DIRT_C_00; break;
    case TILE_FLOOR_STONE_DIRT_R_00: key = TX_STONE_DIRT_R_00; break;
    case TILE_FLOOR_STONE_DIRT_DL_00: key = TX_STONE_DIRT_DL_00; break;
    case TILE_FLOOR_STONE_DIRT_D_00: key = TX_STONE_DIRT_D_00; break;
    case TILE_FLOOR_STONE_DIRT_DR_00: key = TX_STONE_DIRT_DR_00; break;

    case TILE_FLOOR_STONE_DIRT_UL_01: key = TX_STONE_DIRT_UL_01; break;
    case TILE_FLOOR_STONE_DIRT_U_01: key = TX_STONE_DIRT_U_01; break;
    case TILE_FLOOR_STONE_DIRT_UR_01: key = TX_STONE_DIRT_UR_01; break;
    case TILE_FLOOR_STONE_DIRT_L_01: key = TX_STONE_DIRT_L_01; break;
    case TILE_FLOOR_STONE_DIRT_C_01: key = TX_STONE_DIRT_C_01; break;
    case TILE_FLOOR_STONE_DIRT_R_01: key = TX_STONE_DIRT_R_01; break;
    case TILE_FLOOR_STONE_DIRT_DL_01: key = TX_STONE_DIRT_DL_01; break;
    case TILE_FLOOR_STONE_DIRT_D_01: key = TX_STONE_DIRT_D_01; break;
    case TILE_FLOOR_STONE_DIRT_DR_01: key = TX_STONE_DIRT_DR_01; break;

    case TILE_FLOOR_STONE_DIRT_UL_02: key = TX_STONE_DIRT_UL_02; break;
    case TILE_FLOOR_STONE_DIRT_U_02: key = TX_STONE_DIRT_U_02; break;
    case TILE_FLOOR_STONE_DIRT_UR_02: key = TX_STONE_DIRT_UR_02; break;
    case TILE_FLOOR_STONE_DIRT_L_02: key = TX_STONE_DIRT_L_02; break;
    case TILE_FLOOR_STONE_DIRT_C_02: key = TX_STONE_DIRT_C_02; break;
    case TILE_FLOOR_STONE_DIRT_R_02: key = TX_STONE_DIRT_R_02; break;
    case TILE_FLOOR_STONE_DIRT_DL_02: key = TX_STONE_DIRT_DL_02; break;
    case TILE_FLOOR_STONE_DIRT_D_02: key = TX_STONE_DIRT_D_02; break;
    case TILE_FLOOR_STONE_DIRT_DR_02: key = TX_STONE_DIRT_DR_02; break;

    case TILE_FLOOR_STONE_DIRT_UL_03: key = TX_STONE_DIRT_UL_03; break;
    case TILE_FLOOR_STONE_DIRT_U_03: key = TX_STONE_DIRT_U_03; break;
    case TILE_FLOOR_STONE_DIRT_UR_03: key = TX_STONE_DIRT_UR_03; break;
    case TILE_FLOOR_STONE_DIRT_DL_03: key = TX_STONE_DIRT_DL_03; break;
    case TILE_FLOOR_STONE_DIRT_D_03: key = TX_STONE_DIRT_D_03; break;
    case TILE_FLOOR_STONE_DIRT_DR_03: key = TX_STONE_DIRT_DR_03; break;

    case TILE_FLOOR_STONE_DIRT_UL_04: key = TX_STONE_DIRT_UL_04; break;
    case TILE_FLOOR_STONE_DIRT_U_04: key = TX_STONE_DIRT_U_04; break;
    case TILE_FLOOR_STONE_DIRT_UR_04: key = TX_STONE_DIRT_UR_04; break;
    case TILE_FLOOR_STONE_DIRT_DL_04: key = TX_STONE_DIRT_DL_04; break;
    case TILE_FLOOR_STONE_DIRT_D_04: key = TX_STONE_DIRT_D_04; break;
    case TILE_FLOOR_STONE_DIRT_DR_04: key = TX_STONE_DIRT_DR_04; break;

    case TILE_FLOOR_STONE_DIRT_UL_05: key = TX_STONE_DIRT_UL_05; break;
    case TILE_FLOOR_STONE_DIRT_U_05: key = TX_STONE_DIRT_U_05; break;
    case TILE_FLOOR_STONE_DIRT_UR_05: key = TX_STONE_DIRT_UR_05; break;
    case TILE_FLOOR_STONE_DIRT_DL_05: key = TX_STONE_DIRT_DL_05; break;
    case TILE_FLOOR_STONE_DIRT_D_05: key = TX_STONE_DIRT_D_05; break;
    case TILE_FLOOR_STONE_DIRT_DR_05:
        key = TX_STONE_DIRT_DR_05;
        break;

        //case TILE_STONE_WALL_04: key = TX_STONEWALL_04; break;
        //case TILE_STONE_WALL_05: key = TX_STONEWALL_05; break;
        //case TILE_STONE_WALL_06: key = TX_STONEWALL_06; break;
        //case TILE_STONE_WALL_07: key = TX_STONEWALL_07; break;
        //case TILE_STONE_WALL_08: key = TX_STONEWALL_08; break;
        //case TILE_STONE_WALL_09: key = TX_STONEWALL_09; break;
        //case TILE_STONE_WALL_10: key = TX_STONEWALL_10; break;
        //case TILE_STONE_WALL_11: key = TX_STONEWALL_11; break;
        //case TILE_STONE_WALL_12: key = TX_STONEWALL_12; break;
        //case TILE_STONE_WALL_13: key = TX_STONEWALL_13; break;
        //case TILE_STONE_WALL_14: key = TX_STONEWALL_14; break;
        //case TILE_STONE_WALL_15: key = TX_STONEWALL_15; break;
        //case TILE_STONE_WALL_16: key = TX_STONEWALL_16; break;
        //case TILE_STONE_WALL_17: key = TX_STONEWALL_17; break;
        //case TILE_STONE_WALL_18: key = TX_STONEWALL_18; break;
        //case TILE_STONE_WALL_19: key = TX_STONEWALL_19; break;
        //case TILE_STONE_WALL_20: key = TX_STONEWALL_20; break;
        //case TILE_STONE_WALL_21: key = TX_STONEWALL_21; break;
        //case TILE_STONE_WALL_22: key = TX_STONEWALL_22; break;
        //case TILE_STONE_WALL_23: key = TX_STONEWALL_23; break;
        //case TILE_STONE_WALL_24: key = TX_STONEWALL_24; break;
        //case TILE_STONE_WALL_25: key = TX_STONEWALL_25; break;
        //case TILE_STONE_WALL_26: key = TX_STONEWALL_26; break;
        //case TILE_STONE_WALL_27: key = TX_STONEWALL_27; break;
        //case TILE_STONE_WALL_28: key = TX_STONEWALL_28; break;
        //case TILE_STONE_WALL_29: key = TX_STONEWALL_29; break;
        //case TILE_STONE_WALL_30: key = TX_STONEWALL_30; break;
        //case TILE_STONE_WALL_31: key = TX_STONEWALL_31; break;
        //case TILE_STONE_WALL_32: key = TX_STONEWALL_32; break;
        //case TILE_STONE_WALL_33: key = TX_STONEWALL_33; break;
        //case TILE_STONE_WALL_34: key = TX_STONEWALL_34; break;
        //case TILE_STONE_WALL_35: key = TX_STONEWALL_35; break;

    case TILE_FLOOR_STONE_00: key = TX_STONE_00; break;
    case TILE_FLOOR_STONE_01: key = TX_STONE_01; break;
    case TILE_FLOOR_STONE_02: key = TX_STONE_02; break;
    case TILE_FLOOR_STONE_03: key = TX_STONE_03; break;
    case TILE_FLOOR_STONE_04: key = TX_STONE_04; break;
    case TILE_FLOOR_STONE_05: key = TX_STONE_05; break;
    case TILE_FLOOR_STONE_06: key = TX_STONE_06; break;
    case TILE_FLOOR_STONE_07: key = TX_STONE_07; break;
    case TILE_FLOOR_STONE_08: key = TX_STONE_08; break;
    case TILE_FLOOR_STONE_09: key = TX_STONE_09; break;
    case TILE_FLOOR_STONE_10: key = TX_STONE_10; break;
    case TILE_FLOOR_STONE_11: key = TX_STONE_11; break;
    case TILE_FLOOR_GRASS_00: key = TX_GRASS_00; break;
    case TILE_FLOOR_GRASS_01: key = TX_GRASS_01; break;
    case TILE_FLOOR_GRASS_02: key = TX_GRASS_02; break;
    case TILE_FLOOR_GRASS_03: key = TX_GRASS_03; break;
    case TILE_FLOOR_GRASS_04: key = TX_GRASS_04; break;
    case TILE_FLOOR_GRASS_05: key = TX_GRASS_05; break;
    case TILE_FLOOR_GRASS_06: key = TX_GRASS_06; break;
    case TILE_FLOOR_GRASS_07: key = TX_GRASS_07; break;
    case TILE_FLOOR_GRASS_08: key = TX_GRASS_08; break;
    case TILE_FLOOR_GRASS_09: key = TX_GRASS_09; break;
    case TILE_FLOOR_GRASS_10: key = TX_GRASS_10; break;
    case TILE_FLOOR_GRASS_11: key = TX_GRASS_11; break;
    case TILE_FLOOR_GRASS_12: key = TX_GRASS_12; break;
    case TILE_FLOOR_GRASS_13: key = TX_GRASS_13; break;
    case TILE_FLOOR_GRASS_14: key = TX_GRASS_14; break;
    case TILE_FLOOR_GRASS_15: key = TX_GRASS_15; break;
    case TILE_FLOOR_GRASS_16: key = TX_GRASS_16; break;
    case TILE_FLOOR_GRASS_17: key = TX_GRASS_17; break;
    case TILE_FLOOR_GRASS_18: key = TX_GRASS_18; break;
    case TILE_FLOOR_GRASS_19: key = TX_GRASS_19; break;
    //case TILE_FLOOR_DIRT_GRASS_00: key = TX_DIRT_GRASS_00; break;
    //case TILE_FLOOR_DIRT_GRASS_01: key = TX_DIRT_GRASS_01; break;
    //case TILE_FLOOR_DIRT_GRASS_02: key = TX_DIRT_GRASS_02; break;
    //case TILE_FLOOR_DIRT_GRASS_03: key = TX_DIRT_GRASS_03; break;
    //case TILE_FLOOR_DIRT_GRASS_04: key = TX_DIRT_GRASS_04; break;
    //case TILE_FLOOR_DIRT_GRASS_05: key = TX_DIRT_GRASS_05; break;
    //case TILE_FLOOR_DIRT_GRASS_06: key = TX_DIRT_GRASS_06; break;
    //case TILE_FLOOR_DIRT_GRASS_07: key = TX_DIRT_GRASS_07; break;
    //case TILE_FLOOR_DIRT_GRASS_08: key = TX_DIRT_GRASS_08; break;
    //case TILE_FLOOR_DIRT_GRASS_09: key = TX_DIRT_GRASS_09; break;
    //case TILE_FLOOR_DIRT_GRASS_10: key = TX_DIRT_GRASS_10; break;
    //case TILE_FLOOR_DIRT_GRASS_11: key = TX_DIRT_GRASS_11; break;
    //case TILE_FLOOR_DIRT_GRASS_12: key = TX_DIRT_GRASS_12; break;
    //case TILE_FLOOR_DIRT_GRASS_13: key = TX_DIRT_GRASS_13; break;
    //case TILE_FLOOR_DIRT_GRASS_14: key = TX_DIRT_GRASS_14; break;
    //case TILE_WATER_00: key = TX_WATER_00; break;
    //case TILE_WATER_01: key = TX_WATER_01; break;
    //case TILE_FLOOR_DIRT_00: key = TX_DIRT_00; break;
    //case TILE_FLOOR_DIRT_01: key = TX_DIRT_01; break;
    //case TILE_FLOOR_DIRT_02: key = TX_DIRT_02; break;
    //case TILE_FLOOR_DIRT_03: key = TX_DIRT_03; break;
    //case TILE_FLOOR_DIRT_04: key = TX_DIRT_04; break;
    //case TILE_FLOOR_DIRT_05: key = TX_DIRT_05; break;
    //case TILE_FLOOR_DIRT_06: key = TX_DIRT_06; break;
    //case TILE_FLOOR_DIRT_07: key = TX_DIRT_07; break;
    //case TILE_FLOOR_DIRT_08: key = TX_DIRT_08; break;
    case TILE_FLOOR_STONE_TRAP_OFF_00: key = TX_STONE_TRAP_OFF_00; break;
    case TILE_FLOOR_STONE_TRAP_ON_00: key = TX_STONE_TRAP_ON_00; break;
    default: break;
    }
    return key;
}
