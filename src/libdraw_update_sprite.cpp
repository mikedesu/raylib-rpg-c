#include "ComponentTraits.h"
#include "entitytype.h"
#include "libdraw_set_sg_is_attacking.h"
#include "libdraw_update_sprite.h"
#include "set_sg.h"
#include "spritegroup_anim.h"

extern unordered_map<entityid, spritegroup_t*> spritegroups;

void libdraw_update_sprite_pre(shared_ptr<gamestate> g, entityid id) {
    minfo2("Begin update sprite pre: %d", id);
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");

    if (spritegroups.find(id) == spritegroups.end()) {
        merror("Could not find spritegroup for id %d", id);
        return;
    }

    minfo2("Grabbing spritegroup...");
    spritegroup_t* sg = spritegroups[id];
    if (sg) {
        minfo2("Updating spritegroup...");
        libdraw_update_sprite_ptr(g, id, sg);
    }
    //int num_spritegroups = ht_entityid_sg_get_num_entries_for_key(spritegroups, id);
    //for (int i = 0; i < num_spritegroups; i++) {
    //    spritegroup_t* const sg = hashtable_entityid_spritegroup_get_by_index(spritegroups, id, i);
    //    if (sg) {
    //        libdraw_update_sprite_ptr(g, id, sg);
    //    }
    //}
    msuccess2("End update sprite pre: %d", id);
}


void libdraw_update_sprite_position(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    minfo2("update sprite position");
    massert(g, "gamestate is NULL");
    massert(sg, "spritegroup is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");

    optional<Rectangle> maybe_sprite_move = g->ct.get<spritemove>(id);
    if (maybe_sprite_move.has_value()) {
        Rectangle sprite_move = g->ct.get<spritemove>(id).value();

        if (sprite_move.x != 0 || sprite_move.y != 0) {
            sg->move.x = sprite_move.x;
            sg->move.y = sprite_move.y;
            //g_update_sprite_move(g, id, (Rectangle){0, 0, 0, 0});

            g->ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
            //entitytype_t type = g_get_type(g, id);
            entitytype_t type = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
            massert(type != ENTITY_NONE, "entity type is none");

            if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
                //race_t race = g_get_race(g, id);
                //sg->current = race == RACE_BAT ? SG_ANIM_BAT_IDLE : race == RACE_GREEN_SLIME ? SG_ANIM_SLIME_IDLE : SG_ANIM_NPC_WALK;
                //if (loc == RACE_BAT)
                //    sg->current = SG_ANIM_BAT_IDLE;
                //else if (race == RACE_GREEN_SLIME)
                //    sg->current = SG_ANIM_SLIME_IDLE;
                //else if (race > RACE_NONE && race < RACE_COUNT)
                //    sg->current = SG_ANIM_NPC_WALK;
                //else
                sg->current = SG_ANIM_NPC_WALK;
            }
            g->frame_dirty = true;
        }
    }
}


void libdraw_update_sprite_context_ptr(shared_ptr<gamestate> g, spritegroup_t* group, direction_t dir) {
    massert(g, "gamestate is NULL");
    massert(group != NULL, "group is NULL");
    int old_ctx = group->sprites2->at(group->current)->currentcontext;
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
    if (ctx != old_ctx) {
        g->frame_dirty = true;
    }
    spritegroup_setcontexts(group, ctx);
}


void libdraw_update_sprite_ptr(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    minfo2("Begin update sprite ptr: %d", id);
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");

    if (g->ct.get<update>(id).value_or(false)) {
        if (g->ct.has<direction>(id)) {
            direction_t d = g->ct.get<direction>(id).value();
            libdraw_update_sprite_context_ptr(g, sg, d);
        }
        g->ct.set<update>(id, false);
    }

    // Copy movement intent from sprite_move_x/y if present
    minfo2("updating sprite position");
    libdraw_update_sprite_position(g, id, sg);

    minfo2("checking sprite block success...");
    if (g->ct.get<block_success>(id).value_or(false)) {
        libdraw_set_sg_block_success(g, id, sg);
    }

    minfo2("checking sprite attacking...");
    if (g->ct.get<attacking>(id).value_or(false)) {
        libdraw_set_sg_is_attacking(g, id, sg);
    }

    minfo2("checking sprite dead...");
    if (g->ct.get<dead>(id).value_or(false)) {
        libdraw_set_sg_is_dead(g, id, sg);
    }

    minfo2("checking sprite damaged...");
    if (g->ct.get<damaged>(id).value_or(false)) {
        libdraw_set_sg_is_damaged(g, id, sg);
    }


    minfo2("checking doors...");
    const entitytype_t type = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
    if (type == ENTITY_DOOR) {
        auto maybe_door_open = g->ct.get<door_open>(id);
        if (maybe_door_open.has_value()) {
            bool is_open = maybe_door_open.value();
            if (is_open) {
                spritegroup_set_current(sg, 1);

            } else {
                spritegroup_set_current(sg, 0);
            }
        }
    }


    // Update movement as long as sg->move.x/y is non-zero
    minfo2("checking update dest...");
    if (spritegroup_update_dest(sg)) {
        g->frame_dirty = true;
    }
    // Snap to the tile position only when movement is fully complete
    //vec3 loc = g_get_loc(g, id);
    //massert(g->ct.has<Location>(id), "id %d lacks location component", id);


    minfo2("checking sprite loc...");
    optional<vec3> maybe_loc = g->ct.get<location>(id);
    if (maybe_loc.has_value()) {
        vec3 loc = maybe_loc.value();
        spritegroup_snap_dest(sg, loc.x, loc.y);
    }

    msuccess2("End update sprite ptr: %d", id);
}
