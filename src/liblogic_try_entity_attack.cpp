#include "ComponentTraits.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate_flag.h"
#include "liblogic_add_message.h"
#include "liblogic_try_entity_attack.h"
#include "sfx.h"
#include "weapon.h"

void handle_attack_helper(shared_ptr<gamestate> g, shared_ptr<tile_t> tile, entityid attacker_id, bool* successful);
bool handle_attack_helper_innerloop(shared_ptr<gamestate> g, shared_ptr<tile_t> tile, int i, entityid attacker_id, bool* attack_successful);
void handle_attack_success(shared_ptr<gamestate> g, entityid atk_id, entityid tgt_id, bool* atk_successful);
void handle_attack_success_gamestate_flag(shared_ptr<gamestate> g, entitytype_t type, bool success);


void handle_attack_success_gamestate_flag(shared_ptr<gamestate> g, entitytype_t type, bool success) {
    if (!success)
        g->flag = type == ENTITY_PLAYER ? GAMESTATE_FLAG_PLAYER_ANIM : type == ENTITY_NPC ? GAMESTATE_FLAG_NPC_ANIM : GAMESTATE_FLAG_NONE;
    else if (type == ENTITY_PLAYER)
        g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
}


void try_entity_attack(shared_ptr<gamestate> g, entityid atk_id, int tgt_x, int tgt_y) {
    massert(g, "gamestate is NULL");
    massert(!g->ct.get<dead>(atk_id).value_or(false), "attacker entity is dead");
    minfo("Trying to attack...");

    const vec3 loc = g->ct.get<location>(atk_id).value();
    shared_ptr<dungeon_floor_t> floor = d_get_floor(g->dungeon, loc.z);
    massert(floor, "failed to get dungeon floor");

    shared_ptr<tile_t> tile = df_tile_at(floor, (vec3){tgt_x, tgt_y, loc.z});
    if (!tile) {
        merror("No tile at location, cannot attack");
        return;
    }

    // Calculate direction based on target position
    bool ok = false;

    const vec3 eloc = g->ct.get<location>(atk_id).value();
    const int dx = tgt_x - eloc.x;
    const int dy = tgt_y - eloc.y;

    g->ct.set<direction>(atk_id, get_dir_from_xy(dx, dy));
    g->ct.set<attacking>(atk_id, true);
    g->ct.set<update>(atk_id, true);

    handle_attack_helper(g, tile, atk_id, &ok);

    if (ok) {
        // default metal on flesh
        PlaySound(g->sfx->at(SFX_HIT_METAL_ON_FLESH));
    } else {
        // need to select appropriate sound effect based on equipment- get attacker's equipped weapon if any
        const entityid weapon_id = g->ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);
        if (weapon_id != ENTITYID_INVALID) {
            // attacker has equipped weapon - get its type
            const weapontype_t wpn_type = g->ct.get<weapontype>(weapon_id).value_or(WEAPON_NONE);
            const int index = wpn_type == WEAPON_SWORD    ? SFX_SLASH_ATTACK_SWORD_1
                              : wpn_type == WEAPON_AXE    ? SFX_SLASH_ATTACK_HEAVY_1
                              : wpn_type == WEAPON_DAGGER ? SFX_SLASH_ATTACK_LIGHT_1
                                                          : SFX_SLASH_ATTACK_SWORD_1;
            PlaySound(g->sfx->at(index));
        }
    }

    const entitytype_t type0 = g->ct.get<entitytype>(atk_id).value_or(ENTITY_NONE);
    handle_attack_success_gamestate_flag(g, type0, ok);
}


void handle_attack_helper(shared_ptr<gamestate> g, shared_ptr<tile_t> tile, entityid attacker_id, bool* successful) {
    massert(g, "gamestate is NULL");
    massert(tile, "tile is NULL");
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
    massert(successful, "attack_successful is NULL");

    for (int i = 0; (size_t)i < tile->entities->size(); i++) {
        handle_attack_helper_innerloop(g, tile, i, attacker_id, successful);
    }
}


bool handle_attack_helper_innerloop(shared_ptr<gamestate> g, shared_ptr<tile_t> tile, int i, entityid attacker_id, bool* attack_successful) {
    massert(g, "gamestate is NULL");
    massert(tile, "tile is NULL");
    massert(i >= 0, "i is out of bounds");
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
    massert(attack_successful, "attack_successful is NULL");

    const entityid target_id = tile->entities->at(i);
    if (target_id == ENTITYID_INVALID) {
        return false;
    }

    const entitytype_t type = g->ct.get<entitytype>(target_id).value_or(ENTITY_NONE);

    if (type != ENTITY_PLAYER && type != ENTITY_NPC) return false;
    if (g->ct.get<dead>(target_id).value_or(true)) {
        return false;
    }

    // lets try an experiment...
    // get the armor class of the target
    //int base_ac = g_get_stat(g, target_id, STATS_AC);
    //int base_str = g_get_stat(g, attacker_id, STATS_STR);
    //int str_bonus = bonus_calc(base_str);
    //int str_bonus = 0;
    //int atk_bonus = g_get_stat(g, attacker_id, STATS_ATTACK_BONUS);
    //int attack_roll = rand() % 20 + 1 + str_bonus + atk_bonus; // 1d20 + str bonus + attack bonus
    //*attack_successful = false;
    *attack_successful = true;
    //if (attack_roll >= base_ac) {
    //    return handle_shield_check(g, attacker_id, target_id, attack_roll, base_ac, attack_successful);
    //}
    // attack misses
    handle_attack_success(g, attacker_id, target_id, attack_successful);
    return false;
}


void handle_attack_success(shared_ptr<gamestate> g, entityid atk_id, entityid tgt_id, bool* atk_successful) {
    massert(g, "gamestate is NULL");
    massert(atk_id != ENTITYID_INVALID, "attacker entity id is invalid");
    massert(tgt_id != ENTITYID_INVALID, "target entity id is invalid");
    massert(atk_successful, "attack_successful is NULL");

    const entitytype_t tgttype = g->ct.get<entitytype>(tgt_id).value_or(ENTITY_NONE);

    if (*atk_successful) {
        //msuccess("Attack was SUCCESSFUL!");
        const entityid equipped_wpn = g->ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);
        const vec3 dmg_range = g->ct.get<damage>(equipped_wpn).value_or((vec3){1, 2, 0});
        const int dmg = GetRandomValue(dmg_range.x, dmg_range.y);

        g->ct.set<damaged>(tgt_id, true);
        g->ct.set<update>(tgt_id, true);

        optional<int> maybe_tgt_hp = g->ct.get<hp>(tgt_id);
        if (maybe_tgt_hp.has_value()) {
            int tgt_hp = maybe_tgt_hp.value();
            if (tgt_hp <= 0) {
                merror("Target is already dead, hp was: %d", tgt_hp);
                g->ct.set<dead>(tgt_id, true);
                return;
            }
            tgt_hp -= dmg;
            g->ct.set<hp>(tgt_id, tgt_hp);
            //if (tgttype == ENTITY_PLAYER) {
            //add_message_history(g,
            //                    "%s attacked you for %d damage!",
            //                    g_get_name(g, atk_id).c_str(),
            //                    dmg);
            //} else if (tgttype == ENTITY_NPC) {
            //add_message_history(g,
            //                    "%s attacked %s for %d damage!",
            //                    g_get_name(g, atk_id).c_str(),
            //                    g_get_name(g, tgt_id).c_str(),
            //                    dmg);
            //}

            // get the equipped weapon of the attacker
            entityid wpn_id = g->ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);

            int dura = g->ct.get<durability>(wpn_id).value_or(0);
            g->ct.set<durability>(wpn_id, dura - 1 < 0 ? 0 : dura - 1);

            if (tgt_hp <= 0) {
                //g_update_dead(g, tgt_id, true);
                g->ct.set<dead>(tgt_id, true);
                if (tgttype == ENTITY_NPC) {
                    // increment attacker's xp
                    const int old_xp = g->ct.get<xp>(atk_id).value_or(0);
                    const int reward_xp = 1;
                    const int new_xp = old_xp + reward_xp;
                    g->ct.set<xp>(atk_id, new_xp);
                    // handle item drops?
                    // ...
                    //vec3 loc = g_get_loc(g, tgt_id);
                    //vec3 loc_cast = {loc.x, loc.y, loc.z};
                    //entityid id = ENTITYID_INVALID;
                    //while (id == ENTITYID_INVALID)
                    //    id = potion_create(g,
                    //                       loc,
                    //                       POTION_HEALTH_SMALL,
                    //                       "small health potion");
                } else {
                    add_message(g, "You died");
                }
            } else {
                g->ct.set<dead>(tgt_id, false);
            }
        } else {
            merror("Target does not have an HP component");
        }
    } else {
        merror("Attack MISSED");
    }
}
