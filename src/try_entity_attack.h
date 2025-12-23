
#pragma once

#include "add_message.h"
#include "attack_result.h"
#include "check_hearing.h"
#include "compute_armor_class.h"
#include "compute_attack_roll.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate.h"
#include "get_cached_npc.h"
#include "handle_durability_loss.h"
#include "manage_inventory.h"
#include "play_sound.h"
#include "recompute_entity_cache.h"
#include "set_npc_dead.h"
#include "sfx.h"
#include "stat_bonus.h"

static inline void set_gamestate_flag_for_attack_animation(gamestate& g, entitytype_t type) {
    massert(type == ENTITY_PLAYER || type == ENTITY_NPC, "type is not player or npc!");
    if (type == ENTITY_PLAYER)
        g.flag = GAMESTATE_FLAG_PLAYER_ANIM;
    else if (type == ENTITY_NPC)
        g.flag = GAMESTATE_FLAG_NPC_ANIM;
    else
        merror("Unknown flag state, invalid type: %d", type);
}


static inline void handle_attack_sfx(gamestate& g, entityid attacker, attack_result_t result) {
    if (!check_hearing(g, g.hero_id, g.ct.get<location>(attacker).value_or((vec3){-1, -1, -1})))
        return;
    int index = SFX_SLASH_ATTACK_SWORD_1;
    if (result == ATTACK_RESULT_BLOCK) {
        index = SFX_HIT_METAL_ON_METAL;
    } else if (result == ATTACK_RESULT_HIT) {
        index = SFX_HIT_METAL_ON_FLESH;
    } else if (result == ATTACK_RESULT_MISS) {
        const entityid weapon_id = g.ct.get<equipped_weapon>(attacker).value_or(ENTITYID_INVALID);
        const weapontype_t wpn_type = g.ct.get<weapontype>(weapon_id).value_or(WEAPON_NONE);
        index = wpn_type == WEAPON_SWORD    ? SFX_SLASH_ATTACK_SWORD_1
                : wpn_type == WEAPON_AXE    ? SFX_SLASH_ATTACK_HEAVY_1
                : wpn_type == WEAPON_DAGGER ? SFX_SLASH_ATTACK_LIGHT_1
                                            : SFX_SLASH_ATTACK_SWORD_1;
    }
    PlaySound(g.sfx[index]);
}


static inline void handle_shield_block_sfx(gamestate& g, entityid target_id) {
    const bool event_heard = check_hearing(g, g.hero_id, g.ct.get<location>(target_id).value_or((vec3){-1, -1, -1}));
    if (event_heard) {
        PlaySound(g.sfx[SFX_HIT_METAL_ON_METAL]);
    }
}


static inline void process_attack_results(gamestate& g, entityid atk_id, entityid tgt_id, bool atk_successful) {
    massert(atk_id != ENTITYID_INVALID, "attacker entity id is invalid");
    massert(tgt_id != ENTITYID_INVALID, "target entity id is invalid");
    if (!atk_successful) {
        minfo("Missed attack");
        add_message_history(
            g, "%s swings at %s and misses!", g.ct.get<name>(atk_id).value_or("no-name").c_str(), g.ct.get<name>(tgt_id).value_or("no-name").c_str());
        return;
    }
    const entityid equipped_wpn = g.ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);
    const vec3 dmg_range = g.ct.get<damage>(equipped_wpn).value_or((vec3){1, 2, 0});
    const int dmg = GetRandomValue(dmg_range.x, dmg_range.y);
    g.ct.set<damaged>(tgt_id, true);
    g.ct.set<update>(tgt_id, true);
    auto maybe_tgt_hp = g.ct.get<hp>(tgt_id);
    if (!maybe_tgt_hp.has_value()) {
        merror("target has no HP component");
        return;
    }
    int tgt_hp = maybe_tgt_hp.value();
    if (tgt_hp <= 0) {
        merror("Target is already dead, hp was: %d", tgt_hp);
        g.ct.set<dead>(tgt_id, true);
        return;
    }
    add_message_history(
        g, "%s deals %d damage to %s", g.ct.get<name>(atk_id).value_or("no-name").c_str(), dmg, g.ct.get<name>(tgt_id).value_or("no-name").c_str());
    tgt_hp -= dmg;
    g.ct.set<hp>(tgt_id, tgt_hp);
    // decrement weapon durability
    handle_weapon_durability_loss(g, atk_id, tgt_id);
    if (tgt_hp > 0) {
        g.ct.set<dead>(tgt_id, false);
        return;
    }
    set_npc_dead(g, tgt_id);
    const entitytype_t tgttype = g.ct.get<entitytype>(tgt_id).value_or(ENTITY_NONE);
    switch (tgttype) {
    case ENTITY_NPC: {
        const int old_xp = g.ct.get<xp>(atk_id).value_or(0);
        const int reward_xp = 1;
        const int new_xp = old_xp + reward_xp;
        g.ct.set<xp>(atk_id, new_xp);
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
    if (attack_successful) {
        // check for shield
        const entityid shield_id = g.ct.get<equipped_shield>(target_id).value_or(ENTITYID_INVALID);
        // if has shield
        if (shield_id != ENTITYID_INVALID) {
            // compute chance to block
            const int roll = GetRandomValue(1, 100);
            const int chance = g.ct.get<block_chance>(shield_id).value_or(100);
            const int low_roll = 100 - chance;
            if (roll <= low_roll) {
                // failed to block
                process_attack_results(g, attacker_id, target_id, attack_successful); // <===== ############
                return ATTACK_RESULT_HIT;
            }
            // block successful
            handle_shield_block_sfx(g, target_id);
            g.ct.set<block_success>(target_id, true);
            g.ct.set<update>(target_id, true);
            add_message_history(g,
                                "%s blocked an attack from %s",
                                g.ct.get<name>(target_id).value_or("no-name").c_str(),
                                g.ct.get<name>(attacker_id).value_or("no-name").c_str());
            return ATTACK_RESULT_BLOCK;
        }
        // if no shield
        process_attack_results(g, attacker_id, target_id, attack_successful); // <===== ############
        return ATTACK_RESULT_HIT;
    }
    // attack unsuccessful
    process_attack_results(g, attacker_id, target_id, attack_successful); // <===== ############
    return ATTACK_RESULT_MISS;
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
