#include "ComponentTraits.h"
#include "entityid.h"
#include "gamestate.h"
#include "libdraw.h"
#include "libdraw_from_texture.h"
#include "libdraw_load_music.h"
#include "libdraw_load_sfx.h"
#include "libdraw_shaders.h"
#include "libdraw_to_texture.h"
#include "race.h"
#include "spritegroup.h"

//#include "get_txkey_for_tiletype.h"
//#include "gamestate_equipped_weapon.h"
//#include "tx_keys_rings.h"
//#include "tx_keys_shields.h"

unordered_map<entityid, spritegroup_t*> spritegroups;

textureinfo txinfo[GAMESTATE_SIZEOFTEXINFOARRAY];


//Shader shader_grayscale = {0};
//Shader shader_glow = {0};
//Shader shader_red_glow = {0};
//Shader shader_color_noise = {0};
//Shader shader_psychedelic_0 = {0};


RenderTexture2D title_target_texture = {0};
RenderTexture2D char_creation_target_texture = {0};
RenderTexture2D main_game_target_texture = {0};
RenderTexture2D hud_target_texture = {0};


RenderTexture2D target = {0};
Rectangle target_src = {0, 0, DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT};
Rectangle target_dest = {0, 0, DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT};
Rectangle win_dest = {0, 0, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};


Music music;


int ANIM_SPEED = DEFAULT_ANIM_SPEED;
int libdraw_restart_count = 0;


//void libdraw_set_sg_block_success(shared_ptr<gamestate> g, entityid id, spritegroup_t* const sg);
//void draw_gameplay_settings_menu(shared_ptr<gamestate> g);
//void draw_gameover_menu(shared_ptr<gamestate> g);
//void draw_shield_sprite_front(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
//void draw_shield_sprite_back(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
//void create_sg_byid(shared_ptr<gamestate> g, entityid id);
//sprite* get_shield_front_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
//sprite* get_shield_back_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
// sprite* get_weapon_back_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
// sprite* get_weapon_front_sprite(shared_ptr<gamestate> g, entityid id, spritegroup_t* sg);
// bool libdraw_check_default_animations(const shared_ptr<gamestate> g);
// void draw_shadow_for_entity(const shared_ptr<gamestate> g,
//                                   spritegroup_t* sg,
//                                   entityid id);

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
    DrawTexturePro(target.texture, target_src, win_dest, (Vector2){0, 0}, 0.0f, WHITE);
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
    unload_shaders();

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
