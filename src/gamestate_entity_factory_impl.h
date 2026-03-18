/** @file gamestate_entity_factory_impl.h
 *  @brief Entity/item/NPC factory helpers implemented on `gamestate`.
 */

#pragma once

inline entityid gamestate::create_weapon_with(with_fun weaponInitFunction) {
    entityid id = add_entity();
    ct.set<entitytype>(id, ENTITY_ITEM);
    ct.set<itemtype>(id, ITEM_WEAPON);
    ct.set<spritemove>(id, Rectangle{0, 0, 0, 0});
    ct.set<update>(id, true);
    weaponInitFunction(ct, id);
    return id;
}

inline with_fun gamestate::dagger_init() {
    return [](CT& ct, const entityid id) {
        ct.set<name>(id, "dagger");
        ct.set<description>(id, "Stabby stabby.");
        ct.set<weapontype>(id, WEAPON_DAGGER);
        ct.set<damage>(id, (vec3){1, 4, 0});
        ct.set<durability>(id, 100);
        ct.set<max_durability>(id, 100);
        ct.set<rarity>(id, RARITY_COMMON);
    };
}

inline with_fun gamestate::axe_init() {
    return [](CT& ct, const entityid id) {
        ct.set<name>(id, "axe");
        ct.set<description>(id, "Choppy choppy");
        ct.set<weapontype>(id, WEAPON_AXE);
        ct.set<damage>(id, (vec3){1, 8, 0});
        ct.set<durability>(id, 100);
        ct.set<max_durability>(id, 100);
        ct.set<rarity>(id, RARITY_COMMON);
    };
}

inline with_fun gamestate::sword_init() {
    return [](CT& ct, const entityid id) {
        ct.set<name>(id, "short sword");
        ct.set<description>(id, "your basic soldier's short sword");
        ct.set<weapontype>(id, WEAPON_SHORT_SWORD);
        ct.set<damage>(id, vec3{1, 6, 0});
        ct.set<durability>(id, 100);
        ct.set<max_durability>(id, 100);
        ct.set<rarity>(id, RARITY_COMMON);
    };
}

inline with_fun gamestate::shield_init() {
    return [](CT& ct, const entityid id) {
        ct.set<name>(id, "kite shield");
        ct.set<description>(id, "Standard knight's shield");
        ct.set<shieldtype>(id, SHIELD_KITE);
        ct.set<block_chance>(id, 90);
        ct.set<rarity>(id, RARITY_COMMON);
        ct.set<durability>(id, 100);
        ct.set<max_durability>(id, 100);
    };
}

inline with_fun gamestate::potion_init(potiontype_t pt) {
    return [pt](CT& ct, const entityid id) {
        ct.set<potiontype>(id, pt);
        if (pt == POTION_HP_SMALL) {
            ct.set<name>(id, "small healing potion");
            ct.set<description>(id, "a small healing potion");
            ct.set<healing>(id, (vec3){1, 6, 0});
        }
    };
}

inline with_fun gamestate::player_init(int maxhp_roll) {
    return [this, maxhp_roll](CT& ct, const entityid id) {
        ct.set<strength>(id, chara_creation.strength);
        ct.set<dexterity>(id, chara_creation.dexterity);
        ct.set<constitution>(id, chara_creation.constitution);
        ct.set<intelligence>(id, chara_creation.intelligence);
        ct.set<wisdom>(id, chara_creation.wisdom);
        ct.set<charisma>(id, chara_creation.charisma);
        ct.set<hd>(id, (vec3){1, chara_creation.hitdie, 0});
        ct.set<hp>(id, vec2{maxhp_roll, maxhp_roll});
        ct.set<alignment>(id, chara_creation.alignment);
    };
}

inline alignment_t gamestate::default_alignment_for_race(race_t rt) {
    switch (rt) {
    case RACE_HUMAN: return ALIGNMENT_NEUTRAL_NEUTRAL;
    case RACE_ELF: return ALIGNMENT_GOOD_CHAOTIC;
    case RACE_DWARF: return ALIGNMENT_GOOD_LAWFUL;
    case RACE_HALFLING: return ALIGNMENT_GOOD_NEUTRAL;
    case RACE_GOBLIN: return ALIGNMENT_EVIL_NEUTRAL;
    case RACE_ORC: return ALIGNMENT_EVIL_CHAOTIC;
    case RACE_BAT: return ALIGNMENT_NEUTRAL_NEUTRAL;
    case RACE_GREEN_SLIME: return ALIGNMENT_NEUTRAL_NEUTRAL;
    case RACE_WOLF: return ALIGNMENT_NEUTRAL_NEUTRAL;
    case RACE_WARG: return ALIGNMENT_EVIL_NEUTRAL;
    case RACE_RAT: return ALIGNMENT_NEUTRAL_NEUTRAL;
    case RACE_SKELETON: return ALIGNMENT_EVIL_LAWFUL;
    case RACE_ZOMBIE: return ALIGNMENT_EVIL_NEUTRAL;
    case RACE_NONE:
    case RACE_COUNT:
    default: break;
    }
    return ALIGNMENT_NONE;
}

inline bool gamestate::alignment_is_aggressive(alignment_t alignment_value) {
    switch (alignment_value) {
    case ALIGNMENT_EVIL_LAWFUL:
    case ALIGNMENT_EVIL_NEUTRAL:
    case ALIGNMENT_EVIL_CHAOTIC: return true;
    default: break;
    }
    return false;
}

inline with_fun gamestate::npc_alignment_init(alignment_t alignment_value) {
    return [alignment_value](CT& ct, const entityid id) { ct.set<alignment>(id, alignment_value); };
}

inline entityid gamestate::create_weapon_at_with(ComponentTable& ct, vec3 loc, with_fun weaponInitFunction) {
    minfo2("create weapon at with: %d %d %d", loc.x, loc.y, loc.z);
    if (d.floors.size() == 0) {
        merror2("dungeon floors size is 0");
        return ENTITYID_INVALID;
    }
    if (!d.is_initialized) {
        merror2("dungeon is_initialized flag not set");
        return ENTITYID_INVALID;
    }
    if (vec3_invalid(loc)) {
        merror2("loc is invalid");
        return ENTITYID_INVALID;
    }
    shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
    tile_t& tile = df->tile_at(loc);
    if (!tile_is_walkable(tile.get_type())) {
        merror2("cannot create entity on non-walkable tile");
        return ENTITYID_INVALID;
    }
    entityid id = create_weapon_with(weaponInitFunction);
    if (id == ENTITYID_INVALID) {
        minfo2("failed to create weapon");
        return ENTITYID_INVALID;
    }
    if (df->df_add_at(id, ENTITY_ITEM, loc) == ENTITYID_INVALID) {
        minfo2("failed to add weapon to df");
        return ENTITYID_INVALID;
    }
    ct.set<location>(id, loc);
    return id;
}

inline entityid gamestate::create_weapon_at_random_loc_with(CT& ct, with_fun weaponInitFunction) {
    vec3 loc = d.floors[d.current_floor]->get_random_loc();
    if (vec3_invalid(loc)) {
        merror("loc is invalid");
        return ENTITYID_INVALID;
    }
    return create_weapon_at_with(ct, loc, weaponInitFunction);
}

inline entityid gamestate::create_shield_with(ComponentTable& ct, with_fun shieldInitFunction) {
    entityid id = add_entity();
    ct.set<entitytype>(id, ENTITY_ITEM);
    ct.set<itemtype>(id, ITEM_SHIELD);
    ct.set<durability>(id, 100);
    ct.set<max_durability>(id, 100);
    ct.set<rarity>(id, RARITY_COMMON);
    ct.set<update>(id, false);
    shieldInitFunction(ct, id);
    return id;
}

inline entityid gamestate::create_shield_at_with(ComponentTable& ct, vec3 loc, with_fun shieldInitFunction) {
    if (d.floors.size() == 0) {
        return ENTITYID_INVALID;
    }
    entityid id = create_shield_with(ct, shieldInitFunction);
    shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
    if (!df->df_add_at(id, ENTITY_ITEM, loc)) {
        return ENTITYID_INVALID;
    }
    ct.set<location>(id, loc);
    return id;
}

inline entityid gamestate::create_potion_with(with_fun potionInitFunction) {
    entityid id = add_entity();
    ct.set<entitytype>(id, ENTITY_ITEM);
    ct.set<itemtype>(id, ITEM_POTION);
    ct.set<update>(id, true);
    potionInitFunction(ct, id);
    return id;
}

inline entityid gamestate::create_potion_at_with(vec3 loc, with_fun potionInitFunction) {
    shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
    tile_t& tile = df->tile_at(loc);
    if (!tile_is_walkable(tile.get_type())) {
        return ENTITYID_INVALID;
    }
    entityid id = create_potion_with(potionInitFunction);
    if (id == ENTITYID_INVALID) {
        return ENTITYID_INVALID;
    }
    if (!df->df_add_at(id, ENTITY_ITEM, loc)) {
        return ENTITYID_INVALID;
    }
    ct.set<location>(id, loc);
    ct.set<update>(id, true);
    return id;
}

inline race_t gamestate::random_monster_type() {
    vector<race_t> monster_races = {RACE_GOBLIN, RACE_ORC, RACE_BAT, RACE_WOLF, RACE_WARG, RACE_ZOMBIE, RACE_SKELETON, RACE_RAT, RACE_GREEN_SLIME};
    uniform_int_distribution<int> gen(0, monster_races.size() - 1);
    int random_index = gen(mt);
    return monster_races[random_index];
}

inline void gamestate::set_npc_starting_stats(entityid id) {
    race_t rt = ct.get<race>(id).value_or(RACE_NONE);
    if (rt == RACE_NONE) {
        return;
    }
    int str_m = get_racial_modifiers(rt, 0);
    int dex_m = get_racial_modifiers(rt, 1);
    int int_m = get_racial_modifiers(rt, 2);
    int wis_m = get_racial_modifiers(rt, 3);
    int con_m = get_racial_modifiers(rt, 4);
    int cha_m = get_racial_modifiers(rt, 5);
    uniform_int_distribution<int> gen(3, 18);
    int strength_ = gen(mt) + str_m;
    int dexterity_ = gen(mt) + dex_m;
    int intelligence_ = gen(mt) + int_m;
    int wisdom_ = gen(mt) + wis_m;
    int constitution_ = gen(mt) + con_m;
    int charisma_ = gen(mt) + cha_m;
    ct.set<strength>(id, strength_);
    ct.set<dexterity>(id, dexterity_);
    ct.set<intelligence>(id, intelligence_);
    ct.set<wisdom>(id, wisdom_);
    ct.set<constitution>(id, constitution_);
    ct.set<charisma>(id, charisma_);
    vec3 hitdie = {1, 8, 0};
    switch (rt) {
    case RACE_HUMAN: hitdie.y = 8; break;
    case RACE_ELF: hitdie.y = 6; break;
    case RACE_DWARF: hitdie.y = 10; break;
    case RACE_HALFLING: hitdie.y = 6; break;
    case RACE_GOBLIN: hitdie.y = 6; break;
    case RACE_ORC: hitdie.y = 8; break;
    case RACE_BAT: hitdie.y = 3; break;
    case RACE_GREEN_SLIME: hitdie.y = 4; break;
    case RACE_WOLF: hitdie.y = 6; break;
    case RACE_WARG: hitdie.y = 12; break;
    case RACE_RAT: hitdie.y = 4; break;
    case RACE_SKELETON: hitdie.y = 8; break;
    case RACE_ZOMBIE: hitdie.y = 8; break;
    default: break;
    }
    uniform_int_distribution<int> gen2(1, hitdie.y);
    int my_maxhp = std::max(1, gen2(mt) + get_stat_bonus(constitution_));
    int my_hp = my_maxhp;
    ct.set<hp>(id, vec2{my_hp, my_maxhp});
    ct.set<base_ac>(id, 10);
    ct.set<hd>(id, hitdie);
}

inline void gamestate::set_npc_defaults(entityid id) {
    ct.set<entitytype>(id, ENTITY_NPC);
    ct.set<spritemove>(id, Rectangle{0, 0, 0, 0});
    ct.set<dead>(id, false);
    ct.set<update>(id, true);
    ct.set<direction>(id, DIR_DOWN_RIGHT);
    ct.set<attacking>(id, false);
    ct.set<blocking>(id, false);
    ct.set<block_success>(id, false);
    ct.set<damaged>(id, false);
    ct.set<inventory>(id, make_shared<vector<entityid>>());
    ct.set<equipped_weapon>(id, ENTITYID_INVALID);
    ct.set<aggro>(id, false);
    ct.set<vision_distance>(id, 3);
    ct.set<hearing_distance>(id, 3);
    ct.set<level>(id, 1);
    ct.set<xp>(id, 0);
    ct.set<hunger_points>(id, hunger_points_t{100, 100});
    ct.set<entity_default_action>(id, ENTITY_DEFAULT_ACTION_NONE);
    ct.set<target_path>(id, make_shared<vector<vec3>>());
}

inline entityid gamestate::create_npc_with(race_t rt, with_fun npcInitFunction) {
    entityid id = add_entity();
    set_npc_defaults(id);
    ct.set<race>(id, rt);
    const alignment_t default_alignment = default_alignment_for_race(rt);
    npc_alignment_init(default_alignment)(ct, id);
    ct.set<aggro>(id, alignment_is_aggressive(default_alignment));
    set_npc_starting_stats(id);
    npcInitFunction(ct, id);
    if (!ct.get<name>(id).has_value()) {
        ct.set<name>(id, race2str(rt));
    }
    if (!ct.get<dialogue_text>(id).has_value()) {
        ct.set<dialogue_text>(id, "They give you a guarded look but say nothing.");
    }
    return id;
}

inline entityid gamestate::create_npc_at_with(race_t rt, vec3 loc, with_fun npcInitFunction) {
    minfo2("create npc at with: (%d, %d, %d)", loc.x, loc.y, loc.z);
    auto df = d.get_floor(loc.z);
    tile_t& tile = df->tile_at(loc);
    if (!tile_is_walkable(tile.get_type())) {
        merror2("cannot create entity on non-walkable tile: tile.type: %s", tiletype2str(tile.get_type()).c_str());
        return ENTITYID_INVALID;
    }
    if (tile_has_box(loc.x, loc.y, loc.z) != ENTITYID_INVALID) {
        merror2("cannot create entity on tile with box");
        return ENTITYID_INVALID;
    }
    if (tile_has_pushable(loc.x, loc.y, loc.z) != ENTITYID_INVALID) {
        merror2("cannot create entity on tile with pushable");
        return ENTITYID_INVALID;
    }
    if (tile.get_cached_live_npc() != ENTITYID_INVALID) {
        merror2("cannot create entity on tile with live npc");
        return ENTITYID_INVALID;
    }
    entityid id = create_npc_with(rt, npcInitFunction);
    if (id == ENTITYID_INVALID) {
        merror("failed to create npc");
        return ENTITYID_INVALID;
    }
    if (df->df_add_at(id, ENTITY_NPC, loc) == ENTITYID_INVALID) {
        merror("failed to add npc %d to %d, %d", id, loc.x, loc.y);
        return ENTITYID_INVALID;
    }
    minfo2("setting location for %d", id);
    ct.set<location>(id, loc);
    msuccess2("created npc %d", id);
    return id;
}

inline bool gamestate::add_to_inventory(entityid actor_id, entityid item_id) {
    minfo2("adding %d to %d's inventory", actor_id, item_id);
    auto maybe_inventory = ct.get<inventory>(actor_id);
    if (!maybe_inventory.has_value()) {
        merror2("%d has no inventory component", actor_id);
        return false;
    }
    auto inventory = maybe_inventory.value();
    inventory->push_back(item_id);
    msuccess2("added %d to %d's inventory", actor_id, item_id);
    return true;
}

inline entityid gamestate::create_orc_with(with_fun monsterInitFunction) {
    constexpr race_t r = RACE_ORC;
    entityid id = create_npc_with(r, monsterInitFunction);
    ct.set<name>(id, get_random_orc_name());
    return id;
}

inline entityid gamestate::create_orc_at_with(vec3 loc, with_fun monsterInitFunction) {
    if (vec3_invalid(loc)) {
        return ENTITYID_INVALID;
    }
    auto df = d.get_floor(loc.z);
    tile_t& t = df->tile_at(loc);
    if (!tile_is_walkable(t.get_type())) {
        return ENTITYID_INVALID;
    }
    if (t.get_cached_live_npc() != ENTITYID_INVALID) {
        return ENTITYID_INVALID;
    }
    entityid id = create_orc_with(monsterInitFunction);
    if (id == ENTITYID_INVALID) {
        return ENTITYID_INVALID;
    }
    if (!df->df_add_at(id, ENTITY_NPC, loc)) {
        return ENTITYID_INVALID;
    }
    ct.set<location>(id, loc);
    ct.set<update>(id, true);
    return id;
}

inline entityid gamestate::create_player_at_with(vec3 loc, string n, with_fun playerInitFunction) {
    minfo2("create player with: loc=(%d, %d, %d), n=%s", loc.x, loc.y, loc.z, n.c_str());
    massert(n != "", "name is empty string");
    race_t rt = chara_creation.race;
    entityid id = create_npc_at_with(rt, loc, playerInitFunction);
    if (id == ENTITYID_INVALID && !vec3_invalid(loc)) {
        auto spawn_floor = d.get_floor(loc.z);
        const vec3 fallback_loc = spawn_floor->get_random_loc();
        if (!vec3_invalid(fallback_loc)) {
            minfo2("retrying player creation at fallback loc=(%d, %d, %d)", fallback_loc.x, fallback_loc.y, fallback_loc.z);
            loc = fallback_loc;
            id = create_npc_at_with(rt, loc, playerInitFunction);
        }
    }
    massert(id != ENTITYID_INVALID, "id is invalid");
    constexpr int hp_ = 10;
    constexpr int maxhp_ = 10;
    constexpr int vis_dist = 20;
    constexpr int light_rad = 3;
    constexpr int hear_dist = 3;
    set_hero_id(id);
    ct.set<entitytype>(id, ENTITY_PLAYER);

    auto df = d.get_floor(loc.z);
    tile_t& tile = df->tile_at(loc);
    tile.set_cached_player_present(true);
    tile.set_cached_live_npc(id);

    ct.set<hp>(id, vec2{hp_, maxhp_});
    ct.set<vision_distance>(id, vis_dist);
    ct.set<light_radius>(id, light_rad);
    ct.set<hearing_distance>(id, hear_dist);
    ct.set<name>(id, n);
    ct.set<dead>(id, false);
    return id;
}

inline entityid gamestate::create_box_with() {
    entityid id = add_entity();
    ct.set<entitytype>(id, ENTITY_BOX);
    ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
    ct.set<update>(id, true);
    ct.set<pushable>(id, true);
    ct.set<pullable>(id, true);
    ct.set<name>(id, "box");
    ct.set<description>(id, "A plain wooden box with rough handles cut into the sides for hauling.");
    return id;
}

inline entityid gamestate::create_box_at_with(vec3 loc) {
    shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
    tile_t& tile = df->tile_at(loc);
    if (!tile_is_walkable(tile.get_type())) {
        merror("cannot create entity on non-walkable tile");
        return ENTITYID_INVALID;
    }
    if (tile_has_box(loc.x, loc.y, loc.z) != ENTITYID_INVALID) {
        merror("cannot create entity on tile with box");
        return ENTITYID_INVALID;
    }
    entityid id = create_box_with();
    if (df->df_add_at(id, ENTITY_BOX, loc) == ENTITYID_INVALID) {
        merror("failed df_add_at: %d, %d, %d", id, loc.x, loc.y);
        return ENTITYID_INVALID;
    }
    ct.set<location>(id, loc);
    return id;
}
