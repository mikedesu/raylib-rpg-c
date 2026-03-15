/** @file gamestate_world_interaction_impl.h
 *  @brief Movement, interaction, pickup, stairs, and world-physics helpers on `gamestate`.
 */

#pragma once

static inline string interaction_state_sentence(const char* noun, bool is_open) {
    return TextFormat("The %s stands %s.", noun, is_open ? "open" : "closed");
}

static inline string interaction_chest_text(const string& base_text, bool is_open) {
    return TextFormat("%s %s", base_text.c_str(), interaction_state_sentence("lid", is_open).c_str());
}

static inline string interaction_door_text(const string& base_text, bool is_open) {
    return TextFormat("%s %s", base_text.c_str(), interaction_state_sentence("door", is_open).c_str());
}

inline entityid gamestate::tile_has_pullable(int x, int y, int z) {
    minfo("tile_has_pullable(%d, %d, %d)", x, y, z);
    massert(z >= 0, "floor is out of bounds");
    massert((size_t)z < d.floors.size(), "floor is out of bounds");
    shared_ptr<dungeon_floor> df = d.get_floor(z);
    tile_t& t = df->tile_at(vec3{x, y, z});
    entityid box_id = t.get_cached_box();
    if (box_id != INVALID) {
        bool is_pullable = ct.get<pullable>(box_id).value_or(false);
        if (is_pullable) {
            return box_id;
        }
    }
    entityid chest_id = t.get_cached_chest();
    if (chest_id != INVALID) {
        bool is_pullable = ct.get<pullable>(chest_id).value_or(false);
        if (is_pullable) {
            return chest_id;
        }
    }
    entityid dead_npc_id = t.get_cached_dead_npc();
    if (dead_npc_id != INVALID) {
        bool is_pullable = ct.get<pullable>(dead_npc_id).value_or(false);
        if (is_pullable) {
            return dead_npc_id;
        }
    }
    entityid prop_id = t.get_cached_prop();
    if (prop_id != INVALID) {
        bool is_pullable = ct.get<pullable>(prop_id).value_or(false);
        if (is_pullable) {
            return prop_id;
        }
    }
    return ENTITYID_INVALID;
}

inline bool gamestate::tile_has_solid(int x, int y, int z) {
    massert(z >= 0, "floor is out of bounds");
    massert((size_t)z < d.floors.size(), "floor is out of bounds");
    shared_ptr<dungeon_floor> df = d.get_floor(z);
    tile_t& t = df->tile_at(vec3{x, y, z});

    entityid id = t.get_cached_live_npc();
    bool is_solid = ct.get<solid>(id).value_or(false);
    if (id != ENTITYID_INVALID && is_solid) {
        return true;
    }

    id = t.get_cached_box();
    is_solid = ct.get<solid>(id).value_or(false);
    if (id != ENTITYID_INVALID && is_solid) {
        return true;
    }

    id = t.get_cached_chest();
    is_solid = ct.get<solid>(id).value_or(false);
    if (id != ENTITYID_INVALID && is_solid) {
        return true;
    }

    id = t.get_cached_prop();
    is_solid = ct.get<solid>(id).value_or(false);
    if (id != ENTITYID_INVALID && is_solid) {
        return true;
    }

    id = t.get_cached_door();
    is_solid = ct.get<solid>(id).value_or(false);
    if (id != ENTITYID_INVALID && is_solid) {
        return true;
    }

    id = t.get_cached_item();
    is_solid = ct.get<solid>(id).value_or(false);
    if (id != ENTITYID_INVALID && is_solid) {
        return true;
    }

    return false;
}

inline bool gamestate::handle_box_push(entityid id, vec3 v) {
    bool can_push = ct.get<pushable>(id).value_or(false);
    if (!can_push) {
        return false;
    }
    return try_entity_move(id, v);
}

inline entityid gamestate::tile_has_pushable(int x, int y, int z) {
    massert(z >= 0, "floor is out of bounds");
    massert((size_t)z < d.floors.size(), "floor is out of bounds");
    shared_ptr<dungeon_floor> df = d.get_floor(z);
    tile_t& t = df->tile_at(vec3{x, y, z});

    entityid id = t.get_cached_box();
    bool is_pushable = ct.get<pushable>(id).value_or(false);
    if (id != ENTITYID_INVALID && is_pushable) {
        return id;
    }

    id = t.get_cached_chest();
    is_pushable = ct.get<pushable>(id).value_or(false);
    if (id != ENTITYID_INVALID && is_pushable) {
        return id;
    }

    id = t.get_cached_prop();
    is_pushable = ct.get<pushable>(id).value_or(false);
    if (id != ENTITYID_INVALID && is_pushable) {
        return id;
    }

    return ENTITYID_INVALID;
}

inline entityid gamestate::tile_has_door(vec3 v) {
    massert(v.z >= 0, "floor is out of bounds");
    massert(static_cast<size_t>(v.z) < d.floors.size(), "floor is out of bounds");
    shared_ptr<dungeon_floor> df = d.get_floor(static_cast<size_t>(v.z));
    tile_t& t = df->tile_at(v);
    return t.get_cached_door();
}

inline bool gamestate::check_hearing(entityid id, vec3 loc) {
    if (id == ENTITYID_INVALID || vec3_invalid(loc)) {
        return false;
    }
    vec3 hero_loc = ct.get<location>(hero_id).value_or(vec3{-1, -1, -1});
    if (vec3_invalid(hero_loc) || hero_loc.z != loc.z) {
        return false;
    }

    float x0 = hero_loc.x;
    float y0 = hero_loc.y;
    float x1 = loc.x;
    float y1 = loc.y;
    Vector2 p0 = {x0, y0};
    Vector2 p1 = {x1, y1};

    float dist = Vector2Distance(p0, p1);
    float hearing = ct.get<hearing_distance>(hero_id).value_or(3);
    return dist <= hearing;
}

inline bool gamestate::tile_has_pressure_plate_occupant(vec3 loc) {
    if (vec3_invalid(loc) || loc.z < 0 || static_cast<size_t>(loc.z) >= d.floors.size()) {
        return false;
    }

    tile_t& tile = d.get_floor(static_cast<size_t>(loc.z))->tile_at(loc);
    if (tile.get_cached_player_present()) {
        return true;
    }
    if (tile.get_cached_live_npc() != ENTITYID_INVALID || tile.get_dead_npc_count() > 0) {
        return true;
    }

    const entityid box_id = tile.get_cached_box();
    if (box_id != ENTITYID_INVALID && (ct.get<pushable>(box_id).value_or(false) || ct.get<pullable>(box_id).value_or(false))) {
        return true;
    }

    const entityid chest_id = tile.get_cached_chest();
    if (chest_id != ENTITYID_INVALID && (ct.get<pushable>(chest_id).value_or(false) || ct.get<pullable>(chest_id).value_or(false))) {
        return true;
    }

    const entityid prop_id = tile.get_cached_prop();
    if (prop_id != ENTITYID_INVALID && (ct.get<pushable>(prop_id).value_or(false) || ct.get<pullable>(prop_id).value_or(false))) {
        return true;
    }

    return false;
}

inline bool gamestate::door_is_pressure_plate_controlled(entityid door_id) const {
    if (door_id == ENTITYID_INVALID) {
        return false;
    }
    for (const floor_pressure_plate_t& plate : floor_pressure_plates) {
        if (!plate.destroyed && plate.linked_door_id == door_id) {
            return true;
        }
    }
    return false;
}

inline floor_pressure_plate_t* gamestate::get_floor_pressure_plate(vec3 loc) {
    for (floor_pressure_plate_t& plate : floor_pressure_plates) {
        if (vec3_equal(plate.loc, loc)) {
            return &plate;
        }
    }
    return nullptr;
}

inline void gamestate::update_pressure_plates_for_floor(int z) {
    vector<entityid> linked_doors;
    for (floor_pressure_plate_t& plate : floor_pressure_plates) {
        if (plate.loc.z != z || plate.destroyed) {
            continue;
        }

        const bool active = tile_has_pressure_plate_occupant(plate.loc);
        if (plate.linked_door_id == ENTITYID_INVALID || ct.get<entitytype>(plate.linked_door_id).value_or(ENTITY_NONE) != ENTITY_DOOR) {
            plate.linked_door_id = ENTITYID_INVALID;
            plate.active = false;
            continue;
        }

        if (std::find(linked_doors.begin(), linked_doors.end(), plate.linked_door_id) == linked_doors.end()) {
            linked_doors.push_back(plate.linked_door_id);
        }
        plate.active = active;
    }

    for (const entityid door_id : linked_doors) {
        bool should_open = false;
        for (const floor_pressure_plate_t& plate : floor_pressure_plates) {
            if (plate.destroyed || plate.linked_door_id != door_id) {
                continue;
            }
            if (plate.active) {
                should_open = true;
                break;
            }
        }

        const bool was_open = ct.get<door_open>(door_id).value_or(false);
        ct.set<door_open>(door_id, should_open);
        ct.set<update>(door_id, true);
        if (!was_open && should_open && IsAudioDeviceReady() && sfx.size() > SFX_CHEST_OPEN) {
            PlaySound(sfx.at(SFX_CHEST_OPEN));
        }
    }

    frame_dirty = true;
}

inline void gamestate::refresh_pressure_plates() {
    for (size_t z = 0; z < d.floors.size(); z++) {
        update_pressure_plates_for_floor(static_cast<int>(z));
    }
}

inline void gamestate::clear_gameplay_events() {
    gameplay_events.clear();
}

inline bool gamestate::queue_gameplay_event(const gameplay_event_t& event) {
    if (event.type == EVENT_NONE) {
        return false;
    }
    gameplay_events.push_back(event);
    return true;
}

inline bool gamestate::queue_move_event(entityid id, vec3 v) {
    gameplay_event_t event;
    event.type = EVENT_MOVE_INTENT;
    event.actor_id = id;
    event.delta = v;
    return queue_gameplay_event(event);
}

inline bool gamestate::queue_attack_event(entityid id, vec3 loc) {
    gameplay_event_t event;
    event.type = EVENT_ATTACK_INTENT;
    event.actor_id = id;
    event.target_loc = loc;
    return queue_gameplay_event(event);
}

inline bool gamestate::queue_pull_event(entityid id) {
    gameplay_event_t event;
    event.type = EVENT_PULL_INTENT;
    event.actor_id = id;
    return queue_gameplay_event(event);
}

inline bool gamestate::queue_open_door_event(entityid id, vec3 loc) {
    gameplay_event_t event;
    event.type = EVENT_OPEN_DOOR_INTENT;
    event.actor_id = id;
    event.target_loc = loc;
    return queue_gameplay_event(event);
}

inline bool gamestate::queue_open_chest_event(entityid id, vec3 loc) {
    gameplay_event_t event;
    event.type = EVENT_OPEN_CHEST_INTENT;
    event.actor_id = id;
    event.target_loc = loc;
    return queue_gameplay_event(event);
}

inline bool gamestate::queue_traverse_stairs_event(entityid id) {
    gameplay_event_t event;
    event.type = EVENT_TRAVERSE_STAIRS_INTENT;
    event.actor_id = id;
    return queue_gameplay_event(event);
}

inline bool gamestate::queue_pressure_plate_refresh_event(int z) {
    gameplay_event_t event;
    event.type = EVENT_REFRESH_PRESSURE_PLATES;
    event.floor = z;
    return queue_gameplay_event(event);
}

inline gameplay_event_result_t gamestate::process_gameplay_event(const gameplay_event_t& event) {
    gameplay_event_result_t result;
    result.type = event.type;
    result.actor_id = event.actor_id;
    switch (event.type) {
    case EVENT_MOVE_INTENT: {
        result.handled = true;
        result.succeeded = try_entity_move(event.actor_id, event.delta);
        if (result.succeeded) {
            const vec3 loc = ct.get<location>(event.actor_id).value_or(vec3{-1, -1, -1});
            if (vec3_valid(loc)) {
                queue_pressure_plate_refresh_event(loc.z);
            }
        }
        return result;
    }
    case EVENT_ATTACK_INTENT:
        result.handled = true;
        result.attack_result = try_entity_attack(event.actor_id, event.target_loc.x, event.target_loc.y);
        result.succeeded = result.attack_result != ATTACK_RESULT_NONE;
        return result;
    case EVENT_PULL_INTENT: {
        result.handled = true;
        result.succeeded = try_entity_pull(event.actor_id);
        if (result.succeeded) {
            const vec3 loc = ct.get<location>(event.actor_id).value_or(vec3{-1, -1, -1});
            if (vec3_valid(loc)) {
                queue_pressure_plate_refresh_event(loc.z);
            }
        }
        return result;
    }
    case EVENT_OPEN_DOOR_INTENT:
        result.handled = true;
        result.succeeded = try_entity_open_door(event.actor_id, event.target_loc);
        return result;
    case EVENT_OPEN_CHEST_INTENT:
        result.handled = true;
        result.succeeded = try_entity_open_chest(event.actor_id, event.target_loc);
        return result;
    case EVENT_TRAVERSE_STAIRS_INTENT:
        result.handled = true;
        result.succeeded = try_entity_stairs(event.actor_id);
        return result;
    case EVENT_REFRESH_PRESSURE_PLATES:
        result.handled = true;
        if (event.floor >= 0 && static_cast<size_t>(event.floor) < d.floors.size()) {
            update_pressure_plates_for_floor(event.floor);
            result.succeeded = true;
        }
        return result;
    case EVENT_NONE:
    case EVENT_COUNT:
    default:
        return result;
    }
}

inline gameplay_event_result_t gamestate::process_gameplay_events() {
    gameplay_event_result_t first_result;
    if (processing_actions) {
        return first_result;
    }

    processing_actions = true;
    for (size_t i = 0; i < gameplay_events.size(); i++) {
        const gameplay_event_result_t result = process_gameplay_event(gameplay_events[i]);
        if (first_result.type == EVENT_NONE && result.type != EVENT_NONE) {
            first_result = result;
        }
    }
    gameplay_events.clear();
    processing_actions = false;
    return first_result;
}

inline bool gamestate::run_move_action(entityid id, vec3 v) {
    clear_gameplay_events();
    if (!queue_move_event(id, v)) {
        return false;
    }
    return process_gameplay_events().succeeded;
}

inline attack_result_t gamestate::run_attack_action(entityid id, vec3 loc) {
    clear_gameplay_events();
    if (!queue_attack_event(id, loc)) {
        return ATTACK_RESULT_NONE;
    }
    return process_gameplay_events().attack_result;
}

inline bool gamestate::run_pull_action(entityid id) {
    clear_gameplay_events();
    if (!queue_pull_event(id)) {
        return false;
    }
    return process_gameplay_events().succeeded;
}

inline bool gamestate::run_open_door_action(entityid id, vec3 loc) {
    clear_gameplay_events();
    if (!queue_open_door_event(id, loc)) {
        return false;
    }
    return process_gameplay_events().succeeded;
}

inline bool gamestate::run_open_chest_action(entityid id, vec3 loc) {
    clear_gameplay_events();
    if (!queue_open_chest_event(id, loc)) {
        return false;
    }
    return process_gameplay_events().succeeded;
}

inline bool gamestate::run_traverse_stairs_action(entityid id) {
    clear_gameplay_events();
    if (!queue_traverse_stairs_event(id)) {
        return false;
    }
    return process_gameplay_events().succeeded;
}

inline bool gamestate::try_entity_move(entityid id, vec3 v) {
    massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
    minfo2("entity %d is trying to move: (%d,%d,%d)", id, v.x, v.y, v.z);
    ct.set<direction>(id, get_dir_from_xy(v.x, v.y));
    ct.set<update>(id, true);
    massert(ct.has<location>(id), "id %d has no location", id);

    vec3 loc = ct.get<location>(id).value_or((vec3){-1, -1, -1});
    massert(!vec3_invalid(loc), "id %d location invalid", id);
    vec3 aloc = {loc.x + v.x, loc.y + v.y, loc.z};

    minfo2("entity %d is trying to move to (%d,%d,%d)", id, aloc.x, aloc.y, aloc.z);

    shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
    if (aloc.x < 0 || aloc.x >= df->get_width() || aloc.y < 0 || aloc.y >= df->get_height()) {
        merror2("destination is invalid: (%d, %d, %d)", aloc.x, aloc.y, aloc.z);
        return false;
    }

    tile_t& tile = df->tile_at(aloc);
    if (!tile_is_walkable(tile.get_type())) {
        if (!(god_mode && id == hero_id)) {
            merror2("tile is not walkable");
            return false;
        }
    }
    entityid box_id = tile_has_box(aloc.x, aloc.y, aloc.z);
    if (box_id != ENTITYID_INVALID) {
        merror2("box present, trying to push");
        return handle_box_push(box_id, v);
    }
    entityid pushable_id = tile_has_pushable(aloc.x, aloc.y, aloc.z);
    if (pushable_id != ENTITYID_INVALID) {
        merror2("pushable present, trying to push");
        return handle_box_push(pushable_id, v);
    }
    bool has_solid = tile_has_solid(aloc.x, aloc.y, aloc.z);
    if (has_solid) {
        merror2("solid present, cannot move");
        return false;
    }
    else if (tile.get_cached_live_npc() != INVALID) {
        merror2("live npcs present, cannot move");
        return false;
    }

    entityid door_id = tile_has_door(aloc);
    if (door_id != ENTITYID_INVALID) {
        massert(ct.has<door_open>(door_id), "door_id %d doesnt have a door_open component", door_id);
        if (!ct.get<door_open>(door_id).value_or(false)) {
            merror2("door is closed");
            return false;
        }
    }

    if (!df->df_remove_at(id, loc)) {
        merror2("Failed to remove %d from (%d, %d)", id, loc.x, loc.y);
        return false;
    }

    auto type = ct.get<entitytype>(id).value_or(ENTITY_NONE);
    bool is_dead_npc = type == ENTITY_NPC && ct.get<dead>(id).value_or(false);
    if (is_dead_npc) {
        tile_t& dst_tile = df->tile_at(aloc);
        dst_tile.add_dead_npc(id);
    }
    else if (df->df_add_at(id, type, aloc) == ENTITYID_INVALID) {
        merror2("Failed to add %d to (%d, %d)", id, aloc.x, aloc.y);
        return false;
    }

    ct.set<location>(id, aloc);
    float mx = v.x * DEFAULT_TILE_SIZE;
    float my = v.y * DEFAULT_TILE_SIZE;
    ct.set<spritemove>(id, (Rectangle){mx, my, 0, 0});
    if (check_hearing(hero_id, aloc)) {
        if (IsAudioDeviceReady()) {
            PlaySound(sfx[SFX_STEP_STONE_1]);
        }
    }
    ct.set<steps_taken>(id, ct.get<steps_taken>(id).value_or(0) + 1);
    if (!processing_actions) {
        update_pressure_plates_for_floor(loc.z);
    }
    msuccess2("npc %d moved to (%d,%d,%d)", id, aloc.x, aloc.y, aloc.z);
    return true;
}

inline bool gamestate::handle_move_up(inputstate& is, bool is_dead) {
    if (is_action_pressed(is, INPUT_ACTION_MOVE_UP)) {
        if (is_dead) {
            return add_message("You cannot move while dead");
        }
        run_move_action(hero_id, (vec3){0, -1, 0});
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}

inline bool gamestate::handle_move_down(inputstate& is, bool is_dead) {
    if (is_action_pressed(is, INPUT_ACTION_MOVE_DOWN)) {
        if (is_dead) {
            return add_message("You cannot move while dead");
        }
        run_move_action(hero_id, (vec3){0, 1, 0});
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}

inline bool gamestate::handle_move_left(inputstate& is, bool is_dead) {
    if (is_action_pressed(is, INPUT_ACTION_MOVE_LEFT)) {
        if (is_dead) {
            return add_message("You cannot move while dead");
        }
        run_move_action(hero_id, (vec3){-1, 0, 0});
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}

inline bool gamestate::handle_move_right(inputstate& is, bool is_dead) {
    if (is_action_pressed(is, INPUT_ACTION_MOVE_RIGHT)) {
        if (is_dead) {
            return add_message("You cannot move while dead");
        }
        run_move_action(hero_id, (vec3){1, 0, 0});
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}

inline bool gamestate::handle_move_up_left(inputstate& is, bool is_dead) {
    if (is_action_pressed(is, INPUT_ACTION_MOVE_UP_LEFT)) {
        if (is_dead) {
            return add_message("You cannot move while dead");
        }
        run_move_action(hero_id, (vec3){-1, -1, 0});
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}

inline bool gamestate::handle_move_up_right(inputstate& is, bool is_dead) {
    if (is_action_pressed(is, INPUT_ACTION_MOVE_UP_RIGHT)) {
        if (is_dead) {
            add_message("You cannot move while dead");
            return true;
        }
        run_move_action(hero_id, (vec3){1, -1, 0});
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}

inline bool gamestate::handle_move_down_left(inputstate& is, bool is_dead) {
    if (is_action_pressed(is, INPUT_ACTION_MOVE_DOWN_LEFT)) {
        if (is_dead) {
            return add_message("You cannot move while dead");
        }
        run_move_action(hero_id, (vec3){-1, 1, 0});
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}

inline bool gamestate::handle_move_down_right(inputstate& is, bool is_dead) {
    if (is_action_pressed(is, INPUT_ACTION_MOVE_DOWN_RIGHT)) {
        if (is_dead) {
            return add_message("You cannot move while dead");
        }
        run_move_action(hero_id, (vec3){1, 1, 0});
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}

inline vec3 gamestate::get_loc_facing_player() {
    optional<vec3> maybe_loc = ct.get<location>(hero_id);
    if (maybe_loc.has_value()) {
        vec3 loc = ct.get<location>(hero_id).value();
        direction_t dir = ct.get<direction>(hero_id).value();
        if (dir == DIR_UP) {
            loc.y -= 1;
        }
        else if (dir == DIR_DOWN) {
            loc.y += 1;
        }
        else if (dir == DIR_LEFT) {
            loc.x -= 1;
        }
        else if (dir == DIR_RIGHT) {
            loc.x += 1;
        }
        else if (dir == DIR_UP_LEFT) {
            loc.x -= 1;
            loc.y -= 1;
        }
        else if (dir == DIR_UP_RIGHT) {
            loc.x += 1;
            loc.y -= 1;
        }
        else if (dir == DIR_DOWN_LEFT) {
            loc.x -= 1;
            loc.y += 1;
        }
        else if (dir == DIR_DOWN_RIGHT) {
            loc.x += 1;
            loc.y += 1;
        }
        return loc;
    }
    return vec3{-1, -1, -1};
}

inline entityid gamestate::tile_get_item(shared_ptr<tile_t> t) {
    return t->get_cached_item();
}

inline bool gamestate::try_entity_pull(entityid id) {
    minfo("try_entity_pull(%d)", id);
    massert(id != ENTITYID_INVALID, "Entity is NULL!");
    ct.set<update>(id, true);
    vec3 loc = ct.get<location>(id).value_or(vec3{-1, -1, -1});
    massert(!vec3_invalid(loc), "loc is invalid");
    auto df = d.get_floor(loc.z);
    direction_t facing_d = ct.get<direction>(id).value_or(DIR_NONE);
    massert(facing_d != DIR_NONE, "direction d is none");
    direction_t d = get_opposite_dir(facing_d);
    vec3 v = get_loc_from_dir(d);
    vec3 aloc = {loc.x + v.x, loc.y + v.y, loc.z};
    vec3 fv = get_loc_from_dir(facing_d);
    vec3 bloc = {loc.x + fv.x, loc.y + fv.y, loc.z};

    if (aloc.x < 0 || aloc.x >= df->get_width() || aloc.y < 0 || aloc.y >= df->get_height()) {
        merror("destination is invalid: (%d, %d, %d)", aloc.x, aloc.y, aloc.z);
        return false;
    }

    if (bloc.x < 0 || bloc.x >= df->get_width() || bloc.y < 0 || bloc.y >= df->get_height()) {
        merror("pull source is invalid: (%d, %d, %d)", bloc.x, bloc.y, bloc.z);
        return false;
    }

    tile_t& tile_dest = df->tile_at(aloc);

    if (!tile_is_walkable(tile_dest.get_type())) {
        merror("tile is not walkable");
        return false;
    }

    entityid box_id = tile_has_box(aloc.x, aloc.y, aloc.z);
    if (box_id != ENTITYID_INVALID) {
        merror("box present, can't push and pull simultaneously");
        return false;
    }

    bool has_solid = tile_has_solid(aloc.x, aloc.y, aloc.z);
    if (has_solid) {
        merror("solid present, cannot move");
        return false;
    }

    if (tile_dest.get_cached_live_npc() != INVALID) {
        merror("live npcs present, cannot move");
        return false;
    }

    //if (tile_at_cur_floor(aloc).get_cached_player_present()) {
    //    merror("player present, cannot move");
    //    return false;
    //}

    entityid door_id = tile_has_door(aloc);
    if (door_id != ENTITYID_INVALID) {
        massert(ct.has<door_open>(door_id), "door_id %d doesnt have a door_open component", door_id);
        if (!ct.get<door_open>(door_id).value_or(false)) {
            merror("door is closed");
            return false;
        }
    }

    entityid box_id2 = tile_has_pullable(bloc.x, bloc.y, bloc.z);
    if (box_id2 == ENTITYID_INVALID) {
        return false;
    }

    if (!df->df_remove_at(id, loc)) {
        merror("Failed to remove %d from (%d, %d)", id, loc.x, loc.y);
        return false;
    }

    auto type = ct.get<entitytype>(id).value_or(ENTITY_NONE);
    bool is_dead_npc = type == ENTITY_NPC && ct.get<dead>(id).value_or(false);
    if (is_dead_npc) {
        tile_t& dst_tile = df->tile_at(aloc);
        dst_tile.add_dead_npc(id);
    }

    if (df->df_add_at(id, type, aloc) == ENTITYID_INVALID) {
        merror("Failed to add %d to (%d, %d)", id, aloc.x, aloc.y);
        return false;
    }

    ct.set<location>(id, aloc);
    float mx = v.x * DEFAULT_TILE_SIZE;
    float my = v.y * DEFAULT_TILE_SIZE;
    ct.set<spritemove>(id, (Rectangle){mx, my, 0, 0});
    if (check_hearing(hero_id, aloc) && IsAudioDeviceReady()) {
        PlaySound(sfx[SFX_STEP_STONE_1]);
    }

    ct.set<steps_taken>(id, ct.get<steps_taken>(id).value_or(0) + 1);
    msuccess("npc %d moved to (%d,%d,%d)", id, aloc.x, aloc.y, aloc.z);
    try_entity_move(box_id2, v);
    if (!processing_actions) {
        update_pressure_plates_for_floor(loc.z);
    }
    msuccess("try_entity_pull(%d)", id);
    return true;
}

inline bool gamestate::try_entity_pickup(entityid id) {
    massert(id != ENTITYID_INVALID, "Entity is NULL!");
    ct.set<update>(id, true);
    optional<vec3> maybe_loc = ct.get<location>(id);
    if (!maybe_loc.has_value()) {
        merror("id %d has no location", id);
        return false;
    }
    vec3 loc = maybe_loc.value();
    shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
    tile_t& tile = df->tile_at(loc);
    bool item_picked_up = false;
    entityid item_id = tile.get_cached_item();
    if (item_id != ENTITYID_INVALID && add_to_inventory(id, item_id)) {
        tile.tile_remove(item_id);
        PlaySound(sfx[SFX_CONFIRM_01]);
        item_picked_up = true;
        string item_name = ct.get<name>(item_id).value_or("no-name-item");
        add_message_history("You picked up %s", item_name.c_str());
    }
    else if (item_id == ENTITYID_INVALID) {
        mwarning("No item cached");
    }
    entitytype_t t = ct.get<entitytype>(id).value_or(ENTITY_NONE);
    if (t == ENTITY_PLAYER) {
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
    }
    return item_picked_up;
}

inline bool gamestate::handle_pickup_item(inputstate& is, bool is_dead) {
    if (!is_action_pressed(is, INPUT_ACTION_PICKUP)) {
        return false;
    }
    if (is_dead) {
        return add_message("You cannot pick up items while dead");
    }
    try_entity_pickup(hero_id);
    flag = GAMESTATE_FLAG_PLAYER_ANIM;
    return true;
}

inline bool gamestate::try_entity_stairs(entityid id) {
    massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
    ct.set<update>(id, true);
    vec3 loc = ct.get<location>(id).value();
    int current_floor = d.current_floor;
    shared_ptr<dungeon_floor> df = d.floors[current_floor];
    tile_t& t = df->tile_at(loc);
    if (t.get_type() == TILE_UPSTAIRS) {
        if (current_floor == 0) {
            add_message("You are already on the top floor!");
        }
        else {
            df->df_remove_at(hero_id, loc);
            d.current_floor--;
            int new_floor = d.current_floor;
            shared_ptr<dungeon_floor> df2 = d.floors[new_floor];
            vec3 uloc = df2->get_downstairs_loc();
            df2->df_add_at(hero_id, ENTITY_PLAYER, uloc);
            ct.set<location>(hero_id, uloc);
            if (processing_actions) {
                queue_pressure_plate_refresh_event(current_floor);
                queue_pressure_plate_refresh_event(new_floor);
            }
            else {
                refresh_pressure_plates();
            }
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            PlaySound(sfx.at(SFX_STEP_STONE_1));
            return true;
        }
    }
    else if (t.get_type() == TILE_DOWNSTAIRS) {
        if ((size_t)current_floor < d.floors.size() - 1) {
            df->df_remove_at(hero_id, loc);
            d.current_floor++;
            int new_floor = d.current_floor;
            shared_ptr<dungeon_floor> df2 = d.floors[new_floor];
            vec3 uloc = df2->get_upstairs_loc();
            df2->df_add_at(hero_id, ENTITY_PLAYER, uloc);
            ct.set<location>(hero_id, uloc);
            if (processing_actions) {
                queue_pressure_plate_refresh_event(current_floor);
                queue_pressure_plate_refresh_event(new_floor);
            }
            else {
                refresh_pressure_plates();
            }
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            PlaySound(sfx.at(SFX_STEP_STONE_1));
            return true;
        }
        else {
            add_message("You can't go downstairs anymore!");
        }
    }
    return false;
}

inline bool gamestate::handle_traverse_stairs(inputstate& is, bool is_dead) {
    if (is_action_pressed(is, INPUT_ACTION_STAIRS)) {
        if (is_dead) {
            return add_message("You cannot traverse stairs while dead");
        }
        run_traverse_stairs_action(hero_id);
        return true;
    }
    return false;
}

inline bool gamestate::try_entity_open_door(entityid id, vec3 loc) {
    massert(id != ENTITYID_INVALID, "id is invalid");
    const entityid door_id = tile_has_door(loc);
    if (door_id == ENTITYID_INVALID) {
        return false;
    }
    massert(tile_has_door(loc) == door_id, "door cache mismatch at (%d, %d, %d)", loc.x, loc.y, loc.z);
    if (door_is_pressure_plate_controlled(door_id)) {
        return add_message("You cannot open this door with your hands");
    }
    optional<bool> maybe_is_open = ct.get<door_open>(door_id);
    massert(maybe_is_open.has_value(), "door %d has no `is_open` component", door_id);
    ct.set<door_open>(door_id, !maybe_is_open.value());
    if (IsAudioDeviceReady() && sfx.size() > SFX_CHEST_OPEN) {
        PlaySound(sfx.at(SFX_CHEST_OPEN));
    }
    return true;
}

inline bool gamestate::try_entity_open_chest(entityid id, vec3 loc) {
    massert(id != ENTITYID_INVALID, "id is invalid");
    const entityid chest_id = tile_has_chest(loc.x, loc.y, loc.z);
    if (chest_id == ENTITYID_INVALID) {
        return false;
    }
    optional<bool> maybe_is_open = ct.get<door_open>(chest_id);
    massert(maybe_is_open.has_value(), "chest %d has no open state component", chest_id);
    if (maybe_is_open.value()) {
        close_chest_menu();
        return true;
    }
    return open_chest_menu(chest_id);
}

inline bool gamestate::try_entity_interact(entityid id, vec3 loc) {
    massert(id != ENTITYID_INVALID, "id is invalid");
    shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
    tile_t& tile = df->tile_at(loc);

    const entityid npc_id = tile.get_cached_live_npc();
    if (npc_id != ENTITYID_INVALID && npc_id != hero_id) {
        const string speaker_name = ct.get<name>(npc_id).value_or("Someone");
        const string text = ct.get<dialogue_text>(npc_id).value_or("They have nothing to say.");
        open_interaction_modal(npc_id, speaker_name, text);
        return true;
    }

    const entityid prop_id = tile.get_cached_prop();
    if (prop_id != ENTITYID_INVALID) {
        const string prop_name = ct.get<name>(prop_id).value_or("Prop");
        const string text = ct.get<description>(prop_id).value_or("There is nothing notable about it.");
        open_interaction_modal(prop_id, prop_name, text);
        return true;
    }

    const entityid box_id = tile.get_cached_box();
    if (box_id != ENTITYID_INVALID) {
        const string box_name = ct.get<name>(box_id).value_or("Box");
        const string text = ct.get<description>(box_id).value_or("A plain wooden box.");
        open_interaction_modal(box_id, box_name, text);
        return true;
    }

    const entityid chest_id = tile.get_cached_chest();
    if (chest_id != ENTITYID_INVALID) {
        const string chest_name = ct.get<name>(chest_id).value_or("Treasure chest");
        const bool is_open = ct.get<door_open>(chest_id).value_or(false);
        const string base_text = ct.get<description>(chest_id).value_or("A stout treasure chest reinforced with iron bands and built to survive rough handling.");
        const string text = interaction_chest_text(base_text, is_open);
        open_interaction_modal(chest_id, chest_name, text);
        return true;
    }

    const entityid door_id = tile.get_cached_door();
    if (door_id != ENTITYID_INVALID) {
        const string door_name = ct.get<name>(door_id).value_or("Door");
        const bool is_open = ct.get<door_open>(door_id).value_or(false);
        const string base_text = ct.get<description>(door_id).value_or("A heavy wooden door bound with iron straps and swollen from the dungeon damp.");
        open_interaction_modal(door_id, door_name, interaction_door_text(base_text, is_open));
        return true;
    }

    return false;
}

inline bool gamestate::handle_interact(inputstate& is, bool is_dead) {
    if (!is_action_pressed(is, INPUT_ACTION_INTERACT)) {
        return false;
    }
    if (is_dead) {
        return add_message("You cannot interact while dead");
    }
    vec3 loc = get_loc_facing_player();
    if (!try_entity_interact(hero_id, loc)) {
        return add_message("There is nothing to interact with");
    }
    flag = GAMESTATE_FLAG_PLAYER_ANIM;
    return true;
}

inline bool gamestate::handle_open_door(inputstate& is, bool is_dead) {
    if (is_action_pressed(is, INPUT_ACTION_OPEN)) {
        if (is_dead) {
            return add_message("You cannot open doors while dead");
        }
        vec3 loc = get_loc_facing_player();
        if (!run_open_chest_action(hero_id, loc)) {
            run_open_door_action(hero_id, loc);
        }
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }
    return false;
}
