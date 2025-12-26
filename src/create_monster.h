#pragma once


#include "create_npc.h"
#include "entity_templates.h"
#include "gamestate.h"
#include "race.h"


static inline race_t random_monster_type() {
    const vector<race_t> monster_races = {RACE_GOBLIN, RACE_ORC, RACE_BAT, RACE_WOLF, RACE_WARG, RACE_ZOMBIE, RACE_SKELETON, RACE_RAT, RACE_GREEN_SLIME};
    const int random_index = GetRandomValue(0, monster_races.size() - 1);
    return monster_races[random_index];
}

static inline entityid create_random_monster_with(gamestate& g, function<void(gamestate&, entityid)> monsterInitFunction) {
    const race_t r = random_monster_type();
    //const race_t r = RACE_BAT;
    function<void(gamestate&, entityid)> hook;
    if (r == RACE_ORC) {
        hook = [&monsterInitFunction](gamestate& g, entityid id) {
            orc_init_test(g, id);
            monsterInitFunction(g, id);
        };
    } else {
        hook = [&monsterInitFunction, r](gamestate& g, entityid id) {
            g.ct.set<name>(id, race2str(r));
            monsterInitFunction(g, id);
        };
    }
    return create_npc_with(g, r, hook);
}

static inline entityid create_random_monster_at_with(gamestate& g, vec3 loc, function<void(gamestate&, entityid)> monsterInitFunction) {
    auto df = d_get_floor(g.dungeon, loc.z);
    auto tile = df_tile_at(df, loc);
    if (!tile_is_walkable(tile.type))
        return ENTITYID_INVALID;
    if (tile_has_live_npcs(g, tile))
        return ENTITYID_INVALID;
    //function<void(gamestate&, entityid)> hook;
    //if (r == RACE_ORC) {
    //    hook = [&monsterInitFunction](gamestate& g, entityid id) {
    //        orc_init_test(g, id);
    //        monsterInitFunction(g, id);
    //    };
    //} else {
    //    hook = [&monsterInitFunction, r](gamestate& g, entityid id) {
    //        g.ct.set<name>(id, race2str(r));
    //        monsterInitFunction(g, id);
    //    };
    //}

    //const auto id = create_random_monster_with(g, monsterInitFunction);
    const auto id = create_random_monster_with(g, monsterInitFunction);
    if (id == ENTITYID_INVALID)
        return ENTITYID_INVALID;
    minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
    if (!df_add_at(df, id, loc.x, loc.y))
        return ENTITYID_INVALID;
    g.ct.set<location>(id, loc);
    g.ct.set<update>(id, true);
    return id;
}
