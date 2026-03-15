/** @file libdraw.cpp
 *  @brief Top-level renderer entry points and process-lifetime renderer globals.
 */

#include "libdraw.h"
#include "ComponentTraits.h"
#include "create_sg_byid.h"
#include "draw_handle_gamestate_flag.h"
#include "entitytype.h"
#include "item.h"
#include "libdraw_frame_stats.h"
#include "libdraw_from_texture.h"
#include "libdraw_create_spritegroup.h"
#include "libdraw_update_shield_for_entity.h"
#include "libdraw_to_texture.h"
#include "libdraw_update_sprites.h"
#include "libdraw_update_weapon_for_entity.h"
#include "load_music.h"
#include "load_sfx.h"
#include "load_textures.h"
#include "set_sg.h"
#include "shaders.h"
#include "spritegroup_anim.h"
#include "tx_keys_boxes.h"
#include "tx_keys_chests.h"
#include "tx_keys_doors.h"
#include "tx_keys_monsters.h"
#include "tx_keys_npcs.h"
#include "tx_keys_potions.h"
#include "tx_keys_props.h"
#include "tx_keys_shields.h"
#include "tx_keys_weapons.h"
#include "unload_textures.h"
#include "update_sprite.h"
#include <cstdio>

libdraw_context_t libdraw_ctx;

bool create_spritegroup(gamestate& g, entityid id, int* keys, int num_keys, int offset_x, int offset_y) {
    minfo("BEGIN create_spritegroup");
    massert(libdraw_ctx.txinfo, "txinfo is null");
    spritegroup* group = new spritegroup(SPRITEGROUP_DEFAULT_SIZE);

    massert(group, "spritegroup is NULL");
    auto maybe_loc = g.ct.get<location>(id);

    minfo("checking if has location");
    if (maybe_loc.has_value()) {
        minfo("it DOES have a location");
        const vec3 loc = maybe_loc.value();
        massert(loc.z >= 0 && static_cast<size_t>(loc.z) < g.d.get_floor_count(), "location z out of bounds: %d", loc.z);
        auto df = g.d.get_floor(loc.z);
        massert(df, "dungeon floor is NULL");
        massert(loc.x >= 0 && loc.x < df->get_width(), "location x out of bounds: %d", loc.x);
        massert(loc.y >= 0 && loc.y < df->get_height(), "location y out of bounds: %d", loc.y);

        minfo2("creating spritegroups...");
        minfo2("num_keys: %d", num_keys);
        int count = 0;
        for (int i = 0; i < num_keys; i++) {
            const int k = keys[i];
            minfo("k: %d", k);
            Texture2D* tex = &libdraw_ctx.txinfo[k].texture;
            auto s = make_shared<sprite>(tex, libdraw_ctx.txinfo[k].contexts, libdraw_ctx.txinfo[k].num_frames);
            massert(s, "s is NULL for some reason!");
            group->add(s);
            count++;
        }
        msuccess2("spritegroups created");
        minfo2("count: %d", count);
        minfo2("setting id: %d", id);
        group->id = id;

        string n = g.ct.get<name>(id).value_or("no-name");
        minfo2("name: %s", n.c_str());
        entitytype_t t = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
        string t_s = entitytype_to_str(t);
        minfo2("type: %s", t_s.c_str());
        minfo2("group->get(0)");
        minfo2("group->count(): %lu", group->count());

        auto s = group->get(0);
        massert(s, "sprite is NULL");

        minfo2("group->current = 0");
        group->current = 0;

        float x = loc.x * DEFAULT_TILE_SIZE + offset_x;
        float y = loc.y * DEFAULT_TILE_SIZE + offset_y;
        minfo("getting width");
        float w = s->get_width();
        minfo("getting height");
        float h = s->get_height();

        minfo("setting destination...");
        group->dest = Rectangle{x, y, w, h};
        group->off_x = offset_x;
        group->off_y = offset_y;
        libdraw_ctx.spritegroups[id] = group;

        msuccess("END create spritegroup");
        return true;
    }

    minfo("it does NOT have a location");
    for (int i = 0; i < num_keys; i++) {
        int k = keys[i];
        Texture2D* tex = &libdraw_ctx.txinfo[k].texture;
        auto s = make_shared<sprite>(tex, libdraw_ctx.txinfo[k].contexts, libdraw_ctx.txinfo[k].num_frames);
        group->add(s);
    }
    group->id = id;
    group->current = 0;
    auto s = group->get(0);
    massert(s, "sprite is NULL");
    float x = -DEFAULT_TILE_SIZE + offset_x;
    float y = -DEFAULT_TILE_SIZE + offset_y;
    float w = s->get_width();
    float h = s->get_height();
    group->dest = Rectangle{x, y, w, h};
    group->off_x = offset_x;
    group->off_y = offset_y;
    libdraw_ctx.spritegroups[id] = group;
    msuccess("END create spritegroup");
    return true;
}

bool create_sg(gamestate& g, entityid id, int* keys, int num_keys) {
    return create_spritegroup(g, id, keys, num_keys, -12, -12);
}

void create_npc_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");

    const race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
    massert(r != RACE_NONE, "race is none for id %d", id);

    int* keys = NULL;
    int key_count = 0;

    switch (r) {
    case RACE_HUMAN: keys = TX_HUMAN_KEYS; key_count = TX_HUMAN_COUNT; break;
    case RACE_ORC: keys = TX_ORC_KEYS; key_count = TX_ORC_COUNT; break;
    case RACE_ELF: keys = TX_ELF_KEYS; key_count = TX_ELF_COUNT; break;
    case RACE_DWARF: keys = TX_DWARF_KEYS; key_count = TX_DWARF_COUNT; break;
    case RACE_HALFLING: keys = TX_HALFLING_KEYS; key_count = TX_HALFLING_COUNT; break;
    case RACE_GOBLIN: keys = TX_GOBLIN_KEYS; key_count = TX_GOBLIN_COUNT; break;
    case RACE_WOLF: keys = TX_WOLF_KEYS; key_count = TX_WOLF_COUNT; break;
    case RACE_BAT: keys = TX_BAT_KEYS; key_count = TX_BAT_COUNT; break;
    case RACE_WARG: keys = TX_WARG_KEYS; key_count = TX_WARG_COUNT; break;
    case RACE_GREEN_SLIME: keys = TX_GREEN_SLIME_KEYS; key_count = TX_GREEN_SLIME_COUNT; break;
    case RACE_SKELETON: keys = TX_SKELETON_KEYS; key_count = TX_SKELETON_COUNT; break;
    case RACE_RAT: keys = TX_RAT_KEYS; key_count = TX_RAT_COUNT; break;
    case RACE_ZOMBIE: keys = TX_ZOMBIE_KEYS; key_count = TX_ZOMBIE_COUNT; break;
    default: break;
    }

    massert(keys != NULL, "keys is null");
    massert(key_count > 0, "key_count is not > 0");
    create_sg(g, id, keys, key_count);
}

void create_door_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    minfo("create_door_sg_byid: %d", id);
    create_sg(g, id, TX_WOODEN_DOOR_KEYS, TX_WOODEN_DOOR_COUNT);
}

void create_box_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    create_sg(g, id, TX_WOODEN_BOX_KEYS, TX_WOODEN_BOX_COUNT);
}

void create_chest_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    create_sg(g, id, TX_TREASURE_CHEST_KEYS, TX_TREASURE_CHEST_COUNT);
}

void create_potion_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<potiontype>(id).value_or(POTION_NONE)) {
    case POTION_HP_SMALL: create_sg(g, id, TX_POTION_HP_SMALL_KEYS, TX_POTION_HP_SMALL_COUNT); break;
    case POTION_HP_MEDIUM: create_sg(g, id, TX_POTION_HP_MEDIUM_KEYS, TX_POTION_HP_MEDIUM_COUNT); break;
    case POTION_HP_LARGE: create_sg(g, id, TX_POTION_HP_LARGE_KEYS, TX_POTION_HP_LARGE_COUNT); break;
    case POTION_MP_SMALL: create_sg(g, id, TX_POTION_MP_SMALL_KEYS, TX_POTION_MP_SMALL_COUNT); break;
    case POTION_MP_MEDIUM: create_sg(g, id, TX_POTION_MP_MEDIUM_KEYS, TX_POTION_MP_MEDIUM_COUNT); break;
    case POTION_MP_LARGE: create_sg(g, id, TX_POTION_MP_LARGE_KEYS, TX_POTION_MP_LARGE_COUNT); break;
    default: break;
    }
}

void create_weapon_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<weapontype>(id).value_or(WEAPON_NONE)) {
    case WEAPON_DAGGER: create_sg(g, id, TX_DAGGER_KEYS, TX_DAGGER_COUNT); break;
    case WEAPON_SHORT_SWORD: create_sg(g, id, TX_SHORT_SWORD_KEYS, TX_SHORT_SWORD_COUNT); break;
    case WEAPON_AXE: create_sg(g, id, TX_AXE_KEYS, TX_AXE_COUNT); break;
    default: break;
    }
}

void create_shield_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<shieldtype>(id).value_or(SHIELD_NONE)) {
    case SHIELD_BUCKLER: create_sg(g, id, TX_BUCKLER_KEYS, TX_BUCKLER_COUNT); break;
    case SHIELD_KITE: create_sg(g, id, TX_KITE_SHIELD_KEYS, TX_BUCKLER_COUNT); break;
    case SHIELD_TOWER: create_sg(g, id, TX_TOWER_SHIELD_KEYS, TX_TOWER_SHIELD_COUNT); break;
    default: break;
    }
}

void create_item_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<itemtype>(id).value_or(ITEM_NONE)) {
    case ITEM_POTION: create_potion_sg_byid(g, id); break;
    case ITEM_WEAPON: create_weapon_sg_byid(g, id); break;
    case ITEM_SHIELD: create_shield_sg_byid(g, id); break;
    default: break;
    }
}

void create_prop_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<proptype>(id).value_or(PROP_NONE)) {
    case PROP_DUNGEON_BANNER_00: create_sg(g, id, TX_PROP_DUNGEON_BANNER_00_KEYS, TX_PROP_DUNGEON_BANNER_00_COUNT); break;
    case PROP_DUNGEON_BANNER_01: create_sg(g, id, TX_PROP_DUNGEON_BANNER_01_KEYS, TX_PROP_DUNGEON_BANNER_01_COUNT); break;
    case PROP_DUNGEON_BANNER_02: create_sg(g, id, TX_PROP_DUNGEON_BANNER_02_KEYS, TX_PROP_DUNGEON_BANNER_02_COUNT); break;
    case PROP_DUNGEON_WOODEN_TABLE_00: create_sg(g, id, TX_PROP_DUNGEON_WOODEN_TABLE_00_KEYS, TX_PROP_DUNGEON_WOODEN_TABLE_00_COUNT); break;
    case PROP_DUNGEON_WOODEN_TABLE_01: create_sg(g, id, TX_PROP_DUNGEON_WOODEN_TABLE_01_KEYS, TX_PROP_DUNGEON_WOODEN_TABLE_01_COUNT); break;
    case PROP_DUNGEON_WOODEN_CHAIR_00: create_sg(g, id, TX_PROP_DUNGEON_WOODEN_CHAIR_00_KEYS, TX_PROP_DUNGEON_WOODEN_CHAIR_00_COUNT); break;
    case PROP_DUNGEON_STATUE_00: create_sg(g, id, TX_PROP_DUNGEON_STATUE_00_KEYS, TX_PROP_DUNGEON_STATUE_00_COUNT); break;
    case PROP_DUNGEON_TORCH_00: create_sg(g, id, TX_PROP_DUNGEON_TORCH_00_KEYS, TX_PROP_DUNGEON_TORCH_00_COUNT); break;
    case PROP_DUNGEON_CANDLE_00: create_sg(g, id, TX_PROP_DUNGEON_CANDLE_00_KEYS, TX_PROP_DUNGEON_CANDLE_00_COUNT); break;
    case PROP_DUNGEON_JAR_00: create_sg(g, id, TX_PROP_DUNGEON_JAR_00_KEYS, TX_PROP_DUNGEON_JAR_00_COUNT); break;
    case PROP_DUNGEON_PLATE_00: create_sg(g, id, TX_PROP_DUNGEON_PLATE_00_KEYS, TX_PROP_DUNGEON_PLATE_00_COUNT); break;
    case PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_EMPTY:
        create_sg(g, id, TX_PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_EMPTY_KEYS, TX_PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_EMPTY_COUNT);
        break;
    case PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_WATER:
        create_sg(g, id, TX_PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_WATER_KEYS, TX_PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_WATER_COUNT);
        break;
    default: break;
    }
}

void create_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<entitytype>(id).value_or(ENTITY_NONE)) {
    case ENTITY_PLAYER:
    case ENTITY_NPC: create_npc_sg_byid(g, id); break;
    case ENTITY_DOOR: create_door_sg_byid(g, id); break;
    case ENTITY_BOX: create_box_sg_byid(g, id); break;
    case ENTITY_CHEST: create_chest_sg_byid(g, id); break;
    case ENTITY_ITEM: create_item_sg_byid(g, id); break;
    case ENTITY_PROP: create_prop_sg_byid(g, id); break;
    default: break;
    }
}

void libdraw_set_sg_block_success(gamestate& g, entityid id, spritegroup* const sg) {
    minfo("set sg block success");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    const race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
    if (r == RACE_GREEN_SLIME) {
        minfo("setting SG_ANIM_SLIME_IDLE");
        sg->set_current(SG_ANIM_SLIME_IDLE);
    }
    else if (r == RACE_WOLF) {
        minfo("setting SG_ANIM_WOLF_IDLE");
        sg->set_current(SG_ANIM_WOLF_IDLE);
    }
    else {
        minfo("setting SG_ANIM_NPC_GUARD_SUCCESS");
        sg->set_current(SG_ANIM_NPC_GUARD_SUCCESS);
    }
    update_shield_for_entity(g, id, sg);
    g.ct.set<block_success>(id, false);
}

void libdraw_set_sg_is_damaged(gamestate& g, entityid id, spritegroup* const sg) {
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    const race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
    const int anim_index = r == RACE_GREEN_SLIME ? SG_ANIM_SLIME_DMG : SG_ANIM_NPC_DMG;
    sg->set_current(anim_index);
}

void libdraw_set_sg_is_dead(gamestate& g, entityid id, spritegroup* const sg) {
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");

    if (!g.ct.has<dead>(id)) {
        return;
    }

    if (!g.ct.get<dead>(id).value()) {
        return;
    }

    const race_t r = g.ct.get<race>(id).value();
    if (r == RACE_NONE) {
        return;
    }

    const int anim_index = r == RACE_BAT           ? SG_ANIM_BAT_DIE
                           : r == RACE_GREEN_SLIME ? SG_ANIM_SLIME_DIE
                           : r == RACE_SKELETON    ? SG_ANIM_SKELETON_DIE
                           : r == RACE_RAT         ? SG_ANIM_RAT_DIE
                           : r == RACE_ZOMBIE      ? SG_ANIM_ZOMBIE_DIE
                                                   : SG_ANIM_NPC_SPINDIE;
    if (sg->current == anim_index) {
        return;
    }

    sg->set_default_anim(anim_index);
    sg->set_current(sg->default_anim);
    sg->set_stop_on_last_frame(true);
}

void libdraw_set_sg_is_attacking(gamestate& g, entityid id, spritegroup* const sg) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    const race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
    massert(r != RACE_NONE, "race cant be none");
    sg->set_current(
        r == RACE_GREEN_SLIME ? SG_ANIM_SLIME_JUMP_ATTACK
        : r == RACE_SKELETON  ? SG_ANIM_SKELETON_ATTACK
        : r == RACE_RAT       ? SG_ANIM_RAT_ATTACK
        : r == RACE_ZOMBIE    ? SG_ANIM_ZOMBIE_ATTACK
        : r == RACE_BAT       ? SG_ANIM_BAT_ATTACK
                              : SG_ANIM_NPC_ATTACK);
    update_weapon_for_entity(g, id, sg);
    g.ct.set<attacking>(id, false);
}

void libdraw_update_sprite_context_ptr(gamestate& g, spritegroup* group, direction_t dir) {
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
    if (ctx != old_ctx) {
        g.frame_dirty = true;
    }
    group->setcontexts(ctx);
}

void libdraw_update_sprite_position(gamestate& g, entityid id, spritegroup* sg) {
    massert(sg, "spritegroup is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    auto maybe_sprite_move = g.ct.get<spritemove>(id);
    if (!maybe_sprite_move.has_value()) {
        return;
    }
    Rectangle sprite_move = g.ct.get<spritemove>(id).value();
    if (sprite_move.x != 0 || sprite_move.y != 0) {
        sg->move.x = sprite_move.x;
        sg->move.y = sprite_move.y;
        g.ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
        entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
        massert(type != ENTITY_NONE, "entity type is none");
        if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
            race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
            if (r == RACE_BAT) {
                sg->current = SG_ANIM_BAT_IDLE;
            }
            else if (r == RACE_GREEN_SLIME) {
                sg->current = SG_ANIM_SLIME_IDLE;
            }
            else {
                sg->current = SG_ANIM_NPC_WALK;
            }
        }
        g.frame_dirty = true;
    }
}

void libdraw_update_sprite_ptr(gamestate& g, entityid id, spritegroup* sg) {
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

    libdraw_update_sprite_position(g, id, sg);

    if (g.ct.get<block_success>(id).value_or(false)) {
        libdraw_set_sg_block_success(g, id, sg);
    }

    if (g.ct.get<attacking>(id).value_or(false)) {
        libdraw_set_sg_is_attacking(g, id, sg);
    }

    const entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);

    if (g.ct.get<dead>(id).value_or(false)) {
        libdraw_set_sg_is_dead(g, id, sg);
    }
    else if (g.ct.get<damaged>(id).value_or(false)) {
        libdraw_set_sg_is_damaged(g, id, sg);
    }

    if (type == ENTITY_DOOR || type == ENTITY_CHEST) {
        auto maybe_door_open = g.ct.get<door_open>(id);
        if (maybe_door_open.has_value()) {
            sg->set_current(maybe_door_open.value() ? 1 : 0);
        }
    }

    if (sg->update_dest()) {
        g.frame_dirty = true;
    }

    auto maybe_loc = g.ct.get<location>(id);
    if (!maybe_loc.has_value()) {
        return;
    }
    const vec3 loc = maybe_loc.value();
    sg->snap_dest(loc.x, loc.y);
}

void libdraw_update_sprite_pre(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    if (libdraw_ctx.spritegroups.find(id) != libdraw_ctx.spritegroups.end()) {
        libdraw_update_sprite_ptr(g, id, libdraw_ctx.spritegroups[id]);
    }
}

void libdraw_handle_dirty_entities(gamestate& g) {
    if (!g.dirty_entities) {
        return;
    }
    for (entityid i = g.new_entityid_begin; i < g.new_entityid_end; i++) {
        create_sg_byid(g, i);
        libdraw_update_sprite_pre(g, i);
    }
    g.dirty_entities = false;
    g.new_entityid_begin = ENTITYID_INVALID;
    g.new_entityid_end = ENTITYID_INVALID;
    g.frame_dirty = true;
}

void libdraw_update_sprites_pre(gamestate& g) {
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

void libdraw_update_sprites_post(gamestate& g) {
    if (g.current_scene != SCENE_GAMEPLAY) {
        g.frame_dirty = false;
        return;
    }

    if (g.framecount % libdraw_ctx.anim_speed != 0) {
        libdraw_handle_gamestate_flag(g);
        return;
    }

    libdraw_handle_dirty_entities(g);
    g.frame_dirty = true;

    for (entityid id = 0; id < g.next_entityid; id++) {
        const entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (type == ENTITY_NONE) {
            continue;
        }

        spritegroup* sg = libdraw_ctx.spritegroups[id];
        if (!sg) {
            continue;
        }

        auto s = sg->sprites2->at(sg->current);
        if (!s) {
            continue;
        }

        s->incr_frame();

        if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
            if (s->get_num_loops() >= 1) {
                sg->current = sg->default_anim;
                s->set_num_loops(0);
            }
        }
        else if (type == ENTITY_ITEM) {
            const itemtype_t itype = g.ct.get<itemtype>(id).value_or(ITEM_NONE);
            switch (itype) {
            case ITEM_WEAPON: {
                if (sg->current == 0) {
                    break;
                }
                auto s2 = sg->sprites2->at(sg->current + 1);
                if (!s2) {
                    break;
                }
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
    }
    libdraw_handle_gamestate_flag(g);
}

void libdraw_render_current_scene_to_scene_texture(gamestate& g) {
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

void libdraw_draw_current_scene_from_scene_texture(gamestate& g) {
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

void libdraw_refresh_dirty_scene(gamestate& g) {
    if (!g.frame_dirty) {
        return;
    }

    minfo3("frame is dirty");
    libdraw_render_current_scene_to_scene_texture(g);
    g.frame_dirty = false;
    g.frame_updates++;
    msuccess3("frame is no longer dirty");
}

void libdraw_compose_scene_to_window_target(gamestate& g) {
    BeginTextureMode(libdraw_ctx.target);
    ClearBackground(BLUE);
    libdraw_draw_current_scene_from_scene_texture(g);
    EndTextureMode();
}

void libdraw_present_window_target(gamestate& g) {
    (void)g;
    libdraw_ctx.win_dest.width = GetScreenWidth();
    libdraw_ctx.win_dest.height = GetScreenHeight();
    DrawTexturePro(libdraw_ctx.target.texture, libdraw_ctx.target_src, libdraw_ctx.win_dest, Vector2{0, 0}, 0.0f, WHITE);
    DrawFPS(10, 10);
}

void drawframe(gamestate& g) {
    minfo3("drawframe");
    const double start_time = libdraw_frame_begin_time();
    libdraw_update_sprites_pre(g);

    BeginDrawing();
    ClearBackground(RED);
    libdraw_refresh_dirty_scene(g);
    libdraw_compose_scene_to_window_target(g);
    libdraw_present_window_target(g);
    EndDrawing();
    libdraw_finish_frame_stats(g, start_time);

    libdraw_update_sprites_post(g);
    msuccess3("drawframe");
}

void libdraw_init_render_targets(gamestate& g) {
    g.windowwidth = DEFAULT_WIN_WIDTH;
    g.windowheight = DEFAULT_WIN_HEIGHT;
    g.targetwidth = DEFAULT_TARGET_WIDTH;
    g.targetheight = DEFAULT_TARGET_HEIGHT;

    const TextureFilter filter = TEXTURE_FILTER_POINT;
    libdraw_ctx.target = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    libdraw_ctx.title_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    libdraw_ctx.char_creation_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    libdraw_ctx.main_game_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    libdraw_ctx.hud_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);

    SetTextureFilter(libdraw_ctx.target.texture, filter);
    SetTextureFilter(libdraw_ctx.title_target_texture.texture, filter);
    SetTextureFilter(libdraw_ctx.char_creation_target_texture.texture, filter);
    SetTextureFilter(libdraw_ctx.main_game_target_texture.texture, filter);
    SetTextureFilter(libdraw_ctx.hud_target_texture.texture, filter);

    libdraw_ctx.target_src = Rectangle{0, 0, DEFAULT_TARGET_WIDTH * 1.0f, -DEFAULT_TARGET_HEIGHT * 1.0f};
    libdraw_ctx.target_dest = Rectangle{0, 0, DEFAULT_TARGET_WIDTH * 1.0f, DEFAULT_TARGET_HEIGHT * 1.0f};
}

void libdraw_init_resources(gamestate& g) {
    load_textures(libdraw_ctx.txinfo);
    load_shaders();

    const float x = DEFAULT_TARGET_WIDTH / 4.0f;
    const float y = DEFAULT_TARGET_HEIGHT / 4.0f;
    g.cam2d.offset = Vector2{x, y};

    draw_title_screen_to_texture(g, false);
    draw_char_creation_to_texture(g);

    InitAudioDevice();
    while (!IsAudioDeviceReady()) {
    }
    libdraw_load_music(g);
    libdraw_load_sfx(g);
}

void libdraw_init_rest(gamestate& g) {
    SetExitKey(KEY_ESCAPE);
    SetTargetFPS(60);
    libdraw_init_render_targets(g);
    libdraw_init_resources(g);
}

void libdraw_init(gamestate& g) {
    const int w = DEFAULT_WIN_WIDTH;
    const int h = DEFAULT_WIN_HEIGHT;
    const char* title = WINDOW_TITLE;
    char full_title[1024] = {0};
    snprintf(full_title, sizeof(full_title), "%s - %s", title, g.version.c_str());
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(w, h, full_title);
    g.windowwidth = w;
    g.windowheight = h;
    SetWindowMinSize(320, 240);
    libdraw_init_rest(g);
}

bool libdraw_windowshouldclose(gamestate& g) {
    return g.do_quit;
}

void libdraw_close_partial() {
    UnloadMusicStream(libdraw_ctx.audio.music);
    CloseAudioDevice();
    libdraw_unload_textures(libdraw_ctx.txinfo);
    unload_shaders();
    UnloadRenderTexture(libdraw_ctx.title_target_texture);
    UnloadRenderTexture(libdraw_ctx.char_creation_target_texture);
    UnloadRenderTexture(libdraw_ctx.main_game_target_texture);
    UnloadRenderTexture(libdraw_ctx.hud_target_texture);
    UnloadRenderTexture(libdraw_ctx.target);
}

void libdraw_close() {
    libdraw_close_partial();
    CloseWindow();
}
