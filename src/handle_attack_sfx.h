#pragma once

#include "attack_result.h"
#include "check_hearing.h"
#include "gamestate.h"
#include "sfx.h"

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
