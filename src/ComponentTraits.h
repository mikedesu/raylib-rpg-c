#pragma once

#include "direction.h"
#include "entityid.h"
#include "entitytype.h"
#include "item.h"
#include "potion.h"
#include "proptype.h"
#include "race.h"
#include "rarity.h"
#include "shield.h"
#include "spell.h"
#include "tactics.h"
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

using TileID = int;

template <typename Kind>
struct ComponentTraits;

// Component kinds
struct level { }; // int
template <>
struct ComponentTraits<level> {
    using Type = int;
};

struct name { }; // string
template <>
struct ComponentTraits<name> {
    using Type = string;
};

struct entitytype { }; // entitytype_t
template <>
struct ComponentTraits<entitytype> {
    using Type = entitytype_t;
};

struct race { }; // race_t
template <>
struct ComponentTraits<race> {
    using Type = race_t;
};

struct location { }; //vec3
template <>
struct ComponentTraits<location> {
    using Type = vec3;
};

struct spritemove { }; // rectangle
template <>
struct ComponentTraits<spritemove> {
    using Type = Rectangle;
};

struct dead { }; // bool
template <>
struct ComponentTraits<dead> {
    using Type = bool;
};

struct update { }; // bool
template <>
struct ComponentTraits<update> {
    using Type = bool;
};

struct direction { }; // dir_t
template <>
struct ComponentTraits<direction> {
    using Type = direction_t;
};

struct attacking { }; // bool
template <>
struct ComponentTraits<attacking> {
    using Type = bool;
};

// this casting is for NPCs
struct casting { }; // bool
template <>
struct ComponentTraits<casting> {
    using Type = bool;
};


// this casting is for spells
struct spell_casting { }; // bool
template <>
struct ComponentTraits<spell_casting> {
    using Type = bool;
};

struct spell_persisting { }; // bool
template <>
struct ComponentTraits<spell_persisting> {
    using Type = bool;
};

struct spell_ending { }; // bool
template <>
struct ComponentTraits<spell_ending> {
    using Type = bool;
};

struct spell_complete { }; // bool
template <>
struct ComponentTraits<spell_complete> {
    using Type = bool;
};


struct blocking { }; // bool
template <>
struct ComponentTraits<blocking> {
    using Type = bool;
};

struct block_success { }; // bool
template <>
struct ComponentTraits<block_success> {
    using Type = bool;
};

struct damaged { }; // bool
template <>
struct ComponentTraits<damaged> {
    using Type = bool;
};

struct txalpha { }; // int
template <>
struct ComponentTraits<txalpha> {
    using Type = int;
};

struct pushable { }; // bool
template <>
struct ComponentTraits<pushable> {
    using Type = bool;
};

struct itemtype { }; // itemtype_t
template <>
struct ComponentTraits<itemtype> {
    using Type = itemtype_t;
};

struct potiontype { }; // potiontype_t
template <>
struct ComponentTraits<potiontype> {
    using Type = potiontype_t;
};

struct weapontype { }; // weapontype_t
template <>
struct ComponentTraits<weapontype> {
    using Type = weapontype_t;
};

struct shieldtype { }; // shieldtype_t
template <>
struct ComponentTraits<shieldtype> {
    using Type = shieldtype_t;
};

struct inventory { }; // shared_ptr<vector<entityid>>
template <>
struct ComponentTraits<inventory> {
    using Type = shared_ptr<vector<entityid>>;
};

struct equipped_weapon { }; // entityid
template <>
struct ComponentTraits<equipped_weapon> {
    using Type = entityid;
};

struct equipped_shield { }; // entityid
template <>
struct ComponentTraits<equipped_shield> {
    using Type = entityid;
};


struct xp { }; // int
template <>
struct ComponentTraits<xp> {
    using Type = int;
};


struct hp { }; // int
template <>
struct ComponentTraits<hp> {
    using Type = int;
};

struct maxhp { }; // int
template <>
struct ComponentTraits<maxhp> {
    using Type = int;
};

struct strength { }; // int
template <>
struct ComponentTraits<strength> {
    using Type = int;
};

struct dexterity { }; // int
template <>
struct ComponentTraits<dexterity> {
    using Type = int;
};

struct intelligence { }; // int
template <>
struct ComponentTraits<intelligence> {
    using Type = int;
};

struct wisdom { }; // int
template <>
struct ComponentTraits<wisdom> {
    using Type = int;
};

struct constitution { }; // int
template <>
struct ComponentTraits<constitution> {
    using Type = int;
};

struct charisma { }; // int
template <>
struct ComponentTraits<charisma> {
    using Type = int;
};

struct target_id { }; // int
template <>
struct ComponentTraits<target_id> {
    using Type = entityid;
};

struct light_radius { }; // int
template <>
struct ComponentTraits<light_radius> {
    using Type = int;
};

struct vision_distance { }; // int
template <>
struct ComponentTraits<vision_distance> {
    using Type = int;
};

struct aggro { }; // bool
template <>
struct ComponentTraits<aggro> {
    using Type = bool;
};


struct damage { }; // vec3
template <>
struct ComponentTraits<damage> {
    using Type = vec3;
};


struct healing { }; // vec3
template <>
struct ComponentTraits<healing> {
    using Type = vec3;
};


struct description { }; //string
template <>
struct ComponentTraits<description> {
    using Type = string;
};


struct durability { }; //string
template <>
struct ComponentTraits<durability> {
    using Type = int;
};


struct max_durability { }; //string
template <>
struct ComponentTraits<max_durability> {
    using Type = int;
};

struct destroyed { }; //string
template <>
struct ComponentTraits<destroyed> {
    using Type = bool;
};


struct door_open { }; //string
template <>
struct ComponentTraits<door_open> {
    using Type = bool;
};


struct hearing_distance { }; //string
template <>
struct ComponentTraits<hearing_distance> {
    using Type = int;
};


struct block_chance { }; //int
template <>
struct ComponentTraits<block_chance> {
    using Type = int;
};


struct base_ac { }; //int
template <>
struct ComponentTraits<base_ac> {
    using Type = int;
};


struct damage_reduction { }; //int
template <>
struct ComponentTraits<damage_reduction> {
    using Type = int;
};


struct hd { }; //int
template <>
struct ComponentTraits<hd> {
    using Type = vec3;
};


struct rarity { }; //int
template <>
struct ComponentTraits<rarity> {
    using Type = rarity_t;
};


struct spelltype { }; //int
template <>
struct ComponentTraits<spelltype> {
    using Type = spelltype_t;
};


struct spellstate { }; //int
template <>
struct ComponentTraits<spellstate> {
    using Type = spellstate_t;
};


struct proptype { }; //int
template <>
struct ComponentTraits<proptype> {
    using Type = proptype_t;
};


struct solid { }; // bool
template <>
struct ComponentTraits<solid> {
    using Type = bool;
};




struct tactics { }; // vector of tactic_t
template <>
struct ComponentTraits<tactics> {
    using Type = vector<tactic>;
};
