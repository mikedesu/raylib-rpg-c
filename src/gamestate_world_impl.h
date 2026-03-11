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

inline bool dungeon_room_fits(Rectangle candidate, const vector<room>& rooms, int width, int height, int overlap_padding = 1) {
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
        if (dungeon_rects_overlap(candidate, existing.get_area(), overlap_padding)) {
            return false;
        }
    }
    return true;
}

inline bool dungeon_prop_is_solid(proptype_t type) {
    switch (type) {
    case PROP_DUNGEON_STATUE_00:
    case PROP_DUNGEON_WOODEN_TABLE_00:
    case PROP_DUNGEON_WOODEN_TABLE_01:
    case PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_EMPTY:
    case PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_WATER:
        return true;
    default:
        return false;
    }
}

inline bool dungeon_prop_is_pushable(proptype_t type) {
    switch (type) {
    case PROP_DUNGEON_STATUE_00:
        return true;
    default:
        return false;
    }
}

inline const char* dungeon_prop_name(proptype_t type) {
    switch (type) {
    case PROP_DUNGEON_STATUE_00: return "statue";
    case PROP_DUNGEON_TORCH_00: return "torch";
    case PROP_DUNGEON_CANDLE_00: return "candle";
    case PROP_DUNGEON_JAR_00: return "jar";
    case PROP_DUNGEON_PLATE_00: return "plate";
    case PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_EMPTY: return "empty barrel";
    case PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_WATER: return "water barrel";
    case PROP_DUNGEON_WOODEN_CHAIR_00: return "wooden chair";
    case PROP_DUNGEON_WOODEN_TABLE_00: return "wooden table";
    case PROP_DUNGEON_WOODEN_TABLE_01: return "wooden table";
    case PROP_DUNGEON_BANNER_00:
    case PROP_DUNGEON_BANNER_01:
    case PROP_DUNGEON_BANNER_02:
        return "banner";
    default:
        return "prop";
    }
}

inline const char* dungeon_prop_description(proptype_t type) {
    switch (type) {
    case PROP_DUNGEON_STATUE_00: return "A heavy carved statue.";
    case PROP_DUNGEON_TORCH_00: return "A wall torch repurposed as floor clutter.";
    case PROP_DUNGEON_CANDLE_00: return "A small candle.";
    case PROP_DUNGEON_JAR_00: return "A ceramic jar.";
    case PROP_DUNGEON_PLATE_00: return "A dusty plate.";
    case PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_EMPTY: return "An empty wooden barrel.";
    case PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_WATER: return "A wooden barrel filled with water.";
    case PROP_DUNGEON_WOODEN_CHAIR_00: return "A wooden chair.";
    case PROP_DUNGEON_WOODEN_TABLE_00:
    case PROP_DUNGEON_WOODEN_TABLE_01:
        return "A sturdy wooden table.";
    default:
        return "A dungeon prop.";
    }
}

inline with_fun dungeon_prop_init(proptype_t type) {
    return [type](CT& ct, const entityid id) {
        ct.set<name>(id, dungeon_prop_name(type));
        ct.set<description>(id, dungeon_prop_description(type));
        ct.set<solid>(id, dungeon_prop_is_solid(type));
        ct.set<pushable>(id, dungeon_prop_is_pushable(type));
    };
}

inline proptype_t dungeon_random_floor_prop_type(mt19937& mt) {
    static constexpr proptype_t floor_prop_types[] = {
        PROP_DUNGEON_STATUE_00,
        PROP_DUNGEON_TORCH_00,
        PROP_DUNGEON_CANDLE_00,
        PROP_DUNGEON_JAR_00,
        PROP_DUNGEON_PLATE_00,
        PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_EMPTY,
        PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_WATER,
        PROP_DUNGEON_WOODEN_CHAIR_00,
        PROP_DUNGEON_WOODEN_TABLE_00,
        PROP_DUNGEON_WOODEN_TABLE_01,
    };
    uniform_int_distribution<int> dist(0, static_cast<int>(sizeof(floor_prop_types) / sizeof(floor_prop_types[0])) - 1);
    return floor_prop_types[dist(mt)];
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

    const bool compact_map = w <= 8 && h <= 8;
    const int min_room_w = compact_map ? 2 : 3;
    const int min_room_h = compact_map ? 2 : 3;
    const int max_room_w = compact_map ? 2 : dungeon_clamp_int(w / 10, 4, 6);
    const int max_room_h = compact_map ? 2 : dungeon_clamp_int(h / 10, 4, 6);
    const int start_w = dungeon_random_range(mt, min_room_w, max_room_w);
    const int start_h = dungeon_random_range(mt, min_room_h, max_room_h);
    Rectangle start_area = dungeon_make_rect(w / 2 - start_w / 2, h / 2 - start_h / 2, start_w, start_h);
    start_area = df->df_paint_floor_area(start_area);
    rooms.push_back(room(0, TextFormat("room-%d", 0), TextFormat("room-%d description", 0), start_area));

    const int dungeon_area = w * h;
    const int density_divisor = compact_map
        ? dungeon_clamp_int(static_cast<int>(16.0f / parts), 8, 16)
        : dungeon_clamp_int(static_cast<int>(48.0f / parts), 24, 64);
    const int target_room_count = compact_map
        ? dungeon_clamp_int(dungeon_area / density_divisor, 4, 5)
        : dungeon_clamp_int(dungeon_area / density_divisor, 6, 96);
    const int min_gap = compact_map ? 0 : 1;
    const int max_gap = compact_map ? 0 : dungeon_clamp_int((w + h) / 96, 1, 2);
    const int overlap_padding = compact_map ? 0 : 1;
    int attempts_remaining = target_room_count * 96;
    while (static_cast<int>(rooms.size()) < target_room_count && attempts_remaining-- > 0) {
        const int anchor_index = dungeon_random_range(mt, 0, static_cast<int>(rooms.size()) - 1);
        const room& anchor = rooms[anchor_index];
        const direction_t dir = dirs[dungeon_random_range(mt, 0, 3)];
        const int room_w = dungeon_random_range(mt, min_room_w, max_room_w);
        const int room_h = dungeon_random_range(mt, min_room_h, max_room_h);
        const int gap = dungeon_random_range(mt, min_gap, max_gap);
        Rectangle candidate = df->df_get_adjacent_area(anchor.get_area(), dir, gap, room_w, room_h);
        if (!dungeon_room_fits(candidate, rooms, w, h, overlap_padding)) {
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

inline bool gamestate::assign_random_stairs_to_floor(shared_ptr<dungeon_floor> df) {
    massert(df, "dungeon floor is null");
    auto upstairs_locs = df->df_get_possible_upstairs_locs();
    if (!upstairs_locs || upstairs_locs->empty()) {
        merror("no valid upstairs location exists for floor %d", df->get_floor());
        return false;
    }

    uniform_int_distribution<int> upstairs_dist(0, static_cast<int>(upstairs_locs->size()) - 1);
    const vec3 upstairs_loc = upstairs_locs->at(upstairs_dist(mt));
    if (!df->df_set_upstairs_loc(upstairs_loc)) {
        merror("failed to assign upstairs for floor %d", df->get_floor());
        return false;
    }

    auto downstairs_locs = df->df_get_possible_downstairs_locs();
    if (!downstairs_locs || downstairs_locs->empty()) {
        merror("no valid downstairs location exists for floor %d", df->get_floor());
        return false;
    }

    uniform_int_distribution<int> downstairs_dist(0, static_cast<int>(downstairs_locs->size()) - 1);
    const vec3 downstairs_loc = downstairs_locs->at(downstairs_dist(mt));
    if (vec3_equal(upstairs_loc, downstairs_loc)) {
        merror("upstairs and downstairs resolved to the same tile on floor %d", df->get_floor());
        return false;
    }
    if (!df->df_set_downstairs_loc(downstairs_loc)) {
        merror("failed to assign downstairs for floor %d", df->get_floor());
        return false;
    }
    return true;
}

inline bool gamestate::assign_random_stairs() {
    massert(d.floors.size() > 0, "dungeon has no floors");
    for (size_t i = 0; i < d.floors.size(); i++) {
        if (!assign_random_stairs_to_floor(d.get_floor(i))) {
            return false;
        }
    }
    return true;
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
    massert(assign_random_stairs(), "failed to assign dungeon stairs");
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
    tile_t& tile = df->tile_at(loc);
    if (!tile_is_walkable(tile.get_type())) {
        return ENTITYID_INVALID;
    }
    if (tile.entity_count() > 0) {
        return ENTITYID_INVALID;
    }
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
    int placed_props = 0;
    for (int z = 0; z < (int)d.floors.size(); z++) {
        shared_ptr<dungeon_floor> df = d.get_floor(z);
        vector<vec3> candidates;
        for (int x = 0; x < df->get_width(); x++) {
            for (int y = 0; y < df->get_height(); y++) {
                vec3 loc = {x, y, z};
                tile_t& t = df->tile_at(loc);
                if (!tile_is_walkable(t.get_type())) {
                    continue;
                }
                if (t.get_type() == TILE_UPSTAIRS || t.get_type() == TILE_DOWNSTAIRS) {
                    continue;
                }
                if (t.get_can_have_door()) {
                    continue;
                }
                if (t.entity_count() != 0) {
                    continue;
                }
                candidates.push_back(loc);
            }
        }

        if (candidates.empty()) {
            continue;
        }

        std::shuffle(candidates.begin(), candidates.end(), mt);
        const int target_prop_count = dungeon_clamp_int(static_cast<int>(candidates.size()) / 24, 1, 6);
        const int prop_count = std::min(target_prop_count, static_cast<int>(candidates.size()));
        for (int i = 0; i < prop_count; i++) {
            const vec3 loc = candidates[i];
            const proptype_t type = dungeon_random_floor_prop_type(mt);
            entityid id = create_prop_at_with(type, loc, dungeon_prop_init(type));
            if (id != ENTITYID_INVALID) {
                placed_props++;
            }
        }
    }
    return placed_props;
}
