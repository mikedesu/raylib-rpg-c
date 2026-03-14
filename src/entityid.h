/** @file entityid.h
 *  @brief Shared entity id alias and invalid-id sentinel macros.
 */

#pragma once

/// @brief Integer identifier type used for all runtime entities.
typedef int entityid;
/// @brief Canonical invalid entity id sentinel.
#define ENTITYID_INVALID -1
/// @brief Legacy invalid entity-id alias.
#define ENTITY_INVALID -1
/// @brief Legacy shorthand invalid entity-id alias.
#define E_INVALID -1
/// @brief Legacy invalid id alias.
#define ID_INVALID -1
/// @brief Generic invalid sentinel used throughout older gameplay code.
#define INVALID -1
/// @brief Alternate invalid entity-id alias.
#define INVALID_ENTITY -1
