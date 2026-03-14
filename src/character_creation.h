/** @file character_creation.h
 *  @brief Data captured during new-character setup before gameplay begins.
 */

#pragma once

#include "alignment.h"
#include "race.h"
#include <string>

using std::string;

/**
 * @brief Player-selected character identity and starting stat bundle.
 *
 * This struct is filled during character creation and later copied into the
 * runtime entity-component state for the hero.
 */
typedef struct character_creation_t {
    string name;

    race_t race;
    alignment_t alignment;

    int hitdie;
    int strength;
    int dexterity;
    int intelligence;
    int wisdom;
    int constitution;
    int charisma;

} character_creation;
