#include "libdraw_update_sprite.h"
#include "spritegroup_anim.h"

void libdraw_update_sprite_position(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(sg, "spritegroup is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");

    //Rectangle sprite_move = g_get_sprite_move(g, id);
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
