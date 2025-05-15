#include "component.h"
#include "direction.h"
#include "dungeon_tile_type.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate.h"
#include "gamestate_flag.h"
#include "get_txkey_for_tiletype.h"
#include "hashtable_entityid_spritegroup.h"
#include "libdraw.h"
#include "libgame_defines.h"
#include "massert.h"
#include "mprint.h"
#include "race.h"
#include "rlgl.h"
#include "specifier.h"
#include "sprite.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"
#include "textureinfo.h"
#include "tx_keys.h"
#include <raylib.h>

#define DEFAULT_SPRITEGROUPS_SIZE 128
//#define DEFAULT_WIN_WIDTH 800
//#define DEFAULT_WIN_HEIGHT 480
//#define DEFAULT_WIN_WIDTH 960
//#define DEFAULT_WIN_HEIGHT 540

#define DEFAULT_WIN_WIDTH 1920
#define DEFAULT_WIN_HEIGHT 1080

//#define DEFAULT_WIN_WIDTH 1280
//#define DEFAULT_WIN_HEIGHT 720

#define SPRITEGROUP_DEFAULT_SIZE 32
#define DEFAULT_TILE_SIZE_SCALED 32

hashtable_entityid_spritegroup_t* spritegroups = NULL;
textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

Shader shader_grayscale = {0};
Shader shader_glow = {0};
Shader shader_red_glow = {0};
Shader shader_color_noise = {0};
Shader shader_psychedelic_0 = {0};

RenderTexture2D target = {0};
Rectangle target_src = {0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};
Rectangle target_dest = {0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};
Vector2 target_origin = {0, 0};
Vector2 zero_vec = {0, 0};

int ANIM_SPEED = DEFAULT_ANIM_SPEED;

static inline bool libdraw_camera_lock_on(gamestate* const g);
static inline void update_debug_panel(gamestate* const g);
static inline void handle_debug_panel(gamestate* const g);
static void libdraw_handle_gamestate_flag(gamestate* const g);

static bool load_texture(int txkey, int ctxs, int frames, bool do_dither, char* path);

static bool libdraw_check_default_animations(const gamestate* const g);

static void update_weapon_for_entity(gamestate* g, entityid id, spritegroup_t* sg);

//static sprite* get_weapon_back_sprite(const gamestate* g, const entity* e, spritegroup_t* sg);
static sprite* get_weapon_back_sprite(const gamestate* g, entityid id, spritegroup_t* sg);
//static sprite* get_weapon_front_sprite(const gamestate* g, const entity* e, spritegroup_t* sg);
static sprite* get_weapon_front_sprite(const gamestate* g, entityid id, spritegroup_t* sg);
//static sprite* get_shield_front_sprite(const gamestate* g, const entity* e, spritegroup_t* sg);
static sprite* get_shield_front_sprite(const gamestate* g, entityid id, spritegroup_t* sg);
//static sprite* get_shield_back_sprite(const gamestate* g, const entity* e, spritegroup_t* sg);
static sprite* get_shield_back_sprite(const gamestate* g, entityid id, spritegroup_t* sg);
static void draw_inventory_menu(gamestate* const g);
static void draw_hud(gamestate* const g);
static bool libdraw_unload_texture(int txkey);
//static bool draw_dungeon_floor_tile(const gamestate* const g, dungeon_floor_t* const df, int x, int y);
static bool draw_dungeon_floor_tile(const gamestate* const g, int x, int y, int z);
//static bool draw_dungeon_tiles_2d(const gamestate* const g, dungeon_floor_t* df);
static bool draw_dungeon_tiles_2d(const gamestate* const g, int z, dungeon_floor_t* df);
//static bool draw_entities_2d(const gamestate* const g, dungeon_floor_t* df, bool dead);
static bool draw_entities_2d(const gamestate* const g, int z, dungeon_floor_t* df, bool dead);
//static bool draw_entities_2d_at(const gamestate* const g, dungeon_floor_t* const df, bool dead, int x, int y);
//static bool draw_entities_2d_at(const gamestate* const g, dungeon_floor_t* const df, bool dead, int x, int y, int z);
static bool draw_entities_2d_at(const gamestate* const g, dungeon_floor_t* const df, bool dead, loc_t loc);
static bool libdraw_draw_dungeon_floor(const gamestate* const g);
static bool libdraw_draw_player_target_box(const gamestate* const g);

//static void libdraw_set_sg_door(gamestate* const g, entity_t* const e, spritegroup_t* const sg);
//static void libdraw_set_sg_is_damaged(gamestate* const g, entity_t* const e, spritegroup_t* const sg);
static void libdraw_set_sg_is_damaged(gamestate* const g, entityid id, spritegroup_t* const sg);
//static void libdraw_set_sg_is_dead(gamestate* const g, entity_t* const e, spritegroup_t* const sg);
static void libdraw_set_sg_is_dead(gamestate* const g, entityid id, spritegroup_t* const sg);
//static void libdraw_set_sg_is_attacking(gamestate* const g, entity_t* const e, spritegroup_t* const sg);
static void libdraw_set_sg_is_attacking(gamestate* const g, entityid id, spritegroup_t* const sg);
//static void libdraw_set_sg_is_blocking(gamestate* const g, entity_t* const e, spritegroup_t* const sg);
static void libdraw_set_sg_is_blocking(gamestate* const g, entityid id, spritegroup_t* const sg);
//static void libdraw_set_sg_block_success(gamestate* const g, entity_t* const e, spritegroup_t* const sg);
static void libdraw_set_sg_block_success(gamestate* const g, entityid id, spritegroup_t* const sg);
static void load_shaders();
static void libdraw_unload_shaders();
static void load_textures();
static void libdraw_unload_textures();
//static void libdraw_update_sprite_attack(gamestate* const g, entity_t* e, spritegroup_t* sg);
static void libdraw_update_sprite_attack(gamestate* const g, entityid id, spritegroup_t* sg);
//static void libdraw_update_sprite_position(gamestate* const g, spritegroup_t* sg, entity_t* e);
//static void libdraw_update_sprite_position(gamestate* const g, spritegroup_t* sg, entityid id);
static void libdraw_update_sprite_position(gamestate* const g, entityid id, spritegroup_t* sg);
static void libdraw_update_sprite_context_ptr(gamestate* const g, spritegroup_t* group, direction_t dir);
//static void libdraw_update_sprite_ptr(gamestate* const g, entity* e, spritegroup_t* sg);
static void libdraw_update_sprite_ptr(gamestate* const g, entityid id, spritegroup_t* sg);
static void libdraw_update_sprite(gamestate* const g, entityid id);
static void libdraw_handle_frame_incr(gamestate* const g, entityid id, spritegroup_t* const sg);
static void draw_message_history(gamestate* const g);
static void draw_message_box(gamestate* g);
static void draw_sprite_and_shadow(const gamestate* const g, entityid id);
static void libdraw_draw_debug_panel(gamestate* const g);
static void libdraw_drawframe_2d(gamestate* const g);
static void create_sg_byid(gamestate* const g, entityid id);
static void calc_debugpanel_size(gamestate* const g);
static void create_spritegroup(gamestate* const g, entityid id, int* keys, int num_keys, int offset_x, int offset_y, specifier_t spec);
//static void draw_shadow_for_entity(const gamestate* const g, spritegroup_t* sg, const entity* e);
static void draw_shadow_for_entity(const gamestate* const g, spritegroup_t* sg, entityid id);

//static bool draw_dungeon_floor_tile(const gamestate* const g, dungeon_floor_t* const df, int x, int y) {
static bool draw_dungeon_floor_tile(const gamestate* const g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    //massert(df, "dungeon_floor is NULL");
    massert(x >= 0, "x is less than 0");
    massert(y >= 0, "y is less than 0");
    dungeon_floor_t* df = d_get_floor(g->d, z);
    massert(x < df->width, "x is out of bounds");
    massert(y < df->height, "y is out of bounds");
    massert(df, "dungeon_floor is NULL");
    tile_t* tile = df_tile_at(df, (loc_t){x, y, z});
    massert(tile, "tile is NULL");
    // check if the tile type is none
    if (tile->type == TILE_NONE) return true;
    // just draw the tile itself
    // tile values in get_txkey_for_tiletype.h
    int txkey = get_txkey_for_tiletype(tile->type);
    if (txkey < 0) return false;

    Texture2D* texture = &txinfo[txkey].texture;
    if (texture->id <= 0) return false;

    const int offset_x = -12;
    const int offset_y = -12;
    const int dx = x * DEFAULT_TILE_SIZE + offset_x;
    const int dy = y * DEFAULT_TILE_SIZE + offset_y;

    const Rectangle src = (Rectangle){0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
    const Rectangle dest = (Rectangle){dx, dy, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};

    DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);

    if (tile->has_pressure_plate) {
        const int txkey2 = tile->pressure_plate_up_tx_key;
        if (txkey2 < 0) return false;
        Texture2D* texture = &txinfo[txkey2].texture;
        if (texture->id <= 0) return false;
        DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
    }

    if (tile->has_wall_switch) {
        const int txkey = tile->wall_switch_on ? tile->wall_switch_down_tx_key : tile->wall_switch_up_tx_key;
        if (txkey < 0) return false;
        Texture2D* texture = &txinfo[txkey].texture;
        if (texture->id <= 0) return false;
        DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
    }

    return true;
}

static bool draw_dungeon_tiles_2d(const gamestate* const g, int z, dungeon_floor_t* df) {
    //static bool draw_dungeon_tiles_2d(const gamestate* const g, int z) {
    massert(g, "gamestate is NULL");
    //massert(z >= 0, "z is less than 0");
    //massert(z < g->d->num_floors, "z is out of bounds");
    //dungeon_floor_t* df = d_get_floor(g->d, z);
    massert(df, "dungeon_floor is NULL");
    //massert(df->width > 0, "dungeon_floor width is less than 0");
    //massert(df->height > 0, "dungeon_floor height is less than 0");
    //massert(df, "dungeon_floor is NULL");
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            //if (df_tile_is_wall(df, x, y)) continue;
            draw_dungeon_floor_tile(g, x, y, z);
        }
    }
    return true;
}

static sprite* get_weapon_front_sprite(const gamestate* g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(sg, "spritegroup is NULL");
    entityid weapon = g_get_equipment(g, id, EQUIP_SLOT_WEAPON);
    if (weapon == ENTITYID_INVALID) return NULL;
    spritegroup_t* w_sg = hashtable_entityid_spritegroup_get(spritegroups, weapon);
    if (!w_sg) {
        merror("weapon spritegroup is NULL for id %d", weapon);
        return NULL;
    }
    if (sg->current == SPRITEGROUP_ANIM_HUMAN_ATTACK) {
        return spritegroup_get(w_sg, SG_ANIM_LONGSWORD_SLASH_F);
    }
    return NULL;
}

//static sprite* get_weapon_back_sprite(const gamestate* g, const entity* e, spritegroup_t* sg) {
static sprite* get_weapon_back_sprite(const gamestate* g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(sg, "spritegroup is NULL");
    entityid weapon = g_get_equipment(g, id, EQUIP_SLOT_WEAPON);
    if (weapon == ENTITYID_INVALID) return NULL;
    spritegroup_t* w_sg = hashtable_entityid_spritegroup_get(spritegroups, weapon);
    if (!w_sg) {
        merror("weapon spritegroup is NULL for id %d", weapon);
        return NULL;
    }
    if (sg->current == SPRITEGROUP_ANIM_HUMAN_ATTACK) {
        return spritegroup_get(w_sg, SG_ANIM_LONGSWORD_SLASH_B);
    }
    return NULL;
}

//static sprite* get_shield_front_sprite(const gamestate* g, const entity* e, spritegroup_t* sg) {
static sprite* get_shield_front_sprite(const gamestate* g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(sg, "spritegroup is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");

    entityid shield = g_get_equipment(g, id, EQUIP_SLOT_SHIELD);
    if (shield == ENTITYID_INVALID) return NULL;
    spritegroup_t* s_sg = hashtable_entityid_spritegroup_get(spritegroups, shield);
    if (!s_sg) return NULL;
    if (sg->current == SPRITEGROUP_ANIM_HUMAN_GUARD_SUCCESS || sg->current == SPRITEGROUP_ANIM_ORC_GUARD_SUCCESS) {
        return spritegroup_get(s_sg, SG_ANIM_BUCKLER_SUCCESS_FRONT);
    }
    //if (e->is_blocking) { return spritegroup_get(shield_sg, SG_ANIM_BUCKLER_FRONT); }
    //if (g_get_blocking(g, e->id)) { return spritegroup_get(shield_sg, SG_ANIM_BUCKLER_FRONT); }
    return NULL;
}

//static sprite* get_shield_back_sprite(const gamestate* g, const entity* e, spritegroup_t* sg) {
static sprite* get_shield_back_sprite(const gamestate* g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(sg, "spritegroup is NULL");
    entityid shield = g_get_equipment(g, id, EQUIP_SLOT_SHIELD);
    if (shield == ENTITYID_INVALID) return NULL;
    spritegroup_t* s_sg = hashtable_entityid_spritegroup_get(spritegroups, shield);
    if (!s_sg) return NULL;
    if (sg->current == SPRITEGROUP_ANIM_HUMAN_GUARD_SUCCESS || sg->current == SPRITEGROUP_ANIM_ORC_GUARD_SUCCESS) {
        return spritegroup_get(s_sg, SG_ANIM_BUCKLER_SUCCESS_BACK);
    }
    //if (e->shield == ENTITYID_INVALID) return NULL;
    //spritegroup_t* shield_sg = hashtable_entityid_spritegroup_get(spritegroups, e->shield);
    //if (!shield_sg) return NULL;
    //if (sg->current == SPRITEGROUP_ANIM_HUMAN_GUARD_SUCCESS || sg->current == SPRITEGROUP_ANIM_ORC_GUARD_SUCCESS) {
    //    return spritegroup_get(shield_sg, SG_ANIM_BUCKLER_SUCCESS_BACK);
    //}
    //if (e->is_blocking) { return spritegroup_get(shield_sg, SG_ANIM_BUCKLER_BACK); }
    //if (g_get_blocking(g, e->id)) { return spritegroup_get(shield_sg, SG_ANIM_BUCKLER_BACK); }
    return NULL;
}

//static void draw_shadow_for_entity(const gamestate* const g, spritegroup_t* sg, const entity* e) {
static void draw_shadow_for_entity(const gamestate* const g, spritegroup_t* sg, entityid id) {
    massert(g, "gamestate is NULL");
    massert(sg, "spritegroup is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    //if (!sg || !e) return;
    entitytype_t type = g_get_type(g, id);
    if (type != ENTITY_PLAYER && type != ENTITY_NPC) return;

    sprite* shadow = sg_get_current_plus_one(sg);
    if (shadow) {
        Rectangle dest = {sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};
        DrawTexturePro(*shadow->texture, shadow->src, dest, (Vector2){0, 0}, 0, WHITE);
    }
}

static void draw_sprite_and_shadow(const gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(spritegroups, id);
    //massert(sg, "spritegroup is NULL: id %d name %s", id, e->name);
    //minfo("draw_sprite_and_shadow: id %d", id);
    sprite* s = sg_get_current(sg);
    massert(s, "sprite is NULL");
    Rectangle dest = {sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};
    draw_shadow_for_entity(g, sg, id);
    // check for a shield
    sprite* shield_front_s = get_shield_front_sprite(g, id, sg);
    sprite* shield_back_s = get_shield_back_sprite(g, id, sg);
    // check for a weapon
    sprite* weapon_front_s = get_weapon_front_sprite(g, id, sg);
    sprite* weapon_back_s = get_weapon_back_sprite(g, id, sg);
    if (shield_back_s) DrawTexturePro(*shield_back_s->texture, shield_back_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
    if (weapon_back_s) {
        //msuccess("weapon_back_s");
        DrawTexturePro(*weapon_back_s->texture, weapon_back_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
    }

    // Draw sprite on top
    //DrawTexturePro(*s->texture, s->src, dest, zero_vec, 0, WHITE);
    DrawTexturePro(*s->texture, s->src, dest, zero_vec, 0, (Color){255, 255, 255, 255});

    if (shield_front_s) DrawTexturePro(*shield_front_s->texture, shield_front_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
    if (weapon_front_s) {
        //msuccess("weapon_front_s");
        DrawTexturePro(*weapon_front_s->texture, weapon_front_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
    }
    //msuccess("draw_sprite_and_shadow: id %d", id);
}

static bool draw_entities_2d_at(const gamestate* const g, dungeon_floor_t* const df, bool dead, loc_t loc) {
    massert(g, "draw_entities_2d: gamestate is NULL");
    massert(df, "draw_entities_2d: dungeon_floor is NULL");
    massert(df->width > 0, "draw_entities_2d: dungeon_floor width is 0");
    massert(df->height > 0, "draw_entities_2d: dungeon_floor height is 0");
    massert(df->width <= DEFAULT_DUNGEON_FLOOR_WIDTH, "draw_entities_2d: dungeon_floor width is too large");
    massert(df->height <= DEFAULT_DUNGEON_FLOOR_HEIGHT, "draw_entities_2d: dungeon_floor height is too large");
    massert(loc.x >= 0, "draw_entities_2d: x is out of bounds");
    massert(loc.x < df->width, "draw_entities_2d: x is out of bounds");
    massert(loc.y >= 0, "draw_entities_2d: y is out of bounds");
    massert(loc.y < df->height, "draw_entities_2d: y is out of bounds");
    tile_t* tile = df_tile_at(df, loc);
    if (!tile) return false;
    if (tile_is_wall(tile->type)) return false;
    for (int i = 0; i < tile_entity_count(tile); i++) {
        entityid id = tile_get_entity(tile, i);
        //minfo("draw_entities_2d_at: id %d", id);
        if (g_is_dead(g, id) == dead) {
            draw_sprite_and_shadow(g, id);
        }
    }
    //msuccess("draw_entities_2d_at: x %d y %d", x, y);
    return true;
}

static bool draw_entities_2d(const gamestate* const g, int z, dungeon_floor_t* df, bool dead) {
    massert(g, "draw_entities_2d: gamestate is NULL");
    massert(df, "draw_entities_2d: dungeon_floor is NULL");
    massert(df->width > 0, "draw_entities_2d: dungeon_floor width is 0");
    massert(df->height > 0, "draw_entities_2d: dungeon_floor height is 0");
    massert(df->width <= DEFAULT_DUNGEON_FLOOR_WIDTH, "draw_entities_2d: dungeon_floor width is too large");
    massert(df->height <= DEFAULT_DUNGEON_FLOOR_HEIGHT, "draw_entities_2d: dungeon_floor height is too large");
    for (int y = 0; y < df->height; y++)
        for (int x = 0; x < df->width; x++) draw_entities_2d_at(g, df, dead, (loc_t){x, y, z});
    return true;
}

//static bool draw_wall_tiles_2d(const gamestate* g, dungeon_floor_t* df) {
//    massert(g, "draw_wall_tiles_2d: gamestate is NULL");
//    massert(df, "draw_wall_tiles_2d: dungeon_floor is NULL");
//    for (int y = 0; y < df->height; y++)
//        for (int x = 0; x < df->width; x++) {
//            tile_t* t = df_tile_at(df, x, y);
//            if (!t) return false;
//            if (!tile_is_wall(t->type)) continue;
//            draw_dungeon_floor_tile(g, df, x, y);
//        }
//    return true;
//}

static void load_shaders() {
    shader_grayscale = LoadShader(0, "grayscale.frag"); // No vertex shader needed
    shader_glow = LoadShader(0, "glow.frag");
    shader_red_glow = LoadShader(0, "red-glow.frag");
    shader_color_noise = LoadShader(0, "colornoise.frag");
    shader_psychedelic_0 = LoadShader(0, "psychedelic-0.frag");

    //shader_tile_glow = LoadShader(0, "psychedelic_ripple.frag");
}

static void libdraw_unload_shaders() {
    UnloadShader(shader_grayscale);
    UnloadShader(shader_glow);
    UnloadShader(shader_red_glow);
    UnloadShader(shader_color_noise);
    UnloadShader(shader_psychedelic_0);
}

static inline bool libdraw_camera_lock_on(gamestate* const g) {
    massert(g, "gamestate is NULL");
    if (!g->cam_lockon) {
        return false;
    }
    spritegroup_t* grp = hashtable_entityid_spritegroup_get(spritegroups, g->hero_id);
    massert(grp, "spritegroup is NULL");
    g->cam2d.target = (Vector2){grp->dest.x, grp->dest.y};
    return true;
}

static bool libdraw_check_default_animations(const gamestate* const g) {
    massert(g, "gamestate is NULL");
    for (entityid id = 0; id < g->next_entityid; id++) {
        //const entityid id = g->entityids[i];
        spritegroup_t* const sg = hashtable_entityid_spritegroup_get(spritegroups, id);
        if (sg && sg->current != sg->default_anim) {
            // which sg isnt done?
            return false;
        }
    }
    return true;
}

//static void libdraw_set_sg_is_damaged(gamestate* const g, entity_t* const e, spritegroup_t* const sg) {
static void libdraw_set_sg_is_damaged(gamestate* const g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");

    race_t race = g_get_race(g, id);

    if (race == RACE_HUMAN)
        spritegroup_set_current(sg, SPRITEGROUP_ANIM_HUMAN_DMG);
    else if (race == RACE_ORC)
        spritegroup_set_current(sg, SPRITEGROUP_ANIM_ORC_DMG);
    else if (race == RACE_ELF)
        spritegroup_set_current(sg, SPRITEGROUP_ANIM_ELF_DMG);
    else if (race == RACE_DWARF)
        spritegroup_set_current(sg, SPRITEGROUP_ANIM_DWARF_DMG);
    else if (race == RACE_HALFLING)
        spritegroup_set_current(sg, SPRITEGROUP_ANIM_HALFLING_DMG);
    else if (race == RACE_GOBLIN)
        spritegroup_set_current(sg, SPRITEGROUP_ANIM_GOBLIN_DMG);

    //e->is_damaged = false;
    g_set_damaged(g, id, false);
}

//static void libdraw_set_sg_is_dead(gamestate* const g, entity_t* const e, spritegroup_t* const sg) {
static void libdraw_set_sg_is_dead(gamestate* const g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");

    //if (!e->dead) return;
    //minfo("calling g_is_dead 1");
    if (!g_is_dead(g, id)) return;

    race_t race = g_get_race(g, id);

    if (race == RACE_HUMAN) {
        if (sg->current == SPRITEGROUP_ANIM_HUMAN_SPINDIE) return;
        sg_set_default_anim(sg, SPRITEGROUP_ANIM_HUMAN_SPINDIE);
        spritegroup_set_current(sg, sg->default_anim);
        spritegroup_set_stop_on_last_frame(sg, true);
    } else if (race == RACE_ORC) {
        if (sg->current == SPRITEGROUP_ANIM_ORC_DIE) return;
        sg_set_default_anim(sg, SPRITEGROUP_ANIM_ORC_DIE);
        spritegroup_set_current(sg, sg->default_anim);
        spritegroup_set_stop_on_last_frame(sg, true);
    } else if (race == RACE_ELF) {
        if (sg->current == SPRITEGROUP_ANIM_ELF_SPINDIE) return;
        sg_set_default_anim(sg, SPRITEGROUP_ANIM_ELF_SPINDIE);
        spritegroup_set_current(sg, sg->default_anim);
        spritegroup_set_stop_on_last_frame(sg, true);
    } else if (race == RACE_DWARF) {
        if (sg->current == SPRITEGROUP_ANIM_DWARF_SPINDIE) return;
        sg_set_default_anim(sg, SPRITEGROUP_ANIM_DWARF_SPINDIE);
        spritegroup_set_current(sg, sg->default_anim);
        spritegroup_set_stop_on_last_frame(sg, true);
    } else if (race == RACE_HALFLING) {
        if (sg->current == SPRITEGROUP_ANIM_HALFLING_SPINDIE) return;
        sg_set_default_anim(sg, SPRITEGROUP_ANIM_HALFLING_SPINDIE);
        spritegroup_set_current(sg, sg->default_anim);
        spritegroup_set_stop_on_last_frame(sg, true);
    } else if (race == RACE_GOBLIN) {
        if (sg->current == SPRITEGROUP_ANIM_GOBLIN_SPINDIE) return;
        sg_set_default_anim(sg, SPRITEGROUP_ANIM_GOBLIN_SPINDIE);
        spritegroup_set_current(sg, sg->default_anim);
        spritegroup_set_stop_on_last_frame(sg, true);
    }
}

static void update_weapon_for_entity(gamestate* g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");

    entityid weaponid = g_get_equipment(g, id, EQUIP_SLOT_WEAPON);
    if (weaponid == ENTITYID_INVALID) return;

    spritegroup_t* w_sg = hashtable_entityid_spritegroup_get(spritegroups, weaponid);
    if (!w_sg) return;

    int ctx = sg->sprites[sg->current]->currentcontext;
    spritegroup_setcontexts(w_sg, ctx);
    spritegroup_set_current(w_sg, SG_ANIM_LONGSWORD_SLASH_F);
}

//static void libdraw_set_sg_door(gamestate* const g, entity_t* const e, spritegroup_t* const sg) {
//static void libdraw_set_sg_door(gamestate* const g, entityid id, spritegroup_t* const sg) {
//    massert(g, "gamestate is NULL");
//    massert(id != ENTITYID_INVALID, "entity id is -1");
//    massert(sg, "spritegroup is NULL");
//if (e->type == ENTITY_DOOR && e->do_update) {
//if (g_is_type(g, e->id, ENTITY_DOOR) && e->do_update) {
//    if (g_is_type(g, id, ENTITY_DOOR) && g_get_update(g, id)) {
//if (e->type == ENTITY_DOOR) {
//if (e->door_is_open) {
//spritegroup_set_current(sg, 1);
//    sg_set_default_anim(sg, 1);
//spritegroup_set_current(sg, 1);
//e->do_update = false;
//    g_set_update(g, id, false);
//} else {
//spritegroup_set_current(sg, 0);
//    sg_set_default_anim(sg, 0);
//e->do_update = false;
//    g_set_update(g, id, false);
//}
//}
//}

//static void libdraw_set_sg_is_attacking(gamestate* const g, entity_t* const e, spritegroup_t* const sg) {
static void libdraw_set_sg_is_attacking(gamestate* const g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");

    race_t race = g_get_race(g, id);
    int cur = 0;

    if (race == RACE_HUMAN) {
        cur = SPRITEGROUP_ANIM_HUMAN_ATTACK;
    } else if (race == RACE_ORC) {
        cur = SPRITEGROUP_ANIM_ORC_ATTACK;
    } else if (race == RACE_ELF) {
        cur = SPRITEGROUP_ANIM_ELF_ATTACK;
    } else if (race == RACE_DWARF) {
        cur = SPRITEGROUP_ANIM_DWARF_ATTACK;
    } else if (race == RACE_HALFLING) {
        cur = SPRITEGROUP_ANIM_HALFLING_ATTACK;
    } else if (race == RACE_GOBLIN) {
        cur = SPRITEGROUP_ANIM_GOBLIN_ATTACK;
    }
    spritegroup_set_current(sg, SPRITEGROUP_ANIM_GOBLIN_ATTACK);
    update_weapon_for_entity(g, id, sg);

    //e->is_attacking = false;
    g_set_attacking(g, id, false);
}

//static void libdraw_set_sg_is_blocking(gamestate* const g, entity_t* const e, spritegroup_t* const sg) {
static void libdraw_set_sg_is_blocking(gamestate* const g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    race_t race = g_get_race(g, id);
    if (race == RACE_HUMAN) {
        spritegroup_set_current(sg, SPRITEGROUP_ANIM_HUMAN_GUARD);
        //if (e->shield != ENTITYID_INVALID) {
        //    spritegroup_t* shield_sg = hashtable_entityid_spritegroup_get(spritegroups, e->shield);
        //    if (shield_sg) {
        //        int player_ctx = sg->sprites[sg->current]->currentcontext;
        //        spritegroup_setcontexts(shield_sg, player_ctx);
        //        spritegroup_set_current(shield_sg, SG_ANIM_BUCKLER_FRONT);
        //    }
        //}
    } else if (race == RACE_ORC) {
        spritegroup_set_current(sg, SPRITEGROUP_ANIM_ORC_GUARD);
        //if (e->shield != ENTITYID_INVALID) {
        //    spritegroup_t* shield_sg = hashtable_entityid_spritegroup_get(spritegroups, e->shield);
        //    if (shield_sg) {
        //        int player_ctx = sg->sprites[sg->current]->currentcontext;
        //        spritegroup_setcontexts(shield_sg, player_ctx);
        //        spritegroup_set_current(shield_sg, SG_ANIM_BUCKLER_FRONT);
        //    }
        //}
    }

    g->test_guard = false;
}

//static void libdraw_set_sg_block_success(gamestate* const g, entity_t* const e, spritegroup_t* const sg) {
static void libdraw_set_sg_block_success(gamestate* const g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    race_t race = g_get_race(g, id);

    int anim_index = -1;

    if (race == RACE_HUMAN) {
        anim_index = SPRITEGROUP_ANIM_HUMAN_GUARD_SUCCESS;
        //spritegroup_set_current(sg, SPRITEGROUP_ANIM_HUMAN_GUARD_SUCCESS);
        //entityid shield_id = g_get_equipment(g, id, EQUIP_SLOT_SHIELD);
        //if (shield_id != ENTITYID_INVALID) {
        //    spritegroup_t* shield_sg = hashtable_entityid_spritegroup_get(spritegroups, shield_id);
        //    if (shield_sg) {
        //        sprite* player_sprite = sg_get_current(sg);
        //        int player_ctx = sprite_get_context(player_sprite);
        //        spritegroup_set_current(shield_sg, SG_ANIM_BUCKLER_SUCCESS_FRONT);
        //        spritegroup_setcontexts(shield_sg, player_ctx);
        //    }
        //}
    } else if (race == RACE_ORC) {
        anim_index = SPRITEGROUP_ANIM_ORC_GUARD_SUCCESS;
        //spritegroup_set_current(sg, SPRITEGROUP_ANIM_ORC_GUARD_SUCCESS);
        //entityid shield_id = g_get_equipment(g, id, EQUIP_SLOT_SHIELD);
        //if (shield_id != ENTITYID_INVALID) {
        //    spritegroup_t* shield_sg = hashtable_entityid_spritegroup_get(spritegroups, shield_id);
        //    if (shield_sg) {
        //        sprite* player_sprite = sg_get_current(sg);
        //        int player_ctx = sprite_get_context(player_sprite);
        //        spritegroup_set_current(shield_sg, SG_ANIM_BUCKLER_SUCCESS_FRONT);
        //        spritegroup_setcontexts(shield_sg, player_ctx);
        //    }
        //}
    }
    spritegroup_set_current(sg, anim_index);

    entityid shield_id = g_get_equipment(g, id, EQUIP_SLOT_SHIELD);
    if (shield_id != ENTITYID_INVALID) {
        spritegroup_t* shield_sg = hashtable_entityid_spritegroup_get(spritegroups, shield_id);
        if (shield_sg) {
            sprite* player_sprite = sg_get_current(sg);
            int player_ctx = sprite_get_context(player_sprite);
            spritegroup_set_current(shield_sg, SG_ANIM_BUCKLER_SUCCESS_FRONT);
            spritegroup_setcontexts(shield_sg, player_ctx);
        }
    }

    //e->block_success = false;
    g_set_block_success(g, id, false);
}

//static void libdraw_update_sprite_attack(gamestate* const g, entity_t* e, spritegroup_t* sg) {
static void libdraw_update_sprite_attack(gamestate* const g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");

    //minfo("libdraw_update_sprite_attack: id %d", e->id);
    //if (e->is_attacking) {
    if (g_get_attacking(g, id)) {
        libdraw_set_sg_is_attacking(g, id, sg);
    } else if (g_get_block_success(g, id)) {
        libdraw_set_sg_block_success(g, id, sg);
        //}
        //else if (g->test_guard) {
        //    libdraw_set_sg_is_blocking(g, id, sg);
        //} else if (e->is_damaged) {
    } else if (g_get_damaged(g, id)) {
        libdraw_set_sg_is_damaged(g, id, sg);
    } else if (g_is_dead(g, id)) {
        libdraw_set_sg_is_dead(g, id, sg);
    }
}

//static void libdraw_update_sprite_position(gamestate* const g, spritegroup_t* sg, entity_t* e) {
//static void libdraw_update_sprite_position(gamestate* const g, spritegroup_t* sg, entityid id) {
static void libdraw_update_sprite_position(gamestate* const g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(sg, "spritegroup is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    //if (!sg || !e) {
    //    merror("spritegroup or entity is NULL");
    //    return;
    //}

    loc_t sprite_move = g_get_sprite_move(g, id);

    if (sprite_move.x != 0 || sprite_move.y != 0) {
        sg->move.x = sprite_move.x;
        sg->move.y = sprite_move.y;
        g_update_sprite_move(g, id, (loc_t){0, 0, 0});

        entitytype_t type = g_get_type(g, id);
        if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
            race_t race = g_get_race(g, id);
            if (race == RACE_HUMAN)
                sg->current = SPRITEGROUP_ANIM_HUMAN_WALK;
            else if (race == RACE_ORC)
                sg->current = SPRITEGROUP_ANIM_ORC_WALK;
            else if (race == RACE_ELF)
                sg->current = SPRITEGROUP_ANIM_ELF_WALK;
            else if (race == RACE_DWARF)
                sg->current = SPRITEGROUP_ANIM_DWARF_WALK;
            else if (race == RACE_HALFLING)
                sg->current = SPRITEGROUP_ANIM_HALFLING_WALK;
            else if (race == RACE_GOBLIN)
                sg->current = SPRITEGROUP_ANIM_GOBLIN_WALK;
        }
    }
}

static void libdraw_update_sprite_context_ptr(gamestate* const g, spritegroup_t* group, direction_t dir) {
    massert(g, "gamestate is NULL");
    massert(group != NULL, "group is NULL");
    const int old_ctx = group->sprites[group->current]->currentcontext;
    int ctx = old_ctx;
    ctx = dir == DIR_NONE         ? old_ctx
          : dir == DIR_DOWN_RIGHT ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_DOWN_LEFT  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_UP_RIGHT   ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_UP_LEFT    ? SPRITEGROUP_CONTEXT_L_U

          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_R_D ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_L_D ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_R_U ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_L_U ? SPRITEGROUP_CONTEXT_L_D

          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_R_D ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_L_D ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_R_U ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_L_U ? SPRITEGROUP_CONTEXT_L_U

          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_R_D ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_L_D ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_R_U ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_L_U ? SPRITEGROUP_CONTEXT_R_U

          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_R_D ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_L_D ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_R_U ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_L_U ? SPRITEGROUP_CONTEXT_L_U
                                                              : old_ctx;
    //if (ctx != old_ctx) minfo("ctx changed from %d to %d", old_ctx, ctx);
    spritegroup_setcontexts(group, ctx);
}

static void libdraw_update_sprite_ptr(gamestate* const g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    if (g_is_dead(g, id) && !spritegroup_is_animating(sg)) return;
    if (g_get_update(g, id)) {
        libdraw_update_sprite_context_ptr(g, sg, g_get_direction(g, id));
        g_set_update(g, id, false);
    }
    // Copy movement intent from sprite_move_x/y if present
    libdraw_update_sprite_position(g, id, sg);
    libdraw_update_sprite_attack(g, id, sg);
    // Update movement as long as sg->move.x/y is non-zero
    spritegroup_update_dest(sg);
    // Snap to the tile position only when movement is fully complete
    loc_t loc = g_get_location(g, id);
    spritegroup_snap_dest(sg, loc);
}

//static void libdraw_handle_frame_incr(gamestate* const g, spritegroup_t* const sg) {
static void libdraw_handle_frame_incr(gamestate* const g, entityid id, spritegroup_t* const sg) {
    //minfo("libdraw_handle_frame_incr: id %d", id);
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    sprite* const s = sg_get_current(sg);
    massert(s, "sprite is NULL");

    //sprite* const s_shadow = sg->sprites[sg->current + 1];
    if (g->framecount % ANIM_SPEED == 0) {
        sprite_incrframe(s);
        if (s->num_loops >= 1) {
            sg->current = sg->default_anim;
            s->num_loops = 0;
        }

        // attempt to grab the sprite's shadow
        sprite* const s_shadow = sg_get_current_plus_one(sg);
        if (s_shadow) {
            sprite_incrframe(s_shadow);
            if (s_shadow->num_loops >= 1) {
                sg->current = sg->default_anim;
                s_shadow->num_loops = 0;
            }
        }
    }
}

static void libdraw_update_sprite(gamestate* const g, entityid id) {
    //minfo("update sprite: %d", id);
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    int num_spritegroups = ht_entityid_sg_get_num_entries_for_key(spritegroups, id);
    for (int i = 0; i < num_spritegroups; i++) {
        spritegroup_t* const sg = hashtable_entityid_spritegroup_get_by_index(spritegroups, id, i);
        if (sg) {
            libdraw_update_sprite_ptr(g, id, sg);
            libdraw_handle_frame_incr(g, id, sg);
        }
    }
}

static void libdraw_handle_gamestate_flag(gamestate* const g) {
    massert(g, "gamestate is NULL");
    const bool done = libdraw_check_default_animations(g);
    if (done) {
        //if (g->flag == GAMESTATE_FLAG_PLAYER_INPUT) {
        //minfo("PLAYER INPUT");
        //g->flag = GAMESTATE_FLAG_NPC_TURN;
        //g->test_guard = false;
        //} else
        if (g->flag == GAMESTATE_FLAG_PLAYER_ANIM) {
            //minfo("PLAYER ANIM");
            g->flag = GAMESTATE_FLAG_NPC_TURN;
            g->test_guard = false;
            //}
            //else if (g->flag == GAMESTATE_FLAG_NPC_TURN) {
            //minfo("NPC TURN");
            //g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
            //g->turn_count++;
            //if (g->flag == GAMESTATE_FLAG_PLAYER_ANIM) {
            //    g->flag = GAMESTATE_FLAG_NPC_TURN;
            //    g->test_guard = false;
        } else if (g->flag == GAMESTATE_FLAG_NPC_ANIM) {
            //minfo("NPC ANIM");
            g->entity_turn = g->hero_id; // Reset directly to hero
            g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
            g->turn_count++;
        }
    }
}

void libdraw_update_sprites(gamestate* const g) {
    //minfo("libdraw_update_sprites");
    if (g) {
        if (g->dirty_entities) {
            minfo("libdraw_update_sprites: dirty_entities");
            for (entityid i = g->new_entityid_begin; i < g->new_entityid_end; i++) {
                create_sg_byid(g, i);
            }

            g->dirty_entities = false;
            g->new_entityid_begin = ENTITYID_INVALID;
            g->new_entityid_end = ENTITYID_INVALID;
        }
        // for each entityid in our entitymap, update the spritegroup
        //for (int i = 0; i < g->index_entityids; i++) {
        for (entityid id = 0; id < g->next_entityid; id++) {
            libdraw_update_sprite(g, id);
        }
        libdraw_handle_gamestate_flag(g);
    }
}

static bool libdraw_draw_dungeon_floor(const gamestate* const g) {
    massert(g, "gamestate is NULL");
    dungeon_floor_t* const df = d_get_current_floor(g->d);
    massert(df, "dungeon_floor is NULL");
    int z = g->d->current_floor;
    draw_dungeon_tiles_2d(g, z, df);
    draw_entities_2d(g, z, df, true); // dead entities
    draw_entities_2d(g, z, df, false); // alive entities
    //draw_wall_tiles_2d(g, df);
    //msuccess("libdraw_draw_dungeon_floor: done");
    return true;
}

static void libdraw_draw_debug_panel(gamestate* const g) {
    massert(g, "gamestate is NULL");
    const Color bg = Fade((Color){0x66, 0x66, 0x66}, 0.8f), fg = WHITE;
    const int w0 = g->debugpanel.w + g->debugpanel.pad_left + g->debugpanel.pad_right * 4;
    const int h0 = g->debugpanel.h + g->debugpanel.pad_top + g->debugpanel.pad_bottom;
    const int x1 = g->debugpanel.x + g->debugpanel.pad_left;
    const int y1 = g->debugpanel.y + g->debugpanel.pad_top;
    DrawRectangle(g->debugpanel.x, g->debugpanel.y, w0, h0, bg);
    DrawText(g->debugpanel.buffer, x1, y1, g->debugpanel.font_size, fg);
}

static bool libdraw_draw_player_target_box(const gamestate* const g) {
    massert(g, "gamestate is NULL");
    const entityid id = g->hero_id;
    if (id == -1) {
        merror("libdraw_draw_player_target_box: id is -1");
        return false;
    }
    //direction_t dir = e->direction;
    direction_t dir = g_get_direction(g, id);
    loc_t loc = g_get_location(g, id);
    int x = loc.x + get_x_from_dir(dir);
    int y = loc.y + get_y_from_dir(dir);
    int ds = DEFAULT_TILE_SIZE;
    Color base_c = GREEN;
    float a = 0.5f;
    Color c = Fade(base_c, a);
    DrawRectangleLinesEx((Rectangle){x * ds, y * ds, ds, ds}, 1, c);
    return true;
}

static void libdraw_drawframe_2d(gamestate* const g) {
    BeginMode2D(g->cam2d);

    //BeginShaderMode(shader_color_noise);
    //float time = (float)GetTime(); // Current time in seconds
    //SetShaderValue(shader_color_noise, GetShaderLocation(shader_color_noise, "time"), &time, SHADER_UNIFORM_FLOAT);
    ClearBackground(BLACK);
    //EndShaderMode();

    if (!libdraw_draw_dungeon_floor(g)) merror("failed to draw dungeon floor");
    if (!libdraw_draw_player_target_box(g)) merror("failed to draw player target box");
    if (!libdraw_camera_lock_on(g)) merror("failed to lock camera on hero");
    //msuccess("libdraw_drawframe_2d: done");
    EndMode2D();
}

static void draw_message_box(gamestate* g) {
    if (!g->msg_system.is_active || g->msg_system.count == 0) {
        return;
    }
    const char* prompt = "[A] Next";
    const char* msg = g->msg_system.messages[g->msg_system.index];
    Color message_bg = (Color){0x33, 0x33, 0x33, 0xff};
    //Color message_bg = (Color){0, 0, 0xff, 0xff};
    // copy the message to a temporary buffer
    char tmp[1024] = {0};
    snprintf(tmp, sizeof(tmp), "%s", msg);
    // Measure text (split into lines if needed)
    int text_width = MeasureText(msg, g->font_size);
    int text_height = g->font_size;
    // Calculate centered box position
    const Rectangle box = {.x = (g->windowwidth - text_width) / 2.0 - g->pad,
                           .y = (g->windowheight - text_height) / 8.0 - g->pad,
                           .width = text_width + g->pad * 2,
                           .height = text_height + g->pad * 2};
    // Draw box (semi-transparent black with white border)
    DrawRectangleRec(box, message_bg);
    DrawRectangleLinesEx(box, 2, WHITE);
    // Draw text (centered in box)
    //DrawText(msg, box.x + g->pad, box.y + g->pad, g->font_size, WHITE);
    DrawText(tmp, box.x + g->pad, box.y + g->pad, g->font_size, WHITE);
    // Show "Next" prompt if more messages exist
    if (g->msg_system.count > 1 && g->msg_system.index < g->msg_system.count - 1) {
        int prompt_font_size = 10;
        int prompt_offset = 10; // Offset from box edges
        char tmp_prompt[1024] = {0};
        snprintf(tmp_prompt, sizeof(tmp_prompt), "%s %d/%d", prompt, g->msg_system.index + 1, g->msg_system.count);
        Vector2 prompt_size = MeasureTextEx(GetFontDefault(), tmp_prompt, prompt_font_size, 1.0f);
        DrawText(tmp_prompt,
                 box.x + box.width - prompt_size.x - prompt_offset, // Right-align in box
                 box.y + box.height - prompt_size.y - prompt_offset / 2.0, // Bottom of box
                 prompt_font_size,
                 WHITE);
    }
}

static inline void update_debug_panel(gamestate* const g) {
    // concat a string onto the end of the debug panel message
    char tmp[1024] = {0};
    //snprintf(tmp, sizeof(tmp), "7777\n");
    strncat(g->debugpanel.buffer, tmp, sizeof(g->debugpanel.buffer) - strlen(g->debugpanel.buffer) - 1);
}

static inline void handle_debug_panel(gamestate* const g) {
    if (g->debugpanelon) {
        update_debug_panel(g);
        libdraw_draw_debug_panel(g);
    }
}

void libdraw_drawframe(gamestate* const g) {
    double start_time = GetTime();
    BeginDrawing();
    ClearBackground(WHITE);
    BeginTextureMode(target);
    //BeginShaderMode(shader_psychedelic_0);
    //float time = (float)GetTime(); // Current time in seconds
    //SetShaderValue(shader_psychedelic_0, GetShaderLocation(shader_psychedelic_0, "time"), &time, SHADER_UNIFORM_FLOAT);
    //EndShaderMode();
    libdraw_drawframe_2d(g);
    //minfo("libdraw_drawframe: message box");
    draw_message_history(g);
    draw_message_box(g);
    //minfo("libdraw_drawframe: message history");
    //minfo("libdraw_drawframe: hud");
    draw_hud(g);
    //minfo("libdraw_drawframe: inventory menu");
    draw_inventory_menu(g);
    EndTextureMode();
    DrawTexturePro(target.texture, target_src, target_dest, target_origin, 0.0f, WHITE);
    //minfo("libdraw_drawframe: debug panel");
    handle_debug_panel(g);
    EndDrawing();
    //double elapsed_time = GetTime() - start_time;
    g->last_frame_time = GetTime() - start_time;
    g->framecount++;
}

static bool libdraw_unload_texture(int txkey) {
    if (txkey < 0 || txkey >= GAMESTATE_SIZEOFTEXINFOARRAY) {
        merror("libdraw_unload_texture: txkey out of bounds");
        return false;
    }
    UnloadTexture(txinfo[txkey].texture);
    txinfo[txkey].texture = (Texture2D){0};
    txinfo[txkey].contexts = 0;
    //msuccess("libdraw_unload_texture: texture unloaded successfully");
    return true;
}

static void libdraw_unload_textures() {
    for (int i = 0; i < GAMESTATE_SIZEOFTEXINFOARRAY; i++) {
        libdraw_unload_texture(i);
    }
}

void libdraw_close() {
    libdraw_unload_textures();
    libdraw_unload_shaders();
    CloseWindow();
}

static bool load_texture(int txkey, int ctxs, int frames, bool do_dither, char* path) {
    if (txkey < 0 || txkey >= GAMESTATE_SIZEOFTEXINFOARRAY) {
        merror("load_texture: txkey out of bounds: %d", txkey);
        return false;
    } else if (ctxs < 0) {
        merror("load_texture: contexts out of bounds: %d", ctxs);
        return false;
    } else if (frames < 0) {
        merror("load_texture: frames out of bounds: %d", frames);
        return false;
    } else if (txinfo[txkey].texture.id > 0) {
        merror("load_texture: texture already loaded: %d", txkey);
        return false;
    } else if (strlen(path) == 0) {
        merror("load_texture: path is empty");
        return false;
    } else if (strcmp(path, "\n") == 0) {
        merror("load_texture: path is newline");
        return false;
    }
    Image image = LoadImage(path);
    if (do_dither) {
        ImageDither(&image, 4, 4, 4, 4);
    }
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    txinfo[txkey].texture = texture;
    txinfo[txkey].contexts = ctxs;
    txinfo[txkey].num_frames = frames;
    //msuccess("load_texture: texture loaded successfully");
    return true;
}

static void load_textures() {
    const char* textures_file = "textures.txt";
    FILE* file = fopen(textures_file, "r");
    if (!file) {
        merror("textures.txt not found");
        return;
    }
    char line[1024] = {0};
    while (fgets(line, sizeof(line), file)) {
        int txkey = -1;
        int contexts = -1;
        int frames = -1;
        int do_dither = 0;
        char path[512] = {0};
        // check if the line begins with a #
        if (line[0] == '#') {
            minfo("skipping comment line");
            continue;
        }
        sscanf(line, "%d %d %d %d %s", &txkey, &contexts, &frames, &do_dither, path);
        if (txkey < 0 || contexts < 0 || frames < 0) {
            merror("invalid line in textures.txt");
            continue;
        }
        load_texture(txkey, contexts, frames, do_dither, path);
    }
    fclose(file);
}

static void create_spritegroup(gamestate* const g, entityid id, int* keys, int num_keys, int offset_x, int offset_y, specifier_t spec) {
    //minfo("create_spritegroup");
    if (!g) {
        merror("gamestate is NULL");
        return;
    }
    // can hold up to 32 sprites
    spritegroup_t* group = spritegroup_create(SPRITEGROUP_DEFAULT_SIZE);
    if (!group) {
        merror("failed to create spritegroup");
        return;
    }
    //disabling this check until dungeon_floor created
    dungeon_floor_t* df = d_get_current_floor(g->d);
    if (!df) {
        merror("dungeon_floor is NULL");
        spritegroup_destroy(group);
        return;
    }
    const int df_w = df->width;
    const int df_h = df->height;
    loc_t loc = g_get_location(g, id);
    if (loc.x < 0 || loc.x >= df_w || loc.y < 0 || loc.y >= df_h) {
        merror("entity pos out of bounds %d %d", loc.x, loc.y);
        spritegroup_destroy(group);
        return;
    }
    for (int i = 0; i < num_keys; i++) {
        const int k = keys[i];
        Texture2D* tex = &txinfo[k].texture;
        sprite* s = sprite_create(tex, txinfo[k].contexts, txinfo[k].num_frames);
        spritegroup_add(group, s);
    }
    spritegroup_set_specifier(group, spec);
    group->id = id;
    sprite* s = spritegroup_get(group, 0);
    if (!s) {
        merror("sprite is NULL");
        spritegroup_destroy(group);
        return;
    }
    group->current = 0;
    group->dest = (Rectangle){loc.x * DEFAULT_TILE_SIZE + offset_x, loc.y * DEFAULT_TILE_SIZE + offset_y, s->width, s->height};
    group->off_x = offset_x;
    group->off_y = offset_y;
    hashtable_entityid_spritegroup_insert(spritegroups, id, group);
}

static void calc_debugpanel_size(gamestate* const g) {
    massert(g, "gamestate is NULL");
    Vector2 s = MeasureTextEx(GetFontDefault(), g->debugpanel.buffer, g->debugpanel.font_size, 1);
    const float width_factor = 1.1f;
    g->debugpanel.w = s.x * width_factor;
    g->debugpanel.h = s.y;
}

static void create_sg_byid(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    int* keys = NULL;
    int num_keys = 0;
    const int offset_x = -12, offset_y = -12;
    entitytype_t type = g_get_type(g, id);
    if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
        race_t race = g_get_race(g, id);
        switch (race) {
        case RACE_HUMAN:
            keys = TX_HUMAN_KEYS;
            num_keys = TX_HUMAN_KEY_COUNT;
            break;
        // Add cases for other races here
        case RACE_ORC:
            keys = TX_ORC_KEYS;
            num_keys = TX_ORC_KEY_COUNT;
            break;
        case RACE_ELF:
            keys = TX_ELF_KEYS;
            num_keys = TX_ELF_KEY_COUNT;
            break;
        case RACE_DWARF:
            keys = TX_DWARF_KEYS;
            num_keys = TX_DWARF_KEY_COUNT;
            break;
        case RACE_HALFLING:
            keys = TX_HALFLING_KEYS;
            num_keys = TX_HALFLING_KEY_COUNT;
            break;
        case RACE_GOBLIN:
            keys = TX_GOBLIN_KEYS;
            num_keys = TX_GOBLIN_KEY_COUNT;
            break;
        default: merror("unknown race %d", race); return;
        }
        create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
    } else if (type == ENTITY_ITEM) {
        // check the item type
        itemtype item_type = g_get_itemtype(g, id);
        if (item_type == ITEM_WEAPON) {
            // check the weapon type
            weapontype weapon_type = g_get_weapontype(g, id);
            if (weapon_type == WEAPON_SWORD) {
                keys = TX_LONG_SWORD_KEYS;
                num_keys = TX_LONG_SWORD_KEY_COUNT;
            } else if (weapon_type == WEAPON_DAGGER) {
                keys = TX_DAGGER_KEYS;
                num_keys = TX_DAGGER_KEY_COUNT;
            } else if (weapon_type == WEAPON_AXE) {
                keys = TX_AXE_KEYS;
                num_keys = TX_AXE_KEY_COUNT;
            }

            create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
        } else if (item_type == ITEM_SHIELD) {
            keys = TX_BUCKLER_KEYS;
            num_keys = TX_BUCKLER_KEY_COUNT;
            create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
        } else if (item_type == ITEM_POTION) {
            potiontype potion_type = g_get_potiontype(g, id);
            if (potion_type == POTION_HEALTH_SMALL) {
                keys = TX_POTION_HP_SMALL_KEYS;
                num_keys = TX_POTION_HP_SMALL_KEY_COUNT;
            } else if (potion_type == POTION_HEALTH_MEDIUM) {
                keys = TX_POTION_HP_MEDIUM_KEYS;
                num_keys = TX_POTION_HP_MEDIUM_KEY_COUNT;
            } else if (potion_type == POTION_HEALTH_LARGE) {
                keys = TX_POTION_HP_LARGE_KEYS;
                num_keys = TX_POTION_HP_LARGE_KEY_COUNT;
            }

            create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
        }
    }
    //    // for now we only have 1 sprite for weapons
    //    keys = TX_LONG_SWORD_KEYS;
    //    num_keys = TX_LONG_SWORD_KEY_COUNT;
    //    create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
    //} else if (type == ENTITY_SHIELD) {
    //    keys = TX_BUCKLER_KEYS;
    //    num_keys = TX_BUCKLER_KEY_COUNT;
    //    create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
    //}
    //else if (type == ENTITY_POTION) {
    //    massert(e->potion_type != POTION_NONE, "potion_type is NONE");
    //    // check the potion type
    //    if (e->potion_type == POTION_HP_SMALL) {
    //        keys = TX_POTION_HP_SMALL_KEYS;
    //        num_keys = TX_POTION_HP_SMALL_KEY_COUNT;
    //        create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
    //    } else if (e->potion_type == POTION_HP_MEDIUM) {
    //        keys = TX_POTION_HP_MEDIUM_KEYS;
    //        num_keys = TX_POTION_HP_MEDIUM_KEY_COUNT;
    //        create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
    //    } else if (e->potion_type == POTION_HP_LARGE) {
    //        keys = TX_POTION_HP_LARGE_KEYS;
    //        num_keys = TX_POTION_HP_LARGE_KEY_COUNT;
    //        create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
    //    }
    //} else if (type == ENTITY_DOOR) {
    //    keys = TX_WOODEN_DOOR_KEYS;
    //    num_keys = TX_WOODEN_DOOR_KEY_COUNT;
    //    create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
    //}
}

static void draw_hud(gamestate* const g) {
    massert(g, "gamestate is NULL");
    const int turn = g->turn_count;
    int stat_count = 0;
    int* stats = g_get_stats(g, g->hero_id, &stat_count);
    massert(stats, "stats is NULL");
    //const int hp = g_get_stat(g, g->hero_id, STATS_HP);
    const int hp = stats[STATS_HP];
    //const int maxhp = g_get_stat(g, g->hero_id, STATS_MAXHP);
    const int maxhp = stats[STATS_MAXHP];
    //const int level = g_get_stat(g, g->hero_id, STATS_LEVEL);
    const int level = stats[STATS_LEVEL];
    const int xp = stats[STATS_XP];
    const int ac = stats[STATS_AC];
    loc_t loc = g_get_location(g, g->hero_id);
    dungeon_floor_t* const df = d_get_current_floor(g->d);
    const char* room_name = df_get_room_name(df, loc);
    char buffer[1024] = {0};
    snprintf(buffer, sizeof(buffer), "%s Lvl %d HP %d/%d AC: %d XP %d Room: %s Turn %d", g_get_name(g, g->hero_id), level, hp, maxhp, ac, xp, room_name, turn);
    const Vector2 text_size = MeasureTextEx(GetFontDefault(), buffer, g->font_size, g->line_spacing);
    const int box_w = text_size.x + g->pad;
    const int box_h = text_size.y + g->pad;
    //const int box_x = (g->windowwidth - box_w) / 2;
    //const int box_y = (g->windowheight - box_h) * 7 / 8;
    const int box_x = 0;
    const int box_y = 0;
    const Color bg = (Color){0x33, 0x33, 0x33, 0xFF}, fg = WHITE;
    //const Color bg = (Color){0, 0, 0xff, 0xFF}, fg = WHITE;
    DrawRectangleRec((Rectangle){box_x, box_y, box_w, box_h}, bg);
    DrawRectangleLinesEx((Rectangle){box_x, box_y, box_w, box_h}, 2, fg);
    // Calculate text position to center it within the box
    const float text_x = box_x + (box_w - text_size.x) / 2;
    const float text_y = box_y + (box_h - text_size.y) / 2;
    DrawTextEx(GetFontDefault(), buffer, (Vector2){text_x, text_y}, g->font_size, g->line_spacing, fg);
}

void libdraw_init(gamestate* const g) {
    massert(g, "gamestate is NULL");
    const int w = DEFAULT_WIN_WIDTH;
    const int h = DEFAULT_WIN_HEIGHT;
    const int x = w / 3;
    const int y = h / 3;
    const char* title = WINDOW_TITLE;
    InitWindow(w, h, title);
    g->windowwidth = w;
    g->windowheight = h;
    SetTargetFPS(60);
    target = LoadRenderTexture(w, h);
    target_src = (Rectangle){0, 0, w, -h};
    target_dest = (Rectangle){0, 0, w, h};
    spritegroups = hashtable_entityid_spritegroup_create(DEFAULT_SPRITEGROUPS_SIZE);
    load_textures();
    //for (entityid i = 0; i < g->next_entityid; i++) create_sg_byid(g, i);
    calc_debugpanel_size(g);
    load_shaders();
    g->cam2d.offset = (Vector2){x, y};
    gamestate_set_debug_panel_pos_top_right(g);
    //msuccess("libdraw_init");
}

static void draw_message_history(gamestate* const g) {
    massert(g, "gamestate is NULL");
    // if there are no messages in the message history, return
    if (g->msg_history.count == 0) {
        return;
    }
    const int max_messages = 20;
    const int x = 0;
    const int y = 42;
    int current_count = 0;
    char tmp_buffer[2048] = {0};
    Color message_bg = (Color){0x33, 0x33, 0x33, 0xff};
    //Color message_bg = (Color){0, 0, 0xff, 0xff};
    // instead of a placeholder message, we now need to actually draw the message history
    // we might only render the last N messages
    for (int i = g->msg_history.count - 1; i >= 0 && current_count < max_messages; i--) {
        strncat(tmp_buffer, g->msg_history.messages[i], sizeof(tmp_buffer) - strlen(tmp_buffer) - 1);
        strncat(tmp_buffer, "\n", sizeof(tmp_buffer) - strlen(tmp_buffer) - 1);
        current_count++;
    }
    // chop off the last newline
    if (strlen(tmp_buffer) > 0) {
        tmp_buffer[strlen(tmp_buffer) - 1] = '\0';
    }
    // Measure text (split into lines if needed)
    const Vector2 text_size = MeasureTextEx(GetFontDefault(), tmp_buffer, g->font_size, g->line_spacing);
    // Calculate box position
    // we want the box to be in the top left corner of the screen
    //const Rectangle box = {.x = x, .y = y, .width = text_size.x + g->pad * 2, .height = text_size.y + g->pad * 2};
    const Rectangle box = {.x = x, .y = y, .width = text_size.x + g->pad, .height = text_size.y + g->pad};
    // Draw box (semi-transparent black with white border)
    DrawRectangleRec(box, message_bg);
    DrawRectangleLinesEx(box, 2, WHITE);
    // Draw text (centered in box)
    const float text_x = box.x + (box.width - text_size.x) / 2;
    const float text_y = box.y + (box.height - text_size.y) / 2;
    //DrawTextEx(GetFontDefault(), tmp_buffer, (Vector2){box.x + g->pad, box.y + g->pad}, g->font_size, g->line_spacing, WHITE);
    DrawTextEx(GetFontDefault(), tmp_buffer, (Vector2){text_x, text_y}, g->font_size, g->line_spacing, WHITE);
}

static void draw_inventory_menu(gamestate* const g) {
    massert(g, "gamestate is NULL");
    if (!g->display_inventory_menu) return;

    const char* menu_title = "Inventory Menu";

    // Parameters
    const int box_pad = g->pad;
    const int section_gap = 16;
    const int item_list_pad = g->pad;
    const int max_visible_items = 12; // arbitrary limit for list height

    // Measure title
    Vector2 title_size = MeasureTextEx(GetFontDefault(), menu_title, g->font_size, g->line_spacing);

    // Menu box size
    float menu_width_percent = 0.80f;
    float menu_height_percent = 0.80f;
    float menu_width = g->windowwidth * menu_width_percent;
    float menu_height = g->windowheight * menu_height_percent;

    Rectangle menu_box = {.x = (g->windowwidth - menu_width) / 2.0f, .y = (g->windowheight - menu_height) / 4.0f, .width = menu_width, .height = menu_height};

    // Draw menu background and border
    //DrawRectangleRec(menu_box, (Color){0x33, 0x33, 0x33, 0xff});
    DrawRectangleRec(menu_box, (Color){0x33, 0x33, 0x33, 0x99});
    DrawRectangleLinesEx(menu_box, 2, WHITE);

    // Draw menu title centered at top
    float title_x = menu_box.x + (menu_box.width - title_size.x) / 2.0f;
    float title_y = menu_box.y + box_pad;
    //DrawTextEx(GetFontDefault(), menu_title, (Vector2){title_x, title_y}, g->font_size, g->line_spacing, WHITE);
    DrawText(menu_title, title_x, title_y, g->font_size, WHITE);

    // Partition into left/right halves (with gap)
    float half_width = (menu_box.width - section_gap) / 2.0f;
    float half_height = menu_box.height - title_size.y - box_pad * 2.0f - box_pad;

    // Left box: inventory list
    Rectangle left_box = {.x = menu_box.x + box_pad, .y = title_y + title_size.y + box_pad, .width = half_width - box_pad, .height = half_height};

    // Right box: item info
    Rectangle right_box = {.x = left_box.x + half_width + section_gap, .y = left_box.y, .width = half_width - box_pad * 2, .height = half_height};

    // Draw left and right boxes
    DrawRectangleRec(left_box, (Color){0x22, 0x22, 0x22, 0xff});
    DrawRectangleLinesEx(left_box, 2, WHITE);
    DrawRectangleRec(right_box, (Color){0x22, 0x22, 0x22, 0xff});
    DrawRectangleLinesEx(right_box, 2, WHITE);

    float item_y = left_box.y + item_list_pad;

    int inventory_count = 0;
    entityid* inventory = g_get_inventory(g, g->hero_id, &inventory_count);

    //for (int i = 0; i < hero->inventory_count && i < max_visible_items; i++) {
    for (int i = 0; i < g_get_inventory_count(g, g->hero_id) && i < max_visible_items; i++) {
        entityid item_id = inventory[i];
        if (item_id == ENTITYID_INVALID) continue;
        //int item_id = g_get_inventory_item(g, g->hero_id, i);
        //    if (item_id == 0) continue;
        float item_x = left_box.x + item_list_pad;
        char item_display[128];
        entitytype_t item_type = g_get_type(g, item_id);
        bool is_equipped = g_is_equipped(g, g->hero_id, item_id);
        //if (item_type == ENTITY_WEAPON) {
        //        is_equipped = (hero->weapon == item_id);
        //    } else if (item_type == ENTITY_SHIELD) {
        //        is_equipped = (hero->shield == item_id);
        //    }
        if (i == g->inventory_menu_selection) {
            //        //snprintf(item_display, sizeof(item_display), "> %s", item_entity->name);
            //snprintf(item_display, sizeof(item_display), "> %s", "[placeholder]");
            snprintf(item_display, sizeof(item_display), "> %s", g_get_name(g, item_id));
        } else {
            snprintf(item_display, sizeof(item_display), "  %s", g_get_name(g, item_id));
        }
        if (is_equipped) {
            strncat(item_display, " (Equipped)", sizeof(item_display) - strlen(item_display) - 1);
        }
        DrawTextEx(GetFontDefault(), item_display, (Vector2){item_x, item_y}, g->font_size, g->line_spacing, WHITE);
        item_y += g->font_size + 4;
    }

    // Draw item info in right_box
    const char* info_title = "Item Info:";

    char info_text[256] = {0};

    spritegroup_t* sg = NULL;

    if (g->inventory_menu_selection >= 0 && g->inventory_menu_selection < inventory_count) {
        entityid item_id = inventory[g->inventory_menu_selection];
        snprintf(info_text, sizeof(info_text), "%s\nType: %d", g_get_name(g, item_id), g_get_type(g, item_id));
        //    entityid item_id = hero->inventory[g->inventory_menu_selection];
        //        //snprintf(info_text, sizeof(info_text), "%s\nType: %d", item_entity->name, item_entity->type);
        //        snprintf(info_text, sizeof(info_text), "%s\nType: %d", "[placeholder]", g_get_type(g, item_id));
        sg = hashtable_entityid_spritegroup_get(spritegroups, item_id);
        //} else {
        //    snprintf(info_text, sizeof(info_text), "Invalid item data");
        //}
    } else {
        snprintf(info_text, sizeof(info_text), "Select an item to view details here.");
    }

    float info_title_y = right_box.y + item_list_pad;
    float info_text_y = info_title_y + g->font_size + 8;

    DrawTextEx(
        GetFontDefault(), info_title, (Vector2){right_box.x + item_list_pad, info_title_y}, g->font_size, g->line_spacing, (Color){0xaa, 0xaa, 0xaa, 0xff});
    DrawTextEx(GetFontDefault(), info_text, (Vector2){right_box.x + item_list_pad, info_text_y}, g->font_size, g->line_spacing, WHITE);

    if (sg) {
        sprite* s = sg_get_current(sg);
        if (s) {
            const int scale = 8;
            const float sprite_width = s->width * scale;
            const float sprite_height = s->height * scale;
            const float sprite_margin = -6 * scale; // space from top and right edges
            //const float sprite_margin = 0; // space from top and right edges

            // Anchor to top-right of right_box, account for margin
            const float sprite_x = right_box.x + right_box.width - sprite_margin - sprite_width;
            const float sprite_y = right_box.y + sprite_margin;

            DrawTexturePro(*s->texture,
                           s->src,
                           (Rectangle){sprite_x, sprite_y, sprite_width, sprite_height},
                           (Vector2){0, 0}, // Top-left corner as origin
                           0.0f,
                           WHITE);
        }
    }
}

void libdraw_update_input(inputstate* const is) { inputstate_update(is); }

bool libdraw_windowshouldclose() { return WindowShouldClose(); }
