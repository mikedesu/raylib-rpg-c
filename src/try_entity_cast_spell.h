#pragma once

#include "add_message.h"
#include "check_hearing.h"
#include "create_spell.h"
#include "entityid.h"
#include "gamestate.h"
#include "manage_inventory.h"
#include "play_sound.h"
#include "sfx.h"
#include "tile_has_door.h"

static inline void try_entity_cast_spell(gamestate& g, entityid id, int tgt_x, int tgt_y) {
    minfo("Trying to cast spell...");

    const auto maybe_loc = g.ct.get<location>(id);
    if (!maybe_loc.has_value()) {
        merror("no location for entity id %d", id);
        return;
    }
    const vec3 loc = maybe_loc.value();

    auto floor = d_get_floor(g.dungeon, loc.z);
    const vec3 spell_loc = {tgt_x, tgt_y, loc.z};
    auto tile = df_tile_at(floor, spell_loc);

    // Calculate direction based on target position
    bool ok = false;
    const int dx = tgt_x - loc.x;
    const int dy = tgt_y - loc.y;
    g.ct.set<direction>(id, get_dir_from_xy(dx, dy));
    g.ct.set<casting>(id, true);
    g.ct.set<update>(id, true);

    // ok...
    // we are hard-coding a spell cast
    // in this example, we will 'create' a 'spell entity' of type 'fire' and place it on a tile
    const entityid spell_id = create_spell_at_with(g, spell_loc, [](gamestate& g, entityid id) {
        //...
        g.ct.set<spellstate>(id, SPELLSTATE_CAST);
        g.ct.set<spelltype>(id, SPELLTYPE_FIRE);
        g.ct.set<spell_casting>(id, true);
    });


    if (spell_id != ENTITYID_INVALID) {
        ok = true;

        // lets do an example of processing a spell effect immediately
        // first we need to iterate the entities on the tile
        // if there's an NPC we damage it

        if (tile_has_live_npcs(g, tile)) {
            entityid npcid = ENTITYID_INVALID;
            for (auto id : *tile.entities) {
                if (g.ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_NPC) {
                    npcid = id;
                    break;
                }
            }

            const int dmg = GetRandomValue(1, 6);

            g.ct.set<damaged>(npcid, true);
            g.ct.set<update>(npcid, true);

            auto maybe_tgt_hp = g.ct.get<hp>(npcid);
            if (!maybe_tgt_hp.has_value()) {
                merror("target has no HP component");
                return;
            }

            int tgt_hp = maybe_tgt_hp.value();
            if (tgt_hp <= 0) {
                merror("Target is already dead, hp was: %d", tgt_hp);
                g.ct.set<dead>(npcid, true);
                return;
            }

            tgt_hp -= dmg;
            g.ct.set<hp>(npcid, tgt_hp);

            if (tgt_hp > 0) {
                g.ct.set<dead>(npcid, false);
                return;
            }

            auto tgttype = g.ct.get<entitytype>(npcid).value_or(ENTITY_NONE);

            g.ct.set<dead>(npcid, true);
            if (tgttype == ENTITY_NPC) {
                // increment attacker's xp
                const int old_xp = g.ct.get<xp>(id).value_or(0);
                const int reward_xp = 1;
                const int new_xp = old_xp + reward_xp;
                g.ct.set<xp>(id, new_xp);

                // handle item drops
                drop_all_from_inventory(g, npcid);
            } else if (tgttype == ENTITY_PLAYER) {
                add_message(g, "You died");
            }
        }

        if (tile_has_door(g, spell_loc)) {
            // find the door id
            entityid doorid = ENTITYID_INVALID;
            for (auto id : *tile.entities) {
                if (g.ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_DOOR) {
                    doorid = id;
                    break;
                }
            }

            // mark it 'destroyed'
            // remove it from the tile
            if (doorid != ENTITYID_INVALID) {
                g.ct.set<destroyed>(doorid, true);
                df_remove_at(floor, doorid, spell_loc.x, spell_loc.y);
            }
        }

        g.ct.set<destroyed>(spell_id, true);
    }

    // did the hero hear this event?
    const bool event_heard = check_hearing(g, g.hero_id, (vec3){tgt_x, tgt_y, loc.z});

    if (ok) {
        // default metal on flesh
        //play_sound_if_heard(SFX_ITEM_FUSION, event_heard);
        if (event_heard)
            PlaySound(g.sfx[SFX_ITEM_FUSION]);
    } else {
        // need to select appropriate sound effect based on equipment- get attacker's equipped weapon if any
        //const entityid weapon_id = g.ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);
        // attacker has equipped weapon - get its type
        //const weapontype_t wpn_type = g.ct.get<weapontype>(g.ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID)).value_or(WEAPON_NONE);
        //const int index = wpn_type == WEAPON_SWORD    ? SFX_SLASH_ATTACK_SWORD_1
        //                  : wpn_type == WEAPON_AXE    ? SFX_SLASH_ATTACK_HEAVY_1
        //                  : wpn_type == WEAPON_DAGGER ? SFX_SLASH_ATTACK_LIGHT_1
        //                                              : SFX_SLASH_ATTACK_SWORD_1;
        //play_sound_if_heard(index, event_heard);
    }
}
