#include "ComponentTraits.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate.h"
#include "libdraw.h"
#include "libdraw_camera_lock_on.h"
#include "libdraw_dungeon_tiles_2d.h"
#include "libdraw_handle_debug_panel.h"
#include "libdraw_load_music.h"
#include "libdraw_load_sfx.h"
#include "libdraw_player_target_box.h"
//#include "libdraw_update_debug_panel.h"
#include "race.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"
#include "tx_keys_boxes.h"
#include "tx_keys_monsters.h"
#include "tx_keys_npcs.h"
#include "tx_keys_potions.h"
#include "tx_keys_weapons.h"

//#include "get_txkey_for_tiletype.h"
//#include "gamestate_equipped_weapon.h"
//#include "libdraw_version.h"
//#include "tx_keys_rings.h"
//#include "tx_keys_shields.h"

unordered_map<entityid, spritegroup_t*> spritegroups;

textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];


Shader shader_grayscale = {0};
Shader shader_glow = {0};
Shader shader_red_glow = {0};
Shader shader_color_noise = {0};
Shader shader_psychedelic_0 = {0};


RenderTexture2D title_target_texture = {0};
RenderTexture2D char_creation_target_texture = {0};
RenderTexture2D main_game_target_texture = {0};
RenderTexture2D hud_target_texture = {0};


RenderTexture2D target = {0};
Rectangle target_src = {0, 0, DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT};
Rectangle target_dest = {0, 0, DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT};
Rectangle win_dest = {0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};


Vector2 target_origin = {0, 0};
Vector2 zero_vec = {0, 0};


Music music;


int ANIM_SPEED = DEFAULT_ANIM_SPEED;
int libdraw_restart_count = 0;


void draw_hud_from_texture(shared_ptr<gamestate> g);

void draw_hud_to_texture(shared_ptr<gamestate> g);

void libdraw_drawframe_2d_from_texture(shared_ptr<gamestate> g);

void libdraw_drawframe_2d_to_texture(shared_ptr<gamestate> g);

void libdraw_update_sprite_pre(shared_ptr<gamestate> g, entityid id);

void libdraw_handle_gamestate_flag(shared_ptr<gamestate> g);

void libdraw_set_sg_is_damaged(shared_ptr<gamestate> g, entityid id, spritegroup_t* const sg);

void libdraw_set_sg_is_dead(shared_ptr<gamestate> g, entityid id, spritegroup_t* const sg);

void libdraw_set_sg_is_attacking(shared_ptr<gamestate> g, entityid id, spritegroup_t* const sg);

void libdraw_set_sg_block_success(shared_ptr<gamestate> g, entityid id, spritegroup_t* const sg);

void libdraw_unload_shaders();

void libdraw_update_sprite_position(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);

void libdraw_update_sprite_context_ptr(shared_ptr<gamestate> g, spritegroup_t* group, direction_t dir);

void libdraw_update_sprite_ptr(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);

void libdraw_drawframe_2d(shared_ptr<gamestate> g);

void draw_gameplay_settings_menu(shared_ptr<gamestate> g);

void draw_gameover_menu(shared_ptr<gamestate> g);

void draw_weapon_sprite_front(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);

void draw_weapon_sprite_back(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);

void draw_shield_sprite_front(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);

void draw_shield_sprite_back(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);

void draw_title_screen_to_texture(shared_ptr<gamestate> g, bool show_menu);

void draw_title_screen_from_texture(shared_ptr<gamestate> g);

void draw_character_creation_screen_from_texture(shared_ptr<gamestate> g);

void draw_character_creation_screen_to_texture(shared_ptr<gamestate> g);

void draw_message_box(shared_ptr<gamestate> g);

void draw_sprite_and_shadow(const shared_ptr<gamestate> g, entityid id);

void draw_debug_panel(shared_ptr<gamestate> g);

void create_sg_byid(shared_ptr<gamestate> g, entityid id);

void load_shaders();

bool draw_entities_2d_at(const shared_ptr<gamestate> g, shared_ptr<dungeon_floor_t> df, bool dead, vec3 loc);

bool libdraw_draw_dungeon_floor(const shared_ptr<gamestate> g);

void update_weapon_for_entity(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);

shared_ptr<sprite> get_weapon_back_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);

shared_ptr<sprite> get_weapon_front_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);

sprite* get_shield_front_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);

sprite* get_shield_back_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);

// sprite* get_weapon_back_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
// sprite* get_weapon_front_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
// bool libdraw_check_default_animations(const shared_ptr<gamestate> g);
// void draw_shadow_for_entity(const shared_ptr<gamestate> g,
//                                   spritegroup_t* sg,
//                                   entityid id);


shared_ptr<sprite> get_weapon_front_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(sg, "spritegroup is NULL");
    shared_ptr<sprite> retval = nullptr;
    entityid weapon = g->ct.get<equipped_weapon>(id).value_or(ENTITYID_INVALID);
    if (weapon == ENTITYID_INVALID) return retval;
    auto it = spritegroups.find(weapon);
    if (it == spritegroups.end()) return retval;
    spritegroup_t* w_sg = it->second;
    if (!w_sg) return retval;

    if (sg->current == SG_ANIM_NPC_ATTACK) {
        retval = spritegroup_get(w_sg, SG_ANIM_LONGSWORD_SLASH_F);
    }
    //else if (sg->current == SG_ANIM_NPC_SHOT) {
    //    retval = spritegroup_get(w_sg, SG_ANIM_BOW_SHOT_F);
    //}
    return retval;
}


shared_ptr<sprite> get_weapon_back_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(sg, "spritegroup is NULL");

    shared_ptr<sprite> retval = nullptr;
    entityid weapon = g->ct.get<equipped_weapon>(id).value_or(ENTITYID_INVALID);
    if (weapon == ENTITYID_INVALID) return retval;
    auto it = spritegroups.find(weapon);
    if (it == spritegroups.end()) return retval;
    spritegroup_t* w_sg = it->second;
    if (!w_sg) return retval;

    if (sg->current == SG_ANIM_NPC_ATTACK) {
        retval = spritegroup_get(w_sg, SG_ANIM_LONGSWORD_SLASH_B);
    }
    //else if (sg->current == SG_ANIM_NPC_SHOT) {
    //    retval = spritegroup_get(w_sg, SG_ANIM_BOW_SHOT_B);
    //}


    return retval;
}


/*
sprite* get_shield_front_sprite(const shared_ptr<gamestate> g,
                                       entityid id,
                                       spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(sg, "spritegroup is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    entityid shield = g_get_equipment(g, id, EQUIP_SLOT_SHIELD);
    if (shield == ENTITYID_INVALID) return NULL;
    spritegroup_t* s_sg =
        hashtable_entityid_spritegroup_get(spritegroups, shield);
    if (!s_sg) {
        return NULL;
    }
    sprite* retval = NULL;
    if (sg->current == SG_ANIM_NPC_GUARD_SUCCESS) {
        retval = spritegroup_get(s_sg, SG_ANIM_BUCKLER_SUCCESS_FRONT);
    }
    return retval;
}
*/


/*
sprite* get_shield_back_sprite(const shared_ptr<gamestate> g,
                                      entityid id,
                                      spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(sg, "spritegroup is NULL");
    entityid shield = g_get_equipment(g, id, EQUIP_SLOT_SHIELD);
    if (shield == ENTITYID_INVALID) return NULL;
    spritegroup_t* s_sg =
        hashtable_entityid_spritegroup_get(spritegroups, shield);
    if (!s_sg) return NULL;
    sprite* retval = NULL;
    if (sg->current == SG_ANIM_NPC_GUARD_SUCCESS) {
        retval = spritegroup_get(s_sg, SG_ANIM_BUCKLER_SUCCESS_BACK);
    }
    return retval;
}
*/


// void draw_shadow_for_entity(const shared_ptr<gamestate> g,
//                                   spritegroup_t* sg,
//                                   entityid id) {
//    massert(g, "gamestate is NULL");
//    massert(sg, "spritegroup is NULL");
//    massert(id != ENTITYID_INVALID, "id is -1");
//    entitytype_t t = g_get_type(g, id);
//    if (t != ENTITY_PLAYER && t != ENTITY_NPC) return;
//    sprite* sh = sg_get_current_plus_one(sg);
//    if (sh) {
//        DrawTexturePro(
//            *sh->texture,
//            sh->src,
//            (Rectangle){
//                sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height},
//            (Vector2){0, 0},
//            0,
//            WHITE);
//    }
//}


void draw_entity_sprite(const shared_ptr<gamestate> g, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(sg, "spritegroup is NULL");
    //sprite* s = sg_get_current(sg);
    shared_ptr<sprite> s = sg_get_current(sg);
    massert(s, "sprite is NULL");
    Rectangle dest = {sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};
    //unsigned char a = 255;

    //unsigned char a = (unsigned char)g_get_tx_alpha(g, sg->id);
    unsigned char a = g->ct.get<txalpha>(sg->id).value_or(255);

    DrawTexturePro(*s->texture, s->src, dest, zero_vec, 0, (Color){255, 255, 255, a});
    // draw a box around the sprite
    //DrawRectangleLinesEx(dest, 1, (Color){255, 0, 0, 255});
}

/*
 void draw_shield_sprite_back(const shared_ptr<gamestate> g,
                                    entityid id,
                                    spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    sprite* shield_back_s = get_shield_back_sprite(g, id, sg);
    if (shield_back_s) {
        DrawTexturePro(*shield_back_s->texture,
                       shield_back_s->src,
                       sg->dest,
                       (Vector2){0, 0},
                       0,
                       WHITE);
    }
}
*/

/*
 void draw_shield_sprite_front(const shared_ptr<gamestate> g,
                                     entityid id,
                                     spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    sprite* shield_front_s = get_shield_front_sprite(g, id, sg);
    if (shield_front_s)
        DrawTexturePro(*shield_front_s->texture,
                       shield_front_s->src,
                       sg->dest,
                       (Vector2){0, 0},
                       0,
                       WHITE);
}
*/

void draw_weapon_sprite_back(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    shared_ptr<sprite> weapon_back_s = get_weapon_back_sprite(g, id, sg);
    if (weapon_back_s) {
        DrawTexturePro(*weapon_back_s->texture, weapon_back_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
    }
}


void draw_weapon_sprite_front(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    shared_ptr<sprite> weapon_front_s = get_weapon_front_sprite(g, id, sg);
    if (weapon_front_s) {
        DrawTexturePro(*weapon_front_s->texture, weapon_front_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
    }
}


void draw_sprite_and_shadow(const shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    if (spritegroups.find(id) == spritegroups.end()) {
        return;
    }
    spritegroup_t* sg = spritegroups[id];


    // old
    //entitytype_t type = g_get_type(g, id);
    //massert(sg, "spritegroup is NULL: id %d type: %s", id, entitytype_to_string(type));
    // Draw components in correct order
    //draw_shadow_for_entity(g, sg, id);
    //draw_shield_sprite_back(g, id, sg);
    draw_weapon_sprite_back(g, id, sg);
    draw_entity_sprite(g, sg);
    //draw_shield_sprite_front(g, id, sg);
    draw_weapon_sprite_front(g, id, sg);
}


bool draw_entities_2d_at(const shared_ptr<gamestate> g, shared_ptr<dungeon_floor_t> df, bool dead, vec3 loc) {
    massert(g, "draw_entities_2d: gamestate is NULL");
    massert(df, "draw_entities_2d: dungeon_floor is NULL");
    massert(df->width > 0, "draw_entities_2d: dungeon_floor width is 0");
    massert(df->height > 0, "draw_entities_2d: dungeon_floor height is 0");
    //massert(df->width <= DEFAULT_DUNGEON_FLOOR_WIDTH, "draw_entities_2d: dungeon_floor width is too large");
    //massert(df->height <= DEFAULT_DUNGEON_FLOOR_HEIGHT, "draw_entities_2d: dungeon_floor height is too large");
    massert(loc.x >= 0, "draw_entities_2d: x is out of bounds");
    massert(loc.x < df->width, "draw_entities_2d: x is out of bounds");
    massert(loc.y >= 0, "draw_entities_2d: y is out of bounds");
    massert(loc.y < df->height, "draw_entities_2d: y is out of bounds");
    shared_ptr<tile_t> tile = df_tile_at(df, loc);
    if (!tile) {
        //merror("draw_entities_2d: tile is NULL at (%d, %d)", (int)loc.x, (int)loc.y);
        return false;
    }
    if (tile_is_wall(tile->type)) {
        //merror("draw_entities_2d: tile is a wall at (%d, %d)", (int)loc.x, (int)loc.y);
        return false;
    }
    if (!tile->visible) {
        //merror("draw_entities_2d: tile is not visible at (%d, %d)", (int)loc.x, (int)loc.y);
        return true; // Do not draw entities on invisible tiles
    }
    if (tile->is_empty) {
        return true;
    }
    // Get hero's vision distance and location
    //int vision_distance = g_get_vision_distance(g, g->hero_id);
    //int light_dist = g_get_light_radius(g, g->hero_id) + g_get_entity_total_light_radius_bonus(g, g->hero_id);
    //int light_dist = 3;
    //vec3 hero_loc = g_get_location(g, g->hero_id);
    //int dist_to_check = MAX(vision_distance, light_dist);
    // Calculate Manhattan distance from hero to this tile (diamond pattern)
    //int distance = abs(loc.x - hero_loc.x) + abs(loc.y - hero_loc.y);
    // Only draw entities within vision distance
    //if (distance <= vision_distance) {
    //if (distance <= dist_to_check) {
    //for (size_t i = 0; i < tile_entity_count(tile); i++) {
    for (size_t i = 0; i < tile->entities->size(); i++) {
        entityid id = tile_get_entity(tile, i);
        draw_sprite_and_shadow(g, id);
    }
    return true;
}


void load_shaders() {
    shader_grayscale = LoadShader(0, "grayscale.frag"); // No vertex shader needed
    shader_glow = LoadShader(0, "glow.frag");
    shader_red_glow = LoadShader(0, "red-glow.frag");
    shader_color_noise = LoadShader(0, "colornoise.frag");
    shader_psychedelic_0 = LoadShader(0, "psychedelic-0.frag");
    //shader_tile_glow = LoadShader(0, "psychedelic_ripple.frag");
}


void libdraw_unload_shaders() {
    UnloadShader(shader_grayscale);
    UnloadShader(shader_glow);
    UnloadShader(shader_red_glow);
    UnloadShader(shader_color_noise);
    UnloadShader(shader_psychedelic_0);
}


// bool libdraw_check_default_animations(const gamestate* const g) {
//    massert(g, "gamestate is NULL");
//    for (entityid id = 0; id < g->next_entityid; id++) {
//        spritegroup_t* const sg = hashtable_entityid_spritegroup_get(spritegroups, id);
//        if (sg && sg->current != sg->default_anim) {
//            // which sg isnt done?
//            return false;
//        }
//    }
//    return true;
//}

void libdraw_set_sg_is_damaged(shared_ptr<gamestate> g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    race_t r = g->ct.get<race>(id).value_or(RACE_NONE);
    int anim_index = SG_ANIM_NPC_DMG;
    if (r == RACE_GREEN_SLIME) {
        anim_index = SG_ANIM_SLIME_DMG;
    }
    spritegroup_set_current(sg, anim_index);
    g->ct.set<damaged>(id, true);
}


void libdraw_set_sg_is_dead(shared_ptr<gamestate> g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    if (!g->ct.get<dead>(id).has_value()) {
        return;
    }
    if (!g->ct.get<dead>(id).value()) {
        return;
    }

    race_t r = g->ct.get<race>(id).value_or(RACE_NONE);
    if (r == RACE_NONE) {
        return;
    }

    int anim_index = SG_ANIM_NPC_SPINDIE;

    if (r == RACE_BAT) {
        anim_index = SG_ANIM_BAT_DIE;
    } else if (r == RACE_GREEN_SLIME) {
        anim_index = SG_ANIM_SLIME_DIE;
    }

    if (sg->current == anim_index) {
        return;
    }

    sg_set_default_anim(sg, anim_index);
    spritegroup_set_current(sg, sg->default_anim);
    spritegroup_set_stop_on_last_frame(sg, true);
}


void update_weapon_for_entity(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");

    spritegroup_t* w_sg = nullptr;
    entityid weaponid = ENTITYID_INVALID;
    int ctx = -1;

    weaponid = g->ct.get<equipped_weapon>(id).value_or(ENTITYID_INVALID);
    if (weaponid == ENTITYID_INVALID) return;
    w_sg = spritegroups[weaponid];
    if (!w_sg) return;

    ctx = sg->sprites2->at(sg->current)->currentcontext;
    spritegroup_setcontexts(w_sg, ctx);

    // this really should be either SLASH_F or SLASH_B
    // eventually we will select this based on other factors as well
    // or rather we'll have a better way to select which animation joins the entity's attack
    // lets test this theory by changing it to the B anim

    //spritegroup_set_current(w_sg, SG_ANIM_LONGSWORD_SLASH_B);
    spritegroup_set_current(w_sg, SG_ANIM_LONGSWORD_SLASH_F);
}


void libdraw_set_sg_is_attacking(shared_ptr<gamestate> g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    //race_t race = g_get_race(g, id);
    //entityid weapon = g_get_equipment(g, id, EQUIP_SLOT_WEAPON);
    //weapontype wtype = g_get_weapontype(g, weapon);
    int cur = SG_ANIM_NPC_ATTACK;
    //if (wtype == WEAPON_BOW) {
    //    cur = SG_ANIM_NPC_SHOT;
    //}
    //if (race == RACE_BAT) {
    //    cur = SG_ANIM_BAT_ATTACK;
    //} else if (race == RACE_GREEN_SLIME) {
    //    cur = SG_ANIM_SLIME_ATTACK;
    //}
    spritegroup_set_current(sg, cur);
    update_weapon_for_entity(g, id, sg);
    g->ct.set<attacking>(id, false);
}


/*
 void libdraw_set_sg_block_success(shared_ptr<gamestate> g,
                                         entityid id,
                                         spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    race_t race = g_get_race(g, id);
    int anim_index = -1;
    anim_index = SG_ANIM_NPC_GUARD_SUCCESS;
    if (race == RACE_BAT) {
        anim_index = SG_ANIM_BAT_IDLE;
    } else if (race == RACE_GREEN_SLIME) {
        anim_index = SG_ANIM_SLIME_IDLE;
    }
    spritegroup_set_current(sg, anim_index);
    entityid shield_id = g_get_equipment(g, id, EQUIP_SLOT_SHIELD);
    if (shield_id != ENTITYID_INVALID) {
        spritegroup_t* shield_sg =
            hashtable_entityid_spritegroup_get(spritegroups, shield_id);
        if (shield_sg) {
            sprite* player_sprite = sg_get_current(sg);
            int player_ctx = sprite_get_context(player_sprite);
            spritegroup_set_current(shield_sg, SG_ANIM_BUCKLER_SUCCESS_FRONT);
            spritegroup_setcontexts(shield_sg, player_ctx);
        }
    }
    g_set_block_success(g, id, false);
}
*/


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


void libdraw_update_sprite_ptr(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg) {
    minfo2("Begin update sprite ptr: %d", id);
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    //if (g_is_dead(g, id) && !spritegroup_is_animating(sg)) return;
    //if (g_get_update(g, id)) {
    if (g->ct.get<update>(id).value_or(false)) {
        //libdraw_update_sprite_context_ptr(g, sg, g_get_direction(g, id));
        //libdraw_update_sprite_context_ptr(g, sg, DIR_DOWN_LEFT);

        //libdraw_update_sprite_context_ptr(g, sg, g_get_dir(g, id));

        if (g->ct.has<direction>(id)) {
            direction_t d = g->ct.get<direction>(id).value();
            libdraw_update_sprite_context_ptr(g, sg, d);
        }
        //g_set_update(g, id, false);
        g->ct.set<update>(id, false);
    }
    // Copy movement intent from sprite_move_x/y if present
    libdraw_update_sprite_position(g, id, sg);

    //if (g_get_attacking(g, id)) {
    if (g->ct.get<attacking>(id).value_or(false)) {
        libdraw_set_sg_is_attacking(g, id, sg);
    }
    //else if (g_get_block_success(g, id)) {
    //    libdraw_set_sg_block_success(g, id, sg);
    //} else

    if (g->ct.get<dead>(id).has_value()) {
        bool is_dead = g->ct.get<dead>(id).value();
        if (is_dead) {
            libdraw_set_sg_is_dead(g, id, sg);
        }
    } else if (g->ct.get<damaged>(id).has_value()) {
        bool is_damaged = g->ct.get<damaged>(id).value();
        if (is_damaged) {
            libdraw_set_sg_is_damaged(g, id, sg);
        }
    }


    // Update movement as long as sg->move.x/y is non-zero
    if (spritegroup_update_dest(sg)) {
        g->frame_dirty = true;
    }
    // Snap to the tile position only when movement is fully complete
    //vec3 loc = g_get_loc(g, id);
    //massert(g->ct.has<Location>(id), "id %d lacks location component", id);
    vec3 loc = g->ct.get<location>(id).value();
    spritegroup_snap_dest(sg, loc.x, loc.y);
    minfo2("End update sprite ptr: %d", id);
}


void libdraw_update_sprite_pre(shared_ptr<gamestate> g, entityid id) {
    minfo2("Begin update sprite pre: %d", id);
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    if (spritegroups.find(id) == spritegroups.end()) {
        return;
    }
    spritegroup_t* sg = spritegroups[id];
    if (sg) {
        libdraw_update_sprite_ptr(g, id, sg);
    }
    //int num_spritegroups = ht_entityid_sg_get_num_entries_for_key(spritegroups, id);
    //for (int i = 0; i < num_spritegroups; i++) {
    //    spritegroup_t* const sg = hashtable_entityid_spritegroup_get_by_index(spritegroups, id, i);
    //    if (sg) {
    //        libdraw_update_sprite_ptr(g, id, sg);
    //    }
    //}
    minfo2("End update sprite pre: %d", id);
}


void libdraw_handle_gamestate_flag(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    if (g->flag == GAMESTATE_FLAG_PLAYER_ANIM) {
        g->flag = GAMESTATE_FLAG_NPC_TURN;
        //g->test_guard = false;
    } else if (g->flag == GAMESTATE_FLAG_NPC_ANIM) {
        g->entity_turn = g->hero_id; // Reset directly to hero
        g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
        g->turn_count++;
    }
}


void libdraw_handle_dirty_entities(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    minfo2("Begin handle dirty entities");
    if (g->dirty_entities) {
        for (entityid i = g->new_entityid_begin; i < g->new_entityid_end; i++) {
            create_sg_byid(g, i);
        }
        g->dirty_entities = false;
        g->new_entityid_begin = ENTITYID_INVALID;
        g->new_entityid_end = ENTITYID_INVALID;
        g->frame_dirty = true;
    }
    minfo2("End handle dirty entities");
}


void libdraw_update_sprites_pre(shared_ptr<gamestate> g) {
    minfo2("Begin update sprites pre");
    massert(g, "gamestate is NULL");

    UpdateMusicStream(music);
    //if (g->music_volume_changed) {
    //    SetMusicVolume(music, g->music_volume);
    //    g->music_volume_changed = false;
    //}
    if (g->current_scene == SCENE_GAMEPLAY) {
        libdraw_handle_dirty_entities(g);
        minfo2("Begin update sprites pre loop");
        for (entityid id = 0; id < g->next_entityid; id++) {
            libdraw_update_sprite_pre(g, id);
        }
        minfo2("End update sprites pre loop");
    }
    minfo2("End update sprites pre");
}


void libdraw_update_sprites_post(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    //if (g->music_volume_changed) {
    //    SetMusicVolume(music, g->music_volume);
    //    g->music_volume_changed = false;
    //}
    //UpdateMusicStream(music);
    if (g->current_scene != SCENE_GAMEPLAY) {
        g->frame_dirty = false;
        return;
    }
    // for the gameplay scene...
    if (g->framecount % ANIM_SPEED == 0) {
        libdraw_handle_dirty_entities(g);
        g->frame_dirty = true;


        // For every entity...
        for (entityid id = 0; id < g->next_entityid; id++) {
            // verify it has an entity type
            entitytype_t type = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
            //spritegroup_t* const sg = hashtable_entityid_spritegroup_get_by_index(spritegroups, id, i);

            // grab the sprite group for that entity
            spritegroup_t* sg = spritegroups[id];
            if (sg) {
                // for every sprite in the spritegroup (INCORRECT!!!! NOT WHAT WE WANT, CAUSE-OF-BUG!!!)
                //int num_sprites = sg->sprites2->size();
                //for (int i = 0; i < num_sprites; i++) {
                //shared_ptr<sprite> s = sg->sprites2->at(i);
                shared_ptr<sprite> s = sg->sprites2->at(sg->current);
                if (s) {
                    sprite_incrframe2(s);
                    g->frame_dirty = true;
                    // this condition for the animation reset seems incorrect
                    // certain cases are causing animations to drop-off mid-sequence
                    if (s->num_loops >= 1) {
                        sg->current = sg->default_anim;
                        s->num_loops = 0;
                    }

                    // lets try something
                    if (type == ENTITY_ITEM) {
                        itemtype_t itype = g->ct.get<itemtype>(id).value_or(ITEM_NONE);
                        if (itype == ITEM_WEAPON) {
                            if (sg->current > 0) {
                                shared_ptr<sprite> s2 = sg->sprites2->at(sg->current + 1);
                                if (s2) {
                                    sprite_incrframe2(s2);
                                    g->frame_dirty = true;
                                    // this condition for the animation reset seems incorrect
                                    // certain cases are causing animations to drop-off mid-sequence
                                    if (s2->num_loops >= 1) {
                                        sg->current = sg->default_anim;
                                        s2->num_loops = 0;
                                    }
                                }
                            }
                        }
                    }
                }


                //}
            }
            //}
            //if (s_shadow) {
            //    sprite_incrframe(s_shadow);
            //    if (s_shadow->num_loops >= 1) {
            //        sg->current = sg->default_anim;
            //        s_shadow->num_loops = 0;
            //    }
            //}
        }
    }
    libdraw_handle_gamestate_flag(g);
}


bool libdraw_draw_dungeon_floor(const shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    //dungeon_floor_t* const df = d_get_current_floor(g->d);
    shared_ptr<dungeon_floor_t> df = d_get_current_floor(g->dungeon);
    massert(df, "dungeon_floor is NULL");
    int z = g->dungeon->current_floor;
    draw_dungeon_tiles_2d(g, z, df);
    // dead
    //for (int y = 0; y < df->height; y++) {
    //    for (int x = 0; x < df->width; x++) {
    //        draw_entities_2d_at(g, df, true, (vec3){x, y, z});
    //    }
    //}

    // living
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            //draw_entities_2d_at(g, df, false, (vec3){x, y, z});

            shared_ptr<tile_t> tile = df_tile_at(df, (vec3){x, y, z});
            if (!tile) {
                continue;
            }
            if (tile_is_wall(tile->type)) {
                continue;
            }
            if (!tile->visible) {
                continue; // Do not draw entities on invisible tiles
            }
            if (tile->is_empty) {
                continue;
            }

            // Get hero's vision distance and location
            //int vision_distance = g_get_vision_distance(g, g->hero_id);
            //int light_dist = g_get_light_radius(g, g->hero_id) + g_get_entity_total_light_radius_bonus(g, g->hero_id);
            //int light_dist = 3;
            //vec3 hero_loc = g_get_location(g, g->hero_id);
            //int dist_to_check = MAX(vision_distance, light_dist);
            // Calculate Manhattan distance from hero to this tile (diamond pattern)
            //int distance = abs(loc.x - hero_loc.x) + abs(loc.y - hero_loc.y);
            // Only draw entities within vision distance
            //if (distance <= vision_distance) {
            //if (distance <= dist_to_check) {
            for (size_t i = 0; i < tile->entities->size(); i++) {
                entityid id = tile_get_entity(tile, i);
                draw_sprite_and_shadow(g, id);
            }
        }
    }

    return true;
}


void libdraw_drawframe_2d(shared_ptr<gamestate> g) {
    //BeginShaderMode(shader_color_noise);
    //float time = (float)GetTime(); // Current time in seconds
    //SetShaderValue(shader_color_noise, GetShaderLocation(shader_color_noise, "time"), &time, SHADER_UNIFORM_FLOAT);
    camera_lock_on(g);
    //minfo("Drawing frame");
    BeginMode2D(g->cam2d);
    ClearBackground(BLACK);
    //EndShaderMode();
    libdraw_draw_dungeon_floor(g);
    libdraw_draw_player_target_box(g);
    EndMode2D();
    //if (g->frame_dirty) {
    //draw_hud_to_texture(g);
    //} else {
    //draw_hud_from_texture(g);
    //}
    draw_hud(g);
    draw_message_history(g);
    draw_message_box(g);
    if (g->display_inventory_menu) {
        draw_inventory_menu(g);
    }
    //else if (g->display_gameplay_settings_menu) {
    //    draw_gameplay_settings_menu(g);
    //}
    handle_debug_panel(g);
    //draw_version(g);
    //int x = 0;
    if (g->display_help_menu) {
        draw_help_menu(g);
    }
    //if (g->gameover) {
    //    draw_gameover_menu(g);
    //}
}


void libdraw_drawframe_2d_to_texture(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    BeginTextureMode(main_game_target_texture);
    libdraw_drawframe_2d(g);
    EndTextureMode();
}


void libdraw_drawframe_2d_from_texture(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    DrawTexturePro(main_game_target_texture.texture, target_src, target_dest, target_origin, 0.0f, WHITE);
}


void draw_message_box(shared_ptr<gamestate> g) {
    if (!g->msg_system_is_active || g->msg_system->size() == 0) {
        return;
    }
    const string msg = g->msg_system->at(0);
    const Color message_bg = (Color){0x33, 0x33, 0x33, 0xff};
    const int font_size = 10;
    char tmp[1024] = {0};
    snprintf(tmp, sizeof(tmp), "%s", msg.c_str());
    const int measure = MeasureText(tmp, font_size);
    const float text_width = measure;
    const float text_height = font_size;
    const int w = DEFAULT_TARGET_WIDTH;
    const float x = (w - text_width) / 2.0 - g->pad;
    const float y = (DEFAULT_TARGET_HEIGHT - text_height) / 2.0 - g->pad;
    const Rectangle box = {x, y, text_width + g->pad * 2, text_height + g->pad * 2};

    DrawRectangleRec(box, message_bg);
    DrawRectangleLinesEx(box, 1, WHITE);
    DrawText(tmp, box.x + g->pad, box.y + g->pad, font_size, WHITE);
}


void libdraw_drawframe(shared_ptr<gamestate> g) {
    minfo2("Begin draw frame");
    double start_time = GetTime();
    libdraw_update_sprites_pre(g);
    BeginDrawing();
    ClearBackground(RED);
    //BeginShaderMode(shader_psychedelic_0);
    //float time = (float)GetTime(); // Current time in seconds
    //SetShaderValue(shader_psychedelic_0, GetShaderLocation(shader_psychedelic_0, "time"), &time, SHADER_UNIFORM_FLOAT);
    //EndShaderMode();
    if (g->frame_dirty) {
        minfo2("handling dirty frame");
        if (g->current_scene == SCENE_TITLE) {
            draw_title_screen_to_texture(g, false);
        } else if (g->current_scene == SCENE_MAIN_MENU) {
            draw_title_screen_to_texture(g, true);
        } else if (g->current_scene == SCENE_CHARACTER_CREATION) {
            draw_character_creation_screen_to_texture(g);
        } else if (g->current_scene == SCENE_GAMEPLAY) {
            libdraw_drawframe_2d_to_texture(g);
        }
        g->frame_dirty = false;
        g->frame_updates++;
    }
    // draw to the target texture
    minfo2("drawing frame to target texture");
    BeginTextureMode(target);
    ClearBackground(BLUE);
    if (g->current_scene == SCENE_TITLE) {
        draw_title_screen_from_texture(g);
    } else if (g->current_scene == SCENE_MAIN_MENU) {
        draw_title_screen_from_texture(g);
    } else if (g->current_scene == SCENE_CHARACTER_CREATION) {
        draw_character_creation_screen_from_texture(g);
    } else if (g->current_scene == SCENE_GAMEPLAY) {
        libdraw_drawframe_2d_from_texture(g);
    }
    //DrawFPS(0, DEFAULT_TARGET_HEIGHT - 20);
    //handle_debug_panel(g);
    EndTextureMode();
    // draw the target texture to the window
    win_dest.width = GetScreenWidth();
    win_dest.height = GetScreenHeight();
    DrawTexturePro(target.texture, target_src, win_dest, target_origin, 0.0f, WHITE);
    EndDrawing();
    g->last_frame_time = GetTime() - start_time;
    g->framecount++;
    libdraw_update_sprites_post(g);
    msuccess2("End draw frame");
}


void libdraw_close_partial() {
    UnloadMusicStream(music);
    CloseAudioDevice();
    libdraw_unload_textures(txinfo);
    libdraw_unload_shaders();

    UnloadRenderTexture(title_target_texture);
    UnloadRenderTexture(char_creation_target_texture);
    UnloadRenderTexture(main_game_target_texture);
    UnloadRenderTexture(hud_target_texture);
    UnloadRenderTexture(target);
}


void libdraw_close() {
    libdraw_close_partial();
    CloseWindow();
}


bool create_spritegroup(shared_ptr<gamestate> g, textureinfo* txinfo, entityid id, int* keys, int num_keys, int offset_x, int offset_y) {
    massert(g, "gamestate is NULL");
    massert(txinfo, "txinfo is null");
    // can hold up to 32 sprites
    spritegroup_t* group = spritegroup_create(SPRITEGROUP_DEFAULT_SIZE);
    massert(group, "spritegroup is NULL");
    //disabling this check until dungeon_floor created
    shared_ptr<dungeon_floor_t> df = d_get_current_floor(g->dungeon);
    massert(df, "dungeon_floor is NULL");
    if (!df) {
        spritegroup_destroy(group);
        return false;
    }
    int df_w = df->width;
    int df_h = df->height;
    //vec3 loc = g_get_loc(g, id);
    vec3 loc = g->ct.get<location>(id).value();
    massert(loc.x >= 0 && loc.x < df_w, "location x out of bounds: %d", loc.x);
    massert(loc.y >= 0 && loc.y < df_h, "location y out of bounds: %d", loc.y);
    if (loc.x < 0 || loc.x >= df_w || loc.y < 0 || loc.y >= df_h) {
        spritegroup_destroy(group);
        return false;
    }
    //minfo("Creating spritegroup for entity %d at loc %d,%d", id, (int)loc.x, (int)loc.y);
    for (int i = 0; i < num_keys; i++) {
        //minfo("loading sprite for key %d", keys[i]);
        int k = keys[i];
        Texture2D* tex = &txinfo[k].texture;
        //sprite* s = sprite_create(tex, txinfo[k].contexts, txinfo[k].num_frames);
        //minfo("creating sprite...");
        shared_ptr<sprite> s = sprite_create2(tex, txinfo[k].contexts, txinfo[k].num_frames);
        //msuccess("created sprite!");
        // as expected lol
        // easy fix
        // boom, fixed
        //minfo("adding sprite to group...");
        spritegroup_add(group, s);
        //msuccess("added sprite to group!");
    }
    //msuccess("Created spritegroup with sprites");
    group->id = id;
    //sprite* s = spritegroup_get(group, 0);
    shared_ptr<sprite> s = spritegroup_get(group, 0);
    massert(s, "sprite is NULL");
    if (!s) {
        spritegroup_destroy(group);
        return false;
    }
    group->current = 0;
    group->dest = (Rectangle){(float)loc.x * DEFAULT_TILE_SIZE + offset_x, (float)loc.y * DEFAULT_TILE_SIZE + offset_y, (float)s->width, (float)s->height};
    group->off_x = offset_x;
    group->off_y = offset_y;
    //hashtable_entityid_spritegroup_insert(spritegroups, id, group);
    // how its done in the future...
    spritegroups[id] = group;
    return true;
}


void create_sg_byid(shared_ptr<gamestate> g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");

    //entitytype_t type = g_get_type(g, id);
    entitytype_t type = g->ct.get<entitytype>(id).value_or(ENTITY_NONE);
    massert(type != ENTITY_NONE, "entity type is none");

    if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
        //race_t race = g_get_race(g, id);
        race_t r = g->ct.get<race>(id).value_or(RACE_NONE);

        switch (r) {
        case RACE_HUMAN: create_spritegroup(g, id, TX_HUMAN_KEYS, TX_HUMAN_COUNT, -12, -12); break;
        case RACE_ORC: create_spritegroup(g, id, TX_ORC_KEYS, TX_ORC_COUNT, -12, -12); break;
        case RACE_ELF: create_spritegroup(g, id, TX_ELF_KEYS, TX_ELF_COUNT, -12, -12); break;
        case RACE_DWARF: create_spritegroup(g, id, TX_DWARF_KEYS, TX_DWARF_COUNT, -12, -12); break;
        case RACE_HALFLING: create_spritegroup(g, id, TX_HALFLING_KEYS, TX_HALFLING_COUNT, -12, -12); break;
        case RACE_GOBLIN: create_spritegroup(g, id, TX_GOBLIN_KEYS, TX_GOBLIN_COUNT, -12, -12); break;
        case RACE_WOLF: create_spritegroup(g, id, TX_WOLF_KEYS, TX_WOLF_COUNT, -12, -12); break;
        case RACE_BAT: create_spritegroup(g, id, TX_BAT_KEYS, TX_BAT_COUNT, -12, -12); break;
        case RACE_WARG: create_spritegroup(g, id, TX_WARG_KEYS, TX_WARG_COUNT, -12, -12); break;
        case RACE_GREEN_SLIME: create_spritegroup(g, id, TX_GREEN_SLIME_KEYS, TX_GREEN_SLIME_COUNT, -12, -12); break;
        default: merror("unknown race %d", r); return;
        }
    } else if (type == ENTITY_WOODEN_BOX) {
        create_spritegroup(g, id, TX_WOODEN_BOX_KEYS, TX_WOODEN_BOX_COUNT, -12, -12);
    } else if (type == ENTITY_ITEM) {
        //itemtype item_type = g_get_item_type(g, id);
        itemtype_t item_type = g->ct.get<itemtype>(id).value_or(ITEM_NONE);

        if (item_type == ITEM_POTION) {
            //potiontype_t potion_type = g_get_potion_type(g, id);
            potiontype_t potion_type = g->ct.get<potiontype>(id).value_or(POTION_NONE);

            switch (potion_type) {
            case POTION_HP_SMALL: create_spritegroup(g, id, TX_POTION_HP_SMALL_KEYS, TX_POTION_HP_SMALL_COUNT, -12, -12); break;
            case POTION_HP_MEDIUM: create_spritegroup(g, id, TX_POTION_HP_MEDIUM_KEYS, TX_POTION_HP_MEDIUM_COUNT, -12, -12); break;
            case POTION_HP_LARGE: create_spritegroup(g, id, TX_POTION_HP_LARGE_KEYS, TX_POTION_HP_LARGE_COUNT, -12, -12); break;
            case POTION_MP_SMALL: create_spritegroup(g, id, TX_POTION_MP_SMALL_KEYS, TX_POTION_MP_SMALL_COUNT, -12, -12); break;
            case POTION_MP_MEDIUM: create_spritegroup(g, id, TX_POTION_MP_MEDIUM_KEYS, TX_POTION_MP_MEDIUM_COUNT, -12, -12); break;
            case POTION_MP_LARGE: create_spritegroup(g, id, TX_POTION_MP_LARGE_KEYS, TX_POTION_MP_LARGE_COUNT, -12, -12); break;
            default: merror("unknown potion type %d", potion_type); return;
            }
            return;
        } else if (item_type == ITEM_WEAPON) {
            //weapontype weapon_type = g_get_weapon_type(g, id);
            weapontype_t weapon_type = g->ct.get<weapontype>(id).value_or(WEAPON_NONE);

            switch (weapon_type) {
            case WEAPON_DAGGER: create_spritegroup(g, id, TX_DAGGER_KEYS, TX_DAGGER_COUNT, -12, -12); break;
            case WEAPON_SWORD: create_spritegroup(g, id, TX_SWORD_KEYS, TX_SWORD_COUNT, -12, -12); break;
            case WEAPON_AXE: create_spritegroup(g, id, TX_AXE_KEYS, TX_AXE_COUNT, -12, -12); break;
            case WEAPON_BOW: create_spritegroup(g, id, TX_BOW_KEYS, TX_BOW_COUNT, -12, -12); break;
            case WEAPON_TWO_HANDED_SWORD: create_spritegroup(g, id, TX_TWO_HANDED_SWORD_KEYS, TX_TWO_HANDED_SWORD_COUNT, -12, -12); break;
            case WEAPON_WARHAMMER: create_spritegroup(g, id, TX_WARHAMMER_KEYS, TX_WARHAMMER_COUNT, -12, -12); break;
            case WEAPON_FLAIL: create_spritegroup(g, id, TX_WHIP_KEYS, TX_WHIP_COUNT, -12, -12); break;
            default: merror("unknown weapon type %d", weapon_type); return;
            }
            return;
        }
        //else if (item_type == ITEM_SHIELD) {
        //    create_spritegroup(g, id, TX_BUCKLER_KEYS, TX_BUCKLER_COUNT, -12, -12);
        //} else if (item_type == ITEM_WAND) {
        //    create_spritegroup(g, id, TX_WAND_BASIC_KEYS, TX_WAND_BASIC_COUNT, -12, -12);
        //} else if (item_type == ITEM_RING) {
        //    ringtype ring_type = g_get_ring_type(g, id);
        //    switch (ring_type) {
        //    case RING_GOLD: create_spritegroup(g, id, TX_GOLD_RING_KEYS, TX_GOLD_RING_COUNT, -12, -12); break;
        //    case RING_SILVER: create_spritegroup(g, id, TX_SILVER_RING_KEYS, TX_SILVER_RING_COUNT, -12, -12); break;
        //    default: merror("unknown ring type %d", ring_type); return;
        //    }
        //} else {
        //    merror("unknown item type %d", item_type);
    }
}


void draw_hud_to_texture(shared_ptr<gamestate> g) {
    BeginTextureMode(hud_target_texture);
    draw_hud(g);
    EndTextureMode();
}

void draw_hud_from_texture(shared_ptr<gamestate> g) { DrawTexturePro(hud_target_texture.texture, target_src, target_dest, (Vector2){0, 0}, 0.0f, WHITE); }


void libdraw_init_rest(shared_ptr<gamestate> g) {
    //minfo("libdraw_init_rest: initializing rest of the libdraw");
    SetExitKey(KEY_ESCAPE);
    SetTargetFPS(60);
    int w = DEFAULT_WIN_WIDTH;
    int h = DEFAULT_WIN_HEIGHT;
    int target_w = DEFAULT_TARGET_WIDTH;
    int target_h = DEFAULT_TARGET_HEIGHT;
    //minfo("libdraw_init_rest: window size: %dx%d", w, h);
    massert(w > 0 && h > 0, "window width or height is not set properly");
    g->windowwidth = w;
    g->windowheight = h;
    g->targetwidth = target_w;
    g->targetheight = target_h;
    TextureFilter filter = TEXTURE_FILTER_POINT; // Use trilinear filtering for better quality
    //TextureFilter filter = TEXTURE_FILTER_BILINEAR; // Use trilinear filtering for better quality
    //TextureFilter filter = TEXTURE_FILTER_TRILINEAR; // Use trilinear filtering for better quality
    //TextureFilter filter = TEXTURE_FILTER_ANISOTROPIC_16X;
    target = LoadRenderTexture(target_w, target_h);
    //SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(target.texture, filter);
    // Use anisotropic filtering for better quality
    //target = LoadRenderTexture(w, h);

    title_target_texture = LoadRenderTexture(target_w, target_h);
    SetTextureFilter(title_target_texture.texture, filter);

    char_creation_target_texture = LoadRenderTexture(target_w, target_h);
    SetTextureFilter(char_creation_target_texture.texture, filter);

    main_game_target_texture = LoadRenderTexture(target_w, target_h);
    SetTextureFilter(main_game_target_texture.texture, filter);

    hud_target_texture = LoadRenderTexture(target_w, target_h);
    SetTextureFilter(hud_target_texture.texture, filter);

    target_src = (Rectangle){0, 0, target_w * 1.0f, -target_h * 1.0f};
    target_dest = (Rectangle){0, 0, target_w * 1.0f, target_h * 1.0f};
    //target_dest = (Rectangle){0, 0, w, h};
    //spritegroups = hashtable_entityid_spritegroup_create(DEFAULT_SPRITEGROUPS_SIZE);
    load_textures(txinfo);
    //calc_debugpanel_size(g);
    load_shaders();
    //float x = target_w / 2.0f - DEFAULT_TILE_SIZE * 4;
    const float x = target_w / 4.0f;
    //float y = target_h / 16.0f;
    //float y = DEFAULT_TILE_SIZE * 2;
    const float y = target_h / 4.0f;
    //float x = DEFAULT_TILE_SIZE * 4;
    //float y = DEFAULT_TILE_SIZE * 8;
    //float x = 0;
    //float y = 0;
    g->cam2d.offset = (Vector2){x, y};
    //g->cam2d.zoom = 1.0f;
    //gamestate_set_debug_panel_pos_top_right(g);
    // set the camera target to the center of the dungeon
    //dungeon_floor_t* const df = d_get_current_floor(g->d);
    //massert(df, "dungeon_floor is NULL");
    //int df_w = df->width;
    //int df_h = df->height;
    //g->cam2d.target = (Vector2){df_w * DEFAULT_TILE_SIZE / 2.0f,
    //                            df_h * DEFAULT_TILE_SIZE / 2.0f};
    draw_title_screen_to_texture(g, false);
    draw_character_creation_screen_to_texture(g);

    InitAudioDevice();
    libdraw_load_music(g);
    libdraw_load_sfx(g);

    //if (!camera_lock_on(g)) merror("failed to lock camera on hero");
    //msuccess("libdraw_init_rest: done");
}


void libdraw_init(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    int w = DEFAULT_WIN_WIDTH;
    int h = DEFAULT_WIN_HEIGHT;
    const char* title = WINDOW_TITLE;
    char full_title[1024] = {0};
    snprintf(full_title, sizeof(full_title), "%s - %s", title, g->version.c_str());
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    InitWindow(w, h, full_title);
    g->windowwidth = w;
    g->windowheight = h;

    SetWindowMinSize(320, 240);
    libdraw_init_rest(g);
}


bool libdraw_windowshouldclose(const shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    return g->do_quit;
}


void draw_title_screen_to_texture(shared_ptr<gamestate> g, bool show_menu) {
    massert(g, "gamestate is NULL");
    BeginTextureMode(title_target_texture);
    draw_title_screen(g, show_menu);
    handle_debug_panel(g);
    EndTextureMode();
}


void draw_title_screen_from_texture(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    DrawTexturePro(title_target_texture.texture, target_src, target_dest, (Vector2){0, 0}, 0.0f, WHITE);
}


void draw_character_creation_screen_to_texture(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    BeginTextureMode(char_creation_target_texture);
    draw_character_creation_screen(g);
    handle_debug_panel(g);
    EndTextureMode();
}


void draw_character_creation_screen_from_texture(shared_ptr<gamestate> g) {
    massert(g, "gamestate is NULL");
    DrawTexturePro(char_creation_target_texture.texture, target_src, target_dest, (Vector2){0, 0}, 0.0f, WHITE);
}
