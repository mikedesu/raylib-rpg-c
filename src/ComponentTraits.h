/** @file ComponentTraits.h
 *  @brief ECS component tag declarations and tag-to-storage type mappings.
 */

#pragma once

#include "alignment.h"
#include "direction.h"
#include "entity_actions.h"
#include "entityid.h"
#include "entitytype.h"
#include "hunger_points.h"
#include "item.h"
#include "potion.h"
#include "proptype.h"
#include "race.h"
#include "rarity.h"
#include "shield.h"
#include "vec2.h"
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

/**
 * @brief Map a component tag type to its stored value type.
 *
 * `ComponentTable` uses these specializations to determine the concrete value
 * type for each empty tag struct declared in this header.
 *
 * @tparam Kind Component tag type.
 */
template <typename Kind>
struct ComponentTraits;

/// @brief Component tag for actor level.
struct level { };
template <>
struct ComponentTraits<level> {
    using Type = int;
};

/// @brief Component tag for entity display names.
struct name { };
template <>
struct ComponentTraits<name> {
    using Type = string;
};

/// @brief Component tag for high-level entity category.
struct entitytype { };
template <>
struct ComponentTraits<entitytype> {
    using Type = entitytype_t;
};

/// @brief Component tag for creature race selection.
struct race { };
template <>
struct ComponentTraits<race> {
    using Type = race_t;
};

/// @brief Component tag for dungeon-space entity location.
struct location { };
template <>
struct ComponentTraits<location> {
    using Type = vec3;
};

/// @brief Component tag for sprite movement/interpolation state.
struct spritemove { };
template <>
struct ComponentTraits<spritemove> {
    using Type = Rectangle;
};

/// @brief Component tag for dead/alive state.
struct dead { };
template <>
struct ComponentTraits<dead> {
    using Type = bool;
};

/// @brief Component tag for per-frame update eligibility.
struct update { };
template <>
struct ComponentTraits<update> {
    using Type = bool;
};

/// @brief Component tag for actor facing direction.
struct direction { };
template <>
struct ComponentTraits<direction> {
    using Type = direction_t;
};

/// @brief Component tag for attack animation or action state.
struct attacking { };
template <>
struct ComponentTraits<attacking> {
    using Type = bool;
};

/// @brief Component tag for active blocking/defending state.
struct blocking { };
template <>
struct ComponentTraits<blocking> {
    using Type = bool;
};

/// @brief Component tag for successful block resolution.
struct block_success { };
template <>
struct ComponentTraits<block_success> {
    using Type = bool;
};

/// @brief Component tag for recent damage feedback state.
struct damaged { };
template <>
struct ComponentTraits<damaged> {
    using Type = bool;
};

/// @brief Component tag for entities that can be pushed.
struct pushable { };
template <>
struct ComponentTraits<pushable> {
    using Type = bool;
};

/// @brief Component tag for entities that can be pulled.
struct pullable { };
template <>
struct ComponentTraits<pullable> {
    using Type = bool;
};

/// @brief Component tag for high-level item category.
struct itemtype { };
template <>
struct ComponentTraits<itemtype> {
    using Type = itemtype_t;
};

/// @brief Component tag for potion subtype.
struct potiontype { };
template <>
struct ComponentTraits<potiontype> {
    using Type = potiontype_t;
};

/// @brief Component tag for weapon subtype.
struct weapontype { };
template <>
struct ComponentTraits<weapontype> {
    using Type = weapontype_t;
};

/// @brief Component tag for shield subtype.
struct shieldtype { };
template <>
struct ComponentTraits<shieldtype> {
    using Type = shieldtype_t;
};

/// @brief Component tag for inventory ownership lists.
struct inventory { };
template <>
struct ComponentTraits<inventory> {
    using Type = shared_ptr<vector<entityid>>;
};

/// @brief Component tag for the equipped weapon entity id.
struct equipped_weapon { };
template <>
struct ComponentTraits<equipped_weapon> {
    using Type = entityid;
};

/// @brief Component tag for the equipped shield entity id.
struct equipped_shield { };
template <>
struct ComponentTraits<equipped_shield> {
    using Type = entityid;
};

/// @brief Component tag for accumulated experience points.
struct xp { };
template <>
struct ComponentTraits<xp> {
    using Type = int;
};

/// @brief Component tag for current hit points.
struct hp { };
template <>
struct ComponentTraits<hp> {
    using Type = vec2;
};

/// @brief Component tag for strength.
struct strength { };
template <>
struct ComponentTraits<strength> {
    using Type = int;
};

/// @brief Component tag for dexterity.
struct dexterity { };
template <>
struct ComponentTraits<dexterity> {
    using Type = int;
};

/// @brief Component tag for intelligence.
struct intelligence { };
template <>
struct ComponentTraits<intelligence> {
    using Type = int;
};

/// @brief Component tag for wisdom.
struct wisdom { };
template <>
struct ComponentTraits<wisdom> {
    using Type = int;
};

/// @brief Component tag for constitution.
struct constitution { };
template <>
struct ComponentTraits<constitution> {
    using Type = int;
};

/// @brief Component tag for charisma.
struct charisma { };
template <>
struct ComponentTraits<charisma> {
    using Type = int;
};

/// @brief Component tag for an entity's current target id.
struct target_id { };
template <>
struct ComponentTraits<target_id> {
    using Type = entityid;
};

/// @brief Component tag for emitted light radius.
struct light_radius { };
template <>
struct ComponentTraits<light_radius> {
    using Type = int;
};

/// @brief Component tag for vision range.
struct vision_distance { };
template <>
struct ComponentTraits<vision_distance> {
    using Type = int;
};

/// @brief Component tag for hostile aggro state.
struct aggro { };
template <>
struct ComponentTraits<aggro> {
    using Type = bool;
};

/// @brief Component tag for damage dice or damage payload values.
struct damage { };
template <>
struct ComponentTraits<damage> {
    using Type = vec3;
};

/// @brief Component tag for healing dice or healing payload values.
struct healing { };
template <>
struct ComponentTraits<healing> {
    using Type = vec3;
};

/// @brief Component tag for descriptive text.
struct description { };
template <>
struct ComponentTraits<description> {
    using Type = string;
};

/// @brief Component tag for NPC dialogue text.
struct dialogue_text { };
template <>
struct ComponentTraits<dialogue_text> {
    using Type = string;
};

/// @brief Component tag for current durability.
struct durability { };
template <>
struct ComponentTraits<durability> {
    using Type = int;
};

/// @brief Component tag for maximum durability.
struct max_durability { };
template <>
struct ComponentTraits<max_durability> {
    using Type = int;
};

/// @brief Component tag for destroyed/broken state.
struct destroyed { };
template <>
struct ComponentTraits<destroyed> {
    using Type = bool;
};

/// @brief Component tag for door open/closed state.
struct door_open { };
template <>
struct ComponentTraits<door_open> {
    using Type = bool;
};

/// @brief Component tag for hearing radius.
struct hearing_distance { };
template <>
struct ComponentTraits<hearing_distance> {
    using Type = int;
};

/// @brief Component tag for block chance percentage or score.
struct block_chance { };
template <>
struct ComponentTraits<block_chance> {
    using Type = int;
};

/// @brief Component tag for base armor class.
struct base_ac { };
template <>
struct ComponentTraits<base_ac> {
    using Type = int;
};

/// @brief Component tag for flat damage reduction.
struct damage_reduction { };
template <>
struct ComponentTraits<damage_reduction> {
    using Type = int;
};

/// @brief Component tag for hit-die style values.
struct hd { };
template <>
struct ComponentTraits<hd> {
    using Type = vec3;
};

/// @brief Component tag for rarity classification.
struct rarity { };
template <>
struct ComponentTraits<rarity> {
    using Type = rarity_t;
};

/// @brief Component tag for prop type selection.
struct proptype { };
template <>
struct ComponentTraits<proptype> {
    using Type = proptype_t;
};

/// @brief Component tag for solid collision state.
struct solid { };
template <>
struct ComponentTraits<solid> {
    using Type = bool;
};

/// @brief Component tag for counted movement steps.
struct steps_taken { };
template <>
struct ComponentTraits<steps_taken> {
    using Type = unsigned int;
};

/// @brief Component tag for undead/zombie state.
struct zombie { };
template <>
struct ComponentTraits<zombie> {
    using Type = bool;
};

/// @brief Component tag for hunger progression state.
struct hunger_points { };
template <>
struct ComponentTraits<hunger_points> {
    using Type = hunger_points_t;
};

/// @brief Component tag for an entity's default AI or behavior action.
struct entity_default_action { };
template <>
struct ComponentTraits<entity_default_action> {
    using Type = entity_default_action_t;
};

/// @brief Component tag for a cached path toward the current target.
struct target_path { };
template <>
struct ComponentTraits<target_path> {
    using Type = shared_ptr<vector<vec3>>;
};

/// @brief Component tag for an entity's alignment.
struct alignment { };
template <>
struct ComponentTraits<alignment> {
    using Type = alignment_t;
};
