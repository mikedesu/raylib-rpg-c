#pragma once

#include "libgame_defines.h"

#define TX_HUMAN_KEY_COUNT 18
static int TX_HUMAN_KEYS[TX_HUMAN_KEY_COUNT] = {TX_HUMAN_IDLE,
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
                                                TX_HUMAN_SOUL_DIE,
                                                TX_HUMAN_SOUL_DIE_SHADOW,
                                                TX_HUMAN_GUARD,
                                                TX_HUMAN_GUARD_SHADOW,
                                                TX_HUMAN_GUARD_SUCCESS,
                                                TX_HUMAN_GUARD_SUCCESS_SHADOW};

#define TX_ORC_KEY_COUNT 14
static int TX_ORC_KEYS[TX_ORC_KEY_COUNT] = {
    TX_ORC_IDLE,
    TX_ORC_IDLE_SHADOW,
    TX_ORC_WALK,
    TX_ORC_WALK_SHADOW,
    TX_ORC_ATTACK,
    TX_ORC_ATTACK_SHADOW,
    TX_ORC_CHARGED_ATTACK,
    TX_ORC_CHARGED_ATTACK_SHADOW,
    TX_ORC_JUMP,
    TX_ORC_JUMP_SHADOW,
    TX_ORC_DMG,
    TX_ORC_DMG_SHADOW,
    TX_ORC_DIE,
    TX_ORC_DIE_SHADOW,
};

#define TX_ELF_KEY_COUNT 14
static int TX_ELF_KEYS[TX_ELF_KEY_COUNT] = {
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
    TX_ELF_SOUL_DIE,
    TX_ELF_SOUL_DIE_SHADOW,
};

#define TX_DWARF_KEY_COUNT 14
static int TX_DWARF_KEYS[TX_ELF_KEY_COUNT] = {
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
    TX_DWARF_SOUL_DIE,
    TX_DWARF_SOUL_DIE_SHADOW,
};

// Weapons

#define TX_LONG_SWORD_KEY_COUNT 1
static int TX_LONG_SWORD_KEYS[TX_LONG_SWORD_KEY_COUNT] = {TX_LONG_SWORD_00};

// Shields

#define TX_BUCKLER_KEY_COUNT 5
static int TX_BUCKLER_KEYS[TX_BUCKLER_KEY_COUNT] = {
    TX_BUCKLER,
    TX_GUARD_BUCKLER_FRONT,
    TX_GUARD_BUCKLER_BACK,
    TX_GUARD_BUCKLER_SUCCESS_FRONT,
    TX_GUARD_BUCKLER_SUCCESS_BACK,
};
