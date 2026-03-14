/** @file rarity.h
 *  @brief Item rarity identifiers.
 */

#pragma once

/// @brief Supported rarity tiers for generated items or rewards.
typedef enum
{
    RARITY_NONE,
    RARITY_COMMON,
    RARITY_UNCOMMON,
    RARITY_RARE,
    RARITY_EPIC,
    RARITY_LEGENDARY,
    RARITY_COUNT,
} rarity_t;
