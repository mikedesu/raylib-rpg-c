#pragma once

//#define DEFAULT_WINDOW_TITLE "project rpg v0.00000001"
//#define DEFAULT_TARGET_FPS 30
#define DEFAULT_TARGET_FPS 60
#define FRAMEINTERVAL 10

//#define DEFAULT_TARGET_WIDTH 640
//#define DEFAULT_TARGET_HEIGHT 360
//#define DEFAULT_SCALE 1
//#define DEFAULT_WINDOW_WIDTH (DEFAULT_TARGET_WIDTH * DEFAULT_SCALE) // quarter monitor
//#define DEFAULT_WINDOW_HEIGHT (DEFAULT_TARGET_HEIGHT * DEFAULT_SCALE) // quarter monitor
//#define DEFAULT_WINDOW_POS_X 0
//#define DEFAULT_WINDOW_POS_X 1920
//#define DEFAULT_WINDOW_POS_X 1080
//#define DEFAULT_WINDOW_POS_Y 0

#define DEFAULT_TILE_SIZE 8

//#define TXDUNGEONSHEET 3

#define TX_DIRT_00 0

#define TX_HERO_IDLE 1
#define TX_HERO_IDLE_SHADOW 2
#define TX_HERO_WALK 3
#define TX_HERO_WALK_SHADOW 4
#define TX_HERO_ATTACK 5
#define TX_HERO_ATTACK_SHADOW 6
#define TX_HERO_JUMP 7
#define TX_HERO_JUMP_SHADOW 8
#define TX_HERO_SPIN_DIE 9
#define TX_HERO_SPIN_DIE_SHADOW 10
#define TX_HERO_SOUL_DIE 11
#define TX_HERO_SOUL_DIE_SHADOW 12

#define TX_ORC_IDLE 13
#define TX_ORC_IDLE_SHADOW 14
#define TX_ORC_WALK 15
#define TX_ORC_WALK_SHADOW 16
#define TX_ORC_ATTACK 17
#define TX_ORC_ATTACK_SHADOW 18
#define TX_ORC_CHARGED_ATTACK 19
#define TX_ORC_CHARGED_ATTACK_SHADOW 20
#define TX_ORC_JUMP 21
#define TX_ORC_JUMP_SHADOW 22
#define TX_ORC_DIE 23
#define TX_ORC_DIE_SHADOW 24
#define TX_ORC_DMG 25
#define TX_ORC_DMG_SHADOW 26

#define TX_DIRT_01 27
#define TX_DIRT_02 28

#define TX_STONE_00 29
#define TX_STONE_01 30
#define TX_STONE_02 31
#define TX_STONE_03 32
#define TX_STONE_04 33
#define TX_STONE_05 34
#define TX_STONE_06 35
#define TX_STONE_07 36
#define TX_STONE_08 37
#define TX_STONE_09 38
#define TX_STONE_10 39
#define TX_STONE_11 40
#define TX_STONE_12 41
#define TX_STONE_13 42

#define TX_STONEWALL_00 43

//#define TXSWORD 46
//#define TXSHIELD 47
//#define TX_HUMAN_GUARD 48
//#define TX_HUMAN_GUARD_SHADOW 49
//#define TX_GUARD_WOODEN_SHIELD_FRONT 50
//#define TX_GUARD_WOODEN_SHIELD_BACK 51



//#define TXTORCH 14
//#define TX_TILE_STONE_00 15
//#define TX_TILE_STONE_01 16
//#define TX_TILE_STONE_02 17
//#define TX_TILE_STONE_03 18
//#define TX_TILE_STONE_04 19
//#define TX_TILE_STONE_05 20
//#define TX_TILE_STONE_06 21
//#define TX_TILE_STONE_07 22
//#define TX_TILE_STONE_08 23
//#define TX_TILE_STONE_09 24
//#define TX_TILE_STONE_10 25
//#define TX_TILE_STONE_11 26
//#define TX_TILE_STONE_12 27
//#define TX_TILE_STONE_13 28
//#define TX_TILE_STONE_WALL_00 29

//#define TX_DIRT_01 30
//#define TX_DIRT_02 31


#define COMPANYSCENE 0
#define TITLESCENE 1
#define GAMEPLAYSCENE 2

#define COMPANYNAME "@evildojo666"
#define COMPANYFILL "   x  x x   "

#define DEFAULT_FONT_PATH "fonts/hack.ttf"

#define DEFAULT_VECTOR_ENTITYID_SIZE 16
#define DEFAULT_HASHTABLE_ENTITYID_ENTITY_SIZE 16
#define DEFAULT_HASHTABLE_ENTITYID_SPRITEGROUP_SIZE 16
