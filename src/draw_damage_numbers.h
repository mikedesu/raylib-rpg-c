/** @file draw_damage_numbers.h
 *  @brief Floating combat damage-number drawing helpers.
 */

#pragma once

#include "gamestate.h"
#include <string>

/** @brief Compute a zoom-aware world-space font size for one damage popup. */
static inline float damage_popup_font_size_world(const gamestate& g, const damage_popup_t& popup) {
    const float progress = popup.lifetime_seconds > 0.0f ? popup.age_seconds / popup.lifetime_seconds : 1.0f;
    const float pop_scale = 1.0f + (1.0f - std::min(progress, 1.0f)) * 0.18f;
    const float zoom = std::max(1.0f, g.cam2d.zoom);
    return std::max(3.0f, (18.0f * pop_scale) / zoom);
}

/** @brief Draw all active floating damage-number popups for the current floor. */
static inline void draw_damage_numbers(gamestate& g) {
    if (g.damage_popups.empty()) {
        return;
    }

    for (const damage_popup_t& popup : g.damage_popups) {
        if (popup.floor != g.d.current_floor) {
            continue;
        }

        const float progress = popup.lifetime_seconds > 0.0f ? popup.age_seconds / popup.lifetime_seconds : 1.0f;
        const float clamped_progress = std::min(std::max(progress, 0.0f), 1.0f);
        const float rise = popup.rise_distance * (1.0f - (1.0f - clamped_progress) * (1.0f - clamped_progress));
        const float drift_x = popup.drift_x * clamped_progress;
        const float font_size = damage_popup_font_size_world(g, popup);
        const float spacing = 0.0f;
        const std::string text = std::to_string(popup.amount);
        const Vector2 measure = MeasureTextEx(GetFontDefault(), text.c_str(), font_size, spacing);
        const Vector2 pos = {
            popup.world_anchor.x + drift_x - measure.x / 2.0f,
            popup.world_anchor.y - rise,
        };
        const unsigned char alpha = static_cast<unsigned char>(255.0f * (1.0f - clamped_progress));
        const Color text_color = popup.critical ? Color{255, 64, 64, alpha} : Color{255, 255, 255, alpha};
        const Color shadow_color = Color{0, 0, 0, static_cast<unsigned char>(alpha * 0.7f)};

        DrawTextEx(GetFontDefault(), text.c_str(), Vector2{pos.x + 0.5f, pos.y + 0.5f}, font_size, spacing, shadow_color);
        DrawTextEx(GetFontDefault(), text.c_str(), pos, font_size, spacing, text_color);
    }
}
