#pragma once

//#include "SpriteGroup.h"
//#include "Direction.h"
//#include "entityid.h"
#include "entitytype.h"
#include <raylib.h>
#include <string>

using std::string;

//using entityid = int;
using TileID = int;

// Component kinds
struct Name { };
struct EntityType { };
//struct HitPoints { };
//struct MagicPoints { };
//struct Position { };
//struct Sprites { };
//struct Direction { };

// Traits to associate each Kind with its data type
template <typename Kind>
struct ComponentTraits;

template <>
struct ComponentTraits<Name> {
    using Type = string;
};

//template <>
//struct ComponentTraits<HitPoints> {
//    using Type = Vector2;
//};

//template <>
//struct ComponentTraits<MagicPoints> {
//    using Type = Vector2;
//};

//template <>
//struct ComponentTraits<Position> {
//    using Type = Vector3;
//};

template <>
struct ComponentTraits<EntityType> {
    using Type = entitytype_t;
};

//template <>
//struct ComponentTraits<Sprites> {
//    using Type = shared_ptr<SpriteGroup>;
//};

//template <>
//struct ComponentTraits<Direction> {
//    using Type = Dir;
//};
