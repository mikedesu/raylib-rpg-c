#include "ComponentTraits.h"
#include "entityid.h"
#include "liblogic_add_message.h"
#include "liblogic_try_entity_attack.h"
#include "sfx.h"
#include "weapon.h"

void handle_attack_helper(shared_ptr<gamestate> g, shared_ptr<tile_t> tile, entityid attacker_id, bool* successful);
bool handle_attack_helper_innerloop(shared_ptr<gamestate> g, shared_ptr<tile_t> tile, int i, entityid attacker_id, bool* attack_successful);
void handle_attack_success(shared_ptr<gamestate> g, entityid atk_id, entityid tgt_id, bool* atk_successful);
void handle_attack_success_gamestate_flag(shared_ptr<gamestate> g, entitytype_t type, bool success);

void try_entity_attack(shared_ptr<gamestate> g, entityid atk_id, int tgt_x, int tgt_y) {
    massert(g, "gamestate is NULL");
    massert(!g->ct.get<dead>(atk_id).value_or(false), "attacker entity is dead");

    minfo("Trying to attack...");

    vec3 loc = g->ct.get<location>(atk_id).value();
    shared_ptr<dungeon_floor_t> floor = d_get_floor(g->dungeon, loc.z);
    massert(floor, "failed to get dungeon floor");
    shared_ptr<tile_t> tile = df_tile_at(floor, (vec3){tgt_x, tgt_y, loc.z});
    if (!tile) {
        return;
    }
    // Calculate direction based on target position
    bool ok = false;
    //vec3 eloc = g_get_loc(g, atk_id);
    vec3 eloc = g->ct.get<location>(atk_id).value();
    int dx = tgt_x - eloc.x;
    int dy = tgt_y - eloc.y;
    g->ct.set<direction>(atk_id, get_dir_from_xy(dx, dy));
    g->ct.set<attacking>(atk_id, true);
    g->ct.set<update>(atk_id, true);


    handle_attack_helper(g, tile, atk_id, &ok);

    if (ok) {
        // default metal on flesh
        PlaySound(g->sfx->at(SFX_HIT_METAL_ON_FLESH));
    } else {
        // need to select appropriate sound effect based on equipment- get attacker's equipped weapon if any
        entityid weapon_id = g->ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);
        int index = SFX_SLASH_ATTACK_SWORD_1;
        if (weapon_id != ENTITYID_INVALID) {
            // attacker has equipped weapon - get its type
            weapontype_t wpn_type = g->ct.get<weapontype>(weapon_id).value_or(WEAPON_NONE);
            index = wpn_type == WEAPON_SWORD    ? SFX_SLASH_ATTACK_SWORD_1
                    : wpn_type == WEAPON_AXE    ? SFX_SLASH_ATTACK_HEAVY_1
                    : wpn_type == WEAPON_DAGGER ? SFX_SLASH_ATTACK_LIGHT_1
                                                : 0;
        }
        PlaySound(g->sfx->at(index));
    }


    entitytype_t type0 = g->ct.get<entitytype>(atk_id).value_or(ENTITY_NONE);
    handle_attack_success_gamestate_flag(g, type0, ok);
}


void handle_attack_helper(shared_ptr<gamestate> g, shared_ptr<tile_t> tile, entityid attacker_id, bool* successful) {
    massert(g, "gamestate is NULL");
    massert(tile, "tile is NULL");
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
    massert(successful, "attack_successful is NULL");
    //for (size_t i = 0; i < tile->entity_max; i++) {
    for (int i = 0; (size_t)i < tile->entities->size(); i++) {
        handle_attack_helper_innerloop(g, tile, i, attacker_id, successful);
    }
}


bool handle_attack_helper_innerloop(shared_ptr<gamestate> g, shared_ptr<tile_t> tile, int i, entityid attacker_id, bool* attack_successful) {
    massert(g, "gamestate is NULL");
    massert(tile, "tile is NULL");
    massert(i >= 0, "i is out of bounds");
    //massert((size_t)i < tile->entity_max, "i is out of bounds");
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
    massert(attack_successful, "attack_successful is NULL");
    entityid target_id = tile->entities->at(i);
    if (target_id == ENTITYID_INVALID) {
        return false;
    }

    entitytype_t type = g->ct.get<entitytype>(target_id).value_or(ENTITY_NONE);

    if (type != ENTITY_PLAYER && type != ENTITY_NPC) return false;
    if (g->ct.get<dead>(target_id).value_or(true)) {
        return false;
    }
    //    // lets try an experiment...
    //    // get the armor class of the target
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
    entitytype_t tgttype = g->ct.get<entitytype>(tgt_id).value_or(ENTITY_NONE);
    if (*atk_successful) {
        msuccess("Attack was SUCCESSFUL!");
        //entityid attacker_weapon_id = g_get_equipment(g, atk_id, EQUIP_SLOT_WEAPON);
        //entityid attacker_weapon_id = g_get_equipped_weapon(g, atk_id);
        int dmg = 1;
        //if (attacker_weapon_id == ENTITYID_INVALID) {
        // no weapon
        // get the entity's base attack damage
        //vec3 dmg_roll = g_get_base_attack_damage(g, atk_id);
        //dmg = do_roll(dmg_roll);
        //int atk_bonus = g_get_stat(g, atk_id, STATS_ATTACK_BONUS);
        //dmg += atk_bonus;
        //} else {
        // weapon
        // we will calculate damage based on weapon attributes
        //massert(g_has_damage(g, attacker_weapon_id),
        //        "attacker weapon does not have damage attached");
        //vec3 dmg_roll = g_get_damage(g, attacker_weapon_id);
        //dmg = do_roll(dmg_roll);
        //int atk_bonus = g_get_stat(g, atk_id, STATS_ATTACK_BONUS);
        //dmg += atk_bonus;
        //}
        //    g_set_damaged(g, tgt_id, true);
        g->ct.set<damaged>(tgt_id, true);
        //    g_set_update(g, tgt_id, true);
        g->ct.set<update>(tgt_id, true);
        //int hp = g_get_stat(g, tgt_id, STATS_HP);

        //int hp = 1;

        optional<int> maybe_tgt_hp = g->ct.get<hp>(tgt_id);
        if (maybe_tgt_hp.has_value()) {
            int tgt_hp = maybe_tgt_hp.value();
            if (tgt_hp <= 0) {
                merror("Target is already dead, hp was: %d", tgt_hp);
                //g_update_dead(g, tgt_id, true);
                g->ct.set<dead>(tgt_id, true);
                return;
            }
            tgt_hp -= dmg;
            g->ct.set<hp>(tgt_id, tgt_hp);
            //g_set_stat(g, tgt_id, STATS_HP, hp);
            if (tgttype == ENTITY_PLAYER) {
                //add_message_history(g,
                //                    "%s attacked you for %d damage!",
                //                    g_get_name(g, atk_id).c_str(),
                //                    dmg);
            } else if (tgttype == ENTITY_NPC) {
                //add_message_history(g,
                //                    "%s attacked %s for %d damage!",
                //                    g_get_name(g, atk_id).c_str(),
                //                    g_get_name(g, tgt_id).c_str(),
                //                    dmg);
            }
            if (tgt_hp <= 0) {
                //g_update_dead(g, tgt_id, true);
                g->ct.set<dead>(tgt_id, true);
                if (tgttype == ENTITY_NPC) {
                    //add_message_history(g,
                    //                    "%s killed %s!",
                    //                    g_get_name(g, atk_id).c_str(),
                    //                    g_get_name(g, tgt_id).c_str());
                    // increment attacker's xp
                    //int old_xp = g_get_stat(g, atk_id, STATS_XP);
                    //massert(old_xp >= 0, "attacker's xp is negative");
                    //int reward_xp = calc_reward_xp(g, atk_id, tgt_id);
                    //int reward_xp = 1;
                    //massert(reward_xp >= 0, "reward xp is negative");
                    //int new_xp = old_xp + reward_xp;
                    //massert(new_xp >= 0, "new xp is negative");
                    //g_set_stat(g, atk_id, STATS_XP, new_xp);
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
                //g_update_dead(g, tgt_id, false);
                g->ct.set<dead>(tgt_id, false);
            }
        } else {
            merror("Target does not have an HP component");
        }
    } else {
        merror("Attack MISSED");
    }
    // handle attack miss
    //if (tgttype == ENTITY_PLAYER)
    //add_message_history(
    //    g, "%s's attack missed!", g_get_name(g, atk_id).c_str());
    //else if (tgttype == ENTITY_NPC)
    //add_message_history(g,
    //                    "%s missed %s!",
    //                    g_get_name(g, atk_id).c_str(),
    //                    g_get_name(g, tgt_id).c_str());
    //}
    //e_set_hp(target, e_get_hp(target) - dmg); // Reduce HP by 1
    //if (target->type == ENTITY_PLAYER) add_message_and_history(g, "%s attacked you for %d damage!", attacker->name, dmg);
    //if (attacker->type == ENTITY_PLAYER) add_message_and_history(g, "%s attacked %s for %d damage!", attacker->name, target->name, dmg);
    //if (e_get_hp(target) <= 0) {
    //target->dead = true;
    //g_update_dead(g, tgt_id, true);
    //}
}


void handle_attack_success_gamestate_flag(shared_ptr<gamestate> g, entitytype_t type, bool success) {
    //g->flag = success && type == ENTITY_PLAYER    ? GAMESTATE_FLAG_PLAYER_ANIM
    //          : !success && type == ENTITY_PLAYER ? GAMESTATE_FLAG_PLAYER_ANIM
    //          : !success && type == ENTITY_NPC    ? GAMESTATE_FLAG_NPC_ANIM
    //          : !success                          ? GAMESTATE_FLAG_NONE
    //                                              : g->flag;
    if (success) {
        if (type == ENTITY_PLAYER) {
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        }
    } else {
        if (type == ENTITY_PLAYER) {
            g->flag = GAMESTATE_FLAG_PLAYER_ANIM;
        } else if (type == ENTITY_NPC) {
            g->flag = GAMESTATE_FLAG_NPC_ANIM;
        } else {
            // this should NEVER happen!
            g->flag = GAMESTATE_FLAG_NONE;
        }
    }
}
