#include "direction.h"
#include "dungeon_tile_type.h"
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
#define DEFAULT_WIN_WIDTH 960
#define DEFAULT_WIN_HEIGHT 540
//#define DEFAULT_WIN_WIDTH 1920
//#define DEFAULT_WIN_HEIGHT 1080
#define SPRITEGROUP_DEFAULT_SIZE 32
#define DEFAULT_TILE_SIZE_SCALED 32

hashtable_entityid_spritegroup_t* spritegroups = NULL;
textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];

Shader shader_grayscale = {0};
Shader shader_tile_glow = {0};

RenderTexture2D target = {0};
Rectangle target_src = {0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};
Rectangle target_dest = {0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};
Vector2 target_origin = {0, 0};
Vector2 zero_vec = {0, 0};

int ANIM_SPEED = DEFAULT_ANIM_SPEED;

static inline void draw_hud(gamestate* const g);
static void draw_message_history(gamestate* const g);
static bool draw_dungeon_floor_tile(const gamestate* const g, dungeon_floor_t* const df, int x, int y);
static bool draw_dungeon_tiles_2d(const gamestate* g, dungeon_floor_t* df);
static void draw_sprite_and_shadow(const gamestate* const g, entityid id);
static bool draw_entities_2d(const gamestate* g, dungeon_floor_t* df, bool dead);
static void load_shaders();
static void libdraw_unload_shaders();
static bool libdraw_camera_lock_on(gamestate* const g);
static bool libdraw_check_default_animations(gamestate* const g);
static void libdraw_set_sg_is_damaged(gamestate* const g, entity_t* const e, spritegroup_t* const sg);
static void libdraw_set_sg_is_dead(gamestate* const g, entity_t* const e, spritegroup_t* const sg);
static void libdraw_set_sg_is_attacking(gamestate* const g, entity_t* const e, spritegroup_t* const sg);
static void libdraw_set_sg_is_blocking(gamestate* const g, entity_t* const e, spritegroup_t* const sg);
static void libdraw_set_sg_block_success(gamestate* const g, entity_t* const e, spritegroup_t* const sg);
static void libdraw_update_sprite_attack(gamestate* const g, entity_t* e, spritegroup_t* sg);
static void libdraw_update_sprite_position(gamestate* const g, spritegroup_t* sg, entity_t* e);
static void libdraw_update_sprite_context_ptr(gamestate* const g, spritegroup_t* group, direction_t dir);
static void libdraw_update_sprite_ptr(gamestate* const g, entity* e, spritegroup_t* sg);
static void libdraw_handle_frame_incr(gamestate* const g, spritegroup_t* const sg);
static void libdraw_update_sprite(gamestate* const g, entityid id);
static inline void libdraw_handle_gamestate_flag(gamestate* const g);
static bool libdraw_draw_dungeon_floor(const gamestate* const g);
static void libdraw_draw_debug_panel(gamestate* const g);
static bool libdraw_draw_player_target_box(const gamestate* const g);
static void libdraw_drawframe_2d(gamestate* const g);
static void draw_message_box(gamestate* g);
static bool libdraw_unload_texture(int txkey);
static void libdraw_unload_textures();
static bool load_texture(int txkey, int ctxs, int frames, bool do_dither, char* path);
static void load_textures();
static void create_spritegroup(gamestate* const g,
                               entityid id,
                               int* keys,
                               int num_keys,
                               int offset_x,
                               int offset_y,
                               specifier_t spec);
static void calc_debugpanel_size(gamestate* const g);
static void create_sg_byid(gamestate* const g, entityid id);

static bool draw_dungeon_floor_tile(const gamestate* const g, dungeon_floor_t* const df, int x, int y) {
    if (!g || !df) {
        merror("libdraw_draw_dungeon_floor_tile: gamestate or dungeon_floor is NULL");
        return false;
    }
    if (x < 0 || x >= df->width || y < 0 || y >= df->height) {
        return false;
    }
    tile_t* tile = dungeon_floor_tile_at(df, x, y);
    if (!tile) {
        return false;
    }
    // check if the tile type is none
    if (tile->type == TILE_NONE) {
        return true;
    }
    // just draw the tile itself
    // tile values in get_txkey_for_tiletype.h
    int txkey = get_txkey_for_tiletype(tile->type);
    if (txkey < 0) {
        return false;
    }
    Texture2D* texture = &txinfo[txkey].texture;
    if (texture->id <= 0) {
        return false;
    }
    // atm hard-coding the size of the new tiles and their destinations
    const int offset_x = -12;
    const int offset_y = -12;
    const int dx = x * DEFAULT_TILE_SIZE + offset_x;
    const int dy = y * DEFAULT_TILE_SIZE + offset_y;
    Rectangle src = (Rectangle){0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
    Rectangle dest = (Rectangle){dx, dy, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
    DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
    // draw the pressure plate if it exists
    if (tile->has_pressure_plate) {
        const int txkey2 = tile->pressure_plate_up_tx_key;
        if (txkey2 < 0) {
            return false;
        }
        Texture2D* texture = &txinfo[txkey2].texture;
        if (texture->id <= 0) {
            return false;
        }
        DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
    }
    // draw the wall switch
    if (tile->has_wall_switch) {
        int txkey = -1;
        if (tile->wall_switch_on) {
            txkey = tile->wall_switch_down_tx_key;
        } else {
            txkey = tile->wall_switch_up_tx_key;
        }
        if (txkey < 0) {
            return false;
        }
        Texture2D* texture = &txinfo[txkey].texture;
        if (texture->id <= 0) {
            return false;
        }
        DrawTexturePro(*texture, src, dest, (Vector2){0, 0}, 0, WHITE);
    }
    return true;
}

static bool draw_dungeon_tiles_2d(const gamestate* g, dungeon_floor_t* df) {
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            if (df_tile_is_wall(df, x, y)) {
                continue;
            }
            draw_dungeon_floor_tile(g, df, x, y);
        }
    }
    return true;
}

static void draw_sprite_and_shadow(const gamestate* const g, entityid id) {
    massert(g, "draw_sprite_and_shadow: gamestate is NULL");
    massert(id != -1, "draw_sprite_and_shadow: id is -1");
    entity* e = em_get(g->entitymap, id);
    if (!e) {
        merror("draw_sprite_and_shadow: entity not found: id %d", id);
        return;
    }
    spritegroup_t* sg = hashtable_entityid_spritegroup_get(spritegroups, id);
    if (!sg) {
        merror("draw_sprite_and_shadow: spritegroup not found: id %d", id);
        return;
    }
    sprite* s = spritegroup_get(sg, sg->current);
    if (!s) {
        merror("draw_sprite_and_shadow: sprite not found at current %d", sg->current);
        return;
    }
    Rectangle dest = {sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};
    if (e->type == ENTITY_PLAYER || e->type == ENTITY_NPC) {
        sprite* shadow = spritegroup_get(sg, sg->current + 1);
        if (shadow) {
            DrawTexturePro(*shadow->texture, shadow->src, dest, (Vector2){0, 0}, 0, WHITE);
        }
    }
    // check for a shield
    entityid shield_id = e->shield;
    spritegroup_t* shield_sg = NULL;
    sprite* shield_front_s = NULL;
    sprite* shield_back_s = NULL;

    //if (shield_id != -1 && g->test_guard) {
    if (shield_id != -1 && e->block_success) {
        shield_sg = hashtable_entityid_spritegroup_get(spritegroups, shield_id);
        if (shield_sg) {
            shield_front_s = spritegroup_get(shield_sg, SG_ANIM_BUCKLER_SUCCESS_FRONT);
            shield_back_s = spritegroup_get(shield_sg, SG_ANIM_BUCKLER_SUCCESS_BACK);
        }
    } else if (shield_id != -1 && e->is_blocking) {
        shield_sg = hashtable_entityid_spritegroup_get(spritegroups, shield_id);
        if (shield_sg) {
            //if (e->block_success) {
            //    shield_front_s = spritegroup_get(shield_sg, SG_ANIM_BUCKLER_FRONT);
            //    shield_back_s = spritegroup_get(shield_sg, SG_ANIM_BUCKLER_BACK);

            //} else {

            shield_front_s = spritegroup_get(shield_sg, SG_ANIM_BUCKLER_FRONT);
            shield_back_s = spritegroup_get(shield_sg, SG_ANIM_BUCKLER_BACK);

            //shield_front_s = spritegroup_get(shield_sg, SG_ANIM_BUCKLER_SUCCESS_FRONT);
            //shield_back_s = spritegroup_get(shield_sg, SG_ANIM_BUCKLER_SUCCESS_BACK);
            //}
        }
    }

    if (shield_back_s) {
        DrawTexturePro(*shield_back_s->texture, shield_back_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
    }

    // Draw sprite on top
    DrawTexturePro(*s->texture, s->src, dest, zero_vec, 0, WHITE);
    if (shield_front_s) {
        DrawTexturePro(*shield_front_s->texture, shield_front_s->src, sg->dest, (Vector2){0, 0}, 0, WHITE);
    }
}

static bool draw_entities_2d(const gamestate* g, dungeon_floor_t* df, bool dead) {
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* tile = dungeon_floor_tile_at(df, x, y);
            if (!tile || dungeon_tile_is_wall(tile->type)) {
                continue;
            }
            for (int i = 0; i < tile_entity_count(tile); i++) {
                entityid id = tile_get_entity(tile, i);
                entity* e = em_get(g->entitymap, id);
                if (e && e->is_dead == dead) {
                    draw_sprite_and_shadow(g, id);
                }
            }
        }
    }
    return true;
}

static bool draw_wall_tiles_2d(const gamestate* g, dungeon_floor_t* df) {
    for (int y = 0; y < df->height; y++) {
        for (int x = 0; x < df->width; x++) {
            tile_t* tile = dungeon_floor_tile_at(df, x, y);
            if (!tile) {
                return false;
            }
            if (!dungeon_tile_is_wall(tile->type)) {
                continue;
            }
            draw_dungeon_floor_tile(g, df, x, y);
        }
    }
    return true;
}

static void load_shaders() {
    //shader_grayscale = LoadShader(0, "grayscale.frag"); // No vertex shader needed
    //shader_tile_glow = LoadShader(0, "glow.frag");
    //shader_tile_glow = LoadShader(0, "psychedelic_ripple.frag");
}

static void libdraw_unload_shaders() {
    UnloadShader(shader_grayscale);
    UnloadShader(shader_tile_glow);
}

static bool libdraw_camera_lock_on(gamestate* const g) {
    massert(g, "libdraw_camera_lock_on: gamestate is NULL");
    if (!g->cam_lockon) {
        return false;
    }
    spritegroup_t* grp = hashtable_entityid_spritegroup_get(spritegroups, g->hero_id);
    massert(grp, "libdraw_camera_lock_on: spritegroup is NULL");
    g->cam2d.target = (Vector2){grp->dest.x, grp->dest.y};
    return true;
}

static bool libdraw_check_default_animations(gamestate* const g) {
    massert(g, "libdraw_check_default_animations: gamestate is NULL");
    for (int i = 0; i < g->index_entityids; i++) {
        const entityid id = g->entityids[i];
        spritegroup_t* const sg = hashtable_entityid_spritegroup_get(spritegroups, id);
        if (sg) {
            if (sg->current != sg->default_anim) {
                return false;
            }
        }
    }
    return true;
}

static void libdraw_set_sg_is_damaged(gamestate* const g, entity_t* const e, spritegroup_t* const sg) {
    massert(g, "libdraw_set_sg_is_damaged: gamestate is NULL");
    if (!e || !sg) {
        merror("libdraw_set_sg_is_damaged: entity or spritegroup is NULL");
        return;
    }
    if (e->race == RACE_HUMAN) {
        sg->current = SPRITEGROUP_ANIM_HUMAN_DMG;
    } else if (e->race == RACE_ORC) {
        sg->current = SPRITEGROUP_ANIM_ORC_DMG;
    }
    e->is_damaged = false;
}

static void libdraw_set_sg_is_dead(gamestate* const g, entity_t* const e, spritegroup_t* const sg) {
    if (!e || !sg) {
        merror("libdraw_set_sg_is_dead: entity or spritegroup is NULL");
        return;
    }
    if (e->race == RACE_HUMAN) {
        sg->current = SPRITEGROUP_ANIM_HUMAN_SPINDIE;
        sg->default_anim = SPRITEGROUP_ANIM_HUMAN_SPINDIE;
        spritegroup_set_stop_on_last_frame(sg, true);
    } else if (e->race == RACE_ORC) {
        sg->current = SPRITEGROUP_ANIM_ORC_DIE;
        sg->default_anim = SPRITEGROUP_ANIM_ORC_DIE;
        spritegroup_set_stop_on_last_frame(sg, true);
    }
}

static void libdraw_set_sg_is_attacking(gamestate* const g, entity_t* const e, spritegroup_t* const sg) {
    massert(g, "libdraw_set_sg_is_attacking: gamestate is NULL");
    massert(e, "libdraw_set_sg_is_attacking: entity is NULL");
    massert(sg, "libdraw_set_sg_is_attacking: spritegroup is NULL");
    if (e->race == RACE_HUMAN) {
        sg->current = SPRITEGROUP_ANIM_HUMAN_ATTACK;
    } else if (e->race == RACE_ORC) {
        sg->current = SPRITEGROUP_ANIM_ORC_ATTACK;
    }
    e->is_attacking = false;
}

static void libdraw_set_sg_is_blocking(gamestate* const g, entity_t* const e, spritegroup_t* const sg) {
    massert(g, "libdraw_set_sg_is_blocking: gamestate is NULL");
    massert(e, "libdraw_set_sg_is_blocking: entity is NULL");
    massert(sg, "libdraw_set_sg_is_blocking: spritegroup is NULL");
    if (e->race == RACE_HUMAN) {
        sg->current = SPRITEGROUP_ANIM_HUMAN_GUARD;
        if (e->shield != ENTITYID_INVALID) {
            spritegroup_t* shield_sg = hashtable_entityid_spritegroup_get(spritegroups, e->shield);
            if (shield_sg) {
                int player_ctx = sg->sprites[sg->current]->currentcontext;
                spritegroup_setcontexts(shield_sg, player_ctx);

                shield_sg->current = SG_ANIM_BUCKLER_FRONT;
            }
        }
    }
    //e->is_blocking = false;
    g->test_guard = false;
}

static void libdraw_set_sg_block_success(gamestate* const g, entity_t* const e, spritegroup_t* const sg) {
    massert(g, "libdraw_set_sg_block_success: gamestate is NULL");
    massert(e, "libdraw_set_sg_block_success: entity is NULL");
    massert(sg, "libdraw_set_sg_block_success: spritegroup is NULL");
    if (e->race == RACE_HUMAN) {
        sg->current = SPRITEGROUP_ANIM_HUMAN_GUARD_SUCCESS;
        if (e->shield != ENTITYID_INVALID) {
            spritegroup_t* shield_sg = hashtable_entityid_spritegroup_get(spritegroups, e->shield);
            if (shield_sg) {
                int player_ctx = sg->sprites[sg->current]->currentcontext;
                spritegroup_setcontexts(shield_sg, player_ctx);
                shield_sg->current = SG_ANIM_BUCKLER_SUCCESS_FRONT;
            }
        }
    }
    //e->is_blocking = false;
    //g->test_guard = false;
}

static void libdraw_update_sprite_attack(gamestate* const g, entity_t* e, spritegroup_t* sg) {
    massert(g, "libdraw_update_sprite_attack: gamestate is NULL");
    massert(e, "libdraw_update_sprite_attack: entity is NULL");
    massert(sg, "libdraw_update_sprite_attack: spritegroup is NULL");
    if (e->is_attacking) {
        libdraw_set_sg_is_attacking(g, e, sg);
        //} else if (e->is_blocking) {
    } else if (e->block_success) {
        libdraw_set_sg_block_success(g, e, sg);
    } else if (g->test_guard) {
        libdraw_set_sg_is_blocking(g, e, sg);
    } else if (e->is_damaged) {
        libdraw_set_sg_is_damaged(g, e, sg);
    } else if (e->is_dead) {
        libdraw_set_sg_is_dead(g, e, sg);
    }
}

static void libdraw_update_sprite_position(gamestate* const g, spritegroup_t* sg, entity_t* e) {
    if (!sg || !e) {
        merror("libdraw_update_sprite_position: spritegroup or entity is NULL");
        return;
    }
    if (e->sprite_move_x != 0 || e->sprite_move_y != 0) {
        sg->move.x = e->sprite_move_x;
        sg->move.y = e->sprite_move_y;
        e->sprite_move_x = 0;
        e->sprite_move_y = 0;
        if (e->type == ENTITY_PLAYER || e->type == ENTITY_NPC) {
            if (e->race == RACE_HUMAN) {
                sg->current = SPRITEGROUP_ANIM_HUMAN_WALK; // Set animation
            } else if (e->race == RACE_ORC) {
                sg->current = SPRITEGROUP_ANIM_ORC_WALK; // Set animation
            } // else no sprite animation update
        }
    }
}

static void libdraw_update_sprite_context_ptr(gamestate* const g, spritegroup_t* group, direction_t dir) {
    massert(g, "libdraw_update_sprite_context: gamestate is NULL");
    massert(group != NULL, "libdraw_update_sprite_context: group is NULL");
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
    if (ctx != old_ctx) {
        minfo("libdraw_update_sprite_context: ctx changed from %d to %d", old_ctx, ctx);
    }
    spritegroup_setcontexts(group, ctx);
}

static void libdraw_update_sprite_ptr(gamestate* const g, entity* e, spritegroup_t* sg) {
    if (!e || !sg) {
        merror("libdraw_update_sprite_ptr: entity or spritegroup is NULL");
        return;
    }
    if (e->is_dead && !spritegroup_is_animating(sg)) {
        return;
    }

    if (e->do_update) {
        libdraw_update_sprite_context_ptr(g, sg, e->direction);
        e->do_update = false;
    }

    // Copy movement intent from e->sprite_move_x/y if present
    libdraw_update_sprite_position(g, sg, e);
    libdraw_update_sprite_attack(g, e, sg);
    // Update movement as long as sg->move.x/y is non-zero
    spritegroup_update_dest(sg);
    // Snap to the tile position only when movement is fully complete
    spritegroup_snap_dest(sg, e->x, e->y);
}

static void libdraw_handle_frame_incr(gamestate* const g, spritegroup_t* const sg) {
    if (!g) {
        merror("libdraw_handle_frame_incr: gamestate is NULL");
        return;
    }
    sprite* const s = sg->sprites[sg->current];
    if (!s) {
        merror("libdraw_update_sprite: sprite is NULL");
        return;
    }
    // attempt to grab the sprite's shadow
    sprite* const s_shadow = sg->sprites[sg->current + 1];
    if (g->framecount % ANIM_SPEED == 0) {
        sprite_incrframe(s);
        if (s->num_loops >= 1) {
            sg->current = sg->default_anim;
            s->num_loops = 0;
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

static void libdraw_update_sprite(gamestate* const g, entityid id) {
    if (!g) return;
    entity* const e = em_get(g->entitymap, id);
    if (!e) {
        return;
    }
    int num_spritegroups = ht_entityid_sg_get_num_entries_for_key(spritegroups, id);
    for (int i = 0; i < num_spritegroups; i++) {
        spritegroup_t* const sg = hashtable_entityid_spritegroup_get_by_index(spritegroups, id, i);
        if (sg) {
            libdraw_update_sprite_ptr(g, e, sg);
            libdraw_handle_frame_incr(g, sg);
        }
    }
}

static inline void libdraw_handle_gamestate_flag(gamestate* const g) {
    const bool done = libdraw_check_default_animations(g);
    if (done) {
        if (g->flag == GAMESTATE_FLAG_PLAYER_ANIM) {
            g->flag = GAMESTATE_FLAG_NPC_TURN;
            g->test_guard = false;
        } else if (g->flag == GAMESTATE_FLAG_NPC_ANIM) {
            g->entity_turn = g->hero_id; // Reset directly to hero
            g->flag = GAMESTATE_FLAG_PLAYER_INPUT;
            g->turn_count++;
        }
    }
}

void libdraw_update_sprites(gamestate* const g) {
    if (!g) {
        merror("libdraw_update_sprites: gamestate is NULL");
        return;
    }
    // for each entityid in our entitymap, update the spritegroup
    for (int i = 0; i < g->index_entityids; i++) {
        const entityid id = g->entityids[i];
        libdraw_update_sprite(g, id);
    }
    libdraw_handle_gamestate_flag(g);
}

static bool libdraw_draw_dungeon_floor(const gamestate* const g) {
    if (!g) {
        merror("libdraw_draw_dungeon_floor: gamestate is NULL");
        return false;
    }
    dungeon_floor_t* const df = dungeon_get_current_floor(g->dungeon);
    if (!df) {
        merror("libdraw_draw_dungeon_floor: dungeon_floor is NULL");
        return false;
    }
    draw_dungeon_tiles_2d(g, df);
    draw_entities_2d(g, df, true); // dead entities
    draw_entities_2d(g, df, false); // alive entities
    draw_wall_tiles_2d(g, df);
    return true;
}

static void libdraw_draw_debug_panel(gamestate* const g) {
    if (!g) {
        merror("libdraw_draw_debug_panel: gamestate is NULL");
        return;
    }
    const Color bg = Fade((Color){0x66, 0x66, 0x66}, 0.8f);
    const Color fg = WHITE;
    const int w0 = g->debugpanel.w + g->debugpanel.pad_left + g->debugpanel.pad_right * 4;
    const int h0 = g->debugpanel.h + g->debugpanel.pad_top + g->debugpanel.pad_bottom;
    const int x1 = g->debugpanel.x + g->debugpanel.pad_left;
    const int y1 = g->debugpanel.y + g->debugpanel.pad_top;
    DrawRectangle(g->debugpanel.x, g->debugpanel.y, w0, h0, bg);
    DrawText(g->debugpanel.buffer, x1, y1, g->debugpanel.font_size, fg);
}

static bool libdraw_draw_player_target_box(const gamestate* const g) {
    if (!g) {
        merror("libdraw_draw_player_target_box: gamestate is NULL");
        return false;
    }
    const entityid id = g->hero_id;
    if (id == -1) {
        merror("libdraw_draw_player_target_box: id is -1");
        return false;
    }
    entity* const e = em_get(g->entitymap, id);
    if (!e) {
        return false;
    }
    direction_t dir = e->direction;
    int x = e->x;
    int y = e->y;
    x += get_x_from_dir(dir);
    y += get_y_from_dir(dir);
    const int ds = DEFAULT_TILE_SIZE;
    const Color base_c = GREEN;
    const float a = 0.5f;
    const Color c = Fade(base_c, a);
    DrawRectangleLinesEx((Rectangle){x * ds, y * ds, ds, ds}, 1, c);
    return true;
}

static void libdraw_drawframe_2d(gamestate* const g) {
    BeginMode2D(g->cam2d);
    ClearBackground(BLACK);
    if (!libdraw_camera_lock_on(g)) {
        merror("libdraw_drawframe: failed to lock camera on hero");
    }
    if (!libdraw_draw_dungeon_floor(g)) {
        merror("libdraw_drawframe: failed to draw dungeon floor");
    }
    if (!libdraw_draw_player_target_box(g)) {
        merror("libdraw_drawframe: failed to draw player target box");
    }
    EndMode2D();
}

static void draw_message_box(gamestate* g) {
    if (!g->msg_system.is_active || g->msg_system.count == 0) {
        return;
    }
    const char* prompt = "[A] Next";
    const char* msg = g->msg_system.messages[g->msg_system.index];
    const Color message_bg = Fade((Color){0x33, 0x33, 0x33, 0xff}, 0.8f);
    const int font_size = 30;
    const int pad = 40; // Inner padding (text <-> box edges)
    const float line_spacing = 1.0f;
    // Measure text (split into lines if needed)
    const Vector2 text_size = MeasureTextEx(GetFontDefault(), msg, font_size, line_spacing);
    // Calculate centered box position
    const Rectangle box = {.x = (g->windowwidth - text_size.x) / 2 - pad, // Center X
                           .y = (g->windowheight - text_size.y) / 2 - pad, // Center Y
                           .width = text_size.x + pad * 2,
                           .height = text_size.y + pad * 2};
    // Draw box (semi-transparent black with white border)
    DrawRectangleRec(box, message_bg);
    DrawRectangleLinesEx(box, 2, WHITE);
    // Draw text (centered in box)
    DrawTextEx(GetFontDefault(), msg, (Vector2){box.x + pad, box.y + pad}, font_size, line_spacing, WHITE);
    // Show "Next" prompt if more messages exist
    if (g->msg_system.count > 1) {
        int prompt_font_size = 10;
        int prompt_offset = 10; // Offset from box edges
        //int prompt_width = MeasureText(prompt, 10);
        Vector2 prompt_size = MeasureTextEx(GetFontDefault(), prompt, prompt_font_size, 1.0f);
        DrawText(prompt,
                 box.x + box.width - prompt_size.x - prompt_offset, // Right-align in box
                 box.y + box.height - prompt_size.y - prompt_offset, // Bottom of box
                 prompt_font_size,
                 WHITE);
    }
}

void libdraw_drawframe(gamestate* const g) {
    double start_time = GetTime();
    BeginDrawing();
    ClearBackground(WHITE);
    BeginTextureMode(target);
    libdraw_drawframe_2d(g);
    EndTextureMode();
    //BeginShaderMode(shader_grayscale);
    //float time = (float)GetTime(); // Current time in seconds
    //SetShaderValue(shader_grayscale, GetShaderLocation(shader_grayscale, "time"), &time, SHADER_UNIFORM_FLOAT);
    //EndShaderMode();
    DrawTexturePro(target.texture, target_src, target_dest, target_origin, 0.0f, WHITE);
    draw_message_box(g);
    draw_message_history(g);
    draw_hud(g);
    if (g->debugpanelon) {
        // concat a string onto the end of the debug panel message
        char tmp[1024] = {0};
        entityid hero_id = g->hero_id;
        entity* e = em_get(g->entitymap, hero_id);
        massert(e, "libdraw_drawframe: entity is NULL");
        entityid shield_id = e->shield;
        if (shield_id != -1) {
            spritegroup_t* sg = hashtable_entityid_spritegroup_get(spritegroups, shield_id);
            if (sg) {
                sprite* shield_s_front = spritegroup_get(sg, SG_ANIM_BUCKLER_FRONT);
                sprite* shield_s_back = spritegroup_get(sg, SG_ANIM_BUCKLER_BACK);
                int front_context = sprite_get_context(shield_s_front);
                int back_context = sprite_get_context(shield_s_back);
                snprintf(tmp, sizeof(tmp), "shield front back: %d %d\n", front_context, back_context);
                strncat(g->debugpanel.buffer, tmp, sizeof(g->debugpanel.buffer) - strlen(g->debugpanel.buffer) - 1);
            }
        }
        libdraw_draw_debug_panel(g);
    }
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
    msuccess("libdraw_unload_texture: texture unloaded successfully");
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
        merror("load_texture: txkey out of bounds");
        return false;
    } else if (ctxs < 0) {
        merror("load_texture: contexts out of bounds");
        return false;
    } else if (frames < 0) {
        merror("load_texture: frames out of bounds");
        return false;
    } else if (txinfo[txkey].texture.id > 0) {
        merror("load_texture: texture already loaded");
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
    msuccess("load_texture: texture loaded successfully");
    return true;
}

static void load_textures() {
    const char* textures_file = "textures.txt";
    FILE* file = fopen(textures_file, "r");
    if (!file) {
        merror("load_textures: textures.txt not found");
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
        if (line[0] == '#') continue;
        sscanf(line, "%d %d %d %d %s", &txkey, &contexts, &frames, &do_dither, path);
        if (txkey < 0 || contexts < 0 || frames < 0) {
            merror("libdraw_load_textures: invalid line in textures.txt");
            continue;
        }
        load_texture(txkey, contexts, frames, do_dither, path);
    }
    fclose(file);
}

static void create_spritegroup(gamestate* const g,
                               entityid id,
                               int* keys,
                               int num_keys,
                               int offset_x,
                               int offset_y,
                               specifier_t spec) {
    minfo("create_spritegroup");
    if (!g) {
        merror("create_spritegroup: gamestate is NULL");
        return;
    }
    msuccess("g was not NULL");
    // can hold up to 32 sprites
    spritegroup_t* group = spritegroup_create(SPRITEGROUP_DEFAULT_SIZE);
    if (!group) {
        merror("create_spritegroup: failed to create spritegroup");
        return;
    }
    msuccess("group was not NULL");
    const entity* const e = em_get(g->entitymap, id);
    if (!e) {
        merror("create_spritegroup: entity not found %d", id);
        spritegroup_destroy(group);
        return;
    }
    msuccess("entity found %d", id);
    //disabling this check until dungeon_floor created
    dungeon_floor_t* df = dungeon_get_current_floor(g->dungeon);
    if (!df) {
        merror("create_spritegroup: dungeon_floor is NULL");
        spritegroup_destroy(group);
        return;
    }
    const int df_w = df->width;
    const int df_h = df->height;
    if (e->x < 0 || e->x >= df_w || e->y < 0 || e->y >= df_h) {
        merror("create_spritegroup: entity pos out of bounds %d %d", e->x, e->y);
        spritegroup_destroy(group);
        return;
    }
    minfo("create_spritegroup: creating spritegroup for entityid %d", id);
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
        merror("create_spritegroup: sprite is NULL");
        spritegroup_destroy(group);
        return;
    }
    const float w = s->width;
    const float h = s->height;
    const float dst_x = e->x * DEFAULT_TILE_SIZE;
    const float dst_y = e->y * DEFAULT_TILE_SIZE;
    group->current = 0;
    group->dest = (Rectangle){dst_x + offset_x, dst_y + offset_y, w, h};
    group->off_x = offset_x;
    group->off_y = offset_y;
    hashtable_entityid_spritegroup_insert(spritegroups, id, group);
}

static void calc_debugpanel_size(gamestate* const g) {
    if (!g) {
        merror("calc_debugpanel_size: gamestate is NULL");
        return;
    }
    Vector2 s = MeasureTextEx(GetFontDefault(), g->debugpanel.buffer, g->debugpanel.font_size, 1);
    const float width_factor = 1.1f;
    g->debugpanel.w = s.x * width_factor;
    g->debugpanel.h = s.y;
}

static void create_sg_byid(gamestate* const g, entityid id) {
    if (!g) {
        merror("create_sg_byid: gamestate is NULL");
        return;
    }
    entity* const e = em_get(g->entitymap, id);
    if (!e) {
        return;
    }
    int* keys = NULL;
    int num_keys = 0;
    const int offset_x = -12;
    const int offset_y = -12;
    if (e->type == ENTITY_PLAYER || e->type == ENTITY_NPC) {
        switch (e->race) {
        case RACE_HUMAN:
            keys = TX_HUMAN_KEYS;
            num_keys = TX_HUMAN_KEY_COUNT;
            break;
        // Add cases for other races here
        case RACE_ORC:
            keys = TX_ORC_KEYS;
            num_keys = TX_ORC_KEY_COUNT;
            break;
        default:
            merror("libdraw_create_sg_byid: unknown race %d", e->race);
            return;
        }
        create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
    } else if (e->type == ENTITY_WEAPON) {
        // for now we only have 1 sprite for weapons
        keys = TX_LONG_SWORD_KEYS;
        num_keys = TX_LONG_SWORD_KEY_COUNT;
        create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
    } else if (e->type == ENTITY_SHIELD) {
        keys = TX_BUCKLER_KEYS;
        num_keys = TX_BUCKLER_KEY_COUNT;
        create_spritegroup(g, id, keys, num_keys, offset_x, offset_y, SPECIFIER_NONE);
    }
}

static inline void draw_hud(gamestate* const g) {
    if (!g) {
        merror("draw_hud: gamestate is NULL");
        return;
    }
    // Draw the HUD
    int fontsize = 20;
    int hp = -1;
    int maxhp = -1;
    int mp = -1;
    int maxmp = -1;
    int level = 1;
    int turn = g->turn_count;
    char buffer[1024] = {0};
    char* name = NULL;
    const Color bg = (Color){0x33, 0x33, 0x33, 0xff};
    const Color fg = WHITE;
    const int padding = 20;
    const float width_factor = 1.1f; // 10% extra width
    entity* const e = em_get(g->entitymap, g->hero_id);
    if (e) {
        hp = e->hp;
        maxhp = e->maxhp;
        mp = e->mp;
        maxmp = e->maxmp;
        level = e->level;
        name = e->name;
    }
    snprintf(buffer,
             sizeof(buffer),
             //"Name: %s\nLevel: %d\nHP: %d/%d MP: %d/%d\nTurn: %d",
             "%s Lvl %d HP %d/%d MP %d/%d Turn %d",
             name,
             level,
             hp,
             maxhp,
             mp,
             maxmp,
             turn);
    const Vector2 size = MeasureTextEx(GetFontDefault(), buffer, fontsize, 1);
    // Add slight extra width factor to account for text measurement inconsistency
    const int box_width = (size.x * width_factor) + (padding * 2);
    const int box_height = size.y + (padding * 2);
    // Position box
    const int box_x = (g->windowwidth - box_width) / 2;
    const int box_y = g->windowheight - box_height - 100;
    // Draw everything
    DrawRectangle(box_x, box_y, box_width, box_height, bg);
    DrawRectangleLinesEx((Rectangle){box_x, box_y, box_width, box_height}, 2, fg);
    DrawText(buffer, box_x + padding, box_y + padding, fontsize, fg);
}

void libdraw_init(gamestate* const g) {
    if (!g) {
        merror("libdraw_init g is NULL");
        return;
    }
    const int w = DEFAULT_WIN_WIDTH;
    const int h = DEFAULT_WIN_HEIGHT;
    const int x = w / 4;
    const int y = h / 4;
    const char* title = "evildojo666";
    InitWindow(w, h, title);
    g->windowwidth = w;
    g->windowheight = h;
    SetTargetFPS(60);
    target = LoadRenderTexture(w, h);
    target_src = (Rectangle){0, 0, w, -h};
    target_dest = (Rectangle){0, 0, w, h};
    spritegroups = hashtable_entityid_spritegroup_create(DEFAULT_SPRITEGROUPS_SIZE);
    load_textures();
    for (int i = 0; i < g->index_entityids; i++) {
        create_sg_byid(g, g->entityids[i]);
    }
    calc_debugpanel_size(g);
    load_shaders();
    g->cam2d.offset = (Vector2){x, y};
    gamestate_set_debug_panel_pos_top_right(g);
    msuccess("libdraw_init");
}

static void draw_message_history(gamestate* const g) {
    if (!g) {
        merror("libdraw_draw_message_history: gamestate is NULL");
        return;
    }
    // if there are no messages in the message history, return
    if (g->msg_history.count == 0) {
        return;
    }
    const int font_size = 20;
    const int pad = 40; // Inner padding (text <-> box edges)
    const float line_spacing = 1.0f;
    const int max_messages = 5;
    const int x = 0;
    const int y = 0;
    int current_count = 0;
    char tmp_buffer[2048] = {0};
    Color message_bg = Fade((Color){0x33, 0x33, 0x33, 0xff}, 0.8f);
    // instead of a placeholder message, we now need to actually draw the message history
    // we might only render the last N messages
    //int index = g->msg_history.count;
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
    const Vector2 text_size = MeasureTextEx(GetFontDefault(), tmp_buffer, font_size, line_spacing);
    // Calculate box position
    // we want the box to be in the top left corner of the screen
    const Rectangle box = {.x = x, .y = y, .width = text_size.x + pad * 2, .height = text_size.y + pad * 2};
    // Draw box (semi-transparent black with white border)
    DrawRectangleRec(box, message_bg);
    DrawRectangleLinesEx(box, 2, WHITE);
    // Draw text (centered in box)
    DrawTextEx(GetFontDefault(), tmp_buffer, (Vector2){box.x + pad, box.y + pad}, font_size, line_spacing, WHITE);
}

void libdraw_update_input(inputstate* const is) { inputstate_update(is); }

bool libdraw_windowshouldclose() { return WindowShouldClose(); }
