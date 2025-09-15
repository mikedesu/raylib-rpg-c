#pragma once

//#include "SpriteGroup.h"
//#include "Direction.h"
//#include "entityid.h"
#include "entitytype.h"
#include "race.h"
#include "vec3.h"
#include <raylib.h>
#include <string>

using std::string;

//using entityid = int;
using TileID = int;

// Component kinds
struct Name { }; // string
struct EntityType { }; // entitytype_t
struct Race { }; // race_t
struct Location { }; //vec3
struct SpriteMove { }; // Rectangle
struct Dead { }; // bool
struct Update { }; // bool
struct Direction { }; // dir_t
struct Attacking { }; // bool
struct Blocking { }; // bool
struct BlockSuccess { }; // bool
struct Damaged { }; // bool
struct TxAlpha { }; // int
//struct Stats { }; // ??

// Traits to associate each Kind with its data type
template <typename Kind>
struct ComponentTraits;

template <>
struct ComponentTraits<Name> {
    using Type = string;
};

template <>
struct ComponentTraits<EntityType> {
    using Type = entitytype_t;
};

template <>
struct ComponentTraits<Race> {
    using Type = race_t;
};

template <>
struct ComponentTraits<Location> {
    using Type = vec3;
};

template <>
struct ComponentTraits<SpriteMove> {
    using Type = Rectangle;
};

template <>
struct ComponentTraits<Dead> {
    using Type = bool;
};

template <>
struct ComponentTraits<Update> {
    using Type = bool;
};

template <>
struct ComponentTraits<Direction> {
    using Type = int; // direction_t;
};

template <>
struct ComponentTraits<Attacking> {
    using Type = bool;
};

template <>
struct ComponentTraits<Blocking> {
    using Type = bool;
};

template <>
struct ComponentTraits<BlockSuccess> {
    using Type = bool;
};

template <>
struct ComponentTraits<Damaged> {
    using Type = bool;
};

template <>
struct ComponentTraits<TxAlpha> {
    using Type = int;
};
