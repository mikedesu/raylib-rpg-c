
#pragma once

#include "add_message.h"
#include "check_hearing.h"
#include "compute_armor_class.h"
#include "entitytype.h"
#include "gamestate.h"
#include "manage_inventory.h"
#include "play_sound.h"
#include "sfx.h"
#include "stat_bonus.h"


static inline void set_gamestate_flag_for_attack_animation(gamestate& g, entitytype_t type, bool success) {
    if (!success) {
        if (type == ENTITY_PLAYER)
            g.flag = GAMESTATE_FLAG_PLAYER_ANIM;
        else if (type == ENTITY_NPC)
            g.flag = GAMESTATE_FLAG_NPC_ANIM;
        else
            g.flag = GAMESTATE_FLAG_NONE;

    } else if (type == ENTITY_PLAYER)
        g.flag = GAMESTATE_FLAG_PLAYER_ANIM;
}

static inline void process_attack_results(gamestate& g, entityid atk_id, entityid tgt_id, bool atk_successful) {
    massert(atk_id != ENTITYID_INVALID, "attacker entity id is invalid");
    massert(tgt_id != ENTITYID_INVALID, "target entity id is invalid");
    //massert(atk_successful, "attack_successful is NULL");

    const entitytype_t tgttype = g.ct.get<entitytype>(tgt_id).value_or(ENTITY_NONE);

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

    // get the equipped weapon of the attacker
    const entityid wpn_id = g.ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);

    // decrement its durability
    auto maybe_dura = g.ct.get<durability>(wpn_id);

    if (maybe_dura.has_value()) {
        const int dura = maybe_dura.value();
        g.ct.set<durability>(wpn_id, dura - 1 < 0 ? 0 : dura - 1);
        if (dura == 0) {
            // item destroyed
            g.ct.set<destroyed>(wpn_id, true);
            // remove item from attacker's inventory
            remove_from_inventory(g, atk_id, wpn_id);
            // unequip item
            g.ct.set<equipped_weapon>(atk_id, ENTITYID_INVALID);
            const bool event_heard = check_hearing(g, g.hero_id, g.ct.get<location>(tgt_id).value_or((vec3){-1, -1, -1}));
            if (event_heard)
                PlaySound(g.sfx[SFX_05_ALCHEMY_GLASS_BREAK]);

            add_message_history(g, "%s broke!", g.ct.get<name>(wpn_id).value_or("no-name").c_str());
        }
    }

    if (tgt_hp > 0) {
        g.ct.set<dead>(tgt_id, false);
        return;
    }

    g.ct.set<dead>(tgt_id, true);

    switch (tgttype) {
    case ENTITY_NPC: {
        //const int old_xp = g.ct.get<xp>(atk_id).value_or(0);
        //const int reward_xp = 1;
        //const int new_xp = old_xp + reward_xp;
        g.ct.set<xp>(atk_id, g.ct.get<xp>(atk_id).value_or(0) + 1);
        // handle item drops
        drop_all_from_inventory(g, tgt_id);
    } break;
    case ENTITY_PLAYER: add_message(g, "You died"); break;
    default: break;
    }

    if (tgttype == ENTITY_NPC) {
        // increment attacker's xp
        const int old_xp = g.ct.get<xp>(atk_id).value_or(0);
        const int reward_xp = 1;
        const int new_xp = old_xp + reward_xp;
        g.ct.set<xp>(atk_id, new_xp);

        // handle item drops
        drop_all_from_inventory(g, tgt_id);
    } else if (tgttype == ENTITY_PLAYER) {
        add_message(g, "You died");
    }
}


static inline bool process_attack_entity(gamestate& g, tile_t& tile, int i, entityid attacker_id) {
    massert(i >= 0, "i is out of bounds");
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");

    const entityid target_id = tile.entities->at(i);
    if (target_id == ENTITYID_INVALID)
        return false;

    const entitytype_t type = g.ct.get<entitytype>(target_id).value_or(ENTITY_NONE);
    if (type != ENTITY_PLAYER && type != ENTITY_NPC)
        return false;

    if (g.ct.get<dead>(target_id).value_or(true))
        return false;

    // lets try an experiment...
    // get the armor class of the target
    //int base_ac = g_get_stat(g, target_id, STATS_AC);
    //int base_str = g_get_stat(g, attacker_id, STATS_STR);
    //int str_bonus = bonus_calc(base_str);
    //int str_bonus = 0;
    //int atk_bonus = g_get_stat(g, attacker_id, STATS_ATTACK_BONUS);
    //int attack_roll = rand() % 20 + 1 + str_bonus + atk_bonus; // 1d20 + str bonus + attack bonus
    //if (attack_roll >= base_ac) {

    auto maybe_shield = g.ct.get<equipped_shield>(target_id);
    if (!maybe_shield.has_value()) {
        // no shield
        // compute attack roll
        // eventually we will need to select str or dex bonus
        // depending on weapon type, class, etc
        const int roll = GetRandomValue(1, 20) + get_stat_bonus(g.ct.get<strength>(attacker_id).value_or(10));
        const int target_ac = compute_armor_class(g, target_id);
        const bool attack_successful = roll >= target_ac;
        process_attack_results(g, attacker_id, target_id, attack_successful);
        return attack_successful;
    }

    const auto shield_id = maybe_shield.value();
    if (shield_id == ENTITYID_INVALID) {
        // no shield
        // compute attack roll
        const int roll = GetRandomValue(1, 20) + get_stat_bonus(g.ct.get<strength>(attacker_id).value_or(10));
        const int target_ac = compute_armor_class(g, target_id);
        const bool attack_successful = roll >= target_ac;
        process_attack_results(g, attacker_id, target_id, attack_successful);
        return attack_successful;
    }

    // they have a shield
    // compute chance to block
    const int roll = GetRandomValue(1, 100);
    const int chance = g.ct.get<block_chance>(shield_id).value_or(100);
    const int low_roll = 100 - chance;
    if (low_roll == 0 && shield_id != ENTITYID_INVALID) {
        // 100% block chance
        const bool event_heard = check_hearing(g, g.hero_id, g.ct.get<location>(target_id).value_or((vec3){-1, -1, -1}));
        if (event_heard)
            PlaySound(g.sfx[SFX_HIT_METAL_ON_METAL]);
        g.ct.set<block_success>(target_id, true);
        g.ct.set<update>(target_id, true);
        add_message_history(
            g, "%s blocked an attack from %s", g.ct.get<name>(target_id).value_or("no-name").c_str(), g.ct.get<name>(attacker_id).value_or("no-name").c_str());
        return false;
    } else if (roll <= low_roll) {
        // failed to block
        // compute attack roll
        const int roll = GetRandomValue(1, 20) + get_stat_bonus(g.ct.get<strength>(attacker_id).value_or(10));
        const int target_ac = compute_armor_class(g, target_id);
        const bool attack_successful = roll >= target_ac;
        process_attack_results(g, attacker_id, target_id, attack_successful);
        return attack_successful;
    } else {
        // block successful
        if (shield_id != ENTITYID_INVALID) {
            const bool event_heard = check_hearing(g, g.hero_id, g.ct.get<location>(target_id).value_or((vec3){-1, -1, -1}));
            if (event_heard)
                PlaySound(g.sfx[SFX_HIT_METAL_ON_METAL]);
            g.ct.set<block_success>(target_id, true);
            g.ct.set<update>(target_id, true);
            add_message_history(g,
                                "%s blocked an attack from %s",
                                g.ct.get<name>(target_id).value_or("no-name").c_str(),
                                g.ct.get<name>(attacker_id).value_or("no-name").c_str());
        }
    }
    return false;
}

static inline bool process_attack_entities(gamestate& g, tile_t& tile, entityid attacker_id) {
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
    bool ok = false;
    for (int i = 0; (size_t)i < tile.entities->size(); i++)
        ok |= process_attack_entity(g, tile, i, attacker_id);
    return ok;
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

    bool ok = process_attack_entities(g, tile, atk_id);

    // did the hero hear this event?
    const bool event_heard = check_hearing(g, g.hero_id, (vec3){tgt_x, tgt_y, loc.z});

    if (ok && event_heard) {
        // default metal on flesh
        PlaySound(g.sfx[SFX_HIT_METAL_ON_FLESH]);

    } else if (event_heard) {
        // need to select appropriate sound effect based on equipment- get attacker's equipped weapon if any
        //const entityid weapon_id = g.ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);
        // attacker has equipped weapon - get its type
        const weapontype_t wpn_type = g.ct.get<weapontype>(g.ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID)).value_or(WEAPON_NONE);
        const int index = wpn_type == WEAPON_SWORD    ? SFX_SLASH_ATTACK_SWORD_1
                          : wpn_type == WEAPON_AXE    ? SFX_SLASH_ATTACK_HEAVY_1
                          : wpn_type == WEAPON_DAGGER ? SFX_SLASH_ATTACK_LIGHT_1
                                                      : SFX_SLASH_ATTACK_SWORD_1;
        PlaySound(g.sfx[index]);
    }

    set_gamestate_flag_for_attack_animation(g, g.ct.get<entitytype>(atk_id).value_or(ENTITY_NONE), ok);
}
