#pragma once

inline int gamestate::compute_armor_class(entityid id) {
    massert(ENTITYID_INVALID != id, "id is invalid");
    int base_armor_class = ct.get<base_ac>(id).value_or(10);
    int dex_bonus = get_stat_bonus(ct.get<dexterity>(id).value_or(10));
    int total_ac = base_armor_class + dex_bonus;
    return total_ac;
}

inline bool gamestate::compute_attack_roll(entityid attacker, entityid target) {
    int str = ct.get<strength>(attacker).value_or(10);
    int bonus = get_stat_bonus(str);
    uniform_int_distribution<int> gen(1, 20);
    int roll = gen(mt) + bonus;
    int ac = compute_armor_class(target);
    return roll >= ac;
}

inline int gamestate::compute_attack_damage(entityid attacker, entityid target) {
    int str = ct.get<strength>(attacker).value_or(10);
    int bonus = std::max(0, get_stat_bonus(str));
    entityid equipped_wpn = ct.get<equipped_weapon>(attacker).value_or(ENTITYID_INVALID);
    vec3 dmg_range = ct.get<damage>(equipped_wpn).value_or(MINIMUM_DAMAGE);
    uniform_int_distribution<int> gen(dmg_range.x, dmg_range.y);
    int dmg = std::max(1, gen(mt));
    return dmg + bonus;
}

inline void gamestate::handle_weapon_durability_loss(entityid atk_id, entityid tgt_id) {
    entityid equipped_wpn = ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);
    optional<int> maybe_dura = ct.get<durability>(equipped_wpn);
    if (!maybe_dura.has_value()) {
        return;
    }
    int dura = maybe_dura.value();
    ct.set<durability>(equipped_wpn, dura - 1 < 0 ? 0 : dura - 1);
    if (dura > 0) {
        return;
    }
    ct.set<equipped_weapon>(atk_id, ENTITYID_INVALID);
    remove_from_inventory(atk_id, equipped_wpn);
    ct.set<destroyed>(equipped_wpn, true);
    bool event_heard = check_hearing(hero_id, ct.get<location>(tgt_id).value_or((vec3){-1, -1, -1}));
    if (event_heard) {
        PlaySound(sfx[SFX_05_ALCHEMY_GLASS_BREAK]);
    }
    add_message_history("%s broke!", ct.get<name>(equipped_wpn).value_or("no-name-weapon").c_str());
}

inline void gamestate::handle_shield_durability_loss(entityid defender, entityid attacker) {
    entityid shield = ct.get<equipped_shield>(defender).value_or(ENTITYID_INVALID);
    optional<int> maybe_dura = ct.get<durability>(shield);
    if (!maybe_dura.has_value()) {
        return;
    }
    int dura = maybe_dura.value();
    ct.set<durability>(shield, dura - 1 < 0 ? 0 : dura - 1);
    if (dura > 0) {
        return;
    }
    ct.set<equipped_shield>(defender, ENTITYID_INVALID);
    remove_from_inventory(defender, shield);
    ct.set<destroyed>(shield, true);
    bool event_heard = check_hearing(hero_id, ct.get<location>(defender).value_or((vec3){-1, -1, -1}));
    if (event_heard) {
        PlaySound(sfx[SFX_05_ALCHEMY_GLASS_BREAK]);
    }
    add_message_history("%s broke!", ct.get<name>(shield).value_or("no-name-shield").c_str());
}

inline int gamestate::get_npc_xp(entityid id) {
    return ct.get<xp>(id).value_or(0);
}

inline void gamestate::update_npc_xp(entityid id, entityid target_id) {
    int old_xp = get_npc_xp(id);
    int reward_xp = 1;
    int new_xp = old_xp + reward_xp;
    ct.set<xp>(id, new_xp);
}

inline void gamestate::provoke_npc(entityid npc_id, entityid source_id) {
    if (npc_id == ENTITYID_INVALID) {
        return;
    }
    if (ct.get<entitytype>(npc_id).value_or(ENTITY_NONE) != ENTITY_NPC) {
        return;
    }
    if (ct.get<dead>(npc_id).value_or(true)) {
        return;
    }

    ct.set<aggro>(npc_id, true);
    ct.set<update>(npc_id, true);

    const entityid target = source_id != ENTITYID_INVALID ? source_id : hero_id;
    if (target != ENTITYID_INVALID) {
        ct.set<target_id>(npc_id, target);
    }

    update_npc_behavior(npc_id);
}

inline void gamestate::process_attack_results(tile_t& tile, entityid atk_id, entityid tgt_id, bool atk_successful) {
    massert(atk_id != ENTITYID_INVALID, "attacker entity id is invalid");
    massert(tgt_id != ENTITYID_INVALID, "target entity id is invalid");

    string attacker_name = ct.get<name>(atk_id).value_or("no-name");
    string target_name = ct.get<name>(tgt_id).value_or("no-name");
    const char* atk_name = attacker_name.c_str();
    const char* tgt_name = target_name.c_str();

    if (ct.get<dead>(tgt_id).value_or(false)) {
        add_message_history("%s swings at a dead target", atk_name);
        return;
    }
    if (!atk_successful) {
        add_message_history("%s swings at %s and misses!", atk_name, tgt_name);
        return;
    }

    int dmg = compute_attack_damage(atk_id, tgt_id);
    ct.set<damaged>(tgt_id, true);
    ct.set<update>(tgt_id, true);

    optional<vec2> maybe_tgt_hp = ct.get<hp>(tgt_id);
    if (!maybe_tgt_hp.has_value()) {
        merror("target has no HP component");
        return;
    }

    vec2 tgt_hp = maybe_tgt_hp.value();
    tgt_hp.x -= dmg;
    add_message_history("%s deals %d damage to %s", atk_name, dmg, tgt_name);
    ct.set<hp>(tgt_id, tgt_hp);
    handle_weapon_durability_loss(atk_id, tgt_id);
    if (tgt_hp.x > 0) {
        return;
    }

    ct.set<dead>(tgt_id, true);
    ct.set<pullable>(tgt_id, true);
    tile.tile_remove(tgt_id);
    tile.add_dead_npc(tgt_id);

    switch (ct.get<entitytype>(tgt_id).value_or(ENTITY_NONE)) {
    case ENTITY_NPC:
        update_npc_xp(atk_id, tgt_id);
        drop_all_from_inventory(tgt_id);
        break;
    case ENTITY_PLAYER:
        add_message("You died");
        break;
    default:
        break;
    }
}

inline void gamestate::handle_shield_block_sfx(entityid target_id) {
    bool event_heard = check_hearing(hero_id, ct.get<location>(target_id).value_or((vec3){-1, -1, -1}));
    if (event_heard) {
        PlaySound(sfx[SFX_HIT_METAL_ON_METAL]);
    }
}

inline attack_result_t gamestate::process_attack_entity(tile_t& tile, entityid attacker_id, entityid target_id) {
    massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
    if (target_id == INVALID) {
        return ATTACK_RESULT_MISS;
    }

    entitytype_t type = ct.get<entitytype>(target_id).value_or(ENTITY_NONE);
    if (type != ENTITY_PLAYER && type != ENTITY_NPC) {
        return ATTACK_RESULT_MISS;
    }
    if (ct.get<dead>(target_id).value_or(true)) {
        return ATTACK_RESULT_MISS;
    }
    if (type == ENTITY_NPC) {
        provoke_npc(target_id, attacker_id);
    }

    bool attack_successful = compute_attack_roll(attacker_id, target_id);
    if (!attack_successful) {
        process_attack_results(tile, attacker_id, target_id, false);
        return ATTACK_RESULT_MISS;
    }

    entityid shield_id = ct.get<equipped_shield>(target_id).value_or(ENTITYID_INVALID);
    if (shield_id == ENTITYID_INVALID) {
        process_attack_results(tile, attacker_id, target_id, true);
        return ATTACK_RESULT_HIT;
    }

    uniform_int_distribution<int> gen(1, MAX_BLOCK_CHANCE);
    int roll = gen(mt);
    int chance = ct.get<block_chance>(shield_id).value_or(MAX_BLOCK_CHANCE);
    int low_roll = MAX_BLOCK_CHANCE - chance;
    if (roll <= low_roll) {
        process_attack_results(tile, attacker_id, target_id, true);
        return ATTACK_RESULT_HIT;
    }

    handle_shield_durability_loss(target_id, attacker_id);
    handle_shield_block_sfx(target_id);
    ct.set<block_success>(target_id, true);
    ct.set<update>(target_id, true);
    string atk_name = ct.get<name>(attacker_id).value_or("no-name");
    string tgt_name = ct.get<name>(target_id).value_or("no-name");
    add_message_history("%s blocked an attack from %s", tgt_name.c_str(), atk_name.c_str());
    return ATTACK_RESULT_BLOCK;
}

inline void gamestate::handle_attack_sfx(entityid attacker, attack_result_t result) {
    if (test) {
        return;
    }
    vec3 loc = ct.get<location>(attacker).value_or(vec3{-1, -1, -1});
    massert(vec3_valid(loc), "loc is invalid");
    if (!check_hearing(hero_id, loc)) {
        return;
    }
    int index = SFX_SLASH_ATTACK_SWORD_1;
    if (result == ATTACK_RESULT_BLOCK) {
        index = SFX_HIT_METAL_ON_METAL;
    }
    else if (result == ATTACK_RESULT_HIT) {
        index = SFX_HIT_METAL_ON_FLESH;
    }
    else if (result == ATTACK_RESULT_MISS) {
        entityid weapon_id = ct.get<equipped_weapon>(attacker).value_or(ENTITYID_INVALID);
        weapontype_t wpn_type = ct.get<weapontype>(weapon_id).value_or(WEAPON_NONE);
        index = wpn_type == WEAPON_SHORT_SWORD ? SFX_SLASH_ATTACK_SWORD_1
                : wpn_type == WEAPON_AXE       ? SFX_SLASH_ATTACK_HEAVY_1
                : wpn_type == WEAPON_DAGGER    ? SFX_SLASH_ATTACK_LIGHT_1
                                               : SFX_SLASH_ATTACK_SWORD_1;
    }
    PlaySound(sfx[index]);
    msuccess("attack sfx played");
}

inline void gamestate::set_gamestate_flag_for_attack_animation(entitytype_t type) {
    massert(type == ENTITY_PLAYER || type == ENTITY_NPC, "type is not player or npc!");
    if (type == ENTITY_PLAYER) {
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
    }
    else if (type == ENTITY_NPC) {
        flag = GAMESTATE_FLAG_NPC_ANIM;
    }
}

inline attack_result_t gamestate::try_entity_attack(entityid id, int x, int y) {
    minfo2("entity %d is attacking location %d, %d", id, x, y);
    massert(!ct.get<dead>(id).value_or(false), "attacker entity is dead");
    massert(ct.has<location>(id), "entity %d has no location", id);
    vec3 loc = ct.get<location>(id).value();
    shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
    tile_t& tile = df->tile_at(vec3{x, y, loc.z});
    int dx = x - loc.x;
    int dy = y - loc.y;
    ct.set<direction>(id, get_dir_from_xy(dx, dy));
    ct.set<attacking>(id, true);
    ct.set<update>(id, true);

    entityid npc_id = tile.get_cached_live_npc();
    attack_result_t result = process_attack_entity(tile, id, npc_id);
    handle_attack_sfx(id, result);
    if (!test) {
        set_gamestate_flag_for_attack_animation(ct.get<entitytype>(id).value_or(ENTITY_NONE));
    }
    return result;
}

inline bool gamestate::is_entity_adjacent(entityid id0, entityid id1) {
    massert(id0 != ENTITYID_INVALID, "id0 is invalid");
    massert(id1 != ENTITYID_INVALID, "id1 is invalid");
    massert(id0 != id1, "id0 and id1 are the same");
    vec3 a = ct.get<location>(id0).value_or((vec3){-1, -1, -1});
    vec3 b = ct.get<location>(id1).value_or((vec3){-1, -1, -1});
    if (a.z == -1 || b.z == -1 || a.z != b.z) {
        return false;
    }
    return (a.x - 1 == b.x && a.y - 1 == b.y) || (a.x == b.x && a.y - 1 == b.y) || (a.x + 1 == b.x && a.y - 1 == b.y) || (a.x - 1 == b.x && a.y == b.y) ||
           (a.x + 1 == b.x && a.y == b.y) || (a.x - 1 == b.x && a.y + 1 == b.y) || (a.x == b.x && a.y + 1 == b.y) || (a.x + 1 == b.x && a.y + 1 == b.y);
}

inline void gamestate::update_path_to_target(entityid id) {
    massert(id != INVALID, "id is invalid");
    if (!ct.has<target_path>(id)) {
        merror("id doesn't have a target_path");
        return;
    }

    auto path_to_target = ct.get<target_path>(id).value();
    path_to_target->clear();
    entityid target = ct.get<target_id>(id).value_or(ENTITYID_INVALID);
    if (target == ENTITYID_INVALID) {
        return;
    }

    optional<vec3> start_opt = ct.get<location>(id);
    optional<vec3> goal_opt = ct.get<location>(target);
    if (!start_opt.has_value() || !goal_opt.has_value()) {
        return;
    }

    vec3 start = start_opt.value();
    vec3 goal = goal_opt.value();
    if (start.z != goal.z) {
        return;
    }

    shared_ptr<dungeon_floor> df = d.floors[start.z];

    struct Node {
        vec3 pos;
        float gScore;
        float fScore;
        vec3 cameFrom;
        bool operator>(const Node& other) const {
            return fScore > other.fScore;
        }
    };

    auto heuristic = [](vec3 a, vec3 b) { return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)); };

    map<vec3, Node> openSet;
    map<vec3, Node> cameFrom;
    map<vec3, float> gScore;

    auto node_cmp = [](const Node& a, const Node& b) { return a > b; };
    priority_queue<Node, vector<Node>, decltype(node_cmp)> pq(node_cmp);

    Node startNode = {start, 0, heuristic(start, goal), {-1, -1, -1}};
    openSet[start] = startNode;
    pq.push(startNode);
    gScore[start] = 0;

    bool found = false;
    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();

        if (vec3_equal(current.pos, goal)) {
            found = true;
            break;
        }
        if (openSet.find(current.pos) == openSet.end()) {
            continue;
        }
        openSet.erase(current.pos);

        vec3 directions[8] = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0}};
        for (vec3 dir : directions) {
            vec3 neighbor = {current.pos.x + dir.x, current.pos.y + dir.y, current.pos.z};
            if (neighbor.x < 0 || neighbor.x >= df->get_width() || neighbor.y < 0 || neighbor.y >= df->get_height()) {
                continue;
            }
            tile_t& tile = df->tile_at(neighbor);
            if (!tile_is_walkable(tile.get_type()) || tile_has_pushable(neighbor.x, neighbor.y, neighbor.z) != ENTITYID_INVALID) {
                continue;
            }

            float tentative_gScore = gScore[current.pos] + 1;
            if (dir.x != 0 && dir.y != 0) {
                tentative_gScore = gScore[current.pos] + 1.414f;
            }

            if (gScore.find(neighbor) == gScore.end() || tentative_gScore < gScore[neighbor]) {
                Node neighborNode = {neighbor, tentative_gScore, tentative_gScore + heuristic(neighbor, goal), current.pos};
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentative_gScore;
                openSet[neighbor] = neighborNode;
                pq.push(neighborNode);
            }
        }
    }

    if (found) {
        vec3 current = goal;
        while (!vec3_equal(current, start) && cameFrom.find(current) != cameFrom.end()) {
            path_to_target->push_back(current);
            current = cameFrom[current].pos;
        }
        reverse(path_to_target->begin(), path_to_target->end());
    }
}

inline bool gamestate::try_entity_move_to_target(entityid id) {
    massert(ct.has<target_path>(id), "id has no target_path");
    update_path_to_target(id);
    auto path = ct.get<target_path>(id).value();
    if (path->size() > 0) {
        massert(ct.has<location>(id), "id has no location");
        vec3 id_location = ct.get<location>(id).value();
        vec3 next_location = path->at(0);
        vec3 diff_location = vec3_sub(next_location, id_location);
        if (try_entity_move(id, diff_location)) {
            msuccess2("try entity move succeeded");
            return true;
        }
    }
    return false;
}

inline bool gamestate::try_entity_move_random(entityid id) {
    uniform_int_distribution<int> dist(-1, 1);
    if (try_entity_move(id, vec3{dist(mt), dist(mt), 0})) {
        msuccess2("try entity move succeeded");
        return true;
    }
    return false;
}

inline bool gamestate::handle_npc(entityid id) {
    minfo2("handle npc %d", id);
    massert(id != ENTITYID_INVALID, "Entity is NULL!");
    if (ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_NPC) {
        return false;
    }
    auto id_name = ct.get<name>(id).value_or("no-name");
    auto maybe_dead = ct.get<dead>(id);
    massert(maybe_dead.has_value(), "npc id %d name %s has no dead component", id, id_name.c_str());
    if (!maybe_dead.has_value()) {
        return false;
    }
    bool is_dead = maybe_dead.value();
    if (is_dead) {
        return false;
    }

    entityid tgt_id = ct.get<target_id>(id).value_or(hero_id);
    entity_default_action_t d_action = ct.get<entity_default_action>(id).value_or(ENTITY_DEFAULT_ACTION_NONE);
    if (d_action == ENTITY_DEFAULT_ACTION_NONE) {
        return true;
    }
    else if (d_action == ENTITY_DEFAULT_ACTION_RANDOM_MOVE) {
        if (try_entity_move_random(id)) {
            return true;
        }
    }
    else if (d_action == ENTITY_DEFAULT_ACTION_MOVE_TO_TARGET) {
        if (try_entity_move_to_target(id)) {
            return true;
        }
    }
    else if (d_action == ENTITY_DEFAULT_ACTION_ATTACK_TARGET_IF_ADJACENT) {
        if (is_entity_adjacent(id, tgt_id)) {
            vec3 loc = ct.get<location>(tgt_id).value();
            try_entity_attack(id, loc.x, loc.y);
            return true;
        }
    }
    else if (d_action == ENTITY_DEFAULT_ACTION_RANDOM_MOVE_AND_ATTACK_TARGET_IF_ADJACENT) {
        if (is_entity_adjacent(id, tgt_id)) {
            vec3 loc = ct.get<location>(tgt_id).value();
            try_entity_attack(id, loc.x, loc.y);
            return true;
        }
        else if (try_entity_move_random(id)) {
            return true;
        }
    }
    else if (d_action == ENTITY_DEFAULT_ACTION_MOVE_TO_TARGET_AND_ATTACK_TARGET_IF_ADJACENT) {
        if (is_entity_adjacent(id, tgt_id)) {
            vec3 loc = ct.get<location>(tgt_id).value();
            try_entity_attack(id, loc.x, loc.y);
            return true;
        }
        else if (try_entity_move_to_target(id)) {
            return true;
        }
    }

    merror2("failed to handle npc %d", id);
    return false;
}

inline void gamestate::handle_npcs() {
    minfo2("handle npcs");
    if (flag == GAMESTATE_FLAG_NPC_TURN) {
#ifndef NPCS_ALL_AT_ONCE
        if (entity_turn >= 1 && entity_turn < next_entityid) {
            if (ct.get<entitytype>(entity_turn).value_or(ENTITY_NONE) == ENTITY_NPC) {
                handle_npc(entity_turn);
                flag = GAMESTATE_FLAG_NPC_ANIM;
            }
            else {
                flag = GAMESTATE_FLAG_NPC_ANIM;
            }
        }
#else
        auto df = d.get_current_floor();
        (void)df;
        for (entityid id = 0; id < next_entityid; id++) {
            auto type = ct.get<entitytype>(id).value_or(ENTITY_NONE);
            if (type == ENTITY_NPC) {
                const bool result = handle_npc(id);
                if (result) {
                    msuccess2("npc %d handled successfully", entity_turn);
                }
                else {
                    merror2("npc %d handle failed", entity_turn);
                }
            }
        }
        flag = GAMESTATE_FLAG_NPC_ANIM;
#endif
    }
}
