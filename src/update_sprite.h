#pragma once
#include "ComponentTraits.h"
#include "entitytype.h"
#include "gamestate.h"
#include "set_sg.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"
#include "update_sprite.h"

extern unordered_map<entityid, spritegroup*> spritegroups;

static inline void libdraw_update_sprite_context_ptr(gamestate& g, spritegroup* group, direction_t dir) {
    massert(group != NULL, "group is NULL");
    int old_ctx = group->sprites2->at(group->current)->get_currentcontext();
    int ctx = old_ctx;
    ctx = dir == DIR_NONE                                      ? old_ctx
          : dir == DIR_DOWN_RIGHT                              ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_DOWN_LEFT                               ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_UP_RIGHT                                ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_UP_LEFT                                 ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_R_D  ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_L_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_R_U  ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_L_U  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_R_D    ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_L_D    ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_R_U    ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_L_U    ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_R_D ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_L_D ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_R_U ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_L_U ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_R_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_L_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_R_U  ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_L_U  ? SPRITEGROUP_CONTEXT_L_U
                                                               : old_ctx;
    if (ctx != old_ctx)
        g.frame_dirty = true;
    group->setcontexts(ctx);
}

static inline void libdraw_update_sprite_position(gamestate& g, entityid id, spritegroup* sg) {
    massert(sg, "spritegroup is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    auto maybe_sprite_move = g.ct.get<spritemove>(id);
    if (!maybe_sprite_move.has_value())
        return;
    Rectangle sprite_move = g.ct.get<spritemove>(id).value();
    if (sprite_move.x != 0 || sprite_move.y != 0) {
        sg->move.x = sprite_move.x;
        sg->move.y = sprite_move.y;
        g.ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
        entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
        massert(type != ENTITY_NONE, "entity type is none");
        if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
            race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
            if (r == RACE_BAT)
                sg->current = SG_ANIM_BAT_IDLE;
            else if (r == RACE_GREEN_SLIME)
                sg->current = SG_ANIM_SLIME_IDLE;
            else
                sg->current = SG_ANIM_NPC_WALK;
        }
        g.frame_dirty = true;
    }
}

static inline void libdraw_update_sprite_ptr(gamestate& g, entityid id, spritegroup* sg) {
    minfo3("Begin update sprite ptr: %d", id);
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");

    massert(g.ct.has<update>(id), "id %d has no update component, name %s", id, g.ct.get<name>(id).value().c_str());

    const bool do_update = g.ct.get<update>(id).value();
    if (do_update) {
        if (g.ct.has<direction>(id)) {
            const direction_t d = g.ct.get<direction>(id).value();
            libdraw_update_sprite_context_ptr(g, sg, d);
        }
        g.ct.set<update>(id, false);
    }
    // Copy movement intent from sprite_move_x/y if present
    libdraw_update_sprite_position(g, id, sg);
    if (g.ct.get<block_success>(id).value_or(false))
        libdraw_set_sg_block_success(g, id, sg);
    if (g.ct.get<attacking>(id).value_or(false))
        libdraw_set_sg_is_attacking(g, id, sg);
    //if (g.ct.get<casting>(id).value_or(false))
    //    libdraw_set_sg_is_casting(g, id, sg);
    //if (g->ct.get<spell_casting>(id).value_or(false))
    //    libdraw_set_sg_spell_casting(g, id, sg);
    //else if (g->ct.get<spell_persisting>(id).value_or(false))
    //    libdraw_set_sg_spell_persisting(g, id, sg);
    //else if (g->ct.get<spell_ending>(id).value_or(false))
    //    libdraw_set_sg_spell_ending(g, id, sg);
    if (g.ct.get<dead>(id).value_or(false))
        libdraw_set_sg_is_dead(g, id, sg);
    else if (g.ct.get<damaged>(id).value_or(false))
        libdraw_set_sg_is_damaged(g, id, sg);
    const entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
    if (type == ENTITY_DOOR) {
        auto maybe_door_open = g.ct.get<door_open>(id);
        if (maybe_door_open.has_value())
            sg->set_current(maybe_door_open.value() ? 1 : 0);
    }
    // Update movement as long as sg->move.x/y is non-zero
    //if (spritegroup_update_dest(sg))
    if (sg->update_dest())
        g.frame_dirty = true;
    // Snap to the tile position only when movement is fully complete
    auto maybe_loc = g.ct.get<location>(id);
    if (!maybe_loc.has_value())
        return;
    const vec3 loc = maybe_loc.value();
    sg->snap_dest(loc.x, loc.y);
}

static inline void libdraw_update_sprite_pre(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    if (spritegroups.find(id) != spritegroups.end())
        libdraw_update_sprite_ptr(g, id, spritegroups[id]);
}
