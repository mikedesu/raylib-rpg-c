#pragma once

// specifier.h
// this is used in conjunction with spritegroups
// in order to facilitate easy-selection of multiple spritegroups
// who might share the same entityID
//
// in practice, the collision-system is designed to deal with if
// a entityID collides with another entityID
//
// however
// we can abuse this behavior to stuff multiple spritegroups into one
// hashtable slot, chain them as a linked-list, and attach a 'specifier'
// so that when time comes to draw, we can grab the correct spritegroup
// without having to do any weird fix like using multiple entityIDs for
// the same entity etc

#pragma once

#include <string.h>

typedef enum specifier_t { SPECIFIER_NONE, SPECIFIER_SHIELD_ON_TILE, SPECIFIER_SHIELD_BLOCK, SPECIFIER_SHIELD_BLOCK_SUCCESS, SPECIFIER_COUNT } specifier_t;

const char* specifier_get_str(const specifier_t spec);
