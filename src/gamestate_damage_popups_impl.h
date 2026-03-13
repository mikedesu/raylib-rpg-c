#pragma once

inline void gamestate::add_damage_popup(entityid target_id, int amount, bool critical) {
    if (amount <= 0) {
        return;
    }

    const optional<vec3> maybe_loc = ct.get<location>(target_id);
    if (!maybe_loc.has_value()) {
        return;
    }

    const vec3 loc = maybe_loc.value();
    const size_t active_on_target = std::count_if(
        damage_popups.begin(),
        damage_popups.end(),
        [target_id, floor = loc.z](const damage_popup_t& popup) {
            return popup.target_id == target_id && popup.floor == floor;
        });

    uniform_real_distribution<float> drift_dist(-2.5f, 2.5f);
    damage_popup_t popup = {};
    popup.target_id = target_id;
    popup.amount = amount;
    popup.critical = critical;
    popup.floor = loc.z;
    popup.world_anchor = Vector2{
        loc.x * DEFAULT_TILE_SIZE + (DEFAULT_TILE_SIZE * 0.5f),
        loc.y * DEFAULT_TILE_SIZE - 4.0f - static_cast<float>(active_on_target) * 2.0f,
    };
    popup.age_seconds = 0.0f;
    popup.lifetime_seconds = 0.7f;
    popup.drift_x = drift_dist(mt);
    popup.rise_distance = 10.0f + static_cast<float>(active_on_target);

    constexpr size_t max_damage_popups = 48;
    if (damage_popups.size() >= max_damage_popups) {
        damage_popups.erase(damage_popups.begin());
    }
    damage_popups.push_back(popup);
    frame_dirty = true;
}

inline void gamestate::update_damage_popups(float dt_seconds) {
    if (damage_popups.empty()) {
        return;
    }

    const float dt = std::max(0.0f, dt_seconds);
    for (damage_popup_t& popup : damage_popups) {
        popup.age_seconds += dt;
    }

    damage_popups.erase(
        std::remove_if(
            damage_popups.begin(),
            damage_popups.end(),
            [](const damage_popup_t& popup) { return popup.age_seconds >= popup.lifetime_seconds; }),
        damage_popups.end());

    frame_dirty = true;
}
