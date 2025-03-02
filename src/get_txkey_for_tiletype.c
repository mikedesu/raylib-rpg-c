#include "get_txkey_for_tiletype.h"
#include "libgame_defines.h"

//int get_txkey_for_tiletype(tiletype_t type) {
int get_txkey_for_tiletype(dungeon_tile_type_t type) {
    int key = -1;
    switch (type) {
    //case TILETYPE_DIRT_00:
    case DUNGEON_TILE_TYPE_FLOOR_DIRT:
        key = TX_DIRT_00;
        break;
    case DUNGEON_TILE_TYPE_STONE_WALL:
        key = TX_STONEWALL_00;
        break;
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
    default:
        break;
    }
    return key;
}
