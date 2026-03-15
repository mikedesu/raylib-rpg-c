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
void draw_damage_numbers(gamestate& g);
