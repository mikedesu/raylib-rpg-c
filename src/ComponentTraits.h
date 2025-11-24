#pragma once

//#include "SpriteGroup.h"
//#include "Direction.h"
//#include "entityid.h"
#include "direction.h"
#include "entityid.h"
#include "entitytype.h"
#include "item.h"
#include "potion.h"
#include "race.h"
#include "vec3.h"
#include "weapon.h"
#include <memory>
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>

using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;

//using entityid = int;
using TileID = int;

// Component kinds
struct name { }; // string
struct entitytype { }; // entitytype_t
struct race { }; // race_t
struct location { }; //vec3
struct spritemove { }; // rectangle
struct dead { }; // bool
struct update { }; // bool
struct direction { }; // dir_t
struct attacking { }; // bool
struct blocking { }; // bool
struct blocksuccess { }; // bool
struct damaged { }; // bool
struct txalpha { }; // int
struct pushable { }; // bool
struct itemtype { }; // itemtype_t
struct potiontype { }; // potiontype_t
struct weapontype { }; // weapontype_t
struct inventory { }; // shared_ptr<vector<entityid>>
struct equipped_weapon { }; // entityid
struct hp { }; // int
struct maxhp { }; // int
struct target { }; // int
struct light_radius { }; // int
struct vision_distance { }; // int
struct aggro { }; // bool

//struct Stats { }; // ??

// Traits to associate each Kind with its data type
template <typename Kind>
struct ComponentTraits;

template <>
struct ComponentTraits<name> {
    using Type = string;
};

template <>
struct ComponentTraits<entitytype> {
    using Type = entitytype_t;
};

template <>
struct ComponentTraits<race> {
    using Type = race_t;
};

template <>
struct ComponentTraits<location> {
    using Type = vec3;
};

template <>
struct ComponentTraits<spritemove> {
    using Type = Rectangle;
};

template <>
struct ComponentTraits<dead> {
    using Type = bool;
};

template <>
struct ComponentTraits<update> {
    using Type = bool;
};

template <>
struct ComponentTraits<direction> {
    using Type = direction_t;
};

template <>
struct ComponentTraits<attacking> {
    using Type = bool;
};

template <>
struct ComponentTraits<blocking> {
    using Type = bool;
};

template <>
struct ComponentTraits<blocksuccess> {
    using Type = bool;
};

template <>
struct ComponentTraits<damaged> {
    using Type = bool;
};

template <>
struct ComponentTraits<txalpha> {
    using Type = int;
};

template <>
struct ComponentTraits<pushable> {
    using Type = bool;
};

template <>
struct ComponentTraits<itemtype> {
    using Type = itemtype_t;
};

template <>
struct ComponentTraits<potiontype> {
    using Type = potiontype_t;
};

template <>
struct ComponentTraits<weapontype> {
    using Type = weapontype_t;
};

template <>
struct ComponentTraits<inventory> {
    using Type = shared_ptr<vector<entityid>>;
};

template <>
struct ComponentTraits<equipped_weapon> {
    using Type = entityid;
};

template <>
struct ComponentTraits<hp> {
    using Type = int;
};

template <>
struct ComponentTraits<maxhp> {
    using Type = int;
};

template <>
struct ComponentTraits<target> {
    using Type = entityid;
};

template <>
struct ComponentTraits<light_radius> {
    using Type = int;
};

template <>
struct ComponentTraits<vision_distance> {
    using Type = int;
};

template <>
struct ComponentTraits<aggro> {
    using Type = bool;
};
