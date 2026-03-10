#pragma once

inline void gamestate::create_and_add_df_0(biome_t type, int w, int h, int df_count, float parts) {
    auto df = d.create_floor(type, w, h);
    float dw = df->get_width();
    float dh = df->get_height();
    vector<room> rooms;
    room r(0, TextFormat("room-%d", 0), TextFormat("room-%d description", 0), Rectangle{0, 0, dw, dh});
    df->set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, r.get_area());
    d.add_floor(df);
}

inline void gamestate::create_and_add_df_1(biome_t type, int w, int h, int df_count, float parts) {
    float min_room_w = 2;
    auto df = d.create_floor(type, w, h);
    float dw = df->get_width();
    float dh = df->get_height();
    vector<room> rooms;
    vector<room> doorways;
    minfo2("creating rooms");
    for (int i = 0; i < parts; i++) {
        for (int j = 0; j < parts; j++) {
            Rectangle q = {(dw / parts) * j, (dh / parts) * i, dw / parts, dh / parts};
            float max_room_w = q.width / 2.0;
            uniform_int_distribution<int> pgx(q.x, q.x + q.width / 2.0 - 1);
            uniform_int_distribution<int> pgy(q.y, q.y + q.height / 2.0 - 1);
            uniform_int_distribution<int> sg(min_room_w, max_room_w);
            Rectangle ra1 = {static_cast<float>(pgx(mt)), static_cast<float>(pgy(mt)), static_cast<float>(sg(mt)), static_cast<float>(sg(mt))};
            room_id rid = i * parts + j;
            room r(rid, TextFormat("room-%d", rid), TextFormat("room-%d description", rid), ra1);
            rooms.push_back(r);
        }
    }
    minfo2("setting rooms");
    for (size_t i = 0; i < rooms.size(); i++) {
        df->set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, rooms[i].get_area());
    }
    minfo2("sorting rooms");
    std::sort(rooms.begin(), rooms.end(), [](room& a, room& b) { return a.get_x() < b.get_x() && a.get_y() < b.get_y(); });
    for (size_t y = 0; y < parts; y++) {
        for (size_t x = 0; x < parts; x++) {
            size_t index = y * parts + x;
            room& r = rooms[index];
            int rx = r.get_x() + r.get_w() / 2;
            int ry = r.get_y() + r.get_h() / 2;
            float rix = rx;
            float riy = ry;
            if (x < parts - 1) {
                room& r2 = rooms[index + 1];
                int rx_e = r.get_x() + r.get_w();
                float rxf_e = r.get_x() + r.get_w();
                int rjx_e = r2.get_x();
                float rw = rjx_e - rx_e;
                Rectangle c0 = {rxf_e, riy, rw, 1};
                df->set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, c0);
                tile_t& first_tile = df->tile_at(vec3{rx_e, ry, -1});
                first_tile.set_can_have_door(true);
                int rw_i = rw;
                tile_t& last_tile = df->tile_at(vec3{rx_e + rw_i - 1, ry, -1});
                last_tile.set_can_have_door(true);
            }
            if (y < parts - 1) {
                size_t index2 = (y + 1) * parts + x;
                room& r2 = rooms[index2];
                int ry_e = r.get_y() + r.get_h();
                float ryf_e = r.get_y() + r.get_h();
                int rjy_e = r2.get_y();
                float rh = rjy_e - ry_e;
                Rectangle c0 = {rix, ryf_e, 1, rh};
                df->set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, c0);
                tile_t& first_tile = df->tile_at(vec3{rx, ry_e, -1});
                first_tile.set_can_have_door(true);
            }
        }
    }
    for (int y = 1; y < dh - 1; y++) {
        for (int x = 1; x < dw - 1; x++) {
            vec3 loc = {x, y, -1};
            if (df->tile_is_good_for_upgrade(loc)) {
                df->set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_10, Rectangle{static_cast<float>(loc.x), static_cast<float>(loc.y), 1, 1});
            }
        }
    }

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
        create_and_add_df_0(type, width, height, df_count, parts);
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
