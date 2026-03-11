#pragma once

namespace {
inline int dungeon_clamp_int(int value, int min_value, int max_value) {
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

inline int dungeon_random_range(mt19937& mt, int min_value, int max_value) {
    if (max_value <= min_value) {
        return min_value;
    }
    uniform_int_distribution<int> dist(min_value, max_value);
    return dist(mt);
}

inline Rectangle dungeon_make_rect(int x, int y, int w, int h) {
    return Rectangle{static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h)};
}

inline bool dungeon_rects_overlap(Rectangle a, Rectangle b, int padding = 0) {
    const int ax0 = static_cast<int>(a.x) - padding;
    const int ay0 = static_cast<int>(a.y) - padding;
    const int ax1 = static_cast<int>(a.x + a.width) + padding;
    const int ay1 = static_cast<int>(a.y + a.height) + padding;
    const int bx0 = static_cast<int>(b.x);
    const int by0 = static_cast<int>(b.y);
    const int bx1 = static_cast<int>(b.x + b.width);
    const int by1 = static_cast<int>(b.y + b.height);
    return ax0 < bx1 && ax1 > bx0 && ay0 < by1 && ay1 > by0;
}

inline bool dungeon_room_fits(Rectangle candidate, const vector<room>& rooms, int width, int height) {
    if (candidate.width <= 0 || candidate.height <= 0) {
        return false;
    }
    if (candidate.x < 1 || candidate.y < 1) {
        return false;
    }
    if (candidate.x + candidate.width > width - 1 || candidate.y + candidate.height > height - 1) {
        return false;
    }
    for (const room& existing : rooms) {
        if (dungeon_rects_overlap(candidate, existing.get_area(), 1)) {
            return false;
        }
    }
    return true;
}
}

inline void gamestate::create_and_add_df_0(biome_t type, int w, int h, int df_count, float parts) {
    (void)df_count;
    (void)parts;
    auto df = d.create_floor(type, w, h);
    Rectangle room_area = dungeon_make_rect(1, 1, w - 2, h - 2);
    df->df_paint_floor_area(room_area);
    df->df_upgrade_floor_tiles();
    df->df_refresh_door_candidates();
    d.add_floor(df);
}

inline void gamestate::create_and_add_df_1(biome_t type, int w, int h, int df_count, float parts) {
    (void)df_count;
    auto df = d.create_floor(type, w, h);
    vector<room> rooms;
    constexpr direction_t dirs[] = {DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN};

    const int min_room_w = 3;
    const int min_room_h = 3;
    const int max_room_w = dungeon_clamp_int(w / 10, 4, 6);
    const int max_room_h = dungeon_clamp_int(h / 10, 4, 6);
    const int start_w = dungeon_random_range(mt, min_room_w, max_room_w);
    const int start_h = dungeon_random_range(mt, min_room_h, max_room_h);
    Rectangle start_area = dungeon_make_rect(w / 2 - start_w / 2, h / 2 - start_h / 2, start_w, start_h);
    start_area = df->df_paint_floor_area(start_area);
    rooms.push_back(room(0, TextFormat("room-%d", 0), TextFormat("room-%d description", 0), start_area));

    const int dungeon_area = w * h;
    const int density_divisor = dungeon_clamp_int(static_cast<int>(48.0f / parts), 24, 64);
    const int target_room_count = dungeon_clamp_int(dungeon_area / density_divisor, 6, 96);
    const int max_gap = dungeon_clamp_int((w + h) / 96, 1, 2);
    int attempts_remaining = target_room_count * 96;
    while (static_cast<int>(rooms.size()) < target_room_count && attempts_remaining-- > 0) {
        const int anchor_index = dungeon_random_range(mt, 0, static_cast<int>(rooms.size()) - 1);
        const room& anchor = rooms[anchor_index];
        const direction_t dir = dirs[dungeon_random_range(mt, 0, 3)];
        const int room_w = dungeon_random_range(mt, min_room_w, max_room_w);
        const int room_h = dungeon_random_range(mt, min_room_h, max_room_h);
        const int gap = dungeon_random_range(mt, 1, max_gap);
        Rectangle candidate = df->df_get_adjacent_area(anchor.get_area(), dir, gap, room_w, room_h);
        if (!dungeon_room_fits(candidate, rooms, w, h)) {
            continue;
        }
        Rectangle painted = df->df_paint_adjacent_floor_area(anchor.get_area(), dir, gap, room_w, room_h, 1);
        if (painted.width <= 0 || painted.height <= 0) {
            continue;
        }
        room_id rid = static_cast<room_id>(rooms.size());
        rooms.push_back(room(rid, TextFormat("room-%d", rid), TextFormat("room-%d description", rid), painted));
    }

    df->df_upgrade_floor_tiles();
    df->df_refresh_door_candidates();

    d.add_floor(df);
}

inline void gamestate::init_dungeon(biome_t type, int df_count, float parts, int width, int height) {
    minfo2("init_dungeon");
    massert(df_count > 0, "df_count is <= 0");
    massert(df_count > 0, "df_count == 0");
    massert(type > BIOME_NONE, "biome is invalid");
    massert(type < BIOME_COUNT, "biome is invalid 2");
    massert(width > 0, "width must be greater than zero");
    massert(height > 0, "height must be greater than zero");
    if (d.is_initialized) {
        merror("dungeon is already initialized");
        return;
    }
    for (int i = 0; i < df_count; i++) {
        create_and_add_df_1(type, width, height, df_count, parts);
    }
    minfo2("END floor creation loop");
    d.is_initialized = true;
}

inline entityid gamestate::create_door_with(with_fun doorInitFunction) {
    entityid id = add_entity();
    ct.set<entitytype>(id, ENTITY_DOOR);
    doorInitFunction(ct, id);
    return id;
}

inline entityid gamestate::create_door_at_with(vec3 loc, with_fun doorInitFunction) {
    shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
    tile_t& tile = df->tile_at(loc);
    if (!tile_is_walkable(tile.get_type())) {
        return INVALID;
    }
    entityid id = create_door_with(doorInitFunction);
    if (id == INVALID) {
        return INVALID;
    }
    if (!df->df_add_at(id, ENTITY_DOOR, loc)) {
        return INVALID;
    }
    ct.set<location>(id, loc);
    ct.set<door_open>(id, false);
    ct.set<update>(id, true);
    return id;
}

inline size_t gamestate::place_doors() {
    minfo2("gamestate.place_doors");
    size_t placed_doors = 0;
    for (size_t z = 0; z < d.floors.size(); z++) {
        shared_ptr<dungeon_floor> df = d.get_floor(z);
        for (int x = 0; x < df->get_width(); x++) {
            for (int y = 0; y < df->get_height(); y++) {
                vec3 loc = {x, y, static_cast<int>(z)};
                tile_t& tile = df->tile_at(loc);
                if (!tile.get_can_have_door()) {
                    continue;
                }
                entityid door_id = create_door_at_with(loc, [](CT& ct, const entityid id) {});
                if (door_id == ENTITYID_INVALID) {
                    continue;
                }
                placed_doors++;
            }
        }
    }
    msuccess("placed %ld doors", placed_doors);
    return placed_doors;
}

inline entityid gamestate::create_prop_with(proptype_t type, with_fun initFun) {
    entityid id = add_entity();
    ct.set<entitytype>(id, ENTITY_PROP);
    ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
    ct.set<update>(id, true);
    ct.set<proptype>(id, type);
    initFun(ct, id);
    return id;
}

inline entityid gamestate::create_prop_at_with(proptype_t type, vec3 loc, with_fun initFun) {
    shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
    entityid id = create_prop_with(type, initFun);
    if (id == ENTITYID_INVALID) {
        return ENTITYID_INVALID;
    }
    entityid result = df->df_add_at(id, ENTITY_PROP, loc);
    if (result == ENTITYID_INVALID) {
        return ENTITYID_INVALID;
    }
    ct.set<location>(id, loc);
    return id;
}

inline int gamestate::place_props() {
    auto defaultInit = [](CT& ct, const entityid id) {};
    int placed_props = 0;
    for (int z = 0; z < (int)d.floors.size(); z++) {
        shared_ptr<dungeon_floor> df = d.get_floor(z);
        for (int x = 0; x < df->get_width(); x++) {
            for (int y = 0; y < df->get_height(); y++) {
                vec3 loc = {x, y, z};
                tile_t& t = df->tile_at(loc);
                if (t.get_type() == TILE_UPSTAIRS || t.get_type() == TILE_DOWNSTAIRS) {
                    continue;
                }
                if (t.get_can_have_door()) {
                    continue;
                }
                if (t.tile_is_wall()) {
                    uniform_int_distribution<int> gen(0, 20);
                    int flip = gen(mt);
                    if (flip == 0) {
                        uniform_int_distribution<int> gen2(1, 3);
                        int r = gen2(mt);
                        switch (r) {
                        case 1: {
                            entityid id = create_prop_at_with(PROP_DUNGEON_BANNER_00, loc, defaultInit);
                            if (id != ENTITYID_INVALID) {
                                placed_props++;
                            }
                        } break;
                        case 2: {
                            entityid id = create_prop_at_with(PROP_DUNGEON_BANNER_01, loc, defaultInit);
                            if (id != ENTITYID_INVALID) {
                                placed_props++;
                            }
                        } break;
                        case 3: {
                            entityid id = create_prop_at_with(PROP_DUNGEON_BANNER_02, loc, defaultInit);
                            if (id != ENTITYID_INVALID) {
                                placed_props++;
                            }
                        } break;
                        default: break;
                        }
                    }
                }
                else {
                    uniform_int_distribution<int> gen(0, 20);
                    int flip = gen(mt);
                    if (flip == 0) {
                        entityid id = create_prop_at_with(PROP_DUNGEON_WOODEN_TABLE_00, loc, defaultInit);
                        ct.set<solid>(id, true);
                        if (id != INVALID) {
                            placed_props++;
                        }
                    }
                    else if (flip == 1) {
                        entityid id = create_prop_at_with(PROP_DUNGEON_WOODEN_TABLE_01, loc, defaultInit);
                        if (id != INVALID) {
                            placed_props++;
                            ct.set<solid>(id, true);
                        }
                    }
                    else if (flip == 2) {
                        entityid id = create_prop_at_with(PROP_DUNGEON_WOODEN_CHAIR_00, loc, defaultInit);
                        if (id != INVALID) {
                            placed_props++;
                        }
                    }
                    else if (flip == 3) {
                        entityid id = create_prop_at_with(PROP_DUNGEON_STATUE_00, loc, defaultInit);
                        if (id != INVALID) {
                            placed_props++;
                            ct.set<solid>(id, true);
                            ct.set<pushable>(id, true);
                        }
                    }
                }
            }
        }
    }
    return placed_props;
}
