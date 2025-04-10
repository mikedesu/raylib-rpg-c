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
                                                TX_HUMAN_SPIN_DIE,
                                                TX_HUMAN_SPIN_DIE_SHADOW,
                                                TX_HUMAN_SOUL_DIE,
                                                TX_HUMAN_SOUL_DIE_SHADOW,
                                                TX_HUMAN_GUARD,
                                                TX_HUMAN_GUARD_SHADOW,
                                                TX_HUMAN_GUARD_SUCCESS,
                                                TX_HUMAN_GUARD_SUCCESS_SHADOW,
                                                TX_HUMAN_DMG,
                                                TX_HUMAN_DMG_SHADOW};

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
/*
*/

#define TX_LONG_SWORD_KEY_COUNT 1
static int TX_LONG_SWORD_KEYS[TX_LONG_SWORD_KEY_COUNT] = {TX_LONG_SWORD_00};

#define TX_BUCKLER_KEY_COUNT 1
static int TX_BUCKLER_KEYS[TX_BUCKLER_KEY_COUNT] = {TX_BUCKLER};

//#define TX_GUARD_BUCKLER_KEY_COUNT 2
//static int TX_GUARD_BUCKLER_KEYS[TX_GUARD_BUCKLER_KEY_COUNT] = {
//    TX_GUARD_BUCKLER_FRONT,
//    TX_GUARD_BUCKLER_BACK,
//};

//#define TX_GUARD_BUCKLER_SUCCESS_KEY_COUNT 2
//static int TX_GUARD_BUCKLER_SUCCESS_KEYS[TX_GUARD_BUCKLER_SUCCESS_KEY_COUNT] = {
//    TX_GUARD_BUCKLER_SUCCESS_FRONT, TX_GUARD_BUCKLER_SUCCESS_BACK};
