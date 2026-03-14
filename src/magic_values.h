/** @file magic_values.h
 *  @brief Shared gameplay constants that survived spell-system cleanup.
 */

#pragma once

/// @brief Fallback minimum damage roll expressed as `(count, sides, modifier)`.
#define MINIMUM_DAMAGE ((vec3){1, 1, 0})
/// @brief Upper bound for percent-like block chance values.
#define MAX_BLOCK_CHANCE 100
