#pragma once

#include "gamestate.h"
#include <vector>

static inline vector<string> build_item_detail_lines(gamestate& g, entityid selection_id) {
    vector<string> lines;
    const itemtype_t item_type = g.ct.get<itemtype>(selection_id).value_or(ITEM_NONE);

    lines.push_back(g.ct.get<name>(selection_id).value_or("no-name"));

    if (item_type == ITEM_WEAPON) {
        const vec3 dmg = g.ct.get<damage>(selection_id).value_or(vec3{-1, -1, -1});
        lines.push_back(TextFormat("Damage: %d-%d", dmg.x, dmg.y));

        const int dura = g.ct.get<durability>(selection_id).value_or(-1);
        const int max_dura = g.ct.get<max_durability>(selection_id).value_or(-1);
        lines.push_back(TextFormat("Durability: %d/%d", dura, max_dura));
    }
    else if (item_type == ITEM_SHIELD) {
        const int block = g.ct.get<block_chance>(selection_id).value_or(-1);
        lines.push_back(TextFormat("Block chance: %d", block));

        const int dura = g.ct.get<durability>(selection_id).value_or(-1);
        const int max_dura = g.ct.get<max_durability>(selection_id).value_or(-1);
        lines.push_back(TextFormat("Durability: %d/%d", dura, max_dura));
    }
    else if (item_type == ITEM_POTION) {
        const vec3 heal = g.ct.get<healing>(selection_id).value_or(vec3{-1, -1, -1});
        lines.push_back(TextFormat("Heal amount: %d-%d", heal.x, heal.y));
    }

    lines.push_back(g.ct.get<description>(selection_id).value_or("no-description"));
    return lines;
}
