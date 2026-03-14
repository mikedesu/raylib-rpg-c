/** @file libdraw_update_sprites.h
 *  @brief Pre/post-frame spritegroup and animation update helpers.
 */

#pragma once

#include "ComponentTraits.h"
#include "draw_handle_gamestate_flag.h"
#include "handle_dirty_entities.h"
#include "libdraw_context.h"
#include "libgame_defines.h"
#include "load_music.h"
#include "update_sprite.h"
#include <raylib.h>

/** @brief Run pre-draw sprite and dirty-entity updates for the current frame. */
static inline void libdraw_update_sprites_pre(gamestate& g) {
    minfo2("BEGIN update sprites pre");
    handle_music_stream(g);
    if (g.current_scene == SCENE_GAMEPLAY) {
        libdraw_handle_dirty_entities(g);
        for (entityid id = 0; id < g.next_entityid; id++) {
            libdraw_update_sprite_pre(g, id);
        }
    }
    msuccess2("END update sprites pre");
}

/** @brief Run post-draw animation advancement and gamestate-flag handling. */
static inline void libdraw_update_sprites_post(gamestate& g) {
    if (g.current_scene != SCENE_GAMEPLAY) {
        g.frame_dirty = false;
        return;
    }

    // for the gameplay scene...
    if (g.framecount % ANIM_SPEED != 0) {
        libdraw_handle_gamestate_flag(g);
        return;
    }

    libdraw_handle_dirty_entities(g);

    g.frame_dirty = true;

    // For every entity...
    for (entityid id = 0; id < g.next_entityid; id++) {
        // verify it has an entity type
        const entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (type == ENTITY_NONE) {
            continue;
        }

        // grab the sprite group for that entity
        spritegroup* sg = spritegroups[id];
        if (!sg) {
            continue;
        }

        auto s = sg->sprites2->at(sg->current);
        if (!s) {
            continue;
        }

        //sprite_incrframe2(s);
        s->incr_frame();

        // this condition for the animation reset seems incorrect
        // certain cases are causing animations to drop-off mid-sequence
        if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
            if (s->get_num_loops() >= 1) {
                sg->current = sg->default_anim;
                s->set_num_loops(0);
            }
        }

        // lets try something
        else if (type == ENTITY_ITEM) {
            const itemtype_t itype = g.ct.get<itemtype>(id).value_or(ITEM_NONE);
            switch (itype) {
            case ITEM_WEAPON: {
                // if the current is > 0, this implies it is equipped
                // or being displayed in a non-tile context
                if (sg->current == 0)
                    break;
                auto s2 = sg->sprites2->at(sg->current + 1);
                if (!s2)
                    break;
                //sprite_incrframe2(s2);
                s2->incr_frame();
                g.frame_dirty = true;
                if (s2->get_num_loops() >= 1) {
                    sg->current = sg->default_anim;
                    s2->set_num_loops(0);
                }
            } break;
            case ITEM_SHIELD: {
                if (sg->current == 0) {
                    break;
                }
                minfo("shield appears to be equipped...");
                auto s2 = sg->sprites2->at(sg->current + 1);
                if (!s2) {
                    break;
                }
                //sprite_incrframe2(s2);
                s2->incr_frame();
                g.frame_dirty = true;
                if (s2->get_num_loops() >= 1) {
                    sg->current = sg->default_anim;
                    s2->set_num_loops(0);
                }
            } break;
            default: break;
            }
        }

        //if (s_shadow) {
        //    sprite_incrframe(s_shadow);
        //    if (s_shadow->num_loops >= 1) {
        //        sg->current = sg->default_anim;
        //        s_shadow->num_loops = 0;
        //    }
        //}
    }
    libdraw_handle_gamestate_flag(g);
}
