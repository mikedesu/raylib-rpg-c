#include "get_txkey_for_tiletype.h"
#include "libgame_defines.h"

int get_txkey_for_tiletype(tiletype_t type) {
    int key = -1;
    switch (type) {
    case TILETYPE_DIRT_00:
        key = TX_DIRT_00;
        break;
    //case TILETYPE_DIRT_01:
    //    key = TX_DIRT_01;
    //    break;
    //case TILETYPE_DIRT_02:
    //    key = TX_DIRT_02;
    //    break;
    //case TILETYPE_STONE_00:
    //    key = TX_TILE_STONE_00;
    //    break;
    //case TILETYPE_STONE_01:
    //    key = TX_TILE_STONE_01;
    //    break;
    //case TILETYPE_STONE_02:
    //    key = TX_TILE_STONE_02;
    //    break;
    //case TILETYPE_STONE_03:
    //    key = TX_TILE_STONE_03;
    //    break;
    //case TILETYPE_STONE_04:
    //    key = TX_TILE_STONE_04;
    //    break;
    //case TILETYPE_STONE_05:
    //    key = TX_TILE_STONE_05;
    //    break;
    //case TILETYPE_STONE_06:
    //    key = TX_TILE_STONE_06;
    //    break;
    //case TILETYPE_STONE_07:
    //    key = TX_TILE_STONE_07;
    //    break;
    //case TILETYPE_STONE_08:
    //    key = TX_TILE_STONE_08;
    //    break;
    //case TILETYPE_STONE_09:
    //    key = TX_TILE_STONE_09;
    //    break;
    //case TILETYPE_STONE_10:
    //    key = TX_TILE_STONE_10;
    //    break;
    //case TILETYPE_STONE_11:
    //    key = TX_TILE_STONE_11;
    //    break;
    //case TILETYPE_STONE_12:
    //    key = TX_TILE_STONE_12;
    //    break;
    //case TILETYPE_STONE_13:
    //    key = TX_TILE_STONE_13;
    //    break;
    //case TILETYPE_STONE_14:
    //    key = TX_TILE_STONE_14;
    //    break;
    //case TILETYPE_STONE_WALL_00:
    //    key = TX_TILE_STONE_WALL_00;
    //    break;
    default:
        break;
    }
    return key;
}
