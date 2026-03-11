#pragma once

#include "libdraw_context.h"
#include "libdraw_from_texture.h"
#include "libdraw_to_texture.h"

static inline void libdraw_render_current_scene_to_scene_texture(gamestate& g) {
    switch (g.current_scene) {
    case SCENE_TITLE:
        draw_title_screen_to_texture(g, false);
        break;
    case SCENE_MAIN_MENU:
        draw_title_screen_to_texture(g, true);
        break;
    case SCENE_CHARACTER_CREATION:
        minfo3("draw character creation scene to texture");
        draw_char_creation_to_texture(g);
        break;
    case SCENE_GAMEPLAY: {
        const int vision_dist = g.ct.get<vision_distance>(g.hero_id).value_or(0);
        const int light_rad = g.ct.get<light_radius>(g.hero_id).value_or(0);
        libdraw_drawframe_2d_to_texture(g, vision_dist, light_rad);
    } break;
    default:
        break;
    }
}

static inline void libdraw_draw_current_scene_from_scene_texture(gamestate& g) {
    switch (g.current_scene) {
    case SCENE_TITLE:
    case SCENE_MAIN_MENU:
        draw_title_screen_from_texture(g);
        break;
    case SCENE_CHARACTER_CREATION:
        draw_char_creation_from_texture(g);
        break;
    case SCENE_GAMEPLAY:
        libdraw_drawframe_2d_from_texture(g);
        break;
    default:
        break;
    }
}
