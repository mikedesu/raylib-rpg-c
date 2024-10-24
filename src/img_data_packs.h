#pragma once

#include "img_data.h"
#include "img_data_pack.h"

#define PK_TILE_DIRT_00 ((img_data_pack_t){DIRT_00_WIDTH, DIRT_00_HEIGHT, DIRT_00_FORMAT, TX_DIRT_00, 1, 1, DIRT_00_DATA})
#define PK_TILE_DIRT_01 ((img_data_pack_t){DIRT_01_WIDTH, DIRT_01_HEIGHT, DIRT_01_FORMAT, TX_DIRT_01, 1, 1, DIRT_01_DATA})
#define PK_TILE_DIRT_02 ((img_data_pack_t){DIRT_02_WIDTH, DIRT_02_HEIGHT, DIRT_02_FORMAT, TX_DIRT_02, 1, 1, DIRT_02_DATA})

#define PK_TILE_STONE_00 ((img_data_pack_t){STONE_00_WIDTH, STONE_00_HEIGHT, STONE_00_FORMAT, TX_STONE_00, 1, 1, STONE_00_DATA})
#define PK_TILE_STONE_01 ((img_data_pack_t){STONE_01_WIDTH, STONE_01_HEIGHT, STONE_01_FORMAT, TX_STONE_01, 1, 1, STONE_01_DATA})
#define PK_TILE_STONE_02 ((img_data_pack_t){STONE_02_WIDTH, STONE_02_HEIGHT, STONE_02_FORMAT, TX_STONE_02, 1, 1, STONE_02_DATA})
#define PK_TILE_STONE_03 ((img_data_pack_t){STONE_03_WIDTH, STONE_03_HEIGHT, STONE_03_FORMAT, TX_STONE_03, 1, 1, STONE_03_DATA})
#define PK_TILE_STONE_04 ((img_data_pack_t){STONE_04_WIDTH, STONE_04_HEIGHT, STONE_04_FORMAT, TX_STONE_04, 1, 1, STONE_04_DATA})
#define PK_TILE_STONE_05 ((img_data_pack_t){STONE_05_WIDTH, STONE_05_HEIGHT, STONE_05_FORMAT, TX_STONE_05, 1, 1, STONE_05_DATA})
#define PK_TILE_STONE_06 ((img_data_pack_t){STONE_06_WIDTH, STONE_06_HEIGHT, STONE_06_FORMAT, TX_STONE_06, 1, 1, STONE_06_DATA})
#define PK_TILE_STONE_07 ((img_data_pack_t){STONE_07_WIDTH, STONE_07_HEIGHT, STONE_07_FORMAT, TX_STONE_07, 1, 1, STONE_07_DATA})
#define PK_TILE_STONE_08 ((img_data_pack_t){STONE_08_WIDTH, STONE_08_HEIGHT, STONE_08_FORMAT, TX_STONE_08, 1, 1, STONE_08_DATA})
#define PK_TILE_STONE_09 ((img_data_pack_t){STONE_09_WIDTH, STONE_09_HEIGHT, STONE_09_FORMAT, TX_STONE_09, 1, 1, STONE_09_DATA})
#define PK_TILE_STONE_10 ((img_data_pack_t){STONE_10_WIDTH, STONE_10_HEIGHT, STONE_10_FORMAT, TX_STONE_10, 1, 1, STONE_10_DATA})
#define PK_TILE_STONE_11 ((img_data_pack_t){STONE_11_WIDTH, STONE_11_HEIGHT, STONE_11_FORMAT, TX_STONE_11, 1, 1, STONE_11_DATA})
#define PK_TILE_STONE_12 ((img_data_pack_t){STONE_12_WIDTH, STONE_12_HEIGHT, STONE_12_FORMAT, TX_STONE_12, 1, 1, STONE_12_DATA})
#define PK_TILE_STONE_13 ((img_data_pack_t){STONE_13_WIDTH, STONE_13_HEIGHT, STONE_13_FORMAT, TX_STONE_13, 1, 1, STONE_13_DATA})

#define PK_TILE_STONEWALL_00 ((img_data_pack_t){STONEWALL_00_WIDTH, STONEWALL_00_HEIGHT, STONEWALL_00_FORMAT, TX_STONEWALL_00, 1, 1, STONEWALL_00_DATA})


#define PK_HUMAN_IDLE ((img_data_pack_t){HUMAN_IDLE_WIDTH, HUMAN_IDLE_HEIGHT, HUMAN_IDLE_FORMAT, TX_HERO_IDLE, 4, 16, HUMAN_IDLE_DATA})
#define PK_HUMAN_IDLE_SHADOW ((img_data_pack_t){HUMAN_IDLE_SHADOW_WIDTH, HUMAN_IDLE_SHADOW_HEIGHT, HUMAN_IDLE_SHADOW_FORMAT, TX_HERO_IDLE_SHADOW, 4, 16, HUMAN_IDLE_SHADOW_DATA})
#define PK_HUMAN_WALK ((img_data_pack_t){HUMAN_WALK_WIDTH, HUMAN_WALK_HEIGHT, HUMAN_WALK_FORMAT, TX_HERO_WALK, 4, 4, HUMAN_WALK_DATA})
#define PK_HUMAN_WALK_SHADOW ((img_data_pack_t){HUMAN_WALK_SHADOW_WIDTH, HUMAN_WALK_SHADOW_HEIGHT, HUMAN_WALK_SHADOW_FORMAT, TX_HERO_WALK_SHADOW, 4, 4, HUMAN_WALK_SHADOW_DATA})
#define PK_HUMAN_ATTACK ((img_data_pack_t){HUMAN_ATTACK_WIDTH, HUMAN_ATTACK_HEIGHT, HUMAN_ATTACK_FORMAT, TX_HERO_ATTACK, 4, 4, HUMAN_ATTACK_DATA})
#define PK_HUMAN_ATTACK_SHADOW ((img_data_pack_t){HUMAN_ATTACK_SHADOW_WIDTH, HUMAN_ATTACK_SHADOW_HEIGHT, HUMAN_ATTACK_SHADOW_FORMAT, TX_HERO_ATTACK_SHADOW, 4, 4, HUMAN_ATTACK_SHADOW_DATA})
#define PK_HUMAN_JUMP ((img_data_pack_t){HUMAN_JUMP_WIDTH, HUMAN_JUMP_HEIGHT, HUMAN_JUMP_FORMAT, TX_HERO_JUMP, 4, 4, HUMAN_JUMP_DATA})
#define PK_HUMAN_JUMP_SHADOW ((img_data_pack_t){HUMAN_JUMP_SHADOW_WIDTH, HUMAN_JUMP_SHADOW_HEIGHT, HUMAN_JUMP_SHADOW_FORMAT, TX_HERO_JUMP_SHADOW, 4, 4, HUMAN_JUMP_SHADOW_DATA})
#define PK_HUMAN_SPIN_DIE ((img_data_pack_t){HUMAN_SPIN_DIE_WIDTH, HUMAN_SPIN_DIE_HEIGHT, HUMAN_SPIN_DIE_FORMAT, TX_HERO_SPIN_DIE, 1, 12, HUMAN_SPIN_DIE_DATA})
#define PK_HUMAN_SPIN_DIE_SHADOW                                                                                                                                                                       \
    ((img_data_pack_t){HUMAN_SPIN_DIE_SHADOW_WIDTH, HUMAN_SPIN_DIE_SHADOW_HEIGHT, HUMAN_SPIN_DIE_SHADOW_FORMAT, TX_HERO_SPIN_DIE_SHADOW, 1, 12, HUMAN_SPIN_DIE_SHADOW_DATA})
#define PK_HUMAN_SOUL_DIE ((img_data_pack_t){HUMAN_SOUL_DIE_WIDTH, HUMAN_SOUL_DIE_HEIGHT, HUMAN_SOUL_DIE_FORMAT, TX_HERO_SOUL_DIE, 1, 12, HUMAN_SOUL_DIE_DATA})
#define PK_HUMAN_SOUL_DIE_SHADOW                                                                                                                                                                       \
    ((img_data_pack_t){HUMAN_SOUL_DIE_SHADOW_WIDTH, HUMAN_SOUL_DIE_SHADOW_HEIGHT, HUMAN_SOUL_DIE_SHADOW_FORMAT, TX_HERO_SOUL_DIE_SHADOW, 1, 12, HUMAN_SOUL_DIE_SHADOW_DATA})



#define PK_ORC_IDLE ((img_data_pack_t){ORC_IDLE_WIDTH, ORC_IDLE_HEIGHT, ORC_IDLE_FORMAT, TX_ORC_IDLE, 4, 16, ORC_IDLE_DATA})
#define PK_ORC_IDLE_SHADOW ((img_data_pack_t){ORC_IDLE_SHADOW_WIDTH, ORC_IDLE_SHADOW_HEIGHT, ORC_IDLE_SHADOW_FORMAT, TX_ORC_IDLE_SHADOW, 4, 16, ORC_IDLE_SHADOW_DATA})

#define PK_ORC_WALK ((img_data_pack_t){ORC_WALK_WIDTH, ORC_WALK_HEIGHT, ORC_WALK_FORMAT, TX_ORC_WALK, 4, 4, ORC_WALK_DATA})
#define PK_ORC_WALK_SHADOW ((img_data_pack_t){ORC_WALK_SHADOW_WIDTH, ORC_WALK_SHADOW_HEIGHT, ORC_WALK_SHADOW_FORMAT, TX_ORC_WALK_SHADOW, 4, 4, ORC_WALK_SHADOW_DATA})

#define PK_ORC_ATTACK ((img_data_pack_t){ORC_ATTACK_WIDTH, ORC_ATTACK_HEIGHT, ORC_ATTACK_FORMAT, TX_ORC_ATTACK, 4, 4, ORC_ATTACK_DATA})
#define PK_ORC_ATTACK_SHADOW ((img_data_pack_t){ORC_ATTACK_SHADOW_WIDTH, ORC_ATTACK_SHADOW_HEIGHT, ORC_ATTACK_SHADOW_FORMAT, TX_ORC_ATTACK_SHADOW, 4, 4, ORC_ATTACK_SHADOW_DATA})

#define PK_ORC_CHARGED_ATTACK ((img_data_pack_t){ORC_CHARGED_ATTACK_WIDTH, ORC_CHARGED_ATTACK_HEIGHT, ORC_CHARGED_ATTACK_FORMAT, TX_ORC_CHARGED_ATTACK, 4, 4, ORC_CHARGED_ATTACK_DATA})
#define PK_ORC_CHARGED_ATTACK_SHADOW                                                                                                                                                                   \
    ((img_data_pack_t){ORC_CHARGED_ATTACK_SHADOW_WIDTH, ORC_CHARGED_ATTACK_SHADOW_HEIGHT, ORC_CHARGED_ATTACK_SHADOW_FORMAT, TX_ORC_CHARGED_ATTACK_SHADOW, 4, 4, ORC_CHARGED_ATTACK_SHADOW_DATA})

#define PK_ORC_JUMP ((img_data_pack_t){ORC_JUMP_WIDTH, ORC_JUMP_HEIGHT, ORC_JUMP_FORMAT, TX_ORC_JUMP, 4, 4, ORC_JUMP_DATA})
#define PK_ORC_JUMP_SHADOW ((img_data_pack_t){ORC_JUMP_SHADOW_WIDTH, ORC_JUMP_SHADOW_HEIGHT, ORC_JUMP_SHADOW_FORMAT, TX_ORC_JUMP_SHADOW, 4, 4, ORC_JUMP_SHADOW_DATA})

#define PK_ORC_DMG ((img_data_pack_t){ORC_DMG_WIDTH, ORC_DMG_HEIGHT, ORC_DMG_FORMAT, TX_ORC_DMG, 4, 4, ORC_DMG_DATA})
#define PK_ORC_DMG_SHADOW ((img_data_pack_t){ORC_DMG_SHADOW_WIDTH, ORC_DMG_SHADOW_HEIGHT, ORC_DMG_SHADOW_FORMAT, TX_ORC_DMG_SHADOW, 4, 4, ORC_DMG_SHADOW_DATA})

#define PK_ORC_DIE ((img_data_pack_t){ORC_DIE_WIDTH, ORC_DIE_HEIGHT, ORC_DIE_FORMAT, TX_ORC_DIE, 1, 12, ORC_DIE_DATA})
#define PK_ORC_DIE_SHADOW ((img_data_pack_t){ORC_DIE_SHADOW_WIDTH, ORC_DIE_SHADOW_HEIGHT, ORC_DIE_SHADOW_FORMAT, TX_ORC_DIE_SHADOW, 1, 12, ORC_DIE_SHADOW_DATA})



//#define PK_ORC_SOUL_DIE ((img_data_pack_t){ORC_SOUL_DIE_WIDTH, ORC_SOUL_DIE_HEIGHT, ORC_SOUL_DIE_FORMAT, TX_ORC_SOUL_DIE, 1, 12, ORC_SOUL_DIE_DATA})
//#define PK_ORC_SOUL_DIE_SHADOW ((img_data_pack_t){ORC_SOUL_DIE_SHADOW_WIDTH, ORC_SOUL_DIE_SHADOW_HEIGHT, ORC_SOUL_DIE_SHADOW_FORMAT, TX_ORC_SOUL_DIE_SHADOW, 1, 12, ORC_SOUL_DIE_SHADOW_DATA})
