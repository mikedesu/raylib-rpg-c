#pragma once

#include "check_hearing.h"
#include "gamestate.h"
#include "sfx.h"

static inline void handle_shield_block_sfx(gamestate& g, entityid target_id) {
    const bool event_heard = check_hearing(g, g.hero_id, g.ct.get<location>(target_id).value_or((vec3){-1, -1, -1}));
    if (event_heard)
        PlaySound(g.sfx[SFX_HIT_METAL_ON_METAL]);
}
