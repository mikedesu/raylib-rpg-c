//#include "component.h"
#include "bonus_table.h"
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
#include "scene.h"
#include "specifier.h"
#include "sprite.h"
#include "spritegroup.h"
#include "spritegroup_anim.h"
#include "textureinfo.h"
#include "tx_keys.h"
#include "weapon.h"
#include <raylib.h>

#define DEFAULT_SPRITEGROUPS_SIZE 128
#define DEFAULT_MUSIC_VOLUME 0.5f

//#define DEFAULT_WIN_WIDTH 800
//#define DEFAULT_WIN_HEIGHT 480
//#define DEFAULT_WIN_WIDTH 960
//#define DEFAULT_WIN_HEIGHT 540

//#define DEFAULT_WIN_WIDTH 640
//#define DEFAULT_WIN_HEIGHT 360
//#define DEFAULT_WIN_WIDTH 1920
//#define DEFAULT_WIN_HEIGHT 1080

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

#define SPRITEGROUP_DEFAULT_SIZE 32
#define DEFAULT_TILE_SIZE_SCALED 32

hashtable_entityid_spritegroup_t* spritegroups = NULL;
textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

Shader shader_grayscale = {0};
Shader shader_glow = {0};
Shader shader_red_glow = {0};
Shader shader_color_noise = {0};
Shader shader_psychedelic_0 = {0};

RenderTexture2D title_target_texture = {0};
RenderTexture2D char_creation_target_texture = {0};
RenderTexture2D main_game_target_texture = {0};

RenderTexture2D target = {0};
Rectangle target_src = {0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};
Rectangle target_dest = {0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};
Vector2 target_origin = {0, 0};
Vector2 zero_vec = {0, 0};

Music music;

int ANIM_SPEED = DEFAULT_ANIM_SPEED;

int libdraw_restart_count = 0;

static int get_total_ac(gamestate* const g, entityid id);

static inline bool libdraw_camera_lock_on(gamestate* const g);

static inline void update_debug_panel(gamestate* const g);
static inline void handle_debug_panel(gamestate* const g);

static void libdraw_drawframe_2d_from_texture(gamestate* const g);
static void libdraw_drawframe_2d_to_texture(gamestate* const g);
static void draw_help_menu(gamestate* const g);
static void draw_gameover_menu(gamestate* const g);
//static void libdraw_update_sprite_post(gamestate* const g, entityid id);
static void libdraw_update_sprite_pre(gamestate* const g, entityid id);
static void libdraw_handle_gamestate_flag(gamestate* const g);
static void draw_weapon_sprite_front(const gamestate* const g, entityid id, spritegroup_t* sg);
static void draw_weapon_sprite_back(const gamestate* const g, entityid id, spritegroup_t* sg);
static void draw_shield_sprite_front(const gamestate* const g, entityid id, spritegroup_t* sg);
static void draw_shield_sprite_back(const gamestate* const g, entityid id, spritegroup_t* sg);
static void update_weapon_for_entity(gamestate* g, entityid id, spritegroup_t* sg);
static void draw_inventory_menu(gamestate* const g);
static void draw_hud(gamestate* const g);
static void draw_title_screen(gamestate* const g, bool show_menu);
static void draw_title_screen_to_texture(gamestate* const g, bool show_menu);
static void draw_title_screen_from_texture(gamestate* const g);
static void draw_character_creation_screen(gamestate* const g);
static void draw_character_creation_screen_from_texture(gamestate* const g);
static void draw_character_creation_screen_to_texture(gamestate* const g);
static void libdraw_set_sg_is_damaged(gamestate* const g, entityid id, spritegroup_t* const sg);
static void libdraw_set_sg_is_dead(gamestate* const g, entityid id, spritegroup_t* const sg);
static void libdraw_set_sg_is_attacking(gamestate* const g, entityid id, spritegroup_t* const sg);
static void libdraw_set_sg_block_success(gamestate* const g, entityid id, spritegroup_t* const sg);
static void load_shaders();
static void libdraw_unload_shaders();
static void load_textures();
static void libdraw_unload_textures();
static void libdraw_update_sprite_attack(gamestate* const g, entityid id, spritegroup_t* sg);
static void libdraw_update_sprite_position(gamestate* const g, entityid id, spritegroup_t* sg);
static void libdraw_update_sprite_context_ptr(gamestate* const g, spritegroup_t* group, direction_t dir);
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
static void draw_shadow_for_entity(const gamestate* const g, spritegroup_t* sg, entityid id);
static void draw_version(const gamestate* const g);

static sprite* get_weapon_back_sprite(const gamestate* g, entityid id, spritegroup_t* sg);
static sprite* get_weapon_front_sprite(const gamestate* g, entityid id, spritegroup_t* sg);
static sprite* get_shield_front_sprite(const gamestate* g, entityid id, spritegroup_t* sg);
static sprite* get_shield_back_sprite(const gamestate* g, entityid id, spritegroup_t* sg);

static bool load_texture(int txkey, int ctxs, int frames, bool do_dither, char* path);
//static bool libdraw_check_default_animations(const gamestate* const g);
static bool libdraw_unload_texture(int txkey);
static bool draw_dungeon_floor_tile(const gamestate* const g, int x, int y, int z);
static bool draw_dungeon_tiles_2d(const gamestate* const g, int z, dungeon_floor_t* df);
static bool draw_entities_2d_at(const gamestate* const g, dungeon_floor_t* const df, bool dead, vec3 loc);
static bool libdraw_draw_dungeon_floor(const gamestate* const g);
static bool libdraw_draw_player_target_box(const gamestate* const g);

static bool draw_dungeon_floor_tile(const gamestate* const g, int x, int y, int z) {
    massert(g, "gamestate is NULL");
    //massert(df, "dungeon_floor is NULL");
    massert(x >= 0, "x is less than 0");
    massert(y >= 0, "y is less than 0");
    dungeon_floor_t* df = d_get_floor(g->d, z);
    massert(x < df->width, "x is out of bounds");
    massert(y < df->height, "y is out of bounds");
    massert(df, "dungeon_floor is NULL");
    tile_t* tile = df_tile_at(df, (vec3){x, y, z});
    massert(tile, "tile is NULL");
    // check if the tile type is none
    if (tile->type == TILE_NONE) {
        return true;
    }
    // check if tile is visible
    if (!tile->visible) {
        return true;
    }
    // just draw the tile itself
    // tile values in get_txkey_for_tiletype.h
    int txkey = get_txkey_for_tiletype(tile->type);
    if (txkey < 0) return false;
    Texture2D* texture = &txinfo[txkey].texture;
    if (texture->id <= 0) return false;
    int offset_x = -12, offset_y = -12;
    int dx = x * DEFAULT_TILE_SIZE + offset_x;
    int dy = y * DEFAULT_TILE_SIZE + offset_y;
    const Rectangle src = (Rectangle){0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
    const Rectangle dest = (Rectangle){dx, dy, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
    DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
    if (tile->has_pressure_plate) {
        int txkey2 = tile->pressure_plate_up_tx_key;
        if (txkey2 < 0) return false;
        Texture2D* texture = &txinfo[txkey2].texture;
        if (texture->id <= 0) return false;
        DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
    }
    if (tile->has_wall_switch) {
        int txkey = tile->wall_switch_on ? tile->wall_switch_down_tx_key : tile->wall_switch_up_tx_key;
        if (txkey < 0) return false;
        Texture2D* texture = &txinfo[txkey].texture;
        if (texture->id <= 0) return false;
        DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
    }
    return true;
}

static bool draw_dungeon_tiles_2d(const gamestate* const g, int z, dungeon_floor_t* df) {
    massert(g, "gamestate is NULL");
    massert(df, "dungeon_floor is NULL");
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
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
    sprite* retval = NULL;
    if (sg->current == SG_ANIM_NPC_ATTACK)
        retval = spritegroup_get(w_sg, SG_ANIM_LONGSWORD_SLASH_F);
    else if (sg->current == SG_ANIM_NPC_SHOT)
        retval = spritegroup_get(w_sg, SG_ANIM_BOW_SHOT_F);
    return retval;
}

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
    sprite* retval = NULL;
    if (sg->current == SG_ANIM_NPC_ATTACK)
        retval = spritegroup_get(w_sg, SG_ANIM_LONGSWORD_SLASH_B);
    else if (sg->current == SG_ANIM_NPC_SHOT)
        retval = spritegroup_get(w_sg, SG_ANIM_BOW_SHOT_B);
    return retval;
}

static sprite* get_shield_front_sprite(const gamestate* g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(sg, "spritegroup is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    entityid shield = g_get_equipment(g, id, EQUIP_SLOT_SHIELD);
    if (shield == ENTITYID_INVALID) return NULL;
    spritegroup_t* s_sg = hashtable_entityid_spritegroup_get(spritegroups, shield);
    if (!s_sg) return NULL;
    sprite* retval = NULL;
    if (sg->current == SG_ANIM_NPC_GUARD_SUCCESS) retval = spritegroup_get(s_sg, SG_ANIM_BUCKLER_SUCCESS_FRONT);
    return retval;
}

//static sprite* get_shield_back_sprite(const gamestate* g, const entity* e, spritegroup_t* sg) {
static sprite* get_shield_back_sprite(const gamestate* g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    massert(sg, "spritegroup is NULL");
    entityid shield = g_get_equipment(g, id, EQUIP_SLOT_SHIELD);
    if (shield == ENTITYID_INVALID) {
        return NULL;
    }
    spritegroup_t* s_sg = hashtable_entityid_spritegroup_get(spritegroups, shield);
    if (!s_sg) {
        return NULL;
    }
    sprite* retval = NULL;
    if (sg->current == SG_ANIM_NPC_GUARD_SUCCESS) retval = spritegroup_get(s_sg, SG_ANIM_BUCKLER_SUCCESS_BACK);
    return retval;
}

static void draw_shadow_for_entity(const gamestate* const g, spritegroup_t* sg, entityid id) {
    massert(g, "gamestate is NULL");
    massert(sg, "spritegroup is NULL");
    massert(id != ENTITYID_INVALID, "id is -1");
    entitytype_t type = g_get_type(g, id);
    if (type != ENTITY_PLAYER && type != ENTITY_NPC) {
        return;
    }
    sprite* shadow = sg_get_current_plus_one(sg);
    if (shadow) {
        Rectangle dest = {sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};
        DrawTexturePro(*shadow->texture, shadow->src, dest, (Vector2){0, 0}, 0, WHITE);
    }
}

static void draw_entity_sprite(const gamestate* const g, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(sg, "spritegroup is NULL");
    sprite* s = sg_get_current(sg);
    massert(s, "sprite is NULL");
    Rectangle dest = {sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};
    DrawTexturePro(*s->texture, s->src, dest, zero_vec, 0, (Color){255, 255, 255, 255});
    // draw a box around the sprite
    //DrawRectangleLinesEx(dest, 1, (Color){255, 0, 0, 255});
}

static void draw_shield_sprite_back(const gamestate* const g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    sprite* shield_back_s = get_shield_back_sprite(g, id, sg);
    if (shield_back_s) DrawTexturePro(*shield_back_s->texture, shield_back_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
}

static void draw_shield_sprite_front(const gamestate* const g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    sprite* shield_front_s = get_shield_front_sprite(g, id, sg);
    if (shield_front_s) DrawTexturePro(*shield_front_s->texture, shield_front_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
}

static void draw_weapon_sprite_back(const gamestate* const g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    sprite* weapon_back_s = get_weapon_back_sprite(g, id, sg);
    if (weapon_back_s) DrawTexturePro(*weapon_back_s->texture, weapon_back_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
}

static void draw_weapon_sprite_front(const gamestate* const g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    sprite* weapon_front_s = get_weapon_front_sprite(g, id, sg);
    if (weapon_front_s) DrawTexturePro(*weapon_front_s->texture, weapon_front_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
}

static void draw_sprite_and_shadow(const gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "id is invalid");
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(spritegroups, id);
    entitytype_t type = g_get_type(g, id);
    massert(sg, "spritegroup is NULL: id %d type: %s", id, entitytype_to_string(type));
    // Draw components in correct order
    draw_shadow_for_entity(g, sg, id);
    draw_shield_sprite_back(g, id, sg);
    draw_weapon_sprite_back(g, id, sg);
    draw_entity_sprite(g, sg);
    draw_shield_sprite_front(g, id, sg);
    draw_weapon_sprite_front(g, id, sg);
}

static bool draw_entities_2d_at(const gamestate* const g, dungeon_floor_t* const df, bool dead, vec3 loc) {
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
    if (!tile->visible) return true; // Do not draw entities on invisible tiles
    for (int i = 0; i < tile_entity_count(tile); i++) {
        entityid id = tile_get_entity(tile, i);
        if (g_is_dead(g, id) == dead) {
            draw_sprite_and_shadow(g, id);
        }
    }
    return true;
}

//static bool draw_entities_2d(const gamestate* const g, int z, dungeon_floor_t* df, bool dead) {
//    massert(g, "draw_entities_2d: gamestate is NULL");
//    massert(df, "draw_entities_2d: dungeon_floor is NULL");
//    massert(df->width > 0, "draw_entities_2d: dungeon_floor width is 0");
//    massert(df->height > 0, "draw_entities_2d: dungeon_floor height is 0");
//    massert(df->width <= DEFAULT_DUNGEON_FLOOR_WIDTH, "draw_entities_2d: dungeon_floor width is too large");
//    massert(df->height <= DEFAULT_DUNGEON_FLOOR_HEIGHT, "draw_entities_2d: dungeon_floor height is too large");
//    for (int y = 0; y < df->height; y++)
//        for (int x = 0; x < df->width; x++) draw_entities_2d_at(g, df, dead, (vec3){x, y, z});
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
    if (!g->cam_lockon) return false;
    spritegroup_t* grp = hashtable_entityid_spritegroup_get(spritegroups, g->hero_id);
    massert(grp, "spritegroup is NULL");
    // get the old camera position
    Vector2 old_target = g->cam2d.target;
    g->cam2d.target = (Vector2){grp->dest.x, grp->dest.y};
    // if the target changes, we need to set a flag indicating as such
    if (old_target.x != g->cam2d.target.x || old_target.y != g->cam2d.target.y) {
        g->cam_changed = g->frame_dirty = true;
    } else {
        g->cam_changed = false;
    }
    return true;
}

//static bool libdraw_check_default_animations(const gamestate* const g) {
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

static void libdraw_set_sg_is_damaged(gamestate* const g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    race_t race = g_get_race(g, id);
    int anim_index = SG_ANIM_NPC_DMG;
    if (race == RACE_BAT) {
        anim_index = SG_ANIM_BAT_DMG;
    } else if (race == RACE_GREEN_SLIME) {
        anim_index = SG_ANIM_SLIME_DMG;
    }
    spritegroup_set_current(sg, anim_index);
    g_set_damaged(g, id, false);
}

static void libdraw_set_sg_is_dead(gamestate* const g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    if (!g_is_dead(g, id)) return;
    race_t race = g_get_race(g, id);
    int anim_index = SG_ANIM_NPC_SPINDIE;
    if (race == RACE_BAT) {
        anim_index = SG_ANIM_BAT_DIE;
    } else if (race == RACE_GREEN_SLIME) {
        anim_index = SG_ANIM_SLIME_DIE;
    }
    if (sg->current == anim_index) return;
    sg_set_default_anim(sg, anim_index);
    spritegroup_set_current(sg, sg->default_anim);
    spritegroup_set_stop_on_last_frame(sg, true);
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

static void libdraw_set_sg_is_attacking(gamestate* const g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    race_t race = g_get_race(g, id);
    entityid weapon = g_get_equipment(g, id, EQUIP_SLOT_WEAPON);
    weapontype wtype = g_get_weapontype(g, weapon);
    int cur = 0;
    cur = SG_ANIM_NPC_ATTACK;
    if (wtype == WEAPON_BOW) {
        cur = SG_ANIM_NPC_SHOT;
    }
    if (race == RACE_BAT) {
        cur = SG_ANIM_BAT_ATTACK;
    } else if (race == RACE_GREEN_SLIME) {
        cur = SG_ANIM_SLIME_ATTACK;
    }
    spritegroup_set_current(sg, cur);
    update_weapon_for_entity(g, id, sg);
    //e->is_attacking = false;
    g_set_attacking(g, id, false);
}

static void libdraw_set_sg_block_success(gamestate* const g, entityid id, spritegroup_t* const sg) {
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
        spritegroup_t* shield_sg = hashtable_entityid_spritegroup_get(spritegroups, shield_id);
        if (shield_sg) {
            sprite* player_sprite = sg_get_current(sg);
            int player_ctx = sprite_get_context(player_sprite);
            spritegroup_set_current(shield_sg, SG_ANIM_BUCKLER_SUCCESS_FRONT);
            spritegroup_setcontexts(shield_sg, player_ctx);
        }
    }
    g_set_block_success(g, id, false);
}

static void libdraw_update_sprite_attack(gamestate* const g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    if (g_get_attacking(g, id)) {
        libdraw_set_sg_is_attacking(g, id, sg);
    } else if (g_get_block_success(g, id)) {
        libdraw_set_sg_block_success(g, id, sg);
    } else if (g_get_damaged(g, id)) {
        libdraw_set_sg_is_damaged(g, id, sg);
    } else if (g_is_dead(g, id)) {
        libdraw_set_sg_is_dead(g, id, sg);
    }
}

static void libdraw_update_sprite_position(gamestate* const g, entityid id, spritegroup_t* sg) {
    massert(g, "gamestate is NULL");
    massert(sg, "spritegroup is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    Rectangle sprite_move = g_get_sprite_move(g, id);
    if (sprite_move.x != 0 || sprite_move.y != 0) {
        sg->move.x = sprite_move.x;
        sg->move.y = sprite_move.y;
        g_update_sprite_move(g, id, (Rectangle){0, 0, 0, 0});
        entitytype_t type = g_get_type(g, id);
        if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
            race_t race = g_get_race(g, id);
            sg->current = race == RACE_BAT                        ? SG_ANIM_BAT_IDLE
                          : race == RACE_GREEN_SLIME              ? SG_ANIM_SLIME_IDLE
                          : race > RACE_NONE && race < RACE_COUNT ? SG_ANIM_NPC_WALK
                                                                  : SG_ANIM_NPC_WALK;
            //if (race == RACE_BAT)
            //    sg->current = SG_ANIM_BAT_IDLE;
            //else if (race == RACE_GREEN_SLIME)
            //    sg->current = SG_ANIM_SLIME_IDLE;
            //else if (race > RACE_NONE && race < RACE_COUNT)
            //    sg->current = SG_ANIM_NPC_WALK;
            //else
            //    sg->current = SG_ANIM_NPC_WALK;
        }
        g->frame_dirty = true;
    }
}

static void libdraw_update_sprite_context_ptr(gamestate* const g, spritegroup_t* group, direction_t dir) {
    massert(g, "gamestate is NULL");
    massert(group != NULL, "group is NULL");
    int old_ctx = group->sprites[group->current]->currentcontext;
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
        minfo("ctx changed from %d to %d", old_ctx, ctx);
        g->frame_dirty = true;
    }
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
    if (spritegroup_update_dest(sg)) {
        //minfo("update dest");
        g->frame_dirty = true;
    }
    // Snap to the tile position only when movement is fully complete
    vec3 loc = g_get_location(g, id);
    //vec3 loc_cast = {loc.x, loc.y, loc.z};
    //spritegroup_snap_dest(sg, loc_cast);
    //const bool done = libdraw_check_default_animations(g);
    //if (done) {
    spritegroup_snap_dest(sg, loc.x, loc.y);
    //if (spritegroup_snap_dest(sg, loc.x, loc.y)) {
    //minfo("snap dest");
    //g->frame_dirty = true;
    //}
}

static void libdraw_handle_frame_incr(gamestate* const g, entityid id, spritegroup_t* const sg) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    sprite* const s = sg_get_current(sg);
    massert(s, "sprite is NULL");
    //sprite* const s_shadow = sg->sprites[sg->current + 1];
    g->frame_dirty = true;
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

static void libdraw_update_sprite(gamestate* const g, entityid id) {
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

static void libdraw_update_sprite_pre(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    int num_spritegroups = ht_entityid_sg_get_num_entries_for_key(spritegroups, id);
    for (int i = 0; i < num_spritegroups; i++) {
        spritegroup_t* const sg = hashtable_entityid_spritegroup_get_by_index(spritegroups, id, i);
        if (sg) {
            libdraw_update_sprite_ptr(g, id, sg);
            //libdraw_handle_frame_incr(g, id, sg);
        }
    }
}

//static void libdraw_update_sprite_post(gamestate* const g, entityid id) {
//    massert(g, "gamestate is NULL");
//    massert(id != ENTITYID_INVALID, "entityid is invalid");
//    int num_spritegroups = ht_entityid_sg_get_num_entries_for_key(spritegroups, id);
//    for (int i = 0; i < num_spritegroups; i++) {
//        spritegroup_t* const sg = hashtable_entityid_spritegroup_get_by_index(spritegroups, id, i);
//        if (sg) {
//            //libdraw_update_sprite_ptr(g, id, sg);
//            libdraw_handle_frame_incr(g, id, sg);
//        }
//    }
//}

static void libdraw_handle_gamestate_flag(gamestate* const g) {
    massert(g, "gamestate is NULL");
    if (g->flag == GAMESTATE_FLAG_PLAYER_ANIM) {
        g->flag = GAMESTATE_FLAG_NPC_TURN;
        g->test_guard = false;
    } else if (g->flag == GAMESTATE_FLAG_NPC_ANIM) {
        g->entity_turn = g->hero_id; // Reset directly to hero
        g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
        g->turn_count++;
    }
}

static void libdraw_handle_dirty_entities(gamestate* const g) {
    massert(g, "gamestate is NULL");
    if (g->dirty_entities) {
        minfo("libdraw_handle_dirty_entities: creating new spritegroups");
        for (entityid i = g->new_entityid_begin; i < g->new_entityid_end; i++) {
            create_sg_byid(g, i);
        }
        g->dirty_entities = false;
        g->new_entityid_begin = ENTITYID_INVALID;
        g->new_entityid_end = ENTITYID_INVALID;
        g->frame_dirty = true;
    }
}

void libdraw_update_sprites(gamestate* const g) {
    if (g) {
        UpdateMusicStream(music);
        if (g->current_scene == SCENE_GAMEPLAY) {
            libdraw_handle_dirty_entities(g);
            for (entityid id = 0; id < g->next_entityid; id++) libdraw_update_sprite(g, id);
            libdraw_handle_gamestate_flag(g);
        }
    }
}

void libdraw_update_sprites_pre(gamestate* const g) {
    if (g) {
        UpdateMusicStream(music);
        if (g->current_scene == SCENE_GAMEPLAY) {
            libdraw_handle_dirty_entities(g);
            for (entityid id = 0; id < g->next_entityid; id++) {
                libdraw_update_sprite_pre(g, id);
            }
        }
    }
}

void libdraw_update_sprites_post(gamestate* const g) {
    if (g) {
        UpdateMusicStream(music);
        if (g->current_scene == SCENE_GAMEPLAY) {
            libdraw_handle_dirty_entities(g);
            if (g->framecount % ANIM_SPEED == 0) {
                for (entityid id = 0; id < g->next_entityid; id++) {
                    int num_spritegroups = ht_entityid_sg_get_num_entries_for_key(spritegroups, id);
                    for (int i = 0; i < num_spritegroups; i++) {
                        spritegroup_t* const sg = hashtable_entityid_spritegroup_get_by_index(spritegroups, id, i);
                        if (sg) {
                            sprite* const s = sg_get_current(sg);
                            massert(s, "sprite is NULL");
                            // attempt to grab the sprite's shadow
                            sprite* const s_shadow = sg_get_current_plus_one(sg);
                            g->frame_dirty = true;
                            if (s) {
                                sprite_incrframe(s);
                                if (s->num_loops >= 1) {
                                    sg->current = sg->default_anim;
                                    s->num_loops = 0;
                                }
                            }
                            if (s_shadow) {
                                sprite_incrframe(s_shadow);
                                if (s_shadow->num_loops >= 1) {
                                    sg->current = sg->default_anim;
                                    s_shadow->num_loops = 0;
                                }
                            }
                        }
                    }
                }
            }
            libdraw_handle_gamestate_flag(g);
        }
    }
}

static bool libdraw_draw_dungeon_floor(const gamestate* const g) {
    massert(g, "gamestate is NULL");
    dungeon_floor_t* const df = d_get_current_floor(g->d);
    massert(df, "dungeon_floor is NULL");
    int z = g->d->current_floor;
    draw_dungeon_tiles_2d(g, z, df);
    //draw_entities_2d(g, z, df, true); // dead entities
    //draw_entities_2d(g, z, df, false); // alive entities
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            draw_entities_2d_at(g, df, true, (vec3){x, y, z});
        }
    }
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            draw_entities_2d_at(g, df, false, (vec3){x, y, z});
        }
    }
    return true;
}

static void libdraw_draw_debug_panel(gamestate* const g) {
    massert(g, "gamestate is NULL");
    const Color bg = Fade((Color){0x66, 0x66, 0x66}, 0.8f), fg = WHITE;
    int w0 = g->debugpanel.w + g->debugpanel.pad_left + g->debugpanel.pad_right * 4;
    int h0 = g->debugpanel.h + g->debugpanel.pad_top + g->debugpanel.pad_bottom;
    int x1 = g->debugpanel.x + g->debugpanel.pad_left;
    int y1 = g->debugpanel.y + g->debugpanel.pad_top;
    DrawRectangle(g->debugpanel.x, g->debugpanel.y, w0, h0, bg);
    DrawText(g->debugpanel.buffer, x1, y1, g->debugpanel.font_size, fg);
}

static bool libdraw_draw_player_target_box(const gamestate* const g) {
    massert(g, "gamestate is NULL");
    const entityid id = g->hero_id;
    if (id == -1) {
        //merror("libdraw_draw_player_target_box: id is -1");
        return false;
    }
    direction_t dir = g_get_direction(g, id);
    vec3 loc = g_get_location(g, id);
    int x = loc.x + get_x_from_dir(dir);
    int y = loc.y + get_y_from_dir(dir);
    int ds = DEFAULT_TILE_SIZE;
    Color base_c = GREEN;
    float a = 0.25f;
    if (g->player_changing_direction) {
        a = 1.0f;
    }
    Color c = Fade(base_c, a);
    DrawRectangleLinesEx((Rectangle){x * ds, y * ds, ds, ds}, 1, c);
    return true;
}

static void libdraw_drawframe_2d(gamestate* const g) {
    //BeginShaderMode(shader_color_noise);
    //float time = (float)GetTime(); // Current time in seconds
    //SetShaderValue(shader_color_noise, GetShaderLocation(shader_color_noise, "time"), &time, SHADER_UNIFORM_FLOAT);
    libdraw_camera_lock_on(g);
    BeginMode2D(g->cam2d);
    ClearBackground(BLACK);
    //EndShaderMode();
    //if (!libdraw_camera_lock_on(g)) merror("failed to lock camera on hero");
    libdraw_draw_dungeon_floor(g);
    //if (!libdraw_draw_dungeon_floor(g)) merror("failed to draw dungeon floor");
    libdraw_draw_player_target_box(g);
    //if (!libdraw_draw_player_target_box(g)) merror("failed to draw player target box");
    //msuccess("libdraw_drawframe_2d: done");
    EndMode2D();
    draw_message_history(g);
    draw_message_box(g);
    draw_hud(g);
    draw_inventory_menu(g);
    handle_debug_panel(g);
    draw_version(g);
    if (g->display_help_menu) {
        draw_help_menu(g);
    }
    if (g->gameover) {
        draw_gameover_menu(g);
    }
}

static void libdraw_drawframe_2d_to_texture(gamestate* const g) {
    massert(g, "gamestate is NULL");
    BeginTextureMode(main_game_target_texture);
    libdraw_drawframe_2d(g);
    EndTextureMode();
}

static void libdraw_drawframe_2d_from_texture(gamestate* const g) {
    massert(g, "gamestate is NULL");
    //DrawTexturePro(title_target_texture.texture, target_src, target_dest, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTexturePro(main_game_target_texture.texture, target_src, (Rectangle){0, 0, g->windowwidth, g->windowheight}, (Vector2){0, 0}, 0.0f, WHITE);
}

static void draw_message_box(gamestate* g) {
    if (!g->msg_system.is_active || g->msg_system.count == 0) return;
    const char* prompt = "[A] Next";
    const char* msg = g->msg_system.messages[g->msg_system.index];
    Color message_bg = (Color){0x33, 0x33, 0x33, 0xff};
    //Color message_bg = (Color){0, 0, 0xff, 0xff};
    // copy the message to a temporary buffer
    int font_size = 20;
    char tmp[1024] = {0};
    snprintf(tmp, sizeof(tmp), "%s", msg);
    // Measure text (split into lines if needed)
    int measure = MeasureText(tmp, font_size);
    int text_width = measure;
    //int text_height = g->font_size;
    int text_height = font_size;
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
    DrawText(tmp, box.x + g->pad, box.y + g->pad, font_size, WHITE);
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
    strncat(g->debugpanel.buffer, tmp, sizeof(g->debugpanel.buffer) - strlen(g->debugpanel.buffer) - 1);
}

static inline void handle_debug_panel(gamestate* const g) {
    if (g->debugpanelon) {
        update_debug_panel(g);
        libdraw_draw_debug_panel(g);
    }
}

static void draw_help_menu(gamestate* const g) {
    if (!g->display_help_menu) return;
        // const char* help_text = g->help_menu_text;

#define HELP_TEXT_COUNT 17
    char* help_text[HELP_TEXT_COUNT] = {"# Help Menu",
                                        "",
                                        "## Controls",
                                        "-----------------------------------",
                                        "- Movement: q w e a d z x c",
                                        "- Attack: '",
                                        "- Pickup Item: /",
                                        "- Turn In Place: s [movement]",
                                        "- Wait: s s",
                                        "- Traverse: ]",
                                        "- Inventory: Esc",
                                        "- Nav Inventory: arrow up/down, w/x",
                                        "- Use/Equip/Unequip Item: enter, '",
                                        "- Drop Item: ]",
                                        "",
                                        "Press any key to close this menu.",
                                        ""};

    Color bg_color = Fade((Color){0x33, 0x33, 0x33}, 1.0f);
    int font_size = 10;
    int measure = MeasureText(help_text[11], font_size);
    int text_width = measure;
    int text_height = font_size;
    int x = (g->windowwidth - text_width) / 2;
    int y = (g->windowheight - text_height * HELP_TEXT_COUNT) / 2;
    // Draw background box
    DrawRectangle(x - 10, y - 10, text_width + 20, text_height * HELP_TEXT_COUNT + 20, bg_color);
    DrawRectangleLines(x - 10, y - 10, text_width + 20, text_height * HELP_TEXT_COUNT + 20, WHITE);
    // Draw each line of help text
    for (int i = 0; i < HELP_TEXT_COUNT; i++) {
        DrawText(help_text[i], x, y + i * text_height, font_size, WHITE);
    }
}

static void draw_gameover_menu(gamestate* const g) {
    if (!g->gameover) return;
    const char* gameover_text = "Game Over";
    const char* restart_text = "Press any key to try again";
    //Color bg_color = Fade((Color){0x33, 0x33, 0x33}, 1.0f);
    Color bg_color = BLACK;
    int font_size = 20;
    int measure = MeasureText(restart_text, font_size);
    int text_width = measure;
    int text_height = font_size;
    int x = (g->windowwidth - text_width) / 2;
    int y = (g->windowheight - text_height * 2) / 2;
    Rectangle rect = {x - 10, y - 10, text_width + 20, text_height * 2 + 20};
    // Draw background box
    DrawRectangle(x - 10, y - 10, text_width + 20, text_height * 2 + 20, bg_color);
    //DrawRectangleLines(x - 10, y - 10, text_width + 20, text_height * 2 + 20, WHITE);
    DrawRectangleLinesEx(rect, 2, RED);
    // Draw game over text
    //DrawText(gameover_text, x, y, font_size, WHITE);
    DrawText(gameover_text, x, y, font_size, RED);
    // Draw restart/quit prompt
    //DrawText(restart_text, x, y + text_height, font_size, WHITE);
    DrawText(restart_text, x, y + text_height, font_size, RED);
}

void libdraw_drawframe(gamestate* const g) {
    double start_time = GetTime();
    BeginDrawing();
    ClearBackground(RED);
    //BeginShaderMode(shader_psychedelic_0);
    //float time = (float)GetTime(); // Current time in seconds
    //SetShaderValue(shader_psychedelic_0, GetShaderLocation(shader_psychedelic_0, "time"), &time, SHADER_UNIFORM_FLOAT);
    //EndShaderMode();
    //minfo("drawframe current scene: %d", g->current_scene);
    //BeginTextureMode(target);
    //ClearBackground(BLUE);
    if (g->frame_dirty) {
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
    EndTextureMode();
    DrawTexturePro(target.texture, target_src, target_dest, target_origin, 0.0f, WHITE);
    EndDrawing();
    g->last_frame_time = GetTime() - start_time;
    g->framecount++;
    //g->frame_dirty = false;
}

static bool libdraw_unload_texture(int txkey) {
    massert(txkey >= 0, "txkey is invalid");
    if (txkey < 0 || txkey >= GAMESTATE_SIZEOFTEXINFOARRAY) {
        merror("libdraw_unload_texture: txkey out of bounds");
        return false;
    }
    UnloadTexture(txinfo[txkey].texture);
    txinfo[txkey].texture = (Texture2D){0};
    txinfo[txkey].contexts = 0;
    return true;
}

static void libdraw_unload_textures() {
    for (int i = 0; i < GAMESTATE_SIZEOFTEXINFOARRAY; i++) {
        libdraw_unload_texture(i);
    }
    UnloadRenderTexture(title_target_texture);
    UnloadRenderTexture(char_creation_target_texture);
    UnloadRenderTexture(main_game_target_texture);
    UnloadRenderTexture(target);
}

void libdraw_close_partial() {
    UnloadMusicStream(music);
    CloseAudioDevice();

    libdraw_unload_textures();
    libdraw_unload_shaders();
    //CloseWindow();
}

void libdraw_close() {
    libdraw_close_partial();
    CloseWindow();
}

static bool load_texture(int txkey, int ctxs, int frames, bool do_dither, char* path) {
    massert(path, "path is NULL");
    massert(txkey >= 0, "txkey is invalid");
    massert(ctxs >= 0, "contexts is invalid");
    massert(frames >= 0, "frames is invalid");
    //msuccess("load_texture: txkey: %d, contexts: %d, frames: %d, path: %s", txkey, ctxs, frames, path);
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
    massert(file, "textures.txt file is NULL");
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
        massert(txkey >= 0, "txkey is invalid");
        massert(contexts >= 0, "contexts is invalid");
        massert(frames >= 0, "frames is invalid");
        if (txkey < 0 || contexts < 0 || frames < 0) {
            merror("invalid line in textures.txt");
            continue;
        }
        load_texture(txkey, contexts, frames, do_dither, path);
    }
    fclose(file);
}

static void create_spritegroup(gamestate* const g, entityid id, int* keys, int num_keys, int offset_x, int offset_y, specifier_t spec) {
    massert(g, "gamestate is NULL");
    // can hold up to 32 sprites
    spritegroup_t* group = spritegroup_create(SPRITEGROUP_DEFAULT_SIZE);
    massert(group, "spritegroup is NULL");
    //disabling this check until dungeon_floor created
    dungeon_floor_t* df = d_get_current_floor(g->d);
    massert(df, "dungeon_floor is NULL");
    if (!df) {
        spritegroup_destroy(group);
        return;
    }
    int df_w = df->width, df_h = df->height;
    vec3 loc = g_get_location(g, id);
    massert(loc.x >= 0 && loc.x < df_w, "location x out of bounds: %d", loc.x);
    massert(loc.y >= 0 && loc.y < df_h, "location y out of bounds: %d", loc.y);
    if (loc.x < 0 || loc.x >= df_w || loc.y < 0 || loc.y >= df_h) {
        spritegroup_destroy(group);
        return;
    }
    for (int i = 0; i < num_keys; i++) {
        int k = keys[i];
        Texture2D* tex = &txinfo[k].texture;
        sprite* s = sprite_create(tex, txinfo[k].contexts, txinfo[k].num_frames);
        spritegroup_add(group, s);
    }
    group->id = id;
    sprite* s = spritegroup_get(group, 0);
    massert(s, "sprite is NULL");
    if (!s) {
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
    int offset_x = -12;
    int offset_y = -12;
    entitytype_t type = g_get_type(g, id);
    if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
        race_t race = g_get_race(g, id);
        switch (race) {
        case RACE_HUMAN:
            keys = TX_HUMAN_KEYS;
            num_keys = TX_HUMAN_COUNT;
            break;
        // Add cases for other races here
        case RACE_ORC:
            keys = TX_ORC_KEYS;
            num_keys = TX_ORC_COUNT;
            break;
        case RACE_ELF:
            keys = TX_ELF_KEYS;
            num_keys = TX_ELF_COUNT;
            break;
        case RACE_DWARF:
            keys = TX_DWARF_KEYS;
            num_keys = TX_DWARF_COUNT;
            break;
        case RACE_HALFLING:
            keys = TX_HALFLING_KEYS;
            num_keys = TX_HALFLING_COUNT;
            break;
        case RACE_GOBLIN:
            keys = TX_GOBLIN_KEYS;
            num_keys = TX_GOBLIN_COUNT;
            break;
        case RACE_WOLF:
            keys = TX_WOLF_KEYS;
            num_keys = TX_WOLF_COUNT;
            break;
        case RACE_BAT:
            keys = TX_BAT_KEYS;
            num_keys = TX_BAT_COUNT;
            break;
        case RACE_WARG:
            keys = TX_WARG_KEYS;
            num_keys = TX_WARG_COUNT;
            break;
        case RACE_GREEN_SLIME:
            keys = TX_GREEN_SLIME_KEYS;
            num_keys = TX_GREEN_SLIME_COUNT;
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
                num_keys = TX_LONG_SWORD_COUNT;
            } else if (weapon_type == WEAPON_DAGGER) {
                keys = TX_DAGGER_KEYS;
                num_keys = TX_DAGGER_COUNT;
            } else if (weapon_type == WEAPON_AXE) {
                keys = TX_AXE_KEYS;
                num_keys = TX_AXE_COUNT;
            } else if (weapon_type == WEAPON_BOW) {
                keys = TX_BOW_KEYS;
                num_keys = TX_BOW_COUNT;
            }
            create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
        } else if (item_type == ITEM_SHIELD) {
            keys = TX_BUCKLER_KEYS;
            num_keys = TX_BUCKLER_COUNT;
            create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
        } else if (item_type == ITEM_POTION) {
            potiontype potion_type = g_get_potiontype(g, id);
            if (potion_type == POTION_HEALTH_SMALL) {
                keys = TX_POTION_HP_SMALL_KEYS;
                num_keys = TX_POTION_HP_SMALL_COUNT;
            }
            //else if (potion_type == POTION_HEALTH_MEDIUM) {
            //    keys = TX_POTION_HP_MEDIUM_KEYS;
            //    num_keys = TX_POTION_HP_MEDIUM_COUNT;
            //} else if (potion_type == POTION_HEALTH_LARGE) {
            //    keys = TX_POTION_HP_LARGE_KEYS;
            //    num_keys = TX_POTION_HP_LARGE_COUNT;
            //}
            create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
        } else if (item_type == ITEM_WAND) {
            keys = TX_WAND_BASIC_KEYS;
            num_keys = TX_WAND_BASIC_COUNT;
            create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
        }
    }
}

static int get_total_ac(gamestate* const g, entityid id) {
    massert(g, "gamestate is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    // get the ac stat
    int stat_count = 0;
    int* stats = g_get_stats(g, id, &stat_count);
    massert(stats, "stats is NULL");
    int ac = stats[STATS_AC];
    // get the equipped shield
    entityid shield_id = g_get_equipment(g, id, EQUIP_SLOT_SHIELD);
    if (shield_id != ENTITYID_INVALID) {
        // get the shield's ac
        int shield_ac = g_get_ac(g, shield_id);
        massert(shield_ac >= 0, "shield_ac is negative");
        ac += shield_ac;
    }
    int dex = g_get_stat(g, id, STATS_DEX);
    int dex_bonus = bonus_calc(dex);
    ac += dex_bonus;
    return ac;
}

static void draw_hud(gamestate* const g) {
    massert(g, "gamestate is NULL");
    int turn = g->turn_count;
    int stat_count = 0;
    int* stats = g_get_stats(g, g->hero_id, &stat_count);
    massert(stats, "stats is NULL");
    int hp = stats[STATS_HP];
    int maxhp = stats[STATS_MAXHP];
    int level = stats[STATS_LEVEL];
    int xp = stats[STATS_XP];
    int next_level_xp = stats[STATS_NEXT_LEVEL_XP];
    int attack_bonus = stats[STATS_ATTACK_BONUS];
    int str = stats[STATS_STR];
    int con = stats[STATS_CON];
    int dex = stats[STATS_DEX];
    int ac = get_total_ac(g, g->hero_id);
    //loc_t loc = g_get_location(g, g->hero_id);
    //dungeon_floor_t* const df = d_get_current_floor(g->d);
    int floor = g->d->current_floor;
    //int font_size = g->font_size;
    int font_size = 20;

    //const char* room_name = df_get_room_name(df, loc);
    char buffer[1024] = {0};
    const char* format_str = "%s Lvl %d HP %d/%d Atk: %d AC: %d XP %d/%d STR: %d CON: %d DEX: %d Floor: %d Turn %d";
    //snprintf(buffer, sizeof(buffer), "%s Lvl %d HP %d/%d AC: %d XP %d Room: %s Turn %d", g_get_name(g, g->hero_id), level, hp, maxhp, ac, xp, room_name, turn);
    snprintf(buffer, sizeof(buffer), format_str, g_get_name(g, g->hero_id), level, hp, maxhp, attack_bonus, ac, xp, next_level_xp, str, con, dex, floor, turn);
    const Vector2 text_size = MeasureTextEx(GetFontDefault(), buffer, font_size, g->line_spacing);
    int box_w = text_size.x + g->pad;
    int box_h = text_size.y + g->pad;
    int box_x = 0;
    int box_y = 0;
    const Color bg = (Color){0x33, 0x33, 0x33, 0xFF}, fg = WHITE;
    DrawRectangleRec((Rectangle){box_x, box_y, box_w, box_h}, bg);
    DrawRectangleLinesEx((Rectangle){box_x, box_y, box_w, box_h}, 2, fg);
    // Calculate text position to center it within the box
    const float text_x = box_x + (box_w - text_size.x) / 2;
    const float text_y = box_y + (box_h - text_size.y) / 2;
    DrawTextEx(GetFontDefault(), buffer, (Vector2){text_x, text_y}, font_size, g->line_spacing, fg);
}

void libdraw_init_rest(gamestate* const g) {
    minfo("libdraw_init_rest: initializing rest of the libdraw");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    int w = DEFAULT_WIN_WIDTH, h = DEFAULT_WIN_HEIGHT;
    int x = w / 3, y = h / 3;
    minfo("libdraw_init_rest: window size: %dx%d", w, h);
    massert(w > 0 && h > 0, "window width or height is not set properly");
    g->windowwidth = w;
    g->windowheight = h;
    target = LoadRenderTexture(w, h);
    title_target_texture = LoadRenderTexture(w, h);
    char_creation_target_texture = LoadRenderTexture(w, h);
    main_game_target_texture = LoadRenderTexture(w, h);
    target_src = (Rectangle){0, 0, w, -h};
    target_dest = (Rectangle){0, 0, w, h};
    spritegroups = hashtable_entityid_spritegroup_create(DEFAULT_SPRITEGROUPS_SIZE);
    load_textures();
    calc_debugpanel_size(g);
    load_shaders();
    g->cam2d.offset = (Vector2){x, y};
    gamestate_set_debug_panel_pos_top_right(g);
    // set the camera target to the center of the dungeon
    dungeon_floor_t* const df = d_get_current_floor(g->d);
    massert(df, "dungeon_floor is NULL");
    int df_w = df->width, df_h = df->height;
    g->cam2d.target = (Vector2){df_w * DEFAULT_TILE_SIZE / 2.0f, df_h * DEFAULT_TILE_SIZE / 2.0f};
    draw_title_screen_to_texture(g, false);
    draw_character_creation_screen_to_texture(g);
    InitAudioDevice();
    // select a random indices for current music
    g->current_music_index = rand() % g->total_music_paths;
    // load the music stream from the selected path
    const char* music_path = g->music_file_paths[g->current_music_index];
    massert(music_path, "music_path is NULL");
    minfo("Loading music from path: %s", music_path);
    char real_music_path[1024] = {0};
    snprintf(real_music_path, sizeof(real_music_path), "%s%s", "audio/music/", music_path);
    music = LoadMusicStream(real_music_path);
    //SetMusicVolume(music, 0.50f); // Set initial music volume
    SetMusicVolume(music, DEFAULT_MUSIC_VOLUME); // Set initial music volume
    //SetMusicLooping(music, true); // Loop the music
    PlayMusicStream(music);
    //if (!libdraw_camera_lock_on(g)) merror("failed to lock camera on hero");
    msuccess("libdraw_init_rest: done");
}

void libdraw_init(gamestate* const g) {
    massert(g, "gamestate is NULL");
    int w = DEFAULT_WIN_WIDTH, h = DEFAULT_WIN_HEIGHT;
    //int x = w / 3;
    //int y = h / 3;
    const char* title = WINDOW_TITLE;
    char full_title[1024] = {0};
    snprintf(full_title, sizeof(full_title), "%s - %s", title, g->version);
    InitWindow(w, h, full_title);
    libdraw_init_rest(g);
}

static void draw_message_history(gamestate* const g) {
    massert(g, "gamestate is NULL");
    // if there are no messages in the message history, return
    if (g->msg_history.count == 0) return;
    int font_size = 20;
    int max_messages = 20;
    int x = 0;
    int y = 42;
    int current_count = 0;
    char tmp_buffer[2048] = {0};
    //Color message_bg = (Color){0x33, 0x33, 0x33, 0xff};
    Color message_bg = (Color){0x33, 0x33, 0x33, 200}; // semi-transparent
    //Color message_bg = (Color){0, 0, 0xff, 0xff};
    // instead of a placeholder message, we now need to actually draw the message history
    // we might only render the last N messages
    for (int i = g->msg_history.count - 1; i >= 0 && current_count < max_messages; i--) {
        strncat(tmp_buffer, g->msg_history.messages[i], sizeof(tmp_buffer) - strlen(tmp_buffer) - 1);
        strncat(tmp_buffer, "\n", sizeof(tmp_buffer) - strlen(tmp_buffer) - 1);
        current_count++;
    }
    // chop off the last newline
    if (strlen(tmp_buffer) > 0) tmp_buffer[strlen(tmp_buffer) - 1] = '\0';
    // Measure text (split into lines if needed)
    const Vector2 text_size = MeasureTextEx(GetFontDefault(), tmp_buffer, font_size, g->line_spacing);
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
    DrawTextEx(GetFontDefault(), tmp_buffer, (Vector2){text_x, text_y}, font_size, g->line_spacing, WHITE);
}

static void draw_inventory_menu(gamestate* const g) {
    massert(g, "gamestate is NULL");
    if (!g->display_inventory_menu) return;
    const char* menu_title = "Inventory Menu";
    // Parameters
    int box_pad = g->pad, section_gap = 8, item_list_pad = g->pad, font_size = 20, max_visible_items = 12, inventory_count = 0, scale = 8;
    // Measure title
    //Vector2 title_size = MeasureTextEx(GetFontDefault(), menu_title, g->font_size, g->line_spacing);
    Vector2 title_size = MeasureTextEx(GetFontDefault(), menu_title, font_size, g->line_spacing);
    // Menu box size
    float menu_width_percent = 0.75f, menu_height_percent = 0.75f;
    float menu_width = g->windowwidth * menu_width_percent, menu_height = g->windowheight * menu_height_percent;
    Rectangle menu_box = {.x = (g->windowwidth - menu_width) / 2.0f, .y = (g->windowheight - menu_height) / 4.0f, .width = menu_width, .height = menu_height};
    float title_x = menu_box.x + (menu_box.width - title_size.x) / 2.0f, title_y = menu_box.y + box_pad, half_width = (menu_box.width - section_gap) / 2.0f,
          half_height = menu_box.height - title_size.y - box_pad * 2.0f - box_pad;
    const char* info_title = "Item Info:";
    char info_text[256] = {0};
    spritegroup_t* sg = NULL;
    // Draw menu background and border
    //DrawRectangleRec(menu_box, (Color){0x33, 0x33, 0x33, 0xff});
    DrawRectangleRec(menu_box, (Color){0x33, 0x33, 0x33, 0x99});
    DrawRectangleLinesEx(menu_box, 2, WHITE);
    // Draw menu title centered at top
    //DrawTextEx(GetFontDefault(), menu_title, (Vector2){title_x, title_y}, g->font_size, g->line_spacing, WHITE);
    DrawText(menu_title, title_x, title_y, font_size, WHITE);
    // Partition into left/right halves (with gap)
    // Left box: inventory list
    // Right box: item info
    Rectangle left_box = {.x = menu_box.x + box_pad, .y = title_y + title_size.y + box_pad, .width = half_width - box_pad, .height = half_height},
              right_box = {.x = left_box.x + half_width + section_gap, .y = left_box.y, .width = half_width - box_pad * 2, .height = half_height};
    // Draw left and right boxes
    float item_y = left_box.y + item_list_pad, info_title_y = right_box.y + item_list_pad, info_text_y = info_title_y + font_size + 8;
    DrawRectangleRec(left_box, (Color){0x22, 0x22, 0x22, 0xff});
    DrawRectangleLinesEx(left_box, 2, WHITE);
    DrawRectangleRec(right_box, (Color){0x22, 0x22, 0x22, 0xff});
    DrawRectangleLinesEx(right_box, 2, WHITE);
    entityid* inventory = g_get_inventory(g, g->hero_id, &inventory_count);
    //for (int i = 0; i < hero->inventory_count && i < max_visible_items; i++) {
    for (int i = 0; i < g_get_inventory_count(g, g->hero_id) && i < max_visible_items; i++) {
        entityid item_id = inventory[i];
        if (item_id == ENTITYID_INVALID) continue;
        float item_x = left_box.x + item_list_pad;
        char item_display[128];
        bool is_equipped = g_is_equipped(g, g->hero_id, item_id);
        if (i == g->inventory_menu_selection) {
            snprintf(item_display, sizeof(item_display), "> %s", g_get_name(g, item_id));
        } else {
            snprintf(item_display, sizeof(item_display), "  %s", g_get_name(g, item_id));
        }
        if (is_equipped) {
            strncat(item_display, " (Equipped)", sizeof(item_display) - strlen(item_display) - 1);
        }
        DrawTextEx(GetFontDefault(), item_display, (Vector2){item_x, item_y}, font_size, g->line_spacing, WHITE);
        item_y += font_size + 4;
    }
    // Draw item info in right_box
    if (g->inventory_menu_selection >= 0 && g->inventory_menu_selection < inventory_count) {
        entityid item_id = inventory[g->inventory_menu_selection];
        const char* name = g_get_name(g, item_id);
        itemtype item_type = g_get_itemtype(g, item_id);
        if (g_has_damage(g, item_id)) {
            vec3 dmg_roll = g_get_damage(g, item_id);
            int n = dmg_roll.x;
            int sides = dmg_roll.y;
            int modifier = dmg_roll.z;
            if (modifier) {
                snprintf(info_text, sizeof(info_text), "%s\nType: %d\nDamage: %dd%d+%d", name, item_type, n, sides, modifier);
            } else {
                snprintf(info_text, sizeof(info_text), "%s\nType: %d\nDamage: %dd%d", name, item_type, n, sides);
            }
        } else if (g_has_ac(g, item_id)) {
            int ac = g_get_ac(g, item_id);
            snprintf(info_text, sizeof(info_text), "%s\nType: %d\nAC: %d", name, item_type, ac);
        } else {
            snprintf(info_text, sizeof(info_text), "%s\nType: %d", name, item_type);
        }
        sg = hashtable_entityid_spritegroup_get(spritegroups, item_id);
    } else {
        snprintf(info_text, sizeof(info_text), "Select an item to view details here.");
    }
    DrawTextEx(GetFontDefault(), info_title, (Vector2){right_box.x + item_list_pad, info_title_y}, font_size, g->line_spacing, (Color){0xaa, 0xaa, 0xaa, 0xff});
    DrawTextEx(GetFontDefault(), info_text, (Vector2){right_box.x + item_list_pad, info_text_y}, font_size, g->line_spacing, WHITE);
    if (sg) {
        sprite* s = sg_get_current(sg);
        if (s) {
            float sprite_width = s->width * scale, sprite_height = s->height * scale, sprite_margin = -6 * scale;
            // Anchor to top-right of right_box, account for margin
            const float sprite_x = right_box.x + right_box.width - sprite_margin - sprite_width, sprite_y = right_box.y + sprite_margin;
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

bool libdraw_windowshouldclose(const gamestate* const g) {
    massert(g, "gamestate is NULL");
    return g->do_quit;
}

static void draw_version(const gamestate* const g) {
    massert(g, "gamestate is NULL");
    const char* version = g->version;
    int font_size = 10;
    // also grab the current music track path
    const char* current_music_path = g->music_file_paths[g->current_music_index];
    char buffer[1024] = {0};
    snprintf(buffer, sizeof(buffer), "%s | Music: %s", version, current_music_path);
    const Vector2 text_size = MeasureTextEx(GetFontDefault(), buffer, font_size, 1.0f);
    const float x = g->windowwidth - text_size.x - g->pad;
    const float y = 0;
    DrawTextEx(GetFontDefault(), buffer, (Vector2){x, y}, font_size, 1.0f, WHITE);
}

static void draw_title_screen(gamestate* const g, bool show_menu) {
    massert(g, "gamestate is NULL");
    //const char* title_text = "project.rpg";
    // Space between title texts
    Color active_color = WHITE, disabled_color = {0x99, 0x99, 0x99, 0xFF}, selection_color;
    const char* menu_text[3] = {"New Game", "Continue", "Settings"};
    const char *title_text_0 = "project.", *title_text_1 = "rpg", *version_text = g->version, *start_text = "Press any key to begin";
    char buffer[1024] = {0};
    Color title_text_0_color = {0x66, 0x66, 0x66, 0xFF}, title_text_1_color = {0xFF, 0xFF, 0xFF, 0xFF};
    int sm_font_size = 20, font_size = 80, measure = MeasureText(title_text_0, font_size), start_measure = MeasureText(start_text, sm_font_size), padding = 10,
        version_measure = MeasureText(version_text, sm_font_size);
    float x = (g->windowwidth - measure) / 2.0f, y = (g->windowheight - font_size * 2) / 2.0f, start_x = (g->windowwidth - start_measure) / 2.0f,
          title_text_1_x = x + measure + padding, version_x = (g->windowwidth - version_measure) / 2.0f, version_y = y + font_size + 10,
          start_y = y + font_size * 1 + 20 + sm_font_size;
    ClearBackground(BLACK);
    DrawText(title_text_0, x, y, font_size, title_text_0_color);
    DrawText(title_text_1, title_text_1_x, y, font_size, title_text_1_color);
    DrawText(version_text, version_x, version_y, sm_font_size, WHITE);
    if (!show_menu) {
        DrawText(start_text, start_x, start_y, sm_font_size, WHITE);
        return;
    }
    // If show_menu is true, draw the new game, continue, options selection text
    int menu_count = sizeof(menu_text) / sizeof(menu_text[0]), menu_spacing = 10, current_selection_index = g->title_screen_selection;
    for (int i = 0; i < menu_count; i++) {
        bzero(buffer, sizeof(buffer));
        float menu_x = (g->windowwidth - MeasureText(menu_text[i], sm_font_size)) / 2.0f, menu_y = start_y + (i * (sm_font_size + menu_spacing));
        if (i == current_selection_index)
            snprintf(buffer, sizeof(buffer), "> %s", menu_text[i]);
        else
            snprintf(buffer, sizeof(buffer), "  %s", menu_text[i]);
        selection_color = i == 0 ? active_color : disabled_color; // First item is always active
        DrawText(buffer, menu_x, menu_y, sm_font_size, selection_color);
    }
}

static void draw_title_screen_to_texture(gamestate* const g, bool show_menu) {
    massert(g, "gamestate is NULL");
    BeginTextureMode(title_target_texture);
    draw_title_screen(g, show_menu);
    handle_debug_panel(g);
    EndTextureMode();
}

static void draw_title_screen_from_texture(gamestate* const g) {
    massert(g, "gamestate is NULL");
    // Draw the title screen texture
    DrawTexturePro(title_target_texture.texture, target_src, target_dest, (Vector2){0, 0}, 0.0f, WHITE);
}

static void draw_character_creation_screen_to_texture(gamestate* const g) {
    massert(g, "gamestate is NULL");
    BeginTextureMode(char_creation_target_texture);
    draw_character_creation_screen(g);
    handle_debug_panel(g);
    EndTextureMode();
}

static void draw_character_creation_screen_from_texture(gamestate* const g) {
    massert(g, "gamestate is NULL");
    // Draw the character creation screen texture
    DrawTexturePro(char_creation_target_texture.texture, target_src, target_dest, (Vector2){0, 0}, 0.0f, WHITE);
}

static void draw_character_creation_screen(gamestate* const g) {
    massert(g, "gamestate is NULL");
    const char* title_text = "Character Creation";
    const char* stats_fmt[] = {
        "Name: %s", "Race: %s", "Hitdie: %d", 
        "Strength: %d", "Dexterity: %d", "Constitution: %d"
    };
    const char* remaining_text[] = {
        "Press SPACE to re-roll stats", 
        "Press LEFT/RIGHT to change race (unavailable for now)", 
        "Press UP/DOWN to change class (unavailable for now)", 
        "Press ENTER to confirm"
    };
    
    int font_size = 40;
    int cx = g->windowwidth / 2;
    int sy = g->windowheight / 4;
    int x = cx;
    int y = sy;
    char buffer[2048] = {0};

    ClearBackground(BLACK);
    DrawText(title_text, x, y, font_size, WHITE);
    y += font_size;
    font_size = 20;

    // Draw character stats
    for (int i = 0; i < sizeof(stats_fmt)/sizeof(stats_fmt[0]); i++) {
        snprintf(buffer, sizeof(buffer), stats_fmt[i], 
            i == 0 ? g->chara_creation.name :
            i == 1 ? get_race_str(g->chara_creation.race) :
            i == 2 ? g->chara_creation.hitdie :
            i == 3 ? g->chara_creation.strength :
            i == 4 ? g->chara_creation.dexterity :
            g->chara_creation.constitution);
        DrawText(buffer, x, y, font_size, WHITE);
        y += font_size + 4;
    }

    // Draw sprite placeholder
    DrawRectangleLinesEx((Rectangle){cx - 210, sy, 200, 200}, 4, RED);

    // Draw instructions
    y += font_size + 8;
    for (int i = 0; i < sizeof(remaining_text)/sizeof(remaining_text[0]); i++) {
        DrawText(remaining_text[i], x, y, font_size, WHITE);
        y += font_size + 4;
    }
}
