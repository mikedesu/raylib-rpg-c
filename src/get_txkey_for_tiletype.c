#include "get_txkey_for_tiletype.h"
#include "dungeon_tile_type.h"
#include "libgame_defines.h"

int get_txkey_for_tiletype(const dungeon_tile_type_t type) {
    int key = -1;
    switch (type) {
    //case TILETYPE_DIRT_00:
    case TILE_FLOOR_DIRT: key = TX_DIRT_00; break;
    case TILE_STONE_WALL_00: key = TX_STONEWALL_00; break;
    case TILE_STONE_WALL_01: key = TX_STONEWALL_01; break;
    case TILE_STONE_WALL_02: key = TX_STONEWALL_02; break;
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
    //case TILE_FLOOR_STONE_12:
    //    key = TX_STONE_12;
    //    break;
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
    case TILE_FLOOR_DIRT_GRASS_00: key = TX_DIRT_GRASS_00; break;
    case TILE_FLOOR_DIRT_GRASS_01: key = TX_DIRT_GRASS_01; break;
    case TILE_FLOOR_DIRT_GRASS_02: key = TX_DIRT_GRASS_02; break;
    case TILE_FLOOR_DIRT_GRASS_03: key = TX_DIRT_GRASS_03; break;
    case TILE_FLOOR_DIRT_GRASS_04: key = TX_DIRT_GRASS_04; break;
    case TILE_FLOOR_DIRT_GRASS_05: key = TX_DIRT_GRASS_05; break;
    case TILE_FLOOR_DIRT_GRASS_06: key = TX_DIRT_GRASS_06; break;
    case TILE_FLOOR_DIRT_GRASS_07: key = TX_DIRT_GRASS_07; break;
    case TILE_FLOOR_DIRT_GRASS_08: key = TX_DIRT_GRASS_08; break;
    case TILE_FLOOR_DIRT_GRASS_09: key = TX_DIRT_GRASS_09; break;
    case TILE_FLOOR_DIRT_GRASS_10: key = TX_DIRT_GRASS_10; break;
    case TILE_FLOOR_DIRT_GRASS_11: key = TX_DIRT_GRASS_11; break;
    case TILE_FLOOR_DIRT_GRASS_12: key = TX_DIRT_GRASS_12; break;
    case TILE_FLOOR_DIRT_GRASS_13: key = TX_DIRT_GRASS_13; break;
    case TILE_FLOOR_DIRT_GRASS_14: key = TX_DIRT_GRASS_14; break;
    case TILE_WATER_00: key = TX_WATER_00; break;
    case TILE_WATER_01: key = TX_WATER_01; break;
    case TILE_FLOOR_DIRT_00: key = TX_DIRT_00; break;
    case TILE_FLOOR_DIRT_01: key = TX_DIRT_01; break;
    case TILE_FLOOR_DIRT_02: key = TX_DIRT_02; break;
    case TILE_FLOOR_DIRT_03: key = TX_DIRT_03; break;
    case TILE_FLOOR_DIRT_04: key = TX_DIRT_04; break;
    case TILE_FLOOR_DIRT_05: key = TX_DIRT_05; break;
    case TILE_FLOOR_DIRT_06: key = TX_DIRT_06; break;
    case TILE_FLOOR_DIRT_07: key = TX_DIRT_07; break;
    case TILE_FLOOR_DIRT_08: key = TX_DIRT_08; break;
    case TILE_FLOOR_STONE_TRAP_OFF_00: key = TX_STONE_TRAP_OFF_00; break;
    case TILE_FLOOR_STONE_TRAP_ON_00: key = TX_STONE_TRAP_ON_00; break;

    //case TILETYPE_DIRT_01:
    //    key = TX_DIRT_01;
    //    break;
    //case TILETYPE_DIRT_02:
    //    key = TX_DIRT_02;
    //    break;
    //case TILETYPE_DIRT_03:
    //    key = TX_DIRT_03;
    //    break;
    //case TILETYPE_DIRT_04:
    //    key = TX_DIRT_04;
    //    break;
    //case TILETYPE_DIRT_05:
    //    key = TX_DIRT_05;
    //    break;

    //case TILETYPE_STONE_00:
    //    key = TX_STONE_00;
    //    break;
    //case TILETYPE_STONE_01:
    //    key = TX_STONE_01;
    //    break;
    //case TILETYPE_STONE_02:
    //    key = TX_STONE_02;
    //    break;
    //case TILETYPE_STONE_03:
    //    key = TX_STONE_03;
    //    break;
    //case TILETYPE_STONE_04:
    //    key = TX_STONE_04;
    //    break;
    //case TILETYPE_STONE_05:
    //    key = TX_STONE_05;
    //    break;
    //case TILETYPE_STONE_06:
    //    key = TX_STONE_06;
    //    break;
    //case TILETYPE_STONE_07:
    //    key = TX_STONE_07;
    //    break;
    //case TILETYPE_STONE_08:
    //    key = TX_STONE_08;
    //    break;
    //case TILETYPE_STONE_09:
    //    key = TX_STONE_09;
    //    break;
    //case TILETYPE_STONE_10:
    //    key = TX_STONE_10;
    //    break;
    //case TILETYPE_STONE_11:
    //    key = TX_STONE_11;
    //    break;
    //case TILETYPE_STONE_12:
    //    key = TX_STONE_12;
    //    break;
    //case TILETYPE_STONE_13:
    //    key = TX_STONE_13;
    //    break;
    //case TILETYPE_STONE_WALL_00:
    //    key = TX_STONEWALL_00;
    //    break;
    //case TILETYPE_STONE_WALL_01:
    //    key = TX_STONEWALL_01;
    //    break;
    //case TILETYPE_STONE_WALL_02:
    //    key = TX_STONEWALL_02;
    //    break;
    //case TILETYPE_STONE_WALL_03:
    //    key = TX_STONEWALL_03;
    //    break;
    //case TILETYPE_STONE_WALL_04:
    //    key = TX_STONEWALL_04;
    //    break;
    //case TILETYPE_STONE_WALL_05:
    //    key = TX_STONEWALL_05;
    //    break;
    //case TILETYPE_STONE_WALL_06:
    //    key = TX_STONEWALL_06;
    //    break;
    //case TILETYPE_STONE_WALL_07:
    //    key = TX_STONEWALL_07;
    //    break;
    //case TILETYPE_STONE_WALL_08:
    //    key = TX_STONEWALL_08;
    //    break;
    //case TILETYPE_STONE_WALL_09:
    //    key = TX_STONEWALL_09;
    //    break;
    //case TILETYPE_STONE_WALL_10:
    //    key = TX_STONEWALL_10;
    //    break;
    //case TILETYPE_STONE_WALL_11:
    //    key = TX_STONEWALL_11;
    //    break;
    //case TILETYPE_STONE_WALL_12:
    //    key = TX_STONEWALL_12;
    //    break;
    //case TILETYPE_STONE_WALL_13:
    //    key = TX_STONEWALL_13;
    //    break;
    //case TILETYPE_STONE_WALL_14:
    //    key = TX_STONEWALL_14;
    //    break;
    //case TILETYPE_GRASS_00:
    //    key = TX_GRASS_00;
    //    break;
    //case TILETYPE_GRASS_01:
    //    key = TX_GRASS_01;
    //    break;
    //case TILETYPE_GRASS_02:
    //    key = TX_GRASS_02;
    //    break;
    //case TILETYPE_GRASS_03:
    //    key = TX_GRASS_03;
    //    break;
    //case TILETYPE_GRASS_04:
    //    key = TX_GRASS_04;
    //    break;
    default: break;
    }
    return key;
}
