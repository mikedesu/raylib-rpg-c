#pragma once

#include "check_hearing.h"
#include "create_spell.h"
#include "entityid.h"
#include "gamestate.h"
#include "play_sound.h"
#include "sfx.h"

static inline void try_entity_cast_spell(shared_ptr<gamestate> g, entityid id, int tgt_x, int tgt_y) {
    massert(g, "gamestate is NULL");
    //massert(!g->ct.get<dead>(atk_id).value_or(false), "attacker entity is dead");
    minfo("Trying to cast spell...");

    const auto maybe_loc = g->ct.get<location>(id);
    if (!maybe_loc.has_value()) {
        merror("no location for entity id %d", id);
        return;
    }
    const vec3 loc = maybe_loc.value();

    auto floor = d_get_floor(g->dungeon, loc.z);
    massert(floor, "failed to get dungeon floor");
    auto tile = df_tile_at(floor, (vec3){tgt_x, tgt_y, loc.z});
    if (!tile) {
        merror("No tile at location, cannot attack");
        return;
    }

    // Calculate direction based on target position
    bool ok = false;
    const int dx = tgt_x - loc.x;
    const int dy = tgt_y - loc.y;
    g->ct.set<direction>(id, get_dir_from_xy(dx, dy));
    g->ct.set<casting>(id, true);
    g->ct.set<update>(id, true);
    //handle_attack_helper(g, tile, atk_id, &ok);

    // ok...
    // we are hard-coding a spell cast
    // in this example, we will 'create' a 'spell entity' of type 'fire' and place it on a tile

    const entityid spell_id = create_spell_at_with(g, (vec3){tgt_x, tgt_y, loc.z}, [](shared_ptr<gamestate> g, entityid id) {
        //...
        g->ct.set<spellstate>(id, SPELLSTATE_CAST);
        g->ct.set<spelltype>(id, SPELLTYPE_FIRE);
    });

    if (spell_id != ENTITYID_INVALID) {
        ok = true;
    }


    // did the hero hear this event?
    //const vec3 hero_loc = g->ct.get<location>(g->hero_id).value_or((vec3){-1, -1, -1});
    //const float hx = static_cast<float>(hero_loc.x);
    //const float hy = static_cast<float>(hero_loc.y);
    //const float tx = static_cast<float>(tgt_x);
    //const float ty = static_cast<float>(tgt_y);
    //const Vector2 p0 = {hx, hy};
    //const Vector2 p1 = {tx, ty};
    //float dist = Vector2Distance(p0, p1);
    //float hearing = g->ct.get<hearing_distance>(g->hero_id).value_or(3);
    const bool event_heard = check_hearing(g, g->hero_id, (vec3){tgt_x, tgt_y, loc.z});

    if (ok) {
        // default metal on flesh
        play_sound_if_heard(SFX_ITEM_FUSION, event_heard);
    } else {
        // need to select appropriate sound effect based on equipment- get attacker's equipped weapon if any
        //const entityid weapon_id = g->ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);
        // attacker has equipped weapon - get its type
        //const weapontype_t wpn_type = g->ct.get<weapontype>(g->ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID)).value_or(WEAPON_NONE);
        //const int index = wpn_type == WEAPON_SWORD    ? SFX_SLASH_ATTACK_SWORD_1
        //                  : wpn_type == WEAPON_AXE    ? SFX_SLASH_ATTACK_HEAVY_1
        //                  : wpn_type == WEAPON_DAGGER ? SFX_SLASH_ATTACK_LIGHT_1
        //                                              : SFX_SLASH_ATTACK_SWORD_1;
        //play_sound_if_heard(index, event_heard);
    }

    //const entitytype_t type = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
    //g->flag = type == ENTITY_PLAYER ? GAMESTATE_FLAG_PLAYER_ANIM : type == ENTITY_NPC ? GAMESTATE_FLAG_NPC_ANIM : GAMESTATE_FLAG_NONE;


    //const auto type = g->ct.get<entitytype>(atk_id).value_or(ENTITY_NONE);
    //handle_attack_success_gamestate_flag(g, type, ok);
}
