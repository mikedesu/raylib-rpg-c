
#pragma once

#include "add_message.h"
#include "attack_result.h"
#include "check_hearing.h"
#include "compute_armor_class.h"
#include "compute_attack_damage.h"
#include "compute_attack_roll.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate.h"
#include "get_cached_npc.h"
#include "get_entity_name.h"
#include "get_entity_type.h"
#include "get_npc_dead.h"
#include "get_npc_xp.h"
#include "handle_attack_sfx.h"
#include "handle_durability_loss.h"
#include "handle_shield_block_sfx.h"
#include "magic_values.h"
#include "manage_inventory.h"
#include "play_sound.h"
#include "recompute_entity_cache.h"
#include "set_npc_dead.h"
#include "sfx.h"
#include "stat_bonus.h"
#include "update_npc_xp.h"

static inline void set_gamestate_flag_for_attack_animation(gamestate& g, entitytype_t type) {
    massert(type == ENTITY_PLAYER || type == ENTITY_NPC, "type is not player or npc!");
    if (type == ENTITY_PLAYER)
        g.flag = GAMESTATE_FLAG_PLAYER_ANIM;
    else if (type == ENTITY_NPC)
        g.flag = GAMESTATE_FLAG_NPC_ANIM;
    else
        merror("Unknown flag state, invalid type: %d", type);
}


static inline void process_attack_results(gamestate& g, entityid atk_id, entityid tgt_id, bool atk_successful) {
    massert(atk_id != ENTITYID_INVALID, "attacker entity id is invalid");
    massert(tgt_id != ENTITYID_INVALID, "target entity id is invalid");

    const string attacker_name = get_entity_name(g, atk_id);
    const string target_name = get_entity_name(g, tgt_id);
    const char* atk_name = attacker_name.c_str();
    const char* tgt_name = target_name.c_str();

    if (!atk_successful) {
        minfo("Missed attack");
        add_message_history(g, "%s swings at %s and misses!", atk_name, tgt_name);
        return;
    }

    if (get_npc_dead(g, tgt_id)) {
        minfo("Target is dead");
        add_message_history(g, "%s swings at a dead target", atk_name);
        return;
    }

    const int dmg = compute_attack_damage(g, atk_id, tgt_id);
    g.ct.set<damaged>(tgt_id, true);
    g.ct.set<update>(tgt_id, true);

    auto maybe_tgt_hp = g.ct.get<hp>(tgt_id);

    if (!maybe_tgt_hp.has_value()) {
        merror("target has no HP component");
        return;
    }

    const int tgt_hp = maybe_tgt_hp.value();

    if (tgt_hp <= 0) {
        merror("Target is already dead, hp was: %d", tgt_hp);
        set_npc_dead(g, tgt_id);
        return;
    }

    minfo("damage dealt");
    add_message_history(g, "%s deals %d damage to %s", atk_name, dmg, tgt_name);

    g.ct.set<hp>(tgt_id, tgt_hp - dmg);

    // decrement weapon durability
    handle_weapon_durability_loss(g, atk_id, tgt_id);

    if (tgt_hp > 0)
        return;

    set_npc_dead(g, tgt_id);

    switch (get_entity_type(g, tgt_id)) {
    case ENTITY_NPC: {
        update_npc_xp(g, atk_id, tgt_id);
        // handle item drops
        drop_all_from_inventory(g, tgt_id);
    } break;
    case ENTITY_PLAYER: add_message(g, "You died"); break;
    default: break;
    }
}


static inline attack_result_t process_attack_entity(gamestate& g, tile_t& tile, entityid attacker_id, entityid target_id) {
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
    if (target_id == ENTITYID_INVALID)
        return ATTACK_RESULT_MISS;
    const entitytype_t type = g.ct.get<entitytype>(target_id).value_or(ENTITY_NONE);
    if (type != ENTITY_PLAYER && type != ENTITY_NPC)
        return ATTACK_RESULT_MISS;
    if (g.ct.get<dead>(target_id).value_or(true))
        return ATTACK_RESULT_MISS;
    // they have a shield
    // still need to do attack successful check
    const bool attack_successful = compute_attack_roll(g, attacker_id, target_id);

    // attack unsuccessful
    if (!attack_successful) {
        process_attack_results(g, attacker_id, target_id, false);
        return ATTACK_RESULT_MISS;
    }

    // check for shield
    const entityid shield_id = g.ct.get<equipped_shield>(target_id).value_or(ENTITYID_INVALID);

    // if no shield
    if (shield_id == ENTITYID_INVALID) {
        process_attack_results(g, attacker_id, target_id, true);
        return ATTACK_RESULT_HIT;
    }

    // if has shield
    // compute chance to block
    const int roll = GetRandomValue(1, MAX_BLOCK_CHANCE);
    const int chance = g.ct.get<block_chance>(shield_id).value_or(MAX_BLOCK_CHANCE);
    const int low_roll = MAX_BLOCK_CHANCE - chance;
    if (roll <= low_roll) {
        // failed to block
        process_attack_results(g, attacker_id, target_id, true);
        return ATTACK_RESULT_HIT;
    }

    handle_shield_block_sfx(g, target_id);
    g.ct.set<block_success>(target_id, true);
    g.ct.set<update>(target_id, true);

    const string attacker_name = get_entity_name(g, attacker_id);
    const string target_name = get_entity_name(g, target_id);
    const char* atk_name = attacker_name.c_str();
    const char* tgt_name = target_name.c_str();

    minfo("attack blocked");
    add_message_history(g, "%s blocked an attack from %s", tgt_name, atk_name);
    return ATTACK_RESULT_BLOCK;
}


static inline void try_entity_attack(gamestate& g, entityid atk_id, int tgt_x, int tgt_y) {
    massert(!g.ct.get<dead>(atk_id).value_or(false), "attacker entity is dead");
    minfo("Trying to attack...");
    const vec3 loc = g.ct.get<location>(atk_id).value();
    auto df = d_get_floor(g.dungeon, loc.z);
    auto tile = df_tile_at(df, (vec3){tgt_x, tgt_y, loc.z});
    // Calculate direction based on target position
    const int dx = tgt_x - loc.x;
    const int dy = tgt_y - loc.y;
    g.ct.set<direction>(atk_id, get_dir_from_xy(dx, dy));
    g.ct.set<attacking>(atk_id, true);
    g.ct.set<update>(atk_id, true);
    const entityid npc_id = get_cached_npc(g, tile);
    const attack_result_t result = process_attack_entity(g, tile, atk_id, npc_id);
    // did the hero hear this event?
    handle_attack_sfx(g, atk_id, result);
    set_gamestate_flag_for_attack_animation(g, g.ct.get<entitytype>(atk_id).value_or(ENTITY_NONE));
}
