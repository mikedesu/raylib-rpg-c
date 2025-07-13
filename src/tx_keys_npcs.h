#pragma once
#include "libgame_defines.h"

#define TX_HUMAN_COUNT 16
static int TX_HUMAN_KEYS[TX_HUMAN_COUNT] = {TX_HUMAN_IDLE,
                                            TX_HUMAN_IDLE_SHADOW,
                                            TX_HUMAN_WALK,
                                            TX_HUMAN_WALK_SHADOW,
                                            TX_HUMAN_ATTACK,
                                            TX_HUMAN_ATTACK_SHADOW,
                                            TX_HUMAN_JUMP,
                                            TX_HUMAN_JUMP_SHADOW,
                                            TX_HUMAN_DMG,
                                            TX_HUMAN_DMG_SHADOW,
                                            TX_HUMAN_SPIN_DIE,
                                            TX_HUMAN_SPIN_DIE_SHADOW,
                                            TX_HUMAN_GUARD_SUCCESS,
                                            TX_HUMAN_GUARD_SUCCESS_SHADOW,
                                            TX_HUMAN_SHOT,
                                            TX_HUMAN_SHOT_SHADOW};

#define TX_ORC_COUNT 14
static int TX_ORC_KEYS[TX_ORC_COUNT] = {
    TX_ORC_IDLE,
    TX_ORC_IDLE_SHADOW,
    TX_ORC_WALK,
    TX_ORC_WALK_SHADOW,
    TX_ORC_ATTACK,
    TX_ORC_ATTACK_SHADOW,
    TX_ORC_JUMP,
    TX_ORC_JUMP_SHADOW,
    TX_ORC_DMG,
    TX_ORC_DMG_SHADOW,
    TX_ORC_DIE,
    TX_ORC_DIE_SHADOW,
    TX_ORC_GUARD_SUCCESS,
    TX_ORC_GUARD_SUCCESS_SHADOW,
    //TX_ORC_SHOT,
    //TX_ORC_SHOT_SHADOW,
};


#define TX_ELF_COUNT 16
static int TX_ELF_KEYS[TX_ELF_COUNT] = {
    TX_ELF_IDLE,
    TX_ELF_IDLE_SHADOW,
    TX_ELF_WALK,
    TX_ELF_WALK_SHADOW,
    TX_ELF_ATTACK,
    TX_ELF_ATTACK_SHADOW,
    TX_ELF_JUMP,
    TX_ELF_JUMP_SHADOW,
    TX_ELF_DMG,
    TX_ELF_DMG_SHADOW,
    TX_ELF_SPIN_DIE,
    TX_ELF_SPIN_DIE_SHADOW,
    TX_ELF_GUARD_SUCCESS,
    TX_ELF_GUARD_SUCCESS_SHADOW,
    TX_ELF_SHOT,
    TX_ELF_SHOT_SHADOW,
};


#define TX_DWARF_COUNT 16
static int TX_DWARF_KEYS[TX_DWARF_COUNT] = {
    TX_DWARF_IDLE,
    TX_DWARF_IDLE_SHADOW,
    TX_DWARF_WALK,
    TX_DWARF_WALK_SHADOW,
    TX_DWARF_ATTACK,
    TX_DWARF_ATTACK_SHADOW,
    TX_DWARF_JUMP,
    TX_DWARF_JUMP_SHADOW,
    TX_DWARF_DMG,
    TX_DWARF_DMG_SHADOW,
    TX_DWARF_SPIN_DIE,
    TX_DWARF_SPIN_DIE_SHADOW,
    TX_DWARF_GUARD_SUCCESS,
    TX_DWARF_GUARD_SUCCESS_SHADOW,
    TX_DWARF_SHOT,
    TX_DWARF_SHOT_SHADOW,
};

#define TX_HALFLING_COUNT 16
static int TX_HALFLING_KEYS[TX_HALFLING_COUNT] = {
    TX_HALFLING_IDLE,
    TX_HALFLING_IDLE_SHADOW,
    TX_HALFLING_WALK,
    TX_HALFLING_WALK_SHADOW,
    TX_HALFLING_ATTACK,
    TX_HALFLING_ATTACK_SHADOW,
    TX_HALFLING_JUMP,
    TX_HALFLING_JUMP_SHADOW,
    TX_HALFLING_DMG,
    TX_HALFLING_DMG_SHADOW,
    TX_HALFLING_SPIN_DIE,
    TX_HALFLING_SPIN_DIE_SHADOW,
    TX_HALFLING_GUARD_SUCCESS,
    TX_HALFLING_GUARD_SUCCESS_SHADOW,
    TX_HALFLING_SHOT,
    TX_HALFLING_SHOT_SHADOW,
};

#define TX_GOBLIN_COUNT 16
static int TX_GOBLIN_KEYS[TX_GOBLIN_COUNT] = {
    TX_GOBLIN_IDLE,
    TX_GOBLIN_IDLE_SHADOW,
    TX_GOBLIN_WALK,
    TX_GOBLIN_WALK_SHADOW,
    TX_GOBLIN_ATTACK,
    TX_GOBLIN_ATTACK_SHADOW,
    TX_GOBLIN_JUMP,
    TX_GOBLIN_JUMP_SHADOW,
    TX_GOBLIN_DMG,
    TX_GOBLIN_DMG_SHADOW,
    TX_GOBLIN_SPIN_DIE,
    TX_GOBLIN_SPIN_DIE_SHADOW,
    TX_GOBLIN_GUARD_SUCCESS,
    TX_GOBLIN_GUARD_SUCCESS_SHADOW,
    TX_GOBLIN_SHOT,
    TX_GOBLIN_SHOT_SHADOW,
};
